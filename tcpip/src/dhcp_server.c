/*******************************************************************************
  Dynamic Host Configuration Protocol (DHCP) Server

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides automatic IP address, subnet mask, gateway address, 
      DNS server address, and other configuration parameters on DHCP 
      enabled networks.
    - Reference: RFC 2131, 2132
*******************************************************************************/


/*****************************************************************************
 Copyright (C) 2012-2021 Microchip Technology Inc. and its subsidiaries.

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







#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DHCP_SERVER

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/dhcp_server_private.h"
#include "system/sys_random_h2_adapter.h"

#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DHCP_SERVER_V2)


static TCPIP_DHCPS_DCPT*        gDhcpDcpt = 0;
static int                      dhcpSInitCount = 0;     // initialization count
static uint8_t                  gDhcpProcPacket[_TCPIP_DHCPS_PROCESS_BUFFER_SIZE];  // RX/TX processing buffer
static TCPIP_DHCPS_RX_OPTIONS   gRxOptions;             // parsed RX options;
                                                        // this buffer increases as we add more options

static uint32_t                 gDhcpSecCount;        // rough time keeping - sec
static uint32_t                 gDhcpMsecCount;       // rough time keeping - ms

static TCPIP_DHCPS_LEASE_STATE _DHCPS_ReplyToDiscovery(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER *Header, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);
static TCPIP_DHCPS_LEASE_STATE _DHCPS_ReplyToRequest(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);
static TCPIP_DHCPS_LEASE_STATE _DHCPS_ReplyToInform(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);
static TCPIP_DHCPS_LEASE_STATE _DHCPS_ReplyToDecline(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);
static TCPIP_DHCPS_LEASE_STATE _DHCPS_ReplyToRelease(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _DHCPS_Cleanup(void);
#else
#define _DHCPS_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
static bool isMacAddrValid(const uint8_t *macAddr);
static TCPIP_DHCPS_RES _DHCPS_Enable(TCPIP_NET_IF* pNetIf, bool checkStart);
static bool _DHCPS_Disable(TCPIP_NET_IF* pNetIf);

static void _DHCPS_ProcessTick(void);
static void _DHCPS_ProcessRx(void);
static void _DHCPS_ProcessSkt(UDP_SOCKET skt, uint16_t avlblBytes);
static void _DHCPS_SocketRxHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

static TCPIP_DHCPS_RES _DHCPS_ValidateConfig(const TCPIP_DHCPS_INTERFACE_CONFIG *pIfConfig, size_t ifConfigNo);
static TCPIP_DHCPS_RES _DHCPS_AddLeasePools(const TCPIP_DHCPS_INTERFACE_CONFIG* pIfConfig, uint16_t nConfigs);
static TCPIP_DHCPS_RES _DHCPS_ParseConfigSglOption(TCPIP_DHCPS_CLIENT_OPTIONS* pCliOpt, const TCPIP_DHCPS_CLIENT_OPTION_CONFIG* pOptConfig);
static TCPIP_DHCPS_RES _DHCPS_ParseConfigOptions(TCPIP_DHCPS_CLIENT_OPTIONS* pCliOpt, const TCPIP_DHCPS_CLIENT_OPTION_CONFIG* pOptConfig, size_t nOptions);

#if (TCPIP_DHCPS_OPTION_ROUTER_VALUES  != 0) || (TCPIP_DHCPS_OPTION_DNS_VALUES  != 0) || (TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES  != 0) || (TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES  != 0 || TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES != 0)
static int _DHCPS_ParseIPStrOption(const char* ipStr, uint32_t* pDest, size_t destSize);
#endif  // (TCPIP_DHCPS_OPTION_ROUTER_VALUES  != 0) || (TCPIP_DHCPS_OPTION_DNS_VALUES  != 0) || (TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES  != 0) || (TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES  != 0 || TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES != 0)

static bool _DHCPS_ParseOptions(TCPIP_DHCPS_DATA* pGetData, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, BOOTP_HEADER* pHeader);
static bool _DHCPS_OptionMessageType(uint8_t* pOpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt);
static bool _DHCPS_OptionReqClientId(uint8_t* pOpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt);
static bool _DHCPS_OptionReqIpAddress(uint8_t* pOpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt);
static bool _DHCPS_OptionReqLeaseTime(uint8_t* pOpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt);
static bool _DHCPS_OptionReqServerIdentifier(uint8_t* pOpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt);

static bool _DHCPS_ClientReqLeaseTime(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, uint32_t* pLeaseTime);

static void _DHCPS_ProcessMsg(TCPIP_DHCPS_INTERFACE_DCPT* pIfDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt);
static void _DHCPS_SaveClientState(DHCPS_HASH_ENTRY* he, BOOTP_HEADER* pHeader, DHCP_MESSAGE_TYPE_VAL msgType);
static void _DHCPS_InitIpMap(TCPIP_DHCPS_INTERFACE_DCPT* pIfDcpt);
static bool _DHCPS_SendProbe(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, DHCPS_HASH_ENTRY* he);
static void _DHCPS_PingHandler(const TCPIP_ICMP_ECHO_REQUEST* pReqData, TCPIP_ICMP_REQUEST_HANDLE icmpHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result, const void* param);

static uint8_t* _DHCPS_SetBootpHeader(uint8_t* pBuff, DHCPS_HASH_ENTRY* he);
static uint8_t* _DHCPS_SetMultIntOption(uint8_t* wrPtr, uint8_t optionType, uint32_t* pSrc, size_t nSources);
static bool _DHCPS_SendMessage(DHCPS_HASH_ENTRY* he);
static void _DHCPS_RemoveEntry(DHCPS_HASH_ENTRY* he, bool delBusy);
static void _DHCPS_RemoveAllEntries(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, bool delPerm, bool delBusy);

#if (_TCPIP_DHCPS_NOTIFICATIONS_ENABLE != 0)
static void _DHCPS_NotifyClients(TCPIP_NET_IF* pktIf, TCPIP_DHCPS_EVENT_TYPE evType, DHCPS_HASH_ENTRY* he, TCPIP_DHCPS_EVENT_DATA_INFO* evInfo);
#endif  // (_TCPIP_DHCPS_NOTIFICATIONS_ENABLE != 0)

static void IpIndexMarkBusy(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, size_t index);
static void IpIndexMarkFree(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, size_t index);
static bool IpIndexCheckFree(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, size_t index, bool markTaken);
static int  IpIndexFindFree(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, bool markTaken);
static void IpIndexMarkBusyBlock(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, size_t stIndex);
static int  IpIsAddressValid(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, uint32_t requestedIpAddr, DHCPS_VALID_CHECK validCheck);

static DHCPS_HASH_ENTRY* _DHCPS_AssignNewOffer(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, size_t ipIndex);

static size_t _DHCPS_FormatHashKey(uint8_t* destKey, const uint8_t* id, size_t idLen);
static bool _DHCPS_LeaseEntryPopulate(DHCPS_HASH_ENTRY* he, TCPIP_DHCPS_LEASE_INFO* pLeaseInfo);

static void _DHCPS_TickFnc_ProbeSend(DHCPS_HASH_ENTRY* he);
static void _DHCPS_TickFnc_ProbeWait(DHCPS_HASH_ENTRY* he);
static void _DHCPS_TickFnc_Reprobe(DHCPS_HASH_ENTRY* he);
static void _DHCPS_TickFnc_SendOffer(DHCPS_HASH_ENTRY* he);
static void _DHCPS_TickFnc_Offered(DHCPS_HASH_ENTRY* he);
static void _DHCPS_TickFnc_Bound(DHCPS_HASH_ENTRY* he);


// hash functions
static size_t TCPIP_DHCPS_ClientIdHash(OA_HASH_DCPT* pOH, const void* key);
static void TCPIP_DHCPS_IdKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);
static int TCPIP_DHCPS_IdKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
static OA_HASH_ENTRY* TCPIP_DHCPS_HashDeleteEntry(OA_HASH_DCPT* pOH);
#if defined(OA_DOUBLE_HASH_PROBING)
static size_t TCPIP_DHCPS_HashProbeHash(OA_HASH_DCPT* pOH, const void* key);
#endif  // defined(OA_DOUBLE_HASH_PROBING)

#if ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_BASIC) != 0)
volatile int _DhcpsStayAssertLoop = 0;
static void _DhcpsAssert(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("DHCPS Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_DhcpsStayAssertLoop != 0);
    }
}

#else
#define _DhcpsAssert(cond, message, lineNo)
#endif  // (_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_BASIC)

#if ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_STATUS) != 0)

static const char* statMsgTbl[] = 
{
    "none",         // TCPIP_DHCPS_LEASE_STATE_IDLE
    "probe",        // TCPIP_DHCPS_LEASE_STATE_SEND_PROBE
    "waitp",        // TCPIP_DHCPS_LEASE_STATE_WAIT_PROBE
    "reprobe",      // TCPIP_DHCPS_LEASE_STATE_REPROBE
    "sendoffer",    // TCPIP_DHCPS_LEASE_STATE_SEND_OFFER
    "offerred",     // TCPIP_DHCPS_LEASE_STATE_OFFERED
    "bound",        // TCPIP_DHCPS_LEASE_STATE_BOUND
    "release",      // TCPIP_DHCPS_LEASE_STATE_RELEASED
    "expired",      // TCPIP_DHCPS_LEASE_STATE_EXPIRED
    "inform",       // TCPIP_DHCPS_LEASE_STATE_INFORM
};

static void _DhcpsDisplayStat(DHCPS_HASH_ENTRY* he)
{
    if(he->state != he->prevState) 
    {
        char addBuff[20];
        TCPIP_Helper_IPAddressToString(&he->ipAddress, addBuff, sizeof(addBuff));
        const char* msg = statMsgTbl[he->state];
        (void)msg;
        he->prevState = he->state;
        SYS_CONSOLE_PRINT("DHCPS state: %s, if: %d, address: %s \r\n", msg, he->parent->pNetIf->netIfIx, addBuff);
    }
}
#else
#define _DhcpsDisplayStat(he)
#endif  // (_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_STATUS)

#if (_TCPIP_DHCPS_DEBUG_LEVEL & (_TCPIP_DHCPS_DEBUG_MASK_ERROR_EVENT | _TCPIP_DHCPS_DEBUG_MASK_CLIENT_EVENT)) != 0

#if (_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_ERROR_EVENT ) != 0
static const char* _DHCPS_ErrorEvent_Tbl[] = 
{
    "p_lock",       // TCPIP_DHCPS_EVENT_PROCESS_LOCK
    "t_lock",       // TCPIP_DHCPS_EVENT_TICK_LOCK
    "ovfl",         // TCPIP_DHCPS_EVENT_MSG_OVERFLOW
    "unfl",         // TCPIP_DHCPS_EVENT_MSG_UNDERFLOW
    "if_dis",       // TCPIP_DHCPS_EVENT_IF_DISABLED
    "if_err",       // TCPIP_DHCPS_EVENT_IF_ERROR
    "msg_fmt",      // TCPIP_DHCPS_EVENT_MSG_FORMAT_ERROR
    "pool_empty",   // TCPIP_DHCPS_EVENT_POOL_EMPTY
    "cache_full",   // TCPIP_DHCPS_EVENT_CACHE_FULL
    "probe_fail",   // TCPIP_DHCPS_EVENT_ECHO_PROBE_FAIL
    "echo_fail",    // TCPIP_DHCPS_EVENT_ECHO_FAIL
    "reprobe_fail", // TCPIP_DHCPS_EVENT_REPROBE_FAIL
    "arp_fail",     // TCPIP_DHCPS_EVENT_ARP_FAIL
    "req_add_err",  // TCPIP_DHCPS_EVENT_REQ_ADDRESS_ERROR
    "req_unk",      // TCPIP_DHCPS_EVENT_REQ_UNKNOWN
    "req_fmt_err",  // TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR
    "info_fmt_err", // TCPIP_DHCPS_EVENT_INFO_FORMAT_ERROR
    "req_unexpct",  // TCPIP_DHCPS_EVENT_REQ_UNEXPECT
    "declined",     // TCPIP_DHCPS_EVENT_DECLINED
    "info_invalid", // TCPIP_DHCPS_EVENT_INFORM_INVALID
};
#endif // (_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_ERROR_EVENT ) != 0

#if (_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_CLIENT_EVENT ) != 0
static const char* _DHCPS_ClientEvent_Tbl[] = 
{
    "probe",        // TCPIP_DHCPS_EVENT_ECHO_PROBE_SENT
    "discover",     // TCPIP_DHCPS_EVENT_DISCOVER
    "req_offer",    // TCPIP_DHCPS_EVENT_REQUEST_OFFERRED
    "req_renew",    // TCPIP_DHCPS_EVENT_REQUEST_RENEW
    "req_rebind",   // TCPIP_DHCPS_EVENT_REQUEST_REBIND
    "req_expire",   // TCPIP_DHCPS_EVENT_REQUEST_EXPIRED
    "svc_ena",      // TCPIP_DHCPS_EVENT_OTHER_SELECT
    "inform",       // TCPIP_DHCPS_EVENT_INFORM
};
#endif  // (_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_CLIENT_EVENT ) != 0


static void _DHCPS_EventPrint(TCPIP_NET_IF* pktIf, TCPIP_DHCPS_EVENT_TYPE evType, DHCPS_HASH_ENTRY* he, TCPIP_DHCPS_EVENT_DATA_INFO* evInfo)
{
    const char* evStr = "illegal";
    const char* evMsg = "illegal";
    int evIx;

    if(evType == TCPIP_DHCPS_EVENT_NONE)
    {
        _DhcpsAssert(false, __func__, __LINE__);
    }

#if (_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_ERROR_EVENT ) != 0
    else if(evType < 0)
    {
        evStr = "error event";
        evIx = -(evType + 1);
        _DhcpsAssert(evIx < sizeof(_DHCPS_ErrorEvent_Tbl) / sizeof(*_DHCPS_ErrorEvent_Tbl), __func__, __LINE__);
        evMsg = _DHCPS_ErrorEvent_Tbl[evIx];
    }
#endif // (_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_ERROR_EVENT ) != 0

#if (_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_CLIENT_EVENT ) != 0
    else if(evType > 0)
    {
        evStr = "run event";
        evIx = evType - 1;
        _DhcpsAssert(evIx < sizeof(_DHCPS_ClientEvent_Tbl) / sizeof(*_DHCPS_ClientEvent_Tbl), __func__, __LINE__);
        evMsg = _DHCPS_ClientEvent_Tbl[evIx];
    }
#endif // (_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_CLIENT_EVENT ) != 0

    (void)evStr; (void)evMsg;
    uint32_t ipAddress = 0;
    uint32_t data = 0;
    uint16_t flags = 0;

    if(evInfo != 0)
    {
        flags = evInfo->flags.val;
        ipAddress = evInfo->ipAddress;
        data = evInfo->data;
    }

    (void)ipAddress; (void)data; (void)flags;
    SYS_CONSOLE_PRINT("DHCPs - if: %d %s: %s, ipAddress: 0x%08x, data: 0x%08x, flags: 0x%04x\r\n", TCPIP_STACK_NetIndexGet(pktIf), evStr, evMsg, ipAddress, data, flags);
}
#else
#define _DHCPS_EventPrint(pktIf, evType, he, evInfo) do { (void)he; (void)evInfo; } while (0)
#endif  // (_TCPIP_DHCPS_DEBUG_LEVEL & (_TCPIP_DHCPS_DEBUG_MASK_ERROR_EVENT | _TCPIP_DHCPS_DEBUG_MASK_CLIENT_EVENT)) != 0

#if (_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_SENT_MESSAGE ) != 0
static void _DHCPS_SendMessagePrint(DHCPS_HASH_ENTRY* he, uint32_t destAdd)
{

    static const char* _DHCPS_MsgType_Tbl[] = 
    {
        "discover", // DHCP_MESSAGE_TYPE_DISCOVER
        "offer",    // DHCP_MESSAGE_TYPE_OFFER   
        "request",  // DHCP_MESSAGE_TYPE_REQUEST
        "decline",  // DHCP_MESSAGE_TYPE_DECLINE
        "ack",      // DHCP_MESSAGE_TYPE_ACK     
        "nak",      // DHCP_MESSAGE_TYPE_NAK     
        "release",  // DHCP_MESSAGE_TYPE_RELEASE
        "inform",   // DHCP_MESSAGE_TYPE_INFORM  
    };

    char addBuff[20];
    IPV4_ADDR dAddr;
    dAddr.Val =  destAdd;
    TCPIP_Helper_IPAddressToString(&dAddr, addBuff, sizeof(addBuff));
    _DhcpsAssert(he->srvMsgType > 0, __func__, __LINE__); 
    _DhcpsAssert(he->cliMsgType > 0, __func__, __LINE__); 
    uint8_t srvMsgType = he->srvMsgType - 1;
    uint8_t cliMsgType = he->cliMsgType - 1;
    _DhcpsAssert(srvMsgType < sizeof(_DHCPS_MsgType_Tbl) / sizeof(*_DHCPS_MsgType_Tbl), __func__, __LINE__); 
    _DhcpsAssert(cliMsgType < sizeof(_DHCPS_MsgType_Tbl) / sizeof(*_DHCPS_MsgType_Tbl), __func__, __LINE__); 
    const char* srvMsg = _DHCPS_MsgType_Tbl[srvMsgType];
    const char* cliMsg = _DHCPS_MsgType_Tbl[cliMsgType];

    (void)cliMsg; (void)srvMsg; (void)addBuff;
    SYS_CONSOLE_PRINT("DHCPs sent msg - if: %d to: %s, cliMsg: %s, srvMsg: %s\r\n", he->parent->pNetIf->netIfIx, addBuff, cliMsg, srvMsg);
}

#else
#define _DHCPS_SendMessagePrint(he, destAdd) 
#endif

// DHCPs lock for shared access
static __inline__ OSAL_CRITSECT_DATA_TYPE __attribute__((always_inline)) _DHCPS_Lock(void)
{
    return OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
}

static __inline__ void __attribute__((always_inline)) _DHCPS_Unlock(OSAL_CRITSECT_DATA_TYPE lock)
{
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, lock);   
}

#if (_TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)
static bool _DHCPS_AccessLock(void)
{ 
    bool res = false;

    OSAL_CRITSECT_DATA_TYPE critStat = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(gDhcpDcpt != 0)
    {
        if(gDhcpDcpt->accessLock == 0)
        {
            gDhcpDcpt->accessLock = 1;
            res = true;
        }
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStat);
    return res;
}

static void _DHCPS_AccessUnlock(void)
{ 
    OSAL_CRITSECT_DATA_TYPE critStat = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    gDhcpDcpt->accessLock = 0;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStat);
}
#else
static __inline__ bool __attribute__((always_inline)) _DHCPS_AccessLock(void)
{
    if(gDhcpDcpt != 0 && gDhcpDcpt->accessLock == 0)
    {
        gDhcpDcpt->accessLock = 1;
        return  true;
    }

    return false;
}
static __inline__ void __attribute__((always_inline)) _DHCPS_AccessUnlock(void)
{
    gDhcpDcpt->accessLock = 0;
}
#endif  // (_TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)


static __inline__ uint32_t __attribute__((always_inline)) _DHCPS_SecCount(void)
{
    return gDhcpSecCount;
}

static __inline__ uint32_t __attribute__((always_inline)) _DHCPS_MsecCount(void)
{
    return gDhcpMsecCount;
}

static __inline__ uint32_t __attribute__((always_inline)) _DHCPS_SecondCountSet(void)
{
    uint32_t tmrFreq = SYS_TMR_SystemCountFrequencyGet();
    // use a 64 bit count to avoid roll over
    uint64_t tmrCount = SYS_TMR_SystemCountGet();

    gDhcpSecCount = tmrCount / tmrFreq; 
    gDhcpMsecCount = tmrCount / (tmrFreq / 1000); 

    return gDhcpSecCount;
}

static __inline__ void __attribute__((always_inline)) _DHCPS_LeaseRestart(DHCPS_HASH_ENTRY* he, uint32_t leaseTime)
{
    he->cliLeaseTime = leaseTime;
    he->leaseStartTime = _DHCPS_SecCount();
    he->leaseEndTime = he->leaseStartTime + he->cliLeaseTime;
}

// returns true if the DHCPs is configured on pNetIf
// false otherwise
// DHCPs and pNetIf both valid
static __inline__  bool __attribute__((always_inline)) _DHCPS_MappedOnNet(TCPIP_NET_IF* pNetIf)
{
    DHCPS_IF_MAP* pIfMap = gDhcpDcpt->ifMap + pNetIf->netIfIx;
    return (pIfMap->mapFlag & DHCPS_IF_FLAG_MAPPED) != 0;
}

// calculate an IP mask from a prefix length
// mask is returned in proper network order
static __inline__ uint32_t __attribute__((always_inline)) _IPMask_FromPrefix(size_t prefixLen)
{
    return TCPIP_Helper_htonl(((1 << prefixLen) - 1) << (32 - prefixLen));
}

// number of leases calculated from the prefix length
static __inline__ size_t __attribute__((always_inline)) _IPLeases_FromPrefix(size_t prefixLen)
{
    return (1 << (32 - prefixLen));
}

// returns the IP address corresponding to a pool index
// assumes the index is valid
static __inline__ uint32_t __attribute__((always_inline)) _IpAddressFromPoolIndex(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, size_t ipIndex)
{
    return TCPIP_Helper_htonl(TCPIP_Helper_ntohl(pIDcpt->startIPAddress.Val) + ipIndex);
}

static __inline__ void __attribute__((always_inline)) _DHCPS_SetEntryState(DHCPS_HASH_ENTRY* he, TCPIP_DHCPS_LEASE_STATE setState)
{
     he->state = setState;
}

static __inline__ void __attribute__((always_inline)) _DHCPS_InitProbe(DHCPS_HASH_ENTRY* he, bool setReprobe)
{
    if(setReprobe)
    {
        he->reprobeCount = gDhcpDcpt->nReprobes;
    }
    he->probeCount = gDhcpDcpt->nProbes;
    he->icmpRetries = TCPIP_DHCPS_ICMP_ECHO_RETRIES;
}

// returns the pool index corresponding to an IP address
// the IP address has to be part of the pool
// returns -1 if error
static int32_t   _PoolIndexFromIpAddress(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, uint32_t ipAddress)
{
    uint32_t hostAddress = TCPIP_Helper_ntohl(ipAddress);
    uint32_t startAddress = TCPIP_Helper_ntohl(pIDcpt->startIPAddress.Val);

    if(hostAddress >= startAddress)
    {
        size_t index = hostAddress - startAddress;
        if(index < pIDcpt->leaseEntries)
        {   // valid
            return index;
        }
    }

    return -1;

}

// returns the DHCPs interface index corresponding
// to a stack interface
// DHCPs and pNetIf both valid
static int _DHCPS_InterfaceMapIx(TCPIP_NET_IF* pNetIf)
{
    DHCPS_IF_MAP* pIfMap = gDhcpDcpt->ifMap + pNetIf->netIfIx;
    if((pIfMap->mapFlag & DHCPS_IF_FLAG_MAPPED) != 0)
    {
        return pIfMap->mapIndex;
    }

    // no DHCPs on this interface
    return -1;
}

// formats a key to the right size, padding it right if necessary
// the destKey is _TCPIP_DHCPS_CLIENT_ID_SIZE in length!
// returns real size of the used key: could be < than idLen if truncation occurred!
static size_t _DHCPS_FormatHashKey(uint8_t* destKey, const uint8_t* id, size_t idLen)
{
    size_t usedSize = idLen > _TCPIP_DHCPS_CLIENT_ID_SIZE ? _TCPIP_DHCPS_CLIENT_ID_SIZE : idLen;

    memcpy(destKey, id, usedSize);

    if(idLen < _TCPIP_DHCPS_CLIENT_ID_SIZE)
    { 
        size_t padSize = _TCPIP_DHCPS_CLIENT_ID_SIZE - idLen;
        memset(destKey + idLen, 0, padSize);
    }

    return usedSize;
}


// removes an existing hash entry
// marks the corresponding pool index as free or keeps it busy
static void _DHCPS_RemoveEntry(DHCPS_HASH_ENTRY* he, bool delBusy)
{
    _DhcpsAssert(he != 0, __func__, __LINE__);
    uint32_t ipAddr = he->ipAddress.Val;
    _DhcpsAssert(ipAddr != 0, __func__, __LINE__);

    TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = he->parent;
    _DhcpsAssert(pIDcpt != 0, __func__, __LINE__);

    TCPIP_OAHASH_EntryRemove(pIDcpt->hashDcpt, (OA_HASH_ENTRY*)he);
    if(delBusy == true)
    {
        int32_t entryIx = _PoolIndexFromIpAddress(pIDcpt, ipAddr); 
        // should be a valid IP address since it's in the database!
        _DhcpsAssert(entryIx >= 0, __func__, __LINE__);
        IpIndexMarkFree(pIDcpt, entryIx);
    }
}

// removes all existing hash entries
// marks the corresponding pool indexes as free or keeps them busy
static void _DHCPS_RemoveAllEntries(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, bool delPerm, bool delBusy)
{
    OA_HASH_ENTRY*  pBkt;
    DHCPS_HASH_ENTRY* he;
    size_t      bktIx;
    OA_HASH_DCPT* pOH = pIDcpt->hashDcpt;

    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        pBkt = TCPIP_OAHASH_EntryGet(pOH, bktIx);       
        he = (DHCPS_HASH_ENTRY*)pBkt;
        if(pBkt->flags.busy != 0 && (delPerm == true || (he->runFlags & DHCPS_RUN_FLAG_PERM_LEASE) == 0))
        {   // delete entry
            uint32_t ipAddr = he->ipAddress.Val;
            _DhcpsAssert(ipAddr != 0, __func__, __LINE__);

            TCPIP_OAHASH_EntryRemove(pOH, pBkt);
            if(delBusy == true)
            {
                int32_t entryIx = _PoolIndexFromIpAddress(pIDcpt, ipAddr); 
                // should be a valid IP address since it's in the database!
                _DhcpsAssert(entryIx >= 0, __func__, __LINE__);
                IpIndexMarkFree(pIDcpt, entryIx);
            }
        }
    }
}

// access should be locked if called at run-time!
static TCPIP_DHCPS_RES _DHCPS_AddLeasePools(const TCPIP_DHCPS_INTERFACE_CONFIG* pIfConfig, uint16_t nConfigs)
{
    TCPIP_DHCPS_RES valRes = _DHCPS_ValidateConfig(pIfConfig, nConfigs);
    if(valRes < 0)
    {
        return valRes;
    }

    
    TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt;
    TCPIP_NET_IF* pNetIf;
    DHCPS_IF_MAP* pIfMap;
    size_t hashMemSize;
    OA_HASH_DCPT*   hashDcpt;
    TCPIP_DHCPS_CLIENT_OPTIONS cliOptions;

    int ix;

    for(ix = 0; ix < nConfigs; ix++, pIfConfig++)
    {
        memset(&cliOptions, 0, sizeof(cliOptions));
#if (_TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS == 0)
        // set the T1 and T2 options, just in case; user can overwrite
        cliOptions.t1Mult = _TCPIP_DHCPS_DEFAULT_T1_MULT;
        cliOptions.t1Div = _TCPIP_DHCPS_DEFAULT_T1_DIV;
        cliOptions.t2Mult = _TCPIP_DHCPS_DEFAULT_T2_MULT;
        cliOptions.t2Div = _TCPIP_DHCPS_DEFAULT_T2_DIV;
#endif  // (_TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS == 0)

        valRes = _DHCPS_ParseConfigOptions(&cliOptions, pIfConfig->pOptConfig, pIfConfig->nOptConfigs);
        if(valRes < 0)
        {   // failed
            return valRes;
        }
        // 
#if (_TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS == 0)
        // check T2 > T1
        if(cliOptions.t1Mult * cliOptions.t2Div > cliOptions.t2Mult * cliOptions.t1Div)
        {   // failed; use defaults
            return TCPIP_DHCPS_RES_T1_T2_ERR;
        } 
#endif  // (_TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS == 0)

        pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(pIfConfig->ifIndex); 
        int ifIx = _DHCPS_InterfaceMapIx(pNetIf); 
        if(ifIx >= 0)
        {   // this interface is already mapped; kill the DB
            pIDcpt = gDhcpDcpt->ifDcpt + ifIx;
            _DhcpsAssert(pIDcpt->hashDcpt != 0, __func__, __LINE__);
            _DHCPS_RemoveAllEntries(pIDcpt, true, true);
            TCPIP_HEAP_Free(gDhcpDcpt->memH, pIDcpt->hashDcpt);
            pIDcpt->hashDcpt = 0;
        }
        else
        {   // new interface to map; get the next free slot
            ifIx = ++gDhcpDcpt->maxIx;
        }

        pIDcpt = gDhcpDcpt->ifDcpt + ifIx;

        hashMemSize = sizeof(OA_HASH_DCPT) + pIfConfig->leaseEntries * sizeof(DHCPS_HASH_ENTRY);
        hashDcpt = (OA_HASH_DCPT*)TCPIP_HEAP_Malloc(gDhcpDcpt->memH, hashMemSize);
        if(hashDcpt == 0)
        {   // failed
            return TCPIP_DHCPS_RES_ALLOC_ERR;
        }

        // populate the hash entries
        hashDcpt->memBlk = hashDcpt + 1;
        hashDcpt->hParam = 0;
        hashDcpt->hEntrySize = sizeof(DHCPS_HASH_ENTRY);
        hashDcpt->hEntries = pIfConfig->leaseEntries;
        hashDcpt->probeStep = DHCPS_HASH_PROBE_STEP;

        hashDcpt->hashF = TCPIP_DHCPS_ClientIdHash;
#if defined(OA_DOUBLE_HASH_PROBING)
        hashDcpt->probeHash = TCPIP_DHCPS_HashProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
        hashDcpt->cpyF = TCPIP_DHCPS_IdKeyCopy;
        hashDcpt->delF = TCPIP_DHCPS_HashDeleteEntry;
        hashDcpt->cmpF = TCPIP_DHCPS_IdKeyCompare;
        TCPIP_OAHASH_Initialize(hashDcpt);

        // populate the interface descriptor
        pIDcpt->configFlags = pIfConfig->configFlags;
        pIDcpt->leaseEntries = pIfConfig->leaseEntries;
        pIDcpt->prefixLen = (uint8_t)pIfConfig->prefixLen;
        pIDcpt->pNetIf = pNetIf;
        TCPIP_Helper_StringToIPAddress(pIfConfig->serverIPAddress, &pIDcpt->serverIPAddress);
        pIDcpt->ipMaskAddress.Val =  _IPMask_FromPrefix(pIfConfig->prefixLen);
        TCPIP_Helper_StringToIPAddress(pIfConfig->startIPAddress, &pIDcpt->startIPAddress);
        pIDcpt->cliLeaseDuration = pIfConfig->leaseDuration;
        pIDcpt->minLeaseDuration = (pIfConfig->minLeaseDuration == 0) ? _TCPIP_DHCPS_LEASE_DEFAULT_MIN_TIME : pIfConfig->minLeaseDuration;
        pIDcpt->maxLeaseDuration = (pIfConfig->maxLeaseDuration == 0) ? pIfConfig->leaseDuration : pIfConfig->maxLeaseDuration;
        pIDcpt->unreqOfferTmo = (pIfConfig->unreqOfferTmo == 0) ? _TCPIP_DHCPS_LEASE_DEFAULT_UNREQ_OFFER_TMO : pIfConfig->unreqOfferTmo;
        pIDcpt->hashDcpt = hashDcpt;
        pIDcpt->ipMapSize = ((pIfConfig->leaseEntries + 31) / 32);  // round up to multiple of 32
        _DHCPS_InitIpMap(pIDcpt);

        // update the if map
        pIfMap = gDhcpDcpt->ifMap + pIfConfig->ifIndex;
        pIfMap->mapFlag |= DHCPS_IF_FLAG_MAPPED;
        pIfMap->mapIndex = ifIx;

        // set the client options
        pIDcpt->cliOptions = cliOptions; 
    }


    return TCPIP_DHCPS_RES_OK;
}



// validate the DHCP server configuration data
static TCPIP_DHCPS_RES _DHCPS_ValidateConfig(const TCPIP_DHCPS_INTERFACE_CONFIG *pIfConfig, size_t ifConfigNo)
{
    int ix;
    IPV4_ADDR   serverIPAddress;
    IPV4_ADDR   startIPAddress;
    IPV4_ADDR   ipMaskAddress;
    IPV4_ADDR   netStartAdd;
    size_t      maxLeases, avlblLeases;
    uint32_t    minLease, maxLease;
    DHCPS_IF_MAP* pIfMap;
    TCPIP_DHCPS_RES optRes;
    DHCPS_IF_MAP ifMap[_TCPIP_DHCPS_NET_INTERFACES_COUNT]; // map of the used interfaces

    if(pIfConfig == 0 || ifConfigNo == 0 || ifConfigNo > TCPIP_DHCPS_INTERFACE_COUNT)
    {
        return TCPIP_DHCPS_RES_IF_CONFIG_ERR;
    }

    // verify data for each interface
    memset(ifMap, 0, sizeof(ifMap));
    for(ix = 0; ix < ifConfigNo; ix++, pIfConfig++)
    {
        if(pIfConfig->ifIndex >= gDhcpDcpt->stackIfs)
        {
            return TCPIP_DHCPS_RES_IF_CONFIG_ERR;
        }

        if(pIfConfig->leaseEntries == 0 || pIfConfig->leaseEntries > gDhcpDcpt->maxLeases || pIfConfig->leaseDuration == 0)
        {   // wrong parameters
            return TCPIP_DHCPS_RES_LEASE_PARAM_ERR;
        }
        minLease = (pIfConfig->minLeaseDuration == 0) ? _TCPIP_DHCPS_LEASE_DEFAULT_MIN_TIME : pIfConfig->minLeaseDuration;
        maxLease = (pIfConfig->maxLeaseDuration == 0) ? pIfConfig->leaseDuration : pIfConfig->maxLeaseDuration;
        
        if(minLease > pIfConfig->leaseDuration || maxLease < pIfConfig->leaseDuration)
        {   // wrong parameters
            return TCPIP_DHCPS_RES_LEASE_PARAM_ERR;
        }

        if(pIfConfig->prefixLen == 0 || pIfConfig->prefixLen > 32)
        {
            return TCPIP_DHCPS_RES_PREFIX_LEN_ERR;
        }

        // check the supplied addresses
        if( !TCPIP_Helper_StringToIPAddress(pIfConfig->serverIPAddress, &serverIPAddress) ||
            !TCPIP_Helper_StringToIPAddress(pIfConfig->startIPAddress, &startIPAddress) )
        {
            return TCPIP_DHCPS_RES_INVALID_ADDRESS;
        }

        if(serverIPAddress.Val == 0 || startIPAddress.Val == 0)
        {
            return TCPIP_DHCPS_RES_NULL_ADDRESS;
        }

        // check the addresses space 
        maxLeases = _IPLeases_FromPrefix(pIfConfig->prefixLen);
        if(maxLeases < 2)
        {
            maxLeases = 0;
        }
        else
        {
            maxLeases -= 2;   // exclude all 0 and all 1 hosts
        }

        if(maxLeases == 0 || maxLeases < pIfConfig->leaseEntries)
        {   // not enough leases for this
            return TCPIP_DHCPS_RES_NOT_ENOUGH_LEASES;
        }

        // check DHCP server is in the same subnet as the leases...
        ipMaskAddress.Val = _IPMask_FromPrefix(pIfConfig->prefixLen);
        if((serverIPAddress.Val & ipMaskAddress.Val) != (startIPAddress.Val & ipMaskAddress.Val))
        {
            return TCPIP_DHCPS_RES_SUBNET_ERR;
        }

        startIPAddress.Val =  TCPIP_Helper_ntohl(startIPAddress.Val);
        // check that [start, start + leases] length is within [net start, net end]
        netStartAdd.Val = startIPAddress.Val & TCPIP_Helper_ntohl(ipMaskAddress.Val);
        avlblLeases = maxLeases - (startIPAddress.Val - netStartAdd.Val);
        if(avlblLeases <  pIfConfig->leaseEntries)
        {
            return TCPIP_DHCPS_RES_NOT_ENOUGH_LEASES;
        }

        // check the supplied options
        optRes = _DHCPS_ParseConfigOptions(0, pIfConfig->pOptConfig, pIfConfig->nOptConfigs);
        if(optRes != TCPIP_DHCPS_RES_OK)
        {   // failed
            return optRes;
        }

        // check we can add this interface to the map
        pIfMap = ifMap + pIfConfig->ifIndex;
        if((pIfMap->mapFlag & DHCPS_IF_FLAG_MAPPED) != 0) 
        {   // same interface multiple times
            return TCPIP_DHCPS_RES_IF_REPEAT_ERR;
        }

        pIfMap->mapFlag |= DHCPS_IF_FLAG_MAPPED; 
    }

    return TCPIP_DHCPS_RES_OK;
}


#if (TCPIP_DHCPS_OPTION_ROUTER_VALUES  != 0) || (TCPIP_DHCPS_OPTION_DNS_VALUES  != 0) || (TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES  != 0) || (TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES  != 0 || TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES != 0)
// parses an IP string and returns the number of values set to the destination
// < 0 if an error
static int _DHCPS_ParseIPStrOption(const char* ipStr, uint32_t* pDest, size_t destSize)
{
    int ix;
    IPV4_ADDR ipAddr;

    if(!TCPIP_Helper_StringToIPAddress(ipStr, &ipAddr))
    {
        return -1;
    }
    if(ipAddr.Val == 0)
    {
        return -1;
    }

    // OK
    if(pDest != 0)
    {   // add it; ignore excess values
        for(ix = 0; ix < destSize; ix++, pDest++)
        {
            if(*pDest == 0)
            {
                *pDest = ipAddr.Val;
                return 1;
            }
        }
    }
    return 0; 
}
#endif  // (TCPIP_DHCPS_OPTION_ROUTER_VALUES  != 0) || (TCPIP_DHCPS_OPTION_DNS_VALUES  != 0) || (TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES  != 0) || (TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES  != 0 || TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES != 0)

// parses all configuration options
// returns TCPIP_DHCPS_RES_OK if OK, otherwise an error 
static TCPIP_DHCPS_RES _DHCPS_ParseConfigOptions(TCPIP_DHCPS_CLIENT_OPTIONS* pCliOpt, const TCPIP_DHCPS_CLIENT_OPTION_CONFIG* pOptConfig, size_t nOptions)
{
    size_t optIx;

    if(pOptConfig != 0)
    {   // configuration options provided
        for(optIx = 0; optIx < nOptions; optIx++, pOptConfig++)
        {
            TCPIP_DHCPS_RES optRes = _DHCPS_ParseConfigSglOption(pCliOpt, pOptConfig);
            if(optRes != TCPIP_DHCPS_RES_OK)
            {   // failed
                return optRes;
            }
        }
    }

    return TCPIP_DHCPS_RES_OK;
}

// parses a single configuration option
// returns TCPIP_DHCPS_RES_OK if OK, otherwise an error 
// if pCliOpt != 0, it actually stores the data; otherwise just parse
static TCPIP_DHCPS_RES _DHCPS_ParseConfigSglOption(TCPIP_DHCPS_CLIENT_OPTIONS* pCliOpt, const TCPIP_DHCPS_CLIENT_OPTION_CONFIG* pOptConfig)
{
    int nSets;

    TCPIP_DHCPS_RES res = TCPIP_DHCPS_RES_OK;

    switch(pOptConfig->optType)
    {
#if (TCPIP_DHCPS_OPTION_ROUTER_VALUES  != 0)
        case TCPIP_DHCPS_CLIENT_OPTION_ROUTER:
            nSets = _DHCPS_ParseIPStrOption(pOptConfig->ipStr, pCliOpt == 0 ? 0 : pCliOpt->cliRouter, sizeof(pCliOpt->cliRouter) / sizeof(*pCliOpt->cliRouter));
            if(nSets < 0)
            {
                res = TCPIP_DHCPS_RES_INVALID_ADDRESS; 
            }
            else if (pCliOpt != 0 && nSets > 0)
            {
                pCliOpt->cliOptFlags |= TCPIP_DHCPS_CLIENT_OPTION_FLAG_ROUTER; 
            }
            break;

#endif // (TCPIP_DHCPS_OPTION_ROUTER_VALUES  != 0)
#if (TCPIP_DHCPS_OPTION_DNS_VALUES  != 0)
        case TCPIP_DHCPS_CLIENT_OPTION_DNS:
            nSets = _DHCPS_ParseIPStrOption(pOptConfig->ipStr, pCliOpt == 0 ? 0 : pCliOpt->cliDNS, sizeof(pCliOpt->cliDNS) / sizeof(*pCliOpt->cliDNS));
            if(nSets < 0)
            {
                res = TCPIP_DHCPS_RES_INVALID_ADDRESS; 
            }
            else if (pCliOpt != 0 && nSets > 0)
            {
                pCliOpt->cliOptFlags |= TCPIP_DHCPS_CLIENT_OPTION_FLAG_DNS; 
            }
            break;

#endif // (TCPIP_DHCPS_OPTION_DNS_VALUES  != 0)

#if (TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES  != 0)
        case TCPIP_DHCPS_CLIENT_OPTION_TIME_SERVER:
            nSets = _DHCPS_ParseIPStrOption(pOptConfig->ipStr, pCliOpt == 0 ? 0 : pCliOpt->cliTServer, sizeof(pCliOpt->cliTServer) / sizeof(*pCliOpt->cliTServer));
            if(nSets < 0)
            {
                res = TCPIP_DHCPS_RES_INVALID_ADDRESS; 
            }
            else if (pCliOpt != 0 && nSets > 0)
            {
                pCliOpt->cliOptFlags |= TCPIP_DHCPS_CLIENT_OPTION_FLAG_TIME_SERVER; 
            }
            break;


#endif // (TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES  != 0)
#if (TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES  != 0)
        case TCPIP_DHCPS_CLIENT_OPTION_NAME_SERVER:
            nSets = _DHCPS_ParseIPStrOption(pOptConfig->ipStr, pCliOpt == 0 ? 0 : pCliOpt->cliNServer, sizeof(pCliOpt->cliNServer) / sizeof(*pCliOpt->cliNServer));
            if(nSets < 0)
            {
                res = TCPIP_DHCPS_RES_INVALID_ADDRESS; 
            }
            else if (pCliOpt != 0 && nSets > 0)
            {
                pCliOpt->cliOptFlags |= TCPIP_DHCPS_CLIENT_OPTION_FLAG_NAME_SERVER; 
            }
            break;

#endif  // (TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES  != 0)
#if (TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES  != 0)
        case TCPIP_DHCPS_CLIENT_OPTION_NTP_SERVER:
            nSets = _DHCPS_ParseIPStrOption(pOptConfig->ipStr, pCliOpt == 0 ? 0 : pCliOpt->cliNtpServer, sizeof(pCliOpt->cliNtpServer) / sizeof(*pCliOpt->cliNtpServer));
            if(nSets < 0)
            {
                res = TCPIP_DHCPS_RES_INVALID_ADDRESS; 
            }
            else if (pCliOpt != 0 && nSets > 0)
            {
                pCliOpt->cliOptFlags |= TCPIP_DHCPS_CLIENT_OPTION_FLAG_NTP_SERVER; 
            }
            break;


#endif // (TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES  != 0)

#if (_TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS == 0)
        case TCPIP_DHCPS_CLIENT_OPTION_T1_RENEWAL:
            if(pOptConfig->mult == 0 || pOptConfig->div == 0)
            {
                res = TCPIP_DHCPS_RES_T1_T2_ERR; 
            }
            else if(pCliOpt != 0)
            {
                pCliOpt->t1Mult = pOptConfig->mult;
                pCliOpt->t1Div = pOptConfig->div;
            }
            break;
            
        case TCPIP_DHCPS_CLIENT_OPTION_T2_REBINDING:
            if(pOptConfig->mult == 0 || pOptConfig->div == 0)
            {
                res = TCPIP_DHCPS_RES_T1_T2_ERR; 
            }
            else if(pCliOpt != 0)
            {
                pCliOpt->t2Mult = pOptConfig->mult;
                pCliOpt->t2Div = pOptConfig->div;
            }
            break;


#endif  // (_TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS == 0)

        default:
            (void)nSets;
            res = TCPIP_DHCPS_RES_OPTION_ERR;
            break;
    }

    return res;
}

#if (_TCPIP_DHCPS_NOTIFICATIONS_ENABLE != 0)
static void _DHCPS_NotifyClients(TCPIP_NET_IF* pktIf, TCPIP_DHCPS_EVENT_TYPE evType, DHCPS_HASH_ENTRY* he, TCPIP_DHCPS_EVENT_DATA_INFO* evInfo)
{
    int ix;
    TCPIP_DHCPS_EVENT_NODE  registeredUsers[TCPIP_DHCPS_MAX_EVENT_REGISTRATIONS];
    TCPIP_DHCPS_EVENT_DATA  evData;

   _DhcpsAssert(evType != 0, __func__, __LINE__);

    bool doNotify;

#if (TCPIP_DHCPS_REPORT_ERROR_EVENT != 0)
    if(evType < 0)
    {
        doNotify = true;
    }
#elif (TCPIP_DHCPS_REPORT_CLIENT_EVENT != 0)
    if(evType > 0)
    {
        doNotify = true;
    }
#endif

   if(doNotify)
   {
       // make a quick copy
       OSAL_CRITSECT_DATA_TYPE lock = _DHCPS_Lock();
       memcpy(registeredUsers, gDhcpDcpt->registeredUsers, sizeof(registeredUsers));
       _DHCPS_Unlock(lock);


       memset(&evData, 0, sizeof(evData));
       evData.hNet = (TCPIP_NET_HANDLE)pktIf;
       evData.evTime = _DHCPS_SecCount();
       evData.evType = (int16_t)evType;
       if(evInfo)
       {
           evData.evInfo = *evInfo;
       }
       
       if(he != 0)
       {
           int32_t leaseIndex = TCPIP_OAHASH_EntryGetIndex(he->parent->hashDcpt, &he->hEntry);
            _DhcpsAssert(leaseIndex >= 0, __func__, __LINE__);
           evData.leaseIndex = (uint16_t)leaseIndex;
           evData.evInfo.flags.leaseIndexValid = 1;

           memcpy(evData.macAdd.v, he->chaddr, sizeof(evData.macAdd));
           evData.evInfo.flags.macAddValid = 1;

           if(evData.evInfo.flags.infoIpValid == 0)
           {
               evData.evInfo.ipAddress = he->ipAddress.Val;
               evData.evInfo.flags.infoIpValid = 1;
           }
       }



       TCPIP_DHCPS_EVENT_NODE* pEvent = registeredUsers;
       for(ix = 0; ix < sizeof(registeredUsers) / sizeof(*registeredUsers); ix++, pEvent++)
       {
           if(pEvent->handler != 0)
           {   // found handler
               if(pEvent->hNet == 0 || pEvent->hNet == pktIf)
               {   // trigger event
                   (*pEvent->handler)(&evData, pEvent->hParam);
               }
           }
       }
   }

    _DHCPS_EventPrint(pktIf, evType, he, evInfo);
}
#else
#define _DHCPS_NotifyClients(pktIf, evType, he, evInfo) _DHCPS_EventPrint(pktIf, evType, he, evInfo)
#endif  // (_TCPIP_DHCPS_NOTIFICATIONS_ENABLE != 0)

bool TCPIP_DHCPS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_DHCPS_MODULE_CONFIG* pDhcpsConfig)
{    
    TCPIP_DHCPS_INTERFACE_DCPT* pIfDcpt;
    TCPIP_DHCPS_RES initRes;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        _DhcpsAssert(gDhcpDcpt != 0, __func__, __LINE__);

        if(_DHCPS_MappedOnNet(stackCtrl->pNetIf))
        {
            pIfDcpt = gDhcpDcpt->ifDcpt + stackCtrl->pNetIf->netIfIx;
            if((pIfDcpt->configFlags & TCPIP_DHCPS_CONFIG_FLAG_DELETE_OLD) != 0)
            {   // remove the old entries, if there
                _DHCPS_RemoveAllEntries(pIfDcpt, true, true);
            }
        }

        if(stackCtrl->pNetIf->Flags.bIsDHCPSrvEnabled != 0)
        {
            _DHCPS_Enable(stackCtrl->pNetIf, false);
        }
        
        return true;
    }
    
    
    // stack init
    initRes = TCPIP_DHCPS_RES_OK;
    while(dhcpSInitCount == 0)
    {   // first time we're run
        gDhcpDcpt = 0;

        if(pDhcpsConfig == 0)
        {
            initRes = TCPIP_DHCPS_RES_NO_INIT_DATA;
            break;
        }

        // allocate the descriptor
        gDhcpDcpt = (TCPIP_DHCPS_DCPT*)TCPIP_HEAP_Calloc(stackCtrl->memH, 1, sizeof(TCPIP_DHCPS_DCPT));
        if(gDhcpDcpt == 0)
        {   // failed
            initRes = TCPIP_DHCPS_RES_ALLOC_ERR;
            break;
        }

        gDhcpDcpt->memH = stackCtrl->memH;
        gDhcpDcpt->stackIfs = stackCtrl->nIfs;
        gDhcpDcpt->uSkt = INVALID_UDP_SOCKET;
        gDhcpDcpt->ifCount = (uint8_t)sizeof(gDhcpDcpt->ifDcpt) / sizeof(*gDhcpDcpt->ifDcpt);
        gDhcpDcpt->nProbes = pDhcpsConfig->nProbes ? pDhcpsConfig->nProbes : _TCPIP_DHCPS_DEFAULT_PROBE_COUNT;
        gDhcpDcpt->nReprobes = pDhcpsConfig->conflictAttempts ? pDhcpsConfig->conflictAttempts : _TCPIP_DHCPS_DEFAULT_REPROBE_COUNT;
        gDhcpDcpt->probeTmoMs = (TCPIP_ICMP_ECHO_REQUEST_TIMEOUT * 3) / 2;  // make sure we timeout after ICMP does!
        gDhcpDcpt->maxLeases = _TCPIP_DHCPS_MAX_LEASES;
        gDhcpDcpt->icmpSequenceNo = SYS_RANDOM_PseudoGet();
        gDhcpDcpt->icmpIdentifier = SYS_RANDOM_PseudoGet();
        gDhcpDcpt->maxIx = -1;
        gDhcpDcpt->stackSigHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_DHCPS_Task, TCPIP_DHCPS_TASK_PROCESS_RATE);
        if(gDhcpDcpt->stackSigHandle == 0)
        {
            initRes = TCPIP_DHCPS_RES_SIGNAL_ERR;
            break;
        }

        gDhcpDcpt->uSkt = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, TCPIP_DHCP_SERVER_PORT,  0);
        if(gDhcpDcpt->uSkt == INVALID_UDP_SOCKET)
        {
            initRes = TCPIP_DHCPS_RES_SKT_CREATE_ERR;
            break;
        }

        size_t txBuffSize;
        bool optRes = TCPIP_UDP_OptionsGet(gDhcpDcpt->uSkt, UDP_OPTION_TX_BUFF, (void*)&txBuffSize);
        if(optRes != false && txBuffSize < _TCPIP_DHCPS_MIN_REPONSE_PACKET_SIZE)
        {
            optRes = TCPIP_UDP_OptionsSet(gDhcpDcpt->uSkt, UDP_OPTION_TX_BUFF, (void*)_TCPIP_DHCPS_MIN_REPONSE_PACKET_SIZE);
        }
        if(optRes ==  false)
        {
            initRes = TCPIP_DHCPS_RES_SKT_OPTION_ERR;
            break;
        }

        optRes = TCPIP_UDP_OptionsSet(gDhcpDcpt->uSkt, UDP_OPTION_ENFORCE_STRICT_NET, (void*)false);
        if(optRes ==  false)
        {
            initRes = TCPIP_DHCPS_RES_SKT_OPTION_ERR;
            break;
        }

        if(TCPIP_UDP_SignalHandlerRegister(gDhcpDcpt->uSkt, TCPIP_UDP_SIGNAL_RX_DATA, _DHCPS_SocketRxHandler, 0) == 0)
        {
            initRes = TCPIP_DHCPS_RES_SKT_SIGNAL_ERR;
            break;
        }

        if(pDhcpsConfig->pIfConfig != 0)
        {
            initRes = _DHCPS_AddLeasePools(pDhcpsConfig->pIfConfig, pDhcpsConfig->nConfigs);
        }

        break;
    }

    if(initRes < 0 )
    {
        _DHCPS_Cleanup();
        SYS_ERROR_PRINT(SYS_ERROR_WARNING, "DHCPS: Initialization failed: %d!\r\n", initRes);
        return false;
    }
    
    if(stackCtrl->pNetIf->Flags.bIsDHCPSrvEnabled != 0)
    {   // override the DHCPs configuration with what the stack manager says
        _DHCPS_Enable(stackCtrl->pNetIf, false);
    }

    dhcpSInitCount++;

    return true;
}


#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_DHCPS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    if(dhcpSInitCount > 0)
    {   // we're up and running
        // this interface is going down no matter what
        _DHCPS_Disable(stackCtrl->pNetIf);
        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--dhcpSInitCount == 0)
            {   // all closed
                // release resources
                _DHCPS_Cleanup();
            }
        }
    }
}

// resources clean up
static void _DHCPS_Cleanup(void)
{
    int ix;
    TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt;

    if(gDhcpDcpt != 0)
    {
        if( gDhcpDcpt->uSkt != INVALID_UDP_SOCKET)
        {
            TCPIP_UDP_Close(gDhcpDcpt->uSkt);
        }

        pIDcpt = gDhcpDcpt->ifDcpt;
        for(ix = 0; ix <  gDhcpDcpt->ifCount; ix++, pIDcpt++)
        {
            if(pIDcpt->hashDcpt != 0)
            {   
                _DHCPS_RemoveAllEntries(pIDcpt, true, true);   // Remove all the HASH entries
                TCPIP_HEAP_Free(gDhcpDcpt->memH, pIDcpt->hashDcpt);       // Free HASH descriptor
                pIDcpt->hashDcpt = 0;
            }
        }

        if(gDhcpDcpt->stackSigHandle)
        {   // Free timer handler
            _TCPIPStackSignalHandlerDeregister(gDhcpDcpt->stackSigHandle);
            gDhcpDcpt->stackSigHandle = 0;
        }
        // Free Pool entry DHCP server descriptor
        TCPIP_HEAP_Free(gDhcpDcpt->memH, gDhcpDcpt);
        gDhcpDcpt = 0;
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_DHCPS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(gDhcpDcpt != 0)
    {
        if((sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
        { //  RX signal occurred
            _DHCPS_ProcessRx();
        }

        if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
        { // regular TMO occurred
            _DHCPS_ProcessTick();
        }
    }

}

// send a signal to the DHCPS module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void _DHCPS_SocketRxHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

static bool TCPIP_DHCPS_GetDataByte(TCPIP_DHCPS_DATA* getbuf, uint8_t* dst)
{
    if(getbuf->rdPtr != getbuf->endPtr)
    {
        if(dst != 0)
        {
            *dst = *getbuf->rdPtr;
        }
        getbuf->rdPtr++;
        return true;
    }
    return false;

}

static int TCPIP_DHCPS_GetDataArray(TCPIP_DHCPS_DATA *getbuf, uint8_t *destBuff, int reqBytes)
{
    int avlblBytes = getbuf->endPtr - getbuf->rdPtr;
    if(avlblBytes == 0)
    {
        return 0;
    }

    if(reqBytes > avlblBytes)
    {
        reqBytes = avlblBytes;
    }

    if(destBuff != 0)
    {
        memcpy(destBuff, getbuf->rdPtr, reqBytes);
    }

    getbuf->rdPtr += reqBytes;
    return reqBytes;
}

// process incoming DHCPS traffic
static void _DHCPS_ProcessRx(void)
{
    UDP_SOCKET          skt;
    uint16_t            avlblBytes;

    skt = gDhcpDcpt->uSkt;

    while(true)
    {
        // Check to see if a valid DHCP packet has arrived
        avlblBytes = TCPIP_UDP_GetIsReady(skt);
        if(avlblBytes == 0) 
        {
            break;
        }

        _DHCPS_ProcessSkt(skt, avlblBytes);
        // discard the current packet and point to the next queued packet
        TCPIP_UDP_Discard(skt);
    }
}


static void _DHCPS_ProcessSkt(UDP_SOCKET skt, uint16_t avlblBytes)
{
    UDP_SOCKET_INFO     udpSockInfo;
    uint32_t            dhcpCookie;
    int                 ifIx;       // DHCPs interface index
    BOOTP_HEADER*       pHeader;
    uint8_t*            rxPtr;
    TCPIP_DHCPS_EVENT_TYPE evType = TCPIP_DHCPS_EVENT_NONE;
    bool processPkt = false;
    bool accessLocked = false;
    TCPIP_NET_IF*       pktIf;  // interface on which the packet arrived
    TCPIP_DHCPS_INTERFACE_DCPT* pIfDcpt = 0; // associated DHCPs interface descriptor
    TCPIP_DHCPS_DATA    udpGetData = {0};
    TCPIP_DHCPS_EVENT_DATA_INFO evInfo;

    while(true)
    {
        evInfo.flags.val = 0;

        TCPIP_UDP_SocketInfoGet(skt, &udpSockInfo);
        pktIf = (TCPIP_NET_IF*)udpSockInfo.hNet;

        // set most common values
        evInfo.ipAddress = udpSockInfo.sourceIPaddress.v4Add.Val;
        evInfo.flags.infoIpValid = 1;

        if(_DHCPS_AccessLock() == false)
        {
            evType = TCPIP_DHCPS_EVENT_PROCESS_LOCK;
            break;
        } 

        accessLocked = true;
        if(avlblBytes < _TCPIP_DHCPS_MIN_PROCESS_PACKET_SIZE)
        {
            evType = TCPIP_DHCPS_EVENT_MSG_UNDERFLOW;
            evInfo.flags.infoSize = 1;
            evInfo.data = (uint32_t)avlblBytes;
            break;
        }

        if(pktIf->Flags.bIsDHCPSrvEnabled == 0)
        {   // arriving on wrong interface
            evType = TCPIP_DHCPS_EVENT_IF_DISABLED;
            break;
        }

        // get the associated DHCP interface
        ifIx = _DHCPS_InterfaceMapIx(pktIf);
        if(ifIx < 0)
        {   // arriving on wrong interface
            evType = TCPIP_DHCPS_EVENT_IF_ERROR;
            break;
        }
        
        pIfDcpt = gDhcpDcpt->ifDcpt + ifIx;
        _DhcpsAssert(pIfDcpt->hashDcpt != 0, __func__, __LINE__);

        if(avlblBytes > sizeof(gDhcpProcPacket))
        {   // truncate if too big
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
            pIfDcpt->statData.msgOvflCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
            evType = TCPIP_DHCPS_EVENT_MSG_OVERFLOW;
            evInfo.flags.infoSize = 1;
            evInfo.data = (uint32_t)avlblBytes;
            avlblBytes = sizeof(gDhcpProcPacket);
        }

        pIfDcpt->msgFlags = 0;
        if(udpSockInfo.destIPaddress.v4Add.Val == 0xffffffff)
        {
            pIfDcpt->msgFlags |= TCPIP_DHCPS_CLIENT_MESSAGE_FLAG_BROADCAST; 
        }

        rxPtr = gDhcpProcPacket;

        // Get Compleate Array of DHCP server Reply bytes
        TCPIP_UDP_ArrayGet(skt, rxPtr, avlblBytes);

        // point to the BOOTP header and validate
        pHeader = (BOOTP_HEADER*)rxPtr;
        if((pHeader->op != _TCPIP_DHCPS_BOOT_REQUEST) || (pHeader->htype != _TCPIP_DHCPS_BOOT_HW_TYPE) || (pHeader->hlen != _TCPIP_DHCPS_BOOT_LEN_OF_HW_TYPE))
        {
            evType = TCPIP_DHCPS_EVENT_MSG_FORMAT_ERROR;
            break;
        }

        if(!isMacAddrValid(pHeader->chaddr))
        {   // no entry stored in database for invalid MAC addresses
            evType = TCPIP_DHCPS_EVENT_MSG_FORMAT_ERROR;
            break;
        }

        // throw away: server host name, and boot file name -- unsupported
        rxPtr += sizeof(BOOTP_HEADER) + _TCPIP_DHCPS_UNUSED_BYTES_FOR_TX;


        // verify Magic Cookie
        memcpy((uint8_t*)&dhcpCookie, rxPtr, sizeof(dhcpCookie));

        if(TCPIP_Helper_htonl(dhcpCookie) != _TCPIP_DHCPS_MAGIC_COOKIE_NET)
        {   // ignore ill formatted message
            evType = TCPIP_DHCPS_EVENT_MSG_FORMAT_ERROR;
            break;
        }

        rxPtr += sizeof(dhcpCookie);

        // set the current get pointer s

        udpGetData.headPtr = gDhcpProcPacket;     // point at the begining of the buffer: BOOTP_HEADER*
        udpGetData.endPtr = gDhcpProcPacket + avlblBytes;
        udpGetData.rdPtr = rxPtr;       // current read pointer

        processPkt = true;
        break;
    }

    if(evType != TCPIP_DHCPS_EVENT_NONE)
    {
        _DHCPS_NotifyClients(pktIf, evType, 0, &evInfo);
    }

    if(processPkt)
    {
        // parse the received options
        if(_DHCPS_ParseOptions(&udpGetData, &gRxOptions, pHeader))
        {   // ignore ill formatted options
            _DHCPS_ProcessMsg(pIfDcpt, pHeader, &gRxOptions);
        }
    }
    // else ill formatted packet
    if(accessLocked)
    {
        _DHCPS_AccessUnlock();
    }
}


// processing function for DHCPs message type    
static bool _DHCPS_OptionMessageType(uint8_t* pOpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt)
{
    if(pRxOpt->messageType.optionType == 0)
    {
        TCPIP_DHCPS_OPTION_DATA_MSG_TYPE* pMsgType = (TCPIP_DHCPS_OPTION_DATA_MSG_TYPE*)pOpt;
        pRxOpt->messageType = *pMsgType;
        return true;
    }

    // multiple message types?
    return false;
}

// processing function for DHCPs request client ID
static bool _DHCPS_OptionReqClientId(uint8_t* pOpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt)
{
    if(pRxOpt->requestClientId.optionType == 0)
    {
        TCPIP_DHCPS_OPTION_REQUEST_CLIENT_ID* pCliId = (TCPIP_DHCPS_OPTION_REQUEST_CLIENT_ID*)pOpt;
        if(pCliId->hwType == _TCPIP_DHCPS_BOOT_HW_TYPE)
        {
            memcpy(&pRxOpt->requestClientId, pCliId, pCliId->optionLen + 2);
            return true;
        }
    }

    return false;
}

// processing function for DHCPs request IP address
static bool _DHCPS_OptionReqIpAddress(uint8_t* pOpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt)
{
    if(pRxOpt->requestAddress.optionType == 0)
    {
        TCPIP_DHCPS_OPTION_REQUEST_IP_ADDRESS* pReqAdd = (TCPIP_DHCPS_OPTION_REQUEST_IP_ADDRESS*)pOpt;
        pRxOpt->requestAddress = *pReqAdd;
        return true;
    }

    return false;
}

static bool _DHCPS_OptionReqLeaseTime(uint8_t* pOpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt)
{
    if(pRxOpt->requestLeaseTime.optionType == 0)
    {
        TCPIP_DHCPS_OPTION_REQUEST_LEASE_TIME* pReqLease = (TCPIP_DHCPS_OPTION_REQUEST_LEASE_TIME*)pOpt;
        pRxOpt->requestLeaseTime = *pReqLease;
        return true;
    }

    return false;

}

static bool _DHCPS_OptionReqServerIdentifier(uint8_t* pOpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt)
{
    if(pRxOpt->serverIdentifier.optionType == 0)
    {
        TCPIP_DHCPS_OPTION_SERVER_IDENTIFIER* pSrvId = (TCPIP_DHCPS_OPTION_SERVER_IDENTIFIER*)pOpt;
        pRxOpt->serverIdentifier = *pSrvId;
        return true;
    }

    return false;

}

static bool _DHCPS_ParseOptions(TCPIP_DHCPS_DATA* pGetData, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, BOOTP_HEADER* pHeader)
{
    // table for processing/parsing received options from a client
    // options not in this table are ignored/skipped 
    static const _DHCPS_OPT_DCPT    _DHCPS_OptionDcptTbl[] = 
    {
        // option type                   // minLen   //max Len                           // process function
        {DHCP_OPTION_MESSAGE_TYPE,       1,          1,                                  _DHCPS_OptionMessageType},
        {DHCP_OPTION_CLIENT_ID,          2,          _TCPIP_DHCPS_CLIENT_ID_SIZE + 1,    _DHCPS_OptionReqClientId},
        {DHCP_OPTION_REQUEST_IP_ADDRESS, 4,          4,                                  _DHCPS_OptionReqIpAddress},
        {DHCP_OPTION_IP_LEASE_TIME,      4,          4,                                  _DHCPS_OptionReqLeaseTime},
        {DHCP_OPTION_SERVER_IDENTIFIER,  4,          4,                                  _DHCPS_OptionReqServerIdentifier},
    };

    int ix;
    uint8_t option, optionLen;
    bool    optRes;
    const _DHCPS_OPT_DCPT *pDcpt, *pFoundDcpt;
    uint8_t* pCurrOpt;   
    size_t      keySize;
    uint8_t*    pKeySrc;

    // start fresh
    memset(pRxOpt, 0, sizeof(*pRxOpt));

    while(true)
    {
        pCurrOpt = pGetData->rdPtr;     // save the beginning of the option

        // Get option type
        if(!TCPIP_DHCPS_GetDataByte(pGetData, &option))
        {   // we're done; but should be ended by END
            return false;
        }

        if(option == DHCP_OPTION_END)
        {   // done; ended successfully
            break;
        }

        // Get option length
        if(!TCPIP_DHCPS_GetDataByte(pGetData, &optionLen))
        {
            return false;
        }

        // make sure all data is in there and advance to the next option
        if(TCPIP_DHCPS_GetDataArray(pGetData, 0, optionLen) != optionLen)
        {
            return false;
        }

        // check if we process this option type
        // simple linear search
        pDcpt = _DHCPS_OptionDcptTbl;
        pFoundDcpt = 0;
        for(ix = 0; ix < sizeof(_DHCPS_OptionDcptTbl) / sizeof(*_DHCPS_OptionDcptTbl); ix++, pDcpt++)
        {
            if(pDcpt->optionType == option)
            {
                pFoundDcpt = pDcpt;
                break;
            }
        }

        if(pFoundDcpt)
        {   // do a minimum check
            if(optionLen < pFoundDcpt->minLen || optionLen > pFoundDcpt->maxLen)
            {   // wrong option, reject
                optRes = false;
            }
            else
            {
                optRes = pFoundDcpt->optFunc(pCurrOpt, pRxOpt);
            }

            if(optRes == false)
            {   // failed...
                return false;
            }
        }
        // else unknown option; ignore

    }

    // check that we have received a proper DHCP_OPTION_MESSAGE_TYPE
    if(pRxOpt->messageType.optionType == DHCP_OPTION_MESSAGE_TYPE)
    {   // all good; set the client identification
        if(pRxOpt->requestClientId.optionType == DHCP_OPTION_CLIENT_ID) 
        {   // use the requested client ID
            keySize = pRxOpt->requestClientId.optionLen - 1;
            pKeySrc = pRxOpt->requestClientId.clientId;
        }
        else
        {   // regular chaddr
            keySize = sizeof(TCPIP_MAC_ADDR);
            pKeySrc = pHeader->chaddr;
        }
        
        // properly format the clientId to the standard size
        // and save the actual client ID zise
        pRxOpt->idSize = _DHCPS_FormatHashKey(pRxOpt->clientId, pKeySrc, keySize);

        return true;
    }

    return false;
}

// process the DHCP message
// pRxOpt contains the parsed options
static void _DHCPS_ProcessMsg(TCPIP_DHCPS_INTERFACE_DCPT* pIfDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt)
{
    // table with the processed messages
    static const DHCPS_MSG_PROC_DCPT    DHCPS_MsgProcTbl[] =
    {
        // msgType                      // procFlags                    // procFunc
        {DHCP_MESSAGE_TYPE_DISCOVER,    DHCPS_PROC_FLAG_CHECK_SIZE,     _DHCPS_ReplyToDiscovery}, 
        {DHCP_MESSAGE_TYPE_REQUEST,     DHCPS_PROC_FLAG_CHECK_SIZE,     _DHCPS_ReplyToRequest}, 
        {DHCP_MESSAGE_TYPE_RELEASE,     DHCPS_PROC_FLAG_NONE,           _DHCPS_ReplyToRelease}, 
        {DHCP_MESSAGE_TYPE_DECLINE,     DHCPS_PROC_FLAG_NONE,           _DHCPS_ReplyToDecline}, 
        {DHCP_MESSAGE_TYPE_INFORM,      DHCPS_PROC_FLAG_CHECK_SIZE,     _DHCPS_ReplyToInform}, 
    };

    int                 ix;
    UDP_SOCKET          skt;
    DHCPS_HASH_ENTRY    *he, *oldHe;

    oldHe = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(pIfDcpt->hashDcpt, pRxOpt->clientId);
    _DhcpsAssert(oldHe == 0 || oldHe->parent == pIfDcpt, __func__, __LINE__);

    const DHCPS_MSG_PROC_DCPT* pProcEntry = DHCPS_MsgProcTbl;

    for(ix = 0; ix < sizeof(DHCPS_MsgProcTbl) / sizeof(*DHCPS_MsgProcTbl); ix++, pProcEntry++)
    {
        if(pRxOpt->messageType.type == pProcEntry->msgType)
        {   // found processing entry
            // check flags
            if((pProcEntry->procFlags & DHCPS_PROC_FLAG_CHECK_SIZE) != 0)
            {
                skt = gDhcpDcpt->uSkt;
                // check enough space is available to generate the DHCP response
                if(TCPIP_UDP_PutIsReady(skt) < _TCPIP_DHCPS_MIN_REPONSE_PACKET_SIZE)
                {   // should not happen
                    _DhcpsAssert(false, __func__, __LINE__);
                    return;
                }
            }

            if(oldHe != 0)
            {   // save client info
                _DhcpsAssert(oldHe->parent == pIfDcpt, __func__, __LINE__);
                // store the info needed for processing
                _DHCPS_SaveClientState(oldHe, pHeader, pProcEntry->msgType);
            }

            // process
            he = oldHe;
            TCPIP_DHCPS_LEASE_STATE newState = pProcEntry->procFunc(pIfDcpt, pHeader, pRxOpt, &he);
            if(he != 0)
            {
                _DhcpsAssert(newState != TCPIP_DHCPS_LEASE_STATE_IDLE, __func__, __LINE__);
                if(oldHe == 0)
                {   // new entry; client info needs saving
                    _DHCPS_SaveClientState(he, pHeader, pProcEntry->msgType);
                }
                _DHCPS_SetEntryState(he, newState);
            }
            break;  // done
        }
    }

}

// stores the client info needed for processing
static void _DHCPS_SaveClientState(DHCPS_HASH_ENTRY* he, BOOTP_HEADER* pHeader, DHCP_MESSAGE_TYPE_VAL msgType)
{
    he->ciaddr = pHeader->ciaddr;    // client should use 0 for the Discovery message
    he->giaddr = pHeader->giaddr;  // save where the request came from
    he->xid = pHeader->xid;  // save the client xid
    he->cliFlags = pHeader->flags;  // save the client flags
    memcpy(he->chaddr, pHeader->chaddr, sizeof(he->chaddr));
    he->cliMsgType = msgType;
}


// RFC 2131 3.1 Client-server interaction - allocating a network address - pg 13/46
// 1. The client broadcasts a DHCPDISCOVER message on its local physical
//       subnet.  The DHCPDISCOVER message MAY include options that suggest
//       values for the network address and lease duration.  BOOTP relay
//       agents may pass the message on to DHCP servers not on the same
//       physical subnet.
// 
//    2. Each server may respond with a DHCPOFFER message that includes an
//       available network address in the ’yiaddr’ field (and other
//       configuration parameters in DHCP options).  Servers need not
//       reserve the offered network address, although the protocol will
//       work more efficiently if the server avoids allocating the offered
//       network address to another client.  When allocating a new address,
//       servers SHOULD check that the offered network address is not
//       already in use;

// Reply to a DHCP Discovery message
// RFC 2131 4.3.1 DHCPDISCOVER message. pg 27/46:
/*
   When a server receives a DHCPDISCOVER message from a client, the
   server chooses a network address for the requesting client.  If no
   address is available, the server may choose to report the problem to
   the system administrator. 
    
   If an address is available, the new address SHOULD be chosen as follows:

      o The client’s current address as recorded in the client’s current
        binding, ELSE

      o The client’s previous address as recorded in the client’s (now
        expired or released) binding, if that address is in the server’s
        pool of available addresses and not already allocated, ELSE

      o The address requested in the ’Requested IP Address’ option, if that
        address is valid and not already allocated, ELSE

      o A new address allocated from the server’s pool of available
        addresses; the address is selected based on the subnet from which
        the message was received (if ’giaddr’ is 0) or on the address of
        the relay agent that forwarded the message (’giaddr’ when not 0).


        While not required for correct operation of DHCP,
        the server SHOULD NOT reuse the selected network address before the client responds to
        the server’s DHCPOFFER message.
        The server may choose to record the address as offered to the client.
*/

