#ifndef __VFRAME_RTMP_H__
#define __VFRAME_RTMP_H__

#if defined(SRS_LIBRTMP)
#include "srs_librtmp.h"
#else
#include "rtmp.h"
#endif
#include "AVg726ToAac.h"
#include <functional>
#include <string>

class AVframeRtmp {
private:
#if defined(SRS_LIBRTMP)
    srs_rtmp_t _rtmp;
#else
    RTMP* _rtmp;
#endif
    AVg726ToAac        _g726ToAac;
    const char*        _errMsg;
    int                _errCode;
    unsigned long long _startVTime;
    unsigned long long _startATime;
    bool               _isWaitKeyframe;
    int                _frameType;
    // 发布流
    std::function<int(RTMP*, std::string, uint32_t)> _publishFunc;

public:
    AVframeRtmp();
    ~AVframeRtmp();

    bool        initUrl(const char* url);
    const char* errorMsg();
    int         errorCode();

    void publishVideoframe(char* frame, int len, int type, unsigned long long pts);
    void publishAudioframe(char* frame, int len, unsigned long long pts);
};

#endif