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

#ifndef MIMOAPICLIENT_HPP
#define MIMOAPICLIENT_HPP

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QNetworkReply>

class QNetworkAccessManager;
class QNetworkRequest;

//! @class MiMoAPIClient
//! MiMo API client for OpenAI-compatible API requests.
class MiMoAPIClient : public QObject
{
	Q_OBJECT

public:
	//! Enum for supported API clusters
	enum class Cluster {
		China,     //!< China cluster
		Singapore, //!< Singapore cluster
		Europe     //!< Europe cluster
	};
	Q_ENUM(Cluster)

	//! Struct for function tool definition
	struct Tool {
		QString type;         //!< Tool type, usually "function"
		QString name;         //!< Function name
		QString description;  //!< Function description
		QVariantMap parameters; //!< Function parameters schema
	};

	//! Struct for message content (text or image)
	struct MessageContent {
		enum class Type {
			Text,  //!< Text content
			Image  //!< Image content (URL or base64)
		};
		Type type;
		QString text;          //!< Text content (for Text type)
		QString imageUrl;      //!< Image URL or base64 data (for Image type)
	};

	//! Struct for chat message
	struct ChatMessage {
		QString role;                    //!< Message role: "system", "user", "assistant", "tool"
		QList<MessageContent> contents;  //!< Message contents (for multi-modal)
		QString content;                 //!< Simple text content (for compatibility)
		QString toolCallId;              //!< Tool call ID (for tool responses)
		QVariantList toolCalls;          //!< Tool calls made by assistant
	};

	//! Struct for chat completion request
	struct ChatCompletionRequest {
		QString model;                  //!< Model name
		QList<ChatMessage> messages;    //!< Conversation messages
		QList<Tool> tools;              //!< Available tools for function calling
		double temperature;             //!< Sampling temperature
		double topP;                    //!< Top-p sampling
		int maxTokens;                  //!< Maximum tokens to generate
		bool stream;                    //!< Whether to stream the response
	};

	//! Struct for chat completion choice
	struct ChatChoice {
		int index;                      //!< Choice index
		ChatMessage message;            //!< Generated message
		QString finishReason;           //!< Finish reason
	};

	//! Struct for chat completion response
	struct ChatCompletionResponse {
		QString id;                     //!< Response ID
		QString object;                 //!< Object type
		qint64 created;                 //!< Creation timestamp
		QString model;                  //!< Model used
		QList<ChatChoice> choices;      //!< Generated choices
		QVariantMap usage;              //!< Token usage info
	};

	explicit MiMoAPIClient(QObject* parent = nullptr);
	~MiMoAPIClient() override;

	//! Set the API key
	void setApiKey(const QString& apiKey);

	//! Get the current API key
	QString apiKey() const { return m_apiKey; }

	//! Set the API cluster
	void setCluster(Cluster cluster);

	//! Get the current API cluster
	Cluster cluster() const { return m_cluster; }

	//! Set a custom base URL (overrides cluster setting)
	void setCustomBaseUrl(const QString& baseUrl);

	//! Get the current base URL
	QString baseUrl() const { return m_baseUrl; }

	//! Set the model to use
	void setModel(const QString& model);

	//! Get the current model
	QString model() const { return m_model; }

	//! Create a chat completion request
	//! @param request The chat completion request parameters
	//! @return A reply object that emits finished signal when done
	Q_INVOKABLE QNetworkReply* createChatCompletion(const ChatCompletionRequest& request);

	//! Convenience method for simple chat completion
	//! @param messages List of chat messages
	//! @return A reply object that emits finished signal when done
	Q_INVOKABLE QNetworkReply* chat(const QList<ChatMessage>& messages);

	//! Convenience method for text-only chat
	//! @param userMessage The user's message
	//! @param systemMessage Optional system message
	//! @return A reply object that emits finished signal when done
	Q_INVOKABLE QNetworkReply* chat(const QString& userMessage, const QString& systemMessage = QString());

	//! Convenience method for vision (image understanding)
	//! @param userMessage Text message to accompany the image
	//! @param imageUrl Image URL or base64 data
	//! @param systemMessage Optional system message
	//! @return A reply object that emits finished signal when done
	Q_INVOKABLE QNetworkReply* visionChat(const QString& userMessage, const QString& imageUrl, const QString& systemMessage = QString());

	//! Parse a chat completion response from JSON
	//! @param jsonData JSON response data
	//! @return Parsed chat completion response
	static ChatCompletionResponse parseChatCompletionResponse(const QByteArray& jsonData);

	//! Parse error from JSON response
	//! @param jsonData JSON error response data
	//! @return Error message
	static QString parseError(const QByteArray& jsonData);

signals:
	//! Emitted when an API error occurs
	void errorOccurred(const QString& error);

	//! Emitted when a chat completion is received
	void chatCompletionReceived(const ChatCompletionResponse& response);

private:
	//! Build the API URL for chat completions
	QString buildChatCompletionUrl() const;

	//! Build the network request with proper headers
	QNetworkRequest buildRequest(const QString& url) const;

	//! Convert ChatCompletionRequest to JSON object
	static QVariantMap requestToJson(const ChatCompletionRequest& request);

	//! Convert ChatMessage to JSON
	static QVariant messageToJson(const ChatMessage& message);

	//! Convert Tool to JSON
	static QVariant toolToJson(const Tool& tool);

	QString m_apiKey;
	Cluster m_cluster;
	QString m_baseUrl;
	QString m_model;
	QNetworkAccessManager* m_networkManager;
};

Q_DECLARE_METATYPE(MiMoAPIClient::Cluster)
Q_DECLARE_METATYPE(MiMoAPIClient::ChatCompletionResponse)

#endif // MIMOAPICLIENT_HPP