static TCPIP_DHCPS_LEASE_STATE _DHCPS_ReplyToDiscovery(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe)
{
    TCPIP_DHCPS_LEASE_STATE newState = TCPIP_DHCPS_LEASE_STATE_SEND_OFFER;
    TCPIP_DHCPS_EVENT_DATA_INFO evInfo;

    evInfo.flags.val = 0;

    DHCPS_HASH_ENTRY* he = *ppHe;

    // use the client current or expired binding, if exists
    if(he == 0 )
    {   // no previous entry for this client
        int reqIndex = -1;  // client required index/IP 
                            // if < 0 it's either invalid or unavailable

        // use the Requested IP address if present
        if(pRxOpt->requestAddress.optionType == DHCP_OPTION_REQUEST_IP_ADDRESS)
        {   // check client requested IP address is valid
            reqIndex = IpIsAddressValid(pIDcpt, pRxOpt->requestAddress.reqIpAddr, DHCPS_VALID_CHECK_FREE_AND_TAKE);
        }

        if(reqIndex < 0)
        {   // last choice, allocate a new address from the pool
            // Use of giaddr: RFC 2131 pg 27/46:
            //      Thus, DHCP does not require that the client be assigned as address from the subnet in ’giaddr’
            // Currently we do not support multiple pools
            reqIndex = IpIndexFindFree(pIDcpt, true);
            if(reqIndex < 0)
            {   // no pool entry available
                _DHCPS_NotifyClients(pIDcpt->pNetIf, TCPIP_DHCPS_EVENT_POOL_EMPTY, 0, 0);
                return TCPIP_DHCPS_LEASE_STATE_IDLE;
            } 
        }

        // assign an entry to this client in the database
        // do not set the state yet
        he = _DHCPS_AssignNewOffer(pIDcpt, pRxOpt, reqIndex);
        if( he == 0) 
        {   // data base full
            // release the acquired index
            IpIndexMarkFree(pIDcpt, reqIndex);
            evInfo.flags.infoSlots = 1;
            evInfo.data = (uint32_t)pIDcpt->hashDcpt->fullSlots;
            _DHCPS_NotifyClients(pIDcpt->pNetIf, TCPIP_DHCPS_EVENT_CACHE_FULL, 0, &evInfo);
            return TCPIP_DHCPS_LEASE_STATE_IDLE;
        }

        // assigned new hash entry
        // set its state it so we can send a notification
        _DHCPS_SaveClientState(he, pHeader, DHCP_MESSAGE_TYPE_DISCOVER);

        *ppHe = he;
        if((pIDcpt->configFlags & TCPIP_DHCPS_CONFIG_FLAG_NO_CONFLICT_DETECT) == 0)
        {
            _DHCPS_InitProbe(he, true);
            newState = TCPIP_DHCPS_LEASE_STATE_SEND_PROBE;
        } 
        // else regular TCPIP_DHCPS_LEASE_STATE_SEND_OFFER state
    }
    else
    {   // we already have this lease
        if(he->state < TCPIP_DHCPS_LEASE_STATE_BOUND)
        {   // keep doing what you're doing...
            return he->state;
        }
        // else: grant a new lease 
                // state == TCPIP_DHCPS_LEASE_STATE_BOUND:
                //      if the client specifically requested a lease, grant a new lease/offer
                // state == TCPIP_DHCPS_LEASE_STATE_RELEASED || state == TCPIP_DHCPS_LEASE_STATE_EXPIRED
                //      dead lease; ressurect/offer
                // state == TCPIP_DHCPS_LEASE_STATE_INFORM
                //      client coming back to get a lease (?)
    }

    // choose the lease time
    /* RFC 2131 4.3.1 for lease time expiration pg 28/46:
          The server must also choose an expiration time for the lease, as
          follows:
      
           o IF the client has not requested a specific lease in the
             DHCPDISCOVER message and the client already has an assigned network
             address, the server returns the lease expiration time previously
             assigned to that address
             (note that the client must explicitly request a specific lease to extend
             the expiration time on a previously assigned address),
             
             ELSE

           o IF the client has not requested a specific lease in the
             DHCPDISCOVER message and the client does not have an assigned
             network address, the server assigns a locally configured default
             lease time,
             
             ELSE

           o IF the client has requested a specific lease in the DHCPDISCOVER
             message (regardless of whether the client has an assigned network
             address), the server may choose either to return the requested
             lease (if the lease is acceptable to local policy) or select
             another lease.
    */

    uint32_t cliReqTime;
    uint32_t currTime = _DHCPS_SecCount();
    if(_DHCPS_ClientReqLeaseTime(pIDcpt, pRxOpt, &cliReqTime))
    {   // client wants specific time; restart the lease
        _DHCPS_LeaseRestart(he, cliReqTime);
    }
    else
    {   // client has not requested specific lease time
        if(he->state == TCPIP_DHCPS_LEASE_STATE_BOUND)
        {   // keep the existent leaseTime; do not restart
            _DhcpsAssert(he->leaseEndTime >= currTime, __func__, __LINE__);
            he->cliLeaseTime = he->leaseEndTime - currTime;
        }
        else
        {
            _DHCPS_LeaseRestart(he, pIDcpt->cliLeaseDuration);
        }
    }

    _DHCPS_NotifyClients(pIDcpt->pNetIf, TCPIP_DHCPS_EVENT_DISCOVER, he, 0);

    return newState;
    

}

