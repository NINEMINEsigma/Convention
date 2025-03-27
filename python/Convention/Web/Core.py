from http.server import HTTPServer, BaseHTTPRequestHandler
import http.server
import socketserver
import os
import json
import urllib.parse
from typing import Dict, Callable, Any, Optional
import websockets
import asyncio
import threading
import mimetypes

#from ..Str.Core import *

# 全局变量
local_message = "<init>"
# 路由表，用于存储不同HTTP方法对应的路由处理器
routes: Dict[str, Dict[str, Callable]] = {
    'GET': {},
    'POST': {},
    'PUT': {},
    'DELETE': {}
}

class WebResponse(any_class):
    """
    Web响应封装类，用于统一处理HTTP响应

    使用示例:
    ```python
    # 创建JSON响应
    response = WebResponse(status_code=200, content_type='application/json')
    response.set_body({"message": "success"})

    # 创建HTML响应
    response = WebResponse(status_code=200, content_type='text/html')
    response.set_body("<html><body>Hello World</body></html>")
    ```
    """
    def __init__(self, status_code: int = 200, content_type: str = 'text/html'):
        self.status_code = status_code
        self.content_type = content_type
        self.headers: Dict[str, str] = {}
        self.body: Any = None

    def set_header(self, key: str, value: str):
        """设置响应头"""
        self.headers[key] = value

    def set_body(self, body: Any):
        """设置响应体"""
        self.body = body

    def to_bytes(self) -> bytes:
        """将响应体转换为字节"""
        if isinstance(self.body, str):
            return self.body.encode('utf-8')
        elif isinstance(self.body, (dict, list)):
            return json.dumps(self.body).encode('utf-8')
        return bytes(self.body) if self.body else b''

class WebRequest(any_class):
    """
    Web请求封装类，用于统一处理HTTP请求

    使用示例:
    ```python
    def handle_request(request: WebRequest) -> WebResponse:
        # 获取查询参数
        user_id = request.query_params.get('id')
        # 获取请求头
        content_type = request.headers.get('Content-Type')
        # 获取请求路径
        path = request.path
        return create_json_response({"status": "success"})
    ```
    """
    def __init__(self, handler: BaseHTTPRequestHandler):
        self.path = handler.path
        self.method = handler.command
        self.headers = dict(handler.headers)
        self.query_params = {}
        self.body = None

        # 解析查询参数
        if '?' in self.path:
            self.path, query = self.path.split('?', 1)
            self.query_params = dict(urllib.parse.parse_qsl(query))

class Router(any_class):
    """
    路由管理类，用于注册和获取路由处理器

    使用示例:
    ```python
    # 注册路由
    def handle_users(request: WebRequest) -> WebResponse:
        return create_json_response({"users": ["张三", "李四"]})

    Router.add_route('GET', '/api/users', handle_users)

    # 获取路由处理器
    handler = Router.get_handler('GET', '/api/users')
    ```
    """
    @staticmethod
    def add_route(method: str, path: str, handler: Callable):
        """注册路由处理器"""
        routes[method][path] = handler

    @staticmethod
    def get_handler(method: str, path: str) -> Optional[Callable]:
        """获取路由处理器"""
        return routes[method].get(path)

class __SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    """
    HTTP请求处理器，处理所有HTTP请求

    使用示例:
    ```python
    # 创建服务器
    server = HTTPServer(('localhost', 8080), __SimpleHTTPRequestHandler)
    server.serve_forever()
    ```
    """
    def __init__(self, request, client_address, server):
        super().__init__(request, client_address, server)

    def _do_success_send(self, keyword='Content-type', value='text/html'):
        """发送成功响应头"""
        self.send_response(200)
        self.send_header(keyword=keyword, value=value)
        self.end_headers()

    def _headers_length(self):
        """获取请求体长度"""
        return int(self.headers.get('Content-Length', 0))

    def do_log(self, tag="message"):
        """记录日志"""
        print(f"[{tag}]: {local_message}")

    def _handle_request(self):
        """处理请求"""
        request = WebRequest(self)
        handler = Router.get_handler(request.method, request.path)

        if handler:
            try:
                response = handler(request)
                self.send_response(response.status_code)
                for key, value in response.headers.items():
                    self.send_header(key, value)
                self.end_headers()
                self.wfile.write(response.to_bytes())
            except Exception as e:
                self.send_error(500, str(e))
        else:
            self.send_error(404, "Not Found")

    def do_GET(self):
        """处理GET请求"""
        self._handle_request()

    def do_POST(self):
        """处理POST请求"""
        content_length = self._headers_length()
        if content_length > 0:
            self.rfile.read(content_length)
        self._handle_request()

    def do_PUT(self):
        """处理PUT请求"""
        self._handle_request()

    def do_DELETE(self):
        """处理DELETE请求"""
        self._handle_request()

