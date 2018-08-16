/*******************************************************************************
  System Initialization File

  File Name:
    initialization.c

  Summary:
    This file contains source code necessary to initialize the system.

  Description:
    This file contains source code necessary to initialize the system.  It
    implements the "SYS_Initialize" function, defines the configuration bits,
    and allocates any necessary global system resources,
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017-2018 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
#include "definitions.h"


// ****************************************************************************
// ****************************************************************************
// Section: Configuration Bits
// ****************************************************************************
// ****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Driver Initialization Data
// *****************************************************************************
// *****************************************************************************

/* MIIM Driver Configuration */
static const DRV_MIIM_INIT drvMiimInitData =
{
    .ethphyId = DRV_MIIM_ETH_MODULE_ID,
};

// <editor-fold defaultstate="collapsed" desc="DRV_MEMORY Instance 0 Initialization Data">

uint8_t gDrvMemory0EraseBuffer[EFC0_ERASE_BUFFER_SIZE] __attribute__((aligned(32)));

DRV_MEMORY_CLIENT_OBJECT gDrvMemory0ClientObject[DRV_MEMORY_CLIENTS_NUMBER_IDX0] = { 0 };

DRV_MEMORY_BUFFER_OBJECT gDrvMemory0BufferObject[DRV_MEMORY_BUFFER_QUEUE_SIZE_IDX0] = { 0 };

const MEMORY_DEVICE_API drvMemory0DeviceAPI = {
    .Open               = DRV_EFC0_Open,
    .Close              = DRV_EFC0_Close,
    .Status             = DRV_EFC0_Status,
    .SectorErase        = DRV_EFC0_SectorErase,
    .Read               = DRV_EFC0_Read,
    .PageWrite          = DRV_EFC0_PageWrite,
    .GeometryGet        = (GEOMETRY_GET)DRV_EFC0_GeometryGet,
    .TransferStatusGet  = (TRANSFER_STATUS_GET)DRV_EFC0_TransferStatusGet
};

const DRV_MEMORY_INIT drvMemory0InitData =
{
    .memDevIndex          = 0,
    .memoryDevice         = &drvMemory0DeviceAPI,
    .isFsEnabled          = true,
    .deviceMediaType      = (uint8_t)SYS_FS_MEDIA_TYPE_NVM,
    .ewBuffer             = &gDrvMemory0EraseBuffer[0],
    .clientObjPool        = (uintptr_t)&gDrvMemory0ClientObject[0],
    .bufferObj            = (uintptr_t)&gDrvMemory0BufferObject[0],
    .queueSize            = DRV_MEMORY_BUFFER_QUEUE_SIZE_IDX0,
    .nClientsMax          = DRV_MEMORY_CLIENTS_NUMBER_IDX0
};

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="DRV_USART Instance 0 Initialization Data">

USART_PLIB_API drvUsart0PlibAPI = {
        .readCallbackRegister = (USART_ReadCallbackRegister)USART1_ReadCallbackRegister,
        .read = (USART_Read)USART1_Read,
        .readIsBusy = (USART_ReadIsBusy)USART1_ReadIsBusy,
        .readCountGet = (USART_ReadCountGet)USART1_ReadCountGet,
        .writeCallbackRegister = (USART_WriteCallbackRegister)USART1_WriteCallbackRegister,
        .write = (USART_Write)USART1_Write,
        .writeIsBusy = (USART_WriteIsBusy)USART1_WriteIsBusy,
        .writeCountGet = (USART_WriteCountGet)USART1_WriteCountGet,
        .errorGet = (USART_ErrorGet)USART1_ErrorGet,
        .serialSetup = (USART_SerialSetup)USART1_SerialSetup
};

DRV_USART_INIT drvUsart0InitData =
{
    .usartPlib = &drvUsart0PlibAPI,

    .dmaChannelTransmit = DMA_CHANNEL_NONE,

    .dmaChannelReceive = DMA_CHANNEL_NONE,

    .queueSizeTransmit = DRV_USART_XMIT_QUEUE_SIZE_IDX0,

    .queueSizeReceive = DRV_USART_RCV_QUEUE_SIZE_IDX0,

    .interruptUSART = USART1_IRQn,

    .interruptDMA = XDMAC_IRQn,

};