// Replies to a DHCP Inform message.
//
// RFC 2131: 3.4 pg 20/46
//     If a client has obtained a network address through some other means
//     (e.g., manual configuration), it may use a DHCPINFORM request message
//     to obtain other local configuration parameters.  Servers receiving a
//     DHCPINFORM message construct a DHCPACK message with any local
//     configuration parameters appropriate for the client without:
//     allocating a new address, checking for an existing binding, filling
//     in ’yiaddr’ or including lease time parameters.  The servers SHOULD
//     unicast the DHCPACK reply to the address given in the ’ciaddr’ field
//     of the DHCPINFORM message.
//     The server SHOULD check the network address in a DHCPINFORM message
//     for consistency, but MUST NOT check for an existing lease.  The
//     server forms a DHCPACK message containing the configuration
//     parameters for the requesting client and sends the DHCPACK message
//     directly to the client.
// 
// 
// RFC 2131: 4.3.5 DHCPINFORM message - pg 33/46
//      The server responds to a DHCPINFORM message by sending a DHCPACK
//      message directly to the address given in the ’ciaddr’ field of the
//      DHCPINFORM message.
//      The server MUST NOT send a lease expiration time to the client
//      and SHOULD NOT fill in ’yiaddr’.
//      The server includes other parameters in the DHCPACK message as defined in section 4.3.1.
//
//
static TCPIP_DHCPS_LEASE_STATE _DHCPS_ReplyToInform(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe)
{
    DHCP_MESSAGE_TYPE_VAL srvMsgType = DHCP_MESSAGE_TYPE_NAK;
    DHCPS_HASH_ENTRY* he = *ppHe;

    TCPIP_DHCPS_EVENT_TYPE evType = TCPIP_DHCPS_EVENT_INFORM;
    TCPIP_DHCPS_EVENT_DATA_INFO evInfo;
    evInfo.flags.val = 0;

    evInfo.flags.infoIpValid = 1;
    evInfo.ipAddress = pHeader->ciaddr;
    TCPIP_DHCPS_LEASE_STATE newState = TCPIP_DHCPS_LEASE_STATE_INFORM; 

    // basic sanity check
    while(true)
    {
        if(pRxOpt->requestAddress.optionType == DHCP_OPTION_REQUEST_IP_ADDRESS)
        {   // Requested IP address  MUST NOT be in an INFORM message
            evType = TCPIP_DHCPS_EVENT_INFO_FORMAT_ERROR; 
            evInfo.flags.infoFmtMask = 1;
            evInfo.data = TCPIP_DHCPS_REQ_FORMAT_ERR_REQUEST_IP_SET; 
            break;
        }

        // check client requested IP address is valid
        int reqIndex = IpIsAddressValid(pIDcpt, pHeader->ciaddr, 0);
        if(reqIndex < 0)
        {   // doesn't seem to be part of our network
            evType = TCPIP_DHCPS_EVENT_INFORM_INVALID; 
            break;
        }
        
        // address OK; do NOT check for existing lease/or if already taken
        // mark it as busy, just in case
        IpIndexMarkBusy(pIDcpt, reqIndex);

        // ok, somehow an available address
        if(he == 0)
        {   // no existing info for this client...we need some to be able to send an DHCPACK message!
            // assign an entry to this client in the database
            he = _DHCPS_AssignNewOffer(pIDcpt, pRxOpt, reqIndex);
            if(he == 0) 
            {   // data base full
                // keep the acquired index as busy
                evType = TCPIP_DHCPS_EVENT_CACHE_FULL;
                evInfo.flags.infoSlots = 1;
                evInfo.data = (uint32_t)pIDcpt->hashDcpt->fullSlots; 
                break;
            }

            // assigned new hash entry
            *ppHe = he;
        }
        else
        {   // already have something for this client
            if(he->ipAddress.Val == pHeader->ciaddr && he->state == TCPIP_DHCPS_LEASE_STATE_BOUND)
            {   // some clients (Windows) send inform after bound (?)...keep them bound
                newState = TCPIP_DHCPS_LEASE_STATE_BOUND;
            }
            else
            {   // update the info
                he->ipAddress.Val = pHeader->ciaddr;
            }
        }

        // success
        srvMsgType = DHCP_MESSAGE_TYPE_ACK;
        break;
    }

    _DHCPS_NotifyClients(pIDcpt->pNetIf, evType, he, &evInfo);

    if(he != 0)
    {
        if(srvMsgType == DHCP_MESSAGE_TYPE_ACK)
        {
            he->srvMsgType = DHCP_MESSAGE_TYPE_ACK;
            _DHCPS_SendMessage(he);
        }

        return newState;
    }


   return  TCPIP_DHCPS_LEASE_STATE_IDLE;
}



