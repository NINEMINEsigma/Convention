# Workflow 模块

Workflow模块提供了一个灵活的工作流管理系统，用于构建和执行复杂的数据处理流程。

## 目录结构

- `Core.py`: 核心工作流功能

## 功能特性

### 1. 工作流管理

#### 1.1 节点系统

- 节点类型
  - 开始节点 (StartNode)
  - 步骤节点 (StepNode)
  - 结束节点 (EndNode)
  - 资源节点 (ResourceNode)
  - 文本节点 (TextNode)
  - 动态节点 (DynamicNode)
- 节点操作
  - 创建节点
  - 销毁节点
  - 节点连接
  - 节点验证

#### 1.2 插槽系统

- 插槽类型
  - 输入插槽
  - 输出插槽
- 插槽操作
  - 连接插槽
  - 断开连接
  - 参数传递
  - 类型检查

#### 1.3 事件系统

- 事件类型
  - 开始事件 (StartEvent)
  - 停止事件 (StopEvent)
  - 错误事件 (ErrorEvent)
- 事件处理
  - 事件广播
  - 事件响应
  - 状态管理

### 2. 工作流执行

#### 2.1 执行控制

- 启动工作流
- 停止工作流
- 超时控制
- 错误处理

#### 2.2 状态管理

- 节点状态
- 执行时间
- 错误信息
- 完成状态

#### 2.3 结果处理

- 上下文结果
- 节点结果
- 结果保存
- 结果加载

## 使用示例

### 1. 创建工作流

```python
from Convention.Workflow import Core

# 创建节点信息
start_info = Core.StartNodeInfo(
    title="开始节点",
    outmapping={"output": Core.NodeSlotInfo(
        slotName="output",
        typeIndicator="string",
        IsInmappingSlot=False
    )}
)

step_info = Core.StepNodeInfo(
    title="处理节点",
    funcname="process_data",
    inmapping={"input": Core.NodeSlotInfo(
        slotName="input",
        typeIndicator="string",
        IsInmappingSlot=True
    )},
    outmapping={"result": Core.NodeSlotInfo(
        slotName="result",
        typeIndicator="string",
        IsInmappingSlot=False
    )}
)

end_info = Core.EndNodeInfo(
    title="结束节点",
    inmapping={"input": Core.NodeSlotInfo(
        slotName="input",
        typeIndicator="string",
        IsInmappingSlot=True
    )}
)

# 创建工作流
workflow = Core.Workflow.Create(start_info, step_info, end_info)

# 创建管理器
manager = Core.WorkflowManager(workflow)
```

### 2. 执行工作流

```python
# 启动工作流
await manager.RunWorkflow(verbose=True)

# 获取结果
result = manager.GetLastContext()
print(result)
```

### 3. 保存和加载工作流

```python
# 保存工作流
manager.SaveWorkflow("workflow.json")

# 加载工作流
loaded_workflow = manager.LoadWorkflow("workflow.json")
```

## 依赖项

- pydantic: 数据验证
- asyncio: 异步支持
- pathlib: 路径处理

## 注意事项

1. 工作流执行时注意超时设置
2. 节点连接时注意类型匹配
3. 资源加载时注意路径处理
4. 错误处理时注意状态保存

## 性能优化

1. 合理设置超时时间
2. 优化节点连接结构
3. 使用异步操作提高效率
4. 合理管理资源加载

## 贡献指南

欢迎提交Issue和Pull Request来改进功能或添加新特性。
