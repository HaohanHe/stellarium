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

#include "MiMoAPIClient.hpp"
#include "StelJsonParser.hpp"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QBuffer>
#include <QDebug>

namespace {
	const QString CLUSTER_URL_CHINA = "https://token-plan-cn.xiaomimimo.com/v1";
	const QString CLUSTER_URL_SINGAPORE = "https://token-plan-sg.xiaomimimo.com/v1";
	const QString CLUSTER_URL_EUROPE = "https://token-plan-eu.xiaomimimo.com/v1";
	const QString DEFAULT_MODEL = "gpt-4o";
}

MiMoAPIClient::MiMoAPIClient(QObject* parent)
	: QObject(parent)
	, m_cluster(Cluster::China)
	, m_model(DEFAULT_MODEL)
	, m_networkManager(new QNetworkAccessManager(this))
{
	setCluster(m_cluster);
}

MiMoAPIClient::~MiMoAPIClient()
{
}

void MiMoAPIClient::setApiKey(const QString& apiKey)
{
	m_apiKey = apiKey;
}

void MiMoAPIClient::setCluster(Cluster cluster)
{
	m_cluster = cluster;
	switch (cluster) {
		case Cluster::China:
			m_baseUrl = CLUSTER_URL_CHINA;
			break;
		case Cluster::Singapore:
			m_baseUrl = CLUSTER_URL_SINGAPORE;
			break;
		case Cluster::Europe:
			m_baseUrl = CLUSTER_URL_EUROPE;
			break;
	}
}

void MiMoAPIClient::setCustomBaseUrl(const QString& baseUrl)
{
	m_baseUrl = baseUrl;
}

void MiMoAPIClient::setModel(const QString& model)
{
	m_model = model;
}

QString MiMoAPIClient::buildChatCompletionUrl() const
{
	return m_baseUrl + "/chat/completions";
}

QNetworkRequest MiMoAPIClient::buildRequest(const QString& url) const
{
	QNetworkRequest request(QUrl(url));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	if (!m_apiKey.isEmpty()) {
		request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());
	}
	return request;
}

QVariantMap MiMoAPIClient::requestToJson(const ChatCompletionRequest& request)
{
	QVariantMap json;
	json["model"] = request.model;
	json["temperature"] = request.temperature;
	json["top_p"] = request.topP;
	if (request.maxTokens > 0) {
		json["max_tokens"] = request.maxTokens;
	}
	json["stream"] = request.stream;

	QVariantList messagesList;
	for (const auto& message : request.messages) {
		messagesList.append(messageToJson(message));
	}
	json["messages"] = messagesList;

	if (!request.tools.isEmpty()) {
		QVariantList toolsList;
		for (const auto& tool : request.tools) {
			toolsList.append(toolToJson(tool));
		}
		json["tools"] = toolsList;
	}

	return json;
}

QVariant MiMoAPIClient::messageToJson(const ChatMessage& message)
{
	QVariantMap json;
	json["role"] = message.role;

	if (!message.contents.isEmpty()) {
		QVariantList contentList;
		for (const auto& content : message.contents) {
			QVariantMap contentItem;
			if (content.type == MessageContent::Type::Text) {
				contentItem["type"] = "text";
				contentItem["text"] = content.text;
			} else if (content.type == MessageContent::Type::Image) {
				contentItem["type"] = "image_url";
				QVariantMap imageUrl;
				imageUrl["url"] = content.imageUrl;
				contentItem["image_url"] = imageUrl;
			}
			contentList.append(contentItem);
		}
		json["content"] = contentList;
	} else if (!message.content.isEmpty()) {
		json["content"] = message.content;
	}

	if (!message.toolCallId.isEmpty()) {
		json["tool_call_id"] = message.toolCallId;
	}

	if (!message.toolCalls.isEmpty()) {
		json["tool_calls"] = message.toolCalls;
	}

	return json;
}

QVariant MiMoAPIClient::toolToJson(const Tool& tool)
{
	QVariantMap json;
	json["type"] = tool.type;
	QVariantMap function;
	function["name"] = tool.name;
	function["description"] = tool.description;
	function["parameters"] = tool.parameters;
	json["function"] = function;
	return json;
}

