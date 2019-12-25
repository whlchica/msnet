#include "SrvTcpBusiness.h"
#include "x2struct.hpp"

ho::MsgHeader_t new_response(unsigned short codeId, int len)
{
    ho::MsgHeader_t pMsg;
    pMsg.ucFlag       = ho::MsgFlag;
    pMsg.ucVersion    = ho::MsgVersion;
    pMsg.usCodeID     = codeId;
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
    resp.ss  = req.ss;
    resp.err = 0;
    return x2struct::X::tojson(resp);
}
