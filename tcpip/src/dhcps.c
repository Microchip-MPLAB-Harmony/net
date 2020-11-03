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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DHCP_SERVER

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/dhcps_private.h"

#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DHCP_SERVER)


uint32_t initTime ;

#define TCPIP_DHCPS_MAX_RECEIVED_BUFFER_SIZE 480
static DHCPS_HASH_DCPT gPdhcpsHashDcpt = { 0,0};

static DHCPS_MOD    dhcps_mod;
// DHCP is running on all interfaces
static DHCP_SRVR_DCPT      *gPdhcpSDcpt=NULL;
static const void*          dhcpSMemH = 0;        // memory handle

static int                  dhcpSInitCount = 0;     // initialization count

static void _DHCPSUpdateEntry(DHCPS_HASH_ENTRY* dhcpsHE);

static void DHCPReplyToDiscovery(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf);
static void DHCPReplyToRequest(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, bool bAccept, bool bRenew,DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf,DHCP_SRVR_DCPT * pDhcpsDcpt);
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _DHCPServerCleanup(void);
#else
#define _DHCPServerCleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
static DHCPS_RESULT preAssignToDHCPClient(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt);
static bool isMacAddrEffective(const TCPIP_MAC_ADDR *macAddr);
static void DHCPSReplyToInform(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, DHCP_SRVR_DCPT* pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,bool bAccept,TCPIP_DHCPS_DATA *getBuf);
static void _DHCPSrvClose(TCPIP_NET_IF* pNetIf, bool disable);
static size_t DHCPSgetFreeHashIndex(OA_HASH_DCPT* pOH,void* key,IPV4_ADDR *requestedAddr);
static DHCPS_RESULT DHCPSLocateRequestedIpAddress(IPV4_ADDR *requestedAddr);
static  DHCPS_RESULT DHCPSRemoveHashEntry(TCPIP_MAC_ADDR* hwAdd, const uint8_t* pIPAddr);
static int TCPIP_DHCPS_CopyDataArrayToProcessBuff(uint8_t *val ,TCPIP_DHCPS_DATA *putbuf,int len);
static void TCPIP_DHCPS_DataCopyToProcessBuffer(uint8_t val ,TCPIP_DHCPS_DATA *putbuf);
static bool _DHCPSAddCompleteEntry(int intfIdx, const uint8_t* pIPAddr, TCPIP_MAC_ADDR* hwAdd, DHCPS_ENTRY_FLAGS entryFlag);
static bool _DHCPSDescriptorGetFromIntf(TCPIP_NET_IF *pNetIf,uint32_t *dcptIdx);
static bool _DHCPS_ProcessGetPktandSendResponse(void);
static bool _DHCPS_Enable(TCPIP_NET_HANDLE hNet,bool checkIfUp);
static bool _DHCPS_StartOperation(TCPIP_NET_IF* pNetIf,DHCP_SRVR_DCPT* pDhcpsDcpt);

static void TCPIP_DHCPS_TaskForLeaseTime(void);
static void TCPIP_DHCPS_Process(void);
static void TCPIP_DHCPSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);




/*static __inline__*/static  void /*__attribute__((always_inline))*/ _DHCPSSetHashEntry(DHCPS_HASH_ENTRY* dhcpsHE, DHCPS_ENTRY_FLAGS newFlags, TCPIP_MAC_ADDR* hwAdd, const uint8_t* pIPAddr)
{
    dhcpsHE->hEntry.flags.value &= ~DHCPS_FLAG_ENTRY_VALID_MASK;
    dhcpsHE->hEntry.flags.value |= newFlags;
    
    if(hwAdd)
    {
        dhcpsHE->hwAdd = *hwAdd;		
        memcpy(dhcpsHE->ipAddress.v, pIPAddr, sizeof(dhcpsHE->ipAddress));
    }
    
    dhcpsHE->Client_Lease_Time = SYS_TMR_TickCountGet();
    dhcpsHE->pendingTime = SYS_TMR_TickCountGet();
}

/*static __inline__*/static  void /*__attribute__((always_inline))*/ _DHCPSRemoveCacheEntries(DHCPS_HASH_DCPT* pDHCPSHashDcpt)
{
    if(pDHCPSHashDcpt->hashDcpt)
    {
        TCPIP_OAHASH_EntriesRemoveAll(pDHCPSHashDcpt->hashDcpt);
    }
}

