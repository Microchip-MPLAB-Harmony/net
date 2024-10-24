/*******************************************************************************
  TCP/IP Stack Manager

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    -Handles internal RX packet pre-processing prior to dispatching
     to upper application layers.
    -Reference: AN833
*******************************************************************************/

/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MANAGER

#include "tcpip/src/tcpip_private.h"


#include "tcpip/tcpip_mac_object.h"

#include "tcpip_module_manager.h"

#if defined(TCPIP_STACK_USE_PPP_INTERFACE)
#include "driver/ppp/drv_ppp.h"
#endif  // defined(TCPIP_STACK_USE_PPP_INTERFACE)

#if defined(TCPIP_STACK_TIME_MEASUREMENT)
#include <cp0defs.h>
#endif // defined(TCPIP_STACK_TIME_MEASUREMENT)

#define TCPIP_STACK_HDR_MESSAGE   "TCP/IP Stack: "

// MAC events enabled by the stack manager
#define TCPIP_STACK_MAC_ALL_EVENTS          (TCPIP_MAC_EV_RX_DONE | TCPIP_MAC_EV_TX_DONE | TCPIP_MAC_EV_RXTX_ERRORS)

// MAC events used by the stack manager to detect that
// there are active RX events that need processing
// always subset of TCPIP_STACK_MAC_ALL_EVENTS
#define TCPIP_STACK_MAC_ACTIVE_RX_EVENTS    (TCPIP_MAC_EV_RX_DONE | TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA)


// constant data
//

// table with TCPIP interfaces alias names per MAC type
static const char* TCPIP_STACK_IF_ALIAS_NAME_TBL[TCPIP_MAC_TYPES] = 
{
    TCPIP_STACK_IF_NAME_ALIAS_UNK,      // TCPIP_MAC_TYPE_NONE
    TCPIP_STACK_IF_NAME_ALIAS_ETH,      // TCPIP_MAC_TYPE_ETH
    TCPIP_STACK_IF_NAME_ALIAS_WLAN,     // TCPIP_MAC_TYPE_WLAN
    TCPIP_STACK_IF_NAME_ALIAS_PPP,      // TCPIP_MAC_TYPE_PPP
    TCPIP_STACK_IF_NAME_ALIAS_G3ADP,    // TCPIP_MAC_TYPE_G3ADP

};


// notification modules
// Since the modules that need connection notification is
// known to the stack manager no dynamic approach is taken.
// But simply a call table is maintained.
#if (_TCPIP_STACK_RUN_TIME_INIT == 0)
static const tcpipModuleConnHandler  TCPIP_STACK_CONN_EVENT_TBL [] =
{
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)
    TCPIP_DHCP_ConnectionHandler,
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)

#if defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
    TCPIP_DHCPV6_ConnectionHandler,
#endif  // defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)

    // add other needed handlers here
};
#else
static void _TCPIP_DHCP_RunConnectionHandler(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT connEvent)
{
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)
    if(_TCPIPStack_ModuleIsRunning(TCPIP_MODULE_DHCP_CLIENT))
    {
        TCPIP_DHCP_ConnectionHandler(pNetIf, connEvent);
    }   
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)
}

static void _TCPIP_DHCPV6_RunConnectionHandler(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT connEvent)
{
#if defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
    if(_TCPIPStack_ModuleIsRunning(TCPIP_MODULE_DHCPV6_CLIENT))
    {
        TCPIP_DHCPV6_ConnectionHandler(pNetIf, connEvent);
    }   
#endif  // defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
}

static const tcpipModuleConnHandler  TCPIP_STACK_CONN_EVENT_TBL [] =
{
    _TCPIP_DHCP_RunConnectionHandler,
    _TCPIP_DHCPV6_RunConnectionHandler,

    // add other needed handlers here
};
#endif  // (_TCPIP_STACK_RUN_TIME_INIT == 0)


// variables

static TCPIP_NET_IF* tcpipNetIf = 0;       // dynamically allocated

// Main default interfaces
typedef struct
{
    TCPIP_NET_IF* defaultNet;     // default network interface
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)
    TCPIP_NET_IF* defaultMcastNet; // default multicast network interface
#endif
}TCPIPDefaultIF;


static TCPIPDefaultIF tcpipDefIf = { 0 };

static volatile int    totTcpipEventsCnt = 0;
static volatile int    newTcpipTickAvlbl = 0;

static volatile int    newTcpipErrorEventCnt = 0;
static volatile int    newTcpipStackEventCnt = 0;


#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
static void    _TCPIP_MacEventCB(TCPIP_MAC_EVENT event, const void* hParam);
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

static TCPIP_STACK_MODULE_CTRL  tcpip_stack_ctrl_data = {0};

// last data structure passed for the stack initialization 
static TCPIP_STACK_INIT     tcpip_init_data = { {0} };

//
static SYS_TMR_HANDLE       tcpip_stack_tickH = SYS_TMR_HANDLE_INVALID;      // tick handle

static uint32_t             stackTaskRate;  // actual task running rate, ms
static int32_t              stackLinkTmo;   // timeout for checking the link status, ms

static uint32_t             stackAsyncSignalCount;   // global counter of the number of times the modules requested a TCPIP_MODULE_SIGNAL_ASYNC
                                                    // whenever !=0, it means that async signal requests are active!
// a quick, constant time dispatch, approach taken here
// at the expense of some extra RAM used!
// Note: TCPIP_MODULE_NONE is used as a manager entry for TMO signals!
static TCPIP_MODULE_SIGNAL_ENTRY  TCPIP_STACK_MODULE_SIGNAL_TBL [TCPIP_MODULES_NUMBER] = { {0} };

// table with RX packets queues for modules that queue up incoming packets.
// Layer 0 - the manager own RX queue
// Layer 1 - manager pushes messages to these protocols
//           these are protocols that the manager/stack handles and 
//           are part of TCPIP_FRAME_PROCESS_TBL!
// Layer 2 - modules that have incoming RX queues and layer 1 protocols 
//           use them to push RX packets.
//           Currently only TCP and UDP have this behavior.
//
static SINGLE_LIST      TCPIP_MODULES_QUEUE_TBL [TCPIP_MODULE_LAYER3] = 
{
    // 0 layer handling
    { 0 }, // TCPIP_MODULE_NONE; dummy entry; not used
    { 0 }, // TCPIP_MODULE_MANAGER; manager special entry; consistent handling of all rxQueue's

    // 1st layer handling
    { 0 }, // TCPIP_MODULE_ARP
    { 0 }, // TCPIP_MODULE_IPV4
    { 0 }, // TCPIP_MODULE_IPV6
    { 0 }, // TCPIP_MODULE_LLDP

    // 2nd layer handling
    { 0 }, // TCPIP_MODULE_ICMP
    { 0 }, // TCPIP_MODULE_ICMPV6
    { 0 }, // TCPIP_MODULE_NDP
    { 0 }, // TCPIP_MODULE_UDP
    { 0 }, // TCPIP_MODULE_TCP
    { 0 }, // TCPIP_MODULE_IGMP

    // add other types of supported frames here

};


// table keeping track of the currently running modules
#if (_TCPIP_STACK_RUN_TIME_INIT != 0)
static TCPIP_MODULE_RUN_DCPT TCPIP_MODULES_RUN_TBL[TCPIP_MODULES_NUMBER] = { {0}}; 
#endif  // (_TCPIP_STACK_RUN_TIME_INIT != 0)

#if (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
// table containing the recipients of the TCPIP_MODULE_SIGNAL_INTERFACE_CHANGE signal
// keep it short, as most of the modules do not need notifications
// just the socket ones and ARP (just to be gracious on the net) 
static const uint16_t TCPIP_STACK_MODULE_SIGNAL_CHANGE_TBL[] = 
{
    TCPIP_MODULE_ARP, TCPIP_MODULE_UDP, TCPIP_MODULE_TCP
};
// table containing the recipients of the TCPIP_MODULE_SIGNAL_INTERFACE_CONFIG signal
// keep it short, as only the socket clients need it
// not used internally
static const uint16_t TCPIP_STACK_MODULE_SIGNAL_CONFIG_TBL[] = 
{
    TCPIP_MODULE_UDP, TCPIP_MODULE_TCP
};
#endif  // (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

// table containing the layer 1 frames processed by this stack
static const TCPIP_FRAME_PROCESS_ENTRY TCPIP_FRAME_PROCESS_TBL [] = 
{
    // frameType                                 // moduleId                        // pktTypeFlags         
    // 1st layer handling                                                                 
#if defined(TCPIP_STACK_USE_IPV4)                                                         
    {.frameType = TCPIP_ETHER_TYPE_ARP,         .moduleId = TCPIP_MODULE_ARP,       .pktTypeFlags = TCPIP_MAC_PKT_FLAG_ARP},  // ARP entry
#else                                                                               
    {.frameType = TCPIP_ETHER_TYPE_UNKNOWN,     .moduleId = TCPIP_MODULE_ARP,      .pktTypeFlags = 0},                       // ARP not processed
#endif  // defined(TCPIP_STACK_USE_IPV4)                                            
                                                                                    
#if defined(TCPIP_STACK_USE_IPV4)                                                   
    {.frameType = TCPIP_ETHER_TYPE_IPV4,        .moduleId = TCPIP_MODULE_IPV4,     .pktTypeFlags = TCPIP_MAC_PKT_FLAG_IPV4}, // IPv4 entry
#else                                                                               
    {.frameType = TCPIP_ETHER_TYPE_UNKNOWN,     .moduleId = TCPIP_MODULE_IPV4,     .pktTypeFlags = 0},                       // IPv4 not processed
#endif  // defined(TCPIP_STACK_USE_IPV4)                                            
                                                                                    
#if defined(TCPIP_STACK_USE_IPV6)                                                   
    {.frameType = TCPIP_ETHER_TYPE_IPV6,        .moduleId = TCPIP_MODULE_IPV6,     .pktTypeFlags = TCPIP_MAC_PKT_FLAG_IPV6}, // IPv6 entry
#else                                                                               
    {.frameType = TCPIP_ETHER_TYPE_UNKNOWN,     .moduleId = TCPIP_MODULE_IPV6,     .pktTypeFlags = 0},                       // IPv6 not processed
#endif // defined(TCPIP_STACK_USE_IPV6)                                             
                                                                                    
#if defined(TCPIP_STACK_USE_LLDP)                                                   
    {.frameType = TCPIP_ETHER_TYPE_LLDP,        .moduleId = TCPIP_MODULE_LLDP,     .pktTypeFlags = TCPIP_MAC_PKT_FLAG_LLDP}, // LLDP entry
#else                                                                               
    {.frameType = TCPIP_ETHER_TYPE_UNKNOWN,     .moduleId = TCPIP_MODULE_LLDP,     .pktTypeFlags = 0},                       // LLDP not processed
#endif  // defined(TCPIP_STACK_USE_LLDP)

    // add other types of supported frames here

};


static SYS_STATUS       tcpip_stack_status = SYS_STATUS_UNINITIALIZED;

static TCPIP_STACK_INIT_CALLBACK tcpip_stack_init_cb;    // callback to be called at init

static TCPIP_STACK_HEAP_CONFIG  tcpip_heap_config = { 0 };      // copy of the heap that the stack uses

static uint32_t _tcpip_SecCount;            // current second value
static uint32_t _tcpip_MsecCount;           // current millisecond value
static void _TCPIP_SecondCountSet(void);    // local time keeping

#if defined(TCPIP_STACK_TIME_MEASUREMENT)
static uint64_t         tcpip_stack_time = 0;
static bool             tcpip_stack_timeEnable = 0;
extern void             TCPIP_Commands_ExecTimeUpdate(void);
#endif // defined(TCPIP_STACK_TIME_MEASUREMENT)

static bool _TCPIPStackIsRunState(void);

static void _TCPIP_STACK_TickHandler(uintptr_t context, uint32_t currTick);        // stack tick handler

static void _TCPIP_ProcessTickEvent(void);
static int  _TCPIPExtractMacRxPackets(TCPIP_NET_IF* pNetIf);

static uint32_t _TCPIPProcessMacPackets(bool signal);

static void _TCPIP_ProcessMACErrorEvents(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT activeEvent);

static bool _InitNetConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, int nNets);
static bool _LoadNetworkConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, TCPIP_NET_IF* pNetIf, bool restartIf);

static const TCPIP_STACK_MODULE_CONFIG* _TCPIP_STACK_FindModuleData(TCPIP_STACK_MODULE moduleId, const TCPIP_STACK_MODULE_CONFIG* pModConfig, int nModules);

static void  TCPIP_STACK_BringNetDown(TCPIP_STACK_MODULE_CTRL* stackCtrlData, TCPIP_NET_IF* pNetIf, TCPIP_STACK_ACTION action, TCPIP_MAC_POWER_MODE powerMode);
static bool  TCPIP_STACK_BringNetUp(TCPIP_STACK_MODULE_CTRL* stackCtrlData, const TCPIP_NETWORK_CONFIG* pNetConf, const TCPIP_STACK_MODULE_CONFIG* pModConfig, int nModules);

static void _TCPIPStackSetIpAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* ipAddress, const IPV4_ADDR* mask, const IPV4_ADDR* gw, bool setDefault);

#if (TCPIP_STACK_CONFIGURATION_SAVE_RESTORE != 0)
static void* _NetConfigStringToBuffer(void** ppDstBuff, void* pSrcBuff, size_t* pDstSize, size_t* pNeedLen, size_t* pActLen);
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static TCPIP_MAC_ACTION TCPIP_STACK_StackToMacAction(TCPIP_STACK_ACTION action);

static void TCPIP_STACK_StacktoMacCtrl(TCPIP_MAC_MODULE_CTRL* pMacCtrl, TCPIP_STACK_MODULE_CTRL* stackCtrlData);

static bool TCPIP_STACK_CheckEventsPending(void);

static void _TCPIP_NetIfEvent(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT event, bool isrProtect);

static TCPIP_MODULE_SIGNAL  _TCPIPStackManagerSignalClear(TCPIP_MODULE_SIGNAL clrMask);

static void     TCPIP_STACK_KillStack(void);

static bool     _TCPIP_DoInitialize(const TCPIP_STACK_INIT * init);

static void     _TCPIP_InitCallback(TCPIP_STACK_INIT_CALLBACK cback);

#if !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)
static void _TCPIPStackExecuteModules(void);
#endif  // !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)

static void _TCPIPStackSignalTmo(void);

static bool _TCPIPStackCreateTimer(void);

static bool _TCPIPStack_AdjustTimeouts(void);

static void _TCPIP_SelectDefaultNet(TCPIP_NET_IF* pDownIf);

static void _TCPIPStackSetIfNumberName(void);

#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
static void _TCPIPCopyMacAliasIf(TCPIP_NET_IF* pAliasIf, TCPIP_NET_IF* pPriIf);
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

#if (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
static void _TCPIPStackSetConfig(TCPIP_NET_IF* pNetIf, bool config);
#else
static __inline__ void __attribute__((always_inline)) _TCPIPStackSetConfig(TCPIP_NET_IF* pNetIf, bool config)
{
    pNetIf->Flags.bInConfig = config;
}
#endif  // (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

static __inline__ void __attribute__((always_inline)) _TCPIPInsertMacRxPacket(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt)
{
    pRxPkt->pktIf = pNetIf;
    // insertion into the manager queue should always succeed - manager is always running
    _TCPIPStackModuleRxInsert(TCPIP_MODULE_MANAGER, pRxPkt, 0);
}

// protection against MAC ISR
static __inline__ uint32_t __attribute__((always_inline)) _TCPIPMacIsrSuspend(TCPIP_NET_IF* pNetIf)
{
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
    return OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
#else
    return 0;
#endif // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
}

static __inline__ void __attribute__((always_inline)) _TCPIPMacIsrResume(TCPIP_NET_IF* pNetIf, uint32_t suspLevel)
{
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, suspLevel);
#endif // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
}

static void _TCPIPSignalEntrySetNotify(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signals, uint32_t sigParam);

static  void* _TCPIPSignalEntrySet(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signal);

static __inline__ TCPIP_MODULE_SIGNAL_ENTRY* __attribute__((always_inline)) _TCPIPModuleToSignalEntry(TCPIP_STACK_MODULE modId)
{
    return TCPIP_STACK_MODULE_SIGNAL_TBL + modId;
}

static void     _TCPIPSignalEntryNotify(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signal, uint32_t sigParam);

// sets module signals and calls user notification
static __inline__ void __attribute__((always_inline)) _TCPIPModuleSignalSetNotify(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL signals)
{
    _TCPIPSignalEntrySetNotify(_TCPIPModuleToSignalEntry(modId), signals, 0);
}

static __inline__ void __attribute__((always_inline)) _TCPIP_ClearMacEvent(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT event)
{
    uint32_t suspLvl = _TCPIPMacIsrSuspend(pNetIf);

    pNetIf->activeEvents &= ~event;

    _TCPIPMacIsrResume(pNetIf, suspLvl);
}

static __inline__ void __attribute__((always_inline)) _TCPIPAsyncSignalInc(void)
{
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    stackAsyncSignalCount += 1; 
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
}

// Note the TCPIP_EVENT enum is aligned with the TCPIP_MAC_EVENT!
static __inline__ TCPIP_EVENT __attribute__((always_inline)) TCPIP_STACK_Mac2TcpipEvent(TCPIP_MAC_EVENT macEvent)
{
    return (TCPIP_EVENT)macEvent;
}

// restores the Default DNS settings
static __inline__ void __attribute__((always_inline)) _TCPIP_StackSetDefaultDns(TCPIP_NET_IF* pNetIf)
{
    pNetIf->dnsServer[0].Val = pNetIf->DefaultDNSServer[0].Val;
    pNetIf->dnsServer[1].Val = pNetIf->DefaultDNSServer[1].Val;
}

static bool    _TCPIP_StackSyncFunction(void* synchHandle, TCPIP_MAC_SYNCH_REQUEST req);

// table with stack's modules
static const TCPIP_STACK_MODULE_ENTRY TCPIP_STACK_MODULE_ENTRY_TBL [] =
#if (TCPIP_STACK_DOWN_OPERATION != 0)
{
    // ModuleID                  // InitFunc                                        // DeInitFunc                
#if defined(TCPIP_STACK_USE_IPV4)
    {.moduleId = TCPIP_MODULE_ARP,           .initFunc = (tcpipModuleInitFunc)TCPIP_ARP_Initialize,         .deInitFunc = TCPIP_ARP_Deinitialize},            // TCPIP_MODULE_ARP
    {.moduleId = TCPIP_MODULE_IPV4,          .initFunc = (tcpipModuleInitFunc)TCPIP_IPV4_Initialize,        .deInitFunc = TCPIP_IPV4_Deinitialize},           // TCPIP_MODULE_IPV4
#endif
#if defined(TCPIP_STACK_USE_IPV6)
    {.moduleId = TCPIP_MODULE_IPV6,          .initFunc = (tcpipModuleInitFunc)TCPIP_IPV6_Initialize,        .deInitFunc = TCPIP_IPV6_Deinitialize},           // TCPIP_MODULE_IPV6
    {.moduleId = TCPIP_MODULE_ICMPV6,        .initFunc = (tcpipModuleInitFunc)TCPIP_ICMPV6_Initialize,      .deInitFunc = TCPIP_ICMPV6_Deinitialize},         // TCPIP_MODULE_ICMPV6
    {.moduleId = TCPIP_MODULE_NDP,           .initFunc = (tcpipModuleInitFunc)TCPIP_NDP_Initialize,         .deInitFunc = TCPIP_NDP_Deinitialize},            // TCPIP_MODULE_NDP
#endif
#if defined(TCPIP_STACK_USE_LLDP)
    {.moduleId = TCPIP_MODULE_LLDP,          .initFunc = (tcpipModuleInitFunc)TCPIP_LLDP_Initialize,        .deInitFunc = TCPIP_LLDP_Deinitialize},           // TCPIP_MODULE_LLDP
#endif    
#if defined(TCPIP_STACK_USE_IPV4) && (defined(TCPIP_STACK_USE_ICMP_CLIENT) || defined(TCPIP_STACK_USE_ICMP_SERVER))
    {.moduleId = TCPIP_MODULE_ICMP,          .initFunc = (tcpipModuleInitFunc)TCPIP_ICMP_Initialize,        .deInitFunc = TCPIP_ICMP_Deinitialize},           // TCPIP_MODULE_ICMP
#endif
#if defined(TCPIP_STACK_USE_UDP)
    {.moduleId = TCPIP_MODULE_UDP,           .initFunc = (tcpipModuleInitFunc)TCPIP_UDP_Initialize,         .deInitFunc = TCPIP_UDP_Deinitialize},            // TCPIP_MODULE_UDP
#endif
#if defined(TCPIP_STACK_USE_TCP)
    {.moduleId = TCPIP_MODULE_TCP,           .initFunc = (tcpipModuleInitFunc)TCPIP_TCP_Initialize,         .deInitFunc = TCPIP_TCP_Deinitialize},            //  TCPIP_MODULE_TCP
#endif    
#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
    {.moduleId = TCPIP_MODULE_DHCP_CLIENT,   .initFunc = (tcpipModuleInitFunc)TCPIP_DHCP_Initialize,        .deInitFunc = TCPIP_DHCP_Deinitialize},           // TCPIP_MODULE_DHCP_CLIENT
#endif
#if defined(TCPIP_STACK_USE_DHCP_SERVER) || defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
    {.moduleId = TCPIP_MODULE_DHCP_SERVER,   .initFunc = (tcpipModuleInitFunc)TCPIP_DHCPS_Initialize,       .deInitFunc = TCPIP_DHCPS_Deinitialize},          // TCPIP_MODULE_DHCP_SERVER
#endif
#if defined(TCPIP_STACK_USE_ANNOUNCE)
    {.moduleId = TCPIP_MODULE_ANNOUNCE,      .initFunc = (tcpipModuleInitFunc)TCPIP_ANNOUNCE_Initialize,    .deInitFunc = TCPIP_ANNOUNCE_Deinitialize},       // TCPIP_MODULE_ANNOUNCE
#endif
#if defined(TCPIP_STACK_USE_NBNS)
    {.moduleId = TCPIP_MODULE_NBNS,          .initFunc = (tcpipModuleInitFunc)TCPIP_NBNS_Initialize,        .deInitFunc = TCPIP_NBNS_Deinitialize},           // TCPIP_MODULE_NBNS
#endif
#if defined(TCPIP_STACK_USE_FTP_SERVER)
    {.moduleId = TCPIP_MODULE_FTP_SERVER,    .initFunc = (tcpipModuleInitFunc)TCPIP_FTP_ServerInitialize,   .deInitFunc = TCPIP_FTP_ServerDeinitialize},      // TCPIP_MODULE_FTP_SERVER
#endif
#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    {.moduleId = TCPIP_MODULE_DYNDNS_CLIENT, .initFunc = (tcpipModuleInitFunc)TCPIP_DDNS_Initialize,        .deInitFunc = TCPIP_DDNS_Deinitialize},           // TCPIP_MODULE_DYNDNS_CLIENT
#endif
#if defined(TCPIP_STACK_USE_REBOOT_SERVER)
    {.moduleId = TCPIP_MODULE_REBOOT_SERVER, .initFunc = (tcpipModuleInitFunc)TCPIP_REBOOT_Initialize,      .deInitFunc = TCPIP_REBOOT_Deinitialize},         // TCPIP_MODULE_REBOOT_SERVER
#endif
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    {.moduleId = TCPIP_MODULE_ZCLL,          .initFunc = (tcpipModuleInitFunc)TCPIP_ZCLL_Initialize,        .deInitFunc = TCPIP_ZCLL_Deinitialize},           // TCPIP_MODULE_ZCLL
#if defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
    {.moduleId = TCPIP_MODULE_MDNS,          .initFunc = (tcpipModuleInitFunc)TCPIP_MDNS_Initialize,        .deInitFunc = TCPIP_MDNS_Deinitialize},           // TCPIP_MODULE_MDNS
#endif
#endif
#if defined(TCPIP_STACK_USE_TFTP_CLIENT)
    {.moduleId = TCPIP_MODULE_TFTP_CLIENT,   .initFunc = (tcpipModuleInitFunc)TCPIP_TFTPC_Initialize,       .deInitFunc = TCPIP_TFTPC_Deinitialize},          // TCPIP_MODULE_TFTP_CLIENT
#endif
#if defined(TCPIP_STACK_USE_IGMP)
    {.moduleId = TCPIP_MODULE_IGMP,        .initFunc = (tcpipModuleInitFunc)TCPIP_IGMP_Initialize,        .deInitFunc = TCPIP_IGMP_Deinitialize},          // TCPIP_MODULE_IGMP
#endif
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_SMTP_CLIENT)
    {.moduleId = TCPIP_MODULE_SMTP_CLIENT,   .initFunc = (tcpipModuleInitFunc)TCPIP_SMTP_ClientInitialize,  .deInitFunc = TCPIP_SMTP_ClientDeinitialize},     // TCPIP_MODULE_SMTP_CLIENT