// </editor-fold>


// *****************************************************************************
// *****************************************************************************
// Section: System Data
// *****************************************************************************
// *****************************************************************************
/* Structure to hold the object handles for the modules in the system. */
SYSTEM_OBJECTS sysObj;
// *****************************************************************************
// *****************************************************************************
// Section: Library/Stack Initialization Data
// *****************************************************************************
// *****************************************************************************


/*** ETH PHY Initialization Data ***/



const DRV_ETHPHY_INIT tcpipPhyInitData =
{
    .ethphyId               = TCPIP_INTMAC_MODULE_ID,
    .phyAddress             = TCPIP_INTMAC_PHY_ADDRESS,
    .phyFlags               = TCPIP_INTMAC_PHY_CONFIG_FLAGS,
    .pPhyObject             = &DRV_ETHPHY_OBJECT_SMSC_LAN8740,
    .resetFunction          = 0,
    .pMiimObject            = &DRV_MIIM_OBJECT_BASE_Default,
    .pMiimInit              = &drvMiimInitData,
    .miimIndex              = DRV_MIIM_DRIVER_INDEX,

};


/* Net Presentation Layer Data Definitions */
#include "net/pres/net_pres_enc_glue.h"


static const NET_PRES_INST_DATA netPresCfgs[] = 
{  
    {
        .pProvObject_ss = NULL,
        .pProvObject_sc = NULL,
        .pProvObject_ds = NULL,
        .pProvObject_dc = NULL,
    },
};

static const NET_PRES_INIT_DATA netPresInitData = 
{
    .numLayers = sizeof(netPresCfgs) / sizeof(NET_PRES_INST_DATA),
    .pInitData = netPresCfgs
};
  
 

/*** File System Initialization Data ***/


const SYS_FS_MEDIA_MOUNT_DATA sysfsMountTable[SYS_FS_VOLUME_NUMBER] = 
{
	{NULL}
};



const SYS_FS_REGISTRATION_TABLE sysFSInit [ SYS_FS_MAX_FILE_SYSTEM_TYPE ] =
{
    {
        .nativeFileSystemType = MPFS2,
        .nativeFileSystemFunctions = &MPFSFunctions
    }
};



// <editor-fold defaultstate="collapsed" desc="TCP/IP Stack Initialization Data">
// *****************************************************************************
// *****************************************************************************
// Section: TCPIP Data
// *****************************************************************************
// *****************************************************************************
/*** ARP Service Initialization Data ***/
const TCPIP_ARP_MODULE_CONFIG tcpipARPInitData =
{ 
    .cacheEntries       = TCPIP_ARP_CACHE_ENTRIES,     
    .deleteOld          = TCPIP_ARP_CACHE_DELETE_OLD,    
    .entrySolvedTmo     = TCPIP_ARP_CACHE_SOLVED_ENTRY_TMO, 
    .entryPendingTmo    = TCPIP_ARP_CACHE_PENDING_ENTRY_TMO, 
    .entryRetryTmo      = TCPIP_ARP_CACHE_PENDING_RETRY_TMO, 
    .permQuota          = TCPIP_ARP_CACHE_PERMANENT_QUOTA, 
    .purgeThres         = TCPIP_ARP_CACHE_PURGE_THRESHOLD, 
    .purgeQuanta        = TCPIP_ARP_CACHE_PURGE_QUANTA, 
    .retries            = TCPIP_ARP_CACHE_ENTRY_RETRIES, 
    .gratProbeCount     = TCPIP_ARP_GRATUITOUS_PROBE_COUNT,
};

/*** Announce Discovery Initialization Data ***/
const TCPIP_ANNOUNCE_MODULE_CONFIG tcpipAnnounceInitData =
{ 
};

/*** UDP Sockets Initialization Data ***/
const TCPIP_UDP_MODULE_CONFIG tcpipUDPInitData =
{
    .nSockets       = TCPIP_UDP_MAX_SOCKETS,
    .sktTxBuffSize  = TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE, 
};

