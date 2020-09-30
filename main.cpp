#include<iostream>

#include "spdlog/spdlog.h"

#include "bufio.hpp"
#include "cpptoml.h"
#include "resp.hpp"

const int listen_port = 8081;
const int target_port = 8080;

int main(){

    auto config = cpptoml::parse_file("../config.toml");
    auto port = config->get_qualified_as<int>("ENV.port");

    std::cout << "server will start at port :" << *port << "\n";

    SimpleString str("OK");

    spdlog::info("request: {}", str.get_value());

    sockpp::tcp_acceptor acc(listen_port);
    sockpp::inet_address peer;
    for (;;) {
        sockpp::tcp_socket sock = acc.accept(&peer);
        if (!sock) {
            spdlog::error("accept error");
            continue;
        }
        spdlog::info("accept a connect from {}", peer.address());

        bufio::BufReader<MaxBufSize>:: Src(&sock);
        
    }

}