// Replies to a DHCP Request message.
static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_NoRecord(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);

typedef TCPIP_DHCPS_LEASE_STATE (*_DHCPReplyToRequestFnc)(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);

static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_Idle(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);
static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_Ignore(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);
static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_Offerred(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);
static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_Bound(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);
static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_Expired(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);



static const _DHCPReplyToRequestFnc _DHCPReplyToRequesTBL[] = 
{
    _DHCPReplyToRequest_Idle,       // TCPIP_DHCPS_LEASE_STATE_IDLE
    _DHCPReplyToRequest_Ignore,     // TCPIP_DHCPS_LEASE_STATE_SEND_PROBE
    _DHCPReplyToRequest_Ignore,     // TCPIP_DHCPS_LEASE_STATE_WAIT_PROBE
    _DHCPReplyToRequest_Ignore,     // TCPIP_DHCPS_LEASE_STATE_REPROBE
    _DHCPReplyToRequest_Ignore,     // TCPIP_DHCPS_LEASE_STATE_SEND_OFFER
    _DHCPReplyToRequest_Offerred,   // TCPIP_DHCPS_LEASE_STATE_OFFERED
    _DHCPReplyToRequest_Bound,      // TCPIP_DHCPS_LEASE_STATE_BOUND
    _DHCPReplyToRequest_Expired,    // TCPIP_DHCPS_LEASE_STATE_RELEASED
    _DHCPReplyToRequest_Expired,    // TCPIP_DHCPS_LEASE_STATE_EXPIRED


};

static TCPIP_DHCPS_LEASE_STATE _DHCPS_ReplyToRequest(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe)
{
    // figure out in what state we are
    DHCPS_HASH_ENTRY* he = *ppHe;

    if(he == 0)
    {   // no record of this client
        return _DHCPReplyToRequest_NoRecord(pIDcpt, pHeader, pRxOpt, ppHe);
    }

    // else respond according to state
    return _DHCPReplyToRequesTBL[he->state](pIDcpt, pHeader, pRxOpt, ppHe);
}