/*** TCP Sockets Initialization Data ***/
const TCPIP_TCP_MODULE_CONFIG tcpipTCPInitData =
{
    .nSockets       = TCPIP_TCP_MAX_SOCKETS,
    .sktTxBuffSize  = TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE, 
    .sktRxBuffSize  = TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE,
};

/*** HTTP Server Initialization Data ***/
const TCPIP_HTTP_MODULE_CONFIG tcpipHTTPInitData =
{
    .nConnections   = TCPIP_HTTP_MAX_CONNECTIONS,
    .dataLen		= TCPIP_HTTP_MAX_DATA_LEN,
    .sktTxBuffSize	= TCPIP_HTTP_SKT_TX_BUFF_SIZE,
    .sktRxBuffSize	= TCPIP_HTTP_SKT_RX_BUFF_SIZE,
    .configFlags	= TCPIP_HTTP_CONFIG_FLAGS,
    .http_malloc_fnc    = TCPIP_HTTP_MALLOC_FUNC,
    .http_free_fnc      = TCPIP_HTTP_FREE_FUNC,
};


/*** SNTP Client Initialization Data ***/
const TCPIP_SNTP_MODULE_CONFIG tcpipSNTPInitData =
{
    .ntp_server		        = TCPIP_NTP_SERVER,
    .ntp_interface		    = TCPIP_NTP_DEFAULT_IF,
    .ntp_connection_type	= TCPIP_NTP_DEFAULT_CONNECTION_TYPE,
    .ntp_reply_timeout		= TCPIP_NTP_REPLY_TIMEOUT,
    .ntp_stamp_timeout		= TCPIP_NTP_TIME_STAMP_TMO,
    .ntp_success_interval	= TCPIP_NTP_QUERY_INTERVAL,
    .ntp_error_interval		= TCPIP_NTP_FAST_QUERY_INTERVAL,
};


/*** SMTPC client Initialization Data ***/
const TCPIP_SMTPC_MODULE_CONFIG tcpipSMTPCInitData =
{
    .nMailConnections       = TCPIP_SMTPC_MAIL_CONNECTIONS,
    .serverReplyTmo         = TCPIP_SMTPC_SERVER_REPLY_TIMEOUT,
    .serverDataTmo          = TCPIP_SMTPC_SERVER_DATA_TIMEOUT,
    .tlsHandshakeTmo        = TCPIP_SMTPC_TLS_HANDSHAKE_TIMEOUT,
    .nMailRetries           = TCPIP_SMTPC_MAIL_RETRIES,
    .serverRetryTmo         = TCPIP_SMTPC_SERVER_TRANSIENT_RETRY_TIMEOUT,
    .smtpcRetryTmo          = TCPIP_SMTPC_INTERNAL_RETRY_TIMEOUT,
    .sktTxBuffSize          = TCPIP_SMTPC_SKT_TX_BUFF_SIZE,
    .sktRxBuffSize          = TCPIP_SMTPC_SKT_RX_BUFF_SIZE,
};

/*** DHCP client Initialization Data ***/
const TCPIP_DHCP_MODULE_CONFIG tcpipDHCPInitData =
{     
    .dhcpEnable     = TCPIP_DHCP_CLIENT_ENABLED,   
    .dhcpTmo        = TCPIP_DHCP_TIMEOUT,
    .dhcpCliPort    = TCPIP_DHCP_CLIENT_CONNECT_PORT,
    .dhcpSrvPort    = TCPIP_DHCP_SERVER_LISTEN_PORT,

};


/*** ICMP Server Initialization Data ***/
const TCPIP_ICMP_MODULE_CONFIG tcpipICMPInitData = 
{
};

/*** NBNS Server Initialization Data ***/
const TCPIP_NBNS_MODULE_CONFIG tcpipNBNSInitData =
{ 
};


