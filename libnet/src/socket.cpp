#include "net/socket.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef WIN32
#define socklen_t int
#pragma comment(lib, "ws2_32.lib")
#else
#include <netdb.h>
#include <netinet/in.h>

#include <sys/select.h>
#include <sys/socket.h>
#endif

namespace net {

int GetHostIP(const char* pIP, unsigned int& unAddr)
{

#if (defined __APPLE__) || (defined WIN32)
    struct hostent* hp = NULL;
    if ((hp = gethostbyname(pIP)) != NULL) {
        if (hp->h_addr_list[0]) {
            lAddr = *(( unsigned int* )hp->h_addr_list[0]);
        } else {
            return NET_FALSE;
        }
    }
#else
    struct hostent  hostbuf;
    struct hostent* hp              = NULL;
    size_t          hstbufLen       = 1024;
    char            tmphstBuf[1024] = { 0 };
    int             res = 0, herr = 0;
    res = gethostbyname_r(pIP, &hostbuf, tmphstBuf, hstbufLen, &hp, &herr);
    if (res || hp == NULL) {
        return NET_FALSE;
    }
    unAddr = *(( unsigned int* )hp->h_addr_list[0]);
#endif
    return NET_OK;
}

int SetSocketStatus(int nSocket)
{
#ifdef WIN32
    unsigned long ul = 1;
    ioctlsocket(nSocket, FIONBIO, &ul);
#else
    // 获取文件描述符状态
    int nFlags = fcntl(nSocket, F_GETFL, 0);
    if (-1 == nFlags) {
        return NET_FALSE;
    }
    // 设置文件描述符为非阻塞
    if (-1 == fcntl(nSocket, F_SETFL, nFlags | O_NONBLOCK)) {
        return NET_FALSE;
    }
#endif

    int bReUseAddr = -1;
    // 设置套接字的属性使它能够在计算机重启的时候可以再次使用套接字的端口和IP
    int error = setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, ( const char* )&bReUseAddr, size_t(bReUseAddr));
    if (0 != error) {
        return NET_FALSE;
    }

    // SO_LINGER, 优雅关闭socket，会保证缓冲区的数据全部发送完成
    //此选项对UDP的SOCKET无效
    // struct linger zeroLinger = {1, 2};
    // setsockopt(nSocket, SOL_SOCKET, SO_LINGER, (const char *)&zeroLinger, sizeof(linger));

    //设置SOCKET缓存
    // SetSockSendBufLen(nSocket, 64 * 1024);
    return NET_OK;
}

int SetSocketLinger(int nSocket, int nOnoff, int nLinger)
{
    struct linger zeroLinger = { 1, 2 };
    setsockopt(nSocket, SOL_SOCKET, SO_LINGER, ( const char* )&zeroLinger, sizeof(linger));
    return NET_OK;
}

int InitUdpSocket(int* pSocket)
{
    int nSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == nSocket) {
        return NET_FALSE;
    }
    SetSocketStatus(nSocket);
    *pSocket = nSocket;
    return NET_OK;
}

int CloseSocket(int& nSocket)
{
    if (-1 == nSocket) {
        return NET_FALSE;
    }
#ifdef WIN32
    closesocket(nSocket);
#else
    close(nSocket);
#endif
    nSocket = -1;
    return NET_OK;
}

int InitUdpListen(int* pUdpSocket, unsigned short usPort)
{
    int nSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == nSocket) {
        return NET_FALSE;
    }
    SetSocketStatus(nSocket);
    struct sockaddr_in addr = { 0 };
    addr.sin_family         = AF_INET;
    addr.sin_port           = htons(usPort);
    addr.sin_addr.s_addr    = INADDR_ANY;
    int iRet                = bind(nSocket, ( struct sockaddr* )&addr, sizeof(struct sockaddr_in));
    if (-1 == iRet) {
        CloseSocket(nSocket);
    } else {
        *pUdpSocket = nSocket;
        return NET_OK;
    }
    return NET_FALSE;
}

