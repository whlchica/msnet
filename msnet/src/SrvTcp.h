#ifndef TCPSERVER_H
#define TCPSERVER_H
#pragma once

#include "SrvTcpSession.h"
class TcpServer : public asio::noncopyable {
    asio::ip::tcp::acceptor _acceptor;

public:
    TcpServer(asio::io_context& ioCcontext, int port)
        : _acceptor(ioCcontext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    {
        doAccept();
    }

private:
    void doAccept()
    {
        _acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                std::make_shared<TcpSession>(std::move(socket))->start();
            }
            doAccept();
        });
    }
};

#endif  // !TCPSERVER_H
