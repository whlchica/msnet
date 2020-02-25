#ifndef __VFRAME_RTMP_H__
#define __VFRAME_RTMP_H__

// #define SRS_LIBRTMP 1

#if defined(SRS_LIBRTMP)
#include "srs/srs_librtmp.h"
#else
#include "librtmp/log.h"
#include "rtmp.h"
#endif
#include "AVg726ToAac.h"
// #include "AVmp4.h"
// #include "fpWriter.h"
#include <functional>
#include <string>

class AVframeRtmp {
private:
    AVg726ToAac        _g726ToAac;
    const char*        _errMsg;
    int                _errCode;
    unsigned long long _startVTime;
    unsigned long long _startATime;
    bool               _isWaitKeyframe;
    int                _frameType;
    // 发布流
#if defined(SRS_LIBRTMP)
    srs_rtmp_t _rtmp;
    AVg726     _avg726;
#else
    RTMP*                                            _rtmp;
    std::function<int(RTMP*, std::string, uint32_t)> _publishFunc;
#endif
    // fpWriter    _g726Writer;
    // AVmp4       _mp4Writer;
    std::string _fpName;

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