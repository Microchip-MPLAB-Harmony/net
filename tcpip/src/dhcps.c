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

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DHCP_SERVER

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/dhcps_private.h"

#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DHCP_SERVER)

//#warning "The dhcps module is obsolete. The new dhcp_server module should be used!"

//ICMP Request and Reply global variables

static TCPIP_DHCPS_ICMP_PROCESS_TYPE dhcpsEchoRequestState=TCPIP_DHCPS_ECHO_REQUEST_IDLE;
static uint16_t             dhcpsicmpSequenceNo;         // current sequence number
static uint16_t             dhcpsicmpIdentifier;         // current ID number

static size_t               dhcpsicmpReqNo;              // number of requests to send
static size_t               dhcpsicmpReqCount;           // current request counter
static size_t               dhcpsicmpAckRecv;            // number of acks
static size_t               dhcpsicmpReqDelay;
static DHCPS_ICMP_PROCESS   dhcpsicmpProcessSteps = DHCPS_ICMP_IDLE;
static uint32_t             dhcpsicmpStartTick;
static TCPIP_NET_HANDLE     dhcpsicmpNetH = NULL;
static void TCPIP_DHCPS_EchoICMPRequestTask(void);
static void                 DHCPSPingHandler(const  TCPIP_ICMP_ECHO_REQUEST* pEchoReq, TCPIP_ICMP_REQUEST_HANDLE iHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result, const void* param);

static IPV4_ADDR            dhcpsicmpTargetAddr;         // current target address
static uint8_t              dhcpsicmpPingBuff[TCPIP_DHCPS_ICMP_ECHO_REQ_BUFF_SIZE];
static uint16_t             dhcpsicmpPingSize = (uint16_t)TCPIP_DHCPS_ICMP_ECHO_REQ_DATA_SIZE;
static TCPIP_ICMP_REQUEST_HANDLE dhcpsicmpReqHandle;     // current transaction handle
static DHCPS_RESULT F_DHCPS_VerifyEchoRequestedAddress(void);
static DHCPS_RESULT F_DHCPS_EchoRequestInit(const TCPIP_NET_IF *pNetIf,IPV4_ADDR *requestedIpAddr);

// DHCP server Magic bytes "DHCP"
#define TCPIP_DHCPS_MAGIC_COOKIE 0x63538263UL

#define TCPIP_DHCPS_MAX_RECEIVED_BUFFER_SIZE 480
static DHCPS_HASH_DCPT gPdhcpsHashDcpt = { 0,0};
static const TCPIP_NET_IF* gpDhcpsNetH = NULL;
static DHCPS_MOD    dhcps_mod;
static BOOTP_HEADER gBOOTPHeader;
// DHCP is running on all interfaces
static DHCP_SRVR_DCPT      *gPdhcpSDcpt=NULL;
static const void*          dhcpSMemH = NULL;        // memory handle

static int                  dhcpSInitCount = 0;     // initialization count

static uint32_t             sysFreq = 0U;           // current running frequency

static void F_DHCPSUpdateEntry(DHCPS_HASH_ENTRY* dhcpsHE);
static bool F_DHCPS_GetOptionLen(TCPIP_DHCPS_DATA *inputBuf,uint8_t *optionVal,uint8_t *optionLen);
static void DHCPReplyToDiscovery(const TCPIP_NET_IF* pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf);
static void DHCPReplyToRequest(const TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, bool bAccept, bool bRenew,DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf,DHCP_SRVR_DCPT * pDhcpsDcpt);
static DHCPS_RESULT F_DHCPS_FindValidAddressFromPool(BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,IPV4_ADDR *reqIPAddress);
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_DHCPServerCleanup(void);
#else
#define F_DHCPServerCleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
static DHCPS_RESULT preAssignToDHCPClient(const TCPIP_NET_IF* pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt);
static bool isMacAddrEffective(const TCPIP_MAC_ADDR *macAddr);
static void DHCPSReplyToInform(const TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, DHCP_SRVR_DCPT* pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,bool bAccept,TCPIP_DHCPS_DATA *getBuf);
static void F_DHCPSrvClose(TCPIP_NET_IF* pNetIf, bool disable);
static size_t DHCPSgetFreeHashIndex(OA_HASH_DCPT* pOH, void* key, IPV4_ADDR *ipAddr, IPV4_ADDR *ipAddrExists);
static DHCPS_RESULT DHCPSLocateRequestedIpAddress(IPV4_ADDR *requestedIpAddr);
static  DHCPS_RESULT DHCPSRemoveHashEntry(TCPIP_MAC_ADDR* hwAdd, const uint8_t* pIPAddr);
static size_t TCPIP_DHCPS_CopyDataArrayToProcessBuff(uint8_t *val ,TCPIP_DHCPS_DATA *putbuf,size_t len);
static void TCPIP_DHCPS_DataCopyToProcessBuffer(uint8_t val ,TCPIP_DHCPS_DATA *putBuf);
static DHCPS_RESULT F_DHCPSAddCompleteEntry(size_t intfIdx, const uint8_t* pIPAddr, TCPIP_MAC_ADDR* hwAdd, DHCPS_ENTRY_FLAGS entryFlag);
static bool F_DHCPSDescriptorGetFromIntf(const TCPIP_NET_IF *pNetIf,uint32_t *dcptIdx);
static bool F_DCHPS_FindRequestIPAddress(TCPIP_DHCPS_DATA *inputBuf,uint8_t *reqAddr);
static bool F_DHCPS_Enable(TCPIP_NET_HANDLE hNet,bool checkIfUp);
static bool F_DHCPS_StartOperation(TCPIP_NET_IF* pNetIf,DHCP_SRVR_DCPT* pDhcpsDcpt);
static bool F_DHCPS_ValidatePktReceivedIntf(const TCPIP_NET_IF *pNetIfFromDcpt);

static void TCPIP_DHCPS_TaskForLeaseTime(void);
static void TCPIP_DHCPS_Process(void);
static void TCPIP_DHCPSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

#if ((TCPIP_DHCPS_DEBUG_LEVEL & TCPIP_DHCPS_DEBUG_MASK_BASIC) != 0)
// a Basic debug Print and Message

static void F_DHCPSDbgCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("DHCP Server Cond: %s, in line: %d, \r\n", message, lineNo);
    }
}

#else
#define F_DHCPSDbgCond(cond, message, lineNo)
#endif  // (TCPIP_DHCPS_DEBUG_LEVEL & TCPIP_DHCPS_DEBUG_MASK_BASIC)

#if ((TCPIP_DHCPS_DEBUG_LEVEL & TCPIP_DHCPS_DEBUG_MASK_ICMP) != 0)
static void F_DhcpsICMPDebugPrint(uint16_t requestCount,char* icmpMessage)
{
    if(icmpMessage == NULL)
    {
        SYS_CONSOLE_PRINT("ICMP : Request count:: %d \r\n", requestCount);
    }
    else
    {
        SYS_CONSOLE_PRINT("ICMP : %s :: %d \r\n", icmpMessage,requestCount);
    }
}
static void F_DhcpsICMPDebugMessage(char* icmpMessage)
{  
    SYS_CONSOLE_PRINT("ICMP : %s ", icmpMessage);
}
#else
#define F_DhcpsICMPDebugPrint(requestCount,icmpMessage)
#define F_DhcpsICMPDebugMessage(icmpMessage)
#endif  // (TCPIP_DHCPS_DEBUG_LEVEL & TCPIP_DHCPS_DEBUG_MASK_ICMP)

// conversion functions/helpers
//
static __inline__ DHCPS_HASH_ENTRY* __attribute__((always_inline)) FC_OaHash2DhcpsHash(OA_HASH_ENTRY* he)
{
    union
    {
        OA_HASH_ENTRY*    he;
        DHCPS_HASH_ENTRY* dhe;
    }U_OA_HASH_DHCPS_HASH;

    U_OA_HASH_DHCPS_HASH.he = he;
    return U_OA_HASH_DHCPS_HASH.dhe;
}

static  void F_DHCPSSetHashEntry(DHCPS_HASH_ENTRY* dhcpsHE, DHCPS_ENTRY_FLAGS newFlags, TCPIP_MAC_ADDR* hwAdd, const uint8_t* pIPAddr)
{
    dhcpsHE->hEntry.flags.value &= ~(uint16_t)DHCPS_FLAG_ENTRY_VALID_MASK;
    dhcpsHE->hEntry.flags.value |= (uint16_t)newFlags;
    
    if(hwAdd != NULL)
    {
        dhcpsHE->hwAdd = *hwAdd;        
        (void)memcpy(dhcpsHE->ipAddress.v, pIPAddr, sizeof(dhcpsHE->ipAddress));
    }
    
    dhcpsHE->Client_Lease_Time = SYS_TMR_TickCountGet();
    dhcpsHE->pendingTime = SYS_TMR_TickCountGet();
}

static  void F_DHCPSRemoveCacheEntries(DHCPS_HASH_DCPT* pDHCPSHashDcpt)
{
    if(pDHCPSHashDcpt->hashDcpt != NULL)
    {
        TCPIP_OAHASH_EntriesRemoveAll(pDHCPSHashDcpt->hashDcpt);
    }
}

static __inline__ void __attribute__((always_inline)) F_DHCPSStateSet(TCPIP_DHCPS_STATE_STATUS newState)
{
    dhcps_mod.smState = newState;
}

static  DHCPS_RESULT DHCPSRemoveHashEntry(TCPIP_MAC_ADDR* hwAdd, const uint8_t* pIPAddr)
{
    DHCPS_HASH_DCPT *pDhcpsHashDcpt;
    OA_HASH_ENTRY*  hE;

    pDhcpsHashDcpt = &gPdhcpsHashDcpt;
    if(hwAdd != NULL)
    {
        hE = TCPIP_OAHASH_EntryLookup(pDhcpsHashDcpt->hashDcpt,hwAdd);
        if(hE != NULL)
        {
            if(TCPIP_DHCPS_HashIPKeyCompare(pDhcpsHashDcpt->hashDcpt, hE, pIPAddr)== 0)
            {
                TCPIP_OAHASH_EntryRemove(pDhcpsHashDcpt->hashDcpt, hE);
                return DHCPS_RES_OK;
            }
        }
    }

    return DHCPS_RES_NO_ENTRY;
    
}

static void F_DHCPSUpdateEntry(DHCPS_HASH_ENTRY* dhcpsHE)
{    
     dhcpsHE->Client_Lease_Time = SYS_TMR_TickCountGet();
     dhcpsHE->pendingTime = 0;

     dhcpsHE->hEntry.flags.value &= ~(uint16_t)DHCPS_FLAG_ENTRY_VALID_MASK;
     dhcpsHE->hEntry.flags.value |= (uint16_t)DHCPS_FLAG_ENTRY_COMPLETE;
}

// validate the IP address pool from the DHCP server configuration and poolCnt returns the valid pool numbers
static void dhcpServPoolAddressValidation(const TCPIP_DHCPS_MODULE_CONFIG* pDhcpsConfig, size_t nIfx, uint32_t *poolCnt)
{
    uint32_t ix=0;
    uint32_t tempPoolCnt=0U;
    const char *tempStr = {"0.0.0.0"};
    IPV4_ADDR   serverIPAddress;
    IPV4_ADDR   startIPv4PoolAddress;
    IPV4_ADDR   netMask;

    *poolCnt =tempPoolCnt;
    
    for(ix=0;ix<pDhcpsConfig->dhcpServerCnt;ix++)
    {
        if(pDhcpsConfig->dhcpServer == NULL) 
        {
            break;
        }
        
        pDhcpsConfig->dhcpServer[ix].poolEnabled = false;
        // if the interface index of dhcp configuration is greater than the 
        //number of interface supported, then that DHCP  lease address pool configuration 
        // should not be considered

        if(pDhcpsConfig->dhcpServer[ix].interfaceIndex >= nIfx)
        {
            continue;
        }
        if((pDhcpsConfig->dhcpServer[ix].serverIPAddress == NULL)||
            (pDhcpsConfig->dhcpServer[ix].startIPAddRange == NULL) ||
              (pDhcpsConfig->dhcpServer[ix].ipMaskAddress == NULL) )
        {
            continue;
        }

         // if the server address or netmask or ip address range is zero, 
        //then that pool should not be considered
        if((strcmp(pDhcpsConfig->dhcpServer[ix].serverIPAddress,tempStr)==0)||
           (strcmp(pDhcpsConfig->dhcpServer[ix].startIPAddRange,tempStr)==0) ||
           (strcmp(pDhcpsConfig->dhcpServer[ix].ipMaskAddress,tempStr)==0))
        {
            continue;
        }

        // if start IP address range and serverIp address are not in the same 
        // subnet , then don't consider that pool
//Server Ip address
        (void)TCPIP_Helper_StringToIPAddress(pDhcpsConfig->dhcpServer[ix].serverIPAddress,&serverIPAddress);
//start IP address range
        (void)TCPIP_Helper_StringToIPAddress(pDhcpsConfig->dhcpServer[ix].startIPAddRange,&startIPv4PoolAddress);
//NetMask
        (void)TCPIP_Helper_StringToIPAddress(pDhcpsConfig->dhcpServer[ix].ipMaskAddress,&netMask);        
        if((serverIPAddress.Val & netMask.Val) != (startIPv4PoolAddress.Val & netMask.Val))
        {
            continue;
        }
        if(startIPv4PoolAddress.Val>serverIPAddress.Val)
        {
            tempPoolCnt++;
            pDhcpsConfig->dhcpServer[ix].poolEnabled = true;
        }
    }
    // if there is no pool, then pool count make it as 1 and use the default values for the server configuration.
    if(tempPoolCnt==0U)
    {
        F_DHCPSDbgCond(false, " Debug Message: No pool and Default Server configuration is use. This condition should not come.", __LINE__);
        tempPoolCnt=1U;
    }
    *poolCnt = tempPoolCnt;
}

