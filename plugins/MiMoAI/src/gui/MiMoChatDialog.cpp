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

#include "MiMoChatDialog.hpp"
#include "MiMoAI.hpp"
#include "ui_MiMoChatDialog.h"

#include <QScrollBar>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileDialog>
#include "StelApp.hpp"
#include "StelGui.hpp"
#include "StelTranslator.hpp"

MiMoChatDialog::MiMoChatDialog(QObject* parent)
    : StelDialogSeparate("MiMoChat", parent)
    , ui(new Ui_MiMoChatDialogForm)
    , plugin(nullptr)
{
    setObjectName("MiMoChatDialog");
}

MiMoChatDialog::~MiMoChatDialog()
{
    delete ui;
}

void MiMoChatDialog::retranslate()
{
    if (dialog)
    {
        ui->retranslateUi(dialog);
    }
}

void MiMoChatDialog::createDialogContent()
{
    plugin = GETSTELMODULE(MiMoAI);
    Q_ASSERT(plugin);

    ui->setupUi(dialog);

    connect(&StelApp::getInstance(), SIGNAL(languageChanged()), this, SLOT(retranslate()));
    connect(ui->titleBar, &TitleBar::closeClicked, plugin, [this]() { plugin->setEnabled(false); });
    connect(ui->titleBar, SIGNAL(movedTo(QPoint)), this, SLOT(handleMovedTo(QPoint)));
    connect(ui->pushButton_send, SIGNAL(clicked()), this, SLOT(sendMessage()));
    connect(ui->pushButton_clear, SIGNAL(clicked()), this, SLOT(clearChat()));
    connect(ui->lineEdit_input, SIGNAL(returnPressed()), this, SLOT(sendMessage()));

    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(saveChatHistory()));
    connect(ui->pushButton_load, SIGNAL(clicked()), this, SLOT(loadChatHistory()));

    connect(ui->pushButton_quick1, SIGNAL(clicked()), this, SLOT(onQuickQuestionClicked()));
    connect(ui->pushButton_quick2, SIGNAL(clicked()), this, SLOT(onQuickQuestionClicked()));
    connect(ui->pushButton_quick3, SIGNAL(clicked()), this, SLOT(onQuickQuestionClicked()));
    connect(ui->pushButton_quick4, SIGNAL(clicked()), this, SLOT(onQuickQuestionClicked()));

    // Connect tool execution signal
    connect(plugin, &MiMoAI::toolExecuted, this, &MiMoChatDialog::onToolExecuted);

    // Kinetic scrolling
    kineticScrollingList << ui->textBrowser_chat;
    StelGui* gui = dynamic_cast<StelGui*>(StelApp::getInstance().getGui());
    if (gui)
    {
        enableKineticScrolling(gui->getFlagUseKineticScrolling());
        connect(gui, SIGNAL(flagUseKineticScrollingChanged(bool)), this, SLOT(enableKineticScrolling(bool)));
    }

    setLoading(false);
}

void MiMoChatDialog::sendMessage()
{
    QString message = ui->lineEdit_input->text().trimmed();
    if (message.isEmpty())
        return;

    ui->lineEdit_input->clear();
    addMessage("user", message);

    MiMoAPIClient::ChatMessage chatMessage;
    chatMessage.role = "user";
    chatMessage.content = message;
    m_chatHistory.append(chatMessage);

    setLoading(true);

    plugin->sendChatMessage(m_chatHistory);
}

void MiMoChatDialog::onChatCompletionReceived(const MiMoAPIClient::ChatCompletionResponse& response)
{
    if (!response.choices.isEmpty())
    {
        QString content = response.choices.first().message.content;
        addMessage("assistant", content);

        MiMoAPIClient::ChatMessage chatMessage;
        chatMessage.role = "assistant";
        chatMessage.content = content;
        m_chatHistory.append(chatMessage);
    }
    setLoading(false);
}

void MiMoChatDialog::onErrorOccurred(const QString& error)
{
    addMessage("system", QString("Error: %1").arg(error));
    setLoading(false);
}

void MiMoChatDialog::onQuickQuestionClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button)
    {
        ui->lineEdit_input->setText(button->text());
        sendMessage();
    }
}

void MiMoChatDialog::clearChat()
{
    ui->textBrowser_chat->clear();
    m_chatHistory.clear();
}

