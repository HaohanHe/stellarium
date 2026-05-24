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

#include "MiMoTools.hpp"
#include "StelApp.hpp"
#include "StelTranslator.hpp"
#include <QDateTime>

// SearchObjectTool
SearchObjectTool::SearchObjectTool(QObject* parent)
    : MiMoTool(parent)
{
}

QString SearchObjectTool::name() const
{
    return "search_object";
}

QString SearchObjectTool::description() const
{
    return "Search for celestial objects by name";
}

QVariantMap SearchObjectTool::parameters() const
{
    QVariantMap params;
    params["type"] = "object";
    QVariantMap properties;
    QVariantMap name;
    name["type"] = "string";
    name["description"] = "Name of the celestial object to search for";
    properties["name"] = name;
    params["properties"] = properties;
    QStringList required;
    required << "name";
    params["required"] = required;
    return params;
}

QVariantMap SearchObjectTool::execute(const QVariantMap& arguments)
{
    QString name = arguments.value("name").toString();
    if (name.isEmpty())
    {
        return errorResult("Object name is required");
    }

    StelObjectMgr* objMgr = GETSTELMODULE(StelObjectMgr);
    StelObjectP obj = objMgr->searchByName(name);

    if (!obj)
    {
        return errorResult(QString("Object not found: %1").arg(name));
    }

    QVariantMap data;
    data["name"] = obj->getEnglishName();
    data["localized_name"] = obj->getNameI18n();
    data["type"] = obj->getType();

    return successResult(QString("Found object: %1").arg(obj->getEnglishName()), data);
}

// GetObjectInfoTool
GetObjectInfoTool::GetObjectInfoTool(QObject* parent)
    : MiMoTool(parent)
{
}

QString GetObjectInfoTool::name() const
{
    return "get_object_info";
}

QString GetObjectInfoTool::description() const
{
    return "Get detailed information about a celestial object";
}

QVariantMap GetObjectInfoTool::parameters() const
{
    QVariantMap params;
    params["type"] = "object";
    QVariantMap properties;
    QVariantMap name;
    name["type"] = "string";
    name["description"] = "Name of the celestial object";
    properties["name"] = name;
    params["properties"] = properties;
    QStringList required;
    required << "name";
    params["required"] = required;
    return params;
}

QVariantMap GetObjectInfoTool::execute(const QVariantMap& arguments)
{
    QString name = arguments.value("name").toString();
    if (name.isEmpty())
    {
        return errorResult("Object name is required");
    }

    StelObjectMgr* objMgr = GETSTELMODULE(StelObjectMgr);
    StelObjectP obj = objMgr->searchByName(name);

    if (!obj)
    {
        return errorResult(QString("Object not found: %1").arg(name));
    }

    StelCore* core = StelApp::getInstance().getCore();
    QVariantMap info = StelObjectMgr::getObjectInfo(obj);

    return successResult(QString("Information about %1").arg(obj->getEnglishName()), info);
}

// LocateObjectTool
LocateObjectTool::LocateObjectTool(QObject* parent)
    : MiMoTool(parent)
{
}

QString LocateObjectTool::name() const
{
    return "locate_object";
}

QString LocateObjectTool::description() const
{
    return "Locate and center the view on a celestial object";
}

QVariantMap LocateObjectTool::parameters() const
{
    QVariantMap params;
    params["type"] = "object";
    QVariantMap properties;
    QVariantMap name;
    name["type"] = "string";
    name["description"] = "Name of the celestial object to locate";
    properties["name"] = name;
    QVariantMap zoom;
    zoom["type"] = "boolean";
    zoom["description"] = "Whether to zoom in on the object";
    properties["zoom"] = zoom;
    params["properties"] = properties;
    QStringList required;
    required << "name";
    params["required"] = required;
    return params;
}

QVariantMap LocateObjectTool::execute(const QVariantMap& arguments)
{
    QString name = arguments.value("name").toString();
    bool zoom = arguments.value("zoom", true).toBool();

    if (name.isEmpty())
    {
        return errorResult("Object name is required");
    }

    StelObjectMgr* objMgr = GETSTELMODULE(StelObjectMgr);
    StelObjectP obj = objMgr->searchByName(name);

    if (!obj)
    {
        return errorResult(QString("Object not found: %1").arg(name));
    }

    objMgr->setSelectedObject(obj);

    StelMovementMgr* moveMgr = StelApp::getInstance().getCore()->getMovementMgr();
    moveMgr->moveToObject(obj, 1.0f, zoom ? StelMovementMgr::ZoomIn : StelMovementMgr::ZoomNone);

    return successResult(QString("Located object: %1").arg(obj->getEnglishName()));
}