#endif
#if defined(TCPIP_STACK_USE_SNTP_CLIENT)
    {.moduleId = TCPIP_MODULE_SNTP,          .initFunc = (tcpipModuleInitFunc)TCPIP_SNTP_Initialize,        .deInitFunc = TCPIP_SNTP_Deinitialize},           // TCPIP_MODULE_SNTP
#endif
#if defined(TCPIP_STACK_USE_DNS)
    {.moduleId = TCPIP_MODULE_DNS_CLIENT,    .initFunc = (tcpipModuleInitFunc)TCPIP_DNS_ClientInitialize,   .deInitFunc = TCPIP_DNS_ClientDeinitialize},      // TCPIP_MODULE_DNS_CLIENT
#endif
#if defined(TCPIP_STACK_USE_BERKELEY_API)
    {.moduleId = TCPIP_MODULE_BERKELEY,      .initFunc = (tcpipModuleInitFunc)BerkeleySocketInitialize,     .deInitFunc = BerkeleySocketDeinitialize},       // TCPIP_MODULE_BERKELEY
#endif
#if defined(TCPIP_STACK_USE_HTTP_SERVER)
    {.moduleId = TCPIP_MODULE_HTTP_SERVER,   .initFunc = (tcpipModuleInitFunc)TCPIP_HTTP_Initialize,        .deInitFunc = TCPIP_HTTP_Deinitialize},           // TCPIP_MODULE_HTTP_SERVER
#endif
#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)
    {.moduleId = TCPIP_MODULE_HTTP_NET_SERVER, .initFunc = (tcpipModuleInitFunc)TCPIP_HTTP_NET_Initialize,  .deInitFunc = TCPIP_HTTP_NET_Deinitialize},      // TCPIP_MODULE_HTTP_NET_SERVER
#endif
#if defined(TCPIP_STACK_USE_TELNET_SERVER)
    {.moduleId = TCPIP_MODULE_TELNET_SERVER, .initFunc = (tcpipModuleInitFunc)TCPIP_TELNET_Initialize,      .deInitFunc = TCPIP_TELNET_Deinitialize},         // TCPIP_MODULE_TELNET_SERVER
#endif
#if defined(TCPIP_STACK_USE_SNMP_SERVER)
    {.moduleId = TCPIP_MODULE_SNMP_SERVER,   .initFunc = (tcpipModuleInitFunc)TCPIP_SNMP_Initialize,        .deInitFunc = TCPIP_SNMP_Deinitialize},           // TCPIP_MODULE_SNMP_SERVER
#endif
#if defined(TCPIP_STACK_USE_DNS_SERVER)
    {.moduleId = TCPIP_MODULE_DNS_SERVER,    .initFunc = (tcpipModuleInitFunc)TCPIP_DNSS_Initialize,        .deInitFunc = TCPIP_DNSS_Deinitialize},           // TCPIP_MODULE_DNS_SERVER
#endif
#if defined(TCPIP_STACK_COMMAND_ENABLE)
    {.moduleId = TCPIP_MODULE_COMMAND,       .initFunc = (tcpipModuleInitFunc)TCPIP_Commands_Initialize,    .deInitFunc = TCPIP_Commands_Deinitialize},       // TCPIP_MODULE_COMMAND
#endif
#if defined(TCPIP_STACK_USE_IPERF)
    {.moduleId = TCPIP_MODULE_IPERF,         .initFunc = (tcpipModuleInitFunc)TCPIP_IPERF_Initialize,       .deInitFunc = TCPIP_IPERF_Deinitialize},          // TCPIP_MODULE_IPERF
#endif
#if defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
    {.moduleId = TCPIP_MODULE_DHCPV6_CLIENT, .initFunc = (tcpipModuleInitFunc)TCPIP_DHCPV6_Initialize,      .deInitFunc = TCPIP_DHCPV6_Deinitialize},          // TCPIP_MODULE_DHCPV6_CLIENT
#endif
#if defined(TCPIP_STACK_USE_SMTPC)
    {.moduleId = TCPIP_MODULE_SMTPC,        .initFunc = (tcpipModuleInitFunc)TCPIP_SMTPC_Initialize,        .deInitFunc = TCPIP_SMTPC_Deinitialize},          // TCPIP_MODULE_SMTPC
#endif
#if defined(TCPIP_STACK_USE_TFTP_SERVER)
    {.moduleId = TCPIP_MODULE_TFTP_SERVER,  .initFunc = (tcpipModuleInitFunc)TCPIP_TFTPS_Initialize,        .deInitFunc = TCPIP_TFTPS_Deinitialize},          // TCPIP_MODULE_TFTP_SERVER
#endif   
#if defined(TCPIP_STACK_USE_FTP_CLIENT)
    {.moduleId = TCPIP_MODULE_FTP_CLIENT,   .initFunc = (tcpipModuleInitFunc)TCPIP_FTPC_Initialize,        .deInitFunc = TCPIP_FTPC_Deinitialize},          // TCPIP_MODULE_FTP_CLIENT
#endif 
#if defined(TCPIP_STACK_USE_MAC_BRIDGE)
    {.moduleId = TCPIP_MODULE_MAC_BRIDGE,   .initFunc = (tcpipModuleInitFunc)TCPIP_MAC_Bridge_Initialize,  .deInitFunc = TCPIP_MAC_Bridge_Deinitialize},      // TCPIP_MODULE_MAC_BRIDGE
#endif 
#if defined(TCPIP_STACK_USE_HTTP_SERVER_V2)
    {.moduleId = TCPIP_MODULE_HTTP_SERVER_V2, .initFunc = (tcpipModuleInitFunc)TCPIP_HTTP_Server_Initialize,  .deInitFunc = TCPIP_HTTP_Server_Deinitialize},      // TCPIP_STACK_USE_HTTP_SERVER_V2
#endif  // defined(TCPIP_STACK_USE_HTTP_SERVER_V2)
    // Add other stack modules here
     
};
#else
{
    // ModuleID                  // InitFunc                                        
#if defined(TCPIP_STACK_USE_IPV4)
    {.moduleId = TCPIP_MODULE_ARP,           .initFunc = (tcpipModuleInitFunc)TCPIP_ARP_Initialize},            // TCPIP_MODULE_ARP
    {.moduleId = TCPIP_MODULE_IPV4,          .initFunc = (tcpipModuleInitFunc)TCPIP_IPV4_Initialize},           // TCPIP_MODULE_IPV4
#endif
#if defined(TCPIP_STACK_USE_IPV6)
    {.moduleId = TCPIP_MODULE_IPV6,          .initFunc = (tcpipModuleInitFunc)TCPIP_IPV6_Initialize},           // TCPIP_MODULE_IPV6
    {.moduleId = TCPIP_MODULE_ICMPV6,        .initFunc = (tcpipModuleInitFunc)TCPIP_ICMPV6_Initialize},         // TCPIP_MODULE_ICMPV6
    {.moduleId = TCPIP_MODULE_NDP,           .initFunc = (tcpipModuleInitFunc)TCPIP_NDP_Initialize},            // TCPIP_MODULE_NDP
#endif
#if defined(TCPIP_STACK_USE_LLDP)
    {.moduleId = TCPIP_MODULE_LLDP,          .initFunc = (tcpipModuleInitFunc)TCPIP_LLDP_Initialize},           // TCPIP_MODULE_LLDP
#endif    
#if defined(TCPIP_STACK_USE_IPV4) && (defined(TCPIP_STACK_USE_ICMP_CLIENT) || defined(TCPIP_STACK_USE_ICMP_SERVER))
    {.moduleId = TCPIP_MODULE_ICMP,          .initFunc = (tcpipModuleInitFunc)TCPIP_ICMP_Initialize},           // TCPIP_MODULE_ICMP
#endif
#if defined(TCPIP_STACK_USE_UDP)
    {.moduleId = TCPIP_MODULE_UDP,           .initFunc = (tcpipModuleInitFunc)TCPIP_UDP_Initialize},            // TCPIP_MODULE_UDP
#endif
#if defined(TCPIP_STACK_USE_TCP)
    {.moduleId = TCPIP_MODULE_TCP,           .initFunc = (tcpipModuleInitFunc)TCPIP_TCP_Initialize},            //  TCPIP_MODULE_TCP
#endif    
#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
    {.moduleId = TCPIP_MODULE_DHCP_CLIENT,   .initFunc = (tcpipModuleInitFunc)TCPIP_DHCP_Initialize},           // TCPIP_MODULE_DHCP_CLIENT
#endif
#if defined(TCPIP_STACK_USE_DHCP_SERVER) || defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
    {.moduleId = TCPIP_MODULE_DHCP_SERVER,   .initFunc = (tcpipModuleInitFunc)TCPIP_DHCPS_Initialize},          // TCPIP_MODULE_DHCP_SERVER
#endif
#if defined(TCPIP_STACK_USE_ANNOUNCE)
    {.moduleId = TCPIP_MODULE_ANNOUNCE,      .initFunc = (tcpipModuleInitFunc)TCPIP_ANNOUNCE_Initialize},       // TCPIP_MODULE_ANNOUNCE
#endif
#if defined(TCPIP_STACK_USE_NBNS)
    {.moduleId = TCPIP_MODULE_NBNS,          .initFunc = (tcpipModuleInitFunc)TCPIP_NBNS_Initialize},           // TCPIP_MODULE_NBNS
#endif
#if defined(TCPIP_STACK_USE_FTP_SERVER)
    {.moduleId = TCPIP_MODULE_FTP_SERVER,    .initFunc = (tcpipModuleInitFunc)TCPIP_FTP_ServerInitialize},      // TCPIP_MODULE_FTP_SERVER
#endif
#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    {.moduleId = TCPIP_MODULE_DYNDNS_CLIENT, .initFunc = (tcpipModuleInitFunc)TCPIP_DDNS_Initialize},           // TCPIP_MODULE_DYNDNS_CLIENT
#endif
#if defined(TCPIP_STACK_USE_REBOOT_SERVER)
    {.moduleId = TCPIP_MODULE_REBOOT_SERVER, .initFunc = (tcpipModuleInitFunc)TCPIP_REBOOT_Initialize},         // TCPIP_MODULE_REBOOT_SERVER
#endif
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    {.moduleId = TCPIP_MODULE_ZCLL,          .initFunc = (tcpipModuleInitFunc)TCPIP_ZCLL_Initialize},           // TCPIP_MODULE_ZCLL
#if defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
    {.moduleId = TCPIP_MODULE_MDNS,          .initFunc = (tcpipModuleInitFunc)TCPIP_MDNS_Initialize},           // TCPIP_MODULE_MDNS
#endif
#endif
#if defined(TCPIP_STACK_USE_TFTP_CLIENT)
    {.moduleId = TCPIP_MODULE_TFTP_CLIENT,   .initFunc = (tcpipModuleInitFunc)TCPIP_TFTPC_Initialize},          // TCPIP_MODULE_TFTP_CLIENT
#endif
#if defined(TCPIP_STACK_USE_IGMP)
    {.moduleId = TCPIP_MODULE_IGMP,          .initFunc = (tcpipModuleInitFunc)TCPIP_IGMP_Initialize},          // TCPIP_MODULE_IGMP
#endif
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_SMTP_CLIENT)
    {.moduleId = TCPIP_MODULE_SMTP_CLIENT,   .initFunc = (tcpipModuleInitFunc)TCPIP_SMTP_ClientInitialize},     // TCPIP_MODULE_SMTP_CLIENT
#endif
#if defined(TCPIP_STACK_USE_SNTP_CLIENT)
    {.moduleId = TCPIP_MODULE_SNTP,          .initFunc = (tcpipModuleInitFunc)TCPIP_SNTP_Initialize},           // TCPIP_MODULE_SNTP
#endif
#if defined(TCPIP_STACK_USE_DNS)
    {.moduleId = TCPIP_MODULE_DNS_CLIENT,    .initFunc = (tcpipModuleInitFunc)TCPIP_DNS_ClientInitialize},      // TCPIP_MODULE_DNS_CLIENT
#endif
#if defined(TCPIP_STACK_USE_BERKELEY_API)
    {.moduleId = TCPIP_MODULE_BERKELEY,      .initFunc = (tcpipModuleInitFunc)BerkeleySocketInitialize},        // TCPIP_MODULE_BERKELEY
#endif
#if defined(TCPIP_STACK_USE_HTTP_SERVER)
    {.moduleId = TCPIP_MODULE_HTTP_SERVER,   .initFunc = (tcpipModuleInitFunc)TCPIP_HTTP_Initialize},           // TCPIP_MODULE_HTTP_SERVER
#endif
#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)
    {.moduleId = TCPIP_MODULE_HTTP_NET_SERVER, .initFunc = (tcpipModuleInitFunc)TCPIP_HTTP_NET_Initialize},      // TCPIP_MODULE_HTTP_NET_SERVER
#endif
#if defined(TCPIP_STACK_USE_TELNET_SERVER)
    {.moduleId = TCPIP_MODULE_TELNET_SERVER, .initFunc = (tcpipModuleInitFunc)TCPIP_TELNET_Initialize},         // TCPIP_MODULE_TELNET_SERVER
#endif
#if defined(TCPIP_STACK_USE_SNMP_SERVER)
    {.moduleId = TCPIP_MODULE_SNMP_SERVER,   .initFunc = (tcpipModuleInitFunc)TCPIP_SNMP_Initialize},           // TCPIP_MODULE_SNMP_SERVER
#endif
#if defined(TCPIP_STACK_USE_DNS_SERVER)
    {.moduleId = TCPIP_MODULE_DNS_SERVER,    .initFunc = (tcpipModuleInitFunc)TCPIP_DNSS_Initialize},           // TCPIP_MODULE_DNS_SERVER
#endif
#if defined(TCPIP_STACK_COMMAND_ENABLE)
    {.moduleId = TCPIP_MODULE_COMMAND,       .initFunc = (tcpipModuleInitFunc)TCPIP_Commands_Initialize},       // TCPIP_MODULE_COMMAND
#endif
#if defined(TCPIP_STACK_USE_IPERF)
    {.moduleId = TCPIP_MODULE_IPERF,         .initFunc = (tcpipModuleInitFunc)TCPIP_IPERF_Initialize},          // TCPIP_MODULE_IPERF
#endif
#if defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
    {.moduleId = TCPIP_MODULE_DHCPV6_CLIENT, .initFunc = (tcpipModuleInitFunc)TCPIP_DHCPV6_Initialize},          // TCPIP_MODULE_DHCPV6_CLIENT
#endif
#if defined(TCPIP_STACK_USE_SMTPC)
    {.moduleId = TCPIP_MODULE_SMTPC,        .initFunc = (tcpipModuleInitFunc)TCPIP_SMTPC_Initialize},          // TCPIP_MODULE_SMTPC
#endif
#if defined(TCPIP_STACK_USE_TFTP_SERVER)
    {.moduleId = TCPIP_MODULE_TFTP_SERVER,  .initFunc = (tcpipModuleInitFunc)TCPIP_TFTPS_Initialize},          // TCPIP_MODULE_TFTP_SERVER
#endif   
#if defined(TCPIP_STACK_USE_FTP_CLIENT)
    {.moduleId = TCPIP_MODULE_FTP_CLIENT,   .initFunc = (tcpipModuleInitFunc)TCPIP_FTPC_Initialize},          // TCPIP_MODULE_FTP_CLIENT
#endif 
#if defined(TCPIP_STACK_USE_HTTP_SERVER_V2)
    {.moduleId = TCPIP_MODULE_HTTP_SERVER_V2, .initFunc = (tcpipModuleInitFunc)TCPIP_HTTP_Server_Initialize},   // TCPIP_STACK_USE_HTTP_SERVER_V2
#endif  // defined(TCPIP_STACK_USE_HTTP_SERVER_V2)
    // Add other stack modules here
     
};
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

SYS_MODULE_OBJ TCPIP_STACK_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{

    if(tcpipNetIf != 0)
    {   // already up and running
        return (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data;
    }

    if(init == 0)
    {   // no initialization data passed
        return SYS_MODULE_OBJ_INVALID;
    }

    // start stack initialization
    totTcpipEventsCnt = 0;

    newTcpipErrorEventCnt = 0;
    newTcpipStackEventCnt = 0;
    newTcpipTickAvlbl = 0;
    stackTaskRate = stackLinkTmo = 0;

    memset(&tcpip_stack_ctrl_data, 0, sizeof(tcpip_stack_ctrl_data));

    SYS_CONSOLE_MESSAGE(TCPIP_STACK_HDR_MESSAGE "Initialization Started \r\n");

    tcpip_stack_status = SYS_STATUS_BUSY;
    if((tcpip_stack_init_cb = ((TCPIP_STACK_INIT*)init)->initCback) == 0)
    {   // perform the immediate initialization
        bool init_res = _TCPIP_DoInitialize((const TCPIP_STACK_INIT*)init);
        return init_res ? (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data : SYS_MODULE_OBJ_INVALID;
    }

    // continue initialization in TCPIP_STACK_Task()
    return (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data;

}

// calls the user initialization callback
// as part of the stack initialization
static void _TCPIP_InitCallback(TCPIP_STACK_INIT_CALLBACK cback)
{
    const TCPIP_STACK_INIT* pInit;

    int cRes = (*cback)(&pInit);

    if(cRes > 0)
    {   // pending
        return;
    }

    if (cRes == 0 && pInit != 0)
    {   // we're good to go
        _TCPIP_DoInitialize(pInit);
    }
    else
    {   // some error has occurred
        tcpip_stack_status = SYS_STATUS_UNINITIALIZED;
    }
}

// performs the stack initialization
// returns true if succesful
// false otherwise
static bool _TCPIP_DoInitialize(const TCPIP_STACK_INIT * init)
{
    int                     netIx, ix;
    int                     initFail;
    TCPIP_STACK_HEAP_HANDLE heapH;
    TCPIP_NET_IF           *pIf, *pScanIf;
    TCPIP_MAC_POWER_MODE    powerMode;
    int                     nNets, nModules;
    const TCPIP_NETWORK_CONFIG* pUsrConfig;
    const TCPIP_STACK_MODULE_CONFIG* pModConfig;
    const TCPIP_STACK_HEAP_CONFIG* heapData;
    const TCPIP_STACK_MODULE_CONFIG* pHeapConfig;
    const TCPIP_MAC_OBJECT*  pPriMac; 
    IPV4_ADDR               dupIpAddr;


    pUsrConfig = init->pNetConf;
    nNets = init->nNets;
    pModConfig = init->pModConfig;
    nModules = init->nModules;

    // minimum sanity check
    if(nNets == 0 || pUsrConfig == 0 || pUsrConfig->pMacObject == 0 || pModConfig == 0 || nModules == 0)
    {   // cannot run with no interface/init data
        return false;
    }

    // snapshot of the initialization data
    tcpip_init_data = *init;
    
    while(true)
    {
        initFail = 0;


        // find the heap settings
        pHeapConfig = _TCPIP_STACK_FindModuleData(TCPIP_MODULE_MANAGER, pModConfig, nModules);
        heapData = (pHeapConfig != 0) ? (const TCPIP_STACK_HEAP_CONFIG*)pHeapConfig->configData : 0;
        // create the heap and get a handle to the heap memory
        heapH = TCPIP_HEAP_Create(heapData, 0);

        if((tcpip_stack_ctrl_data.memH = heapH) == 0)
        {   // cannot instantiate a heap
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Heap creation failed, type: %d\r\n", heapData ? heapData->heapType : TCPIP_STACK_HEAP_TYPE_NONE);
            initFail = 1;
            break;
        }
        tcpip_stack_ctrl_data.heapType = heapData->heapType;

        tcpipNetIf = (TCPIP_NET_IF*)TCPIP_HEAP_Calloc(heapH, nNets, sizeof(TCPIP_NET_IF)); // allocate for each network interface
        if(tcpipNetIf == 0)
        {   // failed
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Network configuration allocation failed: %lu\r\n", nNets * sizeof(TCPIP_NET_IF));
            initFail = 2;
            break;
        }

        if(TCPIP_PKT_Initialize(heapH, pUsrConfig, nNets) == false)
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Packet initialization failed: 0x%x\r\n", (uint32_t)heapH);
            initFail = 3;
            break;
        }

        // initialize the run time table
        // select the running modules
#if (_TCPIP_STACK_RUN_TIME_INIT != 0)
        memset(&TCPIP_MODULES_RUN_TBL, 0, sizeof(TCPIP_MODULES_RUN_TBL));
        TCPIP_MODULES_RUN_TBL[TCPIP_MODULE_MANAGER].val = TCPIP_MODULE_RUN_FLAG_IS_RUNNING; 
        TCPIP_MODULE_RUN_DCPT* pRDcpt;
        size_t modIx;
        const TCPIP_STACK_MODULE_ENTRY* pEntry = TCPIP_STACK_MODULE_ENTRY_TBL + 0;

        for(modIx = 0; modIx < sizeof(TCPIP_STACK_MODULE_ENTRY_TBL) / sizeof(*TCPIP_STACK_MODULE_ENTRY_TBL); modIx++, pEntry++)
        {
            if(_TCPIP_STACK_FindModuleData(pEntry->moduleId, pModConfig, nModules) != 0)
            {
                pRDcpt = TCPIP_MODULES_RUN_TBL + pEntry->moduleId;
                pRDcpt->isRunning = 1;  // module should be started
            }
        }
#endif  // (_TCPIP_STACK_RUN_TIME_INIT != 0)


        tcpip_stack_ctrl_data.nIfs = nNets;
        tcpip_stack_ctrl_data.nModules = nModules;

        if(!_InitNetConfig(pUsrConfig, nNets))
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Network configuration initialization failed: %d\r\n", nNets);
            initFail = 4;   // failed the initialization
            break;
        }

        // detect the primary and alias interfaces
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        TCPIP_NET_IF           *pPriIf;

        for(netIx = 0, pIf = tcpipNetIf; netIx < nNets; netIx++, pIf++)
        {
            if(pIf->pPriIf == 0)
            {   // new scanned interface, make it primary
                pIf->pPriIf = pPriIf = pIf;
                pPriMac = pPriIf->pMacObj;
                for(ix = netIx + 1, pScanIf = pIf + 1; ix < nNets; ix++, pScanIf++)
                {
                    if(pScanIf->pMacObj == pPriMac)
                    {   // alias interface; insert it
                        pScanIf->pPriIf = pPriIf;
                        pScanIf->pAlias = pPriIf->pAlias;
                        pPriIf->pAlias = pScanIf;
                        // count it
                        tcpip_stack_ctrl_data.nAliases++;
                    }
                }
            }
        }
#else
        for(netIx = 0, pIf = tcpipNetIf; netIx < nNets; netIx++, pIf++)
        {
            pPriMac = pIf->pMacObj;
            for(ix = netIx + 1, pScanIf = pIf + 1; ix < nNets; ix++, pScanIf++)
            {
                if(pScanIf->pMacObj == pPriMac)
                {   // alias interface; count it
                    tcpip_stack_ctrl_data.nAliases++;
                }
            }
        }

        if(tcpip_stack_ctrl_data.nAliases != 0)
        {
            initFail = 5;
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Network configuration: Aliases not supported: %d\r\n", tcpip_stack_ctrl_data.nAliases);
            break;
        }

#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

        // make sure that there's no 2 interfaces with identical IP address
        dupIpAddr.Val = 0;
        for(netIx = 0, pIf = tcpipNetIf; netIx < nNets && !initFail; netIx++, pIf++)
        {
            if((dupIpAddr.Val = pIf->DefaultIPAddr.Val) != 0)
            {   // IP address 0 does not count as duplicate
                for(ix = netIx + 1, pScanIf = pIf + 1; ix < nNets; ix++, pScanIf++)
                {
                    if(pScanIf->DefaultIPAddr.Val == dupIpAddr.Val)
                    {
                        initFail = 6;
                        break;
                    }
                }
            }
        }

        if(initFail)
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Network configuration: invalid IP address: 0x%8x\r\n", dupIpAddr.Val);
            break;
        }

        // delete the old defaults
        tcpipDefIf.defaultNet = 0;
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)
        tcpipDefIf.defaultMcastNet = 0;
#endif

        _TCPIPStackSetIfNumberName();

        // initialize the signal handlers
        memset(TCPIP_STACK_MODULE_SIGNAL_TBL, 0x0, sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL));
        stackAsyncSignalCount = 0;

        // save the heap configuration
        tcpip_heap_config = *heapData; 

        // initialize the processed frames table
        for(ix = 0; ix < sizeof(TCPIP_MODULES_QUEUE_TBL)/sizeof(*TCPIP_MODULES_QUEUE_TBL); ix++)
        {
            TCPIP_Helper_SingleListInitialize(TCPIP_MODULES_QUEUE_TBL + ix);
        }

        // start per interface initializing
        tcpip_stack_ctrl_data.stackAction = TCPIP_STACK_ACTION_INIT;

        for(netIx = 0, pIf = tcpipNetIf; netIx < nNets; netIx++, pIf++, pUsrConfig++)
        {
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            if(!_TCPIPStackNetIsPrimary(pIf))
            {   // first bring up the primary, then the aliases!
                continue;
            }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

            // check the power mode
            powerMode = TCPIP_Helper_StringToPowerMode(pUsrConfig->powerMode);
            if(powerMode != TCPIP_MAC_POWER_FULL && powerMode != TCPIP_MAC_POWER_DOWN)
            {   
                SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Power Mode initialization fail: %d\r\n", powerMode);
                initFail = 7;
                break;
            }

            // set transient data
            tcpip_stack_ctrl_data.powerMode = powerMode;
            tcpip_stack_ctrl_data.pNetIf = pIf;
            tcpip_stack_ctrl_data.netIx = netIx;


            if(!TCPIP_STACK_BringNetUp(&tcpip_stack_ctrl_data, pUsrConfig, pModConfig, nModules))
            {
                initFail = 8;
                break;
            }

            // interface success
        }

        // start the aliases, if any
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        pUsrConfig = init->pNetConf;
        for(netIx = 0, pIf = tcpipNetIf; netIx < nNets && !initFail; netIx++, pIf++, pUsrConfig++)
        {
            if(_TCPIPStackNetIsPrimary(pIf))
            {   // primaries should be up
                continue;
            }

            // check the power mode
            powerMode = TCPIP_Helper_StringToPowerMode(pUsrConfig->powerMode);
            if(powerMode != TCPIP_MAC_POWER_FULL && powerMode != TCPIP_MAC_POWER_DOWN)
            {   
                SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Power Mode initialization fail: %d\r\n", powerMode);
                initFail = 9;
                break;
            }

            // set transient data
            tcpip_stack_ctrl_data.powerMode = powerMode;
            tcpip_stack_ctrl_data.pNetIf = pIf;
            tcpip_stack_ctrl_data.netIx = netIx;


            if(!TCPIP_STACK_BringNetUp(&tcpip_stack_ctrl_data, pUsrConfig, pModConfig, nModules))
            {
                initFail = 10;
                break;
            }

            // alias interface success
        }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

        break;
    }

    // initialization done
    if(!initFail)
    {
#if !defined (TCPIP_STACK_USE_EXTERNAL_HEAP)
        size_t heapLeft;
        // check the amount of heap left
        heapLeft = TCPIP_HEAP_FreeSize(heapH);
        if(heapLeft < TCPIP_STACK_DRAM_RUN_LIMIT)
        {
            SYS_ERROR_PRINT(SYS_ERROR_WARNING, TCPIP_STACK_HDR_MESSAGE "Dynamic memory is low: %lu\r\n", heapLeft);
        }
#endif  // !defined (TCPIP_STACK_USE_EXTERNAL_HEAP)
        return true;
    }


    SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Initialization failed %d - Aborting! \r\n", initFail);
    TCPIP_STACK_KillStack();
    return false;

}


