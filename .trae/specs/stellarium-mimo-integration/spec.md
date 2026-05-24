# Stellarium 小米 MiMo 多模态 LLM 集成 - 产品需求文档

## Overview
- **Summary**: 为 Stellarium 天文馆软件接入小米 MiMo 多模态大模型，实现智能天文问答、星空场景分析、图片理解等功能，增强用户的天文学习和探索体验。
- **Purpose**: 将先进的多模态 AI 能力与 Stellarium 结合，为用户提供更智能、更丰富的天文探索体验，包括：智能天文知识问答、星空截图分析、语音交互、TTS 朗读等功能。
- **Target Users**: 
  - 天文爱好者：想要更深入了解天文现象和知识的用户
  - 学生和教育者：用于天文教育和学习
  - 普通用户：对星空和宇宙好奇的大众用户

## Goals
- [ ] 为 Stellarium 添加新的 AI 助手插件，支持与小米 MiMo 模型对话
- [ ] 实现文本对话功能，解答用户的天文相关问题
- [ ] 实现图片理解功能，分析 Stellarium 星空截图或用户上传的天文图片
- [ ] 实现语音合成 (TTS) 功能，朗读天文内容
- [ ] 实现多轮对话功能，保持上下文理解
- [ ] 提供配置界面，让用户设置 API Key 和偏好选项
- [ ] 与 Stellarium 现有功能深度集成，如选中天体后自动获取相关信息

## Non-Goals (Out of Scope)
- 不实现完整的 Agent 系统或工具调用（第一阶段）
- 不实现 Stellarium 功能的完全自动化（如自动寻找天体）
- 不实现语音识别 (ASR) 功能（第一阶段）
- 不实现视频理解功能（第一阶段）
- 不修改 Stellarium 的核心渲染引擎

## Background & Context
- Stellarium 是一个开源的天文馆软件，拥有丰富的天体数据和星空渲染能力
- 小米 MiMo 平台提供多模态大模型能力，包括：文本对话、图片理解、语音合成、视频理解等
- MiMo 平台支持 OpenAI 兼容协议和 Anthropic 兼容协议，部署在多个集群（中国、新加坡、欧洲）
- Stellarium 已有插件架构，包括 OnlineQueries 等现有插件，便于新功能集成

## Functional Requirements
- **FR-1**: 创建新的 "MiMo AI 助手" 插件，作为与 LLM 交互的核心模块
- **FR-2**: 提供配置界面，支持设置 API Key、选择集群、选择模型、设置系统提示词
- **FR-3**: 实现文本对话界面，支持用户与 LLM 进行天文相关的多轮对话
- **FR-4**: 支持将 Stellarium 中的天体信息（名称、坐标、类型等）发送给 LLM 作为上下文
- **FR-5**: 实现图片理解功能，支持：
  - 分析 Stellarium 当前星空截图
  - 分析用户上传的天文图片
- **FR-6**: 实现 TTS 功能，支持朗读 LLM 的回答和天文信息
- **FR-7**: 对话历史记录功能，支持保存和恢复对话
- **FR-8**: 提供快捷问题按钮（预设的常见天文问题）
- **FR-9**: 与 Stellarium UI 深度集成，如在选中天体时显示"询问 AI"按钮

## Non-Functional Requirements
- **NFR-1**: 响应速度：对话请求在网络正常情况下应在 3 秒内返回
- **NFR-2**: 稳定性：插件不应导致 Stellarium 崩溃
- **NFR-3**: 可配置性：所有 API 相关参数应可配置
- **NFR-4**: 安全性：API Key 应安全存储，不应泄露
- **NFR-5**: 兼容性：支持 Qt5 和 Qt6，支持 Windows、Linux、macOS

## Constraints
- **Technical**: 
  - 必须使用 C++ 和 Qt 框架开发，符合 Stellarium 现有技术栈
  - 必须遵循 Stellarium 的插件开发规范
  - API 调用使用 OpenAI 兼容协议
- **Business**: 
  - 需要用户自行获取小米 MiMo API Key
  - 需遵守小米 MiMo 平台的服务条款
- **Dependencies**: 
  - Qt Network 模块
  - Qt WebEngine（可选，用于显示富文本内容）

## Assumptions
- 用户具有有效的小米 MiMo API Key
- 用户的设备可以访问小米 MiMo 平台
- 用户对天文知识有基本了解，但需要更智能的辅助
- 小米 MiMo API 保持稳定的兼容性

## Acceptance Criteria

### AC-1: 插件安装和启用
- **Given**: 用户已安装 Stellarium
- **When**: 用户在插件管理器中启用 "MiMo AI 助手" 插件
- **Then**: 插件成功加载，工具栏显示 AI 助手按钮
- **Verification**: `programmatic`

### AC-2: 配置界面
- **Given**: 插件已启用
- **When**: 用户点击配置按钮
- **Then**: 显示配置界面，包含 API Key 输入框、集群选择、模型选择、系统提示词设置
- **Verification**: `human-judgment`

### AC-3: 文本对话功能
- **Given**: 用户已配置好 API Key
- **When**: 用户在对话界面输入天文问题并发送
- **Then**: LLM 返回回答并显示在对话界面中
- **Verification**: `programmatic`

### AC-4: 多轮对话
- **Given**: 用户正在与 LLM 对话
- **When**: 用户继续追问与之前对话相关的问题
- **Then**: LLM 理解上下文并给出相关回答
- **Verification**: `human-judgment`

### AC-5: 天体信息集成
- **Given**: 用户在 Stellarium 中选中了一个天体
- **When**: 用户点击"询问 AI"按钮
- **Then**: 自动发送该天体的信息给 LLM 并请求详细介绍
- **Verification**: `programmatic`

### AC-6: 图片理解功能
- **Given**: 用户已配置好 API Key
- **When**: 用户选择发送当前星空截图或上传图片
- **Then**: LLM 分析图片并给出相关的天文解释
- **Verification**: `human-judgment`

### AC-7: TTS 朗读功能
- **Given**: LLM 已返回回答
- **When**: 用户点击朗读按钮
- **Then**: 使用 MiMo TTS 朗读回答内容
- **Verification**: `human-judgment`

### AC-8: 对话历史
- **Given**: 用户有历史对话记录
- **When**: 用户重新打开对话界面
- **Then**: 可以查看和恢复之前的对话
- **Verification**: `programmatic`

### AC-9: 快捷问题
- **Given**: 对话界面已打开
- **When**: 用户点击快捷问题按钮
- **Then**: 自动发送预设问题给 LLM
- **Verification**: `programmatic`

## Open Questions
- [ ] 第一阶段是否需要实现语音识别 (ASR)？
- [ ] 是否需要支持除小米 MiMo 以外的其他 LLM 服务？
- [ ] 对话历史记录的存储位置和格式如何确定？
- [ ] 图片理解功能是否需要支持截图标记功能（用户在图上标注感兴趣的区域）？