// DHCP server descriptor update has been done at the init  time only.
static void F_DHCPS_AddressPoolDescConfiguration(const TCPIP_DHCPS_MODULE_CONFIG* pDhcpsConfig)
{
    uint32_t poolCount=0U;
    TCPIP_DHCPS_ADDRESS_CONFIG *pPoolServer=NULL;
    DHCP_SRVR_DCPT *pServerDcpt=NULL;
    uint32_t ix=0U,localPoolIndex=0U;


    poolCount = dhcps_mod.poolCount;    
    if(gPdhcpSDcpt == NULL)
    {
        return;
    }
    
    for(ix=0;ix<poolCount;ix++)
    {
        if(pDhcpsConfig->dhcpServer == NULL)
        {
            break;
        }
        
        pPoolServer = pDhcpsConfig->dhcpServer + ix;
        if(pPoolServer->poolEnabled == false)
        {
            continue;
        }

        pServerDcpt = gPdhcpSDcpt+ix;
        // Server IPv4 address
        (void)TCPIP_Helper_StringToIPAddress(pPoolServer->serverIPAddress,&pServerDcpt->intfAddrsConf.serverIPAddress);
        //NET Mask
        (void)TCPIP_Helper_StringToIPAddress(pPoolServer->ipMaskAddress,&pServerDcpt->intfAddrsConf.serverMask);
        // Server start of IPv4 address pool
        (void)TCPIP_Helper_StringToIPAddress(pPoolServer->startIPAddRange,&pServerDcpt->intfAddrsConf.startIPAddress);
#if defined(TCPIP_STACK_USE_DNS)
        // primary DNS server
        (void)TCPIP_Helper_StringToIPAddress(pPoolServer->priDNS,&pServerDcpt->intfAddrsConf.serverDNS);
        // Secondary DNS server
        (void)TCPIP_Helper_StringToIPAddress(pPoolServer->secondDNS,&pServerDcpt->intfAddrsConf.serverDNS2);
#endif
        pServerDcpt->intfAddrsConf.poolIndex = localPoolIndex;
        pServerDcpt->netIx = pPoolServer->interfaceIndex;

        localPoolIndex++;
       
    }
    
    // Get the default pool configuration
    if(localPoolIndex == 0U)
    {
        pServerDcpt = gPdhcpSDcpt+localPoolIndex;
        (void)memset(&pServerDcpt->intfAddrsConf,0,sizeof(pServerDcpt->intfAddrsConf));
        pServerDcpt->netIx = (size_t)TCPIP_STACK_NetIndexGet(TCPIP_STACK_NetDefaultGet());
    }
}

bool TCPIP_DHCPS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{    
    size_t hashMemSize=0;
    OA_HASH_DCPT*   hashDcpt;
    const TCPIP_DHCPS_MODULE_CONFIG* pDhcpsConfig;    
    uint32_t poolCnt=0;

    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        if(stackCtrl->pNetIf->Flags.bIsDHCPSrvEnabled != 0U)
        {
            (void)TCPIP_DHCPS_Enable(stackCtrl->pNetIf);
        }
        dhcpSMemH = stackCtrl->memH;
        return true;
    }
    
    
    // stack init
    if(dhcpSInitCount == 0)
    {
        pDhcpsConfig = (const TCPIP_DHCPS_MODULE_CONFIG*)initData;        
        if(pDhcpsConfig == NULL)
        {
            return false;
        }
        // first time we're run
        sysFreq = SYS_TMR_TickCounterFrequencyGet();
        // store the memory allocation handle
        dhcpSMemH = stackCtrl->memH;
        dhcpServPoolAddressValidation(pDhcpsConfig,stackCtrl->nIfs,&poolCnt);
        if(poolCnt != 0U)
        {
            gPdhcpSDcpt = (DHCP_SRVR_DCPT*)TCPIP_HEAP_Calloc(dhcpSMemH, poolCnt, sizeof(DHCP_SRVR_DCPT));
            if(gPdhcpSDcpt == NULL)
            {   // failed
                return false;
            }
        }
        else
        {
            SYS_ERROR(SYS_ERROR_WARNING, "DHCPS: Initialization failed! \r\n");
            return false;
        }

        hashMemSize = sizeof(OA_HASH_DCPT) + pDhcpsConfig->leaseEntries * sizeof(DHCPS_HASH_ENTRY);

        hashDcpt = (OA_HASH_DCPT*)TCPIP_HEAP_Malloc(dhcpSMemH, hashMemSize);

        if(hashDcpt == NULL)
        {   // failed
        // Remove all the DHCPS lease entries.
            // Free DHCP server Pool entries
            (void)TCPIP_HEAP_Free(dhcpSMemH,gPdhcpSDcpt);
            return false;
        }
        // populate the entries
        hashDcpt->memBlk = hashDcpt+1;
        hashDcpt->hParam = &gPdhcpsHashDcpt;
        hashDcpt->hEntrySize = sizeof(DHCPS_HASH_ENTRY);
        hashDcpt->hEntries = pDhcpsConfig->leaseEntries;
        hashDcpt->probeStep = DHCPS_HASH_PROBE_STEP;

        hashDcpt->hashF= &TCPIP_DHCPS_MACHashKeyHash;
#if defined(OA_DOUBLE_HASH_PROBING)
        hashDcpt->probeHash = &TCPIP_DHCPS_HashProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
        hashDcpt->cpyF = &TCPIP_DHCPS_HashMACKeyCopy;
        hashDcpt->delF = &TCPIP_DHCPS_HashDeleteEntry;
        hashDcpt->cmpF = &TCPIP_DHCPS_HashMACKeyCompare;
        TCPIP_OAHASH_Initialize(hashDcpt);
        gPdhcpsHashDcpt.hashDcpt = hashDcpt;
        gPdhcpsHashDcpt.leaseDuartion = pDhcpsConfig->entrySolvedTmo;

        if(pDhcpsConfig->deleteOldLease)
        {   // remove the old entries, if there
            F_DHCPSRemoveCacheEntries(&gPdhcpsHashDcpt);
        }
        
        dhcps_mod.signalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_DHCPS_Task, TCPIP_DHCPS_TASK_PROCESS_RATE);
        if(dhcps_mod.signalHandle == NULL)
        {
            F_DHCPServerCleanup();
            return false;
        }
        dhcps_mod.smServer = DHCP_SERVER_IDLE;
        dhcps_mod.uSkt = INVALID_UDP_SOCKET;
        dhcps_mod.poolCount = poolCnt;
        dhcps_mod.dhcpNextLease.Val = 0;
        dhcps_mod.smState = TCPIP_DHCPS_STATE_IDLE;
        (void)memset(&gBOOTPHeader,0,sizeof(gBOOTPHeader));

        // expected that max number of pool entry is similar to the interface index
        // copy the valid interface details to the global dhcps descriptor table
       F_DHCPS_AddressPoolDescConfiguration(pDhcpsConfig);
    }
    
    if(stackCtrl->pNetIf->Flags.bIsDHCPSrvEnabled != 0U)
    {   // override the pDhcpsConfig->dhcpEnable passed with the what the stack manager says
        (void)F_DHCPS_Enable(stackCtrl->pNetIf,false);
    }
       
    // Reset state machine and flags to default values

    dhcpSInitCount++;

    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_DHCPS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    if(dhcpSInitCount > 0)
    {   // we're up and running
        // this interface is going down no matter what
        F_DHCPSrvClose(stackCtrl->pNetIf,true);
        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--dhcpSInitCount == 0)
            {   // all closed
                // release resources
                F_DHCPServerCleanup();
                dhcpSMemH = NULL;
            }
        }
    }
}