/*********************************************************************
 * Function:        bool TCPIP_STACK_BringNetUp(TCPIP_NET_IF* pNetIf, const TCPIP_NETWORK_CONFIG* pNetConf, const TCPIP_STACK_MODULE_CONFIG* pModConfig, int nModules)
 *
 * PreCondition:    None
 *
 * Input:           net interface to bring up
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the initialization of a net interface
 *
 * Note:            None
 ********************************************************************/
static bool TCPIP_STACK_BringNetUp(TCPIP_STACK_MODULE_CTRL* stackCtrlData, const TCPIP_NETWORK_CONFIG* pNetConf, const TCPIP_STACK_MODULE_CONFIG* pModConfig, int nModules)
{
    TCPIP_NET_IF           *pNetIf;
    bool                    netUpFail;
    const void             *macConfig, *configData;
    const TCPIP_STACK_MODULE_CONFIG* pConfig;
    TCPIP_MAC_MODULE_CTRL   macCtrl;
    const TCPIP_MAC_OBJECT*  pMacObj;

    netUpFail = false;
    pNetIf = stackCtrlData->pNetIf;
    // restore the dynamic interface data
    pNetIf->netIfIx = stackCtrlData->netIx;
    pNetIf->Flags.powerMode = stackCtrlData->powerMode;
    pMacObj = pNetIf->pMacObj;

    while(true)
    {
        if(_TCPIPStackNetIsPrimary(pNetIf))
        {   // init primary interface
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)
            if(!TCPIP_Notification_Initialize(&pNetIf->registeredClients))
            {
                SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "%s notification initialization failed\r\n", pMacObj->macName);
                netUpFail = 1;
                break;
            }
#endif
            // start stack MAC modules initialization for primary interfaces
            // find MAC initialization data; use old if no new one
            macConfig = pNetIf->pMacConfig;
            if (pModConfig != 0)
            {
                pConfig = _TCPIP_STACK_FindModuleData(pMacObj->macId, pModConfig, nModules);
                if(pConfig != 0)
                {   // there's new MAC config data
                    pNetIf->pMacConfig = macConfig = pConfig->configData;
                }
            }

            if(stackCtrlData->powerMode == TCPIP_MAC_POWER_FULL)
            {   // init the MAC
                TCPIP_STACK_StacktoMacCtrl(&macCtrl, stackCtrlData);
                TCPIP_MAC_INIT macInit =
                {
                    { 0 }, // SYS_MODULE_INIT not currently used
                    &macCtrl,
                    macConfig,
                };

                pNetIf->macObjHandle = (*pMacObj->TCPIP_MAC_Initialize)(pMacObj->macId, &macInit.moduleInit);

                if( pNetIf->macObjHandle == SYS_MODULE_OBJ_INVALID)
                {
                    pNetIf->macObjHandle = 0;
                    SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "%s MAC initialization failed\r\n", pMacObj->macName);
                    netUpFail = 1;
                    break;
                }

                // open the MAC
                pNetIf->hIfMac = (*pMacObj->TCPIP_MAC_Open)(pMacObj->macId, DRV_IO_INTENT_READWRITE);
                if(pNetIf->hIfMac == DRV_HANDLE_INVALID)
                {
                    pNetIf->hIfMac = 0;
                    pNetIf->macObjHandle = 0;
                    SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "%s MAC Open failed\r\n", pMacObj->macName);
                    netUpFail = 1;
                    break;
                }
            }
        }

        // start stack initialization per module
        // #if (_TCPIP_STACK_RUN_TIME_INIT == 0)
        //          then all modules from the build time TCPIP_STACK_MODULE_ENTRY_TBL will be initialized
        // #if (_TCPIP_STACK_RUN_TIME_INIT != 0)
        //          then only modules that have initialization data will be started and initialized
        //          at stack start-up (pModConfig != 0) we determine the modules that need to run
        //          at interface up (pModConfig == 0) we call only the running modules

        int modIx;
#if (_TCPIP_STACK_RUN_TIME_INIT != 0)
#endif  // (_TCPIP_STACK_RUN_TIME_INIT != 0)
        const TCPIP_STACK_MODULE_ENTRY*  pEntry = TCPIP_STACK_MODULE_ENTRY_TBL + 0;

        for(modIx = 0; modIx < sizeof(TCPIP_STACK_MODULE_ENTRY_TBL) / sizeof(*TCPIP_STACK_MODULE_ENTRY_TBL); modIx++)
        {
            configData = 0;
            if (pModConfig != 0)
            {
                pConfig = _TCPIP_STACK_FindModuleData(pEntry->moduleId, pModConfig, nModules);
                if(pConfig != 0)
                {
                    configData = pConfig->configData;
                }
            }

#if (_TCPIP_STACK_RUN_TIME_INIT != 0)
            TCPIP_MODULE_RUN_DCPT* pRDcpt = TCPIP_MODULES_RUN_TBL + pEntry->moduleId;
            if(pRDcpt->isRunning)
            {
#endif  // (_TCPIP_STACK_RUN_TIME_INIT != 0)
                if(!pEntry->initFunc(stackCtrlData, configData))
                {
                    SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Module no: %d Initialization failed\r\n", pEntry->moduleId);
                    netUpFail = 1;
                    break;
                }
#if (_TCPIP_STACK_RUN_TIME_INIT != 0)
            }
#endif  // (_TCPIP_STACK_RUN_TIME_INIT != 0)
            pEntry++;
        }


        if(!netUpFail && pNetIf->hIfMac != 0)
        {
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
            if(_TCPIPStackNetIsPrimary(pNetIf))
            {
                if(!(*pNetIf->pMacObj->TCPIP_MAC_EventMaskSet)(pNetIf->hIfMac, TCPIP_STACK_MAC_ALL_EVENTS, true))
                {
                    SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "%s MAC event notification setting failed\r\n", pNetIf->pMacObj->macName);
                    netUpFail = 1;
                    break;
                }
            }
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

            // completed the MAC initialization
            pNetIf->Flags.bMacInitialize = true;
            pNetIf->Flags.bMacInitDone = false;
        }

        break;
    }

    if(netUpFail)
    {
        return false;
    }


#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    TCPIP_NET_IF* pPriIf;
    if((pPriIf = _TCPIPStackNetGetPrimary(pNetIf)) != pNetIf)
    {   // alias
        if(pPriIf->Flags.bInterfaceEnabled != 0)
        {   // primary already enabled: enable alias too
            _TCPIPCopyMacAliasIf(pNetIf, pPriIf);

            pNetIf->Flags.bInterfaceEnabled = true;
            pNetIf->Flags.bMacInitialize = false;
            pNetIf->Flags.bMacInitDone = true;
        }
    }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

    return true;

}

#if (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
bool TCPIP_STACK_NetUp(TCPIP_NET_HANDLE netH, const TCPIP_NETWORK_CONFIG* pUsrConfig)
{
    bool    success;
    TCPIP_MAC_POWER_MODE  powerMode;

    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);

    if(pNetIf)
    {
        if(pNetIf->Flags.bInterfaceEnabled || pNetIf->Flags.bMacInitialize)
        {   // already up
            return true;
        }

        if(pUsrConfig == 0)
        {   // no configuration present
            return false;
        }

        // if this is a alias interface, the primary should be up and running!
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        if(!_TCPIPStackNetIsPrimary(pNetIf))
        {
            TCPIP_NET_IF* pPriIf = _TCPIPStackNetGetPrimary(pNetIf);
            if(pPriIf->Flags.bInterfaceEnabled == 0 && pPriIf->Flags.bMacInitialize == 0)
            {
                return false;
            }
        }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

        // Before we load the default config, we should save what used to be the netIfIx
        // set transient data
        tcpip_stack_ctrl_data.pNetIf = pNetIf;
        tcpip_stack_ctrl_data.netIx = pNetIf->netIfIx;
        tcpip_stack_ctrl_data.stackAction = TCPIP_STACK_ACTION_IF_UP;

        if(!_LoadNetworkConfig(pUsrConfig, pNetIf, true))
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Network configuration load failed: %d\r\n", pNetIf->netIfIx);
            return false;
        }

        powerMode = TCPIP_Helper_StringToPowerMode(pUsrConfig->powerMode);
        if(powerMode != TCPIP_MAC_POWER_FULL && powerMode != TCPIP_MAC_POWER_DOWN)
        {   
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Power Mode initialization fail: %d\r\n", powerMode);
            return false;
        }

        tcpip_stack_ctrl_data.powerMode = powerMode;
        success = TCPIP_STACK_BringNetUp(&tcpip_stack_ctrl_data, pUsrConfig, 0, 0);
        if(!success)
        {   // don't let the MAC hanging because of a module failure
            TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
        }
        return success;
    }

    return false;

}

bool TCPIP_STACK_NetDown(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pDownIf = _TCPIPStackHandleToNet(netH);

    if(pDownIf)
    {
        if(pDownIf->Flags.bInterfaceEnabled || pDownIf->Flags.bMacInitialize)
        {
            // kill interface
            // if this is primary, kill all its aliases
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            if(_TCPIPStackNetIsPrimary(pDownIf))
            {
                TCPIP_NET_IF *pIf;
                for(pIf = _TCPIPStackNetGetAlias(pDownIf); pIf != 0; pIf = _TCPIPStackNetGetAlias(pIf)) 
                {
                    TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
                }
            }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pDownIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
            _TCPIP_SelectDefaultNet(pDownIf);
        }
        return true;
    }

    return false;

}
#endif  // (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)


#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_STACK_Deinitialize(SYS_MODULE_OBJ object)
{
    if(object != (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data || tcpipNetIf == 0)
    {   // invalid handle/already dead
        return;
    }

    TCPIP_STACK_KillStack();
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

// kills the stack
static void TCPIP_STACK_KillStack(void)
{
    int         netIx;
    TCPIP_NET_IF* pNetIf;

#if (TCPIP_STACK_DOWN_OPERATION != 0)
    if (tcpip_stack_tickH != SYS_TMR_HANDLE_INVALID)
    {
        SYS_TMR_CallbackStop(tcpip_stack_tickH);
        tcpip_stack_tickH = SYS_TMR_HANDLE_INVALID;
    }
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

    // kill interfces
    for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
    {
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        if(_TCPIPStackNetIsPrimary(pNetIf))
        {   // first kill the aliases!
            TCPIP_NET_IF* pAlias;
            for(pAlias = _TCPIPStackNetGetAlias(pNetIf); pAlias != 0; pAlias = _TCPIPStackNetGetAlias(pAlias)) 
            {
                TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pAlias, TCPIP_STACK_ACTION_DEINIT, TCPIP_MAC_POWER_DOWN);
            }
            // kill primary interface
            TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_DEINIT, TCPIP_MAC_POWER_DOWN);
        }
#else
        // kill primary interface
        TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_DEINIT, TCPIP_MAC_POWER_DOWN);
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    }


#if (TCPIP_STACK_DOWN_OPERATION != 0)
    TCPIP_PKT_Deinitialize();
    if(tcpip_stack_ctrl_data.memH != 0)
    {
        TCPIP_HEAP_Free(tcpip_stack_ctrl_data.memH, tcpipNetIf);
        if(TCPIP_HEAP_Delete(tcpip_stack_ctrl_data.memH) < 0)     // destroy the heap
        {
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)    
            int     ix, nEntries, nTraces;
            TCPIP_HEAP_TRACE_ENTRY    tEntry;

            TCPIP_STACK_HEAP_HANDLE heapH = tcpip_stack_ctrl_data.memH;
            nTraces = TCPIP_HEAP_TraceGetEntriesNo(heapH, true);
            if(nTraces)
            {
                SYS_CONSOLE_PRINT(TCPIP_STACK_HDR_MESSAGE "Heap Delete fail! Trace info: \r\n");
                nEntries = TCPIP_HEAP_TraceGetEntriesNo(heapH, false);
                for(ix = 0; ix < nEntries; ix++)
                {
                    if(TCPIP_HEAP_TraceGetEntry(heapH, ix, &tEntry))
                    {
                        if(tEntry.currAllocated != 0)
                        {
                            SYS_CONSOLE_PRINT("\tModule: %4d, nAllocs: %6d, nFrees: %6d\r\n", tEntry.moduleId, tEntry.nAllocs, tEntry.nFrees);
                            SYS_CONSOLE_PRINT("\t\ttotAllocated: %6d, currAllocated: %6d, totFailed: %6d, maxFailed: %6d\r\n", tEntry.totAllocated, tEntry.currAllocated, tEntry.totFailed, tEntry.maxFailed);
                        }
                    }
                }
            }
            else
            {
                SYS_CONSOLE_PRINT(TCPIP_STACK_HDR_MESSAGE "Heap Delete fail! No trace exists\r\n");
            }
#else
            SYS_CONSOLE_PRINT(TCPIP_STACK_HDR_MESSAGE "Heap Delete fail! No trace exists\r\n");
#endif // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)    
        }
    }
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

    tcpip_stack_ctrl_data.memH = 0;
    tcpip_stack_ctrl_data.heapType = 0;
    tcpipNetIf = 0;

    tcpip_stack_ctrl_data.nIfs = 0;
    tcpip_stack_ctrl_data.nModules = 0;
    tcpip_stack_status = SYS_STATUS_UNINITIALIZED;
    stackAsyncSignalCount = 0;
    memset(&tcpip_heap_config, 0, sizeof(tcpip_heap_config));
}

SYS_STATUS TCPIP_STACK_Status ( SYS_MODULE_OBJ object )
{
    if(object != (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data)
    {   // invalid handle
        return SYS_STATUS_ERROR;
    }

    return tcpip_stack_status;
}

bool TCPIP_STACK_InitializeDataGet(SYS_MODULE_OBJ object, TCPIP_STACK_INIT* pStackInit)
{
    if(object != (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data)
    {   // invalid handle
        return false;
    }

    if(pStackInit)
    {
        *pStackInit = tcpip_init_data;
    }

    return true;
}


// Performs the de-initialization of a net interface
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_STACK_BringNetDown(TCPIP_STACK_MODULE_CTRL* stackCtrlData, TCPIP_NET_IF* pNetIf, TCPIP_STACK_ACTION action, TCPIP_MAC_POWER_MODE powerMode)
{
    const TCPIP_STACK_MODULE_ENTRY*  pEntry;

    stackCtrlData->pNetIf = pNetIf;
    stackCtrlData->netIx = pNetIf->netIfIx;
    stackCtrlData->stackAction = action;
    stackCtrlData->powerMode = powerMode;

    // Go to the last entry in the table
    pEntry = TCPIP_STACK_MODULE_ENTRY_TBL + sizeof(TCPIP_STACK_MODULE_ENTRY_TBL)/sizeof(*TCPIP_STACK_MODULE_ENTRY_TBL);
    do
    {
        pEntry--;
        pEntry->deInitFunc(stackCtrlData);
    }
    while (pEntry != TCPIP_STACK_MODULE_ENTRY_TBL);

    _TCPIPStackModuleRxPurge(TCPIP_MODULE_MANAGER, pNetIf);
    if(_TCPIPStackNetIsPrimary(pNetIf))
    {   // primary interface
        if(pNetIf->hIfMac != 0)
        {
            (*pNetIf->pMacObj->TCPIP_MAC_Close)(pNetIf->hIfMac);
        }

        if(pNetIf->macObjHandle != 0)
        {   // kill the MAC
            (*pNetIf->pMacObj->TCPIP_MAC_Deinitialize)(pNetIf->macObjHandle);
        }
    }

    pNetIf->hIfMac = 0;
    pNetIf->macObjHandle = 0;

    pNetIf->Flags.bInterfaceEnabled = pNetIf->Flags.bMacInitialize = false;

#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)
    if(_TCPIPStackNetIsPrimary(pNetIf))
    {
        TCPIP_Notification_Deinitialize(&pNetIf->registeredClients, tcpip_stack_ctrl_data.memH);
    }
#endif
}
#else
// light de-initialization: modules are not de initialized
static void TCPIP_STACK_BringNetDown(TCPIP_STACK_MODULE_CTRL* stackCtrlData, TCPIP_NET_IF* pNetIf, TCPIP_STACK_ACTION action, TCPIP_MAC_POWER_MODE powerMode)
{

    if(_TCPIPStackNetIsPrimary(pNetIf))
    {   // primary interface
        // kill the MAC anyway
        if(pNetIf->hIfMac != 0)
        {
            (*pNetIf->pMacObj->TCPIP_MAC_Close)(pNetIf->hIfMac);
        }

        if(pNetIf->macObjHandle != 0)
        {   
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
            (*pNetIf->pMacObj->TCPIP_MAC_Deinitialize)(pNetIf->macObjHandle);
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
        }
    }

    pNetIf->hIfMac = 0;
    pNetIf->macObjHandle = 0;

    pNetIf->Flags.bInterfaceEnabled = pNetIf->Flags.bMacInitialize = false;

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)



// create the stack tick timer
static bool _TCPIPStackCreateTimer(void)
{
    bool createRes = false;

    tcpip_stack_tickH = SYS_TMR_CallbackPeriodic(TCPIP_STACK_TICK_RATE, 0, _TCPIP_STACK_TickHandler);
    if(tcpip_stack_tickH != SYS_TMR_HANDLE_INVALID)
    {
        uint32_t sysRes = SYS_TMR_TickCounterFrequencyGet();
        uint32_t rateMs = ((sysRes * TCPIP_STACK_TICK_RATE) + 999 )/1000;    // round up
        stackTaskRate = (rateMs * 1000) / sysRes;
        // SYS_TMR_CallbackPeriodicSetRate(tcpip_stack_tickH, rateMs);
        // adjust module timeouts
        createRes = _TCPIPStack_AdjustTimeouts();
        // adjust the link rate to be a multiple of the stack task rate
        stackLinkTmo = ((_TCPIP_STACK_LINK_RATE + stackTaskRate - 1) / stackTaskRate) * stackTaskRate; 
    }

    if(createRes == false)
    {
        SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Tick registration failed: %d\r\n", TCPIP_STACK_TICK_RATE);
    }

    return createRes;

}

// makes sure that the modules have a proper timeout value
// when the stackTaskRate is calculated
static bool _TCPIPStack_AdjustTimeouts(void)
{
    int     modIx;
    TCPIP_MODULE_SIGNAL_ENTRY*  pSigEntry;

    pSigEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + TCPIP_MODULE_LAYER1;
    for(modIx = TCPIP_MODULE_LAYER1; modIx < sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL)/sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL); modIx++, pSigEntry++)
    {
        if(pSigEntry->signalHandler != 0 && pSigEntry->asyncTmo != 0)
        {
            if(!_TCPIPStackSignalHandlerSetParams((TCPIP_STACK_MODULE)modIx, pSigEntry, pSigEntry->asyncTmo))
            {   // should NOT happen
                return false;
            }
        }
    }
    return true;
}

