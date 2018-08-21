/*******************************************************************************
  Internet Protocol (IP) Version 6 Communications Layer

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    -Provides a transport for TCP, UDP, and ICMP messages
    -Reference: RFC
*******************************************************************************/

/*******************************************************************************
File Name:  ipv6.c
Copyright © 2012 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_IPV6

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/ipv6_private.h"
#include "crypto/crypto.h"

#if defined(TCPIP_STACK_USE_IPV6)


// This is left shifted by 4.  Actual value is 0x04.
#define IPv4_VERSION        (0x40u)
// This is left shifted by 4.  Actual value is 0x06.
#define IPv6_VERSION        (0x60u)

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

static uint32_t             fragmentId = 0;             // Static ID to use when sending fragmented packets

static int                  nStackIfs = 0;              // number of interfaces the stack is currently running on

static PROTECTED_SINGLE_LIST          ipv6RegisteredUsers = { {0} };

static PROTECTED_SINGLE_LIST          mcastQueue = { {0} };

static int                  ipv6ModuleInitCount = 0;      // Indicator of how many times the IP module has been initialized


static tcpipSignalHandle     ipv6TaskHandle = 0;          // Handle for the IPv6 task

static uint32_t             ipv6StartTick = 0;

static const void*          ipv6MemH = 0;                     // memory handle

static PROTECTED_SINGLE_LIST        ipv6QueuedPackets = { {0} };

// Enumeration defining IPv6 initialization states
enum
{
    IPV6_INIT_STATE_NONE = 0,                           // IPv6 initialization is not in progress
    IPV6_INIT_STATE_INITIALIZE,                         // Initializes IPv6 variables
    IPV6_INIT_STATE_DAD,                                // Duplicate address detection is being performed on the interfaces link-local unicast address
    IPV6_INIT_STATE_SOLICIT_ROUTER,                     // The interface is soliciting routers
    IPV6_INIT_STATE_DONE,                               // The interface is up
    IPV6_INIT_STATE_FAIL                                // The initialization has failed
} IPV6_INIT_STATE;


// IPv6 address policy table for default address selection
const IPV6_ADDRESS_POLICY gPolicyTable[] = {
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}, 128, 50,  0},          // Loopback address
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},   0, 40,  1},            // Unspecified address
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00}},  96, 35,  4},           // IPv4-mapped address
    {{{0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  16, 30,  2},           // 2002::/15 - 6to4
    {{{0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  32,  5,  5},            // 2001::/32 - Teredo tunneling
    {{{0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},   7,  3, 13},            // ULA
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  96,  1,  3},            //
    {{{0xfe, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  10,  1, 11},            // 
    {{{0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  16,  1, 12},            // 

    {{},0xFF,0,0},
};

// Array of global configuration and state variables for an IPv6 interface
static IPV6_INTERFACE_CONFIG* ipv6Config = 0;

// ULA state machine
#if defined (TCPIP_STACK_USE_SNTP_CLIENT)
static TCPIP_IPV6_ULA_STATE ulaState = TCPIP_IPV6_ULA_IDLE;
static TCPIP_NET_IF*   ulaNetIf = 0;
static uint16_t        ulaSubnetId;  // subnet to generate for
static IPV6_ULA_FLAGS  ulaFlags;
static uint32_t        ulaOperStartTick;


#endif  // defined (TCPIP_STACK_USE_SNTP_CLIENT)


/************************************************************************/
/****************               Prototypes               ****************/
/************************************************************************/

// Free all of the dynamically linked lists in an IPv6 net configuration
void TCPIP_IPV6_FreeConfigLists (IPV6_INTERFACE_CONFIG * pNetIf);

// performs resources cleanup
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _TCPIP_IPV6_Cleanup(const void* memH);
static void TCPIP_IPV6_ProtectedSingleListFree (PROTECTED_SINGLE_LIST * list);
#else
#define _TCPIP_IPV6_Cleanup(memH)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void _TCPIP_IPV6_QueuedPacketTransmitTask (PROTECTED_SINGLE_LIST* pList);

static void _TCPIP_IPV6_PacketEnqueue(IPV6_PACKET * pkt, SINGLE_LIST* pList, int queueLimit, int tmoSeconds, bool isProtected);

#if defined (TCPIP_STACK_USE_SNTP_CLIENT)

static void TCPIP_IPV6_UlaTask (void);
static void TCPIP_IPV6_EUI64(TCPIP_NET_IF* pNetIf, uint64_t* pRes);

#endif  // defined (TCPIP_STACK_USE_SNTP_CLIENT)

// MAC API TX functions
static uint16_t             TCPIP_IPV6_PacketPayload(IPV6_PACKET* pkt);
static TCPIP_MAC_PACKET*    TCPIP_IPV6_MacPacketTxAllocate(IPV6_PACKET* pkt, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags);
static void                 TCPIP_IPV6_MacPacketTxPutHeader(IPV6_PACKET* pkt, TCPIP_MAC_PACKET* pMacPkt, uint16_t pktType);
static bool                 TCPIP_IPV6_MacPacketTxAck(TCPIP_MAC_PACKET* pkt,  const void* param);
static void                 TCPIP_IPV6_MacPacketTxAddSegments(IPV6_PACKET* ptrPacket, TCPIP_MAC_PACKET* pMacPkt, uint16_t segFlags);

// MAC API RX functions
typedef uint8_t*    TCPIP_MAC_PTR_TYPE; 
static TCPIP_MAC_PTR_TYPE   MACSetBaseReadPtr(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PTR_TYPE address);
static void                 MACSetReadPtrInRx(TCPIP_MAC_PACKET* pRxPkt, uint16_t offset);
static TCPIP_MAC_PTR_TYPE   MACSetReadPtr(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PTR_TYPE address);
static uint16_t             MACGetArray(TCPIP_MAC_PACKET* pRxPkt, uint8_t *address, uint16_t len);
static TCPIP_MAC_PTR_TYPE   MACGetReadPtrInRx(TCPIP_MAC_PACKET* pRxPkt);

static IPV6_HEAP_NDP_DR_ENTRY* TCPIP_IPV6_NewRouterEntry(TCPIP_NET_IF* pNetIf, IPV6_ADDR* pGatewayAddr, unsigned long validTime);

static void TCPIP_IPV6_InitializeTask (void);

static void TCPIP_IPV6_Timeout (void);

static void TCPIP_IPV6_ProcessPackets(void);

static void TCPIP_IPV6_Process (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt);


// ipv6_manager.h
bool TCPIP_IPV6_Initialize(const TCPIP_STACK_MODULE_CTRL* const pStackInit, const TCPIP_IPV6_MODULE_CONFIG* pIpv6Init)
{
    TCPIP_NET_IF* pNetIf;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    bool    iniRes;

    // if(pStackInit->stackAction == TCPIP_STACK_ACTION_INIT)   // stack going up
    // if(pStackInit->stackAction == TCPIP_STACK_ACTION_IF_UP)  // interface is going up

    if(pStackInit->stackAction == TCPIP_STACK_ACTION_INIT)
    {   // stack going up
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
            if(ipv6Config == 0)
            {   // failed
                return false;
            }

            while(true)
            {
                iniRes = (ipv6TaskHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_IPV6_Task, TCPIP_IPV6_INIT_TASK_PROCESS_RATE)) != 0;

                if(iniRes == false)
                {
                    break;
                }

                if((iniRes = TCPIP_Notification_Initialize(&ipv6RegisteredUsers)) == false)
                {
                    break;
                }

                if((iniRes = TCPIP_Helper_ProtectedSingleListInitialize (&mcastQueue)) == false)
                {
                    break;
                }

                iniRes = TCPIP_Helper_ProtectedSingleListInitialize (&ipv6QueuedPackets);

                break;
            }

            if(iniRes == false)
            {
                _TCPIP_IPV6_Cleanup(pStackInit->memH);
                return false;
            }

            ipv6StartTick = 0;

            // initialize IPv6 configuration parameter
            pIpv6Config = ipv6Config + pStackInit->netIx;
            pIpv6Config->rxfragmentBufSize = pIpv6Init->rxfragmentBufSize;
            pIpv6Config->fragmentPktRxTimeout = pIpv6Init->fragmentPktRxTimeout;
        }

        ipv6ModuleInitCount++;
    }


    // init this interface
    pNetIf = pStackInit->pNetIf;

    // Check to see if this interface is already being initialized
    if (pNetIf->Flags.bIPv6InConfig == false)
    {
        pNetIf->Flags.bIPv6InConfig = true;
        pIpv6Config = ipv6Config + pStackInit->netIx;

        // Initialize the IPv6 parameters for this net
        pIpv6Config->initState = IPV6_INIT_STATE_INITIALIZE;
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

        pIpv6Config->policyPreferTempOrPublic = IPV6_PREFER_PUBLIC_ADDRESSES;
        
        ipv6InitCount++;
    }

    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _TCPIP_IPV6_Cleanup(const void* memH)
{
    TCPIP_Notification_Deinitialize(&ipv6RegisteredUsers, ipv6MemH);
    if(ipv6TaskHandle)
    {
        _TCPIPStackSignalHandlerDeregister(ipv6TaskHandle);
        ipv6TaskHandle = 0;
    }

    if(ipv6Config)
    {
        TCPIP_HEAP_Free(memH, ipv6Config);
        ipv6Config = 0;
    }

    // kill the multicast queue
    TCPIP_IPV6_ProtectedSingleListFree(&mcastQueue);

    // kill the queued packets
    TCPIP_IPV6_ProtectedSingleListFree(&ipv6QueuedPackets);

    ipv6MemH = 0;
}

