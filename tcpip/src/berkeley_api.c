/*******************************************************************************
  Berkeley Socket Distribution API Source File

  Summary:
    
  Description:
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_BERKELEY

#include "tcpip/src/tcpip_private.h"
#include "net_pres/pres/net_pres_socketapi.h"

#if defined(TCPIP_STACK_USE_BERKELEY_API)

#ifdef __ICCARM__
__attribute__((section(".bss.errno"))) int errno = 0;           // initialization required to provide definition
#include "toolchain_specifics.h"                                // extended E codes not provided in IAR errno.h
#else
#include <errno.h>
#if (__XC32_VERSION < 4000) || (__XC32_VERSION == 243739000)
// xc32 versions >= v4.0 no longer have sys/errno.h 
#include <sys/errno.h>
#endif
#endif


static bool TCP_SocketWasReset(SOCKET s);
static bool TCP_SocketWasDisconnected(SOCKET s, bool cliAbort);

static void TCP_SignalFunction(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param);

static int _BSD_SetIp4AddrInfo(uint32_t ipAddr, const struct addrinfo* hints, struct addrinfo** res);
static int _BSD_SetIp6AddrInfo(const IPV6_ADDR* ipAddr, const struct addrinfo* hints, struct addrinfo** res);

static int _BSD_StartAddrInfo(const char* node, const struct addrinfo* hints, struct addrinfo** res, TCPIP_DNS_RESOLVE_TYPE dnstype);
static int _BSD_CheckAddrInfo(const char* node, TCPIP_DNS_RESOLVE_TYPE dnstype);

#if !defined(MAX_BSD_SOCKETS)
#define MAX_BSD_SOCKETS 4
#endif


// Array of BSDSocket elements; used to track all socket state and connection information.
static const void*  bsdApiHeapH = 0;                    // memory allocation handle
static struct BSDSocket  * BSDSocketArray = NULL;
static uint8_t BSD_SOCKET_COUNT = MAX_BSD_SOCKETS;

// The initialization count, so we know how many interfaces are up.
static int InitCount = 0;

static OSAL_SEM_HANDLE_TYPE bsdSemaphore;
static tcpipSignalHandle    bsdSignalHandle;

// validates the socket and returns the pointer to the internal BSDSocket
// returns 0 if error
struct BSDSocket* _getBsdSocket(SOCKET s)
{
    if(s >= 0 && s < BSD_SOCKET_COUNT)
    {
        return BSDSocketArray + s;
    }

    return 0;
}

void _cfgBsdSocket(struct BSDSocket * socketInfo)
{
    if (socketInfo->SocketID == NET_PRES_INVALID_SOCKET)
    {
        return;
    }

    socketInfo->nativeSkt = NET_PRES_SocketGetTransportHandle(socketInfo->SocketID);            
    if (socketInfo->SocketType == SOCK_DGRAM)
    {
        if (socketInfo->sndBufSize)
        {
            NET_PRES_SocketOptionsSet(socketInfo->SocketID, UDP_OPTION_TX_BUFF, (void*)socketInfo->sndBufSize);
        }
        if(socketInfo->udpBcastEnabled != 0)
        {
            TCPIP_UDP_BcastIPV4AddressSet(socketInfo->nativeSkt, UDP_BCAST_NETWORK_LIMITED, 0);
        }

    }
    else if (socketInfo->SocketType == SOCK_STREAM)
    {
        TCP_OPTION_LINGER_DATA lingerData;
        lingerData.lingerEnable = socketInfo->tcpLinger;
        lingerData.lingerTmo = socketInfo->lingerTmo;
        lingerData.gracefulEnable = socketInfo->tcpGracefulDisable == 0;

        NET_PRES_SocketOptionsSet(socketInfo->SocketID, TCP_OPTION_LINGER, (void*)&lingerData);

        if (socketInfo->sndBufSize)
        {
            NET_PRES_SocketOptionsSet(socketInfo->SocketID, TCP_OPTION_TX_BUFF, (void*)socketInfo->sndBufSize);
        }
        if (socketInfo->rcvBufSize)
        {
            NET_PRES_SocketOptionsSet(socketInfo->SocketID, TCP_OPTION_RX_BUFF, (void*)socketInfo->rcvBufSize);
        }
        NET_PRES_SocketOptionsSet(socketInfo->SocketID, TCP_OPTION_NODELAY, (void*)(socketInfo->tcpNoDelay ? 0xffffffff : 0));

        if(socketInfo->needsSignal)
        {
            NET_PRES_SocketSignalHandlerRegister(socketInfo->SocketID, TCPIP_TCP_SIGNAL_RX_FIN | TCPIP_TCP_SIGNAL_RX_RST | TCPIP_TCP_SIGNAL_TX_RST, TCP_SignalFunction, socketInfo);
        }
    }

}

/*****************************************************************************
  Function:
	void BerkeleySocketInit(const TCPIP_STACK_MODULE_CTRL* const stackData,
                        const BERKELEY_MODULE_GONFIG* berkeleyData)

  Summary:
	Initializes the Berkeley socket structure array.

  Description:
	This function initializes the Berkeley socket array. This function should
	be called before any BSD socket call.

  Precondition:
	None.

  Parameters:
	None.

  Returns:
	None

  Remarks:
	None.
  ***************************************************************************/
bool BerkeleySocketInit(const TCPIP_STACK_MODULE_CTRL* const stackData,
                        const BERKELEY_MODULE_CONFIG* berkeleyData)
{
    unsigned int s;
    struct BSDSocket *socket;
    // OSAL_CRITSECT_DATA_TYPE intStatus;

    { // This needs hard protection  An Automutext would be best here
        if (InitCount++)
        {
            return true;
        }
        if(OSAL_SEM_Create(&bsdSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_TRUE)
        {
            InitCount--;
            return false;
        }
    }

    bsdSignalHandle = 0;
    if (berkeleyData != NULL)
    {
        BSD_SOCKET_COUNT = berkeleyData->maxSockets;
    }
    else
    {
        BSD_SOCKET_COUNT = MAX_BSD_SOCKETS;
    }
    bsdApiHeapH = stackData->memH;


    BSDSocketArray = TCPIP_HEAP_Calloc(bsdApiHeapH, BSD_SOCKET_COUNT, sizeof (struct BSDSocket));
    if (BSDSocketArray == NULL)
    {
        if (OSAL_SEM_Delete(&bsdSemaphore) != OSAL_RESULT_TRUE)
        {
            // SYS_DEBUG message
        }
        InitCount--;
        return false;
    }
    
    bsdSignalHandle  =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_BSD_Task, 0);
    if(bsdSignalHandle == 0)
    {   // failed
        TCPIP_HEAP_Free(bsdApiHeapH, BSDSocketArray);
        OSAL_SEM_Delete(&bsdSemaphore);
        return false;
    }

    for (s = 0; s < BSD_SOCKET_COUNT; s++)
    {
        socket = (struct BSDSocket *) &BSDSocketArray[s];
        socket->bsdState = SKT_CLOSED;
        socket->SocketID = NET_PRES_INVALID_SOCKET;
        socket->rcvBufSize = 0;
        socket->sndBufSize = 0;
        socket->rcvTmOut = 0;
        socket->sndTmOut = 0;
        socket->lingerTmo = 0;

        socket->w = 0;
    }

    return true;
}

/*****************************************************************************
  Function:
	void BerkeleySocketDeinit(void)

  Summary:
	De-Initializes the Berkeley socket structure array.

  Description:
	This function deinitializes the Berkeley socket array. This function should
	be called when closing out any BSD socket call.

  Precondition:
	None.

  Parameters:
	None.

  Returns:
	None

  Remarks:
	None.
  ***************************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void BerkeleySocketDeInit(const TCPIP_STACK_MODULE_CTRL* const stackData)
{
    uint8_t s;
    struct BSDSocket *socket;

    if (InitCount == 0 || --InitCount)
    {
        return;
    }

    socket = BSDSocketArray;
    for (s = 0; s < BSD_SOCKET_COUNT; s++, socket++)
    {
        if (socket->SocketID == NET_PRES_INVALID_SOCKET)
        {
            continue;
        }
        closesocket(s);  // Too deep to lock fully
        socket->bsdState = SKT_CLOSED;
        socket->SocketID = NET_PRES_INVALID_SOCKET;
    }

    _TCPIPStackSignalHandlerDeregister(bsdSignalHandle);
    bsdSignalHandle = 0;
    TCPIP_HEAP_Free(bsdApiHeapH, BSDSocketArray);

    if (OSAL_SEM_Delete(&bsdSemaphore) != OSAL_RESULT_TRUE)
    {
        // SYS_DEBUG message
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_BSD_Task(void)
{
    // received an asynchronous message
    // check for the sockets that need closing
    int ix;
    int asyncSignals = 0;
    struct BSDSocket* pBSkt = BSDSocketArray;
    for(ix = 0; ix < BSD_SOCKET_COUNT; ix++, pBSkt++)
    {
        if(pBSkt->bsdState == SKT_DISCONNECTING && pBSkt->needsClose == true)
        {
            NET_PRES_SocketClose(pBSkt->SocketID);
            pBSkt->bsdState = SKT_CLOSED;
            pBSkt->SocketID = NET_PRES_INVALID_SOCKET;
            asyncSignals++;
        }
    }

    while(asyncSignals--)
    {   // clear the signals
        _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC); 
    }


}

/*****************************************************************************
Function:
    SOCKET socket( int af, int type, int protocol )

Summary:
    This function creates a new Berkeley socket.

Description:
    This function creates a new BSD socket for the microchip
    TCPIP stack. The return socket descriptor is used for the subsequent
    BSD operations.

Precondition:
    BerkeleySocketInit function should be called.

Parameters:
    af - address family - AF_INET.
    type - socket type SOCK_DGRAM or SOCK_STREAM.
    protocol - IP protocol IPPROTO_UDP or IPPROTO_TCP.

Returns:
    New socket descriptor. SOCKET_ERROR in case of error.
(and errno set accordingly).

Remarks:
    None.
 ***************************************************************************/
