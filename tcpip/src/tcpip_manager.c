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
#define TCPIP_STACK_MAC_ALL_EVENTS          ((uint32_t)TCPIP_MAC_EV_RX_DONE | (uint32_t)TCPIP_MAC_EV_TX_DONE | (uint32_t)TCPIP_MAC_EV_RXTX_ERRORS)

// MAC events used by the stack manager to detect that
// there are active RX events that need processing
// always subset of TCPIP_STACK_MAC_ALL_EVENTS
#define TCPIP_STACK_MAC_ACTIVE_RX_EVENTS    ((uint32_t)TCPIP_MAC_EV_RX_DONE | (uint32_t)TCPIP_MAC_EV_RX_OVFLOW | (uint32_t)TCPIP_MAC_EV_RX_BUFNA)


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
#if (M_TCPIP_STACK_RUN_TIME_INIT == 0)
static const tcpipModuleConnHandler  TCPIP_STACK_CONN_EVENT_TBL [] =
{
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)
    &TCPIP_DHCP_ConnectionHandler,
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)

#if defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
    &TCPIP_DHCPV6_ConnectionHandler,
#endif  // defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)

    // add other needed handlers here
};
#else
static void F_TCPIP_DHCP_RunConnectionHandler(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT connEvent)
{
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)
    if(TCPIPStack_ModuleIsRunning(TCPIP_MODULE_DHCP_CLIENT))
    {
        TCPIP_DHCP_ConnectionHandler(pNetIf, connEvent);
    }   
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)
}

static void F_TCPIP_DHCPV6_RunConnectionHandler(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT connEvent)
{
#if defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
    if(TCPIPStack_ModuleIsRunning(TCPIP_MODULE_DHCPV6_CLIENT))
    {
        TCPIP_DHCPV6_ConnectionHandler(pNetIf, connEvent);
    }   
#endif  // defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
}

static const tcpipModuleConnHandler  TCPIP_STACK_CONN_EVENT_TBL [] =
{
    &F_TCPIP_DHCP_RunConnectionHandler,
    &F_TCPIP_DHCPV6_RunConnectionHandler,

    // add other needed handlers here
};
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT == 0)


// variables

static TCPIP_NET_IF* tcpipNetIf = NULL;       // dynamically allocated

// Main default interfaces
typedef struct
{
    TCPIP_NET_IF* defaultNet;     // default network interface
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)
    TCPIP_NET_IF* defaultMcastNet; // default multicast network interface
#endif
}TCPIPDefaultIF;


static TCPIPDefaultIF tcpipDefIf = { 0 };

static volatile int32_t    totTcpipEventsCnt = 0;
static volatile int32_t    newTcpipTickAvlbl = 0;

static volatile int32_t    newTcpipErrorEventCnt = 0;
static volatile int32_t    newTcpipStackEventCnt = 0;


#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
static void    F_TCPIP_MacEventCB(TCPIP_MAC_EVENT event, const void* hParam);
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

static TCPIP_STACK_MODULE_CTRL  tcpip_stack_ctrl_data = {0};

// last data structure passed for the stack initialization 
static TCPIP_STACK_INIT     tcpip_init_data = { {0} };

//
static SYS_TMR_HANDLE       tcpip_stack_tickH = SYS_TMR_HANDLE_INVALID;      // tick handle

static uint32_t             sysTmrFreq;        // current sys time frequency

static uint32_t             stackTaskRate;  // actual task running rate, ms
                                            // should be safely cast to int16_t, so maximum value is 32767!
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
#if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
static TCPIP_MODULE_RUN_DCPT TCPIP_MODULES_RUN_TBL[TCPIP_MODULES_NUMBER] = { {0}}; 
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT != 0)

#if (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
// table containing the recipients of the TCPIP_MODULE_SIGNAL_IF_CHANGE signal
// keep it short, as most of the modules do not need notifications
// just the socket ones and ARP (just to be gracious on the net) 
static const uint16_t TCPIP_STACK_MODULE_SIGNAL_CHANGE_TBL[] = 
{
    (uint16_t)TCPIP_MODULE_ARP, (uint16_t)TCPIP_MODULE_UDP, (uint16_t)TCPIP_MODULE_TCP
};
// table containing the recipients of the TCPIP_MODULE_SIGNAL_IF_CONFIG signal
// keep it short, as only the socket clients need it
// not used internally
static const uint16_t TCPIP_STACK_MODULE_SIGNAL_CONFIG_TBL[] = 
{
    (uint16_t)TCPIP_MODULE_UDP, (uint16_t)TCPIP_MODULE_TCP
};
#endif  // (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

// table containing the layer 1 frames processed by this stack
static const TCPIP_FRAME_PROCESS_ENTRY TCPIP_FRAME_PROCESS_TBL [] = 
{
    // frameType                                 // moduleId                        // pktTypeFlags         
    // 1st layer handling                                                                 
#if defined(TCPIP_STACK_USE_IPV4)                                                         
    {.frameType = (uint16_t)TCPIP_ETHER_TYPE_ARP,       .moduleId = (uint16_t)TCPIP_MODULE_ARP,     .pktTypeFlags = (uint32_t)TCPIP_MAC_PKT_FLAG_ARP},  // ARP entry
#endif  // defined(TCPIP_STACK_USE_IPV4)                                            
                                                                                    
#if defined(TCPIP_STACK_USE_IPV4)                                                   
    {.frameType = (uint16_t)TCPIP_ETHER_TYPE_IPV4,      .moduleId = (uint16_t)TCPIP_MODULE_IPV4,    .pktTypeFlags = (uint32_t)TCPIP_MAC_PKT_FLAG_IPV4}, // IPv4 entry
#endif  // defined(TCPIP_STACK_USE_IPV4)                                            
                                                                                    
#if defined(TCPIP_STACK_USE_IPV6)                                                   
    {.frameType = (uint16_t)TCPIP_ETHER_TYPE_IPV6,      .moduleId = (uint16_t)TCPIP_MODULE_IPV6,    .pktTypeFlags = (uint32_t)TCPIP_MAC_PKT_FLAG_IPV6}, // IPv6 entry
#endif // defined(TCPIP_STACK_USE_IPV6)                                             
                                                                                    
#if 0   // defined(TCPIP_STACK_USE_LLDP) - no LLDP support
    {.frameType = (uint16_t)TCPIP_ETHER_TYPE_LLDP,      .moduleId = (uint16_t)TCPIP_MODULE_LLDP,    .pktTypeFlags = (uint32_t)TCPIP_MAC_PKT_FLAG_LLDP}, // LLDP entry
#endif  // defined(TCPIP_STACK_USE_LLDP)

    // add other types of supported frames here
};


static SYS_STATUS       tcpip_stack_status = SYS_STATUS_UNINITIALIZED;

static TCPIP_STACK_INIT_CALLBACK tcpip_stack_init_cb;    // callback to be called at init

static TCPIP_STACK_HEAP_CONFIG  tcpip_heap_config = { 0 };      // copy of the heap that the stack uses

static uint32_t tcpip_SecCount;            // current second value
static uint32_t tcpip_MsecCount;           // current millisecond value
static void F_TCPIP_SecondCountSet(void);    // local time keeping

static int16_t  pktGapDcptOffset;
static uint32_t pktGapSign;

#if defined(TCPIP_STACK_TIME_MEASUREMENT)
static uint64_t         tcpip_stack_time = 0;
static bool             tcpip_stack_timeEnable = 0;
extern void             TCPIP_Commands_ExecTimeUpdate(void);
#endif // defined(TCPIP_STACK_TIME_MEASUREMENT)

static bool F_TCPIPStackIsRunState(void);

static void F_TCPIP_STACK_TickHandler(uintptr_t context, uint32_t currTick);        // stack tick handler

static void F_TCPIP_ProcessTickEvent(void);
static int  F_TCPIPExtractMacRxPackets(TCPIP_NET_IF* pNetIf);

static uint32_t F_TCPIPProcessMacPackets(bool signal_t);

static void F_TCPIP_ProcessMACErrorEvents(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT activeEvent);

static bool F_InitNetConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, size_t nNets);
static bool F_LoadNetworkConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, TCPIP_NET_IF* pNetIf, bool restartIf);

static const TCPIP_STACK_MODULE_CONFIG* F_TCPIP_STACK_FindModuleData(TCPIP_STACK_MODULE moduleId, const TCPIP_STACK_MODULE_CONFIG* pModConfig, size_t nModules);

static void  TCPIP_STACK_BringNetDown(TCPIP_STACK_MODULE_CTRL* stackCtrlData, TCPIP_NET_IF* pNetIf, TCPIP_STACK_ACTION action, TCPIP_MAC_POWER_MODE powerMode);
static bool  TCPIP_STACK_BringNetUp(TCPIP_STACK_MODULE_CTRL* stackCtrlData, const TCPIP_NETWORK_CONFIG* pNetConf, const TCPIP_STACK_MODULE_CONFIG* pModConfig, size_t nModules);

static void F_TCPIPStackSetIpAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* ipAddress, const IPV4_ADDR* mask, const IPV4_ADDR* gw, bool setDefault);

#if (TCPIP_STACK_CONFIGURATION_SAVE_RESTORE != 0)
static void* F_NetConfigStringToBuffer(void** ppDstBuff, void* pSrcBuff, size_t* pDstSize, size_t* pNeedLen, size_t* pActLen);
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static TCPIP_MAC_ACTION TCPIP_STACK_StackToMacAction(TCPIP_STACK_ACTION action);

static void TCPIP_STACK_StacktoMacCtrl(TCPIP_MAC_MODULE_CTRL* pMacCtrl, TCPIP_STACK_MODULE_CTRL* stackCtrlData);

static bool TCPIP_STACK_CheckEventsPending(void);

static void F_TCPIP_NetIfEvent(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT event, bool isrProtect);

static TCPIP_MODULE_SIGNAL  F_TCPIPStackManagerSignalClear(TCPIP_MODULE_SIGNAL clrMask);

static void     TCPIP_STACK_KillStack(void);

static bool     F_TCPIP_DoInitialize(const TCPIP_STACK_INIT * init);

static void     F_TCPIP_InitCallback(TCPIP_STACK_INIT_CALLBACK cback);

#if !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)
static void F_TCPIPStackExecuteModules(void);
#endif  // !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)

static void F_TCPIPStackSignalTmo(void);

static bool F_TCPIPStackCreateTimer(void);

static bool F_TCPIPStack_AdjustTimeouts(void);

static void F_TCPIP_SelectDefaultNet(TCPIP_NET_IF* pDownIf);

static void F_TCPIPStackSetIfNumberName(void);

#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
static void F_TCPIPCopyMacAliasIf(TCPIP_NET_IF* pAliasIf, TCPIP_NET_IF* pPriIf);
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

#if (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
static void F_TCPIPStackSetConfig(TCPIP_NET_IF* pNetIf, bool config);
#else
static __inline__ void __attribute__((always_inline)) F_TCPIPStackSetConfig(TCPIP_NET_IF* pNetIf, bool config)
{
    pNetIf->Flags.bInConfig = (uint8_t)config;
}
#endif  // (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

static __inline__ void __attribute__((always_inline)) F_TCPIPInsertMacRxPacket(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt)
{
    pRxPkt->pktIf = pNetIf;
    // insertion into the manager queue should always succeed - manager is always running
    (void) TCPIPStackModuleRxInsert(TCPIP_MODULE_MANAGER, pRxPkt, 0);
}

// protection against MAC ISR
static __inline__ OSAL_CRITSECT_DATA_TYPE __attribute__((always_inline)) F_TCPIPMacIsrSuspend(TCPIP_NET_IF* pNetIf)
{
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
    return OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
#else
    return 0;
#endif // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
}

static __inline__ void __attribute__((always_inline)) F_TCPIPMacIsrResume(TCPIP_NET_IF* pNetIf, OSAL_CRITSECT_DATA_TYPE suspLevel)
{
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, suspLevel);
#endif // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
}

static void F_TCPIPSignalEntrySetNotify(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signals, uint32_t sigParam);

static  void* F_TCPIPSignalEntrySet(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signal_t);

static __inline__ TCPIP_MODULE_SIGNAL_ENTRY* __attribute__((always_inline)) F_TCPIPModuleToSignalEntry(TCPIP_STACK_MODULE modId)
{
    return TCPIP_STACK_MODULE_SIGNAL_TBL + (uint16_t)modId;
}

static void     F_TCPIPSignalEntryNotify(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signal_t, uint32_t sigParam);

// sets module signals and calls user notification
static __inline__ void __attribute__((always_inline)) F_TCPIPModuleSignalSetNotify(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL signals)
{
    F_TCPIPSignalEntrySetNotify(F_TCPIPModuleToSignalEntry(modId), signals, 0);
}

static __inline__ void __attribute__((always_inline)) F_TCPIP_ClearMacEvent(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT event)
{
    OSAL_CRITSECT_DATA_TYPE suspLvl = F_TCPIPMacIsrSuspend(pNetIf);

    pNetIf->activeEvents &= ~(uint16_t)event;

    F_TCPIPMacIsrResume(pNetIf, suspLvl);
}

static __inline__ void __attribute__((always_inline)) F_TCPIPAsyncSignalInc(void)
{
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    stackAsyncSignalCount += 1U; 
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
}

// Note the TCPIP_EVENT enum is aligned with the TCPIP_MAC_EVENT!
static __inline__ TCPIP_EVENT __attribute__((always_inline)) TCPIP_STACK_Mac2TcpipEvent(TCPIP_MAC_EVENT macEvent)
{
    return (TCPIP_EVENT)macEvent;
}

// restores the Default DNS settings
static __inline__ void __attribute__((always_inline)) F_TCPIP_StackSetDefaultDns(TCPIP_NET_IF* pNetIf)
{
    pNetIf->dnsServer[0].Val = pNetIf->DefaultDNSServer[0].Val;
    pNetIf->dnsServer[1].Val = pNetIf->DefaultDNSServer[1].Val;
}

static bool    F_TCPIP_StackSyncFunction(void* synchHandle, TCPIP_MAC_SYNCH_REQUEST req);

// conversion functions/helpers
//
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)
static __inline__ TCPIP_EVENT_LIST_NODE* __attribute__((always_inline)) FC_ListNode2EventNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE * node;
        TCPIP_EVENT_LIST_NODE * evNode;
    }U_SGL_NODE_EVENT_NODE;

    U_SGL_NODE_EVENT_NODE.node = node;
    return U_SGL_NODE_EVENT_NODE.evNode;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_EvHndl2ListNode(TCPIP_EVENT_HANDLE evHandle)
{
    union
    {
        TCPIP_EVENT_HANDLE  evHandle;
        SGL_LIST_NODE*      node;
    }U_EVENT_HNDL_LIST_NODE;

    U_EVENT_HNDL_LIST_NODE.evHandle = evHandle;
    return U_EVENT_HNDL_LIST_NODE.node;
}
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)   

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static __inline__ TCPIP_MAC_HEAP_MallocF __attribute__((always_inline)) FC_MallocDbg2Malloc(TCPIP_MAC_HEAP_MallocFDbg dbgF)
{
    union
    {
        TCPIP_MAC_HEAP_MallocFDbg dbgF;
        TCPIP_MAC_HEAP_MallocF  mallocF;
    }U_DBG_MALLOC_F;

    U_DBG_MALLOC_F.dbgF = dbgF;
    return U_DBG_MALLOC_F.mallocF;
}

static __inline__ TCPIP_MAC_HEAP_CallocF __attribute__((always_inline)) FC_CallocDbg2Calloc(TCPIP_MAC_HEAP_CallocFDbg dbgF)
{
    union
    {
        TCPIP_MAC_HEAP_CallocFDbg dbgF;
        TCPIP_MAC_HEAP_CallocF  callocF;
    }U_DBG_CALLOC_F;

    U_DBG_CALLOC_F.dbgF = dbgF;
    return U_DBG_CALLOC_F.callocF;
}

static __inline__ TCPIP_MAC_HEAP_FreeF __attribute__((always_inline)) FC_FreeDbg2Free(TCPIP_MAC_HEAP_FreeFDbg dbgF)
{
    union
    {
        TCPIP_MAC_HEAP_FreeFDbg dbgF;
        TCPIP_MAC_HEAP_FreeF  freeF;
    }U_DBG_FREE_F;

    U_DBG_FREE_F.dbgF = dbgF;
    return U_DBG_FREE_F.freeF;
}
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
static __inline__ TCPIP_MAC_PKT_AllocF __attribute__((always_inline)) FC_PktAllocDbg2PktAlloc(TCPIP_PKT_ALLOC_FNC_DBG dbgAllocF)
{
    union
    {
        TCPIP_PKT_ALLOC_FNC_DBG dbgAllocF;
        TCPIP_MAC_PKT_AllocF  pktAllocFnc;
    }U_PKT_DBG_ALLOC_F;

    U_PKT_DBG_ALLOC_F.dbgAllocF = dbgAllocF;
    return U_PKT_DBG_ALLOC_F.pktAllocFnc;
}

static __inline__ TCPIP_MAC_PKT_FreeF __attribute__((always_inline)) FC_PktFreeDbg2PktFree(TCPIP_PKT_FREE_FNC_DBG dbgFreeF)
{
    union
    {
        TCPIP_PKT_FREE_FNC_DBG dbgFreeF;
        TCPIP_MAC_PKT_FreeF  pktFreeFnc;
    }U_PKT_DBG_FREE_F;

    U_PKT_DBG_FREE_F.dbgFreeF = dbgFreeF;
    return U_PKT_DBG_FREE_F.pktFreeFnc;
}

static __inline__ TCPIP_MAC_PKT_AckF __attribute__((always_inline)) FC_PktAckDbg2PktAck(TCPIP_PKT_ACK_FNC_DBG dbgAckF)
{
    union
    {
        TCPIP_PKT_ACK_FNC_DBG dbgAckF;
        TCPIP_MAC_PKT_AckF  pktAckFnc;
    }U_PKT_DBG_ACK_F;

    U_PKT_DBG_ACK_F.dbgAckF = dbgAckF;
    return U_PKT_DBG_ACK_F.pktAckFnc;
}


#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)


static __inline__ TCPIP_MODULE_SIGNAL_ENTRY* __attribute__((always_inline)) FC_SigHndl2SigEntry(TCPIP_SIGNAL_HANDLE sigH)
{
    union
    {
        TCPIP_SIGNAL_HANDLE sigH;
        TCPIP_MODULE_SIGNAL_ENTRY* sigEntry;
    }U_SIG_HNDL_SIG_ENTRY;

    U_SIG_HNDL_SIG_ENTRY.sigH = sigH;
    return U_SIG_HNDL_SIG_ENTRY.sigEntry;
}

static __inline__ TCPIP_STACK_PROCESS_HANDLE __attribute__((always_inline)) FC_PktHndl2ProcHndl(TCPIP_STACK_PACKET_HANDLER pktHandler)
{
    union
    {
        TCPIP_STACK_PACKET_HANDLER  pktHandler;
        TCPIP_STACK_PROCESS_HANDLE  procHandle;
    }U_PKT_PROC_HANDLER;

    U_PKT_PROC_HANDLER.pktHandler = pktHandler;
   return U_PKT_PROC_HANDLER.procHandle; 
}

static __inline__ TCPIP_STACK_PACKET_HANDLER __attribute__((always_inline)) FC_ProcHndl2PktHndl(TCPIP_STACK_PROCESS_HANDLE procHandle)
{
    union
    {
        TCPIP_STACK_PROCESS_HANDLE  procHandle;
        TCPIP_STACK_PACKET_HANDLER  pktHandler;
    }U_PROC_PKT_HANDLER;

    U_PROC_PKT_HANDLER.procHandle = procHandle;
   return U_PROC_PKT_HANDLER.pktHandler; 
}

// table with stack's modules
static const TCPIP_STACK_MODULE_ENTRY TCPIP_STACK_MODULE_ENTRY_TBL [] =
#if (TCPIP_STACK_DOWN_OPERATION != 0)
{
    // ModuleID                  // InitFunc                                        // DeInitFunc                
#if defined(TCPIP_STACK_USE_IPV4)
    {.moduleId = (uint16_t)TCPIP_MODULE_ARP,            .initFunc = &TCPIP_ARP_Initialize,      .deInitFunc = &TCPIP_ARP_Deinitialize},            // TCPIP_MODULE_ARP
    {.moduleId = (uint16_t)TCPIP_MODULE_IPV4,           .initFunc = &TCPIP_IPV4_Initialize,     .deInitFunc = &TCPIP_IPV4_Deinitialize},           // TCPIP_MODULE_IPV4
#endif
#if defined(TCPIP_STACK_USE_IPV6)
    {.moduleId = (uint16_t)TCPIP_MODULE_IPV6,           .initFunc = &TCPIP_IPV6_Initialize,     .deInitFunc = &TCPIP_IPV6_Deinitialize},           // TCPIP_MODULE_IPV6
    {.moduleId = (uint16_t)TCPIP_MODULE_ICMPV6,         .initFunc = &TCPIP_ICMPV6_Initialize,   .deInitFunc = &TCPIP_ICMPV6_Deinitialize},         // TCPIP_MODULE_ICMPV6
    {.moduleId = (uint16_t)TCPIP_MODULE_NDP,            .initFunc = &TCPIP_NDP_Initialize,      .deInitFunc = &TCPIP_NDP_Deinitialize},            // TCPIP_MODULE_NDP
#endif
#if defined(TCPIP_STACK_USE_IPV4) && (defined(TCPIP_STACK_USE_ICMP_CLIENT) || defined(TCPIP_STACK_USE_ICMP_SERVER))
    {.moduleId = (uint16_t)TCPIP_MODULE_ICMP,           .initFunc = &TCPIP_ICMP_Initialize,     .deInitFunc = &TCPIP_ICMP_Deinitialize},           // TCPIP_MODULE_ICMP
#endif
#if defined(TCPIP_STACK_USE_UDP)
    {.moduleId = (uint16_t)TCPIP_MODULE_UDP,            .initFunc = &TCPIP_UDP_Initialize,      .deInitFunc = &TCPIP_UDP_Deinitialize},            // TCPIP_MODULE_UDP
#endif
#if defined(TCPIP_STACK_USE_TCP)
    {.moduleId = (uint16_t)TCPIP_MODULE_TCP,            .initFunc = &TCPIP_TCP_Initialize,      .deInitFunc = &TCPIP_TCP_Deinitialize},            //  TCPIP_MODULE_TCP
#endif    
#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_DHCP_CLIENT,    .initFunc = &TCPIP_DHCP_Initialize,     .deInitFunc = &TCPIP_DHCP_Deinitialize},           // TCPIP_MODULE_DHCP_CLIENT
#endif
#if defined(TCPIP_STACK_USE_DHCP_SERVER) || defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
    {.moduleId = (uint16_t)TCPIP_MODULE_DHCP_SERVER,    .initFunc = &TCPIP_DHCPS_Initialize,    .deInitFunc = &TCPIP_DHCPS_Deinitialize},          // TCPIP_MODULE_DHCP_SERVER
#endif
#if defined(TCPIP_STACK_USE_ANNOUNCE)
    {.moduleId = (uint16_t)TCPIP_MODULE_ANNOUNCE,       .initFunc = &TCPIP_ANNOUNCE_Initialize, .deInitFunc = &TCPIP_ANNOUNCE_Deinitialize},       // TCPIP_MODULE_ANNOUNCE
#endif
#if defined(TCPIP_STACK_USE_NBNS)
    {.moduleId = (uint16_t)TCPIP_MODULE_NBNS,           .initFunc = &TCPIP_NBNS_Initialize,     .deInitFunc = &TCPIP_NBNS_Deinitialize},           // TCPIP_MODULE_NBNS
#endif
#if defined(TCPIP_STACK_USE_FTP_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_FTP_SERVER,    .initFunc = &TCPIP_FTP_ServerInitialize, .deInitFunc = &TCPIP_FTP_ServerDeinitialize},      // TCPIP_MODULE_FTP_SERVER
#endif
#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_DYNDNS_CLIENT, .initFunc = &TCPIP_DDNS_Initialize,      .deInitFunc = &TCPIP_DDNS_Deinitialize},           // TCPIP_MODULE_DYNDNS_CLIENT
#endif
#if defined(TCPIP_STACK_USE_REBOOT_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_REBOOT_SERVER, .initFunc = &TCPIP_REBOOT_Initialize,    .deInitFunc = &TCPIP_REBOOT_Deinitialize},         // TCPIP_MODULE_REBOOT_SERVER
#endif
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    {.moduleId = (uint16_t)TCPIP_MODULE_ZCLL,          .initFunc = &TCPIP_ZCLL_Initialize,      .deInitFunc = &TCPIP_ZCLL_Deinitialize},           // TCPIP_MODULE_ZCLL
#if defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
    {.moduleId = (uint16_t)TCPIP_MODULE_MDNS,          .initFunc = &TCPIP_MDNS_Initialize,      .deInitFunc = &TCPIP_MDNS_Deinitialize},           // TCPIP_MODULE_MDNS
#endif
#endif
#if defined(TCPIP_STACK_USE_TFTP_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_TFTP_CLIENT,   .initFunc = &TCPIP_TFTPC_Initialize,     .deInitFunc = &TCPIP_TFTPC_Deinitialize},          // TCPIP_MODULE_TFTP_CLIENT
#endif
#if defined(TCPIP_STACK_USE_IGMP)
    {.moduleId = (uint16_t)TCPIP_MODULE_IGMP,          .initFunc = &TCPIP_IGMP_Initialize,      .deInitFunc = &TCPIP_IGMP_Deinitialize},          // TCPIP_MODULE_IGMP
#endif
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_SNTP_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_SNTP,          .initFunc = &TCPIP_SNTP_Initialize,      .deInitFunc = &TCPIP_SNTP_Deinitialize},           // TCPIP_MODULE_SNTP
#endif
#if defined(TCPIP_STACK_USE_DNS)
    {.moduleId = (uint16_t)TCPIP_MODULE_DNS_CLIENT,    .initFunc = &TCPIP_DNS_ClientInitialize, .deInitFunc = &TCPIP_DNS_ClientDeinitialize},      // TCPIP_MODULE_DNS_CLIENT