static void TCPIP_IPV6_ProtectedSingleListFree (PROTECTED_SINGLE_LIST * list)
{
    SGL_LIST_NODE * node;
    while ((node = TCPIP_Helper_ProtectedSingleListHeadRemove(list)))
    {
        TCPIP_HEAP_Free (ipv6MemH, node);
    }
    TCPIP_Helper_ProtectedSingleListDeinitialize (list);

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

    if(ipv6ModuleInitCount)
    {
        pNetIf = stackCtrl->pNetIf;
        TCPIP_IPV6_InitializeStop (pNetIf);
        TCPIP_IPV6_FreeConfigLists (ipv6Config + stackCtrl->netIx);
        pNetIf->Flags.bIPv6Enabled = false;

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down
            if(--ipv6ModuleInitCount == 0)
            {
                _TCPIP_IPV6_Cleanup(stackCtrl->memH);
                nStackIfs = 0;
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


// ipv6_manager.h
void TCPIP_IPV6_InitializeStop (TCPIP_NET_IF * pNetIf)
{
    if(ipv6InitCount)
    {   // We're up and running
        if (pNetIf->Flags.bIPv6InConfig == true)
        {
            pNetIf->Flags.bIPv6InConfig = false;
            if (--ipv6InitCount == 0)
            {   // stop the timing for the init task!
                _TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, ipv6TaskHandle, TCPIP_IPV6_TASK_PROCESS_RATE);
            }
        }
    }
}


// ipv6_manager.h
static void TCPIP_IPV6_InitializeTask (void)
{
    IPV6_ADDR_STRUCT * localAddressPointer;
    IPV6_ADDR linkLocalAddress = {{0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0 | 0x02, 0, 0, 0xFF, 0xFE, 0, 0, 0}};
    int netIx;
    TCPIP_NET_IF * pNetIf;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    IPV6_ADDR* pStatAddr, *pGatewayAddr;
    int statPrefixLen;

    for (netIx = 0; netIx < TCPIP_STACK_NumberOfNetworksGet(); netIx++)
    {
        pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet (netIx);
        pIpv6Config = ipv6Config + netIx;

        if ((pNetIf->Flags.bIPv6InConfig == true) && (TCPIP_STACK_NetworkIsLinked(pNetIf)))
        {
            linkLocalAddress.v[8] = pNetIf->netMACAddr.v[0] | 0x02;
            linkLocalAddress.v[9] = pNetIf->netMACAddr.v[1];
            linkLocalAddress.v[10] = pNetIf->netMACAddr.v[2];
            linkLocalAddress.v[13] = pNetIf->netMACAddr.v[3];
            linkLocalAddress.v[14] = pNetIf->netMACAddr.v[4];
            linkLocalAddress.v[15] = pNetIf->netMACAddr.v[5];

            switch (pIpv6Config->initState)
            {
                case IPV6_INIT_STATE_INITIALIZE:
                    // Add the all-nodes multicast listener to this node
                    localAddressPointer = TCPIP_IPV6_MulticastListenerAdd (pNetIf, (IPV6_ADDR *)&IPV6_FIXED_ADDR_ALL_NODES_MULTICAST);
                    if (localAddressPointer == NULL)
                    {
                        pIpv6Config->initState = IPV6_INIT_STATE_FAIL;
                        break;
                    }
                    // Configure link-local address
                    localAddressPointer = TCPIP_IPV6_UnicastAddressAdd (pNetIf, &linkLocalAddress, 0, false);
                    if (localAddressPointer == NULL)
                    {
                        pIpv6Config->initState = IPV6_INIT_STATE_FAIL;
                        break;
                    }

                    // Configure static IPv6 address
                    pStatAddr = (IPV6_ADDR*)TCPIP_STACK_NetStaticIPv6AddressGet(pNetIf, &statPrefixLen);
                    if(pStatAddr != 0)
                    {
                        localAddressPointer = TCPIP_IPV6_UnicastAddressAdd (pNetIf, pStatAddr, statPrefixLen, false);
                        if(localAddressPointer == 0 )
                        {
                            pIpv6Config->initState = IPV6_INIT_STATE_FAIL;
                            break;
                        }
                    }
                    
                    // Enable IPv6 functionality for now so we can process ICMPv6 messages for stateless address autoconfiguration
                    pNetIf->Flags.bIPv6Enabled = true;
                    pIpv6Config->initState = IPV6_INIT_STATE_DAD;
                    break;
                case IPV6_INIT_STATE_DAD:
                    if (TCPIP_IPV6_AddressFind (pNetIf, &linkLocalAddress, IPV6_ADDR_TYPE_UNICAST))
                    {
                        pGatewayAddr = (IPV6_ADDR*)TCPIP_STACK_NetDefaultIPv6GatewayGet(pNetIf);
                        if(pGatewayAddr)
                        {   // add a new router entry, valid forever
                            if(TCPIP_IPV6_NewRouterEntry(pNetIf, pGatewayAddr, 0xffffffff) == 0)
                            {
                                pIpv6Config->initState = IPV6_INIT_STATE_FAIL;
                                break;
                            }
                        }

                        pIpv6Config->initState = IPV6_INIT_STATE_SOLICIT_ROUTER;
                    }
                    else if (TCPIP_IPV6_AddressFind (pNetIf, &linkLocalAddress, IPV6_ADDR_TYPE_UNICAST_TENTATIVE) == NULL)
                    {
                        pIpv6Config->initState = IPV6_INIT_STATE_FAIL;
                    }
                    break;
                case IPV6_INIT_STATE_SOLICIT_ROUTER:
                    TCPIP_NDP_RouterSolicitStart(pNetIf);
                    pIpv6Config->initState = IPV6_INIT_STATE_DONE;
                case IPV6_INIT_STATE_DONE:
                    TCPIP_IPV6_InitializeStop (pNetIf);
                    break;
                case IPV6_INIT_STATE_FAIL:
                    TCPIP_IPV6_FreeConfigLists (pIpv6Config);
                    TCPIP_IPV6_InitializeStop (pNetIf);
                    pNetIf->Flags.bIPv6Enabled = 0;
                    break;
                default:
                    break;
            }
        }
    }
}


// ipv6.h
bool TCPIP_IPV6_InterfaceIsReady (TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF * pNetIf =  _TCPIPStackHandleToNetUp(netH);

    if(pNetIf != 0)
    {
        if (!pNetIf->Flags.bIPv6InConfig && pNetIf->Flags.bIPv6Enabled)
        {
            return true;
        }
    }

    return false;
}


// ipv6.h
IPV6_PACKET * TCPIP_IPV6_TxPacketAllocate (TCPIP_NET_HANDLE netH, IPV6_PACKET_ACK_FNC ackFnc, void* ackParam)
{
    TCPIP_NET_IF * pNetIf =  _TCPIPStackHandleToNet(netH);
    IPV6_PACKET * ptrPacket = (IPV6_PACKET *)TCPIP_HEAP_Malloc (ipv6MemH, sizeof (IPV6_PACKET));

    if (ptrPacket == NULL)
        return NULL;

    ptrPacket->next = 0;
    ptrPacket->netIfH = pNetIf;
    ptrPacket->flags.val = 0;
    ptrPacket->upperLayerChecksumOffset = IPV6_NO_UPPER_LAYER_CHECKSUM;

    ptrPacket->payload.dataLocation = (uint8_t*)&ptrPacket->ipv6Header;
    ptrPacket->payload.segmentSize = sizeof (IPV6_HEADER);
    ptrPacket->payload.segmentLen = sizeof (IPV6_HEADER);
    ptrPacket->offsetInSegment = 0;

    ptrPacket->payload.memory = IPV6_DATA_PIC_RAM;
    ptrPacket->payload.segmentType = TYPE_IPV6_HEADER;
    ptrPacket->payload.nextSegment = NULL;

    TCPIP_IPV6_PacketIPProtocolSet (ptrPacket);

    ptrPacket->headerLen = 0;
    ptrPacket->flags.queued = false;
    ptrPacket->flags.sourceSpecified = false;
    ptrPacket->flags.useUnspecAddr = false;

    ptrPacket->neighbor = NULL;

    ptrPacket->payloadLen = 0;
    ptrPacket->upperLayerHeaderLen = 0;
    memset (&ptrPacket->remoteMACAddr, 0x00, sizeof (TCPIP_MAC_ADDR));

    ptrPacket->ackFnc = ackFnc;
    ptrPacket->ackParam = ackParam;
    ptrPacket->macAckFnc = 0;

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
    if(destination->flags.addressType != source->flags.addressType || destination->flags.addressType != IP_ADDRESS_TYPE_IPV6)
    {
        return false;
    }

    memcpy ((void *)&destination->remoteMACAddr, (void *)&source->remoteMACAddr, sizeof (TCPIP_MAC_ADDR));
    destination->flags.useUnspecAddr = source->flags.useUnspecAddr;
    destination->flags.sourceSpecified = source->flags.sourceSpecified;
    destination->flags.addressType = IP_ADDRESS_TYPE_IPV6;


    memcpy ((void *)&destination->payload, (void *)&source->payload, sizeof (IPV6_DATA_SEGMENT_HEADER));

    destination->payload.nextSegment = NULL;

    destination->neighbor = source->neighbor;
    memcpy ((void *)&destination->ipv6Header, (void *)&source->ipv6Header, sizeof (IPV6_HEADER));
    destination->ipv6Header.PayloadLength = 0x0000;

    destination->payload.dataLocation = (uint8_t*)&destination->ipv6Header;

    if ((ptrSegmentSource = TCPIP_IPV6_DataSegmentGetByType (source, TYPE_IPV6_UPPER_LAYER_HEADER)) != NULL)
    {
        if ((ptrSegmentDest = TCPIP_IPV6_DataSegmentGetByType (destination, ptrSegmentSource->segmentType)) == NULL)
        {
            ptrSegmentDest = TCPIP_IPV6_DataSegmentHeaderAllocate (ptrSegmentSource->segmentSize);
            if (ptrSegmentDest == NULL)
                return false;
            ptrSegmentDest->nextSegment = NULL;
            TCPIP_IPV6_PacketSegmentInsert (ptrSegmentDest, destination, ptrSegmentSource->segmentType);
        }

        memcpy ((void *)ptrSegmentDest->dataLocation, (void *)ptrSegmentSource->dataLocation, ptrSegmentSource->segmentLen);
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

    ptrPacket->payload.segmentLen = sizeof (IPV6_HEADER);
    ptrPacket->flags.addressType = IP_ADDRESS_TYPE_IPV6;
}


// ipv6_manager.h
void TCPIP_IPV6_HeaderPut(IPV6_PACKET * ptrPacket, uint8_t protocol)
{
    if(ptrPacket->flags.addressType != IP_ADDRESS_TYPE_IPV6)
    {
        return;
    }

    void * ptrSegment = TCPIP_IPV6_DataSegmentContentsGetByType (ptrPacket, TYPE_IPV6_HEADER);

    if (ptrSegment == NULL)
        return;


    ((IPV6_HEADER *)ptrSegment)->V_T_F = (uint32_t)IPv6_VERSION;
    ((IPV6_HEADER *)ptrSegment)->HopLimit = 0;
}


// ipv6_manager.h
IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_UpperLayerHeaderPut (IPV6_PACKET * ptrPacket, void * header, unsigned short len, unsigned char type, unsigned short checksumOffset)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate(len);

    if (ptrSegment == NULL)
        return NULL;

    ptrSegment->segmentLen = len;

    if (header != NULL)
        memcpy (ptrSegment->data, header, len);

    ptrPacket->upperLayerHeaderLen = len;
    ptrPacket->upperLayerHeaderType = type;
    ptrPacket->upperLayerChecksumOffset = checksumOffset;

    TCPIP_IPV6_PacketSegmentInsert (ptrSegment, ptrPacket, TYPE_IPV6_UPPER_LAYER_HEADER);

    return ptrSegment;
}


// ipv6_private.h
void TCPIP_IPV6_PacketSegmentInsert (IPV6_DATA_SEGMENT_HEADER * ptrSegment, IPV6_PACKET * ptrPacket, IPV6_SEGMENT_TYPE type)
{
    IPV6_DATA_SEGMENT_HEADER * tempSegment;
    uint8_t * nextHeader;

    if ((ptrSegment == NULL) || (ptrPacket == NULL))
        return;

    tempSegment = &ptrPacket->payload;

    ptrSegment->segmentType = type;

    if (type == TYPE_IPV6_END_OF_LIST)
    {
        while (tempSegment->nextSegment != NULL)
            tempSegment = tempSegment->nextSegment;
    }
    else
    {
        while ((tempSegment->nextSegment != NULL) && (ptrSegment->segmentType > tempSegment->nextSegment->segmentType))
            tempSegment = tempSegment->nextSegment;
    }
    ptrSegment->nextSegment = tempSegment->nextSegment;
    tempSegment->nextSegment = ptrSegment;

    if (type == TYPE_IPV6_END_OF_LIST)
    {
        ptrSegment->segmentType = TYPE_IPV6_UPPER_LAYER_PAYLOAD;
    }

    if (tempSegment->segmentType < TYPE_IPV6_UPPER_LAYER_HEADER)
    {
        if (tempSegment == &ptrPacket->payload)
        {
            nextHeader = &ptrPacket->ipv6Header.NextHeader;
        }
        else
        {
            nextHeader = (uint8_t *)&tempSegment->data;
        }
        switch (ptrSegment->segmentType)
        {
            case TYPE_IPV6_EX_HEADER_HOP_BY_HOP_OPTIONS:
                *nextHeader = IPV6_PROT_HOP_BY_HOP_OPTIONS_HEADER;
                break;
            case TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_1:
            case TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_2:
                *nextHeader = IPV6_PROT_DESTINATION_OPTIONS_HEADER;
                break;
            case TYPE_IPV6_EX_HEADER_ROUTING:
                *nextHeader = IPV6_PROT_ROUTING_HEADER;
                break;
            case TYPE_IPV6_EX_HEADER_FRAGMENT:
                *nextHeader = IPV6_PROT_FRAGMENTATION_HEADER;
                break;
            case TYPE_IPV6_EX_HEADER_AUTHENTICATION_HEADER:
                *nextHeader = IPV6_PROT_AUTHENTICATION_HEADER;
                break;
            case TYPE_IPV6_EX_HEADER_ENCAPSULATING_SECURITY_PAYLOAD:
                *nextHeader = IPV6_PROT_ENCAPSULATING_SECURITY_PAYLOAD_HEADER;
                break;
            default:
                *nextHeader = ptrPacket->upperLayerHeaderType;
                break;
        }
    }

    if (ptrSegment->segmentType < TYPE_IPV6_UPPER_LAYER_HEADER)
    {
        nextHeader = (uint8_t *)&ptrSegment->data;
        if (ptrSegment->nextSegment == NULL)
        {
            *nextHeader = IPV6_PROT_NONE;
        }
        else
        {
            switch (ptrSegment->nextSegment->segmentType)
            {
                case TYPE_IPV6_EX_HEADER_HOP_BY_HOP_OPTIONS:
                    *nextHeader = IPV6_PROT_HOP_BY_HOP_OPTIONS_HEADER;
                    break;
                case TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_1:
                case TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_2:
                    *nextHeader = IPV6_PROT_DESTINATION_OPTIONS_HEADER;
                    break;
                case TYPE_IPV6_EX_HEADER_ROUTING:
                    *nextHeader = IPV6_PROT_ROUTING_HEADER;
                    break;
                case TYPE_IPV6_EX_HEADER_FRAGMENT:
                    *nextHeader = IPV6_PROT_FRAGMENTATION_HEADER;
                    break;
                case TYPE_IPV6_EX_HEADER_AUTHENTICATION_HEADER:
                    *nextHeader = IPV6_PROT_AUTHENTICATION_HEADER;
                    break;
                case TYPE_IPV6_EX_HEADER_ENCAPSULATING_SECURITY_PAYLOAD:
                    *nextHeader = IPV6_PROT_ENCAPSULATING_SECURITY_PAYLOAD_HEADER;
                    break;
                default:
                    *nextHeader = ptrPacket->upperLayerHeaderType;
                    break;
            }
        }
    }
}


// ipv6_private.h
IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_DataSegmentGetByType (IPV6_PACKET * ptrPacket, IPV6_SEGMENT_TYPE type)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;

    if (ptrPacket == NULL)
        return NULL;

    ptrSegment = &ptrPacket->payload;

    if (type != TYPE_IPV6_BEGINNING_OF_WRITABLE_PART)
    {
        while ((ptrSegment != NULL) && (ptrSegment->segmentType < type))
        {
            ptrSegment = ptrSegment->nextSegment;
        }
    }
    else
    {
        IPV6_DATA_SEGMENT_HEADER * ptrTempSegment;

        type = TYPE_IPV6_UPPER_LAYER_PAYLOAD;

        while ((ptrSegment != NULL) && (ptrSegment->segmentType < type))
        {
            ptrSegment = ptrSegment->nextSegment;
        }

        while (ptrSegment != NULL)
        {
            // Move ptrSegment to the next dynamically allocated segment
            while ((ptrSegment != NULL) && (ptrSegment->memory != IPV6_DATA_DYNAMIC_BUFFER))
                ptrSegment = ptrSegment->nextSegment;
            // Break out of the loop if the pointer gets to the end of the linked list
            if (ptrSegment == NULL)
                break;

            // Initialize ptrTempSegment to the first segment after the beginning of the dynamically allocated segments
            ptrTempSegment = ptrSegment->nextSegment;
            // Check to see if the dynamically allocated section is contiguous at the end of the linked list (or find the break)
            while ((ptrTempSegment != NULL) && (ptrTempSegment->memory == IPV6_DATA_DYNAMIC_BUFFER))
                ptrTempSegment = ptrTempSegment->nextSegment;

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
                    if (ptrTempSegment->segmentLen != 0)
                        ptrSegment = ptrTempSegment;
                    ptrTempSegment = ptrTempSegment->nextSegment;
                }
                break;
            }
        }
    }

    if (ptrSegment == NULL)
        return NULL;
    else if (ptrSegment->segmentType == type)
        return ptrSegment;
    else
        return NULL;
}


// ipv6_private.h
void * TCPIP_IPV6_DataSegmentContentsGetByType (IPV6_PACKET * ptrPacket, IPV6_SEGMENT_TYPE type)
{
    IPV6_DATA_SEGMENT_HEADER * ptr;

    ptr = TCPIP_IPV6_DataSegmentGetByType(ptrPacket, type);

    if (ptr != NULL)
    {
        return (void *)ptr->dataLocation;
    }
    else
        return NULL;
}


// ipv6.h
unsigned short TCPIP_IPV6_TxIsPutReady (IPV6_PACKET * ptrPacket, unsigned short count)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;
    unsigned short payloadSize = 0;
    unsigned short availableSpace;

    if (ptrPacket == NULL)
        return 0;

    payloadSize = (count > TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE)?count:TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE;

    ptrSegment = TCPIP_IPV6_DataSegmentGetByType (ptrPacket, TYPE_IPV6_UPPER_LAYER_PAYLOAD);

    // Verify that there is a valid upper layer payload
    if (ptrSegment == NULL)
    {
        ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate(payloadSize);
        if (ptrSegment == NULL)
        {
            return 0;
        }

        TCPIP_IPV6_PacketSegmentInsert (ptrSegment, ptrPacket, TYPE_IPV6_END_OF_LIST);

        return payloadSize;
    }

    ptrSegment = TCPIP_IPV6_DataSegmentGetByType (ptrPacket, TYPE_IPV6_BEGINNING_OF_WRITABLE_PART);

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
    if(payloadSize < TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE)
    {
        payloadSize = TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE;
    }

    ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate(payloadSize);
    if (ptrSegment == NULL)
    {
        return 0;
    }

    TCPIP_IPV6_PacketSegmentInsert (ptrSegment, ptrPacket, TYPE_IPV6_END_OF_LIST);

    return availableSpace + payloadSize ;
}


// ipv6.h
bool TCPIP_IPV6_Put (IPV6_PACKET * pkt, unsigned char v)
{
    return (TCPIP_IPV6_PutArray (pkt, &v, 1) == 1)?true:false;
}