SOCKET
socket (int af, int type, int protocol)
{
  struct BSDSocket *socket = BSDSocketArray;
    SOCKET s;

    if (af != AF_INET && af != AF_INET6)
    {
        errno = EAFNOSUPPORT;
        return SOCKET_ERROR;
    }

    if (protocol == IPPROTO_IP)
    {
        switch (type)
        {
        case SOCK_DGRAM:
            protocol = IPPROTO_UDP;
            break;

        case SOCK_STREAM:
            protocol = IPPROTO_TCP;
            break;

        default:
            break;
        }
    }

    if (OSAL_SEM_Pend(&bsdSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        // SYS_DEBUG message
    }

    for (s = 0; s < BSD_SOCKET_COUNT; s++, socket++)
    {
        if (socket->bsdState != SKT_CLOSED) //socket in use
            continue;

        socket->SocketType = type;
        socket->localIP = IP_ADDR_ANY; // updated by bind()
#if defined(TCPIP_STACK_USE_IPV6)
        socket->addressFamily = af;
#endif

        if (type == SOCK_DGRAM && protocol == IPPROTO_UDP)
        {
            socket->bsdState = SKT_CREATED;
            if (OSAL_SEM_Post(&bsdSemaphore) != OSAL_RESULT_TRUE)
            {
                // SYS_DEBUG message
            }
            return s;
        }
        else if (type == SOCK_STREAM && protocol == IPPROTO_TCP)
        {
            socket->bsdState = SKT_CREATED;
            if (OSAL_SEM_Post(&bsdSemaphore) != OSAL_RESULT_TRUE)
            {
                // SYS_DEBUG message
            }
            return s;
        }
        else
        {
            errno = EINVAL;
            if (OSAL_SEM_Post(&bsdSemaphore) != OSAL_RESULT_TRUE)
            {
                // SYS_DEBUG message
            }
            return SOCKET_ERROR;
        }
    }

    errno = EMFILE;
    if (OSAL_SEM_Post(&bsdSemaphore) != OSAL_RESULT_TRUE)
    {
        // SYS_DEBUG message
    }
    return SOCKET_ERROR;
}

/*****************************************************************************
  Function:
	int bind( SOCKET s, const struct sockaddr* name, int namelen )

  Summary:
	This function assigns a name to the socket descriptor.

  Description:
	The bind function assigns a name to an unnamed socket. The
    name represents the local address of the communication
    endpoint. For sockets of type SOCK_STREAM, the name of the
    remote endpoint is assigned when a connect or accept function
    is executed.

  Precondition:
	socket function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	name - pointer to the sockaddr structure containing the
	local address of the socket.
	namelen - length of the sockaddr structure.

  Returns:
	If bind is successful, a value of 0 is returned. A return
    value of SOCKET_ERROR indicates an error.
    (and errno set accordingly).

  Remarks:
	None.
  ***************************************************************************/
int bind( SOCKET s, const struct sockaddr* name, int namelen )
{
    uint16_t lPort;
    IPV4_ADDR lAddr;
    lAddr.Val = 0;
#if defined(TCPIP_STACK_USE_IPV6)
    IPV6_ADDR lAddr6;
    lAddr6.d[0] = 0;
    lAddr6.d[1] = 0;
    lAddr6.d[2] = 0;
    lAddr6.d[3] = 0;
#endif

	struct BSDSocket *socket = _getBsdSocket(s);

    if (socket == 0)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    if (socket->bsdState != SKT_CREATED) //only work with recently created socket
    {   
        errno = EINVAL;
        return SOCKET_ERROR;
    }

    if ((unsigned int) namelen < sizeof (struct sockaddr_in)) {
        errno = EFAULT;
        return SOCKET_ERROR;
    }

#if defined(TCPIP_STACK_USE_IPV6)
    if (socket->addressFamily == AF_INET)
#endif
    {
        struct sockaddr_in *local_addr;
        local_addr = (struct sockaddr_in *) name;
        lAddr.Val = local_addr->sin_addr.S_un.S_addr == IP_ADDR_ANY ? 0 : local_addr->sin_addr.S_un.S_addr;

        lPort = local_addr->sin_port;
    }
#if defined(TCPIP_STACK_USE_IPV6)
    else
    {
        struct sockaddr_in6 *local_addr = (struct sockaddr_in6*) name;
        struct  in6_addr* sin6_addr = (struct  in6_addr*)((uint8_t*)local_addr + offsetof(struct sockaddr_in6, sin6_addr));
        memcpy(&lAddr6, sin6_addr->in6_u.u6_addr8, sizeof(struct in6_addr));

        if (lAddr6.d[0] == IP_ADDR_ANY)
        {
            lAddr6.d[0] = 0;
        }
        lPort = local_addr->sin6_port;
    }
#endif
    if (socket->SocketType == SOCK_DGRAM)
    {
        if (socket->SocketID == INVALID_SOCKET)
        { // create server socket

#if defined(TCPIP_STACK_USE_IPV6)
            if (socket->addressFamily == AF_INET)
            {
#endif
                socket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_DATAGRAM_SERVER, IP_ADDRESS_TYPE_IPV4, lPort, (NET_PRES_ADDRESS *)(lAddr.Val == 0 ? 0 : (IP_MULTI_ADDRESS*) & lAddr), NULL);
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                socket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_DATAGRAM_SERVER, IP_ADDRESS_TYPE_IPV6, lPort, (NET_PRES_ADDRESS *)(lAddr6.d[0] == 0 ? 0 : (IP_MULTI_ADDRESS*) & lAddr6), NULL);
            }
#endif
            if (socket->SocketID == INVALID_UDP_SOCKET)
            {
                errno = ENOBUFS;
                return SOCKET_ERROR;
            }
            _cfgBsdSocket(socket);

        } 
#if defined(TCPIP_STACK_USE_IPV6)
        if (socket->addressFamily == AF_INET)
        {
#endif
            if (!NET_PRES_SocketBind(socket->SocketID, IP_ADDRESS_TYPE_IPV4, lPort, (NET_PRES_ADDRESS *)((((struct sockaddr_in *) name)->sin_addr.S_un.S_addr == IP_ADDR_ANY) ? 0 : (IP_MULTI_ADDRESS*) & lAddr)))
            {
                errno = EINVAL;
                return SOCKET_ERROR;
            }
#if defined(TCPIP_STACK_USE_IPV6)
        }
        else
        {
            if (!NET_PRES_SocketBind(socket->SocketID, IP_ADDRESS_TYPE_IPV6, lPort, (NET_PRES_ADDRESS*) & lAddr6))
            {
                errno = EINVAL;
                return SOCKET_ERROR;
            }
        }
#endif
    }

    socket->localPort = lPort;
#if defined(TCPIP_STACK_USE_IPV6)
            if (socket->addressFamily == AF_INET)
            {
#endif
                socket->localIP = lAddr.Val;
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                socket->localIP = lAddr6.d[0];
                socket->localIPv6[0] = lAddr6.d[1];
                socket->localIPv6[1] = lAddr6.d[2];
                socket->localIPv6[2] = lAddr6.d[3];
            }
#endif
    socket->bsdState = SKT_BOUND;
    return 0; //success
}

/*****************************************************************************
  Function:
	int listen( SOCKET s, int backlog )

  Summary:
	The listen function sets the specified socket in a listen mode

  Description:
	This function sets the specified socket in a listen
	mode. Calling the listen function indicates that the
	application is ready to accept connection requests arriving
	at a socket of type SOCK_STREAM. The connection request is
	queued (if possible) until accepted with an accept function.
	The backlog parameter defines the maximum number of pending
	connections that may be queued.

  Precondition:
	bind() must have been called on the s socket first.

  Parameters:
	s - Socket identifier returned from a prior socket() call.
	backlog - Maximum number of connection requests that can be queued.  Note 
		that each backlog requires a TCP socket to be allocated.
	
  Returns:
	Returns 0 on success, else return SOCKET_ERROR.
    (and errno set accordingly).

  Remarks:
	None
  ***************************************************************************/
int listen( SOCKET s, int backlog ) 
{
    struct BSDSocket *ps, *cs;
    SOCKET clientSockID;
    unsigned int socketcount;
    unsigned char assigned;
    IPV4_ADDR lclAddr;
    lclAddr.Val = 0;
#if defined(TCPIP_STACK_USE_IPV6)
    IPV6_ADDR lclAddr6;
    lclAddr6.d[0] = 0;
    lclAddr6.d[1] = 0;
    lclAddr6.d[2] = 0;
    lclAddr6.d[3] = 0;
#endif
    
    ps = _getBsdSocket(s);
    if(ps == 0)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    if (ps->SocketType != SOCK_STREAM)
    {
        errno = EOPNOTSUPP;
        return SOCKET_ERROR;
    }

    if (ps->bsdState == SKT_BSD_LISTEN)
        backlog = ps->backlog;

    if ((ps->bsdState != SKT_BOUND) && (ps->bsdState != SKT_BSD_LISTEN))
    {
        errno = EINVAL;
        return SOCKET_ERROR;
    }

    while (backlog--)
    {
        assigned = 0;
        for (socketcount = 0; socketcount < BSD_SOCKET_COUNT; socketcount++)
        {
            if (BSDSocketArray[socketcount].bsdState != SKT_CLOSED)
                continue;

#if defined(TCPIP_STACK_USE_IPV6)
            if (ps->addressFamily == AF_INET)
            {
#endif
                if (ps->localIP)
                {
                    lclAddr.Val = ps->localIP;
                }
                clientSockID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_SERVER, IP_ADDRESS_TYPE_IPV4, ps->localPort, (NET_PRES_ADDRESS *)(lclAddr.Val == 0 ? 0 : (IP_MULTI_ADDRESS*) & lclAddr), 0);
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                if (ps->localIP)
                {
                    lclAddr6.d[0] = ps->localIP;
                    lclAddr6.d[1] = ps->localIPv6[0];
                    lclAddr6.d[2] = ps->localIPv6[1];
                    lclAddr6.d[3] = ps->localIPv6[2];
                }
                clientSockID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_SERVER, IP_ADDRESS_TYPE_IPV6, ps->localPort, (NET_PRES_ADDRESS*)(lclAddr6.d[0] == 0 ? 0 : (IP_MULTI_ADDRESS*) & lclAddr6), 0);
            }
#endif

            if (clientSockID == INVALID_SOCKET)
            {
                errno = ENOBUFS;
                return SOCKET_ERROR;
            }

            cs = BSDSocketArray + socketcount;
            cs->SocketID = clientSockID;
            cs->bsdState = SKT_LISTEN;
            cs->isServer = true;
            cs->needsSignal = true;
            cs->localPort = ps->localPort;
            cs->SocketType = SOCK_STREAM;
            cs->localIP = ps->localIP;
            cs->parentId = s; 

            _cfgBsdSocket(cs);


            // Clear the first reset flag
            NET_PRES_SocketWasReset(clientSockID);

            assigned = 1;
            ps->bsdState = SKT_BSD_LISTEN;
            ps->backlog = backlog;

            break;
        }
        if (!assigned)
        {
            errno = EMFILE;
            return SOCKET_ERROR;
        }
    }
    return 0; //Success
}


/*****************************************************************************
  Function:
	SOCKET accept(SOCKET s, struct sockaddr* addr, int* addrlen)

  Summary:
	This function accepts connection requests queued for a listening socket.

  Description:
	The accept function is used to accept connection requests
	queued for a listening socket. If a connection request is
	pending, accept removes the request from the queue, and a new
	socket is created for the connection. The original listening
	socket remains open and continues to queue new connection
	requests. The socket must be a SOCK_STREAM type socket.

  Precondition:
	listen function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to
	socket. must be bound to a local name and in listening mode.
	addr - Optional pointer to a buffer that receives the address
	of the connecting entity.
	addrlen - Optional pointer to an integer that contains the
	length of the address addr

  Returns:
	If the accept function succeeds, it returns a non-negative
	integer that is a descriptor for the accepted socket.
	Otherwise, the value SOCKET_ERROR is returned.
    (and errno set accordingly).

  Remarks:
	None.
  ***************************************************************************/
