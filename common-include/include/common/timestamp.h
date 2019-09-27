#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#include <inttypes.h>
#include <stdint.h>
#include <string>
#include <sys/time.h>

class CTimeStamp {
private:
    /* data */
public:
    CTimeStamp() : _microSecondsSinceEpoch(0) {}

    int64_t MicroSecondsSinceEpoch() const
    {
        return _microSecondsSinceEpoch;
    }
    //@param microSecondsSinceEpoch
    explicit CTimeStamp(int64_t microSecondsSinceEpoch) : _microSecondsSinceEpoch(microSecondsSinceEpoch) {}

    //
    // Get time of now.
    //
    std::string ToString() const
    {
        char    buf[32]      = { 0 };
        int64_t seconds      = _microSecondsSinceEpoch / kMicroSecondsPerSecond;
        int64_t microseconds = _microSecondsSinceEpoch % kMicroSecondsPerSecond;
        snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
        return buf;
    }

    bool Valid() const
    {
        return _microSecondsSinceEpoch > 0;
    }

    static CTimeStamp Now()
    {
        struct timeval tv;
        gettimeofday(&tv, 0);
        int64_t seconds = tv.tv_sec;
        return CTimeStamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
    }
    static CTimeStamp Invalid()
    {
        return CTimeStamp();
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t _microSecondsSinceEpoch;
};

inline CTimeStamp AddTime(CTimeStamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * CTimeStamp::kMicroSecondsPerSecond);
    return CTimeStamp(timestamp.MicroSecondsSinceEpoch() + delta);
}

inline bool operator<(CTimeStamp lhs, CTimeStamp rhs)
{
    return lhs.MicroSecondsSinceEpoch() < rhs.MicroSecondsSinceEpoch();
}

inline bool operator==(CTimeStamp lhs, CTimeStamp rhs)
{
    return lhs.MicroSecondsSinceEpoch() == rhs.MicroSecondsSinceEpoch();
}

#endif
