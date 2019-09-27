#ifndef __G7XX_H__
#define __G7XX_H__

//windows
#if defined(_WINDOWS) || defined(_WIN32) || defined(WIN32)

#ifdef G726ENDEC_EXPORTS
#define G7XX_EXPORT extern "C" __declspec(dllexport)
#else
#define G7XX_EXPORT extern "C" __declspec(dllimport)
#endif

#else
#define G7XX_EXPORT extern "C"
#endif

namespace g7xx
{
//比特率 bit rate
typedef enum
{
    BIT_RATE_16 = 0,
    BIT_RATE_24,
    BIT_RATE_32,
    BIT_RATE_40
} g7xx_bit_rate_e;

//打包方式 pack mode
typedef enum
{
    G7XX_PACKING_NONE = 0,
    G7XX_PACKING_LEFT, //对应海思MEDIA_G726类型 the type like hisi's MEDIA_G726
    G7XX_PACKING_RIGHT //对应海思G726类型 the type like hisi's G726
} g7xx_packing_e;

class G726EnDec;
class G7xxEnDec
{
public:
    virtual int Decodec(char *g7xxBuffer, int g7xxBytes, char pcmBuffer[1920]) = 0;
    virtual int Encode(char *pcmBuffer, int pcmBytes, char *g7xxBuffer) = 0;
    virtual ~G7xxEnDec();
};
G7XX_EXPORT const G7xxEnDec *NewG726EnDec(g7xx_bit_rate_e br, g7xx_packing_e pk);
} // namespace g7xx

#endif