// GetCurrentViewTool
GetCurrentViewTool::GetCurrentViewTool(QObject* parent)
    : MiMoTool(parent)
{
}

QString GetCurrentViewTool::name() const
{
    return "get_current_view";
}

QString GetCurrentViewTool::description() const
{
    return "Get information about the current view";
}

QVariantMap GetCurrentViewTool::parameters() const
{
    QVariantMap params;
    params["type"] = "object";
    QVariantMap properties;
    params["properties"] = properties;
    return params;
}

QVariantMap GetCurrentViewTool::execute(const QVariantMap& arguments)
{
    Q_UNUSED(arguments)

    StelCore* core = StelApp::getInstance().getCore();
    StelMovementMgr* moveMgr = core->getMovementMgr();

    StelLocation location = core->getCurrentLocation();
    double jd = core->getJD();
    QDateTime dateTime = StelUtils::getDateTimeFromJulianDay(jd);

    QVariantMap data;
    data["location"] = location.name;
    data["latitude"] = location.latitude;
    data["longitude"] = location.longitude;
    data["altitude"] = location.altitude;
    data["datetime"] = dateTime.toString(Qt::ISODate);
    data["julian_day"] = jd;
    data["fov"] = moveMgr->getCurrentFov();

    return successResult("Current view information", data);
}

// SetTimeTool
SetTimeTool::SetTimeTool(QObject* parent)
    : MiMoTool(parent)
{
}

QString SetTimeTool::name() const
{
    return "set_time";
}

QString SetTimeTool::description() const
{
    return "Set the simulation time";
}

QVariantMap SetTimeTool::parameters() const
{
    QVariantMap params;
    params["type"] = "object";
    QVariantMap properties;
    QVariantMap datetime;
    datetime["type"] = "string";
    datetime["description"] = "ISO 8601 date and time string (e.g., 2024-05-20T20:30:00)";
    properties["datetime"] = datetime;
    QVariantMap use_now;
    use_now["type"] = "boolean";
    use_now["description"] = "Set to current real time";
    properties["use_now"] = use_now;
    params["properties"] = properties;
    return params;
}

QVariantMap SetTimeTool::execute(const QVariantMap& arguments)
{
    bool useNow = arguments.value("use_now", false).toBool();
    QString datetimeStr = arguments.value("datetime").toString();

    StelCore* core = StelApp::getInstance().getCore();

    if (useNow)
    {
        core->setTimeNow();
        return successResult("Time set to current real time");
    }
    else if (!datetimeStr.isEmpty())
    {
        QDateTime datetime = QDateTime::fromString(datetimeStr, Qt::ISODate);
        if (datetime.isValid())
        {
            double jd = StelUtils::getJulianDayFromDateTime(datetime);
            core->setJD(jd);
            return successResult(QString("Time set to: %1").arg(datetime.toString()));
        }
        else
        {
            return errorResult("Invalid datetime format. Use ISO 8601.");
        }
    }
    else
    {
        return errorResult("Either datetime or use_now must be specified");
    }
}

// ControlTimeTool
ControlTimeTool::ControlTimeTool(QObject* parent)
    : MiMoTool(parent)
{
}

QString ControlTimeTool::name() const
{
    return "control_time";
}

QString ControlTimeTool::description() const
{
    return "Control time flow (pause, play, speed)";
}

QVariantMap ControlTimeTool::parameters() const
{
    QVariantMap params;
    params["type"] = "object";
    QVariantMap properties;
    QVariantMap action;
    action["type"] = "string";
    action["description"] = "Action to perform: pause, play, speed_up, speed_down, real_time";
    action["enum"] = QStringList() << "pause" << "play" << "speed_up" << "speed_down" << "real_time";
    properties["action"] = action;
    params["properties"] = properties;
    QStringList required;
    required << "action";
    params["required"] = required;
    return params;
}

