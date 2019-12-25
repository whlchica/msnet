#ifndef TCPBUSINESSS_H
#define TCPBUSINESSS_H

#include <string>
#include "Common_Def.h"
ho::MsgHeader_t new_response(unsigned short codeId, int len);
std::string resolve_register_medialink(const char* data, int len, std::string& ss);

#endif