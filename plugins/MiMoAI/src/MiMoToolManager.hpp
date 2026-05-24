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

#ifndef MIMOTOOLMANAGER_HPP
#define MIMOTOOLMANAGER_HPP

#include <QObject>
#include <QMap>
#include "MiMoTool.hpp"
#include "MiMoAPIClient.hpp"

class MiMoToolManager : public QObject
{
    Q_OBJECT

public:
    explicit MiMoToolManager(QObject* parent = nullptr);
    ~MiMoToolManager() override;

    void registerTool(MiMoTool* tool);
    QList<MiMoAPIClient::Tool> getToolDefinitions() const;
    QVariantMap executeTool(const QString& name, const QVariantMap& arguments);
    bool hasTool(const QString& name) const;

signals:
    void toolExecuted(const QString& toolName, const QVariantMap& result);

private:
    QMap<QString, MiMoTool*> m_tools;
};

#endif // MIMOTOOLMANAGER_HPP