static  DHCPS_RESULT DHCPSRemoveHashEntry(TCPIP_MAC_ADDR* hwAdd, const uint8_t* pIPAddr)
{
    DHCPS_HASH_DCPT *pDhcpsHashDcpt;
    OA_HASH_ENTRY*	hE;

    pDhcpsHashDcpt = &gPdhcpsHashDcpt;
    if(hwAdd)
    {
    	hE = TCPIP_OAHASH_EntryLookup(pDhcpsHashDcpt->hashDcpt,hwAdd);
        if(hE != 0)
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

static void _DHCPSUpdateEntry(DHCPS_HASH_ENTRY* dhcpsHE)
{    
     dhcpsHE->Client_Lease_Time = SYS_TMR_TickCountGet();
     dhcpsHE->pendingTime = 0;

     dhcpsHE->hEntry.flags.value &= ~DHCPS_FLAG_ENTRY_VALID_MASK;
     dhcpsHE->hEntry.flags.value |= DHCPS_FLAG_ENTRY_COMPLETE;
}

// validate the IP address pool from the DHCP server configuration and poolCnt returns the valid pool numbers
static void dhcpServPoolAddressValidation(const TCPIP_DHCPS_MODULE_CONFIG* pDhcpsConfig,int nIfx,int *poolCnt)
{
    int ix=0;
    int tempPoolCnt=0;    
    const char *tempStr = {"0.0.0.0"};
    IPV4_ADDR	serverIPAddress;
    IPV4_ADDR	startIPv4PoolAddress;
    IPV4_ADDR   netMask;
    TCPIP_DHCPS_ADDRESS_CONFIG *pServer;

    *poolCnt =tempPoolCnt;
    
    for(ix=0;ix<nIfx;ix++)
    {
        if(pDhcpsConfig->dhcpServer == NULL) break;
        
        pServer = pDhcpsConfig->dhcpServer+ix;
        if(pServer == NULL)
        {
            continue;
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
        TCPIP_Helper_StringToIPAddress((char*)pDhcpsConfig->dhcpServer[ix].serverIPAddress,&serverIPAddress);
//start IP address range
        TCPIP_Helper_StringToIPAddress((char*)pDhcpsConfig->dhcpServer[ix].startIPAddRange,&startIPv4PoolAddress);
//NetMask
        TCPIP_Helper_StringToIPAddress((char*)pDhcpsConfig->dhcpServer[ix].ipMaskAddress,&netMask);        
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
    if(tempPoolCnt==0)  tempPoolCnt=1;
    *poolCnt = tempPoolCnt;
}

// DHCP server descriptor update has been done at the init  time only.
static void _DHCPS_AddressPoolDescConfiguration(const TCPIP_DHCPS_MODULE_CONFIG* pDhcpsConfig)
{
    int poolCount=0;
    TCPIP_DHCPS_ADDRESS_CONFIG *pPoolServer=NULL;
    DHCP_SRVR_DCPT *pServerDcpt=NULL;
    int nIntf=0,ix=0,poolIndex=0;

    nIntf = TCPIP_STACK_NumberOfNetworksGet();
    poolCount = dhcps_mod.poolCount;    
    if(gPdhcpSDcpt == NULL)
    {
        return;
    }
    
    for(ix=0;ix<nIntf;ix++)
    {
        if(pDhcpsConfig->dhcpServer == NULL)
        {
            break;
        }
        
        pPoolServer = pDhcpsConfig->dhcpServer+ix;
        if(pPoolServer == NULL)
        {
            continue;
        }
        if(pPoolServer->poolEnabled == false)
        {
            continue;
        }

        pServerDcpt = gPdhcpSDcpt+poolIndex;
        if(pServerDcpt == NULL)
        {
            continue;
        }
// Server IPv4 address
        TCPIP_Helper_StringToIPAddress((char*)pPoolServer->serverIPAddress,&pServerDcpt->intfAddrsConf.serverIPAddress);
    //NET Mask
        TCPIP_Helper_StringToIPAddress((char*)pPoolServer->ipMaskAddress,&pServerDcpt->intfAddrsConf.serverMask);
// Server start of IPv4 address pool
        TCPIP_Helper_StringToIPAddress((char*)pPoolServer->startIPAddRange,&pServerDcpt->intfAddrsConf.startIPAddress);
#if defined(TCPIP_STACK_USE_DNS)
    // primary DNS server
        TCPIP_Helper_StringToIPAddress((char*)pPoolServer->priDNS,&pServerDcpt->intfAddrsConf.serverDNS);
    // Secondary DNS server
        TCPIP_Helper_StringToIPAddress((char*)pPoolServer->secondDNS,&pServerDcpt->intfAddrsConf.serverDNS2);
#endif
        pServerDcpt->netIx = pPoolServer->interfaceIndex;

        poolIndex++;
        if(poolCount==poolIndex)
        {
            break;
        }
    }
    
    // Get the default pool configuration
    if(poolIndex == 0)
    {
        pServerDcpt = gPdhcpSDcpt+poolIndex;
        memset(&pServerDcpt->intfAddrsConf,0,sizeof(pServerDcpt->intfAddrsConf));
        pServerDcpt->netIx = TCPIP_STACK_NetIndexGet(TCPIP_STACK_NetDefaultGet());
    }
}

bool TCPIP_DHCPS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_DHCPS_MODULE_CONFIG* pDhcpsConfig)
{    
    size_t hashMemSize=0;
    OA_HASH_DCPT*   hashDcpt;
    int poolCnt=0;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        if(stackCtrl->pNetIf->Flags.bIsDHCPSrvEnabled != 0)
        {
            TCPIP_DHCPS_Enable(stackCtrl->pNetIf);
        }
        dhcpSMemH = stackCtrl->memH;
        return true;
    }
	
    
    // stack init
    if(dhcpSInitCount == 0)
    {
        if(pDhcpsConfig == 0)
        {
            return false;
        }
        // first time we're run
        // store the memory allocation handle
        dhcpSMemH = stackCtrl->memH;
        dhcpServPoolAddressValidation(pDhcpsConfig,stackCtrl->nIfs,&poolCnt);
        if(poolCnt > 0)
        {
            gPdhcpSDcpt = (DHCP_SRVR_DCPT*)TCPIP_HEAP_Calloc(dhcpSMemH, poolCnt, sizeof(DHCP_SRVR_DCPT));
            if(gPdhcpSDcpt == 0)
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

        if(hashDcpt == 0)
        {	// failed
        // Remove all the DHCPS lease entries.
            // Free DHCP server Pool entries
            TCPIP_HEAP_Free(dhcpSMemH,gPdhcpSDcpt);
            return false;
        }
        // populate the entries
        hashDcpt->memBlk = hashDcpt+1;
        hashDcpt->hParam = &gPdhcpsHashDcpt;
        hashDcpt->hEntrySize = sizeof(DHCPS_HASH_ENTRY);
        hashDcpt->hEntries = pDhcpsConfig->leaseEntries;
        hashDcpt->probeStep = DHCPS_HASH_PROBE_STEP;

        hashDcpt->hashF= TCPIP_DHCPS_MACHashKeyHash;
#if defined(OA_DOUBLE_HASH_PROBING)
        hashDcpt->probeHash = TCPIP_DHCPS_HashProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
        hashDcpt->cpyF = TCPIP_DHCPS_HashMACKeyCopy;
        hashDcpt->delF = TCPIP_DHCPS_HashDeleteEntry;
        hashDcpt->cmpF = TCPIP_DHCPS_HashMACKeyCompare;
        TCPIP_OAHASH_Initialize(hashDcpt);
        gPdhcpsHashDcpt.hashDcpt = hashDcpt;
        gPdhcpsHashDcpt.leaseDuartion = pDhcpsConfig->entrySolvedTmo;

        if(pDhcpsConfig->deleteOldLease)
        {	// remove the old entries, if there
            _DHCPSRemoveCacheEntries(&gPdhcpsHashDcpt);
        }
		
        dhcps_mod.signalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_DHCPS_Task, TCPIP_DHCPS_TASK_PROCESS_RATE);
        if(dhcps_mod.signalHandle == 0)
        {
            _DHCPServerCleanup();
            return false;
        }
        dhcps_mod.smServer = DHCP_SERVER_IDLE;
        dhcps_mod.uSkt = INVALID_UDP_SOCKET;
        dhcps_mod.poolCount = poolCnt;
        dhcps_mod.dhcpNextLease.Val = 0;

        // expected that max number of pool entry is simillar to the interface index
        // copy the valid interface details to the global dhcps descpritor table
       _DHCPS_AddressPoolDescConfiguration(pDhcpsConfig);
    }
	
    if(stackCtrl->pNetIf->Flags.bIsDHCPSrvEnabled != 0)
    {   // override the pDhcpsConfig->dhcpEnable passed with the what the stack manager says
        _DHCPS_Enable(stackCtrl->pNetIf,false);
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
        _DHCPSrvClose(stackCtrl->pNetIf,true);
        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--dhcpSInitCount == 0)
            {   // all closed
                // release resources
                _DHCPServerCleanup();
                dhcpSMemH = 0;
            }
        }
    }
}