static void F_DHCPServerCleanup(void)
{
    // Free HASH descriptor 
    if(gPdhcpsHashDcpt.hashDcpt != NULL)
    {
        // Remove all the HASH entries
        F_DHCPSRemoveCacheEntries(&gPdhcpsHashDcpt);
        (void)TCPIP_HEAP_Free(dhcpSMemH,gPdhcpsHashDcpt.hashDcpt);
        gPdhcpsHashDcpt.hashDcpt = NULL;
    }
    // Free Pool entry DHCP server descriptor
    if(gPdhcpSDcpt != NULL)
    {
        (void)TCPIP_HEAP_Free(dhcpSMemH,gPdhcpSDcpt);
    }
    // Free timer handler
    if(dhcps_mod.signalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(dhcps_mod.signalHandle);
        dhcps_mod.signalHandle = NULL;
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void F_DHCPSrvClose(TCPIP_NET_IF* pNetIf, bool disable)
{
    size_t netIx;
    const TCPIP_NET_IF* pIf=NULL;
    bool serverIsEnabled=false;
    if(disable)
    {
        pNetIf->Flags.bIsDHCPSrvEnabled = 0U;
    }
    for(netIx = 0;netIx < TCPIP_STACK_NumberOfNetworksGet(); netIx++)
    {
        pIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
        if(pIf->Flags.bIsDHCPSrvEnabled != 0U)
        {
            serverIsEnabled=true;
            break;
        }
    }
    // it means that  there is no server is enabled and so close the DHCP server socket
    if(serverIsEnabled == false)
    {
        if( dhcps_mod.uSkt != INVALID_UDP_SOCKET)
        {
            (void)TCPIP_UDP_Close(dhcps_mod.uSkt);
            dhcps_mod.uSkt = INVALID_UDP_SOCKET;
            dhcps_mod.smServer = DHCP_SERVER_IDLE;
            F_DHCPSStateSet(TCPIP_DHCPS_STATE_IDLE);
        }
    }
    else
    {
        dhcps_mod.smServer = DHCP_SERVER_LISTEN;
    }
    
}

static bool F_DHCPS_ValidatePktReceivedIntf(const TCPIP_NET_IF *pNetIfFromDcpt)
{
    if(TCPIPStackHandleToNetLinked(pNetIfFromDcpt) == NULL)
    {
        return false;
    }
    if(TCPIPStackNetAddress(pNetIfFromDcpt) == 0U)
    {
        return false;
    }
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
    // Make sure we don't clobber anyone else's DHCP server
    if(TCPIP_DHCP_IsServerDetected(pNetIfFromDcpt)|| TCPIP_DHCP_IsEnabled(pNetIfFromDcpt))
    {
        return false;
    }
#endif
    if(!TCPIP_DHCPS_IsEnabled(pNetIfFromDcpt))
    {
        return false;
    }
    return true;
}

static bool TCPIP_DHCPS_GetDataFromUDPBuff(TCPIP_DHCPS_DATA *getbuf,uint8_t *val)
{
    int nBytes = getbuf->endPtr - getbuf->wrPtr;
    if(nBytes == 0)
    {
        return false;
    }
    *val = getbuf->wrPtr[0];
    getbuf->wrPtr = getbuf->wrPtr+1;    
    return true;
}

static size_t TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(TCPIP_DHCPS_DATA *getbuf,uint8_t *buf, size_t nBytes)
{
    size_t avlBytes = FC_PtrDiff2UI32(getbuf->endPtr - getbuf->wrPtr);
    if(avlBytes == 0U)
    {
        return 0U;
    }

    if(nBytes < avlBytes)
    {
        avlBytes =  nBytes;
    }
    if(buf == NULL)
    {
        getbuf->wrPtr = getbuf->wrPtr + avlBytes;
        return avlBytes;
    }
    (void)memcpy(buf,getbuf->wrPtr,avlBytes);
    getbuf->wrPtr = getbuf->wrPtr+avlBytes;
    return avlBytes;
}

void TCPIP_DHCPS_Task(void)
{
    uint16_t sigPend;
    DHCPS_RESULT res;
    
    sigPend = (uint16_t)TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(dhcps_mod.uSkt == INVALID_UDP_SOCKET) 
    {
        return;
    }

    if((sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    { //  RX signal occurred
        TCPIP_DHCPS_Process();
    }
    
    if(dhcps_mod.smServer == DHCP_SERVER_ICMP_PROCESS)
    {
        // run ICMP ECHO request task
        TCPIP_DHCPS_EchoICMPRequestTask();
        res = F_DHCPS_VerifyEchoRequestedAddress();
        if(res == DHCPS_RES_ECHO_IN_PROCESS)
        {
            return;
        }
        else if(res == DHCPS_RES_ENTRY_EXIST)
        {
            //update global variable that can be used for @HS
            dhcps_mod.smState = TCPIP_DHCPS_GET_NEW_ADDRESS;
            dhcps_mod.smServer = DHCP_SERVER_LISTEN;
            return;
        }
        else if(res == DHCPS_RES_NO_ENTRY)
        {
            //update global variable that can be used for @HS
            dhcps_mod.smState = TCPIP_DHCPS_SEND_OFFER;
            dhcps_mod.smServer = DHCP_SERVER_LISTEN;
        }
        else
        {
            // do nothing
        }
    }
        
    if((sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred
        TCPIP_DHCPS_TaskForLeaseTime();
    }

}

// send a signal to the DHCPS module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void TCPIP_DHCPSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_DHCPS_Process(void)
{
    uint16_t            buffsize=0;
    uint8_t         i;
    uint8_t     Option, Len;    
    uint32_t        dw;
    bool        bAccept=false, bRenew=false;
    UDP_SOCKET          s;
    OA_HASH_ENTRY       *hE;
    DHCPS_HASH_DCPT     *pdhcpsHashDcpt;
    DHCP_SRVR_DCPT*     pDhcpsDcpt=NULL;
    UDP_SOCKET_INFO     udpSockInfo;
    const TCPIP_NET_IF* pNetIfFromDcpt=NULL;
    uint32_t            ix=0;
    uint8_t             getBuffer[TCPIP_DHCPS_MAX_RECEIVED_BUFFER_SIZE];
    TCPIP_DHCPS_DATA    udpGetBufferData = {0};
    IPV4_ADDR           ClientIP = {0};
    TCPIP_DHCPS_STATE_STATUS dhcpsSmSate;
    BOOTP_HEADER       BOOTPHeader;
    
    s = dhcps_mod.uSkt;
    dhcpsSmSate = dhcps_mod.smState;
    if(gPdhcpSDcpt == NULL)
    {
        return;
    }
      
    pdhcpsHashDcpt = &gPdhcpsHashDcpt;

    bool doRet = false;

    while(true)
    {
        switch(dhcpsSmSate)
        {
            case TCPIP_DHCPS_STATE_IDLE:
                doRet = true;
                break;

            case TCPIP_DHCPS_START_RECV_NEW_PACKET:
                
            // Check to see if a valid DHCP packet has arrived
                buffsize = TCPIP_UDP_GetIsReady(s);
                if(buffsize == 0U) 
                {
                    doRet = true;
                    break;
                }
                if(buffsize < (uint16_t)TCPIP_DHCPS_MIN_DISCOVERY_PKT_SIZE)
                {
                    (void)TCPIP_UDP_Discard(s);
                    continue;
                }
                if(dhcps_mod.smServer == DHCP_SERVER_ICMP_PROCESS)
                {
                    (void)TCPIP_UDP_Discard(s);
                    continue;
                }
                (void)memset(getBuffer,0,sizeof(getBuffer));
                dhcpsSmSate = TCPIP_DHCPS_DETECT_VALID_INTF;
                break;
                
            case TCPIP_DHCPS_DETECT_VALID_INTF:

                (void)memset(&udpSockInfo, 0, sizeof(udpSockInfo));
                (void)TCPIP_UDP_SocketInfoGet(s, &udpSockInfo);
                pNetIfFromDcpt = (const TCPIP_NET_IF*)udpSockInfo.hNet;
                // check if DHCP server is enabled or Not for this incoming packet interface
                if(!F_DHCPS_ValidatePktReceivedIntf(pNetIfFromDcpt))
                {
                    (void)TCPIP_UDP_Discard(s);
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }
                gpDhcpsNetH = pNetIfFromDcpt;
                dhcpsSmSate = TCPIP_DHCPS_PARSE_RECVED_PACKET;
                break;
                
            case TCPIP_DHCPS_PARSE_RECVED_PACKET:
                if(buffsize > (uint16_t)sizeof(getBuffer))
                {
                    buffsize = (uint16_t)sizeof(getBuffer);
                }
                udpGetBufferData.head = getBuffer;
                udpGetBufferData.wrPtr = udpGetBufferData.head;
                udpGetBufferData.endPtr = udpGetBufferData.head+buffsize;
                // Get Complete Array of DHCP server Reply bytes
                (void)TCPIP_UDP_ArrayGet(s,udpGetBufferData.head, buffsize);
               // discard the current packet and point to the next queued packet
                (void)TCPIP_UDP_Discard(s);

                // Retrieve the BOOTP header
                if(TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData, (uint8_t*)&BOOTPHeader, sizeof(BOOTPHeader)) == 0U)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }
                
                //copy the local BOOTP_Header to gBOOTPHeader 
                (void)memcpy(&gBOOTPHeader,&BOOTPHeader,sizeof(BOOTPHeader));
                hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &gBOOTPHeader.ClientMAC);
                if(hE != NULL)
                {
                    ClientIP = gBOOTPHeader.ClientIP;
                    if(TCPIP_DHCPS_HashIPKeyCompare(pdhcpsHashDcpt->hashDcpt, hE, ClientIP.v) == 0)
                    {
                        bRenew= true;
                        bAccept = true;
                    }
                    else
                    {
                        bRenew = false;
                        bAccept = false;
                    }
                }
                else if(gBOOTPHeader.ClientIP.Val == 0x00000000u)
                {
                    bRenew = false;
                    bAccept = true;
                }
                else
                {
                    bRenew = false;
                    bAccept = false;
                }

                // Validate first three fields
                if((gBOOTPHeader.MessageType != 1u) || (gBOOTPHeader.HardwareType != 1u) || (gBOOTPHeader.HardwareLen != 6u))
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }

                // Throw away 10 unused bytes of hardware address,
                // server host name, and boot file name -- unsupported/not needed.
                if(TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData,NULL, DHCPS_UNUSED_BYTES_FOR_TX) == 0U)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }
      
                // Obtain Magic Cookie and verify DHCP
                if(TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData,(uint8_t*)&dw, sizeof(uint32_t)) == 0U)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }

                if(dw != TCPIP_DHCPS_MAGIC_COOKIE)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }
                dhcpsSmSate = TCPIP_DHCPS_FIND_DESCRIPTOR;
                break;

             case TCPIP_DHCPS_FIND_DESCRIPTOR:
                ix=0;
                // get the Descriptor index from the interface
                if(F_DHCPSDescriptorGetFromIntf(pNetIfFromDcpt,&ix) == false)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    // the interface is not configured for DHCP Server
                    doRet = true;
                    break;
                }
                // assign the dhcpDescriptor value
                pDhcpsDcpt = gPdhcpSDcpt+ix;
                dhcpsSmSate = TCPIP_DHCPS_MESSAGE_TYPE;
                break;

            case   TCPIP_DHCPS_MESSAGE_TYPE:  
                // Obtain options //while(true)
                if(F_DHCPS_GetOptionLen(&udpGetBufferData,&Option,&Len)!= true)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }

                if(Option == DHCP_MESSAGE_TYPE)
                {
                    if(!TCPIP_DHCPS_GetDataFromUDPBuff(&udpGetBufferData,&i))
                    {
                        dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                        continue;
                    }
                    if(i == DHCP_DISCOVER_MESSAGE)
                    {
                        if(F_DCHPS_FindRequestIPAddress(&udpGetBufferData,ClientIP.v)!= true)
                        {
                            dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                            continue;
                        }
                        else
                        { // 
                            /* Validating if the requested IP address should not be part of any Hash Entry */
                            if(DHCPSLocateRequestedIpAddress(&ClientIP) != DHCPS_RES_OK)
                            { // use the alternate address
                                dhcps_mod.dhcpNextLease.Val = 0;
                            }
                            else
                            {// address is present
                                dhcps_mod.dhcpNextLease.Val = ClientIP.Val;
                            }
                            // Find the new address
                            if(F_DHCPS_FindValidAddressFromPool(&gBOOTPHeader,pDhcpsDcpt,pdhcpsHashDcpt,(IPV4_ADDR*)NULL) != DHCPS_RES_OK)
                            {
                                dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                                continue;
                            }
                            if(dhcps_mod.dhcpNextLease.Val != 0U)
                            {
                                dhcpsSmSate = TCPIP_DHCPS_START_ICMP_PROCESS;
                            }
                            else
                            {
                                dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                                continue;
                            }
                        }                        
                    }
                    else if(i==DHCP_REQUEST_MESSAGE)
                    {
                        DHCPReplyToRequest(pNetIfFromDcpt,&gBOOTPHeader, bAccept,bRenew,pdhcpsHashDcpt,&udpGetBufferData,pDhcpsDcpt);
                        dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                        (void)memset(&gBOOTPHeader,0,sizeof(gBOOTPHeader));
                    }
                    // Need to handle these if supporting more than one DHCP lease
                    else if((i== DHCP_RELEASE_MESSAGE) ||(i==DHCP_DECLINE_MESSAGE))
                    {
                        ClientIP = gBOOTPHeader.ClientIP;
                        (void)DHCPSRemoveHashEntry(&gBOOTPHeader.ClientMAC, ClientIP.v);
                        dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                        (void)memset(&gBOOTPHeader,0,sizeof(gBOOTPHeader));
                    }
                    else if(i==DHCP_INFORM_MESSAGE)
                    {
                        DHCPSReplyToInform(pNetIfFromDcpt,&gBOOTPHeader,pDhcpsDcpt,pdhcpsHashDcpt,bAccept,&udpGetBufferData);
                        dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                        (void)memset(&gBOOTPHeader,0,sizeof(gBOOTPHeader));
                    }
                    else
                    {
                        // do nothing
                    }
                }
               
                // Remove any unprocessed bytes that we don't care about
                if(TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData,NULL, (size_t)Len) == 0U)
                {
                    break;
                }
                Len = 0;
                F_DHCPSStateSet(dhcpsSmSate);
                break;

            case   TCPIP_DHCPS_START_ICMP_PROCESS:
                if(F_DHCPS_EchoRequestInit((const TCPIP_NET_IF *)gpDhcpsNetH,(IPV4_ADDR *)&dhcps_mod.dhcpNextLease) == DHCPS_RES_ECHO_IN_PROCESS)
                {
                    dhcps_mod.smServer = DHCP_SERVER_ICMP_PROCESS;
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    F_DHCPSStateSet(dhcpsSmSate);
                    doRet = true;
                    break;
                }                
                break;

            case TCPIP_DHCPS_GET_NEW_ADDRESS:
                 // Find the new address // this is the address search with previous address value which was failed 
                // during ICMP processing
                ix=0;
                // get the Descriptor index from the interface
                if(F_DHCPSDescriptorGetFromIntf(gpDhcpsNetH,&ix) == false)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    F_DHCPSStateSet(dhcpsSmSate);
                    // the interface is not configured for DHCP Server
                    doRet = true;
                    break;
                }

                // assign the dhcpDescriptor value
                pDhcpsDcpt = gPdhcpSDcpt+ix;
                if(F_DHCPS_FindValidAddressFromPool(&gBOOTPHeader,pDhcpsDcpt,pdhcpsHashDcpt,(IPV4_ADDR*)&dhcps_mod.dhcpNextLease) != DHCPS_RES_OK)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    F_DHCPSStateSet(dhcpsSmSate);
                    continue;
                }
                if(dhcps_mod.dhcpNextLease.Val != 0U)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_ICMP_PROCESS;
                    F_DHCPSStateSet(dhcpsSmSate);
                }
                else
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    F_DHCPSStateSet(dhcpsSmSate);
                    continue;
                }
                break;

            case TCPIP_DHCPS_SEND_OFFER:
                 ix=0;
                // get the Descriptor index from the interface
                if(F_DHCPSDescriptorGetFromIntf(gpDhcpsNetH,&ix) == false)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    F_DHCPSStateSet(dhcpsSmSate);
                    // the interface is not configured for DHCP Server
                    doRet = true;
                    break;
                }
                // assign the dhcpDescriptor value
                pDhcpsDcpt = gPdhcpSDcpt+ix;
                DHCPReplyToDiscovery(gpDhcpsNetH,&gBOOTPHeader,pDhcpsDcpt,pdhcpsHashDcpt,&udpGetBufferData);
                dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                F_DHCPSStateSet(dhcpsSmSate);
                break;

            default:
                doRet = true;
                break;
        }

        if(doRet)
        {
            return;
        }
    }    
}

static void TCPIP_DHCPS_DataCopyToProcessBuffer(uint8_t val ,TCPIP_DHCPS_DATA *putBuf)
{
    if(putBuf->wrPtr < putBuf->endPtr)
    {
        *putBuf->wrPtr++ = val;
    }
}

static size_t TCPIP_DHCPS_CopyDataArrayToProcessBuff(uint8_t *val ,TCPIP_DHCPS_DATA *putbuf,size_t len)
{
    ptrdiff_t diffBytes = putbuf->endPtr - putbuf->wrPtr;
    if(diffBytes <= 0)
    {
        return 0U;
    }

    size_t nBytes = (size_t)diffBytes;
    if(len < nBytes)
    {
        nBytes = len;
    }
    if(nBytes == 0U)
    {
        return 0U;
    }
    if(val == NULL)
    {
        putbuf->wrPtr = putbuf->wrPtr+nBytes;
        return 0U;
    }
    (void)memcpy(putbuf->wrPtr, val,nBytes);
    putbuf->wrPtr += nBytes;
    return nBytes;
}

static bool F_DHCPS_GetOptionLen(TCPIP_DHCPS_DATA *inputBuf,uint8_t *optionVal,uint8_t *optionLen)
{
     // Get option type
    if(!TCPIP_DHCPS_GetDataFromUDPBuff(inputBuf,(uint8_t *)optionVal))
    {
        return false;
    }
    if(*optionVal == DHCP_END_OPTION)
    {
        return false;
    }

    // Get option length
    if(!TCPIP_DHCPS_GetDataFromUDPBuff(inputBuf,(uint8_t *)optionLen))
    {
        return false;
    }
    
    return true;
}

