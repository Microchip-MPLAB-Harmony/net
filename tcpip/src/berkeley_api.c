/*******************************************************************************
  Berkeley Socket Distribution API Source File

  Summary:
    
  Description:
*******************************************************************************/

/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_BERKELEY

#include "tcpip/src/tcpip_private.h"
#include "net_pres/pres/net_pres_socketapi.h"

#if defined(TCPIP_STACK_USE_BERKELEY_API)

#include <errno.h>
#if (__XC32_VERSION < 4000) || (__XC32_VERSION == 243739000)
// xc32 versions >= v4.0 no longer have sys/errno.h 
#include <sys/errno.h>
#endif


static bool TCP_SocketWasReset(SOCKET s);
static bool TCP_SocketWasDisconnected(SOCKET s, bool cliAbort);

static void TCP_SignalFunction(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param);

static int F_BSD_SetIp4AddrInfo(uint32_t ipAddr, const struct addrinfo* hints, struct addrinfo** res);
static int F_BSD_SetIp6AddrInfo(const IPV6_ADDR* ipAddr, const struct addrinfo* hints, struct addrinfo** res);

static int F_BSD_StartAddrInfo(const char* node, const struct addrinfo* hints, struct addrinfo** res, TCPIP_DNS_RESOLVE_TYPE dnstype);
static int F_BSD_CheckAddrInfo(const char* node, TCPIP_DNS_RESOLVE_TYPE dnstype);

static int F_setsockopt_ip(const struct BSDSocket * s, uint32_t option_name, const uint8_t *option_value, uint32_t option_length);
static int F_setsockopt_socket(struct BSDSocket * s, uint32_t option_name, const uint8_t *option_value, uint32_t option_length);
static int F_setsockopt_tcp(struct BSDSocket * s, uint32_t option_name, const uint8_t *option_value, uint32_t option_length);

static int F_getsockopt_ip(const struct BSDSocket * s, uint32_t option_name, uint8_t *option_value, uint32_t *option_length);
static int F_getsockopt_socket(struct BSDSocket * s, uint32_t option_name, uint8_t *option_value, uint32_t *option_length);
static int F_getsockopt_tcp(struct BSDSocket * s, uint32_t option_name, uint8_t *option_value, uint32_t *option_length);

static int F_setsockopt_ipv6(struct BSDSocket * s, uint32_t option_name, const uint8_t *option_value, uint32_t option_length);
static int F_setsockopt_icmp6(struct BSDSocket * s, uint32_t option_name, const uint8_t *option_value, uint32_t option_length);

static int F_getsockopt_ipv6(struct BSDSocket * s, uint32_t option_name, uint8_t *option_value, uint32_t *option_length);
static int F_getsockopt_icmp6(struct BSDSocket * s, uint32_t option_name, uint8_t *option_value, uint32_t *option_length);

#if defined(TCPIP_STACK_USE_IPV4)
static int connect_stream_v4(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* remoteIP, uint16_t remotePort, SOCKET s);
static int connect_bound_v4(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* remoteIP, uint16_t remotePort);
static int connect_dgram_v4(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* remoteIP, uint16_t remotePort);
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
static int connect_stream_v6(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* pLocalAddr6, IP_MULTI_ADDRESS* pRemoteIP6, uint16_t remotePort, SOCKET s);
static int connect_bound_v6(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* pLocalAddr6, IP_MULTI_ADDRESS* pRemoteIP6, uint16_t remotePort);
static int connect_dgram_v6(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* pRemoteIP6, uint16_t remotePort);
#endif  // defined(TCPIP_STACK_USE_IPV6)


#if !defined(MAX_BSD_SOCKETS)
#define MAX_BSD_SOCKETS 4
#endif


// Array of BSDSocket elements; used to track all socket state and connection information.
static const void*  bsdApiHeapH = NULL;                    // memory allocation handle
static struct BSDSocket  * BSDSocketArray = NULL;
static int16_t BSD_SOCKET_COUNT = MAX_BSD_SOCKETS;

// The initialization count, so we know how many interfaces are up.
static int InitCount = 0;

static OSAL_SEM_HANDLE_TYPE bsdSemaphore;
static TCPIP_SIGNAL_HANDLE    bsdSignalHandle;

static uint32_t sgaihash = 0U;
static uint32_t sgaistate = (uint32_t)TCPIP_BERKELEY_GAI_INACTIVE;   // TCPIP_BERKELEY_GAI_STATE value

// helpers
static __inline__  struct BSDSocket*  __attribute__((always_inline)) FC_CVPtr2BSD(const void* cvptr)
{
    union
    {
        const void* cvptr;
        struct BSDSocket* pSkt;
    }U_CVPTR_BSD;

    U_CVPTR_BSD.cvptr = cvptr;
    return U_CVPTR_BSD.pSkt;
} 

static __inline__  const struct sockaddr_in*  __attribute__((always_inline)) FC_CSockAddr2In(const struct sockaddr* saddr)
{
    union
    {
        const struct sockaddr*      saddr;
        const struct sockaddr_in*   s_addr_in;
    }U_SADDR_IN_CPTR;

    U_SADDR_IN_CPTR.saddr = saddr;
    return U_SADDR_IN_CPTR.s_addr_in;
}

static __inline__  struct sockaddr_in*  __attribute__((always_inline)) FC_SockAddr2In(struct sockaddr* saddr)
{
    union
    {
        struct sockaddr*      saddr;
        struct sockaddr_in*   s_addr_in;
    }U_SADDR_IN_PTR;

    U_SADDR_IN_PTR.saddr = saddr;
    return U_SADDR_IN_PTR.s_addr_in;
}

static __inline__  struct sockaddr*  __attribute__((always_inline)) FC_SockIn2Addr(struct sockaddr_in* s_addr_in)
{
    union
    {
        struct sockaddr_in* s_addr_in;
        struct sockaddr*    saddr;
    }U_SIN_ADDR_PTR;

    U_SIN_ADDR_PTR.s_addr_in = s_addr_in;
    return U_SIN_ADDR_PTR.saddr;
}

static __inline__  const struct sockaddr_in6*  __attribute__((always_inline)) FC_CSockAddr2In6(const struct sockaddr* saddr)
{
    union
    {
        const struct sockaddr*      saddr;
        const struct sockaddr_in6*  s_addr_in6;
    }U_SADDR_IN6_CPTR;

    U_SADDR_IN6_CPTR.saddr = saddr;
    return U_SADDR_IN6_CPTR.s_addr_in6;
}

static __inline__  struct sockaddr_in6*  __attribute__((always_inline)) FC_SockAddr2In6(struct sockaddr* saddr)
{
    union
    {
        struct sockaddr*      saddr;
        struct sockaddr_in6*  s_addr_in6;
    }U_SADDR_IN6_PTR;

    U_SADDR_IN6_PTR.saddr = saddr;
    return U_SADDR_IN6_PTR.s_addr_in6;
}

static __inline__  struct sockaddr*  __attribute__((always_inline)) FC_SockIn62Addr(struct sockaddr_in6* s_addr_in6)
{
    union
    {
        struct sockaddr_in6*  s_addr_in6;
        struct sockaddr*      saddr;
    }U_SIN6_ADDR_PTR;

    U_SIN6_ADDR_PTR.s_addr_in6 = s_addr_in6;
    return U_SIN6_ADDR_PTR.saddr;
}

static __inline__  const struct in6_addr*  __attribute__((always_inline)) FC_CU8Ptr2In6(const uint8_t* u8ptr)
{
    union
    {
        const uint8_t*          u8ptr;
        const struct in6_addr*  in6_ptr;
    }U_U8_IN6_CPTR;

    U_U8_IN6_CPTR.u8ptr = u8ptr;
    return U_U8_IN6_CPTR.in6_ptr;
}

static __inline__  struct in6_addr*  __attribute__((always_inline)) FC_U8Ptr2In6(uint8_t* u8ptr)
{
    union
    {
        uint8_t*          u8ptr;
        struct in6_addr*  in6_ptr;
    }U_U8_IN6_PTR;

    U_U8_IN6_PTR.u8ptr = u8ptr;
    return U_U8_IN6_PTR.in6_ptr;
}