int InitTcpListen(int* pTcpSocket, unsigned short usPort)
{
    int nSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == nSocket) {
        return NET_FALSE;
    }

    SetSocketStatus(nSocket);
    struct sockaddr_in addr = { 0 };
    addr.sin_family         = AF_INET;
    addr.sin_port           = htons(usPort);
    addr.sin_addr.s_addr    = INADDR_ANY;
    int iRet                = bind(nSocket, ( struct sockaddr* )&addr, sizeof(struct sockaddr_in));
    if (-1 == iRet) {
        CloseSocket(nSocket);
        return NET_FALSE;
    }

    iRet = listen(nSocket, NET_SOCKET_MAX_LISTEN);
    if (-1 == iRet) {
        CloseSocket(nSocket);
        return NET_FALSE;
    }
    *pTcpSocket = nSocket;
    return NET_OK;
}

int TcpConnect(int* pTcpSocket, const char* szSvrIP, unsigned short usPort, unsigned long ulTimeout, unsigned short usBindPort)
{
    int nSocket = socket(PF_INET, SOCK_STREAM, 0);
    *pTcpSocket = -1;
    if (-1 == nSocket) {
        return NET_FALSE;
    }
    SetSocketStatus(nSocket);
    if (0 != usBindPort) {
        struct sockaddr_in addr = { 0 };
        addr.sin_family         = AF_INET;
        addr.sin_port           = htons(usBindPort);
        addr.sin_addr.s_addr    = INADDR_ANY;
        if (-1 == bind(nSocket, ( struct sockaddr* )&addr, sizeof(struct sockaddr_in))) {
            CloseSocket(nSocket);
            return NET_FALSE;
        }
    }

    struct sockaddr_in servAddr = { 0 };
    servAddr.sin_family         = AF_INET;
    servAddr.sin_port           = htons(usPort);
    //		servAddr.sin_addr.s_addr = inet_addr(szSvrIP);
    unsigned int addr = 0;
    if (-1 == GetHostIP(szSvrIP, addr)) {
        CloseSocket(nSocket);
        return NET_FALSE;
    }

    servAddr.sin_addr.s_addr = ( unsigned int )addr;
    int nRet                 = connect(nSocket, ( struct sockaddr* )&servAddr, sizeof(servAddr));
    if (-1 == nRet) {
#ifdef WIN32
        if (WSAEWOULDBLOCK != WSAGetLastError())
#else
        if (errno != EINPROGRESS)
#endif
        {
            CloseSocket(nSocket);
            return NET_FALSE;
        }

        struct timeval tm;
        fd_set         set;
        tm.tv_sec  = ulTimeout / 1000;
        tm.tv_usec = (ulTimeout % 1000) * 1000;
        FD_ZERO(&set);
        FD_SET(nSocket, &set);
#ifdef WIN32
        nRet = select(nSocket, NULL, &set, NULL, &tm);
#else
        nRet = select(nSocket + 1, NULL, &set, NULL, &tm);
#endif
        if (nRet > 0) {

            int       error = 0;
            socklen_t len   = sizeof(int);
#ifdef WIN32
            if ((0 == getsockopt(nSocket, SOL_SOCKET, SO_ERROR, ( char* )&error, &len)))
#else
            if ((0 == getsockopt(nSocket, SOL_SOCKET, SO_ERROR, &error, &len)))
#endif
            {
                if (0 == error) {
                    *pTcpSocket = nSocket;
                    return NET_OK;
                }
            }
        }
        CloseSocket(nSocket);
        return NET_FALSE;
    }
    *pTcpSocket = nSocket;
    return NET_OK;
}