SOCKET accept(SOCKET s, struct sockaddr* addr, int* addrlen)
{
    struct BSDSocket *pListenSock;
    TCP_SOCKET_INFO tcpSockInfo;
    unsigned int sockCount;
    TCP_SOCKET hTCP;

    pListenSock = _getBsdSocket(s); /* Get the pointer to listening server socket */

    if (pListenSock == 0)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    if (pListenSock->bsdState != SKT_BSD_LISTEN)
    {
        errno = EINVAL;
        return SOCKET_ERROR;
    }
    if (pListenSock->SocketType != SOCK_STREAM)
    {
        errno = EOPNOTSUPP;
        return SOCKET_ERROR;
    }

    for (sockCount = 0; sockCount < BSD_SOCKET_COUNT; sockCount++)
    {
        if (BSDSocketArray[sockCount].bsdState != SKT_LISTEN)
            continue;

        if (BSDSocketArray[sockCount].localPort != pListenSock->localPort)
            continue;

        hTCP = BSDSocketArray[sockCount].SocketID;

        // We don't care about connections and disconnections before we can
        // process them, so clear the reset flag
        NET_PRES_SocketWasReset(hTCP);

        if (NET_PRES_SocketIsConnected(hTCP))
        {
            NET_PRES_SocketInfoGet(hTCP, &tcpSockInfo);
            if (addr)
            {
                if (addrlen)
                {
#if defined(TCPIP_STACK_USE_IPV6)
                    if (pListenSock->addressFamily == AF_INET)
                    {
#endif
                        struct sockaddr_in *addrRemote;
                        if ((unsigned int) *addrlen < sizeof (struct sockaddr_in))
                        {
                            errno = EFAULT;
                            return SOCKET_ERROR;
                        }
                        addrRemote = (struct sockaddr_in *) addr;
                        addrRemote->sin_addr.S_un.S_addr = tcpSockInfo.remoteIPaddress.v4Add.Val == IP_ADDR_ANY ? 0: tcpSockInfo.remoteIPaddress.v4Add.Val;
                        addrRemote->sin_port = tcpSockInfo.remotePort;
                        *addrlen = sizeof (struct sockaddr_in);
#if defined(TCPIP_STACK_USE_IPV6)
                    }
                    else
                    {
                        struct sockaddr_in6 *addrRemote;
                        if ((unsigned int) *addrlen < sizeof (struct sockaddr_in6))
                        {
                            errno = EFAULT;
                            return SOCKET_ERROR;
                        }
                        addrRemote = (struct sockaddr_in6*) addr;
                        addrRemote->sin6_addr.in6_u.u6_addr8[0] = tcpSockInfo.remoteIPaddress.v6Add.d[0];
                        addrRemote->sin6_addr.in6_u.u6_addr8[1] = tcpSockInfo.remoteIPaddress.v6Add.d[1];
                        addrRemote->sin6_addr.in6_u.u6_addr8[2] = tcpSockInfo.remoteIPaddress.v6Add.d[2];
                        addrRemote->sin6_addr.in6_u.u6_addr8[3] = tcpSockInfo.remoteIPaddress.v6Add.d[3];
                        addrRemote->sin6_port = tcpSockInfo.remotePort;
                        *addrlen = sizeof (struct sockaddr_in6);
                    }
#endif
                }
            }
            BSDSocketArray[sockCount].remotePort = tcpSockInfo.remotePort;
            BSDSocketArray[sockCount].remoteIP = tcpSockInfo.remoteIPaddress.v4Add.Val;
            BSDSocketArray[sockCount].bsdState = SKT_EST;
            return sockCount;
        }
    }

    errno = EMFILE;
    return SOCKET_ERROR;
}

/*****************************************************************************
  Function:
	int connect( SOCKET s, struct sockaddr* name, int namelen )

  Summary:
	This function connects to the peer communications end point.

  Description:
	The connect function assigns the address of the peer
	communications endpoint. For stream sockets, connection is
	established between the endpoints. For datagram sockets, an
	address filter is established between the endpoints until
	changed with another connect() function.

  Precondition:
	socket function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	name - pointer to the sockaddr structure containing the
	peer address and port number.
	namelen - length of the sockaddr structure.

  Returns:
	If the connect() function succeeds, it returns 0. Otherwise,
	the value SOCKET_ERROR is returned to indicate an error
	condition (and errno set accordingly).
    For stream based socket, if the connection is not
	established yet, connect returns SOCKET_ERROR and
    errno = EINPROGRESS.

  Remarks:
	None.
  ***************************************************************************/
int connect( SOCKET s, struct sockaddr* name, int namelen ) 
{
    struct sockaddr_in *addr;
    uint32_t remoteIP = 0;
    uint16_t remotePort = 0;    
    IPV4_ADDR localAddr;
    IPV4_ADDR remoteAddr;
    localAddr.Val = 0;
    remoteAddr.Val = 0;
#if defined(TCPIP_STACK_USE_IPV6)
    IPV6_ADDR localAddr6;
    localAddr6.d[0] = 0;
    localAddr6.d[1] = 0;
    localAddr6.d[2] = 0;
    localAddr6.d[3] = 0;
    IPV6_ADDR remoteIP6;
    remoteIP6.d[0] = 0;
    remoteIP6.d[1] = 0;
    remoteIP6.d[2] = 0;
    remoteIP6.d[3] = 0;
    struct sockaddr_in6 * addr6;
#endif

	struct BSDSocket *socket = _getBsdSocket(s);
    if (socket == 0 || socket->bsdState < SKT_CREATED) 
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    if ((unsigned int) namelen < sizeof (struct sockaddr_in)) 
    {
        errno = EFAULT;
        return SOCKET_ERROR;
    }

#if defined(TCPIP_STACK_USE_IPV6)
    if (socket->addressFamily == AF_INET)
    {
#endif
        addr = (struct sockaddr_in *) name;
        remotePort = addr->sin_port;
        remoteIP = addr->sin_addr.S_un.S_addr == IP_ADDR_ANY ? 0: addr->sin_addr.S_un.S_addr;
#if defined(TCPIP_STACK_USE_IPV6)
    }
    else
    {
        addr6 = (struct sockaddr_in6 *) name;
        remotePort = addr6->sin6_port;
        struct  in6_addr* sin6_addr = (struct  in6_addr*)((uint8_t*)addr6 + offsetof(struct sockaddr_in6, sin6_addr));
        memcpy(remoteIP6.d, sin6_addr->in6_u.u6_addr8, sizeof(IPV6_ADDR));
        localAddr6.d[0] = socket->localIP;
        localAddr6.d[1] = socket->localIPv6[0];
        localAddr6.d[2] = socket->localIPv6[1];
        localAddr6.d[3] = socket->localIPv6[2];
    }
#endif

#if defined(TCPIP_STACK_USE_IPV6)
    if (socket->addressFamily == AF_INET)
    {
#endif
        if (remoteIP == 0u || remotePort == 0u)
        {
            errno = EINVAL;
            return SOCKET_ERROR;
        }
#if defined(TCPIP_STACK_USE_IPV6)
    }
    else
    {
        if (remoteIP6.d[0] == 0u || remotePort == 0u)
        {
            errno = EINVAL;
            return SOCKET_ERROR;
        }
    }
#endif
    if (socket->SocketType == SOCK_STREAM) 
    {
        switch (socket->bsdState) 
        {
            case SKT_EST:
                return 0; // already established

            case SKT_IN_PROGRESS:
                if (TCP_SocketWasReset(s)) 
                {
                    errno = ECONNREFUSED;
                    return SOCKET_ERROR;
                }
                else if (TCP_SocketWasDisconnected(s, true))
                {
                    errno = ECONNREFUSED;
                    return SOCKET_ERROR;
                }

                if (!NET_PRES_SocketIsConnected(socket->SocketID)) 
                {
                    errno = EINPROGRESS;
                    return SOCKET_ERROR;
                }

                socket->bsdState = SKT_EST;
                return 0; //success

            case SKT_CREATED:
            case SKT_BOUND:
#if defined(TCPIP_STACK_USE_IPV6)
                if (socket->addressFamily == AF_INET)
                {
#endif
                    if (socket->localIP == IP_ADDR_ANY)
                    {
                        socket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_CLIENT, IP_ADDRESS_TYPE_IPV4, remotePort, (NET_PRES_ADDRESS *)&remoteIP, 0);
                        if(socket->SocketID == INVALID_SOCKET)
                        {
                            errno = ENOBUFS;
                            return SOCKET_ERROR;
                        }
                    }
                    else
                    {
                        socket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_CLIENT, IP_ADDRESS_TYPE_IPV4, 0, 0, 0);
                        if(socket->SocketID == INVALID_SOCKET)
                        {
                            errno = ENOBUFS;
                            return SOCKET_ERROR;
                        }
                        if (!NET_PRES_SocketBind(socket->SocketID, IP_ADDRESS_TYPE_IPV4, 0, (NET_PRES_ADDRESS *)&socket->localIP))
                        {
                            errno = EADDRINUSE;
                            NET_PRES_SocketClose(socket->SocketID);
                            socket->SocketID = INVALID_SOCKET;
                            return SOCKET_ERROR;
                        }
                        if (!NET_PRES_SocketRemoteBind(socket->SocketID, IP_ADDRESS_TYPE_IPV4, remotePort, (NET_PRES_ADDRESS *)&remoteIP))
                        {
                            errno = EADDRINUSE;
                            NET_PRES_SocketClose(socket->SocketID);
                            socket->SocketID = INVALID_SOCKET;
                            return SOCKET_ERROR;
                        }
                        if (!NET_PRES_SocketConnect(socket->SocketID))
                        {
                            errno = EADDRINUSE;
                            NET_PRES_SocketClose(socket->SocketID);
                            socket->SocketID = INVALID_SOCKET;
                            return SOCKET_ERROR;
                        }
                    }
#if defined(TCPIP_STACK_USE_IPV6)
                }
                else
                {
                    if (socket->localIP == IP_ADDR_ANY)
                    {
                        socket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_CLIENT, IP_ADDRESS_TYPE_IPV6, remotePort, (NET_PRES_ADDRESS*)&remoteIP6, 0);
                        if(socket->SocketID == INVALID_SOCKET)
                        {
                            errno = ENOBUFS;
                            return SOCKET_ERROR;
                        }
                    }
                    else
                    {
                        socket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_CLIENT, IP_ADDRESS_TYPE_IPV6, 0, 0, 0);
                        if(socket->SocketID == INVALID_SOCKET)
                        {
                            errno = ENOBUFS;
                            return SOCKET_ERROR;
                        }
                        if (!NET_PRES_SocketBind(socket->SocketID, IP_ADDRESS_TYPE_IPV6, 0, (NET_PRES_ADDRESS*)&localAddr6))
                        {
                            errno = EADDRINUSE;
                            NET_PRES_SocketClose(socket->SocketID);
                            socket->SocketID = INVALID_SOCKET;
                            return SOCKET_ERROR;
                        }
                        if (!NET_PRES_SocketRemoteBind(socket->SocketID, IP_ADDRESS_TYPE_IPV6, remotePort, (NET_PRES_ADDRESS*)&remoteIP))
                        {
                            errno = EADDRINUSE;
                            NET_PRES_SocketClose(socket->SocketID);
                            socket->SocketID = INVALID_SOCKET;
                            return SOCKET_ERROR;
                        }
                        if (!NET_PRES_SocketConnect(socket->SocketID))
                        {
                            errno = EADDRINUSE;
                            NET_PRES_SocketClose(socket->SocketID);
                            socket->SocketID = INVALID_SOCKET;
                            return SOCKET_ERROR;
                        }
                    }

                }