#endif
#if defined(TCPIP_STACK_USE_BERKELEY_API)
    {.moduleId = (uint16_t)TCPIP_MODULE_BERKELEY,      .initFunc = &BerkeleySocketInitialize,   .deInitFunc = &BerkeleySocketDeinitialize},       // TCPIP_MODULE_BERKELEY
#endif
#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_HTTP_NET_SERVER, .initFunc = &TCPIP_HTTP_NET_Initialize, .deInitFunc = &TCPIP_HTTP_NET_Deinitialize},      // TCPIP_MODULE_HTTP_NET_SERVER
#endif
#if defined(TCPIP_STACK_USE_TELNET_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_TELNET_SERVER, .initFunc = &TCPIP_TELNET_Initialize,    .deInitFunc = &TCPIP_TELNET_Deinitialize},         // TCPIP_MODULE_TELNET_SERVER
#endif
#if defined(TCPIP_STACK_USE_SNMP_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_SNMP_SERVER,   .initFunc = &TCPIP_SNMP_Initialize,      .deInitFunc = &TCPIP_SNMP_Deinitialize},           // TCPIP_MODULE_SNMP_SERVER
#endif
#if defined(TCPIP_STACK_USE_DNS_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_DNS_SERVER,    .initFunc = &TCPIP_DNSS_Initialize,      .deInitFunc = &TCPIP_DNSS_Deinitialize},           // TCPIP_MODULE_DNS_SERVER
#endif
#if defined(TCPIP_STACK_COMMAND_ENABLE)
    {.moduleId = (uint16_t)TCPIP_MODULE_COMMAND,       .initFunc = &TCPIP_Commands_Initialize,  .deInitFunc = &TCPIP_Commands_Deinitialize},       // TCPIP_MODULE_COMMAND
#endif
#if defined(TCPIP_STACK_USE_IPERF)
    {.moduleId = (uint16_t)TCPIP_MODULE_IPERF,         .initFunc = &TCPIP_IPERF_Initialize,     .deInitFunc = &TCPIP_IPERF_Deinitialize},          // TCPIP_MODULE_IPERF
#endif
#if defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_DHCPV6_CLIENT, .initFunc = &TCPIP_DHCPV6_Initialize,    .deInitFunc = &TCPIP_DHCPV6_Deinitialize},          // TCPIP_MODULE_DHCPV6_CLIENT
#endif
#if defined(TCPIP_STACK_USE_SMTPC)
    {.moduleId = (uint16_t)TCPIP_MODULE_SMTPC,          .initFunc = &TCPIP_SMTPC_Initialize,    .deInitFunc = &TCPIP_SMTPC_Deinitialize},          // TCPIP_MODULE_SMTPC
#endif
#if defined(TCPIP_STACK_USE_TFTP_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_TFTP_SERVER,    .initFunc = &TCPIP_TFTPS_Initialize,    .deInitFunc = &TCPIP_TFTPS_Deinitialize},          // TCPIP_MODULE_TFTP_SERVER
#endif   
#if defined(TCPIP_STACK_USE_FTP_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_FTP_CLIENT,     .initFunc = &TCPIP_FTPC_Initialize,     .deInitFunc = &TCPIP_FTPC_Deinitialize},          // TCPIP_MODULE_FTP_CLIENT
#endif 
#if defined(TCPIP_STACK_USE_MAC_BRIDGE)
    {.moduleId = (uint16_t)TCPIP_MODULE_MAC_BRIDGE,     .initFunc = &TCPIP_MAC_Bridge_Initialize,  .deInitFunc = &TCPIP_MAC_Bridge_Deinitialize},      // TCPIP_MODULE_MAC_BRIDGE
#endif 
#if defined(TCPIP_STACK_USE_HTTP_SERVER_V2)
    {.moduleId = (uint16_t)TCPIP_MODULE_HTTP_SERVER_V2, .initFunc = &TCPIP_HTTP_Server_Initialize,  .deInitFunc = &TCPIP_HTTP_Server_Deinitialize},      // TCPIP_STACK_USE_HTTP_SERVER_V2
#endif  // defined(TCPIP_STACK_USE_HTTP_SERVER_V2)
#if defined(TCPIP_STACK_USE_WS_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_WS_CLIENT,      .initFunc = &TCPIP_WSC_Initialize,  .deInitFunc = &TCPIP_WSC_Deinitialize},               // TCPIP_MODULE_WS_CLIENT
#endif  // defined(TCPIP_STACK_USE_WS_CLIENT)
    // Add other stack modules here
     
};
#else
{
    // ModuleID                  // InitFunc                                        
#if defined(TCPIP_STACK_USE_IPV4)
    {.moduleId = (uint16_t)TCPIP_MODULE_ARP,           .initFunc = &TCPIP_ARP_Initialize},            // TCPIP_MODULE_ARP
    {.moduleId = (uint16_t)TCPIP_MODULE_IPV4,          .initFunc = &TCPIP_IPV4_Initialize},           // TCPIP_MODULE_IPV4
#endif
#if defined(TCPIP_STACK_USE_IPV6)
    {.moduleId = (uint16_t)TCPIP_MODULE_IPV6,          .initFunc = &TCPIP_IPV6_Initialize},           // TCPIP_MODULE_IPV6
    {.moduleId = (uint16_t)TCPIP_MODULE_ICMPV6,        .initFunc = &TCPIP_ICMPV6_Initialize},         // TCPIP_MODULE_ICMPV6
    {.moduleId = (uint16_t)TCPIP_MODULE_NDP,           .initFunc = &TCPIP_NDP_Initialize},            // TCPIP_MODULE_NDP
#endif
#if defined(TCPIP_STACK_USE_IPV4) && (defined(TCPIP_STACK_USE_ICMP_CLIENT) || defined(TCPIP_STACK_USE_ICMP_SERVER))
    {.moduleId = (uint16_t)TCPIP_MODULE_ICMP,          .initFunc = &TCPIP_ICMP_Initialize},           // TCPIP_MODULE_ICMP
#endif
#if defined(TCPIP_STACK_USE_UDP)
    {.moduleId = (uint16_t)TCPIP_MODULE_UDP,           .initFunc = &TCPIP_UDP_Initialize},            // TCPIP_MODULE_UDP
#endif
#if defined(TCPIP_STACK_USE_TCP)
    {.moduleId = (uint16_t)TCPIP_MODULE_TCP,           .initFunc = &TCPIP_TCP_Initialize},            //  TCPIP_MODULE_TCP
#endif    
#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_DHCP_CLIENT,   .initFunc = &TCPIP_DHCP_Initialize},           // TCPIP_MODULE_DHCP_CLIENT
#endif
#if defined(TCPIP_STACK_USE_DHCP_SERVER) || defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
    {.moduleId = (uint16_t)TCPIP_MODULE_DHCP_SERVER,   .initFunc = &TCPIP_DHCPS_Initialize},          // TCPIP_MODULE_DHCP_SERVER
#endif
#if defined(TCPIP_STACK_USE_ANNOUNCE)
    {.moduleId = (uint16_t)TCPIP_MODULE_ANNOUNCE,      .initFunc = &TCPIP_ANNOUNCE_Initialize},       // TCPIP_MODULE_ANNOUNCE
#endif
#if defined(TCPIP_STACK_USE_NBNS)
    {.moduleId = (uint16_t)TCPIP_MODULE_NBNS,          .initFunc = &TCPIP_NBNS_Initialize},           // TCPIP_MODULE_NBNS
#endif
#if defined(TCPIP_STACK_USE_FTP_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_FTP_SERVER,    .initFunc = &TCPIP_FTP_ServerInitialize},      // TCPIP_MODULE_FTP_SERVER
#endif
#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_DYNDNS_CLIENT, .initFunc = &TCPIP_DDNS_Initialize},           // TCPIP_MODULE_DYNDNS_CLIENT
#endif
#if defined(TCPIP_STACK_USE_REBOOT_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_REBOOT_SERVER, .initFunc = &TCPIP_REBOOT_Initialize},         // TCPIP_MODULE_REBOOT_SERVER
#endif
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    {.moduleId = (uint16_t)TCPIP_MODULE_ZCLL,          .initFunc = &TCPIP_ZCLL_Initialize},           // TCPIP_MODULE_ZCLL
#if defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
    {.moduleId = (uint16_t)TCPIP_MODULE_MDNS,          .initFunc = &TCPIP_MDNS_Initialize},           // TCPIP_MODULE_MDNS
#endif
#endif
#if defined(TCPIP_STACK_USE_TFTP_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_TFTP_CLIENT,   .initFunc = &TCPIP_TFTPC_Initialize},          // TCPIP_MODULE_TFTP_CLIENT
#endif
#if defined(TCPIP_STACK_USE_IGMP)
    {.moduleId = (uint16_t)TCPIP_MODULE_IGMP,          .initFunc = &TCPIP_IGMP_Initialize},          // TCPIP_MODULE_IGMP
#endif
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_SNTP_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_SNTP,          .initFunc = &TCPIP_SNTP_Initialize},           // TCPIP_MODULE_SNTP
#endif
#if defined(TCPIP_STACK_USE_DNS)
    {.moduleId = (uint16_t)TCPIP_MODULE_DNS_CLIENT,    .initFunc = &TCPIP_DNS_ClientInitialize},      // TCPIP_MODULE_DNS_CLIENT
#endif
#if defined(TCPIP_STACK_USE_BERKELEY_API)
    {.moduleId = (uint16_t)TCPIP_MODULE_BERKELEY,      .initFunc = &BerkeleySocketInitialize},        // TCPIP_MODULE_BERKELEY
#endif
#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_HTTP_NET_SERVER, .initFunc = &TCPIP_HTTP_NET_Initialize},      // TCPIP_MODULE_HTTP_NET_SERVER
#endif
#if defined(TCPIP_STACK_USE_TELNET_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_TELNET_SERVER, .initFunc = &TCPIP_TELNET_Initialize},         // TCPIP_MODULE_TELNET_SERVER
#endif
#if defined(TCPIP_STACK_USE_SNMP_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_SNMP_SERVER,   .initFunc = &TCPIP_SNMP_Initialize},           // TCPIP_MODULE_SNMP_SERVER
#endif
#if defined(TCPIP_STACK_USE_DNS_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_DNS_SERVER,    .initFunc = &TCPIP_DNSS_Initialize},           // TCPIP_MODULE_DNS_SERVER
#endif
#if defined(TCPIP_STACK_COMMAND_ENABLE)
    {.moduleId = (uint16_t)TCPIP_MODULE_COMMAND,       .initFunc = &TCPIP_Commands_Initialize},       // TCPIP_MODULE_COMMAND
#endif
#if defined(TCPIP_STACK_USE_IPERF)
    {.moduleId = (uint16_t)TCPIP_MODULE_IPERF,         .initFunc = &TCPIP_IPERF_Initialize},          // TCPIP_MODULE_IPERF
#endif
#if defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_DHCPV6_CLIENT, .initFunc = &TCPIP_DHCPV6_Initialize},          // TCPIP_MODULE_DHCPV6_CLIENT
#endif
#if defined(TCPIP_STACK_USE_SMTPC)
    {.moduleId = (uint16_t)TCPIP_MODULE_SMTPC,        .initFunc = &TCPIP_SMTPC_Initialize},          // TCPIP_MODULE_SMTPC
#endif
#if defined(TCPIP_STACK_USE_TFTP_SERVER)
    {.moduleId = (uint16_t)TCPIP_MODULE_TFTP_SERVER,  .initFunc = &TCPIP_TFTPS_Initialize},          // TCPIP_MODULE_TFTP_SERVER
#endif   
#if defined(TCPIP_STACK_USE_FTP_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_FTP_CLIENT,   .initFunc = &TCPIP_FTPC_Initialize},          // TCPIP_MODULE_FTP_CLIENT
#endif 
#if defined(TCPIP_STACK_USE_MAC_BRIDGE)
    {.moduleId = (uint16_t)TCPIP_MODULE_MAC_BRIDGE,   .initFunc = &TCPIP_MAC_Bridge_Initialize},      // TCPIP_MODULE_MAC_BRIDGE
#endif 
#if defined(TCPIP_STACK_USE_HTTP_SERVER_V2)
    {.moduleId = (uint16_t)TCPIP_MODULE_HTTP_SERVER_V2, .initFunc = &TCPIP_HTTP_Server_Initialize},   // TCPIP_STACK_USE_HTTP_SERVER_V2
#endif  // defined(TCPIP_STACK_USE_HTTP_SERVER_V2)
#if defined(TCPIP_STACK_USE_WS_CLIENT)
    {.moduleId = (uint16_t)TCPIP_MODULE_WS_CLIENT,      .initFunc = &TCPIP_WSC_Initialize},               // TCPIP_MODULE_WS_CLIENT
#endif  // defined(TCPIP_STACK_USE_WS_CLIENT)
    // Add other stack modules here
     
};
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_BASIC) != 0)
#if (M_TCPIP_STACK_ENABLE_ASSERT_LOOP != 0)
volatile int TCPIP_Stack_LeaveAssertLoop = 0;
#endif  // (M_TCPIP_STACK_ENABLE_ASSERT_LOOP != 0)
void TCPIPStack_Assert(bool cond, const char* fileName, const char* funcName, int lineNo)
{
    if(cond == false)
    {
        // remove the path from the fileName
        const char* lastPath = strrchr(fileName, (int)'/');
        if(lastPath == NULL)
        {   // try windows style
            lastPath = strrchr(fileName, (int)'\\');
        }
        if(lastPath != NULL)
        {
            lastPath++;
        }
        else
        {   // no luck
            lastPath = fileName;
        }
        SYS_CONSOLE_PRINT("TCPIP Stack Assert: in file: %s, func: %s, line: %d, \r\n", lastPath, funcName, lineNo);
#if (M_TCPIP_STACK_ENABLE_ASSERT_LOOP != 0)
        while(TCPIP_Stack_LeaveAssertLoop == 0);
#endif  // (M_TCPIP_STACK_ENABLE_ASSERT_LOOP != 0)
    }
}

#if (M_TCPIP_STACK_ENABLE_COND_LOOP != 0)
volatile int TCPIP_Stack_LeaveCondLoop = 0;
#endif  // (M_TCPIP_STACK_ENABLE_COND_LOOP != 0)
void TCPIPStack_Condition(bool cond, const char* fileName, const char* funcName, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("TCPIP Stack Cond: in file: %s, func: %s, line: %d, \r\n", fileName, funcName, lineNo);
#if (M_TCPIP_STACK_ENABLE_COND_LOOP != 0)
        while(TCPIP_Stack_LeaveCondLoop == 0);
#endif // (M_TCPIP_STACK_ENABLE_COND_LOOP != 0)
    }
}
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_BASIC) != 0)

SYS_MODULE_OBJ TCPIP_STACK_Initialize(const SYS_MODULE_INDEX index, const struct TCPIP_STACK_INIT * const init)
{
    if(tcpipNetIf != NULL)
    {   // already up and running
        return (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data;
    }

    if(init == NULL)
    {   // no initialization data passed
        return SYS_MODULE_OBJ_INVALID;
    }

    // start stack initialization
    totTcpipEventsCnt = 0;

    newTcpipErrorEventCnt = 0;
    newTcpipStackEventCnt = 0;
    newTcpipTickAvlbl = 0;
    stackTaskRate = 0U;
    stackLinkTmo = 0;

    (void) memset(&tcpip_stack_ctrl_data, 0, sizeof(tcpip_stack_ctrl_data));

    SYS_CONSOLE_MESSAGE(TCPIP_STACK_HDR_MESSAGE "Initialization Started \r\n");

    const TCPIP_STACK_INIT*  cStackInit = init;
    tcpip_stack_init_cb = cStackInit->initCback;

    tcpip_stack_status = SYS_STATUS_BUSY;
    if(tcpip_stack_init_cb == NULL)
    {   // perform the immediate initialization
        bool init_res = F_TCPIP_DoInitialize(cStackInit);
        return init_res ? (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data : SYS_MODULE_OBJ_INVALID;
    }

    // continue initialization in TCPIP_STACK_Task()
    return (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data;

}

// calls the user initialization callback
// as part of the stack initialization
static void F_TCPIP_InitCallback(TCPIP_STACK_INIT_CALLBACK cback)
{
    const TCPIP_STACK_INIT* pInit;

    int cRes = (*cback)(&pInit);

    if(cRes > 0)
    {   // pending
        return;
    }

    if (cRes == 0 && pInit != NULL)
    {   // we're good to go
        (void) F_TCPIP_DoInitialize(pInit);
    }
    else
    {   // some error has occurred
        tcpip_stack_status = SYS_STATUS_UNINITIALIZED;
    }
}


// performs the stack initialization
// returns true if succesful
// false otherwise
static bool F_TCPIP_DoInitialize(const TCPIP_STACK_INIT * init)
{
    size_t                      netIx, ix;
    size_t                      nNets, nModules;
    int32_t                     initFail;
    TCPIP_STACK_HEAP_HANDLE heapH;
    TCPIP_NET_IF           *pIf, *pScanIf;
    TCPIP_MAC_POWER_MODE    powerMode;
    const TCPIP_NETWORK_CONFIG* pUsrConfig;
    const TCPIP_STACK_MODULE_CONFIG* pModConfig;
    const TCPIP_STACK_HEAP_CONFIG* heapData;
    const TCPIP_STACK_MODULE_CONFIG* pHeapConfig;
    const TCPIP_MAC_OBJECT*  pPriMac; 
    IPV4_ADDR               dupIpAddr;


    pUsrConfig = init->pNetConf;
    nNets = (size_t)init->nNets;
    pModConfig = init->pModConfig;
    nModules = (size_t)init->nModules;

    // minimum sanity check
    if((nNets == 0U) || (pUsrConfig == NULL) || (pUsrConfig->pMacObject == NULL) || (pModConfig == NULL) || (nModules == 0U))
    {   // cannot run with no interface/init data
        return false;
    }

    // snapshot of the initialization data
    tcpip_init_data = *init;
    
    while(true)
    {
        initFail = 0;

        sysTmrFreq = SYS_TIME_FrequencyGet();

        // find the heap settings
        pHeapConfig = F_TCPIP_STACK_FindModuleData(TCPIP_MODULE_MANAGER, pModConfig, nModules);
        heapData = (pHeapConfig != NULL) ? (const TCPIP_STACK_HEAP_CONFIG*)pHeapConfig->configData : NULL;
        // create the heap and get a handle to the heap memory
        heapH = TCPIP_HEAP_Create(heapData, NULL);

        tcpip_stack_ctrl_data.memH = heapH;
        if( tcpip_stack_ctrl_data.memH == NULL)
        {   // cannot instantiate a heap
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Heap creation failed, type: %d\r\n", (heapData != NULL) ? heapData->heapType : TCPIP_STACK_HEAP_TYPE_NONE);
            initFail = 1;
            break;
        }
        tcpip_stack_ctrl_data.heapType = heapData->heapType;

        tcpipNetIf = (TCPIP_NET_IF*)TCPIP_HEAP_Calloc(heapH, (uint32_t)nNets, sizeof(TCPIP_NET_IF)); // allocate for each network interface
        if(tcpipNetIf == NULL)
        {   // failed
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Network configuration allocation failed: %lu\r\n", (uint32_t)nNets * sizeof(TCPIP_NET_IF));
            initFail = 2;
            break;
        }

        if(TCPIP_PKT_Initialize(heapH, pUsrConfig, nNets) == false)
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Packet initialization failed: 0x%x\r\n", heapH);
            initFail = 3;
            break;
        }

        // initialize the run time table
        // select the running modules
#if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
        (void) memset(&TCPIP_MODULES_RUN_TBL, 0, sizeof(TCPIP_MODULES_RUN_TBL));
        TCPIP_MODULES_RUN_TBL[TCPIP_MODULE_MANAGER].val = (uint8_t)TCPIP_MODULE_RUN_FLAG_IS_RUNNING; 
        TCPIP_MODULE_RUN_DCPT* pRDcpt;
        size_t modIx;
        const TCPIP_STACK_MODULE_ENTRY* pEntry = TCPIP_STACK_MODULE_ENTRY_TBL + 0;

        for(modIx = 0; modIx < (sizeof(TCPIP_STACK_MODULE_ENTRY_TBL) / sizeof(*TCPIP_STACK_MODULE_ENTRY_TBL)); modIx++)
        {
            if(F_TCPIP_STACK_FindModuleData((TCPIP_STACK_MODULE)pEntry->moduleId, pModConfig, nModules) != NULL)
            {
                pRDcpt = TCPIP_MODULES_RUN_TBL + pEntry->moduleId;
                pRDcpt->isRunning = 1;  // module should be started
            }
            pEntry++;
        }
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT != 0)


        tcpip_stack_ctrl_data.nIfs = (uint16_t)nNets;
        tcpip_stack_ctrl_data.nModules = (uint16_t)nModules;

        if(!F_InitNetConfig(pUsrConfig, nNets))
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Network configuration initialization failed: %d\r\n", nNets);
            initFail = 4;   // failed the initialization
            break;
        }

        // detect the primary and alias interfaces
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        TCPIP_NET_IF           *pPriIf;

        pIf = tcpipNetIf;
        for(netIx = 0; netIx < nNets; netIx++)
        {
            if(pIf->pPriIf == NULL)
            {   // new scanned interface, make it primary
                pIf->pPriIf = pPriIf = pIf;
                pPriMac = pPriIf->pMacObj;
                pScanIf = pIf + 1;
                for(ix = netIx + 1U; ix < nNets; ix++)
                {
                    if(pScanIf->pMacObj == pPriMac)
                    {   // alias interface; insert it
                        pScanIf->pPriIf = pPriIf;
                        pScanIf->pAlias = pPriIf->pAlias;
                        pPriIf->pAlias = pScanIf;
                        // count it
                        tcpip_stack_ctrl_data.nAliases++;
                    }
                    pScanIf++;
                }
            }
            pIf++;
        }
#else
        netIx = 0;
        pIf = tcpipNetIf;
        while(netIx < nNets)
        {
            pPriMac = pIf->pMacObj;
            pScanIf = pIf + 1;
            for(ix = netIx + 1U; ix < nNets; ix++)
            {
                if(pScanIf->pMacObj == pPriMac)
                {   // alias interface; count it
                    tcpip_stack_ctrl_data.nAliases++;
                }
                pScanIf++;
            }
            netIx++;
            pIf++;
        }

        if(tcpip_stack_ctrl_data.nAliases != 0U)
        {
            initFail = 5;
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Network configuration: Aliases not supported: %d\r\n", tcpip_stack_ctrl_data.nAliases);
            break;
        }

#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

        // make sure that there's no 2 interfaces with identical IP address
        dupIpAddr.Val = 0;
        netIx = 0;
        pIf = tcpipNetIf; 
        while((netIx < nNets) && (initFail == 0)) 
        {
            dupIpAddr.Val = pIf->DefaultIPAddr.Val;
            if(dupIpAddr.Val != 0U)
            {   // IP address 0 does not count as duplicate
                pScanIf = pIf + 1;
                for(ix = netIx + 1U; ix < nNets; ix++)
                {
                    if(pScanIf->DefaultIPAddr.Val == dupIpAddr.Val)
                    {
                        initFail = 6;
                        break;
                    }
                    pScanIf++;
                }
            }
            netIx++;
            pIf++;
        }

        if(initFail != 0)
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Network configuration: invalid IP address: 0x%8x\r\n", dupIpAddr.Val);
            break;
        }

#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
        // check the vlan settings
        pUsrConfig = init->pNetConf;
        netIx = 0;
        int vlanFail = 0;
        while(netIx < nNets) 
        {
            if(pUsrConfig->vlanId == 1U || pUsrConfig->vlanId == 2U || pUsrConfig->vlanId >= 0xfffU)
            {
                vlanFail = 1;
                break;
            }
            if((pUsrConfig->vlanId & M_TCPIP_STACK_VLAN_ID_MASK) != 0U)
            {   // wrong value
                vlanFail = 1;
                break;
            }
            if((pUsrConfig->vlanPcp & M_TCPIP_STACK_VLAN_PCP_MASK) != 0U)
            {   // wrong value
                vlanFail = 2;
                break;
            }

            netIx++;
            pUsrConfig++;
        }

        if(vlanFail != 0)
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "VLAN ID/PCP wrong value: %d\r\n", vlanFail < 2 ? pUsrConfig->vlanId : pUsrConfig->vlanPcp);
            initFail = 7;
            break;
        }