// ipv6.h
unsigned short TCPIP_IPV6_ArrayPutHelper (IPV6_PACKET * ptrPacket, const void * dataSource, uint8_t dataType, unsigned short len)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;
    uint8_t * destBuffer;
    uint8_t* sourceBuffer;
    uint16_t putLen, avlblSize, allocSize, txSize;

    if (ptrPacket == NULL)
        return 0;

    ptrSegment = TCPIP_IPV6_DataSegmentGetByType (ptrPacket, TYPE_IPV6_BEGINNING_OF_WRITABLE_PART);

    if (ptrSegment == NULL)
        return 0;


    sourceBuffer = (uint8_t*)dataSource;
    putLen = 0;
    while(len != 0)
    {
        if(ptrSegment == 0)
        { // allocate new segment
            allocSize = (len < TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE) ? TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE : len;
            ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate(allocSize);
            if (ptrSegment == 0)
                return putLen;

            TCPIP_IPV6_PacketSegmentInsert (ptrSegment, ptrPacket, TYPE_IPV6_END_OF_LIST);
        }

        avlblSize = ptrSegment->segmentSize - ptrSegment->segmentLen;
        if(avlblSize)
        {
            destBuffer = (uint8_t *)ptrSegment->dataLocation + ptrSegment->segmentLen;
            txSize = (len > avlblSize)? avlblSize : len;
            if (dataType == IPV6_DATA_PIC_RAM)
                memcpy (destBuffer, sourceBuffer, txSize);
            else if (dataType == IPV6_DATA_NETWORK_FIFO)
                MACGetArray((TCPIP_MAC_PACKET*)dataSource, destBuffer, txSize);
            else
                return 0;

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

    ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate (0);
    if(ptrSegment == 0)
    {
        return 0;
    }

    ptrSegment->nextSegment = NULL;
    ptrSegment->dataLocation = payload;
    ptrSegment->segmentSize = len;
    ptrSegment->segmentLen = len;
    ptrSegment->memory = IPV6_DATA_PIC_RAM;

    TCPIP_IPV6_PacketSegmentInsert (ptrSegment, ptrPacket, TYPE_IPV6_END_OF_LIST);

    ptrPacket->payloadLen += len;

    return len;
}


// ipv6_private.h
unsigned short TCPIP_IPV6_PseudoHeaderChecksumGet (IPV6_PACKET * ptrPacket)
{
    if (ptrPacket->flags.addressType == IP_ADDRESS_TYPE_IPV6)
    {
        IPV6_PSEUDO_HEADER pseudoHeader;

        pseudoHeader.zero1 = 0;
        pseudoHeader.zero2 = 0;
        pseudoHeader.PacketLength = TCPIP_Helper_ntohs (ptrPacket->upperLayerHeaderLen + ptrPacket->payloadLen);
        pseudoHeader.NextHeader = ptrPacket->upperLayerHeaderType;
        memcpy((void *)&pseudoHeader.SourceAddress, (void *)TCPIP_IPV6_SourceAddressGet (ptrPacket), sizeof (IPV6_ADDR));
        memcpy((void *)&pseudoHeader.DestAddress, (void *)TCPIP_IPV6_DestAddressGet (ptrPacket), sizeof (IPV6_ADDR));
        return TCPIP_Helper_CalcIPChecksum ((void *)&pseudoHeader, sizeof (pseudoHeader), 0);
    }

    return 0;
}


// ipv6_manager.h
void TCPIP_IPV6_HopLimitSet(IPV6_PACKET * ptrPacket, uint8_t hopLimit)
{
    IPV6_HEADER * ptrHeader = TCPIP_IPV6_DataSegmentContentsGetByType (ptrPacket, TYPE_IPV6_HEADER);
    ptrHeader->HopLimit = hopLimit;
}


// ipv6_manager.h
unsigned short TCPIP_IPV6_PayloadChecksumCalculate (IPV6_PACKET * ptrPacket)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;
    TCPIP_UINT32_VAL checksum;
    uint16_t tempChecksum = 0;
    unsigned short checksumByteCount = 0;

    ptrSegment = TCPIP_IPV6_DataSegmentGetByType (ptrPacket, TYPE_IPV6_UPPER_LAYER_HEADER);

    checksum.Val = 0;
    if (ptrSegment != NULL)
    {
        checksum.w[0] = ~TCPIP_Helper_CalcIPChecksum((uint8_t *)ptrSegment->dataLocation, ptrPacket->upperLayerHeaderLen, 0);
        checksumByteCount += ptrPacket->upperLayerHeaderLen;
    }

    ptrSegment = TCPIP_IPV6_DataSegmentGetByType (ptrPacket, TYPE_IPV6_UPPER_LAYER_PAYLOAD);

    while (ptrSegment != NULL)
    {
        switch (ptrSegment->memory)
        {
            case IPV6_DATA_DYNAMIC_BUFFER:
            case IPV6_DATA_PIC_RAM:
                tempChecksum = ~TCPIP_Helper_CalcIPChecksum((uint8_t *)ptrSegment->dataLocation, ptrSegment->segmentLen, 0);
                if (checksumByteCount % 2)
                {
                    tempChecksum = TCPIP_Helper_htons(tempChecksum);
                }
                checksumByteCount += ptrSegment->segmentLen;
                break;
            case IPV6_DATA_NETWORK_FIFO:
            case IPV6_DATA_NONE:
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
    IPV6_DATA_SEGMENT_HEADER * segmentHeader2 = 0;

    while ((segmentHeader != NULL) && (segmentHeader->segmentType != TYPE_IPV6_UPPER_LAYER_PAYLOAD))
    {
        segmentHeader2 = segmentHeader;
        segmentHeader = segmentHeader->nextSegment;
    }

    if(segmentHeader2)
    {
        segmentHeader2->nextSegment = NULL;
    }

    while (segmentHeader != NULL)
    {
        segmentHeader2 = segmentHeader->nextSegment;
        TCPIP_HEAP_Free (ipv6MemH, segmentHeader);
        segmentHeader = segmentHeader2;
    }

    pkt->flags.queued = false;
    pkt->payloadLen = 0;
    pkt->offsetInSegment = 0;
}


// ipv6_manager.h
bool TCPIP_IPV6_AddressIsSolicitedNodeMulticast (const IPV6_ADDR * address)
{
    uint8_t solNodeMulticastFragment[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00};

    if ((address->v[0] == 0xFF) && ((address->v[1] & 0x02) == 0x02) && (memcmp (solNodeMulticastFragment, &address->v[2], sizeof (IPV6_ADDR) - 5) == 0))
    {
        return true;
    }
    return false;
}


// ipv6.h
int TCPIP_IPV6_Flush (IPV6_PACKET * ptrPacket)
{
    IPV6_HEAP_NDP_NC_ENTRY * neighborPointer = 0;
    uint16_t *  checksumPointer;
    bool        toTxPkt = false;
    bool        mcastPkt = false;

    if(ptrPacket == 0 || ptrPacket->flags.addressType != IP_ADDRESS_TYPE_IPV6)
    {
        return -1;
    }

    // Write the Ethernet Header to the MAC TX Buffer
    IPV6_ADDR_STRUCT * sourceAddress;
    IPV6_ADDR * destinationAddress = TCPIP_IPV6_DestAddressGet(ptrPacket);
    IPV6_HEADER * ptrIpHeader = TCPIP_IPV6_DataSegmentContentsGetByType (ptrPacket, TYPE_IPV6_HEADER);

    if (ptrPacket->headerLen == 0u)
    {
        ptrIpHeader->NextHeader = ptrPacket->upperLayerHeaderType;
    }

    ptrIpHeader->PayloadLength = TCPIP_Helper_htons (ptrPacket->payloadLen + ptrPacket->upperLayerHeaderLen + ptrPacket->headerLen);

    if (ptrIpHeader->HopLimit == 0)
        ptrIpHeader->HopLimit = (ipv6Config + TCPIP_STACK_NetIxGet((TCPIP_NET_IF*)ptrPacket->netIfH))->curHopLimit;

    // Check to see if a source address was specified
    if (ptrPacket->flags.sourceSpecified)
    {
        if (ptrPacket->flags.useUnspecAddr)
        {
            TCPIP_IPV6_SourceAddressSet(ptrPacket, (IPV6_ADDR *)&IPV6_FIXED_ADDR_UNSPECIFIED);
            ptrPacket->flags.sourceSpecified = true;
        }
        else
        {
            sourceAddress = TCPIP_IPV6_DASSourceAddressSelect (ptrPacket->netIfH, destinationAddress, &ptrIpHeader->SourceAddress);
            if (sourceAddress == NULL)
            {
                ptrPacket->flags.sourceSpecified = false;
            }
        }
    }

    if (!ptrPacket->flags.sourceSpecified)
    {
        sourceAddress = TCPIP_IPV6_DASSourceAddressSelect (ptrPacket->netIfH, destinationAddress, NULL);
        if (sourceAddress != NULL)
        {
            TCPIP_IPV6_SourceAddressSet(ptrPacket, &(sourceAddress->address));
        }
        else
        {
            // This should never happen; we should always have at least our link-local auto-configured address
            TCPIP_IPV6_SourceAddressSet(ptrPacket, (IPV6_ADDR *)&IPV6_FIXED_ADDR_UNSPECIFIED);
            return -1;
        }
    }

    if (ptrPacket->upperLayerChecksumOffset != IPV6_NO_UPPER_LAYER_CHECKSUM)
    {
        checksumPointer = TCPIP_IPV6_DataSegmentContentsGetByType (ptrPacket, TYPE_IPV6_UPPER_LAYER_HEADER);
        if (checksumPointer != NULL)
        {
            checksumPointer = (uint16_t *)(((uint8_t *)checksumPointer) + ptrPacket->upperLayerChecksumOffset);
            *checksumPointer = ~TCPIP_IPV6_PseudoHeaderChecksumGet (ptrPacket);
            *checksumPointer = TCPIP_IPV6_PayloadChecksumCalculate (ptrPacket);
        }
    }

    if (destinationAddress->v[0] == 0xFF)
    {
        mcastPkt = true;
        // Determine the appropriate link-local address for a multicast address
        ptrPacket->remoteMACAddr.v[0] = 0x33;
        ptrPacket->remoteMACAddr.v[1] = 0x33;
        ptrPacket->remoteMACAddr.v[2] = destinationAddress->v[12];
        ptrPacket->remoteMACAddr.v[3] = destinationAddress->v[13];
        ptrPacket->remoteMACAddr.v[4] = destinationAddress->v[14];
        ptrPacket->remoteMACAddr.v[5] = destinationAddress->v[15];

        toTxPkt = true;
    }
    else
    {
        // Determine the appropriate neighbor to transmit the unicast packet to
        if ((neighborPointer = ptrPacket->neighbor) == NULL)
        {
            neighborPointer = TCPIP_NDP_NextHopGet ((TCPIP_NET_IF*)ptrPacket->netIfH, destinationAddress);
        }

        if (neighborPointer == NULL)
        {
            // The device could not determine a next hop address
            return -1;
        }

        ptrPacket->neighbor = neighborPointer;

        switch (neighborPointer->reachabilityState)
        {
            case NDP_STATE_STALE:
                TCPIP_NDP_ReachabilitySet ((TCPIP_NET_IF*)ptrPacket->netIfH, neighborPointer, NDP_STATE_DELAY);\
                    // Fall through
            case NDP_STATE_REACHABLE:
            case NDP_STATE_DELAY:
            case NDP_STATE_PROBE:
                    memcpy (&ptrPacket->remoteMACAddr, &neighborPointer->remoteMACAddr, sizeof (TCPIP_MAC_ADDR));
                    toTxPkt = true;
                    break;
            case NDP_STATE_INCOMPLETE:
                    TCPIP_NDP_AddressResolve (neighborPointer);
                    _TCPIP_IPV6_PacketEnqueue(ptrPacket, &neighborPointer->queuedPackets, TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT, 0, false);
                    return 0;
            default:
                    TCPIP_NDP_ReachabilitySet ((TCPIP_NET_IF*)ptrPacket->netIfH, neighborPointer, NDP_STATE_INCOMPLETE);
                    TCPIP_NDP_AddressResolve (neighborPointer);
                    _TCPIP_IPV6_PacketEnqueue(ptrPacket, &neighborPointer->queuedPackets, TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT, 0, false);
                    return 0;
        }
    }

    if(toTxPkt)
    {   // packet needs to be transmitted

        if(TCPIP_IPV6_PacketTransmit (ptrPacket))
        {   // success
            if (ptrPacket->ackFnc)
            {
                (*ptrPacket->ackFnc)(ptrPacket, true, ptrPacket->ackParam);
            }
            return 1;
        }
        else
        {
            if(mcastPkt)
            {
                _TCPIP_IPV6_PacketEnqueue(ptrPacket, &mcastQueue.list, TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT, TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT, true);
            }
            else
            {
                _TCPIP_IPV6_PacketEnqueue(ptrPacket, &neighborPointer->queuedPackets, TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT, 0, false);
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

    if (pkt->flags.addressType != IP_ADDRESS_TYPE_IPV6)
    {
        return false;
    }

    if (!TCPIP_STACK_NetworkIsLinked((TCPIP_NET_IF*)pkt->netIfH))
    {
        return false;
    }

    if (pkt->ipv6Header.DestAddress.v[0] == 0xFF)
    {
        mtu = (ipv6Config + TCPIP_STACK_NetIxGet((TCPIP_NET_IF*)pkt->netIfH))->multicastMTU;
    }
    else
    {
        ptrDestination = TCPIP_NDP_RemoteNodeFind ((TCPIP_NET_IF*)pkt->netIfH, &pkt->ipv6Header.DestAddress, IPV6_HEAP_NDP_DC_ID);
        if (ptrDestination)
        {
            mtu = ptrDestination->pathMTU;
        }
        else
        {
            mtu = (ipv6Config + TCPIP_STACK_NetIxGet((TCPIP_NET_IF*)pkt->netIfH))->linkMTU;
        }
    }

    if (TCPIP_Helper_htons(pkt->ipv6Header.PayloadLength) + sizeof (IPV6_HEADER) + pkt->headerLen > mtu)
    {
        return TCPIP_IPV6_PacketTransmitInFragments(pkt, mtu);
    }

    ipv6PktLoad = TCPIP_IPV6_PacketPayload(pkt);
    TCPIP_MAC_PACKET* pMacPkt = TCPIP_IPV6_MacPacketTxAllocate(pkt, ipv6PktLoad, 0); 
    if (pMacPkt == 0)
    {
        return false;
    }

    TCPIP_IPV6_MacPacketTxAddSegments(pkt, pMacPkt, (pkt->payloadLen != 0) ? TCPIP_MAC_SEG_FLAG_USER_PAYLOAD : 0);

    // Write the Ethernet Header to the MAC packet
    TCPIP_IPV6_MacPacketTxPutHeader(pkt, pMacPkt, TCPIP_ETHER_TYPE_IPV6);

    // Transmit the packet
    pMacPkt->pktIf = pkt->netIfH;
    TCPIP_PKT_FlightLogTx(pMacPkt, TCPIP_THIS_MODULE_ID);
    if(_TCPIPStackPacketTx((TCPIP_NET_IF*)pkt->netIfH, pMacPkt) < 0)
    {   // failed
        TCPIP_PKT_FlightLogAcknowledge(pMacPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
        TCPIP_PKT_PacketFree(pMacPkt);
        return false;
    }

    pkt->flags.queued = false;

    return true;
}



// ipv6_private.h
bool TCPIP_IPV6_PacketTransmitInFragments (IPV6_PACKET * pkt, uint16_t mtu)
{
    IPV6_FRAGMENT_HEADER * ptrFragmentHeader = 0;
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;
    IPV6_DATA_SEGMENT_HEADER * ptrFragmentablePart;
    uint16_t currentPayloadLen;
    uint16_t sentPayloadLen = pkt->offsetInSegment;
    uint16_t totalPayloadLen;
    uint16_t unfragmentableLen = 0;
    uint16_t temp;
    uint16_t offsetInSegment = 0;

    if (!TCPIP_STACK_NetworkIsLinked((TCPIP_NET_IF*)pkt->netIfH))
    {
        // Discard the packet if this interface isn't even linked
        return false;
    }

    if ((ptrSegment = TCPIP_IPV6_DataSegmentContentsGetByType(pkt, TYPE_IPV6_EX_HEADER_FRAGMENT)) == NULL)
    {

        ptrSegment = TCPIP_IPV6_DataSegmentHeaderAllocate (sizeof (IPV6_FRAGMENT_HEADER));

        if (ptrSegment == NULL)
        {
            return false;
        }

        ptrSegment->segmentLen = sizeof (IPV6_FRAGMENT_HEADER);

        TCPIP_IPV6_PacketSegmentInsert (ptrSegment, pkt, TYPE_IPV6_EX_HEADER_FRAGMENT);

        ptrFragmentHeader = (IPV6_FRAGMENT_HEADER *)ptrSegment->data;

        ptrFragmentHeader->reserved = 0;
        ptrFragmentHeader->offsetM.bits.reserved2 = 0;
        ptrFragmentHeader->offsetM.bits.m = 0;
        ptrFragmentHeader->offsetM.bits.fragmentOffset = 0;
        ptrFragmentHeader->identification = TCPIP_Helper_htonl(fragmentId);
        fragmentId++;
        pkt->headerLen += sizeof (IPV6_FRAGMENT_HEADER);
    }

    totalPayloadLen = pkt->payloadLen + pkt->headerLen + pkt->upperLayerHeaderLen + sizeof (IPV6_HEADER);

    // If a router specified that the path MTU is less than the minimum link MTU
    // we just need to add a fragmentation header to the packet
    if (mtu < TCPIP_IPV6_MINIMUM_LINK_MTU)
    {
        if (totalPayloadLen < TCPIP_IPV6_DEFAULT_LINK_MTU)
            mtu = TCPIP_IPV6_DEFAULT_LINK_MTU;
    }

    ptrSegment = &pkt->payload;
    while ((ptrSegment != NULL) && (ptrSegment->segmentType <= TYPE_IPV6_EX_HEADER_FRAGMENT))
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
                if (mtu - currentPayloadLen > 8)
                {
                    currentPayloadLen = mtu - (mtu & 0b111);
                }
                else
                {
                    if (currentPayloadLen % 8 != 0)
                    {
                        if ((ptrSegment->segmentLen - offsetInSegment) > (8 - (currentPayloadLen & 0b111)))
                        {
                            currentPayloadLen += (8 - (currentPayloadLen & 0b111));
                        }
                        else
                        {
                            currentPayloadLen -= (currentPayloadLen & 0b111);
                        }
                    }
                }
                break;
            }
            ptrSegment = ptrSegment->nextSegment;
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
        ptrFragmentHeader->offsetM.bits.fragmentOffset = sentPayloadLen >> 3;

        ptrFragmentHeader->offsetM.w = TCPIP_Helper_htons(ptrFragmentHeader->offsetM.w);

        // Calculate new payload length
        pkt->ipv6Header.PayloadLength = TCPIP_Helper_htons(currentPayloadLen - sizeof (IPV6_HEADER));

        TCPIP_MAC_PACKET* pMacPkt = TCPIP_IPV6_MacPacketTxAllocate(pkt, currentPayloadLen, 0); 
        if (pMacPkt == 0)
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
        while (temp)
        {
            if (ptrSegment->segmentLen < temp)
            {
                memcpy(pMacData, ptrSegment->dataLocation, ptrSegment->segmentLen);
                lengthOfSegments += ptrSegment->segmentLen;
                pMacData += ptrSegment->segmentLen;
                temp -= ptrSegment->segmentLen;
                ptrSegment = ptrSegment->nextSegment;
            }
            else
            {
                memcpy(pMacData, ptrSegment->dataLocation, temp);
                lengthOfSegments += temp;

                pMacData += temp;
                break;
            }
        }

        // Write the fragmentable part
        ptrSegment = ptrFragmentablePart;
        temp = currentPayloadLen - unfragmentableLen;
        while (temp)
        {
            if(ptrSegment->segmentType == TYPE_IPV6_UPPER_LAYER_PAYLOAD)
            {
                payloadSeg = true;
            }

            if (ptrSegment->segmentLen < temp)
            {
                memcpy(pMacData, ptrSegment->dataLocation + offsetInSegment, ptrSegment->segmentLen);
                lengthOfSegments += ptrSegment->segmentLen;
                pMacData += ptrSegment->segmentLen;
                temp -= ptrSegment->segmentLen;
                ptrSegment = ptrSegment->nextSegment;
                offsetInSegment = 0;
            }
            else
            {
                memcpy(pMacData, ptrSegment->dataLocation + offsetInSegment, temp);
                lengthOfSegments += temp;

                pMacData += temp;
                if (temp == ptrSegment->segmentLen)
                {
                    ptrSegment = ptrSegment->nextSegment;
                    offsetInSegment = 0;
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
            pMacPkt->pDSeg->segFlags |= TCPIP_MAC_SEG_FLAG_USER_PAYLOAD;
        }
        pMacPkt->pktIf = pkt->netIfH;
        TCPIP_PKT_FlightLogTx(pMacPkt, TCPIP_THIS_MODULE_ID);
        if(_TCPIPStackPacketTx((TCPIP_NET_IF*)pkt->netIfH, pMacPkt) < 0)
        {   // failed, try later on
            TCPIP_PKT_FlightLogAcknowledge(pMacPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
            TCPIP_PKT_PacketFree(pMacPkt);
            pkt->offsetInSegment = sentPayloadLen;
            return false;
        }

        sentPayloadLen += currentPayloadLen - unfragmentableLen;
    } while (sentPayloadLen + unfragmentableLen != totalPayloadLen);

    pkt->flags.queued = false;

    return true;
}


// ipv6_private.h
IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_DataSegmentHeaderAllocate (uint16_t len)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment = (IPV6_DATA_SEGMENT_HEADER *)TCPIP_HEAP_Malloc (ipv6MemH, sizeof (IPV6_DATA_SEGMENT_HEADER) + len);

    if (ptrSegment == NULL)
    {
        return NULL;
    }

    if (len)
    {
        ptrSegment->dataLocation = (uint8_t*)ptrSegment->data;
        ptrSegment->segmentSize = len;
        ptrSegment->segmentLen = 0;
        ptrSegment->memory = IPV6_DATA_DYNAMIC_BUFFER;
    }
    else
    {
        ptrSegment->memory = IPV6_DATA_NONE;
        ptrSegment->dataLocation = (uint8_t*)NULL;
        ptrSegment->segmentSize = 0;
        ptrSegment->segmentLen = 0;
    }
    ptrSegment->nextSegment = NULL;

    return ptrSegment;
}


// ipv6.h
void TCPIP_IPV6_PacketFree (IPV6_PACKET * ptrPacket)
{
    if (ptrPacket == NULL)
        return;

    TCPIP_IPV6_PacketDataFree (ptrPacket);
    TCPIP_HEAP_Free (ipv6MemH, ptrPacket);
}


// ipv6_private.h
void TCPIP_IPV6_PacketDataFree (IPV6_PACKET * ptrPacket)
{
    IPV6_DATA_SEGMENT_HEADER * ptrSegment;
    IPV6_DATA_SEGMENT_HEADER * ptrSegment2;

    if (ptrPacket == NULL)
        return;

    // Set the initial segment to the segment after the IP header (which shouldn't be deallocated
    ptrSegment = ptrPacket->payload.nextSegment;

    while (ptrSegment != NULL)
    {
        ptrSegment2 = ptrSegment->nextSegment;
        TCPIP_HEAP_Free (ipv6MemH, ptrSegment);
        ptrSegment = ptrSegment2;
    }
}

// ipv6_manager.h
bool TCPIP_IPV6_HeaderGet(TCPIP_MAC_PACKET* pRxPkt, IPV6_ADDR * localIPAddr, IPV6_ADDR * remoteIPAddr, uint8_t *protocol, uint16_t *len, uint8_t * hopLimit)
{
    IPV6_HEADER header;


    // Read IP header.
    MACGetArray(pRxPkt, (uint8_t*)&header, sizeof(header));

    // Make sure that this is an IPv6 packet.
    if ((header.V_T_F & 0x000000F0) != IPv6_VERSION)
        return false;

    *hopLimit = header.HopLimit;

    *len = TCPIP_Helper_ntohs(header.PayloadLength);

    *protocol = header.NextHeader;

    memcpy(localIPAddr, &header.DestAddress, sizeof (IPV6_ADDR));

    memcpy(remoteIPAddr, &header.SourceAddress, sizeof (IPV6_ADDR));

    return true;
}

bool TCPIP_IPV6_AddressesGet(TCPIP_MAC_PACKET* pRxPkt, const IPV6_ADDR** pDestIPAddr, const IPV6_ADDR** pSourceIPAddr)
{
    IPV6_HEADER *pHeader;
    uint8_t     *pMacLayer, *pNetLayer;

    // restore the packet MAC and NET pointers
    pMacLayer = pRxPkt->pDSeg->segLoad;
    pNetLayer = pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);


    pHeader = (IPV6_HEADER*)pNetLayer;

    // Make sure that this is an IPv6 packet.
    if ((pHeader->V_T_F & 0x000000F0) != IPv6_VERSION)
    {
        return false;
    }


    if(pDestIPAddr)
    {
        *pDestIPAddr = &pHeader->DestAddress;
    }
    if(pSourceIPAddr)
    {
        *pSourceIPAddr = &pHeader->SourceAddress;
    }

    return true;
}


// ipv6_manager.h
void TCPIP_IPV6_RxBufferSet(TCPIP_MAC_PACKET* pRxPkt, uint16_t Offset)
{
    MACSetReadPtrInRx(pRxPkt, Offset + sizeof (IPV6_HEADER));
}


// ipv6_private.h
void TCPIP_IPV6_FreeConfigLists (IPV6_INTERFACE_CONFIG * pNetIf)
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
IPV6_ADDR_STRUCT * TCPIP_IPV6_AddressFind(TCPIP_NET_IF * pNetIf, const IPV6_ADDR * addr, unsigned char listType)
{
    IPV6_ADDR_STRUCT * nextEntryPointer;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if (!pNetIf)
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
            nextEntryPointer = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6TentativeAddresses.head;
            break;
        case IPV6_ADDR_TYPE_UNICAST:
            nextEntryPointer = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6UnicastAddresses.head;
            break;
        case IPV6_ADDR_TYPE_MULTICAST:
            nextEntryPointer = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6MulticastAddresses.head;
            break;

        default:
            nextEntryPointer = 0;
            break;
    }

    while (nextEntryPointer != NULL)
    {
        if (!memcmp (addr, &(nextEntryPointer->address), sizeof (IPV6_ADDR)))
        {
            return nextEntryPointer;
        }
        nextEntryPointer = nextEntryPointer->next;
    }
    return NULL;
}


// ipv6_private.h
IPV6_ADDR_STRUCT * TCPIP_IPV6_SolicitedNodeMulticastAddressFind(TCPIP_NET_IF * pNetIf, const IPV6_ADDR * addr, unsigned char listType)
{
    IPV6_ADDR_STRUCT * nextEntryPointer;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if (!pNetIf)
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
            nextEntryPointer = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6TentativeAddresses.head;
            break;
        case IPV6_ADDR_TYPE_UNICAST:
            nextEntryPointer = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6UnicastAddresses.head;
            break;

        default:
            nextEntryPointer = 0;
    }

    while (nextEntryPointer != NULL)
    {
        if (!memcmp (&addr->v[13], &(nextEntryPointer->address.v[13]), 3))
        {
            return nextEntryPointer;
        }
        nextEntryPointer = nextEntryPointer->next;
    }
    return NULL;
}


// ipv6.h
IPV6_ADDR_STRUCT * TCPIP_IPV6_MulticastListenerAdd (TCPIP_NET_HANDLE hNet, IPV6_ADDR * address)
{
    IPV6_ADDR_STRUCT * entryLocation;
    IPV6_ADDRESS_TYPE addressType;
    TCPIP_NET_IF* pNetIf = (TCPIP_NET_IF*)hNet;

    if (!pNetIf)
    {
        return NULL;
    }

    if ((entryLocation = TCPIP_IPV6_AddressFind (pNetIf, address, IPV6_ADDR_TYPE_MULTICAST)) == NULL)
    {
        entryLocation = (IPV6_ADDR_STRUCT *) TCPIP_HEAP_Malloc (ipv6MemH, sizeof (IPV6_ADDR_STRUCT));

        if (entryLocation != NULL)
        {
            memcpy (&entryLocation->address, address, sizeof (IPV6_ADDR));
            entryLocation->flags.type = IPV6_ADDR_TYPE_MULTICAST;
            addressType = TCPIP_IPV6_AddressTypeGet(pNetIf, address);
            entryLocation->flags.scope = addressType.bits.scope;
            TCPIP_NDP_LinkedListEntryInsert (pNetIf, entryLocation, IPV6_HEAP_ADDR_MULTICAST_ID);
            TCPIP_IPV6_ClientsNotify(pNetIf, IPV6_EVENT_ADDRESS_ADDED, entryLocation);
        }
    }

    return entryLocation;
}


/*****************************************************************************
  Function:
    void _TCPIP_IPV6_PacketEnqueue (IPV6_PACKET * pkt, PROTECTED_SINGLE_LIST* pList, int queueLimit, int tmoSeconds, bool isProtected)

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
static void _TCPIP_IPV6_PacketEnqueue(IPV6_PACKET * pkt, SINGLE_LIST* pList, int queueLimit, int tmoSeconds, bool isProtected)
{
    IPV6_PACKET * ptrPacket;
    SINGLE_LIST*  sglList;
    PROTECTED_SINGLE_LIST* protList = 0;

    if(isProtected)
    {
        protList = (PROTECTED_SINGLE_LIST*)pList;
        sglList = &protList->list;
    }
    else
    {
        sglList = pList;
    }


    if(protList)
    {
        TCPIP_Helper_ProtectedSingleListLock(protList);
    }

    ptrPacket = (IPV6_PACKET *)sglList->head;

    while ((ptrPacket != NULL) && (ptrPacket != pkt))
    {
        ptrPacket = ptrPacket->next;
    }

    if (ptrPacket == NULL)
    {
        if (sglList->nNodes == queueLimit)
        {
            ptrPacket = (IPV6_PACKET *)TCPIP_Helper_SingleListHeadRemove (sglList);
            ptrPacket->flags.queued = false;
            if (ptrPacket->ackFnc)
            {
                (*ptrPacket->ackFnc)(ptrPacket, false, ptrPacket->ackParam);
            }
        }
        TCPIP_Helper_SingleListTailAdd (sglList, (SGL_LIST_NODE *)pkt);
        pkt->flags.queued = true;
        pkt->queuedPacketTimeout = SYS_TMR_TickCountGet() + (SYS_TMR_TickCounterFrequencyGet() * tmoSeconds);

    }

    if(protList)
    {
        TCPIP_Helper_ProtectedSingleListUnlock(protList);
    }
}


// ipv6_manager.h
IPV6_ADDRESS_TYPE TCPIP_IPV6_AddressTypeGet (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * address)
{
    uint8_t b;
    IPV6_ADDRESS_TYPE returnVal;

    if (address->v[0] == 0xFF)
    {
        // First byte == 0xFF -> multicast
        returnVal.bits.type = IPV6_ADDR_TYPE_MULTICAST;
        b = address->v[1];
        b &= 0x0F;
        switch (b)
        {
            case 1:
            case 2:
            case 4:
            case 5:
            case 8:
            case 0x0E:
                returnVal.bits.scope = b;
                break;
            default:
                returnVal.bits.scope = IPV6_ADDR_SCOPE_UNKNOWN;
                break;
        }
    }
    else
    {
        // First byte != 0xFF -> unicast or anycast
        // Impossible to determine if it's an anycast addr unless
        // it's specifically identified as one somewhere
        returnVal.bits.type = IPV6_ADDR_TYPE_UNICAST;

        if (((address->v[0] == 0xFE) && (address->v[1] == 0x80)) /*|| TCPIP_NDP_PrefixOnLinkStatus(pNetIf, address)*/)
        {
            returnVal.bits.scope = IPV6_ADDR_SCOPE_LINK_LOCAL;
        }
        // Compare to loopback address
        else if ((address->d[0] == 0x00000000) && (address->d[1] == 0x00000000) && (address->d[2] == 0x00000000) && (address->v[12] == 0x00) && \
                     (address->v[13] == 0x00) && (address->v[14] == 0x00) && (address->v[15] == 0x01))
        {
            // This counts as link-local unicast
            returnVal.bits.scope = IPV6_ADDR_SCOPE_INTERFACE_LOCAL;
        }
        else
        {
            returnVal.bits.scope = IPV6_ADDR_SCOPE_GLOBAL;
        }
    }

    return returnVal;
}