static bool F_DCHPS_FindRequestIPAddress(TCPIP_DHCPS_DATA *inputBuf,uint8_t *reqAddr)
{
    
    while(inputBuf->endPtr - inputBuf->wrPtr != 0)
    {
        uint8_t Option, Len = 0;

        if(F_DHCPS_GetOptionLen(inputBuf,&Option,&Len) != true)
        {
            if(Option != 0xFFU)
            {
                return false;
            }
        }

        switch(Option)
        {
            case DHCP_PARAM_REQUEST_IP_ADDRESS:
                if(TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(inputBuf,(uint8_t*)reqAddr,4) == 0U)
                {
                    break;
                }
                if(Len != 4U)
                {
                    break;
                }                              
                break;

            default:
                // do nothing
                break;
        }
        // Remove the unprocessed bytes that we don't care about
        if(TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(inputBuf,NULL, (size_t)Len) == 0U)
        {
            break;
        }
        Len = 0U;
    }
    return true;
}



//Send DHCP Discovery message
static void DHCPReplyToDiscovery(const TCPIP_NET_IF *pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf)
{
    size_t         i;
    DHCPS_RESULT    res;
    UDP_SOCKET      s;
    OA_HASH_ENTRY   *hE;
    TCPIP_DHCPS_DATA   putBuffer;
    DHCPS_HASH_ENTRY *dhcpsHE = NULL;
    BOOTP_HEADER    rxHeader;
    uint32_t magicCookie = 0x63538263;
    TCPIP_DHCPS_OPTION optionType;
    uint32_t    optionTypeTotalLen=0;

    if(getBuf == NULL)
    {
        return;
    }
    // Set the correct socket to active and ensure that
    // enough space is available to generate the DHCP response
    s =dhcps_mod.uSkt;
    if(TCPIP_UDP_TxPutIsReady(s, DHCPS_MAX_REPONSE_PACKET_SIZE) < DHCPS_MAX_REPONSE_PACKET_SIZE)
    {
        return;
    }
//this will put the start pointer at the beginning of the TX buffer
    (void)TCPIP_UDP_TxOffsetSet(s,0,false);
    
    //dhcps_mod.dhcpNextLease.Val = 0;
    (void)memset((void*)&rxHeader,0,sizeof(BOOTP_HEADER));
 
    // find in pool
    res = preAssignToDHCPClient(pNetIf,Header,pDhcpsDcpt,pdhcpsHashDcpt);
    if( res != DHCPS_RES_OK) 
    {
        return;
    }

// Before sending OFFER, get the perfect Hash Entry
    hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC);
    if(hE != NULL)
    {
        dhcpsHE = FC_OaHash2DhcpsHash(hE);
    }

//Get the write pointer:
    putBuffer.head=putBuffer.wrPtr = TCPIP_UDP_TxPointerGet(s);
    // set the write and end pointer for DHCP server data
    putBuffer.wrPtr = putBuffer.head;
    putBuffer.endPtr = putBuffer.head+DHCPS_MAX_REPONSE_PACKET_SIZE;

    // Begin putting the BOOTP Header and DHCP options
    rxHeader.MessageType = BOOT_REPLY;
    // Reply with the same Hardware Type, Hardware Address Length, Hops, and Transaction ID fields
    rxHeader.HardwareType = Header->HardwareType;
    rxHeader.HardwareLen = Header->HardwareLen;
    // BOOTP number of HOPs
    rxHeader.Hops = Header->Hops;
    // BOOTP transaction ID
    rxHeader.TransactionID = Header->TransactionID;
    // Seconds Elaplsed .. Not yet supported
    rxHeader.SecondsElapsed = 0; // NOT USED
    // BOOTP header Flag
    rxHeader.BootpFlags = Header->BootpFlags;
    // Client IP address, only filled in if client is in
    // BOUND, RENEW or REBINDING state and can respond to ARP requests.
    rxHeader.ClientIP.Val = 0; // Not yet Assigned
    // Leased IP address or Client IP address
    rxHeader.YourIP.Val = dhcpsHE->ipAddress.Val;
    // IP address of next server to use in bootstrap;
    //returned in DHCPOFFER, DHCPACK by server.
    rxHeader.NextServerIP.Val = pNetIf->netIPAddr.Val;
    // Relay agent IP address, used in booting via a relay agent.
    rxHeader.RelayAgentIP.Val = 0;
    // Client MAC address
    (void)memcpy(&rxHeader.ClientMAC,&Header->ClientMAC,sizeof(TCPIP_MAC_ADDR));

    // copy the BOOT RX header contents to the processing Buffer
    (void)TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&rxHeader,&putBuffer,sizeof(BOOTP_HEADER));

    // Remaining 10 bytes of client hardware address, server host name: Null string (not used)
    for(i = 0; i < DHCPS_UNUSED_BYTES_FOR_TX; i++)  
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer);
    }
    (void)TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t *)&magicCookie, &putBuffer, sizeof(magicCookie));

    // Options: DHCP Offer
    optionType.messageType.optionType = DHCP_MESSAGE_TYPE;
    optionType.messageType.optionTypeLen = 1;
    optionType.messageType.byteVal = DHCP_OFFER_MESSAGE;
    optionTypeTotalLen = sizeof(optionType.messageType);
  
    // Option: Subnet Mask
    optionType.subnetmaskType.optionType = DHCP_SUBNET_MASK;
    optionType.subnetmaskType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.subnetmaskType.intVal = pNetIf->netMask.Val;
    optionTypeTotalLen += sizeof(optionType.subnetmaskType);
   
    // Option: Server identifier
    optionType.serverIdentifierType.optionType = DHCP_SERVER_IDENTIFIER;
    optionType.serverIdentifierType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.serverIdentifierType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.serverIdentifierType);

    // Option: Router/Gateway address
    optionType.routerType.optionType = DHCP_ROUTER;
    optionType.routerType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.routerType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.routerType);
  
    // Option: DNS server address
    optionType.dnsType.optionType = DHCP_DNS;
    optionType.dnsType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.dnsType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.dnsType);
    
     // Option: Lease duration
    optionType.ipLeaseTimeType.optionType = DHCP_IP_LEASE_TIME;
    optionType.ipLeaseTimeType.optionTypeLen = 4;
    optionType.ipLeaseTimeType.intVal = pdhcpsHashDcpt->leaseDuartion;
    optionType.ipLeaseTimeType.intVal = TCPIP_Helper_htonl(optionType.ipLeaseTimeType.intVal);
    optionTypeTotalLen += sizeof(optionType.ipLeaseTimeType);
  
    // copy the DHCP Reply Option type contents to the processing Buffer
    (void)TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&optionType,&putBuffer,optionTypeTotalLen);

    // No more options, mark ending
    TCPIP_DHCPS_DataCopyToProcessBuffer(DHCP_END_OPTION,&putBuffer);

    // Add zero padding to ensure compatibility with old BOOTP relays that discard small packets (<300 UDP octets)
    while(putBuffer.wrPtr < putBuffer.endPtr)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer);
    }

    // Force remote destination address to be the broadcast address, regardless
    // of what the node's source IP address was (to ensure we don't try to
    // unicast to 0.0.0.0).
    (void)TCPIP_UDP_BcastIPV4AddressSet( s,UDP_BCAST_NETWORK_LIMITED,pNetIf);

    IP_MULTI_ADDRESS tmp_MultiAddr; tmp_MultiAddr.v4Add = pNetIf->netIPAddr;
    (void)TCPIP_UDP_SourceIPAddressSet(s,IP_ADDRESS_TYPE_IPV4,&tmp_MultiAddr);

    // Once it is completed writing into the buffer, you need to update the Tx offset again,
    // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
    (void)TCPIP_UDP_TxOffsetSet(s, FC_PtrDiff2UI16(putBuffer.wrPtr - putBuffer.head), false);

    // Transmit the packet
    (void)TCPIP_UDP_Flush(s);

}

// Replies to a DHCP Inform message.
static void DHCPSReplyToInform(const TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, DHCP_SRVR_DCPT* pDhcpsDcpt,
                                DHCPS_HASH_DCPT *pdhcpsHashDcpt,bool bAccept,TCPIP_DHCPS_DATA *getBuf)
{
    size_t         i;
    UDP_SOCKET      s;
    OA_HASH_ENTRY       *hE;
    TCPIP_DHCPS_DATA   putBuffer;
    BOOTP_HEADER    rxHeader;
    uint32_t magicCookie = 0x63538263;
    TCPIP_DHCPS_OPTION optionType;
    IPV4_ADDR           ClientIP;
    uint32_t    optionTypeTotalLen=0;

    if(getBuf == NULL)
    {
        return;
    }
    // Set the correct socket to active and ensure that
    // enough space is available to generate the DHCP response
    s = dhcps_mod.uSkt;
    if(TCPIP_UDP_TxPutIsReady(s, DHCPS_MAX_REPONSE_PACKET_SIZE) < DHCPS_MAX_REPONSE_PACKET_SIZE)
    {
        return;
    }
    //this will put the start pointer at the beginning of the TX buffer
    (void)TCPIP_UDP_TxOffsetSet(s,0,false);
    
    // Search through all remaining options and look for the Requested IP address field
    // Obtain options

    while(getBuf->endPtr - getBuf->wrPtr != 0)
    {
        uint8_t Option, Len;
        TCPIP_MAC_ADDR tmp_MacAddr;

        // Get option type
        if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t *)&Option))
        {
            return;
        }
        if(Option == DHCP_END_OPTION)
        {
            break;
        }

        // Get option length
        if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t *)&Len))
        {
            return;
        }
 
        if((Option == DHCP_PARAM_REQUEST_CLIENT_ID) && (Len == 7u))
        {
            // Get the requested IP address and see if it is the one we have on offer.  
            // If not, we should send back a NAK, but since there could be some other
            // DHCP server offering this address, we'll just silently ignore this request.
            if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t *)&i))
            {
                return;
            }
            if(TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,(uint8_t*)&tmp_MacAddr, 6U) == 0U)
            {
                return;
            }
            Len -= 7U;
            hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &tmp_MacAddr);
            ClientIP.Val = boot_header->ClientIP.Val;
            if(hE != NULL)
            {
                if(TCPIP_DHCPS_HashIPKeyCompare(pdhcpsHashDcpt->hashDcpt, hE, ClientIP.v) == 0)
                {
                    bAccept = true;
                }
                else
                {
                    bAccept = false;
                }
            }
            else
            {
                // do a IP address validation .check if the Ip address in the same subnet
                // as per IP address range is decided per interface.
                if((pNetIf->netIPAddr.Val & pNetIf->netMask.Val)== (ClientIP.Val & pNetIf->netMask.Val))
                {
                    if(F_DHCPSAddCompleteEntry(pDhcpsDcpt->netIx, ClientIP.v, &boot_header->ClientMAC, DHCPS_FLAG_ENTRY_COMPLETE) != DHCPS_RES_OK)
                    {
                        return;
                    }
                }
                bAccept = false;
            }
            break;
        }

        // Remove the unprocessed bytes that we don't care about
        if(TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,NULL, (size_t)Len) == 0U)
        {
            break;
        }
    }
    if(!bAccept)
    {
        return;
    }


     //Get the write pointer:
    putBuffer.head=putBuffer.wrPtr = TCPIP_UDP_TxPointerGet(s);
    // set the write and end pointer for DHCP server data
    putBuffer.wrPtr = putBuffer.head;
    putBuffer.endPtr = putBuffer.head+DHCPS_MAX_REPONSE_PACKET_SIZE;


    (void)memset((void*)&rxHeader,0,sizeof(BOOTP_HEADER));
    // Begin putting the BOOTP Header and DHCP options
