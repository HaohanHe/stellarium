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

#include "MiMoTool.hpp"

MiMoTool::MiMoTool(QObject* parent)
    : QObject(parent)
{
}

MiMoTool::~MiMoTool()
{
}

MiMoAPIClient::Tool MiMoTool::toToolDefinition() const
{
    MiMoAPIClient::Tool tool;
    tool.type = "function";
    tool.name = name();
    tool.description = description();
    tool.parameters = parameters();
    return tool;
}

QVariantMap MiMoTool::successResult(const QString& message, const QVariantMap& data)
{
    QVariantMap result;
    result["success"] = true;
    result["message"] = message;
    if (!data.isEmpty())
    {
        result["data"] = data;
    }
    return result;
}

QVariantMap MiMoTool::errorResult(const QString& error)
{
    QVariantMap result;
    result["success"] = false;
    result["error"] = error;
    return result;
}