/*** GMAC MAC Initialization Data ***/
const TCPIP_MODULE_MAC_PIC32C_CONFIG tcpipMACPIC32CINTInitData =
{ 
	/** QUEUE 0 Intialization**/
	.gmac_queue_config[0].queueEnable	= true,
	.gmac_queue_config[0].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0,
	.gmac_queue_config[0].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0,
	.gmac_queue_config[0].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE0,
	.gmac_queue_config[0].txBufferSize	= TCPIP_GMAC_TX_BUFF_SIZE_QUE0,
	
	/** QUEUE 1 Intialization**/
	.gmac_queue_config[1].queueEnable	= false,	
	.gmac_queue_config[1].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1,
	.gmac_queue_config[1].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1,
	.gmac_queue_config[1].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE1,
	.gmac_queue_config[1].txBufferSize	= TCPIP_GMAC_TX_BUFF_SIZE_QUE1,

	/** QUEUE 2 Intialization**/
	.gmac_queue_config[2].queueEnable	= false,	
	.gmac_queue_config[2].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2,
	.gmac_queue_config[2].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2,
	.gmac_queue_config[2].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE2,
	.gmac_queue_config[2].txBufferSize	= TCPIP_GMAC_TX_BUFF_SIZE_QUE2,

	/** QUEUE 3 Intialization**/
	.gmac_queue_config[3].queueEnable	= false,	
	.gmac_queue_config[3].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3,
	.gmac_queue_config[3].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3,
	.gmac_queue_config[3].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE3,
	.gmac_queue_config[3].txBufferSize	= TCPIP_GMAC_TX_BUFF_SIZE_QUE3,

	/** QUEUE 4 Intialization**/
	.gmac_queue_config[4].queueEnable	= false,	
	.gmac_queue_config[4].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4,
	.gmac_queue_config[4].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4,
	.gmac_queue_config[4].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE4,
	.gmac_queue_config[4].txBufferSize	= TCPIP_GMAC_TX_BUFF_SIZE_QUE4,

	/** QUEUE 5 Intialization**/
	.gmac_queue_config[5].queueEnable	= false,	
	.gmac_queue_config[5].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5,
	.gmac_queue_config[5].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5,
	.gmac_queue_config[5].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE5,
	.gmac_queue_config[5].txBufferSize	= TCPIP_GMAC_TX_BUFF_SIZE_QUE5,
	
	.ethFlags               = TCPIP_GMAC_ETH_OPEN_FLAGS,	
	.linkInitDelay          = TCPIP_INTMAC_PHY_LINK_INIT_DELAY,
    .ethModuleId            = TCPIP_INTMAC_MODULE_ID,
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_Default,
    .pPhyInit               = &tcpipPhyInitData,
};











/*** DNS Client Initialization Data ***/
const TCPIP_DNS_CLIENT_MODULE_CONFIG tcpipDNSClientInitData =
{
    .deleteOldLease         = TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES,
    .cacheEntries           = TCPIP_DNS_CLIENT_CACHE_ENTRIES,
    .entrySolvedTmo         = TCPIP_DNS_CLIENT_CACHE_ENTRY_TMO,    
    .nIPv4Entries  = TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS,
    .ipAddressType       = TCPIP_DNS_CLIENT_ADDRESS_TYPE,
    .nIPv6Entries  = TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS,
};


/*** IPv6 Initialization Data ***/
const TCPIP_IPV6_MODULE_CONFIG  tcpipIPv6InitData = 
{
    .rxfragmentBufSize      = TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE,
    .fragmentPktRxTimeout   = TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT,
};


TCPIP_STACK_HEAP_INTERNAL_CONFIG tcpipHeapConfig =
{
    .heapType = TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP,
    .heapFlags = TCPIP_STACK_HEAP_USE_FLAGS,
    .heapUsage = TCPIP_STACK_HEAP_USAGE_CONFIG,
    .malloc_fnc = TCPIP_STACK_MALLOC_FUNC,
    .calloc_fnc = TCPIP_STACK_CALLOC_FUNC,
    .free_fnc = TCPIP_STACK_FREE_FUNC,
    .heapSize = TCPIP_STACK_DRAM_SIZE,
};


const TCPIP_NETWORK_CONFIG __attribute__((unused))  TCPIP_HOSTS_CONFIGURATION[] =
{
	/*** Network Configuration Index 0 ***/
    {
        TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0,       // interface
        TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX0,            // hostName
        TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX0,             // macAddr
        TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX0,           // ipAddr
        TCPIP_NETWORK_DEFAULT_IP_MASK_IDX0,              // ipMask
        TCPIP_NETWORK_DEFAULT_GATEWAY_IDX0,              // gateway
        TCPIP_NETWORK_DEFAULT_DNS_IDX0,                  // priDNS
        TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX0,           // secondDNS
        TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX0,           // powerMode
        TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX0,      // startFlags
       &TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX0,           // pMacObject
    },
};