// he == 0
// no previous record of this client
//  DHCPREQUEST generated during INIT-REBOOT state pg 32/46
static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_NoRecord(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe)
{
    uint32_t requestedIpAddr = (pRxOpt->requestAddress.optionType == DHCP_OPTION_REQUEST_IP_ADDRESS) ? pRxOpt->requestAddress.reqIpAddr : 0; 

    TCPIP_DHCPS_EVENT_TYPE evType = TCPIP_DHCPS_EVENT_NONE; 
    TCPIP_DHCPS_EVENT_DATA_INFO evInfo;
    evInfo.flags.val = 0;

    evInfo.flags.infoIpValid = 1;
    evInfo.ipAddress = requestedIpAddr; 

    // do some sanity checks
    if((pIDcpt->configFlags & TCPIP_DHCPS_CONFIG_FLAG_NO_RECORD_KEEP_SILENT) != 0)
    {   // keep silent but report
        evType = TCPIP_DHCPS_EVENT_REQ_UNKNOWN;
    }
    else if(pRxOpt->serverIdentifier.optionType == DHCP_OPTION_SERVER_IDENTIFIER)
    {   // there should NOT be a server identifier! ignore but notify ill formatted message
        evType = TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR;
        evInfo.flags.infoFmtMask = 1;
        evInfo.data = TCPIP_DHCPS_REQ_FORMAT_ERR_SRV_ID_SET;
    }
    else if(pRxOpt->requestAddress.optionType != DHCP_OPTION_REQUEST_IP_ADDRESS) 
    {   // there should be a requested IP address. ignore but notify ill formatted message
        evType = TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR;
        evInfo.flags.infoFmtMask = 1;
        evInfo.data = TCPIP_DHCPS_REQ_FORMAT_ERR_REQUEST_IP_MISS;
    }
    else if(pHeader->ciaddr != 0)
    {
        evType = TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR;
        evInfo.flags.infoFmtMask = 1;
        evInfo.data = TCPIP_DHCPS_REQ_FORMAT_ERR_CIADDR_ERR;
    } 
    else if( (pHeader->giaddr == 0 && ((pIDcpt->startIPAddress.Val ^ requestedIpAddr) & pIDcpt->ipMaskAddress.Val) == 0) ||
            ((pIDcpt->startIPAddress.Val ^ pHeader->giaddr) & pIDcpt->ipMaskAddress.Val) == 0)
    {   // good client address... since we do not have any record for this client (he == 0) we should stay silent:
        // Note: for giaddr != 0, the server should know the 'remote subnet mask' to check the giaddr
        // DHCPREQUEST generated during INIT-REBOOT state pg 32/46:
        //  If the DHCP server has no record of this client, then it MUST remain silent,
        //  and MAY output a warning to the network administrator.
        evType = TCPIP_DHCPS_EVENT_REQ_UNKNOWN;
    }
    else
    {   // wrong network address: should be NAK-ed
        // create a 'fake' hash entry just for this NAK since there's no record
        // Note: if unicast is needed (giaddr != 0) and an ARP injection is needed
        // then this ARP entry will not be removed as this hash entry is fake
        // However, since this is the address of the relay server it shouldn't be a problem
        DHCPS_HASH_ENTRY fakeHe;
        DHCPS_HASH_ENTRY* he = &fakeHe;

        // initialize it
        he->parent = pIDcpt;
        _DHCPS_SaveClientState(he, pHeader, DHCP_MESSAGE_TYPE_REQUEST);
        he->srvMsgType = DHCP_MESSAGE_TYPE_NAK;
        he->nakCode = DHCP_NAK_CODE_REQ_ADDRESS_INVALID;
        _DHCPS_SendMessage(he);
    }


    if(evType != 0)
    {   
        _DHCPS_NotifyClients(pIDcpt->pNetIf, evType, 0, &evInfo);
    }

    return TCPIP_DHCPS_LEASE_STATE_IDLE;
    
}



// he->state == TCPIP_DHCPS_LEASE_STATE_IDLE
// which is invalid
static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_Idle(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe)
{
    _DhcpsAssert(false, __func__, __LINE__);   // this should NOT happen
    return TCPIP_DHCPS_LEASE_STATE_IDLE;
}

static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_Ignore(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe)
{
    DHCPS_HASH_ENTRY* he = *ppHe;
    TCPIP_DHCPS_EVENT_DATA_INFO evInfo;
    evInfo.flags.val = 0;

    evInfo.flags.infoIpValid = 1;
    evInfo.ipAddress = he->ipAddress.Val;
    evInfo.flags.infoState = 1;
    TCPIP_DHCPS_LEASE_STATE currState = he->state;
    evInfo.data = currState;

    _DHCPS_NotifyClients(pIDcpt->pNetIf, TCPIP_DHCPS_EVENT_REQ_UNEXPECT, he, &evInfo);

    return currState;
}


// DHCPREQUEST from a bound client
// RFC 2131 4.3.2 DHCPREQUEST message pg 32/46:
//
// DHCPREQUEST generated during RENEWING state:
//      ’server identifier’ MUST NOT be filled in, ’requested IP address’ option MUST NOT be filled in,
//      ’ciaddr’ MUST be filled in with client’s IP address.
//      In this situation, the client is completely configured, and is trying to extend its lease.
//      This message will be unicast, so no relay agents will be involved in its transmission.
//      Because ’giaddr’ is therefore not filled in, the DHCP server will trust the value in ’ciaddr’,
//      and use it when replying to the client.
//
//      A client MAY choose to renew or extend its lease prior to T1.
//      The server may choose not to extend the lease (as a policy decision by the network administrator),
//      but should return a DHCPACK message regardless.
//       
// DHCPREQUEST generated during REBINDING state:   
//      ’server identifier’ MUST NOT be filled in, ’requested IP address’ option MUST NOT be filled in,
//      ’ciaddr’ MUST be filled in with client’s IP address.
//      In this situation, the client is completely configured, and is trying to extend its lease.
//      This message MUST be broadcast to the 0xffffffff IP broadcast address.
//      The DHCP server SHOULD check ’ciaddr’ for correctness before replying to the DHCPREQUEST.
//   
//      The DHCPREQUEST from a REBINDING client is intended to accommodate sites
//      that have multiple DHCP servers and a mechanism for maintaining consistency
//      among leases managed by multiple servers.
//      A DHCP server MAY extend a client’s lease only if it has local administrative authority to do so.
//    
static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_Bound(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe)
{
    TCPIP_DHCPS_EVENT_DATA_INFO evInfo;
    evInfo.flags.val = 0;

    DHCPS_HASH_ENTRY* he = *ppHe;

    TCPIP_DHCPS_LEASE_STATE newState = he->state;

    uint32_t currTime = _DHCPS_SecCount();

    TCPIP_DHCPS_EVENT_TYPE evType = TCPIP_DHCPS_EVENT_NONE; 

    if(pRxOpt->serverIdentifier.optionType == DHCP_OPTION_SERVER_IDENTIFIER)
    {   // there should NOT be a server identifier! ignore but notify ill formatted message
        evType = TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR;
        evInfo.flags.infoFmtMask = 1;
        evInfo.data = TCPIP_DHCPS_REQ_FORMAT_ERR_SRV_ID_SET;
    }
    else if(pRxOpt->requestAddress.optionType == DHCP_OPTION_REQUEST_IP_ADDRESS) 
    {   // there should NOT be a requested IP address. ignore but notify ill formatted message
        evType = TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR;
        evInfo.flags.infoFmtMask = 1;
        evInfo.data = TCPIP_DHCPS_REQ_FORMAT_ERR_REQUEST_IP_SET;
    }
    else if(pHeader->ciaddr != he->ipAddress.Val)
    {
        evType = TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR;
        evInfo.flags.infoFmtMask = 1;
        evInfo.data = TCPIP_DHCPS_REQ_FORMAT_ERR_CIADDR_ERR;
    } 
    else
    {   // all good...
        // check unicast: renewing, broadcast: rebinding
        bool bRenew = ((pIDcpt->msgFlags & TCPIP_DHCPS_CLIENT_MESSAGE_FLAG_BROADCAST) == 0) ? true : false;
        he->srvMsgType = DHCP_MESSAGE_TYPE_ACK;
        newState = TCPIP_DHCPS_LEASE_STATE_BOUND; 

        // check the lease time
        if((pIDcpt->configFlags & TCPIP_DHCPS_CONFIG_FLAG_NO_LEASE_EXTEND) == 0)
        {   // we can extend the lease
            uint32_t cliReqTime;
            if(!_DHCPS_ClientReqLeaseTime(pIDcpt, pRxOpt, &cliReqTime))
            {   // client has not requested a specific lease; use the default value
                cliReqTime = pIDcpt->cliLeaseDuration;
            }

            // restart the lease
            _DHCPS_LeaseRestart(he, cliReqTime);
        }
        else
        {   // cannot extend the lease; advertise what's left
            _DhcpsAssert(he->leaseEndTime >= currTime, __func__, __LINE__);
            he->cliLeaseTime = he->leaseEndTime - currTime;
        }

#if ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_IGNORE_RENEW) != 0) 
        if(bRenew == false)
        { // ignore renew request
            _DHCPS_SendMessage(he);
        }
#else
        _DHCPS_SendMessage(he);
#endif  // ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_IGNORE_RENEW) != 0) 
        evType = bRenew ? TCPIP_DHCPS_EVENT_REQUEST_RENEW : TCPIP_DHCPS_EVENT_REQUEST_REBIND; 
    }

    if(evType != 0)
    {   
        _DHCPS_NotifyClients(pIDcpt->pNetIf, evType, he, &evInfo);
    }

    return newState;
}

// DHCPREQUEST from an (old) client
// we have info about this but there is no active lease
// he->state == TCPIP_DHCPS_LEASE_STATE_RELEASED
// he->state == TCPIP_DHCPS_LEASE_STATE_EXPIRED
//
// RFC 2131 Using a previously allocated network address - INIT_REBOOT message pg 18/46:
//      2. Servers with knowledge of the client’s configuration parameters
//      respond with a DHCPACK message to the client.  Servers SHOULD NOT
//      check that the client’s network address is already in use;
//
//      pg 22/46:
//      If a server receives a DHCPREQUEST message with an invalid ’requested
//      IP address’, the server SHOULD respond to the client with a DHCPNAK
//      message and may choose to report the problem to the system administrator. 
//
// RFC 2131 4.3.2 DHCPREQUEST message pg 31/46:
// DHCPREQUEST generated during INIT-REBOOT state:
//
//  ’server identifier’ MUST NOT be filled in, ’requested IP address’
//      option MUST be filled in with client’s notion of its previously
//      assigned address. ’ciaddr’ MUST be zero.
//      The client is seeking to verify a previously allocated, cached configuration.
//
//      Server SHOULD send a DHCPNAK message to the client if the ’requested IP address’
//      is incorrect, or is on the wrong network.
//
//
static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_Expired(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe)
{
    TCPIP_DHCPS_EVENT_DATA_INFO evInfo;
    evInfo.flags.val = 0;

    DHCPS_HASH_ENTRY* he = *ppHe;

    TCPIP_DHCPS_LEASE_STATE newState = he->state;

    TCPIP_DHCPS_EVENT_TYPE evType = TCPIP_DHCPS_EVENT_NONE; 

    if(pRxOpt->serverIdentifier.optionType == DHCP_OPTION_SERVER_IDENTIFIER)
    {   // there should NOT be a server identifier! ignore but notify ill formatted message
        evType = TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR;
        evInfo.flags.infoFmtMask = 1;
        evInfo.data = TCPIP_DHCPS_REQ_FORMAT_ERR_SRV_ID_SET;
    }
    else if(pRxOpt->requestAddress.optionType != DHCP_OPTION_REQUEST_IP_ADDRESS) 
    {   // there should be a requested IP address. ignore but notify ill formatted message
        evType = TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR;
        evInfo.flags.infoFmtMask = 1;
        evInfo.data = TCPIP_DHCPS_REQ_FORMAT_ERR_REQUEST_IP_MISS;
    }
    else
    {
        // check that the client idea of its IP address is correct
        _DhcpsAssert(he->ipAddress.Val != 0, __func__, __LINE__);   // an he should have a valid address
        if(pRxOpt->requestAddress.reqIpAddr == he->ipAddress.Val)
        {   // all looks good 
            // we can extend the lease
            uint32_t cliReqTime;
            if(!_DHCPS_ClientReqLeaseTime(pIDcpt, pRxOpt, &cliReqTime))
            {   // client has not requested a specific lease; use the default value
                cliReqTime = pIDcpt->cliLeaseDuration;
            }

            // restart the lease
            _DHCPS_LeaseRestart(he, cliReqTime);
            he->srvMsgType = DHCP_MESSAGE_TYPE_ACK;
            newState = TCPIP_DHCPS_LEASE_STATE_BOUND; 
            evType = TCPIP_DHCPS_EVENT_REQUEST_EXPIRED; 
        }
        else
        {
            he->srvMsgType = DHCP_MESSAGE_TYPE_NAK;
            he->nakCode = DHCP_NAK_CODE_REQ_ADDRESS_INVALID;
            // retain expired state
            evType = TCPIP_DHCPS_EVENT_REQ_ADDRESS_ERROR;
            evInfo.flags.infoReqIp = 1;
            evInfo.data = pRxOpt->requestAddress.reqIpAddr;
        }
        _DHCPS_SendMessage(he);
    }

    if(evType != 0)
    {   
        _DHCPS_NotifyClients(pIDcpt->pNetIf, evType, he, &evInfo);
    }

    return newState;
}

// waiting for a DHCPREQUEST from the client - 'proper' state
// he->state == TCPIP_DHCPS_LEASE_STATE_OFFERED
//
// RFC 2131 4.3.2 DHCPREQUEST message pg 30/46:
// If the DHCPREQUEST message contains a ’server identifier’ option,
//      the message is in response to a DHCPOFFER message.
//      Otherwise, the message is a request to verify or extend an existing lease. 
//
// RFC 2131 3.1 Client-server interaction - allocating a network address - pg 16/46
//      3. ... The client chooses one server from which to request configuration
//      parameters, based on the configuration parameters offered in the
//      DHCPOFFER messages.
//      The client broadcasts a DHCPREQUEST message that MUST include the ’server identifier’
//      option to indicate which server it has selected, and that MAY include other options
//      specifying desired configuration values. 
//      The ’requested IP address’ option MUST be set to the value of ’yiaddr’ in the
//      DHCPOFFER message from the server.
//      This DHCPREQUEST message is broadcast and relayed through DHCP/BOOTP relay agents.
//      ...
//
//      4. The servers receive the DHCPREQUEST broadcast from the client.
//      Those servers not selected by the DHCPREQUEST message use the
//      message as notification that the client has declined that server’s
//      offer. 
//      The server selected in the DHCPREQUEST message commits the
//      binding for the client to persistent storage and responds with a
//      DHCPACK message containing the configuration parameters for the
//      requesting client.
//      The combination of ’client identifier’ or
//      ’chaddr’ and assigned network address constitute a unique
//      identifier for the client’s lease and are used by both the client
//      and server to identify a lease referred to in any DHCP messages.
// 
//      ...
//
//      If the selected server is unable to satisfy the DHCPREQUEST message
//      (e.g., the requested network address has been allocated), the
//      server SHOULD respond with a DHCPNAK message.
// 
//      A server MAY choose to mark addresses offered to clients in
//      DHCPOFFER messages as unavailable.  The server SHOULD mark an
//      address offered to a client in a DHCPOFFER message as available if
//      the server receives no DHCPREQUEST message from that client.
//
//
// RFC 2131 4.3.2 pg 31/46:
//  DHCPREQUEST generated during SELECTING state:
//      Client inserts the address of the selected server in ’server identifier’,
//      ’ciaddr’ MUST be zero, ’requested IP address’ MUST be filled in with the yiaddr value from the chosen DHCPOFFER.
//  
//      servers may not receive a specific DHCPREQUEST from which they can
//      decide whether or not the client has accepted the offer.
//      servers SHOULD NOT reuse offered addresses and may use an implementation-specific
//      timeout mechanism to decide when to reuse an offered address.
//  
// RFC 2131 4.3.2 pg 22/46:
// If a server receives a DHCPREQUEST message with an invalid ’requested
// IP address’, the server SHOULD respond to the client with a DHCPNAK
// message and may choose to report the problem to the system administrator. 
//  
static TCPIP_DHCPS_LEASE_STATE _DHCPReplyToRequest_Offerred(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe)
{
    TCPIP_DHCPS_EVENT_DATA_INFO evInfo;
    evInfo.flags.val = 0;

    DHCPS_HASH_ENTRY* he = *ppHe;
    TCPIP_DHCPS_EVENT_TYPE evType = TCPIP_DHCPS_EVENT_NONE; 

    TCPIP_DHCPS_LEASE_STATE newState = TCPIP_DHCPS_LEASE_STATE_OFFERED; 

    while(true)
    {
        if(pRxOpt->serverIdentifier.optionType != DHCP_OPTION_SERVER_IDENTIFIER)
        {   // there should be a server identifier; wait some more
            evType = TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR;
            evInfo.flags.infoFmtMask = 1;
            evInfo.data = TCPIP_DHCPS_REQ_FORMAT_ERR_SRV_ID_MISS;
            break;
        }

        // check that there is a requested IP address
        if(pRxOpt->requestAddress.optionType != DHCP_OPTION_REQUEST_IP_ADDRESS) 
        {   // missing option ?
            evType = TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR;
            evInfo.flags.infoFmtMask = 1;
            evInfo.data = TCPIP_DHCPS_REQ_FORMAT_ERR_REQUEST_IP_MISS;
            break;
        }

        // check we are selected
        TCPIP_UINT32_VAL uval;
        memcpy(uval.v, pRxOpt->serverIdentifier.id, sizeof(uval.v));
        if(uval.Val != pIDcpt->serverIPAddress.Val)
        {   // not selected; 
            // RFC 2131 pg 31/46: Servers SHOULD NOT reuse offerred addresses and use a timeout mechanism to decide when to reuse!
            // wait for the timeout to reuse this address
            evType = TCPIP_DHCPS_EVENT_OTHER_SELECT;
            evInfo.flags.infoReqIp = 1;
            evInfo.data = pRxOpt->requestAddress.reqIpAddr;
            break;
        }

        // request for us
        // sanity check: match the client requested IP address
        if(pRxOpt->requestAddress.reqIpAddr == he->ipAddress.Val)
        {   // all looks good 
            uint32_t cliReqTime;
            if(!_DHCPS_ClientReqLeaseTime(pIDcpt, pRxOpt, &cliReqTime))
            {   // client has not requested a specific lease; use the default value
                cliReqTime = pIDcpt->cliLeaseDuration;
            }
            // restart the lease
            _DHCPS_LeaseRestart(he, cliReqTime);

            he->srvMsgType = DHCP_MESSAGE_TYPE_ACK; // reply with DHCPACK
            evType = TCPIP_DHCPS_EVENT_REQUEST_OFFERRED; 
            newState = TCPIP_DHCPS_LEASE_STATE_BOUND; 
        }
        else
        {
            he->srvMsgType = DHCP_MESSAGE_TYPE_NAK; // reply with DHCPNAK
            he->nakCode = DHCP_NAK_CODE_REQ_ADDRESS_INVALID;
            newState = TCPIP_DHCPS_LEASE_STATE_OFFERED;   // retain state
            evType = TCPIP_DHCPS_EVENT_REQ_ADDRESS_ERROR;
            evInfo.flags.infoReqIp = 1;
            evInfo.data = pRxOpt->requestAddress.reqIpAddr;
        }

        _DHCPS_SendMessage(he);
        break;
    }

    if(evType != 0)
    {
        _DHCPS_NotifyClients(pIDcpt->pNetIf, evType, he, &evInfo);
    }

    return newState;
}

// RFC 2131: 4.3.3  DHCPDECLINE message - pg 33/46
// If the server receives a DHCPDECLINE message, the client has
// discovered through some other means that the suggested network
// address is already in use.  The server MUST mark the network address
// as not available and SHOULD notify the local system administrator of
// a possible configuration problem.
// Note: the client included DHCP_OPTION_MESSAGE should be included in notification
static TCPIP_DHCPS_LEASE_STATE _DHCPS_ReplyToDecline(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe)
{
    DHCPS_HASH_ENTRY* he = *ppHe;

    if(he != 0)
    { // Remove the leased address from the hash table; keep the address marked as 'busy'
        _DHCPS_NotifyClients(pIDcpt->pNetIf, TCPIP_DHCPS_EVENT_DECLINED, he, 0);
#if ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_BASIC) != 0)
        int32_t entryIx = _PoolIndexFromIpAddress(pIDcpt, he->ipAddress.Val); 
        // should be a valid IP address since it's in the database!
        _DhcpsAssert(entryIx >= 0, __func__, __LINE__);
        _DhcpsAssert(IpIndexCheckFree(pIDcpt, entryIx, false) == false, __func__, __LINE__); 
#endif  // ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_BASIC) != 0)

#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        pIDcpt->statData.declinedCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        _DHCPS_RemoveEntry(he, false);
        *ppHe = 0;
    }
    // else, no record of this lease, ignore

    return TCPIP_DHCPS_LEASE_STATE_IDLE;
}

// RFC 2131: 4.3.4 DHCPRELEASE message
// Upon receipt of a DHCPRELEASE message, the server marks the network
// address as not allocated.  The server SHOULD retain a record of the
// client’s initialization parameters for possible reuse in response to
// subsequent requests from the client.
static TCPIP_DHCPS_LEASE_STATE _DHCPS_ReplyToRelease(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe)
{

    DHCPS_HASH_ENTRY* he = *ppHe;

    if(he != 0)
    {
        if(he->state >= TCPIP_DHCPS_LEASE_STATE_BOUND)
        {
            return TCPIP_DHCPS_LEASE_STATE_RELEASED;
        }
        else
        {   // some other state
            return he->state;
        }
    }
    // else, no record of this lease, ignore

    return TCPIP_DHCPS_LEASE_STATE_IDLE;
}

static bool _DHCPS_ClientReqLeaseTime(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, uint32_t* pLeaseTime)
{
    if(pRxOpt->requestLeaseTime.optionType == DHCP_OPTION_IP_LEASE_TIME) 
    {   // client wants specific time
        TCPIP_UINT32_VAL uval;
        memcpy(uval.v, pRxOpt->requestLeaseTime.lease, sizeof(uval.v));
        if(uval.Val < pIDcpt->minLeaseDuration)
        {
            uval.Val = pIDcpt->minLeaseDuration;
        }
        else if(uval.Val > pIDcpt->maxLeaseDuration)
        {
            uval.Val = pIDcpt->maxLeaseDuration;
        }
        // save the lease time
        *pLeaseTime = uval.Val;
        return true;
    }
    return false;
}