#endif

                if (socket->SocketID == INVALID_SOCKET) 
                {
                    errno = ENOBUFS;
                    return SOCKET_ERROR;
                }
                _cfgBsdSocket(socket);
                // Clear the first reset flag
                NET_PRES_SocketWasReset(socket->SocketID);

#if defined(TCPIP_STACK_USE_IPV6)
                if (socket->addressFamily == AF_INET)
                {
#endif
                    localAddr.Val  = socket->localIP == IP_ADDR_ANY ? 0 : socket->localIP;
                    remoteAddr.Val = remoteIP;
                    TCPIP_TCP_SocketNetSet(socket->nativeSkt, TCPIP_IPV4_SelectSourceInterface(0, &remoteAddr, &localAddr, true), false);
#if defined(TCPIP_STACK_USE_IPV6)
                }
#endif
                socket->isServer = false;
                socket->bsdState = SKT_IN_PROGRESS;
                errno = EINPROGRESS;
                return SOCKET_ERROR;

            default:
                errno = ECONNRESET;
                return SOCKET_ERROR;
        }
    } 
    else 
    {
        // open the socket
        if (socket->bsdState == SKT_CREATED) 
        {
#if defined(TCPIP_STACK_USE_IPV6)
            if (socket->addressFamily == AF_INET)
            {
#endif
                socket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_DATAGRAM_CLIENT, IP_ADDRESS_TYPE_IPV4, remotePort, (NET_PRES_ADDRESS *) & remoteIP, 0);
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                socket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_DATAGRAM_CLIENT, IP_ADDRESS_TYPE_IPV6, remotePort, (NET_PRES_ADDRESS*) & remoteIP6, 0);
            }
#endif
            if (socket->SocketID == INVALID_UDP_SOCKET) 
            {
                errno = ENOBUFS;
                return SOCKET_ERROR;
            }
            _cfgBsdSocket(socket);
            socket->bsdState = SKT_BOUND;
        }
        if (socket->bsdState != SKT_BOUND) 
        {
            errno = EINVAL;
            return SOCKET_ERROR;
        }

        // UDP: remote port is used as a filter. Need to call connect when using
        // send/recv calls. No need to call 'connect' if using sendto/recvfrom
        // calls.
        socket->remotePort = remotePort;
#if defined(TCPIP_STACK_USE_IPV6)
        if (socket->addressFamily == AF_INET)
        {
#endif
            socket->remoteIP = remoteIP;
#if defined(TCPIP_STACK_USE_IPV6)
        }
        else
        {
            socket->remoteIP = remoteIP6.d[0];
            socket->remoteIPv6[0] = remoteIP6.d[1];
            socket->remoteIPv6[1] = remoteIP6.d[2];
            socket->remoteIPv6[2] = remoteIP6.d[3];
        }
#endif
        return 0; //success
    }

}

/*****************************************************************************
  Function:
	int send( SOCKET s, const char* buf, int len, int flags )
	
  Summary:
	The send function is used to send outgoing data on an already
	connected socket.

  Description:
	The send function is used to send outgoing data on an already
	connected socket. This function is used to send a reliable,
	ordered stream of data bytes on a socket of type SOCK_STREAM
	but can also be used to send datagrams on a socket of type SOCK_DGRAM.

  Precondition:
	connect function should be called for TCP and UDP sockets.
	Server side, accept function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	buf - application data buffer containing data to transmit.
	len - length of data in bytes.
	flags - message flags. Currently this field is not supported.

  Returns:
	On success, send returns number of bytes sent.
    In case of error it returns SOCKET_ERROR 
    (and errno set accordingly).

  Remarks:
	None.
  ***************************************************************************/
int send( SOCKET s, const char* buf, int len, int flags )
{
	return sendto(s, buf, len, flags, NULL, 0);
}

/*****************************************************************************
  Function:
	int sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)

  Summary:
	This function used to send the data for both connection oriented and connection-less
	sockets.

  Description:
	The sendto function is used to send outgoing data on a socket.
	The destination address is given by to and tolen. Both 
	Datagram and stream sockets are supported.

  Precondition:
	socket function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	buf - application data buffer containing data to transmit.
	len - length of data in bytes.
	flags - message flags. Currently this field is not supported.
	to - Optional pointer to the the sockaddr structure containing the
		destination address.  If NULL, the currently bound remote port and IP 
		address are used as the destination.
	tolen - length of the sockaddr structure.

  Returns:
	On success, sendto returns number of bytes sent. In case of
	error returns SOCKET_ERROR (and errno set accordingly).

  Remarks:
	None.
  ***************************************************************************/
int sendto( SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen )
{
    int size = SOCKET_ERROR;
    IPV4_ADDR remoteIp;
    remoteIp.Val = 0;
    uint16_t wRemotePort;
    struct sockaddr_in local; 
#if defined(TCPIP_STACK_USE_IPV6)
    IPV6_ADDR remoteIp6;
    remoteIp6.d[0] = 0;
    remoteIp6.d[1] = 0;
    remoteIp6.d[2] = 0;
    remoteIp6.d[3] = 0;
    struct sockaddr_in6 local6;
#endif

	struct BSDSocket *socket = _getBsdSocket(s);

    if (socket == 0 || socket->bsdState == SKT_CLOSED)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    // Handle special case were 0 return value is okay
    if (len == 0)
    {
        return 0;
    }

    if (socket->SocketType == SOCK_DGRAM) //UDP
    {
        // Decide the destination IP address and port
#if defined(TCPIP_STACK_USE_IPV6)
        if (socket->addressFamily == AF_INET)
        {
#endif
            remoteIp.Val = socket->remoteIP;
#if defined(TCPIP_STACK_USE_IPV6)
        }
        else
        {
            remoteIp6.d[0] = socket->remoteIP;
            remoteIp6.d[1] = socket->remoteIPv6[0];
            remoteIp6.d[2] = socket->remoteIPv6[1];
            remoteIp6.d[3] = socket->remoteIPv6[2];
        }
#endif
        wRemotePort = socket->remotePort;
        if (to)
        {
#if defined(TCPIP_STACK_USE_IPV6)
            if (socket->addressFamily == AF_INET)
            {
#endif
                if ((unsigned int) tolen < sizeof (struct sockaddr_in))
                {
                    errno = EFAULT;
                    return SOCKET_ERROR;
                }
                wRemotePort = ((struct sockaddr_in*) to)->sin_port;
                remoteIp.Val = ((struct sockaddr_in*) to)->sin_addr.s_addr;
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                if ((unsigned int) tolen < sizeof (struct sockaddr_in6))
                {
                    errno = EFAULT;
                    return SOCKET_ERROR;
                }
                struct sockaddr_in6* addr6 = (struct sockaddr_in6*)to;
                wRemotePort = addr6->sin6_port;
                struct  in6_addr* sin6_addr = (struct  in6_addr*)((uint8_t*)addr6 + offsetof(struct sockaddr_in6, sin6_addr));
                memcpy(remoteIp6.d, sin6_addr->in6_u.u6_addr8, sizeof(IPV6_ADDR));
            }
#endif

            // Implicitly bind the socket if it isn't already
            if (socket->bsdState == SKT_CREATED)
            {
#if defined(TCPIP_STACK_USE_IPV6)
                if (socket->addressFamily == AF_INET)
                {
#endif
                    memset(&local, 0, sizeof (local));
                    local.sin_addr.s_addr = IP_ADDR_ANY;
                    if (bind(s, (struct sockaddr*) &local, sizeof (local)) == SOCKET_ERROR)
                        return SOCKET_ERROR;
#if defined(TCPIP_STACK_USE_IPV6)
                }
                else
                {
                    memset(&local6, 0x00, sizeof(local6));
                    if (bind(s, (struct sockaddr*) &local6, sizeof (local6)) == SOCKET_ERROR)
                        return SOCKET_ERROR;
                }
#endif
            }
        }

        UDP_SOCKET_INFO udpSockInfo;
        NET_PRES_SocketInfoGet(socket->SocketID, &udpSockInfo);
        UDP_SOCKET uSkt = socket->nativeSkt;

#if defined(TCPIP_STACK_USE_IPV6)
        if (socket->addressFamily == AF_INET)
        {
#endif
            if (remoteIp.Val == IP_ADDR_ANY)
            {
                TCPIP_UDP_BcastIPV4AddressSet(uSkt, UDP_BCAST_NETWORK_LIMITED, 0);
            }
            else
            { // Set the remote IP and MAC address if it is different from what we already have stored in the UDP socket
                if (udpSockInfo.remoteIPaddress.v4Add.Val != remoteIp.Val)
                {
                    TCPIP_UDP_DestinationIPAddressSet(uSkt, IP_ADDRESS_TYPE_IPV4, (IP_MULTI_ADDRESS*) & remoteIp.Val);
                }
            }
            // Set the proper remote port
            TCPIP_UDP_DestinationPortSet(uSkt, wRemotePort);
#if defined(TCPIP_STACK_USE_IPV6)
        }
        else
        {
            if ((udpSockInfo.remoteIPaddress.v6Add.d[0] != remoteIp6.d[0]) ||
                (udpSockInfo.remoteIPaddress.v6Add.d[1] != remoteIp6.d[1]) ||
                (udpSockInfo.remoteIPaddress.v6Add.d[2] != remoteIp6.d[2]) ||
                (udpSockInfo.remoteIPaddress.v6Add.d[3] != remoteIp6.d[3]))
            {
                    TCPIP_UDP_DestinationIPAddressSet(uSkt, IP_ADDRESS_TYPE_IPV6, (IP_MULTI_ADDRESS*) & remoteIp6.d);

            }
            // Set the proper remote port
            TCPIP_UDP_DestinationPortSet(uSkt, wRemotePort);
        }
#endif
        // Select the UDP socket and see if we can write to it
        if (NET_PRES_SocketWriteIsReady(socket->SocketID, len, 0))
        {
            // Write data and send UDP datagram
            size = NET_PRES_SocketWrite(socket->SocketID, (uint8_t*) buf, len);
            NET_PRES_SocketFlush(socket->SocketID);
            return size;
        }
        // just in case there's some old data already in there...
        NET_PRES_SocketFlush(socket->SocketID);
    }
    else if (socket->SocketType == SOCK_STREAM) //TCP will only send to the already established socket.
    {
        if (socket->bsdState != SKT_EST)
        {
            errno = ENOTCONN;
            return SOCKET_ERROR;
        }

        if (TCP_SocketWasReset(s))
        {
            errno = ECONNRESET;
            return SOCKET_ERROR;
        }

        // Write data to the socket. If one or more bytes were written, then
        // return this value.  Otherwise, fail and return SOCKET_ERROR.
        size = NET_PRES_SocketWrite(socket->SocketID, (uint8_t*) buf, len);
        if (size)
        {
            return size;
        }
    }
    errno = EWOULDBLOCK;
    return SOCKET_ERROR;
}