#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 

        // delete the old defaults
        tcpipDefIf.defaultNet = NULL;
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)
        tcpipDefIf.defaultMcastNet = NULL;
#endif

        F_TCPIPStackSetIfNumberName();

        // initialize the signal handlers
        (void) memset(TCPIP_STACK_MODULE_SIGNAL_TBL, 0x0, sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL));
        stackAsyncSignalCount = 0;

        // save the heap configuration
        tcpip_heap_config = *heapData; 

        // initialize the processed frames table
        for(ix = 0; ix < (sizeof(TCPIP_MODULES_QUEUE_TBL) / sizeof(*TCPIP_MODULES_QUEUE_TBL)); ix++)
        {
            TCPIP_Helper_SingleListInitialize(TCPIP_MODULES_QUEUE_TBL + ix);
        }

        // start per interface initializing
        pktGapDcptOffset = TCPIP_PKT_GapDcptOffset(); 
        pktGapSign = TCPIP_PKT_GapSign(); 
        tcpip_stack_ctrl_data.stackAction = (uint8_t)TCPIP_STACK_ACTION_INIT;

        pIf = tcpipNetIf; 
        pUsrConfig = init->pNetConf;
        for(netIx = 0; netIx < nNets ; netIx++)
        {
            if(TCPIPStackNetIsPrimary(pIf))
            {   // first bring up the primary, then the aliases!
                // check the power mode
                powerMode = TCPIP_Helper_StringToPowerMode(pUsrConfig->powerMode);
                if((powerMode != TCPIP_MAC_POWER_FULL) && (powerMode != TCPIP_MAC_POWER_DOWN))
                {   
                    SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Power Mode initialization fail: %d\r\n", powerMode);
                    initFail = 8;
                    break;
                }

                // set transient data
                tcpip_stack_ctrl_data.powerMode = (uint8_t)powerMode;
                tcpip_stack_ctrl_data.pNetIf = pIf;
                tcpip_stack_ctrl_data.netIx = pIf->netIfIx;

                if(!TCPIP_STACK_BringNetUp(&tcpip_stack_ctrl_data, pUsrConfig, pModConfig, nModules))
                {
                    initFail = 9;
                    break;
                }
            }
            pIf++; 
            pUsrConfig++;
        }

        // start the aliases, if any
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        if(initFail != 0)
        {   // failed
            break;
        }

        pUsrConfig = init->pNetConf;
        pIf = tcpipNetIf;
        for(netIx = 0; netIx < nNets ; netIx++)
        {
            if(!TCPIPStackNetIsPrimary(pIf))
            {   // primaries should be up
                // check the power mode
                powerMode = TCPIP_Helper_StringToPowerMode(pUsrConfig->powerMode);
                if((powerMode != TCPIP_MAC_POWER_FULL) && (powerMode != TCPIP_MAC_POWER_DOWN))
                {   
                    SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Power Mode initialization fail: %d\r\n", powerMode);
                    initFail = 10;
                    break;
                }

                // set transient data
                tcpip_stack_ctrl_data.powerMode = (uint8_t)powerMode;
                tcpip_stack_ctrl_data.pNetIf = pIf;
                tcpip_stack_ctrl_data.netIx = pIf->netIfIx;

                if(!TCPIP_STACK_BringNetUp(&tcpip_stack_ctrl_data, pUsrConfig, pModConfig, nModules))
                {
                    initFail = 11;
                    break;
                }
            }
            pIf++;
            pUsrConfig++;
        }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

        break;
    }

    // initialization done
    if(initFail == 0)
    {
#if !defined (TCPIP_STACK_USE_EXTERNAL_HEAP)
        size_t heapLeft;
        // check the amount of heap left
        heapLeft = TCPIP_HEAP_FreeSize(heapH);
        if(heapLeft < (size_t)TCPIP_STACK_DRAM_RUN_LIMIT)
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
 * Function:        bool TCPIP_STACK_BringNetUp(TCPIP_NET_IF* pNetIf, const TCPIP_NETWORK_CONFIG* pNetConf, const TCPIP_STACK_MODULE_CONFIG* pModConfig, size_t nModules)
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
static bool TCPIP_STACK_BringNetUp(TCPIP_STACK_MODULE_CTRL* stackCtrlData, const TCPIP_NETWORK_CONFIG* pNetConf, const TCPIP_STACK_MODULE_CONFIG* pModConfig, size_t nModules)
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
    pNetIf->netIfIx = (uint16_t)stackCtrlData->netIx;
    pNetIf->Flags.powerMode = stackCtrlData->powerMode;
    pMacObj = pNetIf->pMacObj;

    while(true)
    {
        if(TCPIPStackNetIsPrimary(pNetIf))
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
            if (pModConfig != NULL)
            {
                pConfig = F_TCPIP_STACK_FindModuleData((TCPIP_STACK_MODULE)pMacObj->macId, pModConfig, nModules);
                if(pConfig != NULL)
                {   // there's new MAC config data
                    pNetIf->pMacConfig = pConfig->configData;
                    macConfig = pConfig->configData;
                }
            }

            if(stackCtrlData->powerMode == (uint8_t)TCPIP_MAC_POWER_FULL)
            {   // init the MAC
                TCPIP_STACK_StacktoMacCtrl(&macCtrl, stackCtrlData);
                TCPIP_MAC_INIT macInit =
                {
                    { 0 }, // SYS_MODULE_INIT not currently used
                    &macCtrl,
                    macConfig,
                };

                pNetIf->macObjHandle = (*pMacObj->MAC_Initialize)(pMacObj->macId, &macInit.moduleInit);

                if( pNetIf->macObjHandle == SYS_MODULE_OBJ_INVALID)
                {
                    pNetIf->macObjHandle = 0;
                    SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "%s MAC initialization failed\r\n", pMacObj->macName);
                    netUpFail = 1;
                    break;
                }

                // open the MAC
                pNetIf->hIfMac = (*pMacObj->MAC_Open)(pMacObj->macId, DRV_IO_INTENT_READWRITE);
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
        // #if (M_TCPIP_STACK_RUN_TIME_INIT == 0)
        //          then all modules from the build time TCPIP_STACK_MODULE_ENTRY_TBL will be initialized
        // #if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
        //          then only modules that have initialization data will be started and initialized
        //          at stack start-up (pModConfig != 0) we determine the modules that need to run
        //          at interface up (pModConfig == 0) we call only the running modules

        int32_t modIx;
#if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT != 0)
        const TCPIP_STACK_MODULE_ENTRY*  pEntry = TCPIP_STACK_MODULE_ENTRY_TBL + 0;

        for(modIx = 0; modIx < ((int32_t)sizeof(TCPIP_STACK_MODULE_ENTRY_TBL) / (int32_t)sizeof(*TCPIP_STACK_MODULE_ENTRY_TBL)); modIx++)
        {
            configData = NULL;
            if (pModConfig != NULL)
            {
                pConfig = F_TCPIP_STACK_FindModuleData((TCPIP_STACK_MODULE)pEntry->moduleId, pModConfig, nModules);
                if(pConfig != NULL)
                {
                    configData = pConfig->configData;
                }
            }

#if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
            TCPIP_MODULE_RUN_DCPT* pRDcpt = TCPIP_MODULES_RUN_TBL + pEntry->moduleId;
            if(pRDcpt->isRunning != 0U)
            {
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT != 0)
                if(!pEntry->initFunc(stackCtrlData, configData))
                {
                    SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Module no: %d Initialization failed\r\n", pEntry->moduleId);
                    netUpFail = 1;
                    break;
                }
#if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
            }
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT != 0)
            pEntry++;
        }


        if((!netUpFail) && (pNetIf->hIfMac != 0U))
        {
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
            if(TCPIPStackNetIsPrimary(pNetIf))
            {
                if(!(*pNetIf->pMacObj->MAC_EventMaskSet)(pNetIf->hIfMac, TCPIP_STACK_MAC_ALL_EVENTS, true))
                {
                    SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "%s MAC event notification setting failed\r\n", pNetIf->pMacObj->macName);
                    netUpFail = 1;
                    break;
                }
            }
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

            // completed the MAC initialization
            pNetIf->Flags.bMacInitialize = 1;
            pNetIf->Flags.bMacInitDone = 0;
        }

        break;
    }

    if(netUpFail)
    {
        return false;
    }


#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    TCPIP_NET_IF* pPriIf;
    if((pPriIf = TCPIPStackNetGetPrimary(pNetIf)) != pNetIf)
    {   // alias
        if(pPriIf->Flags.bInterfaceEnabled != 0U)
        {   // primary already enabled: enable alias too
            F_TCPIPCopyMacAliasIf(pNetIf, pPriIf);

            pNetIf->Flags.bInterfaceEnabled = 1;
            pNetIf->Flags.bMacInitialize = 0;
            pNetIf->Flags.bMacInitDone = 1;
        }
    }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

    return true;

}

#if (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
bool TCPIP_STACK_NetUp(TCPIP_NET_HANDLE netH, const TCPIP_NETWORK_CONFIG* pUsrConfig)
{
    bool    success;
    TCPIP_MAC_POWER_MODE  powerMode;

    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(netH);

    if(pNetIf != NULL)
    {
        if((pNetIf->Flags.bInterfaceEnabled != 0U) || (pNetIf->Flags.bMacInitialize != 0U))
        {   // already up
            return true;
        }

        if(pUsrConfig == NULL)
        {   // no configuration present
            return false;
        }

        // if this is a alias interface, the primary should be up and running!
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        if(!TCPIPStackNetIsPrimary(pNetIf))
        {
            TCPIP_NET_IF* pPriIf = TCPIPStackNetGetPrimary(pNetIf);
            if((pPriIf->Flags.bInterfaceEnabled == 0U) && (pPriIf->Flags.bMacInitialize == 0U))
            {
                return false;
            }
        }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

        // Before we load the default config, we should save what used to be the netIfIx
        // set transient data
        tcpip_stack_ctrl_data.pNetIf = pNetIf;
        tcpip_stack_ctrl_data.netIx = pNetIf->netIfIx;
        tcpip_stack_ctrl_data.stackAction = (uint8_t)TCPIP_STACK_ACTION_IF_UP;

        if(!F_LoadNetworkConfig(pUsrConfig, pNetIf, true))
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Network configuration load failed: %d\r\n", pNetIf->netIfIx);
            return false;
        }

        powerMode = TCPIP_Helper_StringToPowerMode(pUsrConfig->powerMode);
        if((powerMode != TCPIP_MAC_POWER_FULL) && (powerMode != TCPIP_MAC_POWER_DOWN))
        {   
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Power Mode initialization fail: %d\r\n", powerMode);
            return false;
        }

        tcpip_stack_ctrl_data.powerMode = (uint8_t)powerMode;
        success = TCPIP_STACK_BringNetUp(&tcpip_stack_ctrl_data, pUsrConfig, NULL, 0U);
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
    TCPIP_NET_IF* pDownIf = TCPIPStackHandleToNet(netH);

    if(pDownIf != NULL)
    {
        if((pDownIf->Flags.bInterfaceEnabled != 0U) || (pDownIf->Flags.bMacInitialize != 0U))
        {
            // kill interface
            // if this is primary, kill all its aliases
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            if(TCPIPStackNetIsPrimary(pDownIf))
            {
                TCPIP_NET_IF *pIf;
                for(pIf = TCPIPStackNetGetAlias(pDownIf); pIf != NULL; pIf = TCPIPStackNetGetAlias(pIf)) 
                {
                    TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
                }
            }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pDownIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
            F_TCPIP_SelectDefaultNet(pDownIf);
        }
        return true;
    }

    return false;

}
#endif  // (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)


#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_STACK_Deinitialize(SYS_MODULE_OBJ object)
{
    if((object != (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data) || (tcpipNetIf == NULL))
    {   // invalid handle/already dead
        return;
    }

    TCPIP_STACK_KillStack();
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

// kills the stack
static void TCPIP_STACK_KillStack(void)
{
    int32_t         netIx;
    TCPIP_NET_IF* pNetIf;

#if (TCPIP_STACK_DOWN_OPERATION != 0)
    if (tcpip_stack_tickH != SYS_TMR_HANDLE_INVALID)
    {
        SYS_TMR_CallbackStop(tcpip_stack_tickH);
        tcpip_stack_tickH = SYS_TMR_HANDLE_INVALID;
    }
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

    // kill interfces
    pNetIf = tcpipNetIf;
    for(netIx = 0; netIx < (int32_t)tcpip_stack_ctrl_data.nIfs; netIx++)
    {
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        if(TCPIPStackNetIsPrimary(pNetIf))
        {   // first kill the aliases!
            TCPIP_NET_IF* pAlias;
            for(pAlias = TCPIPStackNetGetAlias(pNetIf); pAlias != NULL; pAlias = TCPIPStackNetGetAlias(pAlias)) 
            {
                TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pAlias, TCPIP_STACK_ACTION_DEINIT, TCPIP_MAC_POWER_DOWN);
            }
            // kill primary interface
            TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_DEINIT, TCPIP_MAC_POWER_DOWN);
        }
#else
        // kill primary interface
        TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_DEINIT, TCPIP_MAC_POWER_DOWN);
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        pNetIf++;
    }


#if (TCPIP_STACK_DOWN_OPERATION != 0)
    TCPIP_PKT_Deinitialize();
    if(tcpip_stack_ctrl_data.memH != NULL)
    {
        (void) TCPIP_HEAP_Free(tcpip_stack_ctrl_data.memH, tcpipNetIf);
        if((uint32_t)TCPIP_HEAP_Delete(tcpip_stack_ctrl_data.memH) < 0U)     // destroy the heap
        {
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)    
            int32_t     ix, nEntries, nTraces;
            TCPIP_HEAP_TRACE_ENTRY    tEntry;

            TCPIP_STACK_HEAP_HANDLE heapH = tcpip_stack_ctrl_data.memH;
            nTraces = (int32_t)TCPIP_HEAP_TraceGetEntriesNo(heapH, true);
            if(nTraces != 0)
            {
                SYS_CONSOLE_PRINT(TCPIP_STACK_HDR_MESSAGE "Heap Delete fail! Trace info: \r\n");
                nEntries = (int32_t)TCPIP_HEAP_TraceGetEntriesNo(heapH, false);
                for(ix = 0; ix < nEntries; ix++)
                {
                    if(TCPIP_HEAP_TraceGetEntry(heapH, (uint32_t)ix, &tEntry))
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

    tcpip_stack_ctrl_data.memH = NULL;
    tcpip_stack_ctrl_data.heapType = (TCPIP_STACK_HEAP_TYPE)0;
    tcpipNetIf = NULL;

    tcpip_stack_ctrl_data.nIfs = 0;
    tcpip_stack_ctrl_data.nModules = 0;
    tcpip_stack_status = SYS_STATUS_UNINITIALIZED;
    stackAsyncSignalCount = 0;
    (void) memset(&tcpip_heap_config, 0, sizeof(tcpip_heap_config));
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

    if(pStackInit != NULL)
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
    stackCtrlData->stackAction = (uint8_t)action;
    stackCtrlData->powerMode = (uint8_t)powerMode;

    // Go to the last entry in the table
    pEntry = TCPIP_STACK_MODULE_ENTRY_TBL + (sizeof(TCPIP_STACK_MODULE_ENTRY_TBL)/sizeof(*TCPIP_STACK_MODULE_ENTRY_TBL));
    do
    {
        pEntry--;
        pEntry->deInitFunc(stackCtrlData);
    }
    while (pEntry != TCPIP_STACK_MODULE_ENTRY_TBL);

    TCPIPStackModuleRxPurge(TCPIP_MODULE_MANAGER, pNetIf);
    if(TCPIPStackNetIsPrimary(pNetIf))
    {   // primary interface
        if(pNetIf->hIfMac != 0U)
        {
            (*pNetIf->pMacObj->MAC_Close)(pNetIf->hIfMac);
        }

        if(pNetIf->macObjHandle != 0U)
        {   // kill the MAC
            (*pNetIf->pMacObj->MAC_Deinitialize)(pNetIf->macObjHandle);
        }
    }

    pNetIf->hIfMac = 0;
    pNetIf->macObjHandle = 0;

    pNetIf->Flags.bInterfaceEnabled = 0;
    pNetIf->Flags.bMacInitialize = 0;

#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)
    if(TCPIPStackNetIsPrimary(pNetIf))
    {
        TCPIP_Notification_Deinitialize(&pNetIf->registeredClients, tcpip_stack_ctrl_data.memH);
    }
#endif
}
#else
// light de-initialization: modules are not de initialized
static void TCPIP_STACK_BringNetDown(TCPIP_STACK_MODULE_CTRL* stackCtrlData, TCPIP_NET_IF* pNetIf, TCPIP_STACK_ACTION action, TCPIP_MAC_POWER_MODE powerMode)
{

    if(TCPIPStackNetIsPrimary(pNetIf))
    {   // primary interface
        // kill the MAC anyway
        if(pNetIf->hIfMac != 0)
        {
            (*pNetIf->pMacObj->MAC_Close)(pNetIf->hIfMac);
        }

        if(pNetIf->macObjHandle != 0)
        {   
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
            (*pNetIf->pMacObj->MAC_Deinitialize)(pNetIf->macObjHandle);
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
        }
    }

    pNetIf->hIfMac = 0;
    pNetIf->macObjHandle = 0;

    pNetIf->Flags.bInterfaceEnabled = pNetIf->Flags.bMacInitialize = false;

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)



// create the stack tick timer
static bool F_TCPIPStackCreateTimer(void)
{
    bool createRes = false;

    tcpip_stack_tickH = SYS_TMR_CallbackPeriodic(TCPIP_STACK_TICK_RATE, 0, &F_TCPIP_STACK_TickHandler);
    if(tcpip_stack_tickH != SYS_TMR_HANDLE_INVALID)
    {
        uint32_t sysRes = SYS_TMR_TickCounterFrequencyGet();
        uint32_t rateMs = ((sysRes * (uint32_t)TCPIP_STACK_TICK_RATE) + 999U ) / 1000U;    // round up
        stackTaskRate = (rateMs * 1000U) / sysRes;
        // make sure the rate is no greater than max value for int16_t!
        if(stackTaskRate < (((uint32_t)1U << 15) - 1U))
        {
            // adjust the link rate to be a multiple of the stack task rate
            uint32_t linkTmo = ((((uint32_t)M_TCPIP_STACK_LINK_RATE + stackTaskRate - 1U) / stackTaskRate) * stackTaskRate); 
            // check that it's a proper int32_t type
            if(linkTmo < (((uint32_t)1U << 31) - 1U))
            {
                stackLinkTmo = (int32_t)linkTmo;
                // SYS_TMR_CallbackPeriodicSetRate(tcpip_stack_tickH, rateMs);
                // adjust module timeouts
                createRes = F_TCPIPStack_AdjustTimeouts();
            }
        }
    }

    if(createRes == false)
    {
        SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Tick registration failed: %d\r\n", TCPIP_STACK_TICK_RATE);
    }

    return createRes;

}

// makes sure that the modules have a proper timeout value
// when the stackTaskRate is calculated
static bool F_TCPIPStack_AdjustTimeouts(void)
{
    size_t     modIx;
    TCPIP_MODULE_SIGNAL_ENTRY*  pSigEntry;

    pSigEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + (uint32_t)TCPIP_MODULE_LAYER1;
    for(modIx = (size_t)TCPIP_MODULE_LAYER1; modIx < (sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL) / sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL)); modIx++)
    {
        if((pSigEntry->signalHandler != NULL) && (pSigEntry->asyncTmo != 0))
        {
            if(!TCPIPStackSignalHandlerSetParams((TCPIP_STACK_MODULE)modIx, pSigEntry, pSigEntry->asyncTmo))
            {   // should NOT happen
                return false;
            }
        }
        pSigEntry++;
    }
    return true;
}

