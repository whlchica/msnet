
#include "AVframeRtmp.h"
#include "UtilRtmp.h"
#include <stdio.h>

#if defined(SRS_LIBRTMP)
AVframeRtmp::AVframeRtmp() {}
AVframeRtmp::~AVframeRtmp()
{
    srs_rtmp_destroy(_rtmp);
}

const char* AVframeRtmp::errorMsg()
{
    return _errMsg;
}

int AVframeRtmp::errorCode()
{
    return _errCode;
}

bool AVframeRtmp::initUrl(const char* url)
{
    printf("%s\n", url);
    _rtmp = srs_rtmp_create(url);
    if (srs_rtmp_handshake(_rtmp) != 0) {
        _errMsg = "simple handshake failed.";
        return false;
    }
    if (srs_rtmp_connect_app(_rtmp) != 0) {
        _errMsg = "connect vhost/app failed.";
        return false;
    }
    if (srs_rtmp_publish_stream(_rtmp) != 0) {
        _errMsg = "publish stream failed.";
        return false;
    }
    _errCode = 0;
    return true;
}

// 这里需要把
void AVframeRtmp::publishVideoFrame(char* frame, int len, int type, unsigned long long pts)
{
    if (_startTime == 0) {
        _startTime = pts;
    }
    _errCode = srs_h264_write_raw_frames(_rtmp, frame, len, (pts - _startTime) / 1000, pts / 1000);
    if (srs_h264_is_dvbsp_error(_errCode)) {
        _errMsg = "ignore drop video error";
    } else if (srs_h264_is_duplicated_sps_error(_errCode)) {
        _errMsg = "ignore duplicated sps";
    } else if (srs_h264_is_duplicated_pps_error(_errCode)) {
        _errMsg = "ignore duplicated pps";
    } else {
        _errMsg = "send h264 raw data failed";
    }
}
#else
#include "UtilRtmp.h"

AVframeRtmp::AVframeRtmp()
{
    _isWaitKeyframe = true;
    _frameType = 0;
    _startVTime = 0;
    _startATime = 0;
}
AVframeRtmp::~AVframeRtmp()
{
    if (_rtmp) {
        RTMP_Close(_rtmp);
        RTMP_Free(_rtmp);
        _rtmp = NULL;
    }
    _isWaitKeyframe = true;
    _frameType = 0;
    _startATime = 0;
    _startVTime = 0;
    _g726ToAac.uinit();
}

const char* AVframeRtmp::errorMsg()
{
    return _errMsg;
}

int AVframeRtmp::errorCode()
{
    return _errCode;
}

bool AVframeRtmp::initUrl(const char* url)
{
    printf("%s\n", url);
    do {
        _rtmp = RTMP_Alloc();
        RTMP_Init(_rtmp);
        // set connection timeout,default 30s
        if (!RTMP_SetupURL(_rtmp, ( char* )url)) {
            _errMsg = "SetupURL Error";
            break;
        }
        //
        RTMP_EnableWrite(_rtmp);
        if (0 == RTMP_Connect(_rtmp, NULL)) {
            _errMsg = "Connect Server Error";
            break;
        }
        //连接流
        if (0 == RTMP_ConnectStream(_rtmp, 0)) {
            _errMsg = "Connect Stream Error";
            RTMP_Close(_rtmp);
            break;
        }
        return true;
    } while (0);
    RTMP_Free(_rtmp);
    _rtmp = NULL;
    return false;
}

// 推流
void AVframeRtmp::publishVideoframe(char* frame, int len, int type, unsigned long long pts)
{
    std::vector<std::string> naluVec = ParseNalUnit(frame, len);
    int                      naluSize = naluVec.size();
    if (naluSize < 0) {
        return;
    }
    // 关键帧
    if (_isWaitKeyframe && type == 0x01) {
        unsigned char type = frame[4] & 0x1F;
        // 0x01 slice 0x05 idr 0x06 sei 0x07 sps 0x08 pps
        switch (type) {
        case avc::NALU_TYPE_IDR:
        case avc::NALU_TYPE_SPS:
        case avc::NALU_TYPE_PPS:
        case avc::NALU_TYPE_SLICE:
        case avc::NALU_TYPE_SEI:
            _frameType = VFRAME_TYPE_ID_H264;
            _publishFunc = Send264Videoframe;
            break;
        default:
            _frameType = VFRAME_TYPE_ID_HEVC;
            _publishFunc = Send265Videoframe;
            break;
        }
        _startVTime = pts;
        _isWaitKeyframe = false;
    }
    if (_frameType == 0) {
        return;
    }
    uint32_t dts = (pts - _startVTime) / 1000;
    for (int i = 0; i < naluSize; i++) {
        _publishFunc(_rtmp, naluVec[i], dts);
    }
}

// 发送AAC RTMP包
// 可以参考 http://billhoo.blog.51cto.com/2337751/1557646/
void AVframeRtmp::publishAudioframe(char* frame, int len, unsigned long long pts)
{
    if (_startATime == 0) {
        _startATime = pts;
        _g726ToAac.init();
    }
    int            aacLen = 0;
    unsigned char* aac = _g726ToAac.toAacEncodec(frame, len, aacLen);
    if (aacLen == 0) {
        return;
    }
    uint32_t    dts = (pts - _startATime) / 1000;
    static bool isSeedSpecialInfo = false;
    if (!isSeedSpecialInfo) {
        int            specialLen = 0;
        unsigned char* pSpecialData = _g726ToAac.aacSpecialData(&specialLen);
        SendAccAudioframe(_rtmp, pSpecialData, specialLen, dts, false);
        isSeedSpecialInfo = true;
    }
    SendAccAudioframe(_rtmp, aac, aacLen, dts);
}

#endif