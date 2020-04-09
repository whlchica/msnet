// #include "3rd/x2struct-1.1/x2struct.hpp"
// // niginx rtmp http req resolve
#include "DevMng.h"
#include "Time.h"
#include <map>
#include <string>

struct StreamItem {
    std::string name;
    int         clis;
    int64_t     bytesOut;
};

#include "3rd/x2struct-1.1/thirdparty/rapidxml/rapidxml.hpp"
#include "3rd/x2struct-1.1/thirdparty/rapidxml/rapidxml_print.hpp"
#include "Asiohttp.h"
void doHttpAccess(XmlConfig* conf, const char* method, const char* page, const char* param)
{
    Asiohttp http;
    http.initUrl(conf->nginx.ip.c_str(), conf->nginx.stat_port.c_str());
    std::string reponse;
    if (!strcmp(method, "POST")) {
        http.post(page, param, reponse);
    } else if (!strcmp(method, "GET")) {
        http.get(page, param, reponse);
    } else {
        return;
    }
    // std::cout << reponse.c_str() << "\n";
    rapidxml::xml_document<> doc;
    doc.parse<0>(( char* )reponse.c_str());
    rapidxml::xml_node<>* root = doc.first_node();
    rapidxml::xml_node<>* streamNode =
        root->first_node("server")->first_node("application")->first_node("live")->first_node("stream");
    if (streamNode == NULL) {
        return;
    }
    auto devMng = DevMng::Instance();
    while (streamNode) {
        if (strcmp(streamNode->name(), "stream") == 0) {
            StreamItem st;
            st.name = streamNode->first_node("name")->value();
            st.bytesOut = atoi(streamNode->first_node("bytes_out")->value());
            st.clis = atoi(streamNode->first_node("nclients")->value());
            std::cout << "name " << st.name << " bytes_out " << st.bytesOut << " nclients " << st.clis << "\n";
            devMng->RemoveDev(st.name, st.clis, conf->dev.auto_close_time);
            streamNode = streamNode->next_sibling();
        } else {
            break;
        }
    }
}

DevMng::DevMng(/* args */) {}

DevMng::~DevMng() {}

DevMng* DevMng::Instance()
{
    static DevMng ins;
    return &ins;
}

bool DevMng::AddDev(std::string devID, TcpSession_Ptr ss)
{
    CommRWLock lock(&_mtx);
    if (_devMap.size() >= GetXmlConfig()->dev.max_num) {
        return false;
    }
    ServerLive_t lv;
    lv.tss = ss;
    lv.tick = NowTickCount();
    printf("::----> %d %d %d\n", sizeof(ServerLive_t), sizeof(lv), _devMap.size());
    _devMap.insert(std::pair<std::string, ServerLive_t>(devID, lv));
    return true;
}

void DevMng::RemoveDev(std::string devID, int nclient, int nCloseTime)
{
    CommRWLock          lock(&_mtx);
    auto it = _devMap.find(devID);
    if (it != _devMap.end()) {
        ServerLive_t& sl = it->second;
        if (nclient <= 1) {
            // 没有消费者， 超时判断断开连接
            if (TimeoutSec(sl.tick, nCloseTime)) {
                sl.tss->autoClose();
                _devMap.erase(it);
            }
        } else {
            // 存在消费者， 更新时间戳
            sl.tick = NowTickCount();
        }
    }
}