QVariantMap ControlTimeTool::execute(const QVariantMap& arguments)
{
    QString action = arguments.value("action").toString();
    StelCore* core = StelApp::getInstance().getCore();

    if (action == "pause")
    {
        core->setZeroTimeSpeed();
        return successResult("Time paused");
    }
    else if (action == "play" || action == "real_time")
    {
        core->setRealTimeSpeed();
        return successResult("Time playing at real speed");
    }
    else if (action == "speed_up")
    {
        core->increaseTimeSpeed();
        return successResult("Time speed increased");
    }
    else if (action == "speed_down")
    {
        core->decreaseTimeSpeed();
        return successResult("Time speed decreased");
    }
    else
    {
        return errorResult("Invalid action. Valid actions: pause, play, speed_up, speed_down, real_time");
    }
}

// SetLocationTool
SetLocationTool::SetLocationTool(QObject* parent)
    : MiMoTool(parent)
{
}

QString SetLocationTool::name() const
{
    return "set_location";
}

QString SetLocationTool::description() const
{
    return "Set the observer location";
}

QVariantMap SetLocationTool::parameters() const
{
    QVariantMap params;
    params["type"] = "object";
    QVariantMap properties;
    QVariantMap location_name;
    location_name["type"] = "string";
    location_name["description"] = "Name of the location to set";
    properties["location_name"] = location_name;
    QVariantMap latitude;
    latitude["type"] = "number";
    latitude["description"] = "Latitude in degrees";
    properties["latitude"] = latitude;
    QVariantMap longitude;
    longitude["type"] = "number";
    longitude["description"] = "Longitude in degrees";
    properties["longitude"] = longitude;
    QVariantMap altitude;
    altitude["type"] = "number";
    altitude["description"] = "Altitude in meters";
    properties["altitude"] = altitude;
    params["properties"] = properties;
    return params;
}

QVariantMap SetLocationTool::execute(const QVariantMap& arguments)
{
    QString locationName = arguments.value("location_name").toString();
    StelCore* core = StelApp::getInstance().getCore();
    StelLocationMgr* locMgr = GETSTELMODULE(StelLocationMgr);

    if (!locationName.isEmpty())
    {
        StelLocation location = locMgr->locationForString(locationName);
        if (location.isValid())
        {
            core->moveObserverTo(location);
            return successResult(QString("Location set to: %1").arg(location.name));
        }
        else
        {
            return errorResult(QString("Location not found: %1").arg(locationName));
        }
    }
    else if (arguments.contains("latitude") && arguments.contains("longitude"))
    {
        double latitude = arguments.value("latitude").toDouble();
        double longitude = arguments.value("longitude").toDouble();
        double altitude = arguments.value("altitude", 0.0).toDouble();

        StelLocation location;
        location.latitude = latitude;
        location.longitude = longitude;
        location.altitude = altitude;
        location.name = QString("Custom (%1, %2)").arg(latitude).arg(longitude);

        core->moveObserverTo(location);
        return successResult(QString("Location set to custom coordinates: %1, %2").arg(latitude).arg(longitude));
    }
    else
    {
        return errorResult("Either location_name or latitude and longitude must be specified");
    }
}

// SetZoomTool
SetZoomTool::SetZoomTool(QObject* parent)
    : MiMoTool(parent)
{
}

QString SetZoomTool::name() const
{
    return "set_zoom";
}

QString SetZoomTool::description() const
{
    return "Set the zoom level (field of view)";
}

QVariantMap SetZoomTool::parameters() const
{
    QVariantMap params;
    params["type"] = "object";
    QVariantMap properties;
    QVariantMap fov;
    fov["type"] = "number";
    fov["description"] = "Field of view in degrees";
    properties["fov"] = fov;
    QVariantMap action;
    action["type"] = "string";
    action["description"] = "Action to perform: zoom_in, zoom_out, default";
    action["enum"] = QStringList() << "zoom_in" << "zoom_out" << "default";
    properties["action"] = action;
    params["properties"] = properties;
    return params;
}

QVariantMap SetZoomTool::execute(const QVariantMap& arguments)
{
    QString action = arguments.value("action").toString();
    StelMovementMgr* moveMgr = StelApp::getInstance().getCore()->getMovementMgr();

    if (arguments.contains("fov"))
    {
        double fov = arguments.value("fov").toDouble();
        moveMgr->zoomTo(fov);
        return successResult(QString("Zoom set to: %1 degrees").arg(fov));
    }
    else if (action == "zoom_in")
    {
        moveMgr->zoomIn(true);
        return successResult("Zoomed in");
    }
    else if (action == "zoom_out")
    {
        moveMgr->zoomOut(true);
        return successResult("Zoomed out");
    }
    else if (action == "default")
    {
        moveMgr->resetInitViewPos();
        return successResult("Zoom reset to default");
    }
    else
    {
        return errorResult("Either fov or action must be specified. Valid actions: zoom_in, zoom_out, default");
    }
}