static void _DHCPServerCleanup(void)
{
    // Free HASH descriptor 
    if(gPdhcpsHashDcpt.hashDcpt != NULL)
    {
        // Remove all the HASH entries
        _DHCPSRemoveCacheEntries(&gPdhcpsHashDcpt);
        TCPIP_HEAP_Free(dhcpSMemH,gPdhcpsHashDcpt.hashDcpt);
        gPdhcpsHashDcpt.hashDcpt = 0;
    }
    // Free Pool entry DHCP server descriptor
    if(gPdhcpSDcpt != NULL)
    {
        TCPIP_HEAP_Free(dhcpSMemH,gPdhcpSDcpt);
    }
    // Free timer handler
    if(dhcps_mod.signalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(dhcps_mod.signalHandle);
        dhcps_mod.signalHandle = 0;
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void _DHCPSrvClose(TCPIP_NET_IF* pNetIf, bool disable)
{
    int netIx=0;
    TCPIP_NET_IF* pIf=NULL;
    bool serverIsEnabled=false;
    if(disable)
    {
        pNetIf->Flags.bIsDHCPSrvEnabled = false;
    }
    for(netIx = 0;netIx < TCPIP_STACK_NumberOfNetworksGet(); netIx++)
    {
        pIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
        if(pIf->Flags.bIsDHCPSrvEnabled)
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
            TCPIP_UDP_Close(dhcps_mod.uSkt);
            dhcps_mod.uSkt = INVALID_UDP_SOCKET;
            dhcps_mod.smServer = DHCP_SERVER_IDLE;
        }
    }
    else
    {
        dhcps_mod.smServer = DHCP_SERVER_LISTEN;
    }
    
}

bool _DHCPS_ValidatePktReceivedIntf(TCPIP_NET_IF *pNetIfFromDcpt)
{
    if(_TCPIPStackHandleToNetLinked(pNetIfFromDcpt) == 0)
    {
        return false;
    }
    if(_TCPIPStackNetAddress(pNetIfFromDcpt) == 0)
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

static int TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(TCPIP_DHCPS_DATA *getbuf,uint8_t *buf,int bytes)
{
    int nBytes = getbuf->endPtr - getbuf->wrPtr;
    if(nBytes == 0)
        return 0;

    if(bytes < nBytes)
    {
        nBytes =  bytes;
    }
    if(buf == NULL)
    {
        getbuf->wrPtr = getbuf->wrPtr+nBytes;
        return nBytes;
    }
    memcpy(buf,getbuf->wrPtr,nBytes);
    getbuf->wrPtr = getbuf->wrPtr+nBytes;
    return nBytes;
}

void TCPIP_DHCPS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(dhcps_mod.uSkt == INVALID_UDP_SOCKET) 
    {
        return;
    }

    if((sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
    { //  RX signal occurred
        TCPIP_DHCPS_Process();
    }

    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
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
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_DHCPS_Process(void)
{
    switch(dhcps_mod.smServer)
    {
        case DHCP_SERVER_LISTEN:
            _DHCPS_ProcessGetPktandSendResponse();
            break;
        case  DHCP_SERVER_IDLE:          // idle state
            break;
    }
    return;
}

static bool _DHCPS_ProcessGetPktandSendResponse(void)
{
    uint32_t            buffsize=0;
    uint8_t 		i;
    uint8_t		Option, Len;
    BOOTP_HEADER	BOOTPHeader;
    uint32_t		dw;
    bool		bAccept, bRenew;
    UDP_SOCKET          s;
    OA_HASH_ENTRY   	*hE;
    DHCPS_HASH_DCPT  	*pdhcpsHashDcpt;
    DHCP_SRVR_DCPT	*pDhcpsDcpt;
    UDP_SOCKET_INFO     udpSockInfo;
    TCPIP_NET_IF        *pNetIfFromDcpt=NULL;
    uint32_t            ix=0;
    uint8_t             getBuffer[TCPIP_DHCPS_MAX_RECEIVED_BUFFER_SIZE];
    TCPIP_DHCPS_DATA    udpGetBufferData;
    IPV4_ADDR           ClientIP;
    
    s = dhcps_mod.uSkt;
    if(gPdhcpSDcpt == NULL)
    {
        return false;
    }
    while(true)
    {
    // Check to see if a valid DHCP packet has arrived
        buffsize = TCPIP_UDP_GetIsReady(s);
        if(buffsize == 0) 
        {
            return false;
        }
        if(buffsize < 241u)
        {
            TCPIP_UDP_Discard(s);
            continue;
        }
        memset(getBuffer,0,sizeof(getBuffer));
        pdhcpsHashDcpt = &gPdhcpsHashDcpt;

        TCPIP_UDP_SocketInfoGet(s, &udpSockInfo);
        pNetIfFromDcpt = (TCPIP_NET_IF*)udpSockInfo.hNet;
        // check if DHCP server is enabled or Not for this incoming packet interface
        if(!_DHCPS_ValidatePktReceivedIntf(pNetIfFromDcpt))
        {
            TCPIP_UDP_Discard(s);
            continue;
        }
        if(buffsize > sizeof(getBuffer))
        {
            buffsize = sizeof(getBuffer);
        }
        udpGetBufferData.head = getBuffer;
        udpGetBufferData.wrPtr = udpGetBufferData.head;
        udpGetBufferData.endPtr = udpGetBufferData.head+buffsize;
        // Get Compleate Array of DHCP server Reply bytes
        TCPIP_UDP_ArrayGet(s,udpGetBufferData.head,buffsize);
       // discard the current packet and point to the next queued packet
        TCPIP_UDP_Discard(s);

        // Retrieve the BOOTP header
        if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData, (uint8_t*)&BOOTPHeader, sizeof(BOOTPHeader)))
        {
            continue;
        }
        hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &BOOTPHeader.ClientMAC);
        if(hE != 0)
        {
            ClientIP = BOOTPHeader.ClientIP;
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
        else if(BOOTPHeader.ClientIP.Val == 0x00000000u)
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
        if((BOOTPHeader.MessageType != 1u) || (BOOTPHeader.HardwareType != 1u) || (BOOTPHeader.HardwareLen != 6u))
        {
            continue;
        }

        // Throw away 10 unused bytes of hardware address,
        // server host name, and boot file name -- unsupported/not needed.
        if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData,0,DHCPS_UNUSED_BYTES_FOR_TX))
        {
            continue;
        }
      
        // Obtain Magic Cookie and verify DHCP
        if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData,(uint8_t*)&dw, sizeof(uint32_t)))
        {
            continue;
        }

        if(dw != 0x63538263ul)
        {
            continue;
        }

        // Obtain options
        while(true)
        {
            // Get option type
            if(!TCPIP_DHCPS_GetDataFromUDPBuff(&udpGetBufferData,&Option))
                break;
            if(Option == DHCP_END_OPTION)
                break;

            // Get option length
            if(!TCPIP_DHCPS_GetDataFromUDPBuff(&udpGetBufferData,&Len))
                break;
            ix=0;
            
            // get the Descriptor index from the interface
            if(_DHCPSDescriptorGetFromIntf(pNetIfFromDcpt,&ix) == false)
            {
                // donot break here, use the  0th index for pool for other
                // interface if it is not configured.
                ix=0;
            }
            // assign the dhcpDescriptor value
            pDhcpsDcpt = gPdhcpSDcpt+ix;
            // Process option
            switch(Option)
            {
                case DHCP_MESSAGE_TYPE:
                    if(!TCPIP_DHCPS_GetDataFromUDPBuff(&udpGetBufferData,&i))
                        break;
                    switch(i)
                    {
                        case DHCP_DISCOVER_MESSAGE:
                            DHCPReplyToDiscovery(pNetIfFromDcpt,&BOOTPHeader,pDhcpsDcpt,pdhcpsHashDcpt,&udpGetBufferData);
                            break;

                        case DHCP_REQUEST_MESSAGE:
                            DHCPReplyToRequest(pNetIfFromDcpt,&BOOTPHeader, bAccept,bRenew,pdhcpsHashDcpt,&udpGetBufferData,pDhcpsDcpt);
                            break;
                            /*
                            Release the leased address from the hash table by checking
                            BOOTPHeader.ClientMAC and BOOTPHeader.ClientIP.Val.
                            */
                        // Need to handle these if supporting more than one DHCP lease
                        case DHCP_RELEASE_MESSAGE:
                        case DHCP_DECLINE_MESSAGE:
                            ClientIP = BOOTPHeader.ClientIP;
                            DHCPSRemoveHashEntry(&BOOTPHeader.ClientMAC, ClientIP.v);
                            break;
                        case DHCP_INFORM_MESSAGE:
                            DHCPSReplyToInform(pNetIfFromDcpt,&BOOTPHeader,pDhcpsDcpt,pdhcpsHashDcpt,bAccept,&udpGetBufferData);
                            break;
                    }
                    break;          

                case DHCP_END_OPTION:
                default:
                    break;
            }
            // Remove any unprocessed bytes that we don't care about
            if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData,0,Len))
            {
                break;
            }
            Len = 0;
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

