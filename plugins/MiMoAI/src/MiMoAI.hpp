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

#ifndef MIMOAI_HPP
#define MIMOAI_HPP

#include "StelModule.hpp"
#include "MiMoAPIClient.hpp"
#include "MiMoToolManager.hpp"

class MiMoConfigDialog;
class MiMoChatDialog;
class StelButton;

//! @class MiMoAI
//! Main class of the MiMoAI plugin.
class MiMoAI : public StelModule
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

public:
    MiMoAI();
    ~MiMoAI() override;

    ///////////////////////////////////////////////////////////////////////////
    // Methods defined in the StelModule class
    void init() override;
    void update(double deltaTime) override;
    void draw(StelCore* core) override;
    double getCallOrder(StelModuleActionName actionName) const override;
    bool configureGui(bool show=true) override;

    void loadSettings();
    void saveSettings();
    void sendChatMessage(const QList<MiMoAPIClient::ChatMessage>& messages);
    MiMoToolManager* getToolManager() { return m_toolManager; }
    static QString getAstronomySystemPrompt();

signals:
    void enabledChanged(bool b);
    void chatCompletionReceived(const MiMoAPIClient::ChatCompletionResponse& response);
    void errorOccurred(const QString& error);
    void toolExecuted(const QString& toolName, const QVariantMap& result);

public slots:
    bool isEnabled() const { return enabled; }
    void setEnabled(bool b);
    void showConfigDialog();

private slots:
    void onChatCompletionReceived(const MiMoAPIClient::ChatCompletionResponse& response);
    void onErrorOccurred(const QString& error);

private:
    bool enabled;
    MiMoAPIClient* m_apiClient;
    MiMoToolManager* m_toolManager;
#ifndef NO_GUI
    MiMoConfigDialog* m_configDialog;
    MiMoChatDialog* m_chatDialog;
    StelButton* m_toolbarButton;
#endif
    QString m_apiKey;
    MiMoAPIClient::Cluster m_cluster;
    QString m_model;
    QString m_systemPrompt;
};

#include <QObject>
#include "StelPluginInterface.hpp"

//! This class is used by Qt to manage a plug-in interface
class MiMoAIStelPluginInterface : public QObject, public StelPluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID StelPluginInterface_iid)
	Q_INTERFACES(StelPluginInterface)
public:
	StelModule* getStelModule() const override;
	StelPluginInfo getPluginInfo() const override;
};

#endif // MIMOAI_HPP
