from ..Internal import *
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
import datetime

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

def build_web_header(url:str) -> Dict[str, str]:
    return {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36",
        "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
        "Accept-Language": "zh-CN,zh;q=0.9,en;q=0.8",
        "Referer": url,
        "Cache-Control": "no-cache",
        "Pragma": "no-cache",
    }

class tool_url(any_class):
    '''
    工具类，用于处理URL相关操作

    使用示例:
    ```python
    # 创建URL对象
    url = tool_url("https://example.com/api/data")

    # GET请求
    response = url.get()

    # POST请求
    response = url.post(json={"name": "test"})

    # 下载文件
    url.download("output.txt")

    # 检查URL是否有效
    if url.is_valid():
        print("URL is valid")

    # WebSocket连接
    async def handle_message(websocket):
        async for message in websocket:
            print(f"收到消息: {message}")
    url.connect_websocket(handle_message)

    ```
    '''
    __url: str = None
    __parsed_url = None
    __scheme: str = None
    __netloc: str = None
    __path: str = None
    __params: str = None
    __query: str = None
    __fragment: str = None
    __username: str = None
    __password: str = None
    __hostname: str = None
    __port: int = None
    __proxy: Dict[str, str] = None
    __verify_ssl: bool = True
    __max_retries: int = 3
    __retry_delay: float = 1.0
    __session = None

    def __init__(self, url: str):
        self.__url = url
        self.__parse_url()

    def __parse_url(self):
        """解析URL的各个组件"""
        self.__parsed_url = urllib.parse.urlparse(self.__url)
        self.__scheme = self.__parsed_url.scheme
        self.__netloc = self.__parsed_url.netloc
        self.__path = self.__parsed_url.path
        self.__params = self.__parsed_url.params
        self.__query = self.__parsed_url.query
        self.__fragment = self.__parsed_url.fragment

        # 解析用户名和密码
        if '@' in self.__netloc:
            auth, host = self.__netloc.split('@')
            if ':' in auth:
                self.__username, self.__password = auth.split(':')
            else:
                self.__username = auth
                self.__password = None
            self.__netloc = host

        # 解析主机名和端口
        if ':' in self.__netloc:
            self.__hostname, port = self.__netloc.split(':')
            self.__port = int(port)
        else:
            self.__hostname = self.__netloc
            self.__port = None

    def get_url(self) -> str:
        """获取完整URL"""
        return self.__url

    def get_scheme(self) -> str:
        """获取URL协议"""
        return self.__scheme

    def get_netloc(self) -> str:
        """获取网络位置"""
        return self.__netloc

    def get_path(self) -> str:
        """获取路径"""
        return self.__path

    def get_params(self) -> str:
        """获取参数"""
        return self.__params

    def get_query(self) -> str:
        """获取查询字符串"""
        return self.__query

    def get_fragment(self) -> str:
        """获取片段"""
        return self.__fragment

    def get_username(self) -> str:
        """获取用户名"""
        return self.__username

    def get_password(self) -> str:
        """获取密码"""
        return self.__password

    def get_hostname(self) -> str:
        """获取主机名"""
        return self.__hostname

    def get_port(self) -> int:
        """获取端口号"""
        return self.__port

    def is_valid(self) -> bool:
        """检查URL是否有效"""
        try:
            result = urllib.parse.urlparse(self.__url)
            return all([result.scheme, result.netloc])
        except:
            return False

    def is_secure(self) -> bool:
        """检查是否是安全URL(HTTPS)"""
        return self.__scheme == 'https'

    def get_query_params(self) -> Dict[str, str]:
        """获取查询参数字典"""
        return dict(urllib.parse.parse_qsl(self.__query))

    def set_query_params(self, params: Dict[str, str]) -> 'tool_url':
        """设置查询参数"""
        query = urllib.parse.urlencode(params)
        self.__query = query
        self.__update_url()
        return self

    def add_query_param(self, key: str, value: str) -> 'tool_url':
        """添加单个查询参数"""
        params = self.get_query_params()
        params[key] = value
        return self.set_query_params(params)

    def remove_query_param(self, key: str) -> 'tool_url':
        """移除查询参数"""
        params = self.get_query_params()
        if key in params:
            del params[key]
        return self.set_query_params(params)

    def __update_url(self):
        """更新完整URL"""
        self.__url = urllib.parse.urlunparse((
            self.__scheme,
            self.__netloc,
            self.__path,
            self.__params,
            self.__query,
            self.__fragment
        ))

    def __retry_request(self, method: str, *args, **kwargs) -> WebResponse:
        """带重试机制的请求"""
        import time
        import requests.exceptions

        for attempt in range(self.__max_retries):
            try:
                response = getattr(self.__session, method.lower())(*args, **kwargs)
                return WebResponse(
                    status_code=response.status_code,
                    content_type=response.headers.get('content-type', 'text/plain')
                ).set_body(response.text)
            except (requests.exceptions.RequestException, requests.exceptions.Timeout) as e:
                if attempt == self.__max_retries - 1:
                    return WebResponse(status_code=500).set_body(str(e))
                time.sleep(self.__retry_delay)
        return WebResponse(status_code=500).set_body("Max retries exceeded")

    def get(self, params: Dict[str, str] = None, headers: Dict[str, str] = None, timeout: int = 30) -> WebResponse:
        """发送GET请求"""
        return self.__retry_request('get', self.__url, params=params, headers=headers, timeout=timeout)

    def post(self, data: Dict[str, Any] = None, json: Dict[str, Any] = None,
             headers: Dict[str, str] = None, timeout: int = 30) -> WebResponse:
        """发送POST请求"""
        return self.__retry_request('post', self.__url, data=data, json=json, headers=headers, timeout=timeout)

    def put(self, data: Dict[str, Any] = None, json: Dict[str, Any] = None,
            headers: Dict[str, str] = None, timeout: int = 30) -> WebResponse:
        """发送PUT请求"""
        return self.__retry_request('put', self.__url, data=data, json=json, headers=headers, timeout=timeout)

    def delete(self, headers: Dict[str, str] = None, timeout: int = 30) -> WebResponse:
        """发送DELETE请求"""
        return self.__retry_request('delete', self.__url, headers=headers, timeout=timeout)

    def head(self, headers: Dict[str, str] = None, timeout: int = 30) -> WebResponse:
        """发送HEAD请求"""
        return self.__retry_request('head', self.__url, headers=headers, timeout=timeout)

    def options(self, headers: Dict[str, str] = None, timeout: int = 30) -> WebResponse:
        """发送OPTIONS请求"""
        return self.__retry_request('options', self.__url, headers=headers, timeout=timeout)

    def patch(self, data: Dict[str, Any] = None, json: Dict[str, Any] = None,
              headers: Dict[str, str] = None, timeout: int = 30) -> WebResponse:
        """发送PATCH请求"""
        return self.__retry_request('patch', self.__url, data=data, json=json, headers=headers, timeout=timeout)

    def connect_websocket(self, message_handler: Callable[[websockets.WebSocketClientProtocol], None]) -> None:
        """建立WebSocket连接"""
        import asyncio
        import websockets

        async def connect():
            try:
                async with websockets.connect(self.__url) as websocket:
                    await message_handler(websocket)
            except Exception as e:
                print(f"WebSocket connection failed: {str(e)}")

        asyncio.run(connect())

    def download(self, save_path: str, chunk_size: int = 8192) -> bool:
        """下载URL指向的资源"""
        import requests
        try:
            response = requests.get(self.__url, stream=True)
            response.raise_for_status()

            with open(save_path, 'wb') as f:
                for chunk in response.iter_content(chunk_size=chunk_size):
                    if chunk:
                        f.write(chunk)
            return True
        except Exception as e:
            print(f"Download failed: {str(e)}")
            return False

    def download_with_progress(self, save_path: str, chunk_size: int = 8192,
                             progress_callback: Callable[[int, int], None] = None) -> bool:
        """带进度回调的下载"""
        import requests
        try:
            response = requests.get(self.__url, stream=True)
            response.raise_for_status()

            total_size = int(response.headers.get('content-length', 0))
            block_size = chunk_size
            downloaded = 0

            with open(save_path, 'wb') as f:
                for chunk in response.iter_content(chunk_size=block_size):
                    if chunk:
                        f.write(chunk)
                        downloaded += len(chunk)
                        if progress_callback:
                            progress_callback(downloaded, total_size)
            return True
        except Exception as e:
            print(f"Download failed: {str(e)}")
            return False

    async def download_async(
        self,
        save_path:          str,
        chunk_size:         int = 8192,
        progress_callback:  Callable[[int, int], None] = None
        ) -> bool:
        """异步下载URL指向的资源"""
        import aiohttp
        import asyncio
        import os

        try:
            async with aiohttp.ClientSession() as session:
                    async with session.get(self.__url) as response:
                        response.raise_for_status()

                        total_size = int(response.headers.get('content-length', 0))
                        block_size = chunk_size
                        downloaded = 0

                        with open(save_path, 'wb') as f:
                            async for chunk in response.content.iter_chunked(block_size):
                                if chunk:
                                    f.write(chunk)
                                    downloaded += len(chunk)
                                    if progress_callback:
                                        progress_callback(downloaded, total_size)
            return True
        except Exception as e:
            print(f"Async download failed: {str(e)}")
            return False

    def download_with_resume(self, save_path: str, chunk_size: int = 8192) -> bool:
        """支持断点续传的下载"""
        import requests
        import os
        try:
            # 获取已下载的文件大小
            first_byte = os.path.getsize(save_path) if os.path.exists(save_path) else 0

            # 设置断点续传的请求头
            headers = {"Range": f"bytes={first_byte}-"} if first_byte else None

            response = requests.get(self.__url, headers=headers, stream=True)
            response.raise_for_status()

            # 获取文件总大小
            total_size = int(response.headers.get('content-length', 0)) + first_byte

            # 追加模式打开文件
            mode = 'ab' if first_byte else 'wb'
            with open(save_path, mode) as f:
                for chunk in response.iter_content(chunk_size=chunk_size):
                    if chunk:
                        f.write(chunk)
            return True
        except Exception as e:
            print(f"Resume download failed: {str(e)}")
            return False

    def download_with_retry(self, save_path: str, max_retries: int = 3,
                          chunk_size: int = 8192) -> bool:
        """带重试机制的下载"""
        for attempt in range(max_retries):
            if self.download(save_path, chunk_size):
                return True
            if attempt < max_retries - 1:
                import time
                time.sleep(self.__retry_delay)
        return False

    def download_multiple(self, urls: List[str], save_dir: str,
                         max_concurrent: int = 5) -> List[bool]:
        """并发下载多个URL"""
        import concurrent.futures
        import os

        def download_single(url: str) -> bool:
            save_path = os.path.join(save_dir, os.path.basename(url))
            return self.download(save_path)

        with concurrent.futures.ThreadPoolExecutor(max_workers=max_concurrent) as executor:
            results = list(executor.map(download_single, urls))
        return results

    def check_connection(self, timeout: int = 5) -> bool:
        """检查URL是否可连接"""
        import requests
        try:
            requests.head(self.__url, timeout=timeout)
            return True
        except:
            return False

    def get_content_type(self) -> str:
        """获取URL指向资源的Content-Type"""
        import requests
        try:
            response = requests.head(self.__url)
            return response.headers.get('content-type', '')
        except:
            return ''

    def get_content_length(self) -> int:
        """获取URL指向资源的大小(字节)"""
        import requests
        try:
            response = requests.head(self.__url)
            return int(response.headers.get('content-length', 0))
        except:
            return 0

    def get_last_modified(self) -> datetime.datetime:
        """获取URL指向资源的最后修改时间"""
        import requests
        try:
            response = requests.head(self.__url)
            last_modified = response.headers.get('last-modified')
            if last_modified:
                return datetime.datetime.strptime(last_modified, '%a, %d %b %Y %H:%M:%S GMT')
            return None
        except:
            return None

    def get_etag(self) -> str:
        """获取URL指向资源的ETag"""
        import requests
        try:
            response = requests.head(self.__url)
            return response.headers.get('etag', '')
        except:
            return ''

    def get_headers(self) -> Dict[str, str]:
        """获取URL的所有响应头"""
        import requests
        try:
            response = requests.head(self.__url)
            return dict(response.headers)
        except:
            return {}

    @override
    def SymbolName(self):
        return f"ToolURL<{self.get_url()}>"

    @override
    def ToString(self):
        return self.get_url()

    def init_session(self):
        """初始化请求会话"""
        import requests
        self.__session = requests.Session()
        if self.__proxy:
            self.__session.proxies = self.__proxy
        self.__session.verify = self.__verify_ssl

    def set_proxy(self, proxy: Dict[str, str]) -> Self:
        """设置代理"""
        self.__proxy = proxy
        if self.__session:
            self.__session.proxies = proxy
        return self

    def set_max_retries(self, max_retries: int) -> 'tool_url':
        """设置最大重试次数"""
        self.__max_retries = max_retries
        return self

    def get_mime_type(self) -> str:
        """获取URL指向资源的MIME类型"""
        content_type = self.get_content_type()
        if ';' in content_type:
            content_type = content_type.split(';')[0]
        return content_type

    def is_downloadable(self) -> bool:
        """检查URL是否可下载"""
        mime_type = self.get_mime_type()
        return mime_type.startswith('application/') or mime_type.startswith('image/') or mime_type.startswith('video/') or mime_type.startswith('audio/')

    def get_file_size(self) -> int:
        """获取文件大小(字节)"""
        return self.get_content_length()

    def get_file_name(self) -> str:
        """从URL中获取文件名"""
        from urllib.parse import unquote
        path = self.get_path()
        if not path:
            return ''
        return unquote(path.split('/')[-1])

    def get_file_extension(self) -> str:
        """获取文件扩展名"""
        file_name = self.get_file_name()
        if '.' not in file_name:
            return ''
        return file_name.split('.')[-1].lower()

    def is_image(self) -> bool:
        """检查是否是图片URL"""
        return self.get_mime_type().startswith('image/')

    def is_video(self) -> bool:
        """检查是否是视频URL"""
        return self.get_mime_type().startswith('video/')

    def is_audio(self) -> bool:
        """检查是否是音频URL"""
        return self.get_mime_type().startswith('audio/')

    def is_pdf(self) -> bool:
        """检查是否是PDF URL"""
        return self.get_mime_type() == 'application/pdf'

    def is_json(self) -> bool:
        """检查是否是JSON URL"""
        return self.get_mime_type() == 'application/json'

    def is_xml(self) -> bool:
        """检查是否是XML URL"""
        return self.get_mime_type() == 'application/xml'

    def is_html(self) -> bool:
        """检查是否是HTML URL"""
        return self.get_mime_type() == 'text/html'

    def is_text(self) -> bool:
        """检查是否是文本URL"""
        return self.get_mime_type().startswith('text/')

    def is_binary(self) -> bool:
        """检查是否是二进制URL"""
        return not self.is_text() and not self.is_json() and not self.is_xml()

    def get_encoding(self) -> str:
        """获取URL指向资源的编码"""
        content_type = self.get_content_type()
        if 'charset=' in content_type:
            return content_type.split('charset=')[-1]
        return 'utf-8'

    def get_language(self) -> str:
        """获取URL指向资源的语言"""
        return self.get_headers().get('content-language', '')

    def get_cache_control(self) -> str:
        """获取缓存控制信息"""
        return self.get_headers().get('cache-control', '')

    def get_expires(self) -> datetime.datetime:
        """获取过期时间"""
        expires = self.get_headers().get('expires')
        if expires:
            try:
                return datetime.datetime.strptime(expires, '%a, %d %b %Y %H:%M:%S GMT')
            except:
                pass
        return None

    def is_cached(self) -> bool:
        """检查是否可缓存"""
        cache_control = self.get_cache_control()
        return 'no-cache' not in cache_control and 'no-store' not in cache_control

    def get_compression(self) -> str:
        """获取压缩方式"""
        return self.get_headers().get('content-encoding', '')

    def is_compressed(self) -> bool:
        """检查是否压缩"""
        return bool(self.get_compression())

    def get_security_headers(self) -> Dict[str, str]:
        """获取安全相关的响应头"""
        headers = self.get_headers()
        security_headers = {}
        for key in ['x-frame-options', 'x-content-type-options', 'x-xss-protection',
                   'strict-transport-security', 'content-security-policy']:
            if key in headers:
                security_headers[key] = headers[key]
        return security_headers

    def is_secure_headers(self) -> bool:
        """检查安全响应头是否完整"""
        security_headers = self.get_security_headers()
        required_headers = ['x-frame-options', 'x-content-type-options', 'x-xss-protection']
        return all(header in security_headers for header in required_headers)

    def get_cookies(self) -> Dict[str, str]:
        """获取响应中的Cookie"""
        return dict(self.__session.cookies) if self.__session else {}

    def set_cookies(self, cookies: Dict[str, str]) -> 'tool_url':
        """设置Cookie"""
        if self.__session:
            self.__session.cookies.update(cookies)
        return self

    def clear_cookies(self) -> 'tool_url':
        """清除Cookie"""
        if self.__session:
            self.__session.cookies.clear()
        return self

    def get_auth(self) -> Tuple[str, str]:
        """获取认证信息"""
        return (self.__username, self.__password) if self.__username else (None, None)

    def set_auth(self, username: str, password: str) -> 'tool_url':
        """设置认证信息"""
        self.__username = username
        self.__password = password
        if self.__session:
            self.__session.auth = (username, password)
        return self

    def clear_auth(self) -> 'tool_url':
        """清除认证信息"""
        self.__username = None
        self.__password = None
        if self.__session:
            self.__session.auth = None
        return self

    def get_timeout(self) -> int:
        """获取超时设置"""
        return self.__session.timeout if self.__session else 30

    def set_timeout(self, timeout: int) -> 'tool_url':
        """设置超时时间"""
        if self.__session:
            self.__session.timeout = timeout
        return self

    def get_retry_delay(self) -> float:
        """获取重试延迟"""
        return self.__retry_delay

    def set_retry_delay(self, delay: float) -> 'tool_url':
        """设置重试延迟"""
        self.__retry_delay = delay
        return self

    def close(self) -> None:
        """关闭会话"""
        if self.__session:
            self.__session.close()
            self.__session = None

    def __del__(self):
        """析构函数"""
        self.close()