/* MISRA C-2012 Rule 21.3 deviated:2 Deviation record ID -  H3_MISRAC_2012_R_21_3_NET_DR_7 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:2 "MISRA C-2012 Rule 21.3" "H3_MISRAC_2012_R_21_3_NET_DR_7" 
static __inline__ void* __attribute__((always_inline)) F_BsdCalloc (size_t items, size_t itemSize)
{
    return TCPIP_STACK_CALLOC_FUNC(items, itemSize);
}

static __inline__ void __attribute__((always_inline)) F_BsdFree(void* ptr)
{
    TCPIP_STACK_FREE_FUNC(ptr);
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 21.3"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

// validates the socket and returns the pointer to the internal BSDSocket
// returns 0 if error
static struct BSDSocket* F_getBsdSocket(SOCKET s)
{
    if(s >= 0 && s < BSD_SOCKET_COUNT)
    {
        return BSDSocketArray + s;
    }

    return NULL;
}

static void F_cfgBsdSocket(struct BSDSocket * socketInfo)
{
    void* ptrOpt;
    if (socketInfo->SocketID == NET_PRES_INVALID_SOCKET)
    {
        return;
    }

    socketInfo->nativeSkt = NET_PRES_SocketGetTransportHandle(socketInfo->SocketID);            
    if (socketInfo->SocketType == (uint16_t)SOCK_DGRAM)
    {
        if (socketInfo->sndBufSize != 0U)
        {
            ptrOpt = FC_Uint2VPtr(socketInfo->sndBufSize); 
            (void)NET_PRES_SocketOptionsSet(socketInfo->SocketID, (NET_PRES_SKT_OPTION_TYPE)UDP_OPTION_TX_BUFF, ptrOpt);
        }
        if(socketInfo->udpBcastEnabled != 0U)
        {
            (void)TCPIP_UDP_BcastIPV4AddressSet(socketInfo->nativeSkt, UDP_BCAST_NETWORK_LIMITED, NULL);
        }

    }
    else if (socketInfo->SocketType == (uint16_t)SOCK_STREAM)
    {
        TCP_OPTION_LINGER_DATA lingerData;
        lingerData.lingerEnable = socketInfo->tcpLinger != 0U;
        lingerData.lingerTmo = socketInfo->lingerTmo;
        lingerData.gracefulEnable = socketInfo->tcpGracefulDisable == 0U;

        (void)NET_PRES_SocketOptionsSet(socketInfo->SocketID, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_LINGER, (void*)&lingerData);

        if (socketInfo->sndBufSize != 0U)
        {
            ptrOpt = FC_Uint2VPtr(socketInfo->sndBufSize); 
            (void)NET_PRES_SocketOptionsSet(socketInfo->SocketID, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, ptrOpt);
        }
        if (socketInfo->rcvBufSize != 0U)
        {
            ptrOpt = FC_Uint2VPtr(socketInfo->rcvBufSize); 
            (void)NET_PRES_SocketOptionsSet(socketInfo->SocketID, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, ptrOpt);
        }

        if(socketInfo->tcpNoDelay != 0U)
        {
            ptrOpt = FC_Uint2VPtr(0xffffffffU); 
        }
        else
        {
            ptrOpt = NULL;
        }
        (void)NET_PRES_SocketOptionsSet(socketInfo->SocketID, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_NODELAY, ptrOpt);

        if(socketInfo->needsSignal != 0U)
        {
            uint16_t sigType = (uint16_t)TCPIP_TCP_SIGNAL_RX_FIN | (uint16_t)TCPIP_TCP_SIGNAL_RX_RST | (uint16_t)TCPIP_TCP_SIGNAL_TX_RST; 
            (void)NET_PRES_SocketSignalHandlerRegister(socketInfo->SocketID, sigType, &TCP_SignalFunction, socketInfo);
        }
    }
    else
    {
        // do nothing
    }

}

/*****************************************************************************
  Function:
    void BerkeleySocketInitialize(const TCPIP_STACK_MODULE_CTRL* const stackData,
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
bool BerkeleySocketInitialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const void* initData)
{
    int16_t s;
    struct BSDSocket *bsocket;
    // OSAL_CRITSECT_DATA_TYPE intStatus;

    { // This needs hard protection  An Automutext would be best here
        if (InitCount++ != 0)
        {
            return true;
        }
        if(OSAL_SEM_Create(&bsdSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_SUCCESS)
        {
            InitCount--;
            return false;
        }
    }

    bsdSignalHandle = NULL;
    const BERKELEY_MODULE_CONFIG* berkeleyData = (const BERKELEY_MODULE_CONFIG*)initData;

    if (berkeleyData != NULL)
    {
        BSD_SOCKET_COUNT = (int16_t)berkeleyData->maxSockets;
    }
    else
    {
        BSD_SOCKET_COUNT = MAX_BSD_SOCKETS;
    }
    bsdApiHeapH = stackData->memH;


    BSDSocketArray = TCPIP_HEAP_Calloc(bsdApiHeapH, (size_t)BSD_SOCKET_COUNT, sizeof (struct BSDSocket));
    if (BSDSocketArray == NULL)
    {
        if (OSAL_SEM_Delete(&bsdSemaphore) != OSAL_RESULT_SUCCESS)
        {
            // SYS_DEBUG message
        }
        InitCount--;
        return false;
    }
    
    bsdSignalHandle  =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_BSD_Task, 0);
    if(bsdSignalHandle == NULL)
    {   // failed
        (void)TCPIP_HEAP_Free(bsdApiHeapH, BSDSocketArray);
        (void)OSAL_SEM_Delete(&bsdSemaphore);
        return false;
    }

    for (s = 0; s < BSD_SOCKET_COUNT; s++)
    {
        bsocket = (struct BSDSocket *) &BSDSocketArray[s];
        bsocket->bsdState = (uint8_t)SKT_CLOSED;
        bsocket->SocketID = NET_PRES_INVALID_SOCKET;
        bsocket->rcvBufSize = 0;
        bsocket->sndBufSize = 0;
        bsocket->rcvTmOut = 0;
        bsocket->sndTmOut = 0;
        bsocket->lingerTmo = 0U;

        bsocket->w = 0;
    }

    return true;
}

/*****************************************************************************
  Function:
    void BerkeleySocketDeinitialize(void)

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
void BerkeleySocketDeinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)
{
    int16_t s;
    struct BSDSocket *bsocket;

    if (InitCount == 0)
    {
        return;
    }

    --InitCount;

    if (InitCount != 0)
    {   // still active
        return;
    }

    bsocket = BSDSocketArray;
    for (s = 0; s < BSD_SOCKET_COUNT; s++)
    {
        if (bsocket->SocketID != NET_PRES_INVALID_SOCKET)
        {
            (void)closesocket(s);  // Too deep to lock fully
            bsocket->bsdState = (uint8_t)SKT_CLOSED;
            bsocket->SocketID = NET_PRES_INVALID_SOCKET;
        }
        bsocket++;
    }

    TCPIPStackSignalHandlerDeregister(bsdSignalHandle);
    bsdSignalHandle = NULL;
    (void)TCPIP_HEAP_Free(bsdApiHeapH, BSDSocketArray);

    if (OSAL_SEM_Delete(&bsdSemaphore) != OSAL_RESULT_SUCCESS)
    {
        // SYS_DEBUG message
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_BSD_Task(void)
{
    // received an asynchronous message
    // check for the sockets that need closing
    int16_t ix;
    int asyncSignals = 0;
    struct BSDSocket* pBSkt = BSDSocketArray;
    for(ix = 0; ix < BSD_SOCKET_COUNT; ix++)
    {
        if(pBSkt->bsdState == (uint8_t)SKT_DISCONNECTING && pBSkt->needsClose == 1U)
        {
            NET_PRES_SocketClose(pBSkt->SocketID);
            pBSkt->bsdState = (uint8_t)SKT_CLOSED;
            pBSkt->SocketID = NET_PRES_INVALID_SOCKET;
            asyncSignals++;
        }
        pBSkt++;
    }

    while(asyncSignals-- != 0)
    {   // clear the signals
        (void)TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC); 
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
    BerkeleySocketInitialize function should be called.

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
SOCKET socket (int af, int type, int protocol)
{
    SOCKET s;

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IPV6)
    if (af != AF_INET && af != AF_INET6)
    {
        errno = EAFNOSUPPORT;
        return SOCKET_ERROR;
    }
#elif defined(TCPIP_STACK_USE_IPV4)
    if (af != AF_INET)
    {
        errno = EAFNOSUPPORT;
        return SOCKET_ERROR;
    }
#elif defined(TCPIP_STACK_USE_IPV6)
    if (af != AF_INET6)
    {
        errno = EAFNOSUPPORT;
        return SOCKET_ERROR;
    }
#else
    TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
    errno = EAFNOSUPPORT;
    return SOCKET_ERROR;
#endif

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
            // do nothing
            break;
        }
    }

    if (OSAL_SEM_Pend(&bsdSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
    {
        // SYS_DEBUG message
    }

    struct BSDSocket *bsocket = BSDSocketArray;
    for (s = 0; s < BSD_SOCKET_COUNT; s++)
    {
        if (bsocket->bsdState == (uint8_t)SKT_CLOSED) //socket not in use
        {
            bsocket->SocketType = (uint16_t)type;
            bsocket->localIPv4 = IP_ADDR_ANY; // updated by bind()
#if defined(TCPIP_STACK_USE_IPV6)
            bsocket->localIPv6[0] = IP_ADDR_ANY; // updated by bind()
#endif

            bsocket->addressFamily = (uint8_t)af;
            if (type == SOCK_DGRAM && protocol == IPPROTO_UDP)
            {
                bsocket->bsdState = (uint8_t)SKT_CREATED;
                if (OSAL_SEM_Post(&bsdSemaphore) != OSAL_RESULT_SUCCESS)
                {
                    // SYS_DEBUG message
                }
                return s;
            }
            else if (type == SOCK_STREAM && protocol == IPPROTO_TCP)
            {
                bsocket->bsdState = (uint8_t)SKT_CREATED;
                if (OSAL_SEM_Post(&bsdSemaphore) != OSAL_RESULT_SUCCESS)
                {
                    // SYS_DEBUG message
                }
                return s;
            }
            else
            {
                errno = EINVAL;
                if (OSAL_SEM_Post(&bsdSemaphore) != OSAL_RESULT_SUCCESS)
                {
                    // SYS_DEBUG message
                }
                return SOCKET_ERROR;
            }
        }
        bsocket++;
    }

    errno = EMFILE;
    if (OSAL_SEM_Post(&bsdSemaphore) != OSAL_RESULT_SUCCESS)
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
    IP_MULTI_ADDRESS lAddr;
    uint16_t lPort;

    (void)memset(lAddr.v6Add.v, 0, sizeof(lAddr));

    struct BSDSocket *bsocket = F_getBsdSocket(s);

    if (bsocket == NULL)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    if (bsocket->bsdState != (uint8_t)SKT_CREATED) //only work with recently created socket
    {   
        errno = EINVAL;
        return SOCKET_ERROR;
    }

    if ((unsigned int) namelen < sizeof (struct sockaddr_in)) {
        errno = EFAULT;
        return SOCKET_ERROR;
    }

#if defined(TCPIP_STACK_USE_IPV6)
    if (bsocket->addressFamily == (uint8_t)AF_INET)
#endif
    {
        const struct sockaddr_in *local_addr = FC_CSockAddr2In(name);
        lAddr.v4Add.Val = local_addr->sin_addr.S_un.S_addr == IP_ADDR_ANY ? 0U : local_addr->sin_addr.S_un.S_addr;
        lPort = local_addr->sin_port;
    }
#if defined(TCPIP_STACK_USE_IPV6)
    else
    {
        const struct sockaddr_in6 *local_addr6 = FC_CSockAddr2In6(name);
        const uint8_t* sin6 = (const uint8_t*)local_addr6 + offsetof(struct sockaddr_in6, sin6_addr);
        const struct  in6_addr* sin6_addr = FC_CU8Ptr2In6(sin6);
        (void)memcpy(lAddr.v6Add.v, sin6_addr->in6_u.u6_addr8, sizeof(struct in6_addr));

        if (lAddr.v6Add.d32[0] == IP_ADDR_ANY)
        {
            lAddr.v6Add.d32[0] = 0;
        }
        lPort = local_addr6->sin6_port;
    }
#endif
    if (bsocket->SocketType == (uint16_t)SOCK_DGRAM)
    {
        if (bsocket->SocketID == INVALID_SOCKET)
        { // create server socket

#if defined(TCPIP_STACK_USE_IPV6)
            if (bsocket->addressFamily == (uint8_t)AF_INET)
            {
#endif
                bsocket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_DATAGRAM_SERVER, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV4, lPort, (lAddr.v4Add.Val == 0U ? NULL : FC_MultiAdd2PresAdd(&lAddr)), NULL);
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                bsocket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_DATAGRAM_SERVER, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV6, lPort, (lAddr.v6Add.d32[0] == 0U ? NULL : FC_MultiAdd2PresAdd(&lAddr)), NULL);
            }
#endif
            if (bsocket->SocketID == INVALID_UDP_SOCKET)
            {
                errno = ENOBUFS;
                return SOCKET_ERROR;
            }
            F_cfgBsdSocket(bsocket);

        } 
#if defined(TCPIP_STACK_USE_IPV6)
        if (bsocket->addressFamily == (uint8_t)AF_INET)
        {
#endif
            const struct sockaddr_in* s_addr_in = FC_CSockAddr2In(name); 
            if (!NET_PRES_SocketBind(bsocket->SocketID, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV4, lPort, ((s_addr_in->sin_addr.S_un.S_addr == IP_ADDR_ANY) ? NULL : FC_MultiAdd2PresAdd(&lAddr))))
            {
                errno = EINVAL;
                return SOCKET_ERROR;
            }
#if defined(TCPIP_STACK_USE_IPV6)
        }
        else
        {
            const struct sockaddr_in6* s_addr_in = FC_CSockAddr2In6(name); 
            if(!NET_PRES_SocketBind(bsocket->SocketID, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV6, lPort, ((s_addr_in->sin6_addr.in6_u.u6_addr32[0] == IP_ADDR_ANY) ? NULL : FC_MultiAdd2PresAdd(&lAddr))))
            {
                errno = EINVAL;
                return SOCKET_ERROR;
            }
        }
#endif
    }

    bsocket->localPort = lPort;
#if defined(TCPIP_STACK_USE_IPV6)
            if (bsocket->addressFamily == (uint8_t)AF_INET)
            {
#endif
                bsocket->localIPv4 = lAddr.v4Add.Val;
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                bsocket->localIPv6[0] = lAddr.v6Add.d32[0];
                bsocket->localIPv6[1] = lAddr.v6Add.d32[1];
                bsocket->localIPv6[2] = lAddr.v6Add.d32[2];
                bsocket->localIPv6[3] = lAddr.v6Add.d32[3];
            }
#endif
    bsocket->bsdState = (uint8_t)SKT_BOUND;
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
    int16_t socketcount;
    unsigned char assigned;
    IP_MULTI_ADDRESS lclAddr;

    ps = F_getBsdSocket(s);
    if(ps == NULL)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    if (ps->SocketType != (uint16_t)SOCK_STREAM)
    {
        errno = EOPNOTSUPP;
        return SOCKET_ERROR;
    }

    if (ps->bsdState == (uint8_t)SKT_BSD_LISTEN)
    {
        backlog = (int)ps->backlog;
    }

    if ((ps->bsdState != (uint8_t)SKT_BOUND) && (ps->bsdState != (uint8_t)SKT_BSD_LISTEN))
    {
        errno = EINVAL;
        return SOCKET_ERROR;
    }
    (void)memset(lclAddr.v6Add.v, 0, sizeof(lclAddr));

    while (backlog-- != 0)
    {
        assigned = 0U;
        for (socketcount = 0; socketcount < BSD_SOCKET_COUNT; socketcount++)
        {
            if (BSDSocketArray[socketcount].bsdState != (uint8_t)SKT_CLOSED)
            {
                continue;
            }

#if defined(TCPIP_STACK_USE_IPV6)
            if (ps->addressFamily == (uint8_t)AF_INET)
            {
#endif
                if (ps->localIPv4 != 0U)
                {
                    lclAddr.v4Add.Val = ps->localIPv4;
                }
                clientSockID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_SERVER, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV4, ps->localPort, (lclAddr.v4Add.Val == 0U ? NULL : FC_MultiAdd2PresAdd(&lclAddr)), NULL);
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                if (ps->localIPv6[0] != 0U)
                {
                    lclAddr.v6Add.d32[0] = ps->localIPv6[0];
                    lclAddr.v6Add.d32[1] = ps->localIPv6[1];
                    lclAddr.v6Add.d32[2] = ps->localIPv6[2];
                    lclAddr.v6Add.d32[3] = ps->localIPv6[3];
                }
                clientSockID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_SERVER, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV6, ps->localPort, (lclAddr.v6Add.d32[0] == 0U ? NULL : FC_MultiAdd2PresAdd(&lclAddr)), NULL);
            }
#endif

            if (clientSockID == INVALID_SOCKET)
            {
                errno = ENOBUFS;
                return SOCKET_ERROR;
            }

            cs = BSDSocketArray + socketcount;
            cs->SocketID = clientSockID;
            cs->bsdState = (uint8_t)SKT_LISTEN;
            cs->isServer = 1U;
            cs->needsSignal = 1U;
            cs->localPort = ps->localPort;
            cs->SocketType = (uint16_t)SOCK_STREAM;
#if defined(TCPIP_STACK_USE_IPV4)
            if (ps->addressFamily == (uint8_t)AF_INET)
            {
                cs->localIPv4 = ps->localIPv4;
            }
#endif
#if defined(TCPIP_STACK_USE_IPV6)
            if (ps->addressFamily == (uint8_t)AF_INET6)
            {
                cs->localIPv6[0] = ps->localIPv6[0];
                cs->localIPv6[1] = ps->localIPv6[1];
                cs->localIPv6[2] = ps->localIPv6[2];
                cs->localIPv6[3] = ps->localIPv6[3];
            }
#endif            
            cs->parentId = (uint16_t)s; 

            F_cfgBsdSocket(cs);


            // Clear the first reset flag
            (void)NET_PRES_SocketWasReset(clientSockID);

            assigned = 1U;
            ps->bsdState = (uint8_t)SKT_BSD_LISTEN;
            ps->backlog = (uint16_t)backlog;

            break;
        }
        if (assigned == 0U)
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
    int16_t sockCount;
    TCP_SOCKET hTCP;

    pListenSock = F_getBsdSocket(s); /* Get the pointer to listening server socket */

    if (pListenSock == NULL)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    if (pListenSock->bsdState != (uint8_t)SKT_BSD_LISTEN)
    {
        errno = EINVAL;
        return SOCKET_ERROR;
    }
    if (pListenSock->SocketType != (uint16_t)SOCK_STREAM)
    {
        errno = EOPNOTSUPP;
        return SOCKET_ERROR;
    }

    for (sockCount = 0; sockCount < BSD_SOCKET_COUNT; sockCount++)
    {
        if (BSDSocketArray[sockCount].bsdState != (uint8_t)SKT_LISTEN)
        {
            continue;
        }

        if (BSDSocketArray[sockCount].localPort != pListenSock->localPort)
        {
            continue;
        }

        hTCP = BSDSocketArray[sockCount].SocketID;

        // We don't care about connections and disconnections before we can
        // process them, so clear the reset flag
        (void)NET_PRES_SocketWasReset(hTCP);

        if (NET_PRES_SocketIsConnected(hTCP))
        {
            (void)NET_PRES_SocketInfoGet(hTCP, &tcpSockInfo);
            if (addr != NULL)
            {
                if (addrlen != NULL)
                {
#if defined(TCPIP_STACK_USE_IPV6)
                    if (pListenSock->addressFamily == (uint8_t)AF_INET)
                    {
#endif
                        struct sockaddr_in *addrRemote;
                        if ((unsigned int) *addrlen < sizeof (struct sockaddr_in))
                        {
                            errno = EFAULT;
                            return SOCKET_ERROR;
                        }
                        addrRemote = FC_SockAddr2In(addr);
                        addrRemote->sin_addr.S_un.S_addr = tcpSockInfo.remoteIPaddress.v4Add.Val == IP_ADDR_ANY ? 0U: tcpSockInfo.remoteIPaddress.v4Add.Val;
                        addrRemote->sin_port = tcpSockInfo.remotePort;
                        *addrlen = (int)sizeof (struct sockaddr_in);
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
                        addrRemote = FC_SockAddr2In6(addr);
                        addrRemote->sin6_addr.in6_u.u6_addr32[0] = tcpSockInfo.remoteIPaddress.v6Add.d32[0];
                        addrRemote->sin6_addr.in6_u.u6_addr32[1] = tcpSockInfo.remoteIPaddress.v6Add.d32[1];
                        addrRemote->sin6_addr.in6_u.u6_addr32[2] = tcpSockInfo.remoteIPaddress.v6Add.d32[2];
                        addrRemote->sin6_addr.in6_u.u6_addr32[3] = tcpSockInfo.remoteIPaddress.v6Add.d32[3];
                        addrRemote->sin6_port = tcpSockInfo.remotePort;
                        *addrlen = (int)sizeof (struct sockaddr_in6);
                    }
#endif
                }
            }
            BSDSocketArray[sockCount].remotePort = tcpSockInfo.remotePort;
#if defined(TCPIP_STACK_USE_IPV6)
            if (tcpSockInfo.addressType == IP_ADDRESS_TYPE_IPV4)
            {
#endif
                BSDSocketArray[sockCount].remoteIPv4 = tcpSockInfo.remoteIPaddress.v4Add.Val;
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                BSDSocketArray[sockCount].remoteIPv6[0] = tcpSockInfo.remoteIPaddress.v6Add.d32[0];
                BSDSocketArray[sockCount].remoteIPv6[1] = tcpSockInfo.remoteIPaddress.v6Add.d32[1];
                BSDSocketArray[sockCount].remoteIPv6[2] = tcpSockInfo.remoteIPaddress.v6Add.d32[2];
                BSDSocketArray[sockCount].remoteIPv6[3] = tcpSockInfo.remoteIPaddress.v6Add.d32[3];
            }
#endif            
            BSDSocketArray[sockCount].bsdState = (uint8_t)SKT_EST;
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
#if defined(TCPIP_STACK_USE_IPV4)
    struct sockaddr_in *addr;
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IPV6)
    IP_MULTI_ADDRESS localAddr;
    struct sockaddr_in6 * addr6;
#endif
    uint16_t remotePort = 0;    
    IP_MULTI_ADDRESS remoteIP;
    int res;

    struct BSDSocket *bsocket = F_getBsdSocket(s);
    if (bsocket == NULL || bsocket->bsdState < (uint8_t)SKT_CREATED) 
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    if ((unsigned int) namelen < sizeof (struct sockaddr_in)) 
    {
        errno = EFAULT;
        return SOCKET_ERROR;
    }

    (void)memset(remoteIP.v6Add.v, 0, sizeof(remoteIP));
#if defined(TCPIP_STACK_USE_IPV6)
    (void)memset(localAddr.v6Add.v, 0, sizeof(localAddr));
#endif  // defined(TCPIP_STACK_USE_IPV6)

    while(true)
    {
#if defined(TCPIP_STACK_USE_IPV4)
        if (bsocket->addressFamily == (uint8_t)AF_INET)
        {
            addr = FC_SockAddr2In(name);
            remotePort = addr->sin_port;
            remoteIP.v4Add.Val = addr->sin_addr.S_un.S_addr == IP_ADDR_ANY ? 0U: addr->sin_addr.S_un.S_addr;

            if (remoteIP.v4Add.Val == 0u || remotePort == 0u)
            {
                errno = EINVAL;
                return SOCKET_ERROR;
            }
            errno = 0;
            if (bsocket->SocketType == (uint16_t)SOCK_STREAM) 
            {
                res = connect_stream_v4(bsocket, &remoteIP, remotePort, s);
                if(errno != 0)
                {
                    res = SOCKET_ERROR;
                }
                return res;
            } 
            else 
            {
                res = connect_dgram_v4(bsocket, &remoteIP, remotePort);
                if(errno != 0)
                {
                    res = SOCKET_ERROR;
                }
                return res;
            }
        }
#endif  // defined(TCPIP_STACK_USE_IPV4)


#if defined(TCPIP_STACK_USE_IPV6)
        if (bsocket->addressFamily == (uint8_t)AF_INET6)
        {
            addr6 = FC_SockAddr2In6(name);
            remotePort = addr6->sin6_port;
            uint8_t* sin6 = (uint8_t*)addr6 + offsetof(struct sockaddr_in6, sin6_addr);
            struct  in6_addr* sin6_addr = FC_U8Ptr2In6(sin6);
            (void)memcpy(remoteIP.v6Add.v, sin6_addr->in6_u.u6_addr8, sizeof(IPV6_ADDR));
            localAddr.v6Add.d32[0] = bsocket->localIPv6[0];
            localAddr.v6Add.d32[1] = bsocket->localIPv6[1];
            localAddr.v6Add.d32[2] = bsocket->localIPv6[2];
            localAddr.v6Add.d32[3] = bsocket->localIPv6[3];

            if (remoteIP.v6Add.d32[0] == 0u || remotePort == 0u)
            {
                errno = EINVAL;
                return SOCKET_ERROR;
            }
            errno = 0;
            if (bsocket->SocketType == (uint16_t)SOCK_STREAM) 
            {
                res = connect_stream_v6(bsocket, &localAddr, &remoteIP, remotePort, s);
                if(errno != 0)
                {
                    res = SOCKET_ERROR;
                }
                return res;
            } 
            else 
            {
                res = connect_dgram_v6(bsocket, &remoteIP, remotePort);
                if(errno != 0)
                {
                    res = SOCKET_ERROR;
                }
                return res;
            }
        }
#endif // defined(TCPIP_STACK_USE_IPV6)

        errno = EAFNOSUPPORT;
        return SOCKET_ERROR;
    }
}