/*****************************************************************************
  Function:
	int recv( SOCKET s, char* buf, int len, int flags )

  Summary:
	The recv() function is used to receive incoming data that has
	been queued for a socket.

  Description:
	The recv() function is used to receive incoming data that has
	been queued for a socket. This function can be used with both 
	datagram and stream socket. If the available data
	is too large to fit in the supplied application buffer buf,
	excess bytes are discarded in case of SOCK_DGRAM type
	sockets.  For SOCK_STREAM types, the data is buffered
	internally so the application can retreive all data by
	multiple calls of recvfrom.

  Precondition:
	connect function should be called for TCP and UDP sockets.
	Server side, accept function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	buf - application data receive buffer.
	len - buffer length in bytes.
	flags - no significance in this implementation

  Returns:
	If the recv function is successful, the socket is valid and it has pending data:
    - if the supplied buffer is non NULL and has non zero length, the function will return
      the number of bytes copied to the application buffer.
    - if the supplied buffer is NULL or has zero length then no data will be copied and
      the function will return the number of bytes pending in the socket buffer.

    A return value of SOCKET_ERROR (-1)
	indicates an error condition (and errno set accordingly).
    errno is set to EWOULDBLOCK if there is no data pendiding in the socket buffer.

    A value of zero indicates socket has been shutdown by the peer. 

  Remarks:
	None.
  ***************************************************************************/
int recv( SOCKET s, char* buf, int len, int flags )
{
    int     nBytes;

	struct BSDSocket *socket = _getBsdSocket(s);
	if( socket == 0 )
    {
        errno = EBADF;
		return SOCKET_ERROR;
    }

	if(socket->SocketType == SOCK_STREAM) //TCP
	{
		if(socket->bsdState != SKT_EST)
        {
            errno = ENOTCONN;
            return SOCKET_ERROR;
        }

		if(TCP_SocketWasReset(s) || TCP_SocketWasDisconnected(s, false))
        {
            return 0;
        }

		nBytes = NET_PRES_SocketReadIsReady(socket->SocketID);
        if(nBytes && buf && len)
        {   // copy available data to user buffer
            nBytes = NET_PRES_SocketRead(socket->SocketID, (uint8_t*)buf, len);
        }

        if(nBytes)
        {
            return nBytes;
        }
        errno = EWOULDBLOCK;
        return SOCKET_ERROR;
	}
	else if(socket->SocketType == SOCK_DGRAM) //UDP
	{
		if(socket->bsdState != SKT_BOUND)
        {
            errno = EINVAL;
            return SOCKET_ERROR;
        }

        nBytes = NET_PRES_SocketReadIsReady(socket->SocketID);
        if(nBytes && buf && len)
        {   // copy available data to user buffer
            nBytes =  NET_PRES_SocketRead(socket->SocketID, (uint8_t*)buf, len);
            if (nBytes <= len)
            {
                // Need to discard the packet now that we're done with it.
                NET_PRES_SocketDiscard(socket->SocketID);
            }
        }

        if(nBytes)
        {
            return nBytes;
        }
        errno = EWOULDBLOCK;
        return SOCKET_ERROR;
	}

	return 0;
}

/*****************************************************************************
  Function:
	int recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen)

  Summary:
	The recvfrom() function is used to receive incoming data that
	has been queued for a socket.

  Description:
	The recvfrom() function is used to receive incoming data that
	has been queued for a socket. This function can be used with
	both datagram and stream type sockets. If the available data
	is too large to fit in the supplied application buffer buf,
	excess bytes are discarded in case of SOCK_DGRAM type
	sockets. For SOCK_STREAM types, the data is buffered
	internally so the application can retreive all data by
	multiple calls of recvfrom.

  Precondition:
	socket function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	buf - application data receive buffer.
	len - buffer length in bytes.
	flags - message flags. Currently this is not supported.
	from - pointer to the sockaddr structure that will be
	filled in with the destination address.
	fromlen - size of buffer pointed by from.

  Returns:
	If recvfrom is successful, the number of bytes copied to
	application buffer buf is returned.
	A return value of SOCKET_ERROR (-1)
	indicates an error condition (and errno set accordingly).
    A value of zero indicates socket has been shutdown by the peer. 

  Remarks:
	None.
  ***************************************************************************/
int recvfrom( SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen )
{
    struct sockaddr_in *rem_addr = NULL;
#if defined(TCPIP_STACK_USE_IPV6)
    struct sockaddr_in6 *rem_addr6 = NULL;
#endif
    TCP_SOCKET_INFO tcpSockInfo;
    int nBytes;

	struct BSDSocket *socket = _getBsdSocket(s);
    if (socket == 0)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

#if defined(TCPIP_STACK_USE_IPV6)
    if (socket->addressFamily == AF_INET)
    {
#endif
        rem_addr = (struct sockaddr_in *) from;
#if defined(TCPIP_STACK_USE_IPV6)
    }
    else
    {
        rem_addr6 = (struct sockaddr_in6 *) from;
    }
#endif

    if (socket->SocketType == SOCK_DGRAM) //UDP
    {
        // If this BSD socket doesn't have a Microchip UDP socket associated
        // with it yet, then no data can be received and we must not use the
        // socket->SocketID parameter, which isn't set yet.
        if (socket->bsdState != SKT_BOUND)
        {
            errno = EINVAL;
            return SOCKET_ERROR;
        }

        if (NET_PRES_SocketReadIsReady(socket->SocketID))
        {
            // Capture sender information (can change packet to packet)
            if (from && fromlen)
            {
#if defined(TCPIP_STACK_USE_IPV6)
                if (socket->addressFamily == AF_INET)
                {
#endif
                    if ((unsigned int) *fromlen >= sizeof (struct sockaddr_in))
                    {
                        UDP_SOCKET_INFO udpSockInfo;
                        NET_PRES_SocketInfoGet(socket->SocketID, &udpSockInfo);
                        if (udpSockInfo.addressType == IP_ADDRESS_TYPE_IPV4)
                        {
                            rem_addr->sin_addr.S_un.S_addr = udpSockInfo.sourceIPaddress.v4Add.Val;
                            rem_addr->sin_port = udpSockInfo.remotePort;
                            *fromlen = sizeof (struct sockaddr_in);
                        }
                    }
#if defined(TCPIP_STACK_USE_IPV6)
                }
                else
                {
                    if ((unsigned int) *fromlen >= sizeof (struct sockaddr_in6))
                    {
                        UDP_SOCKET_INFO udpSockInfo;
                        NET_PRES_SocketInfoGet(socket->SocketID, &udpSockInfo);
                        if (udpSockInfo.addressType == IP_ADDRESS_TYPE_IPV6)
                        {
                            struct  in6_addr* sin6_addr = (struct  in6_addr*)((uint8_t*)rem_addr6 + offsetof(struct sockaddr_in6, sin6_addr));
                            memcpy(sin6_addr->in6_u.u6_addr8, udpSockInfo.remoteIPaddress.v6Add.d, sizeof(IPV6_ADDR));
                            rem_addr6->sin6_port = udpSockInfo.remotePort;
                            *fromlen = sizeof (struct sockaddr_in6);
                        }
                    }
                }
#endif

            }
            nBytes = NET_PRES_SocketRead(socket->SocketID, (uint8_t*) buf, len);
            if (nBytes <= len)
            {
                //Need to discard the packet now that we're done with it.
                NET_PRES_SocketDiscard(socket->SocketID);
            }
        }
        else
        {
            nBytes = 0;
        }

        if (nBytes)
        {
            return nBytes;
        }
        errno = EWOULDBLOCK;
        return SOCKET_ERROR;
    }
    else //TCP recieve from already connected socket.
    {
        if (from && fromlen)
        {
            // Capture sender information (will always match socket connection information)
#if defined(TCPIP_STACK_USE_IPV6)
            if (socket->addressFamily == AF_INET)
            {
#endif
                if ((unsigned int) *fromlen >= sizeof (struct sockaddr_in))
                {
                    NET_PRES_SocketInfoGet(socket->SocketID, &tcpSockInfo);
                    if (tcpSockInfo.addressType == IP_ADDRESS_TYPE_IPV4)
                    {
                        rem_addr->sin_addr.S_un.S_addr = tcpSockInfo.remoteIPaddress.v4Add.Val;
                        rem_addr->sin_port = tcpSockInfo.remotePort;
                        *fromlen = sizeof (struct sockaddr_in);
                    }
                }
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                if ((unsigned int) *fromlen >= sizeof (struct sockaddr_in6))
                {
                    NET_PRES_SocketInfoGet(socket->SocketID, &tcpSockInfo);
                    if (tcpSockInfo.addressType == IP_ADDRESS_TYPE_IPV6)
                    {
                        struct  in6_addr* sin6_addr = (struct  in6_addr*)((uint8_t*)rem_addr6 + offsetof(struct sockaddr_in6, sin6_addr));
                        memcpy(sin6_addr->in6_u.u6_addr8, tcpSockInfo.remoteIPaddress.v6Add.d, sizeof(IPV6_ADDR));
                        rem_addr6->sin6_port = tcpSockInfo.remotePort;
                        *fromlen = sizeof (struct sockaddr_in6);

                    }
                }

            }
#endif
        }
        return recv(s, buf, len, 0);
    }

}

/*****************************************************************************
  Function:
	int gethostname(char* name, int namelen )

  Summary:
	Returns the standard host name for the system.

  Description:
	This function returns the standard host name of the system which is 
	calling this function.	The returned name is null-terminated.

  Precondition:
	None.

  Parameters:
	name - Pointer to a buffer that receives the local host name.
	namelen - size of the name array.

  Returns:
	Success will return a value of 0. 
	If name is too short to hold the host name or any other error occurs, 
	SOCKET_ERROR (-1) will be returned (and errno set accordingly).
    On error, *name will be unmodified 
	and no null terminator will be generated.

  Remarks:
	The function returns the host name as set on the default network interface.


  ***************************************************************************/
int gethostname(char* name, int namelen)
{
	uint16_t wSourceLen;
	uint16_t w;
	uint8_t v;
    TCPIP_NET_IF* pNetIf;

    pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_NetDefaultGet();
    
	wSourceLen = sizeof(pNetIf->NetBIOSName);
	for(w = 0; w < wSourceLen; w++)
	{
		v = pNetIf->NetBIOSName[w];
		if((v == ' ') || (v == 0u))
			break;
	}
	wSourceLen = w;
	if(namelen < (int)wSourceLen + 1)
    {
        errno = EINVAL;
        return SOCKET_ERROR;
    }

	memcpy((void*)name, (void*)pNetIf->NetBIOSName, wSourceLen);
	name[wSourceLen] = 0;

	return 0;
}

/*****************************************************************************
  Function:
	int closesocket( SOCKET s )
	
  Summary:
	The closesocket function closes an existing socket.

  Description:
	The closesocket function closes an existing socket.  
	This function releases the socket descriptor s.  
	Any data buffered at the socket is discarded.  If the 
	socket s is no longer needed, closesocket() must be 
	called in order to release all resources associated with s.

  Precondition:
	None.

  Parameters:
	s - Socket descriptor returned from a previous call to socket

  Returns:
	If closesocket is successful, a value of 0 is returned. 
	A return value of SOCKET_ERROR (-1) indicates an error.
    (and errno set accordingly).

  Remarks:
	None.
  ***************************************************************************/
