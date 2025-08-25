/*******************************************************************************
  Internet Protocol (IP) Version 6 Communications Layer

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    -Provides a transport for TCP, UDP, and ICMP messages
    -Reference: RFC
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









#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_IPV6

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_IPV6)
#include "tcpip/src/ipv6_private.h"

#if (TCPIP_IPV6_RIID_ENABLED != 0) || (TCPIP_IPV6_ULA_GENERATE_ENABLE != 0)
#include "crypto/crypto.h"
#endif  // (TCPIP_IPV6_RIID_ENABLED != 0) || (TCPIP_IPV6_ULA_GENERATE_ENABLE != 0)

// The IPv6 version constant
#define IPv6_VERSION            (0x06u)
// This is left shifted by 4.  Actual value is 0x06.
#define IPv6_SHIFT_VERSION      (0x60u)

// IHL (Internet Header Length) is # of 32 bit words in a header.
// Since, we do not support options, our IP header length will be
// minimum i.e. 20 bytes : IHL = 20 / 4 = 5.
#define IP_IHL              (0x05)

#define IP_SERVICE_NW_CTRL  (0x07)
#define IP_SERVICE_IN_CTRL  (0x06)
#define IP_SERVICE_ECP      (0x05)
#define IP_SERVICE_OVR      (0x04)
#define IP_SERVICE_FLASH    (0x03)
#define IP_SERVICE_IMM      (0x02)
#define IP_SERVICE_PRIOR    (0x01)
#define IP_SERVICE_ROUTINE  (0x00)

#define IP_SERVICE_N_DELAY  (0x00)
#define IP_SERCICE_L_DELAY  (0x08)
#define IP_SERVICE_N_THRPT  (0x00)
#define IP_SERVICE_H_THRPT  (0x10)
#define IP_SERVICE_N_RELIB  (0x00)
#define IP_SERVICE_H_RELIB  (0x20)

#define IP_SERVICE          (IP_SERVICE_ROUTINE | IP_SERVICE_N_DELAY)

#if defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
  #define MY_IP_TTL           (255)  // Time-To-Live in hops
  // IP TTL is set to 255 for Multicast DNS compatibility. See mDNS-draft-08, section 4.
#else
  #define MY_IP_TTL           (100)  // Time-To-Live in hops
#endif

// The IPv6 unspecified address
const IPV6_ADDR IPV6_FIXED_ADDR_UNSPECIFIED = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
// The IPv6 all-nodes multicast addres
const IPV6_ADDR IPV6_FIXED_ADDR_ALL_NODES_MULTICAST = {{0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}};
// The IPv6 all-routers multicast address
const IPV6_ADDR IPV6_FIXED_ADDR_ALL_ROUTER_MULTICAST = {{0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}};
// The IPv6 solicited node multicast address mask
const IPV6_ADDR IPV6_SOLICITED_NODE_MULTICAST = {{0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00}};

const TCPIP_MAC_ADDR  IPV6_MULTICAST_MAC_ADDRESS = {{0x33, 0x33, 0x00, 0x00, 0x00, 0x01}};
static int                  ipv6InitCount = 0;            // Indicator of how many interfaces are initializing the IPv6 module

static uint32_t             fragmentId = 0U;             // Static ID to use when sending fragmented packets

static size_t               nStackIfs = 0U;              // number of interfaces the stack is currently running on

static PROTECTED_SINGLE_LIST    ipv6RegisteredUsers = { {0} };

static PROTECTED_SINGLE_LIST    mcastQueue = { {0} };

static int                  ipv6ModuleInitCount = 0;      // Indicator of how many times the IP module has been initialized


static TCPIP_SIGNAL_HANDLE    ipv6TaskHandle = NULL;          // Handle for the IPv6 task

static uint32_t             ipv6StartTick = 0;

static const void*          ipv6MemH = NULL;                     // memory handle

static PROTECTED_SINGLE_LIST    ipv6QueuedPackets = { {0} };

// Enumeration defining IPv6 initialization states
typedef enum
{
    IPV6_INIT_STATE_NONE = 0,                           // IPv6 initialization is not in progress
    IPV6_INIT_STATE_INITIALIZE,                         // Initializes IPv6 variables
    IPV6_INIT_STATE_DAD,                                // Duplicate address detection is being performed on the interfaces link-local unicast address
    IPV6_INIT_STATE_SOLICIT_ROUTER,                     // The interface is soliciting routers
    IPV6_INIT_STATE_DONE,                               // The interface is up
    IPV6_INIT_STATE_FAIL                                // The initialization has failed
} IPV6_INIT_STATE;


// IPv6 address policy table for default address selection
static const IPV6_ADDRESS_POLICY gPolicyTable[] = {
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}, 128, 50,  0},          // Loopback address
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},   0, 40,  1},            // Unspecified address
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00}},  96, 35,  4},           // IPv4-mapped address
    {{{0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  16, 30,  2},           // 2002::/15 - 6to4
    {{{0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  32,  5,  5},            // 2001::/32 - Teredo tunneling
    {{{0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},   7,  3, 13},            // ULA
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  96,  1,  3},            //
    {{{0xfe, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  10,  1, 11},            // 
    {{{0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  16,  1, 12},            // 

    {{{0}},0xFF,0,0},
};

// Array of global configuration and state variables for an IPv6 interface
static IPV6_INTERFACE_CONFIG* ipv6Config = NULL;

// ULA state machine
#if (TCPIP_IPV6_ULA_GENERATE_ENABLE != 0)
static TCPIP_IPV6_ULA_STATE ulaState = TCPIP_IPV6_ULA_IDLE;
static TCPIP_NET_IF*    ulaNetIf = NULL;
static uint16_t         ulaSubnetId;    // subnet to generate for
static uint8_t          ulaFlags;       // IPV6_ULA_FLAGS value
static uint32_t         ulaOperStartTick;


#endif  // (TCPIP_IPV6_ULA_GENERATE_ENABLE != 0)

#if (TCPIP_IPV6_EXTERN_PACKET_PROCESS != 0)
static TCPIP_IPV6_PACKET_HANDLER ipv6PktHandler = NULL;
static const void* ipv6PktHandlerParam;
#endif  // (TCPIP_IPV6_EXTERN_PACKET_PROCESS != 0)

static TCPIP_IPV6_CONFIG_DCPT  ipv6_config_dcpt;

static TCPIP_IPV6_TX_PRI_HANDLER ipv6PriPktHandler = NULL;
/************************************************************************/
/****************               Prototypes               ****************/
/************************************************************************/

// performs resources cleanup
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_TCPIP_IPV6_Cleanup(const void* memH);
static void TCPIP_IPV6_ProtectedSingleListFree (PROTECTED_SINGLE_LIST * list);
#else
#define F_TCPIP_IPV6_Cleanup(memH)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void F_TCPIP_IPV6_QueuedPacketTransmitTask (PROTECTED_SINGLE_LIST* pList);

static void F_TCPIP_IPV6_PacketEnqueue(IPV6_PACKET * pkt, SINGLE_LIST* pList, size_t queueLimit, uint32_t tmoSeconds, bool isProtected);

#if (TCPIP_IPV6_ULA_GENERATE_ENABLE != 0)

static void TCPIP_IPV6_UlaTask (void);
static void TCPIP_IPV6_EUI64(TCPIP_NET_IF* pNetIf, uint64_t* pRes);

#endif  // (TCPIP_IPV6_ULA_GENERATE_ENABLE != 0)

// MAC API TX functions
static uint16_t             TCPIP_IPV6_PacketPayload(IPV6_PACKET* pkt);
static TCPIP_MAC_PACKET*    TCPIP_IPV6_MacPacketTxAllocate(IPV6_PACKET* pkt, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags);
static void                 TCPIP_IPV6_MacPacketTxPutHeader(IPV6_PACKET* pkt, TCPIP_MAC_PACKET* pMacPkt, uint16_t pktType);
static void                 TCPIP_IPV6_MacPacketTxAck(TCPIP_MAC_PACKET* pMacPkt,  const void* param);
static void                 TCPIP_IPV6_MacPacketTxAddSegments(IPV6_PACKET* ptrPacket, TCPIP_MAC_PACKET* pMacPkt, uint16_t segFlags);

// MAC API RX functions
typedef uint8_t*    TCPIP_MAC_PTR_TYPE; 
static void                 MACSetReadPtrInRx(TCPIP_MAC_PACKET* pRxPkt, uint16_t offset);
static TCPIP_MAC_PTR_TYPE   MACSetReadPtr(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PTR_TYPE address);
static uint16_t             MACGetArray(TCPIP_MAC_PACKET* pRxPkt, uint8_t *address, uint16_t len);
static TCPIP_MAC_PTR_TYPE   MACGetReadPtrInRx(TCPIP_MAC_PACKET* pRxPkt);

static TCPIP_IPV6_RESULT    TCPIP_IPV6_NewRouterEntry(const TCPIP_NET_IF* pNetIf, const IPV6_ADDR* pGwAddr, const TCPIP_MAC_ADDR* pGMacAddr, TCPIP_IPV6_NEIGHBOR_FLAGS flags, unsigned long validTime);

static void TCPIP_IPV6_InitializeTask (void);

static void TCPIP_IPV6_Timeout (void);

static void TCPIP_IPV6_ProcessPackets(void);

static void TCPIP_IPV6_Process (const TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt);

static void F_TCPIP_IPV6_EUI64InterfaceIDSet (IPV6_ADDR *pAddress, const TCPIP_MAC_ADDR * pNetMACAddr);

#if (TCPIP_IPV6_RIID_ENABLED != 0)    
static bool F_TCPIP_IPV6_RandomInterfaceIDSet(const TCPIP_NET_IF* pNetIf, IPV6_ADDR* ip6Addr, uint8_t prefixLen, uint8_t dadCounter);
#endif  // (TCPIP_IPV6_RIID_ENABLED != 0)    

static void TCPIP_IPV6_FragmentBufferFree (IPV6_RX_FRAGMENT_BUFFER* pFragBuffer);

static IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_DataSegmentHeaderAllocate (uint16_t len);

static unsigned short TCPIP_IPV6_PseudoHeaderChecksumGet (IPV6_PACKET * ptrPacket);

static void TCPIP_IPV6_PacketSegmentInsert (IPV6_DATA_SEGMENT_HEADER * ptrSegment, IPV6_PACKET * ptrPacket, uint8_t type);

static IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_DataSegmentGetByType (IPV6_PACKET * ptrPacket, uint8_t type);

static void * TCPIP_IPV6_DataSegmentContentsGetByType (IPV6_PACKET * ptrPacket, uint8_t type);

static void TCPIP_IPV6_PacketDataFree (IPV6_PACKET * ptrPacket);

static IPV6_ACTION TCPIP_IPV6_HopByHopOptionsHeaderProcess (const TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length);

static IPV6_ACTION TCPIP_IPV6_DestinationOptionsHeaderProcess (const TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length);

static IPV6_ACTION TCPIP_IPV6_RoutingHeaderProcess (const TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length);

static IPV6_ACTION TCPIP_IPV6_FragmentationHeaderProcess(const TCPIP_NET_IF* pNetIf, const IPV6_ADDR* remoteIP, const IPV6_ADDR* localIP, uint8_t* nextHeader, uint16_t dataCount, uint16_t headerLen, TCPIP_MAC_PACKET* pRxPkt, uint16_t previousHeaderLen);

static void TCPIP_IPV6_FragPktAck(TCPIP_MAC_PACKET* pkt,  const void* param);

static IPV6_ADDR_STRUCT* TCPIP_IPV6_SolicitedNodeMulticastAddressFind(const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * addr, uint8_t listType);

static bool TCPIP_IPV6_DASPolicyGet (const IPV6_ADDR * addr, uint8_t * label, uint8_t * precedence, uint8_t * prefixLen);

static void TCPIP_IPV6_FragmentTask (void);

static void TCPIP_IPV6_TimestampsTaskUpdate (void);

static bool TCPIP_IPV6_PacketTransmitInFragments (IPV6_PACKET * pkt, uint16_t mtu);

static void TCPIP_IPV6_FreeConfigLists (IPV6_INTERFACE_CONFIG * pNetIf);

static bool TCPIP_IPV6_ASCompareSourceAddresses(TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * addressOne, IPV6_ADDR_STRUCT * addressTwo, const IPV6_ADDR * dest, uint8_t rule);

static __inline__ uint16_t __attribute__((always_inline)) TCPIP_IPV6_GetOptionHeader(TCPIP_MAC_PACKET* pRxPkt, uint8_t* pData, uint16_t len)
{
    return MACGetArray(pRxPkt, pData, len << 3U); 
}

static uint8_t TCPIP_IPV6_TxPriQueue(const TCPIP_NET_IF* pNetIf, uint8_t dscp);

// conversion functions/helpers
//
static __inline__ IPV6_DATA_SEGMENT_HEADER_FULL* __attribute__((always_inline)) FC_SegHdr2FullHdr(IPV6_DATA_SEGMENT_HEADER* pSegHdr)
{
    union
    {
        IPV6_DATA_SEGMENT_HEADER* pSegHdr;
        IPV6_DATA_SEGMENT_HEADER_FULL*  pSegFull;
    }U_SEG_HDR_FULL_HDR;

    U_SEG_HDR_FULL_HDR.pSegHdr = pSegHdr;
    return U_SEG_HDR_FULL_HDR.pSegFull;
}

static __inline__ IPV6_FRAGMENT_HEADER* __attribute__((always_inline)) FC_SegHdr2FragHdr(IPV6_DATA_SEGMENT_HEADER* pSegHdr)
{
    IPV6_DATA_SEGMENT_HEADER_FULL*  pSegFull = FC_SegHdr2FullHdr(pSegHdr);
    union
    {
        uint32_t*  pData;
        IPV6_FRAGMENT_HEADER* pFragHdr;
    }U_SEG_HDR_FRAG_HDR;

    U_SEG_HDR_FRAG_HDR.pData = pSegFull->data;
    return U_SEG_HDR_FRAG_HDR.pFragHdr;
}

static __inline__ uint8_t* __attribute__((always_inline)) FC_SegHdrDataPtr(IPV6_DATA_SEGMENT_HEADER* pSegHdr)
{
    union
    {
        IPV6_DATA_SEGMENT_HEADER* pSegHdr;
        IPV6_DATA_SEGMENT_HEADER_FULL*  pSegFull;
    }U_SEG_HDR_FULL_HDR;

    U_SEG_HDR_FULL_HDR.pSegHdr = pSegHdr;
    return (uint8_t*)U_SEG_HDR_FULL_HDR.pSegFull->data;
}

static __inline__ IPV6_DATA_SEGMENT_HEADER* __attribute__((always_inline)) FC_VPtr2SegHdr(void* vPtr)
{
    union
    {
        void*  vPtr;
        IPV6_DATA_SEGMENT_HEADER* pSegHdr;
    }U_VPTR_SEG_HDR;

    U_VPTR_SEG_HDR.vPtr = vPtr;
    return U_VPTR_SEG_HDR.pSegHdr;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_Ip6Pkt2SglNode(IPV6_PACKET* pkt)
{
    union
    {
        IPV6_PACKET* pkt;
        SGL_LIST_NODE*  node;
    }U_IP6_PKT_SGL_NODE;

    U_IP6_PKT_SGL_NODE.pkt = pkt;
    return U_IP6_PKT_SGL_NODE.node;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_FragBuff2SglNode(IPV6_RX_FRAGMENT_BUFFER* fragBuff)
{
    union
    {
        IPV6_RX_FRAGMENT_BUFFER* fragBuff;
        SGL_LIST_NODE*  node;
    }U_FRAG_BUFF_SGL_NODE;

    U_FRAG_BUFF_SGL_NODE.fragBuff = fragBuff;
    return U_FRAG_BUFF_SGL_NODE.node;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_Ip6Hndl2SglNode(IPV6_HANDLE hIpv6)
{
    union
    {
        IPV6_HANDLE hIpv6;
        SGL_LIST_NODE*  node;
    }U_IP6_HNDL_SGL_NODE;

    U_IP6_HNDL_SGL_NODE.hIpv6 = hIpv6;
    return U_IP6_HNDL_SGL_NODE.node;
}

static __inline__ IPV6_RX_FRAGMENT_BUFFER* __attribute__((always_inline)) FC_SglNode2FragBuff(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        IPV6_RX_FRAGMENT_BUFFER* pFragBuff;
    }U_SGL_NODE_FRAG_BUFF;

    U_SGL_NODE_FRAG_BUFF.node = node;
    return U_SGL_NODE_FRAG_BUFF.pFragBuff;
}

static __inline__ IPV6_LIST_NODE* __attribute__((always_inline)) FC_SglNode2Ip6ListNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        IPV6_LIST_NODE* ip6Node;
    }U_SGL_NODE_IP6_NODE;

    U_SGL_NODE_IP6_NODE.node = node;
    return U_SGL_NODE_IP6_NODE.ip6Node;
}

static __inline__ TCPIP_IPV6_ULA_STATE __attribute__((always_inline)) FC_UlaStateAddVal(TCPIP_IPV6_ULA_STATE currState, int addVal)
{
    union
    {
        TCPIP_IPV6_ULA_STATE  currState;
        int currVal;
    }U_ULA_STATE_VAL;

    U_ULA_STATE_VAL.currState = currState;
    U_ULA_STATE_VAL.currVal += addVal;

    return U_ULA_STATE_VAL.currState;
}

static __inline__ TCPIP_IPV6_PROCESS_HANDLE __attribute__((always_inline)) FC_PktHndl2ProcHndl(TCPIP_IPV6_PACKET_HANDLER pktHndl)
{
    union
    {
        TCPIP_IPV6_PACKET_HANDLER  pktHndl;
        TCPIP_IPV6_PROCESS_HANDLE procHndl;
    }U_PKT_HNDL_PROC_HNDL;

    U_PKT_HNDL_PROC_HNDL.pktHndl = pktHndl;
    return U_PKT_HNDL_PROC_HNDL.procHndl;
}



// ipv6_manager.h
bool TCPIP_IPV6_Initialize(const TCPIP_STACK_MODULE_CTRL* const pStackInit, const void* initData)
{
    TCPIP_NET_IF* pNetIf;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    bool    iniRes;

    // if(pStackInit->stackAction == TCPIP_STACK_ACTION_INIT)   // stack going up
    // if(pStackInit->stackAction == TCPIP_STACK_ACTION_IF_UP)  // interface is going up

    if(pStackInit->stackAction == (uint8_t)TCPIP_STACK_ACTION_INIT)
    {   // stack going up
        const TCPIP_IPV6_MODULE_CONFIG* pIpv6Init = (const TCPIP_IPV6_MODULE_CONFIG*)initData;
        if(pIpv6Init == NULL)
        {
            return false;
        }

        // stack initialization
        if(ipv6ModuleInitCount == 0)
        {   // 1st time we run
            ipv6MemH = pStackInit->memH;
            // save the max number of interfaces the stack is working on
            nStackIfs = pStackInit->nIfs;

            // Initialize the global fragment ID
            fragmentId = 0;

            ipv6Config = (IPV6_INTERFACE_CONFIG*)TCPIP_HEAP_Calloc(pStackInit->memH, pStackInit->nIfs, sizeof(*ipv6Config));
            if(ipv6Config == NULL)
            {   // failed
                return false;
            }

            while(true)
            {
                iniRes = (ipv6TaskHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_IPV6_Task, TCPIP_IPV6_INIT_TASK_PROCESS_RATE)) != NULL;

                if(iniRes == false)
                {
                    break;
                }

                if((iniRes = TCPIP_Notification_Initialize(&ipv6RegisteredUsers)) == false)
                {
                    break;
                }

                if((iniRes = TCPIP_Helper_ProtSglListInitialize (&mcastQueue)) == false)
                {
                    break;
                }

                if((iniRes = TCPIP_Helper_ProtSglListInitialize (&ipv6QueuedPackets)) == false)
                {
                    break;
                }

#if (TCPIP_IPV6_RIID_ENABLED != 0)
                if ((pIpv6Init->configFlags & TCPIP_IPV6_FLAG_RANDOM_INTERFACE_ID) != 0)
                {
                    if(pIpv6Init->pRiidFnc == NULL && pIpv6Init->pSecretKeyFnc == NULL)
                    {
                        iniRes = false;
                        break;
                    }
                }
#endif  // (TCPIP_IPV6_RIID_ENABLED != 0)


#if (TCPIP_IPV6_EXTERN_PACKET_PROCESS != 0)
                ipv6PktHandler = NULL;
#endif  // (TCPIP_IPV6_EXTERN_PACKET_PROCESS != 0)

                break;
            }

            if(iniRes == false)
            {
                F_TCPIP_IPV6_Cleanup(pStackInit->memH);
                return false;
            }

            ipv6StartTick = 0;

            // initialize IPv6 configuration parameters
            ipv6_config_dcpt.rxfragmentBufSize = pIpv6Init->rxfragmentBufSize;
            ipv6_config_dcpt.fragmentPktRxTimeout = pIpv6Init->fragmentPktRxTimeout;
#if (TCPIP_IPV6_RIID_ENABLED != 0)
            ipv6_config_dcpt.riidPrF = pIpv6Init->pRiidFnc;
            ipv6_config_dcpt.riidNetIfaceF = pIpv6Init->pNetIfaceFnc;
            ipv6_config_dcpt.riidNetIdF = pIpv6Init->pNetworkIdFnc;
            ipv6_config_dcpt.riidKeyF = pIpv6Init->pSecretKeyFnc;
#endif // (TCPIP_IPV6_RIID_ENABLED != 0)
            ipv6_config_dcpt.configFlags = (uint16_t)pIpv6Init->configFlags;

            ipv6PriPktHandler = NULL;
        }

        ipv6ModuleInitCount++;
    }


    // init this interface
    pNetIf = pStackInit->pNetIf;

    // Check to see if this interface is already being initialized
    if (pNetIf->Flags.bIPv6InConfig == 0U)
    {
        pNetIf->Flags.bIPv6InConfig = 1U;
        pIpv6Config = ipv6Config + pStackInit->netIx;

        // Initialize the IPv6 parameters for this net
        pIpv6Config->initState = (uint8_t)IPV6_INIT_STATE_INITIALIZE;
        TCPIP_Helper_DoubleListInitialize (&pIpv6Config->listIpv6UnicastAddresses);
        TCPIP_Helper_DoubleListInitialize (&pIpv6Config->listIpv6MulticastAddresses);
        TCPIP_Helper_DoubleListInitialize (&pIpv6Config->listIpv6TentativeAddresses);
        TCPIP_Helper_SingleListInitialize (&pIpv6Config->listNeighborCache);
        TCPIP_Helper_SingleListInitialize (&pIpv6Config->listDefaultRouter);
        TCPIP_Helper_SingleListInitialize (&pIpv6Config->listDestinationCache);
        TCPIP_Helper_SingleListInitialize (&pIpv6Config->listPrefixList);
        TCPIP_Helper_SingleListInitialize (&pIpv6Config->rxFragments);

        pIpv6Config->currentDefaultRouter = NULL;
        pIpv6Config->baseReachableTime = TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME;
        pIpv6Config->reachableTime = TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME;
        pIpv6Config->retransmitTime = TCPIP_IPV6_DEFAULT_RETRANSMIT_TIME;
        pIpv6Config->linkMTU = TCPIP_IPV6_DEFAULT_LINK_MTU;
        pIpv6Config->multicastMTU = TCPIP_IPV6_DEFAULT_LINK_MTU;
        pIpv6Config->mtuIncreaseTimer = 0;
        pIpv6Config->curHopLimit = TCPIP_IPV6_DEFAULT_CUR_HOP_LIMIT;

        pIpv6Config->policyPreferTempOrPublic = (uint8_t)IPV6_PREFER_PUBLIC_ADDRESSES;
        
        ipv6InitCount++;
    }

    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_TCPIP_IPV6_Cleanup(const void* memH)
{
    TCPIP_Notification_Deinitialize(&ipv6RegisteredUsers, ipv6MemH);
    if(ipv6TaskHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(ipv6TaskHandle);
        ipv6TaskHandle = NULL;
    }

    if(ipv6Config != NULL)
    {
        (void)TCPIP_HEAP_Free(memH, ipv6Config);
        ipv6Config = NULL;
    }

    // kill the multicast queue
    TCPIP_IPV6_ProtectedSingleListFree(&mcastQueue);

    // kill the queued packets
    TCPIP_IPV6_ProtectedSingleListFree(&ipv6QueuedPackets);

    ipv6MemH = NULL;
}