#if defined(TCPIP_STACK_USE_IPV4)
// connects an IPv4 stream socket
static int connect_stream_v4(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* remoteIP, uint16_t remotePort, SOCKET s)
{
    int res;

    errno = 0;
    switch (bsocket->bsdState) 
    {
        case (uint8_t)SKT_EST:
            res = 0; // already established
            break;

        case (uint8_t)SKT_IN_PROGRESS:
            if (TCP_SocketWasReset(s)) 
            {
                errno = ECONNREFUSED;
                res = SOCKET_ERROR;
            }
            else if (TCP_SocketWasDisconnected(s, true))
            {
                errno = ECONNREFUSED;
                res = SOCKET_ERROR;
            }
            else if (!NET_PRES_SocketIsConnected(bsocket->SocketID)) 
            {
                errno = EINPROGRESS;
                res = SOCKET_ERROR;
            }
            else
            {
                bsocket->bsdState = (uint8_t)SKT_EST;
                res = 0; //success
            }
            break;

        case (uint8_t)SKT_CREATED:
        case (uint8_t)SKT_BOUND:
            res = connect_bound_v4(bsocket, remoteIP, remotePort);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        default:
            errno = ECONNRESET;
            res = SOCKET_ERROR;
            break;
    }

    return res;
}

static int connect_bound_v4(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* remoteIP, uint16_t remotePort)
{
    IP_MULTI_ADDRESS localAddr;
    IPV4_ADDR remoteAddr;

    remoteAddr.Val = 0;

    while(true)
    {
        if (bsocket->localIPv4 == IP_ADDR_ANY)
        {
            bsocket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_CLIENT, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV4, remotePort, FC_MultiAdd2PresAdd(remoteIP), NULL);
            if(bsocket->SocketID == INVALID_SOCKET)
            {
                errno = ENOBUFS;
                break;
            }
        }
        else
        {
            bsocket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_CLIENT, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV4, 0, NULL, NULL);
            if(bsocket->SocketID == INVALID_SOCKET)
            {
                errno = ENOBUFS;
                break;
            }

            localAddr.v4Add.Val = bsocket->localIPv4; 
            if (!NET_PRES_SocketBind(bsocket->SocketID, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV4, 0, FC_MultiAdd2PresAdd(&localAddr)))
            {
                NET_PRES_SocketClose(bsocket->SocketID);
                bsocket->SocketID = INVALID_SOCKET;
                errno = EADDRINUSE;
                break;
            }

            if (!NET_PRES_SocketRemoteBind(bsocket->SocketID, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV4, remotePort, FC_MultiAdd2PresAdd(remoteIP)))
            {
                NET_PRES_SocketClose(bsocket->SocketID);
                bsocket->SocketID = INVALID_SOCKET;
                errno = EADDRINUSE;
                break;
            }

            if (!NET_PRES_SocketConnect(bsocket->SocketID))
            {
                errno = EADDRINUSE;
                NET_PRES_SocketClose(bsocket->SocketID);
                bsocket->SocketID = INVALID_SOCKET;
                break;
            }

            // success
            if (bsocket->SocketID == INVALID_SOCKET) 
            {
                errno = ENOBUFS;
                break;
            }

            F_cfgBsdSocket(bsocket);
            // Clear the first reset flag
            (void)NET_PRES_SocketWasReset(bsocket->SocketID);

            localAddr.v4Add.Val  = bsocket->localIPv4 == IP_ADDR_ANY ? 0U : bsocket->localIPv4;
            remoteAddr.Val = remoteIP->v4Add.Val;
            (void)TCPIP_TCP_SocketNetSet(bsocket->nativeSkt, TCPIP_IPV4_SelectSourceInterface(NULL, &remoteAddr, &localAddr.v4Add, true), false);

            bsocket->isServer = 0U;
            bsocket->bsdState = (uint8_t)SKT_IN_PROGRESS;
            errno = EINPROGRESS;
            break;
        }
    }

    return SOCKET_ERROR;
}

