from http.server import HTTPServer, BaseHTTPRequestHandler
import http.server
import socketserver
import os

from ..Str.Core import *

local_message = "<init>"

class __SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    def __init__(self, request, client_address, server):
        super().__init__(request, client_address, server)

    def _do_success_send(self, keyword = 'Content-type', value = 'text/html'):
        self.send_response(200)
        self.send_header(keyword=keyword, value=value)
        self.end_headers()

    def _headers_length(self):
        return int(self.headers['Content-Length'])

    def do_log(self, tag="message"):
        print(f"[{tag}]: {local_message}")

    def do_GET(self):
        self._do_success_send()
        self.wfile.write(bytes(local_message, "utf8"))
        self.do_log()

    def do_POST(self):
        global local_message
        local_message = self.rfile.read(self._headers_length()).decode("utf-8")
        self._do_success_send()
        self.wfile.write(bytes(local_message, "utf8"))
        self.do_log()

    def do_PUT(self):
        content_length = int(self.headers['Content-Length'])
        global local_message
        if content_length < 0:
            local_message = self.rfile.read(content_length).decode("utf-8")
        else:
            local_message = "<empty, but put and reflash>"
        self._do_success_send()
        self.wfile.write(bytes(local_message, "utf8"))
        self.do_log()

    def do_DELETE(self):
        self._do_success_send()
        global local_message
        local_message = '<deleted>'
        self.wfile.write(bytes(local_message, "utf8"))
        self.do_log()

def http_default_serve_forever(PORT):
    class CustomHandler(http.server.SimpleHTTPRequestHandler):
        def do_GET(self):
            # 检查请求的文件是否存在
            if os.path.exists(self.translate_path(self.path)):
                super().do_GET()
            else:
                # 如果文件不存在，返回index.html
                self.path = '/index.html'
                super().do_GET()

    #PORT = int(input("输入端口号"))
    # 创建一个TCP服务器
    with socketserver.TCPServer(("", PORT), CustomHandler) as httpd:
        print(f"Serving at port {PORT}")
        # 开始服务，直到按下Ctrl+C
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