static void TCPIP_IPV6_ProtectedSingleListFree (PROTECTED_SINGLE_LIST * list)
{
    SGL_LIST_NODE * node;
    while ((node = TCPIP_Helper_ProtSglListHeadRemove(list)) != NULL)
    {
        (void)TCPIP_HEAP_Free (ipv6MemH, node);
    }
    TCPIP_Helper_ProtSglListDeinitialize (list);

}

#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

/*****************************************************************************
  Function:
    void TCPIP_IPV6_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)

  Summary:
    Disables IPv6 functionality on the specified interface.

  Description:
    This function will disable IPv6 functionality on a specified interface. 
    It will free any dynamically allocated structures.

  Precondition:
    None

  Parameters:
    stackCtrl - Stack initialization parameters

  Returns:
    None
    
  Remarks:
    None
  ***************************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_IPV6_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    TCPIP_NET_IF* pNetIf;

    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    // one way or another this interface is going down

    if(ipv6ModuleInitCount != 0)
    {
        pNetIf = stackCtrl->pNetIf;
        TCPIP_IPV6_InitializeStop (pNetIf);
        TCPIP_IPV6_FreeConfigLists (ipv6Config + stackCtrl->netIx);
        pNetIf->Flags.bIPv6Enabled = 0U;

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down
            if(--ipv6ModuleInitCount == 0)
            {
                F_TCPIP_IPV6_Cleanup(stackCtrl->memH);
                nStackIfs = 0U;
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


// ipv6_manager.h
void TCPIP_IPV6_InitializeStop (TCPIP_NET_IF * pNetIf)
{
    if(ipv6InitCount != 0)
    {   // We're up and running
        if (pNetIf->Flags.bIPv6InConfig == 1U)
        {
            pNetIf->Flags.bIPv6InConfig = 0U;
            if (--ipv6InitCount == 0)
            {   // stop the timing for the init task!
                (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, ipv6TaskHandle, TCPIP_IPV6_TASK_PROCESS_RATE);
            }
        }
    }
}


// ipv6_manager.h
static void TCPIP_IPV6_InitializeTask (void)
{
    IPV6_ADDR_STRUCT * localAddressPointer;
    IPV6_ADDR linkLocalAddress = {{0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    const uint8_t linkLocalPrefixLen = 10;  // LL address: fe80::/10
    uint8_t dadCount;
    uint8_t dadCountLast;
    size_t netIx;
    TCPIP_NET_IF * pNetIf;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    const IPV6_ADDR* pStatAddr, *pGatewayAddr;
    uint8_t statPrefixLen;

    for (netIx = 0; netIx < TCPIP_STACK_NumberOfNetworksGet(); netIx++)
    {
        pNetIf = FC_NetH2NetIf(TCPIP_STACK_IndexToNet (netIx));
        pIpv6Config = ipv6Config + netIx;

        if ((pNetIf->Flags.bIPv6InConfig == 1U) && (TCPIP_STACK_NetworkIsLinked(pNetIf)))
        {
            switch (pIpv6Config->initState)
            {
                case (uint8_t)IPV6_INIT_STATE_INITIALIZE:
                    // Add the all-nodes multicast listener to this node
                    localAddressPointer = TCPIP_IPV6_MulticastListenerAdd (pNetIf, &IPV6_FIXED_ADDR_ALL_NODES_MULTICAST);
                    if (localAddressPointer == NULL)
                    {
                        pIpv6Config->initState = (uint8_t)IPV6_INIT_STATE_FAIL;
                        break;
                    }
                    // Configure link-local address
                    if (TCPIP_IPV6_AddressInterfaceIDSet(pNetIf, &linkLocalAddress, linkLocalPrefixLen, 0) == false)
                    {
                        pIpv6Config->initState = (uint8_t)IPV6_INIT_STATE_FAIL;
                        break;
                    }

                    localAddressPointer = TCPIP_IPV6_UnicastAddressAdd (pNetIf, &linkLocalAddress, 0U, 0U);
                    if (localAddressPointer == NULL)
                    {
                        pIpv6Config->initState = (uint8_t)IPV6_INIT_STATE_FAIL;
                        break;
                    }

                    // Configure static IPv6 address
                    pStatAddr = TCPIP_STACK_NetStaticIPv6AddressGet(pNetIf, &statPrefixLen);
                    if(pStatAddr != NULL)
                    {
                        localAddressPointer = TCPIP_IPV6_UnicastAddressAdd (pNetIf, pStatAddr, statPrefixLen, 0U);
                        if(localAddressPointer == NULL )
                        {
                            pIpv6Config->initState = (uint8_t)IPV6_INIT_STATE_FAIL;
                            break;
                        }
                    }
                    
                    // Enable IPv6 functionality for now so we can process ICMPv6 messages for stateless address autoconfiguration
                    pNetIf->Flags.bIPv6Enabled = 1U;
                    pIpv6Config->initState = (uint8_t)IPV6_INIT_STATE_DAD;
                    break;
                case (uint8_t)IPV6_INIT_STATE_DAD:
#if (TCPIP_IPV6_RIID_ENABLED != 0)
                    if ((ipv6_config_dcpt.configFlags & TCPIP_IPV6_FLAG_RANDOM_INTERFACE_ID) != 0)
                    {
                        dadCountLast = IPV6_RIID_IDGEN_RETRIES - 1U;
                    }
                    else
#endif // (TCPIP_IPV6_RIID_ENABLED != 0)
                    {
                        dadCountLast = 0;
                    }
                    dadCount = 0;
                    while(true)
                    {
                        if (TCPIP_IPV6_AddressInterfaceIDSet(pNetIf, &linkLocalAddress, linkLocalPrefixLen, dadCount) == false)
                        {
                            pIpv6Config->initState = (uint8_t)IPV6_INIT_STATE_FAIL;
                            break;
                        }

                        if (TCPIP_IPV6_AddressFind (pNetIf, &linkLocalAddress, IPV6_ADDR_TYPE_UNICAST) != NULL)
                        {
                            pGatewayAddr = TCPIP_STACK_NetDefaultIPv6GatewayGet(pNetIf);
                            if(pGatewayAddr != NULL)
                            {   // add a new router entry, valid forever
                                if(TCPIP_IPV6_NewRouterEntry(pNetIf, pGatewayAddr, NULL, TCPIP_IPV6_NEIGHBOR_FLAG_NONE, 0xffffffffU) != TCPIP_IPV6_RES_OK)
                                {
                                    pIpv6Config->initState = (uint8_t)IPV6_INIT_STATE_FAIL;
                                    break;
                                }
                            }

                            pIpv6Config->initState = (uint8_t)IPV6_INIT_STATE_SOLICIT_ROUTER;
                            break;
                        }
                        else if (TCPIP_IPV6_AddressFind (pNetIf, &linkLocalAddress, IPV6_ADDR_TYPE_UNICAST_TENTATIVE) == NULL)
                        {
                            if (dadCount == dadCountLast)
                            {
                                pIpv6Config->initState = (uint8_t)IPV6_INIT_STATE_FAIL;
                                break;
                            }
                        }
                        else
                        {
                            // stay in IPV6_INIT_STATE_DAD 
                            break;
                        }
#if (TCPIP_IPV6_RIID_ENABLED != 0)
                        dadCount++;
#endif  // (TCPIP_IPV6_RIID_ENABLED != 0)
                    }
                    break;
                case (uint8_t)IPV6_INIT_STATE_SOLICIT_ROUTER:
                    if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_NO_RS) == 0U)
                    {
                        TCPIP_NDP_RouterSolicitStart(pNetIf);
                    }
                    pIpv6Config->initState = (uint8_t)IPV6_INIT_STATE_DONE;
                    break;
                case (uint8_t)IPV6_INIT_STATE_DONE:
                    TCPIP_IPV6_InitializeStop (pNetIf);
                    break;
                case (uint8_t)IPV6_INIT_STATE_FAIL:
                    TCPIP_IPV6_FreeConfigLists (pIpv6Config);
                    TCPIP_IPV6_InitializeStop (pNetIf);
                    pNetIf->Flags.bIPv6Enabled = 0U;
                    break;
                default:
                    // do nothing
                    break;
            }
        }
    }
}


// ipv6.h
bool TCPIP_IPV6_InterfaceIsReady (TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF * pNetIf =  TCPIPStackHandleToNetUp(netH);

    if(pNetIf != NULL)
    {
        if (pNetIf->Flags.bIPv6InConfig == 0U && pNetIf->Flags.bIPv6Enabled != 0U)
        {
            return true;
        }
    }

    return false;
}


// ipv6.h
IPV6_PACKET * TCPIP_IPV6_TxPacketAllocate (TCPIP_NET_HANDLE netH, IPV6_PACKET_ACK_FNC ack6Fnc, void* ack6Param)
{

    TCPIP_NET_IF * pNetIf =  TCPIPStackHandleToNet(netH);
    IPV6_PACKET * ptrPacket = (IPV6_PACKET *)TCPIP_HEAP_Malloc (ipv6MemH, sizeof (IPV6_PACKET));

    if (ptrPacket == NULL)
    {
        return NULL;
    }

    ptrPacket->next = NULL;
    ptrPacket->netIfH = pNetIf;
    ptrPacket->flags.val = 0;
    ptrPacket->upperLayerChecksumOffset = IPV6_NO_UPPER_LAYER_CHECKSUM;

    ptrPacket->payload.dataLocation = (uint8_t*)&ptrPacket->ipv6Header;
    ptrPacket->payload.segmentSize = (uint16_t)sizeof (IPV6_HEADER);
    ptrPacket->payload.segmentLen = (uint16_t)sizeof (IPV6_HEADER);
    ptrPacket->offsetInSegment = 0U;

    ptrPacket->payload.memory = IPV6_DATA_PIC_RAM;
    ptrPacket->payload.segmentType = (uint8_t)TYPE_IPV6_HEADER;
    ptrPacket->payload.nextSegment = NULL;

    TCPIP_IPV6_PacketIPProtocolSet (ptrPacket);

    ptrPacket->headerLen = 0;
    ptrPacket->flags.queued = 0U;
    ptrPacket->flags.sourceSpecified = 0U;
    ptrPacket->flags.useUnspecAddr = 0U;

    ptrPacket->neighbor = NULL;

    ptrPacket->payloadLen = 0U;
    ptrPacket->upperLayerHeaderLen = 0U;
    (void)memset (&ptrPacket->remoteMACAddr, 0x00, sizeof (TCPIP_MAC_ADDR));

    ptrPacket->ack6Fnc = ack6Fnc;
    ptrPacket->ack6Param = ack6Param;
    ptrPacket->macAckFnc = NULL;

    return ptrPacket;
}


// ipv6_manager.h
bool TCPIP_IPV6_TxPacketStructCopy (IPV6_PACKET * destination, IPV6_PACKET * source)
{
    IPV6_DATA_SEGMENT_HEADER *ptrSegmentSource, *ptrSegmentDest;

    if (destination == NULL || source == NULL)
    {
        return false;
    }
    if(destination->flags.addressType != source->flags.addressType || destination->flags.addressType != (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        return false;
    }

    (void)memcpy ((void *)&destination->remoteMACAddr, (void *)&source->remoteMACAddr, sizeof (TCPIP_MAC_ADDR));
    destination->flags.useUnspecAddr = source->flags.useUnspecAddr;
    destination->flags.sourceSpecified = source->flags.sourceSpecified;
    destination->flags.addressType = (uint8_t)IP_ADDRESS_TYPE_IPV6;


    (void)memcpy ((void *)&destination->payload, (void *)&source->payload, sizeof (IPV6_DATA_SEGMENT_HEADER));

    destination->payload.nextSegment = NULL;

    destination->neighbor = source->neighbor;
    (void)memcpy ((void *)&destination->ipv6Header, (void *)&source->ipv6Header, sizeof (IPV6_HEADER));
    destination->ipv6Header.PayloadLength = 0x0000;

    destination->payload.dataLocation = (uint8_t*)&destination->ipv6Header;

    if ((ptrSegmentSource = TCPIP_IPV6_DataSegmentGetByType (source, (uint8_t)TYPE_IPV6_UPPER_LAYER_HEADER)) != NULL)
    {
        if ((ptrSegmentDest = TCPIP_IPV6_DataSegmentGetByType (destination, ptrSegmentSource->segmentType)) == NULL)
        {
            ptrSegmentDest = TCPIP_IPV6_DataSegmentHeaderAllocate (ptrSegmentSource->segmentSize);
            if (ptrSegmentDest == NULL)
            {
                return false;
            }
            ptrSegmentDest->nextSegment = NULL;
            TCPIP_IPV6_PacketSegmentInsert (ptrSegmentDest, destination, ptrSegmentSource->segmentType);
        }

        (void)memcpy ((void *)ptrSegmentDest->dataLocation, (void *)ptrSegmentSource->dataLocation, ptrSegmentSource->segmentLen);
        ptrSegmentDest->segmentLen = ptrSegmentSource->segmentLen;

        destination->upperLayerHeaderLen = source->upperLayerHeaderLen;
        destination->upperLayerChecksumOffset = source->upperLayerChecksumOffset;
        destination->upperLayerHeaderType = source->upperLayerHeaderType;
    }

    return true;
}


// ipv6_manager.h
void TCPIP_IPV6_PacketIPProtocolSet (IPV6_PACKET * ptrPacket)
{

    ptrPacket->payload.segmentLen = (uint16_t)sizeof (IPV6_HEADER);
    ptrPacket->flags.addressType = (uint8_t)IP_ADDRESS_TYPE_IPV6;
}


// ipv6_manager.h
void TCPIP_IPV6_HeaderPut(IPV6_PACKET * ptrPacket, uint8_t protocol, uint8_t dscp)
{
    if(ptrPacket->flags.addressType != (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        return;
    }

    void * ptrSegment = TCPIP_IPV6_DataSegmentContentsGetByType (ptrPacket, (uint8_t)TYPE_IPV6_HEADER);
    if (ptrSegment == NULL)
    {
        return;
    }

    IPV6_HEADER* pHdr = FC_VPtr2IPv6Hdr(ptrSegment);

    IPV6_VTF vtf;
    vtf.val = 0U;
    vtf.version = IPv6_VERSION; 
    vtf.ds = dscp; 
    pHdr->V_T_F = TCPIP_Helper_htonl(vtf.val);
    pHdr->HopLimit = 0U;
}


// ipv6_manager.h
IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_UpperLayerHeaderPut (IPV6_PACKET * ptrPacket, void * header, unsigned short len, unsigned char type, unsigned short checksumOffset)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate(len);

    if (ptrSegment == NULL)
    {
        return NULL;
    }

    ptrSegment->segmentLen = len;

    if (header != NULL)
    {
        IPV6_DATA_SEGMENT_HEADER_FULL* pFullSeg = FC_SegHdr2FullHdr(ptrSegment);
        (void)memcpy ((void*)pFullSeg->data, header, len);
    }

    ptrPacket->upperLayerHeaderLen = len;
    ptrPacket->upperLayerHeaderType = type;
    ptrPacket->upperLayerChecksumOffset = checksumOffset;

    TCPIP_IPV6_PacketSegmentInsert (ptrSegment, ptrPacket, (uint8_t)TYPE_IPV6_UPPER_LAYER_HEADER);

    return ptrSegment;
}


static void TCPIP_IPV6_PacketSegmentInsert (IPV6_DATA_SEGMENT_HEADER * ptrSegment, IPV6_PACKET * ptrPacket, uint8_t type)
{
    IPV6_DATA_SEGMENT_HEADER * tempSegment;
    uint8_t * nextHeader;

    if ((ptrSegment == NULL) || (ptrPacket == NULL))
    {
        return;
    }

    tempSegment = &ptrPacket->payload;

    ptrSegment->segmentType = type;

    if (type == (uint8_t)TYPE_IPV6_END_OF_LIST)
    {
        while (tempSegment->nextSegment != NULL)
        {
            tempSegment = tempSegment->nextSegment;
        }
    }
    else
    {
        while ((tempSegment->nextSegment != NULL) && (ptrSegment->segmentType > tempSegment->nextSegment->segmentType))
        {
            tempSegment = tempSegment->nextSegment;
        }
    }
    ptrSegment->nextSegment = tempSegment->nextSegment;
    tempSegment->nextSegment = ptrSegment;

    if (type == (uint8_t)TYPE_IPV6_END_OF_LIST)
    {
        ptrSegment->segmentType = (uint8_t)TYPE_IPV6_UPPER_LAYER_PAYLOAD;
    }

    if (tempSegment->segmentType < (uint8_t)TYPE_IPV6_UPPER_LAYER_HEADER)
    {
        if (tempSegment == &ptrPacket->payload)
        {
            nextHeader = &ptrPacket->ipv6Header.NextHeader;
        }
        else
        {
            nextHeader = FC_SegHdrDataPtr(tempSegment);
        }
        switch (ptrSegment->segmentType)
        {
            case (uint8_t)TYPE_IPV6_EX_HEADER_HOP_BY_HOP_OPTIONS:
                *nextHeader = (uint8_t)IPV6_PROT_HOP_BY_HOP_OPTIONS_HEADER;
                break;
            case (uint8_t)TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_1:
            case (uint8_t)TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_2:
                *nextHeader = (uint8_t)IPV6_PROT_DESTINATION_OPTIONS_HEADER;
                break;
            case (uint8_t)TYPE_IPV6_EX_HEADER_ROUTING:
                *nextHeader = (uint8_t)IPV6_PROT_ROUTING_HEADER;
                break;
            case (uint8_t)TYPE_IPV6_EX_HEADER_FRAGMENT:
                *nextHeader = (uint8_t)IPV6_PROT_FRAGMENTATION_HEADER;
                break;
            case (uint8_t)TYPE_IPV6_EX_HEADER_AUTHENTICATION_HEADER:
                *nextHeader = (uint8_t)IPV6_PROT_AUTHENTICATION_HEADER;
                break;
            case (uint8_t)TYPE_IPV6_EX_HEADER_ENCAPSULATING_SECURITY_PAYLOAD:
                *nextHeader = (uint8_t)IPV6_PROT_ENCAPSULATING_SECURITY_PAYLOAD_HEADER;
                break;
            default:
                *nextHeader = ptrPacket->upperLayerHeaderType;
                break;
        }
    }

    if (ptrSegment->segmentType < (uint8_t)TYPE_IPV6_UPPER_LAYER_HEADER)
    {
        nextHeader = FC_SegHdrDataPtr(ptrSegment);
        if (ptrSegment->nextSegment == NULL)
        {
            *nextHeader = (uint8_t)IPV6_PROT_NONE;
        }
        else
        {
            switch (ptrSegment->nextSegment->segmentType)
            {
                case (uint8_t)TYPE_IPV6_EX_HEADER_HOP_BY_HOP_OPTIONS:
                    *nextHeader = (uint8_t)IPV6_PROT_HOP_BY_HOP_OPTIONS_HEADER;
                    break;
                case (uint8_t)TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_1:
                case (uint8_t)TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_2:
                    *nextHeader = (uint8_t)IPV6_PROT_DESTINATION_OPTIONS_HEADER;
                    break;
                case (uint8_t)TYPE_IPV6_EX_HEADER_ROUTING:
                    *nextHeader = (uint8_t)IPV6_PROT_ROUTING_HEADER;
                    break;
                case (uint8_t)TYPE_IPV6_EX_HEADER_FRAGMENT:
                    *nextHeader = (uint8_t)IPV6_PROT_FRAGMENTATION_HEADER;
                    break;
                case (uint8_t)TYPE_IPV6_EX_HEADER_AUTHENTICATION_HEADER:
                    *nextHeader = (uint8_t)IPV6_PROT_AUTHENTICATION_HEADER;
                    break;
                case (uint8_t)TYPE_IPV6_EX_HEADER_ENCAPSULATING_SECURITY_PAYLOAD:
                    *nextHeader = (uint8_t)IPV6_PROT_ENCAPSULATING_SECURITY_PAYLOAD_HEADER;
                    break;
                default:
                    *nextHeader = ptrPacket->upperLayerHeaderType;
                    break;
            }
        }
    }
}


static IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_DataSegmentGetByType (IPV6_PACKET * ptrPacket, uint8_t type)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;

    if (ptrPacket == NULL)
    {
        return NULL;
    }

    ptrSegment = &ptrPacket->payload;

    if (type != (uint8_t)TYPE_IPV6_BEGINNING_OF_WRITABLE_PART)
    {
        while ((ptrSegment != NULL) && (ptrSegment->segmentType < type))
        {
            ptrSegment = ptrSegment->nextSegment;
        }
    }
    else
    {
        IPV6_DATA_SEGMENT_HEADER * ptrTempSegment;

        type = (uint8_t)TYPE_IPV6_UPPER_LAYER_PAYLOAD;

        while ((ptrSegment != NULL) && (ptrSegment->segmentType < type))
        {
            ptrSegment = ptrSegment->nextSegment;
        }

        while (ptrSegment != NULL)
        {
            // Move ptrSegment to the next dynamically allocated segment
            while ((ptrSegment != NULL) && (ptrSegment->memory != IPV6_DATA_DYNAMIC_BUFFER))
            {
                ptrSegment = ptrSegment->nextSegment;
            }
            // Break out of the loop if the pointer gets to the end of the linked list
            if (ptrSegment == NULL)
            {
                break;
            }

            // Initialize ptrTempSegment to the first segment after the beginning of the dynamically allocated segments
            ptrTempSegment = ptrSegment->nextSegment;
            // Check to see if the dynamically allocated section is contiguous at the end of the linked list (or find the break)
            while ((ptrTempSegment != NULL) && (ptrTempSegment->memory == IPV6_DATA_DYNAMIC_BUFFER))
            {
                ptrTempSegment = ptrTempSegment->nextSegment;
            }

            if (ptrTempSegment != NULL)
            {
                // If there is a non-dynamic segment, continue in the loop until we find the final sublist
                // of dynamically allocated segments
                ptrSegment = ptrTempSegment;
            }
            else
            {
                // If we have reached the final sublist of dynamic segments, advance to the
                // section that is writable.
                ptrTempSegment = ptrSegment->nextSegment;
                while (ptrTempSegment != NULL)
                {
                    if (ptrTempSegment->segmentLen != 0U)
                    {
                        ptrSegment = ptrTempSegment;
                    }
                    ptrTempSegment = ptrTempSegment->nextSegment;
                }
                break;
            }
        }
    }

    if (ptrSegment == NULL)
    {
        return NULL;
    }
    else if (ptrSegment->segmentType == type)
    {
        return ptrSegment;
    }
    else
    {
        return NULL;
    }
}


static void * TCPIP_IPV6_DataSegmentContentsGetByType (IPV6_PACKET * ptrPacket, uint8_t type)
{
    IPV6_DATA_SEGMENT_HEADER * ptr;

    ptr = TCPIP_IPV6_DataSegmentGetByType(ptrPacket, type);

    if (ptr != NULL)
    {
        return (void *)ptr->dataLocation;
    }

    return NULL;
}


// ipv6.h
unsigned short TCPIP_IPV6_TxIsPutReady (IPV6_PACKET * ptrPacket, unsigned short count)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;
    unsigned short payloadSize = 0;
    unsigned short availableSpace;

    if (ptrPacket == NULL)
    {
        return 0U;
    }

    payloadSize = (count > (uint16_t)TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE) ? count : (uint16_t)TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE;

    ptrSegment = TCPIP_IPV6_DataSegmentGetByType (ptrPacket, (uint8_t)TYPE_IPV6_UPPER_LAYER_PAYLOAD);

    // Verify that there is a valid upper layer payload
    if (ptrSegment == NULL)
    {
        ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate(payloadSize);
        if (ptrSegment == NULL)
        {
            return 0;
        }

        TCPIP_IPV6_PacketSegmentInsert (ptrSegment, ptrPacket, (uint8_t)TYPE_IPV6_END_OF_LIST);

        return payloadSize;
    }

    ptrSegment = TCPIP_IPV6_DataSegmentGetByType (ptrPacket, (uint8_t)TYPE_IPV6_BEGINNING_OF_WRITABLE_PART);

    availableSpace = 0;

    if (ptrSegment != NULL)
    {
        while (ptrSegment != NULL)
        {
            availableSpace += ptrSegment->segmentSize - ptrSegment->segmentLen;
            ptrSegment = ptrSegment->nextSegment;
        }

        if (availableSpace >= count)
        {
            return availableSpace;
        }
    }

    // allocate new payload
    payloadSize -= availableSpace;
    if(payloadSize <  (uint16_t)TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE)
    {
        payloadSize =  (uint16_t)TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE;
    }

    ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate(payloadSize);
    if (ptrSegment == NULL)
    {
        return 0;
    }

    TCPIP_IPV6_PacketSegmentInsert (ptrSegment, ptrPacket, (uint8_t)TYPE_IPV6_END_OF_LIST);

    return availableSpace + payloadSize ;
}


// ipv6.h
bool TCPIP_IPV6_Put (IPV6_PACKET * pkt, unsigned char v)
{
    return (TCPIP_IPV6_PutArray (pkt, &v, 1) == 1U) ? true : false;
}


// ipv6.h
unsigned short TCPIP_IPV6_ArrayPutHelper (IPV6_PACKET * ptrPacket, const void * dataSource, uint8_t dataType, unsigned short len)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;
    uint8_t * destBuffer;
    const uint8_t* sourceBuffer;
    uint16_t putLen, avlblSize, allocSize, txSize;

    if (ptrPacket == NULL)
    {
        return 0U;
    }

    ptrSegment = TCPIP_IPV6_DataSegmentGetByType (ptrPacket, (uint8_t)TYPE_IPV6_BEGINNING_OF_WRITABLE_PART);

    if (ptrSegment == NULL)
    {
        return 0U;
    }


    sourceBuffer = (const uint8_t*)dataSource;
    putLen = 0U;
    while(len != 0U)
    {
        if(ptrSegment == NULL)
        { // allocate new segment
            allocSize = (len < (uint16_t)TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE) ? (uint16_t)TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE : len;
            ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate(allocSize);
            if (ptrSegment == NULL)
            {
                return putLen;
            }

            TCPIP_IPV6_PacketSegmentInsert (ptrSegment, ptrPacket, (uint8_t)TYPE_IPV6_END_OF_LIST);
        }

        avlblSize = ptrSegment->segmentSize - ptrSegment->segmentLen;
        if(avlblSize != 0U)
        {
            destBuffer = (uint8_t *)ptrSegment->dataLocation + ptrSegment->segmentLen;
            txSize = (len > avlblSize)? avlblSize : len;
            if (dataType == IPV6_DATA_PIC_RAM)
            {
                (void)memcpy (destBuffer, sourceBuffer, txSize);
            }
            else if (dataType == IPV6_DATA_NETWORK_FIFO)
            {
                (void)MACGetArray(FC_CvPtr2MacPkt(dataSource), destBuffer, txSize);
            }
            else
            {
                return 0;
            }

            ptrSegment->segmentLen += txSize;
            ptrPacket->payloadLen  += txSize;
            sourceBuffer += txSize;
            len -= txSize;
            putLen += txSize;
        }
        ptrSegment = ptrSegment->nextSegment;


    }

    return putLen;
}


// ipv6.h
unsigned short TCPIP_IPV6_PayloadSet (IPV6_PACKET * ptrPacket, uint8_t* payload, unsigned short len)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;

    ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate (0U);
    if(ptrSegment == NULL)
    {
        return 0;
    }

    ptrSegment->nextSegment = NULL;
    ptrSegment->dataLocation = payload;
    ptrSegment->segmentSize = len;
    ptrSegment->segmentLen = len;
    ptrSegment->memory = IPV6_DATA_PIC_RAM;

    TCPIP_IPV6_PacketSegmentInsert (ptrSegment, ptrPacket, (uint8_t)TYPE_IPV6_END_OF_LIST);

    ptrPacket->payloadLen += len;

    return len;
}


static unsigned short TCPIP_IPV6_PseudoHeaderChecksumGet (IPV6_PACKET * ptrPacket)
{
    if (ptrPacket->flags.addressType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        IPV6_PSEUDO_HEADER pseudoHeader;

        pseudoHeader.zero1 = 0U;
        pseudoHeader.zero2 = 0U;
        pseudoHeader.PacketLength = TCPIP_Helper_ntohs (ptrPacket->upperLayerHeaderLen + ptrPacket->payloadLen);
        pseudoHeader.NextHeader = ptrPacket->upperLayerHeaderType;
        (void)memcpy((void *)&pseudoHeader.SourceAddress, (void *)TCPIP_IPV6_SourceAddressGet (ptrPacket), sizeof (IPV6_ADDR));
        (void)memcpy((void *)&pseudoHeader.DestAddress, (void *)TCPIP_IPV6_DestAddressGet (ptrPacket), sizeof (IPV6_ADDR));
        return TCPIP_Helper_CalcIPChecksum ((void *)&pseudoHeader, (uint16_t)sizeof (pseudoHeader), 0U);
    }

    return 0U;
}


// ipv6_manager.h
void TCPIP_IPV6_HopLimitSet(IPV6_PACKET * ptrPacket, uint8_t hopLimit)
{
    void * ptrSegment = TCPIP_IPV6_DataSegmentContentsGetByType (ptrPacket, (uint8_t)TYPE_IPV6_HEADER);
    IPV6_HEADER* ptrHeader = FC_VPtr2IPv6Hdr(ptrSegment);
    ptrHeader->HopLimit = hopLimit;
}


// ipv6_manager.h
unsigned short TCPIP_IPV6_PayloadChecksumCalculate (IPV6_PACKET * ptrPacket)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;
    TCPIP_UINT32_VAL checksum;
    uint16_t tempChecksum = 0U;
    unsigned short checksumByteCount = 0U;

    ptrSegment = TCPIP_IPV6_DataSegmentGetByType (ptrPacket, (uint8_t)TYPE_IPV6_UPPER_LAYER_HEADER);

    checksum.Val = 0;
    if (ptrSegment != NULL)
    {
        checksum.w[0] = ~TCPIP_Helper_CalcIPChecksum((uint8_t *)ptrSegment->dataLocation, ptrPacket->upperLayerHeaderLen, 0U);
        checksumByteCount += ptrPacket->upperLayerHeaderLen;
    }

    ptrSegment = TCPIP_IPV6_DataSegmentGetByType (ptrPacket, (uint8_t)TYPE_IPV6_UPPER_LAYER_PAYLOAD);

    while (ptrSegment != NULL)
    {
        switch (ptrSegment->memory)
        {
            case IPV6_DATA_DYNAMIC_BUFFER:
            case IPV6_DATA_PIC_RAM:
                tempChecksum = ~TCPIP_Helper_CalcIPChecksum((uint8_t *)ptrSegment->dataLocation, ptrSegment->segmentLen, 0U);
                if ((checksumByteCount % 2U) != 0U)
                {
                    tempChecksum = TCPIP_Helper_htons(tempChecksum);
                }
                checksumByteCount += ptrSegment->segmentLen;
                break;
            case IPV6_DATA_NETWORK_FIFO:
            case IPV6_DATA_NONE:
                tempChecksum = 0;
                break;
            default:
                TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                tempChecksum = 0;
                break;
        }
        checksum.Val += (uint32_t)tempChecksum;
        ptrSegment = ptrSegment->nextSegment;
    }

    checksum.Val = (uint32_t)checksum.w[0] + (uint32_t)checksum.w[1];
    checksum.w[0] += checksum.w[1];
    return ~checksum.w[0];
}


// ipv6_manager.h
void TCPIP_IPV6_TransmitPacketStateReset (IPV6_PACKET * pkt)
{
    IPV6_DATA_SEGMENT_HEADER * segmentHeader = &pkt->payload;
    IPV6_DATA_SEGMENT_HEADER * segmentHeader2 = NULL;

    while ((segmentHeader != NULL) && (segmentHeader->segmentType != (uint8_t)TYPE_IPV6_UPPER_LAYER_PAYLOAD))
    {
        segmentHeader2 = segmentHeader;
        segmentHeader = segmentHeader->nextSegment;
    }

    if(segmentHeader2 != NULL)
    {
        segmentHeader2->nextSegment = NULL;
    }

    while (segmentHeader != NULL)
    {
        segmentHeader2 = segmentHeader->nextSegment;
        (void)TCPIP_HEAP_Free (ipv6MemH, segmentHeader);
        segmentHeader = segmentHeader2;
    }

    pkt->flags.queued = 0U;
    pkt->payloadLen = 0U;
    pkt->offsetInSegment = 0U;
}


// ipv6_manager.h
bool TCPIP_IPV6_AddressIsSolicitedNodeMulticast (const IPV6_ADDR * address)
{
    uint8_t solNodeMulticastFragment[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00};

    if ((address->v[0] == 0xFFU) && ((address->v[1] & 0x02U) == 0x02U) && (memcmp (solNodeMulticastFragment, &address->v[2], sizeof (IPV6_ADDR) - 5U) == 0))
    {
        return true;
    }
    return false;
}


// ipv6.h
int TCPIP_IPV6_Flush (IPV6_PACKET * ptrPacket)
{
    IPV6_HEAP_NDP_NC_ENTRY * neighborPointer = NULL;
    uint16_t *  checksumPointer;
    bool        toTxPkt = false;
    bool        mcastPkt = false;

    if(ptrPacket == NULL || ptrPacket->flags.addressType != (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        return -1;
    }

    // Write the Ethernet Header to the MAC TX Buffer
    IPV6_ADDR_STRUCT * sourceAddress;
    IPV6_ADDR * destinationAddress = TCPIP_IPV6_DestAddressGet(ptrPacket);
    void * ptrSegment = TCPIP_IPV6_DataSegmentContentsGetByType (ptrPacket, (uint8_t)TYPE_IPV6_HEADER);
    IPV6_HEADER* ptrIpHeader = FC_VPtr2IPv6Hdr(ptrSegment);

    if (ptrPacket->headerLen == 0u)
    {
        ptrIpHeader->NextHeader = ptrPacket->upperLayerHeaderType;
    }

    ptrIpHeader->PayloadLength = TCPIP_Helper_htons (ptrPacket->payloadLen + ptrPacket->upperLayerHeaderLen + ptrPacket->headerLen);

    if (ptrIpHeader->HopLimit == 0U)
    {
        ptrIpHeader->HopLimit = (ipv6Config + TCPIP_STACK_NetIxGet((const TCPIP_NET_IF*)ptrPacket->netIfH))->curHopLimit;
    }

    // Check to see if a source address was specified
    if (ptrPacket->flags.sourceSpecified != 0U)
    {
        if (ptrPacket->flags.useUnspecAddr != 0U)
        {
            TCPIP_IPV6_SourceAddressSet(ptrPacket, &IPV6_FIXED_ADDR_UNSPECIFIED);
            ptrPacket->flags.sourceSpecified = 1U;
        }
        else
        {
            sourceAddress = TCPIP_IPV6_DASSourceAddressSelect (ptrPacket->netIfH, destinationAddress, &ptrIpHeader->SourceAddress);
            if (sourceAddress == NULL)
            {
                ptrPacket->flags.sourceSpecified = 0U;
            }
        }
    }

    if (ptrPacket->flags.sourceSpecified == 0U)
    {
        sourceAddress = TCPIP_IPV6_DASSourceAddressSelect (ptrPacket->netIfH, destinationAddress, NULL);
        if (sourceAddress != NULL)
        {
            const IPV6_ADDR* pSrcAdd = &sourceAddress->address;
            TCPIP_IPV6_SourceAddressSet(ptrPacket, pSrcAdd);
        }
        else
        {
            // This should never happen; we should always have at least our link-local auto-configured address
            TCPIP_IPV6_SourceAddressSet(ptrPacket, &IPV6_FIXED_ADDR_UNSPECIFIED);
            return -1;
        }
    }

    if (ptrPacket->upperLayerChecksumOffset != IPV6_NO_UPPER_LAYER_CHECKSUM)
    {
        checksumPointer = (uint16_t*)TCPIP_IPV6_DataSegmentContentsGetByType (ptrPacket, (uint8_t)TYPE_IPV6_UPPER_LAYER_HEADER);
        if (checksumPointer != NULL)
        {
            if((((const TCPIP_NET_IF*)ptrPacket->netIfH)->txOffload & (uint8_t)TCPIP_MAC_CHECKSUM_IPV6) == 0U)
            {
                TCPIPStack_Assert((ptrPacket->upperLayerChecksumOffset & 0x0001U) == 0U, __FILE__, __func__, __LINE__);
                checksumPointer += ptrPacket->upperLayerChecksumOffset / 2U;
                *checksumPointer = ~TCPIP_IPV6_PseudoHeaderChecksumGet (ptrPacket);
                *checksumPointer = TCPIP_IPV6_PayloadChecksumCalculate (ptrPacket);
            }
            else
            {
                *checksumPointer = 0;
            }
        }
    }

    if (destinationAddress->v[0] == 0xFFU)
    {
        mcastPkt = true;
        // Determine the appropriate link-local address for a multicast address
        ptrPacket->remoteMACAddr.v[0] = 0x33U;
        ptrPacket->remoteMACAddr.v[1] = 0x33U;
        ptrPacket->remoteMACAddr.v[2] = destinationAddress->v[12];
        ptrPacket->remoteMACAddr.v[3] = destinationAddress->v[13];
        ptrPacket->remoteMACAddr.v[4] = destinationAddress->v[14];
        ptrPacket->remoteMACAddr.v[5] = destinationAddress->v[15];

        toTxPkt = true;
    }
    else
    {
        // Determine the appropriate neighbor to transmit the unicast packet to
#if defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)
        bool isG3Host = TCPIP_NDP_IsG3PLC_Neighbor((const TCPIP_NET_IF*)ptrPacket->netIfH, destinationAddress, &ptrPacket->remoteMACAddr);
        if(isG3Host)
        {
            TCPIPStack_Assert(ptrPacket->neighbor == NULL, __FILE__, __func__, __LINE__);
            // G3_PLC packet needs to be transmitted

            if(TCPIP_IPV6_PacketTransmit (ptrPacket))
            {   // success
                TCPIP_IPV6_PacketAck(ptrPacket, true);
                return 1;
            }

            // since there's no real neighbor node for this G3 host, we cannot queue it!
            return 0;
        }
        // for non G3_PLC packets, continue the regular processing
#endif  // defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)

        if ((neighborPointer = ptrPacket->neighbor) == NULL)
        {
            neighborPointer = TCPIP_NDP_NextHopGet ((const TCPIP_NET_IF*)ptrPacket->netIfH, destinationAddress);
        }

        if (neighborPointer == NULL)
        {
            // The device could not determine a next hop address
            return -1;
        }

        ptrPacket->neighbor = neighborPointer;

        switch (neighborPointer->reachabilityState)
        {
            case (uint8_t)NDP_STATE_STALE:
            case (uint8_t)NDP_STATE_REACHABLE:
            case (uint8_t)NDP_STATE_DELAY:
            case (uint8_t)NDP_STATE_PROBE:

                if( neighborPointer->reachabilityState == (uint8_t)NDP_STATE_STALE)
                {
                    TCPIP_NDP_ReachabilitySet ((const TCPIP_NET_IF*)ptrPacket->netIfH, neighborPointer, (uint8_t)NDP_STATE_DELAY);
                }
                (void)memcpy (&ptrPacket->remoteMACAddr, &neighborPointer->remoteMACAddr, sizeof (TCPIP_MAC_ADDR));
                toTxPkt = true;
                break;
            case (uint8_t)NDP_STATE_INCOMPLETE:
                TCPIP_NDP_AddressResolve (neighborPointer);
                F_TCPIP_IPV6_PacketEnqueue(ptrPacket, &neighborPointer->queuedPackets, (size_t)TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT, 0U, false);
                toTxPkt = false;
                break;
            default:
                TCPIP_NDP_ReachabilitySet ((const TCPIP_NET_IF*)ptrPacket->netIfH, neighborPointer, (uint8_t)NDP_STATE_INCOMPLETE);
                TCPIP_NDP_AddressResolve (neighborPointer);
                F_TCPIP_IPV6_PacketEnqueue(ptrPacket, &neighborPointer->queuedPackets, (size_t)TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT, 0U, false);
                toTxPkt = false;
                break;
        }
    }

    if(toTxPkt)
    {   // packet needs to be transmitted

        if(TCPIP_IPV6_PacketTransmit (ptrPacket))
        {   // success
            TCPIP_IPV6_PacketAck(ptrPacket, true);
            return 1;
        }
        else
        {
            if(mcastPkt)
            {
                F_TCPIP_IPV6_PacketEnqueue(ptrPacket, &mcastQueue.list, (size_t)TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT, (uint32_t)TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT, true);
            }
            else
            {
                F_TCPIP_IPV6_PacketEnqueue(ptrPacket, &neighborPointer->queuedPackets, (size_t)TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT, 0U, false);
            }
        }
    }

    return 0;
}

// ipv6_manager.h
bool TCPIP_IPV6_PacketTransmit (IPV6_PACKET * pkt)
{
    uint16_t mtu;
    IPV6_HEAP_NDP_DC_ENTRY * ptrDestination;
    uint16_t    ipv6PktLoad;

    if (pkt->flags.addressType != (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        return false;
    }

    if (!TCPIP_STACK_NetworkIsLinked((const TCPIP_NET_IF*)pkt->netIfH))
    {
        return false;
    }

    if (pkt->ipv6Header.DestAddress.v[0] == 0xFFU)
    {
        mtu = (ipv6Config + TCPIP_STACK_NetIxGet((const TCPIP_NET_IF*)pkt->netIfH))->multicastMTU;
    }
    else
    {
        ptrDestination = TCPIP_NDP_RemoteNodeFind ((const TCPIP_NET_IF*)pkt->netIfH, &pkt->ipv6Header.DestAddress, (uint8_t)IPV6_HEAP_NDP_DC_ID);
        if (ptrDestination != NULL)
        {
            mtu = ptrDestination->pathMTU;
        }
        else
        {
            mtu = (ipv6Config + TCPIP_STACK_NetIxGet((const TCPIP_NET_IF*)pkt->netIfH))->linkMTU;
        }
    }

    if (TCPIP_Helper_htons(pkt->ipv6Header.PayloadLength) + sizeof (IPV6_HEADER) + pkt->headerLen > mtu)
    {
        return TCPIP_IPV6_PacketTransmitInFragments(pkt, mtu);
    }

    ipv6PktLoad = TCPIP_IPV6_PacketPayload(pkt);
    TCPIP_MAC_PACKET* pMacPkt = TCPIP_IPV6_MacPacketTxAllocate(pkt, ipv6PktLoad, TCPIP_MAC_PKT_FLAG_NONE); 
    if (pMacPkt == NULL)
    {
        return false;
    }

    TCPIP_IPV6_MacPacketTxAddSegments(pkt, pMacPkt, (pkt->payloadLen != 0U) ? (uint16_t)TCPIP_MAC_SEG_FLAG_USER_PAYLOAD : 0U);

    // Write the Ethernet Header to the MAC packet
    TCPIP_IPV6_MacPacketTxPutHeader(pkt, pMacPkt, TCPIP_ETHER_TYPE_IPV6);

    // Transmit the packet
    pMacPkt->pktIf = pkt->netIfH;
    TCPIP_PKT_FlightLogTx(pMacPkt, TCPIP_THIS_MODULE_ID);
    if((int)TCPIPStackPacketTx((const TCPIP_NET_IF*)pkt->netIfH, pMacPkt) < 0)
    {   // failed
        TCPIP_PKT_FlightLogAcknowledge(pMacPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
        TCPIP_PKT_PacketFree(pMacPkt);
        return false;
    }

    pkt->flags.queued = 0U;

    return true;
}



static bool TCPIP_IPV6_PacketTransmitInFragments (IPV6_PACKET * pkt, uint16_t mtu)
{
    IPV6_FRAGMENT_HEADER * ptrFragmentHeader = NULL;
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;
    IPV6_DATA_SEGMENT_HEADER * ptrFragmentablePart;
    uint16_t currentPayloadLen;
    uint16_t sentPayloadLen = pkt->offsetInSegment;
    uint16_t totalPayloadLen;
    uint16_t unfragmentableLen = 0U;
    uint16_t temp;
    uint16_t offsetInSegment = 0U;

    if (!TCPIP_STACK_NetworkIsLinked((const TCPIP_NET_IF*)pkt->netIfH))
    {
        // Discard the packet if this interface isn't even linked
        return false;
    }

    ptrSegment = FC_VPtr2SegHdr(TCPIP_IPV6_DataSegmentContentsGetByType(pkt, (uint8_t)TYPE_IPV6_EX_HEADER_FRAGMENT));
    if (ptrSegment == NULL)
    {

        ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate ((uint16_t)sizeof (IPV6_FRAGMENT_HEADER));

        if (ptrSegment == NULL)
        {
            return false;
        }

        ptrSegment->segmentLen = (uint16_t)sizeof (IPV6_FRAGMENT_HEADER);

        TCPIP_IPV6_PacketSegmentInsert (ptrSegment, pkt, (uint8_t)TYPE_IPV6_EX_HEADER_FRAGMENT);

        ptrFragmentHeader = FC_SegHdr2FragHdr(ptrSegment);

        ptrFragmentHeader->reserved = 0;
        ptrFragmentHeader->offsetM.bits.reserved2 = 0;
        ptrFragmentHeader->offsetM.bits.m = 0;
        ptrFragmentHeader->offsetM.bits.fragmentOffset = 0;
        ptrFragmentHeader->identification = TCPIP_Helper_htonl(fragmentId);
        fragmentId++;
        pkt->headerLen += (uint16_t)sizeof (IPV6_FRAGMENT_HEADER);
    }

    totalPayloadLen = pkt->payloadLen + pkt->headerLen + pkt->upperLayerHeaderLen + (uint16_t)sizeof (IPV6_HEADER);

    // If a router specified that the path MTU is less than the minimum link MTU
    // we just need to add a fragmentation header to the packet
    if (mtu < (uint16_t)TCPIP_IPV6_MINIMUM_LINK_MTU)
    {
        if (totalPayloadLen < (uint16_t)TCPIP_IPV6_DEFAULT_LINK_MTU)
        {
            mtu = (uint16_t)TCPIP_IPV6_DEFAULT_LINK_MTU;
        }
    }

    ptrSegment = &pkt->payload;
    while ((ptrSegment != NULL) && (ptrSegment->segmentType <= (uint8_t)TYPE_IPV6_EX_HEADER_FRAGMENT))
    {
        unfragmentableLen += ptrSegment->segmentLen;
        ptrSegment = ptrSegment->nextSegment;
    }

    ptrFragmentablePart = ptrSegment;

    do
    {
        bool     payloadSeg = false;
        uint16_t lengthOfSegments = 0;
        currentPayloadLen = unfragmentableLen;
        ptrSegment = ptrFragmentablePart;

        // Determine the length of the current payload
        while (ptrSegment != NULL)
        {
            if (currentPayloadLen + (ptrSegment->segmentLen - offsetInSegment) <= mtu)
            {
                currentPayloadLen += (ptrSegment->segmentLen - offsetInSegment);
            }
            else
            {
                if (mtu - currentPayloadLen > 8U)
                {
                    currentPayloadLen = mtu - (mtu & 0x07U);
                }
                else
                {
                    if (currentPayloadLen % 8U != 0U)
                    {
                        if ((ptrSegment->segmentLen - offsetInSegment) > (8U - (currentPayloadLen & 0x07U)))
                        {
                            currentPayloadLen += (8U - (currentPayloadLen & 0x07U));
                        }
                        else
                        {
                            currentPayloadLen -= (currentPayloadLen & 0x07U);
                        }
                    }
                }
                break;
            }
            ptrSegment = ptrSegment->nextSegment;
        }

        if(ptrFragmentHeader == NULL)
        {
            return false;
        }

        // Set M flag
        if (sentPayloadLen + currentPayloadLen == totalPayloadLen)
        {
            ptrFragmentHeader->offsetM.bits.m = 0;
        }
        else
        {
            ptrFragmentHeader->offsetM.bits.m = 1;
        }

        // Set fragment offset
        ptrFragmentHeader->offsetM.bits.fragmentOffset = (uint16_t)(sentPayloadLen >> 3U);

        ptrFragmentHeader->offsetM.w = TCPIP_Helper_htons(ptrFragmentHeader->offsetM.w);

        // Calculate new payload length
        pkt->ipv6Header.PayloadLength = TCPIP_Helper_htons(currentPayloadLen - (uint16_t)sizeof (IPV6_HEADER));

        TCPIP_MAC_PACKET* pMacPkt = TCPIP_IPV6_MacPacketTxAllocate(pkt, currentPayloadLen, TCPIP_MAC_PKT_FLAG_NONE); 
        if (pMacPkt == NULL)
        {
            pkt->offsetInSegment = sentPayloadLen;
            return false;
        }

        // Write the Ethernet Header to the MAC packet
        TCPIP_IPV6_MacPacketTxPutHeader(pkt, pMacPkt, TCPIP_ETHER_TYPE_IPV6);

        // Initialize MAC TX Write Pointer
        uint8_t*    pMacData =  pMacPkt->pNetLayer;

        // Write the unfragmentable part
        ptrSegment = &pkt->payload;
        temp = unfragmentableLen;
        while (temp != 0U)
        {
            if (ptrSegment->segmentLen < temp)
            {
                (void)memcpy(pMacData, ptrSegment->dataLocation, ptrSegment->segmentLen);
                lengthOfSegments += ptrSegment->segmentLen;
                pMacData += ptrSegment->segmentLen;
                temp -= ptrSegment->segmentLen;
                ptrSegment = ptrSegment->nextSegment;
            }
            else
            {
                (void)memcpy(pMacData, ptrSegment->dataLocation, temp);
                lengthOfSegments += temp;

                pMacData += temp;
                break;
            }
        }

        // Write the fragmentable part
        ptrSegment = ptrFragmentablePart;
        temp = currentPayloadLen - unfragmentableLen;
        while (temp != 0U)
        {
            if(ptrSegment->segmentType == (uint8_t)TYPE_IPV6_UPPER_LAYER_PAYLOAD)
            {
                payloadSeg = true;
            }

            if (ptrSegment->segmentLen < temp)
            {
                (void)memcpy(pMacData, ptrSegment->dataLocation + offsetInSegment, ptrSegment->segmentLen);
                lengthOfSegments += ptrSegment->segmentLen;
                pMacData += ptrSegment->segmentLen;
                temp -= ptrSegment->segmentLen;
                ptrSegment = ptrSegment->nextSegment;
                offsetInSegment = 0U;
            }
            else
            {
                (void)memcpy(pMacData, ptrSegment->dataLocation + offsetInSegment, temp);
                lengthOfSegments += temp;

                pMacData += temp;
                if (temp == ptrSegment->segmentLen)
                {
                    ptrSegment = ptrSegment->nextSegment;
                    offsetInSegment = 0U;
                }
                else
                {
                    offsetInSegment = temp;
                }
                break;
            }
        }
        ptrFragmentablePart = ptrSegment;

        // Transmit the packet
        pMacPkt->pDSeg->segLen += lengthOfSegments;
        if(payloadSeg)
        {
            pMacPkt->pDSeg->segFlags |= (uint16_t)TCPIP_MAC_SEG_FLAG_USER_PAYLOAD;
        }
        pMacPkt->pktIf = pkt->netIfH;
        TCPIP_PKT_FlightLogTx(pMacPkt, TCPIP_THIS_MODULE_ID);
        if((int)TCPIPStackPacketTx((const TCPIP_NET_IF*)pkt->netIfH, pMacPkt) < 0)
        {   // failed, try later on
            TCPIP_PKT_FlightLogAcknowledge(pMacPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
            TCPIP_PKT_PacketFree(pMacPkt);
            pkt->offsetInSegment = sentPayloadLen;
            return false;
        }

        sentPayloadLen += currentPayloadLen - unfragmentableLen;
    } while (sentPayloadLen + unfragmentableLen != totalPayloadLen);

    pkt->flags.queued = 0U;

    return true;
}


static IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_DataSegmentHeaderAllocate (uint16_t len)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment = (IPV6_DATA_SEGMENT_HEADER *)TCPIP_HEAP_Malloc (ipv6MemH, sizeof (IPV6_DATA_SEGMENT_HEADER) + len);

    if (ptrSegment == NULL)
    {
        return NULL;
    }

    if (len != 0U)
    {
        ptrSegment->dataLocation = FC_SegHdrDataPtr(ptrSegment);
        ptrSegment->segmentSize = len;
        ptrSegment->segmentLen = 0U;
        ptrSegment->memory = IPV6_DATA_DYNAMIC_BUFFER;
    }
    else
    {
        ptrSegment->memory = IPV6_DATA_NONE;
        ptrSegment->dataLocation = (uint8_t*)NULL;
        ptrSegment->segmentSize = 0U;
        ptrSegment->segmentLen = 0U;
    }
    ptrSegment->nextSegment = NULL;

    return ptrSegment;
}


// ipv6.h
void TCPIP_IPV6_PacketFree (IPV6_PACKET * ptrPacket)
{
    if (ptrPacket == NULL)
    {
        return;
    }

    TCPIP_IPV6_PacketDataFree (ptrPacket);
    (void)TCPIP_HEAP_Free (ipv6MemH, ptrPacket);
}


static void TCPIP_IPV6_PacketDataFree (IPV6_PACKET * ptrPacket)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;
    IPV6_DATA_SEGMENT_HEADER * ptrSegment2;

    if (ptrPacket == NULL)
    {
        return;
    }

    // Set the initial segment to the segment after the IP header (which shouldn't be deallocated
    ptrSegment = ptrPacket->payload.nextSegment;

    while (ptrSegment != NULL)
    {
        ptrSegment2 = ptrSegment->nextSegment;
        (void)TCPIP_HEAP_Free (ipv6MemH, ptrSegment);
        ptrSegment = ptrSegment2;
    }
}

// ipv6_manager.h
bool TCPIP_IPV6_HeaderGet(TCPIP_MAC_PACKET* pRxPkt, IPV6_ADDR * localIPAddr, IPV6_ADDR * remoteIPAddr, uint8_t *protocol, uint16_t *len, uint8_t * hopLimit)
{
    IPV6_HEADER header;


    // Read IP header.
    (void)MACGetArray(pRxPkt, (uint8_t*)&header, (uint16_t)sizeof(header));

    // Make sure that this is an IPv6 packet.
    if ((header.V_T_F & 0x000000F0U) != (uint32_t)IPv6_SHIFT_VERSION)
    {
        return false;
    }

    uint16_t payloadLen = TCPIP_Helper_ntohs(header.PayloadLength);

    if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_RX_INTERNAL) != 0U)
    {   // skip the pkt length check for an internally generated packet! Already checked.
        // clear flag; only one internal generation
        pRxPkt->pktFlags &= ~(uint32_t)TCPIP_MAC_PKT_FLAG_RX_INTERNAL;
    }
    else
    {
        uint16_t pktLen = TCPIP_PKT_PayloadLen(pRxPkt);
        if(payloadLen > pktLen)
        {
            return false;
        }
    }

    *hopLimit = header.HopLimit;

    *len = payloadLen;

    *protocol = header.NextHeader;

    (void)memcpy(localIPAddr, &header.DestAddress, sizeof (IPV6_ADDR));

    (void)memcpy(remoteIPAddr, &header.SourceAddress, sizeof (IPV6_ADDR));

    return true;
}

bool TCPIP_IPV6_AddressesGet(const TCPIP_MAC_PACKET* pRxPkt, const IPV6_ADDR** pDestIPAddr, const IPV6_ADDR** pSourceIPAddr)
{
    const IPV6_HEADER *pHeader;
    const uint8_t     *pMacLayer, *pNetLayer;

    // restore the packet MAC and NET pointers
    pMacLayer = pRxPkt->pDSeg->segLoad;
    pNetLayer = pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);


    pHeader = FC_CuPtr2IPv6Hdr(pNetLayer);

    // Make sure that this is an IPv6 packet.
    if ((pHeader->V_T_F & 0x000000F0U) != (uint32_t)IPv6_SHIFT_VERSION)
    {
        return false;
    }


    if(pDestIPAddr != NULL)
    {
        *pDestIPAddr = &pHeader->DestAddress;
    }
    if(pSourceIPAddr != NULL)
    {
        *pSourceIPAddr = &pHeader->SourceAddress;
    }

    return true;
}


// ipv6_manager.h
void TCPIP_IPV6_RxBufferSet(TCPIP_MAC_PACKET* pRxPkt, uint16_t Offset)
{
    MACSetReadPtrInRx(pRxPkt, Offset + (uint16_t)sizeof (IPV6_HEADER));
}


static void TCPIP_IPV6_FreeConfigLists (IPV6_INTERFACE_CONFIG * pNetIf)
{
    TCPIP_IPV6_SingleListFree(&pNetIf->listDestinationCache);
    TCPIP_IPV6_SingleListFree(&pNetIf->listNeighborCache);
    TCPIP_IPV6_SingleListFree(&pNetIf->listDefaultRouter);
    TCPIP_IPV6_SingleListFree(&pNetIf->listPrefixList);
    TCPIP_IPV6_SingleListFree(&pNetIf->rxFragments);
    TCPIP_IPV6_DoubleListFree(&pNetIf->listIpv6UnicastAddresses);
    TCPIP_IPV6_DoubleListFree(&pNetIf->listIpv6MulticastAddresses);
    TCPIP_IPV6_DoubleListFree(&pNetIf->listIpv6TentativeAddresses);
}


// ipv6_manager.h
IPV6_ADDR_STRUCT * TCPIP_IPV6_AddressFind(const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * addr, IPV6_ADDR_TYPE listType)
{
    IPV6_ADDR_STRUCT * nextEntryPointer;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if (pNetIf == NULL)
    {
        return NULL;
    }
    else
    {
        pIpv6Config = ipv6Config + TCPIP_STACK_NetIxGet (pNetIf);
    }

    switch (listType)
    {
        case IPV6_ADDR_TYPE_UNICAST_TENTATIVE:
            nextEntryPointer = FC_DblNode2AddStruct(pIpv6Config->listIpv6TentativeAddresses.head);
            break;
        case IPV6_ADDR_TYPE_UNICAST:
            nextEntryPointer = FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head);
            break;
        case IPV6_ADDR_TYPE_MULTICAST:
            nextEntryPointer = FC_DblNode2AddStruct(pIpv6Config->listIpv6MulticastAddresses.head);
            break;

        default:
            nextEntryPointer = NULL;
            break;
    }

    while (nextEntryPointer != NULL)
    {
        const IPV6_ADDR* pNextAdd = &nextEntryPointer->address;
        if (memcmp (addr->v, pNextAdd->v, sizeof (IPV6_ADDR)) == 0)
        {
            return nextEntryPointer;
        }
        nextEntryPointer = nextEntryPointer->next;
    }
    return NULL;
}


static IPV6_ADDR_STRUCT * TCPIP_IPV6_SolicitedNodeMulticastAddressFind(const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * addr, uint8_t listType)
{
    IPV6_ADDR_STRUCT * nextEntryPointer;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if (pNetIf == NULL)
    {
        return NULL;
    }
    else
    {
        pIpv6Config = ipv6Config + TCPIP_STACK_NetIxGet (pNetIf);
    }

    switch (listType)
    {
        case (uint8_t)IPV6_ADDR_TYPE_UNICAST_TENTATIVE:
            nextEntryPointer = FC_DblNode2AddStruct(pIpv6Config->listIpv6TentativeAddresses.head);
            break;
        case (uint8_t)IPV6_ADDR_TYPE_UNICAST:
            nextEntryPointer = FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head);
            break;

        default:
            nextEntryPointer = NULL;
            break;
    }

    while (nextEntryPointer != NULL)
    {
        const IPV6_ADDR* pNextAdd = &nextEntryPointer->address;
        if (memcmp (&addr->v[13], &pNextAdd->v[13], 3) == 0)
        {
            return nextEntryPointer;
        }
        nextEntryPointer = nextEntryPointer->next;
    }
    return NULL;
}


// ipv6.h
IPV6_ADDR_STRUCT * TCPIP_IPV6_MulticastListenerAdd (TCPIP_NET_HANDLE hNet, const IPV6_ADDR * address)
{
    IPV6_ADDR_STRUCT * entryLocation;
    IPV6_ADDRESS_TYPE addressType;
    const TCPIP_NET_IF* pNetIf = (const TCPIP_NET_IF*)hNet;

    if (pNetIf == NULL)
    {
        return NULL;
    }

    if ((entryLocation = TCPIP_IPV6_AddressFind (pNetIf, address, IPV6_ADDR_TYPE_MULTICAST)) == NULL)
    {
        entryLocation = (IPV6_ADDR_STRUCT *) TCPIP_HEAP_Malloc (ipv6MemH, sizeof (IPV6_ADDR_STRUCT));

        if (entryLocation != NULL)
        {
            IPV6_ADDR* pEntryAdd = &entryLocation->address;
            (void)memcpy (pEntryAdd, address, sizeof (IPV6_ADDR));
            entryLocation->flags.type = (uint8_t)IPV6_ADDR_TYPE_MULTICAST;
            addressType.byte = TCPIP_IPV6_AddressTypeGet(pNetIf, address);
            entryLocation->flags.scope = addressType.bits.scope;
            TCPIP_NDP_LinkedListEntryInsert (pNetIf, entryLocation, (uint8_t)IPV6_HEAP_ADDR_MULTICAST_ID);
            TCPIP_IPV6_ClientsNotify(pNetIf, IPV6_EVENT_ADDRESS_ADDED, entryLocation);
        }
    }

    return entryLocation;
}


/*****************************************************************************
  Function:
    void F_TCPIP_IPV6_PacketEnqueue (IPV6_PACKET * pkt, PROTECTED_SINGLE_LIST* pList, size_t queueLimit, uint32_t tmoSeconds, bool isProtected)

  Summary:
    Queues a packet for future transmission.

  Description:
    Queues a packet for future transmission.

  Precondition:
    None

  Parameters:
    pkt         - The packet to queue.
    pList       - list to use for queuing
    queueLimit  - the max number of packets in the queue.
                  Packets will be removed when the number is reached.
    tmoSeconds  - timeout to set for the entry
    isProtected - access to a PROTECTED list

  Returns:
    None

  Remarks:
    For IPv6 queuing the tmo has to be 0!
    The queue is processed separately by the NDP
  ***************************************************************************/