void TCPIP_STACK_Task(SYS_MODULE_OBJ object)
{
    size_t              netIx, modIx;
    TCPIP_NET_IF        *pNetIf, *pAliasIf;
    TCPIP_MAC_EVENT     activeEvents;
    bool                eventPending;
    bool                wasTickEvent;
    uint32_t            events;
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)
    TCPIP_EVENT         tcpipEvent;
    TCPIP_EVENT_LIST_NODE* tNode;
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)   

    if(object != (SYS_MODULE_OBJ)&tcpip_stack_ctrl_data)
    {   // invalid handle/nothing to do
        return;
    }

    if((tcpip_stack_status != SYS_STATUS_BUSY) && (tcpip_stack_status != SYS_STATUS_READY))
    {   // some error state
        return;
    }

    if(tcpipNetIf == NULL)
    {   // call the user callback...
        F_TCPIP_InitCallback(tcpip_stack_init_cb);
        return;
    }

    if(!F_TCPIPStackIsRunState())
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
    F_TCPIP_SelectDefaultNet(NULL);

    // check stack signals
    eventPending = TCPIP_STACK_CheckEventsPending();
    if((eventPending == false) && (stackAsyncSignalCount == 0U))
    {   // process only when events are pending or modules need async attention
        return;
    }

    if(newTcpipTickAvlbl != 0)
    {
        wasTickEvent = true;
        F_TCPIP_ProcessTickEvent();
    }
    else
    {
        wasTickEvent = false;
    }

    if( totTcpipEventsCnt != 0)
    {   // there are MAC events pending
        totTcpipEventsCnt = 0;

        pNetIf = tcpipNetIf;
        for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
        {
            if ((pNetIf->Flags.bInterfaceEnabled == 0U) || !TCPIPStackNetIsPrimary(pNetIf))
            {
                pNetIf++;
                continue;
            }
            activeEvents =  (TCPIP_MAC_EVENT)pNetIf->activeEvents;

#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
            // get a fresh copy
            events = (uint32_t)activeEvents | (uint32_t)((*pNetIf->pMacObj->MAC_EventPendingGet)(pNetIf->hIfMac));
            activeEvents = (TCPIP_MAC_EVENT)events;
#else
            events = (uint32_t)activeEvents | (uint32_t) (TCPIP_MAC_EV_RX_DONE | TCPIP_MAC_EV_TX_DONE);    // just fake pending events
            activeEvents = (TCPIP_MAC_EVENT)events;
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

            // clear processed events
            F_TCPIP_ClearMacEvent(pNetIf, activeEvents);
            pNetIf->currEvents |= (uint16_t)activeEvents;     // store all the processed events

            // acknowledge MAC events
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
            (void) (*pNetIf->pMacObj->MAC_EventAcknowledge)(pNetIf->hIfMac, activeEvents);
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
            if(((uint32_t)activeEvents & (TCPIP_STACK_MAC_ACTIVE_RX_EVENTS)) != 0U)
            {
                (void) F_TCPIPExtractMacRxPackets(pNetIf);
                newTcpipStackEventCnt++;
            }

            if(pNetIf->Flags.bMacProcessOnEvent != 0U)
            {   // normal MAC internal processing
                (void) (*pNetIf->pMacObj->MAC_Process)(pNetIf->hIfMac);
            }
            pNetIf++;
        }

        (void) F_TCPIPProcessMacPackets(true);

        // clear the pending RX signal so it's not reported
        (void) F_TCPIPStackManagerSignalClear(TCPIP_MODULE_SIGNAL_RX_PENDING);
    }

    // process connection related and error events    
    pNetIf = tcpipNetIf;    
    for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
    {
        if ((pNetIf->Flags.bInterfaceEnabled == 0U) || !TCPIPStackNetIsPrimary(pNetIf))
        {
            pNetIf++;
            continue;
        }

        activeEvents = (TCPIP_MAC_EVENT)pNetIf->currEvents;
        if(((uint32_t)activeEvents & (uint32_t)TCPIP_MAC_EV_RXTX_ERRORS) != 0U)
        {    // some error has occurred
            F_TCPIP_ProcessMACErrorEvents(pNetIf, activeEvents);
        }

        if(pNetIf->exFlags.connEvent != 0U)
        {   // connection related event
            if(pNetIf->exFlags.connEventType != 0U)
            {
                events = (uint32_t)activeEvents | (uint32_t)TCPIP_MAC_EV_CONN_ESTABLISHED;
                activeEvents = (TCPIP_MAC_EVENT)events;
#if defined(TCPIP_STACK_USE_PPP_INTERFACE)
                if(TCPIPStack_NetMacType(pNetIf) == TCPIP_MAC_TYPE_PPP)
                {   // for a PPP interface we set the IP address obtained from negotiation
                    IPV4_ADDR lclIpAddr;
                    lclIpAddr.Val = PPP_GetLocalIpv4Addr(pNetIf->hIfMac);
                    F_TCPIPStackSetIpAddress(pNetIf, &lclIpAddr, NULL, NULL, false);
                }
#endif  // defined(TCPIP_STACK_USE_PPP_INTERFACE)
            }
            else
            {
                events = (uint32_t)activeEvents | (uint32_t)TCPIP_MAC_EV_CONN_LOST;
                activeEvents = (TCPIP_MAC_EVENT)events;
            } 
            pNetIf->exFlags.connEvent = 0;

            for(pAliasIf = TCPIPStackNetGetPrimary(pNetIf); pAliasIf != NULL; pAliasIf = TCPIPStackNetGetAlias(pAliasIf))
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

            tNode = FC_ListNode2EventNode(pNetIf->registeredClients.list.head); 
            while(tNode != NULL)
            {
                if(((uint32_t)tNode->evMask & (uint32_t)tcpipEvent) != 0U)
                {   // trigger event
                    (*tNode->handler)(pNetIf, tcpipEvent, tNode->hParam);
                }
                tNode = tNode->next;
            }
            TCPIP_Notification_Unlock(&pNetIf->registeredClients);
        }
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)

        pNetIf->currEvents = 0;
        pNetIf++;
    }

    if(wasTickEvent)
    {   // send the timeout signals
        F_TCPIPStackSignalTmo();
        // clear the TMO signal so it's not reported anymore
        (void) F_TCPIPStackManagerSignalClear(TCPIP_MODULE_SIGNAL_TMO);
    }

#if !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)
    // execute the signal tasks here, instead of letting the app handle that
    F_TCPIPStackExecuteModules();
    if(stackAsyncSignalCount != 0U)
    {   // when executing the tasks internally
        // signal that attention is required
        F_TCPIPSignalEntryNotify(F_TCPIPModuleToSignalEntry(TCPIP_MODULE_MANAGER), TCPIP_MODULE_SIGNAL_ASYNC, 0);
    }
#endif  // !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)

#if defined(TCPIP_STACK_TIME_MEASUREMENT)
    if(tcpip_stack_timeEnable)
    {
        tcpip_stack_time += _CP0_GET_COUNT() - tTaskStart;
    }
#endif // defined(TCPIP_STACK_TIME_MEASUREMENT)

}

static bool F_TCPIPStackIsRunState(void)
{
    size_t          netIx;
    TCPIP_NET_IF   *pNetIf, *pPriIf;
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    TCPIP_NET_IF   *pAliasIf;
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

    // check that we've created the needed timer
    if((tcpip_stack_status == SYS_STATUS_BUSY) && (tcpip_stack_tickH == SYS_TMR_HANDLE_INVALID))
    {   // check that we can create a timer
        SYS_STATUS tmrStat = SYS_TMR_ModuleStatusGet(0);
        if(tmrStat == SYS_STATUS_BUSY)
        {   // not ready yet
            return false;
        }

        if(tmrStat == SYS_STATUS_READY)
        {   // try to create the timer
            if(F_TCPIPStackCreateTimer() == false)
            {   // failed
                tmrStat = SYS_STATUS_ERROR;
            }
        }
        
        if(tmrStat <= (SYS_STATUS)0)
        {   // something went wrong...
            TCPIP_STACK_KillStack();
            tcpip_stack_status = SYS_STATUS_ERROR;
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Initialization SYS TMR failed: %d - Aborting! \r\n", tmrStat);
            return false;
        }
    }

    pNetIf = tcpipNetIf;
    for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
    {
        if((pNetIf->macObjHandle != 0U) && TCPIPStackNetIsPrimary(pNetIf))
        {
            // process the underlying MAC module tasks
            (*pNetIf->pMacObj->MAC_Tasks)(pNetIf->macObjHandle);

            // check if we're just starting up some interfaces
            if(pNetIf->Flags.bMacInitialize != 0U)
            {   // initializing
                SYS_STATUS macStat = (*pNetIf->pMacObj->MAC_Status)(pNetIf->macObjHandle);
                if(macStat < (SYS_STATUS)0)
                {   // failed; kill the interface
                    TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
                    pNetIf->Flags.bMacInitDone = 1;
                }
                else if(macStat == SYS_STATUS_READY)
                {   // get the MAC address and MAC processing flags
                    // set the default MTU; MAC driver will override if needed
                    TCPIP_MAC_PARAMETERS macParams = {{{0}}};
                    macParams.linkMtu = TCPIP_MAC_LINK_MTU_DEFAULT; 
                    macParams.macTxPrioNum = 1U; 
                    macParams.macRxPrioNum = 1U;    // se the default priority queue numbers 
                    (void) (*pNetIf->pMacObj->MAC_ParametersGet)(pNetIf->hIfMac, &macParams);
                    (void) memcpy(pNetIf->netMACAddr.v, macParams.ifPhyAddress.v, sizeof(pNetIf->netMACAddr));
                    pNetIf->Flags.bMacProcessOnEvent = (uint8_t)(macParams.processFlags != TCPIP_MAC_PROCESS_FLAG_NONE);
                    pNetIf->linkMtu = (uint16_t)macParams.linkMtu;
                    pNetIf->txOffload = (uint8_t)macParams.checksumOffloadTx;
                    pNetIf->rxOffload = (uint8_t)macParams.checksumOffloadRx;
                    pNetIf->txPriNum = macParams.macTxPrioNum;
                    pNetIf->rxPriNum = macParams.macRxPrioNum;

                    // enable this interface
                    pNetIf->Flags.bInterfaceEnabled = 1;
                    pNetIf->Flags.bMacInitialize = 0;
                    pNetIf->Flags.bMacInitDone = 1;

#if defined(TCPIP_STACK_USE_PPP_INTERFACE)
                    if(TCPIPStack_NetMacType(pNetIf) == TCPIP_MAC_TYPE_PPP)
                    {   // for a PPP interface we set the IP address obtained from negotiation
                        IPV4_ADDR lclIpAddr;
                        lclIpAddr.Val = 0;
                        F_TCPIPStackSetIpAddress(pNetIf, &lclIpAddr, NULL, NULL, true);
                    }
#endif  // defined(TCPIP_STACK_USE_PPP_INTERFACE)

#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                    pAliasIf = TCPIPStackNetGetAlias(pNetIf);
                    while(pAliasIf != NULL)
                    {
                        F_TCPIPCopyMacAliasIf(pAliasIf, pNetIf);
                        pAliasIf->Flags.bMacInitialize = 0;
                        pAliasIf->Flags.bMacInitDone = 1;
                        if(pAliasIf->Flags.powerMode == (uint16_t)TCPIP_MAC_POWER_FULL)
                        {   // enable the alias interface too;
                            pAliasIf->Flags.bInterfaceEnabled = 1;
                        }
                        pAliasIf = TCPIPStackNetGetAlias(pAliasIf);                        
                    }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
        pNetIf++;
    }

    // check we're getting out of Initialize procedure
    if(tcpip_stack_status == SYS_STATUS_BUSY)
    {
        uint32_t    ifUpMask = 0;   // masks of interfaces that are up;
        // max 32 interfaces are assumed for now!
        netIx = 0; 
        pNetIf = tcpipNetIf; 
        while(netIx < (size_t)tcpip_stack_ctrl_data.nIfs) 
        {
            pPriIf = TCPIPStackNetGetPrimary(pNetIf);    // look at the primary initialization

            if(pPriIf->Flags.powerMode == (uint16_t)TCPIP_MAC_POWER_FULL)
            {
                if(pPriIf->Flags.bMacInitDone == 0U)
                {
                    return false;  // not done
                }
                else if(pPriIf->Flags.bInterfaceEnabled != 0U)
                {
                    ifUpMask |= (1UL << netIx);
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else
            {   // done
                ifUpMask |= (1UL << netIx);
            }
            netIx++;
            pNetIf++;
        }

        // passed through all interfaces
        if(ifUpMask == ((1UL << netIx) - 1U))
        {   // all interfaces up
            tcpip_stack_status = SYS_STATUS_READY;
            SYS_CONSOLE_MESSAGE(TCPIP_STACK_HDR_MESSAGE "Initialization Ended - success \r\n");
        }
        else
        {   // failed initializing all interfaces;
            pNetIf = tcpipNetIf;
            for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
            {
                if((pNetIf->Flags.bInterfaceEnabled != 0U) || (pNetIf->Flags.bMacInitialize != 0U))
                {
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                    if(TCPIPStackNetIsPrimary(pNetIf))
                    {
                        for(pAliasIf = TCPIPStackNetGetAlias(pNetIf); pAliasIf != NULL; pAliasIf = TCPIPStackNetGetAlias(pAliasIf))
                        {
                            TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pAliasIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
                        }

                        TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
                    }
#else
                    TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                }
                pNetIf++;
            }
            tcpip_stack_status = SYS_STATUS_ERROR;
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Interface Initialization failed: 0x%x - Aborting! \r\n", ifUpMask);
            return false;
        }
    }

    return true;
}

// set the interfaces number/index and name
static void F_TCPIPStackSetIfNumberName(void)
{ 
    size_t netIx;
    TCPIP_NET_IF* pNetIf;
    TCPIP_MAC_TYPE macType;
    size_t ifNumber[TCPIP_MAC_TYPES];

    (void) memset(ifNumber, 0, sizeof(ifNumber));

#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    size_t priIfIx = 0;
#endif // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    pNetIf = tcpipNetIf;
    for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
    {
        // set the interface number
        pNetIf->netIfIx = (uint16_t)netIx;
        // set the interfaces name
        // and update the alias interfaces
        if(TCPIPStackNetIsPrimary(pNetIf))
        {
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            pNetIf->priIfIx = priIfIx++;  // store the primary abs index
#endif // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            macType = (TCPIP_MAC_TYPE)pNetIf->macType;
            const char* ifName = TCPIP_STACK_IF_ALIAS_NAME_TBL[macType]; 
            (void) FC_sprintf(pNetIf->ifName, sizeof(pNetIf->ifName), "%s%zu", ifName, ifNumber[macType]);
            pNetIf->ifName[sizeof(pNetIf->ifName) - 1U] = (char)0;
            // update all its aliases
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            int aliasIx;
            TCPIP_NET_IF* pAliasIf;
            aliasIx = 0;
            for(pAliasIf = TCPIPStackNetGetAlias(pNetIf); pAliasIf != NULL; pAliasIf = TCPIPStackNetGetAlias(pAliasIf))
            {
                (void) FC_sprintf(pAliasIf->ifName, sizeof(pAliasIf->ifName), "%s%zu:%d", ifName, ifNumber[macType], aliasIx % 10);
                pAliasIf->ifName[sizeof(pAliasIf->ifName) - 1U] = (char)0;
                aliasIx++;
            }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            ifNumber[macType]++;
        }
        pNetIf++;
    }
}

#if !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)
static void F_TCPIPStackExecuteModules(void)
{
    size_t      modIx;
    TCPIP_MODULE_SIGNAL_ENTRY*  pSigEntry;
    tcpipModuleSignalHandler    signalHandler;
    uint16_t                    signalVal;
    OSAL_CRITSECT_DATA_TYPE     critSect;

    pSigEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + (size_t)TCPIP_MODULE_LAYER1;
    for(modIx = (size_t)TCPIP_MODULE_LAYER1; modIx < (sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL) / sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL)); modIx++)
    {
        critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        signalHandler = pSigEntry->signalHandler;
        signalVal = pSigEntry->signalVal;
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

        if((signalHandler != NULL) && (signalVal != 0U))
        {   // used slot and pending signals
            // pending signals; either TMO or RX related or ASYNC
            // execute the handler-> module Task function
            // Note: this can set signals for sibling modules!
            (*signalHandler)();
        }
        pSigEntry++;
    }
}
#endif  // !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)

static void F_TCPIP_ProcessTickEvent(void)
{
    size_t      netIx;
    bool        linkCurr, linkPrev;
    TCPIP_NET_IF* pNetIf;

    newTcpipTickAvlbl = 0;

    F_TCPIP_SecondCountSet();    // update time
    stackLinkTmo -= (int32_t)stackTaskRate;
    if(stackLinkTmo <= 0)
    {   // timeout exceeded 
        stackLinkTmo += M_TCPIP_STACK_LINK_RATE; 
        pNetIf = tcpipNetIf;
        for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
        {
            if(pNetIf->Flags.bInterfaceEnabled != 0U)
            {
                linkCurr = (*pNetIf->pMacObj->MAC_LinkCheck)(pNetIf->hIfMac);     // check link status
                linkPrev = pNetIf->exFlags.linkPrev != 0U;
                if(linkPrev != linkCurr)
                {   // link status changed
                    // just set directly the events, and do not involve the MAC notification mechanism
                    pNetIf->exFlags.connEvent = 1;
                    pNetIf->exFlags.connEventType = linkCurr ? 1U : 0U ;
                    pNetIf->exFlags.linkPrev = (uint8_t)linkCurr;
                }
            }
            pNetIf++;
        }
    }

}

#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VCHK_XPKT) != 0) 
uint32_t dbgVFailRx1 = 0;
uint32_t dbgVFailRx2 = 0;
uint32_t dbgVFailRx3 = 0;
uint32_t dbgVFailCheckCnt = 0;
static void F_DbgCheckRxPkt(TCPIP_MAC_PACKET* pRxPkt, int code)
{
    int failChk = 0;
    TCPIP_MAC_DATA_SEGMENT* pSeg = pRxPkt->pDSeg;
    uint16_t* pSegBuffer = (uint16_t*)pSeg->segBuffer;
    if(((uint32_t)pSegBuffer & 0x1U) != 0U)
    {
        dbgVFailRx1++;
        failChk++;
    }

    // check the packet segment
    TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(pSeg->segBuffer + pktGapDcptOffset);
    if(pGap->gapSign != pktGapSign)
    {
        dbgVFailRx2++;
        failChk++;
    }
    TCPIP_MAC_PACKET* pGapPkt = pGap->segmentPktPtr;
    if(pGapPkt != pRxPkt)
    {
        dbgVFailRx3++;
        failChk++;
    }


    if(failChk != 0)
    {
        failChk++;
        failChk++;
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
    }
    dbgVFailCheckCnt++;
}
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VCHK_XPKT) != 0) 


static int F_TCPIPExtractMacRxPackets(TCPIP_NET_IF* pNetIf)
{
    TCPIP_MAC_PACKET*       pRxPkt;
    int32_t     nPackets = 0;

    // get all the new MAC packets
    while((pRxPkt = (*pNetIf->pMacObj->MAC_PacketRx)(pNetIf->hIfMac, NULL, NULL)) != NULL)
    {
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VCHK_XPKT) != 0) 
        F_DbgCheckRxPkt(pRxPkt, 0);
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VCHK_XPKT) != 0) 
        F_TCPIPInsertMacRxPacket(pNetIf, pRxPkt);
        TCPIP_PKT_FlightLogRx(pRxPkt, (TCPIP_STACK_MODULE)pNetIf->macId);
        nPackets++;
    }

    return nPackets;
}

#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
// returns a primary/alias interface running this VLAN
// returns NULL if not found
// pInIf is the interface on which the packet arrived
// it should be a primary interface - unless the packet was internally routed
TCPIP_NET_IF* F_TCPIPMapVlanInterface(const TCPIP_NET_IF* pInIf, uint16_t vlanId)
{
    if(pInIf->vlanId == vlanId)
    {
        return FC_CNetIf2NetIf(pInIf);
    }
    // search an alias
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    TCPIP_NET_IF* pAlias;
    for(pAlias = TCPIPStackNetGetAlias(pInIf); pAlias != NULL; pAlias = TCPIPStackNetGetAlias(pAlias)) 
    {
        if(pAlias->vlanId == vlanId)
        {
            return pAlias;
        }
    }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    return NULL;
}

#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VRETR_F) != 0) 
// debug VLAN retrieve packet function
typedef struct
{
    uint32_t retr1;
    uint32_t retr2;
    uint32_t retr3;
}TCPIP_VLAN_RETR_STRUCT;

TCPIP_VLAN_RETR_STRUCT dbgRxRetr = {0};
TCPIP_VLAN_RETR_STRUCT dbgTxRetr = {0};
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VRETR_F) != 0) 

// helper to retrieve the pointer to the corresponding TCPIP_MAC_PACKET from a buffer pointer
// segLoad should be the pointer that the MAC uses for TX, i.e. pPkt->pDSeg->segLoad!
static TCPIP_MAC_PACKET* F_TCPIP_MAC_PKT_Retrieve(uint8_t* segLoad, TCPIP_MAC_RETRIEVE_REQUEST retrReq)
{
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VRETR_F) != 0) 
    TCPIP_VLAN_RETR_STRUCT* pRetr = (retrReq == TCPIP_MAC_RETRIEVE_RX) ? &dbgRxRetr : &dbgTxRetr;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VRETR_F) != 0) 

    // check if an untagged packet
    uint8_t* segBuffer = (uint8_t*)(FC_CvPtr2U32(segLoad) & 0xfffffffcU);   // Note: TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2 always set! 
    uint8_t* ptrGap = segBuffer + pktGapDcptOffset; 

    TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = FC_Uptr2MacGapDcpt(ptrGap);
    if(pGap->gapSign != pktGapSign)
    {   // that didn't work, may be an VLAN packet
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VCHK_SEGB) != 0) 
        segBuffer -= sizeof(TCPIP_MAC_ETHERNET_VLAN_HEADER) - sizeof(TCPIP_MAC_ETHERNET_HEADER);
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VCHK_SEGB) != 0) 
        ptrGap -= sizeof(TCPIP_MAC_ETHERNET_VLAN_HEADER) - sizeof(TCPIP_MAC_ETHERNET_HEADER);
        pGap = FC_Uptr2MacGapDcpt(ptrGap);
        if(pGap->gapSign != pktGapSign)
        {   // should NOT happen!
            TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VRETR_F) != 0) 
            pRetr->retr3++;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VRETR_F) != 0) 
            return NULL;
        }
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VRETR_F) != 0) 
        else
        {
            pRetr->retr2++;
        }
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VRETR_F) != 0) 
    }
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VRETR_F) != 0) 
    else
    {
        pRetr->retr1++;
    }
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VRETR_F) != 0) 

    TCPIP_MAC_PACKET* pPkt = pGap->segmentPktPtr;

#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VCHK_SEGB) != 0) 
    TCPIPStack_Assert(segBuffer == pPkt->pDSeg->segBuffer, __FILE__, __func__, __LINE__);
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VCHK_SEGB) != 0) 

    return pPkt;
}

#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VTRACE) != 0) 
// debug trace VLAN packets
typedef struct
{
    TCPIP_MAC_PACKET* pPkt;
    uint8_t*  segBuffer;
    uint8_t*  segLoad;
    uint8_t*  pMacLayer;
}TCPIP_VLAN_TRACE_STRUCT;