async def web_search(url: str, query: str, num_results: int = 10) -> List[Dict[str, str]]:
    """
    在url上执行搜索并返回搜索结果列表。
    Args:
        url (str): 搜索的url。
        query (str): 提交给 Bing 的搜索查询。
        num_results (int, optional): 要返回的搜索结果数量。默认为 10。
    Returns:
        List[Dict[str, str]]:
            title: 标题
            url: 链接
            description: 描述
    """
    import urllib.parse
    import aiohttp
    from bs4 import BeautifulSoup

    encoded_query = urllib.parse.quote(query)
    url = f"{url}/search?q={encoded_query}&count={num_results}"
    headers = build_web_header(url)
    results:List[Dict[str, str]] = []
    try:
        async with aiohttp.ClientSession() as session:
            async with session.get(url, headers=headers) as response:
                if response.status == 200:
                    html = await response.text()
                    soup = BeautifulSoup(html, "html.parser")
                    # 提取搜索结果
                    search_results = soup.select("li.b_algo")
                    count = 0
                    for result in search_results:
                        if count >= num_results:
                            break
                        # 提取标题和链接
                        title_element = result.select_one("h2 a")
                        if not title_element:
                            continue
                        title = title_element.get_text(strip=True)
                        link = title_element.get("href", "")
                        # 提取描述
                        abstract = ""
                        abstract_element = result.select_one(".b_caption p")
                        if abstract_element:
                            abstract = abstract_element.get_text(strip=True)
                        results.append({
                            "title": title,
                            "url": link,
                            "description": abstract
                        })
                        count += 1
                else:
                    return [{"title": "搜索失败", "url": "", "description": f"HTTP状态码: {response.status}"}]
    except Exception as e:
        return [{"title": "搜索出错", "url": "", "description": str(e)}]
    return results