int closesocket( SOCKET s )
{	
	uint8_t i;

	struct BSDSocket *socket = _getBsdSocket(s);

    if (socket == 0)
    {
        errno = EBADF;
		return SOCKET_ERROR;
    }

	if(socket->bsdState == SKT_CLOSED)
    {
		return 0;	// Nothing to do, so return success
    }

	if(socket->SocketType == SOCK_STREAM)
	{
		if(socket->bsdState == SKT_BSD_LISTEN)
		{
			// This is a listerner handle, so when we close it we also should 
			// close all TCP sockets that were opened for backlog processing 
			// but didn't actually get connected
			for(i = 0; i < BSD_SOCKET_COUNT; i++)
			{
				if(BSDSocketArray[i].bsdState != SKT_LISTEN)
					continue;
				if(BSDSocketArray[i].localPort == socket->localPort)
				{
					NET_PRES_SocketClose(BSDSocketArray[i].SocketID);
                    socket->SocketID = NET_PRES_INVALID_SOCKET;
					BSDSocketArray[i].bsdState = SKT_CLOSED;
				}
			}
		}
		else if(socket->bsdState >= SKT_LISTEN)
		{
			// For server sockets, if the parent listening socket is still open, 
			// then return this socket to the queue for future backlog processing.
			if(socket->isServer)
			{
				for(i = 0; i < BSD_SOCKET_COUNT; i++)
				{
					if(BSDSocketArray[i].bsdState != SKT_BSD_LISTEN)
						continue;
					if(BSDSocketArray[i].localPort == socket->localPort)
					{
						NET_PRES_SocketDisconnect(socket->SocketID);
						
						// Listener socket is still open, so wait for connection to close
                        // and then return to the listening state so that the user must call accept()
                        // again to reuse this BSD socket
                        // If the other side has already closed, the signal may be already sent. So this is critical!
                        // don't let the TCP/IP thread interfere
                        OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
                        socket->bsdState = TCPIP_TCP_IsConnected(socket->nativeSkt) ? SKT_DISCONNECTING : SKT_LISTEN;
                        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
                        return 0;
                    }
                }
                // If we get down here, then the parent listener socket has 
                // apparently already been closed, so this socket can not be 
                // reused.  Close it complete.
                NET_PRES_SocketClose(socket->SocketID);
            }
            else if(socket->bsdState != SKT_DISCONNECTED)	// this is a client socket that isn't already disconnected
			{
				NET_PRES_SocketClose(socket->SocketID);
			}
		}
	}
	else //udp sockets
	{
		if(socket->bsdState == SKT_BOUND)
        {
			NET_PRES_SocketClose(socket->SocketID);
            socket->SocketID = NET_PRES_INVALID_SOCKET;
        }
	}

    NET_PRES_SocketClose(socket->SocketID);
    socket->SocketID = NET_PRES_INVALID_SOCKET;
	socket->bsdState = SKT_CLOSED;
    socket->SocketID = INVALID_UDP_SOCKET;
    socket->w = 0;
	return 0; //success
}


/*****************************************************************************
  Function:
	static bool TCP_SocketWasReset(SOCKET s)
	
  Summary:
	Internal function that checks for asynchronous TCP connection state 

  Description:
	Internal function that checks for the occurrence of a TCP connection reset 
    If reset occurred, it updates the BSD socket descriptor state to match. 

  Precondition:
	None

  Parameters:
	s - TCP type socket descriptor returned from a previous call to socket.  
	    This socket must be in the SKT_LISTEN, SKT_IN_PROGRESS, SKT_EST, or 
	    SKT_DISCONNECTED states.

  Returns:
	true - Socket has been reset
	false - Socket has not been reset

  ***************************************************************************/
static bool TCP_SocketWasReset(SOCKET s)
{
	struct BSDSocket *socket;
	uint8_t i;

	socket = BSDSocketArray + s;

	// Nothing to do if reset has already been handled
	if(socket->bsdState == SKT_DISCONNECTED)
    {
		return true;	
    }

	// Find out if a reset has occurred
	if(!NET_PRES_SocketWasReset(socket->SocketID))
    {   // Nothing to do if a reset has not occurred
        return false;
    }

	// For server sockets, if the parent listening socket is still open, 
	// then return this socket to the queue for future backlog processing.
	if(socket->isServer)
	{
		for(i = 0; i < BSD_SOCKET_COUNT; i++)
		{
			if(BSDSocketArray[i].bsdState == SKT_BSD_LISTEN)
            {
                if(BSDSocketArray[i].localPort == socket->localPort)
                {   // Listener socket is still open, so just return to the 
                    // listening state so that the user must call accept() again to 
                    // reuse this BSD socket
                    socket->bsdState = SKT_LISTEN;
                    return true;
                }
            }
		}
	}
			
	// If we get down here and the socket was reset, then this socket 
	// should be closed so that no more clients can connect to it.  However, 
	// we can't go to the BSD SKT_CLOSED state directly since the user still 
	// has to call closesocket() with this s SOCKET descriptor first.
    TCPIP_TCP_Abort(socket->nativeSkt, false);
    socket->bsdState = SKT_DISCONNECTED;
    return true;

}

/*****************************************************************************
  Function:
    static bool TCP_SocketWasDisconnected(SOCKET s, bool cliAbort)
	
  Summary:
	Internal function that checks for asynchronous TCP connection state 

  Description:
	Internal function that checks for the occurrence of a TCP disconnection 
    from the remote node.
    If disconnect occurred, it can abort the client connection

  Precondition:
	None

  Parameters:
	s - TCP type socket descriptor returned from a previous call to socket.  
    cliAbort - if true and the disconnect condition detected, the client socket will be aborted
               Note: no action taken on a server socket!

  Returns:
	true - Socket has been remotely disconnected
	false - Socket has not been remotely disconnected

  ***************************************************************************/
static bool TCP_SocketWasDisconnected(SOCKET s, bool cliAbort)
{
	struct BSDSocket *socket;

	socket = BSDSocketArray + s;

	if(socket->bsdState == SKT_DISCONNECTED)
    {   // Nothing to do if a disconnect has already been handled
		return true;	
    }

	// Find out if a reset has occurred
	if(!NET_PRES_SocketWasDisconnected(socket->SocketID))
    {   // Nothing to do if a reset has not occurred
        return false;
    }

	// For server sockets, if the parent listening socket is still open, 
	// then return this socket to the queue for future backlog processing.
	if(socket->isServer == 0 && cliAbort)
    {
        TCPIP_TCP_Abort(socket->nativeSkt, false);
        socket->bsdState = SKT_DISCONNECTED;
    }

    return true;
}