class StaticFileHandler(any_class):
    """
    静态文件处理器，用于处理静态文件请求

    使用示例:
    ```python
    # 创建静态文件处理器
    handler = StaticFileHandler(root_dir='./static')

    # 处理文件请求
    response = handler.handle('/images/logo.png')
    ```
    """
    def __init__(self, root_dir: str):
        self.root_dir = root_dir

    def handle(self, path: str) -> WebResponse:
        """处理静态文件请求"""
        full_path = os.path.join(self.root_dir, path.lstrip('/'))
        if os.path.exists(full_path):
            content_type, _ = mimetypes.guess_type(full_path)
            with open(full_path, 'rb') as f:
                response = WebResponse(content_type=content_type or 'application/octet-stream')
                response.set_body(f.read())
                return response
        return WebResponse(status_code=404)

class WebSocketServer(any_class):
    """
    WebSocket服务器，用于处理WebSocket连接

    使用示例:
    ```python
    # 创建WebSocket服务器
    ws_server = WebSocketServer(host='localhost', port=8765)

    # 添加消息处理器
    async def handle_chat(websocket):
        await websocket.send("收到消息")

    ws_server.add_handler('chat', handle_chat)

    # 启动服务器
    ws_server.start()
    ```
    """
    def __init__(self, host: str = 'localhost', port: int = 8765):
        self.host = host
        self.port = port
        self.clients = set()
        self.handlers = {}

    async def register(self, websocket):
        """注册新的WebSocket连接"""
        self.clients.add(websocket)
        try:
            await self.handle_client(websocket)
        finally:
            self.clients.remove(websocket)

    async def handle_client(self, websocket):
        """处理客户端消息"""
        async for message in websocket:
            if message in self.handlers:
                await self.handlers[message](websocket)

    def add_handler(self, message_type: str, handler: Callable):
        """添加消息处理器"""
        self.handlers[message_type] = handler

    def start(self):
        """启动WebSocket服务器"""
        async def main():
            async with websockets.serve(self.register, self.host, self.port):
                await asyncio.Future()  # 运行直到被取消

        def run_server():
            asyncio.run(main())

        thread = threading.Thread(target=run_server)
        thread.daemon = True
        thread.start()

# 工具函数
def serve_static_files(port: int, root_dir: str):
    """
    启动静态文件服务器

    使用示例:
    ```python
    # 启动静态文件服务器
    serve_static_files(port=8080, root_dir='./static')
    ```
    """
    class StaticHandler(http.server.SimpleHTTPRequestHandler):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, directory=root_dir, **kwargs)

    with socketserver.TCPServer(("", port), StaticHandler) as httpd:
        print(f"Serving static files from {root_dir} at port {port}")
        httpd.serve_forever()

def create_json_response(data: Any, status_code: int = 200) -> WebResponse:
    """
    创建JSON响应

    使用示例:
    ```python
    response = create_json_response({"status": "success"})
    ```
    """
    response = WebResponse(status_code=status_code, content_type='application/json')
    response.set_body(data)
    return response

def create_text_response(text: str, status_code: int = 200) -> WebResponse:
    """
    创建文本响应

    使用示例:
    ```python
    response = create_text_response("Hello World")
    ```
    """
    response = WebResponse(status_code=status_code, content_type='text/plain')
    response.set_body(text)
    return response