// ipv6.h
void TCPIP_IPV6_AddressUnicastRemove(TCPIP_NET_HANDLE netH, IPV6_ADDR * address)
{
    IPV6_ADDR_STRUCT * entryLocation;
    TCPIP_NET_IF * pNetIf =  _TCPIPStackHandleToNet(netH);

    if(pNetIf)
    {

        entryLocation = TCPIP_IPV6_AddressFind (pNetIf, address, IPV6_ADDR_TYPE_UNICAST);
        if (entryLocation != NULL)
        {
            TCPIP_IPV6_ClientsNotify(pNetIf, IPV6_EVENT_ADDRESS_REMOVED, entryLocation);
            TCPIP_NDP_LinkedListEntryRemove (pNetIf, entryLocation, IPV6_HEAP_ADDR_UNICAST_ID);
        }
    }
}


// ipv6.h
void TCPIP_IPV6_MulticastListenerRemove (TCPIP_NET_HANDLE netH, IPV6_ADDR * address)
{
    IPV6_ADDR_STRUCT * entryLocation;
    TCPIP_NET_IF * pNetIf =  _TCPIPStackHandleToNetUp(netH);

    if(pNetIf != 0)
    {
        entryLocation = TCPIP_IPV6_AddressFind (pNetIf, address, IPV6_ADDR_TYPE_MULTICAST);
        if (entryLocation != NULL)
        {
            TCPIP_IPV6_ClientsNotify(pNetIf, IPV6_EVENT_ADDRESS_REMOVED, entryLocation);
            TCPIP_NDP_LinkedListEntryRemove (pNetIf, entryLocation, IPV6_HEAP_ADDR_MULTICAST_ID);
        }
    }

}