int _setsockopt_ip(const struct BSDSocket * s,
               uint32_t option_name,
               const uint8_t *option_value,
               uint32_t option_length)
{
    switch (option_name)
    {
        case IP_OPTIONS:
        case IP_TOS:
        case IP_TTL:
        case IP_MULTICAST_IF:
        case IP_MULTICAST_TTL:
        case IP_MULTICAST_LOOP:
        case IP_ADD_MEMBERSHIP:
        case IP_DROP_MEMBERSHIP:
        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
}

int _setsockopt_socket(struct BSDSocket * s,
               uint32_t option_name,
               const uint8_t *option_value,
               uint32_t option_length)
{
    switch (option_name)
    {
        case SO_SNDBUF:
        {
            s->sndBufSize = *((uint16_t*)option_value);
            break;
        }

        case SO_LINGER:
        {
            if (s->SocketType == SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                return SOCKET_ERROR;
            }
            else
            {
                struct linger * ling = (struct linger*)option_value;
                s->tcpLinger = ling->l_onoff != 0;
                s->lingerTmo = ling->l_linger;
            }
            break;
        }

        case SO_RCVBUF:
        {
            if (s->SocketType == SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                return SOCKET_ERROR;
            }
            s->rcvBufSize = *((uint16_t*)option_value);
            break;
        }

        case SO_BROADCAST:
        {
            if (s->SocketType != SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                return SOCKET_ERROR;
            }
            s->udpBcastEnabled = *((bool*)option_value) != 0;
            break;
        }
        
        case SO_DEBUG:
        case SO_DONTROUTE:
        case SO_KEEPALIVE:
        case SO_RCVLOWAT:
        case SO_RCVTIMEO:
        case SO_REUSEADDR:
        case SO_OOBINLINE:
        case SO_SNDLOWAT:
        case SO_SNDTIMEO:
        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
    _cfgBsdSocket(s);
    return 0;
}

int _setsockopt_tcp(struct BSDSocket * s,
               uint32_t option_name,
               const uint8_t *option_value,
               uint32_t option_length)
{
    if (s->SocketType == SOCK_DGRAM)
    {
        errno = EOPNOTSUPP;
        return SOCKET_ERROR;
    }
    switch (option_name)
    {
        case TCP_NODELAY:
        {
            s->tcpNoDelay = *option_value;
        }
        break;

        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
    _cfgBsdSocket(s);
    return 0;

}

int _setsockopt_ipv6(struct BSDSocket * s,
               uint32_t option_name,
               const uint8_t *option_value,
               uint32_t option_length)
{
    switch (option_name)
    {
        case IPV6_UNICAST_HOPS:
        case IPV6_MULTICAST_IF:
        case IPV6_MULTICAST_HOPS:
        case IPV6_MULTICAST_LOOP:
        case IPV6_JOIN_GROUP:
        case IPV6_LEAVE_GROUP:
        case IPV6_V6ONLY:
        case IPV6_CHECKSUM:
        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
}

int _setsockopt_icmp6(struct BSDSocket * s,
               uint32_t option_name,
               const uint8_t *option_value,
               uint32_t option_length)
{
    switch (option_name)
    {
        case ICMP6_FILTER:
        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
}

int setsockopt(SOCKET s,
               uint32_t level,
               uint32_t option_name,
               const uint8_t *option_value,
               uint32_t option_length)
{
	struct BSDSocket *socket = _getBsdSocket(s);

    if (socket == 0 || socket->bsdState == SKT_CLOSED) 
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    switch (level)
    {
        case IPPROTO_IP:
            return _setsockopt_ip(socket,
                                  option_name,
                                  option_value,
                                  option_length);
        case SOL_SOCKET:
            return _setsockopt_socket(socket,
                                      option_name,
                                      option_value,
                                      option_length);
        case IPPROTO_TCP:
            return _setsockopt_tcp(socket,
                                   option_name,
                                   option_value,
                                   option_length);
        case IPPROTO_IPV6:
            return _setsockopt_ipv6(socket,
                                    option_name,
                                    option_value,
                                    option_length);
        case IPPROTO_ICMPV6:
            return _setsockopt_icmp6(socket,
                                     option_name,
                                     option_value,
                                     option_length);
        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
}


int _getsockopt_ip(const struct BSDSocket * s,
               uint32_t option_name,
               uint8_t *option_value,
               uint32_t *option_length)
{
    switch (option_name)
    {
        case IP_OPTIONS:
        case IP_TOS:
        case IP_TTL:
        case IP_MULTICAST_IF:
        case IP_MULTICAST_TTL:
        case IP_MULTICAST_LOOP:
        case IP_ADD_MEMBERSHIP:
        case IP_DROP_MEMBERSHIP:
        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
}

int _getsockopt_socket(struct BSDSocket * s,
               uint32_t option_name,
               uint8_t *option_value,
               uint32_t *option_length)
{
    switch (option_name)
    {
        case SO_SNDBUF:
        {
            if (s->SocketID == INVALID_SOCKET)
            {
                *(uint16_t*)option_value = s->sndBufSize;
            }
            else
            {
                if (s->SocketType == SOCK_DGRAM)
                {
                    NET_PRES_SocketOptionsGet(s->SocketID, UDP_OPTION_TX_BUFF, option_value);
                }
                else
                {
                    NET_PRES_SocketOptionsGet(s->SocketID, UDP_OPTION_TX_BUFF, option_value);

                }
                *option_length = 2;
            }
            s->sndBufSize = *((uint16_t*)option_value);
            break;
        }

        case SO_LINGER:
        {
            if (s->SocketType == SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                return SOCKET_ERROR;
            }
            else
            {
                struct linger * ling = (struct linger*)option_value;
                if (s->SocketID == INVALID_SOCKET)
                {
                    ling->l_onoff = s->tcpLinger;
                    ling->l_linger = s->lingerTmo;
                }
                else
                {
                    TCP_OPTION_LINGER_DATA tcplinger;
                    NET_PRES_SocketOptionsGet(s->SocketID, TCP_OPTION_LINGER, & tcplinger);
                    ling->l_onoff = tcplinger.lingerEnable;
                    ling->l_linger = tcplinger.lingerTmo;
                }
                *option_length = sizeof(struct linger);
            }
            break;
        }

        case SO_RCVBUF:
        {
            if (s->SocketType == SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                return SOCKET_ERROR;
            }
            if (s->SocketID == INVALID_SOCKET)
            {
                *(uint16_t*)option_value = s->sndBufSize;
            }
            else
            {
                NET_PRES_SocketOptionsGet(s->SocketID, TCP_OPTION_RX_BUFF, option_value);
                *option_length = 2;
            }
            s->sndBufSize = *((uint16_t*)option_value);
            break;
        }

        case SO_BROADCAST:
        {
            if (s->SocketType != SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                return SOCKET_ERROR;
            }

            *(bool*)option_value = s->udpBcastEnabled != 0;
            break;
        }

        case SO_DEBUG:
        case SO_DONTROUTE:
        case SO_KEEPALIVE:
        case SO_RCVLOWAT:
        case SO_RCVTIMEO:
        case SO_REUSEADDR:
        case SO_OOBINLINE:
        case SO_SNDLOWAT:
        case SO_SNDTIMEO:
        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
    return 0;
}

int _getsockopt_tcp(struct BSDSocket * s,
               uint32_t option_name,
               uint8_t *option_value,
               uint32_t *option_length)
{
    if (s->SocketType == SOCK_DGRAM)
    {
        errno = EOPNOTSUPP;
        return SOCKET_ERROR;
    }
    switch (option_name)
    {
        case TCP_NODELAY:
        {
            if (s->SocketID == INVALID_SOCKET)
            {
                *option_value = s->tcpNoDelay;
            }
            else
            {
                *option_value = NET_PRES_SocketOptionsGet(s->SocketID, TCP_OPTION_NODELAY, option_value);
            }
            *option_length = 1;
        }
        break;

        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
    return 0;

}

int _getsockopt_ipv6(struct BSDSocket * s,
               uint32_t option_name,
               uint8_t *option_value,
               uint32_t *option_length)
{
    switch (option_name)
    {
        case IPV6_UNICAST_HOPS:
        case IPV6_MULTICAST_IF:
        case IPV6_MULTICAST_HOPS:
        case IPV6_MULTICAST_LOOP:
        case IPV6_JOIN_GROUP:
        case IPV6_LEAVE_GROUP:
        case IPV6_V6ONLY:
        case IPV6_CHECKSUM:
        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
}

int _getsockopt_icmp6(struct BSDSocket * s,
               uint32_t option_name,
               uint8_t *option_value,
               uint32_t *option_length)
{
    switch (option_name)
    {
        case ICMP6_FILTER:
        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
}

int getsockopt(SOCKET s,
               uint32_t level,
               uint32_t option_name,
               uint8_t *option_value,
               uint32_t *option_length)
{
	struct BSDSocket *socket = _getBsdSocket(s);

    if (socket == 0 || socket->bsdState == SKT_CLOSED) 
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    switch (level)
    {
        case IPPROTO_IP:
            return _getsockopt_ip(socket,
                                  option_name,
                                  option_value,
                                  option_length);
        case SOL_SOCKET:
            return _getsockopt_socket(socket,
                                      option_name,
                                      option_value,
                                      option_length);
        case IPPROTO_TCP:
            return _getsockopt_tcp(socket,
                                   option_name,
                                   option_value,
                                   option_length);
        case IPPROTO_IPV6:
            return _getsockopt_ipv6(socket,
                                    option_name,
                                    option_value,
                                    option_length);
        case IPPROTO_ICMPV6:
            return _getsockopt_icmp6(socket,
                                     option_name,
                                     option_value,
                                     option_length);
        default:
            errno = EOPNOTSUPP;
            return SOCKET_ERROR;
    }
}

static IPV4_ADDR sAddr;
static char * sHostArray[2] = {
    (char *)&sAddr,
    NULL
};

int h_errno;

static struct hostent sHostEnt = {
    NULL,
    NULL,
    AF_INET,
    sizeof(IPV4_ADDR),
    (char**)&sHostArray
};

struct hostent * gethostent(void)
{
    return &sHostEnt;
}


static uint8_t sHaveDnsToken = 0;

struct hostent * gethostbyname(char *name)
{
    TCPIP_DNS_RESULT dRes;    

    if (sHaveDnsToken == 0)
    {
        dRes = TCPIP_DNS_Resolve(name, TCPIP_DNS_TYPE_A);
        if (dRes == TCPIP_DNS_RES_NAME_IS_IPADDRESS)
        {
           TCPIP_Helper_StringToIPAddress(name, & sAddr);
           return &sHostEnt;
        }
        if (dRes != TCPIP_DNS_RES_OK && dRes != TCPIP_DNS_RES_PENDING)
        {
            h_errno = NO_RECOVERY;
            return NULL;
        }
        sHaveDnsToken = 1;
    }
    dRes = TCPIP_DNS_IsNameResolved(name, &sAddr, 0);
    switch (dRes)
    {
        case TCPIP_DNS_RES_PENDING:
            h_errno = TRY_AGAIN;
            return NULL;
        case TCPIP_DNS_RES_SERVER_TMO:
            h_errno = TRY_AGAIN;
            sHaveDnsToken = 0;
            return NULL;
        case TCPIP_DNS_RES_NO_NAME_ENTRY:
            h_errno = HOST_NOT_FOUND;
            sHaveDnsToken = 0;
            return NULL;
        case TCPIP_DNS_RES_OK:
            sHaveDnsToken = 0;
            return &sHostEnt;
        default:
            h_errno = NO_RECOVERY;
            sHaveDnsToken = 0;
            return NULL;
    }
}

int getsockname( SOCKET s, struct sockaddr *addr, int *addrlen)
{
    struct sockaddr_in *rem_addr;

    if(addrlen == 0)
    {
        errno = EINVAL;
        return SOCKET_ERROR;
    }

	struct BSDSocket *socket = _getBsdSocket(s);
    if (socket == 0 || socket->bsdState == SKT_CLOSED)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }


#if defined(TCPIP_STACK_USE_IPV6)
    if (socket->addressFamily != AF_INET)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)

    if(*addrlen >= sizeof(struct sockaddr_in) && addr != 0)
    {
        rem_addr = (struct sockaddr_in*)addr;
        rem_addr->sin_family = AF_INET;
        rem_addr->sin_port = socket->localPort;
        rem_addr->sin_addr.S_un.S_addr = socket->localIP;

        *addrlen = sizeof(struct sockaddr_in);
        return 0;
    }

    errno = EFAULT;
    return SOCKET_ERROR;

}

int TCPIP_BSD_Socket(SOCKET s)
{
	struct BSDSocket *socket = _getBsdSocket(s);

    if (socket == 0 || socket->bsdState == SKT_CLOSED)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }


    int16_t nativeSkt = NET_PRES_SocketGetTransportHandle(socket->SocketID);
    return nativeSkt < 0 ? SOCKET_ERROR : nativeSkt; 
}

int TCPIP_BSD_PresSocket(SOCKET s)
{
	struct BSDSocket *socket = _getBsdSocket(s);

    if (socket == 0 || socket->bsdState == SKT_CLOSED)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    return socket->SocketID;
}

typedef enum 
{
    TCPIP_BERKELEY_GAI_INACTIVE  = 0,
    TCPIP_BERKELEY_GAI_START_IPV4,
    TCPIP_BERKELEY_GAI_WAIT_IPV4,
    TCPIP_BERKELEY_GAI_START_IPV6,
    TCPIP_BERKELEY_GAI_WAIT_IPV6,
    TCPIP_BERKELEY_GAI_FINISHED
} TCPIP_BERKELEY_GAI_STATE;

uint32_t sgaihash = 0;
TCPIP_BERKELEY_GAI_STATE sgaistate = TCPIP_BERKELEY_GAI_INACTIVE;

// Note: a DNS query with TCPIP_DNS_Resolve(name, TCPIP_DNS_TYPE_ANY) is deprecated by some DNS servers
// that's why it needs to be done in turn: TCPIP_DNS_TYPE_A, then TCPIP_DNS_TYPE_AAAA
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
    int counter;
    int numDNS;
    int setres;
    IPV4_ADDR tmpv4;
    IPV6_ADDR tmpv6;

    if(res == 0)
    {
        errno = EINVAL;
        return EAI_SYSTEM;
    }

    uint32_t nodeHash = fnv_32a_hash((void*)node, strlen(node));
    if (sgaihash != 0)
    {
        if (nodeHash != sgaihash)
        {
            errno = EBUSY;
            return EAI_SYSTEM;
        }
    }
    else
    {
        sgaihash = nodeHash;
        sgaistate++;
    }

    *res = 0;

    switch (sgaistate)
    {
        case TCPIP_BERKELEY_GAI_START_IPV4:
            if(hints != NULL && hints->ai_family != AF_UNSPEC && hints->ai_family != AF_INET)
            {   // IPv4 resolution not needed; jump to IPv6
                sgaistate += 2;
                return EAI_AGAIN;
            }

            setres = _BSD_StartAddrInfo(node, hints, res, TCPIP_DNS_TYPE_A);

            if(setres <= 0)
            {   // either done or some error;
                sgaihash = 0;
                sgaistate = TCPIP_BERKELEY_GAI_INACTIVE;
                return setres;
            }

            
            // pending, advance to the new state
            sgaistate ++;
            return EAI_AGAIN;

        case TCPIP_BERKELEY_GAI_WAIT_IPV4:

            setres = _BSD_CheckAddrInfo(node, TCPIP_DNS_TYPE_A);

            if(setres < 0)
            {   // DNS error; abort
                sgaihash = 0;
                sgaistate = TCPIP_BERKELEY_GAI_INACTIVE;
                return EAI_FAIL;
            }
            else if(setres > 0)
            {   // wait some more
                return EAI_AGAIN;
            }

            // ok, advance to the new state
            sgaistate ++;
            return EAI_AGAIN;

        case TCPIP_BERKELEY_GAI_START_IPV6:
            if(hints != NULL && hints->ai_family != AF_UNSPEC && hints->ai_family != AF_INET6)
            {   // IPv6 resolution not needed; jump to done
                sgaistate += 2;
                return EAI_AGAIN;
            }

            setres = _BSD_StartAddrInfo(node, hints, res, TCPIP_DNS_TYPE_AAAA);

            if(setres <= 0)
            {   // either done or some error;
                sgaihash = 0;
                sgaistate = TCPIP_BERKELEY_GAI_INACTIVE;
                return setres;
            }

            // pending, advance to the new state
            sgaistate ++;
            return EAI_AGAIN;

        case TCPIP_BERKELEY_GAI_WAIT_IPV6:

            setres = _BSD_CheckAddrInfo(node, TCPIP_DNS_TYPE_AAAA);

            if(setres < 0)
            {   // DNS error; abort
                sgaihash = 0;
                sgaistate = TCPIP_BERKELEY_GAI_INACTIVE;
                return EAI_FAIL;
            }
            else if(setres > 0)
            {   // wait some more
                return EAI_AGAIN;
            }

            // ok, advance to the new state
            sgaistate ++;
            return EAI_AGAIN;

        case TCPIP_BERKELEY_GAI_FINISHED:
            *res = 0;
            setres = 0;
            if(hints == NULL || hints->ai_family == AF_UNSPEC || hints->ai_family == AF_INET)
            {   // get IPv4
                numDNS = TCPIP_DNS_GetIPAddressesNumber(node, IP_ADDRESS_TYPE_IPV4);

                for (counter = 0; counter < numDNS; counter++)
                {
                    TCPIP_DNS_GetIPv4Addresses(node, counter, &tmpv4, 1);
                    setres = _BSD_SetIp4AddrInfo(tmpv4.Val, hints, res);
                    if(setres != 0)
                    {   // some failure
                        break;
                    }
                }
            }

            if(setres == 0 && (hints == NULL || hints->ai_family == AF_UNSPEC || hints->ai_family == AF_INET6))
            {   // get IPv6
                numDNS = TCPIP_DNS_GetIPAddressesNumber(node, IP_ADDRESS_TYPE_IPV6);

                for (counter = 0; counter < numDNS; counter++)
                {
                    TCPIP_DNS_GetIPv6Addresses(node, counter, &tmpv6, 1);
                    setres = _BSD_SetIp6AddrInfo(&tmpv6, hints, res);
                    if(setres != 0)
                    {   // some failure
                        break;
                    }
                }
            }

            sgaihash = 0;
            sgaistate = TCPIP_BERKELEY_GAI_INACTIVE;
            if (setres == 0 && *res == 0)
            {
                setres = EAI_NONAME;
            }
            return setres;

        default:
            SYS_ASSERT(false, "Should not be here!");
            return EAI_SYSTEM;
    }

}

// starts a DNS resolve query
// returns:
//      < 0 if error
//      0 if done (query was an IP address, not a name)
//      1 if pending
//
static int _BSD_StartAddrInfo(const char* node, const struct addrinfo* hints, struct addrinfo** res, TCPIP_DNS_RESOLVE_TYPE dnstype)
{
    IPV4_ADDR tmpv4;
    IPV6_ADDR tmpv6;

    TCPIP_DNS_RESULT result = TCPIP_DNS_Resolve(node, dnstype);

    if(result < 0)
    {   // DNS error; abort
        return EAI_FAIL;
    }
    else if(result == TCPIP_DNS_RES_PENDING || result == TCPIP_DNS_RES_OK)
    {   // ok, wait a while or DNS name known
        return 1;
    }
    else if(result == TCPIP_DNS_RES_NAME_IS_IPADDRESS)
    {   // name resolution available
        if (TCPIP_Helper_StringToIPAddress(node, &tmpv4))
        {
            return _BSD_SetIp4AddrInfo(tmpv4.Val, hints, res);
        }
        else if (TCPIP_Helper_StringToIPv6Address(node, &tmpv6))
        {
            return _BSD_SetIp6AddrInfo(&tmpv6, hints, res);
        }
    }

    // should not get here
    SYS_ASSERT(false, "getaddrinfo unknown error");
    return EAI_FAIL;

} 

// checks a DNS resolve
// returns:
//      < 0 if error
//      0 if done
//      1 if pending
//
static int _BSD_CheckAddrInfo(const char* node, TCPIP_DNS_RESOLVE_TYPE dnstype)
{
    IPV4_ADDR tmpv4;
    IPV6_ADDR tmpv6;
    TCPIP_DNS_RESULT result;

    if(dnstype == TCPIP_DNS_TYPE_A)
    {
        result = TCPIP_DNS_IsNameResolved(node, &tmpv4, 0);
    }
    else
    {
        result = TCPIP_DNS_IsNameResolved(node, 0, &tmpv6);
    }

    if(result < 0)
    {   // DNS error; abort
        return EAI_FAIL;
    }
    else if(result == TCPIP_DNS_RES_PENDING)
    {   // ok, wait a while
        return 1;
    }
    else if(result == TCPIP_DNS_RES_OK)
    {   // ok, done
        return 0;
    }

    // should not get here
    SYS_ASSERT(false, "getaddrinfo unknown error");
    return EAI_FAIL;

} 

// returns 0 if done
// EAI_MEMORY if out of memory
static int _BSD_SetIp4AddrInfo(uint32_t ipAddr, const struct addrinfo* hints, struct addrinfo** res)
{
    struct addrinfo* ptr = *res;

    if(ptr == 0)
    {
        ptr = TCPIP_STACK_CALLOC_FUNC(1, sizeof (struct addrinfo));
        *res = ptr;
    }
    else
    {
        ptr->ai_next = TCPIP_STACK_CALLOC_FUNC(1, sizeof (struct addrinfo));
        ptr = ptr->ai_next;
    }

    if(ptr == 0)
    {
        SYS_ERROR(SYS_ERROR_WARNING, "Could not allocate memory for address info\r\n");
        return EAI_MEMORY;
    }

    ptr->ai_family = AF_INET;
    ptr->ai_next = NULL;
    if (hints != NULL)
    {
        ptr->ai_flags = hints->ai_flags;
        ptr->ai_socktype = hints->ai_socktype;
        ptr->ai_protocol = hints->ai_protocol;
        ptr->ai_canonname = hints->ai_canonname;
    }

    ptr->ai_addrlen = sizeof(struct sockaddr_in);
    ptr->ai_addr = TCPIP_STACK_CALLOC_FUNC(1, sizeof(struct sockaddr_in));
    if(ptr->ai_addr == 0)
    {
        SYS_ERROR(SYS_ERROR_WARNING, "Could not allocate memory for address info\r\n");
        return EAI_MEMORY;
    }

    ((struct sockaddr_in*)(ptr->ai_addr))->sin_family = AF_INET;
    ((SOCKADDR_IN*)(ptr->ai_addr))->sin_addr.S_un.S_addr = ipAddr;
    return 0;
}

static int _BSD_SetIp6AddrInfo(const IPV6_ADDR* ipAddr, const struct addrinfo* hints, struct addrinfo** res)
{
    struct addrinfo* ptr = *res;

    if(ptr == 0)
    {
        ptr = TCPIP_STACK_CALLOC_FUNC(1, sizeof (struct addrinfo));
        *res = ptr;
    }
    else
    {
        ptr->ai_next = TCPIP_STACK_CALLOC_FUNC(1, sizeof (struct addrinfo));
        ptr = ptr->ai_next;
    }

    if(ptr == 0)
    {
        SYS_ERROR(SYS_ERROR_WARNING, "Could not allocate memory for address info\r\n");
        return EAI_MEMORY;
    }

    ptr->ai_family = AF_INET6;
    ptr->ai_next = NULL;
    if (hints != NULL)
    {
        ptr->ai_flags = hints->ai_flags;
        ptr->ai_socktype = hints->ai_socktype;
        ptr->ai_protocol = hints->ai_protocol;
        ptr->ai_canonname = hints->ai_canonname;
    }

    ptr->ai_addrlen = sizeof(struct sockaddr_in6);
    ptr->ai_addr = TCPIP_STACK_CALLOC_FUNC(1, sizeof(SOCKADDR_IN6));
    if(ptr->ai_addr == 0)
    {
        SYS_ERROR(SYS_ERROR_WARNING, "Could not allocate memory for address info\r\n");
        return EAI_MEMORY;
    }

    ((SOCKADDR_IN6*)(ptr->ai_addr))->sin6_family = AF_INET6;
    ((SOCKADDR_IN6*)(ptr->ai_addr))->sin6_addr.in6_u.u6_addr32[0] = ipAddr->d[0];
    ((SOCKADDR_IN6*)(ptr->ai_addr))->sin6_addr.in6_u.u6_addr32[1] = ipAddr->d[1];
    ((SOCKADDR_IN6*)(ptr->ai_addr))->sin6_addr.in6_u.u6_addr32[2] = ipAddr->d[2];
    ((SOCKADDR_IN6*)(ptr->ai_addr))->sin6_addr.in6_u.u6_addr32[3] = ipAddr->d[3];
    return 0;
}


void freeaddrinfo(struct addrinfo *res)
{
    if (res == NULL)
    {
        return;
    }
    struct addrinfo *ptr = res;
    while (ptr != NULL)
    {
        struct addrinfo *ptr2 = ptr->ai_next;
        if (ptr->ai_addr != NULL)
        {
            TCPIP_STACK_FREE_FUNC(ptr->ai_addr);
        }
        TCPIP_STACK_FREE_FUNC(ptr);
        ptr = ptr2;
    }
}

static void TCP_SignalFunction(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    if((sigType & (TCPIP_TCP_SIGNAL_RX_FIN | TCPIP_TCP_SIGNAL_RX_RST | TCPIP_TCP_SIGNAL_TX_RST)) != 0)
    {   // socket was closed
        // restore the child BSD socket
        struct BSDSocket* pChild = (struct BSDSocket*)param;
        while(true)
        {
            if(pChild->bsdState != SKT_DISCONNECTING)
            {
                break;
            }

            // socket closed remotedly
            // find the parent it belongs to
            uint16_t parentIx = pChild->parentId;
            if(BSDSocketArray != 0 && parentIx < BSD_SOCKET_COUNT)
            {
                struct BSDSocket* pParent = BSDSocketArray + parentIx;
                if(pParent->bsdState == SKT_BSD_LISTEN && pParent->localPort == pChild->localPort)
                {   // found the parent up and running; return the child to accepting state
                    pChild->bsdState = SKT_LISTEN;
                    return;
                }
            }

            // no parent could be found...close the child socket
            pChild->needsClose = true;
            _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC, true); 
            break;
        }
    }
}

// debug stuff

#if (__BERKELEY_DEBUG != 0)
bool TCPIP_BSD_State(SOCKET s, BSD_SKT_INFO* pInfo)
{
	struct BSDSocket *socket = _getBsdSocket(s);
    if( socket == 0 )
    {
        return false;
    }

    if(pInfo)
    {
        pInfo->bsdState = socket->bsdState;
        pInfo->presSktId = socket->SocketID;
    }

    return true;
}
#endif  // (__BERKELEY_DEBUG != 0)


#endif //TCPIP_STACK_USE_BERKELEY_API

