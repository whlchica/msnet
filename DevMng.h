#ifndef DEVMNG_H
#define DEVMNG_H

#include "ConfigXml.h"
#include "SrvTcpSession.h"
#include "ThMutex.h"
#include <iostream>
#include <unordered_map>

void doHttpAccess(XmlConfig* conf, const char* method, const char* page, const char* param);

typedef struct ServerLive {
    TcpSession_Ptr     tss;
    unsigned long long tick;
} ServerLive_t;

// 这里不知道为什么map出问题，会引起can't access memory
typedef std::unordered_map<std::string, ServerLive_t> TyDevsMap;

class DevMng {
private:
    TyDevsMap _devMap;
    CommMutex _mtx;

public:
    DevMng(/* args */);
    ~DevMng();
    static DevMng* Instance();

    bool AddDev(std::string devID, TcpSession_Ptr ss);
    void RemoveDev(std::string devID, int nclient, int nCloseTime);
};

#endif