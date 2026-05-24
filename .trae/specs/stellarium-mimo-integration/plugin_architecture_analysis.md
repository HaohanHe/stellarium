# Stellarium 插件架构深度分析

## 1. 概述

Stellarium 采用模块化的插件架构，允许开发者动态扩展功能而无需修改核心代码。插件通过统一的接口与核心系统交互，主要基于 Qt 框架的插件系统实现。

## 2. 核心接口与基类

### 2.1 StelPluginInterface

这是所有 Stellarium 插件必须实现的接口，位于 `src/core/StelPluginInterface.hpp`。

**主要方法：**
- `getStelModule()` - 返回插件的主模块实例（继承自 StelModule）
- `getPluginInfo()` - 返回插件的元数据（ID、名称、作者等）
- `getExtensionList()` - 可选，用于插件间通信

**关键数据结构 - StelPluginInfo：**
```cpp
struct StelPluginInfo {
    QString id;              // 插件唯一标识符
    QString displayedName;   // 显示名称
    QString authors;         // 作者列表
    QString contact;         // 联系方式
    QString description;     // 描述
    QString version;         // 版本号
    QString license;         // 许可证
    QImage image;            // Logo
    bool startByDefault;     // 是否默认启动
};
```

### 2.2 StelModule

所有模块（包括插件）的基类，位于 `src/core/StelModule.hpp`。

**核心虚方法：**
- `init()` - 初始化模块
- `update(double deltaTime)` - 每帧更新
- `draw(StelCore* core)` - 绘制模块内容
- `handleMouseClicks()` / `handleMouseMoves()` / `handleKeys()` - 处理输入事件
- `getCallOrder(StelModuleActionName actionName)` - 定义模块执行顺序
- `configureGui(bool show)` - 配置 GUI 界面

**执行顺序枚举：**
```cpp
enum StelModuleActionName {
    ActionDraw,
    ActionUpdate,
    ActionHandleMouseClicks,
    ActionHandleMouseMoves,
    ActionHandleKeys
};
```

## 3. 插件目录结构

典型的插件目录结构（以 AngleMeasure 为例）：
```
plugins/
└── AngleMeasure/
    ├── CMakeLists.txt           # 主构建文件
    ├── AngleMeasure.qrc         # Qt 资源文件
    ├── COPYING                  # 许可证
    ├── ChangeLog                # 更新日志
    ├── README                   # 说明文档
    ├── bt_anglemeasure_off.png  # 图标资源
    ├── bt_anglemeasure_on.png
    ├── icons.svg
    └── src/
        ├── CMakeLists.txt       # 源代码构建文件
        ├── AngleMeasure.hpp     # 主模块头文件
        ├── AngleMeasure.cpp     # 主模块实现
        ├── [可选] gui/          # 对话框等 GUI 组件
        └── [可选] other/        # 其他辅助文件
```

## 4. CMake 构建系统

### 4.1 插件根目录 CMakeLists.txt

```cmake
# 插件版本和许可证定义
SET(ANGLEMEASURE_VERSION "0.5.1")
ADD_DEFINITIONS(-DANGLEMEASURE_PLUGIN_VERSION="${ANGLEMEASURE_VERSION}")
ADD_DEFINITIONS(-DANGLEMEASURE_PLUGIN_LICENSE="GNU GPLv2 or later")

# 添加子目录
ADD_SUBDIRECTORY(src)
```

### 4.2 src 目录 CMakeLists.txt

通常包含源代码编译规则和资源文件管理。

## 5. 插件实现模式

### 5.1 最小插件示例 - HelloStelModule

```cpp
// HelloStelModule.hpp
#include "StelModule.hpp"

class HelloStelModule : public StelModule {
public:
    HelloStelModule();
    ~HelloStelModule() override;
    
    void init() override;
    void draw(StelCore* core) override;
    double getCallOrder(StelModuleActionName actionName) const override;
};

// 插件接口类
#include "StelPluginInterface.hpp"

class HelloStelModuleStelPluginInterface : public QObject, public StelPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID StelPluginInterface_iid)
    Q_INTERFACES(StelPluginInterface)
public:
    StelModule* getStelModule() const override;
    StelPluginInfo getPluginInfo() const override;
};
```

### 5.2 完整插件示例 - AngleMeasure

AngleMeasure 插件展示了典型插件的完整实现：

**主要特性：**
- 继承自 StelModule
- 使用 Qt 属性系统 (`Q_PROPERTY`) 暴露配置项
- 实现了完整的输入事件处理
- 支持持久化配置（通过 QSettings）
- 集成 GUI 工具栏和对话框
- 使用 StelPainter 进行绘制

**关键方法分析：**

1. **初始化** - `init()`
   ```cpp
   void AngleMeasure::init() {
       // 加载配置
       // 注册快捷键动作
       // 创建工具栏按钮
   }
   ```

2. **绘制** - `draw(StelCore* core)`
   - 使用 StelPainter 在天球上绘制线条和文本
   - 支持多种坐标系（赤道坐标、地平坐标）