static int TCPIP_DHCPS_CopyDataArrayToProcessBuff(uint8_t *val ,TCPIP_DHCPS_DATA *putbuf,int len)
{
    int nBytes = putbuf->endPtr - putbuf->wrPtr;
    if(len < nBytes)
        nBytes = len;
    if(nBytes <= 0)
        return 0;
    if(val == NULL)
    {
        putbuf->wrPtr = putbuf->wrPtr+nBytes;
        return 0;
    }
    memcpy(putbuf->wrPtr, val,nBytes);
    putbuf->wrPtr += nBytes;
    return nBytes;
}

//Send DHCP Discovery message
static void DHCPReplyToDiscovery(TCPIP_NET_IF *pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf)
{
    uint8_t         i;
    DHCPS_RESULT	res;
    UDP_SOCKET      s;
    OA_HASH_ENTRY   *hE;
    TCPIP_DHCPS_DATA   putBuffer;
    DHCPS_HASH_ENTRY *dhcpsHE = 0;
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
    TCPIP_UDP_TxOffsetSet(s,0,false);
    
    dhcps_mod.dhcpNextLease.Val = 0;
    memset((void*)&rxHeader,0,sizeof(BOOTP_HEADER));
    // Search through all remaining options and look for the Requested IP address field
    // Obtain options

    while(getBuf->endPtr-getBuf->wrPtr)
    {
        uint8_t Option, Len;
        uint32_t dw;

        // Get option type
        if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t *)&Option))
            break;
        if(Option == DHCP_END_OPTION)
            break;

        // Get option length
        if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t *)&Len))
            break;

        switch(Option)
        {
            case DHCP_PARAM_REQUEST_IP_ADDRESS:
            {
                if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,(uint8_t*)&dw,4))
                    break;
                if(Len != 4)
                {
                    break;
                }
                hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC);
                if(hE != 0)
                {	//found entry and this MAC is already in use
                    return;
                }
                /* Validating if the requested IP address should not be part of any Hash Entry */
                if(DHCPSLocateRequestedIpAddress((IPV4_ADDR *)&dw) == DHCPS_RES_OK)
                { // use the alternate address
                    dhcps_mod.dhcpNextLease.Val = 0;
                }
                else
                {
                // the requested address should be in the same address block.
                    if((pNetIf->netIPAddr.Val & pNetIf->netMask.Val) == (dw & pNetIf->netMask.Val))
                    {
                        // use the requested Ip address
                       dhcps_mod.dhcpNextLease.Val = dw;
                    }
                    else
                    {
                        dhcps_mod.dhcpNextLease.Val = 0;
                    }
                }
            }
            break;
        }
        // Remove the unprocessed bytes that we don't care about
        if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,NULL, Len))
        {
            break;
        }
        Len = 0;
    }

    // find in pool
    res = preAssignToDHCPClient(pNetIf,Header,pDhcpsDcpt,pdhcpsHashDcpt);
    if( res != DHCPS_RES_OK) 
    {
        return;
    }

// Before sending OFFER, get the perfect Hash Entry
    hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC);
    if(hE != 0)
    {
        dhcpsHE = ( DHCPS_HASH_ENTRY *) hE;
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
    rxHeader.NextServerIP.Val = 0;
    // Relay agent IP address, used in booting via a relay agent.
    rxHeader.RelayAgentIP.Val = 0;
    // Client MAC address
    memcpy(&rxHeader.ClientMAC,&Header->ClientMAC,sizeof(TCPIP_MAC_ADDR));

    // copy the BOOT RX header contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&rxHeader,&putBuffer,sizeof(BOOTP_HEADER));

    // Remaining 10 bytes of client hardware address, server host name: Null string (not used)
    for(i = 0; i < DHCPS_UNUSED_BYTES_FOR_TX; i++)	
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer);
    }
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t *)&magicCookie, &putBuffer, sizeof(magicCookie));

    // Options: DHCP Offer
    optionType.messageType.optionType = DHCP_MESSAGE_TYPE;
    optionType.messageType.optionTypeLen = 1;
    optionType.messageType.byteVal = DHCP_OFFER_MESSAGE;
    optionTypeTotalLen = sizeof(optionType.messageType);
  
    // Option: Subnet Mask
    optionType.subnetmaskType.optionType = DHCP_SUBNET_MASK;
    optionType.subnetmaskType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.subnetmaskType.intVal = pNetIf->netMask.Val;
    optionTypeTotalLen += sizeof(optionType.subnetmaskType);
   
    // Option: Server identifier
    optionType.serverIdentifierType.optionType = DHCP_SERVER_IDENTIFIER;
    optionType.serverIdentifierType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.serverIdentifierType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.serverIdentifierType);

    // Option: Router/Gateway address
    optionType.routerType.optionType = DHCP_ROUTER;
    optionType.routerType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.routerType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.routerType);
  
    // Option: DNS server address
    optionType.dnsType.optionType = DHCP_DNS;
    optionType.dnsType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.dnsType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.dnsType);
    
     // Option: Lease duration
    optionType.ipLeaseTimeType.optionType = DHCP_IP_LEASE_TIME;
    optionType.ipLeaseTimeType.optionTypeLen = 4;
    optionType.ipLeaseTimeType.intVal = pdhcpsHashDcpt->leaseDuartion;
    optionType.ipLeaseTimeType.intVal = TCPIP_Helper_htonl(optionType.ipLeaseTimeType.intVal);
    optionTypeTotalLen += sizeof(optionType.ipLeaseTimeType);
  
    // copy the DHCP Reply Option type contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&optionType,&putBuffer,optionTypeTotalLen);

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
    TCPIP_UDP_BcastIPV4AddressSet( s,UDP_BCAST_NETWORK_LIMITED,pNetIf);

    IP_MULTI_ADDRESS tmp_MultiAddr; tmp_MultiAddr.v4Add = pNetIf->netIPAddr;
    TCPIP_UDP_SourceIPAddressSet(s,IP_ADDRESS_TYPE_IPV4,&tmp_MultiAddr);

    // Once it is completed writing into the buffer, you need to update the Tx offset again,
    // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
    TCPIP_UDP_TxOffsetSet(s,(uint16_t)(putBuffer.wrPtr - putBuffer.head), false);

    // Transmit the packet
    TCPIP_UDP_Flush(s);

}

