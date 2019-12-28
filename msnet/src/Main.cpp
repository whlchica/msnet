
#include <iostream>
#if 0
#include "SrvTcp.h"
int main(int argc, char* argv[])
{
    std::cout << "Hello World Asio\n";
    try {
        asio::io_context ioContext;
        TcpServer server(ioContext, 33000);
        ioContext.run();
    }
    catch (std::exception* e) {
    }
    for (;;) {
    }
}
#else
#include "PoolSrvTcp.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello World Asio\n";
    try {
        auto          endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8800);
        PoolTcpServer server(endpoint, 4);
        server.run();
    }
    catch (std::exception* e) {
    }
    for (;;) {
    }
}
#endif