int TcpConnectEx(int* pTcpSocket, const char* szSvrIP, unsigned short usPort, unsigned short usBindPort)
{
    // int error;
    int nRet;
    int nSocket = socket(PF_INET, SOCK_STREAM, 0);
    *pTcpSocket = -1;
    if (-1 == nSocket) {
        return NET_FALSE;
    }

    SetSocketStatus(nSocket);
    if (usBindPort != 0) {
        struct sockaddr_in addr = { 0 };
        addr.sin_family         = AF_INET;
        addr.sin_port           = htons(usBindPort);
        addr.sin_addr.s_addr    = INADDR_ANY;
        if (-1 == bind(nSocket, ( struct sockaddr* )&addr, sizeof(struct sockaddr_in))) {
            CloseSocket(nSocket);
            return NET_FALSE;
        }
    }

    struct sockaddr_in servAddr = { 0 };
    servAddr.sin_family         = AF_INET;
    servAddr.sin_port           = htons(usPort);
    //		servAddr.sin_addr.s_addr = inet_addr(szSvrIP);
    unsigned int addr = 0;
    if (NET_FALSE == GetHostIP(szSvrIP, addr)) {
        CloseSocket(nSocket);
        return NET_FALSE;
    }

    servAddr.sin_addr.s_addr = ( unsigned int )addr;
    nRet                     = connect(nSocket, ( struct sockaddr* )&servAddr, sizeof(servAddr));
    if (nRet == -1) {
#ifdef WIN32
        if (WSAEWOULDBLOCK != WSAGetLastError())
#else
        if (errno != EINPROGRESS)
#endif
        {
            CloseSocket(nSocket);
            printf("[%s] connect error(%s)", __func__, strerror(errno));
            return NET_FALSE;
        }
    }
    *pTcpSocket = nSocket;
    return NET_OK;
}

int WaitTcpConnect(int nSocket, unsigned long ulTimeout)
{
    struct timeval tm;
    fd_set         set;
    tm.tv_sec  = ulTimeout / 1000;
    tm.tv_usec = (ulTimeout % 1000) * 1000;
    FD_ZERO(&set);
    FD_SET(nSocket, &set);

#ifdef WIN32
    int nRet = select(nSocket, NULL, &set, NULL, &tm);
#else
    int nRet = select(nSocket + 1, NULL, &set, NULL, &tm);
#endif
    if (nRet > 0) {
        int       error = 0;
        socklen_t len   = sizeof(int);
#ifdef WIN32
        if ((0 == getsockopt(nSocket, SOL_SOCKET, SO_ERROR, ( char* )&error, &len)))
#else
        if ((0 == getsockopt(nSocket, SOL_SOCKET, SO_ERROR, &error, &len)))
#endif
        {
            if (0 == error) {
                return NET_OK;
            }
        }
    }
    return NET_FALSE;
}

int SetSockSendBufLen(int nSocket, int nSendLen)
{
    int sockbuflen = nSendLen;
    setsockopt(nSocket, SOL_SOCKET, SO_SNDBUF, ( char* )&sockbuflen, sizeof(int));
    return NET_OK;
}

int SetSockRecvBufLen(int nSocket, int nRecvLen)
{
    int sockbuflen = nRecvLen;
    setsockopt(nSocket, SOL_SOCKET, SO_RCVBUF, ( char* )&sockbuflen, sizeof(int));
    return NET_OK;
}

int SetSockBroadcast(int nSocket)
{
    int bOptVal = 1;
    setsockopt(nSocket, SOL_SOCKET, SO_BROADCAST, ( const char* )&bOptVal, sizeof(int));
    return NET_OK;
}
int AccpetTcpListen(int nTcpSocket)
{
    int nClientSock = accept(nTcpSocket, NULL, NULL);
    return nClientSock;
}