// Replies to a DHCP Inform message.
static void DHCPSReplyToInform(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, DHCP_SRVR_DCPT* pDhcpsDcpt,
                                DHCPS_HASH_DCPT *pdhcpsHashDcpt,bool bAccept,TCPIP_DHCPS_DATA *getBuf)
{
    uint8_t         i;
    UDP_SOCKET      s;
    OA_HASH_ENTRY   	*hE;
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
    TCPIP_UDP_TxOffsetSet(s,0,false);
    
    // Search through all remaining options and look for the Requested IP address field
    // Obtain options

    while(getBuf->endPtr-getBuf->wrPtr)
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
            if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,(uint8_t*)&tmp_MacAddr, 6))
            {
                return;
            }
            Len -= 7;
            hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &tmp_MacAddr);
            ClientIP = boot_header->ClientIP;
            if(hE !=0)
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
                if((pNetIf->netIPAddr.Val & pNetIf->netMask.Val)==
                        (ClientIP.Val & pNetIf->netMask.Val))
                {
                    if(_DHCPSAddCompleteEntry(pDhcpsDcpt->netIx, ClientIP.v, &boot_header->ClientMAC,DHCPS_FLAG_ENTRY_COMPLETE) != DHCPS_RES_OK)
                    {
                        return;
                    }
                }
                bAccept = true;
            }
            break;
        }

        // Remove the unprocessed bytes that we don't care about
        if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,NULL, Len))
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


    memset((void*)&rxHeader,0,sizeof(BOOTP_HEADER));
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
    if(bAccept)
        rxHeader.ClientIP.Val = boot_header->ClientIP.Val; // Not yet Assigned
    else
        rxHeader.ClientIP.Val = 0; // Not yet Assigned
    // Leased IP address or Client IP address
    rxHeader.YourIP.Val = 0;
    // IP address of next server to use in bootstrap;
    //returned in DHCPOFFER, DHCPACK by server.
    if(bAccept)
        rxHeader.NextServerIP.Val = pNetIf->netIPAddr.Val;
    else
        rxHeader.NextServerIP.Val = 0;
    // Relay agent IP address, used in booting via a relay agent.
    rxHeader.RelayAgentIP.Val = 0;
    // Client MAC address
    memcpy(&rxHeader.ClientMAC,&boot_header->ClientMAC,sizeof(TCPIP_MAC_ADDR));

    // copy the BOOT RX header contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&rxHeader,&putBuffer,sizeof(BOOTP_HEADER));
	
    for(i = 0; i < DHCPS_UNUSED_BYTES_FOR_TX; i++)	// Remaining 10 bytes of client hardware address, server host name: Null string (not used)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer); // Boot filename: Null string (not used)
    }
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&magicCookie, &putBuffer, sizeof(magicCookie));

    // Options: DHCP lease ACKnowledge
    optionType.messageType.optionType = DHCP_OPTION_ACK_MESSAGE;
    optionType.messageType.optionTypeLen = 1;
    if(bAccept)
    {
        optionType.messageType.byteVal = DHCP_ACK_MESSAGE;
    }
    optionTypeTotalLen = sizeof(optionType.messageType);

  // Option: Subnet Mask
    optionType.subnetmaskType.optionType = DHCP_SUBNET_MASK;
    optionType.subnetmaskType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.subnetmaskType.intVal = pNetIf->netMask.Val;
    optionTypeTotalLen += sizeof(optionType.subnetmaskType);

    // Option: Server identifier
    optionType.serverIdentifierType.optionType = DHCP_SERVER_IDENTIFIER;
    optionType.serverIdentifierType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.serverIdentifierType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.serverIdentifierType);
  

    // Option: Router/Gateway address
    optionType.routerType.optionType = DHCP_ROUTER;
    optionType.routerType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.routerType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.routerType);

    // Option: DNS server address
    optionType.dnsType.optionType = DHCP_DNS;
    optionType.dnsType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.dnsType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.dnsType);

    // copy the DHCP Reply Option type contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&optionType,&putBuffer,optionTypeTotalLen);

    // No more options, mark ending
    TCPIP_DHCPS_DataCopyToProcessBuffer(DHCP_END_OPTION,&putBuffer);

    // Add zero padding to ensure compatibility with old BOOTP relays that discard small packets (<300 UDP octets)
    while(putBuffer.wrPtr < putBuffer.endPtr)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer);
    }
// Put completed for DHCP packet buffer to the UDP buffer


    IP_MULTI_ADDRESS tmp_MultiAddr; tmp_MultiAddr.v4Add = pNetIf->netIPAddr;
    TCPIP_UDP_SourceIPAddressSet(s,IP_ADDRESS_TYPE_IPV4,&tmp_MultiAddr);

    // Once it is completed writing into the buffer, you need to update the Tx offset again,
    // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
    TCPIP_UDP_TxOffsetSet(s,(uint16_t)(putBuffer.wrPtr - putBuffer.head), false);
    
    // Transmit the packet
    TCPIP_UDP_Flush(s);
}

//Replies to a DHCP Request message.
static void DHCPReplyToRequest(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, bool bAccept, bool bRenew,
                        DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf,DHCP_SRVR_DCPT * pDhcpsDcpt)
{
    uint8_t         i;
    UDP_SOCKET      s;
    IPV4_ADDR       ipAddr;
    TCPIP_DHCPS_DATA   putBuffer;
    OA_HASH_ENTRY   *hE = 0;
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
        //TCPIP_UDP_Discard(s);
         return;
    }
    //this will put the start pointer at the beginning of the TX buffer
    TCPIP_UDP_TxOffsetSet(s,0,false);
    // Search through all remaining options and look for the Requested IP address field
    // Obtain options
    while(getBuf->endPtr-getBuf->wrPtr)
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
                if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,(uint8_t*)&tmp_MacAddr, 6))
                {
                    return;
                }
                Len -= 7;
                hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &tmp_MacAddr);
                if(hE !=0)
                {
                    IPV4_ADDR ClientIP = boot_header->ClientIP;
                    if(TCPIP_DHCPS_HashIPKeyCompare(pdhcpsHashDcpt->hashDcpt, hE, ClientIP.v) == 0)
                    {
                        DHCPS_HASH_ENTRY * dhcpsHE = (DHCPS_HASH_ENTRY *)hE;
                        // update lease time;
                        _DHCPSUpdateEntry(dhcpsHE);
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
                break;
            }
        }
        else
        {
            if((Option == DHCP_PARAM_REQUEST_IP_ADDRESS) && (Len == 4u))
            {
                // Get the requested IP address and see if it is the one we have on offer.  
                // If not, we should send back a NAK, but since there could be some other
                // DHCP server offering this address, we'll just silently ignore this request.
                if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,(uint8_t*)&dw, 4))
                {
                    return;
                }
                Len -= 4;
                hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &boot_header->ClientMAC);
                if(hE != 0)
                {
                    if(TCPIP_DHCPS_HashIPKeyCompare(pdhcpsHashDcpt->hashDcpt, hE, (uint8_t*)&dw) == 0)
                    {
                        DHCPS_HASH_ENTRY * dhcpsHE = (DHCPS_HASH_ENTRY *)hE;
                        bAccept = true;
                        _DHCPSUpdateEntry(dhcpsHE);
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
                    if(_DHCPSAddCompleteEntry(pNetIf->netIfIx, (uint8_t*)&dw, &boot_header->ClientMAC, DHCPS_FLAG_ENTRY_COMPLETE)!= DHCPS_RES_OK)
                        return ;
                }
                break;
            }
        }
            // Remove the unprocessed bytes that we don't care about
        if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,NULL, Len))
        {
            break;
        }
        Len = 0;
    }

    //Get the write pointer:
    putBuffer.head=putBuffer.wrPtr = TCPIP_UDP_TxPointerGet(s);
    // set the write and end pointer for DHCP server data
    putBuffer.wrPtr = putBuffer.head;
    putBuffer.endPtr = putBuffer.head+DHCPS_MAX_REPONSE_PACKET_SIZE;

    memset((void*)&rxHeader,0,sizeof(BOOTP_HEADER));
    
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
        if(hE != 0)
        {
            ipAddr = ((DHCPS_HASH_ENTRY*)hE)->ipAddress;
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
    memcpy(&rxHeader.ClientMAC,&boot_header->ClientMAC,sizeof(TCPIP_MAC_ADDR));

    // copy the BOOT RX header contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&rxHeader,&putBuffer,sizeof(BOOTP_HEADER));
    for(i = 0; i < DHCPS_UNUSED_BYTES_FOR_TX; i++)	// Remaining 10 bytes of client hardware address, server host name: Null string (not used)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer); // Boot filename: Null string (not used)
    }

    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&magicCookie, &putBuffer, sizeof(magicCookie));

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

     //    // Option: Subnet Mask
    optionType.subnetmaskType.optionType = DHCP_SUBNET_MASK;
    optionType.subnetmaskType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.subnetmaskType.intVal = pNetIf->netMask.Val;
    optionTypeTotalLen += sizeof(optionType.subnetmaskType);

     // Option: Server identifier
    optionType.serverIdentifierType.optionType = DHCP_SERVER_IDENTIFIER;
    optionType.serverIdentifierType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.serverIdentifierType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.serverIdentifierType);

    // Option: Router/Gateway address
    optionType.routerType.optionType = DHCP_ROUTER;
    optionType.routerType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.routerType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.routerType);

    // Option: DNS server address
    optionType.dnsType.optionType = DHCP_DNS;
    optionType.dnsType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.dnsType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.dnsType);

     // Option: Lease duration
    optionType.ipLeaseTimeType.optionType = DHCP_IP_LEASE_TIME;
    optionType.ipLeaseTimeType.optionTypeLen = 4;
    optionType.ipLeaseTimeType.intVal = pdhcpsHashDcpt->leaseDuartion;
    optionType.ipLeaseTimeType.intVal = TCPIP_Helper_htonl(optionType.ipLeaseTimeType.intVal);
    optionTypeTotalLen += sizeof(optionType.ipLeaseTimeType);

     // copy the DHCP Reply Option type contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&optionType,&putBuffer,optionTypeTotalLen);
    
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
        TCPIP_UDP_BcastIPV4AddressSet( s,UDP_BCAST_NETWORK_LIMITED,pNetIf);

    IP_MULTI_ADDRESS tmp_MultiAddr; tmp_MultiAddr.v4Add = pNetIf->netIPAddr;
    TCPIP_UDP_SourceIPAddressSet(s,IP_ADDRESS_TYPE_IPV4,&tmp_MultiAddr);

    // Once it is completed writing into the buffer, you need to update the Tx offset again,
    // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
    TCPIP_UDP_TxOffsetSet(s,(uint16_t)(putBuffer.wrPtr - putBuffer.head), false);

    // Transmit the packet
    TCPIP_UDP_Flush(s);
}

