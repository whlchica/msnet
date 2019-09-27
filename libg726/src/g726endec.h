#ifndef _G726_ENDEC_H_
#define _G726_ENDEC_H_

#include "g7xx/g726.h"
#include "g7xx/g7xx.h"

namespace g7xx
{
class G726EnDec : public G7xxEnDec
{
private:
	g726_state_t *g726;

public:
	G726EnDec(g7xx_bit_rate_e br, g7xx_packing_e pk);
	virtual ~G726EnDec();

	virtual int Decodec(char *g726Buffer, int g726Bytes, char pcmBuffer[1920]);
	virtual int Encode(char *pcmBuffer, int pcmBytes, char *g726Buffer);
};
} // namespace g7xx
#endif
