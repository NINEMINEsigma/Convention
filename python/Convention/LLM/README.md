# LLM 模块

LLM模块提供了大语言模型相关的功能，包括模型加载、对话生成、模型转换等。

## 目录结构

- `llama/`: Llama.cpp相关功能
  - `Core.py`: Llama.cpp核心功能
- `LlamaIndex/`: LlamaIndex相关功能
  - `Core.py`: LlamaIndex核心功能
  - `Extensions.py`: 扩展功能
  - `WorkflowStep.py`: 工作流步骤定义
- `LangChain/`: LangChain相关功能
  - `AbsInterface.py`: 抽象接口
  - `llama.py`: Llama集成
- 模型转换工具
  - `convert_hf_to_gguf.py`: HuggingFace模型转GGUF
  - `convert_llama_ggml_to_gguf.py`: GGML模型转GGUF
  - `convert_lora_to_gguf.py`: LoRA模型转GGUF

## 功能特性

### 1. Llama.cpp集成 (llama/)

#### 1.1 模型加载

- 支持GGUF格式模型
- 支持本地模型文件
- 支持模型参数配置

#### 1.2 对话生成

- 文本补全
- 对话生成
- 流式输出
- 参数控制（温度、top_p等）

#### 1.3 函数调用

- 函数注册
- 函数调用
- 函数管理

### 2. LlamaIndex集成 (LlamaIndex/)

#### 2.1 文档处理

- 文档加载
- 文档索引
- 文档查询

#### 2.2 多层工具

- LLMObject与CustomLLM
- ReActAgentCore与FunctionCall
- 内置提示词模板
- 内置RAG模板引擎

#### 2.2 工作流管理

- 工作流定义
- 步骤执行
- 结果处理

#### 2.3 扩展功能

- 自定义索引器
- 自定义查询器
- 自定义处理器

### 3. LangChain集成 (LangChain/)

#### 3.1 链式调用

- 链式操作
- 中间件支持
- 结果处理

#### 3.2 Llama集成

- Llama模型集成
- 对话管理
- 结果处理

### 4. 模型转换工具

#### 4.1 格式转换

- HuggingFace转GGUF
- GGML转GGUF
- LoRA转GGUF

#### 4.2 参数配置

- 量化配置
- 模型参数
- 转换选项

## 使用示例

### 1. Llama.cpp使用示例

```python
from Convention.LLM.llama import light_llama

# 加载模型
model = light_llama("path/to/model.gguf")

# 生成文本
result = model("Hello, how are you?")
print(result)

# 创建对话
character = light_character(model, "You are a helpful assistant.")
response = character.create_chat_completion({
    "role": "user",
    "content": "What is the meaning of life?"
})
print(response)
```

### 2. LlamaIndex使用示例

```python
from Convention.LLM.LlamaIndex import Core

# 创建索引
index = Core.create_index("path/to/documents")

# 查询文档
results = index.query("What is the meaning of life?")
print(results)
```

### 3. LangChain使用示例

```python
from Convention.LLM.LangChain import llama

# 创建链
chain = llama.create_chain()

# 执行链
result = chain.run("What is the meaning of life?")
print(result)
```

### 4. 模型转换示例

```python
# 转换HuggingFace模型
from Convention.LLM import convert_hf_to_gguf

convert_hf_to_gguf.convert(
    model_path="path/to/hf_model",
    output_path="path/to/output.gguf",
    quantize=True
)
```

## 依赖项

- llama-cpp: Llama.cpp Python绑定
- llama-index: LlamaIndex库
- langchain: LangChain库
- torch: PyTorch
- transformers: HuggingFace Transformers
- gguf: GGUF格式支持

## 注意事项

1. 使用模型时注意显存占用
2. 转换模型时注意参数配置
3. 对话生成时注意上下文管理
4. 文档处理时注意内存使用

## 性能优化

1. 使用量化模型减少显存占用
2. 合理设置批处理大小
3. 使用流式输出减少等待时间
4. 优化文档索引结构

## 贡献指南

欢迎提交Issue和Pull Request来改进功能或添加新特性。