static void F_TCPIP_IPV6_PacketEnqueue(IPV6_PACKET * pkt, SINGLE_LIST* pList, size_t queueLimit, uint32_t tmoSeconds, bool isProtected)
{
    IPV6_PACKET * ptrPacket;
    SINGLE_LIST*  sglList;
    PROTECTED_SINGLE_LIST* protList = NULL;

    if(isProtected)
    {
        protList = FC_SglList2ProtList(pList);
        sglList = &protList->list;
    }
    else
    {
        sglList = pList;
    }


    if(protList != NULL)
    {
        (void)TCPIP_Helper_ProtSglListLock(protList);
    }

    ptrPacket = FC_SglNode2Ip6Pkt(sglList->head);

    while ((ptrPacket != NULL) && (ptrPacket != pkt))
    {
        ptrPacket = ptrPacket->next;
    }

    if (ptrPacket == NULL)
    {
        if (sglList->nNodes == queueLimit)
        {
            ptrPacket = FC_SglNode2Ip6Pkt(TCPIP_Helper_SingleListHeadRemove(sglList));
            ptrPacket->flags.queued = 0U;
            TCPIP_IPV6_PacketAck(ptrPacket, false);
        }
        TCPIP_Helper_SingleListTailAdd (sglList, FC_Ip6Pkt2SglNode(pkt));
        pkt->flags.queued = 1U;
        uint32_t tickFreq = SYS_TMR_TickCounterFrequencyGet();
        pkt->queuedPacketTimeout = SYS_TMR_TickCountGet() + (tickFreq * tmoSeconds);

    }

    if(protList != NULL)
    {
        (void)TCPIP_Helper_ProtSglListUnlock(protList);
    }
}


