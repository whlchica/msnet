#include "SrvTcp.h"
#include <iostream>

TcpServer::TcpServer(asio::io_context& ioCcontext, int port)
    : _acceptor(ioCcontext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
{
    doAccept();
}

void TcpServer::doAccept()
{
    _acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
        if (!ec) {
            std::make_shared<TcpSession>(std::move(socket))->start();
        }
        doAccept();
    });
}