void MiMoChatDialog::addMessage(const QString& role, const QString& content)
{
    QString color;
    QString prefix;

    if (role == "user")
    {
        color = "#4CAF50";
        prefix = "<b>You:</b> ";
    }
    else if (role == "assistant")
    {
        color = "#2196F3";
        prefix = "<b>MiMoAI:</b> ";
    }
    else
    {
        color = "#FF9800";
        prefix = "<b>System:</b> ";
    }

    QString html = QString("<div style='color: %1; margin-bottom: 10px;'>%2%3</div>")
        .arg(color, prefix, content.toHtmlEscaped().replace("\n", "<br>"));

    ui->textBrowser_chat->append(html);

    QScrollBar* scrollBar = ui->textBrowser_chat->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void MiMoChatDialog::setLoading(bool loading)
{
    ui->lineEdit_input->setEnabled(!loading);
    ui->pushButton_send->setEnabled(!loading);
    ui->pushButton_quick1->setEnabled(!loading);
    ui->pushButton_quick2->setEnabled(!loading);
    ui->pushButton_quick3->setEnabled(!loading);
    ui->pushButton_quick4->setEnabled(!loading);
    ui->label_loading->setVisible(loading);
}

void MiMoChatDialog::onToolExecuted(const QString& toolName, const QVariantMap& result)
{
    QString color = "#FFA500";
    QString prefix = "<b>Tool:</b> ";
    QString content;
    
    if (result["success"].toBool())
    {
        QString message = result["message"].toString();
        content = QString("✓ %1 executed successfully: %2").arg(toolName, message);
        if (result.contains("data"))
        {
            QVariantMap data = result["data"].toMap();
            QString dataStr = QJsonDocument::fromVariant(data).toJson(QJsonDocument::Indented);
            content += QString("<br><pre>%1</pre>").arg(dataStr.toHtmlEscaped());
        }
    }
    else
    {
        QString error = result["error"].toString();
        content = QString("✗ %1 failed: %2").arg(toolName, error);
    }
    
    QString html = QString("<div style='color: %1; margin-bottom: 10px;'>%2%3</div>")
        .arg(color, prefix, content);
    
    ui->textBrowser_chat->append(html);
    
    QScrollBar* scrollBar = ui->textBrowser_chat->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void MiMoChatDialog::saveChatHistory()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr,
        tr("Save Chat History"), "",
        tr("JSON Files (*.json);;All Files (*)");
    
    if (!fileName.isEmpty())
    {
        saveChatHistoryToFile(fileName);
    }
}

void MiMoChatDialog::loadChatHistory()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,
        tr("Load Chat History"), "",
        tr("JSON Files (*.json);;All Files (*)");
    
    if (!fileName.isEmpty())
    {
        loadChatHistoryFromFile(fileName);
    }
}

void MiMoChatDialog::saveChatHistoryToFile(const QString& fileName)
{
    QJsonArray jsonArray;
    
    for (const auto& message : m_chatHistory)
    {
        QJsonObject jsonObj;
        jsonObj["role"] = message.role;
        jsonObj["content"] = message.content;
        jsonArray.append(jsonObj);
    }
    
    QJsonDocument jsonDoc(jsonArray);
    
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(jsonDoc.toJson(QJsonDocument::Indented));
        file.close();
        addMessage("system", tr("Chat history saved successfully!"));
    }
    else
    {
        addMessage("system", tr("Failed to save chat history!"));
    }
}

void MiMoChatDialog::loadChatHistoryFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray data = file.readAll();
        file.close();
        
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isArray())
        {
            clearChat();
            QJsonArray jsonArray = jsonDoc.array();
            
            for (const auto& value : jsonArray)
            {
                if (value.isObject())
                {
                    QJsonObject jsonObj = value.toObject();
                    MiMoAPIClient::ChatMessage message;
                    message.role = jsonObj["role"].toString();
                    message.content = jsonObj["content"].toString();
                    
                    m_chatHistory.append(message);
                    addMessage(message.role, message.content);
                }
            }
            
            addMessage("system", tr("Chat history loaded successfully!"));
        }
        else
        {
            addMessage("system", tr("Invalid chat history file!"));
        }
    }
    else
    {
        addMessage("system", tr("Failed to load chat history!"));
    }
}
