#ifndef TCPSERVER_H
#define TCPSERVER_H
#pragma once

#include "SrvTcpSession.h"
typedef std::shared_ptr<TcpSession> tcpsession_shared_ptr;

class TcpServer  {
    asio::ip::tcp::acceptor _acceptor;

public:
    TcpServer(asio::io_context& ioCcontext, int port);
    
private:
    void doAccept();
};

#endif  // !TCPSERVER_H