TCPIP_VLAN_TRACE_STRUCT tcpipVlanTrace[M_TCPIP_STACK_DEBUG_VTRACE_SIZE] = {0};
size_t tcpipVlanTraceIx = 0;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VTRACE) != 0) 


static void F_TCPIP_PKT_VlanAdjust(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET * ptrPacket)
{
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
    TCPIP_NET_IF* pStatIf = FC_CNetIf2NetIf(pNetIf);  // update statistics
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 

    bool setVlan = false;
    TCPIP_MAC_DATA_SEGMENT* pDSeg = ptrPacket->pDSeg;

    if(pNetIf->vlanId != 0U)
    {
        setVlan = true;
    }
    else if((ptrPacket->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_TX) != 0U)
    {   // an TX allocated packet may need to be adjusted
        if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_VLAN_USE_VID_NULL) != 0U)
        {   // transmit the VLAN anyway
            setVlan = true;
        }
        else
        {   // not vlan interface
            if(ptrPacket->pNetLayer == ptrPacket->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_VLAN_HEADER))
            {   // the ETH header is too long; adjust the segLoad, pass over the extra not-transmitted 4 bytes
                pDSeg->segLoad += sizeof(TCPIP_MAC_ETHERNET_VLAN_HEADER) - sizeof(TCPIP_MAC_ETHERNET_HEADER);
                (void)memmove(pDSeg->segLoad, ptrPacket->pMacLayer, sizeof(TCPIP_MAC_ETHERNET_HEADER));
                ptrPacket->pMacLayer = pDSeg->segLoad;
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
                pStatIf->vAdjustTxCnt++;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
            }
            else
            {   // this can happen for an internal looped packet
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
                pStatIf->vOkTxCnt++;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
            }
        }
    }
    else
    {
        // do NOT modify an RX packet that gets transmitted again (ICMP)!
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
        pStatIf->vRxTxCnt++;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
    }

    if(setVlan)
    {   // true/NULL vlan packet
        TCPIP_8021Q_TAG vlanTag;
        pDSeg->segLen += sizeof(TCPIP_MAC_ETHERNET_VLAN_HEADER) - sizeof(TCPIP_MAC_ETHERNET_HEADER);
        TCPIP_MAC_ETHERNET_VLAN_HEADER* pVlanHdr = (TCPIP_MAC_ETHERNET_VLAN_HEADER*)ptrPacket->pMacLayer;
        TCPIP_MAC_ETHERNET_HEADER* pEthHdr = (TCPIP_MAC_ETHERNET_HEADER*)ptrPacket->pMacLayer;
        pVlanHdr->Type = pEthHdr->Type;

        pVlanHdr->vlanTag.tpid = TCPIP_Helper_htons(TCPIP_ETHER_TYPE_C_TPID);
        vlanTag.vid = pNetIf->vlanId;
        vlanTag.pcp = pNetIf->vlanPcp;
        vlanTag.dei = (pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_VLAN_DEI) == 0U ? 0 : 1;
        pVlanHdr->vlanTag.tci = TCPIP_Helper_htons(vlanTag.tci);
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
        pStatIf->vlanTxCnt++;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
    }

#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VTRACE) != 0) 
    TCPIP_VLAN_TRACE_STRUCT* pTrace = tcpipVlanTrace + tcpipVlanTraceIx;
    pTrace->pPkt = ptrPacket;
    pTrace->segBuffer = ptrPacket->pDSeg->segBuffer;
    pTrace->segLoad = ptrPacket->pDSeg->segLoad;
    pTrace->pMacLayer = ptrPacket->pMacLayer;
    if(++tcpipVlanTraceIx == sizeof(tcpipVlanTrace) / sizeof(*tcpipVlanTrace))
    {
        tcpipVlanTraceIx = 0;
    } 
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VTRACE) != 0) 
}

#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
// Process the queued RX packets
// returns the mask of 1st layer frames that have been processed
// signals the 1st layer modules if needed
static uint32_t F_TCPIPProcessMacPackets(bool signal_t)
{
    size_t                      frameIx;
    bool                        frameFound;
    uint16_t                    frameType;
    TCPIP_MAC_PACKET*           pRxPkt;
    TCPIP_MAC_ETHERNET_HEADER*  pMacHdr;
    const TCPIP_FRAME_PROCESS_ENTRY*  pFrameEntry;
    uint32_t                    procFrameMask = 0;

    SINGLE_LIST* pPktQueue = (TCPIP_MODULES_QUEUE_TBL + (size_t)TCPIP_MODULE_MANAGER);

    while(true)
    {
        pRxPkt = FC_SglNode2MacPkt(TCPIP_Helper_SingleListHeadRemove(pPktQueue));
        if(pRxPkt == NULL)
        {   // done
            break;
        }

        // process packet
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
        

        pMacHdr = FC_Uptr2MacEthHdr(pRxPkt->pDSeg->segLoad);
        // get the packet type
        frameType = TCPIP_Helper_ntohs(pMacHdr->Type);

        // re-set pMacLayer and pNetLayer; IPv6 changes these pointers inside the packet!
        pRxPkt->pMacLayer = pRxPkt->pDSeg->segLoad;

#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
        if(frameType == TCPIP_ETHER_TYPE_C_TPID)
        {   // VLAN tagged frame
            pRxPkt->pDSeg->segLen -= (uint16_t)(sizeof(TCPIP_MAC_ETHERNET_VLAN_HEADER));
            pRxPkt->pNetLayer = pRxPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_VLAN_HEADER);
        }
        else
#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
        {   // if VLAN is not enabled this packet type will be discarded
            pRxPkt->pDSeg->segLen -= (uint16_t)(sizeof(TCPIP_MAC_ETHERNET_HEADER));
            pRxPkt->pNetLayer = pRxPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);
        }

#if (TCPIP_STACK_EXTERN_PACKET_PROCESS != 0)
        TCPIP_NET_IF* pNetIf = FC_Cvptr2NetIf(pRxPkt->pktIf);
        TCPIP_STACK_PACKET_HANDLER pktHandler = pNetIf->pktHandler;
        if(pktHandler != NULL)
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

        while(true)
        {
            frameFound = false;

#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
            TCPIP_8021Q_TAG vlanTag;
            TCPIP_NET_IF* pNetIf = FC_Cvptr2NetIf(pRxPkt->pktIf);
            if(frameType == TCPIP_ETHER_TYPE_C_TPID)
            {   // VLAN tagged packet
                TCPIP_MAC_ETHERNET_VLAN_HEADER* pVlanHdr = (TCPIP_MAC_ETHERNET_VLAN_HEADER*)pRxPkt->pMacLayer;
                vlanTag.tci = TCPIP_Helper_ntohs(pVlanHdr->vlanTag.tci); 
                TCPIP_NET_IF* pVlanIf = F_TCPIPMapVlanInterface(pNetIf, vlanTag.vid);  
                if(pVlanIf == NULL)
                {   // vlan packet does not match the interface
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
                    pNetIf->vlanRxMissCnt++;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
                    break;
                }
                // vlan match; make sure the packet interface is the right one!
                pRxPkt->pktIf = pVlanIf;
                frameType = TCPIP_Helper_ntohs(pVlanHdr->Type);
                pRxPkt->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_RX_TAGGED;
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
                pVlanIf->vlanRxHitCnt++;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
            }
            else
            {
                pRxPkt->pktFlags &= ~(uint32_t)TCPIP_MAC_PKT_FLAG_RX_TAGGED;
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
                pNetIf->vUntaggedRxCnt++;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
            }
#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 

            pFrameEntry = TCPIP_FRAME_PROCESS_TBL;
            for(frameIx = 0; frameIx < (sizeof(TCPIP_FRAME_PROCESS_TBL) / sizeof(*TCPIP_FRAME_PROCESS_TBL)); frameIx++)
            {
                if(pFrameEntry->frameType == frameType)
                {   // found proper frame handler
                    pRxPkt->pktFlags &= ~(uint32_t)TCPIP_MAC_PKT_FLAG_TYPE_MASK;
                    pRxPkt->pktFlags |= pFrameEntry->pktTypeFlags;

                    if(TCPIPStackModuleRxInsert((TCPIP_STACK_MODULE)pFrameEntry->moduleId, pRxPkt, 0))
                    {
                        if(signal_t)
                        {   // signal to the module that RX is pending; if not already done so
                            if((procFrameMask & (1UL << frameIx)) == 0U)
                            {   // set the frame mask so we don't signal again
                                procFrameMask |= 1UL << frameIx;
                                F_TCPIPModuleSignalSetNotify((TCPIP_STACK_MODULE)pFrameEntry->moduleId, TCPIP_MODULE_SIGNAL_RX_PENDING);
                            }
                        }
                        frameFound = true;
                    }
                    break;
                }
                pFrameEntry++;
            }

            break;
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
static void    F_TCPIP_MacEventCB(TCPIP_MAC_EVENT event, const void* hParam)
{
    TCPIP_NET_IF* pNetIf = FC_Cvptr2NetIf(hParam);
    F_TCPIP_NetIfEvent(pNetIf, event, false);
}
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

static void F_TCPIP_NetIfEvent(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT event, bool isrProtect)
{
    OSAL_CRITSECT_DATA_TYPE isrSuspLvl = 0;

    TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry;
    pSigEntry = (((uint16_t)event & (uint16_t)TCPIP_STACK_MAC_ACTIVE_RX_EVENTS) != 0U) ? F_TCPIPModuleToSignalEntry(TCPIP_MODULE_MANAGER) : NULL;
   

    if(isrProtect)
    {
        isrSuspLvl = F_TCPIPMacIsrSuspend(pNetIf);
    }

    pNetIf->activeEvents |= (uint16_t)event;
    totTcpipEventsCnt++;

    if(pSigEntry != NULL)
    {
        pSigEntry->signalVal |= (uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING;
    }

    if(isrProtect)
    {
        F_TCPIPMacIsrResume(pNetIf, isrSuspLvl);
    }

    if(pSigEntry != NULL)
    {
        F_TCPIPSignalEntryNotify(pSigEntry, TCPIP_MODULE_SIGNAL_RX_PENDING, 0);
    }
}


/*******************************************************************************
  Function:
    void    F_TCPIP_STACK_TickHandler(uintptr_t context, uint32_t currTick)

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
static void F_TCPIP_STACK_TickHandler(uintptr_t context, uint32_t currTick)
{
    newTcpipTickAvlbl++;

    TCPIP_MODULE_SIGNAL_ENTRY* pTmoEntry = F_TCPIPModuleToSignalEntry(TCPIP_MODULE_NONE);
    TCPIP_MODULE_SIGNAL_ENTRY* pMgrEntry = F_TCPIPModuleToSignalEntry(TCPIP_MODULE_MANAGER);

    pTmoEntry->signalVal |= (uint16_t)TCPIP_MODULE_SIGNAL_TMO;

    F_TCPIPSignalEntryNotify(pMgrEntry, TCPIP_MODULE_SIGNAL_TMO, 0);

}

static void F_TCPIP_ProcessMACErrorEvents(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT activeEvent)
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
    TCPIP_NET_IF* pNewIf = TCPIPStackHandleToNetUp(netH);
    if(pNewIf != NULL)
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
static void F_TCPIP_SelectDefaultNet(TCPIP_NET_IF* pDownIf)
{
    size_t  netIx;
    TCPIP_NET_IF *pIf, *pNewIf;
    bool searchDefault = false;

    if(tcpipDefIf.defaultNet == NULL)
    {   // need search
        searchDefault = true;
    }
    else if(tcpipDefIf.defaultNet == pDownIf)
    {   // interface is going down; replace
        tcpipDefIf.defaultNet = NULL;
        searchDefault = true;
    }
    else if(pDownIf != NULL)
    {
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        if((TCPIPStackNetGetPrimary(tcpipDefIf.defaultNet) == TCPIPStackNetGetPrimary(pDownIf)))
        {
            tcpipDefIf.defaultNet = NULL;
            searchDefault = true;
        }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    }
    else
    {
        /* Do Nothing */
    }

    // repeat for the multicast interface
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)
    if(tcpipDefIf.defaultMcastNet == NULL)
    {   // need search
        searchDefault = true;
    }
    else if(tcpipDefIf.defaultMcastNet == pDownIf)
    {   // interface is going down; replace
        tcpipDefIf.defaultMcastNet = NULL;
        searchDefault = true;
    }
    else if(pDownIf != NULL)
    {
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        if((TCPIPStackNetGetPrimary(tcpipDefIf.defaultMcastNet) == TCPIPStackNetGetPrimary(pDownIf)))
        {
            tcpipDefIf.defaultMcastNet = NULL;
            searchDefault = true;
        }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    }
    else
    {
        /* Do Nothing */
    }
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)


    if(searchDefault)
    {
        pNewIf = NULL;
        pIf = tcpipNetIf;
        for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
        {
            if(pIf->Flags.bInterfaceEnabled != 0U)
            {   // select this one
                pNewIf = pIf;
                break;
            }
            pIf++;
        }

        if(pNewIf != NULL)
        {   // favor a primary interface
            pNewIf = TCPIPStackNetGetPrimary(pNewIf); 
        }
        if(tcpipDefIf.defaultNet == NULL)
        {
            tcpipDefIf.defaultNet = pNewIf;
        }
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)
        if(tcpipDefIf.defaultMcastNet == NULL)
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
    return NULL;
#endif
}

bool TCPIP_STACK_NetMulticastSet(TCPIP_NET_HANDLE netH)
{
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IGMP)
    TCPIP_NET_IF* pNewIf = TCPIPStackHandleToNetUp(netH);
    if(pNewIf != NULL)
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
    TCPIP_NET_IF* pNetIf = NULL;

    if((pIpAddress != NULL) && (pIpAddress->Val != 0U))
    {
        pNetIf = TCPIP_STACK_NetByAddress(pIpAddress);
    }

    if((pNetIf == NULL) && useDefault)
    {
        pNetIf = tcpipDefIf.defaultNet;
    }

    return pNetIf;
}

/*********************************************************************
 * Function:        TCPIPStackIpAddFromAnyNet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* pIpAddress)
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
TCPIP_NET_IF* TCPIPStackIpAddFromAnyNet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* pIpAddress)
{
    size_t  netIx;
    TCPIP_NET_IF* pIf;

    if((pIpAddress != NULL) && (pIpAddress->Val != 0U))
    {
        pIf = tcpipNetIf;
        for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
        {
            if((pNetIf == NULL) || (pIf == pNetIf))
            {
                if(pIf->Flags.bInterfaceEnabled != 0U)
                {
                    if(TCPIPStackIpAddFromLAN(pIf, pIpAddress))
                    {
                        return pIf;
                    }
                }
            }
            pIf++;
        }
    }

    return NULL;
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
    size_t  netIx;
    TCPIP_NET_IF* pNetIf;

    pNetIf = tcpipNetIf;
    for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
    {
        if(pNetIf->hIfMac == hMac)
        {
            return pNetIf;
        }
        pNetIf++;
    }


    return NULL;
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
    int32_t netIx;
    TCPIP_NET_IF* pNetIf;

    pNetIf = tcpipNetIf;
    for(netIx = 0; netIx < (int32_t)tcpip_stack_ctrl_data.nIfs; netIx++)
    {
        if(pNetIf->hIfMac == hMac)
        {
            return netIx;
        }
        pNetIf++;
    }


    return -1;
}

/*********************************************************************
 * Function:        size_t TCPIP_STACK_NumberOfNetworksGet(void)
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
size_t TCPIP_STACK_NumberOfNetworksGet(void)
{
    return (size_t)tcpip_stack_ctrl_data.nIfs;
}

/*********************************************************************
 * Function:        TCPIP_STACK_IndexToNet(size_t netIx)
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
TCPIP_NET_HANDLE TCPIP_STACK_IndexToNet(size_t netIx)
{
    if(netIx < (size_t)tcpip_stack_ctrl_data.nIfs)
    {
        return tcpipNetIf + netIx;
    }

    return NULL;
}

int  TCPIP_STACK_NetIndexGet(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(hNet);
    return TCPIP_STACK_NetIxGet(pNetIf);
}

// check a net handle is a valid network interface
// more checking, for user passed handles
TCPIP_NET_IF*  TCPIP_Stack_UserHandleToNet(TCPIP_NET_HANDLE hNet)
{
    if(tcpipNetIf != NULL)
    {
        TCPIP_NET_IF* pNetIf = FC_NetH2NetIf(hNet);
        int32_t ix = pNetIf - tcpipNetIf;
        if((0 <= ix) && (ix < (int32_t)tcpip_stack_ctrl_data.nIfs))
        {
            if(pNetIf == (tcpipNetIf + ix))
            {
                return pNetIf;
            }
        }

    }

    return NULL;
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
        size_t netIx;
        pNetIf = tcpipNetIf;
        for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
        {
            if(pNetIf->macId == (uint16_t)macId)
            {
                return pNetIf;
            }
            pNetIf++;
        }
    }


    return NULL;

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
    size_t  netIx;
    TCPIP_NET_IF* pNetIf;

    pNetIf = tcpipNetIf;
    for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
    {
        if(strcmp(pNetIf->ifName, interface) == 0)
        {
            return (TCPIP_NET_HANDLE)pNetIf;
        }
        pNetIf++;
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
    TCPIP_NET_IF*  pNetIf = TCPIPStackHandleToNet(netH);

    if((pNetIf != NULL) && (pNetIf->pMacObj != NULL))
    {
        return pNetIf->pMacObj->macName;
    }

    return NULL;
}

size_t TCPIP_STACK_NetAliasNameGet(TCPIP_NET_HANDLE netH, char* nameBuffer, size_t buffSize)
{
    size_t aliasSize;
    TCPIP_NET_IF*  pNetIf = TCPIPStackHandleToNet(netH);


    if(pNetIf != NULL)
    {
        aliasSize = strlen(pNetIf->ifName);
    }
    else
    {
        aliasSize = 0;
    }

    if(nameBuffer != NULL && buffSize != 0U)
    {   // valid buffer
        if(aliasSize != 0U)
        {
            (void)strncpy(nameBuffer, pNetIf->ifName, buffSize - 1U);
            nameBuffer[buffSize - 1U] = (char)0;
        }
        else
        {
            nameBuffer[0] = (char)0;
        }
    }
    return aliasSize;
}


const TCPIP_MAC_OBJECT* TCPIP_STACK_MACObjectGet(TCPIP_NET_HANDLE netH)
{

    TCPIP_NET_IF*  pNetIf = TCPIPStackHandleToNet(netH);

    if(pNetIf != NULL)
    {
        return pNetIf->pMacObj;
    }

    return NULL;
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
    size_t netIx;
    TCPIP_NET_IF* pIf;

    pIf = tcpipNetIf;
    for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
    {
        if((pIf->Flags.bInterfaceEnabled != 0U) && (pIf->netIPAddr.Val == pIpAddress->Val))
        {
            return pIf;
        }
        pIf++;
    }

    return NULL;
}

// finds a network interface matching an IPv4 address
// does NOT check for interface up/down!
TCPIP_NET_IF* TCPIP_STACK_MatchNetAddress(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    size_t netIx;
    TCPIP_NET_IF* pIf;

    if(pIpAdd->Val == 0x0100007fU /* || pNetIf->netIPAddr.Val == pIpAdd->Val*/)
    {
        return FC_CNetIf2NetIf(pNetIf);
    }

    pIf = tcpipNetIf;
    for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
    {
        if(pIf->netIPAddr.Val == pIpAdd->Val)
        {
            return pIf;
        }
        pIf++;
    }

    return NULL;
}

#if defined(TCPIP_STACK_USE_IPV4)
uint32_t TCPIP_STACK_NetAddress(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    return TCPIP_STACK_NetAddressGet(pNetIf);
}
#endif  // defined(TCPIP_STACK_USE_IPV4)


#if defined(TCPIP_STACK_USE_IPV6)
IPV6_ADDR_HANDLE TCPIP_STACK_NetIPv6AddressGet(TCPIP_NET_HANDLE netH, IPV6_ADDR_TYPE addType, IPV6_ADDR_STRUCT* pAddStruct, IPV6_ADDR_HANDLE addHandle)
{
    IPV6_ADDR_STRUCT * addrNode;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    uint8_t*           ptr;

    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf == NULL)
    {
        return NULL;
    }


    // Note: for both unicast and multicast addresses we start from unicast list
    // that's because we need to construct the solicited node multicas address
    // which is not currently stored in its own list!

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);
    if(addHandle == NULL)
    {   // start iteration through the list
        addrNode = FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head);
    }
    else
    {
        addrNode = FC_AddHndl2Ipv6AddStruct(addHandle)->next;
    }

    if(addType == IPV6_ADDR_TYPE_UNICAST)
    {
        if((addrNode != NULL) && (pAddStruct != NULL))
        {
            (void) memcpy(pAddStruct, addrNode, sizeof(*addrNode));
            pAddStruct->next = NULL;
            pAddStruct->prev = NULL;
        }
        return addrNode;
    }

    if(addType == IPV6_ADDR_TYPE_MULTICAST)
    {
        if(addrNode == NULL)
        {
            if((addHandle == NULL) || (FC_AddHndl2Ipv6AddStruct(addHandle)->flags.type == (uint32_t)IPV6_ADDR_TYPE_UNICAST))
            {   // either the unicast list is empty or finished the unicast list
                addrNode = FC_DblNode2AddStruct(pIpv6Config->listIpv6MulticastAddresses.head);
            }
        }

        if((addrNode != NULL) && (addrNode->flags.type == (uint32_t)IPV6_ADDR_TYPE_UNICAST))
        {   // do not report the same solicited node address multiple times
            IPV6_ADDR_STRUCT * unicastHead = FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head);
            IPV6_ADDR_STRUCT * currAddress = unicastHead;
            while(currAddress != addrNode)
            {
                ptr = ((uint8_t*)addrNode + offsetof(struct S_IPV6_ADDR_STRUCT, address));
                IPV6_ADDR* pAddNode = FC_U8Ptr2Ip6Add(ptr);

                ptr = ((uint8_t*)currAddress + offsetof(struct S_IPV6_ADDR_STRUCT, address));
                IPV6_ADDR* pAddCurr = FC_U8Ptr2Ip6Add(ptr);
                if(memcmp(pAddNode->v + (sizeof (IPV6_ADDR) - 3U), pAddCurr->v + (sizeof (IPV6_ADDR) - 3U), 3U) == 0)
                {   // address match; skip this one
                    addrNode = addrNode->next;
                    if(addrNode == NULL)
                    {   // end of list
                        addrNode = FC_DblNode2AddStruct(pIpv6Config->listIpv6MulticastAddresses.head);
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


        if((addrNode != NULL) && (pAddStruct != NULL))
        {
            (void) memcpy(pAddStruct, addrNode, sizeof(*addrNode));
            pAddStruct->next = NULL;
            pAddStruct->prev = NULL;
            if(addrNode->flags.type == (uint32_t)IPV6_ADDR_TYPE_UNICAST)
            {   // construct the solicited node multicast address
                ptr = ((uint8_t*)pAddStruct + offsetof(struct S_IPV6_ADDR_STRUCT, address));
                IPV6_ADDR* pAddNode = FC_U8Ptr2Ip6Add(ptr);
                (void) memcpy(pAddNode->v, IPV6_SOLICITED_NODE_MULTICAST.v, sizeof (IPV6_ADDR) - 3U);
                pAddStruct->flags.type = (uint8_t)IPV6_ADDR_TYPE_MULTICAST;
            }
        }
        return addrNode;
    }


    // no other address type supported
    return NULL;
}

// finds an interface that has the IPv6 address
TCPIP_NET_IF* TCPIPStackIPv6AddToNet(IPV6_ADDR* pIPv6Address, IPV6_ADDR_TYPE addType, bool useDefault)
{
    size_t          netIx;
    TCPIP_NET_IF*   pNetIf;
    TCPIP_NET_IF*   pSrchIf = NULL;

    if(pIPv6Address != NULL)
    {
        for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
        {
            pNetIf = tcpipNetIf + netIx;
            if(TCPIP_IPV6_AddressFind(pNetIf, pIPv6Address, addType) != NULL)
            {    // found interface
                pSrchIf = pNetIf;
                break;
            }
        }
    }

    if((pSrchIf == NULL) && useDefault)
    {
        pSrchIf = tcpipDefIf.defaultNet;
    }

    return pSrchIf;
}


#else
IPV6_ADDR_HANDLE TCPIP_STACK_NetIPv6AddressGet(TCPIP_NET_HANDLE netH, IPV6_ADDR_TYPE addType, IPV6_ADDR_STRUCT* pAddStruct, IPV6_ADDR_HANDLE addHandle)
{
    return NULL;
}
TCPIP_NET_IF* TCPIPStackIPv6AddToNet(IPV6_ADDR* pIPv6Address, IPV6_ADDR_TYPE addType, bool useDefault)
{
    return NULL;
}

#endif  // defined(TCPIP_STACK_USE_IPV6)

// user thread
#if defined(TCPIP_STACK_USE_IPV4)
bool TCPIP_STACK_NetAddressSet(TCPIP_NET_HANDLE netH, IPV4_ADDR* ipAddress, IPV4_ADDR* mask, bool setDefault)
{
    bool success = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if((pNetIf != NULL) && TCPIP_STACK_AddressServiceCanStart(pNetIf, TCPIP_STACK_ADDR_SRVC_NONE))
    {
        F_TCPIPStackSetIpAddress(pNetIf, ipAddress, mask, NULL, setDefault);
        success = true;
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

    if(success)
    {
        F_TCPIPStackSetConfig(pNetIf, false);
    }
    return success;
}

bool TCPIP_STACK_NetAddressGatewaySet(TCPIP_NET_HANDLE netH, IPV4_ADDR* ipAddress)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        TCPIP_STACK_GatewayAddressSet(pNetIf, ipAddress);
        return true;
    }

    return false;
}

uint32_t TCPIP_STACK_NetAddressGateway(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);

    if(pNetIf != NULL)
    {
        return pNetIf->netGateway.Val;
    }

    return 0;
}