QNetworkReply* MiMoAPIClient::createChatCompletion(const ChatCompletionRequest& request)
{
	QString url = buildChatCompletionUrl();
	QNetworkRequest networkRequest = buildRequest(url);

	QVariantMap jsonData = requestToJson(request);
	QByteArray jsonByteArray = StelJsonParser::write(jsonData);

	QNetworkReply* reply = m_networkManager->post(networkRequest, jsonByteArray);

	connect(reply, &QNetworkReply::finished, this, [this, reply]() {
		if (reply->error() != QNetworkReply::NoError) {
			QString errorStr = parseError(reply->readAll());
			if (errorStr.isEmpty()) {
				errorStr = reply->errorString();
			}
			emit errorOccurred(errorStr);
		} else {
			QByteArray data = reply->readAll();
			try {
				ChatCompletionResponse response = parseChatCompletionResponse(data);
				emit chatCompletionReceived(response);
			} catch (...) {
				emit errorOccurred("Failed to parse response");
			}
		}
		reply->deleteLater();
	});

	return reply;
}

QNetworkReply* MiMoAPIClient::chat(const QList<ChatMessage>& messages)
{
	ChatCompletionRequest request;
	request.model = m_model;
	request.messages = messages;
	request.temperature = 0.7;
	request.topP = 1.0;
	request.maxTokens = 0;
	request.stream = false;
	return createChatCompletion(request);
}

QNetworkReply* MiMoAPIClient::chat(const QString& userMessage, const QString& systemMessage)
{
	QList<ChatMessage> messages;

	if (!systemMessage.isEmpty()) {
		ChatMessage sysMsg;
		sysMsg.role = "system";
		sysMsg.content = systemMessage;
		messages.append(sysMsg);
	}

	ChatMessage userMsg;
	userMsg.role = "user";
	userMsg.content = userMessage;
	messages.append(userMsg);

	return chat(messages);
}

QNetworkReply* MiMoAPIClient::visionChat(const QString& userMessage, const QString& imageUrl, const QString& systemMessage)
{
	QList<ChatMessage> messages;

	if (!systemMessage.isEmpty()) {
		ChatMessage sysMsg;
		sysMsg.role = "system";
		sysMsg.content = systemMessage;
		messages.append(sysMsg);
	}

	ChatMessage userMsg;
	userMsg.role = "user";

	MessageContent textContent;
	textContent.type = MessageContent::Type::Text;
	textContent.text = userMessage;
	userMsg.contents.append(textContent);

	MessageContent imageContent;
	imageContent.type = MessageContent::Type::Image;
	imageContent.imageUrl = imageUrl;
	userMsg.contents.append(imageContent);

	messages.append(userMsg);

	return chat(messages);
}

MiMoAPIClient::ChatCompletionResponse MiMoAPIClient::parseChatCompletionResponse(const QByteArray& jsonData)
{
	ChatCompletionResponse response;

	QVariant variant = StelJsonParser::parse(jsonData);
	if (!variant.canConvert<QVariantMap>()) {
		throw std::runtime_error("Invalid JSON format");
	}

	QVariantMap root = variant.toMap();
	response.id = root.value("id").toString();
	response.object = root.value("object").toString();
	response.created = root.value("created").toLongLong();
	response.model = root.value("model").toString();
	response.usage = root.value("usage").toMap();

	QVariantList choicesList = root.value("choices").toList();
	for (const auto& choiceVariant : choicesList) {
		QVariantMap choiceMap = choiceVariant.toMap();
		ChatChoice choice;
		choice.index = choiceMap.value("index").toInt();
		choice.finishReason = choiceMap.value("finish_reason").toString();

		QVariantMap messageMap = choiceMap.value("message").toMap();
		choice.message.role = messageMap.value("role").toString();
		choice.message.content = messageMap.value("content").toString();
		if (messageMap.contains("tool_calls")) {
			choice.message.toolCalls = messageMap.value("tool_calls").toList();
		}

		response.choices.append(choice);
	}

	return response;
}

QString MiMoAPIClient::parseError(const QByteArray& jsonData)
{
	if (jsonData.isEmpty()) {
		return QString();
	}

	try {
		QVariant variant = StelJsonParser::parse(jsonData);
		if (variant.canConvert<QVariantMap>()) {
			QVariantMap root = variant.toMap();
			if (root.contains("error")) {
				QVariantMap error = root.value("error").toMap();
				QString message = error.value("message").toString();
				QString type = error.value("type").toString();
				if (!type.isEmpty()) {
					return QString("%1: %2").arg(type, message);
				}
				return message;
			}
		}
	} catch (...) {
		// Fall through to return empty string
	}

	return QString();
}