int SelectMulSocket(int* pSockArr, int nSockNum, int* pRet, int nType, unsigned long ulTimeout)
{
    struct timeval timeout;
    fd_set         fd;
    int            rc = 0;
    FD_ZERO(&fd);

    timeout.tv_sec  = ulTimeout / 1000;
    timeout.tv_usec = (ulTimeout % 1000) * 1000;
    int maxfd       = 0;
    int i           = 0;
    for (i = 0; i < nSockNum; ++i) {
        if (pSockArr[i] > maxfd) {
            maxfd = pSockArr[i];
        }
        FD_SET(pSockArr[i], &fd);
        pRet[i] = 0;
    }
    if (0 == nType) {
#ifdef WIN32
        rc = select(maxfd, &fd, NULL, NULL, &timeout);
#else
        rc = select(maxfd + 1, &fd, NULL, NULL, &timeout);
#endif
    } else {
#ifdef WIN32
        rc = select(maxfd, NULL, &fd, NULL, &timeout);
#else
        rc = select(maxfd + 1, NULL, &fd, NULL, &timeout);
#endif
    }
    if (rc > 0) {
        for (i = 0; i < nSockNum; ++i) {
            if (FD_ISSET(pSockArr[i], &fd)) {
                pRet[i] = 1;
            } else {
                pRet[i] = 0;
            }
        }
    }
    return rc;
}

int SelectSingleSocket(int nSocket, int nType, unsigned long ulTimeout)
{
    struct timeval timeout;
    fd_set         fd;
    fd_set         ep;
    int            rc = 0;
    FD_ZERO(&fd);
    FD_SET(nSocket, &fd);
    FD_ZERO(&ep);
    FD_SET(nSocket, &ep);
    timeout.tv_sec  = ulTimeout / 1000;
    timeout.tv_usec = (ulTimeout % 1000) * 1000;

    while (1) {
        if (NET_SOCKET_SEL_READ == nType) {
#ifdef WIN32
            rc = select(nSocket, &fd, NULL, NULL, &timeout);
#else
            rc = select(nSocket + 1, &fd, NULL, NULL, &timeout);
#endif
        } else {
#ifdef WIN32
            rc = select(nSocket, NULL, &fd, NULL, &timeout);
#else
            rc = select(nSocket + 1, NULL, &fd, NULL, &timeout);
#endif
        }
        if (rc > 0) {
            if (FD_ISSET(nSocket, &fd)) {
                break;
            }
        } else if (-1 == rc) {
            if (EINTR == errno || EAGAIN == errno) {
                continue;
            }
        }
        break;
    }
    return rc;
}

int SelectDuplexSocket(int nSocket, bool& bRead, bool& bWrite, unsigned long ulTimeout)
{
    fd_set fdRead;
    FD_ZERO(&fdRead);
    FD_SET(nSocket, &fdRead);
    bRead = false;

    fd_set fdWrite;
    FD_ZERO(&fdWrite);
    FD_SET(nSocket, &fdWrite);
    bWrite = false;

    struct timeval timeout;
    timeout.tv_sec  = ulTimeout / 1000;
    timeout.tv_usec = (ulTimeout % 1000) * 1000;

    int rc = 0;
    while (1) {
#ifdef WIN32
        rc = select(nSocket, &fdRead, &fdWrite, NULL, &timeout);
#else
        rc = select(nSocket + 1, &fdRead, &fdWrite, NULL, &timeout);
#endif
        if (rc > 0) {
            if (FD_ISSET(nSocket, &fdWrite)) {
                bWrite = true;
            }
            if (FD_ISSET(nSocket, &fdRead)) {
                bRead = true;
            }
        } else if (-1 == rc) {
            if (EINTR == errno || EAGAIN == errno) {
                continue;
            }
        }
        break;
    }
    return rc;
}

int SendUdpDataToIPAddr(int nUdpSocket, const char* pBuf, int nLen, const char* pIp, int nPort, unsigned long ulTimeout)
{
    struct sockaddr_in addrto = { 0 };
    addrto.sin_family         = AF_INET;  //默认
                                          //	addrto.sin_addr.s_addr = inet_addr(pIp);
    unsigned int addr = 0;
    if (NET_FALSE == GetHostIP(pIp, addr))
        return 0;
    addrto.sin_addr.s_addr = ( unsigned int )addr;

    addrto.sin_port = htons(nPort);
    return SendUdpDataToSockaddr(nUdpSocket, pBuf, nLen, &addrto, ulTimeout);
}