//    TCPIP_DHCPS_DataCopyToProcessBuffer(BOOT_REPLY,&putBuffer);
    rxHeader.MessageType = BOOT_REPLY;
    // Reply with the same Hardware Type, Hardware Address Length, Hops, and Transaction ID fields
    rxHeader.HardwareType = boot_header->HardwareType;
    rxHeader.HardwareLen = boot_header->HardwareLen;
    // BOOTP number of HOPs
    rxHeader.Hops = boot_header->Hops;
    // BOOTP transaction ID
    rxHeader.TransactionID = boot_header->TransactionID;
    // Seconds Elaplsed .. Not yet supported
    rxHeader.SecondsElapsed = 0; // NOT USED
    // BOOTP header Flag
    rxHeader.BootpFlags = boot_header->BootpFlags;
    // Client IP address, only filled in if client is in
    // BOUND, RENEW or REBINDING state and can respond to ARP requests.
    rxHeader.ClientIP.Val = boot_header->ClientIP.Val; // Not yet Assigned
    // Leased IP address or Client IP address
    rxHeader.YourIP.Val = 0;
    // IP address of next server to use in bootstrap;
    //returned in DHCPOFFER, DHCPACK by server.
    rxHeader.NextServerIP.Val = pNetIf->netIPAddr.Val;
    // Relay agent IP address, used in booting via a relay agent.
    rxHeader.RelayAgentIP.Val = 0;
    // Client MAC address
    (void)memcpy(&rxHeader.ClientMAC,&boot_header->ClientMAC,sizeof(TCPIP_MAC_ADDR));

    // copy the BOOT RX header contents to the processing Buffer
    (void)TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&rxHeader,&putBuffer,sizeof(BOOTP_HEADER));
    
    for(i = 0; i < DHCPS_UNUSED_BYTES_FOR_TX; i++)  // Remaining 10 bytes of client hardware address, server host name: Null string (not used)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer); // Boot filename: Null string (not used)
    }
    (void)TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&magicCookie, &putBuffer, sizeof(magicCookie));

    // Options: DHCP lease ACKnowledge
    optionType.messageType.optionType = DHCP_OPTION_ACK_MESSAGE;
    optionType.messageType.optionTypeLen = 1;
    optionType.messageType.byteVal = DHCP_ACK_MESSAGE;
    optionTypeTotalLen = sizeof(optionType.messageType);

  // Option: Subnet Mask
    optionType.subnetmaskType.optionType = DHCP_SUBNET_MASK;
    optionType.subnetmaskType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.subnetmaskType.intVal = pNetIf->netMask.Val;
    optionTypeTotalLen += sizeof(optionType.subnetmaskType);

    // Option: Server identifier
    optionType.serverIdentifierType.optionType = DHCP_SERVER_IDENTIFIER;
    optionType.serverIdentifierType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.serverIdentifierType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.serverIdentifierType);
  

    // Option: Router/Gateway address
    optionType.routerType.optionType = DHCP_ROUTER;
    optionType.routerType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.routerType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.routerType);

    // Option: DNS server address
    optionType.dnsType.optionType = DHCP_DNS;
    optionType.dnsType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.dnsType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.dnsType);

    // copy the DHCP Reply Option type contents to the processing Buffer
    (void)TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&optionType,&putBuffer, (size_t)optionTypeTotalLen);

    // No more options, mark ending
    TCPIP_DHCPS_DataCopyToProcessBuffer(DHCP_END_OPTION,&putBuffer);

    // Add zero padding to ensure compatibility with old BOOTP relays that discard small packets (<300 UDP octets)
    while(putBuffer.wrPtr < putBuffer.endPtr)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer);
    }
// Put completed for DHCP packet buffer to the UDP buffer


    IP_MULTI_ADDRESS tmp_MultiAddr; tmp_MultiAddr.v4Add = pNetIf->netIPAddr;
    (void)TCPIP_UDP_SourceIPAddressSet(s,IP_ADDRESS_TYPE_IPV4,&tmp_MultiAddr);

    // Once it is completed writing into the buffer, you need to update the Tx offset again,
    // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
    (void)TCPIP_UDP_TxOffsetSet(s, FC_PtrDiff2UI16(putBuffer.wrPtr - putBuffer.head), false);
    
    // Transmit the packet
    (void)TCPIP_UDP_Flush(s);
}

//Replies to a DHCP Request message.
static void DHCPReplyToRequest(const TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, bool bAccept, bool bRenew,
                        DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf,DHCP_SRVR_DCPT * pDhcpsDcpt)
{
    size_t         i;
    UDP_SOCKET      s;
    IPV4_ADDR       ipAddr;
    TCPIP_DHCPS_DATA   putBuffer;
    OA_HASH_ENTRY   *hE = NULL;
    BOOTP_HEADER    rxHeader;
    uint32_t magicCookie = 0x63538263;
    TCPIP_DHCPS_OPTION optionType;
    uint32_t    optionTypeTotalLen=0;


    if(getBuf == NULL)
    {
        return;
    }
    // Set the correct socket to active and ensure that
    // enough space is available to generate the DHCP response
    s =dhcps_mod.uSkt;
    if(TCPIP_UDP_TxPutIsReady(s, DHCPS_MAX_REPONSE_PACKET_SIZE) < DHCPS_MAX_REPONSE_PACKET_SIZE)
    {
         return;
    }
    //this will put the start pointer at the beginning of the TX buffer
    (void)TCPIP_UDP_TxOffsetSet(s,0,false);
    // Search through all remaining options and look for the Requested IP address field
    // Obtain options
    while(getBuf->endPtr - getBuf->wrPtr != 0)
    {
        uint8_t Option, Len;
        uint32_t dw;
        TCPIP_MAC_ADDR tmp_MacAddr;

        // Get option type
        if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t*)&Option))
        {
            return;
        }
        if(Option == DHCP_END_OPTION)
        {
            break;
        }

        // Get option length
        if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t*)&Len))
        {
            return;
        }

        // Process option
        if(bRenew)
        {
            if((Option == DHCP_PARAM_REQUEST_CLIENT_ID) && (Len == 7u))
            {
                // Get the requested IP address and see if it is the one we have on offer.  If not, we should send back a NAK, but since there could be some other DHCP server offering this address, we'll just silently ignore this request.
                if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t*)&i))
                {
                    return;
                }
                if(TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,(uint8_t*)&tmp_MacAddr, 6) == 0U)
                {
                    return;
                }
                Len -= 7U;
                hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &tmp_MacAddr);
                if(hE !=NULL)
                {
                    IPV4_ADDR ClientIP = boot_header->ClientIP;
                    if(TCPIP_DHCPS_HashIPKeyCompare(pdhcpsHashDcpt->hashDcpt, hE, ClientIP.v) == 0)
                    {
                        DHCPS_HASH_ENTRY * dhcpsHE = FC_OaHash2DhcpsHash(hE);
                        // update lease time;
                        F_DHCPSUpdateEntry(dhcpsHE);
                    }
                    else
                    {
                        TCPIP_OAHASH_EntryRemove(pdhcpsHashDcpt->hashDcpt,hE);
                        bAccept = false;
                    }
                }
                else
                {
                    bAccept = false;
                }
                continue;
            }
        }
        else
        {
            if((Option == DHCP_PARAM_REQUEST_IP_ADDRESS) && (Len == 4u))
            {
                // Get the requested IP address and see if it is the one we have on offer.  
                // If not, we should send back a NAK, but since there could be some other
                // DHCP server offering this address, we'll just silently ignore this request.
                if(TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,(uint8_t*)&dw, 4) == 0U)
                {
                    return;
                }

                Len -= 4U;
                hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &boot_header->ClientMAC);
                if(hE != NULL)
                {
                    if(TCPIP_DHCPS_HashIPKeyCompare(pdhcpsHashDcpt->hashDcpt, hE, (uint8_t*)&dw) == 0)
                    {
                        DHCPS_HASH_ENTRY * dhcpsHE = FC_OaHash2DhcpsHash(hE);
                        bAccept = true;
                        F_DHCPSUpdateEntry(dhcpsHE);
                    }
                    else
                    {
                        bAccept = false;
                        //remove Hash entry;
                        TCPIP_OAHASH_EntryRemove(pdhcpsHashDcpt->hashDcpt,hE);
                    }
                }
                else
                {
                    if((pNetIf->netIPAddr.Val & pNetIf->netMask.Val) != (dw & pNetIf->netMask.Val))
                    {
                        bAccept = false;
                        break;
                    }
                    if(F_DHCPSAddCompleteEntry(pNetIf->netIfIx, (uint8_t*)&dw, &boot_header->ClientMAC, DHCPS_FLAG_ENTRY_COMPLETE)!= DHCPS_RES_OK)
                    {
                        return ;
                    }
                }
                continue;
            }
        }
            // Remove the unprocessed bytes that we don't care about
        if(TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,NULL, (size_t)Len) == 0U)
        {
            break;
        }
        Len = 0U;
    }

    //Get the write pointer:
    putBuffer.head=putBuffer.wrPtr = TCPIP_UDP_TxPointerGet(s);
    // set the write and end pointer for DHCP server data
    putBuffer.wrPtr = putBuffer.head;
    putBuffer.endPtr = putBuffer.head+DHCPS_MAX_REPONSE_PACKET_SIZE;

    (void)memset((void*)&rxHeader,0,sizeof(BOOTP_HEADER));
    
// Begin putting the BOOTP Header and DHCP options
    rxHeader.MessageType = BOOT_REPLY;
    // Reply with the same Hardware Type, Hardware Address Length, Hops, and Transaction ID fields
    rxHeader.HardwareType = boot_header->HardwareType;
    rxHeader.HardwareLen = boot_header->HardwareLen;
    // BOOTP number of HOPs
    rxHeader.Hops = boot_header->Hops;
    // BOOTP transaction ID
    rxHeader.TransactionID = boot_header->TransactionID;
    // Seconds Elaplsed .. Not yet supported
    rxHeader.SecondsElapsed = 0; // NOT USED
    // BOOTP header Flag
    rxHeader.BootpFlags = boot_header->BootpFlags;
    if(bAccept)
    {
        hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &boot_header->ClientMAC);
        if(hE != NULL)
        {
            ipAddr.Val = FC_OaHash2DhcpsHash(hE)->ipAddress.Val;
        }
        else
        {
            bAccept =  false;
            ipAddr.Val = 0;
        }
    }
    else
    {
        ipAddr.Val=0u;
    }
    // Client IP address, only filled in if client is in
    // BOUND, RENEW or REBINDING state and can respond to ARP requests.
    rxHeader.ClientIP.Val = boot_header->ClientIP.Val; // Not yet Assigned
    // Leased IP address or Client IP address
    rxHeader.YourIP.Val = ipAddr.Val;
    // IP address of next server to use in bootstrap;
    //returned in DHCPOFFER, DHCPACK by server.
    rxHeader.NextServerIP.Val = 0;
    // Relay agent IP address, used in booting via a relay agent.
    rxHeader.RelayAgentIP.Val = 0;
    // Client MAC address
    (void)memcpy(&rxHeader.ClientMAC,&boot_header->ClientMAC,sizeof(TCPIP_MAC_ADDR));

    // copy the BOOT RX header contents to the processing Buffer
    (void)TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&rxHeader,&putBuffer,sizeof(BOOTP_HEADER));
    for(i = 0; i < DHCPS_UNUSED_BYTES_FOR_TX; i++)  // Remaining 10 bytes of client hardware address, server host name: Null string (not used)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer); // Boot filename: Null string (not used)
    }

    (void)TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&magicCookie, &putBuffer, sizeof(magicCookie));

    // Options: DHCP lease ACKnowledge
    optionType.messageType.optionType = DHCP_OPTION_ACK_MESSAGE;
    optionType.messageType.optionTypeLen = 1;
    if(bAccept)
    {
        optionType.messageType.byteVal = DHCP_ACK_MESSAGE;
    }
    else    // Send a NACK
    {
        optionType.messageType.byteVal = DHCP_NAK_MESSAGE;
    }
     optionTypeTotalLen = sizeof(optionType.messageType);
     
    optionType.renewalTime_t1.optionType = DHCP_PARAM_RENEWAL_TIME_OPTION;
    optionType.renewalTime_t1.optionTypeLen = 4;
    optionType.renewalTime_t1.intVal = TCPIP_Helper_htonl(pdhcpsHashDcpt->leaseDuartion / 2U);  // 0.5 * lease duration
    optionTypeTotalLen += sizeof(optionType.renewalTime_t1);
    
    optionType.rebindTime_t2.optionType = DHCP_PARAM_REBIND_TIME_OPTION;
    optionType.rebindTime_t2.optionTypeLen = 4;
    optionType.rebindTime_t2.intVal = TCPIP_Helper_htonl((pdhcpsHashDcpt->leaseDuartion * 175U) / 200U);    // 0.875 * lease duration
    optionTypeTotalLen += sizeof(optionType.rebindTime_t2);
    
     //    // Option: Subnet Mask
    optionType.subnetmaskType.optionType = DHCP_SUBNET_MASK;
    optionType.subnetmaskType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.subnetmaskType.intVal = pNetIf->netMask.Val;
    optionTypeTotalLen += sizeof(optionType.subnetmaskType);

     // Option: Server identifier
    optionType.serverIdentifierType.optionType = DHCP_SERVER_IDENTIFIER;
    optionType.serverIdentifierType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.serverIdentifierType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.serverIdentifierType);

    // Option: Router/Gateway address
    optionType.routerType.optionType = DHCP_ROUTER;
    optionType.routerType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.routerType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.routerType);

    // Option: DNS server address
    optionType.dnsType.optionType = DHCP_DNS;
    optionType.dnsType.optionTypeLen = (uint8_t)sizeof(IPV4_ADDR);
    optionType.dnsType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.dnsType);

     // Option: Lease duration
    optionType.ipLeaseTimeType.optionType = DHCP_IP_LEASE_TIME;
    optionType.ipLeaseTimeType.optionTypeLen = 4;
    optionType.ipLeaseTimeType.intVal = pdhcpsHashDcpt->leaseDuartion;
    optionType.ipLeaseTimeType.intVal = TCPIP_Helper_htonl(optionType.ipLeaseTimeType.intVal);
    optionTypeTotalLen += sizeof(optionType.ipLeaseTimeType);

     // copy the DHCP Reply Option type contents to the processing Buffer
    (void)TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&optionType,&putBuffer, (size_t)optionTypeTotalLen);
    
    // No more options, mark ending
    TCPIP_DHCPS_DataCopyToProcessBuffer(DHCP_END_OPTION,&putBuffer);

    // Add zero padding to ensure compatibility with old BOOTP relays that discard small packets (<300 UDP octets)
     while(putBuffer.wrPtr < putBuffer.endPtr)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer);
    }

