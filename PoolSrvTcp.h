#ifndef __POOL_SRVTCP_H__
#define __POOL_SRVTCP_H__

#include "AsioPool.h"
#include "SrvTcpSession.h"

class PoolTcpServer : public noncopyable {
    IoServicePool           _ioSrvPool;  // 这里必须定义在_acceptor前面
    asio::ip::tcp::acceptor _acceptor;

public:
    PoolTcpServer(const asio::ip::tcp::endpoint& endpoint, size_t threadCnt)
        : _ioSrvPool(threadCnt), _acceptor(_ioSrvPool.ioService())
    {
        _acceptor.open(endpoint.protocol());
        _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(1));
        _acceptor.bind(endpoint);
        _acceptor.listen();
    }

    void run()
    {
        doAccept();
        _ioSrvPool.run();
    }

private:
    void doAccept()
    {
        TcpSession_Ptr newSession(new TcpSession(_ioSrvPool.ioService()));
        auto&          socket = newSession->socket();
        _acceptor.async_accept(socket, [this, newSession = std::move(newSession)](const asio::error_code& err) {
            if (!err) {
                newSession->start();
                doAccept();
            }
        });
    }
};

#endif