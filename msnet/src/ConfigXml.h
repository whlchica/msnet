#include <iostream>

struct NginxConifg {
    std::string ip;
    std::string stat_port;
};

struct DevConfig {
    int max_num;
    int auto_close_time;
    int bytes_out;
};

struct ServerConfig {
    int port;
};

struct XmlConfig {
    NginxConifg nginx;
    DevConfig dev;
    ServerConfig server;
};

bool XmlConfigInit(const char* xmlPath);
struct XmlConfig* GetXmlConfig();
std::string GetRtmpBaseUrl();


