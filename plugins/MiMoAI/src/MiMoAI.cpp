/*
 * Copyright (C) 2024 MiMoAI
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#include "StelTranslator.hpp"
#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StelModuleMgr.hpp"
#include "StelGui.hpp"
#include "StelGuiItems.hpp"
#include "MiMoAI.hpp"
#include "gui/MiMoConfigDialog.hpp"
#include "MiMoChatDialog.hpp"
#include "MiMoTools.hpp"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QPixmap>

//! This method is the one called automatically by the StelModuleMgr just
//! after loading the dynamic library
StelModule* MiMoAIStelPluginInterface::getStelModule() const
{
    return new MiMoAI();
}

StelPluginInfo MiMoAIStelPluginInterface::getPluginInfo() const
{
    // Allow to load the resources when used as a static plugin
    Q_INIT_RESOURCE(MiMoAI);

    StelPluginInfo info;
    info.id = "MiMoAI";
    info.displayedName = N_("MiMoAI");
    info.authors = "MiMoAI Team";
    info.contact = "https://github.com/mimoai";
    info.description = N_("MiMoAI plugin for Stellarium");
    info.version = MIMOAI_PLUGIN_VERSION;
    info.license = MIMOAI_PLUGIN_LICENSE;
    info.startByDefault = false;
    return info;
}

MiMoAI::MiMoAI()
    : enabled(false)
    , m_apiClient(new MiMoAPIClient(this))
    , m_toolManager(new MiMoToolManager(this))
    , m_cluster(MiMoAPIClient::Cluster::China)
    , m_model("gpt-4o")
#ifndef NO_GUI
    , m_toolbarButton(nullptr)
#endif
{
    setObjectName("MiMoAI");
#ifndef NO_GUI
    m_configDialog = new MiMoConfigDialog(this);
    m_chatDialog = new MiMoChatDialog(this);
#endif
    
    // Register tools
    m_toolManager->registerTool(new SearchObjectTool(this));
    m_toolManager->registerTool(new GetObjectInfoTool(this));
    m_toolManager->registerTool(new LocateObjectTool(this));
    m_toolManager->registerTool(new GetCurrentViewTool(this));
    m_toolManager->registerTool(new SetTimeTool(this));
    m_toolManager->registerTool(new ControlTimeTool(this));
    m_toolManager->registerTool(new SetLocationTool(this));
    m_toolManager->registerTool(new SetZoomTool(this));
    m_toolManager->registerTool(new ToggleFeatureTool(this));
    m_toolManager->registerTool(new TakeScreenshotTool(this));
    
    // Connect tool signals
    connect(m_toolManager, &MiMoToolManager::toolExecuted, this, &MiMoAI::toolExecuted);
}

MiMoAI::~MiMoAI()
{
}

bool MiMoAI::configureGui(bool show)
{
#ifndef NO_GUI
    if (show)
    {
        m_configDialog->setVisible(true);
    }
#endif
    return true;
}

//! Determine which "layer" the plugin's drawing will happen on.
double MiMoAI::getCallOrder(StelModuleActionName actionName) const
{
    Q_UNUSED(actionName)
    return 0;
}

void MiMoAI::init()
{
    StelApp& app = StelApp::getInstance();

    // Create action for enable/disable
    addAction("actionShow_MiMoAI", N_("MiMoAI"), N_("MiMoAI Chat"), "enabled");
    addAction("actionConfig_MiMoAI", N_("MiMoAI"), N_("MiMoAI Configuration"), this, "showConfigDialog");

    connect(m_apiClient, &MiMoAPIClient::chatCompletionReceived, this, &MiMoAI::onChatCompletionReceived);
    connect(m_apiClient, &MiMoAPIClient::errorOccurred, this, &MiMoAI::onErrorOccurred);

#ifndef NO_GUI
    connect(this, &MiMoAI::chatCompletionReceived, m_chatDialog, &MiMoChatDialog::onChatCompletionReceived);
    connect(this, &MiMoAI::errorOccurred, m_chatDialog, &MiMoChatDialog::onErrorOccurred);
    
    // Add toolbar button
    try
    {
        StelGui* gui = dynamic_cast<StelGui*>(app.getGui());
        if (gui != nullptr)
        {
            m_toolbarButton = new StelButton(nullptr,
                                             QPixmap(":/MiMoAI/bt_mimoai_on.png"),
                                             QPixmap(":/MiMoAI/bt_mimoai_off.png"),
                                             QPixmap(":/graphicGui/miscGlow32x32.png"),
                                             "actionShow_MiMoAI",
                                             false,
                                             "actionConfig_MiMoAI");
            gui->getButtonBar()->addButton(m_toolbarButton, "065-pluginsGroup");
        }
    }
    catch (std::runtime_error& e)
    {
        qWarning() << "Unable to create toolbar button for MiMoAI plugin:" << e.what();
    }
#endif

    loadSettings();

    qDebug() << "MiMoAI plugin initialized";
}

void MiMoAI::update(double deltaTime)
{
    Q_UNUSED(deltaTime)
}

void MiMoAI::draw(StelCore* core)
{
    Q_UNUSED(core)
    if (!enabled)
        return;
}

void MiMoAI::setEnabled(bool b)
{
    if (b != enabled)
    {
        enabled = b;
        emit enabledChanged(b);
#ifndef NO_GUI
        m_chatDialog->setVisible(enabled);
#endif
        qDebug() << "MiMoAI" << (enabled ? "enabled" : "disabled");
    }
}

void MiMoAI::showConfigDialog()
{
#ifndef NO_GUI
    m_configDialog->setVisible(true);
#endif
}

void MiMoAI::loadSettings()
{
    QSettings* conf = StelApp::getInstance().getSettings();
    conf->beginGroup("MiMoAI");

    m_apiKey = conf->value("api_key", "").toString();
    int clusterIndex = conf->value("cluster", 0).toInt();
    switch (clusterIndex)
    {
        case 0: m_cluster = MiMoAPIClient::Cluster::China; break;
        case 1: m_cluster = MiMoAPIClient::Cluster::Singapore; break;
        case 2: m_cluster = MiMoAPIClient::Cluster::Europe; break;
    }
    m_model = conf->value("model", "gpt-4o").toString();
    m_systemPrompt = conf->value("system_prompt", getAstronomySystemPrompt()).toString();

    conf->endGroup();

    m_apiClient->setApiKey(m_apiKey);
    m_apiClient->setCluster(m_cluster);
    m_apiClient->setModel(m_model);
}

void MiMoAI::saveSettings()
{
    QSettings* conf = StelApp::getInstance().getSettings();
    conf->beginGroup("MiMoAI");
    
    conf->setValue("api_key", m_apiKey);
    int clusterIndex = 0;
    switch (m_cluster)
    {
        case MiMoAPIClient::Cluster::China: clusterIndex = 0; break;
        case MiMoAPIClient::Cluster::Singapore: clusterIndex = 1; break;
        case MiMoAPIClient::Cluster::Europe: clusterIndex = 2; break;
    }
    conf->setValue("cluster", clusterIndex);
    conf->setValue("model", m_model);
    conf->setValue("system_prompt", m_systemPrompt);
    
    conf->endGroup();
}

QString MiMoAI::getAstronomySystemPrompt()
{
    return R"(You are an expert astronomy assistant integrated with Stellarium, a powerful planetarium software. Your role is to help users explore and understand the night sky.

Key Responsibilities:
1. Answer astronomy questions with accurate, educational information
2. Help users navigate Stellarium using the available tools
3. Identify and explain celestial objects
4. Provide guidance on astronomical phenomena

Available Tools:
- search_object: Find celestial objects by name
- get_object_info: Get detailed information about an object
- locate_object: Center the view on a specific object
- get_current_view: Get information about the current view
- set_time: Set the simulation time
- control_time: Control time flow (pause, play, speed)
- set_location: Set the observer's location
- set_zoom: Adjust the zoom level
- toggle_feature: Toggle Stellarium features (constellations, atmosphere, etc.)
- take_screenshot: Capture the current view

Interaction Guidelines:
- When appropriate, use tools to help the user explore the sky
- Explain what you're doing and why
- Keep responses clear and educational
- Use simple language for general audiences, but be precise
- When using multiple tools, explain the sequence
- If you need more information, ask the user for clarification

Remember: You're part of an immersive astronomy experience. Make it engaging and informative!)";
}

void MiMoAI::sendChatMessage(const QList<MiMoAPIClient::ChatMessage>& messages)
{
    QList<MiMoAPIClient::ChatMessage> fullMessages = messages;

    QString systemPrompt = m_systemPrompt.isEmpty() ? getAstronomySystemPrompt() : m_systemPrompt;
    MiMoAPIClient::ChatMessage systemMessage;
    systemMessage.role = "system";
    systemMessage.content = systemPrompt;
    fullMessages.prepend(systemMessage);
    
    MiMoAPIClient::ChatCompletionRequest request;
    request.model = m_model;
    request.messages = fullMessages;
    request.tools = m_toolManager->getToolDefinitions();
    request.temperature = 0.7;
    request.topP = 1.0;
    request.maxTokens = 0;
    request.stream = false;
    
    m_apiClient->createChatCompletion(request);
}

void MiMoAI::onChatCompletionReceived(const MiMoAPIClient::ChatCompletionResponse& response)
{
    if (!response.choices.isEmpty())
    {
        const MiMoAPIClient::ChatChoice& choice = response.choices.first();
        if (!choice.message.toolCalls.isEmpty())
        {
            // Handle tool calls
            for (const QVariant& toolCallVariant : choice.message.toolCalls)
            {
                QVariantMap toolCallMap = toolCallVariant.toMap();
                QVariantMap functionMap = toolCallMap["function"].toMap();
                QString toolName = functionMap["name"].toString();
                QString argumentsStr = functionMap["arguments"].toString();
                QVariantMap arguments;
                
                // Parse arguments from JSON
                QJsonDocument doc = QJsonDocument::fromJson(argumentsStr.toUtf8());
                if (doc.isObject())
                {
                    arguments = doc.object().toVariantMap();
                }
                
                // Execute tool
                if (m_toolManager->hasTool(toolName))
                {
                    QVariantMap result = m_toolManager->executeTool(toolName, arguments);
                    
                    // Create tool response message
                    MiMoAPIClient::ChatMessage toolResponse;
                    toolResponse.role = "tool";
                    toolResponse.content = QJsonDocument::fromVariant(result).toJson(QJsonDocument::Compact);
                    toolResponse.toolCallId = toolCallMap["id"].toString();
                    
                    // TODO: Continue conversation with tool response
                    // For now, just emit the chat completion received
                    emit chatCompletionReceived(response);
                }
            }
        }
        else
        {
            // Regular text response
            emit chatCompletionReceived(response);
        }
    }
    else
    {
        emit chatCompletionReceived(response);
    }
}

void MiMoAI::onErrorOccurred(const QString& error)
{
    emit errorOccurred(error);
}
