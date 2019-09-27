#ifndef SOCKET_H
#define SOKCET_H
#include <fcntl.h>
#include <sys/stat.h>
#ifdef WIN32
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
#include <sys/types.h>

#define NET_FALSE -1
#define NET_OK 0
#define NET_SOCKET_MAX_LISTEN (32)

#define NET_SOCKET_SEL_READ (0)
#define NET_SOCKET_SEL_WRITE (1)

#ifdef __cplusplus
extern "C" {
#endif
namespace net {
    int GetHostIP(const char* pIP, unsigned int& unAddr);
    int SetSocketStatus(int nSocket);
    int SetSocketLinger(int nSocket, int nOnoff, int nLinger);
    int InitUdpSocket(int* pSocket);
    int CloseSocket(int& nSocket);
    int InitUdpListen(int* pUdpSocket, unsigned short usPort);
    int InitTcpListen(int* pTcpSocket, unsigned short usPort);
    int TcpConnect(int* pTcpSocket, const char* szSvrIP, unsigned short usPort, unsigned long ulTimeout, unsigned short usBindPort);
    int TcpConnectEx(int* pTcpSocket, const char* szSvrIP, unsigned short usPort, unsigned short usBindPort);
    int WaitTcpConnect(int nSocket, unsigned long ulTimeout);
    int SetSockSendBufLen(int nSocket, int nSendLen);
    int SetSockRecvBufLen(int nSocket, int nRecvLen);
    int SetSockBroadcast(int nSocket);
    int AccpetTcpListen(int nTcpSocket);
    int SelectMulSocket(int* pSockArr, int nSockNum, int* pRet, int nType, unsigned long ulTimeout);
    int SelectSingleSocket(int nSocket, int nType, unsigned long ulTimeout);
    int SelectDuplexSocket(int nSocket, bool& bRead, bool& bWrite, unsigned long ulTimeout);
    int SendUdpDataToIPAddr(int nUdpSocket, const char* pBuf, int nLen, const char* pIp, int nPort, unsigned long ulTimeout);
    int SendUdpDataToSockaddr(int nUdpSocket, const char* pBuf, int nLen, const struct sockaddr_in* cliaddr, unsigned long ulTimeout);
    int SendTcpData(int nTcpSocket, const char* pBuf, int nLen, unsigned long ulTimeout);
    int RecvUdpData(int nUdpSocket, char* pBuf, int nLen, struct sockaddr_in* cliaddr, unsigned long ulTimeout);
    int RecvUdpDataEx(int nUdpSocket, char* pBuf, int nLen, struct sockaddr_in* cliaddr);
    int RecvTcpData(int nTcpSocket, char* pBuf, int nLen, unsigned long ulTimeout);
    int RecvTcpDataEx(int nTcpSocket, char* pBuf, int nLen);
}  // namespace net
#ifdef __cplusplus
}
#endif
#endif