// ipv6_manager.h
uint8_t TCPIP_IPV6_AddressTypeGet (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * address)
{
    uint8_t b;
    IPV6_ADDRESS_TYPE returnVal;

    if (address->v[0] == 0xFFU)
    {
        // First byte == 0xFF -> multicast
        returnVal.bits.type = (uint8_t)IPV6_ADDR_TYPE_MULTICAST;
        b = address->v[1];
        b &= 0x0FU;
        switch (b)
        {
            case 1:
            case 2:
            case 4:
            case 5:
            case 8:
            case 0x0EU:
                returnVal.bits.scope = b;
                break;
            default:
                returnVal.bits.scope = (uint8_t)IPV6_ADDR_SCOPE_UNKNOWN;
                break;
        }
    }
    else
    {
        // First byte != 0xFF -> unicast or anycast
        // Impossible to determine if it's an anycast addr unless
        // it's specifically identified as one somewhere
        returnVal.bits.type = (uint8_t)IPV6_ADDR_TYPE_UNICAST;

        if (((address->v[0] == 0xFEU) && (address->v[1] == 0x80U)) /*|| TCPIP_NDP_PrefixOnLinkStatus(pNetIf, address)*/)
        {
            returnVal.bits.scope = (uint8_t)IPV6_ADDR_SCOPE_LINK_LOCAL;
        }
        // Compare to loopback address
        else if ((address->d32[0] == 0x00000000U) && (address->d32[1] == 0x00000000U) && (address->d32[2] == 0x00000000U) && (address->v[12] == 0x00U) && \
                     (address->v[13] == 0x00U) && (address->v[14] == 0x00U) && (address->v[15] == 0x01U))
        {
            // This counts as link-local unicast
            returnVal.bits.scope = (uint8_t)IPV6_ADDR_SCOPE_INTERFACE_LOCAL;
        }
        else
        {
            returnVal.bits.scope = (uint8_t)IPV6_ADDR_SCOPE_GLOBAL;
        }
    }

    return returnVal.byte;
}

/*****************************************************************************
  Function:
    void F_TCPIP_IPV6_EUI64InterfaceIDSet (IPV6_ADDR *pAddress, TCPIP_MAC_ADDR * pNetMACAddr)

  Summary:
    Sets the 64-bit interface ID in an IPv6 address using the EUI-64 method.

  Description:
    Sets the 64-bit interface ID in an IPv6 address using the EUI-64 method.

  Precondition:
    None

  Parameters:
    pAddress - The IPv6 address where the interface ID is set.
    pNetMACAddr - The interface MAC address

  Returns:
    None

  Remarks:
    Implements EUI-64 method defined in RFC 4291 Appendix A

  ***************************************************************************/
static void F_TCPIP_IPV6_EUI64InterfaceIDSet (IPV6_ADDR *pAddress, const TCPIP_MAC_ADDR * pNetMACAddr)
{
    uint8_t *pInterfaceID = pAddress->v + IPV6_INTERFACE_ID_OFFSET;

    *pInterfaceID++ = pNetMACAddr->v[0] | 0x02U;
    *pInterfaceID++ = pNetMACAddr->v[1];
    *pInterfaceID++ = pNetMACAddr->v[2];
    *pInterfaceID++ = 0xFFU;
    *pInterfaceID++ = 0xFEU;
    *pInterfaceID++ = pNetMACAddr->v[3];
    *pInterfaceID++ = pNetMACAddr->v[4];
    *pInterfaceID++ = pNetMACAddr->v[5];
}

// ipv6_private.h
// prefixLen should normally be 64.
// However, we allow for any length.
// BUT the size of the generated RID is always 64 bits.
bool TCPIP_IPV6_AddressInterfaceIDSet (const TCPIP_NET_IF* pNetIf, IPV6_ADDR* pAddress, uint8_t prefixLen, uint8_t dadCounter)
{
    if ((pNetIf == NULL) || (pAddress == NULL))
    {
        return false;
    }

#if (TCPIP_IPV6_RIID_ENABLED != 0)
    if ((ipv6_config_dcpt.configFlags & TCPIP_IPV6_FLAG_RANDOM_INTERFACE_ID) != 0)
    {
        bool prfRes;
        if(ipv6_config_dcpt.riidPrF != NULL)
        {   // call the user supplied PRF
            prfRes = ipv6_config_dcpt.riidPrF(pNetIf, pAddress, prefixLen, dadCounter);
        }
        else
        { // call the default SHA-1 PRF function
            prfRes = F_TCPIP_IPV6_RandomInterfaceIDSet(pNetIf, pAddress, prefixLen, dadCounter);
        }

        return prfRes;
    }
    else
#endif
    {
        F_TCPIP_IPV6_EUI64InterfaceIDSet(pAddress, &pNetIf->netMACAddr);
    }

    return true;
}

#if (TCPIP_IPV6_RIID_ENABLED != 0)
/*****************************************************************************
  Function:
    static bool F_TCPIP_IPV6_RandomInterfaceIDSet(const TCPIP_NET_IF * pNetIf, IPV6_ADDR* ip6Addr, uint8_t prefixLen, uint8_t dadCounter)

  Summary:
    Sets a Random Interface ID in an IPv6 address.

  Description:
    Sets a 64 bit Random Interface ID in an IPv6 address as described in RFC7217 Section 5.
    It's using a default PRF SHA-1 function.

  Precondition:
    The network prefix is set in the input IPv6 address

  Parameters:
    pNetIf - the network interface for which this PRF is calculated
    ip6Addr - the address to update - it should contain the right network prefix
    prefixLen - the length of the prefix, as described in RFC7217 Section 5.
    dadCounter - the DAD counter, as described in RFC7217 Section 5. (0: IDGEN_RETRIES - 1)


  Returns:
    true  - On success
    false - Failure to indicate an error occurred 

  Remarks:
     None

  ***************************************************************************/
