#ifndef __POOL_SRVTCP_H__
#define __POOL_SRVTCP_H__

#include "AsioPool.h"
#include "SrvTcpSession.h"

class PoolTcpServer : public noncopyable {
    IoServicePool           _ioSrvPool;  // 这里必须定义在_acceptor前面
    asio::ip::tcp::acceptor _acceptor;

public:
    PoolTcpServer(const asio::ip::tcp::endpoint& endpoint, size_t threadCnt)
        : _ioSrvPool(threadCnt), _acceptor(_ioSrvPool.get_io_service())
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
        std::shared_ptr<TcpSession> new_session(new TcpSession(_ioSrvPool.get_io_service()));
        auto&                       socket = new_session->socket();
        _acceptor.async_accept(socket, [this, new_session = std::move(new_session)](const asio::error_code& err) {
            if (!err) {
                new_session->start();
                doAccept();
            }
        });
    }
};

#endif