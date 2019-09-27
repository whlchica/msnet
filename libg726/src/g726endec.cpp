#include "g726endec.h"
#include "g7xx/g726.h"
#include <stdio.h>
#include <string.h>

namespace g7xx
{
const int KHisiG7xxHeadLen = 4;
const int kBitRates[] = {16000, 24000, 32000, 40000};
G726EnDec::G726EnDec(g7xx_bit_rate_e br, g7xx_packing_e pk)
{
	g726 = g726_init(NULL, kBitRates[br], G726_ENCODING_LINEAR, pk);
}

G726EnDec::~G726EnDec()
{
	if (g726)
	{
		g726_release(g726);
	}
}
// 功能：解码(单包G726音频数据)
// 参数：obj[in]:解码器对象			g726Buffer[in]:G726数据缓冲区
//		g726Bytes[in]:G726数据长度	pcmBuffer[out]:PCM数据缓冲区
// 返回：解码后数据长度
//----------------------
int G726EnDec::Decodec(char *g726Buffer, int g726Bytes, char pcmBuffer[1920])
{
	if (NULL == g726)
		return 0;

	//需要去掉海思四个头字节
	char *buffer = g726Buffer + KHisiG7xxHeadLen;
	g726Bytes -= KHisiG7xxHeadLen;

	int length = g726_decode(g726, (int16_t *)pcmBuffer, (const uint8_t *)buffer, g726Bytes);
	if (0 == length)
		return 0;

	length = length * sizeof(int16_t);
	//320的整数倍,则直接返回
	if ((length % 320) == 0)
		return length;

	int n320Count = length / 320;
	if ((length % 320) != 0)
		n320Count += 1;

	//正确长度大小
	int fullSize = n320Count * 320;
	int fillSize = fullSize - length;
	::memset(pcmBuffer + length, 0, fillSize);

	return length;
}

// 功能：编码
// 参数：obj[in]:编码器对象			pcmBuffer[in]:PCM数据缓冲区
//		pcmBytes[in]:pcm数据长度		g726Buffer[out]:G726数据缓冲区
// 返回：编码后数据长度
//------------------------
int G726EnDec::Encode(char *pcmBuffer, int pcmBytes, char *g726Buffer)
{
	if (0 == g726)
		return 0;

	//4个字节为海思标识头
	char *header = g726Buffer;
	header[0] = 0;
	header[1] = 0x01; //海思音频标识
	header[3] = 0;

	char *buffer = g726Buffer + KHisiG7xxHeadLen;
	int sample = pcmBytes / sizeof(int16_t);
	int length = g726_encode(g726, (uint8_t *)buffer, (const int16_t *)pcmBuffer, sample);

	int hisiLength = length / sizeof(int16_t);

	header[2] = (char)hisiLength; //数据长度

	return length + KHisiG7xxHeadLen;
}

const G7xxEnDec *NewG726EnDec(g7xx_bit_rate_e br, g7xx_packing_e pk)
{
	return new G726EnDec(br, pk);
}
} // namespace g7xx