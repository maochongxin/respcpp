#include "sockpp/tcp_acceptor.h"
#include <iostream>

void print_without_crlf(char *ptr);

int main(){
    // 指定监听端口
    int16_t port = 8080;
    // 使用端口启动监听
    sockpp::tcp_acceptor acc(port);
    if (!acc){
        std::cerr << acc.last_error_str() << '\n';
        exit(1);
    }
    std::cout << "try accept a conn at port " << port << '\n';
    // 接收连接请求
    sockpp::tcp_socket sock = acc.accept();

    // 创建缓存
    char buf[1024];

    // 循环接收打印并且echo
    for(;;){
        auto siz = sock.read(buf, 1024);
        if(siz > 0){
            print_without_crlf(buf);
            sock.write(buf, siz);
        }else{
            // siz < 0（断开连接）会退出
            break;
        }
    }
    std::cout << "closed" << '\n';
}

// 这个函数为什么这么写稍后解释
void print_without_crlf(char *ptr){
    std::cout << "recv: ";
    for(;;){
        if(*ptr == '\r' || *ptr == '\n'){
            break;
        }
        std::cout << *ptr;
        ptr+=1;
    }
    std::cout << '\n';
}