static int connect_dgram_v4(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* remoteIP, uint16_t remotePort)
{
        // open the socket
    if(bsocket->bsdState == (uint8_t)SKT_CREATED) 
    {
        bsocket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_DATAGRAM_CLIENT, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV4, remotePort, FC_MultiAdd2PresAdd(remoteIP), NULL);

        if (bsocket->SocketID == INVALID_UDP_SOCKET) 
        {
            errno = ENOBUFS;
            return SOCKET_ERROR;
        }

        F_cfgBsdSocket(bsocket);
        bsocket->bsdState = (uint8_t)SKT_BOUND;
    }

    if (bsocket->bsdState != (uint8_t)SKT_BOUND) 
    {
        errno = EINVAL;
        return SOCKET_ERROR;
    }

    // UDP: remote port is used as a filter. Need to call connect when using
    // send/recv calls. No need to call 'connect' if using sendto/recvfrom
    // calls.
    bsocket->remotePort = remotePort;
    bsocket->remoteIPv4 = remoteIP->v4Add.Val;

    return 0; //success
}
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
// connects an IPv6 stream socket
static int connect_stream_v6(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* pLocalAddr6, IP_MULTI_ADDRESS* pRemoteIP6, uint16_t remotePort, SOCKET s)
{
    int res;

    errno = 0;
    switch (bsocket->bsdState) 
    {
        case (uint8_t)SKT_EST:
            res = 0; // already established
            break;

        case (uint8_t)SKT_IN_PROGRESS:
            if (TCP_SocketWasReset(s)) 
            {
                errno = ECONNREFUSED;
                res = SOCKET_ERROR;
            }
            else if (TCP_SocketWasDisconnected(s, true))
            {
                errno = ECONNREFUSED;
                res = SOCKET_ERROR;
            }
            else if (!NET_PRES_SocketIsConnected(bsocket->SocketID)) 
            {
                errno = EINPROGRESS;
                res = SOCKET_ERROR;
            }
            else
            {
                bsocket->bsdState = (uint8_t)SKT_EST;
                res = 0; //success
            }
            break;

        case (uint8_t)SKT_CREATED:
        case (uint8_t)SKT_BOUND:
            res = connect_bound_v6(bsocket, pLocalAddr6, pRemoteIP6, remotePort);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        default:
            errno = ECONNRESET;
            res = SOCKET_ERROR;
            break;
    }

    return res;
}

static int connect_bound_v6(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* pLocalAddr6, IP_MULTI_ADDRESS* pRemoteIP6, uint16_t remotePort)
{
    while(true)
    {
        if (bsocket->localIPv6[0] == IP_ADDR_ANY)
        {
            bsocket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_CLIENT, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV6, remotePort, FC_MultiAdd2PresAdd(pRemoteIP6), NULL);
            if(bsocket->SocketID == INVALID_SOCKET)
            {
                errno = ENOBUFS;
                break;
            }
        }
        else
        {
            bsocket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_CLIENT, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV6, 0, NULL, NULL);
            if(bsocket->SocketID == INVALID_SOCKET)
            {
                errno = ENOBUFS;
                break;
            }
            if (!NET_PRES_SocketBind(bsocket->SocketID, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV6, 0, FC_MultiAdd2PresAdd(pLocalAddr6)))
            {
                NET_PRES_SocketClose(bsocket->SocketID);
                bsocket->SocketID = INVALID_SOCKET;
                errno = EADDRINUSE;
                break;
            }
            if (!NET_PRES_SocketRemoteBind(bsocket->SocketID, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV6, remotePort, FC_MultiAdd2PresAdd(pRemoteIP6)))
            {
                NET_PRES_SocketClose(bsocket->SocketID);
                bsocket->SocketID = INVALID_SOCKET;
                errno = EADDRINUSE;
                break;
            }
            if (!NET_PRES_SocketConnect(bsocket->SocketID))
            {
                NET_PRES_SocketClose(bsocket->SocketID);
                bsocket->SocketID = INVALID_SOCKET;
                errno = EADDRINUSE;
                break;
            }
        }

        if (bsocket->SocketID == INVALID_SOCKET) 
        {
            errno = ENOBUFS;
            break;
        }
        F_cfgBsdSocket(bsocket);
        // Clear the first reset flag
        (void)NET_PRES_SocketWasReset(bsocket->SocketID);

        bsocket->isServer = 0U;
        bsocket->bsdState = (uint8_t)SKT_IN_PROGRESS;
        errno = EINPROGRESS;
        break;
    }

    return SOCKET_ERROR;
}

static int connect_dgram_v6(struct BSDSocket *bsocket, IP_MULTI_ADDRESS* pRemoteIP6, uint16_t remotePort)
{
        // open the socket
    if(bsocket->bsdState == (uint8_t)SKT_CREATED) 
    {
        bsocket->SocketID = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_DATAGRAM_CLIENT, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV6, remotePort, FC_MultiAdd2PresAdd(pRemoteIP6), NULL);

        if (bsocket->SocketID == INVALID_UDP_SOCKET) 
        {
            errno = ENOBUFS;
            return SOCKET_ERROR;
        }

        F_cfgBsdSocket(bsocket);
        bsocket->bsdState = (uint8_t)SKT_BOUND;
    }

    if (bsocket->bsdState != (uint8_t)SKT_BOUND) 
    {
        errno = EINVAL;
        return SOCKET_ERROR;
    }

    // UDP: remote port is used as a filter. Need to call connect when using
    // send/recv calls. No need to call 'connect' if using sendto/recvfrom
    // calls.
    bsocket->remotePort = remotePort;

    bsocket->remoteIPv6[0] = pRemoteIP6->v6Add.d32[0];
    bsocket->remoteIPv6[1] = pRemoteIP6->v6Add.d32[1];
    bsocket->remoteIPv6[2] = pRemoteIP6->v6Add.d32[2];
    bsocket->remoteIPv6[3] = pRemoteIP6->v6Add.d32[3];

    return 0; //success
}
#endif  // defined(TCPIP_STACK_USE_IPV6)

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
    IP_MULTI_ADDRESS remoteIp;
    uint16_t wRemotePort;
    struct sockaddr_in local; 
#if defined(TCPIP_STACK_USE_IPV6)
    struct sockaddr_in6 local6;
