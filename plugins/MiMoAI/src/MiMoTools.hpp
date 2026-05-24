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

#ifndef MIMOTOOLS_HPP
#define MIMOTOOLS_HPP

#include "MiMoTool.hpp"
#include "StelCore.hpp"
#include "StelObjectMgr.hpp"
#include "StelMovementMgr.hpp"
#include "StelLocationMgr.hpp"
#include "StelActionMgr.hpp"

class SearchObjectTool : public MiMoTool
{
    Q_OBJECT

public:
    explicit SearchObjectTool(QObject* parent = nullptr);
    QString name() const override;
    QString description() const override;
    QVariantMap parameters() const override;
    QVariantMap execute(const QVariantMap& arguments) override;
};

class GetObjectInfoTool : public MiMoTool
{
    Q_OBJECT

public:
    explicit GetObjectInfoTool(QObject* parent = nullptr);
    QString name() const override;
    QString description() const override;
    QVariantMap parameters() const override;
    QVariantMap execute(const QVariantMap& arguments) override;
};

class LocateObjectTool : public MiMoTool
{
    Q_OBJECT

public:
    explicit LocateObjectTool(QObject* parent = nullptr);
    QString name() const override;
    QString description() const override;
    QVariantMap parameters() const override;
    QVariantMap execute(const QVariantMap& arguments) override;
};

class GetCurrentViewTool : public MiMoTool
{
    Q_OBJECT

public:
    explicit GetCurrentViewTool(QObject* parent = nullptr);
    QString name() const override;
    QString description() const override;
    QVariantMap parameters() const override;
    QVariantMap execute(const QVariantMap& arguments) override;
};

class SetTimeTool : public MiMoTool
{
    Q_OBJECT

public:
    explicit SetTimeTool(QObject* parent = nullptr);
    QString name() const override;
    QString description() const override;
    QVariantMap parameters() const override;
    QVariantMap execute(const QVariantMap& arguments) override;
};

class ControlTimeTool : public MiMoTool
{
    Q_OBJECT

public:
    explicit ControlTimeTool(QObject* parent = nullptr);
    QString name() const override;
    QString description() const override;
    QVariantMap parameters() const override;
    QVariantMap execute(const QVariantMap& arguments) override;
};

class SetLocationTool : public MiMoTool
{
    Q_OBJECT

public:
    explicit SetLocationTool(QObject* parent = nullptr);
    QString name() const override;
    QString description() const override;
    QVariantMap parameters() const override;
    QVariantMap execute(const QVariantMap& arguments) override;
};

class SetZoomTool : public MiMoTool
{
    Q_OBJECT

public:
    explicit SetZoomTool(QObject* parent = nullptr);
    QString name() const override;
    QString description() const override;
    QVariantMap parameters() const override;
    QVariantMap execute(const QVariantMap& arguments) override;
};

class ToggleFeatureTool : public MiMoTool
{
    Q_OBJECT

public:
    explicit ToggleFeatureTool(QObject* parent = nullptr);
    QString name() const override;
    QString description() const override;
    QVariantMap parameters() const override;
    QVariantMap execute(const QVariantMap& arguments) override;
};

class TakeScreenshotTool : public MiMoTool
{
    Q_OBJECT

public:
    explicit TakeScreenshotTool(QObject* parent = nullptr);
    QString name() const override;
    QString description() const override;
    QVariantMap parameters() const override;
    QVariantMap execute(const QVariantMap& arguments) override;
};

#endif // MIMOTOOLS_HPP