static bool isMacAddrValid(const uint8_t *macAddr)
{
    int i;
    for(i = 0; i < 6; i++)
    {
        if(macAddr[i] != 0)
        {
            return true;
        }
    }

    return false;
}

// sends a DHCP message to the client


//  RFC 2131 Table 3:  Fields and options used by DHCP servers - pg 29/46
// Option                    DHCPACK            DHCPNAK
// ------                    -------            -------
// Requested IP address      MUST NOT           MUST NOT
// IP address lease time     MUST (DHCPREQUEST) MUST NOT
//                           MUST NOT (DHCPINFORM)
// Use ’file’/’sname’ fields MAY                MUST NOT
// DHCP message type         DHCPACK            DHCPNAK
// Parameter request list    MUST NOT           MUST NOT
// Message                   SHOULD             SHOULD 
// Client identifier         MUST NOT           MAY
// Vendor class identifier   MAY                MAY
// Server identifier         MUST               MUST
// Maximum message size      MUST NOT           MUST NOT
// All others                MAY                MUST NOT

// sets the BOOTP header to the pBuff
// srvMsgType is one of {DHCP_MESSAGE_TYPE_OFFER, DHCP_MESSAGE_TYPE_ACK, DHCP_MESSAGE_TYPE_NAK}
static uint8_t* _DHCPS_SetBootpHeader(uint8_t* pBuff, DHCPS_HASH_ENTRY* he)
{
    // update the BOOTP header fields
    memset(pBuff, 0, sizeof(BOOTP_HEADER) + _TCPIP_DHCPS_UNUSED_BYTES_FOR_TX);

    BOOTP_HEADER* pHeader = (BOOTP_HEADER*)pBuff;
    pHeader->op = _TCPIP_DHCPS_BOOT_REPLY;
    pHeader->htype = _TCPIP_DHCPS_BOOT_HW_TYPE;

    pHeader->hlen = _TCPIP_DHCPS_BOOT_LEN_OF_HW_TYPE;
    // pHeader->hops = 0;
    pHeader->xid = he->xid;
    // pHeader->secs = 0;
    if(he->srvMsgType == DHCP_MESSAGE_TYPE_ACK)
    {
        pHeader->ciaddr = he->ciaddr;
    }
    // else 0

    if(he->srvMsgType == DHCP_MESSAGE_TYPE_OFFER || (he->srvMsgType == DHCP_MESSAGE_TYPE_ACK && he->cliMsgType != DHCP_MESSAGE_TYPE_INFORM))
    {
        pHeader->yiaddr = he->ipAddress.Val;
    }
    // pHeader->siaddr = 0;
    pHeader->flags = he->cliFlags;
    if(he->srvMsgType == DHCP_MESSAGE_TYPE_NAK && he->cliMsgType == DHCP_MESSAGE_TYPE_REQUEST)
    {
        if(he->giaddr != 0)
        {   // RFC 2131 pg 32/46: special case for DHCPREQUEST
            pHeader->flags |= _TCPIP_DHCPS_BOOTP_FLAG_BROADCAST;
        }
    }
    
    pHeader->giaddr = he->giaddr;
    memcpy(pHeader->chaddr, he->chaddr, sizeof(pHeader->chaddr));

    pBuff += sizeof(BOOTP_HEADER) + _TCPIP_DHCPS_UNUSED_BYTES_FOR_TX;

    return pBuff;
}

// srvMsgType is one of {DHCP_MESSAGE_TYPE_OFFER, DHCP_MESSAGE_TYPE_ACK, DHCP_MESSAGE_TYPE_NAK}
static bool _DHCPS_SendMessage(DHCPS_HASH_ENTRY* he)
{
    // NAK messages
    // Note: less than 256 bytes in length!
    static const char* nak_messgage_tbl[] = 
    {
        "requested address is invalid",     // DHCP_NAK_CODE_REQ_ADDRESS_INVALID
    };

    UDP_SOCKET skt = gDhcpDcpt->uSkt;
    TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = he->parent;

    if(TCPIP_UDP_PutIsReady(skt) < _TCPIP_DHCPS_MIN_REPONSE_PACKET_SIZE)
    {   // socket full ?
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        pIDcpt->statData.sktNotReadyCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        return false;
    }

    uint32_t magicCookie = _TCPIP_DHCPS_MAGIC_COOKIE_HOST;
    uint8_t* wrStartPtr;
    
    union
    {
        TCPIP_DHCPS_TX_OPTIONS* pOption;
        uint8_t*  wrPtr;
    }uPtr;

    wrStartPtr = gDhcpProcPacket;
    uPtr.wrPtr = _DHCPS_SetBootpHeader(wrStartPtr, he);
    // set the magic cookie
    memcpy(uPtr.wrPtr, &magicCookie, sizeof(magicCookie));
    uPtr.wrPtr += sizeof(magicCookie);

    // set options: RFC 2131 Table 3 pg 29/46

    // Option: DHCP_OPTION_REQUEST_IP_ADDRESS: MUST NOT
    //
    // Option: DHCP_OPTION_IP_LEASE_TIME: MUST/MUST NOT 
    if(he->srvMsgType == DHCP_MESSAGE_TYPE_OFFER || (he->srvMsgType == DHCP_MESSAGE_TYPE_ACK && he->cliMsgType == DHCP_MESSAGE_TYPE_REQUEST))
    {
        uPtr.pOption->ipLeaseTimeType.optionType = DHCP_OPTION_IP_LEASE_TIME;
        uPtr.pOption->ipLeaseTimeType.optionLen = 4;
        uPtr.pOption->ipLeaseTimeType.intVal = TCPIP_Helper_htonl(he->cliLeaseTime);
        uPtr.wrPtr += sizeof(uPtr.pOption->ipLeaseTimeType);
    }

    // Option: Use ’file’/’sname’: MAY

    // Option: DHCP message type: MUST
    uPtr.pOption->messageType.optionType = DHCP_OPTION_MESSAGE_TYPE;
    uPtr.pOption->messageType.optionLen = 1;
    uPtr.pOption->messageType.byteVal = he->srvMsgType;
    uPtr.wrPtr += sizeof(uPtr.pOption->messageType);

    // Option: DHCP_OPTION_PARAM_REQUEST_LIST: MUST NOT

    // Option: DHCP_OPTION_MESSAGE: SHOULD (for DHCP_MESSAGE_TYPE_NAK)
    if(he->srvMsgType == DHCP_MESSAGE_TYPE_NAK)
    {
        _DhcpsAssert(he->nakCode != 0, __func__, __LINE__);
        _DhcpsAssert(he->nakCode - 1 < sizeof(nak_messgage_tbl) / sizeof(*nak_messgage_tbl), __func__, __LINE__);

        const char* msg = nak_messgage_tbl[he->nakCode - 1];
        uint8_t msgLen = strlen(msg);

        uPtr.pOption->multByteType.optionType = DHCP_OPTION_MESSAGE;
        uPtr.pOption->multByteType.optionLen = msgLen;
        memcpy(uPtr.pOption->multByteType.byteVal, msg, msgLen);
        uPtr.wrPtr += sizeof(uPtr.pOption->multByteType) + msgLen;
    }

    // Option: DHCP_OPTION_CLIENT_ID: MUST NOT or MAY for DHCP_MESSAGE_TYPE_NAK

    // Option: Vendor class identifier: MAY
    
    // Option: DHCP_OPTION_SERVER_IDENTIFIER: MUST; the server IP address (RFC 2131 pg 23/46)
    uPtr.pOption->serverIdType.optionType = DHCP_OPTION_SERVER_IDENTIFIER;
    uPtr.pOption->serverIdType.optionLen = sizeof(IPV4_ADDR);
    uPtr.pOption->serverIdType.intVal = pIDcpt->serverIPAddress.Val;
    uPtr.wrPtr += sizeof(uPtr.pOption->serverIdType);


    // Option: Maximum message size: MUST NOT
    //
    // other options: MAY

    // Option: Subnet Mask
    uPtr.pOption->subnetMaskType.optionType = DHCP_OPTION_SUBNET_MASK;
    uPtr.pOption->subnetMaskType.optionLen = sizeof(IPV4_ADDR);
    uPtr.pOption->subnetMaskType.intVal = pIDcpt->ipMaskAddress.Val;
    uPtr.wrPtr += sizeof(uPtr.pOption->subnetMaskType);
   
    // Option: Router/Gateway address
#if (TCPIP_DHCPS_OPTION_ROUTER_VALUES  != 0)
    if((pIDcpt->cliOptions.cliOptFlags & TCPIP_DHCPS_CLIENT_OPTION_FLAG_ROUTER) != 0) 
    {   // send the router option
        uPtr.wrPtr = _DHCPS_SetMultIntOption(uPtr.wrPtr, DHCP_OPTION_ROUTER, pIDcpt->cliOptions.cliRouter, sizeof(pIDcpt->cliOptions.cliRouter) / sizeof(*pIDcpt->cliOptions.cliRouter));
    }
#endif  // (TCPIP_DHCPS_OPTION_ROUTER_VALUES  != 0)

    // Option: DNS server address
#if (TCPIP_DHCPS_OPTION_DNS_VALUES  != 0)
    if((pIDcpt->cliOptions.cliOptFlags & TCPIP_DHCPS_CLIENT_OPTION_FLAG_DNS) != 0) 
    {   // send the DNS option
        uPtr.wrPtr = _DHCPS_SetMultIntOption(uPtr.wrPtr, DHCP_OPTION_DNS, pIDcpt->cliOptions.cliDNS, sizeof(pIDcpt->cliOptions.cliDNS) / sizeof(*pIDcpt->cliOptions.cliDNS));
    }
#endif  // (TCPIP_DHCPS_OPTION_DNS_VALUES  != 0)

    // Option: Time server address
#if (TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES  != 0)
    if((pIDcpt->cliOptions.cliOptFlags & TCPIP_DHCPS_CLIENT_OPTION_FLAG_TIME_SERVER) != 0) 
    {   // send the Time Server option
        uPtr.wrPtr = _DHCPS_SetMultIntOption(uPtr.wrPtr, DHCP_OPTION_TIME_SERVER, pIDcpt->cliOptions.cliTServer, sizeof(pIDcpt->cliOptions.cliTServer) / sizeof(*pIDcpt->cliOptions.cliTServer));
    }
#endif  // (TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES  != 0)

    // Option: Name server address
#if (TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES  != 0)
    if((pIDcpt->cliOptions.cliOptFlags & TCPIP_DHCPS_CLIENT_OPTION_FLAG_NAME_SERVER) != 0) 
    {   // send the Time Server option
        uPtr.wrPtr = _DHCPS_SetMultIntOption(uPtr.wrPtr, DHCP_OPTION_NAME_SERVER, pIDcpt->cliOptions.cliNServer, sizeof(pIDcpt->cliOptions.cliNServer) / sizeof(*pIDcpt->cliOptions.cliNServer));
    }
#endif  // (TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES  != 0)

#if (TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES  != 0)
    if((pIDcpt->cliOptions.cliOptFlags & TCPIP_DHCPS_CLIENT_OPTION_FLAG_NTP_SERVER) != 0) 
    {   // send the Time Server option
        uPtr.wrPtr = _DHCPS_SetMultIntOption(uPtr.wrPtr, DHCP_OPTION_NTP_SERVER, pIDcpt->cliOptions.cliNtpServer, sizeof(pIDcpt->cliOptions.cliNtpServer) / sizeof(*pIDcpt->cliOptions.cliNtpServer));
    }
#endif  // (TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES  != 0)

#if (_TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS == 0)
    if(he->srvMsgType == DHCP_MESSAGE_TYPE_ACK && he->cliMsgType != DHCP_MESSAGE_TYPE_INFORM)
    {
        // option DHCP_OPTION_RENEWAL_T1 
        uint32_t t1 = (he->cliLeaseTime * pIDcpt->cliOptions.t1Mult) / pIDcpt->cliOptions.t1Div;
        uPtr.pOption->t1RenewalType.optionType = DHCP_OPTION_RENEWAL_T1;
        uPtr.pOption->t1RenewalType.optionLen = sizeof(uint32_t);
        uPtr.pOption->t1RenewalType.intVal = TCPIP_Helper_htonl(t1);
        uPtr.wrPtr += sizeof(uPtr.pOption->t1RenewalType);

        // option DHCP_OPTION_REBINDING_T2 
        uint32_t t2 = (he->cliLeaseTime * pIDcpt->cliOptions.t2Mult) / pIDcpt->cliOptions.t2Div;
        uPtr.pOption->t2RebindingType.optionType = DHCP_OPTION_REBINDING_T2;
        uPtr.pOption->t2RebindingType.optionLen = sizeof(uint32_t);
        uPtr.pOption->t2RebindingType.intVal = TCPIP_Helper_htonl(t2);
        uPtr.wrPtr += sizeof(uPtr.pOption->t2RebindingType);
    }
#endif  // (_TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS == 0)
  
    // No more options, mark ending
    *uPtr.wrPtr++ = DHCP_OPTION_END;

    // Add zero padding to ensure compatibility with old BOOTP relays that discard small packets (<300 UDP octets)
    size_t paddingSz = _TCPIP_DHCPS_MIN_REPONSE_PACKET_SIZE - (uPtr.wrPtr - wrStartPtr);
    if(paddingSz > 0)
    {
        memset(uPtr.wrPtr, 0, paddingSz);
        uPtr.wrPtr += paddingSz;
    }

    // set the remote destination address:
    // unicast or broadcast
    /* RFC2131 pg 23/46 rules:
        // - If the ’giaddr’ field in a DHCP message from a client is non-zero,
            // the server sends any return messages to the ’DHCP server’ port (67) on the
            // BOOTP relay agent whose address appears in ’giaddr’.
          
        // - If the ’giaddr’ field is zero and the ’ciaddr’ field is nonzero,
            //   then the server unicasts DHCPOFFER and DHCPACK messages to the address in ’ciaddr’.
           
        // - If ’giaddr’ is zero and ’ciaddr’ is zero, and the broadcast bit is
            //   set, then the server broadcasts DHCPOFFER and DHCPACK messages to 0xffffffff.
            
        // - If the broadcast bit is not set and ’giaddr’ is zero and
            //   ’ciaddr’ is zero, then the server unicasts DHCPOFFER and DHCPACK
            //   messages to the client’s hardware address and ’yiaddr’ address.

        // - In all cases, when ’giaddr’ is zero, the server broadcasts any DHCPNAK messages to 0xffffffff.
        // 
    
      RFC2131 pg 32/46 rules for DHCPREQUEST:
        If ’giaddr’ is 0x0 in the DHCPREQUEST message, the client is on
        the same subnet as the server.  The server MUST broadcast the
        DHCPNAK message to the 0xffffffff broadcast address ...

        If ’giaddr’ is set in the DHCPREQUEST message, the client is on a
        different subnet.  The server MUST set the broadcast bit in the
        DHCPNAK, so that the relay agent will broadcast the DHCPNAK to the
        client, ...

      RFC2131 pg 25/46 rules:
        Normally, DHCP servers and BOOTP relay agents attempt to deliver
        DHCPOFFER, DHCPACK and DHCPNAK messages directly to the client using
        unicast delivery.
        The IP destination address (in the IP header) is set to the DHCP ’yiaddr’ address
        and the LINK-LAYER DESTINATION ADDRESS is set to the DHCP ’chaddr’ address!!!

        ...
        ...
      
        If the BROADCAST bit is cleared to 0, the message SHOULD be sent as an IP unicast
        to the IP address specified in the ’yiaddr’ field and the
        LINK-LAYER ADDRESS SPECIFIED IN THE ’chaddr’ field!!!
      
    */
    UDP_PORT destPort = TCPIP_DHCP_CLIENT_PORT;
    IP_MULTI_ADDRESS destAdd;
    bool useBcast = false;

    if(he->giaddr != 0)
    {   // giaddr != 0
        destPort = TCPIP_DHCP_SERVER_PORT; 
        destAdd.v4Add.Val = he->giaddr;
    }
    else if(he->srvMsgType == DHCP_MESSAGE_TYPE_NAK)
    {   // giaddr == 0, DHCPNAK
        useBcast = true;
    }
    else if(he->ciaddr != 0)
    {   // giaddr == 0, ciaddr != 0
        destAdd.v4Add.Val = he->ciaddr;
    }
    else if((he->cliFlags & _TCPIP_DHCPS_BOOTP_FLAG_BROADCAST) != 0) 
    {   // giaddr == 0, ciaddr == 0, broadcast bit is set
        useBcast = true;
        destAdd.v4Add.Val = 0xffffffff;
    }
    else
    {   // giaddr == 0, ciaddr == 0, broadcast bit not set; use yiaddr
        destAdd.v4Add.Val = he->ipAddress.Val;
    }

    TCPIP_UDP_DestinationPortSet(skt, destPort);
    if(useBcast)
    {
        TCPIP_UDP_BcastIPV4AddressSet(skt, UDP_BCAST_NETWORK_LIMITED, pIDcpt->pNetIf);
    }
    else
    {
        TCPIP_UDP_DestinationIPAddressSet(skt, IP_ADDRESS_TYPE_IPV4, &destAdd);
        // inject to ARP so we can send the UDP packet
        TCPIP_ARP_RESULT arpRes = TCPIP_ARP_EntrySet(pIDcpt->pNetIf, &destAdd.v4Add, (TCPIP_MAC_ADDR*)he->chaddr, false); 
        (void)arpRes;
        if(arpRes < 0)
        {
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
            pIDcpt->statData.arpFailCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
            TCPIP_DHCPS_EVENT_DATA_INFO evInfo;
            evInfo.flags.val = 0;
            evInfo.flags.infoTargetIp = 1;
            evInfo.data = destAdd.v4Add.Val;
            _DHCPS_NotifyClients(pIDcpt->pNetIf, TCPIP_DHCPS_EVENT_ARP_FAIL, he, &evInfo);
        }
        else
        {
            he->runFlags |= DHCPS_RUN_FLAG_ARP_INJECT;
            he->injectTimeMs = _DHCPS_MsecCount();
            he->injectAdd.Val = destAdd.v4Add.Val;
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
            pIDcpt->statData.arpInjectCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        }
    }

    IP_MULTI_ADDRESS srcAdd;
    srcAdd.v4Add = pIDcpt->pNetIf->netIPAddr;
    TCPIP_UDP_SourceIPAddressSet(skt, IP_ADDRESS_TYPE_IPV4, &srcAdd);
    TCPIP_UDP_SocketNetSet(skt, pIDcpt->pNetIf);

    // set the data to the socket
    uint16_t wrBytes = TCPIP_UDP_ArrayPut(skt, wrStartPtr, uPtr.wrPtr - wrStartPtr);
    if(wrBytes != uPtr.wrPtr - wrStartPtr)
    {
        _DhcpsAssert(false, __func__, __LINE__);   // this should NOT happen
    }

    // Transmit the packet
    TCPIP_UDP_Flush(skt);

    _DHCPS_SendMessagePrint(he, destAdd.v4Add.Val);

    return true;
}

// populates a mult_int option from a source of uint32_t
// returns the updated pointer
static uint8_t* _DHCPS_SetMultIntOption(uint8_t* wrPtr, uint8_t optionType, uint32_t* pSrc, size_t nSources)
{
    int ix;

    union
    {
        TCPIP_DHCPS_OPTION_MULT_INT_DATATYPE* pOption;
        uint8_t*    wrPtr;
    }uPtr;

    typedef union __attribute__((packed))
    {
        uint8_t     b[4];
        uint32_t    v32;
    }pkd32;

    uPtr.wrPtr = wrPtr;

    uPtr.pOption->optionType = optionType;
    uPtr.pOption->optionLen = 0;
    pkd32* pVal = (pkd32*)uPtr.pOption->intVal;
    for(ix = 0; ix < nSources; ix++, pSrc++)
    {
        if(*pSrc != 0)
        {
            pVal->v32 = *pSrc;
            pVal++;
            uPtr.pOption->optionLen += sizeof(uint32_t);
        }
    }

    return (uint8_t*)pVal;
}

// Note: this is just ICMP the handler and it needs to be short
// defer the actions for the DHCPs task
// Note: this my occur on different threa dcontext if ICMP runs in a different thread!
//      That's why protection is (may be) necessary.
static void _DHCPS_PingHandler(const TCPIP_ICMP_ECHO_REQUEST* pReqData, TCPIP_ICMP_REQUEST_HANDLE icmpHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result, const void* param)
{
    DHCPS_HASH_ENTRY* he = (DHCPS_HASH_ENTRY*)param;
    _DhcpsAssert(he->ipAddress.Val == pReqData->targetAddr.Val, __func__, __LINE__);
    _DhcpsAssert(he->hEntry.flags.busy != 0, __func__, __LINE__);

    OSAL_CRITSECT_DATA_TYPE lock = _DHCPS_Lock();
    _DhcpsAssert((he->state == TCPIP_DHCPS_LEASE_STATE_WAIT_PROBE) != 0, __func__, __LINE__);

    if(result == TCPIP_ICMP_ECHO_REQUEST_RES_OK)
    {   // some host replied to this ping; address is taken, use some other
#if ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_BASIC) != 0)
        int32_t entryIx = _PoolIndexFromIpAddress(he->parent, he->ipAddress.Val); 
        _DhcpsAssert(entryIx >= 0, __func__, __LINE__);
        _DhcpsAssert(IpIndexCheckFree(he->parent, entryIx, false) == false, __func__, __LINE__);
#endif  // ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_BASIC) != 0)
        _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_REPROBE);
    }
    else
    {   // ICMP timeout; no replies
        if(--he->probeCount == 0)
        {   // we're done
            _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_SEND_OFFER);
        }
        else
        {   // more retries to be made; 
            _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_SEND_PROBE);
        }
    }
    
    _DHCPS_Unlock(lock);
}