// Calculate SHA-1 of: (prefix, Net_Iface, Network_ID, DAD_Counter, secret_key);
static bool F_TCPIP_IPV6_RandomInterfaceIDSet(const TCPIP_NET_IF* pNetIf, IPV6_ADDR* ip6Addr, uint8_t prefixLen, uint8_t dadCounter)
{

    CRYPT_SHA_CTX  shaSum;
    uint8_t shaDigest[20];  // SHA1 size is 160 bits

    // calculate the SHA1
    (void)CRYPT_SHA_Initialize(&shaSum);

    // add prefix
    (void)CRYPT_SHA_DataAdd(&shaSum, ip6Addr->v, ((uint32_t)prefixLen + 7UL) / 8U);  // round up the prefixLen to bytes

    TCPIP_IPV6_CONFIG_DCPT*  pIpv6Dcpt = &ipv6_config_dcpt; 

    // add Net_Iface
    if(pIpv6Dcpt->riidNetIfaceF != NULL)
    {
        const uint8_t* pNetIface = NULL;
        size_t netIfaceSize = pIpv6Dcpt->riidNetIfaceF(pNetIf, &pNetIface);
        if(pNetIface != NULL && netIfaceSize != 0U)
        {   // add the Net_Iface parameter
            (void)CRYPT_SHA_DataAdd(&shaSum, pNetIface, netIfaceSize);
        }
    }
    else
    {   // use the default MAC address
        (void)CRYPT_SHA_DataAdd(&shaSum, pNetIf->netMACAddr.v, sizeof(pNetIf->netMACAddr));
    }

    // add the Network_ID
    if(pIpv6Dcpt->riidNetIdF != NULL)
    {
        const uint8_t* pNetworkID = NULL;
        size_t netIdSize = pIpv6Dcpt->riidNetIdF(pNetIf, &pNetworkID);
        if(pNetworkID != NULL && netIdSize != 0U)
        {   // add the Net_Iface parameter
            (void)CRYPT_SHA_DataAdd(&shaSum, pNetworkID, netIdSize);
        }
    }
    // else skip the optional Network_ID 

    // add the DAD_Counter 
    (void)CRYPT_SHA_DataAdd(&shaSum, &dadCounter, sizeof(dadCounter));

    // add the secret_key; riidKeyF cannot be NULL
    const uint8_t* pSecretKey = NULL;
    size_t keySize = pIpv6Dcpt->riidKeyF(pNetIf, &pSecretKey);
    if(pSecretKey == NULL || keySize < IPV6_RIID_SECRET_KEY_MIN_SIZE)
    {   // failed
        return false;
    }

    // add the secret key
    (void)CRYPT_SHA_DataAdd(&shaSum, pSecretKey, keySize);

    // done
    (void)CRYPT_SHA_Finalize(&shaSum, shaDigest);

    // success; InterfaceID = 64 LSbs of RID
    uint8_t* pRid = shaDigest + sizeof(shaDigest) - IPV6_INTERFACE_ID_SIZE / 8U;
    (void)memcpy(ip6Addr->v + IPV6_INTERFACE_ID_OFFSET, pRid, IPV6_INTERFACE_ID_SIZE / 8U);
    return true;
}
#endif

// ipv6.h
void TCPIP_IPV6_AddressUnicastRemove(TCPIP_NET_HANDLE netH, const IPV6_ADDR * address)
{
    IPV6_ADDR_STRUCT * entryLocation;
    const TCPIP_NET_IF * pNetIf =  TCPIPStackHandleToNet(netH);

    if(pNetIf != NULL)
    {

        entryLocation = TCPIP_IPV6_AddressFind (pNetIf, address, IPV6_ADDR_TYPE_UNICAST);
        if (entryLocation != NULL)
        {
            TCPIP_IPV6_ClientsNotify(pNetIf, IPV6_EVENT_ADDRESS_REMOVED, entryLocation);
            (void)TCPIP_NDP_LinkedListEntryRemove (pNetIf, entryLocation, (uint8_t)IPV6_HEAP_ADDR_UNICAST_ID);
        }
    }
}


// ipv6.h
void TCPIP_IPV6_MulticastListenerRemove (TCPIP_NET_HANDLE netH, const IPV6_ADDR * address)
{
    IPV6_ADDR_STRUCT * entryLocation;
    TCPIP_NET_IF * pNetIf =  TCPIPStackHandleToNetUp(netH);

    if(pNetIf != NULL)
    {
        entryLocation = TCPIP_IPV6_AddressFind (pNetIf, address, IPV6_ADDR_TYPE_MULTICAST);
        if (entryLocation != NULL)
        {
            TCPIP_IPV6_ClientsNotify(pNetIf, IPV6_EVENT_ADDRESS_REMOVED, entryLocation);
            (void)TCPIP_NDP_LinkedListEntryRemove (pNetIf, entryLocation, (uint8_t)IPV6_HEAP_ADDR_MULTICAST_ID);
        }
    }

}


// ipv6.h
IPV6_ADDR_STRUCT * TCPIP_IPV6_UnicastAddressAdd (TCPIP_NET_HANDLE netH, const IPV6_ADDR * address, uint8_t prefixLen, uint8_t skipProcessing)
{
    uint8_t label, precedence, dasPrefixLen;
    IPV6_ADDRESS_TYPE i;
    TCPIP_NET_IF * pNetIf =  TCPIPStackHandleToNetUp(netH);

    if(pNetIf == NULL)
    {
        return NULL;
    }

    IPV6_ADDR_STRUCT * entryLocation = TCPIP_IPV6_AddressFind (pNetIf, address, IPV6_ADDR_TYPE_UNICAST);
    if(entryLocation == NULL)
    {
        entryLocation = TCPIP_IPV6_AddressFind (pNetIf, address, IPV6_ADDR_TYPE_UNICAST_TENTATIVE);
        if(entryLocation == NULL)
        {
            entryLocation = (IPV6_ADDR_STRUCT *) TCPIP_HEAP_Malloc (ipv6MemH, sizeof (IPV6_ADDR_STRUCT));

            if (entryLocation != NULL)
            {
                IPV6_ADDR* pEntryAdd = &entryLocation->address;
                (void)memcpy (pEntryAdd, address, sizeof (IPV6_ADDR));
                i.byte = TCPIP_IPV6_AddressTypeGet (pNetIf, address);
                entryLocation->flags.type = i.bits.type;
                entryLocation->flags.scope = i.bits.scope;
                if (TCPIP_IPV6_DASPolicyGet(address, &label, &precedence, &dasPrefixLen))
                {
                    entryLocation->flags.precedence = precedence;
                    entryLocation->flags.label = (uint8_t)(label & 0x0FU);
                }
                else
                {
                    entryLocation->flags.precedence = 0x00U;
                    entryLocation->flags.label = 0x0FU;
                }
                entryLocation->flags.temporary = 0U;

                // Set the Stateless Address Autoconfiguration variables to default values.
                // The Stateless Address AutoConfiguration function will set it to something else
                // if necessary.
                entryLocation->validLifetime = 0xFFFFFFFFU;
                entryLocation->preferredLifetime = 0xFFFFFFFFU;
                entryLocation->prefixLen = (prefixLen == 0U) ? 64U : prefixLen;
                // The skipProcessing flag indicates that the address doesn't need duplicate address
                // detection or an associated solicited node multicast address.
                // This can be used to add loopback addresses, for example.
                if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_NO_DAD) == 0U && skipProcessing == 0U)
                {
                    TCPIP_NDP_LinkedListEntryInsert (pNetIf, entryLocation, (uint8_t)IPV6_HEAP_ADDR_UNICAST_TENTATIVE_ID);
                    if( TCPIP_NDP_DupAddrDiscoveryDetect( pNetIf, entryLocation) == false )
                    {
                        (void)TCPIP_NDP_LinkedListEntryRemove (pNetIf, entryLocation, (uint8_t)IPV6_HEAP_ADDR_UNICAST_TENTATIVE_ID);
                        entryLocation = NULL;
                    }
                }
                else
                {
                    TCPIP_NDP_LinkedListEntryInsert (pNetIf, entryLocation, (uint8_t)IPV6_HEAP_ADDR_UNICAST_ID);
                    TCPIP_IPV6_ClientsNotify(pNetIf, IPV6_EVENT_ADDRESS_ADDED, entryLocation);
                }
            }
        }
    }

    return entryLocation;
}


static IPV6_ACTION TCPIP_IPV6_HopByHopOptionsHeaderProcess(
    const TCPIP_NET_IF *      pNetIf,
    TCPIP_MAC_PACKET *  pRxPkt,
    uint8_t *           nextHeader,
    uint16_t *          length
    )
{
    uint8_t headerLength;
    uint8_t optionLength;
    uint8_t data[8];
    uint8_t i;
    uint8_t j;
    IPV6_TLV_OPTION_TYPE anIpV6TlvOption;

    (void)TCPIP_IPV6_GetOptionHeader( pRxPkt, data, 1U );

    *nextHeader = data[0];
    headerLength = data[1] + 1U;
    *length = (uint16_t)headerLength << 3U;

    anIpV6TlvOption.b = data[2];
    i = 3;

    do {
        IPV6_ACTION retAction = IPV6_ACTION_NONE; 
        switch( anIpV6TlvOption.b )
        {
            case IPV6_TLV_PAD_1:
                // If this option is present, let the post-switch code load new
                // data if necessary and get the next option
                break;
            case IPV6_TLV_PAD_N:
                optionLength = data[i++];
                if (optionLength <= (8U-i))
                {
                    i+= optionLength;
                }
                else
                {
                    optionLength -= (8U-i);
                    j = optionLength >> 3U;
                    headerLength -= j;
                    while (j-- != 0U)
                    {
                        (void)TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1U);
                    }
                    optionLength -= (j << 3U);
                    i = optionLength;
                }
                break;
            case IPV6_TLV_HBHO_PAYLOAD_JUMBOGRAM:
                break;
            case IPV6_TLV_HBHO_ROUTER_ALERT:
                break;
            default:
                switch( anIpV6TlvOption.bits.unrecognizedAction )
                {
                    case IPV6_TLV_UNREC_OPT_SKIP_OPTION:
                        // Ignore this option (treat it like a padding option)
                        optionLength = data[i++];
                        if (optionLength <= (8U-i))
                        {
                            i+= optionLength;
                        }
                        else
                        {
                            optionLength -= (8U-i);
                            j = optionLength >> 3U;
                            headerLength -= j;
                            while (j-- != 0U)
                            {
                                (void)TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1U);
                            }
                            optionLength -= (j << 3U);
                            i = optionLength;
                        }
                        break;
                    case IPV6_TLV_UNREC_OPT_DISCARD_SILENT:
                        *length = *length - ((uint16_t)headerLength << 3U) + ((uint16_t)i - 1U);
                        retAction = IPV6_ACTION_DISCARD_SILENT;
                        break;
                        
                    case IPV6_TLV_UNREC_OPT_DISCARD_PP:
                        *length = *length - ((uint16_t)headerLength << 3U) + ((uint16_t)i - 1U);
                        retAction = IPV6_ACTION_DISCARD_PP_2;
                        break;
                       
                    case IPV6_TLV_UNREC_OPT_DISCARD_PP_NOT_MC:
                        *length = *length - ((uint16_t)headerLength << 3U) + ((uint16_t)i - 1U);
                        retAction = IPV6_ACTION_DISCARD_PP_2_NOT_MC;
                        break;

                    default:
                        // should not happen
                        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                        retAction = IPV6_ACTION_DISCARD_SILENT;
                        break;
                }
                break;
        }

        if(retAction != IPV6_ACTION_NONE)
        {   // some discard needed
            return retAction;
        }

        if( i == 8U )
        {
            headerLength--;
            i = 0;
            if( headerLength != 0U )
            {
                (void)TCPIP_IPV6_GetOptionHeader( pRxPkt, data, 1U );
            }
        }
        anIpV6TlvOption.b = data[ i++ ];
    } while( headerLength  != 0U);

    return IPV6_ACTION_NONE;
}


static IPV6_ACTION TCPIP_IPV6_DestinationOptionsHeaderProcess(
    const TCPIP_NET_IF *      pNetIf,
    TCPIP_MAC_PACKET *  pRxPkt,
    uint8_t *           nextHeader,
    uint16_t *          length
    )
{
    uint8_t headerLength;
    uint8_t optionLength;
    uint8_t data[8];
    uint8_t i;
    uint8_t j;
    IPV6_TLV_OPTION_TYPE anIpV6TlvOption;

    (void)TCPIP_IPV6_GetOptionHeader( pRxPkt, data, 1U );

    *nextHeader = data[0];
    headerLength = data[1] + 1U;
    *length = (uint16_t)headerLength << 3U;

    anIpV6TlvOption.b = data[ 2 ];
    i = 3U;

    do {
        IPV6_ACTION retAction = IPV6_ACTION_NONE; 
        switch( anIpV6TlvOption.bits.option )
        {
            // There are only two options current defined, and they're padding options
            case IPV6_TLV_PAD_1:
                // If this option is present, let the post-switch code load new
                // data if necessary and get the next option
                break;
            case IPV6_TLV_PAD_N:
                optionLength = data[i++];
                if (optionLength <= (8U-i))
                {
                    i+= optionLength;
                }
                else
                {
                    optionLength -= (8U-i);
                    j = optionLength >> 3U;
                    headerLength -= (j + 1U);
                    while (j-- != 0U)
                    {
                        (void)TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1U);
                    }
                    optionLength -= (j << 3U);
                    i = optionLength;
                }
                break;
            default:
                switch( anIpV6TlvOption.bits.unrecognizedAction )
                {
                    case IPV6_TLV_UNREC_OPT_SKIP_OPTION:
                        // Ignore this option (treat it like a padding option)
                        optionLength = data[i++];
                        if (optionLength <= (8U-i))
                        {
                            i+= optionLength;
                        }
                        else
                        {
                            optionLength -= (8U-i);
                            j = optionLength >> 3U;
                            headerLength -= (j + 1U);
                            while (j-- != 0U)
                            {
                                (void)TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1U);
                            }
                            optionLength -= (j << 3U);
                            i = optionLength;
                        }
                        break;
                    case IPV6_TLV_UNREC_OPT_DISCARD_SILENT:
                        // Return the offset of the error in this header in the headerLength parameter
                        *length = *length - ((uint16_t)headerLength << 3U) + ((uint16_t)i - 1U);
                        retAction = IPV6_ACTION_DISCARD_SILENT;
                        break;
                        
                    case IPV6_TLV_UNREC_OPT_DISCARD_PP:
                        // Return the offset of the error in this header in the headerLength parameter
                        *length = *length - ((uint16_t)headerLength << 3U) + ((uint16_t)i - 1U);
                        retAction = IPV6_ACTION_DISCARD_PP_2;
                        break;
                       
                    case IPV6_TLV_UNREC_OPT_DISCARD_PP_NOT_MC:
                        // Return the offset of the error in this header in the headerLength parameter
                        *length = *length - ((uint16_t)headerLength << 3) + ((uint16_t)i - 1U);
                        retAction = IPV6_ACTION_DISCARD_PP_2_NOT_MC;
                        break;

                    default:
                        // should not happen
                        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                        retAction = IPV6_ACTION_DISCARD_SILENT;
                        break;
                }
                break;
        }

        if(retAction != IPV6_ACTION_NONE)
        {   // some discard needed
            return retAction;
        }

        if( i == 8U )
        {
            headerLength--;
            i = 0U;
            if( headerLength != 0U )
            {
                (void)TCPIP_IPV6_GetOptionHeader( pRxPkt, data, 1U );
            }
        }
        anIpV6TlvOption.b = data[ i++ ];
    } while( headerLength != 0U);

    return IPV6_ACTION_NONE;
}


static void TCPIP_IPV6_FragmentBufferFree (IPV6_RX_FRAGMENT_BUFFER* pFragBuffer)
{
    if (pFragBuffer != NULL)
    {
        if(pFragBuffer->ptrPacket != NULL)
        {
            TCPIP_MAC_PACKET* pFragMacPkt = FC_CvPtr2MacPkt(pFragBuffer->ptrPacket);
            TCPIP_PKT_PacketAcknowledge (pFragMacPkt, TCPIP_MAC_PKT_ACK_RX_OK);
        }

        (void)TCPIP_HEAP_Free (ipv6MemH, pFragBuffer);
    }
}

static void TCPIP_IPV6_FragPktAck(TCPIP_MAC_PACKET* pkt,  const void* param)
{
    // just free the packet
    TCPIP_PKT_PacketFree(pkt);
}

static IPV6_ACTION TCPIP_IPV6_FragmentationHeaderProcess(
    const TCPIP_NET_IF *pNetIf,
    const IPV6_ADDR *   remoteIP,
    const IPV6_ADDR *   localIP,
    uint8_t *           nextHeader,
    uint16_t            dataCount,
    uint16_t            headerLen,
    TCPIP_MAC_PACKET *  pRxPkt,
    uint16_t            previousHeaderLen
    )
{
    IPV6_RX_FRAGMENT_BUFFER *   ptrFragment;
    IPV6_FRAGMENT_HEADER        fragmentHeader;
    IPV6_INTERFACE_CONFIG *     pIpv6Config;
    TCPIP_MAC_PACKET *          pFragMacPkt;
    uint8_t *                   pFragPktData;

    if ((pNetIf == NULL) || (dataCount < sizeof (IPV6_FRAGMENT_HEADER)))
    {
        return IPV6_ACTION_DISCARD_SILENT;
    }

    pIpv6Config = ipv6Config + TCPIP_STACK_NetIxGet (pNetIf);

    (void)TCPIP_IPV6_ArrayGet(pRxPkt, (void *)&fragmentHeader, 8);
    dataCount -= (uint16_t)sizeof (IPV6_FRAGMENT_HEADER);

    fragmentHeader.offsetM.w = TCPIP_Helper_ntohs (fragmentHeader.offsetM.w);

    // Set fragment buffer pointer to the head of the linked list of fragmented packets
    ptrFragment = FC_SglNode2FragBuff(pIpv6Config->rxFragments.head);
    
    // Find a packet being reassembled that matches this fragmented packet
    while (ptrFragment != NULL)
    {
        if (ptrFragment->identification == fragmentHeader.identification)
        {
            pFragMacPkt = FC_CvPtr2MacPkt(ptrFragment->ptrPacket); 
            const IPV6_HEADER* pFragHdr = FC_CuPtr2IPv6Hdr(pFragMacPkt->pNetLayer);
            if ((memcmp (pFragHdr->SourceAddress.v, remoteIP->v, sizeof (IPV6_ADDR)) == 0) &&
                (memcmp (pFragHdr->DestAddress.v, localIP->v, sizeof (IPV6_ADDR))== 0 ))
            {
                break;
            }
        }
        ptrFragment = ptrFragment->next;
    }

    // If no existing fragment was found, this is the first fragment in the packet.
    // Create a fragment buffer for it and store the unfragmentable part.
    if (ptrFragment == NULL)
    {
        ptrFragment = (IPV6_RX_FRAGMENT_BUFFER *)TCPIP_HEAP_Malloc (ipv6MemH, sizeof (IPV6_RX_FRAGMENT_BUFFER));

        if (ptrFragment == NULL)
        {
            return IPV6_ACTION_DISCARD_SILENT;
        }
        // Allocate  a full MAC_PACKET for the fragmented packet; data size given by TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE
        
        pFragMacPkt = TCPIP_PKT_PacketAlloc(0, (uint16_t)ipv6_config_dcpt.rxfragmentBufSize, TCPIP_MAC_PKT_FLAG_NONE);
        ptrFragment->ptrPacket = FC_MacPkt2U8Ptr(pFragMacPkt);
        if(ptrFragment->ptrPacket == NULL)
        {   // failed to allocate a fragmentation TCPIP_MAC_PACKET
            TCPIP_IPV6_FragmentBufferFree (ptrFragment);
            return IPV6_ACTION_DISCARD_SILENT;
        }

        pFragMacPkt->pktIf = pNetIf; 
        TCPIP_PKT_PacketAcknowledgeSet(pFragMacPkt, &TCPIP_IPV6_FragPktAck, NULL);
        ptrFragment->next = NULL;
        // The RFC specifies that the fragments must be reassembled in one minute or less
        TCPIPStack_Assert(ipv6_config_dcpt.fragmentPktRxTimeout < 256U, __FILE__, __func__, __LINE__);
        ptrFragment->secondsRemaining = (uint8_t)ipv6_config_dcpt.fragmentPktRxTimeout;
        ptrFragment->identification = fragmentHeader.identification;
        ptrFragment->packetSize = headerLen;
        ptrFragment->bytesInPacket = headerLen;
        ptrFragment->firstFragmentLength = 0;

        // Reset the packet's read pointer
        MACSetReadPtrInRx(pRxPkt, 0);

        // Copy the unfragmentable part of the packet data into the fragment buffer
        pFragPktData = pFragMacPkt->pNetLayer;
        (void)MACGetArray(pRxPkt, pFragPktData, headerLen);

        // Set the packet's read pointer to skip the fragment header
        MACSetReadPtrInRx(pRxPkt, headerLen + (uint16_t)sizeof (IPV6_FRAGMENT_HEADER));

        if (headerLen == (uint16_t)sizeof (IPV6_HEADER))
        {
            pFragPktData[IPV6_HEADER_OFFSET_NEXT_HEADER] = fragmentHeader.nextHeader;
        }
        else
        {
            pFragPktData[headerLen - previousHeaderLen] = fragmentHeader.nextHeader;
        }

        TCPIP_Helper_SingleListTailAdd(&pIpv6Config->rxFragments, FC_FragBuff2SglNode(ptrFragment));
        
    }
    
    pFragMacPkt = FC_CvPtr2MacPkt(ptrFragment->ptrPacket);
    pFragPktData = pFragMacPkt->pNetLayer;

    if (dataCount != 0U)
    {
        if (fragmentHeader.offsetM.bits.fragmentOffset == 0U)
        {
            ptrFragment->firstFragmentLength = dataCount + headerLen;
        }

        if ((headerLen + ((uint16_t)fragmentHeader.offsetM.bits.fragmentOffset << 3U) + dataCount) > ipv6_config_dcpt.rxfragmentBufSize)
        {
            TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, localIP, remoteIP, (uint8_t)ICMPV6_ERR_PP_ERRONEOUS_HEADER, (uint8_t)ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl((uint32_t)headerLen + 2U), dataCount + headerLen + (uint16_t)sizeof (IPV6_FRAGMENT_HEADER));
            (void)TCPIP_Helper_SingleListNodeRemove(&pIpv6Config->rxFragments, FC_FragBuff2SglNode(ptrFragment));
            TCPIP_IPV6_FragmentBufferFree (ptrFragment);
            return IPV6_ACTION_DISCARD_SILENT;
        }

        if (fragmentHeader.offsetM.bits.m != 0U)
        {
            // More fragments
            // Check to ensure the packet's payload length is a multiple of eight bytes.
            if (((headerLen + dataCount) % 8U) != 0U)
            {            
                TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, localIP, remoteIP, (uint8_t)ICMPV6_ERR_PP_ERRONEOUS_HEADER, (uint8_t)ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl(IPV6_HEADER_OFFSET_PAYLOAD_LENGTH), dataCount + headerLen + (uint16_t)sizeof (IPV6_FRAGMENT_HEADER));
                (void)TCPIP_Helper_SingleListNodeRemove(&pIpv6Config->rxFragments, FC_FragBuff2SglNode(ptrFragment));
                TCPIP_IPV6_FragmentBufferFree (ptrFragment);
                return IPV6_ACTION_DISCARD_SILENT;
            }
            (void)MACGetArray(pRxPkt, pFragPktData + headerLen + (fragmentHeader.offsetM.bits.fragmentOffset << 3), dataCount);
            ptrFragment->bytesInPacket += dataCount;
        }
        else
        {
            // No more fragments
            (void)MACGetArray(pRxPkt, pFragPktData + headerLen + (fragmentHeader.offsetM.bits.fragmentOffset << 3), dataCount);
            ptrFragment->bytesInPacket += dataCount;
            ptrFragment->packetSize += (fragmentHeader.offsetM.bits.fragmentOffset << 3) + dataCount;
        }
    }

    //
    if (ptrFragment->packetSize == ptrFragment->bytesInPacket)
    {
        TCPIP_UINT16_VAL    aTcpIpUInt16Val;

        // Subtract the length of the IPV6 header from the payload
        ptrFragment->packetSize -= (uint16_t)sizeof (IPV6_HEADER);

        aTcpIpUInt16Val.Val = ptrFragment->packetSize;
        pFragPktData[ IPV6_HEADER_OFFSET_PAYLOAD_LENGTH ] =       aTcpIpUInt16Val.v[ 1 ];
        pFragPktData[ IPV6_HEADER_OFFSET_PAYLOAD_LENGTH + 1U ] =   aTcpIpUInt16Val.v[ 0 ];

        // fragmentation complete; reinsert the packet for further processing
        pFragMacPkt->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_RX_INTERNAL;    // signal special internal packet
        if(TCPIPStackModuleRxInsert(TCPIP_MODULE_IPV6, pFragMacPkt, true))
        {
            ptrFragment->ptrPacket = NULL;  // do NOT free the packet while in transit!
            ptrFragment->firstFragmentLength = 0;
            (void)TCPIP_Helper_SingleListNodeRemove(&pIpv6Config->rxFragments, FC_FragBuff2SglNode(ptrFragment));
            TCPIP_IPV6_FragmentBufferFree (ptrFragment);
        }
        else
        {   // failed; wait for other fragment or timeout
            pFragMacPkt->pktFlags &= ~(uint32_t)TCPIP_MAC_PKT_FLAG_RX_INTERNAL;
            // return IPV6_ACTION_DISCARD_SILENT and pRxPkt will be discarded
        }
    }

    return IPV6_ACTION_DISCARD_SILENT;
}