void TCPIP_STACK_Task(SYS_MODULE_OBJ object)
{
    int                 netIx, modIx;
    TCPIP_NET_IF       *pNetIf, *pAliasIf;
    TCPIP_MAC_EVENT     activeEvents;
    bool                eventPending;
    bool                wasTickEvent;
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)
    TCPIP_EVENT         tcpipEvent;
    TCPIP_EVENT_LIST_NODE* tNode;
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)   

    if(object != (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data)
    {   // invalid handle/nothing to do
        return;
    }

    if(tcpip_stack_status != SYS_STATUS_BUSY && tcpip_stack_status != SYS_STATUS_READY)
    {   // some error state
        return;
    }

    if(tcpipNetIf == 0)
    {   // call the user callback...
        _TCPIP_InitCallback(tcpip_stack_init_cb);
        return;
    }

    if(!_TCPIPStackIsRunState())
    {   // not properly initialized yet
        return;
    }

#if defined(TCPIP_STACK_TIME_MEASUREMENT)
    uint32_t    tTaskStart;
    if(tcpip_stack_timeEnable)
    {
        tTaskStart = _CP0_GET_COUNT();
    }
    TCPIP_Commands_ExecTimeUpdate();
#endif // defined(TCPIP_STACK_TIME_MEASUREMENT)

    // assign a default interface, if needed
    _TCPIP_SelectDefaultNet(0);

    // check stack signals
    eventPending = TCPIP_STACK_CheckEventsPending();
    if(eventPending == 0 && stackAsyncSignalCount == 0)
    {   // process only when events are pending or modules need async attention
        return;
    }

    if(newTcpipTickAvlbl != 0)
    {
        wasTickEvent = true;
        _TCPIP_ProcessTickEvent();
    }
    else
    {
        wasTickEvent = false;
    }

    if( totTcpipEventsCnt)
    {   // there are MAC events pending
        totTcpipEventsCnt = 0;

        for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
        {
            if (!pNetIf->Flags.bInterfaceEnabled || !_TCPIPStackNetIsPrimary(pNetIf))
            {
                continue;
            }
            activeEvents =  pNetIf->activeEvents;

#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
            // get a fresh copy
            activeEvents |= (*pNetIf->pMacObj->TCPIP_MAC_EventPendingGet)(pNetIf->hIfMac);
#else
            activeEvents |= TCPIP_MAC_EV_RX_DONE|TCPIP_MAC_EV_TX_DONE;    // just fake pending events
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

            // clear processed events
            _TCPIP_ClearMacEvent(pNetIf, activeEvents);
            pNetIf->currEvents |= activeEvents;     // store all the processed events

            // acknowledge MAC events
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
            (*pNetIf->pMacObj->TCPIP_MAC_EventAcknowledge)(pNetIf->hIfMac, activeEvents);
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
            if((activeEvents & (TCPIP_STACK_MAC_ACTIVE_RX_EVENTS)) != 0)
            {
                _TCPIPExtractMacRxPackets(pNetIf);
                newTcpipStackEventCnt++;
            }

            if(pNetIf->Flags.bMacProcessOnEvent != 0)
            {   // normal MAC internal processing
                (*pNetIf->pMacObj->TCPIP_MAC_Process)(pNetIf->hIfMac);
            }
        }

        _TCPIPProcessMacPackets(true);

        // clear the pending RX signal so it's not reported
        _TCPIPStackManagerSignalClear(TCPIP_MODULE_SIGNAL_RX_PENDING);
    }

    // process connection related and error events    
    for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
    {
        if (!pNetIf->Flags.bInterfaceEnabled || !_TCPIPStackNetIsPrimary(pNetIf))
        {
            continue;
        }

        activeEvents = pNetIf->currEvents;
        if((activeEvents & TCPIP_MAC_EV_RXTX_ERRORS) != 0)
        {    // some error has occurred
            _TCPIP_ProcessMACErrorEvents(pNetIf, activeEvents);
        }

        if(pNetIf->exFlags.connEvent != 0)
        {   // connection related event
            if(pNetIf->exFlags.connEventType != 0)
            {
                activeEvents |= TCPIP_MAC_EV_CONN_ESTABLISHED;
#if defined(TCPIP_STACK_USE_PPP_INTERFACE)
                if(_TCPIPStack_NetMacType(pNetIf) == TCPIP_MAC_TYPE_PPP)
                {   // for a PPP interface we set the IP address obtained from negotiation
                    IPV4_ADDR lclIpAddr;
                    lclIpAddr.Val = PPP_GetLocalIpv4Addr(pNetIf->hIfMac);
                    _TCPIPStackSetIpAddress(pNetIf, &lclIpAddr, 0, 0, false);
                }
#endif  // defined(TCPIP_STACK_USE_PPP_INTERFACE)
            }
            else
            {
                activeEvents |= TCPIP_MAC_EV_CONN_LOST;
            } 
            pNetIf->exFlags.connEvent = 0;

            for(pAliasIf = _TCPIPStackNetGetPrimary(pNetIf); pAliasIf != 0; pAliasIf = _TCPIPStackNetGetAlias(pAliasIf))
            {
                for(modIx = 0; modIx < sizeof(TCPIP_STACK_CONN_EVENT_TBL)/sizeof(*TCPIP_STACK_CONN_EVENT_TBL); modIx++)
                {
                    (*TCPIP_STACK_CONN_EVENT_TBL[modIx])(pAliasIf, activeEvents);
                }
            }
        }

#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)
        // notify stack clients 
        if((tcpipEvent = TCPIP_STACK_Mac2TcpipEvent(activeEvents)) != TCPIP_EV_NONE)
        {
            TCPIP_Notification_Lock(&pNetIf->registeredClients);
            for(tNode = (TCPIP_EVENT_LIST_NODE*)pNetIf->registeredClients.list.head; tNode != 0; tNode = tNode->next)
            {
                if((tNode->evMask & tcpipEvent) != 0 )
                {   // trigger event
                    (*tNode->handler)(pNetIf, tcpipEvent, tNode->hParam);
                }
            }
            TCPIP_Notification_Unlock(&pNetIf->registeredClients);
        }
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)

        pNetIf->currEvents = 0;
    }

    if(wasTickEvent)
    {   // send the timeout signals
        _TCPIPStackSignalTmo();
        // clear the TMO signal so it's not reported anymore
        _TCPIPStackManagerSignalClear(TCPIP_MODULE_SIGNAL_TMO);
    }

#if !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)
    // execute the signal tasks here, instead of letting the app handle that
    _TCPIPStackExecuteModules();
    if(stackAsyncSignalCount != 0)
    {   // when executing the tasks internally
        // signal that attention is required
        _TCPIPSignalEntryNotify(_TCPIPModuleToSignalEntry(TCPIP_MODULE_MANAGER), TCPIP_MODULE_SIGNAL_ASYNC, 0);
    }
#endif  // !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)

#if defined(TCPIP_STACK_TIME_MEASUREMENT)
    if(tcpip_stack_timeEnable)
    {
        tcpip_stack_time += _CP0_GET_COUNT() - tTaskStart;
    }
#endif // defined(TCPIP_STACK_TIME_MEASUREMENT)

}

static bool _TCPIPStackIsRunState(void)
{
    int             netIx;
    TCPIP_NET_IF   *pNetIf, *pPriIf;
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    int             aliasIx;
    TCPIP_NET_IF   *pAliasIf;
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

    // check that we've created the needed timer
    if(tcpip_stack_status == SYS_STATUS_BUSY && tcpip_stack_tickH == SYS_TMR_HANDLE_INVALID)
    {   // check that we can create a timer
        SYS_STATUS tmrStat = SYS_TMR_ModuleStatusGet(0);
        if(tmrStat == SYS_STATUS_BUSY)
        {   // not ready yet
            return false;
        }

        if(tmrStat == SYS_STATUS_READY)
        {   // try to create the timer
            if(_TCPIPStackCreateTimer() == false)
            {   // failed
                tmrStat = SYS_STATUS_ERROR;
            }
        }
        
        if(tmrStat <= 0)
        {   // something went wrong...
            TCPIP_STACK_KillStack();
            tcpip_stack_status = SYS_STATUS_ERROR;
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Initialization SYS TMR failed: %d - Aborting! \r\n", tmrStat);
            return false;
        }
    }

    for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
    {
        if(pNetIf->macObjHandle != 0 && _TCPIPStackNetIsPrimary(pNetIf))
        {
            // process the underlying MAC module tasks
            (*pNetIf->pMacObj->TCPIP_MAC_Tasks)(pNetIf->macObjHandle);

            // check if we're just starting up some interfaces
            if(pNetIf->Flags.bMacInitialize)
            {   // initializing
                SYS_STATUS macStat = (*pNetIf->pMacObj->TCPIP_MAC_Status)(pNetIf->macObjHandle);
                if(macStat < 0)
                {   // failed; kill the interface
                    TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
                    pNetIf->Flags.bMacInitDone = true;
                }
                else if(macStat == SYS_STATUS_READY)
                {   // get the MAC address and MAC processing flags
                    // set the default MTU; MAC driver will override if needed
                    TCPIP_MAC_PARAMETERS macParams = {{{0}}};
                    macParams.linkMtu = TCPIP_MAC_LINK_MTU_DEFAULT; 
                    (*pNetIf->pMacObj->TCPIP_MAC_ParametersGet)(pNetIf->hIfMac, &macParams);
                    memcpy(pNetIf->netMACAddr.v, macParams.ifPhyAddress.v, sizeof(pNetIf->netMACAddr));
                    pNetIf->Flags.bMacProcessOnEvent = macParams.processFlags != TCPIP_MAC_PROCESS_FLAG_NONE;
                    pNetIf->linkMtu = macParams.linkMtu;
                    pNetIf->txOffload = (uint8_t)macParams.checksumOffloadTx;
                    pNetIf->rxOffload = (uint8_t)macParams.checksumOffloadRx;

                    // enable this interface
                    pNetIf->Flags.bInterfaceEnabled = true;
                    pNetIf->Flags.bMacInitialize = false;
                    pNetIf->Flags.bMacInitDone = true;

#if defined(TCPIP_STACK_USE_PPP_INTERFACE)
                    if(_TCPIPStack_NetMacType(pNetIf) == TCPIP_MAC_TYPE_PPP)
                    {   // for a PPP interface we set the IP address obtained from negotiation
                        IPV4_ADDR lclIpAddr;
                        lclIpAddr.Val = 0;
                        _TCPIPStackSetIpAddress(pNetIf, &lclIpAddr, 0, 0, true);
                    }
#endif  // defined(TCPIP_STACK_USE_PPP_INTERFACE)

#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                    for(pAliasIf = _TCPIPStackNetGetAlias(pNetIf), aliasIx = 0; pAliasIf != 0; pAliasIf = _TCPIPStackNetGetAlias(pAliasIf), aliasIx++)
                    {
                        _TCPIPCopyMacAliasIf(pAliasIf, pNetIf);
                        pAliasIf->Flags.bMacInitialize = false;
                        pAliasIf->Flags.bMacInitDone = true;
                        if(pAliasIf->Flags.powerMode == TCPIP_MAC_POWER_FULL)
                        {   // enable the alias interface too;
                            pAliasIf->Flags.bInterfaceEnabled = true;
                        }
                    }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                }
            }
        }
    }

    // check we're getting out of Initialize procedure
    if(tcpip_stack_status == SYS_STATUS_BUSY)
    {
        uint32_t    ifUpMask = 0;   // masks of interfaces that are up;
        // max 32 interfaces are assumed for now!
        for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
        {
            pPriIf = _TCPIPStackNetGetPrimary(pNetIf);    // look at the primary initialization

            if(pPriIf->Flags.powerMode == TCPIP_MAC_POWER_FULL)
            {
                if(pPriIf->Flags.bMacInitDone == 0)
                {
                    return false;  // not done
                }
                else if(pPriIf->Flags.bInterfaceEnabled != 0)
                {
                    ifUpMask |= (1 << netIx);
                }
            }
            else
            {   // done
                ifUpMask |= (1 << netIx);
            }
        }

        // passed through all interfaces
        if(ifUpMask == ((1 << netIx) - 1))
        {   // all interfaces up
            tcpip_stack_status = SYS_STATUS_READY;
            SYS_CONSOLE_MESSAGE(TCPIP_STACK_HDR_MESSAGE "Initialization Ended - success \r\n");
        }
        else
        {   // failed initializing all interfaces;
            for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
            {
                if(pNetIf->Flags.bInterfaceEnabled || pNetIf->Flags.bMacInitialize)
                {
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                    if(_TCPIPStackNetIsPrimary(pNetIf))
                    {
                        for(pAliasIf = _TCPIPStackNetGetAlias(pNetIf), aliasIx = 0; pAliasIf != 0; pAliasIf = _TCPIPStackNetGetAlias(pAliasIf), aliasIx++)
                        {
                            TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pAliasIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
                        }

                        TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
                    }
#else
                    TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                }
            }
            tcpip_stack_status = SYS_STATUS_ERROR;
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Interface Initialization failed: 0x%x - Aborting! \r\n", ifUpMask);
            return false;
        }
    }

    return true;
}

static void _TCPIPStackSetIfNumberName(void)
{ 
    int netIx;
    TCPIP_NET_IF* pNetIf;
    TCPIP_MAC_TYPE macType;
    int ifNumber[TCPIP_MAC_TYPES];

    memset(ifNumber, 0, sizeof(ifNumber));

    for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
    {
        // set the interface number
        pNetIf->netIfIx = netIx;
        // set the interfaces name
        // and update the alias interfaces
        if(_TCPIPStackNetIsPrimary(pNetIf))
        {
            macType = (TCPIP_MAC_TYPE)pNetIf->macType;
            const char* ifName = TCPIP_STACK_IF_ALIAS_NAME_TBL[macType]; 
            snprintf(pNetIf->ifName, sizeof(pNetIf->ifName), "%s%d", ifName, ifNumber[macType]);
            pNetIf->ifName[sizeof(pNetIf->ifName) - 1] = 0;
            // update all its aliases
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            int aliasIx;
            TCPIP_NET_IF* pAliasIf;
            for(pAliasIf = _TCPIPStackNetGetAlias(pNetIf), aliasIx = 0; pAliasIf != 0; pAliasIf = _TCPIPStackNetGetAlias(pAliasIf), aliasIx++)
            {
                snprintf(pAliasIf->ifName, sizeof(pAliasIf->ifName), "%s%d:%d", ifName, ifNumber[macType], aliasIx % 10);
                pAliasIf->ifName[sizeof(pAliasIf->ifName) - 1] = 0;
            }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            ifNumber[macType]++;
        }
    }
}

#if !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)
static void _TCPIPStackExecuteModules(void)
{
    int     modIx;
    TCPIP_MODULE_SIGNAL_ENTRY*  pSigEntry;
    tcpipModuleSignalHandler    signalHandler;
    uint16_t                    signalVal;
    OSAL_CRITSECT_DATA_TYPE     critSect;

    pSigEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + TCPIP_MODULE_LAYER1;
    for(modIx = TCPIP_MODULE_LAYER1; modIx < sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL)/sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL); modIx++, pSigEntry++)
    {
        critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        signalHandler = pSigEntry->signalHandler;
        signalVal = pSigEntry->signalVal;
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

        if(signalHandler == 0 || signalVal == 0)
        {   // unused slot or no pending signals
            continue;
        }
        // pending signals; either TMO or RX related or ASYNC
        // execute the handler-> module Task function
        // Note: this can set signals for sibling modules!
        (*signalHandler)();
    }
}
#endif  // !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)

static void _TCPIP_ProcessTickEvent(void)
{
    int     netIx;
    TCPIP_NET_IF* pNetIf;
    bool    linkCurr, linkPrev;

    newTcpipTickAvlbl = 0;

    _TCPIP_SecondCountSet();    // update time
    stackLinkTmo -= stackTaskRate;
    if(stackLinkTmo <= 0)
    {   // timeout exceeded 
        stackLinkTmo += _TCPIP_STACK_LINK_RATE; 
        for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
        {
            if(pNetIf->Flags.bInterfaceEnabled)
            {
                linkCurr = (*pNetIf->pMacObj->TCPIP_MAC_LinkCheck)(pNetIf->hIfMac);     // check link status
                linkPrev = pNetIf->exFlags.linkPrev != 0;
                if(linkPrev != linkCurr)
                {   // link status changed
                    // just set directly the events, and do not involve the MAC notification mechanism
                    pNetIf->exFlags.connEvent = 1;
                    pNetIf->exFlags.connEventType = linkCurr ? 1 : 0 ;
                    pNetIf->exFlags.linkPrev = linkCurr;
                }
            }
        }
    }

}

static int _TCPIPExtractMacRxPackets(TCPIP_NET_IF* pNetIf)
{
    TCPIP_MAC_PACKET*       pRxPkt;
    int     nPackets = 0;

    // get all the new MAC packets
    while((pRxPkt = (*pNetIf->pMacObj->TCPIP_MAC_PacketRx)(pNetIf->hIfMac, 0, 0)) != 0)
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, pNetIf->macId);
        _TCPIPInsertMacRxPacket(pNetIf, pRxPkt);
        nPackets++;
    }

    return nPackets;
}

// Process the queued RX packets
// returns the mask of 1st layer frames that have been processed
// signals the 1st layer modules if needed
static uint32_t _TCPIPProcessMacPackets(bool signal)
{
    int                         frameIx;
    bool                        frameFound;
    uint16_t                    frameType;
    TCPIP_MAC_PACKET*           pRxPkt;
    TCPIP_MAC_ETHERNET_HEADER*  pMacHdr;
    const TCPIP_FRAME_PROCESS_ENTRY*  pFrameEntry;
    uint32_t                    procFrameMask = 0;

    SINGLE_LIST*                pPktQueue = (TCPIP_MODULES_QUEUE_TBL + TCPIP_MODULE_MANAGER);


    while((pRxPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_SingleListHeadRemove(pPktQueue)))
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
        pMacHdr = (TCPIP_MAC_ETHERNET_HEADER*)pRxPkt->pMacLayer;
        // get the packet type
        frameType = TCPIP_Helper_ntohs(pMacHdr->Type);

#if (TCPIP_STACK_EXTERN_PACKET_PROCESS != 0)
        TCPIP_NET_IF* pNetIf = (TCPIP_NET_IF*)pRxPkt->pktIf;
        TCPIP_STACK_PACKET_HANDLER pktHandler = pNetIf->pktHandler;
        if(pktHandler != 0)
        {
            bool was_processed = (*pktHandler)(pNetIf, pRxPkt, frameType, pNetIf->pktHandlerParam);
            if(was_processed)
            {
                TCPIP_PKT_FlightLogAcknowledge(pRxPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_EXTERN);
                continue;
            }
        }
            
#endif  // (TCPIP_STACK_EXTERN_PACKET_PROCESS != 0)

#if defined(TCPIP_STACK_USE_MAC_BRIDGE)
        TCPIP_MAC_BRIDGE_PKT_RES brRes = TCPIP_MAC_Bridge_ProcessPacket( pRxPkt);
        if(brRes != TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS)
        {
            continue;
        } 
        
#endif  // defined(TCPIP_STACK_USE_MAC_BRIDGE)


        frameFound = false;
        pFrameEntry = TCPIP_FRAME_PROCESS_TBL;
        for(frameIx = 0; frameIx < sizeof(TCPIP_FRAME_PROCESS_TBL) / sizeof(*TCPIP_FRAME_PROCESS_TBL); frameIx++, pFrameEntry++)
        {
            if(pFrameEntry->frameType == frameType)
            {   // found proper frame handler
                pRxPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_TYPE_MASK;
                pRxPkt->pktFlags |= pFrameEntry->pktTypeFlags;

                if(_TCPIPStackModuleRxInsert(pFrameEntry->moduleId, pRxPkt, 0))
                {
                    if(signal)
                    {   // signal to the module that RX is pending; if not already done so
                        if((procFrameMask & (1 << frameIx)) == 0)
                        {   // set the frame mask so we don't signal again
                            procFrameMask |= 1 << frameIx;
                            _TCPIPModuleSignalSetNotify(pFrameEntry->moduleId, TCPIP_MODULE_SIGNAL_RX_PENDING);
                        }
                    }
                    frameFound = true;
                }
                break;
            }
        }
        if(!frameFound)
        {   // unknown packet type; discard
            TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_TYPE_ERR); 
        }
    }

    return procFrameMask;
}

#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
// MAC ISR call
static void    _TCPIP_MacEventCB(TCPIP_MAC_EVENT event, const void* hParam)
{
    _TCPIP_NetIfEvent((TCPIP_NET_IF*)hParam, event, false);
}
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

static void _TCPIP_NetIfEvent(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT event, bool isrProtect)
{
    uint32_t isrSuspLvl = 0;

    TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry;
    pSigEntry = ((event & (TCPIP_STACK_MAC_ACTIVE_RX_EVENTS)) != 0) ? _TCPIPModuleToSignalEntry(TCPIP_MODULE_MANAGER) : 0;
   

    if(isrProtect)
    {
        isrSuspLvl = _TCPIPMacIsrSuspend(pNetIf);
    }

    pNetIf->activeEvents |= event;
    totTcpipEventsCnt++;

    if(pSigEntry)
    {
        pSigEntry->signalVal |= TCPIP_MODULE_SIGNAL_RX_PENDING;
    }

    if(isrProtect)
    {
        _TCPIPMacIsrResume(pNetIf, isrSuspLvl);
    }

    if(pSigEntry)
    {
        _TCPIPSignalEntryNotify(pSigEntry, TCPIP_MODULE_SIGNAL_RX_PENDING, 0);
    }
}


/*******************************************************************************
  Function:
    void    _TCPIP_STACK_TickHandler(uintptr_t context, uint32_t currTick)

  Summary:
    Stack tick handler.

  Description:
    This function is called from within the System Tick ISR.
    It provides the Stack tick processing.
    It will call the notification handler registered with SYS_TMR_CallbackPeriodic


  Precondition:
   System Tick should have been initialized
   and the Stack tick handler should have been registered with the SYS_TMR_CallbackPeriodic.

  Parameters:
    currSysTick   - current system tick value at the time of call

  Returns:
    None

  Remarks:
    To avoid synchronization issues between the TCP/IP threads and TMR thread
    or MAC ISR and TMR ISR
    the TMO signal is used on a different signal entry: TCPIP_MODULE_NONE!
*****************************************************************************/
static void _TCPIP_STACK_TickHandler(uintptr_t context, uint32_t currTick)
{
    newTcpipTickAvlbl++;

    TCPIP_MODULE_SIGNAL_ENTRY* pTmoEntry = _TCPIPModuleToSignalEntry(TCPIP_MODULE_NONE);
    TCPIP_MODULE_SIGNAL_ENTRY* pMgrEntry = _TCPIPModuleToSignalEntry(TCPIP_MODULE_MANAGER);

    pTmoEntry->signalVal |= TCPIP_MODULE_SIGNAL_TMO;

    _TCPIPSignalEntryNotify(pMgrEntry, TCPIP_MODULE_SIGNAL_TMO, 0);

}