static bool isMacAddrEffective(const TCPIP_MAC_ADDR *macAddr)
{
    int i;
    for(i=0;i<6;i++)
    {
        if(macAddr->v[i] != 0) return true;
    }
    return false;
}

static bool _DHCPSAddCompleteEntry(int intfIdx, const uint8_t* pIPAddr, TCPIP_MAC_ADDR* hwAdd, DHCPS_ENTRY_FLAGS entryFlag)
{
    DHCPS_HASH_DCPT  *pdhcpsDcpt;
    OA_HASH_ENTRY   *hE;
    DHCPS_HASH_ENTRY* dhcpsHE;

    pdhcpsDcpt = &gPdhcpsHashDcpt;
    
    hE = TCPIP_OAHASH_EntryLookupOrInsert(pdhcpsDcpt->hashDcpt, hwAdd);
    if(hE == 0)
    {   // oops, hash full?
        return DHCPS_RES_CACHE_FULL;
    }

    // now in cache
    dhcpsHE = (DHCPS_HASH_ENTRY*)hE;
    
    if(dhcpsHE->hEntry.flags.newEntry != 0)
    {   // populate the new entry
    	dhcpsHE->intfIdx = intfIdx;
        _DHCPSSetHashEntry(dhcpsHE, entryFlag, hwAdd, pIPAddr);
    }
    else
    {   // existent entry
        _DHCPSUpdateEntry(dhcpsHE);
    }

    return DHCPS_RES_OK;
}


static void TCPIP_DHCPS_TaskForLeaseTime(void)
{
    uint32_t current_timer = SYS_TMR_TickCountGet();
    DHCPS_HASH_ENTRY* dhcpsHE;
    DHCPS_HASH_DCPT  *pdhcpsDcpt;
    OA_HASH_ENTRY	*hE;
    int bktIx=0;
    OA_HASH_DCPT	*pOH;
    TCPIP_NET_IF* pNetIf;

    pdhcpsDcpt = &gPdhcpsHashDcpt;
    pOH = pdhcpsDcpt->hashDcpt;

    if(gPdhcpSDcpt == NULL)
    {
        return;
    }
    
    pNetIf = (TCPIP_NET_IF*)TCPIP_UDP_SocketNetGet(dhcps_mod.uSkt);

// check the lease values and if there is any entry whose lease value exceeds the lease duration remove the lease entries from the HASH.

    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        hE = TCPIP_OAHASH_EntryGet(pOH, bktIx);        
    	if((hE->flags.busy != 0) && (hE->flags.value & DHCPS_FLAG_ENTRY_COMPLETE))
    	{
            dhcpsHE = (DHCPS_HASH_ENTRY*)hE;
            if((current_timer - dhcpsHE->Client_Lease_Time) >= pdhcpsDcpt->leaseDuartion* SYS_TMR_TickCounterFrequencyGet())
            {
                dhcpsHE->Client_Lease_Time = 0;
                TCPIP_OAHASH_EntryRemove(pOH,hE);
            }
    	}// Check if there is any entry whose DHCPS flag is INCOMPLETE, 
        // i,e DHCPS server did not receive the request from the client regarding that leased address.
        else if((hE->flags.busy != 0) && (hE->flags.value & DHCPS_FLAG_ENTRY_INCOMPLETE))
    	{
            dhcpsHE = (DHCPS_HASH_ENTRY*)hE;
            if((current_timer - dhcpsHE->pendingTime) >= TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK* SYS_TMR_TickCounterFrequencyGet())
            {
                dhcpsHE->pendingTime = 0;
                TCPIP_OAHASH_EntryRemove(pOH,hE);
            }
    	}
        // remove the entry if the link is down or Wifi Mac is not connected
        if(hE->flags.busy != 0)
        {
            dhcpsHE = (DHCPS_HASH_ENTRY*)hE;
            pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(dhcpsHE->intfIdx);
            if(pNetIf && !TCPIP_STACK_NetworkIsLinked(pNetIf))
            {                
                dhcpsHE->pendingTime = 0;
                TCPIP_OAHASH_EntryRemove(pOH,hE);
            }
        }
    }
}