// ipv6.h
IPV6_ADDR_STRUCT * TCPIP_IPV6_UnicastAddressAdd (TCPIP_NET_HANDLE netH, IPV6_ADDR * address, int subnetLen, uint8_t skipProcessing)
{
    IPV6_ADDR_STRUCT * entryLocation = 0;
    unsigned char label, precedence, prefixLen;
    IPV6_ADDRESS_TYPE i;
    TCPIP_NET_IF * pNetIf =  _TCPIPStackHandleToNetUp(netH);

    if(pNetIf)
    {

        if (((entryLocation = TCPIP_IPV6_AddressFind (pNetIf, address, IPV6_ADDR_TYPE_UNICAST)) == NULL) &&
                ((entryLocation = TCPIP_IPV6_AddressFind (pNetIf, address, IPV6_ADDR_TYPE_UNICAST_TENTATIVE)) == NULL))
        {
            entryLocation = (IPV6_ADDR_STRUCT *) TCPIP_HEAP_Malloc (ipv6MemH, sizeof (IPV6_ADDR_STRUCT));

            if (entryLocation != NULL)
            {
                memcpy (&entryLocation->address, address, sizeof (IPV6_ADDR));
                i = TCPIP_IPV6_AddressTypeGet (pNetIf, address);
                entryLocation->flags.type = i.bits.type;
                entryLocation->flags.scope = i.bits.scope;
                if (TCPIP_IPV6_DASPolicyGet(address, &label, &precedence, &prefixLen))
                {
                    entryLocation->flags.precedence = precedence;
                    entryLocation->flags.label = label & 0x0F;
                }
                else
                {
                    entryLocation->flags.precedence = 0x00;
                    entryLocation->flags.label = 0xF;
                }
                entryLocation->flags.temporary = 0;

                // Set the Stateless Address Autoconfiguration variables to default values.
                // The Stateless Address AutoConfiguration function will set it to something else
                // if necessary.
                entryLocation->validLifetime = 0xFFFFFFFF;
                entryLocation->preferredLifetime = 0xFFFFFFFF;
                entryLocation->prefixLen = (subnetLen == 0) ? 64 : subnetLen;
                // The skipProcessing flag indicates that the address doesn't need duplicate address
                // detection or an associated solicited node multicast address.
                // This can be used to add loopback addresses, for example.
                if (!skipProcessing)
                {
                    TCPIP_NDP_LinkedListEntryInsert (pNetIf, entryLocation, IPV6_HEAP_ADDR_UNICAST_TENTATIVE_ID);
                    if (TCPIP_NDP_DupAddrDiscoveryDetect (pNetIf, entryLocation) == -1)
                    {
                        TCPIP_NDP_LinkedListEntryRemove (pNetIf, entryLocation, IPV6_HEAP_ADDR_UNICAST_TENTATIVE_ID);
                        entryLocation = NULL;
                    }
                }
                else
                {
                    TCPIP_NDP_LinkedListEntryInsert (pNetIf, entryLocation, IPV6_HEAP_ADDR_UNICAST_ID);
                    TCPIP_IPV6_ClientsNotify(pNetIf, IPV6_EVENT_ADDRESS_ADDED, entryLocation);
                }
            }
        }
    }

    return entryLocation;
}


// ipv6_private.h
uint8_t TCPIP_IPV6_HopByHopOptionsHeaderProcess (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length)
{
    uint8_t headerLength;
    uint8_t option;
    uint8_t optionLength;
    uint8_t data[8];
    uint8_t i;
    uint8_t j;

    TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1);

    *nextHeader = data[0];
    headerLength = data[1] + 1;
    *length = (uint16_t)headerLength << 3;

    option = data[2];

    i = 3;

    do
    {
        switch (option)
        {
            case IPV6_TLV_PAD_1:
                // If this option is present, let the post-switch code load new
                // data if necessary and get the next option
                break;
            case IPV6_TLV_PAD_N:
                optionLength = data[i++];
                if (optionLength <= (8-i))
                {
                    i+= optionLength;
                }
                else
                {
                    optionLength -= (8-i);
                    j = optionLength >> 3;
                    headerLength -= j;
                    while (j--)
                        TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1);
                    optionLength -= (j << 3);
                    i = optionLength;
                }
                break;
            case IPV6_TLV_HBHO_PAYLOAD_JUMBOGRAM:
                break;
            case IPV6_TLV_HBHO_ROUTER_ALERT:
                break;
            default:
                switch (((IPV6_TLV_OPTION_TYPE)option).bits.unrecognizedAction)
                {
                    case IPV6_TLV_UNREC_OPT_SKIP_OPTION:
                        // Ignore this option (treat it like a padding option)
                        optionLength = data[i++];
                        if (optionLength <= (8-i))
                        {
                            i+= optionLength;
                        }
                        else
                        {
                            optionLength -= (8-i);
                            j = optionLength >> 3;
                            headerLength -= j;
                            while (j--)
                                TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1);
                            optionLength -= (j << 3);
                            i = optionLength;
                        }
                        break;
                    case IPV6_TLV_UNREC_OPT_DISCARD_SILENT:
                        *length = *length - (headerLength << 3) + --i;
                        return IPV6_ACTION_DISCARD_SILENT;
                        break;
                    case IPV6_TLV_UNREC_OPT_DISCARD_PP:
                        *length = *length - (headerLength << 3) + --i;
                        return IPV6_ACTION_DISCARD_PP_2;
                        break;
                    case IPV6_TLV_UNREC_OPT_DISCARD_PP_NOT_MC:
                        *length = *length - (headerLength << 3) + --i;
                        return IPV6_ACTION_DISCARD_PP_2_NOT_MC;
                        break;
                }
                break;
        }
        if (i == 8)
        {
            headerLength--;
            i = 0;
            if (headerLength != 0)
                TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1);
        }
        option = data[i++];
    }while (headerLength);

    return IPV6_ACTION_NONE;
}


// ipv6_private.h
uint8_t TCPIP_IPV6_DestinationOptionsHeaderProcess (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length)
{
    uint8_t headerLength;
    uint8_t option;
    uint8_t optionLength;
    uint8_t data[8];
    uint8_t i;
    uint8_t j;

    TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1);

    *nextHeader = data[0];
    headerLength = data[1] + 1;
    *length = (uint16_t)headerLength << 3;

    option = data[2];

    i = 3;

    do
    {
        switch (option)
        {
            // There are only two options current defined, and they're padding options
            case IPV6_TLV_PAD_1:
                // If this option is present, let the post-switch code load new
                // data if necessary and get the next option
                break;
            case IPV6_TLV_PAD_N:
                optionLength = data[i++];
                if (optionLength <= (8-i))
                {
                    i+= optionLength;
                }
                else
                {
                    optionLength -= (8-i);
                    j = optionLength >> 3;
                    headerLength -= (j + 1);
                    while (j--)
                        TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1);
                    optionLength -= (j << 3);
                    i = optionLength;
                }
                break;
            default:
                switch (((IPV6_TLV_OPTION_TYPE)option).bits.unrecognizedAction)
                {
                    case IPV6_TLV_UNREC_OPT_SKIP_OPTION:
                        // Ignore this option (treat it like a padding option)
                        optionLength = data[i++];
                        if (optionLength <= (8-i))
                        {
                            i+= optionLength;
                        }
                        else
                        {
                            optionLength -= (8-i);
                            j = optionLength >> 3;
                            headerLength -= (j + 1);
                            while (j--)
                                TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1);
                            optionLength -= (j << 3);
                            i = optionLength;
                        }
                        break;
                    case IPV6_TLV_UNREC_OPT_DISCARD_SILENT:
                        // Return the offset of the error in this header in the headerLength parameter
                        *length = *length - (headerLength << 3) + --i;
                        return IPV6_ACTION_DISCARD_SILENT;
                        break;
                    case IPV6_TLV_UNREC_OPT_DISCARD_PP:
                        // Return the offset of the error in this header in the headerLength parameter
                        *length = *length - (headerLength << 3) + --i;
                        return IPV6_ACTION_DISCARD_PP_2;
                        break;
                    case IPV6_TLV_UNREC_OPT_DISCARD_PP_NOT_MC:
                        // Return the offset of the error in this header in the headerLength parameter
                        *length = *length - (headerLength << 3) + --i;
                        return IPV6_ACTION_DISCARD_PP_2_NOT_MC;
                        break;
                }
                break;
        }
        if (i == 8)
        {
            headerLength--;
            i = 0;
            if (headerLength != 0)
                TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1);
        }
        option = data[i++];
    }while (headerLength);

    return IPV6_ACTION_NONE;
}


// ipv6_private.h
void TCPIP_IPV6_FragmentBufferFree (void * ptrFragment)
{
    IPV6_RX_FRAGMENT_BUFFER * ptrRxFragment = (IPV6_RX_FRAGMENT_BUFFER*)ptrFragment;
    if (ptrFragment == NULL)
        return;
    if(ptrRxFragment->ptrPacket != NULL)
    {
        TCPIP_HEAP_Free (ipv6MemH, ptrRxFragment->ptrPacket);
    }

    TCPIP_HEAP_Free (ipv6MemH, ptrFragment);
}


// ipv6_private.h
uint8_t TCPIP_IPV6_FragmentationHeaderProcess (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * remoteIP, const IPV6_ADDR * localIP, uint8_t * nextHeader, uint16_t dataCount, uint16_t headerLen, TCPIP_MAC_PACKET* pRxPkt, uint16_t previousHeaderLen)
{
    IPV6_RX_FRAGMENT_BUFFER * ptrFragment;
    IPV6_FRAGMENT_HEADER fragmentHeader;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;


    if ((pNetIf == NULL) || (dataCount < sizeof (IPV6_FRAGMENT_HEADER)))
        return IPV6_ACTION_DISCARD_SILENT;

    pIpv6Config = ipv6Config + TCPIP_STACK_NetIxGet (pNetIf);

    TCPIP_IPV6_ArrayGet(pRxPkt, (void *)&fragmentHeader, 8);
    dataCount -= sizeof (IPV6_FRAGMENT_HEADER);

    fragmentHeader.offsetM.w = TCPIP_Helper_ntohs (fragmentHeader.offsetM.w);

    // Set fragment buffer pointer to the head of the linked list of fragmented packets
    ptrFragment = (IPV6_RX_FRAGMENT_BUFFER *)pIpv6Config->rxFragments.head;
    
    // Find a packet being reassembled that matches this fragmented packet
    while (ptrFragment != NULL)
    {
        if (ptrFragment->identification == fragmentHeader.identification)
        {
            if ((!memcmp (ptrFragment->ptrPacket + IPV6_HEADER_OFFSET_SOURCE_ADDR, remoteIP, sizeof (IPV6_ADDR))) &&
                (!memcmp (ptrFragment->ptrPacket + IPV6_HEADER_OFFSET_DEST_ADDR, localIP, sizeof (IPV6_ADDR))))
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
            return IPV6_ACTION_DISCARD_SILENT;
        //Allocate  memory for the fragmented packet w.r.t to TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE
        ptrFragment->ptrPacket =(uint8_t*)TCPIP_HEAP_Malloc(ipv6MemH,pIpv6Config->rxfragmentBufSize);
        if(ptrFragment->ptrPacket == NULL)
        {
            TCPIP_IPV6_FragmentBufferFree (ptrFragment);
            return IPV6_ACTION_DISCARD_SILENT;
        }

        ptrFragment->next = NULL;
        // The RFC specifies that the fragments must be reassembled in one minute or less
        ptrFragment->secondsRemaining = pIpv6Config->fragmentPktRxTimeout;
        ptrFragment->identification = fragmentHeader.identification;
        ptrFragment->packetSize = headerLen;
        ptrFragment->bytesInPacket = headerLen;
        ptrFragment->firstFragmentLength = 0;

        // Reset the packet's read pointer
        MACSetReadPtrInRx(pRxPkt, 0);

        // Copy the unfragmentable part of the packet data into the fragment buffer
        MACGetArray(pRxPkt, ptrFragment->ptrPacket, headerLen);

        // Set the packet's read pointer to skip the fragment header
        MACSetReadPtrInRx(pRxPkt, headerLen + sizeof (IPV6_FRAGMENT_HEADER));

        if (headerLen == sizeof (IPV6_HEADER))
            ptrFragment->ptrPacket[IPV6_HEADER_OFFSET_NEXT_HEADER] = fragmentHeader.nextHeader;
        else
            ptrFragment->ptrPacket[headerLen - previousHeaderLen] = fragmentHeader.nextHeader;

        TCPIP_Helper_SingleListTailAdd(&pIpv6Config->rxFragments, (SGL_LIST_NODE *)ptrFragment);
        
    }
    
    if (dataCount)
    {
        if (fragmentHeader.offsetM.bits.fragmentOffset == 0)
        {
            ptrFragment->firstFragmentLength = dataCount + headerLen;
        }

        if ((headerLen + (fragmentHeader.offsetM.bits.fragmentOffset << 3) + dataCount) > pIpv6Config->rxfragmentBufSize)
        {
            TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, localIP, remoteIP, ICMPV6_ERR_PP_ERRONEOUS_HEADER, ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl(headerLen + 2), dataCount + headerLen + sizeof (IPV6_FRAGMENT_HEADER));
            if(ptrFragment != NULL)
            {
                TCPIP_Helper_SingleListNodeRemove(&pIpv6Config->rxFragments, (SGL_LIST_NODE *)ptrFragment);
                TCPIP_IPV6_FragmentBufferFree (ptrFragment);
            }
            return IPV6_ACTION_DISCARD_SILENT;
        }

        if (fragmentHeader.offsetM.bits.m)
        {
            // More fragments
            // Check to ensure the packet's payload length is a multiple of eight bytes.
            if (((headerLen + dataCount) % 8) != 0)
            {            
                TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, localIP, remoteIP, ICMPV6_ERR_PP_ERRONEOUS_HEADER, ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl(IPV6_HEADER_OFFSET_PAYLOAD_LENGTH), dataCount + headerLen + sizeof (IPV6_FRAGMENT_HEADER));
                if(ptrFragment != NULL)
                {
                    TCPIP_Helper_SingleListNodeRemove(&pIpv6Config->rxFragments, (SGL_LIST_NODE *)ptrFragment);
                    TCPIP_IPV6_FragmentBufferFree (ptrFragment);
                }
                return IPV6_ACTION_DISCARD_SILENT;
            }
            MACGetArray(pRxPkt, ptrFragment->ptrPacket + headerLen + (fragmentHeader.offsetM.bits.fragmentOffset << 3), dataCount);
            ptrFragment->bytesInPacket += dataCount;
        }
        else
        {
            // No more fragments
            MACGetArray(pRxPkt, ptrFragment->ptrPacket + headerLen + (fragmentHeader.offsetM.bits.fragmentOffset << 3), dataCount);
            ptrFragment->bytesInPacket += dataCount;
            ptrFragment->packetSize += (fragmentHeader.offsetM.bits.fragmentOffset << 3) + dataCount;
        }
    }

    //
    //
    //
    if (ptrFragment->packetSize == ptrFragment->bytesInPacket)
    {
        TCPIP_MAC_PTR_TYPE tempReadPtr;
        TCPIP_MAC_PTR_TYPE tempBaseReadPtr;

        // Subtract the length of the IPV6 header from the payload
        ptrFragment->packetSize -= sizeof (IPV6_HEADER);

        ptrFragment->ptrPacket[IPV6_HEADER_OFFSET_PAYLOAD_LENGTH] = ((TCPIP_UINT16_VAL)ptrFragment->packetSize).v[1];
        ptrFragment->ptrPacket[IPV6_HEADER_OFFSET_PAYLOAD_LENGTH + 1] = ((TCPIP_UINT16_VAL)ptrFragment->packetSize).v[0];

        tempReadPtr = MACSetReadPtr (pRxPkt, (TCPIP_MAC_PTR_TYPE)ptrFragment->ptrPacket);
        tempBaseReadPtr = MACSetBaseReadPtr (pRxPkt, (TCPIP_MAC_PTR_TYPE)ptrFragment->ptrPacket - sizeof (TCPIP_MAC_ETHERNET_HEADER));

        TCPIP_IPV6_Process (pNetIf, pRxPkt);

        MACSetReadPtr (pRxPkt, tempReadPtr);
        MACSetBaseReadPtr (pRxPkt, tempBaseReadPtr);

        
        ptrFragment->firstFragmentLength = 0;

        TCPIP_Helper_SingleListNodeRemove(&pIpv6Config->rxFragments, (SGL_LIST_NODE *)ptrFragment);
        TCPIP_IPV6_FragmentBufferFree (ptrFragment);
    }

    return IPV6_ACTION_DISCARD_SILENT;
}