uint32_t TCPIP_STACK_NetAddressDnsPrimary(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);

    if(pNetIf != NULL)
    {
        return pNetIf->dnsServer[0].Val;
    }

    return 0;
}

uint32_t TCPIP_STACK_NetAddressDnsSecond(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);

    if(pNetIf != NULL)
    {
        return pNetIf->dnsServer[1].Val;
    }

    return 0;
}

bool TCPIP_STACK_NetDnsPrimarySet(TCPIP_NET_HANDLE netH, IPV4_ADDR* ipAddress)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    TCPIP_STACK_PrimaryDNSAddressSet(pNetIf, ipAddress);

    return pNetIf != NULL;
}

bool TCPIP_STACK_NetDnsSecondSet(TCPIP_NET_HANDLE netH, IPV4_ADDR* ipAddress)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
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
        TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(netH);
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
        TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(netH);
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
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        return TCPIP_STACK_NetMaskGet(pNetIf);
    }

    return 0;
}
#endif  // defined(TCPIP_STACK_USE_IPV4)


bool TCPIP_STACK_NetAddressMacSet(TCPIP_NET_HANDLE netH, const TCPIP_MAC_ADDR* pAddr)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        (void) memcpy(pNetIf->netMACAddr.v, pAddr->v, sizeof(pNetIf->netMACAddr));
        return true;
    }

    return false;
}

const char* TCPIP_STACK_NetBIOSName(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(netH);
    if(pNetIf != NULL)
    {
        return (const char*)pNetIf->NetBIOSName;
    }

    return NULL;

}

bool TCPIP_STACK_NetBiosNameSet(TCPIP_NET_HANDLE netH, const char* biosName)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        (void) memcpy(pNetIf->NetBIOSName, (const uint8_t*)biosName, sizeof(pNetIf->NetBIOSName));
        TCPIP_Helper_FormatNetBIOSName(pNetIf->NetBIOSName);
        return true;
    }

    return false;
}

const uint8_t* TCPIP_STACK_NetAddressMac(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(netH);
    return TCPIP_STACK_NetUpMACAddressGet(pNetIf);
}

TCPIP_STACK_MODULE  TCPIP_STACK_NetMACIdGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        return TCPIP_STACK_NetMACId(pNetIf);
    }

    return TCPIP_MODULE_NONE;
}

TCPIP_MAC_TYPE TCPIP_STACK_NetMACTypeGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        return TCPIPStack_NetMacType(pNetIf);
    }

    return TCPIP_MAC_TYPE_NONE;
}

bool TCPIP_STACK_NetMACStatisticsGet(TCPIP_NET_HANDLE netH, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        TCPIP_MAC_RES res = (*pNetIf->pMacObj->MAC_StatisticsGet)(pNetIf->hIfMac, pRxStatistics, pTxStatistics);
        if(res == TCPIP_MAC_RES_OK)
        {
            return true;
        }
    }

    return false;
}

bool  TCPIP_STACK_NetMACRegisterStatisticsGet(TCPIP_NET_HANDLE netH, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, size_t nEntries, size_t* pHwEntries)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        TCPIP_MAC_RES res = (*pNetIf->pMacObj->MAC_RegisterStatisticsGet)(pNetIf->hIfMac, pRegEntries, nEntries, pHwEntries);
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
    if(tcpipNetIf != NULL)
    {   // we should be up and running for this
        size_t          netIx;
        TCPIP_NET_IF*   pNetIf;

        pNetIf = tcpipNetIf;
        for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
        {
            if(pNetIf->macId == (uint16_t)modId)
            {
                if(pNetIf->pMacObj != NULL)
                {   // found MAC module
                    return (*pNetIf->pMacObj->MAC_ConfigGet)(pNetIf->hIfMac, configBuff, buffSize, pNeededSize);
                }
            }
            pNetIf++;
        }
    }


    // not found
    return 0U;
}

// all the parameters are returned without checking
// that the interface is enabled or not!
size_t TCPIP_STACK_NetConfigGet(TCPIP_NET_HANDLE netH, void* configStoreBuff, size_t configStoreSize, size_t* pNeededSize)
{
    TCPIP_NET_IF* pNetIf;
    TCPIP_STACK_NET_IF_DCPT* pNetStg;

    pNetIf = TCPIPStackHandleToNet(netH);
    if((tcpipNetIf == NULL) || (pNetIf == NULL))
    {   // we should be up and running and have a valid IF
        return 0U;
    }

    if((pNeededSize == NULL) && ((configStoreBuff == NULL) || (configStoreSize == 0U) ))
    {   // nothing to do
        return 0;
    }

    // store needed size
    if(pNeededSize != NULL)
    {
        *pNeededSize = sizeof(*pNetStg); 
    }

    if((configStoreBuff != NULL) && (configStoreSize >= sizeof(*pNetStg)))
    {   // copy all the fields
        pNetStg = (TCPIP_STACK_NET_IF_DCPT*)configStoreBuff;

        // the TCPIP_STACK_NET_IF_DCPT has to be at the very beginning of the pNetIf !!!
        void* stgPtr = FC_NetDcpt2Vptr(pNetStg);
        (void) memcpy(stgPtr, (void*)&pNetIf->size, sizeof(*pNetStg));
        // update the size field
        pNetStg->size = (uint16_t)sizeof(*pNetStg);

        return sizeof(*pNetStg);
    }

    return 0U;
}

static void* F_NetConfigStringToBuffer(void** ppDstBuff, void* pSrcBuff, size_t* pDstSize, size_t* pNeedLen, size_t* pActLen)
{
    size_t  currLen;

    currLen = strlen(pSrcBuff) + 1U;

    *pNeedLen += currLen;
    if((currLen != 0U) && (currLen <= *pDstSize))
    {
        void* pCopy = *ppDstBuff;
        (void) memcpy(*ppDstBuff, pSrcBuff, currLen);
        *(uint8_t**)ppDstBuff += currLen;
        *pDstSize -= currLen;
        *pActLen += currLen;
        return pCopy;
    }
    else
    {   // stop copying
        *pDstSize = 0;
        return NULL;
    }

}

// restores pNetConfig from configBuff
TCPIP_NETWORK_CONFIG*   TCPIP_STACK_NetConfigSet(void* configStoreBuff, void* netConfigBuff, size_t buffSize, size_t* pNeededSize)
{
    TCPIP_NETWORK_CONFIG* pNetConf;            
    TCPIP_STACK_NET_IF_DCPT* pNetStg = (TCPIP_STACK_NET_IF_DCPT*)configStoreBuff;
    uint32_t startFlags;

    if((configStoreBuff == NULL) || (pNeededSize == NULL && (netConfigBuff == NULL) ))
    {   // nothing to do
        return NULL;
    }

    // minimum sanity check
    if(pNetStg->size != sizeof(*pNetStg))
    {   // not valid config save?
        return NULL;
    }

    if(buffSize < sizeof(*pNetConf))
    {   // not even enough room to start
        return NULL;
    }

    char    tempBuff[50 + 1];   // buffer large enough to hold any string in a TCPIP_NETWORK_CONFIG!
    void*   pDstBuff;
    size_t  dstSize;
    size_t  needLen, actualLen;
    
    // create at the very beginning of the buffer
    pNetConf = (TCPIP_NETWORK_CONFIG*)netConfigBuff;
    pDstBuff = pNetConf + 1;    // write area
    dstSize = buffSize - sizeof(*pNetConf);
    needLen = 0;
    actualLen = 0;
    tempBuff[sizeof(tempBuff) - 1U] = '\0';   // always end properly
    
    // get each field
    if((pNetStg->pMacObj != NULL) && (pNetStg->pMacObj->macName != NULL))
    {
        (void) strncpy(tempBuff, pNetStg->pMacObj->macName, sizeof(tempBuff) - 1U);
        pNetConf->interface = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);
    }
    else
    {
        pNetConf->interface = NULL;
    }

    (void) strncpy(tempBuff, (char*)pNetStg->NetBIOSName, sizeof(tempBuff) - 1U);
    pNetConf->hostName = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    (void) TCPIP_Helper_MACAddressToString(&pNetStg->netMACAddr, tempBuff, sizeof(tempBuff) - 1U);
    pNetConf->macAddr = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    (void) TCPIP_Helper_IPAddressToString(&pNetStg->netIPAddr, tempBuff, sizeof(tempBuff) - 1U);
    pNetConf->ipAddr = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    (void) TCPIP_Helper_IPAddressToString(&pNetStg->netMask, tempBuff, sizeof(tempBuff) - 1U);
    pNetConf->ipMask = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    (void) TCPIP_Helper_IPAddressToString(&pNetStg->netGateway, tempBuff, sizeof(tempBuff) - 1U);
    pNetConf->gateway = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    (void) TCPIP_Helper_IPAddressToString(&pNetStg->dnsServer[0], tempBuff, sizeof(tempBuff) - 1U);
    pNetConf->priDNS = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    (void) TCPIP_Helper_IPAddressToString(&pNetStg->dnsServer[1], tempBuff, sizeof(tempBuff) - 1U);
    pNetConf->secondDNS = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    (void) strncpy(tempBuff, TCPIP_Helper_PowerModeToString((TCPIP_MAC_POWER_MODE)pNetStg->Flags.powerMode), sizeof(tempBuff) - 1U);
    pNetConf->powerMode = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

    // set the flags
    startFlags = 0U;
    if(pNetStg->Flags.bIsDHCPEnabled != 0U)
    {
        startFlags |= (uint32_t)TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON;
    }
    if(pNetStg->Flags.bIsDHCPSrvEnabled != 0U)
    {
        startFlags |= (uint32_t)TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON; 
    }
    if(pNetStg->Flags.bIsZcllEnabled != 0U)
    {
        startFlags |= (uint32_t)TCPIP_NETWORK_CONFIG_ZCLL_ON;
    }
    // set the MAC driver object
    pNetConf->pMacObject = pNetStg->pMacObj;
#if defined(TCPIP_STACK_USE_IPV6)
    if((pNetStg->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_ADDRESS) != 0U)
    {
        startFlags |= (uint32_t)TCPIP_NETWORK_CONFIG_IPV6_ADDRESS;
        pNetConf->ipv6PrefixLen = (size_t)pNetStg->ipv6PrefixLen;

        (void) TCPIP_Helper_IPv6AddressToString(&pNetStg->netIPv6Addr, tempBuff, sizeof(tempBuff) - 1U);
        pNetConf->ipv6Addr = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);

        (void) TCPIP_Helper_IPv6AddressToString(&pNetStg->netIPv6Gateway, tempBuff, sizeof(tempBuff) - 1U);
        pNetConf->ipv6Gateway = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);
        
        (void) TCPIP_Helper_IPv6AddressToString(pNetStg->netIPv6Dns, tempBuff, sizeof(tempBuff) - 1U);
        pNetConf->ipv6Dns = F_NetConfigStringToBuffer(&pDstBuff, tempBuff, &dstSize, &needLen, &actualLen);
    }

#endif  // defined(TCPIP_STACK_USE_IPV6)

    pNetConf->startFlags = (TCPIP_NETWORK_CONFIG_FLAGS)startFlags; 
    if(pNeededSize != NULL)
    {
        *pNeededSize = needLen + sizeof(*pNetConf);
    }

    if(actualLen == needLen)
    {   // succeeded
        return pNetConf;
    }

    return NULL;

}
#endif  // (TCPIP_STACK_CONFIGURATION_SAVE_RESTORE != 0)

TCPIP_STACK_MODULE TCPIP_STACK_StringToMACId(const char* str)
{
    if(str != NULL)
    {
        size_t  netIx;
        TCPIP_NET_IF* pNetIf;
        const TCPIP_MAC_OBJECT  *pObj;

        pNetIf = tcpipNetIf;
        for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
        {
            pObj = pNetIf->pMacObj;
            if(pObj != NULL)
            {
                if(strcmp(str, pObj->macName) == 0)
                {
                    return (TCPIP_STACK_MODULE)pObj->macId;
                }
            }
            pNetIf++;
        }
    }


    return (TCPIP_STACK_MODULE)TCPIP_MODULE_MAC_NONE;
}



const char* TCPIP_STACK_MACIdToString(TCPIP_STACK_MODULE macId)
{
    size_t  netIx;
    TCPIP_NET_IF* pNetIf;
    const TCPIP_MAC_OBJECT  *pObj;

    pNetIf = tcpipNetIf;
    for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
    {
        pObj = pNetIf->pMacObj;
        if(pObj != NULL)
        {
            if(pObj->macId == (uint16_t)macId)
            {
                return pObj->macName;
            }
        }
        pNetIf++;
    }
    return NULL;
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
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        return (pNetIf->netIPAddr.Val | ~pNetIf->netMask.Val);
    }

    return 0;


}

bool TCPIP_STACK_NetIsUp(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    return pNetIf != NULL;
}

bool TCPIP_STACK_NetIsLinked(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(pNetIf != NULL)
    {
        return TCPIP_STACK_NetworkIsLinked(pNetIf);
    }
    return false;
}

bool TCPIP_STACK_NetIsReady(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetLinked(hNet);
    if(pNetIf != NULL)
    {
        return TCPIPStackIsConfig(pNetIf) == false;
    }
    return false;
}

#if defined(TCPIP_STACK_USE_IPV4)
TCPIP_STACK_DNS_SERVICE_TYPE TCPIP_STACK_DNSServiceSelect(TCPIP_NET_IF* pNetIf, TCPIP_NETWORK_CONFIG_FLAGS configFlags)
{
    // clear all the existing DNS address service bits
    pNetIf->Flags.v &= ~(uint16_t)TCPIP_STACK_DNS_SERVICE_MASK;

    
#if defined(TCPIP_STACK_USE_DNS)
        if(((uint16_t)configFlags & (uint16_t)TCPIP_NETWORK_CONFIG_DNS_CLIENT_ON) != 0U )
        { 
            pNetIf->Flags.bIsDnsClientEnabled = 1;
            return TCPIP_STACK_DNS_SERVICE_CLIENT;
        }
#endif  // defined(TCPIP_STACK_USE_DNS)
    
#if defined(TCPIP_STACK_USE_DNS_SERVER)
        if(((uint16_t)configFlags & (uint16_t)TCPIP_NETWORK_CONFIG_DNS_SERVER_ON) != 0U )
        { 
            pNetIf->Flags.bIsDnsServerEnabled = 1;
            return TCPIP_STACK_DNS_SERVICE_SERVER;
        }
#endif  // defined(TCPIP_STACK_USE_DNS_SERVER)
    // couldn't select a DNS service
    return TCPIP_STACK_DNS_SERVICE_NONE;
}
#endif  // defined(TCPIP_STACK_USE_IPV4)


TCPIP_STACK_ADDR_SRVC_TYPE TCPIP_STACK_AddressServiceSelect(TCPIP_NET_IF* pNetIf, TCPIP_NETWORK_CONFIG_FLAGS configFlags)
{
#if defined(TCPIP_STACK_USE_IPV4)

    // clear all the existing address service bits
    pNetIf->Flags.v &= ~(uint16_t)TCPIP_STACK_ADDR_SRVC_MASK;

    // Set up the address service on this interface
    // Priority (high to low): DHCPc, ZCLL, DHCPS, static IP address
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
#if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
    if(TCPIPStack_ModuleIsRunning(TCPIP_MODULE_DHCP_CLIENT))
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT != 0)
    {
        if(((uint16_t)configFlags & (uint16_t)TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON) != 0U )
        { 
            pNetIf->Flags.bIsDHCPEnabled = 1;
            return TCPIP_STACK_ADDR_SRVC_DHCPC;
        }
    }
#endif  // defined(TCPIP_STACK_USE_DHCP_CLIENT)

#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
#if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
    if(TCPIPStack_ModuleIsRunning(TCPIP_MODULE_ZCLL))
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT != 0)
    {
        if(((uint16_t)configFlags & (uint16_t)TCPIP_NETWORK_CONFIG_ZCLL_ON) != 0U )
        { 
            pNetIf->Flags.bIsZcllEnabled = 1;
            return TCPIP_STACK_ADDR_SRVC_ZCLL;
        }
    }
#endif  // defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)

#if defined(TCPIP_STACK_USE_DHCP_SERVER) || defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
#if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
    if(TCPIPStack_ModuleIsRunning(TCPIP_MODULE_DHCP_SERVER))
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT != 0)
    {
        if(((uint16_t)configFlags & (uint16_t)TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON) != 0U)
        { 
            pNetIf->Flags.bIsDHCPSrvEnabled = 1;
            return TCPIP_STACK_ADDR_SRVC_DHCPS;
        }
    }
#endif  // defined(TCPIP_STACK_USE_DHCP_SERVER) || defined(TCPIP_STACK_USE_DHCP_SERVER_V2)

#endif  // defined(TCPIP_STACK_USE_IPV4)
    // couldn't select an address service
    // use default/static
    return TCPIP_STACK_ADDR_SRVC_NONE;

}


#if defined(TCPIP_STACK_USE_IPV4)
bool TCPIP_STACK_AddressServiceCanStart(TCPIP_NET_IF* pNetIf, TCPIP_STACK_ADDR_SRVC_TYPE adSvcType)
{
    if(pNetIf != NULL)
    {   // enable a different address service only if there's not another one running
        // client has to stop a previos service (DHCP, ZCLL, etc.) in order to start another one
        return (pNetIf->Flags.v & (uint16_t)TCPIP_STACK_ADDR_SRVC_MASK) == (uint16_t)TCPIP_STACK_ADDR_SRVC_NONE;
    }

    return false;
}

TCPIP_STACK_ADDR_SRVC_TYPE TCPIPStackAddressServiceIsRunning(TCPIP_NET_IF* pNetIf)
{
    uint16_t tempflag = pNetIf->Flags.v & (uint16_t)TCPIP_STACK_ADDR_SRVC_MASK;
    return (TCPIP_STACK_ADDR_SRVC_TYPE)(tempflag);
}

void TCPIP_STACK_AddressServiceEvent(TCPIP_NET_IF* pNetIf, TCPIP_STACK_ADDR_SRVC_TYPE adSvcType,
                                    TCPIP_ADDR_SRVC_EV evType)
{

    if(evType == TCPIP_ADDR_SRVC_EV_RUN_RESTORE)
    {   // run time connection restore; it should be the DHCPc
        if(adSvcType == TCPIP_STACK_ADDR_SRVC_DHCPC)
        {
            // make sure any running service is cleared
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
            (void)TCPIP_ZCLL_ServiceEnable(pNetIf, false);
#endif  // defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
            pNetIf->Flags.bIsDHCPEnabled = 1;
            return;
        }
    }
    else if(evType == TCPIP_ADDR_SRVC_EV_CONN_LOST)
    {   // connection loss is considered a temporary event;
        // no need to disable a service
        // since we don't have network connectivity anyway
        return;
    }
    else if(adSvcType == TCPIP_STACK_ADDR_SRVC_DHCPS)
    {   // if DHCP server was stopped/failed
        // we won't start another address service
        // the user will have to take a decision
        return;
    }
    else
    {
        /* Do Nothing */
    }

    // the DHCPc/ZCLL address service failed/stopped:
    // TCPIP_ADDR_SRVC_EV_RUN_FAIL, TCPIP_ADDR_SRVC_EV_USER_STOP
    //
    // make sure any running service is cleared
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    (void)TCPIP_ZCLL_ServiceEnable(pNetIf, false);
#endif  // defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    pNetIf->Flags.v &= ~(uint16_t)TCPIP_STACK_ADDR_SRVC_MASK;
    F_TCPIPStackSetConfig(pNetIf, true);
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    typedef bool(*addressSvcFnc)(TCPIP_NET_IF* pNetIf, bool enable);
    addressSvcFnc   addFnc = NULL;
#endif  // defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    if(adSvcType == TCPIP_STACK_ADDR_SRVC_DHCPC)
    {   // the DHCP client has been stopped or failed
        // if possible we'll select ZCLL
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
        if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_ZCLL_ON) != 0U)
        {   // OK, we can use ZCLL
            addFnc = &TCPIP_ZCLL_ServiceEnable;
        }
#endif
    }
    // else if (adSvcType == TCPIP_STACK_ADDR_SRVC_ZCLL)
    // we'll select the default IP address

    // either way, no DHCP; restore the static DNS
    F_TCPIP_StackSetDefaultDns(pNetIf);

#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
    if(addFnc != NULL)
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
    if(pNetIf != NULL)
    {   // enable a different DNS service only if there's not another one running
        if(dnsSvcType == TCPIP_STACK_DNS_SERVICE_SERVER)
        {
            return (pNetIf->Flags.bIsDnsClientEnabled == 0U);
        }
        else if(dnsSvcType == TCPIP_STACK_DNS_SERVICE_CLIENT)
        {
            return (pNetIf->Flags.bIsDnsServerEnabled == 0U);
        }
        else
        {
            /* Do Nothing */
        }
    }

    return false;
}


