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

#include "MiMoToolManager.hpp"

MiMoToolManager::MiMoToolManager(QObject* parent)
    : QObject(parent)
{
}

MiMoToolManager::~MiMoToolManager()
{
    qDeleteAll(m_tools);
    m_tools.clear();
}

void MiMoToolManager::registerTool(MiMoTool* tool)
{
    if (tool)
    {
        m_tools[tool->name()] = tool;
    }
}

QList<MiMoAPIClient::Tool> MiMoToolManager::getToolDefinitions() const
{
    QList<MiMoAPIClient::Tool> tools;
    for (MiMoTool* tool : m_tools)
    {
        tools.append(tool->toToolDefinition());
    }
    return tools;
}

QVariantMap MiMoToolManager::executeTool(const QString& name, const QVariantMap& arguments)
{
    QVariantMap result;
    if (m_tools.contains(name))
    {
        result = m_tools[name]->execute(arguments);
        emit toolExecuted(name, result);
    }
    else
    {
        result["success"] = false;
        result["error"] = QString("Tool not found: %1").arg(name);
    }
    return result;
}

bool MiMoToolManager::hasTool(const QString& name) const
{
    return m_tools.contains(name);
}