// ipv6_private.h
void TCPIP_IPV6_FragmentTask (void)
{
    IPV6_RX_FRAGMENT_BUFFER * ptrRxFragment;
    IPV6_RX_FRAGMENT_BUFFER * ptrNextRxFragment;
    TCPIP_NET_IF * pNetIf;
    int netIx;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    TCPIP_MAC_PACKET     rxPkt;
    TCPIP_MAC_PACKET*    pRxPkt = &rxPkt;

    for (netIx = 0; netIx < nStackIfs; netIx++)
    {
        pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
        pIpv6Config = ipv6Config + netIx;
        if(TCPIP_STACK_NetworkIsUp(pNetIf))
        {
            ptrRxFragment = (IPV6_RX_FRAGMENT_BUFFER *)pIpv6Config->rxFragments.head;
            while (ptrRxFragment != NULL)
            {
                ptrRxFragment->secondsRemaining--;
                if ((char)(ptrRxFragment->secondsRemaining) == 0)
                {
                    if (ptrRxFragment->firstFragmentLength != 0)
                    {
                        TCPIP_MAC_PTR_TYPE tempReadPtr;
                        TCPIP_MAC_PTR_TYPE tempBaseReadPtr;

                        tempReadPtr = MACSetReadPtr (pRxPkt, (TCPIP_MAC_PTR_TYPE)ptrRxFragment->ptrPacket);
                        tempBaseReadPtr = MACSetBaseReadPtr (pRxPkt, (TCPIP_MAC_PTR_TYPE)ptrRxFragment->ptrPacket - sizeof (TCPIP_MAC_ETHERNET_HEADER));

                        // If we received the first fragment, send it and a Time Exceeded error message
                        TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, (IPV6_ADDR *)&ptrRxFragment->ptrPacket[IPV6_HEADER_OFFSET_DEST_ADDR], (IPV6_ADDR *)&ptrRxFragment->ptrPacket[IPV6_HEADER_OFFSET_SOURCE_ADDR], ICMPV6_ERR_TE_FRAG_ASSEMBLY_TIME_EXCEEDED, ICMPV6_ERROR_TIME_EXCEEDED, 0, ptrRxFragment->firstFragmentLength);

                        MACSetReadPtr (pRxPkt, tempReadPtr);
                        MACSetBaseReadPtr (pRxPkt, tempBaseReadPtr);
                    }
                    TCPIP_Helper_SingleListNodeRemove(&pIpv6Config->rxFragments, (SGL_LIST_NODE *)ptrRxFragment);
                    ptrNextRxFragment = ptrRxFragment->next;
                    TCPIP_IPV6_FragmentBufferFree (ptrRxFragment);
                    ptrRxFragment = ptrNextRxFragment;
                }
                else
                    ptrRxFragment = ptrRxFragment->next;
            }
        }
    }
}