int SendUdpDataToSockaddr(int nUdpSocket, const char* pBuf, int nLen, const struct sockaddr_in* cliaddr, unsigned long ulTimeout)
{
    if (SelectSingleSocket(nUdpSocket, NET_SOCKET_SEL_WRITE, ulTimeout) > 0) {
        int iRetVal = ( int )sendto(nUdpSocket, pBuf, nLen, 0, ( struct sockaddr* )cliaddr, sizeof(struct sockaddr_in));
        return iRetVal;
    }
    return 0;
}

int SendTcpData(int nTcpSocket, const char* pBuf, int nLen, unsigned long ulTimeout)
{
    int nRetVal  = 0;
    int nSendLen = nLen;
    int nSelectRet;

    nSelectRet = SelectSingleSocket(nTcpSocket, NET_SOCKET_SEL_WRITE, ulTimeout);
    if (nSelectRet > 0) {
        nRetVal  = ( int )send(nTcpSocket, pBuf, nLen, 0);
        nSendLen = nRetVal;
    } else {
        if (errno == EBADF) {
            return -1;
        } else {
            int       error = -1;
            socklen_t len   = sizeof(int);
#ifdef WIN32
            getsockopt(nTcpSocket, SOL_SOCKET, SO_ERROR, ( char* )&error, &len);
#else
            getsockopt(nTcpSocket, SOL_SOCKET, SO_ERROR, &error, &len);
#endif
            if (error != 0)  //出错
            {
                nSendLen = -1;
            } else {
                nSendLen = 0;
            }
        }
    }
    return nSendLen;
}

int RecvUdpData(int nUdpSocket, char* pBuf, int nLen, struct sockaddr_in* cliaddr, unsigned long ulTimeout)
{
    if (SelectSingleSocket(nUdpSocket, NET_SOCKET_SEL_READ, ulTimeout) > 0) {
        /*
        错误代码 EBADF 参数s非合法的socket处理代码
        EFAULT 参数中有一指针指向无法存取的内存空间。
        ENOTSOCK 参数s为一文件描述词，非socket。
        EINTR 被信号所中断。
        EAGAIN 此动作会令进程阻断，但参数s的socket为不可阻断。
        ENOBUFS 系统的缓冲内存不足
        ENOMEM 核心内存不足
        EINVAL 传给系统调用的参数不正确。
        */
        socklen_t addrlen = sizeof(sockaddr_in);
        int       recvlen = ( int )recvfrom(nUdpSocket, pBuf, nLen, 0, ( struct sockaddr* )cliaddr, ( socklen_t* )&addrlen);
        return recvlen;
    }
    return 0;
}

int RecvUdpDataEx(int nUdpSocket, char* pBuf, int nLen, struct sockaddr_in* cliaddr)
{
    socklen_t addrlen = sizeof(sockaddr_in);
    int       recvlen = ( int )recvfrom(nUdpSocket, pBuf, nLen, 0, ( struct sockaddr* )cliaddr, ( socklen_t* )&addrlen);
    return recvlen;
}

int RecvTcpData(int nTcpSocket, char* pBuf, int nLen, unsigned long ulTimeout)
{
    if (SelectSingleSocket(nTcpSocket, NET_SOCKET_SEL_READ, ulTimeout) > 0) {
        int recvlen = ( int )recv(nTcpSocket, pBuf, nLen, 0);
        return recvlen;
    }
    return 0;
}

int RecvTcpDataEx(int nTcpSocket, char* pBuf, int nLen)
{
    return ( int )recv(nTcpSocket, pBuf, nLen, 0);
}

}  // namespace net