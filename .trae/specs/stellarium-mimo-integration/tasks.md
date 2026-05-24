# Stellarium 小米 MiMo 多模态 LLM 集成 - 实现计划

## [ ] Task 1: 研究现有插件架构
- **Priority**: P0
- **Depends On**: None
- **Description**: 
  - 深入研究 Stellarium 的插件架构
  - 查看 OnlineQueries 等现有插件的实现
  - 了解 Stellarium 的 Qt 模块使用方式
  - 研究 StelModule 基类和插件接口
- **Acceptance Criteria Addressed**: FR-1
- **Test Requirements**:
  - `programmatic` TR-1.1: 至少分析 2 个现有插件的源代码结构
  - `human-judgement` TR-1.2: 编写插件开发架构分析文档
- **Notes**: 重点关注 [OnlineQueries 插件](file:///workspace/plugins/OnlineQueries/src) 和 [StelPluginInterface](file:///workspace/src/core/StelPluginInterface.hpp)

## [ ] Task 2: 创建插件基本框架
- **Priority**: P0
- **Depends On**: Task 1
- **Description**: 
  - 在 plugins/ 目录下创建 MiMoAI 插件目录
  - 创建基本的 CMakeLists.txt 配置
  - 创建主插件类继承自 StelModule
  - 创建插件接口类继承自 StelPluginInterface
  - 添加基本的插件元数据和描述
- **Acceptance Criteria Addressed**: FR-1, AC-1
- **Test Requirements**:
  - `programmatic` TR-2.1: 插件能被 Stellarium 正确加载
  - `programmatic` TR-2.2: 插件能在插件管理器中显示
  - `human-judgement` TR-2.3: 代码结构符合 Stellarium 规范
- **Notes**: 参考 [AngleMeasure 插件](file:///workspace/plugins/AngleMeasure/src)的目录结构

## [ ] Task 3: 实现 API 客户端核心模块
- **Priority**: P0
- **Depends On**: Task 2
- **Description**: 
  - 创建 MiMoAPIClient 类
  - 实现 OpenAI 兼容协议的 HTTP 请求封装
  - 支持配置不同的集群 URL（中国、新加坡、欧洲）
  - 实现认证头和 API Key 管理
  - 实现基本的聊天完成 API 调用
- **Acceptance Criteria Addressed**: FR-1, FR-2, FR-3
- **Test Requirements**:
  - `programmatic` TR-3.1: 能构造正确的 API 请求 JSON
  - `programmatic` TR-3.2: 能处理 API 响应 JSON
  - `programmatic` TR-3.3: 能正确处理错误响应
  - `human-judgement` TR-3.4: 代码有适当的错误处理
- **Notes**: 使用 Qt 的 QNetworkAccessManager 进行网络请求

## [ ] Task 4: 实现配置界面
- **Priority**: P0
- **Depends On**: Task 3
- **Description**: 
  - 创建配置对话框 UI（.ui 文件）
  - 实现 API Key 输入和安全存储
  - 实现集群选择下拉框
  - 实现模型选择（如需要）
  - 实现系统提示词编辑
  - 保存配置到 Stellarium 的配置系统
- **Acceptance Criteria Addressed**: FR-2, AC-2
- **Test Requirements**:
  - `programmatic` TR-4.1: 配置能正确保存和读取
  - `human-judgement` TR-4.2: 配置界面布局美观易用
  - `programmatic` TR-4.3: API Key 存储在安全的位置
- **Notes**: 参考 [OnlineQueries 配置](file:///workspace/plugins/OnlineQueries/src)的实现方式

## [ ] Task 5: 实现对话界面
- **Priority**: P0
- **Depends On**: Task 4
- **Description**: 
  - 创建对话窗口 UI
  - 实现消息显示区域（区分用户和 AI 消息）
  - 实现输入框和发送按钮
  - 实现加载状态指示器
  - 支持富文本显示（Markdown 渲染）
- **Acceptance Criteria Addressed**: FR-3, AC-3
- **Test Requirements**:
  - `programmatic` TR-5.1: 用户消息能正确显示
  - `programmatic` TR-5.2: AI 响应能正确显示
  - `human-judgement` TR-5.3: 界面交互流畅自然
- **Notes**: 可考虑使用 QTextEdit 或 QWebEngineView 显示消息

## [ ] Task 6: 实现多轮对话功能
- **Priority**: P1
- **Depends On**: Task 5
- **Description**: 
  - 管理对话历史上下文
  - 在每次请求中正确包含历史消息
  - 实现上下文长度管理（防止 token 超限）
  - 提供清空对话历史的功能
- **Acceptance Criteria Addressed**: FR-3, AC-4
- **Test Requirements**:
  - `programmatic` TR-6.1: 历史消息被正确包含在 API 请求中
  - `human-judgement` TR-6.2: AI 能理解对话上下文
  - `programmatic` TR-6.3: 清空功能正常工作
- **Notes**: 需要设计合理的上下文裁剪策略

## [ ] Task 7: 实现对话历史记录
- **Priority**: P1
- **Depends On**: Task 6
- **Description**: 
  - 设计对话历史的存储格式
  - 实现对话历史的保存功能
  - 实现对话历史的加载和恢复功能
  - 提供历史对话列表界面
- **Acceptance Criteria Addressed**: FR-7, AC-8
- **Test Requirements**:
  - `programmatic` TR-7.1: 对话历史能正确保存到文件
  - `programmatic` TR-7.2: 对话历史能正确从文件加载
  - `human-judgement` TR-7.3: 历史对话列表界面易用
- **Notes**: 考虑使用 JSON 格式存储

## [ ] Task 8: 实现快捷问题功能
- **Priority**: P1
- **Depends On**: Task 5
- **Description**: 
  - 设计预设的天文问题列表
  - 在对话界面添加快捷问题按钮
  - 点击按钮自动发送对应问题
  - 支持用户自定义快捷问题（可选）
- **Acceptance Criteria Addressed**: FR-8, AC-9
- **Test Requirements**:
  - `programmatic` TR-8.1: 点击快捷按钮发送正确的问题
  - `human-judgement` TR-8.2: 预设问题与天文相关
- **Notes**: 预设问题可包括："如何找到北极星？"、"什么是流星雨？"等

## [ ] Task 9: 集成天体信息功能
- **Priority**: P1
- **Depends On**: Task 6
- **Description**: 
  - 监听 Stellarium 的天体选择事件
  - 获取选中天体的详细信息（名称、类型、坐标等）
  - 在信息面板或工具栏添加"询问 AI"按钮
  - 自动构造关于该天体的查询并发送给 LLM
- **Acceptance Criteria Addressed**: FR-4, FR-9, AC-5
- **Test Requirements**:
  - `programmatic` TR-9.1: 能正确获取选中天体信息
  - `programmatic` TR-9.2: 点击按钮能发送正确的查询
  - `human-judgement` TR-9.3: UI 集成自然不突兀
- **Notes**: 研究 StelObjectMgr 了解如何获取选中天体

## [ ] Task 10: 实现图片理解功能
- **Priority**: P1
- **Depends On**: Task 6
- **Description**: 
  - 实现截图功能，捕获当前 Stellarium 星空画面
  - 实现图片上传功能
  - 将图片编码为 base64 格式
  - 构造包含图片的 API 请求（OpenAI vision 格式）
  - 显示图片和 AI 的分析结果
- **Acceptance Criteria Addressed**: FR-5, AC-6
- **Test Requirements**:
  - `programmatic` TR-10.1: 截图功能正常工作
  - `programmatic` TR-10.2: 图片编码格式正确
  - `human-judgement` TR-10.3: AI 能正确分析天文图片
- **Notes**: 参考 OpenAI Vision API 格式

## [ ] Task 11: 实现 TTS 朗读功能
- **Priority**: P2
- **Depends On**: Task 6
- **Description**: 
  - 实现 MiMo TTS API 调用
  - 实现音频播放功能
  - 在对话消息旁添加朗读按钮
  - 提供停止朗读的功能
  - 支持语速等参数配置（可选）
- **Acceptance Criteria Addressed**: FR-6, AC-7
- **Test Requirements**:
  - `programmatic` TR-11.1: TTS API 调用正常
  - `human-judgement` TR-11.2: 音频播放清晰流畅
  - `programmatic` TR-11.3: 停止功能正常工作
- **Notes**: 使用 Qt Multimedia 模块播放音频

## [ ] Task 12: 添加工具栏按钮和菜单
- **Priority**: P2
- **Depends On**: Task 5
- **Description**: 
  - 在 Stellarium 工具栏添加 AI 助手按钮
  - 添加相关菜单项
  - 实现按钮点击打开对话窗口
  - 添加键盘快捷键（可选）
- **Acceptance Criteria Addressed**: FR-1, AC-1
- **Test Requirements**:
  - `programmatic` TR-12.1: 工具栏按钮正确显示
  - `programmatic` TR-12.2: 点击按钮打开对话窗口
- **Notes**: 参考其他插件的工具栏实现

## [ ] Task 13: 实现错误处理和用户反馈
- **Priority**: P2
- **Depends On**: Task 3
- **Description**: 
  - 完善网络错误处理
  - 实现 API 错误解析和显示
  - 添加友好的错误提示对话框
  - 实现请求超时处理
  - 添加详细的日志记录
- **Acceptance Criteria Addressed**: NFR-2, NFR-3
- **Test Requirements**:
  - `programmatic` TR-13.1: 网络错误能被正确捕获
  - `human-judgement` TR-13.2: 错误提示对用户友好
  - `programmatic` TR-13.3: 日志记录完整有用
- **Notes**: 使用 [StelLogger](file:///workspace/src/StelLogger.hpp) 记录日志

## [ ] Task 14: 编写文档和翻译
- **Priority**: P2
- **Depends On**: Task 12
- **Description**: 
  - 添加插件说明文档
  - 添加中文和英文翻译字符串
  - 更新插件元数据
- **Acceptance Criteria Addressed**: NFR-5
- **Test Requirements**:
  - `human-judgement` TR-14.1: 文档清晰易懂
  - `programmatic` TR-14.2: 翻译资源正确配置
- **Notes**: 参考其他插件的翻译文件

## [ ] Task 15: 研究 Stellarium 核心 API
- **Priority**: P0
- **Depends On**: Task 1
- **Description**: 
  - 研究 Stellarium 如何控制视角移动
  - 研究如何搜索和查找天体
  - 研究如何设置和控制时间
  - 研究如何调用各种功能（显示/隐藏大气层、星座线等）
  - 研究如何获取当前视野信息
- **Acceptance Criteria Addressed**: FR-10, FR-12
- **Test Requirements**:
  - `programmatic` TR-15.1: 列出可用的 API 和方法
  - `human-judgement` TR-15.2: 编写 API 使用指南文档
- **Notes**: 重点研究 [StelCore](file:///workspace/src/core/StelCore.hpp)、[StelMovementMgr](file:///workspace/src/core/StelMovementMgr.hpp)、[StelObjectMgr](file:///workspace/src/core/StelObjectMgr.hpp) 等

## [ ] Task 16: 设计并实现工具系统框架
- **Priority**: P0
- **Depends On**: Task 3, Task 15
- **Description**: 
  - 设计工具基类和接口
  - 实现工具注册和管理系统
  - 实现 OpenAI 兼容的 Function Calling 协议支持
  - 实现工具调用的执行和结果返回
- **Acceptance Criteria Addressed**: FR-11
- **Test Requirements**:
  - `programmatic` TR-16.1: 工具能正确注册和发现
  - `programmatic` TR-16.2: 能正确生成工具定义 JSON Schema
  - `programmatic` TR-16.3: 能正确处理工具调用和返回结果
- **Notes**: 参考 OpenAI Function Calling 规范

## [ ] Task 17: 实现核心 MCP 工具 - 搜索和定位
- **Priority**: P0
- **Depends On**: Task 16
- **Description**: 
  - 实现 search_object 工具：搜索天体
  - 实现 get_object_info 工具：获取天体详情
  - 实现 locate_object 工具：定位到指定天体
  - 实现 get_current_view 工具：获取当前视野信息
- **Acceptance Criteria Addressed**: FR-12, AC-11, AC-12, AC-13
- **Test Requirements**:
  - `programmatic` TR-17.1: 搜索工具能找到正确的天体
  - `programmatic` TR-17.2: 定位工具能正确移动视角
  - `programmatic` TR-17.3: 获取信息工具返回完整数据
- **Notes**: 需要很好地处理各种天体类型

## [ ] Task 18: 实现 MCP 工具 - 时间和视图控制
- **Priority**: P1
- **Depends On**: Task 16
- **Description**: 
  - 实现 set_time 工具：设置日期时间
  - 实现 control_time 工具：控制时间流动（开始/暂停/速度）
  - 实现 set_location 工具：设置观测位置
  - 实现 set_zoom 工具：控制缩放
- **Acceptance Criteria Addressed**: FR-12, AC-14
- **Test Requirements**:
  - `programmatic` TR-18.1: 时间设置正确
  - `programmatic` TR-18.2: 时间控制功能正常
  - `programmatic` TR-18.3: 位置和缩放设置生效
- **Notes**: 时间格式处理要特别注意

## [ ] Task 19: 实现 MCP 工具 - 功能切换和截图
- **Priority**: P1
- **Depends On**: Task 16
- **Description**: 
  - 实现 toggle_feature 工具：切换各种功能（大气层、星座线、标签等）
  - 实现 take_screenshot 工具：截图并返回
  - 实现 list_features 工具：列出可用功能
- **Acceptance Criteria Addressed**: FR-12
- **Test Requirements**:
  - `programmatic` TR-19.1: 功能切换正常
  - `programmatic` TR-19.2: 截图功能正常
- **Notes**: 功能列表需要覆盖主要的 Stellarium 功能

## [ ] Task 20: 实现工具调用的可视化反馈
- **Priority**: P1
- **Depends On**: Task 5, Task 16
- **Description**: 
  - 在对话界面显示工具调用状态
  - 显示正在调用什么工具
  - 显示工具执行结果
  - 提供工具调用的进度指示
- **Acceptance Criteria Addressed**: FR-14, AC-15
- **Test Requirements**:
  - `human-judgement` TR-20.1: 工具调用状态清晰可见
  - `human-judgement` TR-20.2: 反馈及时且友好
- **Notes**: 可以参考 ChatGPT 的插件执行反馈设计

## [ ] Task 21: 实现模糊找星功能
- **Priority**: P1
- **Depends On**: Task 16, Task 17
- **Description**: 
  - 设计并实现智能的天体搜索系统
  - 让 LLM 理解自然语言描述并转换为搜索条件
  - 整合多个工具实现搜索和定位的工作流
  - 提供匹配度较高的候选结果
- **Acceptance Criteria Addressed**: FR-10, AC-10
- **Test Requirements**:
  - `human-judgement` TR-21.1: 能理解颜色、亮度等描述
  - `human-judgement` TR-21.2: 能正确找到匹配的天体
  - `programmatic` TR-21.3: 定位功能正常工作
- **Notes**: 这需要很好的提示词设计

## [ ] Task 22: 实现多步任务协调
- **Priority**: P1
- **Depends On**: Task 16, Tasks 17-19
- **Description**: 
  - 实现工具链支持：让 LLM 能顺序调用多个工具
  - 处理工具之间的依赖关系
  - 支持复杂任务的完成（如"找行星→设置时间→放大"）
- **Acceptance Criteria Addressed**: FR-13, AC-16
- **Test Requirements**:
  - `human-judgement` TR-22.1: 能完成多步骤任务
  - `programmatic` TR-22.2: 工具调用顺序正确
- **Notes**: 这部分主要靠 LLM 的推理能力，我们提供好工具即可

## [ ] Task 23: 设计和优化系统提示词
- **Priority**: P1
- **Depends On**: Task 16
- **Description**: 
  - 编写专业的天文助手系统提示词
  - 优化工具使用的指令
  - 提供最佳实践的示例
  - 支持用户自定义提示词
- **Acceptance Criteria Addressed**: FR-2, FR-10, FR-13
- **Test Requirements**:
  - `human-judgement` TR-23.1: LLM 能正确理解角色
  - `human-judgement` TR-23.2: 能正确选择和使用工具
- **Notes**: 提示词设计对效果影响很大

## [ ] Task 24: 集成测试和修复问题
- **Priority**: P1
- **Depends On**: Tasks 1-23
- **Description**: 
  - 对所有功能进行完整测试
  - 修复发现的 bug
  - 优化性能问题
  - 进行跨平台测试（Windows、Linux、macOS）
- **Acceptance Criteria Addressed**: 所有 AC
- **Test Requirements**:
  - `programmatic` TR-24.1: 所有功能测试通过
  - `human-judgement` TR-24.2: 用户体验流畅
  - `programmatic` TR-24.3: 在各平台正常工作
- **Notes**: 这是最终的集成测试阶段
