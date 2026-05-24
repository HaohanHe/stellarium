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

#ifndef MIMOTOOL_HPP
#define MIMOTOOL_HPP

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include "MiMoAPIClient.hpp"

class MiMoTool : public QObject
{
    Q_OBJECT

public:
    explicit MiMoTool(QObject* parent = nullptr);
    virtual ~MiMoTool() override;

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QVariantMap parameters() const = 0;
    virtual QVariantMap execute(const QVariantMap& arguments) = 0;

    MiMoAPIClient::Tool toToolDefinition() const;

protected:
    QVariantMap successResult(const QString& message, const QVariantMap& data = QVariantMap());
    QVariantMap errorResult(const QString& error);
};

#endif // MIMOTOOL_HPP