static void _TCPIP_ProcessMACErrorEvents(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT activeEvent)
{
    newTcpipErrorEventCnt++;
}





/*********************************************************************
 * Function:        TCPIP_STACK_NetDefaultGet(void)
 *
 * PreCondition:    TCPIP stack should have been initialized by
 *                    TCPIP_STACK_Initialize()
 *
 * Input:           None
 *
 * Output:          The default net interface for multi-homed hosts
 *
 * Side Effects:    None
 *
 * Note:            Function to dynamically change the default interface
 *                  will be added.
 ********************************************************************/
TCPIP_NET_HANDLE TCPIP_STACK_NetDefaultGet(void)
{
    return tcpipDefIf.defaultNet;
}

// sets the default interface
// returns true if success,
// false if failed (the old interface does not change)
bool TCPIP_STACK_NetDefaultSet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNewIf = _TCPIPStackHandleToNetUp(netH);
    if(pNewIf)
    {
        tcpipDefIf.defaultNet = pNewIf;
        return true;
    }

    return false;
}

// selects a default interface if needed
// the default interface could become 0 
// when a network interface is going down, for example
// if pDownIf != 0 specifies a going down interface
static void _TCPIP_SelectDefaultNet(TCPIP_NET_IF* pDownIf)
{
    int netIx;
    TCPIP_NET_IF *pIf, *pNewIf;
    bool searchDefault = false;

    if(tcpipDefIf.defaultNet == 0)
    {   // need search
        searchDefault = true;
    }
    else if(tcpipDefIf.defaultNet == pDownIf)
    {   // interface is going down; replace
        tcpipDefIf.defaultNet = 0;
        searchDefault = true;
    }
    else if(pDownIf != 0)
    {
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        if((_TCPIPStackNetGetPrimary(tcpipDefIf.defaultNet) == _TCPIPStackNetGetPrimary(pDownIf)))
        {
            tcpipDefIf.defaultNet = 0;
            searchDefault = true;
        }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    }

    // repeat for the multicast interface
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)
    if(tcpipDefIf.defaultMcastNet == 0)
    {   // need search
        searchDefault = true;
    }
    else if(tcpipDefIf.defaultMcastNet == pDownIf)
    {   // interface is going down; replace
        tcpipDefIf.defaultMcastNet = 0;
        searchDefault = true;
    }
    else if(pDownIf != 0)
    {
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        if((_TCPIPStackNetGetPrimary(tcpipDefIf.defaultMcastNet) == _TCPIPStackNetGetPrimary(pDownIf)))
        {
            tcpipDefIf.defaultMcastNet = 0;
            searchDefault = true;
        }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    }
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)


    if(searchDefault)
    {
        pNewIf = 0;
        for(netIx = 0, pIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pIf++)
        {
            if(pIf->Flags.bInterfaceEnabled)
            {   // select this one
                pNewIf = pIf;
                break;
            }
        }

        if(pNewIf != 0)
        {   // favor a primary interface
            pNewIf = _TCPIPStackNetGetPrimary(pNewIf); 
        }
        if(tcpipDefIf.defaultNet == 0)
        {
            tcpipDefIf.defaultNet = pNewIf;
        }
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)
        if(tcpipDefIf.defaultMcastNet == 0)
        {
            tcpipDefIf.defaultMcastNet = pNewIf;
        }
#endif
    }

}

TCPIP_NET_HANDLE TCPIP_STACK_NetMulticastGet(void)
{
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)
    return tcpipDefIf.defaultMcastNet;
#else
    return 0;
#endif
}

bool TCPIP_STACK_NetMulticastSet(TCPIP_NET_HANDLE netH)
{
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)
    TCPIP_NET_IF* pNewIf = _TCPIPStackHandleToNetUp(netH);
    if(pNewIf)
    {
        tcpipDefIf.defaultMcastNet = pNewIf;
        return true;
    }
#endif

    return false;

}
/*********************************************************************
 * Function:        TCPIP_STACK_IPAddToNet(IPV4_ADDR* pIpAddress, bool useDefault)
 *
 * PreCondition:    TCPIP stack should have been initialized by
 *                    TCPIP_STACK_Initialize()
 *
 * Input:           pIpAddress - pointer to an IP address
 *
 *                  useDefault - when no interface is found,
 *                               if true: return the default interface
 *                               else return 0;
 *
 * Output:          Resolves a local IP address to a network interface.
 *
 *
 * Side Effects:    None
 *
 * Note:            None
 ********************************************************************/
TCPIP_NET_IF* TCPIP_STACK_IPAddToNet(IPV4_ADDR* pIpAddress, bool useDefault)
{
    TCPIP_NET_IF* pNetIf = 0;

    if(pIpAddress && pIpAddress->Val != 0)
    {
        pNetIf = TCPIP_STACK_NetByAddress(pIpAddress);
    }

    if(pNetIf == 0 && useDefault)
    {
        pNetIf = tcpipDefIf.defaultNet;
    }

    return pNetIf;
}

/*********************************************************************
 * Function:        _TCPIPStackIpAddFromAnyNet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* pIpAddress)
 *
 * PreCondition:    TCPIP stack should have been initialized by
 *                    TCPIP_STACK_Initialize()
 *
 * Input:           pNetIf  - network interface to check for
 *                            if 0 all interfaces are checked
 *                  pIpAddress - pointer to an IP address
 *
 * Output:          Resolves a local IP address to a network interface
 *                  to which it belongs
 *
 * Side Effects:    None
 *
 * Note:            None
 ********************************************************************/
TCPIP_NET_IF* _TCPIPStackIpAddFromAnyNet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* pIpAddress)
{
    int netIx;
    TCPIP_NET_IF* pIf;

    if(pIpAddress && pIpAddress->Val != 0)
    {
        for(netIx = 0, pIf = tcpipNetIf ; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pIf++)
        {
            if(pNetIf == 0 || pIf == pNetIf)
            {
                if(pIf->Flags.bInterfaceEnabled)
                {
                    if(_TCPIPStackIpAddFromLAN(pIf, pIpAddress))
                    {
                        return pIf;
                    }
                }
            }
        }
    }

    return 0;
}

/*********************************************************************
 * Function:        TCPIP_STACK_MacToNet(TCPIP_MAC_HANDLE hMac)
 *
 * PreCondition:    TCPIP stack should have been initialized by
 *                    TCPIP_STACK_Initialize()
 *
 * Input:           None
 *
 * Output:          Resolves a MAC Id to an tcpipNetIf entry.
 *
 *
 * Side Effects:    None
 *
 * Note:            The tcpipNetIf entries match 1 to 1 the network interfaces
 *                  A more efficient algorithm to find MAC<->tcpipNetIf entry correspondence
 *                  will be eventually added.
 ********************************************************************/
TCPIP_NET_IF* TCPIP_STACK_MacToNet(TCPIP_MAC_HANDLE hMac)
{
    int netIx;
    TCPIP_NET_IF* pNetIf;

    for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
    {
        if(pNetIf->hIfMac == hMac)
        {
            return pNetIf;
        }
    }


    return 0;
}

/*********************************************************************
 * Function:        TCPIP_STACK_MacToNetIndex(TCPIP_MAC_HANDLE hMac)
 *
 * PreCondition:    TCPIP stack should have been initialized by
 *                    TCPIP_STACK_Initialize()
 *
 * Input:           None
 *
 * Output:          Resolves a MAC to an tcpipNetIf entry.
 *
 *
 * Side Effects:    None
 *
 * Note:            The tcpipNetIf entries match 1 to 1 the network interfaces
 *                  A more efficient algorithm to find MAC<->tcpipNetIf entry correspondence
 *                  will be eventually added.
 ********************************************************************/
int TCPIP_STACK_MacToNetIndex(TCPIP_MAC_HANDLE hMac)
{
    int netIx;
    TCPIP_NET_IF* pNetIf;

    for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
    {
        if(pNetIf->hIfMac == hMac)
        {
            return netIx;
        }
    }


    return -1;
}

/*********************************************************************
 * Function:        int TCPIP_STACK_NumberOfNetworksGet(void)
 *
 * PreCondition:    TCPIP stack should have been initialized by TCPIP_STACK_Initialize()
 *
 * Input:           None
 *
 * Output:          Number of network interfaces
 *
 * Side Effects:    None
 *
 * Note:            None
 ********************************************************************/
int TCPIP_STACK_NumberOfNetworksGet(void)
{
    return tcpip_stack_ctrl_data.nIfs;
}

/*********************************************************************
 * Function:        TCPIP_STACK_IndexToNet(int netIx)
 *
 * PreCondition:    TCPIP stack should have been initialized by
 *                    TCPIP_STACK_Initialize()
 *
 * Input:           None
 *
 * Output:          Resolves an index to an tcpipNetIf entry.
 *
 *
 * Side Effects:    None
 *
 * Note:            The tcpipNetIf entries match 1 to 1 the network interfaces
 ********************************************************************/
TCPIP_NET_HANDLE TCPIP_STACK_IndexToNet(int netIx)
{
    if(0 <= netIx && netIx < tcpip_stack_ctrl_data.nIfs)
    {
        return tcpipNetIf + netIx;
    }

    return 0;
}

int  TCPIP_STACK_NetIndexGet(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(hNet);
    return TCPIP_STACK_NetIxGet(pNetIf);
}

// check a net handle is a valid network interface
// more checking, for user passed handles
TCPIP_NET_IF*  TCPIP_Stack_UserHandleToNet(TCPIP_NET_HANDLE hNet)
{
    if(tcpipNetIf != 0)
    {
        TCPIP_NET_IF* pNetIf = (TCPIP_NET_IF*)hNet;
        int ix = pNetIf - tcpipNetIf;
        if(0 <= ix && ix < tcpip_stack_ctrl_data.nIfs)
        {
            if(pNetIf == tcpipNetIf + ix)
            {
                return pNetIf;
            }
        }

    }

    return 0;
}

/*********************************************************************
 * Function:        TCPIP_STACK_MACIdToNet(TCPIP_STACK_MODULE macId)
 *
 * PreCondition:    TCPIP stack should have been initialized by
 *                    TCPIP_STACK_Initialize()
 *
 * Input:           None
 *
 * Output:          Resolves an MAC id to a Net entry.
 *
 *
 * Side Effects:    None
 *
 * Note:            In multi-homed hosts with multiple
 *                  interfaces of the same type,
 *                  the translation might not be unique.
 *                  The first match is returned!
 *
 ********************************************************************/
TCPIP_NET_IF* TCPIP_STACK_MACIdToNet(TCPIP_STACK_MODULE macId)
{
    TCPIP_NET_IF* pNetIf;

    if(macId != (TCPIP_STACK_MODULE)TCPIP_MODULE_MAC_NONE)
    {
        int netIx;
        for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
        {
            if(pNetIf->macId == macId)
            {
                return pNetIf;
            }
        }
    }


    return 0;

}

/*********************************************************************
 * Function:        TCPIP_NET_HANDLE TCPIP_STACK_NetHandleGet(const char* interface)
 *
 * PreCondition:    TCPIP stack should have been initialized by TCPIP_STACK_Initialize()
 *
 * Input:           interface - The names specified in TCPIP_NETWORK_CONFIG.
 *
 * Output:          Resolves an interface name to a handle.
 *
 * Side Effects:    None
 *
 * Example:            TCPIP_NET_HANDLE hNet = TCPIP_STACK_NetHandleGet("PIC32INT\r\n");
 *
 * Note:            None
 ********************************************************************/
TCPIP_NET_HANDLE TCPIP_STACK_NetHandleGet(const char* interface)
{
    // try first the aliases
    int netIx;
    TCPIP_NET_IF* pNetIf;

    for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
    {
        if(strcmp(pNetIf->ifName, interface) == 0)
        {
            return (TCPIP_NET_HANDLE)pNetIf;
        }
    }

    // try the full interface name


    return TCPIP_STACK_MACIdToNet(TCPIP_STACK_StringToMACId(interface));
}


/*********************************************************************
 * Function:        const char* TCPIP_STACK_NetNameGet(TCPIP_NET_HANDLE netH)
 *
 * PreCondition:    TCPIP stack should have been initialized by TCPIP_STACK_Initialize()
 *
 * Input:           netH - Interface handle to get the name of.
 *
 * Output:          it returns the name associated to that interface handle
 *                     returns 0 if no such name
 *
 * Side Effects:    None
 *
 * Example:            TCPIP_NET_HANDLE netH = TCPIP_STACK_IndexToNet(0);
 *                     const char* netName = TCPIP_STACK_NetNameGet(netH);
 *
 * Note:            None
 ********************************************************************/
const char* TCPIP_STACK_NetNameGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF*  pNetIf = _TCPIPStackHandleToNet(netH);

    if(pNetIf != 0 && pNetIf->pMacObj != 0)
    {
        return pNetIf->pMacObj->macName;
    }

    return 0;
}

int TCPIP_STACK_NetAliasNameGet(TCPIP_NET_HANDLE netH, char* nameBuffer, int buffSize)
{
    int aliasSize;
    TCPIP_NET_IF*  pNetIf = _TCPIPStackHandleToNet(netH);


    if(pNetIf)
    {
        aliasSize = strlen(pNetIf->ifName);
    }
    else
    {
        aliasSize = 0;
    }

    if(nameBuffer && buffSize)
    {   // valid buffer
        if(aliasSize)
        {
            strncpy(nameBuffer, pNetIf->ifName, buffSize - 1);
            nameBuffer[buffSize - 1] = 0;
        }
        else
        {
            nameBuffer[0] = 0;
        }
    }
    return aliasSize;
}


const TCPIP_MAC_OBJECT* TCPIP_STACK_MACObjectGet(TCPIP_NET_HANDLE netH)
{

    TCPIP_NET_IF*  pNetIf = _TCPIPStackHandleToNet(netH);

    if(pNetIf)
    {
        return pNetIf->pMacObj;
    }

    return 0;
}



/*********************************************************************
 * Function:        TCPIP_STACK_NetByAddress(const IPV4_ADDR* pIpAddress)
 *
 * PreCondition:    TCPIP stack should have been initialized by
 *                    TCPIP_STACK_Initialize()
 *
 * Input:           pointer to an IP address
 *
 * Output:          The network interface pointer to which this ip
 *                  address belongs to.
 *                  NULL if not one of our addresses.
 *
 * Side Effects:    None
 *
 * Note:            A single network interface can support multiple IP addresses.
 *                  For now this feature is not implemented/supported.
 *
 ********************************************************************/
TCPIP_NET_IF* TCPIP_STACK_NetByAddress(const IPV4_ADDR* pIpAddress)
{
    int netIx;
    TCPIP_NET_IF* pIf;

    for(netIx = 0, pIf = tcpipNetIf ; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pIf++)
    {
        if(pIf->Flags.bInterfaceEnabled && pIf->netIPAddr.Val == pIpAddress->Val)
        {
            return pIf;
        }
    }

    return 0;
}

// finds a network interface matching an IPv4 address
// does NOT check for interface up/down!
TCPIP_NET_IF* TCPIP_STACK_MatchNetAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    int netIx;
    TCPIP_NET_IF* pIf;

    if(pIpAdd->Val == 0x0100007f /* || pNetIf->netIPAddr.Val == pIpAdd->Val*/)
    {
        return pNetIf;
    }

    for(netIx = 0, pIf = tcpipNetIf ; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pIf++)
    {
        if(pIf->netIPAddr.Val == pIpAdd->Val)
        {
            return pIf;
        }
    }

    return 0;
}

#if defined(TCPIP_STACK_USE_IPV4)
uint32_t TCPIP_STACK_NetAddress(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    return TCPIP_STACK_NetAddressGet(pNetIf);
}
#endif  // defined(TCPIP_STACK_USE_IPV4)


#if defined(TCPIP_STACK_USE_IPV6)
IPV6_ADDR_HANDLE TCPIP_STACK_NetIPv6AddressGet(TCPIP_NET_HANDLE netH, IPV6_ADDR_TYPE addType, IPV6_ADDR_STRUCT* pAddStruct, IPV6_ADDR_HANDLE addHandle)
{
    IPV6_ADDR_STRUCT * addrNode;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf == 0)
    {
        return 0;
    }


    // Note: for both unicast and multicast addresses we start from unicast list
    // that's because we need to construct the solicited node multicas address
    // which is not currently stored in its own list!

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);
    if(addHandle == 0)
    {   // start iteration through the list
        addrNode = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6UnicastAddresses.head;
    }
    else
    {
        addrNode = ((IPV6_ADDR_STRUCT*)addHandle)->next;
    }

    if(addType == IPV6_ADDR_TYPE_UNICAST)
    {
        if(addrNode && pAddStruct)
        {
            memcpy(pAddStruct, addrNode, sizeof(*addrNode));
            pAddStruct->next = pAddStruct->prev = 0;
        }
        return addrNode;
    }

    if(addType == IPV6_ADDR_TYPE_MULTICAST)
    {
        if(addrNode == 0)
        {
            if(addHandle == 0 || ((IPV6_ADDR_STRUCT*)addHandle)->flags.type == IPV6_ADDR_TYPE_UNICAST)
            {   // either the unicast list is empty or finished the unicast list
                addrNode = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6MulticastAddresses.head;
            }
        }

        if(addrNode != 0 && addrNode->flags.type == IPV6_ADDR_TYPE_UNICAST)
        {   // do not report the same solicited node address multiple times
            IPV6_ADDR_STRUCT * unicastHead = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6UnicastAddresses.head;
            IPV6_ADDR_STRUCT * currAddress = unicastHead;
            while(currAddress != addrNode)
            {
                IPV6_ADDR* pAddNode = (IPV6_ADDR*)((uint8_t*)addrNode + offsetof(struct _IPV6_ADDR_STRUCT, address));
                IPV6_ADDR* pAddCurr = (IPV6_ADDR*)((uint8_t*)currAddress + offsetof(struct _IPV6_ADDR_STRUCT, address));
                if(memcmp(pAddNode->v + sizeof (IPV6_ADDR) - 3, pAddCurr->v + sizeof (IPV6_ADDR) - 3, 3) == 0)
                {   // address match; skip this one
                    addrNode = addrNode->next;
                    if(addrNode == 0)
                    {   // end of list
                        addrNode = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6MulticastAddresses.head;
                        break;
                    }
                    else
                    {   // restart traversal
                        currAddress = unicastHead; 
                    }
                }
                else
                {
                    currAddress = currAddress->next;
                }
            }
        }


        if(addrNode && pAddStruct)
        {
            memcpy(pAddStruct, addrNode, sizeof(*addrNode));
            pAddStruct->next = pAddStruct->prev = 0;
            if(addrNode->flags.type == IPV6_ADDR_TYPE_UNICAST)
            {   // construct the solicited node multicast address
                IPV6_ADDR* pAddNode = (IPV6_ADDR*)((uint8_t*)pAddStruct + offsetof(struct _IPV6_ADDR_STRUCT, address));
                memcpy(pAddNode->v, IPV6_SOLICITED_NODE_MULTICAST.v, sizeof (IPV6_ADDR) - 3);
                pAddStruct->flags.type = IPV6_ADDR_TYPE_MULTICAST;
            }
        }
        return addrNode;
    }


    // no other address type supported
    return 0;
}

// finds an interface that has the IPv6 address
TCPIP_NET_IF* _TCPIPStackIPv6AddToNet(IPV6_ADDR* pIPv6Address, IPV6_ADDR_TYPE addType, bool useDefault)
{
    TCPIP_NET_IF* pNetIf;
    int           netIx;
    TCPIP_NET_IF* pSrchIf = 0;

    if(pIPv6Address != 0)
    {
        for(netIx = 0; netIx < tcpip_stack_ctrl_data.nIfs; netIx++)
        {
            pNetIf = tcpipNetIf + netIx;
            if(TCPIP_IPV6_AddressFind(pNetIf, pIPv6Address, addType) != 0)
            {    // found interface
                pSrchIf = pNetIf;
                break;
            }
        }
    }

    if(pSrchIf == 0 && useDefault)
    {
        pSrchIf = tcpipDefIf.defaultNet;
    }

    return pSrchIf;
}



#endif  // defined(TCPIP_STACK_USE_IPV6)

// user thread
#if defined(TCPIP_STACK_USE_IPV4)
bool TCPIP_STACK_NetAddressSet(TCPIP_NET_HANDLE netH, IPV4_ADDR* ipAddress, IPV4_ADDR* mask, bool setDefault)
{
    bool success = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf != 0 && TCPIP_STACK_AddressServiceCanStart(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_NONE))
    {
        _TCPIPStackSetIpAddress(pNetIf, ipAddress, mask, 0, setDefault);
        success = true;
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

    if(success)
    {
        _TCPIPStackSetConfig(pNetIf, false);
    }
    return success;
}

bool TCPIP_STACK_NetAddressGatewaySet(TCPIP_NET_HANDLE netH, IPV4_ADDR* ipAddress)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf)
    {
        TCPIP_STACK_GatewayAddressSet(pNetIf, ipAddress);
        return true;
    }

    return false;
}

uint32_t TCPIP_STACK_NetAddressGateway(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);

    if(pNetIf)
    {
        return pNetIf->netGateway.Val;
    }

    return 0;
}

uint32_t TCPIP_STACK_NetAddressDnsPrimary(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);

    if(pNetIf)
    {
        return pNetIf->dnsServer[0].Val;
    }

    return 0;
}

uint32_t TCPIP_STACK_NetAddressDnsSecond(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);

    if(pNetIf)
    {
        return pNetIf->dnsServer[1].Val;
    }

    return 0;
}

bool TCPIP_STACK_NetAddressDnsPrimarySet(TCPIP_NET_HANDLE netH, IPV4_ADDR* ipAddress)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    TCPIP_STACK_PrimaryDNSAddressSet(pNetIf, ipAddress);

    return pNetIf != 0;
}

bool TCPIP_STACK_NetAddressDnsSecondSet(TCPIP_NET_HANDLE netH, IPV4_ADDR* ipAddress)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf)
    {
        TCPIP_STACK_SecondaryDNSAddressSet(pNetIf, ipAddress);
        return true;
    }

    return false;
}

#if defined(TCPIP_STACK_USE_IPV6)
// Note: the scope of the ipv6Address should match the scope of an unicast IPv6 net addresses
bool  TCPIP_STACK_NetDnsIPv6Set(TCPIP_NET_HANDLE netH, const IPV6_ADDR* ipv6Address)
{
    if(ipv6Address != NULL)
    {
        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
        if(pNetIf != NULL)
        {
            IPV6_ADDR_HANDLE    ip6AddHndl = NULL;
            IPV6_ADDR_STRUCT    ip6AddStr, *pAddStr;
            IPV6_ADDRESS_TYPE   dnsType;

            (void)memset(&ip6AddStr, 0, sizeof(ip6AddStr));
            pAddStr = &ip6AddStr;

            dnsType.byte = TCPIP_IPV6_AddressTypeGet(pNetIf, ipv6Address);

            bool res = false;
            while(true)
            {
                ip6AddHndl = TCPIP_STACK_NetIPv6AddressGet(pNetIf, IPV6_ADDR_TYPE_UNICAST, pAddStr, ip6AddHndl);
                if(ip6AddHndl == NULL)
                {   // no more valid IPv6 addresses;
                    break; 
                }
                else if(pAddStr->flags.scope == dnsType.bits.scope)
                {   // found it
                    res = true;
                    break;
                }
                else
                {
                    // continue
                }
            }

            if(res != false)
            {
                pNetIf->netIPv6Dns[0] = *ipv6Address;
                pNetIf->Flags.bIpv6DnsValid = 1;
                return true;
            }
        }
    }

    return false;
}