static DHCPS_RESULT preAssignToDHCPClient(TCPIP_NET_IF  *pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt)
{
    OA_HASH_ENTRY   	*hE;
    IPV4_ADDR		  tempIpv4Addr;
    size_t bktIx = 0;

    if(false == isMacAddrEffective(&(Header->ClientMAC))) return -1;
	
    // Find in Pool, look for the same MAC addr
    hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC);
    if(hE !=0)
    {
        return DHCPS_RES_OK;
    }
    else
    {
        if(dhcps_mod.dhcpNextLease.Val == 0)
        {
            bktIx = DHCPSgetFreeHashIndex(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC,&pDhcpsDcpt->intfAddrsConf.startIPAddress);
            if(bktIx == -1)
            {
                return DHCPS_RES_CACHE_FULL;
            }
            tempIpv4Addr.v[0] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[0] & 0xFF;
            tempIpv4Addr.v[1] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[1] & 0xFF;
            tempIpv4Addr.v[2] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[2] & 0xFF;
            tempIpv4Addr.v[3] = (pDhcpsDcpt->intfAddrsConf.startIPAddress.v[3] & 0xFF) + bktIx;
        }
        else
        {
            tempIpv4Addr.Val = dhcps_mod.dhcpNextLease.Val;
        }
        /* this decided entry to the HASH POOL with a DHCPS_FLAG_ENTRY_INCOMPLETE flag
        After receiving Request and before sending ACK , make this entry to DHCPS_FLAG_ENTRY_COMPLETE
        */
        if(_DHCPSAddCompleteEntry(pNetIf->netIfIx, tempIpv4Addr.v, &Header->ClientMAC, DHCPS_FLAG_ENTRY_INCOMPLETE) != DHCPS_RES_OK)
                return DHCPS_RES_CACHE_FULL;
    }

    return DHCPS_RES_OK;
}


int TCPIP_DHCPS_HashMACKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    return memcmp((void*)&((DHCPS_HASH_ENTRY*)hEntry)->hwAdd, key, DHCPS_HASH_KEY_SIZE);
}

int TCPIP_DHCPS_HashIPKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const uint8_t* key)
{
    return memcmp(((DHCPS_HASH_ENTRY*)hEntry)->ipAddress.v, key, sizeof(((DHCPS_HASH_ENTRY*)hEntry)->ipAddress));
}

void TCPIP_DHCPS_HashIPKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    memcpy(((DHCPS_HASH_ENTRY*)dstEntry)->ipAddress.v, key, sizeof(((DHCPS_HASH_ENTRY*)dstEntry)->ipAddress));
}

void TCPIP_DHCPS_HashMACKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    memcpy(&(((DHCPS_HASH_ENTRY*)dstEntry)->hwAdd), key, DHCPS_HASH_KEY_SIZE);
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
        if(pBkt->flags.busy != 0)
        {
            pE = (DHCPS_HASH_ENTRY*)pBkt;
            if((current_timer - pE->Client_Lease_Time) >= TCPIP_DHCPS_LEASE_DURATION* SYS_TMR_TickCounterFrequencyGet())
            {
                return pBkt;
            }
        }
    }
    return 0;
}

// to get the free index and to use it for calculating Leased IP address.
static size_t DHCPSgetFreeHashIndex(OA_HASH_DCPT* pOH,void* key,IPV4_ADDR *ipAddr)
{
    OA_HASH_ENTRY*  pBkt;
    size_t      bktIx=-1;
    size_t      probeStep=0;
    size_t      bkts = 0;
    TCPIP_UINT32_VAL   dw;

	
#if defined(OA_DOUBLE_HASH_PROBING)
    probeStep = TCPIP_DHCPS_HashProbeHash(pOH, key);
    if(probeStep == 0)
    {	// try to avoid probing the same bucket over and over again
            // when probeHash returns 0.
            probeStep = pOH->probeStep;
    }
#else
    probeStep = pOH->probeStep;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
	
    bktIx = TCPIP_DHCPS_MACHashKeyHash(pOH,key);

    if(bktIx < 0)
    {
        bktIx += pOH->hEntries;
    }
		
    while(bkts < pOH->hEntries)
    {
        pBkt = TCPIP_OAHASH_EntryGet(pOH, bktIx);
        dw.Val = ipAddr->Val;
        dw.v[3] |= bktIx;
        if((pBkt->flags.busy == 0) &&(DHCPSLocateRequestedIpAddress((IPV4_ADDR*)&dw.Val)==DHCPS_RES_NO_ENTRY))
        {	// found unused entry
            return bktIx;
        }
        // advance to the next hash slot
        bktIx += probeStep;
        if(bktIx < 0)
        {
            bktIx += pOH->hEntries;
        }
        else if(bktIx >= pOH->hEntries)
        {
            bktIx -= pOH->hEntries;
        }

        bkts++;
    }
    return -1;	// cache full, not found
}

static DHCPS_RESULT DHCPSLocateRequestedIpAddress(IPV4_ADDR *requestedIpAddr)
{
    DHCPS_HASH_DCPT *pdhcpsDcpt;
    OA_HASH_ENTRY	*hE;
    int 			bktIx=0;
    OA_HASH_DCPT	*pOH;

    pdhcpsDcpt = &gPdhcpsHashDcpt;
    pOH = pdhcpsDcpt->hashDcpt;

// check the Requested Ip address is matching anyone of the hash entry.
    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        hE = TCPIP_OAHASH_EntryGet(pOH, bktIx);
        if((hE->flags.busy != 0) && (hE->flags.value & DHCPS_FLAG_ENTRY_COMPLETE))
        {
            if(TCPIP_DHCPS_HashIPKeyCompare(pOH, hE, requestedIpAddr->v) == 0)
                return DHCPS_RES_OK;
        }
    }
    return DHCPS_RES_NO_ENTRY;
}

TCPIP_DHCPS_LEASE_HANDLE TCPIP_DHCPS_LeaseEntryGet(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_LEASE_ENTRY* pLeaseEntry, TCPIP_DHCPS_LEASE_HANDLE leaseHandle)
{
    int                 entryIx;
    OA_HASH_DCPT*       pOH;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*	pDSHashDcpt;
    uint32_t 		current_time = SYS_TMR_TickCountGet();
    
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
  
    if(pNetIf == 0 || pNetIf->Flags.bIsDHCPSrvEnabled == 0)
    {
        return 0;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;
    if(pOH != 0)
    {   // DHCP Server proper initialized
        for(entryIx = (int)leaseHandle; entryIx < pOH->hEntries; entryIx++)
        {
            pDsEntry = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, entryIx);
            
            if(pDsEntry && (pDsEntry->hEntry.flags.busy != 0) && (pDsEntry->hEntry.flags.value & DHCPS_FLAG_ENTRY_COMPLETE) )
            {                
                if(pDsEntry->intfIdx != TCPIP_STACK_NetIxGet(pNetIf))
            	{
                    continue;
            	}
                // found entry
                if(pLeaseEntry)
                {
                    memcpy(&pLeaseEntry->hwAdd, &pDsEntry->hwAdd, sizeof(pDsEntry->hwAdd));
                    pLeaseEntry->ipAddress.Val = pDsEntry->ipAddress.Val;
                    pLeaseEntry->leaseTime = pDSHashDcpt->leaseDuartion*SYS_TMR_TickCounterFrequencyGet() - (current_time - pDsEntry->Client_Lease_Time);
                }
                return (TCPIP_DHCPS_LEASE_HANDLE)(entryIx + 1);
            }
        }
    }
    // no other entry
    return 0;
}