3. **交互** - 事件处理方法
   - `handleMouseClicks()` - 处理点击
   - `handleMouseMoves()` - 处理拖动
   - `handleKeys()` - 处理键盘

4. **执行顺序控制** - `getCallOrder()`
   ```cpp
   double AngleMeasure::getCallOrder(StelModuleActionName actionName) const {
       if (actionName == StelModule::ActionDraw)
           return StelApp::getInstance().getModuleMgr()
                  ->getModule("LandscapeMgr")->getCallOrder(actionName) + 10.;
       // ...
   }
   ```

### 5.3 在线查询插件 - OnlineQueries

这个插件展示了如何与在线服务集成：

**主要功能：**
- 查询 Wikipedia、AAVSO、GCVS 等在线数据库
- 使用 Qt WebEngine 展示结果
- 支持自定义网站查询
- 通过 StelObjectMgr 获取选中对象信息

## 6. 核心 API 分析

### 6.1 StelCore - 核心控制

**主要功能：**
- 坐标系转换（J2000、赤道坐标、地平坐标等）
- 投影管理
- 时间管理（儒略日、时区等）
- 观察者位置管理

**关键方法：**
- `getProjection(FrameType frameType, RefractionMode refractionMode)`
- `j2000ToAltAz(const Vec3d& v, RefractionMode refractionMode)`
- `altAzToJ2000(const Vec3d& v, RefractionMode refractionMode)`
- `getCurrentLocation()`
- `getJD()` / `setJD(double newJD)`

**坐标系类型：**
```cpp
enum FrameType {
    FrameAltAz,                    // 地平坐标
    FrameEquinoxEqu,               // 赤道坐标（当前分点）
    FrameJ2000,                    // J2000 赤道坐标
    FrameGalactic,                 // 银道坐标
    FrameSupergalactic,            // 超星系坐标
    // ...
};
```

### 6.2 StelMovementMgr - 视图控制

**主要功能：**
- 视角移动和缩放
- 望远镜模式（赤道/地平）
- 对象跟踪
- 视场角 (FOV) 控制

**关键方法：**
- `moveToJ2000(const Vec3d& aim, const Vec3d& aimUp, float duration)`
- `zoomTo(double aimFov, float zoomDuration)`
- `moveToObject(const StelObjectP& target, float duration)`
- `setFlagTracking(bool b)`

### 6.3 StelObjectMgr - 对象管理

**主要功能：**
- 选中对象管理
- 对象搜索（按名称、位置）
- 注册对象模块

**关键方法：**
- `findAndSelect(const StelCore* core, int x, int y, StelModuleSelectAction action)`
- `getSelectedObject()`
- `searchByName(const QString& name)`
- `searchByID(const QString& type, const QString& id)`

### 6.4 其他重要模块

- **StelApp** - 应用程序单例，访问所有模块
- **StelPainter** - 天球绘制工具
- **StelProjector** - 投影系统
- **StelObject** - 天体对象基类

## 7. 模块获取与通信

### 7.1 获取模块指针

使用 `GETSTELMODULE` 宏（通常在内部定义）：
```cpp
StelObjectMgr* objMgr = GETSTELMODULE(StelObjectMgr);
StelMovementMgr* moveMgr = GETSTELMODULE(StelMovementMgr);
```

### 7.2 插件间通信

通过 `getExtensionList()` 机制实现插件间通信，允许插件暴露 QObject 实例供其他插件使用。

## 8. 插件开发最佳实践

1. **正确使用 Qt 插件宏**
   ```cpp
   Q_PLUGIN_METADATA(IID StelPluginInterface_iid)
   Q_INTERFACES(StelPluginInterface)
   ```

2. **设置对象名称**
   ```cpp
   setObjectName("MyPluginName");
   ```

3. **合理使用 getCallOrder**
   - 确保在正确的层次绘制
   - 确保事件处理顺序正确

4. **使用 StelAction 管理快捷键**
   ```cpp
   addAction("actionName", "Group", "Description", this, "slot()", "Ctrl+K");
   ```

5. **配置持久化**
   - 使用 QSettings 保存用户配置
   - 在 init() 中加载，在 configureGui() 或析构时保存

6. **资源管理**
   - 使用 Qt 资源系统 (.qrc) 管理图片等资源
   - 在 getPluginInfo() 中调用 Q_INIT_RESOURCE

## 9. 总结

Stellarium 的插件架构设计清晰，具有以下特点：

- **模块化** - 各功能独立封装为模块
- **可扩展** - 动态加载插件，无需重新编译
- **统一接口** - StelModule 和 StelPluginInterface 提供标准契约
- **丰富 API** - 核心模块提供完整的天文计算和 UI 功能
- **灵活交互** - 支持多种输入事件和 GUI 集成方式

这种架构使得第三方开发者可以轻松创建各类天文功能插件，从简单的工具到复杂的在线数据集成。