bool  TCPIP_STACK_NetDnsIPv6Get(TCPIP_NET_HANDLE netH, IPV6_ADDR* dnsAddress)
{
    if(dnsAddress != NULL)
    {
        TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
        if(pNetIf != NULL)
        {
            (void)memset(dnsAddress->v, 0, sizeof(*dnsAddress));
            if(pNetIf->Flags.bIpv6DnsValid == 1U)
            {
                *dnsAddress = pNetIf->netIPv6Dns[0];
            }
            return true;
        }
    }

    return false;
}
#else
bool TCPIP_STACK_NetDnsIPv6Set(TCPIP_NET_HANDLE netH, const IPV6_ADDR* ipv6Address)
{
    return false;
}
bool TCPIP_STACK_NetDnsIPv6Get(TCPIP_NET_HANDLE netH, IPV6_ADDR* dnsAddress)
{
    return false;
}
#endif  // defined(TCPIP_STACK_USE_IPV6)

uint32_t TCPIP_STACK_NetMask(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf)
    {
        return TCPIP_STACK_NetMaskGet(pNetIf);
    }

    return 0;
}
#endif  // defined(TCPIP_STACK_USE_IPV4)


bool TCPIP_STACK_NetAddressMacSet(TCPIP_NET_HANDLE netH, const TCPIP_MAC_ADDR* pAddr)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf)
    {
        memcpy(pNetIf->netMACAddr.v, pAddr->v, sizeof(pNetIf->netMACAddr));
        return true;
    }

    return false;
}

const char* TCPIP_STACK_NetBIOSName(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
    if(pNetIf)
    {
        return (const char*)pNetIf->NetBIOSName;
    }

    return 0;

}

bool TCPIP_STACK_NetBiosNameSet(TCPIP_NET_HANDLE netH, const char* biosName)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf)
    {
        memcpy(pNetIf->NetBIOSName, biosName, sizeof(pNetIf->NetBIOSName));
        TCPIP_Helper_FormatNetBIOSName(pNetIf->NetBIOSName);
        return true;
    }

    return false;
}

const uint8_t* TCPIP_STACK_NetAddressMac(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(netH);
    return TCPIP_STACK_NetUpMACAddressGet(pNetIf);
}

TCPIP_STACK_MODULE  TCPIP_STACK_NetMACIdGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf)
    {
        return TCPIP_STACK_NetMACId(pNetIf);
    }

    return TCPIP_MODULE_NONE;
}

TCPIP_MAC_TYPE TCPIP_STACK_NetMACTypeGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf)
    {
        return _TCPIPStack_NetMacType(pNetIf);
    }

    return TCPIP_MAC_TYPE_NONE;
}

bool TCPIP_STACK_NetMACStatisticsGet(TCPIP_NET_HANDLE netH, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf)
    {
        TCPIP_MAC_RES res = (*pNetIf->pMacObj->TCPIP_MAC_StatisticsGet)(pNetIf->hIfMac, pRxStatistics, pTxStatistics);
        if(res == TCPIP_MAC_RES_OK)
        {
            return true;
        }
    }

    return false;
}

bool  TCPIP_STACK_NetMACRegisterStatisticsGet(TCPIP_NET_HANDLE netH, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf)
    {
        TCPIP_MAC_RES res = (*pNetIf->pMacObj->TCPIP_MAC_RegisterStatisticsGet)(pNetIf->hIfMac, pRegEntries, nEntries, pHwEntries);
        if(res == TCPIP_MAC_RES_OK)
        {
            return true;
        }
    }

    return false;
}


// only MAC support for now!
#if (TCPIP_STACK_CONFIGURATION_SAVE_RESTORE != 0)
size_t TCPIP_STACK_ModuleConfigGet(TCPIP_STACK_MODULE modId, void* configBuff, size_t buffSize, size_t* pNeededSize)
{
    if(tcpipNetIf != 0)
    {   // we should be up and running for this

        int netIx;
        TCPIP_NET_IF* pNetIf;

        for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
        {
            if(pNetIf->macId == modId)
            {
                if(pNetIf->pMacObj != 0)
                {   // found MAC module
                    return (*pNetIf->pMacObj->TCPIP_MAC_ConfigGet)(pNetIf->hIfMac, configBuff, buffSize, pNeededSize);
                }
            }
        }
    }


    // not found
    return -1;
}

// all the parameters are returned without checking
// that the interface is enabled or not!
size_t TCPIP_STACK_NetConfigGet(TCPIP_NET_HANDLE netH, void* configStoreBuff, size_t configStoreSize, size_t* pNeededSize)
{
    TCPIP_NET_IF* pNetIf;
    TCPIP_STACK_NET_IF_DCPT* pNetStg;

    if(tcpipNetIf == 0 || (pNetIf = _TCPIPStackHandleToNet(netH))== 0)
    {   // we should be up and running and have a valid IF
        return -1;
    }

    if(pNeededSize == 0 && (configStoreBuff == 0 || configStoreSize == 0 ))
    {   // nothing to do
        return 0;
    }
    // store needed size
    if(pNeededSize)
    {
        *pNeededSize = sizeof(*pNetStg); 
    }

    if(configStoreBuff && configStoreSize >= sizeof(*pNetStg))
    {   // copy all the fields
        pNetStg = (TCPIP_STACK_NET_IF_DCPT*)configStoreBuff;

        // the TCPIP_STACK_NET_IF_DCPT has to be at the very beginning of the pNetIf !!!
        memcpy(pNetStg, &pNetIf->size, sizeof(*pNetStg));
        // update the size field
        pNetStg->size = sizeof(*pNetStg);

        return sizeof(*pNetStg);
    }

    return 0;
}

static void* _NetConfigStringToBuffer(void** ppDstBuff, void* pSrcBuff, size_t* pDstSize, size_t* pNeedLen, size_t* pActLen)
{
    size_t  currLen;

    currLen = strlen(pSrcBuff) + 1;

    *pNeedLen += currLen;
    if(currLen && currLen <= *pDstSize)
    {
        void* pCopy = *ppDstBuff;
        memcpy(*ppDstBuff, pSrcBuff, currLen);
        *(uint8_t**)ppDstBuff += currLen;
        *pDstSize -= currLen;
        *pActLen += currLen;
        return pCopy;
    }
    else
    {   // stop copying
        *pDstSize = 0;
        return 0;
    }

}

// restores pNetConfig from configBuff
TCPIP_NETWORK_CONFIG*   TCPIP_STACK_NetConfigSet(void* configStoreBuff, void* netConfigBuff, size_t buffSize, size_t* pNeededSize)
{
    TCPIP_NETWORK_CONFIG* pNetConf;            
    TCPIP_STACK_NET_IF_DCPT* pNetStg = (TCPIP_STACK_NET_IF_DCPT*)configStoreBuff;

    if(configStoreBuff == 0 || (pNeededSize == 0 && netConfigBuff == 0 ))
    {   // nothing to do
        return 0;
    }

    // minimum sanity check
    if(pNetStg->size != sizeof(*pNetStg))
    {   // not valid config save?
        return 0;
    }

    if(buffSize < sizeof(*pNetConf))
    {   // not even enough room to start
        return 0;
    }

    char    tempBuff[50 + 1];   // buffer large enough to hold any string in a TCPIP_NETWORK_CONFIG!
    void*   pDstBuff;
    size_t  dstSize;
    size_t  needLen, actualLen;
    
    // create at the very beginning of the buffer
    pNetConf = (TCPIP_NETWORK_CONFIG*)netConfigBuff;
    pDstBuff = pNetConf + 1;    // write area
    dstSize = buffSize - sizeof(*pNetConf);
    needLen = actualLen = 0;
    tempBuff[sizeof(tempBuff) - 1] = '\0';   // always end properly
    
    // get each field
    if(pNetStg->pMacObj != 0 && pNetStg->pMacObj->macName != 0)
    {
        strncpy(tempBuff, pNetStg->pMacObj->macName, sizeof(tempBuff) - 1);
        pNetConf->interface = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);
    }
    else
    {
        pNetConf->interface = 0;
    }

    strncpy(tempBuff, (char*)pNetStg->NetBIOSName, sizeof(tempBuff) - 1);
    pNetConf->hostName = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    TCPIP_Helper_MACAddressToString(&pNetStg->netMACAddr, tempBuff, sizeof(tempBuff) - 1);
    pNetConf->macAddr = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    TCPIP_Helper_IPAddressToString(&pNetStg->netIPAddr, tempBuff, sizeof(tempBuff) - 1);
    pNetConf->ipAddr = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    TCPIP_Helper_IPAddressToString(&pNetStg->netMask, tempBuff, sizeof(tempBuff) - 1);
    pNetConf->ipMask = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    TCPIP_Helper_IPAddressToString(&pNetStg->netGateway, tempBuff, sizeof(tempBuff) - 1);
    pNetConf->gateway = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    TCPIP_Helper_IPAddressToString(&pNetStg->dnsServer[0], tempBuff, sizeof(tempBuff) - 1);
    pNetConf->priDNS = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    TCPIP_Helper_IPAddressToString(&pNetStg->dnsServer[1], tempBuff, sizeof(tempBuff) - 1);
    pNetConf->secondDNS = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    strncpy(tempBuff, TCPIP_Helper_PowerModeToString(pNetStg->Flags.powerMode), sizeof(tempBuff) - 1);
    pNetConf->powerMode = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    // set the flags
    pNetConf->startFlags = 0; 
    if(pNetStg->Flags.bIsDHCPEnabled)
    {
        pNetConf->startFlags |= TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON; 
    }
    if(pNetStg->Flags.bIsDHCPSrvEnabled)
    {
        pNetConf->startFlags |= TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON; 
    }
    if(pNetStg->Flags.bIsZcllEnabled)
    {
        pNetConf->startFlags |= TCPIP_NETWORK_CONFIG_ZCLL_ON; 
    }
    // set the MAC driver object
    pNetConf->pMacObject = pNetStg->pMacObj;
#if defined(TCPIP_STACK_USE_IPV6)
    if((pNetStg->startFlags & TCPIP_NETWORK_CONFIG_IPV6_ADDRESS) != 0)
    {
        pNetConf->startFlags |= TCPIP_NETWORK_CONFIG_IPV6_ADDRESS;
        pNetConf->ipv6PrefixLen = pNetStg->ipv6PrefixLen;

        TCPIP_Helper_IPv6AddressToString(&pNetStg->netIPv6Addr, tempBuff, sizeof(tempBuff) - 1);
        pNetConf->ipv6Addr = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

        TCPIP_Helper_IPv6AddressToString(&pNetStg->netIPv6Gateway, tempBuff, sizeof(tempBuff) - 1);
        pNetConf->ipv6Gateway = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);
        
        (void) TCPIP_Helper_IPv6AddressToString(pNetStg->netIPv6Dns, tempBuff, sizeof(tempBuff) - 1U);
        pNetConf->ipv6Dns = _NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);
    }

#endif  // defined(TCPIP_STACK_USE_IPV6)

    if(pNeededSize)
    {
        *pNeededSize = needLen + sizeof(*pNetConf);
    }

    if(actualLen == needLen)
    {   // succeeded
        return pNetConf;
    }

    return 0;

}
#endif  // (TCPIP_STACK_CONFIGURATION_SAVE_RESTORE != 0)

TCPIP_STACK_MODULE TCPIP_STACK_StringToMACId(const char* str)
{
    if(str)
    {
        int netIx;
        TCPIP_NET_IF* pNetIf;
        const TCPIP_MAC_OBJECT  *pObj;

        for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
        {
            if((pObj = pNetIf->pMacObj) != 0)
            {
                if(strcmp(str, pObj->macName) == 0)
                {
                    return pObj->macId;
                }
            }
        }
    }


    return (TCPIP_STACK_MODULE)TCPIP_MODULE_MAC_NONE;
}



const char* TCPIP_STACK_MACIdToString(TCPIP_STACK_MODULE moduleId)
{
    int netIx;
    TCPIP_NET_IF* pNetIf;
    const TCPIP_MAC_OBJECT  *pObj;

    for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
    {
        if((pObj = pNetIf->pMacObj) != 0)
        {
            if(pObj->macId == moduleId)
            {
                return pObj->macName;
            }
        }
    }
    return 0;
}


/*********************************************************************
 * Function:        TCPIP_STACK_NetAddressBcast(TCPIP_NET_HANDLE netH)
 *
 * PreCondition:    TCPIP stack should have been initialized by
 *                    TCPIP_STACK_Initialize()
 *
 * Input:           interface handle to get address of
 *
 * Output:          The broadcast IP address of an interface.
 *
 *
 * Side Effects:    None
 *
 * Note:           None
 *                 
 ********************************************************************/
uint32_t TCPIP_STACK_NetAddressBcast(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf)
    {
        return (pNetIf->netIPAddr.Val | ~pNetIf->netMask.Val);
    }

    return 0;


}

bool TCPIP_STACK_NetIsUp(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    return pNetIf != 0;
}

bool TCPIP_STACK_NetIsLinked(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf)
    {
        return TCPIP_STACK_NetworkIsLinked(pNetIf);
    }
    return false;
}

bool TCPIP_STACK_NetIsReady(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetLinked(netH);
    if(pNetIf)
    {
        return _TCPIPStackIsConfig(pNetIf) == 0;
    }
    return false;
}

#if defined(TCPIP_STACK_USE_IPV4)
TCPIP_STACK_DNS_SERVICE_TYPE TCPIP_STACK_DNSServiceSelect(TCPIP_NET_IF* pNetIf, TCPIP_NETWORK_CONFIG_FLAGS configFlags)
{
    // clear all the existing DNS address service bits
    pNetIf->Flags.v &= ~TCPIP_STACK_DNS_SERVICE_MASK;

    
#if defined(TCPIP_STACK_USE_DNS)
        if((configFlags & TCPIP_NETWORK_CONFIG_DNS_CLIENT_ON) != 0 )
        { 
            pNetIf->Flags.bIsDnsClientEnabled = 1;
            return TCPIP_STACK_DNS_SERVICE_CLIENT;
        }
#endif  // defined(TCPIP_STACK_USE_DNS)
    
#if defined(TCPIP_STACK_USE_DNS_SERVER)
        if((configFlags & TCPIP_NETWORK_CONFIG_DNS_SERVER_ON) != 0 )
        { 
            pNetIf->Flags.bIsDnsServerEnabled = 1;
            return TCPIP_STACK_DNS_SERVICE_SERVER;
        }
#endif  // defined(TCPIP_STACK_USE_DNS_SERVER)
    // couldn't select a DNS service
    return TCPIP_STACK_DNS_SERVICE_NONE;
}
#endif  // defined(TCPIP_STACK_USE_IPV4)


TCPIP_STACK_ADDRESS_SERVICE_TYPE TCPIP_STACK_AddressServiceSelect(TCPIP_NET_IF* pNetIf, TCPIP_NETWORK_CONFIG_FLAGS configFlags)
{
#if defined(TCPIP_STACK_USE_IPV4)

    // clear all the existing address service bits
    pNetIf->Flags.v &= ~TCPIP_STACK_ADDRESS_SERVICE_MASK;

    // Set up the address service on this interface
    // Priority (high to low): DHCPc, ZCLL, DHCPS, static IP address
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
#if (_TCPIP_STACK_RUN_TIME_INIT != 0)
    if(_TCPIPStack_ModuleIsRunning(TCPIP_MODULE_DHCP_CLIENT))
#endif  // (_TCPIP_STACK_RUN_TIME_INIT != 0)
    {
        if((configFlags & TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON) != 0 )
        { 
            pNetIf->Flags.bIsDHCPEnabled = 1;
            return TCPIP_STACK_ADDRESS_SERVICE_DHCPC;
        }
    }
#endif  // defined(TCPIP_STACK_USE_DHCP_CLIENT)

#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
#if (_TCPIP_STACK_RUN_TIME_INIT != 0)
    if(_TCPIPStack_ModuleIsRunning(TCPIP_MODULE_ZCLL))
#endif  // (_TCPIP_STACK_RUN_TIME_INIT != 0)
    {
        if((configFlags & TCPIP_NETWORK_CONFIG_ZCLL_ON) != 0 )
        { 
            pNetIf->Flags.bIsZcllEnabled = 1;
            return TCPIP_STACK_ADDRESS_SERVICE_ZCLL;
        }
    }
#endif  // defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)

#if defined(TCPIP_STACK_USE_DHCP_SERVER) || defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
#if (_TCPIP_STACK_RUN_TIME_INIT != 0)
    if(_TCPIPStack_ModuleIsRunning(TCPIP_MODULE_DHCP_SERVER))
#endif  // (_TCPIP_STACK_RUN_TIME_INIT != 0)
    {
        if((configFlags & TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON) != 0 )
        { 
            pNetIf->Flags.bIsDHCPSrvEnabled = 1;
            return TCPIP_STACK_ADDRESS_SERVICE_DHCPS;
        }
    }
#endif  // defined(TCPIP_STACK_USE_DHCP_SERVER) || defined(TCPIP_STACK_USE_DHCP_SERVER_V2)

#endif  // defined(TCPIP_STACK_USE_IPV4)
    // couldn't select an address service
    // use default/static
    return TCPIP_STACK_ADDRESS_SERVICE_NONE;

}


#if defined(TCPIP_STACK_USE_IPV4)
bool TCPIP_STACK_AddressServiceCanStart(TCPIP_NET_IF* pNetIf, TCPIP_STACK_ADDRESS_SERVICE_TYPE adSvcType)
{
    if(pNetIf)
    {   // enable a different address service only if there's not another one running
        // client has to stop a previos service (DHCP, ZCLL, etc.) in order to start another one
        return (pNetIf->Flags.v & TCPIP_STACK_ADDRESS_SERVICE_MASK) == TCPIP_STACK_ADDRESS_SERVICE_NONE;
    }

    return false;
}

TCPIP_STACK_ADDRESS_SERVICE_TYPE _TCPIPStackAddressServiceIsRunning(TCPIP_NET_IF* pNetIf)
{
    return (TCPIP_STACK_ADDRESS_SERVICE_TYPE)(pNetIf->Flags.v & TCPIP_STACK_ADDRESS_SERVICE_MASK);
}

void TCPIP_STACK_AddressServiceEvent(TCPIP_NET_IF* pNetIf, TCPIP_STACK_ADDRESS_SERVICE_TYPE adSvcType,
                                    TCPIP_STACK_ADDRESS_SERVICE_EVENT evType)
{

    if(evType == TCPIP_STACK_ADDRESS_SERVICE_EVENT_RUN_RESTORE)
    {   // run time connection restore; it should be the DHCPc
        if(adSvcType == TCPIP_STACK_ADDRESS_SERVICE_DHCPC)
        {
            // make sure any running service is cleared
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
            TCPIP_ZCLL_ServiceEnable(pNetIf, false);
#endif  // defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
            pNetIf->Flags.bIsDHCPEnabled = 1;
            return;
        }
    }
    else if(evType == TCPIP_STACK_ADDRESS_SERVICE_EVENT_CONN_LOST)
    {   // connection loss is considered a temporary event;
        // no need to disable a service
        // since we don't have network connectivity anyway
        return;
    }
    else if(adSvcType == TCPIP_STACK_ADDRESS_SERVICE_DHCPS)
    {   // if DHCP server was stopped/failed
        // we won't start another address service
        // the user will have to take a decision
        return;
    }

    // the DHCPc/ZCLL address service failed/stopped:
    // TCPIP_STACK_ADDRESS_SERVICE_EVENT_RUN_FAIL, TCPIP_STACK_ADDRESS_SERVICE_EVENT_USER_STOP
    //
    // make sure any running service is cleared
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    TCPIP_ZCLL_ServiceEnable(pNetIf, false);
#endif  // defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    pNetIf->Flags.v &= ~TCPIP_STACK_ADDRESS_SERVICE_MASK;
    _TCPIPStackSetConfig(pNetIf, true);
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    typedef bool(*addSvcFnc)(TCPIP_NET_IF* pNetIf, bool enable);
    addSvcFnc   addFnc = 0;
#endif  // defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    if(adSvcType == TCPIP_STACK_ADDRESS_SERVICE_DHCPC)
    {   // the DHCP client has been stopped or failed
        // if possible we'll select ZCLL
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
        if((pNetIf->startFlags & TCPIP_NETWORK_CONFIG_ZCLL_ON) != 0)
        {   // OK, we can use ZCLL
            addFnc = TCPIP_ZCLL_ServiceEnable;
        }
#endif
    }
    // else if (adSvcType == TCPIP_STACK_ADDRESS_SERVICE_ZCLL)
    // we'll select the default IP address

    // either way, no DHCP; restore the static DNS
    _TCPIP_StackSetDefaultDns(pNetIf);

#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    if(addFnc)
    {
        if((*addFnc)(pNetIf, true) == true)
        {   // success
            return;
        }
    }
#endif  // defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)

    // no other address service or it couldn't be started
    // select the default/static addresses
    TCPIP_STACK_AddressServiceDefaultSet(pNetIf);
}
#endif  // defined(TCPIP_STACK_USE_IPV4)

bool TCPIP_STACK_DNSServiceCanStart(TCPIP_NET_IF* pNetIf, TCPIP_STACK_DNS_SERVICE_TYPE dnsSvcType)
{
    if(pNetIf)
    {   // enable a different DNS service only if there's not another one running
        if(dnsSvcType == TCPIP_STACK_DNS_SERVICE_SERVER)
        {
            return (pNetIf->Flags.bIsDnsClientEnabled == 0);
        }
        else if(dnsSvcType == TCPIP_STACK_DNS_SERVICE_CLIENT)
        {
            return (pNetIf->Flags.bIsDnsServerEnabled == 0);
        }
    }

    return false;
}


/*********************************************************************
 * Function:        bool _InitNetConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, int nNets)
 *
 * PreCondition:    None. Part of TCPIP_STACK_Initialize()
 *
 * Input:           pUsrConfig  - pointer to user configurations
 *                  nNets       - number of networks configurations provided
 *
 * Output:          true if the parameter loading succeeded
 *                  false if failed
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
static bool _InitNetConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, int nNets)
{
    int     ix;
    TCPIP_NET_IF* pConfigIf;


    for(ix =0, pConfigIf = tcpipNetIf; ix < nNets; ix++, pConfigIf++, pUsrConfig++)
    {
        if(!_LoadNetworkConfig(pUsrConfig, pConfigIf, false))
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Default Flash Network configuration load failed %d\r\n", ix);
            return false;
        }
    }



    return true;
}


/*********************************************************************
 * Function:        bool _LoadNetworkConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, TCPIP_NET_IF* pNetIf, bool restartIf)
 *
 * PreCondition:    None
 *
 * Input:           pUsrConfig  - pointer to configurations to use
 *                  pNetIf      - network interface to default configure
 *                  restartIf   - if true this is a restart of the interface
 *
 * Output:          true if the network configuration sucessfully loaded,
 *                  false otherwise
 *
 * Side Effects:    None
 *
 * Overview:        Loads the network configuration values
 *
 * Note:            None
 ********************************************************************/