static void TCPIP_IPV6_FragmentTask (void)
{
    IPV6_RX_FRAGMENT_BUFFER * ptrRxFragment;
    IPV6_RX_FRAGMENT_BUFFER * ptrNextRxFragment;
    const TCPIP_NET_IF * pNetIf;
    size_t netIx;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    for (netIx = 0; netIx < nStackIfs; netIx++)
    {
        pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
        pIpv6Config = ipv6Config + netIx;
        if(TCPIP_STACK_NetworkIsUp(pNetIf))
        {
            ptrRxFragment = FC_SglNode2FragBuff(pIpv6Config->rxFragments.head);
            while (ptrRxFragment != NULL)
            {
                ptrRxFragment->secondsRemaining--;
                if (ptrRxFragment->secondsRemaining == 0U)
                {
                    if (ptrRxFragment->firstFragmentLength != 0U)
                    {
                        TCPIP_MAC_PACKET* pFragMacPkt = FC_CvPtr2MacPkt(ptrRxFragment->ptrPacket);
                        IPV6_HEADER* pFragHdr = FC_VPtr2IPv6Hdr(pFragMacPkt->pNetLayer);

                        // If we received the first fragment, send it and a Time Exceeded error message
                        IPV6_ADDR* remAdd = FC_U8Ptr2Ip6Add(pFragHdr->DestAddress.v);
                        IPV6_ADDR* lclAdd = FC_U8Ptr2Ip6Add(pFragHdr->SourceAddress.v);
                        TCPIP_IPV6_ErrorSend (pNetIf, pFragMacPkt, remAdd, lclAdd, (uint8_t)ICMPV6_ERR_TE_FRAG_ASSEMBLY_TIME_EXCEEDED, (uint8_t)ICMPV6_ERROR_TIME_EXCEEDED, 0U, ptrRxFragment->firstFragmentLength);

                    }
                    (void)TCPIP_Helper_SingleListNodeRemove(&pIpv6Config->rxFragments, FC_FragBuff2SglNode(ptrRxFragment));
                    ptrNextRxFragment = ptrRxFragment->next;
                    TCPIP_IPV6_FragmentBufferFree (ptrRxFragment);
                    ptrRxFragment = ptrNextRxFragment;
                }
                else
                {
                    ptrRxFragment = ptrRxFragment->next;
                }
            }
        }
    }
}



void TCPIP_IPV6_Task (void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    { //  RX signal occurred
        TCPIP_IPV6_ProcessPackets();
    }

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred
        TCPIP_IPV6_Timeout();
    }
}


static void TCPIP_IPV6_Timeout (void)
{
    uint32_t currTick;

    TCPIP_IPV6_InitializeTask();

    currTick = SYS_TMR_TickCountGet();
    uint32_t tickFreq = SYS_TMR_TickCounterFrequencyGet();

    if(currTick - ipv6StartTick >= (tickFreq * (uint32_t)TCPIP_IPV6_TASK_PROCESS_RATE) / 1000U)
    {   // time to run the IPv6 tasks
        TCPIP_IPV6_FragmentTask();
        TCPIP_IPV6_TimestampsTaskUpdate();
        F_TCPIP_IPV6_QueuedPacketTransmitTask(&mcastQueue);
        F_TCPIP_IPV6_QueuedPacketTransmitTask(&ipv6QueuedPackets);
#if (TCPIP_IPV6_ULA_GENERATE_ENABLE != 0)
        TCPIP_IPV6_UlaTask();
#endif  // (TCPIP_IPV6_ULA_GENERATE_ENABLE != 0)

        ipv6StartTick =  currTick;
    }
}

static void TCPIP_IPV6_ProcessPackets(void)
{
    TCPIP_MAC_PACKET* pRxPkt;

    // extract queued IPV6 packets
    while((pRxPkt = TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != NULL)
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
#if (TCPIP_IPV6_EXTERN_PACKET_PROCESS != 0)
        if(ipv6PktHandler != NULL)
        {
            bool was_processed = (*ipv6PktHandler)(pRxPkt->pktIf, pRxPkt, ipv6PktHandlerParam);
            if(was_processed)
            {
                TCPIP_PKT_FlightLogAcknowledge(pRxPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_EXTERN);
                continue;
            }
        }
#endif  // (TCPIP_IPV6_EXTERN_PACKET_PROCESS != 0)
        TCPIP_IPV6_Process ((const TCPIP_NET_IF*)pRxPkt->pktIf, pRxPkt);
    }
}


