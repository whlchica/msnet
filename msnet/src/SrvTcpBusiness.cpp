#include "SrvTcpBusiness.h"
#include "3rd/x2struct-1.1/x2struct.hpp"

ho::MsgHeader_t new_response(unsigned short codeId, int len)
{
    ho::MsgHeader_t pMsg;
    pMsg.ucFlag = ho::MsgFlag;
    pMsg.ucVersion = ho::MsgVersion;
    pMsg.usCodeID = codeId;
    pMsg.unPayloadLen = len;
    return pMsg;
}

struct RegisterMediaLink {
    std::string ss;
    std::string dn;
    std::string at;
    std::string mt;
    std::string ch;
    std::string of;
    std::string ft;
    XTOSTRUCT(O(ss, dn, at, mt, ch, of, ft));
};

struct ResponseMediaLink {
    std::string ss;
    int         err;
    int         of;
    XTOSTRUCT(O(ss, err, of));
};

std::string resolve_register_medialink(const char* data, int len, std::string& ss)
{
    std::string       repStr(data, len);
    RegisterMediaLink req;
    if (!x2struct::X::loadjson(repStr, req, false)) {
        return NULL;
    }
    ss = req.ss;
    ResponseMediaLink resp;
    resp.ss = req.ss;
    resp.err = 0;
    return x2struct::X::tojson(resp);
}

// niginx rtmp http req resolve

struct NginxRtmpStream {
    std::string name;
    uint32_t    nclients;
    uint32_t    publishing;
    XTOSTRUCT(O(name, nclients, publishing));
};

struct NginxRtmpLiveHls {
    std::vector<NginxRtmpStream> stream;
    XTOSTRUCT(M(stream));
};

struct NginxRtmpApplication {
    std::string      name;  //"live" "hls"
    NginxRtmpLiveHls live;
    NginxRtmpLiveHls hls;
    XTOSTRUCT(M(name), O(live, hls));
};

struct NginxRtmpServer {
    std::vector<NginxRtmpApplication> application;
    XTOSTRUCT(M(application));
};
struct NginxRtmpRtmp {
    NginxRtmpServer server;
    XTOSTRUCT(M(server));
};

void resolveNginxRtmpStat(std::string rspStr)
{
    NginxRtmpRtmp rtmp;
    x2struct::X::loadxml(rspStr, rtmp, false);
    printf("stat.application size %d\n", rtmp.server.application.size());
    for (int i = 0; i < rtmp.server.application.size(); i++) {
        NginxRtmpApplication app = rtmp.server.application[i];
        printf("\tapp name %s\n", app.name);
        if (app.name == "live") {
            for (int j = 0; j < app.live.stream.size(); j++) {
                printf("\t\tstream %d, name %s, client %d\n", j, app.live.stream[j].name, app.live.stream[j].nclients);
            }
        } else if (app.name == "hls") {
            for (int j = 0; j < app.live.stream.size(); j++) {
                printf("\t\tstream %d, name %s, client %d\n", j, app.live.stream[j].name, app.live.stream[j].nclients);
            }
        }
    }
}