#endif

    struct BSDSocket *bsocket = F_getBsdSocket(s);

    if (bsocket == NULL || bsocket->bsdState == (uint8_t)SKT_CLOSED)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    // Handle special case were 0 return value is okay
    if (len == 0)
    {
        return 0;
    }

    (void)memset(remoteIp.v6Add.v, 0, sizeof(remoteIp));

    if (bsocket->SocketType == (uint16_t)SOCK_DGRAM) //UDP
    {
        // Decide the destination IP address and port
#if defined(TCPIP_STACK_USE_IPV6)
        if (bsocket->addressFamily == (uint8_t)AF_INET)
        {
#endif
            remoteIp.v4Add.Val = bsocket->remoteIPv4;
#if defined(TCPIP_STACK_USE_IPV6)
        }
        else
        {
            remoteIp.v6Add.d32[0] = bsocket->remoteIPv6[0];
            remoteIp.v6Add.d32[1] = bsocket->remoteIPv6[1];
            remoteIp.v6Add.d32[2] = bsocket->remoteIPv6[2];
            remoteIp.v6Add.d32[3] = bsocket->remoteIPv6[3];
        }
#endif
        wRemotePort = bsocket->remotePort;
        if (to != NULL)
        {
#if defined(TCPIP_STACK_USE_IPV6)
            if (bsocket->addressFamily == (uint8_t)AF_INET)
            {
#endif
                if ((unsigned int) tolen < sizeof (struct sockaddr_in))
                {
                    errno = EFAULT;
                    return SOCKET_ERROR;
                }
                const struct sockaddr_in* s_addr_in = FC_CSockAddr2In(to);
                wRemotePort = s_addr_in->sin_port;
                remoteIp.v4Add.Val = s_addr_in->sin_addr.s_addr;
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                if ((unsigned int) tolen < sizeof (struct sockaddr_in6))
                {
                    errno = EFAULT;
                    return SOCKET_ERROR;
                }
                const struct sockaddr_in6* addr6 = FC_CSockAddr2In6(to);
                wRemotePort = addr6->sin6_port;
                const uint8_t* sin6 = (const uint8_t*)addr6 + offsetof(struct sockaddr_in6, sin6_addr);
                const struct  in6_addr* sin6_addr = FC_CU8Ptr2In6(sin6);
                (void)memcpy(remoteIp.v6Add.v, sin6_addr->in6_u.u6_addr8, sizeof(IPV6_ADDR));
            }
#endif

            // Implicitly bind the socket if it isn't already
            if (bsocket->bsdState == (uint8_t)SKT_CREATED)
            {
#if defined(TCPIP_STACK_USE_IPV6)
                if (bsocket->addressFamily == (uint8_t)AF_INET)
                {
#endif
                    (void)memset(&local, 0, sizeof (local));
                    local.sin_addr.s_addr = IP_ADDR_ANY;
                    if (bind(s, FC_SockIn2Addr(&local), (int)sizeof (local)) == SOCKET_ERROR)
                    {
                        return SOCKET_ERROR;
                    }
#if defined(TCPIP_STACK_USE_IPV6)
                }
                else
                {
                    (void)memset(&local6, 0x00, sizeof(local6));
                    local6.sin6_addr.in6_u.u6_addr32[0] = IP_ADDR_ANY;
                    if (bind(s, FC_SockIn62Addr(&local6), (int)sizeof (local6)) == SOCKET_ERROR)
                    {
                        return SOCKET_ERROR;
                    }
                }
#endif
            }
        }

        UDP_SOCKET_INFO udpSockInfo;
        (void)NET_PRES_SocketInfoGet(bsocket->SocketID, &udpSockInfo);
        UDP_SOCKET uSkt = bsocket->nativeSkt;

#if defined(TCPIP_STACK_USE_IPV6)
        if (bsocket->addressFamily == (uint8_t)AF_INET)
        {
#endif
            if (remoteIp.v4Add.Val == IP_ADDR_ANY)
            {
                (void)TCPIP_UDP_BcastIPV4AddressSet(uSkt, UDP_BCAST_NETWORK_LIMITED, NULL);
            }
            else
            { // Set the remote IP and MAC address if it is different from what we already have stored in the UDP socket
                if (udpSockInfo.remoteIPaddress.v4Add.Val != remoteIp.v4Add.Val)
                {
                    (void)TCPIP_UDP_DestinationIPAddressSet(uSkt, IP_ADDRESS_TYPE_IPV4, &remoteIp);
                }
            }
            // Set the proper remote port
            (void)TCPIP_UDP_DestinationPortSet(uSkt, wRemotePort);
#if defined(TCPIP_STACK_USE_IPV6)
        }
        else
        {
            if ((udpSockInfo.remoteIPaddress.v6Add.d32[0] != remoteIp.v6Add.d32[0]) ||
                (udpSockInfo.remoteIPaddress.v6Add.d32[1] != remoteIp.v6Add.d32[1]) ||
                (udpSockInfo.remoteIPaddress.v6Add.d32[2] != remoteIp.v6Add.d32[2]) ||
                (udpSockInfo.remoteIPaddress.v6Add.d32[3] != remoteIp.v6Add.d32[3]))
            {
                    (void)TCPIP_UDP_DestinationIPAddressSet(uSkt, IP_ADDRESS_TYPE_IPV6, &remoteIp);

            }
            // Set the proper remote port
            (void)TCPIP_UDP_DestinationPortSet(uSkt, wRemotePort);
            // IPv6 client socket requires explicit binding
            (void)TCPIP_UDP_SocketNetSet(uSkt, TCPIP_STACK_NetDefaultGet());            
        }
#endif
        // Select the UDP socket and see if we can write to it
        if (NET_PRES_SocketWriteIsReady(bsocket->SocketID, (uint16_t)len, 0U) != 0U)
        {
            // Write data and send UDP datagram
            size = (int)NET_PRES_SocketWrite(bsocket->SocketID, (const uint8_t*) buf, (uint16_t)len);
            (void)NET_PRES_SocketFlush(bsocket->SocketID);
            return size;
        }
        // just in case there's some old data already in there...
        (void)NET_PRES_SocketFlush(bsocket->SocketID);
    }
    else if (bsocket->SocketType == (uint16_t)SOCK_STREAM) //TCP will only send to the already established socket.
    {
        if (bsocket->bsdState != (uint8_t)SKT_EST)
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
        size = (int)NET_PRES_SocketWrite(bsocket->SocketID, (const uint8_t*) buf, (uint16_t)len);
        if (size != 0)
        {
            return size;
        }
    }
    else
    {
        // do nothing
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

    struct BSDSocket *bsocket = F_getBsdSocket(s);
    if( bsocket == NULL )
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    if(bsocket->SocketType == (uint16_t)SOCK_STREAM) //TCP
    {
        if(bsocket->bsdState != (uint8_t)SKT_EST)
        {
            errno = ENOTCONN;
            return SOCKET_ERROR;
        }

        if(TCP_SocketWasReset(s))
        {
            return 0;
        }
        else if(TCP_SocketWasDisconnected(s, false))
        {
            return 0;
        }
        else
        {
            // continue
        }

        nBytes = (int)NET_PRES_SocketReadIsReady(bsocket->SocketID);
        if(nBytes != 0 && buf != NULL && len != 0)
        {   // copy available data to user buffer
            nBytes = (int)NET_PRES_SocketRead(bsocket->SocketID, (uint8_t*)buf, (uint16_t)len);
        }

        if(nBytes != 0)
        {
            return nBytes;
        }
        errno = EWOULDBLOCK;
        return SOCKET_ERROR;
    }
    else if(bsocket->SocketType == (uint16_t)SOCK_DGRAM) //UDP
    {
        if(bsocket->bsdState != (uint8_t)SKT_BOUND)
        {
            errno = EINVAL;
            return SOCKET_ERROR;
        }

        nBytes = (int)NET_PRES_SocketReadIsReady(bsocket->SocketID);
        if(nBytes != 0 && buf != NULL && len != 0)
        {   // copy available data to user buffer
            nBytes =  (int)NET_PRES_SocketRead(bsocket->SocketID, (uint8_t*)buf, (uint16_t)len);
            if (nBytes <= len)
            {
                // Need to discard the packet now that we're done with it.
                (void)NET_PRES_SocketDiscard(bsocket->SocketID);
            }
        }

        if(nBytes != 0)
        {
            return nBytes;
        }
        errno = EWOULDBLOCK;
        return SOCKET_ERROR;
    }
    else
    {
        // do nothing
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

    struct BSDSocket *bsocket = F_getBsdSocket(s);
    if (bsocket == NULL)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

#if defined(TCPIP_STACK_USE_IPV6)
    if (bsocket->addressFamily == (uint8_t)AF_INET)
    {
#endif
        rem_addr = FC_SockAddr2In(from);
#if defined(TCPIP_STACK_USE_IPV6)
    }
    else
    {
        rem_addr6 = FC_SockAddr2In6(from);
    }
#endif

    if (bsocket->SocketType == (uint16_t)SOCK_DGRAM) //UDP
    {
        // If this BSD socket doesn't have a Microchip UDP socket associated
        // with it yet, then no data can be received and we must not use the
        // bsocket->SocketID parameter, which isn't set yet.
        if (bsocket->bsdState != (uint8_t)SKT_BOUND)
        {
            errno = EINVAL;
            return SOCKET_ERROR;
        }

        if (NET_PRES_SocketReadIsReady(bsocket->SocketID) != 0U)
        {
            // Capture sender information (can change packet to packet)
            if (from != NULL && fromlen != NULL)
            {
#if defined(TCPIP_STACK_USE_IPV6)
                if (bsocket->addressFamily == (uint8_t)AF_INET)
                {
#endif
                    if ((unsigned int) *fromlen >= sizeof (struct sockaddr_in))
                    {
                        UDP_SOCKET_INFO udpSockInfo;
                        (void)NET_PRES_SocketInfoGet(bsocket->SocketID, &udpSockInfo);
                        if (udpSockInfo.addressType == IP_ADDRESS_TYPE_IPV4)
                        {
                            rem_addr->sin_addr.S_un.S_addr = udpSockInfo.sourceIPaddress.v4Add.Val;
                            rem_addr->sin_port = udpSockInfo.remotePort;
                            *fromlen = (int)sizeof (struct sockaddr_in);
                        }
                    }
#if defined(TCPIP_STACK_USE_IPV6)
                }
                else
                {
                    if ((unsigned int) *fromlen >= sizeof (struct sockaddr_in6))
                    {
                        UDP_SOCKET_INFO udpSockInfo;
                        (void)NET_PRES_SocketInfoGet(bsocket->SocketID, &udpSockInfo);
                        if (udpSockInfo.addressType == IP_ADDRESS_TYPE_IPV6)
                        {
                            uint8_t* sin6 = (uint8_t*)rem_addr6 + offsetof(struct sockaddr_in6, sin6_addr);
                            struct  in6_addr* sin6_addr = FC_U8Ptr2In6(sin6);
                            (void)memcpy(sin6_addr->in6_u.u6_addr8, udpSockInfo.remoteIPaddress.v6Add.v, sizeof(IPV6_ADDR));
                            rem_addr6->sin6_port = udpSockInfo.remotePort;
                            *fromlen = (int)sizeof (struct sockaddr_in6);
                        }
                    }
                }
#endif

            }
            nBytes = (int)NET_PRES_SocketRead(bsocket->SocketID, (uint8_t*) buf, (uint16_t)len);
            if (nBytes <= len)
            {
                // Need to discard the packet now that we're done with it.
                (void)NET_PRES_SocketDiscard(bsocket->SocketID);
            }
        }
        else
        {
            nBytes = 0;
        }

        if (nBytes != 0)
        {
            return nBytes;
        }
        errno = EWOULDBLOCK;
        return SOCKET_ERROR;
    }
    else //TCP recieve from already connected socket.
    {
        if (from  != NULL && fromlen != NULL)
        {
            // Capture sender information (will always match socket connection information)
#if defined(TCPIP_STACK_USE_IPV6)
            if (bsocket->addressFamily == (uint8_t)AF_INET)
            {
#endif
                if ((unsigned int) *fromlen >= sizeof (struct sockaddr_in))
                {
                    (void)NET_PRES_SocketInfoGet(bsocket->SocketID, &tcpSockInfo);
                    if (tcpSockInfo.addressType == IP_ADDRESS_TYPE_IPV4)
                    {
                        rem_addr->sin_addr.S_un.S_addr = tcpSockInfo.remoteIPaddress.v4Add.Val;
                        rem_addr->sin_port = tcpSockInfo.remotePort;
                        *fromlen = (int)sizeof (struct sockaddr_in);
                    }
                }
#if defined(TCPIP_STACK_USE_IPV6)
            }
            else
            {
                if ((unsigned int) *fromlen >= sizeof (struct sockaddr_in6))
                {
                    (void)NET_PRES_SocketInfoGet(bsocket->SocketID, &tcpSockInfo);
                    if (tcpSockInfo.addressType == IP_ADDRESS_TYPE_IPV6)
                    {
                        uint8_t* sin6 = (uint8_t*)rem_addr6 + offsetof(struct sockaddr_in6, sin6_addr);
                        struct  in6_addr* sin6_addr = FC_U8Ptr2In6(sin6);
                        (void)memcpy(sin6_addr->in6_u.u6_addr8, tcpSockInfo.remoteIPaddress.v6Add.v, sizeof(IPV6_ADDR));
                        rem_addr6->sin6_port = tcpSockInfo.remotePort;
                        *fromlen = (int)sizeof (struct sockaddr_in6);

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
    calling this function.  The returned name is null-terminated.

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
    const TCPIP_NET_IF* pNetIf;

    if(name == NULL)
    {
        errno = EINVAL;
        return SOCKET_ERROR;
    }

    pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_NetDefaultGet();
    
    wSourceLen = (uint16_t)sizeof(pNetIf->NetBIOSName);
    for(w = 0; w < wSourceLen; w++)
    {
        v = pNetIf->NetBIOSName[w];
        if((v == (uint8_t)' ') || (v == 0u))
        {
            break;
        }
    }
    wSourceLen = w;
    if(namelen < (int)wSourceLen + 1)
    {
        errno = EINVAL;
        return SOCKET_ERROR;
    }

    for(w = 0; w < wSourceLen; w++)
    {
        v = pNetIf->NetBIOSName[w];
        name[w] = (char)v;
    }

    name[wSourceLen] = (char)0;

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
    int16_t i;

    struct BSDSocket *bsocket = F_getBsdSocket(s);

    if (bsocket == NULL)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    if(bsocket->bsdState == (uint8_t)SKT_CLOSED)
    {
        return 0;   // Nothing to do, so return success
    }

    if(bsocket->SocketType == (uint16_t)SOCK_STREAM)
    {
        if(bsocket->bsdState == (uint8_t)SKT_BSD_LISTEN)
        {
            // This is a listerner handle, so when we close it we also should 
            // close all TCP sockets that were opened for backlog processing 
            // but didn't actually get connected
            for(i = 0; i < BSD_SOCKET_COUNT; i++)
            {
                if(BSDSocketArray[i].bsdState != (uint8_t)SKT_LISTEN)
                {
                    continue;
                }

                if(BSDSocketArray[i].localPort == bsocket->localPort)
                {
                    NET_PRES_SocketClose(BSDSocketArray[i].SocketID);
                    bsocket->SocketID = NET_PRES_INVALID_SOCKET;
                    BSDSocketArray[i].bsdState = (uint8_t)SKT_CLOSED;
                }
            }
        }
        else if(bsocket->bsdState >= (uint8_t)SKT_LISTEN)
        {
            // For server sockets, if the parent listening socket is still open, 
            // then return this socket to the queue for future backlog processing.
            if(bsocket->isServer != 0U)
            {
                for(i = 0; i < BSD_SOCKET_COUNT; i++)
                {
                    if(BSDSocketArray[i].bsdState != (uint8_t)SKT_BSD_LISTEN)
                    {
                        continue;
                    }

                    if(BSDSocketArray[i].localPort == bsocket->localPort)
                    {
                        (void)NET_PRES_SocketDisconnect(bsocket->SocketID);
                        
                        // Listener socket is still open, so wait for connection to close
                        // and then return to the listening state so that the user must call accept()
                        // again to reuse this BSD socket
                        // If the other side has already closed, the signal may be already sent. So this is critical!
                        // don't let the TCP/IP thread interfere
                        OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
                        bsocket->bsdState = TCPIP_TCP_IsConnected(bsocket->nativeSkt) ? (uint8_t)SKT_DISCONNECTING : (uint8_t)SKT_LISTEN;
                        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
                        return 0;
                    }
                }
                // If we get down here, then the parent listener socket has 
                // apparently already been closed, so this socket can not be 
                // reused.  Close it complete.
                NET_PRES_SocketClose(bsocket->SocketID);
            }
            else if(bsocket->bsdState != (uint8_t)SKT_DISCONNECTED)   // this is a client socket that isn't already disconnected
            {
                NET_PRES_SocketClose(bsocket->SocketID);
            }
            else
            {
                // do nothing
            }
        }
        else
        {
            // do nothing
        }
    }
    else //udp sockets
    {
        if(bsocket->bsdState == (uint8_t)SKT_BOUND)
        {
            NET_PRES_SocketClose(bsocket->SocketID);
            bsocket->SocketID = NET_PRES_INVALID_SOCKET;
        }
    }

    NET_PRES_SocketClose(bsocket->SocketID);
    bsocket->SocketID = NET_PRES_INVALID_SOCKET;
    bsocket->bsdState = (uint8_t)SKT_CLOSED;
    bsocket->SocketID = INVALID_UDP_SOCKET;
    bsocket->w = 0;
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
    struct BSDSocket *bsocket;
    int16_t i;

    bsocket = BSDSocketArray + s;

    // Nothing to do if reset has already been handled
    if(bsocket->bsdState == (uint8_t)SKT_DISCONNECTED)
    {
        return true;    
    }

    // Find out if a reset has occurred
    if(!NET_PRES_SocketWasReset(bsocket->SocketID))
    {   // Nothing to do if a reset has not occurred
        return false;
    }

    // For server sockets, if the parent listening socket is still open, 
    // then return this socket to the queue for future backlog processing.
    if(bsocket->isServer != 0U)
    {
        for(i = 0; i < BSD_SOCKET_COUNT; i++)
        {
            if(BSDSocketArray[i].bsdState == (uint8_t)SKT_BSD_LISTEN)
            {
                if(BSDSocketArray[i].localPort == bsocket->localPort)
                {   // Listener socket is still open, so just return to the 
                    // listening state so that the user must call accept() again to 
                    // reuse this BSD socket
                    bsocket->bsdState = (uint8_t)SKT_LISTEN;
                    return true;
                }
            }
        }
    }
            
    // If we get down here and the socket was reset, then this socket 
    // should be closed so that no more clients can connect to it.  However, 
    // we can't go to the BSD SKT_CLOSED state directly since the user still 
    // has to call closesocket() with this s SOCKET descriptor first.
    TCPIP_TCP_Abort(bsocket->nativeSkt, false);
    bsocket->bsdState = (uint8_t)SKT_DISCONNECTED;
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
    struct BSDSocket *bsocket;

    bsocket = BSDSocketArray + s;

    if(bsocket->bsdState == (uint8_t)SKT_DISCONNECTED)
    {   // Nothing to do if a disconnect has already been handled
        return true;    
    }

    // Find out if a reset has occurred
    if(!NET_PRES_SocketWasDisconnected(bsocket->SocketID))
    {   // Nothing to do if a reset has not occurred
        return false;
    }

    // For server sockets, if the parent listening socket is still open, 
    // then return this socket to the queue for future backlog processing.
    if(bsocket->isServer == 0U && cliAbort)
    {
        TCPIP_TCP_Abort(bsocket->nativeSkt, false);
        bsocket->bsdState = (uint8_t)SKT_DISCONNECTED;
    }

    return true;
}

static int F_setsockopt_ip(const struct BSDSocket * s, uint32_t option_name, const uint8_t *option_value, uint32_t option_length)
{

    if (option_name == (uint32_t)IP_OPTIONS || option_name == (uint32_t)IP_TOS || option_name == (uint32_t)IP_TTL || option_name == (uint32_t)IP_MULTICAST_IF) 
    {
        // do nothing
    }
    else if(option_name == (uint32_t)IP_MULTICAST_TTL || option_name == (uint32_t)IP_MULTICAST_LOOP || option_name == (uint32_t)IP_ADD_MEMBERSHIP || option_name == (uint32_t)IP_DROP_MEMBERSHIP)
    {
        // do nothing
    } 
    else
    {
        // do nothing
    }

    errno = EOPNOTSUPP;
    return SOCKET_ERROR;
}

static int F_setsockopt_socket(struct BSDSocket * s, uint32_t option_name, const uint8_t *option_value, uint32_t option_length)
{
    union
    {
        const uint8_t*  cu8ptr;
        const uint16_t* cu16ptr;
        const struct linger * cling;
        const bool*  cboolptr;
    }U_CUPTR_OPT;

    int res = 0;
    U_CUPTR_OPT.cu8ptr = option_value;

    switch (option_name)
    {
        case SO_SNDBUF:
        {
            s->sndBufSize = *U_CUPTR_OPT.cu16ptr;
            break;
        }

        case SO_LINGER:
        {
            if (s->SocketType == (uint16_t)SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                res = SOCKET_ERROR;
            }
            else
            {
                const struct linger * ling = U_CUPTR_OPT.cling;
                s->tcpLinger = (ling->l_onoff != 0) ? 1U : 0U;
                s->lingerTmo = (uint16_t)ling->l_linger;
            }
            break;
        }

        case SO_RCVBUF:
        {
            if (s->SocketType == (uint16_t)SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                res = SOCKET_ERROR;
            }
            else
            {
                s->rcvBufSize = *U_CUPTR_OPT.cu16ptr;
            }
            break;
        }

        case SO_BROADCAST:
        {
            if (s->SocketType != (uint16_t)SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                res = SOCKET_ERROR;
            }
            else
            {
                s->udpBcastEnabled = *U_CUPTR_OPT.cboolptr != false ? 1U : 0U;
            }
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
            res = SOCKET_ERROR;
            break;
    }

    if(res == 0)
    {
        F_cfgBsdSocket(s);
    }

    return res;
}

static int F_setsockopt_tcp(struct BSDSocket * s, uint32_t option_name, const uint8_t *option_value, uint32_t option_length)
{
    if (s->SocketType == (uint16_t)SOCK_DGRAM)
    {
        errno = EOPNOTSUPP;
        return SOCKET_ERROR;
    }

    int res = 0;
    switch (option_name)
    {
        case TCP_NODELAY:
        {
            s->tcpNoDelay = *option_value;
        }
        break;

        default:
            errno = EOPNOTSUPP;
            res = SOCKET_ERROR;
            break;
    }

    if(res == 0)
    {
        F_cfgBsdSocket(s);
    }
    return res;

}

static int F_setsockopt_ipv6(struct BSDSocket * s, uint32_t option_name, const uint8_t *option_value, uint32_t option_length)
{
    if (option_name == (uint32_t)IPV6_UNICAST_HOPS || option_name == (uint32_t)IPV6_MULTICAST_IF || option_name == (uint32_t)IPV6_MULTICAST_HOPS || option_name == (uint32_t)IPV6_MULTICAST_LOOP) 
    {
        // do nothing
    }
    else if(option_name == (uint32_t)IPV6_JOIN_GROUP || option_name == (uint32_t)IPV6_LEAVE_GROUP || option_name == (uint32_t)IPV6_V6ONLY || option_name == (uint32_t)IPV6_CHECKSUM)
    {
        // do nothing
    } 
    else
    {
        // do nothing
    }

    errno = EOPNOTSUPP;
    return SOCKET_ERROR;
}

static int F_setsockopt_icmp6(struct BSDSocket * s, uint32_t option_name, const uint8_t *option_value, uint32_t option_length)
{
    if (option_name == (uint32_t)ICMP6_FILTER)
    {
        // do nothing
    }

    errno = EOPNOTSUPP;
    return SOCKET_ERROR;
}

int setsockopt(SOCKET s, uint32_t level, uint32_t option_name, const uint8_t *option_value, uint32_t option_length)
{
    struct BSDSocket *bsocket = F_getBsdSocket(s);

    if (bsocket == NULL || bsocket->bsdState == (uint8_t)SKT_CLOSED) 
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    int res;
    errno = 0;
    switch (level)
    {
        case IPPROTO_IP:
            res = F_setsockopt_ip(bsocket, option_name, option_value, option_length);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        case SOL_SOCKET:
            res = F_setsockopt_socket(bsocket, option_name, option_value, option_length);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        case IPPROTO_TCP:
            res = F_setsockopt_tcp(bsocket, option_name, option_value, option_length);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        case IPPROTO_IPV6:
            res = F_setsockopt_ipv6(bsocket, option_name, option_value, option_length);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        case IPPROTO_ICMPV6:
            res = F_setsockopt_icmp6(bsocket, option_name, option_value, option_length);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        default:
            errno = EOPNOTSUPP;
            res = SOCKET_ERROR;
            break;
    }

    return res;
}


static int F_getsockopt_ip(const struct BSDSocket * s, uint32_t option_name, uint8_t *option_value, uint32_t *option_length)
{
    if (option_name == (uint32_t)IP_OPTIONS || option_name == (uint32_t)IP_TOS || option_name == (uint32_t)IP_TTL || option_name == (uint32_t)IP_MULTICAST_IF)
    {
        // do nothing
    }
    else if(option_name == (uint32_t)IP_MULTICAST_TTL || option_name == (uint32_t)IP_MULTICAST_LOOP || option_name == (uint32_t)IP_ADD_MEMBERSHIP || option_name == (uint32_t)IP_DROP_MEMBERSHIP)
    {
        // do nothing
    }
    else
    {
        // do nothing
    }

    errno = EOPNOTSUPP;
    return SOCKET_ERROR;
}

static int F_getsockopt_socket(struct BSDSocket * s, uint32_t option_name, uint8_t *option_value, uint32_t *option_length)
{
    union
    {
        uint8_t*  u8ptr;
        uint16_t* u16ptr;
        struct linger * ling;
        bool*  boolptr;
    }U_CUPTR_OPT;

    int res = 0;
    U_CUPTR_OPT.u8ptr = option_value;

    switch (option_name)
    {
        case SO_SNDBUF:
        {
            if (s->SocketID == INVALID_SOCKET)
            {
                *U_CUPTR_OPT.u16ptr = (uint16_t)s->sndBufSize;
            }
            else
            {
                if (s->SocketType == (uint16_t)SOCK_DGRAM)
                {
                    (void)NET_PRES_SocketOptionsGet(s->SocketID, (NET_PRES_SKT_OPTION_TYPE)UDP_OPTION_TX_BUFF, option_value);
                }
                else
                {
                    (void)NET_PRES_SocketOptionsGet(s->SocketID, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, option_value);

                }
                *option_length = 2;
            }
            s->sndBufSize = *U_CUPTR_OPT.u16ptr;
            break;
        }

        case SO_LINGER:
        {
            if (s->SocketType == (uint16_t)SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                res = SOCKET_ERROR;
            }
            else
            {
                struct linger * ling = U_CUPTR_OPT.ling;
                if (s->SocketID == INVALID_SOCKET)
                {
                    ling->l_onoff = (int)s->tcpLinger;
                    ling->l_linger = (int)s->lingerTmo;
                }
                else
                {
                    TCP_OPTION_LINGER_DATA tcplinger;
                    (void)NET_PRES_SocketOptionsGet(s->SocketID, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_LINGER, & tcplinger);
                    ling->l_onoff = tcplinger.lingerEnable ? 1 : 0;
                    ling->l_linger = (int)tcplinger.lingerTmo;
                }
                *option_length = sizeof(struct linger);
            }
            break;
        }

        case SO_RCVBUF:
        {
            if (s->SocketType == (uint16_t)SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                res = SOCKET_ERROR;
            }
            else
            {
                if (s->SocketID == INVALID_SOCKET)
                {
                    *U_CUPTR_OPT.u16ptr = (uint16_t)s->sndBufSize;
                }
                else
                {
                    (void)NET_PRES_SocketOptionsGet(s->SocketID, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, option_value);
                    *option_length = 2;
                }
                s->sndBufSize = *U_CUPTR_OPT.u16ptr;
            }
            break;
        }

        case SO_BROADCAST:
        {
            if (s->SocketType != (uint16_t)SOCK_DGRAM)
            {
                errno = EOPNOTSUPP;
                res = SOCKET_ERROR;
            }
            else
            {
                *U_CUPTR_OPT.boolptr = s->udpBcastEnabled != 0U;
            }

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
            res = SOCKET_ERROR;
            break;
    }

    return res;
}

static int F_getsockopt_tcp(struct BSDSocket * s, uint32_t option_name, uint8_t *option_value, uint32_t *option_length)
{
    if (s->SocketType == (uint16_t)SOCK_DGRAM)
    {
        errno = EOPNOTSUPP;
        return SOCKET_ERROR;
    }

    int res = 0;
    switch (option_name)
    {
        case TCP_NODELAY:
        {
            if (s->SocketID == INVALID_SOCKET)
            {
                *option_value = (uint8_t)s->tcpNoDelay;
            }
            else
            {
                *option_value = NET_PRES_SocketOptionsGet(s->SocketID, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_NODELAY, option_value) ? 1U : 0U;
            }
            *option_length = 1;
        }
        break;

        default:
            errno = EOPNOTSUPP;
            res = SOCKET_ERROR;
            break;
    }

    return res;

}

static int F_getsockopt_ipv6(struct BSDSocket * s, uint32_t option_name, uint8_t *option_value, uint32_t *option_length)
{
    if (option_name == (uint32_t)IPV6_UNICAST_HOPS || option_name == (uint32_t)IPV6_MULTICAST_IF || option_name == (uint32_t)IPV6_MULTICAST_HOPS || option_name == (uint32_t)IPV6_MULTICAST_LOOP) 
    {
        // do nothing
    }
    else if(option_name == (uint32_t)IPV6_JOIN_GROUP || option_name == (uint32_t)IPV6_LEAVE_GROUP || option_name == (uint32_t)IPV6_V6ONLY || option_name == (uint32_t)IPV6_CHECKSUM)
    {
        // do nothing
    } 
    else
    {
        // do nothing
    }

    errno = EOPNOTSUPP;
    return SOCKET_ERROR;
}

static int F_getsockopt_icmp6(struct BSDSocket * s, uint32_t option_name, uint8_t *option_value, uint32_t *option_length)
{
    if (option_name == (uint32_t)ICMP6_FILTER)
    {
        // do nothing
    }
    errno = EOPNOTSUPP;
    return SOCKET_ERROR;
}

int getsockopt(SOCKET s, uint32_t level, uint32_t option_name, uint8_t *option_value, uint32_t *option_length)
{
    struct BSDSocket *bsocket = F_getBsdSocket(s);

    if (bsocket == NULL || bsocket->bsdState == (uint8_t)SKT_CLOSED) 
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    int res;
    errno = 0;
    switch (level)
    {
        case IPPROTO_IP:
            res = F_getsockopt_ip(bsocket, option_name, option_value, option_length);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        case SOL_SOCKET:
            res = F_getsockopt_socket(bsocket, option_name, option_value, option_length);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        case IPPROTO_TCP:
            res = F_getsockopt_tcp(bsocket, option_name, option_value, option_length);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        case IPPROTO_IPV6:
            res = F_getsockopt_ipv6(bsocket, option_name, option_value, option_length);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        case IPPROTO_ICMPV6:
            res = F_getsockopt_icmp6(bsocket, option_name, option_value, option_length);
            if(errno != 0)
            {
                res = SOCKET_ERROR;
            }
            break;

        default:
            errno = EOPNOTSUPP;
            res = SOCKET_ERROR;
            break;
    }

    return res;
}

static IPV4_ADDR sAddr;
static char * sHostArray[2] =
{
    (char *)&sAddr,
    NULL
};

int h_errno;

static struct hostent sHostEnt =
{
    NULL,
    NULL,
    AF_INET,
    (int)sizeof(IPV4_ADDR),
    (char**)&sHostArray
};

struct hostent * gethostent(void)
{
    return &sHostEnt;
}


static uint8_t sHaveDnsToken = 0U;

struct hostent * gethostbyname(char *name)
{
    TCPIP_DNS_RESULT dRes;    

    if (sHaveDnsToken == 0U)
    {
        dRes = TCPIP_DNS_Resolve(name, TCPIP_DNS_TYPE_A);
        if (dRes == TCPIP_DNS_RES_NAME_IS_IPADDRESS)
        {
           (void)TCPIP_Helper_StringToIPAddress(name, & sAddr);
           return &sHostEnt;
        }
        if (dRes != TCPIP_DNS_RES_OK && dRes != TCPIP_DNS_RES_PENDING)
        {
            h_errno = NO_RECOVERY;
            return NULL;
        }
        sHaveDnsToken = 1U;
    }
    
    dRes = TCPIP_DNS_IsNameResolved(name, &sAddr, NULL);
    struct hostent* pHost = NULL;
    switch (dRes)
    {
        case TCPIP_DNS_RES_PENDING:
            h_errno = TRY_AGAIN;
            break;
        case TCPIP_DNS_RES_SERVER_TMO:
            h_errno = TRY_AGAIN;
            sHaveDnsToken = 0U;
            break;
        case TCPIP_DNS_RES_NO_NAME_ENTRY:
            h_errno = HOST_NOT_FOUND;
            sHaveDnsToken = 0U;
            break;
        case TCPIP_DNS_RES_OK:
            sHaveDnsToken = 0U;
            pHost = &sHostEnt;
            break;
        default:
            h_errno = NO_RECOVERY;
            sHaveDnsToken = 0U;
            break;
    }

    return pHost;
}

int getsockname( SOCKET s, struct sockaddr *addr, int *addrlen)
{
    struct sockaddr_in *rem_addr;

    if(addrlen == NULL)
    {
        errno = EINVAL;
        return SOCKET_ERROR;
    }

    struct BSDSocket *bsocket = F_getBsdSocket(s);
    if (bsocket == NULL || bsocket->bsdState == (uint8_t)SKT_CLOSED)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }


#if defined(TCPIP_STACK_USE_IPV6)
    if (bsocket->addressFamily != (uint8_t)AF_INET)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)

    if(*addrlen >= (int)sizeof(struct sockaddr_in) && addr != NULL)
    {
        rem_addr = FC_SockAddr2In(addr);
        rem_addr->sin_family = AF_INET;
        rem_addr->sin_port = bsocket->localPort;
        rem_addr->sin_addr.S_un.S_addr = bsocket->localIPv4;

        *addrlen = (int)sizeof(struct sockaddr_in);
        return 0;
    }

    errno = EFAULT;
    return SOCKET_ERROR;

}

int TCPIP_BSD_Socket(SOCKET s)
{
    struct BSDSocket *bsocket = F_getBsdSocket(s);

    if (bsocket == NULL || bsocket->bsdState == (uint8_t)SKT_CLOSED)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }


    int16_t nativeSkt = NET_PRES_SocketGetTransportHandle(bsocket->SocketID);
    return nativeSkt < 0 ? SOCKET_ERROR : nativeSkt; 
}

int TCPIP_BSD_PresSocket(SOCKET s)
{
    struct BSDSocket *bsocket = F_getBsdSocket(s);

    if (bsocket == NULL || bsocket->bsdState == (uint8_t)SKT_CLOSED)
    {
        errno = EBADF;
        return SOCKET_ERROR;
    }

    return bsocket->SocketID;
}


// Note: a DNS query with TCPIP_DNS_Resolve(name, TCPIP_DNS_TYPE_ANY) is deprecated by some DNS servers
// that's why it needs to be done in turn: TCPIP_DNS_TYPE_A, then TCPIP_DNS_TYPE_AAAA
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
    size_t counter, numDNS;
    int setres;
    IPV4_ADDR tmpv4;
    IPV6_ADDR tmpv6;

    if(res == NULL)
    {
        errno = EINVAL;
        return EAI_SYSTEM;
    }

    uint32_t nodeHash = fnv_32a_hash((const void*)node, strlen(node));
    if (sgaihash != 0U)
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

    *res = NULL;

    int infoRes;

    switch (sgaistate)
    {
        case (uint32_t)TCPIP_BERKELEY_GAI_START_IPV4:
            if(hints != NULL && hints->ai_family != AF_UNSPEC && hints->ai_family != AF_INET)
            {   // IPv4 resolution not needed; jump to IPv6
                sgaistate += 2U;
                infoRes = EAI_AGAIN;
            }
            else
            {
                setres = F_BSD_StartAddrInfo(node, hints, res, TCPIP_DNS_TYPE_A);

                if(setres <= 0)
                {   // either done or some error;
                    sgaihash = 0U;
                    sgaistate = (uint32_t)TCPIP_BERKELEY_GAI_INACTIVE;
                    infoRes = setres;
                }
                else
                {
                    // pending, advance to the new state
                    sgaistate ++;
                    infoRes = EAI_AGAIN;
                }
            }
            break;

        case (uint32_t)TCPIP_BERKELEY_GAI_WAIT_IPV4:

            setres = F_BSD_CheckAddrInfo(node, TCPIP_DNS_TYPE_A);

            if(setres < 0)
            {   // DNS error; abort
                sgaihash = 0U;
                sgaistate = (uint32_t)TCPIP_BERKELEY_GAI_INACTIVE;
                infoRes = EAI_FAIL;
            }
            else
            {
                if(setres > 0)
                {   // wait some more
                    infoRes = EAI_AGAIN;
                }
                else
                { // ok, advance to the new state
                    sgaistate ++;
                    infoRes = EAI_AGAIN;
                }
            }
            break;

        case (uint32_t)TCPIP_BERKELEY_GAI_START_IPV6:
            if(hints != NULL && hints->ai_family != AF_UNSPEC && hints->ai_family != AF_INET6)
            {   // IPv6 resolution not needed; jump to done
                sgaistate += 2U;
                infoRes = EAI_AGAIN;
            }
            else
            {
                setres = F_BSD_StartAddrInfo(node, hints, res, TCPIP_DNS_TYPE_AAAA);

                if(setres <= 0)
                {   // either done or some error;
                    sgaihash = 0U;
                    sgaistate = (uint32_t)TCPIP_BERKELEY_GAI_INACTIVE;
                    infoRes = setres;
                }
                else
                {
                    // pending, advance to the new state
                    sgaistate ++;
                    infoRes = EAI_AGAIN;
                }
            }
            break;

        case (uint32_t)TCPIP_BERKELEY_GAI_WAIT_IPV6:

            setres = F_BSD_CheckAddrInfo(node, TCPIP_DNS_TYPE_AAAA);

            if(setres < 0)
            {   // DNS error; abort
                sgaihash = 0U;
                sgaistate = (uint32_t)TCPIP_BERKELEY_GAI_INACTIVE;
                infoRes = EAI_FAIL;
            }
            else if(setres > 0)
            {   // wait some more
                infoRes = EAI_AGAIN;
            }
            else
            {
                // ok, advance to the new state
                sgaistate ++;
                infoRes = EAI_AGAIN;
            }
            break;

        case (uint32_t)TCPIP_BERKELEY_GAI_FINISHED:
            *res = NULL;
            setres = 0;
            if(hints == NULL || hints->ai_family == AF_UNSPEC || hints->ai_family == AF_INET)
            {   // get IPv4
                numDNS = TCPIP_DNS_GetIPAddressesNumber(node, IP_ADDRESS_TYPE_IPV4);

                for (counter = 0; counter < numDNS; counter++)
                {
                    (void)TCPIP_DNS_GetIPv4Addresses(node, counter, &tmpv4, 1);
                    setres = F_BSD_SetIp4AddrInfo(tmpv4.Val, hints, res);
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
                    (void)TCPIP_DNS_GetIPv6Addresses(node, counter, &tmpv6, 1);
                    setres = F_BSD_SetIp6AddrInfo(&tmpv6, hints, res);
                    if(setres != 0)
                    {   // some failure
                        break;
                    }
                }
            }

            sgaihash = 0U;
            sgaistate = (uint32_t)TCPIP_BERKELEY_GAI_INACTIVE;
            if (setres == 0 && *res == NULL)
            {
                setres = EAI_NONAME;
            }
            infoRes = setres;
            break;

        default:
            SYS_ASSERT(false, "Should not be here!");
            infoRes = EAI_SYSTEM;
            break;
    }

    return infoRes;
}

// starts a DNS resolve query
// returns:
//      < 0 if error
//      0 if done (query was an IP address, not a name)
//      1 if pending
//
static int F_BSD_StartAddrInfo(const char* node, const struct addrinfo* hints, struct addrinfo** res, TCPIP_DNS_RESOLVE_TYPE dnstype)
{
    IPV4_ADDR tmpv4;
    IPV6_ADDR tmpv6;

    TCPIP_DNS_RESULT result = TCPIP_DNS_Resolve(node, dnstype);

    if((int)result < 0)
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
            return F_BSD_SetIp4AddrInfo(tmpv4.Val, hints, res);
        }
        else if (TCPIP_Helper_StringToIPv6Address(node, &tmpv6))
        {
            return F_BSD_SetIp6AddrInfo(&tmpv6, hints, res);
        }
        else
        {
            // do nothing
        }
    }
    else
    {
        // do nothing
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
static int F_BSD_CheckAddrInfo(const char* node, TCPIP_DNS_RESOLVE_TYPE dnstype)
{
    IPV4_ADDR tmpv4;
    IPV6_ADDR tmpv6;
    TCPIP_DNS_RESULT result;

    if(dnstype == TCPIP_DNS_TYPE_A)
    {
        result = TCPIP_DNS_IsNameResolved(node, &tmpv4, NULL);
    }
    else
    {
        result = TCPIP_DNS_IsNameResolved(node, NULL, &tmpv6);
    }

    if((int)result < 0)
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
    else
    {
        // should not get here
        SYS_ASSERT(false, "getaddrinfo unknown error");
        return EAI_FAIL;
    }

} 

// returns 0 if done
// EAI_MEMORY if out of memory
static int F_BSD_SetIp4AddrInfo(uint32_t ipAddr, const struct addrinfo* hints, struct addrinfo** res)
{
    struct addrinfo* ptr = *res;

    if(ptr == NULL)
    {
        ptr = F_BsdCalloc(1, sizeof (struct addrinfo));
        *res = ptr;
    }
    else
    {
        ptr->ai_next = F_BsdCalloc(1, sizeof (struct addrinfo));
        ptr = ptr->ai_next;
    }

    if(ptr == NULL)
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
    ptr->ai_addr = F_BsdCalloc(1, sizeof(struct sockaddr_in));
    if(ptr->ai_addr == NULL)
    {
        SYS_ERROR(SYS_ERROR_WARNING, "Could not allocate memory for address info\r\n");
        return EAI_MEMORY;
    }

    struct sockaddr_in* s_addr_in = FC_SockAddr2In(ptr->ai_addr);
    s_addr_in->sin_family = AF_INET;
    s_addr_in->sin_addr.S_un.S_addr = ipAddr;
    return 0;
}

static int F_BSD_SetIp6AddrInfo(const IPV6_ADDR* ipAddr, const struct addrinfo* hints, struct addrinfo** res)
{
    struct addrinfo* ptr = *res;

    if(ptr == NULL)
    {
        ptr = F_BsdCalloc(1, sizeof (struct addrinfo));
        *res = ptr;
    }
    else
    {
        ptr->ai_next = F_BsdCalloc(1, sizeof (struct addrinfo));
        ptr = ptr->ai_next;
    }

    if(ptr == NULL)
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
    ptr->ai_addr = F_BsdCalloc(1, sizeof(SOCKADDR_IN6));
    if(ptr->ai_addr == NULL)
    {
        SYS_ERROR(SYS_ERROR_WARNING, "Could not allocate memory for address info\r\n");
        return EAI_MEMORY;
    }

    SOCKADDR_IN6* s_addr_in6 = FC_SockAddr2In6(ptr->ai_addr);

    s_addr_in6->sin6_family = AF_INET6;
    s_addr_in6->sin6_addr.in6_u.u6_addr32[0] = ipAddr->d32[0];
    s_addr_in6->sin6_addr.in6_u.u6_addr32[1] = ipAddr->d32[1];
    s_addr_in6->sin6_addr.in6_u.u6_addr32[2] = ipAddr->d32[2];
    s_addr_in6->sin6_addr.in6_u.u6_addr32[3] = ipAddr->d32[3];
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
            F_BsdFree(ptr->ai_addr);
        }
        F_BsdFree(ptr);
        ptr = ptr2;
    }
}

static void TCP_SignalFunction(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    uint32_t sig32 = (uint32_t)sigType;
    if((sig32 & ((uint32_t)TCPIP_TCP_SIGNAL_RX_FIN | (uint32_t)TCPIP_TCP_SIGNAL_RX_RST | (uint32_t)TCPIP_TCP_SIGNAL_TX_RST)) != 0U)
    {   // socket was closed
        // restore the child BSD socket
        struct BSDSocket* pChild = FC_CVPtr2BSD(param);
        while(true)
        {
            if(pChild->bsdState != (uint8_t)SKT_DISCONNECTING)
            {
                break;
            }

            // socket closed remotedly
            // find the parent it belongs to
            uint16_t parentIx = pChild->parentId;
            if(BSDSocketArray != NULL && parentIx < (uint16_t)BSD_SOCKET_COUNT)
            {
                struct BSDSocket* pParent = BSDSocketArray + parentIx;
                if(pParent->bsdState == (uint8_t)SKT_BSD_LISTEN && pParent->localPort == pChild->localPort)
                {   // found the parent up and running; return the child to accepting state
                    pChild->bsdState = (uint8_t)SKT_LISTEN;
                    return;
                }
            }

            // no parent could be found...close the child socket
            pChild->needsClose = 1U;
            (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC, true); 
            break;
        }
    }
}

// debug stuff

#if (M__BERKELEY_DEBUG != 0)
bool TCPIP_BSD_State(SOCKET s, BSD_SKT_INFO* pInfo)
{
    struct BSDSocket *bsocket = F_getBsdSocket(s);
    if( bsocket == NULL )
    {
        return false;
    }

    if(pInfo)
    {
        pInfo->bsdState = bsocket->bsdState;
        pInfo->presSktId = bsocket->SocketID;
    }

    return true;
}
#endif  // (M__BERKELEY_DEBUG != 0)


#endif //TCPIP_STACK_USE_BERKELEY_API