//Put completed for DHCP packet buffer to the UDP buffer

    // Force remote destination address to be the broadcast address, regardless
    // of what the node's source IP address was (to ensure we don't try to
    // unicast to 0.0.0.0).
    if(false == bRenew)
    {
        (void)TCPIP_UDP_BcastIPV4AddressSet( s,UDP_BCAST_NETWORK_LIMITED,pNetIf);
    }

    IP_MULTI_ADDRESS tmp_MultiAddr; tmp_MultiAddr.v4Add = pNetIf->netIPAddr;
    (void)TCPIP_UDP_SourceIPAddressSet(s,IP_ADDRESS_TYPE_IPV4,&tmp_MultiAddr);

    // Once it is completed writing into the buffer, you need to update the Tx offset again,
    // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
    (void)TCPIP_UDP_TxOffsetSet(s, FC_PtrDiff2UI16(putBuffer.wrPtr - putBuffer.head), false);

    // Transmit the packet
    (void)TCPIP_UDP_Flush(s);
}

static bool isMacAddrEffective(const TCPIP_MAC_ADDR *macAddr)
{
    int i;
    for(i=0;i<6;i++)
    {
        if(macAddr->v[i] != 0U)
        {
            return true;
        }
    }
    return false;
}

static DHCPS_RESULT F_DHCPSAddCompleteEntry(size_t intfIdx, const uint8_t* pIPAddr, TCPIP_MAC_ADDR* hwAdd, DHCPS_ENTRY_FLAGS entryFlag)
{
    DHCPS_HASH_DCPT  *pdhcpsDcpt;
    OA_HASH_ENTRY   *hE;
    DHCPS_HASH_ENTRY* dhcpsHE;

    pdhcpsDcpt = &gPdhcpsHashDcpt;
    
    hE = TCPIP_OAHASH_EntryLookupOrInsert(pdhcpsDcpt->hashDcpt, hwAdd);
    if(hE == NULL)
    {   // oops, hash full?
        return DHCPS_RES_CACHE_FULL;
    }

    // now in cache
    dhcpsHE = FC_OaHash2DhcpsHash(hE);
    
    if(dhcpsHE->hEntry.flags.newEntry != 0U)
    {   // populate the new entry
        dhcpsHE->intfIdx = intfIdx;
        F_DHCPSSetHashEntry(dhcpsHE, entryFlag, hwAdd, pIPAddr);
    }
    else
    {   // existent entry
        F_DHCPSUpdateEntry(dhcpsHE);
    }

    return DHCPS_RES_OK;
}


static void TCPIP_DHCPS_TaskForLeaseTime(void)
{
    uint32_t current_timer = SYS_TMR_TickCountGet();
    DHCPS_HASH_ENTRY* dhcpsHE;
    DHCPS_HASH_DCPT  *pdhcpsDcpt;
    OA_HASH_ENTRY   *hE;
    size_t bktIx;
    OA_HASH_DCPT    *pOH;
    const TCPIP_NET_IF* pNetIf;

    pdhcpsDcpt = &gPdhcpsHashDcpt;
    pOH = pdhcpsDcpt->hashDcpt;

    if(gPdhcpSDcpt == NULL)
    {
        return;
    }
    
    pNetIf = (const TCPIP_NET_IF*)TCPIP_UDP_SocketNetGet(dhcps_mod.uSkt);

// check the lease values and if there is any entry whose lease value exceeds the lease duration remove the lease entries from the HASH.

    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        hE = TCPIP_OAHASH_EntryGet(pOH, bktIx);        
        if((hE->flags.busy != 0U) && (hE->flags.value & (uint16_t)DHCPS_FLAG_ENTRY_COMPLETE) != 0U)
        {
            dhcpsHE = FC_OaHash2DhcpsHash(hE);
            if((current_timer - dhcpsHE->Client_Lease_Time) >= pdhcpsDcpt->leaseDuartion* sysFreq)
            {
                dhcpsHE->Client_Lease_Time = 0;
                TCPIP_OAHASH_EntryRemove(pOH,hE);
            }
        }// Check if there is any entry whose DHCPS flag is INCOMPLETE, 
        // i,e DHCPS server did not receive the request from the client regarding that leased address.
        else if((hE->flags.busy != 0U) && (hE->flags.value & (uint16_t)DHCPS_FLAG_ENTRY_INCOMPLETE) != 0U)
        {
            dhcpsHE = FC_OaHash2DhcpsHash(hE);
            if((current_timer - dhcpsHE->pendingTime) >= (uint32_t)TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK* sysFreq)
            {
                dhcpsHE->pendingTime = 0U;
                TCPIP_OAHASH_EntryRemove(pOH,hE);
            }
        }
        else
        {
            // do nothing
        }
        // remove the entry if the link is down or Wifi Mac is not connected
        if(hE->flags.busy != 0U)
        {
            dhcpsHE = FC_OaHash2DhcpsHash(hE);
            pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(dhcpsHE->intfIdx);
            if(pNetIf != NULL && !TCPIP_STACK_NetworkIsLinked(pNetIf))
            {                
                dhcpsHE->pendingTime = 0;
                TCPIP_OAHASH_EntryRemove(pOH,hE);
            }
        }
    }
}

static DHCPS_RESULT F_DHCPS_FindValidAddressFromPool(BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,IPV4_ADDR *reqIPAddress)
{
    OA_HASH_ENTRY       *hE;
    IPV4_ADDR         tempIpv4Addr;
    size_t bktIx = 0;
    
    if(reqIPAddress != NULL)
    {
        tempIpv4Addr.Val = reqIPAddress->Val;                
    }
    else
    {
        tempIpv4Addr.Val = 0;
    }
    
    if(false == isMacAddrEffective(&(Header->ClientMAC))) 
    {
        return DHCPS_RES_NO_ENTRY;
    }
    
    // Find in Pool, look for the same MAC addr
    hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC);
    if(hE !=NULL)
    {
        if((DHCPSLocateRequestedIpAddress(&tempIpv4Addr) != DHCPS_RES_OK)&&(dhcps_mod.smState == TCPIP_DHCPS_GET_NEW_ADDRESS))
        {
            bktIx = DHCPSgetFreeHashIndex(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC,&pDhcpsDcpt->intfAddrsConf.startIPAddress,(IPV4_ADDR*)&tempIpv4Addr);
            if(bktIx == 0xFFFFFFFFU)
            {
                return DHCPS_RES_CACHE_FULL;
            }
            dhcps_mod.dhcpNextLease.v[0] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[0] & 0xFFU;
            dhcps_mod.dhcpNextLease.v[1] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[1] & 0xFFU;
            dhcps_mod.dhcpNextLease.v[2] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[2] & 0xFFU;
            dhcps_mod.dhcpNextLease.v[3] = (pDhcpsDcpt->intfAddrsConf.startIPAddress.v[3] & 0xFFU) + (uint8_t)bktIx;
        }
        return DHCPS_RES_OK;
    }
    else
    {
        if((dhcps_mod.dhcpNextLease.Val == 0U) || (dhcps_mod.smState == TCPIP_DHCPS_GET_NEW_ADDRESS))
        {
            bktIx = DHCPSgetFreeHashIndex(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC,&pDhcpsDcpt->intfAddrsConf.startIPAddress,(IPV4_ADDR*)&tempIpv4Addr);
            if(bktIx == 0xFFFFFFFFU)
            {
                return DHCPS_RES_CACHE_FULL;
            }
            dhcps_mod.dhcpNextLease.v[0] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[0] & 0xFFU;
            dhcps_mod.dhcpNextLease.v[1] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[1] & 0xFFU;
            dhcps_mod.dhcpNextLease.v[2] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[2] & 0xFFU;
            dhcps_mod.dhcpNextLease.v[3] = (pDhcpsDcpt->intfAddrsConf.startIPAddress.v[3] & 0xFFU) + (uint8_t)bktIx;
        }        
    }

    return DHCPS_RES_OK;
}


static DHCPS_RESULT preAssignToDHCPClient(const TCPIP_NET_IF  *pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt)
{
    OA_HASH_ENTRY       *hE;
    IPV4_ADDR         tempIpv4Addr;
    size_t bktIx = 0;

    if(false == isMacAddrEffective(&(Header->ClientMAC))) 
    {
        return DHCPS_RES_NO_ENTRY;
    }
    
    // Find in Pool, look for the same MAC addr
    hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC);
    if(hE !=NULL)
    {
        return DHCPS_RES_OK;
    }
    else
    {
        if(dhcps_mod.dhcpNextLease.Val == 0U)
        {
            bktIx = DHCPSgetFreeHashIndex(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC,&pDhcpsDcpt->intfAddrsConf.startIPAddress,(IPV4_ADDR*)NULL);
            if(bktIx == 0xFFFFFFFFU)
            {
                return DHCPS_RES_CACHE_FULL;
            }
            tempIpv4Addr.v[0] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[0] & 0xFFU;
            tempIpv4Addr.v[1] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[1] & 0xFFU;
            tempIpv4Addr.v[2] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[2] & 0xFFU;
            tempIpv4Addr.v[3] = (pDhcpsDcpt->intfAddrsConf.startIPAddress.v[3] & 0xFFU) + (uint8_t)bktIx;
        }
        else
        {
            tempIpv4Addr.Val = dhcps_mod.dhcpNextLease.Val;
        }
        /* this decided entry to the HASH POOL with a DHCPS_FLAG_ENTRY_INCOMPLETE flag
        After receiving Request and before sending ACK , make this entry to DHCPS_FLAG_ENTRY_COMPLETE
        */
        if(F_DHCPSAddCompleteEntry(pNetIf->netIfIx, tempIpv4Addr.v, &Header->ClientMAC, DHCPS_FLAG_ENTRY_INCOMPLETE) != DHCPS_RES_OK)
        {
            return DHCPS_RES_CACHE_FULL;
        }
    }

    return DHCPS_RES_OK;
}


int TCPIP_DHCPS_HashMACKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    const uint8_t* pKeySrc = FC_CVPtr2CU8(key); 
    DHCPS_HASH_ENTRY* dsEntry = FC_OaHash2DhcpsHash(hEntry); 
    return memcmp(dsEntry->hwAdd.v, pKeySrc, DHCPS_HASH_KEY_SIZE);
}

int TCPIP_DHCPS_HashIPKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const uint8_t* key)
{
    DHCPS_HASH_ENTRY* dsEntry = FC_OaHash2DhcpsHash(hEntry); 
    const uint8_t* pKeySrc = FC_CVPtr2CU8(key); 
    return memcmp(dsEntry->ipAddress.v, pKeySrc, sizeof((dsEntry)->ipAddress));
}

void TCPIP_DHCPS_HashIPKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    const uint8_t* pKeySrc = FC_CVPtr2CU8(key); 
    DHCPS_HASH_ENTRY* dsEntry = FC_OaHash2DhcpsHash(dstEntry); 
    (void)memcpy(dsEntry->ipAddress.v, pKeySrc, sizeof((dsEntry)->ipAddress));
}

void TCPIP_DHCPS_HashMACKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    const uint8_t* pKeySrc = FC_CVPtr2CU8(key); 
    DHCPS_HASH_ENTRY* dsEntry = FC_OaHash2DhcpsHash(dstEntry); 
    (void)memcpy(dsEntry->hwAdd.v,pKeySrc, DHCPS_HASH_KEY_SIZE);
}

OA_HASH_ENTRY* TCPIP_DHCPS_HashDeleteEntry(OA_HASH_DCPT* pOH)
{
    OA_HASH_ENTRY*  pBkt;
    size_t      bktIx;
    DHCPS_HASH_ENTRY  *pE;
    uint32_t current_timer = SYS_TMR_TickCountGet();
    
    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        pBkt = TCPIP_OAHASH_EntryGet(pOH, bktIx);       
        if(pBkt->flags.busy != 0U)
        {
            pE = FC_OaHash2DhcpsHash(pBkt);
            if((current_timer - pE->Client_Lease_Time) >= (uint32_t)TCPIP_DHCPS_LEASE_DURATION * sysFreq)
            {
                return pBkt;
            }
        }
    }
    return NULL;
}

// to get the free index and to use it for calculating Leased IP address.
static size_t DHCPSgetFreeHashIndex(OA_HASH_DCPT* pOH,void* key,IPV4_ADDR *ipAddr,IPV4_ADDR *ipAddrExists)
{
    OA_HASH_ENTRY*  pBkt;
    size_t      bktIx = 0xFFFFFFFFU;
    size_t      probeStep=0;
    size_t      bkts = 0;
    IPV4_ADDR   foundIP;
    TCPIP_UINT32_VAL   dw;
    TCPIP_UINT32_VAL   reqIP;

    
#if defined(OA_DOUBLE_HASH_PROBING)
    probeStep = TCPIP_DHCPS_HashProbeHash(pOH, key);
    if(probeStep == 0)
    {   // try to avoid probing the same bucket over and over again
            // when probeHash returns 0.
            probeStep = pOH->probeStep;
    }
#else
    probeStep = pOH->probeStep;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
    if(ipAddrExists != NULL) 
    {// this is added for ICMP processing 
        if(ipAddrExists->Val != 0U)
        {
            reqIP.Val = ipAddrExists->Val;
            dw.Val = 0;
            dw.vByte.LB = (reqIP.v[3] ^ ipAddr->v[3]);
            bktIx = (size_t)dw.Val;
        }
        else
        {
            reqIP.Val = 0;
            bktIx = 0xFFFFFFFFU;
        }
    }
    else
    {
        reqIP.Val = 0;
        bktIx = 0xFFFFFFFFU;
    }
    
    if((bktIx ==0xFFFFFFFFU)||(bktIx >  pOH->hEntries))
    {
        bktIx = TCPIP_DHCPS_MACHashKeyHash(pOH,key);
    }

    while(bkts < pOH->hEntries)
    {
        pBkt = TCPIP_OAHASH_EntryGet(pOH, bktIx);
        dw.Val = ipAddr->Val;
        dw.v[3] |= (uint8_t)bktIx;
        if((reqIP.Val == 0U) || (reqIP.Val != dw.Val))
        {        
            foundIP.Val = dw.Val;
            if((pBkt->flags.busy == 0U) && (DHCPSLocateRequestedIpAddress(&foundIP) == DHCPS_RES_NO_ENTRY))
            {   // found unused entry
                return bktIx;
            }
        }
        // advance to the next hash slot
        bktIx += probeStep;
        if(bktIx >= pOH->hEntries)
        {
            bktIx -= pOH->hEntries;
        }

        bkts++;
    }
    return 0xFFFFFFFFU;  // cache full, not found
}