int TCPIP_DHCPS_GetPoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type)
{
    int                 entryIx;
    int                 noOfEntries=0;
    OA_HASH_DCPT*       pOH;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*	pDSHashDcpt;
	
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf == 0 || pNetIf->Flags.bIsDHCPSrvEnabled == 0)
    {
        return 0;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;
    
    if(pOH != 0)
    {   // DHCP Server proper initialized
        for(entryIx = (int)0; entryIx < pOH->hEntries; entryIx++)
        {
            pDsEntry = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, entryIx);
            
            switch(type)
            {
                case DHCP_SERVER_POOL_ENTRY_ALL:
                    if(pDsEntry && (pDsEntry->hEntry.flags.busy != 0))
                    {
                        if(pDsEntry->intfIdx != TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        noOfEntries++;
                    }
                    break;
                case DHCP_SERVER_POOL_ENTRY_IN_USE:
                    if(pDsEntry && (pDsEntry->hEntry.flags.busy != 0) && (pDsEntry->hEntry.flags.value & DHCPS_FLAG_ENTRY_COMPLETE) != 0)
                    {
                        if(pDsEntry->intfIdx != TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        noOfEntries++;
                    }
                    break;
            }

        }
    }
    return noOfEntries;
        
}

bool TCPIP_DHCPS_LeaseEntryRemove(TCPIP_NET_HANDLE netH, TCPIP_MAC_ADDR* hwAdd)
{
    OA_HASH_DCPT*       pOH;
    OA_HASH_ENTRY*	hE;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*	pDSHashDcpt;
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf == 0 || pNetIf->Flags.bIsDHCPSrvEnabled == 0)
    {
        return false;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;

    if(hwAdd)
    {
    	hE = TCPIP_OAHASH_EntryLookup(pOH,hwAdd);
        if(hE != 0)
        {
            pDsEntry = (DHCPS_HASH_ENTRY*)hE;
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
    int                 entryIx;
    OA_HASH_DCPT*       pOH;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*	pDSHashDcpt;
	
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf == 0 || pNetIf->Flags.bIsDHCPSrvEnabled == 0)
    {
        return false;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;
    
    if(pOH != 0)
    {   // DHCP Server proper initialized
        for(entryIx = (int)0; entryIx < pOH->hEntries; entryIx++)
        {
            pDsEntry = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, entryIx);
            
            switch(type)
            {
                case DHCP_SERVER_POOL_ENTRY_ALL:
                    if(pDsEntry && (pDsEntry->hEntry.flags.busy != 0))
                    {
                        if(pDsEntry->intfIdx != TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        TCPIP_OAHASH_EntryRemove(pOH,&pDsEntry->hEntry);
                    }
                    break;
                case DHCP_SERVER_POOL_ENTRY_IN_USE:
                    if(pDsEntry && (pDsEntry->hEntry.flags.busy != 0) && (pDsEntry->hEntry.flags.value & DHCPS_FLAG_ENTRY_COMPLETE) != 0)
                    {
                        if(pDsEntry->intfIdx != TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        TCPIP_OAHASH_EntryRemove(pOH,&pDsEntry->hEntry);
                    }
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

static bool _DHCPSDescriptorGetFromIntf(TCPIP_NET_IF *pNetIf,uint32_t *dcptIdx)
{
    int ix=0;
    DHCP_SRVR_DCPT *pDhcpServDcpt=NULL;


    for(ix = 0, pDhcpServDcpt = gPdhcpSDcpt; ix < dhcps_mod.poolCount; ix++, pDhcpServDcpt++)
    {
        if(pDhcpServDcpt == NULL)
        {
            return false;
        }
        if(pNetIf->netIfIx != pDhcpServDcpt->netIx )
        {
            continue;
        }
        else
        {
            *dcptIdx = (uint32_t)ix;
            return true;
        }
    }
    return false;
}


bool TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet)
{    
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
 
    if(pNetIf == 0)
    {
        return false;
    }
//  Now stop DHCP server
    _DHCPSrvClose(pNetIf,true);
    TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPS, TCPIP_STACK_ADDRESS_SERVICE_EVENT_USER_STOP);
    TCPIP_STACK_AddressServiceDefaultSet(pNetIf);
    _TCPIPStackSetConfigAddress(pNetIf, 0, 0, true);
     // Remove all the HASH entries
    _DHCPSRemoveCacheEntries(&gPdhcpsHashDcpt);
    return true;
}

static bool _DHCPS_StartOperation(TCPIP_NET_IF* pNetIf,DHCP_SRVR_DCPT* pDhcpsDcpt)
{
    uint32_t poolIndex=0;

    if(gPdhcpSDcpt == NULL)
    {
        return false;
    }

    if( dhcps_mod.uSkt == INVALID_UDP_SOCKET)
    {
        dhcps_mod.uSkt = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, TCPIP_DHCP_SERVER_PORT,  0);
        if(dhcps_mod.uSkt == INVALID_UDP_SOCKET)
        {
            return false;
        }
        TCPIP_UDP_SignalHandlerRegister(dhcps_mod.uSkt, TCPIP_UDP_SIGNAL_RX_DATA, TCPIP_DHCPSSocketRxSignalHandler, 0);
    }
    dhcps_mod.smServer = DHCP_SERVER_LISTEN;

    pNetIf->Flags.bIsDHCPSrvEnabled = true;
    //if there is no pool for any interface , use the 0th index pool configuration details
    // or the default for the interface configuration. here the server IP address will be same for
    // more than interface, if the address pool is not configured.
    if(_DHCPSDescriptorGetFromIntf(pNetIf,&poolIndex) == false)
    {
        poolIndex =0;
    }
    pDhcpsDcpt = gPdhcpSDcpt+poolIndex;
    if(pDhcpsDcpt == NULL)
    {
        return false;
    }
// Get the network interface from the network index and configure IP address,
// Netmask and gateway and DNS
    _TCPIPStackSetConfigAddress(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverIPAddress, &pDhcpsDcpt->intfAddrsConf.serverMask, false);
    TCPIP_STACK_GatewayAddressSet(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverIPAddress);
#if defined(TCPIP_STACK_USE_DNS)
    if(pNetIf->Flags.bIsDNSServerAuto != 0)
    {
        TCPIP_STACK_PrimaryDNSAddressSet(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverDNS);
        TCPIP_STACK_SecondaryDNSAddressSet(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverDNS2);
    }
#endif

    return true;
}

bool TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet)
{
    return _DHCPS_Enable(hNet, true);
}

static bool _DHCPS_Enable(TCPIP_NET_HANDLE hNet,bool checkIfUp)
{
    TCPIP_NET_IF* pNetIf ;
    DHCP_SRVR_DCPT* pDhcpsDcpt;

    if(checkIfUp)
    {
        pNetIf = _TCPIPStackHandleToNetUp(hNet);
    }
    else
    {
        pNetIf = _TCPIPStackHandleToNet(hNet);
    }
    pDhcpsDcpt = gPdhcpSDcpt;
    if((pNetIf==0)||(pDhcpsDcpt==0))
    {
        return false;
    }
    if(dhcps_mod.poolCount == 0)
    {
        SYS_ERROR(SYS_ERROR_WARNING, "DHCPS: DHCP server needs atleast one address pool! \r\n");
        return false;
    }
    if(checkIfUp)
    {
        if(TCPIP_STACK_AddressServiceCanStart(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPS))
        {
            return _DHCPS_StartOperation(pNetIf,pDhcpsDcpt);
        }
    }
    else
    {
        if((pNetIf->Flags.v & TCPIP_STACK_ADDRESS_SERVICE_MASK) == TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON)
        {
            return _DHCPS_StartOperation(pNetIf,pDhcpsDcpt);
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
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    if(pNetIf)
    {
        return pNetIf->Flags.bIsDHCPSrvEnabled != 0;
    }
    return false;
}


#else
bool TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet){return false;}
bool TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet){return false;}
bool TCPIP_DHCPS_IsEnabled(TCPIP_NET_HANDLE hNet){return false;}
#endif //#if defined(TCPIP_STACK_USE_DHCP_SERVER)
#endif // defined(TCPIP_STACK_USE_IPV4)