/*****************************************************************************
  Function:
    void F_TCPIP_IPV6_QueuedPacketTransmitTask (PROTECTED_SINGLE_LIST* pList)

  Summary:
        Task to transmit/time-out IPv6 queued packets

  Description:
        Task to transmit/time-out IPv6 queued packets

  Precondition:
    None

  Parameters:
    pList   - list to process/transmit

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static void F_TCPIP_IPV6_QueuedPacketTransmitTask (PROTECTED_SINGLE_LIST* pList)
{
    IPV6_PACKET * queuedPacket;
    uint32_t tick;


    while((queuedPacket = FC_SglNode2Ip6Pkt(TCPIP_Helper_ProtSglListHeadRemove(pList))) != NULL)
    {
        tick = SYS_TMR_TickCountGet();
        if (FC_Ui322I32(tick - queuedPacket->queuedPacketTimeout) > 0)
        {   // timeout; remove packet
            queuedPacket->flags.queued = 0U;
            TCPIP_IPV6_PacketAck(queuedPacket, false);
        }
        else
        {   // try to transmit it
            if (TCPIP_IPV6_PacketTransmit(queuedPacket))
            {   // success
                queuedPacket->flags.queued = 0U;
                TCPIP_IPV6_PacketAck(queuedPacket, true);
            }
            else
            {   // failed to transmit; reinsert
                TCPIP_Helper_ProtSglListHeadAdd(pList, FC_Ip6Pkt2SglNode(queuedPacket));
                return;
            }
        }
    }
}


static IPV6_ACTION TCPIP_IPV6_RoutingHeaderProcess (const TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length)
{
    uint8_t headerLength;
    uint8_t segmentsLeft;
    uint8_t data[8];


    (void)TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1U);

    *nextHeader = data[0];
    headerLength = data[1];
    *length = ((uint16_t)headerLength << 3U) + 8U;

    segmentsLeft = data[3];


    // uint8_t routingType = data[2];
    // switch (routingType)
    // Type 0 routing headers were deprecated and we aren't a router,
    // so we don't support any routing options
    if (segmentsLeft != 0U)
    {
        // Set the 'length' parameter to the offset of the routingType field.
        // This will allow the TCPIP_IPV6_Process function to find the correct offset
        // for the ICMPv6 Parameter Problem error message.
        *length = 2;
        return IPV6_ACTION_DISCARD_PP_0;
    }

    // If we get here, ignore the rest of the header
    // Since the header size is a multiple of 8 bytes,
    // just discard the rest of the bytes in the first
    // 8-byte unit and read the full header size
    while (headerLength-- != 0U)
    {
        (void)TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1U);
    }

    return IPV6_ACTION_NONE;
}


// ipv6.h
IPV6_ADDR_STRUCT * TCPIP_IPV6_DASSourceAddressSelect (TCPIP_NET_HANDLE hNetIf, const IPV6_ADDR * dest, const IPV6_ADDR * requestedSource)
{
    IPV6_ADDR_STRUCT * currentSource;
    IPV6_ADDR_STRUCT * previousSource;
    uint8_t ruleCounter = (uint8_t)ADDR_SEL_RULE_8;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    TCPIP_NET_IF * pNetIf = TCPIPStackHandleToNetUp(hNetIf);
    if (pNetIf == NULL)
    {
        return NULL;
    }
    else
    {
        pIpv6Config = ipv6Config + TCPIP_STACK_NetIxGet (pNetIf);
    }

    // Check to see if the user is trying to force an address
    if (requestedSource != NULL)
    {
        currentSource = TCPIP_IPV6_AddressFind(pNetIf, requestedSource, IPV6_ADDR_TYPE_UNICAST);
        if (currentSource != NULL)
        {
            return currentSource;
        }
        else
        {
            return NULL;
        }
    } // End manual address selection

    // Simple case: there are no local addresses (this should never happen)
    if (TCPIP_Helper_DoubleListIsEmpty(&pIpv6Config->listIpv6UnicastAddresses))
    {
        return NULL;
    }

    // Simple case: there's only one source address in the list
    if (TCPIP_Helper_DoubleListCount (&pIpv6Config->listIpv6UnicastAddresses) == 1U)
    {
        return FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head);
    }

    // Complex case: Sort the addresses we found using the Address Selection rules

    // Sort the linked list.
    // There are 8 sorting rules.  Starting with the last rule and working to the most
    // important, using a stable sorting algorithm, will produce a sorted list most
    // efficiently.  The best average run time we'll get with a stable sort with O(1)
    // memory usage is O(n^2), so we'll use an insertion sort.  This will usually be
    // most efficient for small lists (which should be the typical case).

    do
    {
        // We know that the list has at least two elements, so these pointers will both have non-null values
        previousSource = FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head);
        currentSource = previousSource->next;

        do
        {
            // Set previousSource to the node before the current node being evaluated
            previousSource = currentSource->prev;

            // Advance backwards through the list until we don't prefer currentSource over previousSource
            // (or until there are no previous source addresses)
            // The TCPIP_IPV6_ASCompareSourceAddresses function will return true if we prefer currentSource over previousSource
            while ((previousSource != NULL) && (TCPIP_IPV6_ASCompareSourceAddresses (pNetIf, previousSource, currentSource, dest, ruleCounter)))
            {
                previousSource = previousSource->prev;
            }

            // Move the currentSource node into the list after previousSource (or to the head if previousSource == NULL)
            currentSource = TCPIP_NDP_UnicastAddressMove (pNetIf, currentSource, previousSource);
        } while (currentSource != NULL);

        ruleCounter--;
        // Skip rules 4 and 5 for now; we don't support Mobile IPv6 or multiple interfaces at this time
        if (ruleCounter == (uint8_t)ADDR_SEL_RULE_5)
        {
            ruleCounter = (uint8_t)ADDR_SEL_RULE_3;
        }
    } while (ruleCounter >= (uint8_t)ADDR_SEL_RULE_1);

    return FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head);
}


static bool TCPIP_IPV6_DASPolicyGet (const IPV6_ADDR * addr, uint8_t * label, uint8_t * precedence, uint8_t * prefixLen)
{
    uint8_t i;
    uint8_t prefixMatch = 0U;
    uint8_t matchingPrefix = 0xFFU;

    for (i = 0; i < (sizeof (gPolicyTable) / sizeof (*gPolicyTable)); i++)
    {
        if (gPolicyTable[i].prefixLength != 0xFFU)
        {
            // If we get to the 0-length prefix and we haven't found a
            // matching prefix, then assume a match
            if (gPolicyTable[i].prefixLength == 0U)
            {
                if (prefixMatch == 0U)
                {
                    matchingPrefix = i;
                }
            }
            else if (gPolicyTable[i].prefixLength > prefixMatch)
            {
                if (TCPIP_Helper_FindCommonPrefix ((const uint8_t *)addr, (const uint8_t *)&gPolicyTable[i].address, 16) >= gPolicyTable[i].prefixLength)
                {
                    matchingPrefix = i;
                    prefixMatch = gPolicyTable[i].prefixLength;
                }
            }
            else
            {
                // do nothing
            }
        }
    }

    if (matchingPrefix == 0xFFU)
    {
        return false;
    }
    else
    {
        if (label != NULL)
        {
            *label = gPolicyTable[matchingPrefix].label;
        }
        if (precedence != NULL)
        {
            *precedence = gPolicyTable[matchingPrefix].precedence;
        }
        if (prefixLen != NULL)
        {
            *prefixLen = gPolicyTable[matchingPrefix].prefixLength;
        }
    }
    return true;
}


static bool TCPIP_IPV6_ASCompareSourceAddresses(TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * addressOne, IPV6_ADDR_STRUCT * addressTwo, const IPV6_ADDR * dest, uint8_t rule)
{
    unsigned char policy1 = 0;
    unsigned char policy2 = 0;
    unsigned char destPolicy;
    IPV6_ADDRESS_TYPE destScope;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    const IPV6_ADDR *pDasOne, *pDasTwo;

    if (pNetIf == NULL)
    {
        return false;
    }

    pIpv6Config = ipv6Config + TCPIP_STACK_NetIxGet (pNetIf);

    switch (rule)
    {
        case (uint8_t)ADDR_SEL_RULE_1:
            // We can assume the the addresses are different; the function to add a local
            // address won't add a new one if it's already in the IPv6 Heap.
            {
                const IPV6_ADDR* pAddTwo = &addressTwo->address;
                if (memcmp (pAddTwo->v, dest->v, 16) == 0)
                {
                    return true;
                }
            }
            break;
        case (uint8_t)ADDR_SEL_RULE_2:
            if (addressOne->flags.scope != addressTwo->flags.scope)
            {
                destScope.byte = TCPIP_IPV6_AddressTypeGet (pNetIf, dest);
                destPolicy = destScope.bits.scope;

                if (addressOne->flags.scope < addressTwo->flags.scope)
                {
                    if (addressOne->flags.scope < destPolicy)
                    {
                        return true;
                    }
                }
                else
                {
                    if (addressTwo->flags.scope >= destPolicy)
                    {
                        return true;
                    }
                }
            }
            break;
        case (uint8_t)ADDR_SEL_RULE_3:
            if (addressTwo->preferredLifetime != 0U && addressOne->preferredLifetime == 0U)
            {
                return true;
            }
            break;
        case (uint8_t)ADDR_SEL_RULE_4:
            // We aren't supporting Mobile IPv6 at this time
            break;
        case (uint8_t)ADDR_SEL_RULE_5:
            // We aren't supporting multiple interfaces at this time
            break;
        case (uint8_t)ADDR_SEL_RULE_6:
            if (!TCPIP_IPV6_DASPolicyGet (dest, &destPolicy, NULL, NULL))
            {
                // If there's no policy that corresponds to the destination, skip this step
                break;
            }
            pDasOne = &addressOne->address;
            if (!TCPIP_IPV6_DASPolicyGet (pDasOne, &policy1, NULL, NULL))
            {
                pDasTwo = &addressTwo->address;
                if (TCPIP_IPV6_DASPolicyGet (pDasTwo, &policy2, NULL, NULL))
                {
                    if (destPolicy == policy2)
                    {
                        return true;
                    }
                }
            }
            else
            {
                pDasTwo = &addressTwo->address;
                if (!TCPIP_IPV6_DASPolicyGet (pDasTwo, &policy2, NULL, NULL))
                {
                    if (destPolicy == policy1)
                    {
                        return false;
                    }
                }
            }
            if (policy1 != policy2)
            {
                if (destPolicy == policy2)
                {
                    return true;
                }
            }
            break;
        case (uint8_t)ADDR_SEL_RULE_7:
            if (addressOne->flags.temporary != addressTwo->flags.temporary)
            {
                if (((addressTwo->flags.temporary == 0U) && (pIpv6Config->policyPreferTempOrPublic == (uint8_t)IPV6_PREFER_PUBLIC_ADDRESSES)) ||
                    ((addressTwo->flags.temporary == 1U) && (pIpv6Config->policyPreferTempOrPublic == (uint8_t)IPV6_PREFER_TEMPORARY_ADDRESSES)))
                {
                    return true;
                }
            }
            break;
        case (uint8_t)ADDR_SEL_RULE_8:
            pDasOne = &addressOne->address;
            pDasTwo = &addressTwo->address;
            policy1 = TCPIP_Helper_FindCommonPrefix ((const uint8_t *)dest, (const uint8_t*)pDasOne, 16);
            policy2 = TCPIP_Helper_FindCommonPrefix ((const uint8_t *)dest, (const uint8_t*)pDasTwo, 16);
            if (policy2 > policy1)
            {
                return true;
            }
            break;

        default:
            // do nothing
            break;
    }
    // If there's no reason to prefer addressTwo, return false
    return false;
}


static void TCPIP_IPV6_TimestampsTaskUpdate (void)
{
    uint32_t timeElapsed;
    uint32_t currentTickTime = SYS_TMR_TickCountGet();

    unsigned long correctedCurrentTime;
    size_t ifIx;
    IPV6_HEAP_NDP_PL_ENTRY * ptrPrefix;
    IPV6_HEAP_NDP_PL_ENTRY * tempPrefix;
    IPV6_ADDR_STRUCT * ptrAddress;
    IPV6_ADDR_STRUCT * tempAddress;
    IPV6_HEAP_NDP_DR_ENTRY * ptrRouter;
    IPV6_HEAP_NDP_DC_ENTRY * ptrDestination;
    const TCPIP_NET_IF * pNetIf;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    uint32_t tickFreq = SYS_TMR_TickCounterFrequencyGet();
    for (ifIx = 0; ifIx < nStackIfs; ifIx++)
    {
        pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(ifIx);
        pIpv6Config = ipv6Config + ifIx;
        if(TCPIP_STACK_NetworkIsUp(pNetIf))
        {
            // Update prefixes
            ptrPrefix = FC_SglNode2NdpEntry(pIpv6Config->listPrefixList.head).pPlEntry;

            while (ptrPrefix != NULL)
            {
                timeElapsed = currentTickTime - ptrPrefix->lastTickTime;
                timeElapsed /= tickFreq;
                correctedCurrentTime = currentTickTime - (timeElapsed % tickFreq);


                ptrPrefix->lastTickTime = correctedCurrentTime;
                if (timeElapsed < ptrPrefix->validLifetime)
                {
                    ptrPrefix->validLifetime -= timeElapsed;
                    tempPrefix = ptrPrefix->next;
                }
                else
                {
                    tempPrefix = ptrPrefix->next;
                    (void)TCPIP_NDP_LinkedListEntryRemove ((const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(ifIx), ptrPrefix, (uint8_t)IPV6_HEAP_NDP_PL_ID);
                }
                ptrPrefix = tempPrefix;
            }

            // Update addresses
            ptrAddress = FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head);

            while (ptrAddress != NULL)
            {
                timeElapsed = currentTickTime - ptrAddress->lastTickTime;
                timeElapsed /= tickFreq;
                correctedCurrentTime = currentTickTime - (timeElapsed % tickFreq);


                ptrAddress->lastTickTime = correctedCurrentTime;
                if (timeElapsed < ptrAddress->preferredLifetime)
                {
                    ptrAddress->preferredLifetime -= timeElapsed;
                }
                else
                {
                    ptrAddress->preferredLifetime = 0;
                }
                if (timeElapsed < ptrAddress->validLifetime)
                {
                    ptrAddress->validLifetime -= timeElapsed;
                    tempAddress = ptrAddress->next;
                }
                else
                {
                    tempAddress = ptrAddress->next;
                    const IPV6_ADDR* pRemAdd = &ptrAddress->address;
                    TCPIP_IPV6_AddressUnicastRemove((const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(ifIx), pRemAdd);
                }
                ptrAddress = tempAddress;

            }

            // Update default routers
            ptrRouter = FC_SglNode2NdpEntry(pIpv6Config->listDefaultRouter.head).pDrEntry;

            while (ptrRouter != NULL)
            {
                if ((currentTickTime - ptrRouter->tickTimer) >= tickFreq)
                {
                    ptrRouter->tickTimer += tickFreq;
                    ptrRouter->invalidationTimer--;
                    if (ptrRouter->invalidationTimer == 0U)
                    {
                        ptrRouter = TCPIP_NDP_LinkedListEntryRemove ((const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(ifIx), ptrRouter, (uint8_t)IPV6_HEAP_NDP_DR_ID);
                    }
                }
                else
                {
                    ptrRouter = ptrRouter->next;
                }
            }

            // Try to periodically increase path MTUs
            ptrDestination = FC_SglNode2NdpEntry(pIpv6Config->listDestinationCache.head).pDcEntry;

            while (ptrDestination != NULL)
            {
                if (ptrDestination->pathMTUIncreaseTimer != 0U)
                {
                    if (FC_Ui322I32(currentTickTime - ptrDestination->pathMTUIncreaseTimer) > 0)
                    {
                        ptrDestination->pathMTU = TCPIP_IPV6_DEFAULT_LINK_MTU;
                        ptrDestination->pathMTUIncreaseTimer = 0;
                    }
                }
                ptrDestination = ptrDestination->next;
            }

            if (pIpv6Config->mtuIncreaseTimer != 0U)
            {
                if (FC_Ui322I32(currentTickTime - pIpv6Config->mtuIncreaseTimer) > 0)
                {
                    pIpv6Config->linkMTU = TCPIP_IPV6_DEFAULT_LINK_MTU;
                    pIpv6Config->multicastMTU = TCPIP_IPV6_DEFAULT_LINK_MTU;
                    pIpv6Config->mtuIncreaseTimer = 0;
                }
            }
        }
    }
}

static void TCPIP_IPV6_Process (const TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt)
{
    IPV6_ADDR tempLocalIPv6Addr;
    IPV6_ADDR tempRemoteIPv6Addr;
    IPV6_ADDR_STRUCT * localAddressPointer;
    uint16_t headerLen = 0U;
    uint16_t extensionHeaderLen;
    IPV6_ACTION action;
    uint8_t hopLimit;
    uint8_t addrType = 0U;
    uint16_t currentOffset = 0U;
    uint8_t cIPFrameType;
    uint16_t dataCount;
    bool ackPacket;
    bool rxInsert;

    const IPV6_ADDR* constLocalIPv6Addr;
    const IPV6_ADDR* constRemoteIPv6Addr;

    // Break out of this processing function is IPv6 is not enabled on this node
    if (pNetIf->Flags.bIPv6Enabled == 0U)
    {
        TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_NET_DOWN); 
        return;
    }

    if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_SPLIT) != 0U)
    {   // MAC fragmented packets not supported yet
        TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_FRAGMENT_ERR); 
        return;
    }

    // Get the relevant IPv6 header parameters
    if (!TCPIP_IPV6_HeaderGet(pRxPkt, &tempLocalIPv6Addr, &tempRemoteIPv6Addr, &cIPFrameType, &dataCount, &hopLimit))
    {
        TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_STRUCT_ERR); 
        return;
    }

    constLocalIPv6Addr = &tempLocalIPv6Addr;
    constRemoteIPv6Addr = &tempRemoteIPv6Addr;

    // set a valid ack result; could be overridden by processing tasks
    pRxPkt->ipv6PktData = (uint16_t)TCPIP_MAC_PKT_ACK_RX_OK;
    
    currentOffset += (uint16_t)sizeof (IPV6_HEADER);

    // Determine if the address corresponds to one of the addresses used by our node
    if (constLocalIPv6Addr->v[0] == 0xFFU)
    {
        // Determine if the address is a solicited node multicast address
        if (TCPIP_IPV6_AddressIsSolicitedNodeMulticast (constLocalIPv6Addr))
        {
            // Determine if we are listening to this address
            if ((localAddressPointer = TCPIP_IPV6_SolicitedNodeMulticastAddressFind(pNetIf, constLocalIPv6Addr, (uint8_t)IPV6_ADDR_TYPE_UNICAST)) != NULL)
            {
                addrType = (uint8_t)IPV6_ADDR_TYPE_SOLICITED_NODE_MULTICAST;
            }
            else if ((localAddressPointer = TCPIP_IPV6_SolicitedNodeMulticastAddressFind(pNetIf, constLocalIPv6Addr, (uint8_t)IPV6_ADDR_TYPE_UNICAST_TENTATIVE)) != NULL)
            {
                addrType = (uint8_t)IPV6_ADDR_TYPE_UNICAST_TENTATIVE;
            }
            else
            {
                // do nothing
            }
        }
        else
        {
            // Find the address in the list of multicast addresses we're listening to
            localAddressPointer = TCPIP_IPV6_AddressFind(pNetIf, constLocalIPv6Addr, IPV6_ADDR_TYPE_MULTICAST);
            addrType = (uint8_t)IPV6_ADDR_TYPE_MULTICAST;
        }
    }
    else
    {
        // Find the address in the list of unicast addresses assigned to our node
        localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, constLocalIPv6Addr, IPV6_ADDR_TYPE_UNICAST);
        addrType = (uint8_t)IPV6_ADDR_TYPE_UNICAST;
    }

    // If the packet's destination address isn't one of the unicast/multicast addresses assigned to this node, check to see if it is a
    // tentative address (ICMPv6/NDP still needs to receive packets addressed to tentative addresses for duplicate address detection).
    // If it is not tentative, return.
    if (localAddressPointer == NULL)
    {
        // If we didn't find a matching configured address try to find one in the tentative address list
        if ((localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, constLocalIPv6Addr, IPV6_ADDR_TYPE_UNICAST_TENTATIVE)) != NULL)
        {
            addrType = (uint8_t)IPV6_ADDR_TYPE_UNICAST_TENTATIVE;
        }
        else
        {
            TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR); 
            return;
        }
    }

    extensionHeaderLen = 0;
    action = IPV6_ACTION_BEGIN_EX_HEADER_PROCESSING;
    ackPacket = true;

    // Process frame
    while (cIPFrameType != (uint8_t)IPV6_PROT_NONE)
    {
        switch (cIPFrameType)
        {
            // Process the frame's routing header
            case (uint8_t)IPV6_PROT_ROUTING_HEADER:
                action = TCPIP_IPV6_RoutingHeaderProcess (pNetIf, pRxPkt, &cIPFrameType, &headerLen);
                dataCount -= headerLen;
                extensionHeaderLen += headerLen;
                break;
            // Process the frame's fragmentation header
            case (uint8_t)IPV6_PROT_FRAGMENTATION_HEADER:
                action = TCPIP_IPV6_FragmentationHeaderProcess (pNetIf, constRemoteIPv6Addr, constLocalIPv6Addr, &cIPFrameType, dataCount, extensionHeaderLen + (uint16_t)sizeof (IPV6_HEADER), pRxPkt, headerLen);
                break;
            // Process the frame's ESP header
            case (uint8_t)IPV6_PROT_ENCAPSULATING_SECURITY_PAYLOAD_HEADER:
                action = IPV6_ACTION_DISCARD_SILENT;
                break;
            // Process the frame's Authentication header
            case (uint8_t)IPV6_PROT_AUTHENTICATION_HEADER:
                action = IPV6_ACTION_DISCARD_SILENT;
                break;
            // Process the frame's destination options header
            case (uint8_t)IPV6_PROT_DESTINATION_OPTIONS_HEADER:
                action = TCPIP_IPV6_DestinationOptionsHeaderProcess(pNetIf, pRxPkt, &cIPFrameType, &headerLen);
                dataCount -= headerLen;
                extensionHeaderLen += headerLen;
                break;
            // Process the frame's TCP header and payload
            case (uint8_t)IPV6_PROT_TCP:
#if defined (TCPIP_STACK_USE_TCP)
                // If the address is tentative, do not process the TCP packet
                if (addrType == (uint8_t)IPV6_ADDR_TYPE_UNICAST_TENTATIVE)
                {
                    cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                    break;
                }
                // set up the packet fields used by TCP
                pRxPkt->pDSeg->segLen = dataCount;
                pRxPkt->totTransportLen = dataCount;
                pRxPkt->ipv6PktData = extensionHeaderLen;
                // forward this packet and signal
                rxInsert = TCPIPStackModuleRxInsert(TCPIP_MODULE_TCP, pRxPkt, true);
                if(rxInsert)
                {   // success
                    ackPacket = false;
                }
                else
                {   // failed to insert
                    pRxPkt->ipv6PktData = (uint16_t)TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR; 
                }

                // either way the process is terminated
                action = IPV6_ACTION_NONE;
                cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                break;
#else
                cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                action = IPV6_ACTION_NONE;
                break;
#endif
            // Process the frame's UDP header and payload
            case (uint8_t)IPV6_PROT_UDP:
#if defined (TCPIP_STACK_USE_UDP)
                // If the address is tentative, do not process the TCP packet
                if (addrType == (uint8_t)IPV6_ADDR_TYPE_UNICAST_TENTATIVE)
                {
                    cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                    break;
                }
                // set up the packet fields used by UDP
                pRxPkt->pDSeg->segLen = dataCount;
                pRxPkt->totTransportLen = dataCount;
                pRxPkt->ipv6PktData = extensionHeaderLen;
                // forward this packet and signal
                rxInsert = TCPIPStackModuleRxInsert(TCPIP_MODULE_UDP, pRxPkt, true);
                if(rxInsert)
                {   // success
                    ackPacket = false;
                }
                else
                {   // failed to insert
                    pRxPkt->ipv6PktData = (uint16_t)TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR; 
                }

                // either way the process is terminated
                action = IPV6_ACTION_NONE;
                cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                break;
#else
                cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                action = IPV6_ACTION_NONE;
                break;
#endif
            // Process the frame's ICMPv6 header and payload
            case (uint8_t)IPV6_PROT_ICMPV6:
                // Process the ICMPv6 packet
                TCPIP_ICMPV6_Process(pNetIf, pRxPkt, localAddressPointer, constLocalIPv6Addr, constRemoteIPv6Addr, dataCount, extensionHeaderLen, hopLimit, addrType); 
                cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                action = IPV6_ACTION_NONE;
                break;
            // Process the frame's hop-by-hop options header
            case (uint8_t)IPV6_PROT_HOP_BY_HOP_OPTIONS_HEADER:
            default:
                if(cIPFrameType == (uint8_t)IPV6_PROT_HOP_BY_HOP_OPTIONS_HEADER && action == IPV6_ACTION_BEGIN_EX_HEADER_PROCESSING)
                {
                    // Action should only equal 0xFF immediately after processing the IPv6 header.
                    // The hop-by-hop options header must occur only after the IPv6 header.
                    action = TCPIP_IPV6_HopByHopOptionsHeaderProcess (pNetIf, pRxPkt, &cIPFrameType, &headerLen);
                    dataCount -= headerLen;
                    extensionHeaderLen += headerLen;
                    break;
                }
                // else unknown header type, parameter problem
                // Send ICMP Parameter Problem Code 1 and discard packet
                // Action should only equal IPV6_ACTION_BEGIN_EX_HEADER_PROCESSING if we haven't been through this loop once
                if (action == IPV6_ACTION_BEGIN_EX_HEADER_PROCESSING)
                {
                    currentOffset = IPV6_HEADER_OFFSET_NEXT_HEADER;
                }
                else
                {
                    currentOffset -= headerLen;
                }

                TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, constLocalIPv6Addr, constRemoteIPv6Addr, (uint8_t)ICMPV6_ERR_PP_UNRECOGNIZED_NEXT_HEADER, (uint8_t)ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl((uint32_t)currentOffset), dataCount + extensionHeaderLen + (uint16_t)sizeof (IPV6_HEADER));
                cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                break;
        }

        // Add the length of the header to the current offset
        // If there was a parameter problem, this value will indicate the offset
        // in the header at which the parameter problem occured.
        currentOffset += headerLen;

        // Take an action depending on the result of our header processing
        switch (action)
        {
            // Silently discard the packet
            case IPV6_ACTION_DISCARD_SILENT:
                cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                break;
            // Discard the packet and send an ICMPv6 Parameter Problem, code 0 error message to the packet's source address
            case IPV6_ACTION_DISCARD_PP_0:
                TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, constLocalIPv6Addr, constRemoteIPv6Addr, (uint8_t)ICMPV6_ERR_PP_ERRONEOUS_HEADER, (uint8_t)ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl((uint32_t)currentOffset), dataCount + extensionHeaderLen + (uint16_t)sizeof (IPV6_HEADER));
                cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                break;
            // Discard the packet and send an ICMPv6 Parameter Problem, code 2 error message to the packet's source address
            case IPV6_ACTION_DISCARD_PP_2:
                TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, constLocalIPv6Addr, constRemoteIPv6Addr, (uint8_t)ICMPV6_ERR_PP_UNRECOGNIZED_IPV6_OPTION, (uint8_t)ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl((uint32_t)currentOffset), dataCount + extensionHeaderLen + (uint16_t)sizeof (IPV6_HEADER));
                cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                break;
            // Discard the packet and send an ICMPv6 Parameter Problem, code 2 error message to the packet's source address if
            // the packet's destination address is not a multicast address.
            case IPV6_ACTION_DISCARD_PP_2_NOT_MC:
                // Check to ensure the packet's destination address wasn't a multicast address
                if (constLocalIPv6Addr->v[0] != 0xFFU)
                {
                    TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, constLocalIPv6Addr, constRemoteIPv6Addr, (uint8_t)ICMPV6_ERR_PP_UNRECOGNIZED_IPV6_OPTION, (uint8_t)ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl((uint32_t)currentOffset), dataCount + extensionHeaderLen + (uint16_t)sizeof (IPV6_HEADER));
                }
                // Discard the packet
                cIPFrameType = (uint8_t)IPV6_PROT_NONE;
                break;

            // No action was required
            case IPV6_ACTION_NONE:
            default:
                // do nothing
                break;
        }
    }

    if(ackPacket)
    {
        TCPIP_PKT_PacketAcknowledge(pRxPkt, (TCPIP_MAC_PKT_ACK_RES)pRxPkt->ipv6PktData); 
    }
}


// ipv6_manager.h
void TCPIP_IPV6_ErrorSend (const TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, uint8_t code, uint8_t type, uint32_t additionalData, uint16_t packetLen)
{
    IPV6_PACKET * pkt;

    if (packetLen + (uint16_t)sizeof (IPV6_HEADER) + (uint16_t)sizeof (ICMPV6_HEADER_ERROR) > (uint16_t)TCPIP_IPV6_MINIMUM_LINK_MTU)
    {
        packetLen = (uint16_t)TCPIP_IPV6_MINIMUM_LINK_MTU - ((uint16_t)sizeof (IPV6_HEADER) + (uint16_t)sizeof (ICMPV6_HEADER_ERROR));
    }

    // An ICMPv6 error message MUST NOT be sent as a result of receiving a packet destined to an IPv6 multicast address
    // Exception: Packet Too Big message, Parameter Problem code 2 message if the unrecognized option has its unrecognized action bits set to 0b10.
    // The unrecognized action bits were already examined in the option processing functions.
    if ((localIP->v[0] == 0xFFU) && (type != (uint8_t)ICMPV6_ERROR_PACKET_TOO_BIG) && ((type != (uint8_t)ICMPV6_ERROR_PARAMETER_PROBLEM) || (code != (uint8_t)ICMPV6_ERR_PP_UNRECOGNIZED_IPV6_OPTION)))
    {
        return;
    }

    // An ICMPv6 error message MUST NOT be sent as a result of receiving a packet whose source address does not uniquely identify a single node.
    if ((remoteIP->v[0] == 0xFFU) || memcmp (remoteIP->v, IPV6_FIXED_ADDR_UNSPECIFIED.v, sizeof (IPV6_ADDR)) == 0)
    {
        return;
    }

    pkt = TCPIP_ICMPV6_HeaderErrorPut (pNetIf, localIP, remoteIP, code, type, additionalData);
    if (pkt != NULL)
    {
        MACSetReadPtrInRx (pRxPkt, 0);
        if (TCPIP_IPV6_TxIsPutReady(pkt, packetLen) < packetLen)
        {
            TCPIP_IPV6_PacketFree (pkt);
            return;
        }
        (void)TCPIP_IPV6_ArrayPutHelper(pkt, pRxPkt, IPV6_DATA_NETWORK_FIFO, packetLen);
        (void)TCPIP_ICMPV6_Flush (pkt);
    }
}


// ipv6_manager.h
void * TCPIP_IPV6_UpperLayerHeaderPtrGet(IPV6_PACKET * pkt)
{
    return TCPIP_IPV6_DataSegmentContentsGetByType(pkt, (uint8_t)TYPE_IPV6_UPPER_LAYER_HEADER);
}


// ipv6_manager.h
unsigned short TCPIP_IPV6_PayloadLengthGet (IPV6_PACKET * pkt)
{
    return pkt->payloadLen + pkt->upperLayerHeaderLen;
}


// ipv6.h
IPV6_ADDR *  TCPIP_IPV6_DestAddressGet(IPV6_PACKET * p)
{
    return &p->ipv6Header.DestAddress;
}


// ipv6.h
void  TCPIP_IPV6_DestAddressSet(IPV6_PACKET * p, const IPV6_ADDR * addr)
{
    if(addr != NULL)
    {
        (void)memcpy (p->ipv6Header.DestAddress.v, addr->v, sizeof (IPV6_ADDR));
    }
    else
    {
        (void)memset (p->ipv6Header.DestAddress.v, 0x0, sizeof (IPV6_ADDR));
    }
}


// ipv6.h
void  TCPIP_IPV6_SourceAddressSet(IPV6_PACKET * p, const IPV6_ADDR * addr)
{
    if(addr != NULL)
    {
        (void)memcpy (&p->ipv6Header.SourceAddress, addr, sizeof(IPV6_ADDR));
        p->flags.sourceSpecified = 1U;
    }
    else
    {
        (void)memset (&p->ipv6Header.SourceAddress, 0x0, sizeof(IPV6_ADDR));
        p->flags.sourceSpecified = 0U;
    }
}


// ipv6.h
IPV6_ADDR *  TCPIP_IPV6_SourceAddressGet(IPV6_PACKET * p)
{
    return &p->ipv6Header.SourceAddress;
}


// ipv6.h - Register an IPv6 event handler. Use hNet == 0 to register on all interfaces available
IPV6_HANDLE TCPIP_IPV6_HandlerRegister(TCPIP_NET_HANDLE hNet, IPV6_EVENT_HANDLER handler, const void* hParam)
{
    if(handler != NULL && ipv6MemH != NULL)
    {
        IPV6_LIST_NODE ipv6Node;
        ipv6Node.next = NULL;
        ipv6Node.handler = handler;
        ipv6Node.hParam = hParam;
        ipv6Node.hNet = hNet;

        return FC_SglNode2Ip6ListNode(TCPIP_Notification_Add(&ipv6RegisteredUsers, ipv6MemH, &ipv6Node, sizeof(ipv6Node)));
    }

    return NULL;
}


// ipv6.h -  deregister the event handler
bool TCPIP_IPV6_HandlerDeregister(IPV6_HANDLE hIpv6)
{
    if(hIpv6 != NULL && ipv6MemH != NULL)
    {
        if(TCPIP_Notification_Remove(FC_Ip6Hndl2SglNode(hIpv6), &ipv6RegisteredUsers, ipv6MemH))
        {
            return true;
        }
    }

    return false;
}


// ipv6_manager.h
void TCPIP_IPV6_ClientsNotify(const TCPIP_NET_IF* pNetIf, IPV6_EVENT_TYPE evType, const void* evParam)
{
    IPV6_LIST_NODE* iNode;

    for(iNode = FC_SglNode2Ip6ListNode(ipv6RegisteredUsers.list.head); iNode != NULL; iNode = iNode->next)
    {
        if(iNode->hNet == NULL || iNode->hNet == pNetIf)
        {   // trigger event
            (*iNode->handler)(pNetIf, evType, evParam, iNode->hParam);
        }
    }

}


// ipv6_manager.h
IPV6_INTERFACE_CONFIG* TCPIP_IPV6_InterfaceConfigGet(const TCPIP_NET_IF* pNetIf)
{
    return ipv6Config + TCPIP_STACK_NetIxGet (pNetIf);
}

const TCPIP_IPV6_CONFIG_DCPT* TCPIP_IPV6_ConfigDcptGet(void)
{
    return &ipv6_config_dcpt;
}

const IPV6_ADDR* TCPIP_IPV6_DefaultRouterGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF * pNetIf =  TCPIPStackHandleToNet(netH);

    if(pNetIf != NULL)
    {
        IPV6_HEAP_NDP_DR_ENTRY* pEntry = TCPIP_NDP_DefaultRouterGet (pNetIf);
        if(pEntry != NULL && pEntry->neighborInfo != NULL)
        {
            return &pEntry->neighborInfo->remoteIPAddress;
        }
    }

    return NULL;

}

// ipv6_manager.h
void TCPIP_IPV6_DoubleListFree (void * list)
{
    DOUBLE_LIST * dList = (DOUBLE_LIST *) list;
    DBL_LIST_NODE * node;
    while ((node = TCPIP_Helper_DoubleListHeadRemove (dList)) != NULL)
    {
        (void)TCPIP_HEAP_Free (ipv6MemH, node);
    }
}


// ipv6_manager.h
void TCPIP_IPV6_SingleListFree (void * list)
{
    SINGLE_LIST * sList = (SINGLE_LIST *) list;
    SGL_LIST_NODE * node;
    while ((node = TCPIP_Helper_SingleListHeadRemove (sList)) != NULL)
    {
        (void)TCPIP_HEAP_Free (ipv6MemH, node);
    }
}



// ipv6.h

#if (TCPIP_IPV6_ULA_GENERATE_ENABLE != 0)
IPV6_ULA_RESULT TCPIP_IPV6_UniqueLocalUnicastAddressAdd (TCPIP_NET_HANDLE netH, uint16_t subnetID, IPV6_ULA_FLAGS genFlags, IP_MULTI_ADDRESS* ntpAddress)
{

    if(ulaState != TCPIP_IPV6_ULA_IDLE)
    {
        return IPV6_ULA_RES_BUSY;
    }

    TCPIP_NET_IF * pNetIf =  TCPIPStackHandleToNet(netH);

    if(pNetIf == NULL)
    {
        return IPV6_ULA_RES_IF_ERR;
    }
    
    // clear the error status
    (void)TCPIP_SNTP_LastErrorGet();
    ulaNetIf = pNetIf;
    ulaSubnetId = subnetID;
    ulaFlags = (uint8_t)genFlags;
    ulaOperStartTick = SYS_TMR_TickCountGet();

    ulaState = TCPIP_IPV6_ULA_PARAM_SET;


    return IPV6_ULA_RES_OK;
}

static void TCPIP_IPV6_UlaTask (void)
{
    TCPIP_SNTP_RESULT sntpRes;
    uint32_t    lastUpdateTick, tStampWindow, currTick;
    CRYPT_SHA_CTX  shaSum;
    uint8_t shaDigest[20];  // SHA1 size is 160 bits
    IPV6_ADDR ulaAddress;
    IPV6_ULA_RESULT genRes = IPV6_ULA_RES_OK;


    union
    {
        struct
        {
            TCPIP_UINT64_VAL eui64;
            uint64_t    tStamp;
        };
        uint8_t         b[128/8];
    }ulaTStamp;

    TCPIP_SNTP_TIME_STAMP sntpStamp;

    if(ulaState == TCPIP_IPV6_ULA_IDLE)
    {   // nothing to do
        return;
    }

    currTick = SYS_TMR_TickCountGet();
    uint32_t tickFreq = SYS_TMR_TickCounterFrequencyGet();

    switch(ulaState)
    {

        case TCPIP_IPV6_ULA_PARAM_SET:
        case TCPIP_IPV6_ULA_CONN_START:
            if(ulaState == TCPIP_IPV6_ULA_PARAM_SET)
            {
                sntpRes = TCPIP_SNTP_ConnectionParamSet(NULL, (ulaFlags & (uint8_t)IPV6_ULA_FLAG_NTPV4) != 0U ? IP_ADDRESS_TYPE_IPV4 : IP_ADDRESS_TYPE_IPV6, NULL);
            }
            else
            {
                sntpRes = TCPIP_SNTP_ConnectionInitiate();
            }

            if((int)sntpRes >= 0)
            {   // success
                ulaState = FC_UlaStateAddVal(ulaState, 1);  // advance to the next state
                ulaOperStartTick = currTick;
                return;
            }

            // failed to set the connection parameters
            if(currTick - ulaOperStartTick < (tickFreq * (uint32_t)TCPIP_IPV6_ULA_NTP_ACCESS_TMO + 999U) / 1000U)
            {   // can try again
                return;
            }

            // failed
            genRes = IPV6_ULA_RES_NTP_ACCESS_ERR;
            break;

        case TCPIP_IPV6_ULA_TSTAMP_GET:
            sntpRes = TCPIP_SNTP_TimeStampGet(&sntpStamp, &lastUpdateTick);
            ulaTStamp.tStamp = sntpStamp.llStamp;
            if(ulaOperStartTick >= lastUpdateTick)
            {
                tStampWindow = ulaOperStartTick - lastUpdateTick;
            }
            else
            {   // more recent result than requested?
                tStampWindow = 0;
            }

            if((int)sntpRes < 0 || tStampWindow > (tickFreq * (uint32_t)TCPIP_IPV6_ULA_NTP_VALID_WINDOW + 999U) / 1000U)
            {   // failed
                if(currTick - ulaOperStartTick < (tickFreq * (uint32_t)TCPIP_IPV6_ULA_NTP_ACCESS_TMO + 999U) / 1000U)

                {   // can try again
                    return;
                }
                else
                {   // failed
                    genRes = IPV6_ULA_RES_NTP_TSTAMP_ERR;
                    break;
                }
            }

            // success, valid response
            // get the interface EUI64 identifier
            TCPIP_IPV6_EUI64(ulaNetIf, &ulaTStamp.eui64.Val);

            // calculate the SHA1
            (void)CRYPT_SHA_Initialize(&shaSum);
            (void)CRYPT_SHA_DataAdd(&shaSum, ulaTStamp.b, sizeof(ulaTStamp));
            (void)CRYPT_SHA_Finalize(&shaSum, shaDigest);


            // format the IPv6 address
            (void)memset(ulaAddress.v, 0, sizeof(ulaAddress));

            ulaAddress.v[0] = 0xfd;     // global ULA prefix
            // Global ID as the SHA digest lowest 40 bits
            (void)memcpy(ulaAddress.v + 1, shaDigest + (sizeof(shaDigest) - 5U), 5U);
            // set the sub net ID
            ulaAddress.v[6] = (uint8_t)(ulaSubnetId >> 8U) ;
            ulaAddress.v[7] = (uint8_t)(ulaSubnetId & 0xffU) ;
            // set the Interface ID
            (void)memcpy(ulaAddress.v + 8, ulaTStamp.eui64.v, sizeof(ulaTStamp.eui64.Val));

            break;

        default:
            // shouldn't happen
            TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
            ulaState = TCPIP_IPV6_ULA_IDLE;
            break; 
    }

    if(ulaState == TCPIP_IPV6_ULA_IDLE)
    {   // nothing to do
        return;
    }

    if(genRes == IPV6_ULA_RES_OK)
    {
        TCPIP_IPV6_ClientsNotify(ulaNetIf, IPV6_EVENT_ULA_ADDRESS_GENERATED, &ulaAddress);

        if((ulaFlags & (uint8_t)IPV6_ULA_FLAG_GENERATE_ONLY) == 0U)
        {
            (void)TCPIP_IPV6_UnicastAddressAdd(ulaNetIf, &ulaAddress, 0U, (ulaFlags & (uint8_t)IPV6_ULA_FLAG_SKIP_DAD));
        }
    }
    else
    {
        TCPIP_IPV6_ClientsNotify(ulaNetIf, IPV6_EVENT_ULA_ADDRESS_FAILED, (const void*)genRes);
    }

    ulaState = TCPIP_IPV6_ULA_IDLE;
}


static void TCPIP_IPV6_EUI64(TCPIP_NET_IF* pNetIf, uint64_t* pRes)
{
    TCPIP_MAC_ADDR netAdd;
    union
    {
        uint8_t     b[8];
        uint64_t    ull;
    }sll;

    (void)memcpy(netAdd.v, TCPIP_STACK_NetAddressMac(pNetIf), sizeof(netAdd));

    sll.b[0] = netAdd.v[0] | 0x02U;
    sll.b[1] = netAdd.v[1];
    sll.b[2] = netAdd.v[2];
    sll.b[3] = 0xffU;
    sll.b[4] = 0xfeU;
    sll.b[5] = netAdd.v[3];
    sll.b[6] = netAdd.v[4];
    sll.b[7] = netAdd.v[5];

    *pRes = sll.ull;

}
#else
IPV6_ULA_RESULT TCPIP_IPV6_UniqueLocalUnicastAddressAdd (TCPIP_NET_HANDLE netH, uint16_t subnetID, IPV6_ULA_FLAGS genFlags, IP_MULTI_ADDRESS* ntpAddress)
{
    return IPV6_ULA_RES_NOT_SUPPORTED;
}
#endif  // (TCPIP_IPV6_ULA_GENERATE_ENABLE != 0)

bool TCPIP_IPV6_RouterAddressAdd(TCPIP_NET_HANDLE netH, const IPV6_ADDR * rAddress, unsigned long validTime, int flags)
{
    TCPIP_NET_IF * pNetIf;

    if(rAddress == NULL || !TCPIP_IPV6_InterfaceIsReady(netH))
    {
        return false;
    }

    pNetIf = TCPIPStackHandleToNet(netH);


    if(validTime == 0U)
    {
        validTime = 0xffffffffU;
    }
    return TCPIP_IPV6_NewRouterEntry(pNetIf, rAddress, NULL, TCPIP_IPV6_NEIGHBOR_FLAG_NONE, validTime) == TCPIP_IPV6_RES_OK;
}

static TCPIP_IPV6_RESULT TCPIP_IPV6_NewRouterEntry(const TCPIP_NET_IF* pNetIf, const IPV6_ADDR* pGwAddr, const TCPIP_MAC_ADDR* pGMacAddr, TCPIP_IPV6_NEIGHBOR_FLAGS flags, unsigned long validTime)
{
    IPV6_HEAP_NDP_NC_ENTRY* pGwNbor;
    IPV6_HEAP_NDP_DR_ENTRY* pGwEntry;

    bool newGwNbor = false;     // false if already there, true if newly allocated

    TCPIP_IPV6_RESULT result = TCPIP_IPV6_RES_OK;

    if(pGwAddr == NULL)
    {
        return TCPIP_IPV6_RES_BAD_ARG;
    }   

    bool isPerm = ((uint16_t)flags & (uint16_t)TCPIP_IPV6_NEIGHBOR_FLAG_PERM) != 0U; 
    NEIGHBOR_UNREACHABILITY_DETECT_STATE nState = isPerm ? NDP_STATE_REACHABLE : NDP_STATE_INCOMPLETE;

    pGwNbor = NULL;
    pGwEntry = NULL;

    while(true)
    {
        pGwEntry = FC_VPtr2NdpEntry(TCPIP_NDP_RemoteNodeFind (pNetIf, pGwAddr, (uint8_t)IPV6_HEAP_NDP_DR_ID)).pDrEntry;

        if(pGwEntry != NULL)
        {   // already existent
            pGwNbor = pGwEntry->neighborInfo; 
            TCPIPStack_Assert(pGwNbor != NULL, __FILE__, __func__, __LINE__);
            if(pGwNbor == NULL)
            {
                result = TCPIP_IPV6_RES_INTERNAL_ERR;
            }
            break;
        }

        // non existent DR ENTRY ; need to add a new router entry
        // check if we have this neighbor
        pGwNbor = (IPV6_HEAP_NDP_NC_ENTRY *)TCPIP_NDP_RemoteNodeFind (pNetIf, pGwAddr, (uint8_t)IPV6_HEAP_NDP_NC_ID);

        if(pGwNbor == NULL)
        {   // new neighbor needed
            pGwNbor = TCPIP_NDP_NborEntryCreate (pNetIf, pGwAddr, pGMacAddr, (uint8_t)nState, 1U, NULL);
            newGwNbor = true;
        }

        if(pGwNbor == NULL)
        {   // failed
            result = TCPIP_IPV6_RES_ALLOC_ERR;
            break;
        }

        // create the GW DR entry
        pGwEntry = TCPIP_NDP_DefaultRouterEntryCreate(pNetIf, pGwNbor, validTime);
        if(pGwEntry == NULL)
        {   // failed
            result = TCPIP_IPV6_RES_ALLOC_ERR;
        }
        // done
        break;
    }

    if(result != TCPIP_IPV6_RES_OK)
    {   // failed
        if(newGwNbor == true && pGwNbor != NULL)
        {   // free created entry
            (void)TCPIP_NDP_NborEntryDelete(pNetIf, pGwNbor);
        }

        TCPIPStack_Assert(pGwEntry == NULL, __FILE__, __func__, __LINE__);
    }
    else
    {   // success
        // update the parameters...
        if(newGwNbor == false)
        {
            TCPIP_NDP_ReachabilitySet(pNetIf, pGwNbor, (uint8_t)nState);
            if(pGMacAddr == NULL)
            {
                (void)memset (pGwNbor->remoteMACAddr.v, 0x00, sizeof (TCPIP_MAC_ADDR));
            }
            else
            {
                (void)memcpy(pGwNbor->remoteMACAddr.v, pGMacAddr->v, sizeof (TCPIP_MAC_ADDR));
            }
        }
        pGwNbor->flags.bIsPerm = isPerm ? 1U : 0U;

        pGwEntry->invalidationTimer = validTime;
        pGwEntry->tickTimer = SYS_TMR_TickCountGet();

    }

    return result;
}

static uint16_t TCPIP_IPV6_PacketPayload(IPV6_PACKET* pkt)
{
    IPV6_DATA_SEGMENT_HEADER * segmentHeader;
    uint16_t pktLen = 0;

    // add the number of segments needed
    for(segmentHeader = &pkt->payload; segmentHeader != NULL; segmentHeader = segmentHeader->nextSegment)
    {
        pktLen += segmentHeader->segmentLen;
    }

    return pktLen;
}

// tries to allocate an associated TCPIP_MAC_PACKET for the IPV6_PACKET 
static TCPIP_MAC_PACKET* TCPIP_IPV6_MacPacketTxAllocate(IPV6_PACKET* pkt, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags)
{
    TCPIP_MAC_PACKET* pMacPkt;

    void * ptrSegment = TCPIP_IPV6_DataSegmentContentsGetByType (pkt, (uint8_t)TYPE_IPV6_HEADER);
    if (ptrSegment == NULL)
    {
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return NULL;
    }

    // allocate a packet with nSegs segments
    uint32_t uFlags = (uint32_t)flags;
    uFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_IPV6 | (uint32_t)TCPIP_MAC_PKT_FLAG_TX;
    pMacPkt = TCPIP_PKT_PacketAlloc((uint16_t)sizeof(*pMacPkt), segLoadLen, (TCPIP_MAC_PACKET_FLAGS)uFlags);

    if(pMacPkt != NULL)
    {
        TCPIP_MAC_PACKET_ACK_FUNC macAckFnc = (pkt->macAckFnc != NULL) ? pkt->macAckFnc : &TCPIP_IPV6_MacPacketTxAck;
        TCPIP_PKT_PacketAcknowledgeSet(pMacPkt, macAckFnc, pkt->ack6Param);

        IPV6_HEADER* pHdr = FC_VPtr2IPv6Hdr(ptrSegment);
        IPV6_VTF vtf;
        vtf.val = TCPIP_Helper_ntohl(pHdr->V_T_F);
        uint8_t dscp = vtf.ds;
        pMacPkt->pktPriority = TCPIP_IPV6_TxPriQueue(pkt->netIfH, dscp);
    }

    return pMacPkt;
}


// adds contiguous IPV6_PACKET segments to a TCPIP_MAC_PACKET 
static void TCPIP_IPV6_MacPacketTxAddSegments(IPV6_PACKET* ptrPacket, TCPIP_MAC_PACKET* pMacPkt, uint16_t segFlags)
{
    IPV6_DATA_SEGMENT_HEADER * segmentHeader;
    TCPIP_MAC_DATA_SEGMENT  *pSeg;
    uint8_t*                pDest;
    uint16_t                destLen;

    // add the number of segments needed
    pSeg = pMacPkt->pDSeg;
    pDest = pMacPkt->pNetLayer;
    destLen = 0;
    for(segmentHeader = &ptrPacket->payload; segmentHeader != NULL; segmentHeader = segmentHeader->nextSegment)
    {
        (void)memcpy(pDest, segmentHeader->dataLocation, segmentHeader->segmentLen);
        destLen += segmentHeader->segmentLen;
        pDest += segmentHeader->segmentLen;
    }


    pSeg->segLen += destLen;
    if(segFlags != 0U)
    {
        pSeg->segFlags |= segFlags;
    }
    
}


// TX packet acknowledge function
static void TCPIP_IPV6_MacPacketTxAck(TCPIP_MAC_PACKET* pMacPkt,  const void* param)
{
    TCPIP_PKT_PacketFree(pMacPkt);
}

static void TCPIP_IPV6_MacPacketTxPutHeader(IPV6_PACKET* pkt, TCPIP_MAC_PACKET* pMacPkt, uint16_t pktType)
{
    TCPIP_MAC_ETHERNET_HEADER* pMacHdr = FC_Uptr2MacEthHdr(pMacPkt->pMacLayer);

    (void)memcpy(pMacHdr->DestMACAddr.v, pkt->remoteMACAddr.v, sizeof(TCPIP_MAC_ADDR));
    (void)memcpy(pMacHdr->SourceMACAddr.v, ((const TCPIP_NET_IF*)pkt->netIfH)->netMACAddr.v, sizeof(TCPIP_MAC_ADDR));
    pMacHdr->Type = TCPIP_Helper_htons(pktType);

    // the 1st segment contains the MAC header
    pMacPkt->pDSeg->segLen += (uint16_t)sizeof(TCPIP_MAC_ETHERNET_HEADER);

}

// RX MAC packet related functions
// pMacLayer is BaseReadPtr
// pNetLayer is currRdPtr for MACGetArray

static void MACSetReadPtrInRx(TCPIP_MAC_PACKET* pRxPkt, uint16_t offset)
{
    uint16_t ethHdrSize;

#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
    if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_RX_TAGGED) != 0U)
    {
        ethHdrSize = sizeof(TCPIP_MAC_ETHERNET_VLAN_HEADER);
    }
    else
#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
    {
        ethHdrSize = sizeof(TCPIP_MAC_ETHERNET_HEADER);
    }

    pRxPkt->pNetLayer = pRxPkt->pMacLayer + ethHdrSize + offset;
}

static TCPIP_MAC_PTR_TYPE MACSetReadPtr(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PTR_TYPE address)
{
    unsigned char* oldPtr;

    oldPtr = pRxPkt->pNetLayer;
    pRxPkt->pNetLayer = (unsigned char*)address;
    return (TCPIP_MAC_PTR_TYPE)oldPtr;
}

static uint16_t MACGetArray(TCPIP_MAC_PACKET* pRxPkt, uint8_t *address, uint16_t len)
{
    if(address != NULL)
    {
        (void)memcpy(address, pRxPkt->pNetLayer, len);
    }

    pRxPkt->pNetLayer += len;
    pRxPkt->pTransportLayer = pRxPkt->pNetLayer;    // update transport layer pointer

    return len;
}


static TCPIP_MAC_PTR_TYPE MACGetReadPtrInRx(TCPIP_MAC_PACKET* pRxPkt)
{
    return (TCPIP_MAC_PTR_TYPE)pRxPkt->pNetLayer;
}

uint8_t TCPIP_IPV6_ArrayGet (TCPIP_MAC_PACKET* pRxPkt, uint8_t *val, uint16_t len)
{
    uint16_t getBytes = MACGetArray(pRxPkt, val, len);
    TCPIPStack_Assert(getBytes < 256U, __FILE__, __func__, __LINE__);
    return (uint8_t)getBytes;
}

uint8_t TCPIP_IPV6_Get (TCPIP_MAC_PACKET* pRxPkt, uint8_t* pData)
{
    uint16_t getBytes = MACGetArray(pRxPkt, pData, 1U);
    return (uint8_t)getBytes;
}

void TCPIP_IPV6_DiscardRx(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_PKT_PacketAcknowledge(pRxPkt, ackRes);
}

void* TCPIP_IPV6_GetReadPtrInRx(TCPIP_MAC_PACKET* pRxPkt)
{
    return MACGetReadPtrInRx(pRxPkt);
}


void* TCPIP_IPV6_SetReadPtr(TCPIP_MAC_PACKET* pRxPkt, void* address)
{
    return MACSetReadPtr(pRxPkt, address);
}

const IPV6_ADDR* TCPIP_IPV6_PacketGetDestAddress(const TCPIP_MAC_PACKET* pRxPkt)
{
    const IPV6_HEADER* pHdr = FC_CuPtr2IPv6Hdr(pRxPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER));
    return &pHdr->DestAddress;
}

const IPV6_ADDR* TCPIP_IPV6_PacketGetSourceAddress(const TCPIP_MAC_PACKET* pRxPkt)
{
    const IPV6_HEADER* pHdr = FC_CuPtr2IPv6Hdr(pRxPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER));
    return &pHdr->SourceAddress;
}


void TCPIP_IPV6_SetRemoteMacAddress(IPV6_PACKET * ptrPacket, const TCPIP_MAC_ADDR* pMacAdd)
{
    (void)memcpy (ptrPacket->remoteMACAddr.v, pMacAdd->v, sizeof (TCPIP_MAC_ADDR));
}

void TCPIP_IPV6_SetPacketMacAcknowledge(IPV6_PACKET * ptrPacket, TCPIP_MAC_PACKET_ACK_FUNC macAckFnc)
{
    ptrPacket->macAckFnc = macAckFnc;
}

void TCPIP_IPV6_PacketAck(IPV6_PACKET * ptrPacket, bool success)
{
    if (ptrPacket->ack6Fnc != NULL)
    {
        if(success != true)
        {   // for failure, force the neighbor to be re-evaluated
            ptrPacket->neighbor = NULL;
        }
        ptrPacket->ack6Fnc(ptrPacket, success, ptrPacket->ack6Param);
    }
}

uint16_t TCPIP_IPV6_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pIf = TCPIPStackHandleToNetUp(netH);

    if(pIf != NULL)
    {
        IPV6_INTERFACE_CONFIG*  pIpv6Config = ipv6Config + TCPIP_STACK_NetIxGet(pIf);
        return pIpv6Config->linkMTU - (uint16_t)sizeof(IPV6_HEADER);
    }

    return 0U;
}

// external packet processing
#if (TCPIP_IPV6_EXTERN_PACKET_PROCESS != 0)
TCPIP_IPV6_PROCESS_HANDLE TCPIP_IPV6_PacketHandlerRegister(TCPIP_IPV6_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    TCPIP_IPV6_PROCESS_HANDLE pHandle = NULL;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(ipv6PktHandler == NULL)
    {
        ipv6PktHandlerParam = handlerParam;
        ipv6PktHandler = pktHandler;
        pHandle = FC_PktHndl2ProcHndl(pktHandler);
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return pHandle;
}

bool TCPIP_IPV6_PacketHandlerDeregister(TCPIP_IPV6_PROCESS_HANDLE pktHandle)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(FC_PktHndl2ProcHndl(ipv6PktHandler) == pktHandle)
    {
        ipv6PktHandler = NULL;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

#else
TCPIP_IPV6_PROCESS_HANDLE TCPIP_IPV6_PacketHandlerRegister(TCPIP_IPV6_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    return NULL;
}
bool TCPIP_IPV6_PacketHandlerDeregister(TCPIP_IPV6_PROCESS_HANDLE pktHandle)
{
    return false;
}
#endif  // (TCPIP_IPV6_EXTERN_PACKET_PROCESS != 0)

TCPIP_IPV6_RESULT TCPIP_IPV6_NeighborAddressAdd(TCPIP_NET_HANDLE netH, const IPV6_ADDR * nAddress, const TCPIP_MAC_ADDR* nMacAddr, TCPIP_IPV6_NEIGHBOR_FLAGS flags)
{
    if(nAddress == NULL || nMacAddr == NULL)
    {
        return TCPIP_IPV6_RES_BAD_ARG;
    }

    TCPIP_NET_IF* pNetIf =  TCPIPStackHandleToNetUp(netH);

    if(pNetIf == NULL)
    {
        return TCPIP_IPV6_RES_IF_ERR;
    }

    if (nAddress->v[0] == 0xFFU)
    {   // Use on-link multicast
        return TCPIP_IPV6_RES_MCAST_ERR;
    }

    if(((uint8_t)flags & (uint8_t)TCPIP_IPV6_NEIGHBOR_FLAG_ROUTER) != 0U) 
    {   // router added
        return TCPIP_IPV6_NewRouterEntry(pNetIf, nAddress, nMacAddr, flags, 0xffffffffU);
    }

    // new neighbor state
    bool isPerm = ((uint8_t)flags & (uint8_t)TCPIP_IPV6_NEIGHBOR_FLAG_PERM) != 0U; 
    NEIGHBOR_UNREACHABILITY_DETECT_STATE nState = isPerm ? NDP_STATE_REACHABLE : NDP_STATE_INCOMPLETE;
    IPV6_INTERFACE_CONFIG* pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);


    IPV6_HEAP_NDP_DC_ENTRY* dcEntry = NULL;
    IPV6_HEAP_NDP_NC_ENTRY* ncEntry = NULL;

    bool newNcEntry = false;     // false if already there, true if newly allocated


    TCPIP_IPV6_RESULT result = TCPIP_IPV6_RES_OK;

    while(true)
    {        
        // check if we have this destination
        dcEntry = FC_VPtr2NdpEntry(TCPIP_NDP_RemoteNodeFind (pNetIf, nAddress, (uint8_t)IPV6_HEAP_NDP_DC_ID)).pDcEntry;
        if(dcEntry != NULL)
        {   // already existent
            ncEntry = dcEntry->nextHopNeighbor;
            TCPIPStack_Assert(ncEntry != NULL, __FILE__, __func__, __LINE__);
            if(ncEntry == NULL)
            {
                result = TCPIP_IPV6_RES_INTERNAL_ERR;
            }
            break;
        }

        // non-existent DC entry
        // check if we have the neighbor
        ncEntry = (IPV6_HEAP_NDP_NC_ENTRY *)TCPIP_NDP_RemoteNodeFind (pNetIf, nAddress, (uint8_t)IPV6_HEAP_NDP_NC_ID);

        if(ncEntry == NULL)
        {   // new neighbor needed
            ncEntry = TCPIP_NDP_NborEntryCreate (pNetIf, nAddress, nMacAddr, (uint8_t)nState, 0U, NULL);
            newNcEntry = true;
        }

        if(ncEntry == NULL)
        {   // failed
            result = TCPIP_IPV6_RES_ALLOC_ERR;
            break;
        }

        // create the DC entry
        dcEntry = TCPIP_NDP_DestCacheEntryCreate (pNetIf, nAddress, pIpv6Config->linkMTU, ncEntry);
        if(dcEntry == NULL)
        {   // failed
            result = TCPIP_IPV6_RES_ALLOC_ERR;
        }
        break;
    }


    if(result != TCPIP_IPV6_RES_OK)
    {   // failed
        if(newNcEntry == true && ncEntry != NULL)
        {
            (void)TCPIP_NDP_NborEntryDelete(pNetIf, ncEntry);
        }

        TCPIPStack_Assert(dcEntry == NULL, __FILE__, __func__, __LINE__);
    }
    else
    {   // success
        // set/update the parameters
        if(newNcEntry == false)
        {
            TCPIP_NDP_ReachabilitySet(pNetIf, ncEntry, (uint8_t)nState);
            (void)memcpy(ncEntry->remoteMACAddr.v, nMacAddr->v, sizeof (TCPIP_MAC_ADDR));
        }

        ncEntry->flags.bIsPerm = isPerm ? 1U : 0U;
    }

    return result;
}

TCPIP_IPV6_RESULT TCPIP_IPV6_NeighborAddressDelete(TCPIP_NET_HANDLE netH, const IPV6_ADDR * nAddress)
{
    if(nAddress == NULL)
    {
        return TCPIP_IPV6_RES_BAD_ARG;
    }

    TCPIP_NET_IF* pNetIf =  TCPIPStackHandleToNetUp(netH);

    if(pNetIf == NULL)
    {
        return TCPIP_IPV6_RES_IF_ERR;
    }

    // check if we have the neighbor
    IPV6_HEAP_NDP_NC_ENTRY* ncEntry = (IPV6_HEAP_NDP_NC_ENTRY *)TCPIP_NDP_RemoteNodeFind (pNetIf, nAddress, (uint8_t)IPV6_HEAP_NDP_NC_ID);
    if(ncEntry == NULL)
    {
        return TCPIP_IPV6_RES_ADDRESS_ERR;
    }
    
    // valid neighbor


    (void)TCPIP_NDP_NborEntryDelete (pNetIf, ncEntry);

    return TCPIP_IPV6_RES_OK;
}

#if defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)
TCPIP_IPV6_RESULT TCPIP_IPV6_G3PLC_PanIdSet(TCPIP_NET_HANDLE netH, uint16_t panId)
{
    TCPIP_NET_IF* pNetIf =  TCPIPStackHandleToNetUp(netH);

    if(pNetIf == NULL)
    {
        return TCPIP_IPV6_RES_IF_ERR;
    }

    if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_G3_NET) == 0U)
    {
        return TCPIP_IPV6_RES_BAD_IF;
    }

    IPV6_INTERFACE_CONFIG*  pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);
    pIpv6Config->g3PanId = panId;
    pIpv6Config->g3PanIdSet = 1;

    return TCPIP_IPV6_RES_OK; 
}
#else
TCPIP_IPV6_RESULT TCPIP_IPV6_G3PLC_PanIdSet(TCPIP_NET_HANDLE netH, uint16_t panId)
{
    return TCPIP_IPV6_RES_OPER_ERR;
}

#endif  // defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)

// calculates a TX packet priority queue based on the network interface
// dscp value should be the IPv6 header IPV6_VTF::ds
static uint8_t TCPIP_IPV6_TxPriQueue(const TCPIP_NET_IF* pNetIf, uint8_t dscp)
{
    if(ipv6PriPktHandler != NULL)
    {
        return ipv6PriPktHandler((TCPIP_NET_HANDLE)pNetIf, dscp);
    }

    // default calculation
    uint16_t qNo = (uint16_t)TCPIPStack_TxPriNum(pNetIf);   // number of the MAC supported queues
    if(dscp > (uint8_t)TCPIP_IPV6_DSCP_MAX)
    {   // avoid overflow
        dscp = (uint8_t)TCPIP_IPV6_DSCP_MAX;
    }

    uint16_t pri16 = ((uint16_t)dscp * qNo) / (uint16_t)TCPIP_IPV6_DSCP_MAX; 
    return pri16 == 0U ? 0U : (uint8_t)pri16 - 1U;
}

bool TCPIP_IPV6_TxPriHandlerRegister(TCPIP_IPV6_TX_PRI_HANDLER priHandler)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(ipv6PriPktHandler == NULL)
    {
        ipv6PriPktHandler = priHandler;
        res = true;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

bool TCPIP_IPV6_TxPriHandlerDeregister(TCPIP_IPV6_TX_PRI_HANDLER priHandler)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(ipv6PriPktHandler == priHandler)
    {
        ipv6PriPktHandler = NULL;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

#endif  // defined(TCPIP_STACK_USE_IPV6)


