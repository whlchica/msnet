
#include <iostream>
// #include "SrvTcp.h"
// void doSrvOnly(size_t port)
// {
//     std::cout << "Hello World Asio\n";
//     asio::io_context ioContext;
//     TcpServer        server(ioContext, port);
//     ioContext.run();
// }

#include "PoolSrvTcp.h"
void doSrvPool(size_t port, size_t thNum)
{
    auto          endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port);
    PoolTcpServer server(endpoint, thNum);
    server.run();
}

int usage()
{
    std::cerr << "Usage: msnet doSrvOnly <port>\n";
    std::cerr << "Usage: msnet doSrvPool <port> <thread num>\n";
    std::cerr << "Usage: msnet doHttpAccess <method> <ip> <port> <page> <param>\n";
}

#include "DevMng.h"
#include "Timer.h"

void doTimerNginxStat()
{
    doHttpAccess(GetXmlConfig(), "POST", "/stat", "");
}

int main(int argc, char const* argv[])
{
    CommTimer doTimerNginx;
    try {
        if (!XmlConfigInit("./conf/msnet.xml")) {
            std::cerr << "load msnet.xml error\n";
            exit(-1);
        }
        XmlConfig* conf = GetXmlConfig();
        // doTimerNginx.SetInterval(doTimerNginxStat, 50 * 1000);
        doSrvPool(GetXmlConfig()->server.port, 4);
    }
    catch (std::exception* e) {
    }
    doTimerNginx.Stop();
    return 0;
}