/*********************************************************************
 * Function:        bool F_InitNetConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, size_t nNets)
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
static bool F_InitNetConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, size_t nNets)
{
    size_t     ix;
    TCPIP_NET_IF* pConfigIf;

    pConfigIf = tcpipNetIf;
    for(ix =0; ix < nNets; ix++)
    {
        if(!F_LoadNetworkConfig(pUsrConfig, pConfigIf, false))
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "Default Flash Network configuration load failed %zu\r\n", ix);
            return false;
        }
        pConfigIf++;
        pUsrConfig++;
    }



    return true;
}


/*********************************************************************
 * Function:        bool F_LoadNetworkConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, TCPIP_NET_IF* pNetIf, bool restartIf)
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
static bool F_LoadNetworkConfig(const TCPIP_NETWORK_CONFIG* pUsrConfig, TCPIP_NET_IF* pNetIf, bool restartIf)
{
    uint16_t    netIfIx = 0;
    bool        loadFault;
    const void* pMacConfig = NULL;             // MAC configuration save
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    TCPIP_MAC_ADDR  oldMACAddr;
    memset(oldMACAddr.v, 0x0, sizeof(oldMACAddr));
    TCPIP_NET_IF    *pPriIf = NULL, *pAliasIf = NULL;
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    char    oldIfName[sizeof(pNetIf->ifName) + 1];

    if(restartIf)
    {   // save old data that's still useful
        (void) strncpy(oldIfName, pNetIf->ifName, sizeof(oldIfName) - 1U); 
        pMacConfig = pNetIf->pMacConfig;
        netIfIx = pNetIf->netIfIx;
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        pPriIf = pNetIf->pPriIf; 
        pAliasIf = pNetIf->pAlias;
        if(!TCPIPStackNetIsPrimary(pNetIf))
        {
            oldMACAddr = pNetIf->netMACAddr;
        }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    }

    while(true)
    {
        loadFault = false;
        (void) memset(pNetIf, 0, sizeof(*pNetIf));

        if(pUsrConfig->macAddr != NULL)
        {
            (void) TCPIP_Helper_StringToMACAddress(pUsrConfig->macAddr, pNetIf->netMACAddr.v);
        }
        else
        {
            (void) memset(pNetIf->netMACAddr.v, 0, sizeof(pNetIf->netMACAddr.v));
        }

        // store the default addresses
#if defined(TCPIP_STACK_USE_IPV4)
        (void) TCPIP_Helper_StringToIPAddress(pUsrConfig->ipAddr, &pNetIf->DefaultIPAddr);
        (void) TCPIP_Helper_StringToIPAddress(pUsrConfig->ipMask, &pNetIf->DefaultMask);

        (void) TCPIP_Helper_StringToIPAddress(pUsrConfig->gateway, &pNetIf->DefaultGateway);
        (void) TCPIP_Helper_StringToIPAddress(pUsrConfig->priDNS, &pNetIf->DefaultDNSServer[0]);
        (void) TCPIP_Helper_StringToIPAddress(pUsrConfig->secondDNS, &pNetIf->DefaultDNSServer[1]);
#endif  // defined(TCPIP_STACK_USE_IPV4)

        pNetIf->pMacObj = pUsrConfig->pMacObject;
        if(pNetIf->pMacObj == NULL)
        {
            loadFault = true;       // no such MAC interface
            break;
        }

        pNetIf->macId = pNetIf->pMacObj->macId;
        pNetIf->macType = pNetIf->pMacObj->macType;
        if((pNetIf->macType == 0U) || (pNetIf->macType >= (uint8_t)TCPIP_MAC_TYPES))
        {
            loadFault = true;       // no such MAC type
            break;
        } 

#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
        pNetIf->vlanId = pUsrConfig->vlanId;
        pNetIf->vlanPcp = pUsrConfig->vlanPcp;
#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 

        // Load the NetBIOS Host Name
        (void) memcpy(pNetIf->NetBIOSName, (const uint8_t*)pUsrConfig->hostName, sizeof(tcpipNetIf[0].NetBIOSName));
        TCPIP_Helper_FormatNetBIOSName(pNetIf->NetBIOSName);


        // store start up flags
        pNetIf->startFlags = (uint16_t)pUsrConfig->startFlags;
#if defined(TCPIP_STACK_USE_IPV6)
        if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_ADDRESS) != 0U)
        {
            bool configIPv6 = true;

            pNetIf->ipv6PrefixLen = (uint16_t)pUsrConfig->ipv6PrefixLen;

            // ignore the static IPv6 address if incorrect
            if(pUsrConfig->ipv6Addr != NULL)
            {
                if(!TCPIP_Helper_StringToIPv6Address (pUsrConfig->ipv6Addr, &pNetIf->netIPv6Addr))
                {   // incorrect static IPv6 address
                    configIPv6 = false;
                }
            }
            // ignore the IPv6 gateway if incorrect; gateway == 0 is allowed.
            if(pUsrConfig->ipv6Gateway != NULL)
            {
                if(!TCPIP_Helper_StringToIPv6Address (pUsrConfig->ipv6Gateway, &pNetIf->netIPv6Gateway))
                {   // incorrect gateway address
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
                pNetIf->startFlags &= ~(uint16_t)TCPIP_NETWORK_CONFIG_IPV6_ADDRESS;
            }
        }
#endif  // defined(TCPIP_STACK_USE_IPV6)

        // Set up the address service on this interface
        F_TCPIPStackSetConfig(pNetIf, true);
#if defined(TCPIP_STACK_USE_IPV4)
        TCPIP_STACK_ADDR_SRVC_TYPE startAddService;
        startAddService = TCPIP_STACK_AddressServiceSelect(pNetIf, pUsrConfig->startFlags);

        if(startAddService == TCPIP_STACK_ADDR_SRVC_NONE)
        {   // couldn't start an address service; use the static values supplied
            TCPIP_STACK_AddressServiceDefaultSet(pNetIf);
        }

        // set the default DNS server
        F_TCPIP_StackSetDefaultDns(pNetIf);

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
        (void) memcpy(pNetIf->ifName, oldIfName, sizeof(pNetIf->ifName)); 
        // restore MAC config data
        pNetIf->pMacConfig = pMacConfig;
        pNetIf->netIfIx = netIfIx;
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
        pNetIf->pPriIf = pPriIf;
        pNetIf->pAlias = pAliasIf;
        if(!TCPIPStackNetIsPrimary(pNetIf))
        {
            pNetIf->netMACAddr = oldMACAddr;
        }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    }
    

    return !loadFault;
}

void TCPIP_STACK_AddressServiceDefaultSet(TCPIP_NET_IF* pNetIf)
{
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    // keep access to IP addresses consistent
    F_TCPIPStackSetIpAddress(pNetIf, &pNetIf->DefaultIPAddr, &pNetIf->DefaultMask, &pNetIf->DefaultGateway, false);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    F_TCPIPStackSetConfig(pNetIf, false);
}


#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

// Stack external event notification support

TCPIP_EVENT TCPIP_STACK_EventsPendingGet(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(pNetIf != NULL)
    {
        return TCPIP_STACK_Mac2TcpipEvent((*pNetIf->pMacObj->MAC_EventPendingGet)(pNetIf->hIfMac));
    }
    return TCPIP_EV_NONE;
}

#if (TCPIP_STACK_USER_NOTIFICATION != 0)
TCPIP_EVENT_HANDLE    TCPIP_STACK_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_EVENT evMask, TCPIP_STACK_EVENT_HANDLER notifyHandler, const void* notifyfParam)
{
    SGL_LIST_NODE*          node;
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);

    if((pNetIf != NULL) && TCPIPStackNetIsPrimary(pNetIf) && (notifyHandler != NULL))
    {
        TCPIP_EVENT_LIST_NODE eventNode;
        eventNode.handler = notifyHandler;
        eventNode.hParam = notifyfParam;
        eventNode.evMask = evMask;
        eventNode.pNetIf = pNetIf;
        eventNode.next   = NULL;

        node = TCPIP_Notification_Add(&pNetIf->registeredClients, tcpip_stack_ctrl_data.memH, &eventNode, sizeof(eventNode));
        return FC_ListNode2EventNode(node);
    }

    return NULL;
}

bool TCPIP_STACK_HandlerDeregister(TCPIP_EVENT_HANDLE hEvent)
{
    if(hEvent != NULL)
    {
        SGL_LIST_NODE* node = FC_EvHndl2ListNode(hEvent); 
        TCPIP_EVENT_LIST_NODE*  eventNode = FC_ListNode2EventNode(node); 

        if(TCPIP_Notification_Remove(node, &eventNode->pNetIf->registeredClients, tcpip_stack_ctrl_data.memH))
        {
            return true;
        }
    }

    return false;

}
#else
TCPIP_EVENT_HANDLE    TCPIP_STACK_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_EVENT evMask, TCPIP_STACK_EVENT_HANDLER notifyHandler, const void* notifyfParam)
{
    return NULL;
}
bool TCPIP_STACK_HandlerDeregister(TCPIP_EVENT_HANDLE hEvent)
{
    return false;
}
#endif  // (TCPIP_STACK_USER_NOTIFICATION != 0)


#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

static const TCPIP_STACK_MODULE_CONFIG* F_TCPIP_STACK_FindModuleData(TCPIP_STACK_MODULE moduleId, const TCPIP_STACK_MODULE_CONFIG* pModConfig, size_t nModules)
{
    while(nModules-- != 0U)
    {
        if(pModConfig->moduleId == moduleId)
        {
            return pModConfig;
        }
        pModConfig++;
    }

    return NULL;
}


int  TCPIP_STACK_NetIxGet(const TCPIP_NET_IF* pNetIf)
{
    if(pNetIf != NULL)
    {
        return (int)pNetIf->netIfIx;
    }
    return -1;
}


uint32_t  TCPIP_STACK_NetAddressGet(const TCPIP_NET_IF* pNetIf)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return TCPIPStackNetAddress(pNetIf);
    }
    return 0;
}


const IPV6_ADDR* TCPIP_STACK_NetStaticIPv6AddressGet(const TCPIP_NET_IF* pNetIf, uint8_t* pPrefixLen)
{
#if defined(TCPIP_STACK_USE_IPV6)
    if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_ADDRESS) != 0U)
    {
        if(pPrefixLen != NULL)
        {
            *pPrefixLen = (uint8_t)pNetIf->ipv6PrefixLen;
        }
        return &pNetIf->netIPv6Addr;
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)

    return NULL;
}

const IPV6_ADDR* TCPIP_STACK_NetDefaultIPv6GatewayGet(const TCPIP_NET_IF* pNetIf)
{
#if defined(TCPIP_STACK_USE_IPV6)
    if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_ADDRESS) != 0U)
    {
        return &pNetIf->netIPv6Gateway;
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)

    return NULL;
}

// sets the IP addresses of the interface
// critical lock should be obtained if done in a multi-threaded system 
static void F_TCPIPStackSetIpAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* ipAddress, const IPV4_ADDR* mask, const IPV4_ADDR* gw, bool setDefault)
{
    if(ipAddress != NULL)
    {
        pNetIf->netIPAddr.Val = ipAddress->Val;
        if(setDefault)
        {
            pNetIf->DefaultIPAddr.Val = ipAddress->Val;
        }
    }

    if(mask != NULL)
    {
        pNetIf->netMask.Val = mask->Val;
        if(setDefault)
        {
            pNetIf->DefaultMask.Val = mask->Val;
        }
    }

    if(gw != NULL)
    {
        pNetIf->netGateway.Val = gw->Val;
        if(setDefault)
        {
            pNetIf->DefaultGateway.Val = gw->Val;
        }
    }
}

#if defined(TCPIP_STACK_USE_IPV4)
void  TCPIPStackSetConfigAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* ipAddress, const IPV4_ADDR* mask, const IPV4_ADDR* gw, bool config)
{
    if(pNetIf != NULL)
    {
        OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        // keep access to IP addresses consistent
        F_TCPIPStackSetIpAddress(pNetIf, ipAddress, mask, gw, false);
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
        F_TCPIPStackSetConfig(pNetIf, config);
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
#if (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
static void F_TCPIPStackSetConfig(TCPIP_NET_IF* pNetIf, bool config)
{
    size_t ix;
    const uint16_t* pModIx;
    TCPIP_MODULE_SIGNAL signal_t;
    uint16_t    sigParam = (uint16_t)(1UL << pNetIf->netIfIx);

    pNetIf->Flags.bInConfig = (uint8_t)config;

    if(config == false)
    {   // we signal when ready
        OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        pModIx = TCPIP_STACK_MODULE_SIGNAL_CHANGE_TBL;
        for(ix = 0; ix < (sizeof(TCPIP_STACK_MODULE_SIGNAL_CHANGE_TBL) / sizeof(*TCPIP_STACK_MODULE_SIGNAL_CHANGE_TBL)); ix++)
        {
            TCPIP_MODULE_SIGNAL_ENTRY*  pSigEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + *pModIx;
            pSigEntry->signalVal |= (uint16_t)TCPIP_MODULE_SIGNAL_IF_CHANGE;
            pSigEntry->signalParam |= sigParam;
            pModIx++;
        }
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    }

    // call the external noftification
    signal_t = (config == false) ? TCPIP_MODULE_SIGNAL_IF_CHANGE : TCPIP_MODULE_SIGNAL_IF_CONFIG;
    pModIx = TCPIP_STACK_MODULE_SIGNAL_CONFIG_TBL;
    for(ix = 0; ix < (sizeof(TCPIP_STACK_MODULE_SIGNAL_CONFIG_TBL) / sizeof(*TCPIP_STACK_MODULE_SIGNAL_CONFIG_TBL)); ix++)
    {
        TCPIP_MODULE_SIGNAL_ENTRY*  pSigEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + *pModIx;
        F_TCPIPSignalEntryNotify(pSigEntry, signal_t, sigParam);
        pModIx++;
    }
}
#endif  // (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

uint32_t  TCPIP_STACK_NetMaskGet(const TCPIP_NET_IF* pNetIf)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return pNetIf->netMask.Val;
    }
    return 0;
}

void  TCPIP_STACK_GatewayAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress)
{
    if(pNetIf != NULL)
    {
        pNetIf->netGateway.Val = ipAddress->Val;
    }
}
void  TCPIP_STACK_PrimaryDNSAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress)
{
    if(pNetIf != NULL)
    {
        pNetIf->dnsServer[0].Val = ipAddress->Val;
    }
}

void  TCPIP_STACK_SecondaryDNSAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress)
{
    if(pNetIf != NULL)
    {
        pNetIf->dnsServer[1].Val = ipAddress->Val;
    }
}

bool  TCPIP_STACK_AddressIsOfNetUp(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return TCPIP_STACK_AddressIsOfNet(pNetIf, pIpAdd);
    }
    return false;
}

// detects net-directed bcast
bool  TCPIP_STACK_NetIsBcastAddress(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
       return (pIpAdd->Val == (pNetIf->netIPAddr.Val | ~pNetIf->netMask.Val));
    }
    return false;
}

// detects limited or net-directed bcast
bool  TCPIP_STACK_IsBcastAddress(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
       return (TCPIP_Helper_IsBcastAddress(pIpAdd) ||  TCPIP_STACK_NetIsBcastAddress(pNetIf, pIpAdd));
    }
    return false;
}

bool TCPIP_STACK_NetworkIsLinked(const TCPIP_NET_IF* pNetIf)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return (bool)pNetIf->exFlags.linkPrev;
    }

    return false;
}

// checks for valid up and linked interface
TCPIP_NET_IF* TCPIPStackHandleToNetLinked(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if((pNetIf != NULL) && (pNetIf->exFlags.linkPrev != 0U))
    {
        return pNetIf;
    }
    
    return NULL;
}

TCPIP_NET_IF* TCPIPStackAnyNetLinked(bool useDefault)
{
    size_t netIx;
    TCPIP_NET_IF* pNetIf = NULL;

    if(useDefault)
    {
        pNetIf = TCPIPStackHandleToNetLinked(tcpipDefIf.defaultNet);
    }

    if(pNetIf == NULL)
    {
        TCPIP_NET_IF* pIf;

        pIf = tcpipNetIf ;
        for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
        {
            if((pIf->Flags.bInterfaceEnabled != 0U) && (pIf->exFlags.linkPrev != 0U))
            {   // found linked interface
                pNetIf = pIf;
                break;
            }
            pIf++;
        }
    }

    return pNetIf;
}


// returns the interface for which this address is a net_directed address
// 0 in not found
// does NOT check network up, linked, etc.
// does NOT check primary/virtual interface!
TCPIP_NET_IF* TCPIPStackAnyNetDirected(const IPV4_ADDR* pIpAdd)
{
    size_t netIx;

    TCPIP_NET_IF* pIf;

    pIf = tcpipNetIf;
    for(netIx = 0; netIx < (size_t)tcpip_stack_ctrl_data.nIfs; netIx++)
    {
        if(TCPIPStack_IsDirectedBcast(pIf, pIpAdd))
        {
            return pIf;
        }
        pIf++;
    }

    return NULL;
}

TCPIP_STACK_MODULE  TCPIP_STACK_NetMACId(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf != NULL) ? (TCPIP_STACK_MODULE)pNetIf->macId:(TCPIP_STACK_MODULE)TCPIP_MODULE_MAC_NONE;
}


 TCPIP_MAC_HANDLE  TCPIP_STACK_NetToMAC(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf != NULL) ? pNetIf->hIfMac : 0U;
}



const uint8_t*  TCPIP_STACK_NetUpMACAddressGet(const TCPIP_NET_IF* pNetIf)
{
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return pNetIf->netMACAddr.v;
    }

    return NULL;
}

static TCPIP_MAC_ACTION TCPIP_STACK_StackToMacAction(TCPIP_STACK_ACTION action)
{   // TCPIP_MAC_ACTION and TCPIP_STACK_ACTION should be kept in sync!
    return (TCPIP_MAC_ACTION)action;
}

static void TCPIP_STACK_StacktoMacCtrl(TCPIP_MAC_MODULE_CTRL* pMacCtrl, TCPIP_STACK_MODULE_CTRL* stackCtrlData)
{
    TCPIP_NET_IF* pNetIf = stackCtrlData->pNetIf;


    (void) memset(pMacCtrl, 0, sizeof(*pMacCtrl));

    pMacCtrl->nIfs = stackCtrlData->nIfs;

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    pMacCtrl->mallocF = FC_MallocDbg2Malloc(&TCPIP_HEAP_MallocDebug);
    pMacCtrl->callocF = FC_CallocDbg2Calloc(&TCPIP_HEAP_CallocDebug);
    pMacCtrl->freeF = FC_FreeDbg2Free(&TCPIP_HEAP_FreeDebug);
#else
    pMacCtrl->mallocF = &TCPIP_HEAP_MallocOutline;
    pMacCtrl->callocF = &TCPIP_HEAP_CallocOutline;
    pMacCtrl->freeF = &TCPIP_HEAP_FreeOutline;
#endif // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    
    pMacCtrl->memH = stackCtrlData->memH;


#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
    pMacCtrl->pktAllocF = FC_PktAllocDbg2PktAlloc(&F_TCPIP_PKT_PacketAllocDebug);
    pMacCtrl->pktFreeF = FC_PktFreeDbg2PktFree(&F_TCPIP_PKT_PacketFreeDebug);
    pMacCtrl->pktAckF = FC_PktAckDbg2PktAck(&F_TCPIP_PKT_PacketAcknowledgeDebug);
#else
    pMacCtrl->pktAllocF = &F_TCPIP_PKT_PacketAlloc;
    pMacCtrl->pktFreeF = &F_TCPIP_PKT_PacketFree;
    pMacCtrl->pktAckF = &F_TCPIP_PKT_PacketAcknowledge;
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

    pMacCtrl->synchF = &F_TCPIP_StackSyncFunction;

#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
    // Stack can use one handler for all network interfaces, like in this case
    // Each time a notification is received, all interfaces are checked
    // Or, more efficient, use a handler per interface
    pMacCtrl->eventF = &F_TCPIP_MacEventCB;
    pMacCtrl->eventParam = pNetIf;
#else
    pMacCtrl->eventF = 0;
    pMacCtrl->eventParam = 0;
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)

    pMacCtrl->netIx = (uint16_t)stackCtrlData->netIx;
    pMacCtrl->gapDcptOffset = pktGapDcptOffset;
    pMacCtrl->gapDcptSize = TCPIP_PKT_GapDcptSize();
#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0)
    if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_VLAN_USE_VID_NULL) == 0U)
    {
        pMacCtrl->retrieveF = &F_TCPIP_MAC_PKT_Retrieve;
    }
#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0)

    pMacCtrl->macAction = (uint8_t)TCPIP_STACK_StackToMacAction((TCPIP_STACK_ACTION)stackCtrlData->stackAction);
    pMacCtrl->powerMode = (uint8_t)stackCtrlData->powerMode;
    pMacCtrl->controlFlags = (uint16_t)TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2;
    (void) memcpy(pMacCtrl->ifPhyAddress.v, pNetIf->netMACAddr.v, sizeof(pMacCtrl->ifPhyAddress));
}

// returns true if the stack needs processing time because of events
static bool TCPIP_STACK_CheckEventsPending(void)
{
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
    int32_t event = totTcpipEventsCnt;
    return ((newTcpipTickAvlbl != 0) || (event != 0));
#else
    // fake pending events
    totTcpipEventsCnt++;
    return true;
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
}


TCPIP_SIGNAL_HANDLE TCPIPStackSignalHandlerRegister(TCPIP_STACK_MODULE modId, tcpipModuleSignalHandler signalHandler, int16_t asyncTmoMs)
{

    if(signalHandler != NULL )
    {
        if((TCPIP_MODULE_LAYER1 <= modId) && ((uint16_t)modId < (sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL)/sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL))))
        {
            TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + (uint16_t)modId;
            if((pSignalEntry->signalHandler == NULL) || (pSignalEntry->signalHandler == signalHandler))
            {   // found module slot
                pSignalEntry->signalHandler = signalHandler;
                if ((asyncTmoMs != 0) && (asyncTmoMs < (int16_t)stackTaskRate))
                {
                    asyncTmoMs = (int16_t)stackTaskRate;
                }
                pSignalEntry->asyncTmo = asyncTmoMs;
                pSignalEntry->currTmo = asyncTmoMs;
                return pSignalEntry;
            }
        }
    }

    return NULL;
}

bool TCPIPStackSignalHandlerSetParams(TCPIP_STACK_MODULE modId, TCPIP_SIGNAL_HANDLE handle, int16_t asyncTmoMs)
{
    TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = FC_SigHndl2SigEntry(handle);

    if(pSignalEntry != NULL && pSignalEntry->signalHandler != NULL)
    {   // minimum sanity check
        if ((asyncTmoMs != 0) && (asyncTmoMs < (int16_t)stackTaskRate))
        {
            asyncTmoMs = (int16_t)stackTaskRate;
        }
        pSignalEntry->asyncTmo = asyncTmoMs;
        pSignalEntry->currTmo = asyncTmoMs;
        return true;
    }

    return false;
}

// de-registers a previous registered timeout handler
void TCPIPStackSignalHandlerDeregister(TCPIP_SIGNAL_HANDLE handle)
{
    TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = FC_SigHndl2SigEntry(handle);

    if(pSignalEntry != NULL)
    {
        (void) memset(pSignalEntry, 0x0, sizeof(*pSignalEntry));
    }
}

// used by stack modules
TCPIP_MODULE_SIGNAL  TCPIPStackModuleSignalGet(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL clrMask)
{

    TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + (uint16_t)modId;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_MODULE_SIGNAL modSignal = (TCPIP_MODULE_SIGNAL)pSignalEntry->signalVal;
    pSignalEntry->signalVal &= (~(uint16_t)clrMask);
    if(((uint16_t)clrMask & (uint16_t)TCPIP_MODULE_SIGNAL_ASYNC) != 0U)
    {
        stackAsyncSignalCount -= 1U; 
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return modSignal;
}

// gets the associated parmeter too
// returns the pending module signals + signal parameter
// signalParam should NOT be 0!
// the signal parameter is cleared after this operation, there's is no peek!
#if (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
TCPIP_MODULE_SIGNAL  TCPIPStackModuleSignalParamGet(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL clrMask, uint32_t* signalParam)
{

    TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + (uint16_t)modId;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_MODULE_SIGNAL modSignal = (TCPIP_MODULE_SIGNAL)pSignalEntry->signalVal;
    *signalParam = pSignalEntry->signalParam;
    pSignalEntry->signalVal &= (~(uint16_t)clrMask);
    if(((uint16_t)clrMask & (uint16_t)TCPIP_MODULE_SIGNAL_ASYNC) != 0U)
    {
        stackAsyncSignalCount -= 1U; 
    }
    pSignalEntry->signalParam = 0;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return modSignal;

}
#endif  // (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

// used by manager!
// TMO and RX_PENDING signals only!
static TCPIP_MODULE_SIGNAL  F_TCPIPStackManagerSignalClear(TCPIP_MODULE_SIGNAL clrMask)
{
    TCPIP_MODULE_SIGNAL tmoMask;
    TCPIP_MODULE_SIGNAL rxMask;
    TCPIP_MODULE_SIGNAL mgrSignal;
    TCPIP_MODULE_SIGNAL_ENTRY* pMgrEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + (uint16_t)TCPIP_MODULE_MANAGER;
    TCPIP_MODULE_SIGNAL_ENTRY* pTmoEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + (uint16_t)TCPIP_MODULE_NONE;
    uint16_t sigMask;
    uint16_t sigVal;


    if((uint16_t)clrMask == 0U)
    {   // read only needed
        sigVal = pMgrEntry->signalVal | pTmoEntry->signalVal;
        return (TCPIP_MODULE_SIGNAL)sigVal;
    }

    sigMask = (uint16_t)clrMask & (uint16_t)TCPIP_MODULE_SIGNAL_TMO;
    tmoMask = (TCPIP_MODULE_SIGNAL)sigMask;
    sigMask = (uint16_t)clrMask & (uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING;
    rxMask = (TCPIP_MODULE_SIGNAL)sigMask;


    // protect against ISRs (MAC + TMR) and other threads too!
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
    sigVal = (uint16_t)pMgrEntry->signalVal | (uint16_t)pTmoEntry->signalVal;
    mgrSignal = (TCPIP_MODULE_SIGNAL)sigVal;

    if((uint16_t)tmoMask != 0U)
    {
        pTmoEntry->signalVal &= (~(uint16_t)TCPIP_MODULE_SIGNAL_TMO);
    }
    if((uint16_t)rxMask != 0U)
    {
        pMgrEntry->signalVal &= (~(uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING);
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, critSect);

    return mgrSignal;
}

// coming from user threads if the module tasks run in user space
static  void* F_TCPIPSignalEntrySet(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signal_t)
{
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    pSigEntry->signalVal |= (uint16_t)signal_t;
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
//  TCPIPStackModuleSignalRequest/TCPIPStackModuleSignalGet for TCPIP_MODULE_SIGNAL_ASYNC in pairs!
//
// NOTE:
//  A signal request sends a signal to the module task itself
//  A signal request will also send a signal to the stack task itself, when the TCP/IP modules are executed internally
//  - unless noMgrAlert is specified
//
bool TCPIPStackModuleSignalRequest(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL signal_t, bool noMgrAlert)
{
    if(modId <= TCPIP_MODULE_MANAGER)
    {   // cannot request signals to manager itself
        return false;
    }

    F_TCPIPModuleSignalSetNotify(modId, signal_t);

    if(((uint16_t)signal_t & (uint16_t)TCPIP_MODULE_SIGNAL_ASYNC) != 0U)
    {
        F_TCPIPAsyncSignalInc();
    }

#if !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)
    if(noMgrAlert == false)
    {
        F_TCPIPSignalEntryNotify(F_TCPIPModuleToSignalEntry(TCPIP_MODULE_MANAGER), signal_t, 0);
    }
#endif  //  !defined(TCPIP_STACK_APP_EXECUTE_MODULE_TASKS)

    return true;
}

static void F_TCPIPSignalEntryNotify(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signal_t, uint32_t sigParam)
{
    TCPIP_MODULE_SIGNAL_FUNC userF;

    userF = pSigEntry->userSignalF;
    if(userF != NULL)
    {
        (*userF)(pSigEntry, pSigEntry - TCPIP_STACK_MODULE_SIGNAL_TBL, signal_t, sigParam);
    }
}

static void F_TCPIPSignalEntrySetNotify(TCPIP_MODULE_SIGNAL_ENTRY* pSigEntry, TCPIP_MODULE_SIGNAL signals, uint32_t sigParam)
{
    (void) F_TCPIPSignalEntrySet(pSigEntry, signals);
    F_TCPIPSignalEntryNotify(pSigEntry, signals, sigParam);
}

// signal the stack manager maintained timeout
static void F_TCPIPStackSignalTmo(void)
{
    size_t  ix;
    TCPIP_MODULE_SIGNAL_ENTRY*  pSigEntry;

    pSigEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + (uint32_t)TCPIP_MODULE_LAYER1;
    for(ix = (size_t)TCPIP_MODULE_LAYER1; ix < (sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL) / sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL)); ix++)
    {
        if((pSigEntry->signalHandler != NULL) && (pSigEntry->asyncTmo != 0))
        {   // used slot
            pSigEntry->currTmo -= (int16_t)stackTaskRate;
            if(pSigEntry->currTmo <= 0)
            {   // timeout: send a signal to this module
                pSigEntry->currTmo += pSigEntry->asyncTmo;
                F_TCPIPSignalEntrySetNotify(pSigEntry, TCPIP_MODULE_SIGNAL_TMO, 0); 
            }
        }
        pSigEntry++;
    }
}

// insert a packet into a module RX queue
// signal should be false when modId == TCPIP_MODULE_MANAGER !
bool TCPIPStackModuleRxInsert(TCPIP_STACK_MODULE modId, TCPIP_MAC_PACKET* pRxPkt, bool signal_t)
{
#if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
    TCPIP_MODULE_RUN_DCPT* pRDcpt = TCPIP_MODULES_RUN_TBL + (uint32_t)modId;
    if(pRDcpt->isRunning == 0U)
    {
        return false;
    }
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT != 0)

    SINGLE_LIST* pQueue = TCPIP_MODULES_QUEUE_TBL + (uint32_t)modId;
    SGL_LIST_NODE* node = FC_MacPkt2SglNode(pRxPkt); 
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_Helper_SingleListTailAdd(pQueue, node);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

    if(signal_t)
    {
        F_TCPIPModuleSignalSetNotify(modId, TCPIP_MODULE_SIGNAL_RX_PENDING);
    }
    return true;
}

//
// extracts a packet from a module RX queue
// returns 0 if queue is empty
TCPIP_MAC_PACKET* TCPIPStackModuleRxExtract(TCPIP_STACK_MODULE modId)
{
    SINGLE_LIST* pQueue = TCPIP_MODULES_QUEUE_TBL + (uint32_t)modId;

    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    SGL_LIST_NODE* node = TCPIP_Helper_SingleListHeadRemove(pQueue);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

    return FC_SglNode2MacPkt(node);
}

// purges the packets from a module RX queue
// belonging to the pNetIf
void TCPIPStackModuleRxPurge(TCPIP_STACK_MODULE modId, TCPIP_NET_IF* pNetIf)
{
    SINGLE_LIST     remList = { 0 };  // list of packets to remove/ack
    SINGLE_LIST     keepList = { 0 };  // list of packets to keep
    SGL_LIST_NODE*  node;
    TCPIP_MAC_PACKET* pMacPkt;
    
    SINGLE_LIST*      pRxQueue = (TCPIP_MODULES_QUEUE_TBL + (uint32_t)modId); // list to extract from
    
    // kill the list
    while(true)
    {
        OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        node = TCPIP_Helper_SingleListHeadRemove(pRxQueue);
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

        pMacPkt = FC_SglNode2MacPkt(node); 
        if(pMacPkt == NULL)
        {
            break;
        }


        if(pMacPkt->pktIf == pNetIf)
        {   // need to remove
            TCPIP_Helper_SingleListTailAdd(&remList, node);
        }
        else
        {   // need to keep
            TCPIP_Helper_SingleListTailAdd(&keepList, node);
        }
    }

    // acknowledge the removed packets
    while(true)
    {
        node = TCPIP_Helper_SingleListHeadRemove(&remList);
        pMacPkt = FC_SglNode2MacPkt(node); 

        if(pMacPkt == NULL)
        {
            break;
        }

        TCPIP_PKT_PacketAcknowledge(pMacPkt, TCPIP_MAC_PKT_ACK_SOURCE_ERR);
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
void TCPIPStackInsertRxPacket(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, bool signal_t)
{
    pRxPkt->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
    // update the frame length
#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
        F_TCPIP_PKT_VlanAdjust(pNetIf, pRxPkt);
#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
    F_TCPIPInsertMacRxPacket(pNetIf, pRxPkt);

    if(signal_t)
    {
        F_TCPIP_NetIfEvent(TCPIPStackNetGetPrimary(pNetIf), TCPIP_MAC_EV_RX_DONE, true);
    }
}

TCPIP_MAC_RES TCPIPStackPacketTx(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET * ptrPacket)
{
    TCPIP_MAC_RES res;

    TCPIP_PKT_FlightLogTx(ptrPacket, TCPIP_THIS_MODULE_ID);
    TCPIP_PKT_FlightLogTx(ptrPacket, (TCPIP_STACK_MODULE)pNetIf->macId);    // MAC doesn't call the log function

    if(pNetIf->hIfMac != 0U)
    {
#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
        F_TCPIP_PKT_VlanAdjust(pNetIf, ptrPacket);
#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 

        res = pNetIf->pMacObj->MAC_PacketTx(pNetIf->hIfMac, ptrPacket);
        // stack should always use well formatted packets!
        TCPIPStack_Assert((int32_t)res >= 0, __FILE__, __func__, __LINE__);
    }
    else
    {
        res = TCPIP_MAC_RES_NOT_READY_ERR;
    }

    return res;
}

static bool F_TCPIP_StackSyncFunction(void* synchHandle, TCPIP_MAC_SYNCH_REQUEST req)
{
    bool res;

    switch(req)
    {
        case TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE: 
            res = (OSAL_SEM_Create((OSAL_SEM_HANDLE_TYPE*)synchHandle, OSAL_SEM_TYPE_BINARY, 1, 1) == OSAL_RESULT_SUCCESS ) ? true : false;
            break;

        case TCPIP_MAC_SYNCH_REQUEST_OBJ_DELETE: 
            res = (OSAL_SEM_Delete((OSAL_SEM_HANDLE_TYPE*)synchHandle) == OSAL_RESULT_SUCCESS ) ? true : false;
            break;

        case TCPIP_MAC_SYNCH_REQUEST_OBJ_LOCK: 
            res = (OSAL_SEM_Pend((OSAL_SEM_HANDLE_TYPE*)synchHandle, OSAL_WAIT_FOREVER) == OSAL_RESULT_SUCCESS ) ? true: false;
            break;

        case TCPIP_MAC_SYNCH_REQUEST_OBJ_UNLOCK: 
            res = (OSAL_SEM_Post((OSAL_SEM_HANDLE_TYPE*)synchHandle) == OSAL_RESULT_SUCCESS ) ? true: false;
            break;
            
        case TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER:
            *(OSAL_CRITSECT_DATA_TYPE*)synchHandle =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
            res = true; 
            break;

        case TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE:
            OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, *(OSAL_CRITSECT_DATA_TYPE*)synchHandle);
            res = true; 
            break;

        default:
            res = false;
            break;
    }

    return res;
}

uint8_t TCPIP_STACK_MacTxPriGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF*  pNetIf = TCPIPStackHandleToNet(netH);
    return TCPIPStack_TxPriNum(pNetIf);
}

uint8_t TCPIP_STACK_MacRxPriGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF*  pNetIf = TCPIPStackHandleToNet(netH);
    return TCPIPStack_RxPriNum(pNetIf);
}

bool TCPIP_STACK_NetVlanCfgGet(TCPIP_NET_HANDLE netH, TCPIP_NETWORK_VLAN_CONFIG* pVlanCfg)
{
#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        if(pVlanCfg)
        {
            pVlanCfg->id = F_TCPIP_NetVlanId(pNetIf);
            pVlanCfg->pcp = F_TCPIP_NetVlanPcp(pNetIf);
            pVlanCfg->dei = (pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_VLAN_DEI) == 0U ? 0 : 1;
            pVlanCfg->useNullVid = (pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_VLAN_USE_VID_NULL) == 0U ? 0 : 1;
        }
        return true;
    }

    return false;
#else
    return false;
#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
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
TCPIP_MODULE_SIGNAL_HANDLE TCPIP_MODULE_SignalFunctionRegister(TCPIP_STACK_MODULE moduleId, TCPIP_MODULE_SIGNAL_FUNC signalF)
{
    TCPIP_MODULE_SIGNAL_ENTRY* pEntry = NULL;

    if(signalF != NULL )
    {
        size_t modId = (size_t)moduleId;
        if((0U < modId) && (modId < (sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL) / sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL))))
        {
            TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + modId;
            OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
            if(pSignalEntry->userSignalF == NULL)
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
    TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = FC_SigHndl2SigEntry(signalHandle);
    if(pSignalEntry != NULL)
    {
        pSignalEntry->userSignalF = NULL;
        return true;
    }

    return false;
}


// from user space
// read only atomic access
TCPIP_MODULE_SIGNAL TCPIP_MODULE_SignalGet(TCPIP_STACK_MODULE moduleId)
{
    size_t modId = (size_t)moduleId;
    if(moduleId == TCPIP_MODULE_MANAGER)
    {
        return (TCPIP_MODULE_SIGNAL)F_TCPIPStackManagerSignalClear((TCPIP_MODULE_SIGNAL)0);
    }
    else if((0U < modId) && (modId < (sizeof(TCPIP_STACK_MODULE_SIGNAL_TBL) / sizeof(*TCPIP_STACK_MODULE_SIGNAL_TBL))))
    {
        TCPIP_MODULE_SIGNAL_ENTRY* pSignalEntry = TCPIP_STACK_MODULE_SIGNAL_TBL + modId;
        return (TCPIP_MODULE_SIGNAL)pSignalEntry->signalVal;
    }
    else
    {
        /* Do Nothing */
    }

    return TCPIP_MODULE_SIGNAL_NONE; 
}