static DHCPS_RESULT DHCPSLocateRequestedIpAddress(IPV4_ADDR *requestedIpAddr)
{
    DHCPS_HASH_DCPT *pdhcpsDcpt;
    OA_HASH_ENTRY   *hE;
    size_t          bktIx;
    OA_HASH_DCPT    *pOH;

    pdhcpsDcpt = &gPdhcpsHashDcpt;
    pOH = pdhcpsDcpt->hashDcpt;

// check the Requested IP address is matching anyone of the hash entry.
    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        hE = TCPIP_OAHASH_EntryGet(pOH, bktIx);
        if((hE->flags.busy != 0U) && (hE->flags.value & (uint16_t)DHCPS_FLAG_ENTRY_COMPLETE) != 0U)
        {
            if(TCPIP_DHCPS_HashIPKeyCompare(pOH, hE, requestedIpAddr->v) == 0)
            {
                return DHCPS_RES_OK;
            }
        }
    }
    return DHCPS_RES_NO_ENTRY;
}

TCPIP_DHCPS_LEASE_HANDLE TCPIP_DHCPS_LeaseEntryGet(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_LEASE_ENTRY* pLeaseEntry, TCPIP_DHCPS_LEASE_HANDLE leaseHandle)
{
    size_t              entryIx;
    OA_HASH_DCPT*       pOH;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*    pDSHashDcpt;
    uint32_t        current_time = SYS_TMR_TickCountGet();
    
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
  
    if(pNetIf == NULL || pNetIf->Flags.bIsDHCPSrvEnabled == 0U)
    {
        return NULL;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;
    if(pOH != NULL)
    {   // DHCP Server proper initialized
        for(entryIx = FC_CvPtr2U32(leaseHandle); entryIx < pOH->hEntries; entryIx++)
        {
            pDsEntry = FC_OaHash2DhcpsHash(TCPIP_OAHASH_EntryGet(pOH, entryIx));
            
            if(pDsEntry != NULL && (pDsEntry->hEntry.flags.busy != 0U) && (pDsEntry->hEntry.flags.value & (uint16_t)DHCPS_FLAG_ENTRY_COMPLETE) != 0U)
            {                
                if(pDsEntry->intfIdx != (size_t)TCPIP_STACK_NetIxGet(pNetIf))
                {
                    continue;
                }
                // found entry
                if(pLeaseEntry != NULL)
                {
                    (void)memcpy(&pLeaseEntry->hwAdd, &pDsEntry->hwAdd, sizeof(pDsEntry->hwAdd));
                    pLeaseEntry->ipAddress.Val = pDsEntry->ipAddress.Val;
                    pLeaseEntry->leaseTime = pDSHashDcpt->leaseDuartion * sysFreq - (current_time - pDsEntry->Client_Lease_Time);
                }
                return FC_U322CvPtr(entryIx + 1U);
            }
        }
    }
    // no other entry
    return NULL;
}

size_t TCPIP_DHCPS_GetPoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type)
{
    size_t              entryIx;
    size_t              noOfEntries=0;
    OA_HASH_DCPT*       pOH;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*    pDSHashDcpt;
    
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf == NULL || pNetIf->Flags.bIsDHCPSrvEnabled == 0U)
    {
        return 0;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;
    
    if(pOH != NULL)
    {   // DHCP Server proper initialized
        for(entryIx = 0; entryIx < pOH->hEntries; entryIx++)
        {
            pDsEntry = FC_OaHash2DhcpsHash(TCPIP_OAHASH_EntryGet(pOH, entryIx));
            
            switch(type)
            {
                case DHCP_SERVER_POOL_ENTRY_ALL:
                    if(pDsEntry != NULL && (pDsEntry->hEntry.flags.busy != 0U))
                    {
                        if(pDsEntry->intfIdx != (size_t)TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        noOfEntries++;
                    }
                    break;

                case DHCP_SERVER_POOL_ENTRY_IN_USE:
                    if(pDsEntry != NULL && (pDsEntry->hEntry.flags.busy != 0U) && (pDsEntry->hEntry.flags.value & (uint16_t)DHCPS_FLAG_ENTRY_COMPLETE) != 0U)
                    {
                        if(pDsEntry->intfIdx != (size_t)TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        noOfEntries++;
                    }
                    break;

                default:
                    // do nothing
                    break;
            }

        }
    }
    return noOfEntries;
        
}

bool TCPIP_DHCPS_LeaseEntryRemove(TCPIP_NET_HANDLE netH, TCPIP_MAC_ADDR* hwAdd)
{
    OA_HASH_DCPT*       pOH;
    OA_HASH_ENTRY*  hE;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*    pDSHashDcpt;
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf == NULL || pNetIf->Flags.bIsDHCPSrvEnabled == 0U)
    {
        return false;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;

    if(hwAdd != NULL)
    {
        hE = TCPIP_OAHASH_EntryLookup(pOH,hwAdd);
        if(hE != NULL)
        {
            pDsEntry = FC_OaHash2DhcpsHash(hE);
            if(pDsEntry->intfIdx == pNetIf->netIfIx)
            {
                TCPIP_OAHASH_EntryRemove(pOH,hE);
                return true;
            }
        }
    }
    return false;
}

bool TCPIP_DHCPS_RemovePoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type)
{
    size_t                 entryIx;
    OA_HASH_DCPT*       pOH;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*    pDSHashDcpt;
    
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf == NULL || pNetIf->Flags.bIsDHCPSrvEnabled == 0U)
    {
        return false;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;
    
    if(pOH != NULL)
    {   // DHCP Server proper initialized
        for(entryIx = 0; entryIx < pOH->hEntries; entryIx++)
        {
            pDsEntry = FC_OaHash2DhcpsHash(TCPIP_OAHASH_EntryGet(pOH, entryIx));
            
            switch(type)
            {
                case DHCP_SERVER_POOL_ENTRY_ALL:
                    if(pDsEntry != NULL && (pDsEntry->hEntry.flags.busy != 0U))
                    {
                        if(pDsEntry->intfIdx != (size_t)TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        TCPIP_OAHASH_EntryRemove(pOH,&pDsEntry->hEntry);
                    }
                    break;

                case DHCP_SERVER_POOL_ENTRY_IN_USE:
                    if(pDsEntry != NULL && (pDsEntry->hEntry.flags.busy != 0U) && (pDsEntry->hEntry.flags.value & (uint16_t)DHCPS_FLAG_ENTRY_COMPLETE) != 0U)
                    {
                        if(pDsEntry->intfIdx != (size_t)TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        TCPIP_OAHASH_EntryRemove(pOH,&pDsEntry->hEntry);
                    }
                    break;

                default:
                    // do nothing
                    break;
            }
        }
    }
    return true;
}

size_t TCPIP_DHCPS_MACHashKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32_hash(key, DHCPS_HASH_KEY_SIZE) % (pOH->hEntries);
}

#if defined(OA_DOUBLE_HASH_PROBING)
size_t TCPIP_DHCPS_HashProbeHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32a_hash(key, DHCPS_HASH_KEY_SIZE) % (pOH->hEntries);
}
#endif  // defined(OA_DOUBLE_HASH_PROBING)

size_t TCPIP_DHCPS_IPAddressHashKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32_hash(key, 4) % (pOH->hEntries);
}

static bool F_DHCPSDescriptorGetFromIntf(const TCPIP_NET_IF *pNetIf,uint32_t *dcptIdx)
{
    if(gPdhcpSDcpt == NULL)
    {
        return false;
    }

    uint32_t ix = 0U;
    DHCP_SRVR_DCPT* pDhcpServDcpt = gPdhcpSDcpt;
    for(ix = 0; ix < dhcps_mod.poolCount; ix++)
    {
        if(pNetIf->netIfIx != pDhcpServDcpt->netIx)
        {
            F_DHCPSDbgCond(false, " Debug Message: DHCP Server Interface Number is not matching to the Network configuration DHCP server enable status :  ", pDhcpServDcpt->netIx);
            pDhcpServDcpt++;
            continue;
        }
        else
        {
            *dcptIdx = pDhcpServDcpt->intfAddrsConf.poolIndex;
            return true;
        }
    }
    return false;
}


bool TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet)
{    
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
 
    if(pNetIf == NULL)
    {
        return false;
    }
//  Now stop DHCP server
    F_DHCPSrvClose(pNetIf,true);
    TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDR_SRVC_DHCPS, TCPIP_ADDR_SRVC_EV_USER_STOP);
    TCPIP_STACK_AddressServiceDefaultSet(pNetIf);
    TCPIPStackSetConfigAddress(pNetIf, NULL, NULL, NULL, true);
     // Remove all the HASH entries
    F_DHCPSRemoveCacheEntries(&gPdhcpsHashDcpt);
    return true;
}

static bool F_DHCPS_StartOperation(TCPIP_NET_IF* pNetIf,DHCP_SRVR_DCPT* pDhcpsDcpt)
{
    uint32_t poolIndex=0;

    if(gPdhcpSDcpt == NULL)
    {
        return false;
    }

    if( dhcps_mod.uSkt == INVALID_UDP_SOCKET)
    {
        dhcps_mod.uSkt = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, TCPIP_DHCP_SERVER_PORT,  NULL);
        if(dhcps_mod.uSkt == INVALID_UDP_SOCKET)
        {
            return false;
        }
        (void)TCPIP_UDP_SignalHandlerRegister(dhcps_mod.uSkt, TCPIP_UDP_SIGNAL_RX_DATA, &TCPIP_DHCPSSocketRxSignalHandler, NULL);
    }
    dhcps_mod.smServer = DHCP_SERVER_LISTEN;
    F_DHCPSStateSet(TCPIP_DHCPS_START_RECV_NEW_PACKET);
    pNetIf->Flags.bIsDHCPSrvEnabled = 1U;
    //if there is no pool for any interface , use the 0th index pool configuration details
    // or the default for the interface configuration. here the server IP address will be same for
    // more than interface, if the address pool is not configured.
    if(F_DHCPSDescriptorGetFromIntf(pNetIf,&poolIndex) == false)
    {
        return false;
    }
    pDhcpsDcpt = gPdhcpSDcpt+poolIndex;

// Get the network interface from the network index and configure IP address,
// Netmask and gateway and DNS
    TCPIPStackSetConfigAddress(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverIPAddress, &pDhcpsDcpt->intfAddrsConf.serverMask, NULL, false);
    TCPIP_STACK_GatewayAddressSet(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverIPAddress);
#if defined(TCPIP_STACK_USE_DNS)
    if(pNetIf->Flags.bIsDNSServerAuto != 0U)
    {
        TCPIP_STACK_PrimaryDNSAddressSet(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverDNS);
        TCPIP_STACK_SecondaryDNSAddressSet(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverDNS2);
    }
#endif
    
    size_t queueSize;
     // make sure the socket is created with enough TX space
    (void)TCPIP_UDP_OptionsGet(dhcps_mod.uSkt, UDP_OPTION_TX_QUEUE_LIMIT, (void*)&queueSize);
    if(queueSize < (size_t)TCPIP_DHCPS_QUEUE_LIMIT_SIZE)
    {
        void* pqSize = FC_Uint2VPtr((uint32_t)TCPIP_DHCPS_QUEUE_LIMIT_SIZE); 
        if(!TCPIP_UDP_OptionsSet(dhcps_mod.uSkt, UDP_OPTION_TX_QUEUE_LIMIT, pqSize))
        {
            return false;
        }
    }
    
     // make sure the socket is created with enough TX space
    (void)TCPIP_UDP_OptionsGet(dhcps_mod.uSkt, UDP_OPTION_RX_QUEUE_LIMIT, (void*)&queueSize);
    if(queueSize < (size_t)TCPIP_DHCPS_QUEUE_LIMIT_SIZE)
    {
        void* pqSize = FC_Uint2VPtr((uint32_t)TCPIP_DHCPS_QUEUE_LIMIT_SIZE); 
        if(!TCPIP_UDP_OptionsSet(dhcps_mod.uSkt, UDP_OPTION_RX_QUEUE_LIMIT, pqSize))
        {
            return false;
        }
    }
    return true;
}

bool TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet)
{
    return F_DHCPS_Enable(hNet, true);
}