def create_html_response(html: str, status_code: int = 200) -> WebResponse:
    """
    创建HTML响应

    使用示例:
    ```python
    response = create_html_response("<html><body>Hello World</body></html>")
    ```
    """
    response = WebResponse(status_code=status_code, content_type='text/html')
    response.set_body(html)
    return response

def http_default_serve_forever(PORT):
    """
    启动默认HTTP服务器

    使用示例:
    ```python
    # 启动服务器在8080端口
    http_default_serve_forever(8080)
    ```
    """
    class CustomHandler(http.server.SimpleHTTPRequestHandler):
        def do_GET(self):
            # 检查请求的文件是否存在
            if os.path.exists(self.translate_path(self.path)):
                super().do_GET()
            else:
                # 如果文件不存在，返回index.html
                self.path = '/index.html'
                super().do_GET()

    with socketserver.TCPServer(("", PORT), CustomHandler) as httpd:
        print(f"Serving at port {PORT}")
        httpd.serve_forever()



#class light_handler(BaseHTTPRequestHandler):
#    def __init__(self, request, client_address, server, callback):
#        super().__init__(request, client_address, server)
#        self.callback = callback
#
#    def _do_send(self, stats:int, keyword:str, value:str):
#        self.send_response(stats)
#        self.send_header(keyword=keyword, value=value)
#        self.end_headers()
#
#    def _do_success_send(self, keyword = 'Content-type', value = 'text/html'):
#        self._do_send(200, keyword=keyword, value=value)
#    def _do_failed_send(self, keyword = 'Content-type', value = 'text/html'):
#        self._do_send(500, keyword=keyword, value=value)
#
#    def _headers_length(self):
#        return int(self.headers['Content-Length'])
#
#    def do_log(self, message, tag="message"):
#        print(f"[{tag}]: {message}")
#
#    def do_GET(self):
#        try:
#            #first callback next get
#            result_callback = self.callback(self, 'get')
#            self._do_success_send()
#            self.wfile.write(result_callback)
#            self.do_log(limit_str(result_callback))
#        except Exception as ex:
#            self._do_failed_send()
#            self.do_log(ex, "error")
#        finally:
#            self.temp_result = None
#
#    def do_POST(self):
#        result = self.rfile.read(self._headers_length())
#        self.temp_result = result
#        try:
#            #first callback next post
#            result_callback = self.callback(self, 'post')
#            self._do_success_send()
#            self.wfile.write(result_callback)
#            self.do_log(limit_str(result_callback))
#        except Exception as ex:
#            self._do_failed_send()
#            self.do_log(ex, "error")
#            self.do_log(limit_str(self.temp_result),"when-error result")
#        finally:
#            self.temp_result = None
#
#    def do_PUT(self):
#        content_length = int(self.headers['Content-Length'])
#        result = self.rfile.read(content_length)
#        self.temp_result = result
#        try:
#            #first callback next post
#            result_callback = self.callback(self, 'put')
#            self._do_success_send()
#            self.wfile.write(result_callback)
#            self.do_log(limit_str(result_callback))
#        except Exception as ex:
#            self._do_failed_send()
#            self.do_log(ex, "error")
#            self.do_log(limit_str(self.temp_result),"when-error result")
#        finally:
#            self.temp_result = None
#
#    def do_DELETE(self):
#        try:
#            #first callback next post
#            result_callback = self.callback(self, 'delete')
#            self._do_success_send()
#            self.wfile.write(result_callback)
#            self.do_log(limit_str(result_callback))
#        except Exception as ex:
#            self._do_failed_send()
#            self.do_log(ex, "error")
#
#
#class light_server:
#    def __init__(self, server_address=('', 8080), requestHandler=SimpleHTTPRequestHandler):
#        self.server_address = server_address
#        self.requestHandler = requestHandler
#
#    def start(self):
#        self.httpd = HTTPServer(self.server_address, self.requestHandler)
#        print(f'Starting httpd server on port {self.server_address[1]}')
#        self.httpd.serve_forever()
#
#if __name__ == '__main__':
#    server_address = ('', 8080)
#    httpd = HTTPServer(server_address, SimpleHTTPRequestHandler)
#    print(f'Starting httpd server on port {8080}')
#    httpd.serve_forever()
