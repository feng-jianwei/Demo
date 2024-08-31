#include <initializer_list>
#include <iostream>
#include <type_traits>
#include "dao/postgres_conn.h"
#include "CivetServer.h"
using namespace std;
class MyHandler : public CivetHandler {
public:
    bool handleGet(CivetServer *server, struct mg_connection *conn) override {
        // 发送 HTTP 响应头
        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\n");
        // 发送 HTTP 响应体
        mg_printf(conn, "Hello, World!\n");
        return true; // 表示请求已经被处理
    }
};

bool Init_Service();

int main() {
    const char *options[] = {
        "document_root", ".",        // 文档根目录
        "listening_ports", "8080r,8443s",        // 监听端口 (8080 for HTTP, 8443 for HTTPS)
        "ssl_certificate", "config/combined.pem",           // SSL 证书文件路径
        "ssl_verify_peer", "0",
        NULL
    };

    // 创建服务器
    CivetServer server(options);

    // 创建并注册请求处理器
    MyHandler handler;
    server.addHandler("/hello", handler);



    // 等待服务器终止 (Ctrl+C)
    while (true) {
        // 保持服务器运行
    }

    return 0;
}