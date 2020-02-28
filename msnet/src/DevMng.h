#ifndef DEVMNG_H
#define DEVMNG_H

#include "ConfigXml.h"
#include "SrvTcpSession.h"
#include "thmutex.h"
#include <iostream>
#include <map>

void doHttpAccess(XmlConfig* conf, const char* method, const char* page, const char* param);

typedef struct ServerLive_t {
    tcp_session_ptr    tss;
    unsigned long long tick;
} ServerLive;

typedef std::map<std::string, ServerLive> DevsMap;

class DevMng {
private:
    DevsMap   _devMap;
    CommMutex _mtx;

public:
    DevMng(/* args */);
    ~DevMng();
    static DevMng* Instance();
    DevsMap        GetDevMap();

    bool AddDev(std::string devID, std::shared_ptr<TcpSession> ss);
    void RemoveDev(std::string devID, int nclient, int nCloseTime);
};

#endif