static bool F_DHCPS_Enable(TCPIP_NET_HANDLE hNet,bool checkIfUp)
{
    TCPIP_NET_IF* pNetIf ;
    DHCP_SRVR_DCPT* pDhcpsDcpt;

    if(checkIfUp)
    {
        pNetIf = TCPIPStackHandleToNetUp(hNet);
    }
    else
    {
        pNetIf = TCPIPStackHandleToNet(hNet);
    }
    pDhcpsDcpt = gPdhcpSDcpt;
    if((pNetIf==NULL)||(pDhcpsDcpt==NULL))
    {
        return false;
    }
    if(dhcps_mod.poolCount == 0U)
    {
        SYS_ERROR(SYS_ERROR_WARNING, "DHCPS: DHCP server needs atleast one address pool! \r\n");
        return false;
    }
    if(checkIfUp)
    {
        if(TCPIP_STACK_AddressServiceCanStart(pNetIf, TCPIP_STACK_ADDR_SRVC_DHCPS))
        {
            return F_DHCPS_StartOperation(pNetIf,pDhcpsDcpt);
        }
    }
    else
    {
        if((pNetIf->Flags.v & (uint16_t)TCPIP_STACK_ADDR_SRVC_MASK) == (uint16_t)TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON)
        {
            return F_DHCPS_StartOperation(pNetIf,pDhcpsDcpt);
        }
    }
    SYS_ERROR(SYS_ERROR_WARNING, "DHCPS: Other Services are enabled for this interface \r\n");
    return false;
}

/*****************************************************************************
  Function:
    bool TCPIP_DHCPS_IsEnabled(CPIP_NET_HANDLE hNet)

  Summary:
    Determins if the DHCP Server is enabled on the specified interface.

  Description:
    Determins if the DHCP Server is enabled on the specified interface.

  Precondition:
    None

  Parameters:
     hNet- Interface to query.

  Returns:
    None
***************************************************************************/
bool TCPIP_DHCPS_IsEnabled(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(pNetIf != NULL)
    {
        return pNetIf->Flags.bIsDHCPSrvEnabled != 0U;
    }
    return false;
}

static DHCPS_RESULT F_DHCPS_EchoRequestInit(const TCPIP_NET_IF *pNetIf,IPV4_ADDR *requestedIpAddr)
{
    // Initialize all the echo request parameters
    dhcpsEchoRequestState = TCPIP_DHCPS_START_ECHO_REQUEST;
    dhcpsicmpNetH = (TCPIP_NET_HANDLE)pNetIf;
    dhcpsicmpTargetAddr = *requestedIpAddr;
    
    return DHCPS_RES_ECHO_IN_PROCESS;
}


static DHCPS_RESULT F_DHCPS_VerifyEchoRequestedAddress(void)
{
    DHCPS_RESULT res;

    switch(dhcpsicmpProcessSteps)
    {
        case DHCPS_ICMP_REQUEST_TIMEDOUT:
        case DHCPS_ICMP_REQUEST_CANCEL:
        case DHCPS_ICMP_NO_RESPONSE_RECEIVED:
            res = DHCPS_RES_NO_ENTRY;
            break;

        case DHCPS_ICMP_REQUEST_SENT:
        case DHCPS_ICMP_WAIT_REPLY:
        case DHCPS_ICMP_WRONG_RESPONSE_RECEIVED:
            res = DHCPS_RES_ECHO_IN_PROCESS;
            break;

        case DHCPS_ICMP_RESPONSE_RECEIVED:
            res = DHCPS_RES_ENTRY_EXIST;
            break;

        case DHCPS_ICMP_IDLE:
            res = DHCPS_RES_OK;
            break;

        default:
            res = DHCPS_RES_OK;            
            break;
    }
    return res;
}

static void TCPIP_DHCPS_EchoICMPRequestTask(void)
{
    ICMP_ECHO_RESULT echoRes;
    TCPIP_ICMP_ECHO_REQUEST echoRequest;
    bool cancelReq, newReq;
    bool killIcmp = false;
    char debugBuf[128];
    uint32_t    currentTime=0;
       
    switch(dhcpsEchoRequestState)
    {
        case TCPIP_DHCPS_START_ECHO_REQUEST:
        case TCPIP_DHCPS_DO_ECHO_REQUEST:
            if(dhcpsEchoRequestState == TCPIP_DHCPS_START_ECHO_REQUEST)
            {
                dhcpsicmpStartTick = 0U;  // try to start as quickly as possible
                dhcpsicmpSequenceNo = (uint16_t)SYS_RANDOM_PseudoGet();
                dhcpsicmpIdentifier = (uint16_t)SYS_RANDOM_PseudoGet();
                dhcpsEchoRequestState = TCPIP_DHCPS_DO_ECHO_REQUEST;
                if(dhcpsicmpReqNo == 0U)
                {
                    dhcpsicmpReqNo = (size_t)TCPIP_DHCPS_ICMP_ECHO_REQUESTS;
                }
                if(dhcpsicmpReqDelay == 0U)
                {
                    dhcpsicmpReqDelay = (size_t)TCPIP_DHCPS_ICMP_ECHO_REQ_DELAY;
                }
            }

            // continue for TCPIP_DHCPS_DO_ECHO_REQUEST
            if(dhcpsicmpReqCount == dhcpsicmpReqNo)
            {   // no more requests to send
                killIcmp = true;
                break;
            }

            // check if time for another request
            cancelReq = newReq = false;
            currentTime = SYS_TMR_TickCountGet();
            if(currentTime - dhcpsicmpStartTick > (sysFreq * dhcpsicmpReqDelay) / 1000U)
            {
                cancelReq = dhcpsicmpReqCount != dhcpsicmpAckRecv && dhcpsicmpReqHandle != NULL;    // cancel if there is another one ongoing
                newReq = true;
            }
            else if(dhcpsicmpReqCount != dhcpsicmpAckRecv)
            {   // no reply received to the last ping 
                if(currentTime - dhcpsicmpStartTick > (sysFreq * TCPIP_DHCPS_ICMP_ECHO_TMO) / 1000U)
                {   // timeout
                    F_DhcpsICMPDebugMessage("request timeout.\r\n");
                    cancelReq = dhcpsicmpReqHandle != NULL;    // cancel if there is another one ongoing
                    newReq = true;
                    dhcpsicmpProcessSteps = DHCPS_ICMP_REQUEST_TIMEDOUT;
                }
                // else wait some more
            }
            else
            {
                // do nothing
            }

            if(cancelReq)
            {
                if(TCPIP_ICMP_EchoRequestCancel(dhcpsicmpReqHandle) != ICMP_ECHO_OK)
                {   // this should NOT happen!
                    F_DhcpsICMPDebugMessage("Cancel failed!!!\r\n");
                }
                else
                {
                    F_DhcpsICMPDebugMessage("Request aborted tmo!\r\n");
                }
            }

            if(!newReq)
            {   // nothing else to do
                break;
            }

            // send another request
            echoRequest.netH = dhcpsicmpNetH;
            echoRequest.targetAddr = dhcpsicmpTargetAddr;
            echoRequest.sequenceNumber = ++dhcpsicmpSequenceNo;
            echoRequest.identifier = dhcpsicmpIdentifier;
            echoRequest.pData = dhcpsicmpPingBuff;
            echoRequest.dataSize = (uint16_t)dhcpsicmpPingSize;
            echoRequest.callback = &DHCPSPingHandler;

            {
                uint16_t ix;
                uint8_t* pBuff = dhcpsicmpPingBuff;
                for(ix = 0; ix < dhcpsicmpPingSize; ix++)
                {
                    *pBuff++ = (uint8_t)SYS_RANDOM_PseudoGet();
                }
            }

            echoRes = TCPIP_ICMP_EchoRequest (&echoRequest, &dhcpsicmpReqHandle);

            if((int)echoRes >= 0 )
            {
                dhcpsicmpStartTick = SYS_TMR_TickCountGet();
                dhcpsicmpReqCount++;
                (void)FC_sprintf(debugBuf, sizeof(debugBuf),"Ping: sent request %d to: [%x]\r\n", dhcpsicmpReqCount, (unsigned int)dhcpsicmpTargetAddr.Val);
                F_DhcpsICMPDebugMessage(debugBuf);
                dhcpsicmpProcessSteps = DHCPS_ICMP_REQUEST_SENT;
            }
            else
            {
                (void)FC_sprintf(debugBuf, sizeof(debugBuf),"Ping: failed to send request %d to: %x, error %d\r\n", dhcpsicmpReqCount, (unsigned int)dhcpsicmpTargetAddr.Val, echoRes);
                F_DhcpsICMPDebugMessage(debugBuf);
                killIcmp = true;
                dhcpsicmpProcessSteps = DHCPS_ICMP_REQUEST_CANCEL;
            }
            if(killIcmp)
            {
                // Stop PING echo request 
                dhcpsEchoRequestState = TCPIP_DHCPS_STOP_ECHO_REQUEST;
            }
            break;

            case TCPIP_DHCPS_STOP_ECHO_REQUEST:
                 if(TCPIP_ICMP_EchoRequestCancel(dhcpsicmpReqHandle) == ICMP_ECHO_OK)
                 {// this should NOT happen!
                     dhcpsicmpProcessSteps = DHCPS_ICMP_REQUEST_CANCEL;
                 }
                dhcpsicmpReqHandle = NULL;
                dhcpsicmpReqCount = 0;
                dhcpsEchoRequestState = TCPIP_DHCPS_ECHO_REQUEST_IDLE;
                //Stop echo request
                break;

            case TCPIP_DHCPS_ECHO_REQUEST_IDLE:
                // do nothing 
                break;

            default:
                killIcmp = true;
                break;
    }

    if(killIcmp)
    {
        // Stop PING echo request 
        dhcpsEchoRequestState = TCPIP_DHCPS_STOP_ECHO_REQUEST;
    }
   
}

static void DHCPSPingHandler(const  TCPIP_ICMP_ECHO_REQUEST* pEchoReq, TCPIP_ICMP_REQUEST_HANDLE iHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result, const void* param)
{
    char debugBuf[128];
    
    if(result == TCPIP_ICMP_ECHO_REQUEST_RES_OK)
    {   // reply has been received
        uint32_t errorMask = 0;     // error mask:
        // 0x1: wrong id
        // 0x2: wrong seq
        // 0x4: wrong target
        // 0x8: wrong size
        // 0x10: wrong data
        //
        if(pEchoReq->identifier != dhcpsicmpIdentifier)
        {
            errorMask |= 0x1U;
        }

        if(pEchoReq->sequenceNumber != dhcpsicmpSequenceNo)
        {
            errorMask |= 0x2U;
        }

        if(pEchoReq->targetAddr.Val != dhcpsicmpTargetAddr.Val)
        {
            errorMask |= 0x4U;
        }

        if(pEchoReq->dataSize != dhcpsicmpPingSize)
        {
            errorMask |= 0x8U;
        }

        // check the data
        uint16_t ix;
        uint16_t checkSize = pEchoReq->dataSize < dhcpsicmpPingSize ? pEchoReq->dataSize : dhcpsicmpPingSize;
        uint8_t* pSrc = dhcpsicmpPingBuff;
        uint8_t* pDst = pEchoReq->pData;
        for(ix = 0; ix < checkSize; ix++)
        {
            if(*pSrc++ != *pDst++)
            {
                errorMask |= 0x10U;
                break;
            }
        }

        if(errorMask != 0U)
        {   // some errors
            (void)FC_sprintf(debugBuf, sizeof(debugBuf),"Ping: wrong reply received. Mask: 0x%2x\r\n", (unsigned int)errorMask);
            F_DhcpsICMPDebugMessage(debugBuf);
            dhcpsicmpProcessSteps = DHCPS_ICMP_WRONG_RESPONSE_RECEIVED;
        }
        else
        {   // good reply
            (void)FC_sprintf(debugBuf, sizeof(debugBuf),"Ping: Good reply received: 0x%2x\r\n", ++dhcpsicmpAckRecv);
            F_DhcpsICMPDebugMessage(debugBuf);
            dhcpsicmpProcessSteps = DHCPS_ICMP_RESPONSE_RECEIVED;
        }
    }
    else
    {
        if(dhcpsicmpReqCount < dhcpsicmpReqNo)
        { // wait till all the request attempts
            (void)FC_sprintf(debugBuf, sizeof(debugBuf),"Ping: Number of request attempted %d\r\n", dhcpsicmpReqCount);
            F_DhcpsICMPDebugMessage(debugBuf);
            dhcpsicmpProcessSteps = DHCPS_ICMP_WAIT_REPLY;
        }
        else
        {
            (void)FC_sprintf(debugBuf, sizeof(debugBuf),"Ping: request aborted by ICMP with result %d\r\n", result);
            F_DhcpsICMPDebugMessage(debugBuf);
            dhcpsicmpProcessSteps = DHCPS_ICMP_NO_RESPONSE_RECEIVED;
        }
    }
    // one way or the other, request is done
    dhcpsicmpReqHandle = NULL;
}
#else
bool TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet){return false;}
bool TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet){return false;}
bool TCPIP_DHCPS_IsEnabled(TCPIP_NET_HANDLE hNet){return false;}
#endif //#if defined(TCPIP_STACK_USE_DHCP_SERVER)
#endif // defined(TCPIP_STACK_USE_IPV4)

