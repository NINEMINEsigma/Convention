# Web 模块

Web模块提供了Web开发相关的功能，包括HTTP服务器、WebSocket服务器、URL处理等。

## 目录结构

- `Core.py`: 核心Web功能
- `Selunit.py`: Selenium单元测试功能
- `BeautifulSoup.py`: HTML解析功能
- `Requests.py`: HTTP请求功能

## 功能特性

### 1. HTTP服务器 (Core.py)

#### 1.1 请求处理

- 请求封装
  - 路径解析
  - 参数解析
  - 头部处理
  - 请求体处理
- 响应封装
  - 状态码设置
  - 头部设置
  - 响应体设置
  - 内容类型处理

#### 1.2 路由管理

- 路由注册
- 路由匹配
- 处理器获取
- 错误处理

#### 1.3 静态文件

- 文件服务
- 目录处理
- MIME类型
- 缓存控制

### 2. WebSocket服务器 (Core.py)

#### 2.1 连接管理

- 客户端注册
- 消息广播
- 连接关闭
- 错误处理

#### 2.2 消息处理

- 消息类型
- 消息处理器
- 消息广播
- 状态管理

### 3. URL处理 (Core.py)

#### 3.1 URL解析

- 协议解析
- 主机解析
- 路径解析
- 参数解析

#### 3.2 请求方法

- GET请求
- POST请求
- PUT请求
- DELETE请求
- 其他方法

#### 3.3 文件下载

- 普通下载
- 断点续传
- 并发下载
- 进度回调

#### 3.4 连接管理

- 会话管理
- 代理设置
- 超时设置
- 重试机制

### 4. Selenium支持 (Selunit.py)

#### 4.1 浏览器控制

- 浏览器启动
- 页面导航
- 元素查找
- 操作执行

#### 4.2 测试功能

- 测试用例
- 断言验证
- 截图保存
- 日志记录

### 5. HTML解析 (BeautifulSoup.py)

#### 5.1 解析功能

- HTML解析
- 元素查找
- 属性获取
- 文本提取

#### 5.2 数据处理

- 数据清洗
- 格式转换
- 编码处理
- 错误处理

## 使用示例

### 1. HTTP服务器示例

```python
from Convention.Web import Core

# 创建响应
def handle_request(request: Core.WebRequest) -> Core.WebResponse:
    return Core.create_json_response({"message": "Hello World"})

# 注册路由
Core.Router.add_route('GET', '/api/hello', handle_request)

# 启动服务器
Core.http_default_serve_forever(8080)
```

### 2. WebSocket服务器示例

```python
from Convention.Web import Core

# 创建服务器
server = Core.WebSocketServer('localhost', 8765)

# 添加消息处理器
def handle_message(websocket):
    async for message in websocket:
        print(f"收到消息: {message}")

server.add_handler('message', handle_message)
server.start()
```

### 3. URL处理示例

```python
from Convention.Web import Core

# 创建URL对象
url = Core.tool_url("https://example.com/api/data")

# GET请求
response = url.get()
print(response.body)

# 下载文件
url.download("output.txt")

# WebSocket连接
async def handle_message(websocket):
    async for message in websocket:
        print(f"收到消息: {message}")
url.connect_websocket(handle_message)
```

### 4. Selenium测试示例

```python
from Convention.Web import Selunit

# 创建测试用例
def test_example():
    driver = Selunit.get_driver()
    driver.get("https://example.com")
    assert "Example" in driver.title
    driver.quit()

# 运行测试
test_example()
```

### 5. HTML解析示例

```python
from Convention.Web import BeautifulSoup

# 解析HTML
html = "<html><body><h1>Hello</h1></body></html>"
soup = BeautifulSoup.parse(html)

# 查找元素
title = soup.find("h1")
print(title.text)
```

## 依赖项

- http.server: HTTP服务器
- websockets: WebSocket支持
- selenium: 浏览器自动化
- beautifulsoup4: HTML解析
- requests: HTTP请求

## 注意事项

1. 使用HTTP服务器时注意端口占用
2. WebSocket连接时注意心跳检测
3. URL处理时注意编码问题
4. Selenium使用时注意浏览器驱动
5. HTML解析时注意编码处理

## 性能优化

1. 使用连接池管理HTTP连接
2. 合理设置WebSocket心跳间隔
3. 优化URL请求重试策略
4. 使用异步操作提高效率
5. 合理管理浏览器实例

## 贡献指南

欢迎提交Issue和Pull Request来改进功能或添加新特性。