def get_webpage_html(url: str) -> str:
    """
    获取网页的HTML内容
    """
    import requests
    response = requests.get(url, headers=build_web_header(url))
    response.raise_for_status()  # 检查请求是否成功
    response.encoding = 'utf-8'
    return response.text

def get_webpage_text(url: str) -> str:
    """
    获取网页的全部文本内容

    使用示例:
    ```python
    text = get_webpage_text("https://example.com")
    ```

    Args:
        url (str): 网页URL

    Returns:
        str: 网页的纯文本内容
    """
    import requests
    from bs4 import BeautifulSoup

    try:
        # 使用BeautifulSoup解析HTML
        soup = BeautifulSoup(get_webpage_html(url), 'html.parser')

        # 移除script和style标签
        for script in soup.find_all(["script", "style"]):
            script.decompose()
        # 获取纯文本，并处理编码问题
        text = soup.get_text(separator='\n', strip=True)
        # 处理可能的编码问题
        try:
            text = text.encode('utf-8').decode('utf-8')
        except UnicodeEncodeError:
            try:
                text = text.encode('gbk').decode('gbk')
            except UnicodeEncodeError:
                text = text.encode('gb2312').decode('gb2312')

        # 清理文本
        lines = (line.strip() for line in text.splitlines())
        chunks = (phrase.strip() for line in lines for phrase in line.split("  "))
        text = ' '.join(chunk for chunk in chunks if chunk)

        return text

    except requests.RequestException as e:
        return f"获取网页内容失败: {str(e)}"
    except Exception as e:
        return f"处理网页内容时出错: {str(e)}"


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
