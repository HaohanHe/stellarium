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

## [ ] Task 15: 集成测试和修复问题
- **Priority**: P1
- **Depends On**: Tasks 1-14
- **Description**: 
  - 对所有功能进行完整测试
  - 修复发现的 bug
  - 优化性能问题
  - 进行跨平台测试（Windows、Linux、macOS）
- **Acceptance Criteria Addressed**: 所有 AC
- **Test Requirements**:
  - `programmatic` TR-15.1: 所有功能测试通过
  - `human-judgement` TR-15.2: 用户体验流畅
  - `programmatic` TR-15.3: 在各平台正常工作
- **Notes**: 这是最终的集成测试阶段