static bool _LoadNetworkConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, TCPIP_NET_IF* pNetIf, bool restartIf)
{
    int     netIfIx = 0;
    bool    loadFault;
    const void*  pMacConfig = 0;             // MAC configuration save
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    TCPIP_MAC_ADDR  oldMACAddr;
    oldMACAddr.v[0] = 0;
    oldMACAddr.v[1] = 0;
    oldMACAddr.v[2] = 0;
    oldMACAddr.v[3] = 0;
    oldMACAddr.v[4] = 0;
    oldMACAddr.v[5] = 0;
    TCPIP_NET_IF    *pPriIf = 0, *pAliasIf = 0;
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    char    oldIfName[sizeof(pNetIf->ifName) + 1];

    if(restartIf)
    {   // save old data that's still useful
        strncpy(oldIfName, pNetIf->ifName, sizeof(oldIfName) - 1); 
        pMacConfig = pNetIf->pMacConfig;
        netIfIx = pNetIf->netIfIx;
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        pPriIf = pNetIf->pPriIf; 
        pAliasIf = pNetIf->pAlias;
        if(!_TCPIPStackNetIsPrimary(pNetIf))
        {
            oldMACAddr = pNetIf->netMACAddr;
        }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    }

    while(true)
    {
        loadFault = false;
        memset(pNetIf, 0, sizeof(*pNetIf));

        if(pUsrConfig->macAddr != 0)
        {
            TCPIP_Helper_StringToMACAddress(pUsrConfig->macAddr, pNetIf->netMACAddr.v);
        }
        else
        {
            memset(pNetIf->netMACAddr.v, 0, sizeof(pNetIf->netMACAddr.v));
        }

        // store the default addresses
#if defined(TCPIP_STACK_USE_IPV4)
        TCPIP_Helper_StringToIPAddress(pUsrConfig->ipAddr, &pNetIf->DefaultIPAddr);
        TCPIP_Helper_StringToIPAddress(pUsrConfig->ipMask, &pNetIf->DefaultMask);

        TCPIP_Helper_StringToIPAddress(pUsrConfig->gateway, &pNetIf->DefaultGateway);
        TCPIP_Helper_StringToIPAddress(pUsrConfig->priDNS, &pNetIf->DefaultDNSServer[0]);
        TCPIP_Helper_StringToIPAddress(pUsrConfig->secondDNS, &pNetIf->DefaultDNSServer[1]);
#endif  // defined(TCPIP_STACK_USE_IPV4)

        if((pNetIf->pMacObj = pUsrConfig->pMacObject) == 0)
        {
            loadFault = true;       // no such MAC interface
            break;
        }

        pNetIf->macId = pNetIf->pMacObj->macId;
        pNetIf->macType = pNetIf->pMacObj->macType;
        if(pNetIf->macType == 0 || pNetIf->macType >= TCPIP_MAC_TYPES)
        {
            loadFault = true;       // no such MAC type
            break;
        } 

        // Load the NetBIOS Host Name
        memcpy(pNetIf->NetBIOSName, pUsrConfig->hostName, sizeof(tcpipNetIf[0].NetBIOSName));
        TCPIP_Helper_FormatNetBIOSName(pNetIf->NetBIOSName);


        // store start up flags
        pNetIf->startFlags = pUsrConfig->startFlags;
#if defined(TCPIP_STACK_USE_IPV6)
        if((pNetIf->startFlags & TCPIP_NETWORK_CONFIG_IPV6_ADDRESS) != 0)
        {
            bool configIPv6 = true;

            pNetIf->ipv6PrefixLen = (uint16_t)pUsrConfig->ipv6PrefixLen;

            // ignore the static IPv6 address if incorrect
            if(pUsrConfig->ipv6Addr == 0)
            {
                if(!TCPIP_Helper_StringToIPv6Address (pUsrConfig->ipv6Addr, &pNetIf->netIPv6Addr))
                {
                    configIPv6 = false;
                }
            }

            // ignore the IPv6 gateway if incorrect; gateway == 0 is allowed.
            if(pUsrConfig->ipv6Gateway != 0)
            {
                if(!TCPIP_Helper_StringToIPv6Address (pUsrConfig->ipv6Gateway, &pNetIf->netIPv6Gateway))
                {   
                    configIPv6 = false;
                }
            }
            // load the IPv6 DNS if provided
            if(pUsrConfig->ipv6Dns != NULL)
            {
                if(!TCPIP_Helper_StringToIPv6Address (pUsrConfig->ipv6Dns, &pNetIf->netIPv6Dns[0]))
                {   // incorrect DNS address
                    configIPv6 = false;
                }
                else
                {
                    pNetIf->Flags.bIpv6DnsValid = 1;
                }
            }

            if(configIPv6 == false)
            {
                pNetIf->startFlags &= ~TCPIP_NETWORK_CONFIG_IPV6_ADDRESS;
            }
        }
#endif  // defined(TCPIP_STACK_USE_IPV6)

        // Set up the address service on this interface
        _TCPIPStackSetConfig(pNetIf, true);
#if defined(TCPIP_STACK_USE_IPV4)
        TCPIP_STACK_ADDRESS_SERVICE_TYPE startAddService;
        startAddService = TCPIP_STACK_AddressServiceSelect(pNetIf, pUsrConfig->startFlags);

        if(startAddService == TCPIP_STACK_ADDRESS_SERVICE_NONE)
        {   // couldn't start an address service; use the static values supplied
            TCPIP_STACK_AddressServiceDefaultSet(pNetIf);
        }

        // set the default DNS server
        _TCPIP_StackSetDefaultDns(pNetIf);

        // Let DHCP update the DNS server
        pNetIf->Flags.bIsDNSServerAuto = 1;

        TCPIP_STACK_DNS_SERVICE_TYPE addDynamicNameService = TCPIP_STACK_DNSServiceSelect(pNetIf, pUsrConfig->startFlags);

        if(addDynamicNameService == TCPIP_STACK_DNS_SERVICE_NONE)
        {   // automatically select the DNS client as default, if possible
#if defined(TCPIP_STACK_USE_DNS)
            pNetIf->Flags.bIsDnsClientEnabled = 1;
#endif  // defined(TCPIP_STACK_USE_DNS)
        }
#else
        TCPIP_STACK_AddressServiceDefaultSet(pNetIf);
#endif  // defined(TCPIP_STACK_USE_IPV4)

        break;
    }


    if(restartIf)
    {   
        // restore the if name
        memcpy(pNetIf->ifName, oldIfName, sizeof(pNetIf->ifName)); 
        // restore MAC config data
        pNetIf->pMacConfig = pMacConfig;
        pNetIf->netIfIx = netIfIx;
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        pNetIf->pPriIf = pPriIf;
        pNetIf->pAlias = pAliasIf;
        if(!_TCPIPStackNetIsPrimary(pNetIf))
        {
            pNetIf->netMACAddr = oldMACAddr;
        }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    }
    

    return !loadFault;
}

void TCPIP_STACK_AddressServiceDefaultSet(TCPIP_NET_IF* pNetIf)
{
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    // keep access to IP addresses consistent
    _TCPIPStackSetIpAddress(pNetIf, &pNetIf->DefaultIPAddr, &pNetIf->DefaultMask, &pNetIf->DefaultGateway, false);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    _TCPIPStackSetConfig(pNetIf, false);
}


#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

// Stack external event notification support

TCPIP_EVENT TCPIP_STACK_EventsPendingGet(const void* h)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(h);
    if(pNetIf)
    {
        return TCPIP_STACK_Mac2TcpipEvent((*pNetIf->pMacObj->TCPIP_MAC_EventPendingGet)(pNetIf->hIfMac));
    }
    return TCPIP_EV_NONE;
}

#if (TCPIP_STACK_USER_NOTIFICATION != 0)
TCPIP_EVENT_HANDLE    TCPIP_STACK_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_EVENT evMask, TCPIP_STACK_EVENT_HANDLER handler, const void* hParam)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);

    if(pNetIf && _TCPIPStackNetIsPrimary(pNetIf) && handler)
    {
        TCPIP_EVENT_LIST_NODE eventNode;
        eventNode.handler = handler;
        eventNode.hParam = hParam;
        eventNode.evMask = evMask;
        eventNode.pNetIf = pNetIf;
        eventNode.next   = NULL;        

        return (TCPIP_EVENT_LIST_NODE*)TCPIP_Notification_Add(&pNetIf->registeredClients, tcpip_stack_ctrl_data.memH, &eventNode, sizeof(eventNode));
    }

    return 0;
}

bool TCPIP_STACK_HandlerDeregister(TCPIP_EVENT_HANDLE hStack)
{
    TCPIP_EVENT_LIST_NODE* pNode = (TCPIP_EVENT_LIST_NODE*)hStack;
    if(pNode)
    {
        if(TCPIP_Notification_Remove((SGL_LIST_NODE*)pNode, &pNode->pNetIf->registeredClients,tcpip_stack_ctrl_data.memH))
        {
            return true;
        }
    }

    return false;

}
#endif  // (TCPIP_STACK_USER_NOTIFICATION != 0)


#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

static const TCPIP_STACK_MODULE_CONFIG* _TCPIP_STACK_FindModuleData(TCPIP_STACK_MODULE moduleId, const TCPIP_STACK_MODULE_CONFIG* pModConfig, int nModules)
{
    while(nModules--)
    {
        if(pModConfig->moduleId == moduleId)
        {
            return pModConfig;
        }
        pModConfig++;
    }

    return 0;
}


int  TCPIP_STACK_NetIxGet(const TCPIP_NET_IF* pNetIf)
{
    if(pNetIf)
    {
        return pNetIf->netIfIx;
    }
    return -1;
}


uint32_t  TCPIP_STACK_NetAddressGet(TCPIP_NET_IF* pNetIf)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return _TCPIPStackNetAddress(pNetIf);
    }
    return 0;
}


const IPV6_ADDR* TCPIP_STACK_NetStaticIPv6AddressGet(TCPIP_NET_IF* pNetIf, int* pPrefixLen)
{
#if defined(TCPIP_STACK_USE_IPV6)
    if((pNetIf->startFlags & TCPIP_NETWORK_CONFIG_IPV6_ADDRESS) != 0)
    {
        if(pPrefixLen)
        {
            *pPrefixLen = (int)pNetIf->ipv6PrefixLen;
        }
        return &pNetIf->netIPv6Addr;
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)

    return 0;
}

const IPV6_ADDR* TCPIP_STACK_NetDefaultIPv6GatewayGet(TCPIP_NET_IF* pNetIf)
{
#if defined(TCPIP_STACK_USE_IPV6)
    if((pNetIf->startFlags & TCPIP_NETWORK_CONFIG_IPV6_ADDRESS) != 0)
    {
        return &pNetIf->netIPv6Gateway;
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)

    return 0;
}

// sets the IP addresses of the interface
// critical lock should be obtained if done in a multi-threaded system 
static void _TCPIPStackSetIpAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* ipAddress, const IPV4_ADDR* mask, const IPV4_ADDR* gw, bool setDefault)
{
    if(ipAddress)
    {
        pNetIf->netIPAddr.Val = ipAddress->Val;
        if(setDefault)
        {
            pNetIf->DefaultIPAddr.Val = ipAddress->Val;
        }
    }

    if(mask)
    {
        pNetIf->netMask.Val = mask->Val;
        if(setDefault)
        {
            pNetIf->DefaultMask.Val = mask->Val;
        }
    }

    if(gw)
    {
        pNetIf->netGateway.Val = gw->Val;
        if(setDefault)
        {
            pNetIf->DefaultGateway.Val = gw->Val;
        }
    }
}

#if defined(TCPIP_STACK_USE_IPV4)
void  _TCPIPStackSetConfigAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* ipAddress, const IPV4_ADDR* mask, const IPV4_ADDR* gw, bool config)
{
    if(pNetIf)
    {
        OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        // keep access to IP addresses consistent
        _TCPIPStackSetIpAddress(pNetIf, ipAddress, mask, gw, false);
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
        _TCPIPStackSetConfig(pNetIf, config);
    }
}
#endif  // defined(TCPIP_STACK_USE_IPV4)

// signal modules about the interface change
// to keep it short send signal to the 1st layer modules only
// they will have to propagate to their children!
// also, no user notification done here; this needs to be quick
//  - if change was done by DHCP - this one has its own notification mechanism
//  - if change was done by the user - no need to alert user probably
//  - anyway, layer 2 modules can alert the users as well
#if (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
void _TCPIPStackSetConfig(TCPIP_NET_IF* pNetIf, bool config)
{
    int ix;
    const uint16_t* pModIx;
    TCPIP_MODULE_SIGNAL signal;
    uint16_t    sigParam = (uint16_t)(1 << pNetIf->netIfIx);

    pNetIf->Flags.bInConfig = config;

    if(config == false)
    {   // we signal when ready
        OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        pModIx = TCPIP_STACK_MODULE_SIGNAL_CHANGE_TBL;
        for(ix = 0; ix < sizeof(TCPIP_STACK_MODULE_SIGNAL_CHANGE_TBL) / sizeof(*TCPIP_STACK_MODULE_SIGNAL_CHANGE_TBL); ix++, pModIx++)
        {
            TCPIP_MODULE_SIGNAL_ENTRY*  pSigEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + *pModIx;
            pSigEntry->signalVal |= (uint16_t)TCPIP_MODULE_SIGNAL_INTERFACE_CHANGE;
            pSigEntry->signalParam |= sigParam;
        }
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    }

    // call the external noftification
    signal = config == false ? TCPIP_MODULE_SIGNAL_INTERFACE_CHANGE : TCPIP_MODULE_SIGNAL_INTERFACE_CONFIG;
    pModIx = TCPIP_STACK_MODULE_SIGNAL_CONFIG_TBL;
    for(ix = 0; ix < sizeof(TCPIP_STACK_MODULE_SIGNAL_CONFIG_TBL) / sizeof(*TCPIP_STACK_MODULE_SIGNAL_CONFIG_TBL); ix++, pModIx++)
    {
        TCPIP_MODULE_SIGNAL_ENTRY*  pSigEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + *pModIx;
        _TCPIPSignalEntryNotify(pSigEntry, signal, sigParam);
    }
}
#endif  // (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

uint32_t  TCPIP_STACK_NetMaskGet(TCPIP_NET_IF* pNetIf)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return pNetIf->netMask.Val;
    }
    return 0;
}

void  TCPIP_STACK_GatewayAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress)
{
    if(pNetIf)
    {
        pNetIf->netGateway.Val = ipAddress->Val;
    }
}
void  TCPIP_STACK_PrimaryDNSAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress)
{
    if(pNetIf)
    {
        pNetIf->dnsServer[0].Val = ipAddress->Val;
    }
}

void  TCPIP_STACK_SecondaryDNSAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress)
{
    if(pNetIf)
    {
        pNetIf->dnsServer[1].Val = ipAddress->Val;
    }
}


bool  TCPIP_STACK_AddressIsOfNetUp( TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return TCPIP_STACK_AddressIsOfNet(pNetIf, pIpAdd);
    }
    return false;
}

// detects net-directed bcast
bool  TCPIP_STACK_NetIsBcastAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
       return (pIpAdd->Val == (pNetIf->netIPAddr.Val | ~pNetIf->netMask.Val));
    }
    return false;
}

// detects limited or net-directed bcast
bool  TCPIP_STACK_IsBcastAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
       return (TCPIP_Helper_IsBcastAddress(pIpAdd) ||  TCPIP_STACK_NetIsBcastAddress(pNetIf, pIpAdd));
    }
    return false;
}

bool TCPIP_STACK_NetworkIsLinked(TCPIP_NET_IF* pNetIf)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return pNetIf->exFlags.linkPrev;
    }

    return false;
}

// checks for valid up and linked interface
TCPIP_NET_IF* _TCPIPStackHandleToNetLinked(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    if(pNetIf != 0 && pNetIf->exFlags.linkPrev != 0)
    {
        return pNetIf;
    }
    
    return 0;
}

TCPIP_NET_IF* _TCPIPStackAnyNetLinked(bool useDefault)
{
    int netIx;
    TCPIP_NET_IF* pNetIf = 0;

    if(useDefault)
    {
        pNetIf = _TCPIPStackHandleToNetLinked(tcpipDefIf.defaultNet);
    }

    if(pNetIf == 0)
    {
        TCPIP_NET_IF* pIf;

        for(netIx = 0, pIf = tcpipNetIf ; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pIf++)
        {
            if(pIf->Flags.bInterfaceEnabled && pIf->exFlags.linkPrev != 0)
            {   // found linked interface
                pNetIf = pIf;
                break;
            }
        }
    }

    return pNetIf;
}


// returns the interface for which this address is a net_directed address
// 0 in not found
// does NOT check network up, linked, etc.
// does NOT check primary/virtual interface!
TCPIP_NET_IF* _TCPIPStackAnyNetDirected(const IPV4_ADDR* pIpAdd)
{
    int netIx;

    TCPIP_NET_IF* pIf;

    for(netIx = 0, pIf = tcpipNetIf ; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pIf++)
    {
        if(_TCPIPStack_IsDirectedBcast(pIf, pIpAdd))
        {
            return pIf;
        }
    }

    return 0;
}

TCPIP_STACK_MODULE  TCPIP_STACK_NetMACId(TCPIP_NET_IF* pNetIf)
{
    return pNetIf ? pNetIf->macId:(TCPIP_STACK_MODULE)TCPIP_MODULE_MAC_NONE;
}


 TCPIP_MAC_HANDLE  TCPIP_STACK_NetToMAC(TCPIP_NET_IF* pNetIf)
{
    return pNetIf ? pNetIf->hIfMac : 0;
}



const uint8_t*  TCPIP_STACK_NetUpMACAddressGet(TCPIP_NET_IF* pNetIf)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return pNetIf->netMACAddr.v;
    }

    return 0;
}

static TCPIP_MAC_ACTION TCPIP_STACK_StackToMacAction(TCPIP_STACK_ACTION action)
{   // TCPIP_MAC_ACTION and TCPIP_STACK_ACTION should be kept in sync!
    return (TCPIP_MAC_ACTION)action;
}

static void TCPIP_STACK_StacktoMacCtrl(TCPIP_MAC_MODULE_CTRL* pMacCtrl, TCPIP_STACK_MODULE_CTRL* stackCtrlData)
{
    TCPIP_NET_IF* pNetIf = stackCtrlData->pNetIf;


    memset(pMacCtrl, 0, sizeof(*pMacCtrl));

    pMacCtrl->nIfs = stackCtrlData->nIfs;

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    pMacCtrl->mallocF = (TCPIP_MAC_HEAP_MallocF)TCPIP_HEAP_MallocDebug;
    pMacCtrl->callocF = (TCPIP_MAC_HEAP_CallocF)TCPIP_HEAP_CallocDebug;
    pMacCtrl->freeF = (TCPIP_MAC_HEAP_FreeF)TCPIP_HEAP_FreeDebug;
#else
    pMacCtrl->mallocF = TCPIP_HEAP_MallocOutline;
    pMacCtrl->callocF = TCPIP_HEAP_CallocOutline;
    pMacCtrl->freeF = TCPIP_HEAP_FreeOutline;
#endif // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    
    pMacCtrl->memH = stackCtrlData->memH;


    pMacCtrl->pktAllocF = (TCPIP_MAC_PKT_AllocF)_TCPIP_PKT_ALLOC_FNC;
    pMacCtrl->pktFreeF = (TCPIP_MAC_PKT_FreeF)_TCPIP_PKT_FREE_FNC;
    pMacCtrl->pktAckF = (TCPIP_MAC_PKT_AckF)_TCPIP_PKT_ACK_FNC;

    pMacCtrl->synchF = _TCPIP_StackSyncFunction;

#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
    // Stack can use one handler for all network interfaces, like in this case
    // Each time a notification is received, all interfaces are checked
    // Or, more efficient, use a handler per interface
    pMacCtrl->eventF = _TCPIP_MacEventCB;
    pMacCtrl->eventParam = pNetIf;
#else
    pMacCtrl->eventF = 0;
    pMacCtrl->eventParam = 0;
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

    pMacCtrl->netIx = stackCtrlData->netIx;
    pMacCtrl->gapDcptOffset = TCPIP_PKT_GapDcptOffset();
    pMacCtrl->gapDcptSize = TCPIP_PKT_GapDcptSize();
    pMacCtrl->macAction = TCPIP_STACK_StackToMacAction(stackCtrlData->stackAction);
    pMacCtrl->powerMode = stackCtrlData->powerMode;
    pMacCtrl->controlFlags = TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2;
    memcpy(pMacCtrl->ifPhyAddress.v, pNetIf->netMACAddr.v, sizeof(pMacCtrl->ifPhyAddress));
}

// returns true if the stack needs processing time because of events
static bool TCPIP_STACK_CheckEventsPending(void)
{
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
    return (newTcpipTickAvlbl != 0 || totTcpipEventsCnt != 0);
#else
    // fake pending events
    totTcpipEventsCnt++;
    return true;
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
}


tcpipSignalHandle _TCPIPStackSignalHandlerRegister(TCPIP_STACK_MODULE modId, tcpipModuleSignalHandler signalHandler, int16_t asyncTmoMs)
{

    if(signalHandler != 0 )
    {
        if(TCPIP_MODULE_LAYER1 <= modId && modId < sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL)/sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL))
        {
            TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + modId;
            if(pSignalEntry->signalHandler == 0 || pSignalEntry->signalHandler == signalHandler)
            {   // found module slot
                pSignalEntry->signalHandler = signalHandler;
                if ((asyncTmoMs != 0) && (asyncTmoMs < stackTaskRate))
                {
                    asyncTmoMs = stackTaskRate;
                }
                pSignalEntry->asyncTmo = pSignalEntry->currTmo = asyncTmoMs;
                return pSignalEntry;
            }
        }
    }

    return 0;
}


bool _TCPIPStackSignalHandlerSetParams(TCPIP_STACK_MODULE modId, tcpipSignalHandle handle, int16_t asyncTmoMs)
{
    TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = (TCPIP_MODULE_SIGNAL_ENTRY*)handle;
    if((pSignalEntry = (TCPIP_MODULE_SIGNAL_ENTRY*)handle) != 0 && pSignalEntry->signalHandler != 0)
    {   // minimum sanity check
        if ((asyncTmoMs != 0) && (asyncTmoMs < stackTaskRate))
        {
            asyncTmoMs = stackTaskRate;
        }
        pSignalEntry->asyncTmo = pSignalEntry->currTmo = asyncTmoMs;
        return true;
    }

    return false;
}

// de-registers a previous registered timeout handler
void _TCPIPStackSignalHandlerDeregister(tcpipSignalHandle handle)
{
    TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry;
    if((pSignalEntry = (TCPIP_MODULE_SIGNAL_ENTRY*)handle) != 0)
    {
        memset(pSignalEntry, 0x0, sizeof(*pSignalEntry));
    }
}