// ToggleFeatureTool
ToggleFeatureTool::ToggleFeatureTool(QObject* parent)
    : MiMoTool(parent)
{
}

QString ToggleFeatureTool::name() const
{
    return "toggle_feature";
}

QString ToggleFeatureTool::description() const
{
    return "Toggle various Stellarium features (constellations, atmosphere, etc.)";
}

QVariantMap ToggleFeatureTool::parameters() const
{
    QVariantMap params;
    params["type"] = "object";
    QVariantMap properties;
    QVariantMap feature;
    feature["type"] = "string";
    feature["description"] = "Feature to toggle: constellations, atmosphere, fog, ground, star_labels, planet_labels, cardinal_points, equatorial_grid, azimuthal_grid, ecliptic_line, milky_way";
    feature["enum"] = QStringList() << "constellations" << "atmosphere" << "fog" << "ground" << "star_labels" << "planet_labels" << "cardinal_points" << "equatorial_grid" << "azimuthal_grid" << "ecliptic_line" << "milky_way";
    properties["feature"] = feature;
    QVariantMap enable;
    enable["type"] = "boolean";
    enable["description"] = "True to enable, false to disable (optional, toggles if not specified)";
    properties["enable"] = enable;
    params["properties"] = properties;
    QStringList required;
    required << "feature";
    params["required"] = required;
    return params;
}

QVariantMap ToggleFeatureTool::execute(const QVariantMap& arguments)
{
    QString feature = arguments.value("feature").toString();
    QVariant enableVar = arguments.value("enable");

    StelActionMgr* actionMgr = StelApp::getInstance().getStelActionManager();
    StelAction* action = nullptr;
    QString actionName;

    if (feature == "constellations")
    {
        actionName = "actionShow_Constellation_Lines";
    }
    else if (feature == "atmosphere")
    {
        actionName = "actionShow_Atmosphere";
    }
    else if (feature == "fog")
    {
        actionName = "actionShow_Fog";
    }
    else if (feature == "ground")
    {
        actionName = "actionShow_Ground";
    }
    else if (feature == "star_labels")
    {
        actionName = "actionShow_StarLabels";
    }
    else if (feature == "planet_labels")
    {
        actionName = "actionShow_PlanetLabels";
    }
    else if (feature == "cardinal_points")
    {
        actionName = "actionShow_Cardinal_Points";
    }
    else if (feature == "equatorial_grid")
    {
        actionName = "actionShow_Equatorial_Grid";
    }
    else if (feature == "azimuthal_grid")
    {
        actionName = "actionShow_Azimuthal_Grid";
    }
    else if (feature == "ecliptic_line")
    {
        actionName = "actionShow_Ecliptic";
    }
    else if (feature == "milky_way")
    {
        actionName = "actionShow_MilkyWay";
    }
    else
    {
        return errorResult(QString("Unknown feature: %1").arg(feature));
    }

    action = actionMgr->findAction(actionName);
    if (!action)
    {
        return errorResult(QString("Action not found for feature: %1").arg(feature));
    }

    if (enableVar.isValid())
    {
        bool enable = enableVar.toBool();
        action->setChecked(enable);
        return successResult(QString("%1 %2").arg(feature).arg(enable ? "enabled" : "disabled"));
    }
    else
    {
        action->toggle();
        return successResult(QString("%1 toggled").arg(feature));
    }
}

// TakeScreenshotTool
TakeScreenshotTool::TakeScreenshotTool(QObject* parent)
    : MiMoTool(parent)
{
}

QString TakeScreenshotTool::name() const
{
    return "take_screenshot";
}

QString TakeScreenshotTool::description() const
{
    return "Take a screenshot of the current view";
}

QVariantMap TakeScreenshotTool::parameters() const
{
    QVariantMap params;
    params["type"] = "object";
    QVariantMap properties;
    params["properties"] = properties;
    return params;
}

QVariantMap TakeScreenshotTool::execute(const QVariantMap& arguments)
{
    Q_UNUSED(arguments)

    StelActionMgr* actionMgr = StelApp::getInstance().getStelActionManager();
    StelAction* action = actionMgr->findAction("actionSave_Screenshot");

    if (action)
    {
        action->trigger();
        return successResult("Screenshot taken");
    }
    else
    {
        return errorResult("Screenshot action not found");
    }
}