// send an ICMP echo request to a target address
// sets the probeStartTimeMs
static bool _DHCPS_SendProbe(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, DHCPS_HASH_ENTRY* he)
{
    TCPIP_DHCPS_EVENT_TYPE evType = TCPIP_DHCPS_EVENT_NONE; 
    int ix;
    uint8_t* pBuff;
    TCPIP_ICMP_ECHO_REQUEST echoRequest;
    uint8_t pingBuffer[_TCPIP_DHCPS_PING_BUFF_SIZE];

    // make sure ARP is flushed, so we make a new query
    TCPIP_ARP_EntryRemove(pIDcpt->pNetIf, &he->ipAddress);
    
    echoRequest.netH = pIDcpt->pNetIf;
    echoRequest.targetAddr = he->ipAddress;
    echoRequest.sequenceNumber = ++gDhcpDcpt->icmpSequenceNo;
    echoRequest.identifier = ++gDhcpDcpt->icmpIdentifier;

    echoRequest.pData = pingBuffer;
    echoRequest.dataSize = sizeof(pingBuffer);
    echoRequest.callback = _DHCPS_PingHandler;
    echoRequest.param = he;

    pBuff = pingBuffer;
    for(ix = 0; ix < sizeof(pingBuffer); ix++)
    {
        *pBuff++ = SYS_RANDOM_PseudoGet();
    }

    TCPIP_ICMP_REQUEST_HANDLE echoHandle;
    ICMP_ECHO_RESULT echoRes = TCPIP_ICMP_EchoRequest (&echoRequest, &echoHandle);

    if(echoRes >= 0)
    {
        he->probeStartTimeMs = _DHCPS_MsecCount(); 
        he->probeHandle = echoHandle;
        evType = TCPIP_DHCPS_EVENT_ECHO_PROBE_SENT;
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        he->parent->statData.icmpProbeCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
    }
    else
    {   // failed; retry at next iteration
        evType = TCPIP_DHCPS_EVENT_ECHO_PROBE_FAIL;
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        he->parent->statData.icmpFailCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 

    }

    (void)evType;
    TCPIP_DHCPS_EVENT_DATA_INFO evInfo;
    evInfo.flags.val = 0;
    evInfo.flags.infoTargetIp = 1;
    evInfo.data = echoRequest.targetAddr.Val;

    _DHCPS_NotifyClients(pIDcpt->pNetIf, evType, he, &evInfo);
    return echoRes >= 0;
}


// main status processing function
static void _DHCPS_ProcessTick(void)
{
    // status processing function table
    static const _DHCPS_STAT_FUNC   _DHCPS_StatTickFncTbl[] =
    {
        0,                          // TCPIP_DHCPS_LEASE_STATE_IDLE
        _DHCPS_TickFnc_ProbeSend,   // TCPIP_DHCPS_LEASE_STATE_SEND_PROBE
        _DHCPS_TickFnc_ProbeWait,   // TCPIP_DHCPS_LEASE_STATE_WAIT_PROBE
        _DHCPS_TickFnc_Reprobe,     // TCPIP_DHCPS_LEASE_STATE_REPROBE
        _DHCPS_TickFnc_SendOffer,   // TCPIP_DHCPS_LEASE_STATE_SEND_OFFER
        _DHCPS_TickFnc_Offered,     // TCPIP_DHCPS_LEASE_STATE_OFFERED
        _DHCPS_TickFnc_Bound,       // TCPIP_DHCPS_LEASE_STATE_BOUND
        0,                          // TCPIP_DHCPS_LEASE_STATE_RELEASED
        0,                          // TCPIP_DHCPS_LEASE_STATE_EXPIRED
        0,                          // TCPIP_DHCPS_LEASE_STATE_INFORM
    };

    DHCPS_HASH_ENTRY* he;
    int ifIx, bktIx;
    OA_HASH_DCPT    *pOH;

    _DHCPS_SecondCountSet();

    if(_DHCPS_AccessLock() == false)
    {
        _DHCPS_NotifyClients(0, TCPIP_DHCPS_EVENT_TICK_LOCK, 0, 0);
        return;
    } 

    uint32_t currMsec = _DHCPS_MsecCount();

    TCPIP_DHCPS_DCPT* pDcpt = gDhcpDcpt;

    TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = pDcpt->ifDcpt;
    // process each interface descriptor
    for(ifIx = 0; ifIx < pDcpt->ifCount; ifIx++, pIDcpt++)
    {
        if((pOH = pIDcpt->hashDcpt) == 0)
        {
            continue;
        }

        for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
        {
            he = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, bktIx);        
            if(he->hEntry.flags.busy != 0)
            {
                _DhcpsAssert(0 < he->state && he->state <= TCPIP_DHCPS_LEASE_STATE_INFORM, __func__, __LINE__); 
                _DhcpsAssert(he->parent == pIDcpt, __func__, __LINE__);
                // process run time flags
                if((he->runFlags & DHCPS_RUN_FLAG_ARP_INJECT) != 0)
                {
                    if((currMsec - he->injectTimeMs) >= _TCPIP_DHCPS_ARP_INJECT_TMO)
                    {
                        TCPIP_ARP_EntryRemove(he->parent->pNetIf, &he->injectAdd);
                        he->runFlags &= ~DHCPS_RUN_FLAG_ARP_INJECT;
                    }
                } 

                // process state
                _DhcpsDisplayStat(he);
                _DHCPS_STAT_FUNC statF = _DHCPS_StatTickFncTbl[he->state];
                if(statF)
                {
                    statF(he);
                }
            }
        }
    }
    _DHCPS_AccessUnlock();
}

// Status processing functions

// TCPIP_DHCPS_LEASE_STATE_SEND_PROBE
static void _DHCPS_TickFnc_ProbeSend(DHCPS_HASH_ENTRY* he)
{
    TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = he->parent;

    if(_DHCPS_SendProbe(pIDcpt, he))
    {   // all good
        _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_WAIT_PROBE);
        return;
    }

    // ICMP echo failed...
    if(he->icmpRetries-- != 0)
    {   // we have more retries; retry later
        _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_SEND_PROBE);
    }
    else
    {   // icmp retries exhausted...
        // discard our ARP query
        TCPIP_ARP_EntryRemove(pIDcpt->pNetIf, &he->ipAddress);
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        pIDcpt->statData.echoFailCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        TCPIP_DHCPS_EVENT_DATA_INFO evInfo;
        evInfo.flags.val = 0;
        evInfo.flags.infoIcmpCount = 1;
        evInfo.data = TCPIP_DHCPS_ICMP_ECHO_RETRIES;
        _DHCPS_NotifyClients(pIDcpt->pNetIf, TCPIP_DHCPS_EVENT_ECHO_FAIL, he, &evInfo);
        if((pIDcpt->configFlags & TCPIP_DHCPS_CONFIG_FLAG_ABORT_IF_PROBE_FAILED) == 0)
        {   // we're going proceed with the offer as it is
            _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_SEND_OFFER);
        }
        else
        {   // abort the offerring, and let it timeout
            _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_OFFERED);
        }
    }
}

// TCPIP_DHCPS_LEASE_STATE_WAIT_PROBE
static void _DHCPS_TickFnc_ProbeWait(DHCPS_HASH_ENTRY* he)
{
    uint32_t currMs = _DHCPS_MsecCount(); 
    if((currMs - he->probeStartTimeMs) >= gDhcpDcpt->probeTmoMs)
    {   // timeout expired and no reply; 
        // check if we need more probes...
        if(--he->probeCount != 0)
        {   // more probes;
            _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_SEND_PROBE);
        }
        else
        {   // probes done; we're good to go
            _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_SEND_OFFER);
        }
    }
    // else wait some more
}

// TCPIP_DHCPS_LEASE_STATE_REPROBE
// got a reply from our ping...
// reissue the ping with the new address
static void _DHCPS_TickFnc_Reprobe(DHCPS_HASH_ENTRY* he)
{
    TCPIP_DHCPS_EVENT_DATA_INFO evInfo;
    evInfo.flags.val = 0;

    TCPIP_DHCPS_EVENT_TYPE evType = TCPIP_DHCPS_EVENT_NONE; 
    bool abort = false;

    while(true)
    {
        if(he->reprobeCount-- == 0)
        {
            abort = true;
            evType = TCPIP_DHCPS_EVENT_REPROBE_FAIL;
            evInfo.flags.infoIcmpCount = 1;
            evInfo.data = (uint32_t)gDhcpDcpt->nReprobes;
            break;
        }

        // OK, make another attempt
        int reqIndex = IpIndexFindFree(he->parent, true);
        if(reqIndex < 0)
        {   // no pool entry available...abort
            evType = TCPIP_DHCPS_EVENT_POOL_EMPTY;
            abort = true;
            break;
        }

        // all good, set the new entry
        he->ipAddress.Val = _IpAddressFromPoolIndex(he->parent, reqIndex);
        _DHCPS_InitProbe(he, false);
        _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_SEND_PROBE);
        break;
    }

    if(evType != TCPIP_DHCPS_EVENT_NONE)
    {
        _DHCPS_NotifyClients(he->parent->pNetIf, evType, he, &evInfo);
    }

    if(abort)
    {   // keep IP address as busy!
        _DHCPS_RemoveEntry(he, false);
    }

}

// TCPIP_DHCPS_LEASE_STATE_SEND_OFFER
static void _DHCPS_TickFnc_SendOffer(DHCPS_HASH_ENTRY* he)
{
    he->srvMsgType = DHCP_MESSAGE_TYPE_OFFER; 
    if(_DHCPS_SendMessage(he))
    {
        _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_OFFERED);
    }
}

// TCPIP_DHCPS_LEASE_STATE_OFFERED
static void _DHCPS_TickFnc_Offered(DHCPS_HASH_ENTRY* he)
{
    TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = he->parent;
    if((pIDcpt->configFlags & TCPIP_DHCPS_CONFIG_FLAG_KEEP_UNREQ_OFFERS) == 0)
    {
        uint32_t currTime = _DHCPS_SecCount();
        if((currTime - he->leaseStartTime) >= pIDcpt->unreqOfferTmo)
        {
            // remove and mark the IP address as available
            _DHCPS_RemoveEntry(he, true);
        }
    }
}

// TCPIP_DHCPS_LEASE_STATE_BOUND
static void _DHCPS_TickFnc_Bound(DHCPS_HASH_ENTRY* he)
{
    uint32_t currTime = _DHCPS_SecCount();

    if(he->state == TCPIP_DHCPS_LEASE_STATE_BOUND)
    {
        if(currTime > he->leaseEndTime)
        {
            _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_EXPIRED); 
        }
    }
}


static int TCPIP_DHCPS_IdKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    DHCPS_HASH_ENTRY* he = (DHCPS_HASH_ENTRY*)hEntry;
    return memcmp(he->clientId, key, sizeof(((DHCPS_HASH_ENTRY*)0)->clientId));

}

static void TCPIP_DHCPS_IdKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    DHCPS_HASH_ENTRY* he = (DHCPS_HASH_ENTRY*)dstEntry;
    memcpy(he->clientId, key, sizeof(((DHCPS_HASH_ENTRY*)0)->clientId));
}

// select one of the released/expired leases
static OA_HASH_ENTRY* TCPIP_DHCPS_HashDeleteEntry(OA_HASH_DCPT* pOH)
{
    size_t      bktIx;
    DHCPS_HASH_ENTRY  *he;
    DHCPS_HASH_ENTRY*  pReleased = 0;
    DHCPS_HASH_ENTRY*  pExpired = 0;

    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        he = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, bktIx);       
        if(he->hEntry.flags.busy != 0 && (he->runFlags & DHCPS_RUN_FLAG_PERM_LEASE) == 0)
        {
            if(he->state == TCPIP_DHCPS_LEASE_STATE_RELEASED )
            {
                if(pReleased == 0 || he->leaseEndTime < pReleased->leaseEndTime)
                {
                    pReleased = he;
                }
            }
            else if(he->state == TCPIP_DHCPS_LEASE_STATE_EXPIRED)
            {
                if(pExpired == 0 || he->leaseEndTime < pExpired->leaseEndTime)
                {
                    pExpired = he;
                }
            }
        }
    }


    if(pReleased)
    {
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        pReleased->parent->statData.releasedDelCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        return &pReleased->hEntry;
    }
    else if(pExpired)
    {
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        pExpired->parent->statData.expiredDelCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        return &pExpired->hEntry;
    }

    return 0;
}


TCPIP_DHCPS_RES  TCPIP_DHCPS_LeaseEntriesGet(TCPIP_NET_HANDLE netH, uint16_t* pLeases, uint16_t* pInUse)
{
    if(!_DHCPS_AccessLock())
    {
        return TCPIP_DHCPS_RES_ACCESS_LOCKED;
    }

    TCPIP_DHCPS_RES res;

    while(true)
    {

        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
        if(pNetIf == 0 || !_DHCPS_MappedOnNet(pNetIf))
        {
            res = TCPIP_DHCPS_RES_INVALID_IF; 
            break;
        }

        int ifIx = _DHCPS_InterfaceMapIx(pNetIf); 
        _DhcpsAssert(ifIx >= 0, __func__, __LINE__);

        TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = gDhcpDcpt->ifDcpt + ifIx;
        if(pInUse)
        {
            *pInUse = pIDcpt->hashDcpt->fullSlots;
        }
        if(pLeases)
        {
            *pLeases = pIDcpt->leaseEntries;
        }

        res = TCPIP_DHCPS_RES_OK;
        break;
    }

    _DHCPS_AccessUnlock();
    return res;
}

static bool _DHCPS_LeaseEntryPopulate(DHCPS_HASH_ENTRY* he, TCPIP_DHCPS_LEASE_INFO* pLeaseInfo)
{
    if(he && (he->hEntry.flags.busy != 0))
    {   // found entry
        if(pLeaseInfo)
        {
            pLeaseInfo->hNet = he->parent->pNetIf;
            pLeaseInfo->ipAddress.Val = he->ipAddress.Val;
            pLeaseInfo->leaseState = he->state;
            memcpy(pLeaseInfo->macAdd.v, he->chaddr, sizeof(pLeaseInfo->macAdd)); 

            if(he->state == TCPIP_DHCPS_LEASE_STATE_BOUND)
            {
                uint32_t currTime = _DHCPS_SecCount();
                _DhcpsAssert(he->leaseEndTime >= currTime, __func__, __LINE__);
                pLeaseInfo->leaseTime = he->leaseEndTime - currTime;
            }
            else
            {
                pLeaseInfo->leaseTime = 0;
            }

            size_t nCpy = he->clientIdLen < pLeaseInfo->clientIdLen ? he->clientIdLen : pLeaseInfo->clientIdLen;
            memcpy(pLeaseInfo->clientId, he->clientId, nCpy);
            pLeaseInfo->clientIdLen = he->clientIdLen;
        }
        return true;
    }

    return false;
}

TCPIP_DHCPS_RES  TCPIP_DHCPS_LeaseGetInfo(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_LEASE_INFO* pLeaseInfo, uint16_t leaseIx)
{

    if(!_DHCPS_AccessLock())
    {
        return TCPIP_DHCPS_RES_ACCESS_LOCKED;
    }

    TCPIP_DHCPS_RES res;
    while(true)
    {
        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
        if(pNetIf == 0 || !_DHCPS_MappedOnNet(pNetIf))
        {
            res = TCPIP_DHCPS_RES_INVALID_IF; 
            break;
        }

        int ifIx = _DHCPS_InterfaceMapIx(pNetIf); 
        _DhcpsAssert(ifIx >= 0, __func__, __LINE__);

        TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = gDhcpDcpt->ifDcpt + ifIx;

        OA_HASH_DCPT* pOH = pIDcpt->hashDcpt;

        if(leaseIx < pOH->hEntries)
        {
            DHCPS_HASH_ENTRY* he = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, leaseIx);
            res = _DHCPS_LeaseEntryPopulate(he, pLeaseInfo) ? TCPIP_DHCPS_RES_OK: TCPIP_DHCPS_RES_UNUSED_INDEX;
        }
        else
        {
            res = TCPIP_DHCPS_RES_INVALID_INDEX; 
        }
        break;
    }

    _DHCPS_AccessUnlock();
    return res;
}

TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseGetInfoById(TCPIP_NET_HANDLE netH, const uint8_t* clientId, size_t idLen, TCPIP_DHCPS_LEASE_INFO* pLeaseInfo)
{
    uint8_t fmtClientId[_TCPIP_DHCPS_CLIENT_ID_SIZE];

    if(clientId == 0 || idLen == 0)
    {
        return TCPIP_DHCPS_RES_PARAM_ERROR;
    }

    if(!_DHCPS_AccessLock())
    {
        return TCPIP_DHCPS_RES_ACCESS_LOCKED;
    }

    TCPIP_DHCPS_RES res;

    while(true)
    {
        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
        if(pNetIf == 0 || !_DHCPS_MappedOnNet(pNetIf))
        {
            res = TCPIP_DHCPS_RES_INVALID_IF; 
            break;
        }

        int ifIx = _DHCPS_InterfaceMapIx(pNetIf); 
        _DhcpsAssert(ifIx >= 0, __func__, __LINE__);

        TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = gDhcpDcpt->ifDcpt + ifIx;

        OA_HASH_DCPT* pOH = pIDcpt->hashDcpt;

        _DHCPS_FormatHashKey(fmtClientId, clientId, idLen);
        DHCPS_HASH_ENTRY* he = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(pOH, fmtClientId);

        res = _DHCPS_LeaseEntryPopulate(he, pLeaseInfo) ? TCPIP_DHCPS_RES_OK: TCPIP_DHCPS_RES_NO_LEASE;
        break;
    }


    _DHCPS_AccessUnlock();
    return res;
}

TCPIP_DHCPS_RES TCPIP_DHCPS_Configure(const TCPIP_DHCPS_INTERFACE_CONFIG* pIfConfig, uint16_t nConfigs)
{
    size_t ix;
    TCPIP_NET_IF* pNetIf;
    int ifIx;
    TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt;
    TCPIP_DHCPS_RES res = TCPIP_DHCPS_RES_OK;

    if(pIfConfig == 0 || nConfigs == 0 || nConfigs > TCPIP_DHCPS_INTERFACE_COUNT)
    {
        return TCPIP_DHCPS_RES_IF_CONFIG_ERR;
    }

    if(!_DHCPS_AccessLock())
    {
        return TCPIP_DHCPS_RES_ACCESS_LOCKED;
    }

    // do configuration one interface at a time 
    for(ix = 0; ix < nConfigs; ix++, pIfConfig++)
    {
        res = _DHCPS_AddLeasePools(pIfConfig, 1);

        if(res < 0)
        {   // failed
            break;
        }

        // configure the interface address and mask 
        pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(pIfConfig->ifIndex); 
        if(pNetIf->Flags.bIsDHCPSrvEnabled)
        {
            ifIx = _DHCPS_InterfaceMapIx(pNetIf); 
            _DhcpsAssert(ifIx >= 0, __func__, __LINE__);
            pIDcpt = gDhcpDcpt->ifDcpt + ifIx;
            _TCPIPStackSetConfigAddress(pNetIf, &pIDcpt->serverIPAddress, &pIDcpt->ipMaskAddress, &pNetIf->DefaultGateway, false);
        }
    }

    _DHCPS_AccessUnlock();

    return res;
}


#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
TCPIP_DHCPS_RES TCPIP_DHCPS_StatisticsDataGet(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_STATISTICS_DATA* pStatData)
{
    if(!_DHCPS_AccessLock())
    {
        return TCPIP_DHCPS_RES_ACCESS_LOCKED;
    }

    TCPIP_DHCPS_RES res;

    while(true)
    {
        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
        if(pNetIf == 0 || !_DHCPS_MappedOnNet(pNetIf))
        {
            res = TCPIP_DHCPS_RES_INVALID_IF; 
            break;
        }

        int ifIx = _DHCPS_InterfaceMapIx(pNetIf); 
        _DhcpsAssert(ifIx >= 0, __func__, __LINE__);

        TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = gDhcpDcpt->ifDcpt + ifIx;
        if(pStatData != 0)
        {
            *pStatData = pIDcpt->statData; 
        }

        res = TCPIP_DHCPS_RES_OK;
        break;
    }

    _DHCPS_AccessUnlock();
    return res;
}
#else
TCPIP_DHCPS_RES TCPIP_DHCPS_StatisticsDataGet(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_STATISTICS_DATA* pStatData)
{
    return TCPIP_DHCPS_RES_NOT_AVAILABLE;
}

#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 


#if (_TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0)
TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseRemove(TCPIP_NET_HANDLE netH, uint16_t leaseIx, bool keepAddBusy)
{
    if(!_DHCPS_AccessLock())
    {
        return TCPIP_DHCPS_RES_ACCESS_LOCKED;
    }

    TCPIP_DHCPS_RES res;

    while(true)
    {
        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
        if(pNetIf == 0 || !_DHCPS_MappedOnNet(pNetIf))
        {
            res = TCPIP_DHCPS_RES_INVALID_IF; 
            break;
        }

        int ifIx = _DHCPS_InterfaceMapIx(pNetIf); 
        _DhcpsAssert(ifIx >= 0, __func__, __LINE__);

        TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = gDhcpDcpt->ifDcpt + ifIx;

        OA_HASH_DCPT* pOH = pIDcpt->hashDcpt;

        if(leaseIx >= pOH->hEntries)
        {
            res = TCPIP_DHCPS_RES_INVALID_INDEX;
            break;
        }

        DHCPS_HASH_ENTRY* he = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, leaseIx);
        if(he == 0 || (he->hEntry.flags.busy == 0))
        {
            res = TCPIP_DHCPS_RES_UNUSED_INDEX; 
            break;
        }

        // found entry
        _DhcpsAssert(he->parent == pIDcpt, __func__, __LINE__);
        _DHCPS_RemoveEntry(he, keepAddBusy == false);
        res = TCPIP_DHCPS_RES_OK;
        break;
    }

    _DHCPS_AccessUnlock();
    return res;
}

TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseRemoveById(TCPIP_NET_HANDLE netH, const uint8_t* clientId, size_t idLen, bool keepAddBusy)
{
    uint8_t fmtClientId[_TCPIP_DHCPS_CLIENT_ID_SIZE];

    if(clientId == 0 || idLen == 0)
    {
        return TCPIP_DHCPS_RES_PARAM_ERROR;
    }

    if(!_DHCPS_AccessLock())
    {
        return TCPIP_DHCPS_RES_ACCESS_LOCKED;
    }

    TCPIP_DHCPS_RES res;

    while(true)
    {
        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
        if(pNetIf == 0 || !_DHCPS_MappedOnNet(pNetIf))
        {
            res = TCPIP_DHCPS_RES_INVALID_IF; 
            break;
        }

        int ifIx = _DHCPS_InterfaceMapIx(pNetIf); 
        _DhcpsAssert(ifIx >= 0, __func__, __LINE__);

        TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = gDhcpDcpt->ifDcpt + ifIx;

        OA_HASH_DCPT* pOH = pIDcpt->hashDcpt;

        _DHCPS_FormatHashKey(fmtClientId, clientId, idLen);
        DHCPS_HASH_ENTRY* he = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(pOH, fmtClientId);

        if(he == 0)
        {
            res = TCPIP_DHCPS_RES_NO_LEASE;
            break;
        }

        _DhcpsAssert(he->parent == pIDcpt, __func__, __LINE__);
        _DHCPS_RemoveEntry(he, keepAddBusy == false);
        res = TCPIP_DHCPS_RES_OK;
        
        break;
    }


    _DHCPS_AccessUnlock();
    return res;
}

TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseRemoveAll(TCPIP_NET_HANDLE netH, bool keepPerm, bool keepAddBusy)
{
    if(!_DHCPS_AccessLock())
    {
        return TCPIP_DHCPS_RES_ACCESS_LOCKED;
    }

    TCPIP_DHCPS_RES res;

    while(true)
    {
        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
        if(pNetIf == 0 || !_DHCPS_MappedOnNet(pNetIf))
        {
            res = TCPIP_DHCPS_RES_INVALID_IF; 
            break;
        }

        int ifIx = _DHCPS_InterfaceMapIx(pNetIf); 
        _DhcpsAssert(ifIx >= 0, __func__, __LINE__);

        TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = gDhcpDcpt->ifDcpt + ifIx;

        _DHCPS_RemoveAllEntries(pIDcpt, keepPerm == false, keepAddBusy == false);

        res = TCPIP_DHCPS_RES_OK;
        break;
    }

    _DHCPS_AccessUnlock();
    return res;
}


TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseAddById(TCPIP_NET_HANDLE netH, const TCPIP_DHCPS_LEASE_SET* pSet)
{
    if(pSet == 0 || pSet->ipAddress.Val == 0 || pSet->clientIdLen == 0)
    {
        return TCPIP_DHCPS_RES_PARAM_ERROR;
    }

    if(pSet->leaseState != TCPIP_DHCPS_LEASE_STATE_BOUND && pSet->leaseState != TCPIP_DHCPS_LEASE_STATE_RELEASED && pSet->leaseState != TCPIP_DHCPS_LEASE_STATE_EXPIRED)
    {
        return TCPIP_DHCPS_RES_STATE_ERROR;
    }

    if(!_DHCPS_AccessLock())
    {
        return TCPIP_DHCPS_RES_ACCESS_LOCKED;
    }

    TCPIP_DHCPS_RES res;

    while(true)
    {
        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
        if(pNetIf == 0 || !_DHCPS_MappedOnNet(pNetIf))
        {
            res = TCPIP_DHCPS_RES_INVALID_IF; 
            break;
        }

        int ifIx = _DHCPS_InterfaceMapIx(pNetIf); 
        _DhcpsAssert(ifIx >= 0, __func__, __LINE__);

        TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = gDhcpDcpt->ifDcpt + ifIx;

        int32_t ipIndex = _PoolIndexFromIpAddress(pIDcpt, pSet->ipAddress.Val);
        if(ipIndex < 0)
        {   // not part of our leases ?
            res = TCPIP_DHCPS_RES_INVALID_ADDRESS;
            break; 
        }

        OA_HASH_DCPT* pOH = pIDcpt->hashDcpt;

        uint8_t fmtClientId[_TCPIP_DHCPS_CLIENT_ID_SIZE];
        size_t keyLen = _DHCPS_FormatHashKey(fmtClientId, pSet->clientId, pSet->clientIdLen);

        // allocate a new database entry
        DHCPS_HASH_ENTRY* he = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(pOH, fmtClientId);
        if(he == 0)
        {   
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
            pIDcpt->statData.cacheFullCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
            res = TCPIP_DHCPS_RES_DB_FULL;
            break;
        }

#if ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_STATUS) != 0)
        // set the prevState for a new entry
        if(he->hEntry.flags.newEntry != 0)
        {
            he->prevState = TCPIP_DHCPS_LEASE_STATE_IDLE; 
        }
#endif  // ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_STATUS) != 0)

        // set up the new entry
        _DHCPS_SetEntryState(he, pSet->leaseState);
        he->parent = pIDcpt;
        he->clientIdLen = keyLen;
        memcpy(he->clientId, fmtClientId, sizeof(he->clientId));
        memset(he->chaddr, 0, sizeof(he->chaddr));
        memcpy(he->chaddr, pSet->macAdd.v, sizeof(pSet->macAdd));
        he->ipAddress.Val = pSet->ipAddress.Val;
        _DHCPS_LeaseRestart(he, pSet->leaseTime); 
        if(pSet->leaseTime == 0xffffffff)
        {
            he->runFlags |= DHCPS_RUN_FLAG_PERM_LEASE;
        }

        IpIndexMarkBusy(pIDcpt, ipIndex);

        res = TCPIP_DHCPS_RES_OK;
       break; 
    }

    _DHCPS_AccessUnlock();
    return res;
}

#endif  // (_TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0)

static size_t TCPIP_DHCPS_ClientIdHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32_hash(key, sizeof(((DHCPS_HASH_ENTRY*)0)->clientId)) % (pOH->hEntries);
}

#if defined(OA_DOUBLE_HASH_PROBING)
static size_t TCPIP_DHCPS_HashProbeHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32a_hash(key, sizeof(((DHCPS_HASH_ENTRY*)0)->clientId)) % (pOH->hEntries);
}
#endif  // defined(OA_DOUBLE_HASH_PROBING)

TCPIP_DHCPS_RES TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet)
{    
    if(!_DHCPS_AccessLock())
    {
        return TCPIP_DHCPS_RES_ACCESS_LOCKED;
    }

    TCPIP_DHCPS_RES res;

    while(true)
    {
        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
        if(pNetIf == 0)
        {
            res = TCPIP_DHCPS_RES_INVALID_IF; 
            break;
        }

        if(!_DHCPS_MappedOnNet(pNetIf))
        {
            res = TCPIP_DHCPS_RES_INVALID_IF;
            break; 
        }
        

        if(pNetIf->Flags.bIsDHCPSrvEnabled == 0)
        {   // already stopped
            res = TCPIP_DHCPS_RES_OK;
            break;
        }



         bool disRes = _DHCPS_Disable(pNetIf);
         if(disRes == false)
         {
             _DhcpsAssert(false, __func__, __LINE__);   // should not happen
             res = TCPIP_DHCPS_RES_SERVICE_ERROR;
            break; 
         }

         res = TCPIP_DHCPS_RES_OK;
         break;
    }

    _DHCPS_AccessUnlock();
    return res;
}

TCPIP_DHCPS_RES TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet)
{

    if(!_DHCPS_AccessLock())
    {
        return TCPIP_DHCPS_RES_ACCESS_LOCKED;
    }

    TCPIP_DHCPS_RES res;

    while(true)
    {

        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
        if(pNetIf == 0)
        {
            res = TCPIP_DHCPS_RES_INVALID_IF; 
            break;
        }

        if(!_DHCPS_MappedOnNet(pNetIf))
        {
            res = TCPIP_DHCPS_RES_INVALID_IF;
            break; 
        }

        if(pNetIf->Flags.bIsDHCPSrvEnabled != 0)
        {   // already running
            res = TCPIP_DHCPS_RES_OK;
            break;
        }

        res = _DHCPS_Enable(pNetIf, true);

        break;
    }

    _DHCPS_AccessUnlock();
    return res;
}

static TCPIP_DHCPS_RES _DHCPS_Enable(TCPIP_NET_IF* pNetIf, bool checkStart)
{
    bool startDhcps =  checkStart ?  TCPIP_STACK_AddressServiceCanStart(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPS) : true;

    if(startDhcps)
    {
        int ifIx = _DHCPS_InterfaceMapIx(pNetIf); 

        _DhcpsAssert(ifIx >= 0, __func__, __LINE__);

        TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt = gDhcpDcpt->ifDcpt + ifIx;

        // configure the interface address, mask and gateway
        _TCPIPStackSetConfigAddress(pNetIf, &pIDcpt->serverIPAddress, &pIDcpt->ipMaskAddress, &pNetIf->DefaultGateway, false);
        pNetIf->Flags.bIsDHCPSrvEnabled = true;

        return TCPIP_DHCPS_RES_OK;
    }

    return TCPIP_DHCPS_RES_SERVICE_START_ERROR;
}

static bool _DHCPS_Disable(TCPIP_NET_IF* pNetIf)
{
    int ifIx = _DHCPS_InterfaceMapIx(pNetIf); 

    if(ifIx >= 0)
    {
        TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPS, TCPIP_STACK_ADDRESS_SERVICE_EVENT_USER_STOP);
        TCPIP_STACK_AddressServiceDefaultSet(pNetIf);

        pNetIf->Flags.bIsDHCPSrvEnabled = false;

        return true;
    }

    return false;
}


bool TCPIP_DHCPS_IsEnabled(TCPIP_NET_HANDLE hNet)
{
    bool res = false;

    OSAL_CRITSECT_DATA_TYPE lock = _DHCPS_Lock();

    if(gDhcpDcpt != 0)
    {
        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
        if(pNetIf != 0 && pNetIf->Flags.bIsDHCPSrvEnabled != 0)
        {
            res = true;
        }
    }

    _DHCPS_Unlock(lock);
    return res;
}

// marks net.00...0 and net.11...1 as taken so they cannot be allocated to a client
// marks the server IP address as taken, so it cannot be allocated to a client
static void _DHCPS_InitIpMap(TCPIP_DHCPS_INTERFACE_DCPT* pIfDcpt)
{
    // all map available
    memset(pIfDcpt->ipMap, 0xff, pIfDcpt->ipMapSize * sizeof(pIfDcpt->ipMap[0]));

    uint32_t netAdd = TCPIP_Helper_ntohl((pIfDcpt->startIPAddress.Val & pIfDcpt->ipMaskAddress.Val));
    uint32_t startAdd = TCPIP_Helper_ntohl(pIfDcpt->startIPAddress.Val);

    // mark the network address busy
    if(startAdd == netAdd)
    {
        IpIndexMarkBusy(pIfDcpt, 0);
    }

    size_t leaseMaxIx = pIfDcpt->leaseEntries;

    // mark the broadcast address busy
    uint32_t bcastAdd = netAdd | (~TCPIP_Helper_ntohl(pIfDcpt->ipMaskAddress.Val));
    size_t bcastIx = bcastAdd - startAdd;
    if(bcastIx < leaseMaxIx)
    {
        IpIndexMarkBusy(pIfDcpt, bcastIx);
    }

    // if the server address is within the pool, mark it as busy
    uint32_t serverAdd = TCPIP_Helper_ntohl(pIfDcpt->serverIPAddress.Val);
    if(serverAdd >= startAdd)
    {
        size_t serverIx = serverAdd - startAdd;
        if(serverIx < leaseMaxIx)
        {
            IpIndexMarkBusy(pIfDcpt, serverIx);
        }
    } 

    // finally mark as taken everything that's outside the lease range 
    size_t mapMaxIx = pIfDcpt->ipMapSize * 32;
    if(leaseMaxIx < mapMaxIx)
    {
        IpIndexMarkBusyBlock(pIfDcpt, leaseMaxIx);
    }

}

// allocates a new hash entry that matches the client ID
// it uses the index as the entry IP address
// returns a valid entry if succeeded, 0 if database full
// the hash entry status is set to TCPIP_DHCPS_LEASE_STATE_IDLE/invalid
static DHCPS_HASH_ENTRY* _DHCPS_AssignNewOffer(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, size_t ipIndex)
{
    DHCPS_HASH_ENTRY* he;
    // allocate a new database entry
    he = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(pIDcpt->hashDcpt, pRxOpt->clientId);
    if(he != 0)
    {   
        _DhcpsAssert(he->hEntry.flags.newEntry != 0, __func__, __LINE__);   // this should be a new entry
#if ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_STATUS) != 0)
        // set the prevState for a new entry
        // if(he->hEntry.flags.newEntry != 0)
        he->prevState = TCPIP_DHCPS_LEASE_STATE_IDLE; 
#endif  // ((_TCPIP_DHCPS_DEBUG_LEVEL & _TCPIP_DHCPS_DEBUG_MASK_STATUS) != 0)

        // set up the new entry
        _DHCPS_SetEntryState(he, TCPIP_DHCPS_LEASE_STATE_IDLE);

        he->parent = pIDcpt;
        he->clientIdLen = pRxOpt->idSize;
        memcpy(he->clientId, pRxOpt->clientId, sizeof(he->clientId));
        he->ipAddress.Val = _IpAddressFromPoolIndex(pIDcpt, ipIndex);

    }
    else
    {
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
        pIDcpt->statData.cacheFullCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
    }

    return he;
}


// checks if an IP address is valid 
// address is validated to be part of the pool of addresses
// returns the address index (>=0) if address is valid
// -1 otherwise
// it can also check if that IP address is available (when valid)
static int IpIsAddressValid(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, uint32_t requestedIpAddr, DHCPS_VALID_CHECK validCheck)
{
    if(((pIDcpt->startIPAddress.Val ^ requestedIpAddr) & pIDcpt->ipMaskAddress.Val) == 0)
    {   // ok, address belongs to the same network
        int32_t index = _PoolIndexFromIpAddress(pIDcpt, requestedIpAddr);

        if(index >= 0)
        {   // valid
            bool markTaken = (validCheck & DHCPS_VALID_MARK_TAKEN) != 0; 
            if((validCheck & DHCPS_VALID_CHECK_FREE) == 0 || IpIndexCheckFree(pIDcpt, index, markTaken))
            { 
                return index;
            }
        }
    }

    return -1;
}

//
// clears an index in the pool; i.e. marks it as busy
// Notes:
//      - no protection
//      - index should be valid!
static void IpIndexMarkBusy(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, size_t index)
{
    uint8_t* pBPool = (uint8_t*)pIDcpt->ipMap;
    uint8_t* pElem = pBPool + index / 8;
    uint8_t elemMask = 1 << (7 - index % 8);
    *pElem &= ~elemMask;
}

// clears an block of indexes in the pool; i.e. marks it as busy
// Notes:
//      - no protection
//      - stIndex should be valid!
static void IpIndexMarkBusyBlock(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, size_t stIndex)
{

    uint32_t* pStart32 = pIDcpt->ipMap +  (stIndex + 31) / 32;
    uint32_t* pEnd32 = pIDcpt->ipMap + pIDcpt->ipMapSize;

    // fill 32 bit values
    while(pStart32 < pEnd32)
    {
        *(--pEnd32) = 0;
    }

    // check for bytes at the beginning
    size_t bytesLeft = stIndex % 32;
    if(bytesLeft)
    {
        uint8_t* pBPool = (uint8_t*)pIDcpt->ipMap;

        uint8_t* stByte = pBPool + (stIndex + 7) / 8;
        uint8_t* endByte = (uint8_t*)pStart32;

        while(stByte < endByte)
        {
            *(--endByte) = 0;
        }

        // finaly check bits left at the beginning
        size_t bitsLeft = stIndex % 8;
        if(bitsLeft)
        {
            uint8_t* pElem = pBPool + stIndex / 8;
            uint8_t mask = ((1 << bitsLeft) - 1) << (8 - bitsLeft);
            *pElem &= mask;
        }
    }
}

// sets an index in the pool; i.e. marks it as free
// Notes:
//      - no protection
//      - index should be valid!
static void IpIndexMarkFree(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, size_t index)
{
    uint8_t* pBPool = (uint8_t*)pIDcpt->ipMap;
    uint8_t* pElem = pBPool + index / 8;
    uint8_t elemMask = 1 << (7 - index % 8);
    *pElem |= elemMask;
}

// check if a certain index is used or free
// returns true if free, false if taken
// index should be < poolSize
static bool IpIndexCheckFree(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, size_t index, bool markTaken)
{
    uint8_t* pBPool = (uint8_t*)pIDcpt->ipMap;

    uint8_t* pElem = pBPool + index / 8;
    uint8_t elemMask = 1 << (7 - index % 8);

    bool isAvailable = ((*pElem) & elemMask) != 0;
    if(isAvailable & markTaken)
    {
        *pElem &= ~elemMask;
    }

    return isAvailable;
}

// returns the index of a free IP address in the IP pool
// i.e. [0, poolSize - 1], where each position in the pool is a bit
// -1 if no pool entry available
static int IpIndexFindFree(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, bool markTaken)
{
    size_t ix;
    uint32_t* pPool = pIDcpt->ipMap;
    size_t   poolSize = pIDcpt->ipMapSize;

    // select starting random index
    size_t stIndex = SYS_RANDOM_PseudoGet() % (poolSize);

    uint32_t* pFound = 0;
    uint32_t* pElem = pPool + stIndex;
    uint32_t* pEnd = pPool + poolSize;
    for(ix = 0; ix < poolSize; ix++)
    {
        if(*pElem != 0)
        {   // found
            pFound = pElem;
            break;
        }
        else if(pElem == pEnd)
        {
            pElem = pPool;
        }
        else
        {
            pElem++;
        }
    }

    if(pFound)
    {
        uint32_t be_elem = TCPIP_Helper_htonl(*pFound); // work in BE
        size_t lead_z = __builtin_clz(be_elem);
        
        int index = (pFound - pPool) * 32 + lead_z;
        if(markTaken)
        {
            IpIndexMarkBusy(pIDcpt, index);
        }


        return index;

    }

    // not found
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
    pIDcpt->statData.poolEmptyCount++;
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
    return -1;
}

// Register an DHCP event handler
// Use hNet == 0 to register on all interfaces available
// Returns a valid handle if the call succeeds,
// or a null handle if the call failed.
// Function has to be called after the DHCPs is initialized
// The hParam is passed by the client and will be used by the DHCP when the notification is made.
// It is used for per-thread content or if more modules, for example, share the same handler
// and need a way to differentiate the callback.
#if (_TCPIP_DHCPS_NOTIFICATIONS_ENABLE != 0)
TCPIP_DHCPS_EVENT_HANDLE TCPIP_DHCPS_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCPS_EVENT_HANDLER handler, const void* hParam)
{
    TCPIP_DHCPS_EVENT_NODE* newEvent = 0;
    if(handler != 0)
    {
        int ix;

        TCPIP_DHCPS_EVENT_NODE dhcpNode;
        dhcpNode.handler = handler;
        dhcpNode.hParam = hParam;
        dhcpNode.hNet = hNet;


        OSAL_CRITSECT_DATA_TYPE lock = _DHCPS_Lock();
        if(gDhcpDcpt != 0)
        {
            TCPIP_DHCPS_EVENT_NODE* pEvent = gDhcpDcpt->registeredUsers;
            for(ix = 0; ix < sizeof(gDhcpDcpt->registeredUsers) / sizeof(*gDhcpDcpt->registeredUsers); ix++, pEvent++)
            {
                if(pEvent->handler == 0)
                {   // found spot
                    *pEvent = dhcpNode;
                    newEvent = pEvent;
                    break;
                }
            }
            _DHCPS_Unlock(lock);
        }
    }

    return (TCPIP_DHCPS_EVENT_HANDLE)newEvent;
}

// deregister the event handler
bool TCPIP_DHCPS_HandlerDeRegister(TCPIP_DHCPS_EVENT_HANDLE hDhcp)
{
     bool res = false;

    if(hDhcp != 0 && gDhcpDcpt != 0)
    {
        TCPIP_DHCPS_EVENT_NODE* pEvent = (TCPIP_DHCPS_EVENT_NODE*)hDhcp;
        int evIx = pEvent - gDhcpDcpt->registeredUsers;
        if(0 <= evIx && evIx < sizeof(gDhcpDcpt->registeredUsers) / sizeof(*gDhcpDcpt->registeredUsers))
        {
            OSAL_CRITSECT_DATA_TYPE lock = _DHCPS_Lock();
           if(gDhcpDcpt != 0 && pEvent == gDhcpDcpt->registeredUsers + evIx)
           {    // looks like it's hours
               if(pEvent->handler != 0)
               {
                   pEvent->handler = 0;
                   res = true;
               } 
           }
           _DHCPS_Unlock(lock);
        }
    }

    return res;
}
#endif  // (_TCPIP_DHCPS_NOTIFICATIONS_ENABLE != 0)


#else
TCPIP_DHCPS_RES TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet){return TCPIP_DHCPS_RES_SERVICE_STOP_ERROR;}
TCPIP_DHCPS_RES TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet){return TCPIP_DHCPS_RES_SERVICE_START_ERROR;}
bool TCPIP_DHCPS_IsEnabled(TCPIP_NET_HANDLE hNet){return false;}
#endif //#if defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
#endif // defined(TCPIP_STACK_USE_IPV4)