void TCPIP_IPV6_Task (void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if((sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
    { //  RX signal occurred
        TCPIP_IPV6_ProcessPackets();
    }

    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
    { // regular TMO occurred
        TCPIP_IPV6_Timeout();
    }
}


static void TCPIP_IPV6_Timeout (void)
{
    uint32_t currTick;

    TCPIP_IPV6_InitializeTask();

    currTick = SYS_TMR_TickCountGet();

    if(currTick - ipv6StartTick >= (SYS_TMR_TickCounterFrequencyGet() * TCPIP_IPV6_TASK_PROCESS_RATE) / 1000)
    {   // time to run the IPv6 tasks
        TCPIP_IPV6_FragmentTask();
        TCPIP_IPV6_TimestampsTaskUpdate();
        _TCPIP_IPV6_QueuedPacketTransmitTask(&mcastQueue);
        _TCPIP_IPV6_QueuedPacketTransmitTask(&ipv6QueuedPackets);
#if defined (TCPIP_STACK_USE_SNTP_CLIENT)
        TCPIP_IPV6_UlaTask();
#endif  // defined (TCPIP_STACK_USE_SNTP_CLIENT)

        ipv6StartTick =  currTick;
    }
}

static void TCPIP_IPV6_ProcessPackets(void)
{
    TCPIP_MAC_PACKET* pRxPkt;

    // extract queued IPV6 packets
    while((pRxPkt = _TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != 0)
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
        TCPIP_IPV6_Process ((TCPIP_NET_IF*)pRxPkt->pktIf, pRxPkt);
    }
}


/*****************************************************************************
  Function:
    void _TCPIP_IPV6_QueuedPacketTransmitTask (PROTECTED_SINGLE_LIST* pList)

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
static void _TCPIP_IPV6_QueuedPacketTransmitTask (PROTECTED_SINGLE_LIST* pList)
{
    IPV6_PACKET * queuedPacket;
    uint32_t tick;


    while((queuedPacket = (IPV6_PACKET *)TCPIP_Helper_ProtectedSingleListHeadRemove(pList)) != 0)
    {
        tick = SYS_TMR_TickCountGet();
        if ((long)(tick - queuedPacket->queuedPacketTimeout) > 0)
        {   // timeout; remove packet
            queuedPacket->flags.queued = false;
            if (queuedPacket->ackFnc)
            {
                (*queuedPacket->ackFnc)(queuedPacket, false, queuedPacket->ackParam);
            }
        }
        else
        {   // try to transmit it
            if (TCPIP_IPV6_PacketTransmit(queuedPacket))
            {   // success
                queuedPacket->flags.queued = false;
                if (queuedPacket->ackFnc)
                {
                    (*queuedPacket->ackFnc)(queuedPacket, true, queuedPacket->ackParam);
                }
            }
            else
            {   // failed to transmit; reinsert
                TCPIP_Helper_ProtectedSingleListHeadAdd(pList, (SGL_LIST_NODE*)queuedPacket);
                return;
            }
        }
    }
}


// ipv6_private.h
uint8_t TCPIP_IPV6_RoutingHeaderProcess (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length)
{
    uint8_t headerLength;
    uint8_t routingType;
    uint8_t segmentsLeft;
    uint8_t data[8];


    TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1);

    *nextHeader = data[0];
    headerLength = data[1];
    *length = ((uint16_t)headerLength << 3) + 8;

    routingType = data[2];
    segmentsLeft = data[3];


    switch (routingType)
    {
        // Type 0 routing headers were deprecated and we aren't a router,
        // so we don't support any routing options
        default:
            if (segmentsLeft == 0)
                break;
            else
            {
                // Set the 'length' parameter to the offset of the routingType field.
                // This will allow the TCPIP_IPV6_Process function to find the correct offset
                // for the ICMPv6 Parameter Problem error message.
                *length = 2;
                return IPV6_ACTION_DISCARD_PP_0;
            }
    }

    // If we get here, ignore the rest of the header
    // Since the header size is a multiple of 8 bytes,
    // just discard the rest of the bytes in the first
    // 8-byte unit and read the full header size
    while (headerLength--)
        TCPIP_IPV6_GetOptionHeader (pRxPkt, data, 1);

    return IPV6_ACTION_NONE;
}


// ipv6.h
IPV6_ADDR_STRUCT * TCPIP_IPV6_DASSourceAddressSelect (TCPIP_NET_HANDLE hNetIf, const IPV6_ADDR * dest, IPV6_ADDR * requestedSource)
{
    IPV6_ADDR_STRUCT * currentSource;
    IPV6_ADDR_STRUCT * previousSource;
    uint8_t ruleCounter = ADDR_SEL_RULE_8;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

	TCPIP_NET_IF * pNetIf = _TCPIPStackHandleToNetUp(hNetIf);
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
        return NULL;

    // Simple case: there's only one source address in the list
    if (TCPIP_Helper_DoubleListCount (&pIpv6Config->listIpv6UnicastAddresses) == 1)
        return (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6UnicastAddresses.head;

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
        previousSource = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6UnicastAddresses.head;
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
        if (ruleCounter == ADDR_SEL_RULE_5)
            ruleCounter = ADDR_SEL_RULE_3;
    } while (ruleCounter >= ADDR_SEL_RULE_1);

    return (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6UnicastAddresses.head;
}


// ipv6_private.h
uint8_t TCPIP_IPV6_DASPolicyGet (const IPV6_ADDR * addr, uint8_t * label, uint8_t * precedence, uint8_t * prefixLen)
{
    uint8_t i;
    uint8_t prefixMatch = 0;
    uint8_t matchingPrefix = 0xFF;

    for (i = 0; i < IPV6_ADDR_POLICY_TABLE_LEN; i++)
    {
        if (gPolicyTable[i].prefixLength != 0xFF)
        {
            // If we get to the 0-length prefix and we haven't found a
            // matching prefix, then assume a match
            if (gPolicyTable[i].prefixLength == 0)
            {
                if (prefixMatch == 0)
                    matchingPrefix = i;
            }
            else if (gPolicyTable[i].prefixLength > prefixMatch)
            {
                if (TCPIP_Helper_FindCommonPrefix ((uint8_t *)addr, (uint8_t *)&gPolicyTable[i].address, 16) >= gPolicyTable[i].prefixLength)
                {
                    matchingPrefix = i;
                    prefixMatch = gPolicyTable[i].prefixLength;
                }
            }
        }
    }

    if (matchingPrefix == 0xFF)
        return false;
    else
    {
        if (label != NULL)
            *label = gPolicyTable[matchingPrefix].label;
        if (precedence != NULL)
            *precedence = gPolicyTable[matchingPrefix].precedence;
        if (prefixLen != NULL)
            *prefixLen = gPolicyTable[matchingPrefix].prefixLength;
    }
    return true;
}


// ipv6_private.h
unsigned char TCPIP_IPV6_ASCompareSourceAddresses(TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * addressOne, IPV6_ADDR_STRUCT * addressTwo, const IPV6_ADDR * dest, IPV6_ADDR_SEL_INDEX rule)
{
    unsigned char policy1 = 0;
    unsigned char policy2 = 0;
    unsigned char destPolicy;
    IPV6_ADDRESS_TYPE destScope;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if (pNetIf == NULL)
        return false;

    pIpv6Config = ipv6Config + TCPIP_STACK_NetIxGet (pNetIf);

    switch (rule)
    {
        case ADDR_SEL_RULE_1:
            // We can assume the the addresses are different; the function to add a local
            // address won't add a new one if it's already in the IPv6 Heap.
            if (memcmp ((void *)&(addressTwo->address), (void *)dest, 16) == 0)
            {
                return true;
            }
            break;
        case ADDR_SEL_RULE_2:
            if (addressOne->flags.scope != addressTwo->flags.scope)
            {
                destScope = TCPIP_IPV6_AddressTypeGet (pNetIf, dest);
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
        case ADDR_SEL_RULE_3:
            if (addressTwo->preferredLifetime && !(addressOne->preferredLifetime))
            {
                return true;
            }
            break;
        case ADDR_SEL_RULE_4:
            // We aren't supporting Mobile IPv6 at this time
            break;
        case ADDR_SEL_RULE_5:
            // We aren't supporting multiple interfaces at this time
            break;
        case ADDR_SEL_RULE_6:
            if (!TCPIP_IPV6_DASPolicyGet (dest, &destPolicy, NULL, NULL))
            {
                // If there's no policy that corresponds to the destination, skip this step
                break;
            }
            if (!TCPIP_IPV6_DASPolicyGet (&(addressOne->address), &policy1, NULL, NULL))
            {
                if (TCPIP_IPV6_DASPolicyGet (&(addressTwo->address), &policy2, NULL, NULL))
                {
                    if (destPolicy == policy2)
                        return true;
                }
            }
            else
            {
                if (!TCPIP_IPV6_DASPolicyGet (&(addressTwo->address), &policy2, NULL, NULL))
                {
                    if (destPolicy == policy1)
                        return false;
                }
            }
            if (policy1 != policy2)
            {
                if (destPolicy == policy2)
                    return true;
            }
            break;
        case ADDR_SEL_RULE_7:
            if (addressOne->flags.temporary != addressTwo->flags.temporary)
            {
                if (((addressTwo->flags.temporary == false) && (pIpv6Config->policyPreferTempOrPublic == IPV6_PREFER_PUBLIC_ADDRESSES)) ||
                    ((addressTwo->flags.temporary == true) && (pIpv6Config->policyPreferTempOrPublic == IPV6_PREFER_TEMPORARY_ADDRESSES)))
                {
                    return true;
                }
            }
            break;
        case ADDR_SEL_RULE_8:
            policy1 = TCPIP_Helper_FindCommonPrefix ((uint8_t *)dest, (uint8_t *)&(addressOne->address), 16);
            policy2 = TCPIP_Helper_FindCommonPrefix ((uint8_t *)dest, (uint8_t *)&(addressTwo->address), 16);
            if (policy2 > policy1)
            {
                return true;
            }
            break;

        default:
            break;
    }
    // If there's no reason to prefer addressTwo, return false
    return false;
}


// ipv6_private.h
void TCPIP_IPV6_TimestampsTaskUpdate (void)
{
    unsigned long timeElapsed;
    unsigned long currentTickTime = SYS_TMR_TickCountGet();

    unsigned long correctedCurrentTime;
    int i;
    IPV6_HEAP_NDP_PL_ENTRY * ptrPrefix;
    IPV6_HEAP_NDP_PL_ENTRY * tempPrefix;
    IPV6_ADDR_STRUCT * ptrAddress;
    IPV6_ADDR_STRUCT * tempAddress;
    IPV6_HEAP_NDP_DR_ENTRY * ptrRouter;
    IPV6_HEAP_NDP_DC_ENTRY * ptrDestination;
    TCPIP_NET_IF * pNetIf;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    for (i = 0; i < nStackIfs; i++)
    {
        pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(i);
        pIpv6Config = ipv6Config + i;
        if(TCPIP_STACK_NetworkIsUp(pNetIf))
        {
            // Update prefixes
            ptrPrefix = (IPV6_HEAP_NDP_PL_ENTRY *)pIpv6Config->listPrefixList.head;

            while (ptrPrefix != NULL)
            {
                timeElapsed = currentTickTime - ptrPrefix->lastTickTime;
                timeElapsed /= SYS_TMR_TickCounterFrequencyGet();
                correctedCurrentTime = currentTickTime - (timeElapsed % SYS_TMR_TickCounterFrequencyGet());


                ptrPrefix->lastTickTime = correctedCurrentTime;
                if (timeElapsed < ptrPrefix->validLifetime)
                {
                    ptrPrefix->validLifetime -= timeElapsed;
                    tempPrefix = ptrPrefix->next;
                }
                else
                {
                    tempPrefix = ptrPrefix->next;
                    TCPIP_NDP_LinkedListEntryRemove ((TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(i), ptrPrefix, IPV6_HEAP_NDP_PL_ID);
                }
                ptrPrefix = tempPrefix;
            }

            // Update addresses
            ptrAddress = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6UnicastAddresses.head;

            while (ptrAddress != NULL)
            {
                timeElapsed = currentTickTime - ptrAddress->lastTickTime;
                timeElapsed /= SYS_TMR_TickCounterFrequencyGet();
                correctedCurrentTime = currentTickTime - (timeElapsed % SYS_TMR_TickCounterFrequencyGet());


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
                    TCPIP_IPV6_AddressUnicastRemove((TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(i), &ptrAddress->address);
                }
                ptrAddress = tempAddress;

            }

            // Update default routers
            ptrRouter = (IPV6_HEAP_NDP_DR_ENTRY *)pIpv6Config->listDefaultRouter.head;

            while (ptrRouter != NULL)
            {
                if ((long)(currentTickTime - ptrRouter->tickTimer) >= SYS_TMR_TickCounterFrequencyGet())
                {
                    ptrRouter->tickTimer += SYS_TMR_TickCounterFrequencyGet();
                    ptrRouter->invalidationTimer--;
                    if (!ptrRouter->invalidationTimer)
                    {
                        ptrRouter = TCPIP_NDP_LinkedListEntryRemove ((TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(i), ptrRouter, IPV6_HEAP_NDP_DR_ID);
                    }
                }
                else
                {
                    ptrRouter = ptrRouter->next;
                }
            }

            // Try to periodically increase path MTUs
            ptrDestination = (IPV6_HEAP_NDP_DC_ENTRY *)pIpv6Config->listDestinationCache.head;

            while (ptrDestination != NULL)
            {
                if (ptrDestination->pathMTUIncreaseTimer != 0)
                {
                    if ((long)(currentTickTime - ptrDestination->pathMTUIncreaseTimer) > 0)
                    {
                        ptrDestination->pathMTU = TCPIP_IPV6_DEFAULT_LINK_MTU;
                        ptrDestination->pathMTUIncreaseTimer = 0;
                    }
                }
                ptrDestination = ptrDestination->next;
            }

            if (pIpv6Config->mtuIncreaseTimer != 0)
            {
                if ((long)(currentTickTime - pIpv6Config->mtuIncreaseTimer) > 0)
                {
                    pIpv6Config->linkMTU = TCPIP_IPV6_DEFAULT_LINK_MTU;
                    pIpv6Config->multicastMTU = TCPIP_IPV6_DEFAULT_LINK_MTU;
                    pIpv6Config->mtuIncreaseTimer = 0;
                }
            }
        }
    }
}


static void TCPIP_IPV6_Process (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt)
{
    IPV6_ADDR tempLocalIPv6Addr;
    IPV6_ADDR tempRemoteIPv6Addr;
    IPV6_ADDR_STRUCT * localAddressPointer;
    uint16_t headerLen = 0;
    uint16_t extensionHeaderLen;
    uint8_t action;
    uint8_t hopLimit;
    uint8_t addrType = 0;
    uint16_t currentOffset = 0;
    uint8_t cIPFrameType;
    uint16_t dataCount;

    const IPV6_ADDR* constLocalIPv6Addr;
    const IPV6_ADDR* constRemoteIPv6Addr;

    // Break out of this processing function is IPv6 is not enabled on this node
    if (!pNetIf->Flags.bIPv6Enabled)
    {
        TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_NET_DOWN); 
        return;
    }

    if((pRxPkt->pktFlags & TCPIP_MAC_PKT_FLAG_SPLIT) != 0)
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
    pRxPkt->pktClientData = TCPIP_MAC_PKT_ACK_RX_OK;
    
    currentOffset += sizeof (IPV6_HEADER);

    // Determine if the address corresponds to one of the addresses used by our node
    if (constLocalIPv6Addr->v[0] == 0xFF)
    {
        // Determine if the address is a solicited node multicast address
        if (TCPIP_IPV6_AddressIsSolicitedNodeMulticast (constLocalIPv6Addr))
        {
            // Determine if we are listening to this address
            if ((localAddressPointer = TCPIP_IPV6_SolicitedNodeMulticastAddressFind(pNetIf, constLocalIPv6Addr, IPV6_ADDR_TYPE_UNICAST)) != NULL)
            {
                addrType = IPV6_ADDR_TYPE_SOLICITED_NODE_MULTICAST;
            }
            else if ((localAddressPointer = TCPIP_IPV6_SolicitedNodeMulticastAddressFind(pNetIf, constLocalIPv6Addr, IPV6_ADDR_TYPE_UNICAST_TENTATIVE)) != NULL)
            {
                addrType = IPV6_ADDR_TYPE_UNICAST_TENTATIVE;
            }
        }
        else
        {
            // Find the address in the list of multicast addresses we're listening to
            localAddressPointer = TCPIP_IPV6_AddressFind(pNetIf, constLocalIPv6Addr, IPV6_ADDR_TYPE_MULTICAST);
            addrType = IPV6_ADDR_TYPE_MULTICAST;
        }
    }
    else
    {
        // Find the address in the list of unicast addresses assigned to our node
        localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, constLocalIPv6Addr, IPV6_ADDR_TYPE_UNICAST);
        addrType = IPV6_ADDR_TYPE_UNICAST;
    }

    // If the packet's destination address isn't one of the unicast/multicast addresses assigned to this node, check to see if it is a
    // tentative address (ICMPv6/NDP still needs to receive packets addressed to tentative addresses for duplicate address detection).
    // If it is not tentative, return.
    if (localAddressPointer == NULL)
    {
        // If we didn't find a matching configured address try to find one in the tentative address list
        if ((localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, constLocalIPv6Addr, IPV6_ADDR_TYPE_UNICAST_TENTATIVE)) != NULL)
        {
            addrType = IPV6_ADDR_TYPE_UNICAST_TENTATIVE;
        }
        else
        {
            TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR); 
            return;
        }
    }

    extensionHeaderLen = 0;
    action = IPV6_ACTION_BEGIN_EX_HEADER_PROCESSING;

    // Process frame
    while (cIPFrameType != IPV6_PROT_NONE)
    {
        switch (cIPFrameType)
        {
            // Process the frame's routing header
            case IPV6_PROT_ROUTING_HEADER:
                action = TCPIP_IPV6_RoutingHeaderProcess (pNetIf, pRxPkt, &cIPFrameType, &headerLen);
                dataCount -= headerLen;
                extensionHeaderLen += headerLen;
                break;
            // Process the frame's fragmentation header
            case IPV6_PROT_FRAGMENTATION_HEADER:
                action = TCPIP_IPV6_FragmentationHeaderProcess (pNetIf, constRemoteIPv6Addr, constLocalIPv6Addr, &cIPFrameType, dataCount, extensionHeaderLen + sizeof (IPV6_HEADER), pRxPkt, headerLen);
                //action = IPV6_ACTION_DISCARD_SILENT;
                break;
            // Process the frame's ESP header
            case IPV6_PROT_ENCAPSULATING_SECURITY_PAYLOAD_HEADER:
                action = IPV6_ACTION_DISCARD_SILENT;
                break;
            // Process the frame's Authentication header
            case IPV6_PROT_AUTHENTICATION_HEADER:
                action = IPV6_ACTION_DISCARD_SILENT;
                break;
            // Process the frame's destination options header
            case IPV6_PROT_DESTINATION_OPTIONS_HEADER:
                action = TCPIP_IPV6_DestinationOptionsHeaderProcess(pNetIf, pRxPkt, &cIPFrameType, &headerLen);
                dataCount -= headerLen;
                extensionHeaderLen += headerLen;
                break;
            // Process the frame's TCP header and payload
            case IPV6_PROT_TCP:
#if defined (TCPIP_STACK_USE_TCP)
                // If the address is tentative, do not process the TCP packet
                if (addrType == IPV6_ADDR_TYPE_UNICAST_TENTATIVE)
                {
                    cIPFrameType = IPV6_PROT_NONE;
                    break;
                }
                // set up the packet fields used by TCP
                pRxPkt->pDSeg->segLen = dataCount;
                pRxPkt->totTransportLen = dataCount;
                pRxPkt->pktClientData = extensionHeaderLen;
                // forward this packet and signal
                _TCPIPStackModuleRxInsert(TCPIP_MODULE_TCP, pRxPkt, true);
                return;
#else
                cIPFrameType = IPV6_PROT_NONE;
                action = 0;
                break;
#endif
            // Process the frame's UDP header and payload
            case IPV6_PROT_UDP:
#if defined (TCPIP_STACK_USE_UDP)
                // If the address is tentative, do not process the TCP packet
                if (addrType == IPV6_ADDR_TYPE_UNICAST_TENTATIVE)
                {
                    cIPFrameType = IPV6_PROT_NONE;
                    break;
                }
                // set up the packet fields used by UDP
                pRxPkt->pDSeg->segLen = dataCount;
                pRxPkt->totTransportLen = dataCount;
                pRxPkt->pktClientData = extensionHeaderLen;
                // forward this packet and signal
                _TCPIPStackModuleRxInsert(TCPIP_MODULE_UDP, pRxPkt, true);
                return;
#else
                cIPFrameType = IPV6_PROT_NONE;
                action = 0;
                break;
#endif
            // Process the frame's ICMPv6 header and payload
            case IPV6_PROT_ICMPV6:
                // Process the ICMPv6 packet
                TCPIP_ICMPV6_Process(pNetIf, pRxPkt, localAddressPointer, constLocalIPv6Addr, constRemoteIPv6Addr, dataCount, extensionHeaderLen, hopLimit, addrType); 
                cIPFrameType = IPV6_PROT_NONE;
                action = 0;
                break;
            // Process the frame's hop-by-hop options header
            case IPV6_PROT_HOP_BY_HOP_OPTIONS_HEADER:
                // Action should only equal 0xFF immediately after processing the IPv6 header.
                // The hop-by-hop options header must occur only after the IPv6 header.
                if (action == IPV6_ACTION_BEGIN_EX_HEADER_PROCESSING)
                {
                    action = TCPIP_IPV6_HopByHopOptionsHeaderProcess (pNetIf, pRxPkt, &cIPFrameType, &headerLen);
                    dataCount -= headerLen;
                    extensionHeaderLen += headerLen;
                    break;
                }
                // else fall through
            // Unknown header type, parameter problem
            default:
                // Send ICMP Parameter Problem Code 1 and discard packet
                // Action should only equal IPV6_ACTION_BEGIN_EX_HEADER_PROCESSING if we haven't been through this loop once
                if (action == IPV6_ACTION_BEGIN_EX_HEADER_PROCESSING)
                    currentOffset = IPV6_HEADER_OFFSET_NEXT_HEADER;
                else
                    currentOffset -= headerLen;

                TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, constLocalIPv6Addr, constRemoteIPv6Addr, ICMPV6_ERR_PP_UNRECOGNIZED_NEXT_HEADER, ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl((uint32_t)currentOffset), dataCount + extensionHeaderLen + sizeof (IPV6_HEADER));
                cIPFrameType = IPV6_PROT_NONE;
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
                cIPFrameType = IPV6_PROT_NONE;
                break;
            // Discard the packet and send an ICMPv6 Parameter Problem, code 0 error message to the packet's source address
            case IPV6_ACTION_DISCARD_PP_0:
                TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, constLocalIPv6Addr, constRemoteIPv6Addr, ICMPV6_ERR_PP_ERRONEOUS_HEADER, ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl((uint32_t)currentOffset), dataCount + extensionHeaderLen + sizeof (IPV6_HEADER));
                cIPFrameType = IPV6_PROT_NONE;
                break;
            // Discard the packet and send an ICMPv6 Parameter Problem, code 2 error message to the packet's source address
            case IPV6_ACTION_DISCARD_PP_2:
                TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, constLocalIPv6Addr, constRemoteIPv6Addr, ICMPV6_ERR_PP_UNRECOGNIZED_IPV6_OPTION, ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl((uint32_t)currentOffset), dataCount + extensionHeaderLen + sizeof (IPV6_HEADER));
                cIPFrameType = IPV6_PROT_NONE;
                break;
            // Discard the packet and send an ICMPv6 Parameter Problem, code 2 error message to the packet's source address if
            // the packet's destination address is not a multicast address.
            case IPV6_ACTION_DISCARD_PP_2_NOT_MC:
                // Check to ensure the packet's destination address wasn't a multicast address
                if (constLocalIPv6Addr->v[0] != 0xFF)
                {
                    TCPIP_IPV6_ErrorSend (pNetIf, pRxPkt, constLocalIPv6Addr, constRemoteIPv6Addr, ICMPV6_ERR_PP_UNRECOGNIZED_IPV6_OPTION, ICMPV6_ERROR_PARAMETER_PROBLEM, TCPIP_Helper_htonl((uint32_t)currentOffset), dataCount + extensionHeaderLen + sizeof (IPV6_HEADER));
                }
                // Discard the packet
                cIPFrameType = IPV6_PROT_NONE;
                break;
            // No action was required
            case IPV6_ACTION_NONE:
            default:
                break;
        }
    }

    TCPIP_PKT_PacketAcknowledge(pRxPkt, pRxPkt->pktClientData); 
 }


// ipv6_manager.h
void TCPIP_IPV6_ErrorSend (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, uint8_t code, uint8_t type, uint32_t additionalData, uint16_t packetLen)
{
    IPV6_PACKET * pkt;

    if (packetLen + sizeof (IPV6_HEADER) + sizeof (ICMPV6_HEADER_ERROR) > TCPIP_IPV6_MINIMUM_LINK_MTU)
        packetLen = TCPIP_IPV6_MINIMUM_LINK_MTU - (sizeof (IPV6_HEADER) + sizeof (ICMPV6_HEADER_ERROR));

    // An ICMPv6 error message MUST NOT be sent as a result of receiving a packet destined to an IPv6 multicast address
    // Exception: Packet Too Big message, Parameter Problem code 2 message if the unrecognized option has its unrecognized action bits set to 0b10.
    // The unrecognized action bits were already examined in the option processing functions.
    if ((localIP->v[0] == 0xFF) && (type != ICMPV6_ERROR_PACKET_TOO_BIG) && ((type != ICMPV6_ERROR_PARAMETER_PROBLEM) || (code != ICMPV6_ERR_PP_UNRECOGNIZED_IPV6_OPTION)))
        return;

    // An ICMPv6 error message MUST NOT be sent as a result of receiving a packet whose source address does not uniquely identify a single node.
    if ((remoteIP->v[0] == 0xFF) || !memcmp (remoteIP, (IPV6_ADDR *)&IPV6_FIXED_ADDR_UNSPECIFIED, sizeof (IPV6_ADDR)))
        return;

    pkt = TCPIP_ICMPV6_HeaderErrorPut (pNetIf, localIP, remoteIP, code, type, additionalData);
    if (pkt != NULL)
    {
        MACSetReadPtrInRx (pRxPkt, 0);
        if (TCPIP_IPV6_TxIsPutReady(pkt, packetLen) < packetLen)
        {
            TCPIP_IPV6_PacketFree (pkt);
            return;
        }
        TCPIP_IPV6_ArrayPutHelper(pkt, pRxPkt, IPV6_DATA_NETWORK_FIFO, packetLen);
        TCPIP_ICMPV6_Flush (pkt);
    }
}


// ipv6_manager.h
void * TCPIP_IPV6_UpperLayerHeaderPtrGet(IPV6_PACKET * pkt)
{
    return TCPIP_IPV6_DataSegmentContentsGetByType(pkt, TYPE_IPV6_UPPER_LAYER_HEADER);
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
    if(addr)
    {
        memcpy (&p->ipv6Header.DestAddress, (const void *)addr, sizeof (IPV6_ADDR));
    }
    else
    {
        memset (&p->ipv6Header.DestAddress, 0x0, sizeof (IPV6_ADDR));
    }
}


// ipv6.h
void  TCPIP_IPV6_SourceAddressSet(IPV6_PACKET * p, const IPV6_ADDR * addr)
{
    if(addr)
    {
        memcpy (&p->ipv6Header.SourceAddress, addr, sizeof(IPV6_ADDR));
        p->flags.sourceSpecified = true;
    }
    else
    {
        memset (&p->ipv6Header.SourceAddress, 0x0, sizeof(IPV6_ADDR));
        p->flags.sourceSpecified = false;
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
    if(handler && ipv6MemH)
    {
        IPV6_LIST_NODE* newNode = (IPV6_LIST_NODE*)TCPIP_Notification_Add(&ipv6RegisteredUsers, ipv6MemH, sizeof(*newNode));

        if(newNode)
        {
            newNode->handler = handler;
            newNode->hParam = hParam;
            newNode->hNet = hNet;
            return newNode;
        }
    }

    return 0;
}


// ipv6.h -  deregister the event handler
bool TCPIP_IPV6_HandlerDeregister(IPV6_HANDLE hIpv6)
{
    if(hIpv6 && ipv6MemH)
    {
        if(TCPIP_Notification_Remove((SGL_LIST_NODE*)hIpv6, &ipv6RegisteredUsers, ipv6MemH))
        {
            return true;
        }
    }

    return false;
}


// ipv6_manager.h
void TCPIP_IPV6_ClientsNotify(TCPIP_NET_IF* pNetIf, IPV6_EVENT_TYPE evType, const void* evParam)
{
    IPV6_LIST_NODE* iNode;

    for(iNode = (IPV6_LIST_NODE*)ipv6RegisteredUsers.list.head; iNode != 0; iNode = iNode->next)
    {
        if(iNode->hNet == 0 || iNode->hNet == pNetIf)
        {   // trigger event
            (*iNode->handler)(pNetIf, evType, evParam, iNode->hParam);
        }
    }

}


// ipv6_manager.h
IPV6_INTERFACE_CONFIG* TCPIP_IPV6_InterfaceConfigGet(TCPIP_NET_IF* pNetIf)
{
    return ipv6Config + TCPIP_STACK_NetIxGet (pNetIf);
}


const IPV6_ADDR* TCPIP_IPV6_DefaultRouterGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF * pNetIf =  _TCPIPStackHandleToNet(netH);

    if(pNetIf != 0)
    {
        IPV6_HEAP_NDP_DR_ENTRY* pEntry = TCPIP_NDP_DefaultRouterGet (pNetIf);
        if(pEntry != 0 && pEntry->neighborInfo != 0)
        {
            return &pEntry->neighborInfo->remoteIPAddress;
        }
    }

    return 0;

}

// ipv6_manager.h
void TCPIP_IPV6_DoubleListFree (void * list)
{
    DOUBLE_LIST * dList = (DOUBLE_LIST *) list;
    DBL_LIST_NODE * node;
    while ((node = TCPIP_Helper_DoubleListHeadRemove (dList)))
        TCPIP_HEAP_Free (ipv6MemH, node);
}


// ipv6_manager.h
void TCPIP_IPV6_SingleListFree (void * list)
{
    SINGLE_LIST * sList = (SINGLE_LIST *) list;
    SGL_LIST_NODE * node;
    while ((node = TCPIP_Helper_SingleListHeadRemove (sList)))
        TCPIP_HEAP_Free (ipv6MemH, node);
}



// ipv6.h

#if defined (TCPIP_STACK_USE_SNTP_CLIENT)
IPV6_ULA_RESULT TCPIP_IPV6_UniqueLocalUnicastAddressAdd (TCPIP_NET_HANDLE netH, uint16_t subnetID, IPV6_ULA_FLAGS genFlags, IP_MULTI_ADDRESS* ntpAddress)
{

    if(ulaState != TCPIP_IPV6_ULA_IDLE)
    {
        return IPV6_ULA_RES_BUSY;
    }

    TCPIP_NET_IF * pNetIf =  _TCPIPStackHandleToNet(netH);

    if(pNetIf == 0)
    {
        return IPV6_ULA_RES_IF_ERR;
    }
    
    // clear the error status
    TCPIP_SNTP_LastErrorGet();
    ulaNetIf = pNetIf;
    ulaSubnetId = subnetID;
    ulaFlags = genFlags;
    ulaOperStartTick = SYS_TMR_TickCountGet();

    ulaState = TCPIP_IPV6_ULA_PARAM_SET;


    return IPV6_ULA_RES_OK;
}

static void TCPIP_IPV6_UlaTask (void)
{
    TCPIP_SNTP_RESULT sntpRes;
    uint32_t    lastUpdateTick, tStampWindow, currTick;
//    CRYPT_SHA_CTX  shaSum;
    uint8_t shaDigest[20];  // SHA1 size is 160 bits
    IPV6_ADDR ulaAddress;
    IPV6_ULA_RESULT genRes = IPV6_ULA_RES_OK;


    union
    {
        struct
        {
            uint64_t    eui64;
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

    switch(ulaState)
    {

        case TCPIP_IPV6_ULA_PARAM_SET:
        case TCPIP_IPV6_ULA_CONN_START:
            if(ulaState == TCPIP_IPV6_ULA_PARAM_SET)
            {
                sntpRes = TCPIP_SNTP_ConnectionParamSet(0, (ulaFlags & IPV6_ULA_FLAG_NTPV4) ? IP_ADDRESS_TYPE_IPV4 : IP_ADDRESS_TYPE_IPV6, 0);
            }
            else
            {
                sntpRes = TCPIP_SNTP_ConnectionInitiate();
            }

            if(sntpRes >= 0)
            {   // success
                ulaState++;
                ulaOperStartTick = currTick;
                return;
            }

            // failed to set the connection parameters
            if(currTick - ulaOperStartTick < (SYS_TMR_TickCounterFrequencyGet() * TCPIP_IPV6_ULA_NTP_ACCESS_TMO + 999)/1000)
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

            if(sntpRes < 0 || tStampWindow > (SYS_TMR_TickCounterFrequencyGet() * TCPIP_IPV6_ULA_NTP_VALID_WINDOW + 999)/1000)
            {   // failed
                if(currTick - ulaOperStartTick < (SYS_TMR_TickCounterFrequencyGet() * TCPIP_IPV6_ULA_NTP_ACCESS_TMO + 999)/1000)

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
            // get the interface RUI64 identifier
            TCPIP_IPV6_EUI64(ulaNetIf, &ulaTStamp.eui64);

            // calculate the SHA1
//            CRYPT_SHA_Initialize(&shaSum);
//            CRYPT_SHA_DataAdd(&shaSum, ulaTStamp.b, sizeof(ulaTStamp));
//            CRYPT_SHA_Finalize(&shaSum, shaDigest);


            // format the IPv6 address
            memset(ulaAddress.v, 0, sizeof(ulaAddress));

            ulaAddress.v[0] = 0xfd;     // global ULA prefix
            // Global ID as the SHA digest lowest 40 bits
            memcpy(ulaAddress.v + 1, shaDigest + (sizeof(shaDigest) - 5), 5);
            // set the sub net ID
            ulaAddress.v[6] = (uint8_t)(ulaSubnetId >> 8) ;
            ulaAddress.v[7] = (uint8_t)(ulaSubnetId & 0xff) ;
            // set the Interface ID
            memcpy(ulaAddress.v + 8, &ulaTStamp.eui64, sizeof(ulaTStamp.eui64));

            break;

        default:
            // shouldn't happen
            ulaState = TCPIP_IPV6_ULA_IDLE;
            return; 
    }

    if(genRes == IPV6_ULA_RES_OK)
    {
        TCPIP_IPV6_ClientsNotify(ulaNetIf, IPV6_EVENT_ULA_ADDRESS_GENERATED, &ulaAddress);

        if((ulaFlags & IPV6_ULA_FLAG_GENERATE_ONLY) == 0)
        {
            TCPIP_IPV6_UnicastAddressAdd(ulaNetIf, &ulaAddress, 0, (ulaFlags & IPV6_ULA_FLAG_SKIP_DAD) != 0);
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

    memcpy(netAdd.v, TCPIP_STACK_NetAddressMac(pNetIf), sizeof(netAdd));

    sll.b[0] = netAdd.v[0] | 0x02;
    sll.b[1] = netAdd.v[1];
    sll.b[2] = netAdd.v[2];
    sll.b[3] = 0xff;
    sll.b[4] = 0xfe;
    sll.b[5] = netAdd.v[3];
    sll.b[6] = netAdd.v[4];
    sll.b[7] = netAdd.v[5];

    *pRes = sll.ull;

}
#endif  // defined (TCPIP_STACK_USE_SNTP_CLIENT)

bool TCPIP_IPV6_RouterAddressAdd(TCPIP_NET_HANDLE netH, IPV6_ADDR * rAddress, unsigned long validTime, int flags)
{
    TCPIP_NET_IF * pNetIf;

    if(rAddress == 0 || !TCPIP_IPV6_InterfaceIsReady(netH))
    {
        return false;
    }

    pNetIf = _TCPIPStackHandleToNet(netH);


    if(validTime == 0)
    {
        validTime = 0xffffffff;
    }
    return TCPIP_IPV6_NewRouterEntry(pNetIf, rAddress, validTime) != 0;
}


static IPV6_HEAP_NDP_DR_ENTRY* TCPIP_IPV6_NewRouterEntry(TCPIP_NET_IF* pNetIf, IPV6_ADDR* pGatewayAddr, unsigned long validTime)
{
    IPV6_HEAP_NDP_NC_ENTRY * pGatewayNbor;
    IPV6_HEAP_NDP_DR_ENTRY * pGatewayEntry;

    if(pGatewayAddr == 0)
    {
        return 0;
    }   

    pGatewayEntry = (IPV6_HEAP_NDP_DR_ENTRY*)TCPIP_NDP_RemoteNodeFind (pNetIf, pGatewayAddr, IPV6_HEAP_NDP_DR_ID);

    if(pGatewayEntry)
    {   // already existent
        pGatewayEntry->invalidationTimer = validTime;
        pGatewayEntry->tickTimer = SYS_TMR_TickCountGet();

        return pGatewayEntry;
    }

    // non existent; add a new router entry
    pGatewayNbor = TCPIP_NDP_NborEntryCreate (pNetIf, pGatewayAddr, 0, NDP_STATE_INCOMPLETE, true, 0);
    if(pGatewayNbor)
    {
        pGatewayEntry = TCPIP_NDP_DefaultRouterEntryCreate(pNetIf, pGatewayNbor, validTime);
    }

    if(pGatewayEntry == 0)
    {
        if(pGatewayNbor)
        {   // free created entry
            TCPIP_NDP_NborEntryDelete(pNetIf, pGatewayNbor);
        }
    }

    return pGatewayEntry;

}

static uint16_t TCPIP_IPV6_PacketPayload(IPV6_PACKET* pkt)
{
    IPV6_DATA_SEGMENT_HEADER * segmentHeader;
    uint16_t pktLen = 0;

    // add the number of segments needed
    for(segmentHeader = &pkt->payload; segmentHeader != 0; segmentHeader = segmentHeader->nextSegment)
    {
        pktLen += segmentHeader->segmentLen;
    }

    return pktLen;
}

// tries to allocate an associated TCPIP_MAC_PACKET for the IPV6_PACKET 
static TCPIP_MAC_PACKET* TCPIP_IPV6_MacPacketTxAllocate(IPV6_PACKET* pkt, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags)
{
    TCPIP_MAC_PACKET* pMacPkt;


    // allocate a packet with nSegs segments
    pMacPkt = TCPIP_PKT_PacketAlloc(sizeof(*pMacPkt), segLoadLen, flags | (TCPIP_MAC_PKT_FLAG_IPV6 | TCPIP_MAC_PKT_FLAG_TX));

    if(pMacPkt != 0)
    {
        TCPIP_MAC_PACKET_ACK_FUNC macAckFnc = (pkt->macAckFnc != 0) ? pkt->macAckFnc : TCPIP_IPV6_MacPacketTxAck;
        TCPIP_PKT_PacketAcknowledgeSet(pMacPkt, macAckFnc, pkt->ackParam);
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
    for(segmentHeader = &ptrPacket->payload; segmentHeader != 0; segmentHeader = segmentHeader->nextSegment)
    {
        memcpy(pDest, segmentHeader->dataLocation, segmentHeader->segmentLen);
        destLen += segmentHeader->segmentLen;
        pDest += segmentHeader->segmentLen;
    }


    pSeg->segLen += destLen;
    if(segFlags)
    {
        pSeg->segFlags |= segFlags;
    }
    
}


// TX packet acknowledge function
static bool TCPIP_IPV6_MacPacketTxAck(TCPIP_MAC_PACKET* pMacPkt,  const void* param)
{
    TCPIP_PKT_PacketFree(pMacPkt);
    return false;
}

static void TCPIP_IPV6_MacPacketTxPutHeader(IPV6_PACKET* pkt, TCPIP_MAC_PACKET* pMacPkt, uint16_t pktType)
{
    TCPIP_MAC_ETHERNET_HEADER* pMacHdr = (TCPIP_MAC_ETHERNET_HEADER*)pMacPkt->pMacLayer;

    memcpy(pMacHdr->DestMACAddr.v, &pkt->remoteMACAddr, sizeof(TCPIP_MAC_ADDR));
    memcpy(pMacHdr->SourceMACAddr.v, ((TCPIP_NET_IF*)pkt->netIfH)->netMACAddr.v, sizeof(TCPIP_MAC_ADDR));
    pMacHdr->Type = TCPIP_Helper_htons(pktType);

    // the 1st segment contains the MAC header
    pMacPkt->pDSeg->segLen += sizeof(TCPIP_MAC_ETHERNET_HEADER);

}

// RX MAC packet related functions
// pMacLayer is BaseReadPtr
// pNetLayer is currRdPtr for MACGetArray

static TCPIP_MAC_PTR_TYPE MACSetBaseReadPtr(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PTR_TYPE address)
{
	unsigned char* oldPtr;

	oldPtr = pRxPkt->pMacLayer;
	pRxPkt->pMacLayer = (unsigned char*)address;
	return (TCPIP_MAC_PTR_TYPE)oldPtr;
}

static void MACSetReadPtrInRx(TCPIP_MAC_PACKET* pRxPkt, uint16_t offset)
{
	pRxPkt->pNetLayer = pRxPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER) + offset;
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
	if(address)
	{
		memcpy(address, pRxPkt->pNetLayer, len);
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
    return MACGetArray(pRxPkt, val, len);
}

uint8_t TCPIP_IPV6_Get (TCPIP_MAC_PACKET* pRxPkt, uint8_t* pData)
{
    return MACGetArray(pRxPkt, pData, 1);
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

void TCPIP_IPV6_SetRemoteMacAddress(IPV6_PACKET * ptrPacket, const TCPIP_MAC_ADDR* pMacAdd)
{
    memcpy (ptrPacket->remoteMACAddr.v, pMacAdd, sizeof (TCPIP_MAC_ADDR));
}

void TCPIP_IPV6_SetPacketMacAcknowledge(IPV6_PACKET * ptrPacket, TCPIP_MAC_PACKET_ACK_FUNC macAckFnc)
{
    ptrPacket->macAckFnc = macAckFnc;
}

int TCPIP_IPV6_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF* pIf = _TCPIPStackHandleToNetUp(netH);

    if(pIf != 0)
    {
        IPV6_INTERFACE_CONFIG*  pIpv6Config = ipv6Config + TCPIP_STACK_NetIxGet(pIf);
        return pIpv6Config->linkMTU - sizeof(IPV6_HEADER);
    }

    return 0;
}

#endif  // defined(TCPIP_STACK_USE_IPV6)


