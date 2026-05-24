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

#ifndef MIMOCHATDIALOG_HPP
#define MIMOCHATDIALOG_HPP

#include "StelDialogSeparate.hpp"
#include "../MiMoAPIClient.hpp"

class MiMoAI;
class Ui_MiMoChatDialogForm;

class MiMoChatDialog : public StelDialogSeparate
{
	Q_OBJECT

public:
	MiMoChatDialog(QObject* parent = nullptr);
	~MiMoChatDialog() override;

public slots:
	void retranslate() override;

protected:
	void createDialogContent() override;

private slots:
	void sendMessage();
	void onChatCompletionReceived(const MiMoAPIClient::ChatCompletionResponse& response);
	void onErrorOccurred(const QString& error);
	void onQuickQuestionClicked();
	void clearChat();
	void onToolExecuted(const QString& toolName, const QVariantMap& result);
	void saveChatHistory();
	void loadChatHistory();

private:
	void addMessage(const QString& role, const QString& content);
	void setLoading(bool loading);
	void saveChatHistoryToFile(const QString& fileName);
	void loadChatHistoryFromFile(const QString& fileName);

	Ui_MiMoChatDialogForm* ui;
	MiMoAI* plugin;
	QList<MiMoAPIClient::ChatMessage> m_chatHistory;
};

#endif // MIMOCHATDIALOG_HPP