// used by stack modules
TCPIP_MODULE_SIGNAL  _TCPIPStackModuleSignalGet(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL clrMask)
{

    TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + modId;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_MODULE_SIGNAL modSignal = pSignalEntry->signalVal;
    pSignalEntry->signalVal &= (uint16_t)(~clrMask);
    if((clrMask & TCPIP_MODULE_SIGNAL_ASYNC) != 0)
    {
        stackAsyncSignalCount -= 1; 
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return modSignal;
}

// gets the associated parmeter too
// returns the pending module signals + signal parameter
// signalParam should NOT be 0!
// the signal parameter is cleared after this operation, there's is no peek!
#if (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
TCPIP_MODULE_SIGNAL  _TCPIPStackModuleSignalParamGet(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL clrMask, uint32_t* signalParam)
{

    TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + modId;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_MODULE_SIGNAL modSignal = pSignalEntry->signalVal;
    *signalParam = pSignalEntry->signalParam;
    pSignalEntry->signalVal &= (uint16_t)(~clrMask);
    if((clrMask & TCPIP_MODULE_SIGNAL_ASYNC) != 0)
    {
        stackAsyncSignalCount -= 1; 
    }
    pSignalEntry->signalParam = 0;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return modSignal;

}
#endif  // (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

// used by manager!
// TMO and RX_PENDING signals only!
static TCPIP_MODULE_SIGNAL  _TCPIPStackManagerSignalClear(TCPIP_MODULE_SIGNAL clrMask)
{
    TCPIP_MODULE_SIGNAL tmoMask;
    TCPIP_MODULE_SIGNAL rxMask;
    TCPIP_MODULE_SIGNAL mgrSignal;
    TCPIP_MODULE_SIGNAL_ENTRY* pMgrEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + TCPIP_MODULE_MANAGER;
    TCPIP_MODULE_SIGNAL_ENTRY* pTmoEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + TCPIP_MODULE_NONE;


    if(clrMask == 0)
    {   // read only needed
        return pMgrEntry->signalVal | pTmoEntry->signalVal;
    }


    tmoMask = clrMask & TCPIP_MODULE_SIGNAL_TMO;
    rxMask = clrMask & TCPIP_MODULE_SIGNAL_RX_PENDING;


    // protect against ISRs (MAC + TMR) and other threads too!
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
    mgrSignal = pMgrEntry->signalVal;
    mgrSignal |= pTmoEntry->signalVal;
    if(tmoMask)
    {
        pTmoEntry->signalVal &= (uint16_t)(~TCPIP_MODULE_SIGNAL_TMO);
    }
    if(rxMask)
    {
        pMgrEntry->signalVal &= (uint16_t)(~TCPIP_MODULE_SIGNAL_RX_PENDING);
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, critSect);

    return mgrSignal;
}

// coming from user threads if the module tasks run in user space
static  void* _TCPIPSignalEntrySet(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signal)
{
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    pSigEntry->signalVal |= (uint16_t)signal;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return pSigEntry;
}

// a stack module requests setting a signal
// allows setting a message to the manager itself, if needed
// coming from user threads if the module tasks run in user space
//
// a stack module requests setting a signal
// This function is mainly intended for requiring attention
// and the signal to be set is TCPIP_MODULE_SIGNAL_ASYNC!
// There is no need for a module to set other signals if there's really
// no pending RX packets or timeouts.
//
// NOTE:
//  a TCPIP_MODULE_SIGNAL_ASYNC is monitored by the stack manager so that as long as it is active
//  the module gets special attention.
//  This is done to allow a module to set the TCPIP_MODULE_SIGNAL_ASYNC signal just once
//  instead of continuously request it.
//  When done with the critical section the module should clear the TCPIP_MODULE_SIGNAL_ASYNC signal.
//  However, the stack manager keeps a global count of how many times the modules required TCPIP_MODULE_SIGNAL_ASYNC
//  That means that a module should always call
//  _TCPIPStackModuleSignalRequest/_TCPIPStackModuleSignalGet for TCPIP_MODULE_SIGNAL_ASYNC in pairs!
//
// NOTE:
//  A signal request sends a signal to the module task itself
//  A signal request will also send a signal to the stack task itself, when the TCP/IP modules are executed internally
//  - unless noMgrAlert is specified
//
bool _TCPIPStackModuleSignalRequest(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL signal, bool noMgrAlert)
{
    if(modId <= TCPIP_MODULE_MANAGER)
    {   // cannot request signals to manager itself
        return false;
    }

    _TCPIPModuleSignalSetNotify(modId, signal);

    if((signal & TCPIP_MODULE_SIGNAL_ASYNC) != 0)
    {
        _TCPIPAsyncSignalInc();
    }

#if !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)
    if(noMgrAlert == 0)
    {
        _TCPIPSignalEntryNotify(_TCPIPModuleToSignalEntry(TCPIP_MODULE_MANAGER), signal, 0);
    }
#endif  //  !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)

    return true;
}

static void _TCPIPSignalEntryNotify(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signal, uint32_t sigParam)
{
    TCPIP_MODULE_SIGNAL_FUNC userF;

    if((userF = pSigEntry->userSignalF) != 0)
    {
        (*userF)(pSigEntry, pSigEntry - TCPIP_STACK_MODULE_SIGNAL_TBL, signal, sigParam);
    }
}

static void _TCPIPSignalEntrySetNotify(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signals, uint32_t sigParam)
{
    _TCPIPSignalEntrySet(pSigEntry, signals);
    _TCPIPSignalEntryNotify(pSigEntry, signals, sigParam);
}

// signal the stack manager maintained timeout
static void _TCPIPStackSignalTmo(void)
{
    int     ix;
    TCPIP_MODULE_SIGNAL_ENTRY*  pSigEntry;

    pSigEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + TCPIP_MODULE_LAYER1;
    for(ix = TCPIP_MODULE_LAYER1; ix < sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL)/sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL); ix++, pSigEntry++)
    {
        if(pSigEntry->signalHandler == 0 || pSigEntry->asyncTmo == 0)
        {   // unused slot
            continue;
        }

        if((pSigEntry->currTmo -= stackTaskRate) <= 0)
        {   // timeout: send a signal to this module
            pSigEntry->currTmo += pSigEntry->asyncTmo;
            _TCPIPSignalEntrySetNotify(pSigEntry, TCPIP_MODULE_SIGNAL_TMO, 0); 
        }
    }
}

// insert a packet into a module RX queue
// signal should be false when modId == TCPIP_MODULE_MANAGER !
bool _TCPIPStackModuleRxInsert(TCPIP_STACK_MODULE modId, TCPIP_MAC_PACKET* pRxPkt, bool signal)
{
#if (_TCPIP_STACK_RUN_TIME_INIT != 0)
    TCPIP_MODULE_RUN_DCPT* pRDcpt = TCPIP_MODULES_RUN_TBL + modId;
    if(pRDcpt->isRunning == 0)
    {
        return false;
    }
#endif  // (_TCPIP_STACK_RUN_TIME_INIT != 0)

    SINGLE_LIST* pQueue = TCPIP_MODULES_QUEUE_TBL + modId;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_Helper_SingleListTailAdd(pQueue, (SGL_LIST_NODE*)pRxPkt);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

    if(signal)
    {
        _TCPIPModuleSignalSetNotify(modId, TCPIP_MODULE_SIGNAL_RX_PENDING);
    }
    return true;
}

//
// extracts a packet from a module RX queue
// returns 0 if queue is empty
TCPIP_MAC_PACKET* _TCPIPStackModuleRxExtract(TCPIP_STACK_MODULE modId)
{
    SINGLE_LIST* pQueue = TCPIP_MODULES_QUEUE_TBL + modId;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_MAC_PACKET* pRxPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_SingleListHeadRemove(pQueue);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return pRxPkt;
}

// purges the packets from a module RX queue
// belonging to the pNetIf
void _TCPIPStackModuleRxPurge(TCPIP_STACK_MODULE modId, TCPIP_NET_IF* pNetIf)
{
    TCPIP_MAC_PACKET* pRxPkt;
    SINGLE_LIST       remList = { 0 };  // list of packets to remove/ack
    SINGLE_LIST       keepList = { 0 };  // list of packets to keep

    SINGLE_LIST*      pRxQueue = (TCPIP_MODULES_QUEUE_TBL + modId); // list to extract from
    
    // kill the list
    while(true)
    {
        OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        pRxPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_SingleListHeadRemove(pRxQueue);
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
        if(pRxPkt == 0)
        {
            break;
        }


        if(pRxPkt->pktIf == pNetIf)
        {   // need to remove
            TCPIP_Helper_SingleListTailAdd(&remList, (SGL_LIST_NODE*)pRxPkt);
        }
        else
        {   // need to keep
            TCPIP_Helper_SingleListTailAdd(&keepList, (SGL_LIST_NODE*)pRxPkt);
        }
    }

    // acknowledge the removed packets
    while((pRxPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_SingleListHeadRemove(&remList)) != 0)
    {
        TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_SOURCE_ERR);
    }

    if(!TCPIP_Helper_SingleListIsEmpty(&keepList))
    {   // replace the remaining packets
        OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        TCPIP_Helper_SingleListAppend(pRxQueue, &keepList);
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    }

}


// inserts a RX packet into the manager RX queue
// this has to be a fully formatted TCPIP_MAC_PACKET
void _TCPIPStackInsertRxPacket(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, bool signal)
{
    pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
    // update the frame length
    pRxPkt->pDSeg->segLen -= sizeof(TCPIP_MAC_ETHERNET_HEADER);
    _TCPIPInsertMacRxPacket(pNetIf, pRxPkt);

    if(signal)
    {
        _TCPIP_NetIfEvent(_TCPIPStackNetGetPrimary(pNetIf), TCPIP_MAC_EV_RX_DONE, true);
    }
}

TCPIP_MAC_RES _TCPIPStackPacketTx(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET * ptrPacket)
{
    TCPIP_MAC_RES res;

    TCPIP_PKT_FlightLogTx(ptrPacket, TCPIP_THIS_MODULE_ID);
    TCPIP_PKT_FlightLogTx(ptrPacket, pNetIf->macId);    // MAC doesn't call the log function

    if(pNetIf->hIfMac != 0)
    {
        res = pNetIf->pMacObj->TCPIP_MAC_PacketTx(pNetIf->hIfMac, ptrPacket);
        // stack should always use well formatted packets!
        _TCPIPStack_Assert(res >= 0, __FILE__, __func__, __LINE__);
    }
    else
    {
        res = TCPIP_MAC_RES_NOT_READY_ERR;
    }

    return res;
}

static bool _TCPIP_StackSyncFunction(void* synchHandle, TCPIP_MAC_SYNCH_REQUEST req)
{
    switch(req)
    {
        case TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE: 
            return (OSAL_SEM_Create((OSAL_SEM_HANDLE_TYPE*)synchHandle, OSAL_SEM_TYPE_BINARY, 1, 1) == OSAL_RESULT_TRUE) ? true : false;

        case TCPIP_MAC_SYNCH_REQUEST_OBJ_DELETE: 
            return (OSAL_SEM_Delete((OSAL_SEM_HANDLE_TYPE*)synchHandle) == OSAL_RESULT_TRUE) ? true : false;

        case TCPIP_MAC_SYNCH_REQUEST_OBJ_LOCK: 
            return (OSAL_SEM_Pend((OSAL_SEM_HANDLE_TYPE*)synchHandle, OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE) ? true: false;

        case TCPIP_MAC_SYNCH_REQUEST_OBJ_UNLOCK: 
            return (OSAL_SEM_Post((OSAL_SEM_HANDLE_TYPE*)synchHandle) == OSAL_RESULT_TRUE) ? true: false;
            
        case TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER:
            *(OSAL_CRITSECT_DATA_TYPE*)synchHandle =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
            return true; 

        case TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE:
            OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, *(OSAL_CRITSECT_DATA_TYPE*)synchHandle);
            return true; 

        default:
            return false;
    }

}

unsigned int TCPIP_STACK_VersionGet ( const SYS_MODULE_INDEX index )
{
    return( ( TCPIP_STACK_VERSION_MAJOR * 10000 ) +
            ( TCPIP_STACK_VERSION_MINOR * 100 ) +
            ( TCPIP_STACK_VERSION_PATCH ) );

}


const char * TCPIP_STACK_VersionStrGet ( const SYS_MODULE_INDEX index )
{
    return TCPIP_STACK_VERSION_STR;
}



// this happens in user space
TCPIP_MODULE_SIGNAL_HANDLE TCPIP_MODULE_SignalFunctionRegister(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL_FUNC signalF)
{
    TCPIP_MODULE_SIGNAL_ENTRY* pEntry = 0;

    if(signalF != 0 )
    {
        if(0 < modId && modId < sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL)/sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL))
        {
            TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + modId;
            OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
            if(pSignalEntry->userSignalF == 0)
            {   // found module slot
                pSignalEntry->userSignalF = signalF;
                pEntry = pSignalEntry;
            }
            OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
        }
    }

    return pEntry;
}

// this happens in user space
// however it should be an atomic access
bool TCPIP_MODULE_SignalFunctionDeregister(TCPIP_MODULE_SIGNAL_HANDLE signalHandle)
{
    TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = (TCPIP_MODULE_SIGNAL_ENTRY*)signalHandle;
    if(pSignalEntry)
    {
        pSignalEntry->userSignalF = 0;
        return true;
    }

    return false;
}


// from user space
// read only atomic access
TCPIP_MODULE_SIGNAL TCPIP_MODULE_SignalGet(TCPIP_STACK_MODULE modId)
{
    if(modId == TCPIP_MODULE_MANAGER)
    {
        return _TCPIPStackManagerSignalClear(0);
    }
    else if(0 < modId && modId < sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL)/sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL))
    {
        TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + modId;
        return pSignalEntry->signalVal;
    }

    return TCPIP_MODULE_SIGNAL_NONE; 
}

#if (_TCPIP_STACK_RUN_TIME_INIT != 0)
bool TCPIP_MODULE_Deinitialize(TCPIP_STACK_MODULE moduleId)
{
    size_t  netIx, entryIx;
    TCPIP_NET_IF   *pNetIf;
    TCPIP_STACK_MODULE_CTRL stackCtrlData;        
    const TCPIP_STACK_MODULE_ENTRY  *pEntry, *pModEntry;

    if(tcpipNetIf != 0)
    {
        if(1 < moduleId && moduleId < sizeof(TCPIP_MODULES_RUN_TBL) / sizeof(*TCPIP_MODULES_RUN_TBL))
        {
            TCPIP_MODULE_RUN_DCPT* pRDcpt = TCPIP_MODULES_RUN_TBL + moduleId;
            if(pRDcpt->isRunning != 0)
            {   // this module up and running
                //
                //
                stackCtrlData.stackAction = TCPIP_STACK_ACTION_DEINIT;
                stackCtrlData.powerMode = TCPIP_MAC_POWER_NONE;
                // find the corresponding module entry
                pModEntry = 0;
                pEntry = TCPIP_STACK_MODULE_ENTRY_TBL;
                for(entryIx = 0; entryIx < sizeof(TCPIP_STACK_MODULE_ENTRY_TBL)/sizeof(*TCPIP_STACK_MODULE_ENTRY_TBL); entryIx++, pEntry++)
                {
                    if(pEntry->moduleId == moduleId)
                    {   // found entry
                        pModEntry = pEntry;
                        break;
                    }
                }

                if(pModEntry != 0)
                {
                    for(netIx = 0, pNetIf = tcpipNetIf; netIx < tcpip_stack_ctrl_data.nIfs; netIx++, pNetIf++)
                    {
                        stackCtrlData.pNetIf = pNetIf;
                        stackCtrlData.netIx = pNetIf->netIfIx;
                        pEntry->deInitFunc(&stackCtrlData);
                    }

                    pRDcpt->isRunning = 0;
                    return true;
                }
            }
        }
    }

    return false;
}

bool TCPIP_MODULE_IsRunning(TCPIP_STACK_MODULE moduleId)
{
    if(tcpipNetIf != 0)
    {
        if(1 < moduleId && moduleId < sizeof(TCPIP_MODULES_RUN_TBL) / sizeof(*TCPIP_MODULES_RUN_TBL))
        {
            TCPIP_MODULE_RUN_DCPT* pRDcpt = TCPIP_MODULES_RUN_TBL + moduleId;
            return (pRDcpt->isRunning != 0);
        }
    }

    return false;
}
#endif  // (_TCPIP_STACK_RUN_TIME_INIT != 0)

TCPIP_STACK_HEAP_HANDLE TCPIP_STACK_HeapHandleGet(TCPIP_STACK_HEAP_TYPE heapType, int heapIndex)
{
    return (heapType == tcpip_stack_ctrl_data.heapType) ? tcpip_stack_ctrl_data.memH : 0;
}

size_t TCPIP_STACK_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return heapH ? TCPIP_HEAP_Size(heapH) : 0;
}


size_t TCPIP_STACK_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return heapH ? TCPIP_HEAP_MaxSize(heapH) : 0;
}



size_t TCPIP_STACK_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return heapH ? TCPIP_HEAP_FreeSize(heapH) : 0;
}

size_t TCPIP_STACK_HEAP_HighWatermark(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return heapH ? TCPIP_HEAP_HighWatermark(heapH) : 0;
}


TCPIP_STACK_HEAP_RES TCPIP_STACK_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return heapH ? TCPIP_HEAP_LastError(heapH) : TCPIP_STACK_HEAP_RES_NO_HEAP; 
}

// return the stack heap configuration parameters
const TCPIP_STACK_HEAP_CONFIG* _TCPIPStackHeapConfig(void)
{
    return &tcpip_heap_config;
}

TCPIP_NETWORK_TYPE TCPIP_STACK_NetGetType(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(hNet);

    if(pNetIf != 0)
    {
        return _TCPIPStackNetIsPrimary(pNetIf) ? TCPIP_NETWORK_TYPE_PRIMARY : TCPIP_NETWORK_TYPE_ALIAS;
    }

    return TCPIP_NETWORK_TYPE_UNKNOWN; 
}

// maps the proper alias network a packet belongs to
// based on the interface the packet arrived on
// (probably a primary interface) and on the packet IP address
// No check is done if the interface (p or v) is UP!
//
// Note: if limited broadcast we cannot replicate on all alias interfaces.
// So, the incoming, primary interface is selected.
// Net directed broadcast should be used, and is preferred!
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
TCPIP_NET_IF* _TCPIPStackMapAliasInterface(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pDestAddress)
{
    TCPIP_NET_IF    *pAliasIf;

    if(!TCPIP_Helper_IsBcastAddress(pDestAddress))
    {
        // browse the aliases belonging to this primary!
        for(pAliasIf = _TCPIPStackNetGetPrimary(pNetIf); pAliasIf != 0; pAliasIf = _TCPIPStackNetGetAlias(pAliasIf))
        {
            if(TCPIP_STACK_AddressIsOfNet(pAliasIf, pDestAddress))
            {
                return pAliasIf;
            }
        }
    }

    // no mapping; return the original interface
    return pNetIf;
}


// copies the alias interface MAC settings from the primary interface
static void _TCPIPCopyMacAliasIf(TCPIP_NET_IF* pAliasIf, TCPIP_NET_IF* pPriIf)
{
    pAliasIf->macType = pPriIf->macType;
    pAliasIf->netMACAddr = pPriIf->netMACAddr;
    pAliasIf->macObjHandle = pPriIf->macObjHandle;
    pAliasIf->hIfMac = pPriIf->hIfMac;
    pAliasIf->pMacConfig = pPriIf->pMacConfig;
    pAliasIf->linkMtu = pPriIf->linkMtu;
}

#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

// external packet processing
#if (TCPIP_STACK_EXTERN_PACKET_PROCESS != 0)
TCPIP_STACK_PROCESS_HANDLE TCPIP_STACK_PacketHandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_STACK_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    TCPIP_STACK_PROCESS_HANDLE pHandle = 0;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);

    if(pNetIf != 0 && pNetIf->pktHandler == 0)
    {
        pNetIf->pktHandlerParam = handlerParam;
        pNetIf->pktHandler = pktHandler;
        pHandle = pktHandler;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return pHandle;
}

bool TCPIP_STACK_PacketHandlerDeregister(TCPIP_NET_HANDLE hNet, TCPIP_STACK_PROCESS_HANDLE pktHandle)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(hNet);

    if(pNetIf != 0 && pNetIf->pktHandler == pktHandle)
    {
        pNetIf->pktHandler = 0;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

#endif  // (TCPIP_STACK_EXTERN_PACKET_PROCESS != 0)

// run time module initialization
#if (_TCPIP_STACK_RUN_TIME_INIT != 0)
bool _TCPIPStack_ModuleIsRunning(TCPIP_STACK_MODULE moduleId)
{
    if(0 < moduleId && moduleId < sizeof(TCPIP_MODULES_RUN_TBL) / sizeof(*TCPIP_MODULES_RUN_TBL))
    {
        TCPIP_MODULE_RUN_DCPT* pRDcpt = TCPIP_MODULES_RUN_TBL + moduleId;
        return pRDcpt->isRunning != 0;
    }
    return false;
}
#endif  // (_TCPIP_STACK_RUN_TIME_INIT != 0)

// local time keeping
// reverts to a sys service, when available

static void _TCPIP_SecondCountSet(void)
{
    uint32_t tmrFreq = SYS_TIME_FrequencyGet();
    // use a 64 bit count to avoid roll over
    uint64_t tmrCount = SYS_TIME_Counter64Get();

    _tcpip_SecCount = tmrCount / tmrFreq; 
    _tcpip_MsecCount = tmrCount / (tmrFreq / 1000); 
}

uint32_t _TCPIP_SecCountGet(void)
{
    return _tcpip_SecCount;
}

uint32_t _TCPIP_MsecCountGet(void)
{
    return _tcpip_MsecCount;
}


// debugging features
//
#if defined(TCPIP_STACK_TIME_MEASUREMENT)
#include <cp0defs.h>

void TCPIP_STACK_TimeMeasureStart(bool reset)
{
    if(reset)
    {
        tcpip_stack_time = 0;
    }

    tcpip_stack_timeEnable = true;
}

uint64_t TCPIP_STACK_TimeMeasureGet(bool stop)
{
    if(stop)
    {
        tcpip_stack_timeEnable = false;
    }

    return tcpip_stack_time;
}

#endif // defined(TCPIP_STACK_TIME_MEASUREMENT)

#if ((_TCPIP_STACK_DEBUG_LEVEL & _TCPIP_STACK_DEBUG_MASK_BASIC) != 0)
#if (_TCPIP_STACK_ENABLE_ASSERT_LOOP != 0)
volatile int _TCPIP_Stack_LeaveAssertLoop = 0;
#endif  // (_TCPIP_STACK_ENABLE_ASSERT_LOOP != 0)
void _TCPIPStack_Assert(bool cond, const char* fileName, const char* funcName, int lineNo)
{
    if(cond == false)
    {
        // remove the path from the fileName
        const char* lastPath = strrchr(fileName, '/');
        if(lastPath == 0)
        {   // try windows style
            lastPath = strrchr(fileName, '\\');
        }
        if(lastPath != 0)
        {
            lastPath++;
        }
        else
        {   // no luck
            lastPath = fileName;
        }
        SYS_CONSOLE_PRINT("TCPIP Stack Assert: in file: %s, func: %s, line: %d, \r\n", lastPath, funcName, lineNo);
#if (_TCPIP_STACK_ENABLE_ASSERT_LOOP != 0)
        while(_TCPIP_Stack_LeaveAssertLoop == 0);
#endif  // (_TCPIP_STACK_ENABLE_ASSERT_LOOP != 0)
    }
}

volatile int _TCPIP_Stack_LeaveCondLoop = 0;
void _TCPIPStack_Condition(bool cond, const char* fileName, const char* funcName, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("TCPIP Stack Cond: in file: %s, func: %s, line: %d, \r\n", fileName, funcName, lineNo);
#if (_TCPIP_STACK_ENABLE_COND_LOOP != 0)
        while(_TCPIP_Stack_LeaveCondLoop == 0);
#endif // (_TCPIP_STACK_ENABLE_COND_LOOP != 0)
    }
}
#endif  // ((_TCPIP_STACK_DEBUG_LEVEL & _TCPIP_STACK_DEBUG_MASK_BASIC) != 0)