const TCPIP_STACK_MODULE_CONFIG TCPIP_STACK_MODULE_CONFIG_TBL [] =
{
    {TCPIP_MODULE_IPV4,             0},

    {TCPIP_MODULE_ICMP,             0},                             // TCPIP_MODULE_ICMP

    {TCPIP_MODULE_ARP,              &tcpipARPInitData},             // TCPIP_MODULE_ARP
    {TCPIP_MODULE_IPV6,             &tcpipIPv6InitData},            // TCPIP_MODULE_IPV6
    {TCPIP_MODULE_ICMPV6,           0},                             // TCPIP_MODULE_ICMPV6
    {TCPIP_MODULE_NDP,              0},                             // TCPIP_MODULE_NDP
    {TCPIP_MODULE_UDP,              &tcpipUDPInitData},             // TCPIP_MODULE_UDP
    {TCPIP_MODULE_TCP,              &tcpipTCPInitData},             // TCPIP_MODULE_TCP
    {TCPIP_MODULE_DHCP_CLIENT,      &tcpipDHCPInitData},            // TCPIP_MODULE_DHCP_CLIENT
    {TCPIP_MODULE_ANNOUNCE,         &tcpipAnnounceInitData},        // TCPIP_MODULE_ANNOUNCE
    {TCPIP_MODULE_DNS_CLIENT,       &tcpipDNSClientInitData},       // TCPIP_MODULE_DNS_CLIENT
    {TCPIP_MODULE_NBNS,             &tcpipNBNSInitData},            // TCPIP_MODULE_NBNS
    {TCPIP_MODULE_SNTP,             &tcpipSNTPInitData},            // TCPIP_MODULE_SNTP

    {TCPIP_MODULE_HTTP_SERVER,      &tcpipHTTPInitData},            // TCPIP_MODULE_HTTP_SERVER
    {TCPIP_MODULE_SMTPC, &tcpipSMTPCInitData},                                  // TCPIP_MODULE_SMTPC,
    { TCPIP_MODULE_MANAGER,         &tcpipHeapConfig },             // TCPIP_MODULE_MANAGER

// MAC modules
    {TCPIP_MODULE_MAC_PIC32C,     &tcpipMACPIC32CINTInitData},     // TCPIP_MODULE_MAC_PIC32C


};

/*********************************************************************
 * Function:        SYS_MODULE_OBJ TCPIP_STACK_Init()
 *
 * PreCondition:    None
 *
 * Input:
 *
 * Output:          valid system module object if Stack and its componets are initialized
 *                  SYS_MODULE_OBJ_INVALID otherwise
 *
 * Overview:        The function starts the initialization of the stack.
 *                  If an error occurs, the SYS_ERROR() is called
 *                  and the function de-initialize itself and will return false.
 *
 * Side Effects:    None
 *
 * Note:            This function must be called before any of the
 *                  stack or its component routines are used.
 *
 ********************************************************************/

SYS_MODULE_OBJ TCPIP_STACK_Init()
{
    TCPIP_STACK_INIT    tcpipInit;

    tcpipInit.pNetConf = TCPIP_HOSTS_CONFIGURATION;
    tcpipInit.nNets = sizeof (TCPIP_HOSTS_CONFIGURATION) / sizeof (*TCPIP_HOSTS_CONFIGURATION);
    tcpipInit.pModConfig = TCPIP_STACK_MODULE_CONFIG_TBL;
    tcpipInit.nModules = sizeof (TCPIP_STACK_MODULE_CONFIG_TBL) / sizeof (*TCPIP_STACK_MODULE_CONFIG_TBL);

    return TCPIP_STACK_Initialize(0, &tcpipInit.moduleInit);
}
// </editor-fold>