#if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
bool TCPIP_MODULE_Deinitialize(TCPIP_STACK_MODULE moduleId)
{
    size_t  netIx, entryIx;
    TCPIP_NET_IF   *pNetIf;
    TCPIP_STACK_MODULE_CTRL stackCtrlData;        
    const TCPIP_STACK_MODULE_ENTRY  *pEntry, *pModEntry;

    if(tcpipNetIf != NULL)
    {
        if(1U < (uint32_t)moduleId && ((uint32_t)moduleId < (sizeof(TCPIP_MODULES_RUN_TBL) / sizeof(*TCPIP_MODULES_RUN_TBL))))
        {
            TCPIP_MODULE_RUN_DCPT* pRDcpt = TCPIP_MODULES_RUN_TBL + (uint32_t)moduleId;
            if(pRDcpt->isRunning != 0U)
            {   // this module up and running
                //
                //
                stackCtrlData.stackAction = (uint8_t)TCPIP_STACK_ACTION_DEINIT;
                stackCtrlData.powerMode = (uint8_t)TCPIP_MAC_POWER_NONE;
                // find the corresponding module entry
                pModEntry = NULL;
                pEntry = TCPIP_STACK_MODULE_ENTRY_TBL;
                for(entryIx = 0; entryIx < (sizeof(TCPIP_STACK_MODULE_ENTRY_TBL)/sizeof(*TCPIP_STACK_MODULE_ENTRY_TBL)); entryIx++)
                {
                    if(pEntry->moduleId == (uint16_t)moduleId)
                    {   // found entry
                        pModEntry = pEntry;
                        break;
                    }
                    pEntry++;
                }

                if(pModEntry != NULL)
                {
                    pNetIf = tcpipNetIf;
                    for(netIx = 0; netIx < tcpip_stack_ctrl_data.nIfs; netIx++)
                    {
                        stackCtrlData.pNetIf = pNetIf;
                        stackCtrlData.netIx = pNetIf->netIfIx;
                        pEntry->deInitFunc(&stackCtrlData);
                        pNetIf++;
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
    if(tcpipNetIf != NULL)
    {
        if((1U < (uint32_t)moduleId) && ((uint32_t)moduleId < (sizeof(TCPIP_MODULES_RUN_TBL) / sizeof(*TCPIP_MODULES_RUN_TBL))))
        {
            TCPIP_MODULE_RUN_DCPT* pRDcpt = TCPIP_MODULES_RUN_TBL + (uint32_t)moduleId;
            return (pRDcpt->isRunning != 0U);
        }
    }

    return false;
}
#else
bool TCPIP_MODULE_Deinitialize(TCPIP_STACK_MODULE moduleId)
{
    return false;
}
bool TCPIP_MODULE_IsRunning(TCPIP_STACK_MODULE moduleId)
{
    return false;
}
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT != 0)

TCPIP_STACK_HEAP_HANDLE TCPIP_STACK_HeapHandleGet(TCPIP_STACK_HEAP_TYPE heapType, size_t heapIndex)
{
    return (heapType == tcpip_stack_ctrl_data.heapType) ? tcpip_stack_ctrl_data.memH : NULL;
}

size_t TCPIP_STACK_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return (heapH != NULL) ? TCPIP_HEAP_Size(heapH) : 0U;
}


size_t TCPIP_STACK_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return (heapH != NULL) ? TCPIP_HEAP_MaxSize(heapH) : 0U;
}



size_t TCPIP_STACK_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return (heapH != NULL) ? TCPIP_HEAP_FreeSize(heapH) : 0U;
}

size_t TCPIP_STACK_HEAP_HighWatermark(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return (heapH != NULL) ? TCPIP_HEAP_HighWatermark(heapH) : 0U;
}


TCPIP_STACK_HEAP_RES TCPIP_STACK_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return (heapH != NULL) ? TCPIP_HEAP_LastError(heapH) : TCPIP_STACK_HEAP_RES_NO_HEAP; 
}

// return the stack heap configuration parameters
const TCPIP_STACK_HEAP_CONFIG* TCPIPStackHeapConfig(void)
{
    return &tcpip_heap_config;
}

TCPIP_NETWORK_TYPE TCPIP_STACK_NetGetType(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(hNet);

    if(pNetIf != NULL)
    {
        return TCPIPStackNetIsPrimary(pNetIf) ? TCPIP_NETWORK_TYPE_PRIMARY : TCPIP_NETWORK_TYPE_ALIAS;
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
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
TCPIP_NET_IF* TCPIPStackMapAliasInterface(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pDestAddress)
{
    TCPIP_NET_IF    *pAliasIf;

    if(!TCPIP_Helper_IsBcastAddress(pDestAddress))
    {
        // browse the aliases belonging to this primary!
        for(pAliasIf = TCPIPStackNetGetPrimary(pNetIf); pAliasIf != NULL; pAliasIf = TCPIPStackNetGetAlias(pAliasIf))
        {
            if(TCPIP_STACK_AddressIsOfNet(pAliasIf, pDestAddress))
            {
                return pAliasIf;
            }
        }
    }

    // no mapping; return the original interface
    return FC_CNetIf2NetIf(pNetIf);
}


// copies the alias interface MAC settings from the primary interface
static void F_TCPIPCopyMacAliasIf(TCPIP_NET_IF* pAliasIf, TCPIP_NET_IF* pPriIf)
{
    pAliasIf->macType = pPriIf->macType;
    pAliasIf->netMACAddr = pPriIf->netMACAddr;
    pAliasIf->macObjHandle = pPriIf->macObjHandle;
    pAliasIf->hIfMac = pPriIf->hIfMac;
    pAliasIf->pMacConfig = pPriIf->pMacConfig;
    pAliasIf->linkMtu = pPriIf->linkMtu;
}

#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

// external packet processing
#if (TCPIP_STACK_EXTERN_PACKET_PROCESS != 0)
TCPIP_STACK_PROCESS_HANDLE TCPIP_STACK_PacketHandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_STACK_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    TCPIP_STACK_PACKET_HANDLER  pktH = NULL;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);

    if((pNetIf != NULL) && (pNetIf->pktHandler == NULL))
    {
        pNetIf->pktHandlerParam = handlerParam;
        pNetIf->pktHandler = pktHandler;
        pktH = pktHandler;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return FC_PktHndl2ProcHndl(pktH);
}

bool TCPIP_STACK_PacketHandlerDeregister(TCPIP_NET_HANDLE hNet, TCPIP_STACK_PROCESS_HANDLE procHandle)
{
    TCPIP_STACK_PACKET_HANDLER pktH = FC_ProcHndl2PktHndl(procHandle);

    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(hNet);

    if((pNetIf != NULL) && (pNetIf->pktHandler == pktH))
    {
        pNetIf->pktHandler = NULL;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

#else
TCPIP_STACK_PROCESS_HANDLE TCPIP_STACK_PacketHandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_STACK_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    return NULL;
}
bool TCPIP_STACK_PacketHandlerDeregister(TCPIP_NET_HANDLE hNet, TCPIP_STACK_PROCESS_HANDLE procHandle)
{
    return false;
}
#endif  // (TCPIP_STACK_EXTERN_PACKET_PROCESS != 0)

// run time module initialization
#if (M_TCPIP_STACK_RUN_TIME_INIT != 0)
bool TCPIPStack_ModuleIsRunning(TCPIP_STACK_MODULE moduleId)
{
    size_t modId = (size_t)moduleId;

    if((0U < modId) && (modId < (sizeof(TCPIP_MODULES_RUN_TBL) / sizeof(*TCPIP_MODULES_RUN_TBL))))
    {
        TCPIP_MODULE_RUN_DCPT* pRDcpt = TCPIP_MODULES_RUN_TBL + modId;
        return pRDcpt->isRunning != 0U;
    }
    return false;
}
#endif  // (M_TCPIP_STACK_RUN_TIME_INIT != 0)

// local time keeping
// reverts to a sys service, when available

static void F_TCPIP_SecondCountSet(void)
{
    // use a 64 bit count to avoid roll over
    uint64_t tmrCount = SYS_TIME_Counter64Get();

    tcpip_SecCount = (uint32_t)(tmrCount / sysTmrFreq); 
    tcpip_MsecCount = (uint32_t)(tmrCount / (sysTmrFreq / 1000ULL)); 
}

uint32_t TCPIP_SecCountGet(void)
{
    return tcpip_SecCount;
}

uint32_t TCPIP_MsecCountGet(void)
{
    return tcpip_MsecCount;
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


