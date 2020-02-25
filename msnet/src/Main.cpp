
#include <iostream>

#include "3rd/x2struct-1.1/xml_reader.h"
#include "SrvTcp.h"
void doSrvOnly(size_t port)
{
    std::cout << "Hello World Asio\n";
    asio::io_context ioContext;
    TcpServer        server(ioContext, port);
    ioContext.run();
}

#include "PoolSrvTcp.h"
void doSrvPool(size_t port, size_t thNum)
{
    auto          endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port);
    PoolTcpServer server(endpoint, thNum);
    server.run();
}

#include "Asiohttp.h"
#include "SrvTcpBusiness.h"
void doHttpAccess(const char* ip, const char* port, const char* method, const char* page, const char* param)
{
    Asiohttp http;
    http.initUrl(ip, port);
    std::string reponse;
    if (!strcmp(method, "POST")) {
        http.post(page, param, reponse);
    } else if (!strcmp(method, "GET")) {
        http.get(page, param, reponse);
    } else {
        return;
    }
    std::cout << reponse.c_str() << "\n";
    resolveNginxRtmpStat(reponse);
}

int usage()
{
    std::cerr << "Usage: msnet doSrvOnly <port>\n";
    std::cerr << "Usage: msnet doSrvPool <port> <thread num>\n";
    std::cerr << "Usage: msnet doHttpAccess <method> <ip> <port> <page> <param>\n";
}

int main(int argc, char const* argv[])
{
    // doSrvPool(33000, 4);
    // doHttpAccess("47.252.16.64", "8800", "POST", "/stat", "");
    // if (argc < 2) {
    //     return usage();
    // }
    try {
        // if (!strcmp(argv[1], "doSrvOnly")) {
        //     if (argc != 2) {
        //         return usage();
        //     }
        //     size_t port = atoi(argv[2]);
        //     doSrvOnly(port);
        // } else if (!strcmp(argv[1], "doSrvPool")) {
        //     if (argc != 4) {
        //         return usage();
        //     }
        //     size_t port = atoi(argv[2]);
        //     size_t thNum = atoi(argv[3]);
        //     doSrvPool(port, thNum);
        // } else if (!strcmp(argv[1], "doHttpAccess")) {
        //     if (argc < 6) {
        //         doHttpAccess("192.168.3.113", "8080", "GET", "/stat", "");
        //     } else {
        //         doHttpAccess(argv[4], argv[2], argv[3], argv[5], argv[6]);
        //     }
        // }
        doSrvPool(7799, 4);
    }
    catch (std::exception* e) {
    }

    for (;;) {
    }
    return 0;
}