// *****************************************************************************
// *****************************************************************************
// Section: System Initialization
// *****************************************************************************
// *****************************************************************************
// <editor-fold defaultstate="collapsed" desc="SYS_CONSOLE Instance 0 Initialization Data">

SYS_MODULE_OBJ sysConsoleObjects[] = { SYS_MODULE_OBJ_INVALID };

/* Declared in console device implementation (sys_console_uart.c) */
extern SYS_CONSOLE_DEV_DESC consUsartDevDesc;

SYS_CONSOLE_INIT consUsartInit0 =
{
    .moduleInit = {0},
    .consDevDesc = &consUsartDevDesc,
};

SYS_DEBUG_INIT debugInit =
{
    .moduleInit = {0},
    .errorLevel = SYS_DEBUG_GLOBAL_ERROR_LEVEL
};

SYS_CMD_INIT sysCmdInit =
{
    .moduleInit = {0},
    .consoleCmdIOParam = SYS_CMD_SINGLE_CHARACTER_READ_CONSOLE_IO_PARAM,
};
// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="SYS_TIME Initialization Data">

TIME_PLIB_API sysTimePlibAPI = {
    .timerCallbackSet = (TIME_CallbackSet)TC0_CH0_TimerCallbackRegister,
    .timerCounterGet = (TIME_CounterGet)TC0_CH0_TimerCounterGet,
    .timerPeriodSet = (TIME_PeriodSet)TC0_CH0_TimerPeriodSet,
    .timerStart = (TIME_Start)TC0_CH0_TimerStart,
    .timerStop = (TIME_Stop)TC0_CH0_TimerStop
};

SYS_TIME_INIT sysTimeInitData =
{
    .timePlib = &sysTimePlibAPI,
    .timeInterrupt = TC0_CH0_IRQn,
    .timeFrequency = TC0_CH0_TimerFrequencyGet()
};

// </editor-fold>


/*******************************************************************************
  Function:
    void SYS_Initialize ( void *data )

  Summary:
    Initializes the board, services, drivers, application and other modules.

  Remarks:
 */

void SYS_Initialize ( void* data )
{
    CLK_Initialize();
	PIO_Initialize();

    NVIC_Initialize();
	RSWDT_REGS->RSWDT_MR|= RSWDT_MR_WDDIS_Msk;	// Disable RSWDT 
	WDT_REGS->WDT_MR|= WDT_MR_WDDIS_Msk; 		// Disable WDT 
	BSP_Initialize();
 
    TC0_CH0_TimerInitialize(); 
     
    
	USART1_Initialize();

    /* Initialize the MIIM Driver */
    sysObj.drvMiim = DRV_MIIM_Initialize(DRV_MIIM_INDEX_0, (const SYS_MODULE_INIT  * const)&drvMiimInitData);

    sysObj.drvMemory0 = DRV_MEMORY_Initialize((SYS_MODULE_INDEX)DRV_MEMORY_INDEX_0, (SYS_MODULE_INIT *)&drvMemory0InitData);

    sysObj.drvUsart0 = DRV_USART_Initialize(DRV_USART_INDEX_0, (SYS_MODULE_INIT *)&drvUsart0InitData);


    sysObj.sysConsole0 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_0, (SYS_MODULE_INIT *)&consUsartInit0);
    sysObj.sysDebug = SYS_DEBUG_Initialize(SYS_DEBUG_INDEX_0, (SYS_MODULE_INIT*)&debugInit);
    SYS_CMD_Initialize((SYS_MODULE_INIT*)&sysCmdInit);

    sysObj.sysTime = SYS_TIME_Initialize(SYS_TIME_INDEX_0, (SYS_MODULE_INIT *)&sysTimeInitData);

    sysObj.netPres = NET_PRES_Initialize(0, (SYS_MODULE_INIT*)&netPresInitData);

    /*** File System Service Initialization Code ***/
    SYS_FS_Initialize( (const void *) sysFSInit );


    /* TCPIP Stack Initialization */
    sysObj.tcpip = TCPIP_STACK_Init();
    SYS_ASSERT(sysObj.tcpip != SYS_MODULE_OBJ_INVALID, "TCPIP_STACK_Init Failed" );



    APP_Initialize();


}


/*******************************************************************************
 End of File
*/

