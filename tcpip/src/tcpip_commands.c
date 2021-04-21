/*******************************************************************************
  TCP/IP commands implementation

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    TCPIP stack commands entities. 
    Note, this module is based on system command parser
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2020 Microchip Technology Inc. and its subsidiaries.

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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_COMMAND

#include "tcpip/src/tcpip_private.h"
#include "tcpip/tcpip_manager.h"

#include "system/debug/sys_debug.h"
#include "system/command/sys_command.h"

#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)
#include "net_pres/pres/net_pres_socketapi.h"
#endif  // defined(TCPIP_STACK_USE_HTTP_NET_SERVER)

#if defined(TCPIP_STACK_COMMAND_ENABLE)

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ICMP_CLIENT) && (TCPIP_ICMP_COMMAND_ENABLE == true)
#define _TCPIP_COMMAND_PING4
#define _TCPIP_COMMAND_PING4_DEBUG      0   // enable/disable extra ping debugging messages
#endif

#if defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_ICMPV6_CLIENT) && (TCPIP_ICMPV6_CLIENT_USER_NOTIFICATION != 0)
#define _TCPIP_COMMAND_PING6
#endif

#if defined(DRV_MIIM_COMMANDS) && (DRV_MIIM_COMMANDS != 0)
#include "driver/miim/drv_miim.h"
#define _TCPIP_COMMANDS_MIIM
#endif

#if defined(_TCPIP_COMMAND_PING4) || defined(_TCPIP_COMMAND_PING6) || defined(TCPIP_STACK_USE_DNS) || defined(_TCPIP_COMMANDS_MIIM)
#define _TCPIP_STACK_COMMAND_TASK
#endif  // defined(_TCPIP_COMMAND_PING4) || defined(_TCPIP_COMMAND_PING6) || defined(TCPIP_STACK_USE_DNS) || defined(_TCPIP_COMMANDS_MIIM)


#if defined(TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && (TCPIP_STACK_CONFIGURATION_SAVE_RESTORE != 0)
#define _TCPIP_STACK_COMMANDS_STORAGE_ENABLE
#endif

static int  initialNetIfs = 0;    // Backup interfaces number for stack restart

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static TCPIP_STACK_INIT        cmdTcpipInitData;        // data that's used for the StackInit
static TCPIP_STACK_INIT*       pCmdTcpipInitData = 0;   // pointer to this data
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((TCPIP_STACK_DOWN_OPERATION != 0) || (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))
typedef struct
{
    size_t                  stgSize;        // size  + valid flag
    TCPIP_STACK_NET_IF_DCPT netDcptStg;     // configuration save
#if defined(TCPIP_STACK_USE_IPV6)
    uint8_t                 restoreBuff[sizeof(TCPIP_NETWORK_CONFIG) + 170]; // buffer to restore the configuration
#else
    uint8_t                 restoreBuff[sizeof(TCPIP_NETWORK_CONFIG) + 120]; // buffer to restore the configuration
#endif  // defined(TCPIP_STACK_USE_IPV6)
}TCPIP_COMMAND_STG_DCPT;

static TCPIP_COMMAND_STG_DCPT*   pCmdStgDcpt = 0;   // store current interface configuration
static TCPIP_NETWORK_CONFIG*     pCmdNetConf = 0;   // create the array of configurations needed for stack initialization

static bool                     tcpipCmdPreserveSavedInfo = false; // do not discard the saved data

#endif  // defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((TCPIP_STACK_DOWN_OPERATION != 0) || (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))

typedef enum 
{
    DNS_SERVICE_COMD_ADD=0,
    DNS_SERVICE_COMD_DEL,
    DNS_SERVICE_COMD_INFO,
    DNS_SERVICE_COMD_ENABLE_INTF,
    DNS_SERVICE_COMD_LOOKUP,
    DNS_SERVICE_COMD_NONE,
}DNS_SERVICE_COMD_TYPE;
typedef struct 
{
	const char *command;
	DNS_SERVICE_COMD_TYPE  val;
}DNSS_COMMAND_MAP;




static int _Command_NetInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int _Command_DefaultInterfaceSet (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#if defined(TCPIP_STACK_USE_IPV4)
static int _Command_AddressService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_STACK_ADDRESS_SERVICE_TYPE svcType);
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
static int _CommandDhcpOptions(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif 
static int _Command_DHCPSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int _Command_ZcllOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int _Command_DNSAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_STACK_USE_IPV4)
static int _Command_IPAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int _Command_GatewayAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int _Command_BIOSNameSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int _Command_MACAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#if (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
static int _Command_NetworkOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static int _Command_StackOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
static int _Command_HeapInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#if defined(TCPIP_STACK_USE_IPV4)
#if (TCPIP_ARP_COMMANDS != 0)
static void _CommandArp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // (TCPIP_ARP_COMMANDS != 0)
#endif  // defined(TCPIP_STACK_USE_IPV4)
static int _Command_MacInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#if defined(TCPIP_STACK_USE_TFTP_CLIENT)
static int _Command_TFTPC_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
#if defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
static int _CommandDhcpv6Options(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
#if defined(TCPIP_STACK_USE_TFTP_SERVER)
static int _Command_TFTPServerOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
#if defined(TCPIP_STACK_USE_DHCP_SERVER)
static int _Command_DHCPLeaseInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  //  defined(TCPIP_STACK_USE_DHCP_SERVER)
#if defined(TCPIP_STACK_USE_DNS)
static int _Command_DNSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int _Command_DNS_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int _Command_ShowDNSResolvedInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
#if defined(TCPIP_STACK_USE_DNS_SERVER)
static int _Command_DNSSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int _Command_AddDelDNSSrvAddress(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv,DNS_SERVICE_COMD_TYPE dnsCommand);
static int _Command_ShowDNSServInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int _Command_DnsServService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif

#if defined(TCPIP_STACK_USE_TFTP_CLIENT)
static char tftpServerHost[TCPIP_TFTPC_SERVERADDRESS_LEN];     // current target server
static char tftpcFileName[TCPIP_TFTPC_FILENAME_LEN]; // TFTP file name that will be for PUT and GET command
#endif

#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)
static int _Command_HttpInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#if (TCPIP_HTTP_NET_SSI_PROCESS != 0)
static int _Command_SsiInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
#endif

#if defined(TCPIP_STACK_USE_SMTPC) && defined(TCPIP_SMTPC_USE_MAIL_COMMAND)
static int _CommandMail(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_STACK_USE_SMTPC) && defined(TCPIP_SMTPC_USE_MAIL_COMMAND)


#if (TCPIP_UDP_COMMANDS)
static int _Command_UdpInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // (TCPIP_UDP_COMMANDS)

#if (TCPIP_TCP_COMMANDS)
static void _Command_TcpInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _Command_TcpTrace(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // (TCPIP_TCP_COMMANDS)

#if (TCPIP_PACKET_LOG_ENABLE)
static int _Command_PktLog(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _CommandPktLogInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _CommandPktLogClear(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _CommandPktLogReset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _CommandPktLogHandler(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _CommandPktLogType(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _CommandPktLogMask(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _CommandPktLogDefHandler(TCPIP_STACK_MODULE moduleId, const TCPIP_PKT_LOG_ENTRY* pLogEntry);

typedef enum
{
    CMD_PKT_XTRACT_RES_OK   = 0,    // all OK
    CMD_PKT_XTRACT_RES_CLR  = 1,    // all OK, 'clr' was requested


    CMD_PKT_XTRACT_RES_ERR  = -1,   // some error occurred

}CMD_PKT_XTRACT_RES;
static CMD_PKT_XTRACT_RES _CommandPktExtractMasks(int argc, char** argv, uint32_t* pAndMask, uint32_t* pOrMask);

static SYS_CMD_DEVICE_NODE*   _pktHandlerCmdIo = 0;

// table with the module names for logger purposes
// only basic modules supported
static const char* _CommandPktLogModuleNames[] = 
{
    "UNK",          // TCPIP_MODULE_NONE
    "MGR",          // TCPIP_MODULE_MANAGER
    "ARP",          // TCPIP_MODULE_ARP
    "IP4",          // TCPIP_MODULE_IPV4
    "IP6",          // TCPIP_MODULE_IPV6
    "LLDP",         // TCPIP_MODULE_LLDP
    "ICMP",         // TCPIP_MODULE_ICMP
    "ICMP6",        // TCPIP_MODULE_ICMPV6
    "NDP",          // TCPIP_MODULE_NDP
    "UDP",          // TCPIP_MODULE_UDP
    "TCP",          // TCPIP_MODULE_TCP
    "IGMP",         // TCPIP_MODULE_IGMP
    "LAYR3",        // TCPIP_MODULE_LAYER3
};


#endif  // (TCPIP_PACKET_LOG_ENABLE)

#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
static int _Command_PktInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
static int _Command_HeapList(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)
static int _Command_Announce(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)

#if defined(_TCPIP_STACK_COMMAND_TASK)
// command task status
typedef enum
{
    TCPIP_CMD_STAT_IDLE = 0,        // command task is idle

    // ping related status
    TCPIP_CMD_STAT_PING_START,      // starting ping commands

    TCPIP_PING_CMD_DNS_GET = TCPIP_CMD_STAT_PING_START,     // get DNS
    TCPIP_PING_CMD_DNS_WAIT,        // wait for DNS
    TCPIP_PING_CMD_START_PING,      // start ping process
    TCPIP_PING_CMD_DO_PING,         // send pings
    TCPIP_PING6_CMD_DNS_GET,        // send pings
    TCPIP_PING6_CMD_DNS_WAIT,       // wait for DNS    
    TCPIP_SEND_ECHO_REQUEST_IPV6,   // send IPv6 ping request

    TCPIP_CMD_STAT_PING_STOP = TCPIP_SEND_ECHO_REQUEST_IPV6,       // stop ping commands

    // DNS related status
    TCPIP_CMD_STAT_DNS_START,                               // starting DNS commands

    TCPIP_DNS_LOOKUP_CMD_GET = TCPIP_CMD_STAT_DNS_START,    // get DNS
    TCPIP_DNS_LOOKUP_CMD_WAIT,                              // wait for DNS

    TCPIP_CMD_STAT_DNS_STOP = TCPIP_DNS_LOOKUP_CMD_WAIT,    // stop DNS commands

    // PHY commands
    TCPIP_PHY_READ,                 // read a PHY register command
    TCPIP_PHY_WRITE,                // write a PHY register command
    TCPIP_PHY_DUMP,                 // dump a range of PHY registers command
}TCPIP_COMMANDS_STAT;

static SYS_CMD_DEVICE_NODE* pTcpipCmdDevice = 0;
static tcpipSignalHandle     tcpipCmdSignalHandle = 0;      // tick handle


static TCPIP_COMMANDS_STAT  tcpipCmdStat = TCPIP_CMD_STAT_IDLE;

#endif  // defined(_TCPIP_STACK_COMMAND_TASK)

static int commandInitCount = 0;        // initialization count

#if defined(TCPIP_STACK_USE_DNS)
static char                 dnslookupTargetHost[31];     // current target host name
static TCPIP_DNS_RESOLVE_TYPE     dnsType=TCPIP_DNS_TYPE_A;
static const void*          dnsLookupCmdIoParam = 0;
static uint32_t             dnsLookUpStartTick;

static int                  _Command_DNSLookUP(SYS_CMD_DEVICE_NODE* pCmdIO, char** argv);

static void                 TCPIPCmdDnsTask(void);
#endif

#if defined(_TCPIP_COMMAND_PING4)

static int                  _CommandPing(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static void                 CommandPingHandler(const  TCPIP_ICMP_ECHO_REQUEST* pEchoReq, TCPIP_ICMP_REQUEST_HANDLE iHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result);

static void                 TCPIPCmdPingTask(void);

static void                 _PingStop(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam);

static IPV4_ADDR            icmpTargetAddr;         // current target address
static uint8_t              icmpPingBuff[TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE];
static int                  icmpPingSize = TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE;
static TCPIP_ICMP_REQUEST_HANDLE icmpReqHandle;     // current transaction handle
#endif  // defined(_TCPIP_COMMAND_PING4)

#if defined(_TCPIP_COMMAND_PING6)
static int                  _Command_IPv6_Ping(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void                 CommandPing6Handler(TCPIP_NET_HANDLE hNetIf,uint8_t type, const IPV6_ADDR * localIP,
                                                                    const IPV6_ADDR * remoteIP, void * data);
static char                 icmpv6TargetAddrStr[42];
static uint32_t             pingPktSize=0;
static IPV6_ADDR            icmpv6TargetAddr;
static ICMPV6_HANDLE        hIcmpv6 = 0;
#endif  // defined(_TCPIP_COMMAND_PING6)


#if defined(_TCPIP_COMMAND_PING4) || defined(_TCPIP_COMMAND_PING6)

#define TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY 5  // minimum delay between successive echo requests

static char                 icmpTargetHost[31];     // current target host name
static char                 icmpTargetAddrStr[16 + 1]; // current target address string
static uint16_t             icmpSequenceNo;         // current sequence number
static uint16_t             icmpIdentifier;         // current ID number

static const void*          icmpCmdIoParam = 0;
static int                  icmpReqNo;              // number of requests to send
static int                  icmpReqCount;           // current request counter
static int                  icmpAckRecv;            // number of acks
static int                  icmpReqDelay;

uint32_t                    icmpStartTick;
static TCPIP_NET_HANDLE     icmpNetH = 0;
#endif  // defined(_TCPIP_COMMAND_PING4) || defined(_TCPIP_COMMAND_PING6)

#if defined(_TCPIP_COMMANDS_MIIM)
static void     TCPIPCmdMiimTask(void);
static int      _CommandMiim(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void     _CommandMiimOp(unsigned int rIx, SYS_CMD_DEVICE_NODE* pCmdIO, TCPIP_COMMANDS_STAT miimCmd);
static void     _CommandMiimSetup(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam);
static DRV_HANDLE _MiimOpen(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam);
static void     _MiimClose(bool idleState);

static const DRV_MIIM_OBJECT_BASE*  miimObj = 0;    // MIIM object associated with the PIC32INT MAC driver
static SYS_MODULE_INDEX             miimObjIx = 0;  // MIIM object index

static DRV_HANDLE           miimHandle = 0; // handle to the MIIM driver
static DRV_MIIM_OPERATION_HANDLE miimOpHandle = 0;  // current operation
static unsigned int         miimRegStart = 0; // start for a dump
static unsigned int         miimRegEnd = 0;   // end for a dump
static unsigned int         miimRegIx = 0;    // current Reg index to read
static unsigned int         miimAdd = 0;    // PHY address
static uint16_t             miimWrData = 0; // write data

static const void*          miimCmdIoParam = 0;

#define         TCPIP_MIIM_COMMAND_TASK_RATE  100   // milliseconds
#endif  // defined(_TCPIP_COMMANDS_MIIM)
#if defined(TCPIP_STACK_USE_FTP_CLIENT) && defined(TCPIP_FTPC_COMMANDS)
static int _Command_FTPC_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif

#if defined(TCPIP_STACK_USE_IPV4)  && defined(TCPIP_IPV4_COMMANDS) && (TCPIP_IPV4_COMMANDS != 0)
static void _CommandIpv4(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif 

// internal test command. Not MHC configurable
#define TCPIP_PKT_ALLOC_COMMANDS    0

#if (TCPIP_PKT_ALLOC_COMMANDS != 0)
static void _CommandPacket(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // (TCPIP_PKT_ALLOC_COMMANDS != 0)

#if defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)
static void _CommandBridge(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif // defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)

// TCPIP stack command table
static const SYS_CMD_DESCRIPTOR    tcpipCmdTbl[]=
{
    {"netinfo",     (SYS_CMD_FNC)_Command_NetInfo,              ": Get network information"},
    {"defnet",      (SYS_CMD_FNC)_Command_DefaultInterfaceSet,  ": Set/Get default interface"},
#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
    {"dhcp",        (SYS_CMD_FNC)_CommandDhcpOptions,           ": DHCP client commands"},
#endif
    {"dhcps",       (SYS_CMD_FNC)_Command_DHCPSOnOff,           ": Turn DHCP server on/off"},
    {"zcll",        (SYS_CMD_FNC)_Command_ZcllOnOff,            ": Turn ZCLL on/off"},
    {"setdns",      (SYS_CMD_FNC)_Command_DNSAddressSet,        ": Set DNS address"},
#endif  // defined(TCPIP_STACK_USE_IPV4)
    {"setip",       (SYS_CMD_FNC)_Command_IPAddressSet,         ": Set IP address and mask"},
    {"setgw",       (SYS_CMD_FNC)_Command_GatewayAddressSet,    ": Set Gateway address"},
    {"setbios",     (SYS_CMD_FNC)_Command_BIOSNameSet,          ": Set host's NetBIOS name"},
    {"setmac",      (SYS_CMD_FNC)_Command_MACAddressSet,        ": Set MAC address"},
#if (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
    {"if",          (SYS_CMD_FNC)_Command_NetworkOnOff,         ": Bring an interface up/down"},
#endif  // (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
#if (TCPIP_STACK_DOWN_OPERATION != 0)
    {"stack",       (SYS_CMD_FNC)_Command_StackOnOff,           ": Stack turn on/off"},
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
    {"heapinfo",    (SYS_CMD_FNC)_Command_HeapInfo,             ": Check heap status"},
#if defined(TCPIP_STACK_USE_DHCP_SERVER)
    {"dhcpsinfo",   (SYS_CMD_FNC)_Command_DHCPLeaseInfo,        ": Display DHCP Server Lease Details" },
#endif  //  defined(TCPIP_STACK_USE_DHCP_SERVER)
#if defined(_TCPIP_COMMAND_PING4)
    {"ping",        (SYS_CMD_FNC)_CommandPing,                  ": Ping an IP address"},
#endif  // defined(_TCPIP_COMMAND_PING4)
#if defined(_TCPIP_COMMAND_PING6)
    {"ping6",       (SYS_CMD_FNC)_Command_IPv6_Ping,            ": Ping an IPV6 address"},
#endif  // defined(_TCPIP_COMMAND_PING6)
#if defined(TCPIP_STACK_USE_IPV4)
#if (TCPIP_ARP_COMMANDS != 0)
    {"arp",         _CommandArp,                                ": ARP commands"},
#endif  // (TCPIP_ARP_COMMANDS != 0)
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DNS_SERVER)
    {"dnss",        (SYS_CMD_FNC)_Command_DnsServService,       ": DNS server commands"},
#endif
#if defined(TCPIP_STACK_USE_DNS)
    {"dnsc",        (SYS_CMD_FNC)_Command_DNS_Service,          ": DNS client commands"},
#endif
    {"macinfo",     (SYS_CMD_FNC)_Command_MacInfo,              ": Check MAC statistics"},
#if defined(TCPIP_STACK_USE_TFTP_CLIENT)
    {"tftpc",       (SYS_CMD_FNC)_Command_TFTPC_Service,        ": TFTP client Service"},
#endif
#if defined(TCPIP_STACK_USE_TFTP_SERVER)
    {"tftps",       (SYS_CMD_FNC)_Command_TFTPServerOnOff,        ": TFTP Server Service"},
#endif
#if defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
    {"dhcpv6",      (SYS_CMD_FNC)_CommandDhcpv6Options,         ": DHCPV6 client commands"},
#endif
#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)
    {"http",        (SYS_CMD_FNC)_Command_HttpInfo,             ": HTTP information"},
#if (TCPIP_HTTP_NET_SSI_PROCESS != 0)
    {"ssi",         (SYS_CMD_FNC)_Command_SsiInfo,              ": SSI information"},
#endif
#endif
#if defined(TCPIP_STACK_USE_SMTPC) && defined(TCPIP_SMTPC_USE_MAIL_COMMAND)
	{"mail", 	    (SYS_CMD_FNC)_CommandMail,			       ": Send Mail Message"},
#endif  // defined(TCPIP_STACK_USE_SMTPC) && defined(TCPIP_SMTPC_USE_MAIL_COMMAND)
#if defined(_TCPIP_COMMANDS_MIIM)
	{"miim", 	    (SYS_CMD_FNC)_CommandMiim,			       ": MIIM commands"},
#endif  // defined(_TCPIP_COMMANDS_MIIM)
#if (TCPIP_UDP_COMMANDS)
    {"udpinfo",     (SYS_CMD_FNC)_Command_UdpInfo,              ": Check UDP statistics"},
#endif  // (TCPIP_UDP_COMMANDS)
#if (TCPIP_TCP_COMMANDS)
    {"tcpinfo",     _Command_TcpInfo,                           ": Check TCP statistics"},
    {"tcptrace",    _Command_TcpTrace,                          ": Enable TCP trace"},
#endif  // (TCPIP_TCP_COMMANDS)
#if (TCPIP_PACKET_LOG_ENABLE)
    {"plog",        (SYS_CMD_FNC)_Command_PktLog,               ": PKT flight log"},
#endif  // (TCPIP_PACKET_LOG_ENABLE)
#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
    {"pktinfo",   (SYS_CMD_FNC)_Command_PktInfo,                ": Check PKT allocation"},
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
#if defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
    {"heaplist",    (SYS_CMD_FNC)_Command_HeapList,             ": List heap"},
#endif  // defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)
    {"announce",    (SYS_CMD_FNC)_Command_Announce,             ": Announce"},
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)
#if defined(TCPIP_STACK_USE_FTP_CLIENT)  && defined(TCPIP_FTPC_COMMANDS)
    {"ftpc", (SYS_CMD_FNC)_Command_FTPC_Service,   ": Connect FTP Client to Server"},
#endif  // (TCPIP_STACK_USE_FTP_CLIENT)    
#if defined(TCPIP_STACK_USE_IPV4)  && defined(TCPIP_IPV4_COMMANDS) && (TCPIP_IPV4_COMMANDS != 0)
    {"ip4", _CommandIpv4,   ": IPv4"},
#endif
#if (TCPIP_PKT_ALLOC_COMMANDS != 0)
    {"pkt", _CommandPacket,   ": IPv4"},
#endif  // (TCPIP_PKT_ALLOC_COMMANDS != 0)
#if defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)
    {"bridge", _CommandBridge,   ": Bridge"},
#endif // defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)
};

bool TCPIP_Commands_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_COMMAND_MODULE_CONFIG* const pCmdInit)
{
    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init
    if(commandInitCount == 0)
    {   // 1st time we run
        initialNetIfs = stackCtrl->nIfs;

        // create command group
        if (!SYS_CMD_ADDGRP(tcpipCmdTbl, sizeof(tcpipCmdTbl)/sizeof(*tcpipCmdTbl), "tcpip", ": stack commands"))
        {
            SYS_ERROR(SYS_ERROR_ERROR, "Failed to create TCPIP Commands\r\n");
            return false;
        }

#if defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((TCPIP_STACK_DOWN_OPERATION != 0) || (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))
        // get storage for interfaces configuration
        // cannot be taken from the TCPIP-HEAP because we need it persistent after
        // TCPIP_STACK_Deinit() is called!
        if(pCmdStgDcpt == 0 && pCmdNetConf == 0)
        {
            pCmdStgDcpt = (TCPIP_COMMAND_STG_DCPT*)TCPIP_STACK_CALLOC_FUNC(initialNetIfs, sizeof(*pCmdStgDcpt));
            pCmdNetConf = (TCPIP_NETWORK_CONFIG*)TCPIP_STACK_CALLOC_FUNC(initialNetIfs, sizeof(*pCmdNetConf));
            if(pCmdStgDcpt == 0 || pCmdNetConf == 0)
            {   // failure is not considered to be catastrophic
                SYS_ERROR(SYS_ERROR_WARNING, "Failed to create TCPIP Commands Storage/Config\r\n");
            }
        }
#endif  // defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((TCPIP_STACK_DOWN_OPERATION != 0) || (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))

#if defined(_TCPIP_COMMAND_PING4)
        icmpAckRecv = 0;
#endif  // defined(_TCPIP_COMMAND_PING4)
#if defined(_TCPIP_COMMAND_PING6)
        hIcmpv6 = 0;
        icmpAckRecv = 0;
#endif  // defined(_TCPIP_COMMAND_PING6)

#if defined(_TCPIP_STACK_COMMAND_TASK)
        tcpipCmdSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_COMMAND_Task, 0);
        if(tcpipCmdSignalHandle == 0)
        {   // timer is not active now
            SYS_ERROR(SYS_ERROR_ERROR, "TCPIP commands task registration failed\r\n");
            return false;
        }
        // else the timer will start when we send a query
        tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
#endif  // defined(_TCPIP_STACK_COMMAND_TASK)

#if defined(_TCPIP_COMMANDS_MIIM)
        // get the MIIM driver object
        miimObj = &DRV_MIIM_OBJECT_BASE_Default;
        miimObjIx = DRV_MIIM_DRIVER_INDEX;
        miimHandle = 0;
        miimOpHandle = 0;
#endif  // defined(_TCPIP_COMMANDS_MIIM)
    }

    commandInitCount++;

    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_Commands_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(commandInitCount > 0 && stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
    {   // whole stack is going down
        if(--commandInitCount == 0)
        {   // close all
#if defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
            if(tcpipCmdPreserveSavedInfo == false)
            {
                TCPIP_STACK_FREE_FUNC(pCmdStgDcpt);
                TCPIP_STACK_FREE_FUNC(pCmdNetConf);
                pCmdStgDcpt = 0;
                pCmdNetConf = 0;
            }
#endif  // defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)

#if defined(_TCPIP_STACK_COMMAND_TASK)
            if(tcpipCmdSignalHandle != 0)
            {
                _TCPIPStackSignalHandlerDeregister(tcpipCmdSignalHandle);
                tcpipCmdSignalHandle = 0;
            }
#endif  // defined(_TCPIP_STACK_COMMAND_TASK)

#if defined(_TCPIP_COMMAND_PING6)
            if(hIcmpv6)
            {
                TCPIP_ICMPV6_CallbackDeregister(hIcmpv6);
            }
#endif  // defined(_TCPIP_COMMAND_PING6)
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static int _Command_NetInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int i;
    TCPIP_NET_HANDLE netH;
    const TCPIP_MAC_ADDR* pMac;
    const char  *hostName;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
#if defined(TCPIP_STACK_USE_IPV6)
    IPV6_ADDR_STRUCT currIpv6Add;
    IPV6_ADDR_HANDLE prevHandle, nextHandle;
#endif  // defined(TCPIP_STACK_USE_IPV6)
#if defined(TCPIP_STACK_USE_IPV4)
    const char  *msgAdd;
    IPV4_ADDR ipAddr;
#endif // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IPV6)
    char   addrBuff[44];
    IPV6_ADDR addr6;
#else
    char   addrBuff[20];
#endif

    if (argc > 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: netinfo\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: netinfo\r\n");
        return false;
    }

    for (i=0; i<initialNetIfs; i++)
    {
        netH = TCPIP_STACK_IndexToNet(i);
        TCPIP_STACK_NetAliasNameGet(netH, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "---------- Interface <%s/%s> ---------- \r\n", addrBuff, TCPIP_STACK_NetNameGet(netH));
        if(!TCPIP_STACK_NetIsUp(netH))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Interface is down\r\n");
            continue;
        }
        hostName = TCPIP_STACK_NetBIOSName(netH); 
#if defined(TCPIP_STACK_USE_NBNS)
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Host Name: %s - NBNS enabled\r\n", hostName);
#else
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Host Name: %s - NBNS disabled \r\n", hostName);
#endif  // defined(TCPIP_STACK_USE_NBNS)
#if defined(TCPIP_STACK_USE_IPV4)
        ipAddr.Val = TCPIP_STACK_NetAddress(netH);
        TCPIP_Helper_IPAddressToString(&ipAddr, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 Address: %s\r\n", addrBuff);

        ipAddr.Val = TCPIP_STACK_NetMask(netH);
        TCPIP_Helper_IPAddressToString(&ipAddr, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Mask: %s\r\n", addrBuff);

        ipAddr.Val = TCPIP_STACK_NetAddressGateway(netH);
        TCPIP_Helper_IPAddressToString(&ipAddr, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Gateway: %s\r\n", addrBuff);

        ipAddr.Val = TCPIP_STACK_NetAddressDnsPrimary(netH);
        TCPIP_Helper_IPAddressToString(&ipAddr, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "DNS1: %s\r\n", addrBuff);

        ipAddr.Val = TCPIP_STACK_NetAddressDnsSecond(netH);
        TCPIP_Helper_IPAddressToString(&ipAddr, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "DNS2: %s\r\n", addrBuff);
#endif  // defined(TCPIP_STACK_USE_IPV4)

        pMac = (const TCPIP_MAC_ADDR*)TCPIP_STACK_NetAddressMac(netH);
        TCPIP_Helper_MACAddressToString(pMac, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "MAC Address: %s\r\n", addrBuff);

        // display IPv6 addresses
#if defined(TCPIP_STACK_USE_IPV6)
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "IPv6 Unicast addresses:\r\n");

        prevHandle = 0;
        do
        {
            nextHandle = TCPIP_STACK_NetIPv6AddressGet(netH, IPV6_ADDR_TYPE_UNICAST, &currIpv6Add, prevHandle);
            if(nextHandle)
            {   // have a valid address; display it
                addr6 = currIpv6Add.address;
                TCPIP_Helper_IPv6AddressToString(&addr6, addrBuff, sizeof(addrBuff));
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "    %s\r\n", addrBuff);
                prevHandle = nextHandle;
            }
        }while(nextHandle != 0);

        if(prevHandle == 0)
        {   // no valid address
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "    Unknown\r\n");
        }
        
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "IPv6 Multicast addresses:\r\n");
        prevHandle = 0;
        do
        {
            nextHandle = TCPIP_STACK_NetIPv6AddressGet(netH, IPV6_ADDR_TYPE_MULTICAST, &currIpv6Add, prevHandle);
            if(nextHandle)
            {   // have a valid address; display it
                addr6 = currIpv6Add.address;
                TCPIP_Helper_IPv6AddressToString(&addr6, addrBuff, sizeof(addrBuff));
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "    %s\r\n", addrBuff);
                prevHandle = nextHandle;
            }
        }while(nextHandle != 0);

        if(prevHandle == 0)
        {   // no valid address
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "    Unknown\r\n");
        }

#endif  // defined(TCPIP_STACK_USE_IPV6)

#if defined(TCPIP_STACK_USE_IPV4)
        
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
        bool      dhcpActive;
        dhcpActive = false;
        if(TCPIP_DHCP_IsActive(netH))
        {
            msgAdd = "dhcp";
            dhcpActive = true;
        }
        else 
#endif
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
        if(TCPIP_ZCLL_IsEnabled(netH))
        {
            msgAdd = "zcll";
        }
        else 
#endif
#if defined(TCPIP_STACK_USE_DHCP_SERVER)
        if(TCPIP_DHCPS_IsEnabled(netH))
        {
            msgAdd = "dhcps";
        }
        else
#endif  // defined(TCPIP_STACK_USE_DHCP_SERVER)        
        {
            msgAdd = "default IP address";
        }

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s is ON\r\n", msgAdd);
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
        if(!dhcpActive)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "dhcp is %s\r\n", TCPIP_DHCP_IsEnabled(netH) ? "enabled" : "disabled");
        }
#endif
#endif  // defined(TCPIP_STACK_USE_IPV4)
        
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Link is %s\r\n", TCPIP_STACK_NetIsLinked(netH) ? "UP" : "DOWN");

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Status: %s\r\n", TCPIP_STACK_NetIsReady(netH) ? "Ready" : "Not Ready");

    }
    return true;
}

#if defined(TCPIP_STACK_USE_DHCP_SERVER)
static int _Command_DHCPLeaseInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    TCPIP_DHCPS_LEASE_HANDLE  prevLease, nextLease;
    TCPIP_DHCPS_LEASE_ENTRY leaseEntry;
    char   addrBuff[20];
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dhcpsinfo <interface> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: dhcpsinfo PIC32INT \r\n");
        return false;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return false;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam,"MAC Address		IPAddress		RemainingLeaseTime \r\n",0);

    prevLease = 0;
    do
    {
        memset((void*)&leaseEntry,0,sizeof(TCPIP_DHCPS_LEASE_ENTRY));
        nextLease = TCPIP_DHCPS_LeaseEntryGet(netH, &leaseEntry, prevLease);
        if(!nextLease)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, " \r\n No more entry present \r\n", 0);
        }
        if(nextLease)
        {   // valid info
            // display info
            TCPIP_Helper_MACAddressToString(&leaseEntry.hwAdd, addrBuff, sizeof(addrBuff));
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s", addrBuff);
            TCPIP_Helper_IPAddressToString(&leaseEntry.ipAddress, addrBuff, sizeof(addrBuff));
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "	%s ", addrBuff);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "	%d Secs\r\n", leaseEntry.leaseTime/SYS_TMR_TickCounterFrequencyGet());

            prevLease = nextLease;
        }
    }while(nextLease != 0);


    return true;

}
#endif  //  defined(TCPIP_STACK_USE_DHCP_SERVER)

static int _Command_DefaultInterfaceSet (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    bool res;
    int nameSize;
    TCPIP_NET_HANDLE netH = 0;
    int defaultOp = 0;      // 0 - nop, error; 1 set; 2 get
    char nameBuff[20];
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    while(argc >= 2)
    {
        if(strcmp(argv[1], "set") == 0)
        {
            if(argc < 3)
            {
                break;
            }

            netH = TCPIP_STACK_NetHandleGet(argv[2]);
            if (netH == 0)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
                return false;
            }
            defaultOp = 1;
        }
        else if(strcmp(argv[1], "get") == 0)
        {
            defaultOp = 2;
        }

        break;
    }

    switch(defaultOp)
    {
        case 1:
            res = TCPIP_STACK_NetDefaultSet(netH);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Default interface set %s\r\n", res ? "successful" : "failed!");
            break;

        case 2:
            netH = TCPIP_STACK_NetDefaultGet();
            nameSize = TCPIP_STACK_NetAliasNameGet(netH, nameBuff, sizeof(nameBuff));
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Default interface is: %s\r\n", nameSize ? nameBuff : "None");
            break;

        default:
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: defnet set/get <interface>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: defnet set eth0\r\n");
            break;
    }


    return false;
}

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)
static int _CommandDhcpOptions(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    IPV4_ADDR       reqIpAddr;
    bool            dhcpRes;
    int             opCode = 0;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 3)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Usage: %s <interface> <on/off/renew/request/info> \r\n", argv[0]);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Ex: %s PIC32INT on \r\n", argv[0]);
        return false;
    }

    reqIpAddr.Val = 0;
    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
        return false;
    }

    if (strcmp(argv[2], "on") == 0)
    {   // turning on a service
        opCode = 1;
    }
    else if (strcmp(argv[2], "off") == 0)
    {   // turning off a service
        opCode = 2;
    }
    else if (strcmp(argv[2], "renew") == 0)
    {   // DHCP renew
        opCode = 3;
    }
    else if (strcmp(argv[2], "request") == 0)
    {   // DHCP request
        opCode = 4;
        if(argc < 4)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Request needs an IP address\r\n");
            return false;
        }

        if (!TCPIP_Helper_StringToIPAddress(argv[3], &reqIpAddr))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IP address string \r\n");
            return false;
        }
    }
    else if (strcmp(argv[2], "info") == 0)
    {   // DHCP info
        TCPIP_DHCP_INFO dhcpInfo;
        char addBuff[20];

        if(TCPIP_DHCP_InfoGet(netH, &dhcpInfo))
        {
            const char* bootName;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP status: %d ( %d == Bound), time: %d\r\n", dhcpInfo.status, TCPIP_DHCP_BOUND, dhcpInfo.dhcpTime);
            if(dhcpInfo.status >= TCPIP_DHCP_BOUND)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP lease start: %d, duration: %ds\r\n", dhcpInfo.leaseStartTime, dhcpInfo.leaseDuration);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP renew time: %d, rebind time: %d\r\n", dhcpInfo.renewTime, dhcpInfo.rebindTime);

                TCPIP_Helper_IPAddressToString(&dhcpInfo.dhcpAddress, addBuff, sizeof(addBuff));
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP address: %s\r\n", addBuff);
                TCPIP_Helper_IPAddressToString(&dhcpInfo.serverAddress, addBuff, sizeof(addBuff));
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP server: %s\r\n", addBuff);
                if(dhcpInfo.bootFileName == 0 || strlen(dhcpInfo.bootFileName) == 0)
                {
                    bootName = "not given";
                }
                else
                {
                    bootName = dhcpInfo.bootFileName;
                }
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP boot name: %s\r\n", bootName);

                if(dhcpInfo.timeServersNo)
                {
                    TCPIP_Helper_IPAddressToString(dhcpInfo.timeServers, addBuff, sizeof(addBuff));
                }
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP Time servers: %d, %s\r\n", dhcpInfo.timeServersNo, dhcpInfo.timeServersNo ? addBuff: "None");

                if(dhcpInfo.ntpServersNo)
                {
                    TCPIP_Helper_IPAddressToString(dhcpInfo.ntpServers, addBuff, sizeof(addBuff));
                }
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP NTP servers: %d, %s\r\n", dhcpInfo.ntpServersNo, dhcpInfo.ntpServersNo ? addBuff :  "None");
            }
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCP: failed to get info\r\n");
        }
        return false;
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown option\r\n");
        return false;
    }


    switch(opCode)
    {
        case 1:
            dhcpRes = TCPIP_DHCP_Enable(netH);
            break;

        case 2:
            dhcpRes = TCPIP_DHCP_Disable(netH);
            break;

        case 3:
            dhcpRes = TCPIP_DHCP_Renew(netH);
            break;

        case 4:
            dhcpRes = TCPIP_DHCP_Request(netH, reqIpAddr);
            break;

        default:
            dhcpRes = false;
            break;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s %s %s\r\n", argv[0], argv[2], dhcpRes ? "success" : "fail");

    return true;
}
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)

#if defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
static int _CommandDhcpv6Options(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 3)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Usage: %s <interface> <[on/off/renew/request/]info/ia n> \r\n", argv[0]);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Ex: %s eth0 on \r\n", argv[0]);
        return false;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
        return false;
    }

    if (strcmp(argv[2], "info") == 0)
    {   // DHCPV6 info
        TCPIP_DHCPV6_CLIENT_INFO dhcpv6Info;
        char dhcpv6StatusBuff[40];
        IPV6_ADDR dhcpv6DnsBuff[1];

        dhcpv6Info.statusBuff = dhcpv6StatusBuff;
        dhcpv6Info.statusBuffSize = sizeof(dhcpv6StatusBuff);
        dhcpv6Info.dnsBuff = dhcpv6DnsBuff;
        dhcpv6Info.dnsBuffSize = sizeof(dhcpv6DnsBuff);
        dhcpv6Info.domainBuff = 0;
        dhcpv6Info.domainBuffSize = 0;


        if(TCPIP_DHCPV6_ClientInfoGet(netH, &dhcpv6Info))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 status: %d ( %d == Run), time: %d\r\n", dhcpv6Info.clientState, TCPIP_DHCPV6_CLIENT_STATE_RUN, dhcpv6Info.dhcpTime);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 IANAs: %d, IATAs: %d, Free IAa: %d\r\n", dhcpv6Info.nIanas, dhcpv6Info.nIatas, dhcpv6Info.nFreeIas);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCPV6: failed to get client info\r\n");
        }
    }
    else if (strcmp(argv[2], "ia") == 0)
    {
        if (argc < 4)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCPV6: provide an IA number\r\n");
        }
        else
        {
            int iaIx = atoi(argv[3]);
            TCPIP_DHCPV6_IA_INFO iaInfo;
            char ipv6AddBuff[42];

            if(TCPIP_DHCPV6_IaInfoGet(netH, iaIx, &iaInfo))
            {
                const char* typeMsg = (iaInfo.iaType == TCPIP_DHCPV6_IA_TYPE_IANA) ? "iana" : (iaInfo.iaType == TCPIP_DHCPV6_IA_TYPE_IATA) ? "iata" : "unknown";
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 IA type: %s, index: %d, id: %d\r\n", typeMsg, iaInfo.iaIndex, iaInfo.iaId);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 IA status: %d ( %d == Bound), sub state: %d\r\n", iaInfo.iaState, TCPIP_DHCPV6_IA_STATE_BOUND, iaInfo.iaSubState);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 IA tAcquire: %d, t1: %d, t2: %d\r\n", iaInfo.tAcquire, iaInfo.t1, iaInfo.t2);
                TCPIP_Helper_IPv6AddressToString(&iaInfo.ipv6Addr, ipv6AddBuff, sizeof(ipv6AddBuff));
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 IA address: %s, pref LTime: %d, valid LTime: %d\r\n", ipv6AddBuff, iaInfo.prefLTime, iaInfo.validLTime);
            }
            else
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCPV6: failed to get IA info\r\n");
            }
        }
    }

    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCPV6: Unknown option\r\n");
    }

    return true;
}
#endif  // defined(TCPIP_STACK_USE_DHCPV6_CLIENT)

#if defined(TCPIP_STACK_USE_IPV4)
static int _Command_DHCPSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    return _Command_AddressService(pCmdIO, argc, argv, TCPIP_STACK_ADDRESS_SERVICE_DHCPS);
}

static int _Command_ZcllOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    return _Command_AddressService(pCmdIO, argc, argv, TCPIP_STACK_ADDRESS_SERVICE_ZCLL);
}

static int _Command_AddressService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_STACK_ADDRESS_SERVICE_TYPE svcType)
{ 
    typedef bool(*addSvcFnc)(TCPIP_NET_HANDLE hNet);

    TCPIP_NET_HANDLE netH;
    addSvcFnc        addFnc;
    bool             addRes, svcEnable;
    const char       *msgOK, *msgFail;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 3)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Usage: %s <interface> <on/off> \r\n", argv[0]);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Ex: %s PIC32INT on \r\n", argv[0]);
        return false;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
        return false;
    }

    if (memcmp(argv[2], "on", 2) == 0)
    {   // turning on a service
        svcEnable = true;
    }
    else if (memcmp(argv[2], "off", 2) == 0)
    {   // turning off a service
        svcEnable = false;
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown option\r\n");
        return false;
    }

    switch(svcType)
    {
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
        case TCPIP_STACK_ADDRESS_SERVICE_DHCPC:
            addFnc = svcEnable?TCPIP_DHCP_Enable:TCPIP_DHCP_Disable;
            break;
#endif 
            
#if defined(TCPIP_STACK_USE_DHCP_SERVER)
        case TCPIP_STACK_ADDRESS_SERVICE_DHCPS:
            addFnc = svcEnable?TCPIP_DHCPS_Enable:TCPIP_DHCPS_Disable;
            break;
#endif  // defined(TCPIP_STACK_USE_DHCP_SERVER)

#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
        case TCPIP_STACK_ADDRESS_SERVICE_ZCLL:
            addFnc = svcEnable?TCPIP_ZCLL_Enable:TCPIP_ZCLL_Disable;
            break;
#endif
        default:
            addFnc = 0;     // unknown service;
            break;
    }

    if(addFnc)
    {
        msgOK   = svcEnable?"enabled":"disabled";
        msgFail = svcEnable?"enable":"disable";

        addRes = (*addFnc)(netH);
        
        if(addRes)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s %s\r\n", argv[0], msgOK);
        }
        else
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Failed to %s %s\r\n", msgFail, argv[0]);
        }
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Unknown service %s\r\n", argv[0]);
    }

    return true;
}
#endif  // defined(TCPIP_STACK_USE_IPV4)


static int _Command_IPAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    TCPIP_NET_IF*   pNetIf;
    IP_ADDRESS_TYPE addType;

#if defined(TCPIP_STACK_USE_IPV4)
    IPV4_ADDR ipAddr, ipMask;
    IPV4_ADDR*  pMask;
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IPV6)
    IPV6_ADDR  ipv6Addr;
    int     prefixLen;
#endif  // defined(TCPIP_STACK_USE_IPV6)
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool     success = false;

    if (argc < 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setip <interface> <ipv4/6 address> <ipv4mask/ipv6 prefix len>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setip PIC32INT 192.168.0.8 255.255.255.0 \r\n");
        return false;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return false;
    }

    pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No such interface is up\r\n");
        return false;
    }


    addType = IP_ADDRESS_TYPE_ANY;

#if defined(TCPIP_STACK_USE_IPV4)
    if (TCPIP_Helper_StringToIPAddress(argv[2], &ipAddr))
    {
        addType = IP_ADDRESS_TYPE_IPV4;
    }
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IPV6)
    if(TCPIP_Helper_StringToIPv6Address (argv[2], &ipv6Addr))
    {
        addType = IP_ADDRESS_TYPE_IPV6;
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)

    if(addType == IP_ADDRESS_TYPE_ANY)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IP address string \r\n");
        return false;
    }
    

#if defined(TCPIP_STACK_USE_IPV4)
    if(addType == IP_ADDRESS_TYPE_IPV4)
    {
        if(_TCPIPStackAddressServiceIsRunning(pNetIf) != TCPIP_STACK_ADDRESS_SERVICE_NONE)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "An address service is already running. Stop DHCP, ZCLL, etc. first\r\n");
            return false;
        }

        if(argc > 3)
        {   // we have net mask too
            if (!TCPIP_Helper_StringToIPAddress(argv[3], &ipMask))
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IP mask string \r\n");
                return false;
            }
            pMask = &ipMask;
        }
        else
        {
            pMask = 0;
        }

        if(TCPIP_STACK_NetAddressSet(netH, &ipAddr, pMask, true))
        {
            success = true;
        }

    }
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)

    if(addType == IP_ADDRESS_TYPE_IPV6)
    {
        if(argc > 3)
        {   // we have prefix length
            prefixLen = atoi(argv[3]);
        }
        else
        {
            prefixLen = 0;
        }

        if(TCPIP_IPV6_UnicastAddressAdd (netH, &ipv6Addr, prefixLen, false) != 0)
        {
            success = true;
        }
    }

#endif  // defined(TCPIP_STACK_USE_IPV6)


    (*pCmdIO->pCmdApi->msg)(cmdIoParam, success ? "Set ip address OK\r\n" : "Set ip address failed\r\n");
    return false;
}

static int _Command_GatewayAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    IP_ADDRESS_TYPE addType;
#if defined(TCPIP_STACK_USE_IPV4)
    IPV4_ADDR ipGateway;
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IPV6)
    IPV6_ADDR  ipv6Gateway;
    unsigned long validTime;
#endif  // defined(TCPIP_STACK_USE_IPV6)
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool     success = false;

    if (argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setgw <interface> <ipv4/6 address> <validTime> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setgw PIC32INT 192.168.0.1 \r\n");
        return false;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return false;
    }

    addType = IP_ADDRESS_TYPE_ANY;

#if defined(TCPIP_STACK_USE_IPV4)
    if (TCPIP_Helper_StringToIPAddress(argv[2], &ipGateway))
    {
        addType = IP_ADDRESS_TYPE_IPV4;
    }
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IPV6)
    if(TCPIP_Helper_StringToIPv6Address (argv[2], &ipv6Gateway))
    {
        addType = IP_ADDRESS_TYPE_IPV6;
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)

    if(addType == IP_ADDRESS_TYPE_ANY)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IP address string \r\n");
        return false;
    }


#if defined(TCPIP_STACK_USE_IPV4)
    if(addType == IP_ADDRESS_TYPE_IPV4)
    {
        success = TCPIP_STACK_NetAddressGatewaySet(netH, &ipGateway);
    }
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
    if(addType == IP_ADDRESS_TYPE_IPV6)
    {
        if(argc > 3)
        {   // we have validity time
            validTime = (unsigned long)atoi(argv[3]);
        }
        else
        {
            validTime = 0;
        }
        success = TCPIP_IPV6_RouterAddressAdd(netH, &ipv6Gateway, validTime, 0);
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)


    (*pCmdIO->pCmdApi->msg)(cmdIoParam, success ? "Set gateway address OK\r\n" : "Set gateway address failed\r\n");
    return false;

}

#if defined(TCPIP_STACK_USE_IPV4)
static int _Command_DNSAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    IPV4_ADDR ipDNS;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 4)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setdns 1/2 <interface> <x.x.x.x> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setdns 1 eth0 255.255.255.0 \r\n");
        return false;
    }

    int dnsIx = atoi(argv[1]);
    if(dnsIx != 1 && dnsIx != 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown DNS index\r\n");
        return false;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[2]);
    if (netH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return false;
    }

    if (!TCPIP_Helper_StringToIPAddress(argv[3], &ipDNS))
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IP address string \r\n");
        return false;
    }

    bool res = dnsIx == 1 ? TCPIP_STACK_NetAddressDnsPrimarySet(netH, &ipDNS) : TCPIP_STACK_NetAddressDnsSecondSet(netH, &ipDNS); 
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Set DNS %d address %s\r\n", dnsIx, res ? "success" : "failed");
    return true;
}
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_TFTP_CLIENT)
static int _Command_TFTPC_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    TCPIP_TFTP_CMD_TYPE cmdType=TFTP_CMD_NONE;
    int  serverIPStrLen =0;
    int  fileNameLen=0;
    IP_MULTI_ADDRESS mAddr;
    IP_ADDRESS_TYPE ipType;
    
    if (argc != 4) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: tftpc <server IP address> <command> <filename>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: Now only supports IPv4 Address\r\n");
        return false;
    }
    serverIPStrLen = strlen(argv[1]);
    if(serverIPStrLen >= sizeof(tftpServerHost))
    {
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "TFTPC: Server name is too long. Retry.\r\n");
        return true;
    }
    strcpy(tftpServerHost, argv[1]);
    
    if(TCPIP_Helper_StringToIPAddress(tftpServerHost, &mAddr.v4Add))
    {
        ipType = IP_ADDRESS_TYPE_IPV4;
    }
    else if (TCPIP_Helper_StringToIPv6Address (tftpServerHost, &mAddr.v6Add))
    {
        ipType = IP_ADDRESS_TYPE_IPV6;
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "TFTPC: Invalid Server IP address.\r\n");
        return true;
    }
    
    if(stricmp("put",argv[2])==0)
    {
        cmdType = TFTP_CMD_PUT_TYPE;
    }
    else if(stricmp("get",argv[2])==0)
    {
        cmdType = TFTP_CMD_GET_TYPE;
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "TFTPC:Command not found.\r\n");
        return true;
    }

    // Process file name
    fileNameLen = strlen(argv[3]);
    if(fileNameLen < sizeof(tftpcFileName))
    {
        strcpy(tftpcFileName, argv[3]);
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "TFTPC:File size should be less than [ %d ] .\r\n",sizeof(tftpcFileName)-1);
        return true;
    }
   
    if(TCPIP_TFTPC_SetCommand(&mAddr,ipType,cmdType,tftpcFileName) != TFTPC_ERROR_NONE)
	{
		(*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "TFTPC:Command processing error.\r\n");
        return true;
	}
    return false;
}
#endif
#if defined(TCPIP_STACK_USE_DNS)
static int _Command_DNSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    bool             addRes, svcEnable;
    const char       *msgOK, *msgFail;
    bool             clearCache = false;
    TCPIP_DNS_ENABLE_FLAGS enableFlags = TCPIP_DNS_ENABLE_DEFAULT;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 3)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Usage: %s <on/off> <interface> <strict/pref>/<clear> \r\n", argv[0]);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Ex: %s on eth0\r\n", argv[0]);
        return false;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[2]);
    if (netH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
        return false;
    }

    if (memcmp(argv[1], "on", 2) == 0)
    {   // turning on a service
        svcEnable = true;
        if(argc > 3)
        {
            if(strcmp(argv[3], "strict") == 0)
            {
                enableFlags = TCPIP_DNS_ENABLE_STRICT;
            }
            else if(strcmp(argv[3], "pref") == 0)
            {
                enableFlags = TCPIP_DNS_ENABLE_PREFERRED;
            }
        }
        
    }
    else if (memcmp(argv[1], "off", 2) == 0)
    {   // turning off a service
        svcEnable = false;
        if(argc > 3)
        {
            if(strcmp(argv[3], "clear") == 0)
            {
                clearCache = true;
            }
        }
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown option\r\n");
        return false;
    }

    if(svcEnable)
    {
        msgOK   = "enabled";
        msgFail = "enable";
        addRes = TCPIP_DNS_Enable(netH, enableFlags);
    }
    else
    {
        msgOK   = "disabled";
        msgFail = "disable";
        addRes = TCPIP_DNS_Disable(netH, clearCache);
    }

    if(addRes)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s %s\r\n", argv[0], msgOK);
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Failed to %s %s\r\n", msgFail, argv[0]);
    }
    return true;
}
static int _Command_DNS_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t             *hostName;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    TCPIP_DNS_RESULT res;
    DNS_SERVICE_COMD_TYPE val=DNS_SERVICE_COMD_NONE;
    DNSS_COMMAND_MAP dnssComnd[]=
            {
                {"del",         DNS_SERVICE_COMD_DEL},
                {"info",        DNS_SERVICE_COMD_INFO},
                {"on",          DNS_SERVICE_COMD_ENABLE_INTF},
                {"off",         DNS_SERVICE_COMD_ENABLE_INTF},
                {"lookup",      DNS_SERVICE_COMD_LOOKUP},
            };
    int i=0;

    if (argc < 2) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnsc <del/info/on/off/lookup> \r\n");
        return false;
    }
    for(i=0;i<(sizeof(dnssComnd)/sizeof(DNSS_COMMAND_MAP));i++)
    {
        if(strcmp(argv[1],dnssComnd[i].command) ==0)
        {
            val = dnssComnd[i].val;
            break;
        }
    }
    switch(val)
    {
        case DNS_SERVICE_COMD_ENABLE_INTF:
            _Command_DNSOnOff(pCmdIO,argc,argv);
            break;
        case DNS_SERVICE_COMD_LOOKUP:
            if (argc != 4) {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnsc lookup <type> <hostName> \r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: <hostName>(URL) - look up for hostname\r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: <type> : a or A for IPv4 address lookup\r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: <type> : aaaa or AAAA for IPv6 address lookup\r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: <type> : any for both IPv4 and IPv6 address lookup\r\n");
                return false;
            }
            _Command_DNSLookUP(pCmdIO,argv);
            break;
        case DNS_SERVICE_COMD_DEL:
            if (argc != 3) {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnsc del <hostName>|all \r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: <hostName>(URL) - Remove the entry if exists \r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: all - Remove all the resolved entry \r\n");
                return false;
            }

            hostName = (uint8_t*)argv[2];
            if (hostName == 0)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown option\r\n");
                return false;
            }
            if(strcmp((char*)hostName,(char*)"all")==0)
            {
                TCPIP_DNS_RemoveAll();
                res = TCPIP_DNS_RES_OK;
            }
            else
            {
                res = TCPIP_DNS_RemoveEntry((const char*)hostName);
            }
            switch(res)
            {
                case TCPIP_DNS_RES_NO_NAME_ENTRY:
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "[%s] not part of the DNS Cache entry \r\n",hostName);
                    return false;
                case TCPIP_DNS_RES_NO_SERVICE:
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Incomplete command \r\n");
                    return false;
                case TCPIP_DNS_RES_OK:
                    return false;
                default:
                    return false;
            }
            break;
        case DNS_SERVICE_COMD_INFO:
            _Command_ShowDNSResolvedInfo(pCmdIO,argc,argv);
            break;
        default:
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Invalid Input Command :[ %s ] \r\n", argv[1]);
            return false;
    }
    return true;
}


static int _Command_DNSLookUP(SYS_CMD_DEVICE_NODE* pCmdIO, char** argv)
{
    if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE)
    {
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "dnsc lookup: command in progress. Retry later.\r\n");
        return true;
    }

    if((strcmp(argv[2], "A") == 0) || (strcmp(argv[2], "a") == 0))
    {
        dnsType=TCPIP_DNS_TYPE_A;
    }
    else if((strcmp(argv[2], "AAAA") == 0) || (strcmp(argv[2], "aaaa") == 0))
    {
        dnsType=TCPIP_DNS_TYPE_AAAA;
    }
    else if((strcmp(argv[2], "ANY") == 0) || (strcmp(argv[2], "any") == 0))
    {
        dnsType=TCPIP_DNS_TYPE_ANY;
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "dnsc lookup: [%s] Lookup Type not supported.\r\n",argv[2]);
        return true;
    }

    if(strlen(argv[3]) > sizeof(dnslookupTargetHost) - 1)
    {
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "dnsc lookup: Host name too long. Retry.\r\n");
        return true;
    }
    strcpy(dnslookupTargetHost, argv[3]);

    dnsLookupCmdIoParam = pCmdIO->cmdIoParam;
    (*pCmdIO->pCmdApi->print)(pCmdIO, "dnsc lookup: resolving host: %s for type:%s \r\n", dnslookupTargetHost,argv[2]);
    tcpipCmdStat = TCPIP_DNS_LOOKUP_CMD_GET;
    pTcpipCmdDevice = pCmdIO;
    _TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, TCPIP_DNS_CLIENT_TASK_PROCESS_RATE);

    return false;
}

static int _Command_ShowDNSResolvedInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_DNS_ENTRY_QUERY dnsQuery;
    TCPIP_DNS_CLIENT_INFO clientInfo;

    IPV4_ADDR       ipv4Addr[TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS];
    char            hostName[TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN + 1];
    int             index, ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    TCPIP_DNS_RESULT res;
    bool entryPresent= false;
    IPV6_ADDR   ipv6Addr[TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS];
    char        addrPrintBuff[44];
    const char* strictName, *prefName;

    if (argc != 2) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnsc info \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: display the DNS cache entry details \r\n");
        return false;
    }


    dnsQuery.hostName = hostName;
    dnsQuery.nameLen = sizeof(hostName);
    dnsQuery.ipv4Entry = ipv4Addr;
    dnsQuery.nIPv4Entries = sizeof(ipv4Addr) / sizeof(*ipv4Addr);

    dnsQuery.ipv6Entry = ipv6Addr;
    dnsQuery.nIPv6Entries = sizeof(ipv6Addr) / sizeof(*ipv6Addr);

    res = TCPIP_DNS_ClientInfoGet(&clientInfo);
    if(res != TCPIP_DNS_RES_OK)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DNS Client is down!\r\n");
        return false;
    }

    strictName = TCPIP_STACK_NetNameGet(clientInfo.strictNet);
    if(strictName == 0)
    {
        strictName = "none";
    }
    prefName = TCPIP_STACK_NetNameGet(clientInfo.prefNet);
    if(prefName == 0)
    {
        prefName = "none";
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "DNS Client IF - Strict: %s, Preferred: %s\r\n", strictName, prefName);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "DNS Client - time: %d, pending: %d, current: %d, total: %d\r\n", clientInfo.dnsTime, clientInfo.pendingEntries, clientInfo.currentEntries, clientInfo.totalEntries);

    index = 0;
    while(1)
    {
        res = TCPIP_DNS_EntryQuery(&dnsQuery, index);
        if(res == TCPIP_DNS_RES_OK)
        {
            entryPresent = true;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Hostname = %s \r\nTimeout = %d \r\n", hostName, dnsQuery.ttlTime);
            if(dnsQuery.nIPv4ValidEntries > 0)
            {
                for(ix = 0; ix < dnsQuery.nIPv4ValidEntries; ix++)
                {                    
                    TCPIP_Helper_IPAddressToString(dnsQuery.ipv4Entry + ix, addrPrintBuff, sizeof(addrPrintBuff)); 
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 =%s\r\n", addrPrintBuff);
                }
            }
            if(dnsQuery.nIPv6Entries > 0)
            {
                for(ix = 0; ix < dnsQuery.nIPv6ValidEntries; ix++)
                {
                    TCPIP_Helper_IPv6AddressToString(dnsQuery.ipv6Entry + ix, addrPrintBuff, sizeof(addrPrintBuff));                   
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv6 = %s\r\n",addrPrintBuff);
                }
            }
            (*pCmdIO->pCmdApi->print)(cmdIoParam,"----------------------------------------------------\r\n",0);
        }
        if(res == TCPIP_DNS_RES_OK || res == TCPIP_DNS_RES_PENDING || res == TCPIP_DNS_RES_EMPTY_IX_ENTRY)
        {
            index++;
            continue;
        }

        // some error
        if(entryPresent == false)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No DNS Client Cache entries \r\n");
        }
        break;
    }
    return false;
}
#endif

#if defined(TCPIP_STACK_USE_DNS_SERVER)
static int _Command_DNSSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    typedef bool(*addSvcFnc)(TCPIP_NET_HANDLE hNet);

    TCPIP_NET_HANDLE netH;
    addSvcFnc        addFnc;
    bool             addRes, svcEnable;
    const char       *msgOK, *msgFail;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 4)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage:dnss service <interface> <on/off> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: dnss service PIC32INT on \r\n");
        return false;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[2]);
    if (netH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
        return false;
    }

    if (memcmp(argv[3], "on", 2) == 0)
    {   // turning on a service
        svcEnable = true;
    }
    else if (memcmp(argv[3], "off", 2) == 0)
    {   // turning off a service
        svcEnable = false;
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown option\r\n");
        return false;
    }
	addFnc = svcEnable?TCPIP_DNSS_Enable:TCPIP_DNSS_Disable;

        msgOK   = svcEnable?"enabled":"disabled";
        msgFail = svcEnable?"enable":"disable";

    addRes = (*addFnc)(netH);

    if(addRes)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s %s  for interface [%s] \r\n", argv[0], msgOK,argv[2]);
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Failed to %s %s for interface [%s]\r\n", msgFail, argv[0],argv[2]);
    }
    return true;
}

static int _Command_AddDelDNSSrvAddress(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv,DNS_SERVICE_COMD_TYPE dnsCommand)
{
    IP_ADDRESS_TYPE     addrType;
    uint8_t             *hostName;
    IP_MULTI_ADDRESS    ipDNS;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint32_t		entryTimeout=0;
#if defined(TCPIP_STACK_USE_IPV6)
    uint8_t     addrBuf[44];
#endif
    TCPIP_DNSS_RESULT res = TCPIP_DNSS_RES_NO_SERVICE;

    if(dnsCommand == DNS_SERVICE_COMD_DEL)
    {
         if (argc != 5) {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnss del <hostName> <IPType> <x.x.x.x>  \r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: hostName - URL , IPType - 4 for Ipv4 address and 6 for Ipv6 address \r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: One IP address per URL at a time will be deleted \r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: dnss del www.xyz.com 4 10.20.30.40  \r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: dnss del www.abc.com 6 2001::101  \r\n");
            return false;
        }
    }
    else if(dnsCommand == DNS_SERVICE_COMD_ADD)
    {
        if (argc < 6) {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnss add <hostName> <IPType> <x.x.x.x> <lifeTime> \r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: hostName - URL , IPType - 4 for Ipv4 address and 6 for Ipv6 address \r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: LifeTime - The life time in Second for each entry to be used \r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: One IP address per URL at a time will be added \r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: dnss add www.xyz.com 4 10.20.30.40 120 \r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: dnss add www.abc.com 6 2001::101 120 \r\n");
            return false;
        }
    }
    else
    {
        return false;
    }

    if(strlen(argv[2])>TCPIP_DNSS_HOST_NAME_LEN)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, " Hostname length should not be more than [%d]\r\n",TCPIP_DNSS_HOST_NAME_LEN);
        return false;
    }
    hostName = (uint8_t*)argv[2];

    if (memcmp(argv[3], "4", 1) == 0)
    {   // turning on a service
        addrType = IP_ADDRESS_TYPE_IPV4;
    }
#if defined(TCPIP_STACK_USE_IPV6)
    else if (memcmp(argv[3], "6", 1) == 0)
    {   // turning off a service
        addrType = IP_ADDRESS_TYPE_IPV6;
    }
#endif
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown option\r\n");
        return false;
    }
    if(addrType == IP_ADDRESS_TYPE_IPV4)
    {
        if (!TCPIP_Helper_StringToIPAddress(argv[4], &ipDNS.v4Add)) {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IPv4 address string \r\n");
            return false;
        }
    }
#if defined(TCPIP_STACK_USE_IPV6)
    if(addrType == IP_ADDRESS_TYPE_IPV6)
    {
        strncpy((char*)addrBuf, argv[4], sizeof(addrBuf) - 1);
        addrBuf[sizeof(addrBuf) - 1] = 0;
        if (!TCPIP_Helper_StringToIPv6Address((char*)addrBuf, &ipDNS.v6Add)) {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IPv6 address string \r\n");
            return false;
        }
    }
#endif

    if(dnsCommand == DNS_SERVICE_COMD_DEL)
    {
        res = TCPIP_DNSS_CacheEntryRemove((const char*)hostName,addrType,&ipDNS);
    }
    else if(dnsCommand == DNS_SERVICE_COMD_ADD)
    {
        entryTimeout = (unsigned long)atoi((char*)argv[5]);
        res = TCPIP_DNSS_EntryAdd((const char*)hostName,addrType,&ipDNS,entryTimeout);
    }

    switch(res)
    {
        case TCPIP_DNSS_RES_NO_ENTRY:
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "The Address is not part of the DNS Cache entry \r\n");
            return false;
        case TCPIP_DNSS_RES_MEMORY_FAIL:
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No memory available \r\n");
            return false;
        case TCPIP_DNSS_RES_CACHE_FULL:
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "No space to add [%s] entry \r\n",hostName);
            return false;
        case TCPIP_DNSS_RES_OK:
            return true;
        default:
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Failed to add [%s] entry \r\n",hostName);
            return false;
    }
}

static int _Command_DnsServService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int i=0;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    DNS_SERVICE_COMD_TYPE val=DNS_SERVICE_COMD_NONE;
    DNSS_COMMAND_MAP dnssComnd[]=
            {
                {"service",DNS_SERVICE_COMD_ENABLE_INTF},
                {"add", DNS_SERVICE_COMD_ADD,},
                {"del",DNS_SERVICE_COMD_DEL,},
                {"info",DNS_SERVICE_COMD_INFO,},
            }; 
	
	
    if (argc < 2) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnss <service/add/del/info> \r\n");
         return false;
    }
    
    for(i=0;i<(sizeof(dnssComnd)/sizeof(DNSS_COMMAND_MAP));i++)
    {
        if(strcmp(argv[1],dnssComnd[i].command) ==0)
        {
            val = dnssComnd[i].val;
            break;
        }
    }
    
    switch(val)
    {
        case DNS_SERVICE_COMD_ENABLE_INTF:
            _Command_DNSSOnOff(pCmdIO,argc,argv);
            break;
        case DNS_SERVICE_COMD_ADD:
            _Command_AddDelDNSSrvAddress(pCmdIO,argc,argv,val);
            break;
        case DNS_SERVICE_COMD_DEL:
            _Command_AddDelDNSSrvAddress(pCmdIO,argc,argv,val);
            break;
        case DNS_SERVICE_COMD_INFO:
            _Command_ShowDNSServInfo(pCmdIO,argc,argv);
            break;
        default:
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Invalid Input Command :[ %s ] \r\n", argv[1]);
            return false;
    }
    return true;
}

static int _Command_ShowDNSServInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    IP_MULTI_ADDRESS ipDNS;
    IP_ADDRESS_TYPE addrType;
    uint8_t         *hostName;
    size_t          ipcount=0;
    int             index=0;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    TCPIP_DNSS_RESULT res;
    uint32_t    ttlTime=0;
    bool        entryPresent=false;
    char        hostBuff[16];
#if defined(TCPIP_STACK_USE_IPV6)
    uint8_t     addrBuf[44];
#endif    

    if (argc != 3) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnsserv info <hostname> | <all>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: display the DNS cache entry details \r\n");
        return false;
    }
    hostName = (uint8_t*)argv[2];
    if(strcmp((char*)argv[2],"all")==0)
    {
    	index = 0;
        (*pCmdIO->pCmdApi->msg)(cmdIoParam,"HostName        IPv4/IPv6Count\r\n");

        while(1)
        {
            res = TCPIP_DNSS_AddressCntGet(index, hostBuff, sizeof(hostBuff), &ipcount);
            if(res == TCPIP_DNSS_RES_OK)
            {
                entryPresent = true;
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s       %d\r\n", hostBuff, ipcount);
            }
            else if(res == TCPIP_DNSS_RES_NO_SERVICE)
            {
                if(entryPresent == false)
                {
                   (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No DNS Server Cache entry \r\n");
                }
                entryPresent = false;
                break;
            }
            else
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No Memory is available \r\n");
                break;
            }
            index++;
        }
        return true;
    }
    addrType = IP_ADDRESS_TYPE_IPV4;
    index = 0;
    (*pCmdIO->pCmdApi->msg)(cmdIoParam,"HostName\t\tIPv4Address\t\tTTLTime \r\n");
    while(1)
    {
        res = TCPIP_DNSS_EntryGet((uint8_t*)hostName,addrType,index,&ipDNS,&ttlTime);
        if(res == TCPIP_DNSS_RES_OK)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s\t\t%d.%d.%d.%d\t\t%d\r\n",hostName,ipDNS.v4Add.v[0],ipDNS.v4Add.v[1],
                ipDNS.v4Add.v[2],ipDNS.v4Add.v[3],ttlTime);
            entryPresent = true;
        }
        else if((res == TCPIP_DNSS_RES_NO_SERVICE)|| (res == TCPIP_DNSS_RES_NO_ENTRY))
        {
            if(entryPresent == false)
            {
               (*pCmdIO->pCmdApi->print)(cmdIoParam, "[%s] No Ipv4 Address with in DNS Cache entry \r\n",hostName);
            }
            entryPresent = false;
            break;
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No Memory is available \r\n");
            break;
        }
        index++;
    }
    
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\r\n");

#if defined(TCPIP_STACK_USE_IPV6)
    addrType = IP_ADDRESS_TYPE_IPV6;
    index = 0;
    (*pCmdIO->pCmdApi->msg)(cmdIoParam,"HostName        IPv6Address             TTLTime \r\n");
    while(1)
    {
        res = TCPIP_DNSS_EntryGet((uint8_t*)hostName,addrType,index,&ipDNS,&ttlTime);
        if(res == TCPIP_DNSS_RES_OK)
        {
            TCPIP_Helper_IPv6AddressToString(&ipDNS.v6Add,(char*)addrBuf,sizeof(addrBuf));
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s       %s      %d\r\n",hostName,addrBuf,ttlTime);
            entryPresent = true;
        }
        else if((res == TCPIP_DNSS_RES_NO_SERVICE)|| (res == TCPIP_DNSS_RES_NO_ENTRY))
        {
            if(entryPresent == false)
            {
               (*pCmdIO->pCmdApi->print)(cmdIoParam, "[%s] No Ipv6 Address DNS Cache entry \r\n",hostName);
            }
            entryPresent = false;
            break;
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No Memory is available \r\n");
            break;
        }
       
        index++;
    }
#endif
    return true;
}
#endif

static int _Command_BIOSNameSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    const char* msg;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setbios <interface> <x.x.x.x> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setbios PIC32INT MCHPBOARD_29 \r\n");
        return false;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return false;
    }

    if(TCPIP_STACK_NetBiosNameSet(netH, argv[2]))
    {
        msg = "Set BIOS Name OK\r\n";
    }
    else
    {
        msg = "Set BIOS Name failed\r\n";
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, msg);
    return true;
}

static int _Command_MACAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    TCPIP_MAC_ADDR macAddr;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 3) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setmac <interface> <x:x:x:x:x:x> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setmac PIC32INT aa:bb:cc:dd:ee:ff \r\n");
        return false;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == 0) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return false;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "argv[2]: %s\r\n", argv[2]);

    if (!TCPIP_Helper_StringToMACAddress(argv[2], macAddr.v)) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid MAC address string \r\n");
        return false;
    }

    if(!TCPIP_STACK_NetAddressMacSet(netH, &macAddr)) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Set MAC address failed\r\n");
        return false;
    }

    return true;
}

#if defined(TCPIP_STACK_USE_TFTP_SERVER)
static int _Command_TFTPServerOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // tftps <interface> <start/stop> <add-type>
    // tftps status

    int  opCode = 0;        // 0- none; 1 - start; 2 - stop
    bool opRes;
    IP_ADDRESS_TYPE ipType = IP_ADDRESS_TYPE_ANY;
    TCPIP_NET_HANDLE netH;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool printUsage = true;

    while(argc >= 2)
    {
        if((strcmp(argv[1], "status") == 0))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "TFTPS - Number of clients running: %d \r\n", TCPIP_TFTPS_ClientsNumber());
            return true;
        }

        if(argc < 3)
        {
            break;
        }

        netH = TCPIP_STACK_NetHandleGet(argv[1]);
        if (netH == 0)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "TFTPS - Unknown interface\r\n");
            return false;
        }

        if (strcmp(argv[2], "start") == 0)
        {
            if(TCPIP_TFTPS_IsEnabled())
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "TFTPS - already running\r\n");
                return true;
            }

            opCode = 1;
        }
        else if (strcmp(argv[2], "stop") == 0)
        {
            if(TCPIP_TFTPS_IsEnabled() == 0)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "TFTPS - already stopped\r\n");
                return true;
            }
            opCode = 2;
        }
        else
        {
            break;
        }

        if(argc > 3)
        {
            int type = atoi(argv[3]);
            if(type == 4)
            {
                ipType = IP_ADDRESS_TYPE_IPV4;
            }
            else if(type == 6)
            {
                ipType = IP_ADDRESS_TYPE_IPV6;
            }
            else if(type == 0)
            {
                ipType = IP_ADDRESS_TYPE_ANY;
            }
            else
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "TFTPS - Invalid address type\r\n");
                return false;
            }
        }

        printUsage = false;
        break;
    }


    if (printUsage)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: tftps <interface> <start/stop> <add-type>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: tftps status\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "add-type: 4 for IPv4, 6 for IPv6, 0/none for ANY \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: tftps eth0 start 4\r\n");
    }

    if(opCode != 0)
    {
        if(opCode == 1)
        {
            opRes = TCPIP_TFTPS_Enable(netH, ipType);
        }
        else
        {
            opRes = TCPIP_TFTPS_Disable(netH);
        }

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "TFTPS - operation %s!\r\n", opRes ? "succesful" : "failed");
    }

    return true;

}
#endif  

#if (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
static int _Command_NetworkOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    bool res = false;
    TCPIP_NET_HANDLE netH;
#if defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
    TCPIP_COMMAND_STG_DCPT*   pDcpt;
    TCPIP_NETWORK_CONFIG*     pNetConf;
#endif  // defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
    TCPIP_NETWORK_CONFIG ifConf, *pIfConf;
    SYS_MODULE_OBJ      tcpipStackObj;
    TCPIP_STACK_INIT    tcpip_init_data;
    uint16_t net_ix = 0;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: if <interface> <down/up> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: if PIC32INT down \r\n");
        return false;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);

    if (netH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return false;
    }

    net_ix = TCPIP_STACK_NetIndexGet(netH);

    if (memcmp(argv[2], "up", 2) == 0)
    {
        if(TCPIP_STACK_NetIsUp(netH))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "This interface already up\r\n");
            return true;
        }

        // get the data passed at initialization
        tcpipStackObj = TCPIP_STACK_Initialize(0, 0);
        TCPIP_STACK_InitializeDataGet(tcpipStackObj, &tcpip_init_data);
        pIfConf = &ifConf;
        memcpy(pIfConf, tcpip_init_data.pNetConf + net_ix, sizeof(*pIfConf));

#if defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
        if(pCmdStgDcpt) 
        {
            // get the saved network configuration
            pDcpt = pCmdStgDcpt + net_ix;
            if(pDcpt->stgSize)
            {   // saved config is valid; restore
                pNetConf = TCPIP_STACK_NetConfigSet(&pDcpt->netDcptStg, pDcpt->restoreBuff, sizeof(pDcpt->restoreBuff), 0);
                if(pNetConf)
                {   // use the saved data
                    pIfConf = pNetConf;
                }
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Interface up: configuration " );
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, pNetConf ? "restored\r\n" : "restore failed!\r\n");
            }
        }
#endif  // defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)

        // change the power mode to FULL
        pIfConf->powerMode = TCPIP_STACK_IF_POWER_FULL;
        res = TCPIP_STACK_NetUp(netH, pIfConf);
    }
    else if (memcmp(argv[2], "down", 4) == 0)
    {
        if(TCPIP_STACK_NetIsUp(netH) == 0)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "This interface already down\r\n");
            return true;
        }

#if defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
        if(pCmdStgDcpt) 
        {
            // get the last network configuration so we use it when
            // restart the stack/interface 
            pDcpt = pCmdStgDcpt + net_ix;
            pDcpt->stgSize = TCPIP_STACK_NetConfigGet(netH, &pDcpt->netDcptStg, sizeof(pDcpt->netDcptStg), 0);

            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Interface down: configuration saved\r\n");
        }
#endif  // defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)

        res = TCPIP_STACK_NetDown(netH);
    } 
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Wrong parameter specified \r\n");
        return false;
    }

    if (res == true)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Operation successful!\r\n");
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Operation failed!\r\n");
    }

    return true;
}
#endif  // (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static int _Command_StackOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
#if defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
    TCPIP_NET_HANDLE netH;
    int              netIx;
    TCPIP_COMMAND_STG_DCPT  *pDcpt;
    TCPIP_NETWORK_CONFIG    *pCurrConf, *pDstConf;
#endif  // defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
    TCPIP_STACK_INIT        tcpipInit;
    SYS_MODULE_OBJ          tcpipStackObj;     // stack handle
    const char              *msg;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: stack <up/down> <preserve>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: stack down preserve\r\n");
        return false;
    }


    if (memcmp(argv[1], "up", 2) == 0)
    {
        // try to get a stack handle
        tcpipStackObj = TCPIP_STACK_Initialize(0, 0);
        if ( tcpipStackObj != SYS_MODULE_OBJ_INVALID)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Stack already up!\r\n");
            return true;
        }
        // check the saved init data when the stack went down
        if(pCmdTcpipInitData == 0)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Turn Stack down and then up!\r\n");
            return true;
        }

        // copy of the init data; use as default
        tcpipInit = *pCmdTcpipInitData;

#if defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
        if(pCmdStgDcpt != 0 && pCmdNetConf != 0) 
        {
            // get the saved network configuration
            pDcpt = pCmdStgDcpt + 0;
            pDstConf = pCmdNetConf + 0; 
            pCurrConf = 0;
            for (netIx = 0; netIx < initialNetIfs; netIx++)
            {
                if(pDcpt->stgSize)
                {   // saved config is valid; restore
                    pCurrConf = TCPIP_STACK_NetConfigSet(&pDcpt->netDcptStg, pDcpt->restoreBuff, sizeof(pDcpt->restoreBuff), 0);
                }
                else
                {   // don't have a config to restore
                    pCurrConf = 0;
                }

                if(pCurrConf == 0)
                {   // restore failed
                    break;
                }
                else
                {   // save into array for the stack initialization
                    // force the interface start with power up
                    pCurrConf->powerMode = TCPIP_STACK_IF_POWER_FULL;
                    memcpy(pDstConf, pCurrConf, sizeof(*pDstConf));
                }

                pDcpt++;
                pDstConf++;
            }

            if(pCurrConf)
            {   // success
                tcpipInit.pNetConf = pCmdNetConf;
                tcpipInit.nNets = initialNetIfs;
                msg = "Stack up: configuration restored\r\n";
            }
            else
            {
                msg = "Stack up: configuration restore failed\r\n";
            }

            (*pCmdIO->pCmdApi->msg)(cmdIoParam, msg);
        }
#endif  // defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Restarting the stack with %d interface(s)\r\n", tcpipInit.nNets);

        tcpipStackObj = TCPIP_STACK_Initialize(0, &tcpipInit.moduleInit);     // init the stack
        if ( tcpipStackObj == SYS_MODULE_OBJ_INVALID)
        {
            msg = "Stack up failed\r\n";
        }
        else
        {
            msg = "Stack up succeeded\r\n";
        }
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, msg);
    }
    else if (memcmp(argv[1], "down", 4) == 0)
    {
        // try to get a handle
        tcpipStackObj = TCPIP_STACK_Initialize(0, 0);
        if ( tcpipStackObj == SYS_MODULE_OBJ_INVALID)
        {
            msg = "Stack down: cannot get a stack handle\r\n";
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, msg);
        }
        else
        {
            // store the data passed at initialization
            TCPIP_STACK_InitializeDataGet(tcpipStackObj, &cmdTcpipInitData);
            pCmdTcpipInitData = &cmdTcpipInitData;

#if defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
            tcpipCmdPreserveSavedInfo = false;
            if(argc == 3 && memcmp(argv[2], "preserve", strlen("preserve")) == 0)
            {
                if(pCmdStgDcpt) 
                {
                    // get the last network configuration so we use it when
                    // restart the stack/interface 
                    pDcpt = pCmdStgDcpt + 0;
                    for (netIx = 0; netIx < initialNetIfs; netIx++)
                    {
                        netH = TCPIP_STACK_IndexToNet(netIx);
                        pDcpt->stgSize = TCPIP_STACK_NetConfigGet(netH, &pDcpt->netDcptStg, sizeof(pDcpt->netDcptStg), 0);
                        pDcpt++;
                    }

                    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Stack down: configuration saved\r\n");
                    tcpipCmdPreserveSavedInfo = true;
                }
            }
#endif  // defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)

            TCPIP_STACK_Deinitialize(tcpipStackObj);
#if defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
            tcpipCmdPreserveSavedInfo = false;          // make sure it doesn't work the next time
#endif  // defined(_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Stack down succeeded\r\n");
        }
    }

    return true;
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static int _Command_HeapInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)    
    int     ix, nEntries;
    TCPIP_HEAP_TRACE_ENTRY    tEntry;
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)    
    int     nTraces;
    size_t  heapSize;
    TCPIP_STACK_HEAP_HANDLE heapH;
    const char* typeMsg;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    unsigned int hType, startType, endType;
    bool hasArgs = false;
    static const char* heapTypeStr[TCPIP_STACK_HEAP_TYPES] = 
    {
        0,              // TCPIP_STACK_HEAP_TYPE_NONE
        "internal",     // TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP
        "pool",         // TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL
        "external",     // TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP
    };


    if (argc > 1)
    {   // there is an arg
        hType = (unsigned int)atoi(argv[1]);
        if(hType == TCPIP_STACK_HEAP_TYPE_NONE || hType >= TCPIP_STACK_HEAP_TYPES)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Unknown heap type. Use: [1, %d]\r\n", TCPIP_STACK_HEAP_TYPES - 1);
            return false;
        }
        // valid
        startType = hType;
        endType = hType + 1;
        hasArgs = true;
    }
    else
    {   // consider all types
        startType = TCPIP_STACK_HEAP_TYPE_NONE + 1;
        endType = TCPIP_STACK_HEAP_TYPES;
    }

    // display info for each type
    for(hType = startType; hType < endType; hType++)
    {
        typeMsg = heapTypeStr[hType];
        heapH = TCPIP_STACK_HeapHandleGet(hType, 0);
        if(heapH == 0)
        {
            if(hasArgs == true)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "No heap info exists for type: %s!\r\n", typeMsg);
            }
            continue;
        }

        // display heap info
        heapSize = TCPIP_HEAP_Size(heapH);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Heap type: %s. Initial created heap size: %d Bytes\r\n", typeMsg, heapSize);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Allocable block heap size: %d Bytes\r\n", TCPIP_HEAP_MaxSize(heapH));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "All available heap size: %d Bytes, high watermark: %d\r\n", TCPIP_HEAP_FreeSize(heapH), TCPIP_HEAP_HighWatermark(heapH));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Last heap error: 0x%x\r\n", TCPIP_HEAP_LastError(heapH));

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)    
        nTraces = TCPIP_HEAP_TraceGetEntriesNo(heapH, true);
        if(nTraces)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Trace info: \r\n");
            nEntries = TCPIP_HEAP_TraceGetEntriesNo(heapH, false);
            for(ix = 0; ix < nEntries; ix++)
            {
                if(TCPIP_HEAP_TraceGetEntry(heapH, ix, &tEntry))
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Module: %4d, totAllocated: %5d, currAllocated: %5d, totFailed: %5d, maxFailed: %5d \r\n", tEntry.moduleId, tEntry.totAllocated, tEntry.currAllocated, tEntry.totFailed, tEntry.maxFailed);
                }

            }
        }
#else
        nTraces = 0;
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)    

        if(nTraces == 0)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No Trace info exists.\r\n");
        }

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
        nEntries = TCPIP_HEAP_DistGetEntriesNo(heapH);
        if(nEntries)
        {
            int     modIx;
            TCPIP_HEAP_DIST_ENTRY distEntry;
            int currLowHitMem = 0;
            int currHiHitMem = 0;

            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "TCPIP Heap distribution: \r\n");

            for(ix = 0; ix < nEntries; ix++)
            {
                TCPIP_HEAP_DistGetEntry(heapH, ix, &distEntry);

                int entryPrint = 0;
                struct moduleDist* pMDist = distEntry.modDist;
                for(modIx = 0; modIx < sizeof(distEntry.modDist)/sizeof(*distEntry.modDist); modIx++, pMDist++)
                {
                    if(pMDist->modHits)
                    {
                        if(entryPrint == 0)
                        {
                            (*pCmdIO->pCmdApi->print)(cmdIoParam, "[%4d,    %5d]:\r\n", distEntry.lowLimit, distEntry.highLimit);
                            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tcurr hits: %d, \r\n", distEntry.currHits);
                            currLowHitMem += distEntry.currHits * distEntry.lowLimit;
                            currHiHitMem += distEntry.currHits * distEntry.highLimit;
                            entryPrint = 1;
                        }
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t mod: %d, \thits: %d, \r\n", pMDist->modId, pMDist->modHits);
                    }
                }
                if(distEntry.gHits)
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t mod: xx \thits: %d, \r\n", distEntry.gHits);
                }
            }

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "curr Low Lim: %d, curr Hi Lim: %d, max Free: %d, min Free: %d\r\n", currLowHitMem, currHiHitMem, heapSize - currLowHitMem, heapSize - currHiHitMem);
        }
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

    }

    return true;
}

static int _Command_MacInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int                     netNo, netIx;
    TCPIP_NET_HANDLE        netH;
    TCPIP_MAC_RX_STATISTICS rxStatistics;
    TCPIP_MAC_TX_STATISTICS txStatistics;
    TCPIP_MAC_STATISTICS_REG_ENTRY  regEntries[50];
    TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntry;
    int                     jx, hwEntries;
    char                    entryName[sizeof(pRegEntry->registerName) + 1];
    const char*             netName;

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 1) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: macinfo \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: macinfo \r\n");
        return false;
    }


    netNo = TCPIP_STACK_NumberOfNetworksGet();
    for(netIx = 0; netIx < netNo; netIx++)
    {
        netH = TCPIP_STACK_IndexToNet(netIx);
        if(TCPIP_STACK_NetGetType(netH) != TCPIP_NETWORK_TYPE_PRIMARY)
        {   // interested only in the primary interfaces
            continue;
        }

        netName = TCPIP_STACK_NetNameGet(netH);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Interface: %s Driver Statistics\r\n", netName);
        if(TCPIP_STACK_NetMACStatisticsGet(netH, &rxStatistics, &txStatistics))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\n Receive Statistics\r\n");
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t nRxOkPackets: %d\r\n\t nRxPendBuffers: %d\r\n\t nRxSchedBuffers: %d\r\n",
                    rxStatistics.nRxOkPackets, rxStatistics.nRxPendBuffers, rxStatistics.nRxSchedBuffers);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t nRxErrorPackets: %d\r\n\t nRxFragmentErrors: %d\r\n\t nRxBuffNotAvailable: %d\r\n", rxStatistics.nRxErrorPackets, rxStatistics.nRxFragmentErrors,rxStatistics.nRxBuffNotAvailable);
            
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\n Transmit Statistics\r\n");
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t nTxOkPackets: %d\r\n\t nTxPendBuffers: %d\r\n\t nTxErrorPackets: %d\r\n\t nTxQueueFull: %d\r\n\r\n",
                    txStatistics.nTxOkPackets, txStatistics.nTxPendBuffers, txStatistics.nTxErrorPackets, txStatistics.nTxQueueFull);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\tnot supported\r\n");
        }

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Interface: %s Hardware Register Status\r\n", netName);
        if(TCPIP_STACK_NetMACRegisterStatisticsGet(netH, regEntries, sizeof(regEntries)/sizeof(*regEntries), &hwEntries))
        {
            entryName[sizeof(entryName) - 1] = 0;
            for(jx = 0, pRegEntry = regEntries; jx < hwEntries && jx < sizeof(regEntries)/sizeof(*regEntries); jx++, pRegEntry++)
            {
                strncpy(entryName, pRegEntry->registerName, sizeof(entryName) - 1);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t %s: 0x%x\r\n", entryName, pRegEntry->registerValue);
            }
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\tnot supported\r\n");
        }

    }

    return true;
}

#if defined(TCPIP_STACK_USE_DNS)
void TCPIPCmdDnsTask(void)
{
    TCPIP_DNS_RESULT  dnsRes;
    char ipv4Index=0,ipv6Index=0;
    int         nIPv4Entries;
    IPV4_ADDR   ip4Address;
    int         nIPv6Entries;
    IPV6_ADDR   ip6Address;
    uint8_t     addrBuf[44];
    uint32_t    timeout=0;

    switch(tcpipCmdStat)
    {
        case TCPIP_DNS_LOOKUP_CMD_GET:
            dnsRes = TCPIP_DNS_Resolve(dnslookupTargetHost, dnsType);
            if(dnsRes != TCPIP_DNS_RES_OK && dnsRes != TCPIP_DNS_RES_PENDING && dnsRes != TCPIP_DNS_RES_NAME_IS_IPADDRESS)
            {   // some other error
                (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "DNS Lookup: DNS failure for %s, err: %d\r\n", dnslookupTargetHost, dnsRes);
                tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
                break;
            }
            tcpipCmdStat = TCPIP_DNS_LOOKUP_CMD_WAIT;
            dnsLookUpStartTick = SYS_TMR_TickCountGet();
            // else wait some more
            break;
        case TCPIP_DNS_LOOKUP_CMD_WAIT:
            dnsRes = TCPIP_DNS_IsResolved(dnslookupTargetHost, 0, IP_ADDRESS_TYPE_ANY);
            timeout = (SYS_TMR_TickCountGet() - dnsLookUpStartTick)/SYS_TMR_TickCounterFrequencyGet();
            if(timeout >= (TCPIP_DNS_CLIENT_SERVER_TMO/2))
            {   // timeout
                (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "DNS Lookup: request timeout.\r\n");
                tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
                break;
            }
            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {   // operation in progress
                break;
            }
            else if(dnsRes < 0 )
            {   // timeout or some other DNS error
                (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "DNS Lookup: DNS failure for %s, err: %d\r\n", dnslookupTargetHost, dnsRes);
                tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
                break;
            }
            _TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, 0);
            tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
            // success
            (*pTcpipCmdDevice->pCmdApi->msg)(dnsLookupCmdIoParam, "Lookup Answer:\r\n----------------------\r\n");
            nIPv4Entries = TCPIP_DNS_GetIPAddressesNumber(dnslookupTargetHost,IP_ADDRESS_TYPE_IPV4);
            nIPv6Entries = TCPIP_DNS_GetIPAddressesNumber(dnslookupTargetHost,IP_ADDRESS_TYPE_IPV6);
            if((nIPv4Entries == 0) && (nIPv6Entries == 0))
            {
                (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "No Lookup entry for [%s]\r\n",dnslookupTargetHost);
                break;
            }
            while(1)
            {
                if(ipv4Index<nIPv4Entries)
                {
                    TCPIP_DNS_GetIPv4Addresses(dnslookupTargetHost, ipv4Index, &ip4Address, 1);
                    (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "[%s] A IPv4 Address : %d.%d.%d.%d\r\n",dnslookupTargetHost,ip4Address.v[0],
                            ip4Address.v[1],ip4Address.v[2],ip4Address.v[3]);
                    ipv4Index++;
                }
                else if(ipv6Index<nIPv6Entries)
                {
                    TCPIP_DNS_GetIPv6Addresses(dnslookupTargetHost, ipv6Index, &ip6Address, 1);
                    memset(addrBuf,0,sizeof(addrBuf));
                    TCPIP_Helper_IPv6AddressToString(&ip6Address,(char*)addrBuf,sizeof(addrBuf));
                    (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "[%s] AAAA IPv6 Address :%s\r\n",dnslookupTargetHost,addrBuf);
                    ipv6Index++;
                }
                else
                {
                    break;
                }
            }

        default:
            break;
    }
}
#endif

#if defined(_TCPIP_COMMAND_PING4)
static int _CommandPing(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int     currIx;    
    TCPIP_COMMANDS_STAT  newCmdStat;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ping Usage: ping <stop>/<name/address> <i interface> <n nPings> <t msPeriod> <s size>\r\n");
        return true;
    }

    if(strcmp(argv[1], "stop") == 0)
    {
        if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE)
        {
            _PingStop(pCmdIO, cmdIoParam);
        }
        return true;
    }

    if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ping: command in progress. Retry later.\r\n");
        return true;
    }

    // get the host
    if(TCPIP_Helper_StringToIPAddress(argv[1], &icmpTargetAddr))
    {
        strncpy(icmpTargetAddrStr, argv[1], sizeof(icmpTargetAddrStr) - 1);
        icmpTargetAddrStr[sizeof(icmpTargetAddrStr) - 1] = 0;
        icmpTargetHost[0] = '\0';
        newCmdStat = TCPIP_PING_CMD_START_PING;
    }
    else
    {   // assume host address
        if(strlen(argv[1]) > sizeof(icmpTargetHost) - 1)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ping: Host name too long. Retry.\r\n");
            return true;
        }
        strcpy(icmpTargetHost, argv[1]);
        newCmdStat = TCPIP_PING_CMD_DNS_GET;
    }

    // get additional parameters, if any
    //
    icmpReqNo = 0;
    icmpReqDelay = 0;

    currIx = 2;

    while(currIx + 1 < argc)
    { 
        char* param = argv[currIx];
        char* paramVal = argv[currIx + 1];

        if(strcmp(param, "i") == 0)
        {
            if((icmpNetH = TCPIP_STACK_NetHandleGet(paramVal)) == 0)
            {   // use default interface
                icmpNetH = TCPIP_STACK_NetDefaultGet();
            }
        }
        else if(strcmp(param, "n") == 0)
        {
            icmpReqNo = atoi(paramVal);
        }
        else if(strcmp(param, "t") == 0)
        {
            icmpReqDelay = atoi(paramVal);
        }
        else if(strcmp(param, "s") == 0)
        {
            int pingSize = atoi(paramVal);
            if(pingSize <= sizeof(icmpPingBuff))
            {
                icmpPingSize = pingSize;
            }
            else
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "Ping: Data size too big. Max: %d. Retry\r\n", sizeof(icmpPingBuff));
                return true;
            }

        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ping: Unknown parameter\r\n");
        }

        currIx += 2;
    }


    tcpipCmdStat = newCmdStat;
    if(tcpipCmdStat == TCPIP_PING_CMD_DNS_GET)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Ping: resolving host: %s\r\n", icmpTargetHost);
    }

    icmpSequenceNo = SYS_RANDOM_PseudoGet();
    icmpIdentifier = SYS_RANDOM_PseudoGet();

    if(icmpReqNo == 0)
    {
        icmpReqNo = TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS;
    }
    if(icmpReqDelay == 0)
    {
        icmpReqDelay = TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY;
    }

    // convert to ticks
    if(icmpReqDelay < TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY)
    {
        icmpReqDelay = TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY;
    }

    pTcpipCmdDevice = pCmdIO;
    icmpCmdIoParam = cmdIoParam; 
    icmpAckRecv = 0;
    icmpReqCount = 0;

    _TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, icmpReqDelay);

    return true;

}

static void CommandPingHandler(const  TCPIP_ICMP_ECHO_REQUEST* pEchoReq, TCPIP_ICMP_REQUEST_HANDLE iHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result)
{
    char addBuff[20];

    if(result == TCPIP_ICMP_ECHO_REQUEST_RES_OK)
    {   // reply has been received
        uint32_t errorMask = 0;     // error mask:
        // 0x1: wrong id
        // 0x2: wrong seq
        // 0x4: wrong target
        // 0x8: wrong size
        // 0x10: wrong data
        //
        if(pEchoReq->identifier != icmpIdentifier)
        {
            errorMask |= 0x1;
        }

        if(pEchoReq->sequenceNumber != icmpSequenceNo)
        {
            errorMask |= 0x2;
        }

        if(pEchoReq->targetAddr.Val != icmpTargetAddr.Val)
        {
            errorMask |= 0x4;
        }

        if(pEchoReq->dataSize != icmpPingSize)
        {
            errorMask |= 0x8;
        }

        // check the data
        int ix;
        int checkSize = pEchoReq->dataSize < icmpPingSize ? pEchoReq->dataSize : icmpPingSize;
        uint8_t* pSrc = icmpPingBuff;
        uint8_t* pDst = pEchoReq->pData;
        for(ix = 0; ix < checkSize; ix++)
        {
            if(*pSrc++ != *pDst++)
            {
                errorMask |= 0x10;
                break;
            }
        }

        if(errorMask != 0)
        {   // some errors
            (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: wrong reply received. Mask: 0x%2x\r\n", errorMask);
        }
        else
        {   // good reply
            uint32_t pingTicks = SYS_TMR_TickCountGet() - icmpStartTick;
            int pingMs = (pingTicks * 1000) / SYS_TMR_TickCounterFrequencyGet();
            if(pingMs == 0)
            {
                pingMs = 1;
            }

            TCPIP_Helper_IPAddressToString(&pEchoReq->targetAddr, addBuff, sizeof(addBuff));

            (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: reply[%d] from %s: time = %dms\r\n", ++icmpAckRecv, addBuff, pingMs);
        }
    }
    else
    {
#if (_TCPIP_COMMAND_PING4_DEBUG != 0)
        (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: request aborted by ICMP with result %d\r\n", result);
#endif  // (_TCPIP_COMMAND_PING4_DEBUG != 0)
    }
    // one way or the other, request is done
    icmpReqHandle = 0;
}

#endif  // defined(_TCPIP_COMMAND_PING4)

#if defined(_TCPIP_COMMAND_PING6)
static int _Command_IPv6_Ping(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint32_t size =0;
    TCPIP_NET_HANDLE netH;
    int     argIx;

    if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ping6: command in progress. Retry later.\r\n");
        return true;
    }

    if((argc < 2) ||(argc > 4))
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ping6 <net> <x::x:x:x:x> <size> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ping6 fe80::23:2222:3333:1234 1500\r\n");
        return true;
    }
 // check the 1st parameter type
    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if(netH == 0)
    {   // use default interface
        icmpNetH = TCPIP_STACK_NetDefaultGet();
        argIx = 1;
    }
    else
    {
        icmpNetH = netH;
        argIx = 2;
        if (argc < 3)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ping6 <net> <x::x:x:x:x> <size> \r\n");
            return true;
        }
    }

    if(TCPIP_Helper_StringToIPv6Address(argv[argIx], &icmpv6TargetAddr))
    {
        strncpy(icmpTargetAddrStr, argv[argIx], sizeof(icmpTargetAddrStr) - 1);
        icmpTargetAddrStr[sizeof(icmpTargetAddrStr) - 1] = 0;
        icmpTargetHost[0] = '\0';
        tcpipCmdStat = TCPIP_SEND_ECHO_REQUEST_IPV6;
        memset(icmpv6TargetAddrStr,0,sizeof(icmpv6TargetAddrStr));
        if(strlen(argv[argIx]) <= sizeof(icmpv6TargetAddrStr))
        {
            strcpy(icmpv6TargetAddrStr,argv[argIx]);
        }
    }
     else
    {   // assume host address
        if(strlen(argv[argIx]) > sizeof(icmpTargetHost) - 1)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ping6: Host name too long. Retry.\r\n");
            return true;
        }
        strcpy(icmpTargetHost, argv[argIx]);
        tcpipCmdStat = TCPIP_PING6_CMD_DNS_GET;
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "ping6: resolving host: %s\r\n", icmpTargetHost);
    }
    
    if(argv[argIx+1] == NULL)
    {
        size  = 0;
    }
    else
    {
        size  = atoi((char *)argv[argIx+1]);
    }
    
     pingPktSize = size;

    if(hIcmpv6 == 0)
    {
        if((hIcmpv6 = TCPIP_ICMPV6_CallbackRegister(CommandPing6Handler)) == 0)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ping6: Failed to register ICMP handler\r\n");
            return true;
        }
    }

    icmpSequenceNo = SYS_RANDOM_PseudoGet();
    icmpIdentifier = SYS_RANDOM_PseudoGet();
    icmpReqNo = 0;
    icmpReqDelay = 0;
    if(icmpReqNo == 0)
    {
        icmpReqNo = TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS;
    }
    if(icmpReqDelay == 0)
    {
        icmpReqDelay = TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY;
    }

    // convert to ticks
    if(icmpReqDelay < TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY)
    {
        icmpReqDelay = TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY;
    }

    pTcpipCmdDevice = pCmdIO;
    icmpCmdIoParam = cmdIoParam;
    icmpAckRecv = 0;
    icmpReqCount = 0;

    _TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, icmpReqDelay);
    return true;

}

static void CommandPing6Handler(TCPIP_NET_HANDLE hNetIf,uint8_t type, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, void * data)
{
    char addBuff[42];

    if(tcpipCmdStat == TCPIP_CMD_STAT_IDLE)
    {
        return; // not our reply?
    }

    if(type != ICMPV6_INFO_ECHO_REPLY)
    {
        return;
    }
    ICMPV6_HEADER_ECHO* pReply = (ICMPV6_HEADER_ECHO*)data;
    uint16_t myRecvId = pReply->identifier;
    uint16_t myRecvSequenceNumber = pReply->sequenceNumber;


    if (myRecvSequenceNumber != icmpSequenceNo || myRecvId != icmpIdentifier)
    {
        (*pTcpipCmdDevice->pCmdApi->msg)(icmpCmdIoParam, "ping6: wrong reply received\r\n");
    }
    else
    {
        uint32_t pingTicks = SYS_TMR_TickCountGet() - icmpStartTick;
        int pingMs = (pingTicks * 1000) / SYS_TMR_TickCounterFrequencyGet();
        if(pingMs == 0)
        {
            pingMs = 1;
        }
        memset(addBuff,0,sizeof(addBuff));
        TCPIP_Helper_IPv6AddressToString(remoteIP, addBuff, sizeof(addBuff));

        (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "ping6: reply from [%s] time = %dms\r\n", addBuff, pingMs);
        icmpAckRecv++;
    }

}
#endif  // defined(_TCPIP_COMMAND_PING6)


#if defined(_TCPIP_COMMAND_PING4) || defined(_TCPIP_COMMAND_PING6)
static void _PingStop(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam)
{
#if defined(_TCPIP_COMMAND_PING4)
    if(icmpReqHandle != 0)
    {
#if (_TCPIP_COMMAND_PING4_DEBUG == 0)
        TCPIP_ICMP_EchoRequestCancel(icmpReqHandle);
#else
        if(TCPIP_ICMP_EchoRequestCancel(icmpReqHandle) != ICMP_ECHO_OK)
        {   // this should NOT happen!
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ping stop failed!\r\n");
        }
        else
        { 
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ping: request aborted by tcpip CMD: stop!\r\n");
        }
#endif  // (_TCPIP_COMMAND_PING4_DEBUG == 0)

        icmpReqHandle = 0;
    }
#endif  // defined(_TCPIP_COMMAND_PING4)

    _TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, 0);
    tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
    if(pCmdIO)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Ping: done. Sent %d requests, received %d replies.\r\n", icmpReqCount, icmpAckRecv);
    }
    pTcpipCmdDevice = 0;
}


static void TCPIPCmdPingTask(void)
{
#if defined(_TCPIP_COMMAND_PING4)
    ICMP_ECHO_RESULT echoRes;
    TCPIP_ICMP_ECHO_REQUEST echoRequest;
    bool cancelReq, newReq;
#endif  // defined(_TCPIP_COMMAND_PING4)
#if defined(_TCPIP_COMMAND_PING6)
    bool ipv6EchoRes=false;
#endif
    TCPIP_DNS_RESULT  dnsRes;
    bool killIcmp = false;
       
    switch(tcpipCmdStat)
    {
#if defined(_TCPIP_COMMAND_PING4)
        case TCPIP_PING_CMD_DNS_GET:          
            dnsRes = TCPIP_DNS_Resolve(icmpTargetHost, TCPIP_DNS_TYPE_A);
            if(dnsRes != TCPIP_DNS_RES_OK && dnsRes != TCPIP_DNS_RES_PENDING && dnsRes != TCPIP_DNS_RES_NAME_IS_IPADDRESS)
            {   // some other error
                (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: DNS failure for %s\r\n", icmpTargetHost);
                killIcmp = true;
                break;
            }
            tcpipCmdStat = TCPIP_PING_CMD_DNS_WAIT;
            // else wait some more
            break;

        case TCPIP_PING_CMD_DNS_WAIT:
            dnsRes = TCPIP_DNS_IsNameResolved(icmpTargetHost, &icmpTargetAddr, 0);
            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {   // operation in progress
                break;
            }
            else if(dnsRes < 0 )
            {   // timeout or some other DNS error
                (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: DNS failure for %s\r\n", icmpTargetHost);
                killIcmp = true;
                break;
            }
            // success
 
            TCPIP_Helper_IPAddressToString(&icmpTargetAddr, icmpTargetAddrStr, sizeof(icmpTargetAddrStr));
            tcpipCmdStat = TCPIP_PING_CMD_START_PING;            
            break;

        case TCPIP_PING_CMD_START_PING:
            icmpStartTick = 0;  // try to start as quickly as possible
            tcpipCmdStat = TCPIP_PING_CMD_DO_PING;            
            // no break needed here!

        case TCPIP_PING_CMD_DO_PING:
            if(icmpReqCount == icmpReqNo)
            {   // no more requests to send
                killIcmp = true;
                break;
            }

            // check if time for another request
            cancelReq = newReq = false;
            if(SYS_TMR_TickCountGet() - icmpStartTick > (SYS_TMR_TickCounterFrequencyGet() * icmpReqDelay) / 1000)
            {
                cancelReq = icmpReqCount != icmpAckRecv && icmpReqHandle != 0;    // cancel if there is another one ongoing
                newReq = true;
            }
            else if(icmpReqCount != icmpAckRecv)
            {   // no reply received to the last ping 
                if(SYS_TMR_TickCountGet() - icmpStartTick > (SYS_TMR_TickCounterFrequencyGet() * TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT) / 1000)
                {   // timeout
#if (_TCPIP_COMMAND_PING4_DEBUG != 0)
                    (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: request timeout.\r\n");
#endif  // (_TCPIP_COMMAND_PING4_DEBUG != 0)
                    cancelReq = icmpReqHandle != 0;    // cancel if there is another one ongoing
                    newReq = true;
                }
                // else wait some more
            }

            if(cancelReq)
            {
#if (_TCPIP_COMMAND_PING4_DEBUG != 0)
                if(TCPIP_ICMP_EchoRequestCancel(icmpReqHandle) != ICMP_ECHO_OK)
                {   // this should NOT happen!
                    (*pTcpipCmdDevice->pCmdApi->msg)(icmpCmdIoParam, "Ping cancel failed!!!\r\n");
                }
                else
                {
                    (*pTcpipCmdDevice->pCmdApi->msg)(icmpCmdIoParam, "Ping: request aborted by tcpip CMD: tmo!\r\n");
                }
#else
                TCPIP_ICMP_EchoRequestCancel(icmpReqHandle);
#endif  // (_TCPIP_COMMAND_PING4_DEBUG != 0)
            }

            if(!newReq)
            {   // nothing else to do
                break;
            }

            // send another request
            echoRequest.netH = icmpNetH;
            echoRequest.targetAddr = icmpTargetAddr;
            echoRequest.sequenceNumber = ++icmpSequenceNo;
            echoRequest.identifier = icmpIdentifier;
            echoRequest.pData = icmpPingBuff;
            echoRequest.dataSize = icmpPingSize;
            echoRequest.callback = CommandPingHandler;

            {
                int ix;
                uint8_t* pBuff = icmpPingBuff;
                for(ix = 0; ix < icmpPingSize; ix++)
                {
                    *pBuff++ = SYS_RANDOM_PseudoGet();
                }
            }

            echoRes = TCPIP_ICMP_EchoRequest (&echoRequest, &icmpReqHandle);

            if(echoRes >= 0 )
            {
                icmpStartTick = SYS_TMR_TickCountGet();
                icmpReqCount++;
#if (_TCPIP_COMMAND_PING4_DEBUG != 0)
                (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: sent request %d to: %s [%s]\r\n", icmpReqCount, icmpTargetHost, icmpTargetAddrStr);
#endif  // (_TCPIP_COMMAND_PING4_DEBUG != 0)
            }
            else
            {
#if (_TCPIP_COMMAND_PING4_DEBUG != 0)
                (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: failed to send request %d to: %s, error %d\r\n", icmpReqCount, icmpTargetAddrStr, echoRes);
#endif  // (_TCPIP_COMMAND_PING4_DEBUG != 0)
                killIcmp = true;
            }

            break;
#endif  // defined(_TCPIP_COMMAND_PING4)
#if defined(_TCPIP_COMMAND_PING6)
        case TCPIP_PING6_CMD_DNS_GET:
            dnsRes = TCPIP_DNS_Resolve(icmpTargetHost, TCPIP_DNS_TYPE_AAAA);
            if(dnsRes != TCPIP_DNS_RES_OK && dnsRes != TCPIP_DNS_RES_PENDING && dnsRes != TCPIP_DNS_RES_NAME_IS_IPADDRESS)
            {   // some other error
                (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping6: DNS failure for %s\r\n", icmpTargetHost);
                killIcmp = true;
                break;
            }
            icmpStartTick = SYS_TMR_TickCountGet();
            tcpipCmdStat = TCPIP_PING6_CMD_DNS_WAIT;
            // else wait some more
            break;

        case TCPIP_PING6_CMD_DNS_WAIT:
            dnsRes = TCPIP_DNS_IsNameResolved(icmpTargetHost, 0, &icmpv6TargetAddr);
            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {   // operation in progress
                break;
            }
            else if(dnsRes < 0 )
            {   // timeout or some other DNS error
                (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: DNS failure for %s\r\n", icmpTargetHost);
                killIcmp = true;
                break;
            }
            // success

            TCPIP_Helper_IPv6AddressToString(&icmpv6TargetAddr, icmpv6TargetAddrStr, sizeof(icmpv6TargetAddrStr));
            tcpipCmdStat = TCPIP_SEND_ECHO_REQUEST_IPV6;
            break;
        case TCPIP_SEND_ECHO_REQUEST_IPV6:
            if(icmpReqCount != 0 && icmpAckRecv == 0)
            {   // no reply received;
                if(SYS_TMR_TickCountGet() - icmpStartTick > (SYS_TMR_TickCounterFrequencyGet() * TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT) / 1000)
                {   // timeout
                    (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "ping6: request timeout.\r\n");
                    killIcmp = true;
                    break;
                }
                // else wait some more
            }
            if(icmpReqCount == icmpReqNo)
            {   // no more requests to send
                killIcmp = true;
                break;
            }

            // send another request
            ipv6EchoRes = TCPIP_ICMPV6_EchoRequestSend (icmpNetH, &icmpv6TargetAddr, ++icmpSequenceNo, icmpIdentifier,pingPktSize);

            if(ipv6EchoRes != 0 )
            {
                icmpStartTick = SYS_TMR_TickCountGet();
                if(icmpReqCount++ == 0)
                {
                    (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "ping6: request sent to: %s \r\n", icmpv6TargetAddrStr);
                }
            }
            else
            {
                (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "ping6: failed to send request to: %s\r\n", icmpv6TargetAddrStr);
                killIcmp = true;
            }

            break;
#endif  // defined(_TCPIP_COMMAND_PING6)

        default:
            killIcmp = true;
            break;

    }

    if(killIcmp)
    {
        _PingStop(pTcpipCmdDevice, icmpCmdIoParam);
    }

}

#endif  // defined(_TCPIP_COMMAND_PING4) || defined(_TCPIP_COMMAND_PING6)

void TCPIP_COMMAND_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
    { // regular TMO occurred

#if  defined(_TCPIP_COMMAND_PING4) || defined(_TCPIP_COMMAND_PING6)
        if(TCPIP_CMD_STAT_PING_START <= tcpipCmdStat && tcpipCmdStat <= TCPIP_CMD_STAT_PING_STOP)
        {
            TCPIPCmdPingTask();
        }
#endif  // defined(_TCPIP_COMMAND_PING4) || defined(_TCPIP_COMMAND_PING6)

#if defined(TCPIP_STACK_USE_DNS)
        if(TCPIP_CMD_STAT_DNS_START <= tcpipCmdStat && tcpipCmdStat <= TCPIP_CMD_STAT_DNS_STOP)
        {
            TCPIPCmdDnsTask();
        }
#endif  // defined(TCPIP_STACK_USE_DNS)
#if defined(_TCPIP_COMMANDS_MIIM)
        if(TCPIP_PHY_READ <= tcpipCmdStat && tcpipCmdStat <= TCPIP_PHY_DUMP)
        {
            TCPIPCmdMiimTask();
        }
#endif  // defined(_TCPIP_COMMANDS_MIIM)
    }
}




#if defined(TCPIP_STACK_USE_IPV4)
#if (TCPIP_ARP_COMMANDS != 0)
static void _CommandArp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // arp <interface> <req/query/del/list> <ipAddr>
    //
    TCPIP_NET_HANDLE netH;
    IPV4_ADDR ipAddr;
    TCPIP_ARP_RESULT  arpRes;
    TCPIP_MAC_ADDR    macAddr;
    char*       message;
    char        addrBuff[20];
    size_t      arpEntries, ix;
    TCPIP_ARP_ENTRY_QUERY arpQuery;
    
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    while(argc > 2)
    {
        netH = TCPIP_STACK_NetHandleGet(argv[1]);
        if (netH == 0)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
            return;
        }

        if (strcmp(argv[2], "list") == 0)
        {   // list the cache contents
            arpEntries = TCPIP_ARP_CacheEntriesNoGet(netH, ARP_ENTRY_TYPE_TOTAL);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "arp: %d slots in the cache\r\n", arpEntries);
            for(ix = 0; ix < arpEntries; ix++)
            {
                TCPIP_ARP_EntryQuery(netH, ix, &arpQuery);
                if(arpQuery.entryType == ARP_ENTRY_TYPE_PERMANENT || arpQuery.entryType == ARP_ENTRY_TYPE_COMPLETE)
                {
                    TCPIP_Helper_IPAddressToString(&arpQuery.entryIpAdd, addrBuff, sizeof(addrBuff));
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "arp: IPv4 address: %s", addrBuff);
                    TCPIP_Helper_MACAddressToString(&arpQuery.entryHwAdd, addrBuff, sizeof(addrBuff));
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, ", MAC Address: %s", addrBuff);
                    if(arpQuery.entryType == ARP_ENTRY_TYPE_COMPLETE)
                    {
                        (*pCmdIO->pCmdApi->msg)(cmdIoParam, ", complete\r\n");
                    }
                    else
                    {
                        (*pCmdIO->pCmdApi->msg)(cmdIoParam, ", permanent\r\n");
                    }
                }
                else if(arpQuery.entryType == ARP_ENTRY_TYPE_INCOMPLETE)
                {
                    TCPIP_Helper_IPAddressToString(&arpQuery.entryIpAdd, addrBuff, sizeof(addrBuff));
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "arp: IPv4 address: %s, queued\r\n", addrBuff);
                }
            }

            return;
        }


        if (argc < 4 || !TCPIP_Helper_StringToIPAddress(argv[3], &ipAddr))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IP address string \r\n");
            return;
        }

        if (strcmp(argv[2], "req") == 0)
        {   // request an address
            arpRes = TCPIP_ARP_EntryGet(netH, &ipAddr, &macAddr, true);
            switch(arpRes)
            {
                case ARP_RES_ENTRY_SOLVED:

                    TCPIP_Helper_MACAddressToString(&macAddr, addrBuff, sizeof(addrBuff));
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "arp: resolved - IPv4 address: %s, MAC Address: %s\r\n", argv[3], addrBuff);
                    return;

                case ARP_RES_ENTRY_QUEUED:
                    message = "arp: address already queued\r\n";
                    break;

                case ARP_RES_ENTRY_NEW:
                    message = "arp: address newly queued\r\n";
                    break;

                default:    // ARP_RES_CACHE_FULL  
                    message = "arp: queue full/error\r\n";
                    break;
            }
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, message);

            return;
        }

        if (strcmp(argv[2], "query") == 0)
        {   // query for an address
            arpRes = TCPIP_ARP_EntryGet(netH, &ipAddr, &macAddr, false);
            if(arpRes == ARP_RES_OK)
            {
                TCPIP_Helper_MACAddressToString(&macAddr, addrBuff, sizeof(addrBuff));
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "arp: IPv4 address: %s, MAC Address: %s\r\n", argv[3], addrBuff);
            }
            else
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "arp: no such entry\r\n");
            }
            return;
        }

        if (strcmp(argv[2], "del") == 0)
        {   // delete an address
            arpRes = TCPIP_ARP_EntryRemove(netH, &ipAddr);
            if(arpRes == ARP_RES_OK)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "arp: removed %s\r\n", argv[3]);
            }
            else
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "arp: no such entry\r\n");
            }
            return;
        }

        break;
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: arp interface list\r\n");
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: arp interface req/query/del ipAddr\r\n");
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: arp eth0 req 192.168.1.105 \r\n");
}
#endif  // (TCPIP_ARP_COMMANDS != 0)
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)
static int _Command_HttpInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int     httpActiveConn, httpOpenConn, connIx, chunkIx;
    TCPIP_HTTP_NET_CONN_INFO    httpInfo;
    TCPIP_HTTP_NET_CHUNK_INFO   httpChunkInfo[6];
    TCPIP_HTTP_NET_CHUNK_INFO*  pChunkInfo;
    TCPIP_HTTP_NET_STAT_INFO    httpStat;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: http info/stat/disconnect\r\n");
        return false;
    }

    httpActiveConn = TCPIP_HTTP_NET_ActiveConnectionCountGet(&httpOpenConn);

    if(strcmp(argv[1], "info") == 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP connections - active: %d, open: %d\r\n", httpActiveConn, httpOpenConn);

        for(connIx = 0; connIx < httpOpenConn; connIx++)
        {
            if(TCPIP_HTTP_NET_InfoGet(connIx, &httpInfo, httpChunkInfo, sizeof(httpChunkInfo)/sizeof(*httpChunkInfo)))
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP conn: %d status: 0x%4x, sm: 0x%4x, chunks: %d, chunk empty: %d, file empty: %d\r\n",
                       connIx, httpInfo.httpStatus, httpInfo.connStatus, httpInfo.nChunks, httpInfo.chunkPoolEmpty, httpInfo.fileBufferPoolEmpty);
                pChunkInfo = httpChunkInfo;
                for(chunkIx = 0; chunkIx < httpInfo.nChunks; chunkIx++, pChunkInfo++)
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tHTTP chunk: %d flags: 0x%4x, status: 0x%4x, fName: %s\r\n", chunkIx, pChunkInfo->flags, pChunkInfo->status, pChunkInfo->chunkFName);
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tHTTP chunk: dyn buffers: %d, var Name: %s\r\n", pChunkInfo->nDynBuffers, pChunkInfo->dynVarName);
                }
            }
            else
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP: failed to get info for conn: %d\r\n", connIx);
            }
        }
    }
    else if(strcmp(argv[1], "stat") == 0)
    {
        TCPIP_HTTP_NET_StatGet(&httpStat);
        
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP connections: %d, active: %d, open: %d\r\n", httpStat.nConns, httpStat.nActiveConns, httpStat.nOpenConns);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP pool empty: %d, max depth: %d, parse retries: %d\r\n", httpStat.dynPoolEmpty, httpStat.maxRecurseDepth, httpStat.dynParseRetry);
    }
    else if(strcmp(argv[1], "disconnect") == 0)
    {
        for(connIx = 0; connIx < httpOpenConn; connIx++)
        {
            TCPIP_HTTP_NET_CONN_HANDLE connHandle = TCPIP_HTTP_NET_ConnectionHandleGet(connIx);
            NET_PRES_SKT_HANDLE_T skt_h = TCPIP_HTTP_NET_ConnectionSocketGet(connHandle);
            NET_PRES_SocketDisconnect(skt_h);
        }

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP disconnected %d connections, active: %d\r\n", httpOpenConn, httpActiveConn);
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "HTTP: unknown parameter\r\n");
    }




    return false;
}
#if (TCPIP_HTTP_NET_SSI_PROCESS != 0)
static int _Command_SsiInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int nSSIVars, ssiEntries, ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    const char* varStr;
    const char* varName;
    TCPIP_HTTP_DYN_ARG_TYPE varType;

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ssi info\r\n");
        return false;
    }

    if(strcmp(argv[1], "info") == 0)
    {
        ssiEntries = TCPIP_HTTP_NET_SSIVariablesNumberGet(&nSSIVars);

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "SSI variable slots - active: %d, total: %d\r\n", ssiEntries, nSSIVars);

        for(ix = 0; ix < nSSIVars; ix++)
        {
            varStr = TCPIP_HTTP_NET_SSIVariableGetByIndex(ix, &varName, &varType, 0);
            if(varStr)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "SSI variable %d name: %s, value: %s, type: %d\r\n", ix, varName, varStr, varType);
            }
        }
    }

    return false;
}
#endif  // (TCPIP_HTTP_NET_SSI_PROCESS != 0)
#endif // defined(TCPIP_STACK_USE_HTTP_NET_SERVER)


#if defined(TCPIP_STACK_USE_SMTPC) && defined(TCPIP_SMTPC_USE_MAIL_COMMAND)

static void tcpipMailCallback(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, const TCPIP_SMTPC_MESSAGE_REPORT* pMailReport);
static void tcpipReplyCallback(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, TCPIP_SMTPC_MESSAGE_STATUS currStat, const char* serverReply);


static const char* tcpipMailBody = "OK, now an email from the command line using the SMTPC mail client.\r\n\
Add your own message body here.\r\n\
Do not exceed the recommended 78 characters per line.\r\n\
End of mail body for now. Bye bye.\r\n";

static TCPIP_SMTPC_ATTACH_BUFFER smtpBuffAttachTbl[] = 
{
    {
    TCPIP_SMTPC_ATTACH_TYPE_TEXT,       // type
    TCPIP_SMTPC_ENCODE_TYPE_7BIT,       // encode
    "test_attach.txt",                  // attachName
    (const uint8_t*)"This is a simple buffer attachment. It will show as a file.\r\n\
     However, it is presented to the mail client as buffer where the data is present.\r\n\
     Real files are added using a file attachment.\r\n\
     That's a short buffer for now.\r\n\
     Adjust to you preferences.\r\n",            // attachBuffer 
     0,                                 // attachSize
    },
    {
    TCPIP_SMTPC_ATTACH_TYPE_TEXT,       // type
    TCPIP_SMTPC_ENCODE_TYPE_7BIT,       // encode
    "second_attach.txt",                  // attachName
    (const uint8_t*)"This is yet another text attachment but supplied as a buffer.\r\n.Enjoy!\r\
     Some real files are attached separately.\r\n",            // attachBuffer 
     0,                                 // attachSize
    },
};

static TCPIP_SMTPC_ATTACH_FILE smtpFileAttachTbl[] = 
{
    {
    TCPIP_SMTPC_ATTACH_TYPE_TEXT,       // type
    TCPIP_SMTPC_ENCODE_TYPE_7BIT,       // encode
    "ssi.htm",                          // fileName
    },
    {
    TCPIP_SMTPC_ATTACH_TYPE_TEXT,       // type
    TCPIP_SMTPC_ENCODE_TYPE_7BIT,       // encode
    "index.htm",                        // fileName
    },
    
};

static TCPIP_SMTPC_MAIL_MESSAGE myMailTestMsg = 
{
    .from = 0,
    .to = 0,
    .sender = 0,
    .cc = 0,
    .bcc = 0,
    .date = "Thu, 21 July 2016 11:17:06 -0600",
    .subject = "my test message",
    .body = 0,
    .bodySize = 0,  
    .nBuffers = 0,
    .attachBuffers = 0,
    .nFiles = 0,
    .attachFiles = 0,
    .username = 0,
    .password = 0,
    .smtpServer = 0,
    .serverPort = 0,
    .messageFlags = 0,
    .messageCallback = tcpipMailCallback,
    .replyCallback = tcpipReplyCallback,
};

static TCPIP_SMTPC_MESSAGE_HANDLE tcpipMailHandle = 0;


static char        tcpipMailServer[20] = "";    // IPv4 mail server address string
static uint16_t    tcpipServerPort = 587;
static char        tcpipAuthUser[80 + 1] = "";
static char        tcpipAuthPass[80 + 1] = "";
static char        tcpipMailFrom[80 + 1] = "";
static char        tcpipMailTo[80 + 1] = "";
static int         tcpipTlsFlag = 0;
static bool        tcpipAuthPlain = 0;
static bool        tcpipForceAuth = 0;
static bool        tcpipHeloGreet = 0;


// returns:
//      1 for success
//      0 if already in progress
//     -1 if failure
static int tcpipSendMail(void)
{

    if(tcpipMailHandle != 0)
    {   // already ongoing
        return 0;
    }

    TCPIP_SMTPC_MESSAGE_RESULT mailRes;

    myMailTestMsg.body = (const uint8_t*)tcpipMailBody;
    myMailTestMsg.bodySize = strlen(tcpipMailBody);
    myMailTestMsg.smtpServer = tcpipMailServer;
    myMailTestMsg.serverPort = tcpipServerPort;
    myMailTestMsg.username = tcpipAuthUser;
    myMailTestMsg.password = tcpipAuthPass;
    myMailTestMsg.from = tcpipMailFrom;
    myMailTestMsg.to = tcpipMailTo;
    myMailTestMsg.messageFlags = (tcpipTlsFlag == 1) ? TCPIP_SMTPC_MAIL_FLAG_CONNECT_TLS : (tcpipTlsFlag == 2) ? TCPIP_SMTPC_MAIL_FLAG_SKIP_TLS : (tcpipTlsFlag == 3) ? TCPIP_SMTPC_MAIL_FLAG_FORCE_TLS : 0;
    if(tcpipAuthPlain)
    {
        myMailTestMsg.messageFlags |= TCPIP_SMTPC_MAIL_FLAG_AUTH_PLAIN;
    }
    if(tcpipForceAuth)
    {
        myMailTestMsg.messageFlags |= TCPIP_SMTPC_MAIL_FLAG_FORCE_AUTH;
    }
    if(tcpipHeloGreet)
    {
        myMailTestMsg.messageFlags |= TCPIP_SMTPC_MAIL_FLAG_GREET_HELO;
    }

    int nBuffs = sizeof(smtpBuffAttachTbl) / sizeof(*smtpBuffAttachTbl);
    int ix;
    TCPIP_SMTPC_ATTACH_BUFFER* pAttachBuff = smtpBuffAttachTbl;
    for(ix = 0; ix < nBuffs; ix++, pAttachBuff++)
    {
        pAttachBuff->attachSize = strlen((const char*)pAttachBuff->attachBuffer);
    }
    
    myMailTestMsg.attachBuffers = smtpBuffAttachTbl;
    myMailTestMsg.nBuffers = nBuffs;

    int nFiles = sizeof(smtpFileAttachTbl) / sizeof(*smtpFileAttachTbl);
    myMailTestMsg.nFiles = nFiles;
    myMailTestMsg.attachFiles = smtpFileAttachTbl;


    tcpipMailHandle = TCPIP_SMTPC_MailMessage(&myMailTestMsg, &mailRes);

    return tcpipMailHandle == 0 ? -1 : 1;
}



static void tcpipMailCallback(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, const TCPIP_SMTPC_MESSAGE_REPORT* pMailReport)
{
    if(pMailReport->messageRes == TCPIP_SMTPC_RES_OK)
    {
        SYS_CONSOLE_PRINT("app: Mail succeeded. Hurrah...warn: 0x%4x\r\n", pMailReport->messageWarn);
    }
    else
    {
        SYS_CONSOLE_PRINT("app: Mail failed err stat: %d, res: %d, warn: 0x%4x, retries: %d\r\n", pMailReport->errorStat, pMailReport->messageRes, pMailReport->messageWarn, pMailReport->leftRetries);
    }

    tcpipMailHandle = 0;
}

static void tcpipReplyCallback(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, TCPIP_SMTPC_MESSAGE_STATUS currStat, const char* serverReply)
{
    // Note: You could add some monitoring of the server replies during the mail transaction like:
    // SYS_CONSOLE_PRINT("app: Mail server reply - stat: %d, msg: %s\r\n", currStat, serverReply);
}

static int _CommandMail(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int currIx;    
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: mail <srv server> <port portNo>\r\n");
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail srv: %s port: %d\r\n", tcpipMailServer, tcpipServerPort);
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: mail <user uname> <pass password>\r\n");
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail user: %s pass: %s\r\n", tcpipAuthUser, tcpipAuthPass);
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: mail <from add> <to add>\r\n");
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail from: %s to: %s\r\n", tcpipMailFrom, tcpipMailTo);
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: mail <tls 0/1/2/3> <auth 0/1> <force 0/1> <helo 0/1>\r\n");
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail tls: %d auth: %d forced auth: %d helo: %d\r\n", tcpipTlsFlag, tcpipAuthPlain, tcpipForceAuth, tcpipHeloGreet);
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: mail <send> <info>\r\n");
        return false;
    }

    if(strcmp(argv[1], "send") == 0)
    {
        int mailRes = tcpipSendMail();
        if(mailRes == 0)
        {   // ongoing
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "mail send: already in progress\r\n");
        }
        else if(mailRes > 0)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "mail send: success!\r\n");
        }
        else
        {   // some error
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "mail send: failed!\r\n");
        }
        return false;  // no more parameters
    }
    else if(strcmp(argv[1], "info") == 0)
    {
        TCPIP_SMTPC_MESSAGE_QUERY mailQuery;
        TCPIP_SMTPC_MESSAGE_RESULT queryRes = TCPIP_SMTPC_MessageQuery(tcpipMailHandle, &mailQuery);
        if(queryRes == TCPIP_SMTPC_RES_OK)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail info - stat: %d, warn: 0x%4x, retries: %d, skt: %d\r\n", mailQuery.messageStat, mailQuery.messageWarn, mailQuery.messageRetries, mailQuery.messageSkt);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "mail info: failed!\r\n");
        }
        return false;  // no more parameters
    }


    currIx = 1;
    while(currIx + 1 < argc)
    { 
        char* param = argv[currIx];

        if(strcmp(param, "srv") == 0)
        {
            strcpy(tcpipMailServer, argv[currIx + 1]);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set the server to: %s\r\n", tcpipMailServer);
        }
        else if(strcmp(param, "port") == 0)
        {
            tcpipServerPort = atoi(argv[currIx + 1]); 
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set the server port to: %d\r\n", tcpipServerPort);
        }
        else if(strcmp(param, "user") == 0)
        {
            strncpy(tcpipAuthUser, argv[currIx + 1], sizeof(tcpipAuthUser) - 1);
            tcpipAuthUser[sizeof(tcpipAuthUser) - 1] = 0;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set auth user to: %s\r\n", tcpipAuthUser);
        }
        else if(strcmp(param, "pass") == 0)
        {
            strncpy(tcpipAuthPass, argv[currIx + 1], sizeof(tcpipAuthPass) - 1);
            tcpipAuthPass[sizeof(tcpipAuthPass) - 1] = 0;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set auth pass to: %s\r\n", tcpipAuthPass);
        }
        else if(strcmp(param, "from") == 0)
        {
            strncpy(tcpipMailFrom, argv[currIx + 1], sizeof(tcpipMailFrom) - 1);
            tcpipMailFrom[sizeof(tcpipMailFrom) - 1] = 0;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set mail-from to: %s\r\n", tcpipMailFrom);
        }
        else if(strcmp(param, "to") == 0)
        {
            strncpy(tcpipMailTo, argv[currIx + 1], sizeof(tcpipMailTo) - 1);
            tcpipMailTo[sizeof(tcpipMailTo) - 1] = 0;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set mail-to to: %s\r\n", tcpipMailTo);
        }
        else if(strcmp(param, "tls") == 0)
        {
            tcpipTlsFlag = atoi(argv[currIx + 1]);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: TLS flag set to: %s\r\n", (tcpipTlsFlag == 1) ? "conn" : (tcpipTlsFlag == 2) ? "skip" : (tcpipTlsFlag == 3) ? "force" : "none");
        }
        else if(strcmp(param, "auth") == 0)
        {
            tcpipAuthPlain = atoi(argv[currIx + 1]);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Authentication set to: %s\r\n", tcpipAuthPlain ? "plain" : "login");
        }
        else if(strcmp(param, "force") == 0)
        {
            tcpipForceAuth = atoi(argv[currIx + 1]);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Forced Auth set to: %d\r\n", tcpipForceAuth);
        }
        else if(strcmp(param, "helo") == 0)
        {
            tcpipHeloGreet = atoi(argv[currIx + 1]);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: HELO greet set to: %d\r\n", tcpipHeloGreet);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "mail: Unknown option\r\n");
        }

        currIx += 2;
    }



    return false;
}

#endif  // defined(TCPIP_STACK_USE_SMTPC) && defined(TCPIP_SMTPC_USE_MAIL_COMMAND)

#if defined(_TCPIP_COMMANDS_MIIM)
static int  _CommandMiim(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int rIx, currIx;    
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <add a> - Sets the PHY address\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <start r> <end r> - Sets the start and end register for a dump op\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <wdata x> - Sets the data for a write op\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <read rix> - Reads register rix\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <write rix> - Writes register rix with data\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <dump> - Dumps all registers [rStart, rEnd]\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <setup> - Performs the PHY setup\r\n");
        return false;
    }

    // no parameter commands 

    if(strcmp(argv[1], "dump") == 0)
    {   // perform dump
        _CommandMiimOp(0, pCmdIO, TCPIP_PHY_DUMP);
        return false;  // no more parameters
    }

    if(strcmp(argv[1], "setup") == 0)
    {   // perform setup
        _CommandMiimSetup(pCmdIO, cmdIoParam);
        return false;  // no more parameters
    }

    // parameter commands 
    currIx = 1;
    while(currIx + 1 < argc)
    { 
        char* param = argv[currIx];
        char* paramVal = argv[currIx + 1];

        if(strcmp(argv[1], "read") == 0)
        {   // read operation
            rIx = atoi(paramVal);
            _CommandMiimOp(rIx, pCmdIO, TCPIP_PHY_READ);
            return false;  // no more parameters
        }

        if(strcmp(argv[1], "write") == 0)
        {   // read operation
            rIx = atoi(paramVal);
            _CommandMiimOp(rIx, pCmdIO, TCPIP_PHY_WRITE);
            return false;  // no more parameters
        }

        if(strcmp(param, "add") == 0)
        {
            miimAdd = atoi(paramVal);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim: Set Add to: %d\r\n", miimAdd);
        }
        else if(strcmp(param, "start") == 0)
        {
            miimRegStart = atoi(paramVal);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim: Set Start Reg to: %d\r\n", miimRegStart);
        }
        else if(strcmp(param, "end") == 0)
        {
            miimRegEnd = atoi(paramVal);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim: Set End Reg to: %d\r\n", miimRegEnd);
        }
        else if(strcmp(param, "wdata") == 0)
        {
            miimWrData = (uint16_t)atoi(paramVal);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim: Set Wr Data to: 0x%4x\r\n", miimWrData);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "miim: Unknown option\r\n");
        }

        currIx += 2;
    }



    return false;
}

static void _CommandMiimOp(unsigned int rIx, SYS_CMD_DEVICE_NODE* pCmdIO, TCPIP_COMMANDS_STAT miimCmd)
{
    DRV_MIIM_RESULT miimRes;
    DRV_MIIM_OPERATION_HANDLE opHandle;
    const char* opMsg = "read"; 
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE || miimOpHandle != 0 || miimHandle != 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "miim: Another operation ongoing. Retry!\r\n");
        return;
    }

    if(_MiimOpen(pCmdIO, cmdIoParam) == 0)
    {
        return;
    }

    miimRegIx = rIx;
    if(miimCmd == TCPIP_PHY_READ)
    {
        opMsg = "read"; 
        opHandle = miimObj->DRV_MIIM_Read(miimHandle, rIx, miimAdd, DRV_MIIM_OPERATION_FLAG_NONE, &miimRes);
    }
    else if(miimCmd == TCPIP_PHY_WRITE)
    {
        opMsg = "write"; 
        opHandle = miimObj->DRV_MIIM_Write(miimHandle, rIx, miimAdd, miimWrData, DRV_MIIM_OPERATION_FLAG_NONE, &miimRes);
    }
    else
    {
        opMsg = "dump"; 
        opHandle = miimObj->DRV_MIIM_Read(miimHandle, (miimRegIx = miimRegStart), miimAdd, DRV_MIIM_OPERATION_FLAG_NONE, &miimRes);
    }

    if(opHandle != 0)
    {   // operation started
        miimOpHandle = opHandle;
        tcpipCmdStat = miimCmd;
        pTcpipCmdDevice = pCmdIO;
        miimCmdIoParam = cmdIoParam; 
        _TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, TCPIP_MIIM_COMMAND_TASK_RATE);
    }
    else
    {
        _MiimClose(true);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim %s: an error occurred: %d!\r\n", opMsg, miimRes);
    }
        
}

static void _CommandMiimSetup(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam)
{
    DRV_MIIM_SETUP miimSetup;
    DRV_MIIM_RESULT res;

    if(miimHandle != 0 || miimOpHandle != 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "miim: Another operation ongoing. Retry!\r\n");
        return;
    }

    if(_MiimOpen(pCmdIO, cmdIoParam) == 0)
    {
        return;
    }

    miimSetup.hostClockFreq = (uint32_t)TCPIP_INTMAC_PERIPHERAL_CLK;
    miimSetup.maxBusFreq = 2000000;
    miimSetup.setupFlags = 0;

    
    res = miimObj->DRV_MIIM_Setup(miimHandle, &miimSetup);

    if(res < 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim setup failed: %d!\r\n", res);
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim setup success:%d\r\n", res);
    }
    _MiimClose(false);
}

static DRV_HANDLE _MiimOpen(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam)
{
    DRV_HANDLE hMiim = miimObj->DRV_MIIM_Open(miimObjIx, DRV_IO_INTENT_SHARED);
    if(hMiim == DRV_HANDLE_INVALID || hMiim == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "miim open: failed!\r\n");
        hMiim = 0;
    }

    return (miimHandle = hMiim);
}

static void _MiimClose(bool idleState)
{
    miimObj->DRV_MIIM_Close(miimHandle);
    miimHandle = 0;
    miimOpHandle = 0;
    if(idleState)
    {
        _TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, 0);
        tcpipCmdStat = TCPIP_CMD_STAT_IDLE; 
    }
}

// MIIM commands state machine
static void TCPIPCmdMiimTask(void)
{
    DRV_MIIM_RESULT  opRes;
    const char* opMsg;
    bool    opContinue;
    uint16_t opData;
    
    opRes = miimObj->DRV_MIIM_OperationResult(miimHandle, miimOpHandle, &opData);

    if(opRes == DRV_MIIM_RES_PENDING)
    {   // ongoing...
        return;
    }

    opContinue = false;
    opMsg = tcpipCmdStat == TCPIP_PHY_READ ? "Read Reg" : tcpipCmdStat == TCPIP_PHY_WRITE ? "Write Reg" : "Dump Reg";

    if(opRes == DRV_MIIM_RES_OK)
    {   // success
        (*pTcpipCmdDevice->pCmdApi->print)(miimCmdIoParam, "Miim %s: %d, add: %d, val: 0x%4x\r\n", opMsg, miimRegIx, miimAdd, opData);
         
        if(tcpipCmdStat == TCPIP_PHY_DUMP)
        {
            if(miimRegIx != miimRegEnd)
            {   // initiate another read
                miimOpHandle = miimObj->DRV_MIIM_Read(miimHandle, ++miimRegIx, miimAdd, DRV_MIIM_OPERATION_FLAG_NONE, &opRes);
                opContinue = true;
            }
        }
    }

    if(opRes < 0)
    {   // error occurred
        (*pTcpipCmdDevice->pCmdApi->print)(miimCmdIoParam, "Miim %s error: %d\r\n", opMsg, opRes);
    } 

    if(opRes < 0 || opContinue == false)
    {
        _MiimClose(true);
    }

}

#endif  // defined(_TCPIP_COMMANDS_MIIM)

#if (TCPIP_UDP_COMMANDS)
static int _Command_UdpInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int  sktNo, ix;
    UDP_SOCKET_INFO sktInfo;

    const void* cmdIoParam = pCmdIO->cmdIoParam;
 
    sktNo = TCPIP_UDP_SocketsNumberGet();
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "UDP sockets: %d \r\n", sktNo);
    for(ix = 0; ix < sktNo; ix++)
    {
        if(TCPIP_UDP_SocketInfoGet(ix, &sktInfo))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tsktIx: %d, addType: %d, remotePort: %d, localPort: %d, rxQueueSize: %d, txSize: %d\r\n",
                    ix, sktInfo.addressType, sktInfo.remotePort, sktInfo.localPort, sktInfo.rxQueueSize, sktInfo.txSize);
        }
    }

    return true;
}

#endif  // (TCPIP_UDP_COMMANDS)


#if (TCPIP_TCP_COMMANDS)
static void _Command_TcpInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{   // tcpinfo <n>
    int  sktNo, ix, startIx, stopIx;
    TCP_SOCKET_INFO sktInfo;

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    sktNo = TCPIP_TCP_SocketsNumberGet();
    
    if(argc > 1)
    {
        startIx = atoi(argv[1]);
        stopIx = startIx + 1;
    }
    else
    {
        startIx = 0;
        stopIx = sktNo;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "TCP sockets: %d \r\n", sktNo);
    for(ix = startIx; ix < stopIx; ix++)
    {
        if(TCPIP_TCP_SocketInfoGet(ix, &sktInfo))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tsktIx: %d, addType: %d, remotePort: %d, localPort: %d, flags: 0x%02x\r\n",
                    ix, sktInfo.addressType, sktInfo.remotePort, sktInfo.localPort, sktInfo.flags);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\trxSize: %d, txSize: %d, state: %d, rxPend: %d, txPend: %d\r\n",
                    sktInfo.rxSize, sktInfo.txSize, sktInfo.state, sktInfo.rxPending, sktInfo.txPending);
        }
    }
}

static void _Command_TcpTrace(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{   // tcptrace on/off n

    int     sktNo;
    bool    traceOn;

    const void* cmdIoParam = pCmdIO->cmdIoParam;
    
    while(argc >= 3)
    {
        sktNo = atoi(argv[2]);
        if(strcmp(argv[1], "on") == 0)
        {
            traceOn = true;
        }
        else if(strcmp(argv[1], "off") == 0)
        {
            traceOn = false;
        }
        else
        {
            break;
        }

        bool res = TCPIP_TCP_SocketTraceSet(sktNo, traceOn);

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "tcp trace %s for socket: %d %s\r\n", argv[1], sktNo, res ? "success" : "failed");
        return;
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: tcptrace on/off n\r\n");
}

#endif  // (TCPIP_TCP_COMMANDS)

#if (TCPIP_PACKET_LOG_ENABLE)
static int _Command_PktLog(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    
    if(argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: plog show <all/unack/ack/err> - Displays the log entries: unack/pending (default), ack, all or error ones\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: plog clear <all> - Clears the acknowledged log entries + persistent\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: plog reset <all> - Resets the log data + all masks\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: plog handler on/off <all> - Turns on/off the local log handler\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: plog type RX/TX/RXTX <clr> - Enables the log for RX, TX or both RX and TX packets\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: plog net and/or none/all/ifIx ifIx ... <clr> - Updates the network log mask for the interface list\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: plog persist and/or none/all/modId modId... <clr> - Updates the persist mask for the module list\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: plog module and/or none/all/modId modId... <clr> - Updates the log mask for the module list\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: plog socket and/or none/all/sktIx sktIx... or <clr> - Updates the log mask for the socket numbers\r\n");
        return false;
    }

    if(strcmp(argv[1], "show") == 0)
    {
        _CommandPktLogInfo(pCmdIO, argc, argv);
    }
    else if(strcmp(argv[1], "clear") == 0)
    {
        _CommandPktLogClear(pCmdIO, argc, argv);
    }
    else if(strcmp(argv[1], "reset") == 0)
    {
        _CommandPktLogReset(pCmdIO, argc, argv);
    }
    else if(strcmp(argv[1], "handler") == 0)
    {
        _CommandPktLogHandler(pCmdIO, argc, argv);
    }
    else if(strcmp(argv[1], "type") == 0)
    {
        _CommandPktLogType(pCmdIO, argc, argv);
    }
    else
    {
        _CommandPktLogMask(pCmdIO, argc, argv);
    }

    return false;
}


static void _CommandPktLogInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // "Usage: plog show <all/unack/ack/err>"
    int ix, jx;
    TCPIP_PKT_LOG_INFO logInfo;
    TCPIP_PKT_LOG_ENTRY logEntry;
    bool modPrint;
    const char* rxtxMsg;
    char   printBuff[20];
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    int showMask = 0;   // 0: unacknowledged/pending ones (default);
                        // 1: only error ones (i.e. done + ack < 0);
                        // 2: only the ack ones (i.e. done + ack >= 0);;
                        // 3: all (i.e. including the ack ones);
    if(argc > 2)
    {
        if(strcmp(argv[2], "unack") == 0)
        {
            showMask = 0; // log unack
        } 
        else if(strcmp(argv[2], "err") == 0)
        {
            showMask = 1; // error ones
        } 
        else if(strcmp(argv[2], "ack") == 0)
        {
            showMask = 2; // acknowledged
        }
        else if(strcmp(argv[2], "all") == 0)
        {
            showMask = 3; // all
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "plog show: Unknown parameter!\r\n");
            return;
        } 
    }

    if(!TCPIP_PKT_FlightLogGetInfo(&logInfo))
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: No packet log data available!\r\n");
        return;
    }

    strcpy(printBuff, logInfo.logType == TCPIP_PKT_LOG_TYPE_RX_ONLY ? "RX" : logInfo.logType == TCPIP_PKT_LOG_TYPE_TX_ONLY ? "TX" : "RXTX");
    if((logInfo.logType & TCPIP_PKT_LOG_TYPE_SKT_ONLY) != 0)
    {
        strcat(printBuff, "_SKT");
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog entries: %d, used: %d, persistent: %d, failed: %d\r\n", logInfo.nEntries, logInfo.nUsed, logInfo.nPersistent, logInfo.nFailed);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog persist mask: 0x%4x, module log mask: 0x%4x, log type: %s\r\n", logInfo.persistMask, logInfo.logModuleMask, printBuff);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog net log mask 0x%4x, socket log mask: 0x%8x, handler: 0x%8x\r\n\n", logInfo.netLogMask, logInfo.sktLogMask, logInfo.logHandler);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog for -%s- entries\r\n", argc > 2 ? argv[2] : "unack");

    for(ix = 0; ix < logInfo.nEntries; ix++)
    {
        if(!TCPIP_PKT_FlightLogGetEntry(ix, &logEntry))
        {
            continue;
        }

        if(showMask == 0)
        {   // show only unacknowledged ones
            if((logEntry.logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
            {   // entry done
                continue;
            }
        }
        else if(showMask == 1)
        {   // show only error ones
            if((logEntry.logFlags & TCPIP_PKT_LOG_FLAG_DONE) == 0 || logEntry.ackRes > 0)
            {   // not done or good ack
                continue;
            }
        }
        else if(showMask == 2)
        {   // show only properly acknowledged ones
            if((logEntry.logFlags & TCPIP_PKT_LOG_FLAG_DONE) == 0 || logEntry.ackRes < 0)
            {   // not done or bad ack
                continue;
            }
        }
        // else: show all

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog entry: %d\r\n", ix);
        if((logEntry.logFlags & (TCPIP_PKT_LOG_FLAG_RX | TCPIP_PKT_LOG_FLAG_TX)) == (TCPIP_PKT_LOG_FLAG_RX | TCPIP_PKT_LOG_FLAG_TX))
        {
            rxtxMsg = "RX/TX";
        }
        else if((logEntry.logFlags & TCPIP_PKT_LOG_FLAG_RX) != 0)
        {
            rxtxMsg = "RX";
        }
        else 
        {
            rxtxMsg = "TX";
        }
        TCPIP_STACK_NetAliasNameGet(logEntry.pPkt->pktIf, printBuff, sizeof(printBuff));


        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tpkt: 0x%8x, if: %s, type: %s %s\r\n", logEntry.pPkt, printBuff, rxtxMsg, (logEntry.logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) != 0 ? "p" : " ");
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\towner: %d, mac: %d, acker: %d, ackRes: %d\r\n\tmodules:\t", logEntry.pktOwner, logEntry.macId, logEntry.pktAcker, logEntry.ackRes);

        modPrint = false;
        for(jx = 1; jx <= TCPIP_MODULE_LAYER3; jx++)
        {
            if((logEntry.moduleLog & ( 1 << jx)) != 0)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s ", _CommandPktLogModuleNames[jx]);
                modPrint = true;
            }
        }
        if(modPrint)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\r\n");
        }

        if((logEntry.logFlags & TCPIP_PKT_LOG_FLAG_SKT_PARAM) != 0)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tskt: %d, lport: %d, rport: %d\r\n", logEntry.sktNo, logEntry.lclPort, logEntry.remPort);
        }

    }

}

static void _CommandPktLogClear(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // "Usage: plog clear <all>"
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool clearPersist = false;

    while(argc >= 3)
    {
        if(strcmp(argv[2], "all") == 0)
        {
            clearPersist = true;
        }
        else
        {   // unknown
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Unknown parameter\r\n");
            return;
        }

        break;
    }

    TCPIP_PKT_FlightLogClear(clearPersist);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog: Cleared the %s log\r\n", clearPersist ? "whole" : "acknowledged");
}

static void _CommandPktLogReset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // "Usage: plog reset <all>"
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool clearMasks = false;

    while(argc >= 3)
    {
        if(strcmp(argv[2], "all") == 0)
        {
            clearMasks = true;
        }
        else
        {   // unknown
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Unknown parameter\r\n");
            return;
        }

        break;
    }

    TCPIP_PKT_FlightLogReset(clearMasks);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog: Reset the %s log\r\n", clearMasks ? "whole" : "data");
}

static void _CommandPktLogHandler(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // "Usage: plog handler on/off <all>"
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc > 2)
    {
        if(strcmp(argv[2], "on") == 0)
        {
            bool logAll = false;
            if(argc > 3)
            {
                if(strcmp(argv[3], "all") == 0)
                {
                    logAll = true;
                }
            }
            TCPIP_PKT_FlightLogRegister(_CommandPktLogDefHandler, logAll);
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Registered the default handler\r\n");
            _pktHandlerCmdIo = pCmdIO;
            return;
        }
        else if(strcmp(argv[2], "off") == 0)
        {
            TCPIP_PKT_FlightLogRegister(0, false);
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Deregistered the default handler\r\n");
            _pktHandlerCmdIo = 0;
            return;
        }
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Unknown parameter\r\n");

}

static void _CommandPktLogType(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // "Usage: plog type RX/TX/RXTX <clr>"
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    while(argc >= 3)
    {
        TCPIP_PKT_LOG_TYPE logType;
        bool clrPersist = false;
        if(argc > 3)
        {
            if(strcmp(argv[3], "clr") == 0)
            {
                clrPersist = true;
            }
        }

        if(strcmp(argv[2], "RX") == 0)
        {
            logType = TCPIP_PKT_LOG_TYPE_RX_ONLY;
        }
        else if(strcmp(argv[2], "TX") == 0)
        {
            logType = TCPIP_PKT_LOG_TYPE_TX_ONLY;
        }
        else if(strcmp(argv[2], "RXTX") == 0)
        {
            logType = TCPIP_PKT_LOG_TYPE_RX_TX;
        }
        else
        {   // unknown
            break;
        }

        TCPIP_PKT_FlightLogTypeSet(logType, clrPersist);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog: Type set to %s, persist%scleared\r\n", argv[2], clrPersist ? " " : " not ");
        return;
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Unknown parameter\r\n");

}

// extracts the AND and OR masks from the argv chars
// returns true if the extraction is OK, false otherwise
// expects input in the format:
    // "plog <net and none/all/ifIx ifIx ... or none/all/ifIx ifIx.... <clr>>"
    // "plog <persist and none/all/modId modId... or none/all/modId modId... <clr> >"
    // "plog <module and none/all/modId modId... or none/all/modId modId... <clr> >"
    // "plog <socket and none/all/sktIx sktIx... or none/all/sktIx sktIx... <clr> >"

typedef enum
{
    CMD_PKT_XTRACT_FLAG_NONE        = 0x00,     // no flag set
    CMD_PKT_XTRACT_FLAG_AND         = 0x01,     // AND command
    CMD_PKT_XTRACT_FLAG_OR          = 0x02,     // OR command
    CMD_PKT_XTRACT_FLAG_CLR         = 0x04,     // CLR command

    CMD_PKT_XTRACT_FLAG_BUSY        = 0x10,     // command exists
    CMD_PKT_XTRACT_FLAG_NEEDED      = 0x20,     // command is mandatory, not optional
    CMD_PKT_XTRACT_FLAG_NEED_PARAMS = 0x40,     // command needs parameters

}CMD_PKT_XTRACT_FLAGS;

typedef struct
{
    const char* cmdName;        // identifier
    uint8_t     cmdFlags;       // a CMD_PKT_XTRACT_FLAGS value
    uint8_t     cmdParams;      // number of parameters gathered so far
    uint16_t    cmdCount;       // number of executions
    uint32_t    cmdMask;        // mask for that command
}CMD_PKT_XTRACT_OP;

static const CMD_PKT_XTRACT_OP const_xtract_op_tbl[] = 
{
    { "and",    CMD_PKT_XTRACT_FLAG_AND | CMD_PKT_XTRACT_FLAG_NEEDED | CMD_PKT_XTRACT_FLAG_NEED_PARAMS},
    { "or",     CMD_PKT_XTRACT_FLAG_OR  | CMD_PKT_XTRACT_FLAG_NEEDED | CMD_PKT_XTRACT_FLAG_NEED_PARAMS},
    { "clr",    CMD_PKT_XTRACT_FLAG_CLR },
};

static CMD_PKT_XTRACT_OP xtract_op_tbl[sizeof(const_xtract_op_tbl) / sizeof(*const_xtract_op_tbl)];

static CMD_PKT_XTRACT_RES _CommandPktExtractMasks(int argc, char** argv, uint32_t* pAndMask, uint32_t* pOrMask)
{
    int ix;
    CMD_PKT_XTRACT_OP *pXtOp, *pCurrOp, *pNewOp;
    const CMD_PKT_XTRACT_OP* pCtOp;
    CMD_PKT_XTRACT_RES xtractRes;
    uint32_t andMask, orMask;
    char argBuff[10 + 1];

    // shortest form needs 4 args 'plog oper and param'
    if(argc < 4)
    {
        return CMD_PKT_XTRACT_RES_ERR;
    }

    // init the data structures
    memset(xtract_op_tbl, 0, sizeof(xtract_op_tbl));
    pXtOp = xtract_op_tbl;
    pCtOp = const_xtract_op_tbl;
    for(ix = 0; ix < sizeof(xtract_op_tbl) / sizeof(*xtract_op_tbl); ix++, pXtOp++, pCtOp++)
    {
        pXtOp->cmdName = pCtOp->cmdName;
        pXtOp->cmdFlags = pCtOp->cmdFlags;
    }

    orMask = 0;
    andMask = 0xffffffff;

    int argIx = 2;
    argc -= 2;
    argBuff[sizeof(argBuff) - 1] = 0;
    pCurrOp = 0;
    int notOptCount = 0;

    while(argc)
    {
        if(pCurrOp == 0)
        {   // extract new command
            pXtOp = xtract_op_tbl;
            pNewOp = 0;
            for(ix = 0; ix < sizeof(xtract_op_tbl) / sizeof(*xtract_op_tbl); ix++, pXtOp++)
            {
                if(strcmp(argv[argIx], pXtOp->cmdName) == 0)
                {   // found command
                    pNewOp = pXtOp;
                    break;
                }
            }

            if(pNewOp == 0)
            {   // no such command ?
                return CMD_PKT_XTRACT_RES_ERR;
            }

            // set the new command
            pCurrOp = pNewOp;
            pCurrOp->cmdCount++;
            pCurrOp->cmdFlags |= CMD_PKT_XTRACT_FLAG_BUSY;
            if((pCurrOp->cmdFlags & CMD_PKT_XTRACT_FLAG_NEEDED) != 0)
            {
                notOptCount++;   // got one mandatory command i.e. or/and
            }
            if((pCurrOp->cmdFlags & CMD_PKT_XTRACT_FLAG_NEED_PARAMS) == 0) 
            {   // no params; stop this op
                pCurrOp = 0;
            }
        }
        else
        {   // ongoing operation; extract parameters
            if(strcmp(argv[argIx], "none") == 0)
            {   // 'none' should be the only parameter 
                if(pCurrOp->cmdParams != 0)
                {   
                    return CMD_PKT_XTRACT_RES_ERR;
                }
                pCurrOp->cmdMask = 0;
                pCurrOp->cmdParams++;
                pCurrOp = 0;    // no params, done 
            }
            else if(strcmp(argv[argIx], "all") == 0)
            {   // 'all' should be the only parameter 
                if(pCurrOp->cmdParams != 0)
                {
                    return CMD_PKT_XTRACT_RES_ERR;
                }
                pCurrOp->cmdMask = 0xffffffff;
                pCurrOp->cmdParams++; 
                pCurrOp = 0;    // no params, done 
            }
            else
            {   // should be a number
                bool argInc = false;
                strncpy(argBuff, argv[argIx], sizeof(argBuff) - 1);
                int len = strlen(argBuff);
                if(argBuff[len - 1] == '0')
                {
                    argBuff[len - 1] += 1;
                    argInc = true;
                }

                int argInt = atoi(argBuff);
                if(argInt == 0)
                {   // not a number?; done with this operation
                    pCurrOp = 0; 
                    continue;
                }
                else
                {   // valid number
                    if(argInc)
                    {
                        argInt--;
                    }
                    pCurrOp->cmdMask |= 1 << argInt;
                    pCurrOp->cmdParams++; 
                }
            }
        }

        argIx++;
        argc--;
    }

    // we're done; collect the result
    if(notOptCount == 0)
    {   // mandatory command not found
        return CMD_PKT_XTRACT_RES_ERR;
    }

    xtractRes = CMD_PKT_XTRACT_RES_OK;
    pXtOp = xtract_op_tbl;
    for(ix = 0; ix < sizeof(xtract_op_tbl) / sizeof(*xtract_op_tbl); ix++, pXtOp++)
    {
        if((pXtOp->cmdFlags & CMD_PKT_XTRACT_FLAG_BUSY) != 0)
        {   // in use entry
            if((pXtOp->cmdFlags & CMD_PKT_XTRACT_FLAG_NEED_PARAMS) != 0 && pXtOp->cmdParams == 0)
            {   // command without parameters
                return CMD_PKT_XTRACT_RES_ERR;
            }

            if((pXtOp->cmdFlags & CMD_PKT_XTRACT_FLAG_AND) != 0)
            {
                andMask &= pXtOp->cmdMask;
            }
            else if((pXtOp->cmdFlags & CMD_PKT_XTRACT_FLAG_OR) != 0)
            {
                orMask |= pXtOp->cmdMask;
            }
            else if((pXtOp->cmdFlags & CMD_PKT_XTRACT_FLAG_CLR) != 0)
            {
                if(pXtOp->cmdCount != 0)
                {   // 'clr' was mentioned
                    xtractRes = CMD_PKT_XTRACT_RES_CLR;
                }
            }
        }
    }

    *pOrMask = orMask;
    *pAndMask = andMask;
    return xtractRes;
}

static void _CommandPktLogMask(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int logMaskOp;  // 1: net; 2: persist; 3: module; 4: socket; 0 error
    uint32_t andMask = 0, orMask = 0;
    CMD_PKT_XTRACT_RES xtRes;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    logMaskOp = 0;
    if(strcmp(argv[1], "net") == 0)
    {
        logMaskOp = 1;
    }
    else if(strcmp(argv[1], "persist") == 0)
    {
        logMaskOp = 2;
    }
    else if(strcmp(argv[1], "module") == 0)
    {
        logMaskOp = 3;
    }
    else if(strcmp(argv[1], "socket") == 0)
    {
        logMaskOp = 4;
    }

    if(logMaskOp == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Unknown command\r\n");
        return;
    }

    xtRes = _CommandPktExtractMasks(argc, argv, &andMask, &orMask);
    if(xtRes == CMD_PKT_XTRACT_RES_ERR)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Wrong command parameters\r\n");
        return;
    }

    switch(logMaskOp)
    {
        case 1:
            TCPIP_PKT_FlightLogUpdateNetMask(andMask, orMask, (xtRes & CMD_PKT_XTRACT_RES_CLR) != 0);
            break;

        case 2:
            TCPIP_PKT_FlightLogUpdatePersistMask(andMask, orMask, (xtRes & CMD_PKT_XTRACT_RES_CLR) != 0);
            break;

        case 3:
            TCPIP_PKT_FlightLogUpdateModuleMask(andMask, orMask, (xtRes & CMD_PKT_XTRACT_RES_CLR) != 0);
            break;

        default:    // 4
            TCPIP_PKT_FlightLogUpdateSocketMask(andMask, orMask, (xtRes & CMD_PKT_XTRACT_RES_CLR) != 0);
            break;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog: %s and with: 0x%4x, or with: 0x%4x, %scleared\r\n", argv[1], andMask, orMask, (xtRes & CMD_PKT_XTRACT_RES_CLR) != 0 ? " " : " not ");

}

static void _CommandPktLogDefHandler(TCPIP_STACK_MODULE moduleId, const TCPIP_PKT_LOG_ENTRY* pLogEntry)
{
    if(_pktHandlerCmdIo != 0)
    {
        const char* logType = (pLogEntry->logFlags & (TCPIP_PKT_LOG_FLAG_RX | TCPIP_PKT_LOG_FLAG_TX)) == (TCPIP_PKT_LOG_FLAG_RX | TCPIP_PKT_LOG_FLAG_TX) ? "RXTX" : (pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_RX) != 0 ? "RX" : "TX";
        (*_pktHandlerCmdIo->pCmdApi->print)( _pktHandlerCmdIo->cmdIoParam, "logger - module : %d, pkt: 0x%8x %s\r\n", moduleId, pLogEntry->pPkt, logType);
    }
}


#endif  // (TCPIP_PACKET_LOG_ENABLE)


#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
static int _Command_PktInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int  ix;
    TCPIP_PKT_TRACE_ENTRY tEntry;
    TCPIP_PKT_TRACE_INFO  tInfo;

    const void* cmdIoParam = pCmdIO->cmdIoParam;


    if(!TCPIP_PKT_TraceGetEntriesNo(&tInfo))
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No packet info available\r\n");
        return true;
    }


    (*pCmdIO->pCmdApi->print)(cmdIoParam, "PKT trace slots: %d, used: %d, fails: %d, ackErr: %d, ownerFail: %d\r\n", tInfo.nEntries, tInfo.nUsed, tInfo.traceFails, tInfo.traceAckErrors, tInfo.traceAckOwnerFails);

    for(ix = 0; ix < tInfo.nEntries; ix++)
    {
        if(TCPIP_PKT_TraceGetEntry(ix, &tEntry))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tmodId: %4d, totAlloc: %5d, currAlloc: %4d, currSize: %4d, totFailed: %4d, nAcks: %4d\r\n",
                    tEntry.moduleId, tEntry.totAllocated, tEntry.currAllocated, tEntry.currSize, tEntry.totFailed, tEntry.nAcks);
        }
    }

    return true;
}
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
static int _Command_HeapList(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{

    TCPIP_STACK_HEAP_HANDLE heapH;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    int ix;
    TCPIP_HEAP_POOL_ENTRY_LIST entryList;

    heapH = TCPIP_STACK_HeapHandleGet(TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL, 0);
    if(heapH == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No pool heap exists!\r\n");
        return false;
    }

    int nEntries = TCPIP_HEAP_POOL_Entries(heapH);

    if(nEntries == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No entries in this pool heap!\r\n");
        return false;
    }

    int totSize = 0;
    int totFreeSize = 0;
    int expansionSize = 0;
    for(ix = 0; ix < nEntries; ix++)
    {
        if(!TCPIP_HEAP_POOL_EntryList(heapH, ix, &entryList))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Failed to list entry: %d\r\n", ix);
        }

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Entry ix: %d, blockSize: %d, nBlocks: %d, freeBlocks: %d\r\n", ix, entryList.blockSize, entryList.nBlocks, entryList.freeBlocks);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Entry ix: %d, totEntrySize: %d, totFreeSize: %d\r\n", ix, entryList.totEntrySize, entryList.totFreeSize);

        totSize += entryList.totEntrySize;
        totFreeSize += entryList.totFreeSize;
        expansionSize = entryList.expansionSize;
    }
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Pool Heap total size: %d, total free: %d, expansion: %d\r\n", totSize, totFreeSize, expansionSize);

    return false;
}
#endif  // defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)
static int _Command_Announce(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // announce 0/1 for limited/network directed broadcast

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: announce 0/1\r\n");
        return false;
    }
    
    int param = atoi(argv[1]);
    TCPIP_ANNOUNCE_BROADCAST_TYPE annBcast;
    const char* msg;

    if(param == 0)
    {
        annBcast = TCPIP_ANNOUNCE_BCAST_NET_LIMITED;
        msg = "Limited";
    }
    else
    {
        annBcast = TCPIP_ANNOUNCE_BCAST_NET_DIRECTED;
        msg = "Directed";
    }

    TCPIP_NET_HANDLE hNet = TCPIP_STACK_IndexToNet(0);
    bool res = TCPIP_ANNOUNCE_MessageRequest(hNet, annBcast);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s bcast submitted: %d\r\n", msg, res);

    return false;
}
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)

#if defined(TCPIP_STACK_USE_FTP_CLIENT) && defined(TCPIP_FTPC_COMMANDS)

//Disable the following define to retrieve file for transmission from file system
//#define FTPC_CALLBACK_TX_PROCESSING

//Disable the following define to store file received in file system
//#define FTPC_CALLBACK_RX_PROCESSING

#ifdef FTPC_CALLBACK_TX_PROCESSING  
char transmitData[] = "This is a text file for testing";
#endif
#ifdef FTPC_CALLBACK_RX_PROCESSING  
char data_buffer[2048];
#endif

TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle;
TCPIP_FTPC_RETURN_TYPE res = TCPIP_FTPC_RET_FAILURE;
char ftpc_username[15];
char ftpc_password[15];
char ftpc_account[15];
char ftpc_src_pathname[20];
char ftpc_dst_pathname[20];
char ctrl_buffer[150];

void ctrlSktHandler(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_CTRL_EVENT_TYPE ftpcEvent,
                                            TCPIP_FTPC_CMD cmd, char * ctrlbuff, uint16_t ctrllen)
{
    
    switch (ftpcEvent)
    {
        case TCPIP_FTPC_CTRL_EVENT_SUCCESS:
            SYS_CONSOLE_MESSAGE("Command Success\r\n");
            break;
        case TCPIP_FTPC_CTRL_EVENT_FAILURE:
            SYS_CONSOLE_MESSAGE("Command Failure\r\n"); 
            break; 
        case TCPIP_FTPC_CTRL_EVENT_DISCONNECTED:
            SYS_CONSOLE_MESSAGE("FTPC Disconnected\r\n");     
            break; 
        case TCPIP_FTPC_CTRL_RCV:
            break;
        case TCPIP_FTPC_CTRL_SEND:
            break;
    }
        
    if(ctrllen)
    {
        memcpy (ctrl_buffer, ctrlbuff, ctrllen);
        ctrl_buffer[ctrllen] = '\0';
        SYS_CONSOLE_PRINT("%s\rLength = %d\r\n\n", ctrl_buffer, ctrllen); 
    }
}

//This callback function returns 'true' when Data-Socket Rx/Tx data is handled in this callback itself.
//Then, FTP Client function won't store/retrieve data to/from FileSystem.
//When it returns 'false', the FTP Client function will store/retrieve data to/from FileSystem.
bool dataSktHandler(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_DATA_EVENT_TYPE ftpcEvent,
                                            TCPIP_FTPC_CMD cmd, char * databuff, uint16_t  * datalen)
{
    static uint32_t buffCount= 0;
    bool callback_processing = false;
#ifdef FTPC_CALLBACK_TX_PROCESSING   
    static uint16_t buff_index =0;
    uint16_t len = 0;
#endif
    
    switch (ftpcEvent)
    {
        case TCPIP_FTPC_DATA_RCV:
#ifdef FTPC_CALLBACK_RX_PROCESSING  
            memcpy (data_buffer, databuff, *datalen);
            callback_processing = true;
#else    
            callback_processing = false;
#endif                    
            buffCount++;
            SYS_CONSOLE_PRINT("Rx Data Len: %d\r\n",*datalen);            
            break;
        case TCPIP_FTPC_DATA_RCV_DONE:
            SYS_CONSOLE_PRINT("Buffer Count: %d\r\n\n", buffCount);
            buffCount = 0;
            break;
        case TCPIP_FTPC_DATA_SEND_READY:     
#ifdef FTPC_CALLBACK_TX_PROCESSING
            len = strlen(&transmitData[buff_index]);
            if(*datalen <= len)
            {
                strncpy(databuff,&transmitData[buff_index], *datalen);
                buff_index += *datalen ;
                buffCount++;
            }
            else
            {
                if(len)
                {
                    strncpy(databuff,&transmitData[buff_index], len);
                    buff_index += len ;
                    *datalen = len;
                    buffCount++;
                }
                else
                {
                    *datalen = 0;
                    buff_index = 0;
                }
            }
            callback_processing = true;
#else
            buffCount++;
            callback_processing = false;
#endif
            SYS_CONSOLE_PRINT("Tx Data Len: %d\r\n",*datalen); 
            break;
        case TCPIP_FTPC_DATA_SEND_DONE:            
            SYS_CONSOLE_PRINT("Buffer Count: %d\r\n\n", buffCount);
            buffCount = 0;
            break;
    }   
    return callback_processing;
    
}

void ftpc_res_print(SYS_CMD_DEVICE_NODE* pCmdIO, TCPIP_FTPC_RETURN_TYPE res)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if(res == TCPIP_FTPC_RET_OK)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Command Started\r\n");
    }
    else if(res == TCPIP_FTPC_RET_BUSY)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Not Ready\r\n");
    }        
    else if(res == TCPIP_FTPC_RET_NOT_CONNECT)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Not Connected\r\n");
    }
    else if(res == TCPIP_FTPC_RET_NOT_LOGIN)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Not Logged In\r\n");
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Failure\r\n");
    }
}

static int _Command_FTPC_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int i;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    TCPIP_FTPC_STATUS_TYPE ftpcStatus;
    
    if(strcmp("connect",argv[1])==0)
    {
        TCPIP_FTPC_CTRL_CONN_TYPE ftpcConn;
        static IP_MULTI_ADDRESS serverIpAddr;
        static IP_ADDRESS_TYPE serverIpAddrType;
        static uint16_t    tcpipServerPort = 0;

        if ((argc < 3)||(argc > 4))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc connect <server ip address> <server port>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc connect 192.168.0.8 0 \r\n");
            return false;
        }
        
        tcpipServerPort = 0;
        if (argc == 4)
        {
            if(strcmp("0",argv[3]) != 0)
            {        
                tcpipServerPort = atoi(argv[3]);
            }            
        }    
        
        if(TCPIP_Helper_StringToIPAddress(argv[2], &serverIpAddr.v4Add))
        {
            serverIpAddrType = IP_ADDRESS_TYPE_IPV4;
        }
        else if (TCPIP_Helper_StringToIPv6Address (argv[2], &serverIpAddr.v6Add))
        {
            serverIpAddrType = IP_ADDRESS_TYPE_IPV6;
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "FTPC: Invalid Server IP address.\r\n");
            return true;
        }        
        
        
        ftpcConn.ftpcServerAddr = &serverIpAddr;
        ftpcConn.ftpcServerIpAddrType = serverIpAddrType;
        ftpcConn.serverCtrlPort = tcpipServerPort;
        
        ftpcHandle = TCPIP_FTPC_Connect(&ftpcConn, ctrlSktHandler, &res);
        if(res != TCPIP_FTPC_RET_OK)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Command Failure\r\n");
        }            
    }  
    else if(strcmp("disconnect",argv[1])==0)
    {
        if (argc != 2)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc disconnect\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc disconnect\r\n");
            return false;
        }
        
        TCPIP_FTPC_Get_Status(ftpcHandle, &ftpcStatus);
        if(ftpcStatus.isConnected)
        {
            if(TCPIP_FTPC_Disconnect(ftpcHandle) != TCPIP_FTPC_RET_OK)
                ftpc_res_print(pCmdIO,res);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Not Connected\r\n");
        }
        
    }
    else if(strcmp("login",argv[1])==0)
    {
        if ((argc < 4)||(argc > 5))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc login <username> <pswd> <account>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc login ftptest test123 0 \r\n");
            return false;
        }
        
        strcpy(ftpc_username, argv[2]);
        strcpy(ftpc_password, argv[3]);
        strcpy(ftpc_account, "0");
        if (argc == 5)
        {
            if(strcmp("0",argv[4]) != 0)
            {        
                strcpy(ftpc_account, argv[4]);
            }            
        }        
        TCPIP_FTPC_Get_Status(ftpcHandle, &ftpcStatus);
        if(ftpcStatus.isConnected)
        {
            res = TCPIP_FTPC_Login(ftpcHandle, ftpc_username, ftpc_password, ftpc_account);
            ftpc_res_print(pCmdIO,res);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Not Connected\r\n");
        }   
    }    
    else if(strcmp("pwd",argv[1])==0)
    {       
        if (argc != 2)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc pwd\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc pwd\r\n");
            return false;
        }        
        res = TCPIP_FTPC_Get_WorkingDir(ftpcHandle);
        ftpc_res_print(pCmdIO,res);
    }
    else if(strcmp("mkdir",argv[1])==0)
    {       
        if (argc != 3)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc mkdir <pathname>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc mkdir test\r\n");
            return false;
        }        
        strcpy(ftpc_src_pathname, argv[2]);        
        res = TCPIP_FTPC_MakeDir(ftpcHandle, ftpc_src_pathname);
        ftpc_res_print(pCmdIO,res);

    }
    else if(strcmp("cd",argv[1])==0)
    {       
        if (argc != 3)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc cd <pathname>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc cd test\r\n");
            return false;
        }        
        strcpy(ftpc_src_pathname, argv[2]);
        res = TCPIP_FTPC_Change_Dir(ftpcHandle, ftpc_src_pathname);
        ftpc_res_print(pCmdIO,res);
        
    }
    else if(strcmp("cdup",argv[1])==0)
    {       
        if (argc != 2)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc cdup\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc cdup \r\n");
            return false;
        }
        res = TCPIP_FTPC_ChangeToParentDir(ftpcHandle);
        ftpc_res_print(pCmdIO,res);   
    }
    else if(strcmp("quit",argv[1])==0)
    {       
        if (argc != 2)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc quit\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc quit\r\n");
            return false;
        }
        res = TCPIP_FTPC_Logout(ftpcHandle);
        ftpc_res_print(pCmdIO,res);   
    }
    else if(strcmp("rmdir",argv[1])==0)
    {       
        if (argc != 3)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc rmdir <pathname>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc rmdir test\r\n");
            return false;
        }
        
        strcpy(ftpc_src_pathname, argv[2]);
        res = TCPIP_FTPC_RemoveDir(ftpcHandle, ftpc_src_pathname);
        ftpc_res_print(pCmdIO,res);         
    }
    else if(strcmp("dele",argv[1])==0)
    {       
        if (argc != 3)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc dele <pathname>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc dele test.txt\r\n");
            return false;
        }
        
        strcpy(ftpc_src_pathname, argv[2]);
        res = TCPIP_FTPC_DeleteFile(ftpcHandle, ftpc_src_pathname);
        ftpc_res_print(pCmdIO,res);         
    }
    else if(strcmp("pasv",argv[1])==0)
    {    
        if (argc != 2)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc pasv\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc pasv\r\n");
            return false;
        }
        res = TCPIP_FTPC_SetPassiveMode(ftpcHandle);
        ftpc_res_print(pCmdIO,res);         
    }
    else if(strcmp("port",argv[1])==0)
    {    
        static TCPIP_FTPC_DATA_CONN_TYPE ftpcDataConn;
        IP_MULTI_ADDRESS dataServerIpAddr;
        
        if (argc != 4)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc port <Data socket ip address> <Data socket port>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc port 192.168.0.8 54216\r\n");
            return false;
        }
        
        if(TCPIP_Helper_StringToIPAddress(argv[2], &dataServerIpAddr.v4Add))
        {
            ftpcDataConn.dataServerIpAddrType = IP_ADDRESS_TYPE_IPV4;
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "FTPC: Invalid DataServer IP address.\r\n");
            return true;
        } 
        memcpy(&(ftpcDataConn.dataServerAddr), &(dataServerIpAddr), sizeof(IP_MULTI_ADDRESS));
           
        ftpcDataConn.dataServerPort = atoi(argv[3]);
        
        res = TCPIP_FTPC_SetActiveMode(ftpcHandle,&ftpcDataConn);
        ftpc_res_print(pCmdIO,res);         
    }
    else if(strcmp("get",argv[1])==0)
    {
        TCPIP_FTPC_DATA_CONN_TYPE ftpcDataConn;
        TCPIP_FTPC_FILE_OPT_TYPE fileOptions;
        static char serverFilename[20];
        static char clientFilename[20];
        uint8_t opt_count = 0;
        
        if ((argc < 3)||(argc > 6))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc get <-a> <-p> <server_filename><client_filename>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc get -a -p test.txt 0 \r\n");
            return false;
        }        
        ftpcDataConn.ftpcIsPassiveMode = false;  
        ftpcDataConn.ftpcDataType = TCPIP_FTPC_DATA_REP_ASCII;
        ftpcDataConn.ftpcDataTxBuffSize = 0;
        ftpcDataConn.ftpcDataRxBuffSize = 0;
        for(i = 1; i < argc; i++)
        {
            if(strcmp("-a",argv[i])==0)
            {
                ftpcDataConn.ftpcDataType = TCPIP_FTPC_DATA_REP_ASCII;
                opt_count++;
            }
            else if(strcmp("-i",argv[i])==0)
            {
                ftpcDataConn.ftpcDataType = TCPIP_FTPC_DATA_REP_IMAGE;  
                opt_count++;
            }
            else if(strcmp("-p",argv[i])==0)
            {
                ftpcDataConn.ftpcIsPassiveMode = true;
                opt_count++;
            }
        }
        
        strcpy(serverFilename, argv[opt_count + 2]);
        if (argc == (opt_count + 4))
        {
            if(strcmp("0",argv[opt_count + 3]) != 0)
            {
                strcpy(clientFilename, argv[opt_count + 3]);
                fileOptions.clientPathName = clientFilename;
            }
            else
            {
                fileOptions.clientPathName = (char *)0;
            }    
        }
        else
        {
            fileOptions.clientPathName = (char *)0;
        } 
        fileOptions.serverPathName = serverFilename;
        res = TCPIP_FTPC_GetFile(ftpcHandle, &ftpcDataConn, &fileOptions, dataSktHandler);
        ftpc_res_print(pCmdIO,res);
    }
    else if(strcmp("put",argv[1])==0)
    {
        TCPIP_FTPC_DATA_CONN_TYPE ftpcDataConn;
        TCPIP_FTPC_FILE_OPT_TYPE fileOptions;
        //TCPIP_FTPC_DATA_REP_TYPE ftpcDataType;
        static char serverFilename[20];
        static char clientFilename[20];
        uint8_t opt_count = 0;
        
        if ((argc < 3)||(argc > 7))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc put <-a> <-p> <-u> <client_filename><server_filename>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc put -a -p test.txt 0 \r\n");
            return false;
        }
        
        fileOptions.store_unique = false;
        ftpcDataConn.ftpcIsPassiveMode = false;        
        ftpcDataConn.ftpcDataType = TCPIP_FTPC_DATA_REP_ASCII;
        ftpcDataConn.ftpcDataTxBuffSize = 0;
        ftpcDataConn.ftpcDataRxBuffSize = 0;
        
        for(i = 1; i < argc; i++)
        {
            if(strcmp("-a",argv[i])==0)
            {
                ftpcDataConn.ftpcDataType = TCPIP_FTPC_DATA_REP_ASCII;
                opt_count++;
            }
            else if(strcmp("-i",argv[i])==0)
            {
                ftpcDataConn.ftpcDataType = TCPIP_FTPC_DATA_REP_IMAGE;  
                opt_count++;
            }
            else if(strcmp("-p",argv[i])==0)
            {
                ftpcDataConn.ftpcIsPassiveMode = true;
                opt_count++;
            }
            else if(strcmp("-u",argv[i])==0)
            {
                fileOptions.store_unique = true;
                opt_count++;
            }
        }
        
        strcpy(clientFilename, argv[opt_count +  2]);        
        if (argc == (opt_count + 4))
        {
            if(strcmp("0",argv[opt_count + 3]) != 0)
            {
                strcpy(serverFilename, argv[opt_count + 3]);
                fileOptions.serverPathName = serverFilename;
            }
            else
            {
                fileOptions.serverPathName = (char *)0;
            } 
        }
        else
        {
            fileOptions.serverPathName = (char *)0;
        }
        
        fileOptions.clientPathName = clientFilename;
        res = TCPIP_FTPC_PutFile(ftpcHandle,&ftpcDataConn,&fileOptions, dataSktHandler);
        ftpc_res_print(pCmdIO,res);
    } 
    else if(strcmp("type",argv[1])==0)
    {
        TCPIP_FTPC_DATA_REP_TYPE ftpcDataType;
       
        if (argc != 3)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc type <representation type : a,e,i>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc type a\r\n");
            return false;
        }
        
        if(strcmp("a",argv[2])==0)
           ftpcDataType =  TCPIP_FTPC_DATA_REP_ASCII;
        else if(strcmp("e",argv[2])==0)
            ftpcDataType =  TCPIP_FTPC_DATA_REP_EBCDIC;
        else if(strcmp("i",argv[2])==0)
            ftpcDataType =  TCPIP_FTPC_DATA_REP_IMAGE;
        else
        {
            ftpcDataType =  TCPIP_FTPC_DATA_REP_UNSUPPORTED;
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Not supported Type\r\n");
        }
        res = TCPIP_FTPC_SetType(ftpcHandle, ftpcDataType);
        ftpc_res_print(pCmdIO,res);        
    }
    else if(strcmp("stru",argv[1])==0)
    {
        TCPIP_FTPC_DATA_STRUCT_TYPE ftpcFileStruct;
       
        if (argc != 3)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc stru <file structure : f,r,p>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc stru f\r\n");
            return false;
        }
        
        if(strcmp("f",argv[2])==0)
           ftpcFileStruct =  TCPIP_FTPC_STRUCT_FILE;
        else if(strcmp("r",argv[2])==0)
            ftpcFileStruct =  TCPIP_FTPC_STRUCT_RECORD;
        else if(strcmp("p",argv[2])==0)
            ftpcFileStruct =  TCPIP_FTPC_STRUCT_PAGE;
        else
        {
            ftpcFileStruct =  TCPIP_FTPC_STRUCT_UNSUPPORTED;
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Not supported Structure\r\n");
        }
        res = TCPIP_FTPC_SetStruct(ftpcHandle, ftpcFileStruct);
        ftpc_res_print(pCmdIO,res);        
    }
    else if(strcmp("mode",argv[1])==0)
    {
        TCPIP_FTPC_TRANSFER_MODE_TYPE ftpcTranMode;
       
        if (argc != 3)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc mode <transfer mode : s,b,c>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc mode s\r\n");
            return false;
        }
        
        if(strcmp("s",argv[2])==0)
           ftpcTranMode =  TCPIP_FTPC_TRANS_STREAM_MODE;
        else if(strcmp("b",argv[2])==0)
            ftpcTranMode =  TCPIP_FTPC_TRANS_BLOCK_MODE;
        else if(strcmp("c",argv[2])==0)
            ftpcTranMode =  TCPIP_FTPC_TRANS_COMPRESS_MODE;
        else
        {
            ftpcTranMode =  TCPIP_FTPC_TRANS_UNSUPPORTED;
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Not supported mode\r\n");
        }
        res = TCPIP_FTPC_SetMode(ftpcHandle, ftpcTranMode);
        ftpc_res_print(pCmdIO,res);        
    }
    else if(strcmp("nlist",argv[1])==0)
    {
        TCPIP_FTPC_DATA_CONN_TYPE ftpcDataConn;
        TCPIP_FTPC_FILE_OPT_TYPE fileOptions;
        static char serverPathname[20];
        static char clientFilename[20];
        uint8_t opt_count = 0;

        if ((argc < 2)||(argc > 5))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc nlist -p <server_pathname><filename_to_savelist>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc nlist -p test.txt\r\n");
            return false;
        }

        ftpcDataConn.ftpcIsPassiveMode = false;
        ftpcDataConn.ftpcDataTxBuffSize = 0;
        ftpcDataConn.ftpcDataRxBuffSize = 0;
        
        for(i = 1; i < argc; i++)
        {
            if(strcmp("-p",argv[i])==0)
            {
                ftpcDataConn.ftpcIsPassiveMode = true;
                opt_count++;
            }
        }
        
        fileOptions.serverPathName = (char *)0;
        if(argc >= (opt_count + 3))
        {
            if(strcmp("0",argv[opt_count + 2]) != 0)
            {
                strcpy(serverPathname, argv[opt_count + 2]);
                fileOptions.serverPathName = serverPathname;
            }		
        }

        fileOptions.clientPathName = (char *)"name_list.txt";
        if (argc == (opt_count + 4))
        {
            if(strcmp("0",argv[opt_count + 3]) != 0)
            {
                strcpy(clientFilename, argv[opt_count + 3]);
                fileOptions.clientPathName = clientFilename;
            } 
        }
        
        res = TCPIP_FTPC_NameList(ftpcHandle, &ftpcDataConn, &fileOptions, dataSktHandler); 
        ftpc_res_print(pCmdIO,res); 
    }
    else if(strcmp("ls",argv[1])==0)
    {
        TCPIP_FTPC_DATA_CONN_TYPE ftpcDataConn;
        TCPIP_FTPC_FILE_OPT_TYPE fileOptions;
        static char serverPathname[20];        
        static char clientFilename[20];
        uint8_t opt_count = 0;

        if ((argc < 2)||(argc > 5))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc ls -p <server_pathname><filename_to_savelist>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc ls -p test list.txt\r\n");
            return false;
        }

        ftpcDataConn.ftpcIsPassiveMode = false;
        ftpcDataConn.ftpcDataTxBuffSize = 0;
        ftpcDataConn.ftpcDataRxBuffSize = 0;
        
        for(i = 1; i < argc; i++)
        {
            if(strcmp("-p",argv[i])==0)
            {
                ftpcDataConn.ftpcIsPassiveMode = true;
                opt_count++;
            }
        }

        fileOptions.serverPathName = (char *)0;
        if(argc == (opt_count + 3))
        {
            if(strcmp("0",argv[opt_count + 2]) != 0)
            {
                strcpy(serverPathname, argv[opt_count + 2]);
                fileOptions.serverPathName = serverPathname;
            }		
        }
        fileOptions.clientPathName = (char *)"list.txt";
        if (argc == (opt_count + 4))
        {
            if(strcmp("0",argv[opt_count + 3]) != 0)
            {
                strcpy(clientFilename, argv[opt_count + 3]);
                fileOptions.clientPathName = clientFilename;
            } 
        }
                
        res = TCPIP_FTPC_List(ftpcHandle, &ftpcDataConn, &fileOptions, dataSktHandler); 
        ftpc_res_print(pCmdIO,res); 
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Invalid Command\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Supported Commands are,\r\n \
        connect - Connect to FTP Server\r\n \
        disconnect - Disconnect from FTP Server\r\n \
        login - Login to FTP Server\r\n \
        pwd - Print Working Directory\r\n \
        mkdir - Create new Directory\r\n \
        rmdir - Remove Directory\r\n \
        cd - Change Directory\r\n \
        cdup - Change to root Directory\r\n \
        quit - Exits from FTP\r\n \
        get - Get file from FTP Server\r\n \
        put - Send file to FTP Server\r\n \
        dele - Delete File\r\n \
        ls - Lists files in Current Directory\r\n \
        nlist - Name of files in Current Directory\r\n \
        pasv - Enable Passive FTP session\r\n \
        port - Send port number for Active FTP session\r\n \
        type - Set file transfer type\r\n \
        stru - Set File Structure\r\n \
        mode - Set Transfer mode\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "For command specific help, enter 'ftpc <command>'\r\n");
    }
    return false;
}

#endif // defined(TCPIP_STACK_USE_FTP_CLIENT)


#if defined(TCPIP_STACK_USE_IPV4)  && defined(TCPIP_IPV4_COMMANDS) && (TCPIP_IPV4_COMMANDS != 0)
static void _CommandIpv4Arp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
static void _CommandIpv4Fwd(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _CommandIpv4Table(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)

static void _CommandIpv4(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // ip4 arp/fwd/table ...

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    bool usage = true;
    while(argc > 1)
    {
        usage = false;
        if(strcmp(argv[1], "arp") == 0)
        {
            _CommandIpv4Arp(pCmdIO, argc, argv);
        }
#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
        else if(strcmp(argv[1], "fwd") == 0)
        {
            _CommandIpv4Fwd(pCmdIO, argc, argv);
        }
        else if(strcmp(argv[1], "table") == 0)
        {
            _CommandIpv4Table(pCmdIO, argc, argv);
        }
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)
        else
        {
            usage = true;
        }

        break;
    }

    if(usage)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ip4 arp/fwd/table ix clr\r\n");
    }
}

static void _CommandIpv4Arp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // ip4 arp clr

    TCPIP_IPV4_ARP_QUEUE_STAT arpStat;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool statClear = false;

    if(argc > 2)
    {
        if(strcmp(argv[2], "clr") == 0)
        {
            statClear = true;
        }
    }

    bool res = TCPIP_IPv4_ArpStatGet(&arpStat, statClear);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 ARP Stat: %s\r\n", res ? "success" : "Failed");
    if(res)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "pool: %d, pend: %d, txSubmit: %d, fwdSubmit: %d\r\n", arpStat.nPool, arpStat.nPend, arpStat.txSubmit, arpStat.fwdSubmit);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "txSolved: %d, fwdSolved: %d, totSolved: %d, totFailed: %d\r\n", arpStat.txSolved, arpStat.fwdSolved, arpStat.totSolved, arpStat.totFailed);
    }
}

#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
static void _CommandIpv4Fwd(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // ip fwd ix clr
    
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    unsigned int index = 0;
    bool clear = false;

    if(argc > 2)
    {
        index = atoi(argv[2]);
    }

    if(argc > 3)
    {
        if(strcmp(argv[3], "clr") == 0)
        {
            clear = true;
        }
    }

    TCPIP_IPV4_FORWARD_STAT fwdStat;
    bool statRes = TCPIP_IPv4_ForwardStatGet(index, &fwdStat, clear);

    if(statRes != true)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 Fwd Stat on if: %d Failed\r\n", index);
        return;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 Fwd Stat on if: %d\r\n", index);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Failures: no route: %d, net down %d, MAC dest: %d\r\n", fwdStat.failNoRoute, fwdStat.failNetDown, fwdStat.failMacDest);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Failures: MTU: %d, ARP queue: %d, Fwd Queue: %d, MAC: %d\r\n", fwdStat.failMtu, fwdStat.failArpQueue, fwdStat.failFwdQueue, fwdStat.failMac);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Counters: ARP queued: %d, Unicast Pkts: %d, Bcast Pkts: %d\r\n", fwdStat.arpQueued, fwdStat.ucastPackets, fwdStat.bcastPackets);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Counters: Mcast Pkts: %d, tot Fwd Pkts: %d, Queued pkts: %d, to MAC pkts: %d\r\n", fwdStat.mcastPackets, fwdStat.fwdPackets, fwdStat.fwdQueuedPackets, fwdStat.macPackets);
}

static void _CommandIpv4Table(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // ip table index
    size_t ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    TCPIP_IPV4_FORWARD_ENTRY_BIN fwdEntry;
    unsigned int index = 0;

    if(argc > 2)
    {
        index = atoi(argv[2]);
    }

    TCPIP_NET_HANDLE netH = TCPIP_STACK_IndexToNet(index);
    if(netH == 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "table - no such iface: %d\r\n", index);
        return;
    }

    size_t tableEntries = TCPIP_IPV4_ForwadTableSizeGet(netH);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 Fwd Table for iface: %d, size: %d\r\n", index, tableEntries);

    for(ix = 0; ix < tableEntries; ix++)
    {
        TCPIP_IPV4_ForwadTableEntryGet(netH, ix, &fwdEntry);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 Fwd Entry: %d\r\n", ix);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tnetAdd: 0x%08x, netMask: 0x%08x, gwAdd: 0x%08x\r\n", fwdEntry.netAddress, fwdEntry.netMask, fwdEntry.gwAddress);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\toutIfIx: %d, inIfIx: %d, metric: %d\r\n", fwdEntry.outIfIx, fwdEntry.inIfIx, fwdEntry.metric);
    }

}
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)


#endif  // defined(TCPIP_STACK_USE_IPV4)  && defined(TCPIP_IPV4_COMMANDS) && (TCPIP_IPV4_COMMANDS != 0)

#if (TCPIP_PKT_ALLOC_COMMANDS != 0)
TCPIP_MAC_PACKET* pktList[10] = {0};

#define TCPIP_MAC_SEGMENT_GAP_TEST 0
#if (TCPIP_MAC_SEGMENT_GAP_TEST != 0)
extern uint32_t    _tcpip_mac_segment_gap;
#endif  // (TCPIP_MAC_SEGMENT_GAP_TEST != 0)

static void _CommandPacket(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // pkt alloc pktLen segLoadLen
    // pkt free pktIx
    // pkt show pktIx
    // pkt gap gapSz
    // pkt list

    int ix;
    TCPIP_MAC_PACKET* pkt;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    while(argc >= 2)
    {

        if(strcmp(argv[1], "list") == 0)
        {
            int nPkts = 0;
            TCPIP_MAC_PACKET** ppPkt = pktList;
            for(ix = 0; ix < sizeof(pktList) / sizeof(*pktList); ix++, ppPkt++)
            {
                if((pkt = *ppPkt) != 0)
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "packet: 0x%08x, ix: %d\r\n", pkt, ix);
                    nPkts++;
                } 
            }
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "allocated packets: %d\r\n", nPkts);
            return;
        }

#if (TCPIP_MAC_SEGMENT_GAP_TEST != 0)
        if(strcmp(argv[1], "gap") == 0)
        {
            if(argc >= 3)
            {
                _tcpip_mac_segment_gap = atoi(argv[2]);
            }

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "MAC segment gap: %d\r\n", _tcpip_mac_segment_gap);
            return;
        }
#endif  // (TCPIP_MAC_SEGMENT_GAP_TEST != 0)

        if(argc < 3)
        {
            break;
        }

        if(strcmp(argv[1], "free") == 0)
        {
            ix = atoi(argv[2]);
            if(ix < sizeof(pktList) / sizeof(*pktList))
            {
                pkt = pktList[ix];
                if(pkt != 0)
                {
                    TCPIP_PKT_PacketFree(pkt);
                    pktList[ix] = 0;
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "freed packet: 0x%08x, index: %d\r\n", pkt, ix);
                    return;
                }
            }
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "free: no such packet index: %d\r\n", ix);
            return;
        }

        if(strcmp(argv[1], "show") == 0)
        {
            ix = atoi(argv[2]);
            if(ix < sizeof(pktList) / sizeof(*pktList))
            {
                pkt = pktList[ix];
                if(pkt != 0)
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "showing packet: 0x%08x, index: %d\r\n", pkt, ix);
                    TCPIP_MAC_DATA_SEGMENT* pSeg = pkt->pDSeg;
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pDSeg: 0x%08x, segLoad: 0x%08x\r\n", pSeg, pSeg->segLoad);
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "segLen: %d, segSize: %d, segLoadOffset: %d, segAllocSize: %d\r\n", pSeg->segLen, pSeg->segSize, pSeg->segLoadOffset, pSeg->segAllocSize);
                    return;
                }
            }
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "show: no such packet index: %d\r\n", ix);
            return;
        }


        if(strcmp(argv[1], "alloc") == 0)
        {
            if(argc < 4)
            {
                break;
            }

            uint16_t segLoadLen = atoi(argv[3]);
            uint16_t pktLen = atoi(argv[2]);

            pkt = TCPIP_PKT_PacketAlloc(pktLen, segLoadLen, 0);
            if(pkt == 0)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Failed to allocate packet!\r\n");
                return;
            }

            // find a spot
            TCPIP_MAC_PACKET** ppPkt = pktList;
            for(ix = 0; ix < sizeof(pktList) / sizeof(*pktList); ix++, ppPkt++)
            {
                if(*ppPkt == 0)
                {
                    *ppPkt = pkt;
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "packet: 0x%08x, added to ix: %d\r\n", pkt, ix);
                    return;
                } 
            }
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "no place for packet: 0x%08x, free some slots first\r\n", pkt);
            return;
        }


        break;
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: pkt alloc pktLen segLoadLen\r\n");
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: pkt free/show pktIx\r\n");
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: pkt list\r\n");
}
#endif  // (TCPIP_PKT_ALLOC_COMMANDS != 0)

#if defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)

static void _CommandBridgeShowStats(SYS_CMD_DEVICE_NODE* pCmdIO, TCPIP_MAC_BRIDGE_HANDLE brH, bool clearStat)
{
    int ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    TCPIP_MAC_BRIDGE_STAT stat;
    TCPIP_MAC_BRIDGE_PORT_STAT* pPort;
    bool res = TCPIP_MAC_Bridge_StatisticsGet(brH, &stat, clearStat);

    if(res == false)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "failed to get stats!%d\r\n");
        return;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "stats returned: %d\r\n", res);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t failPktAlloc: %d, failDcptAlloc: %d, failLocks: %d, fdbFull: %d\r\n", stat.failPktAlloc, stat.failDcptAlloc, stat.failLocks, stat.fdbFull);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t failMac: %d, failMtu: %d, failSize: %d\r\n", stat.failMac, stat.failMtu, stat.failSize);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t allocPackets: %d, freedPackets: %d, ackPackets: %d, delayPackets: %d\r\n", stat.allocPackets, stat.freedPackets, stat.ackPackets, stat.delayPackets);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t pktPoolSize: %d, pktPoolLowSize: %d, pktPoolEmpty: %d\r\n", stat.pktPoolSize, stat.pktPoolLowSize, stat.pktPoolEmpty);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t dcptPoolSize: %d, dcptPoolLowSize: %d, dcptPoolEmpty: %d\r\n", stat.dcptPoolSize, stat.dcptPoolLowSize, stat.dcptPoolEmpty);

    pPort = stat.portStat;
    for(ix = 0; ix < TCPIP_MAC_BRIDGE_MAX_PORTS_NO; ix++, pPort++)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t port %d stats:\r\n", ix);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t pkts received: %d, dest me-ucast: %d, dest notme-ucast: %d, dest mcast: %d\r\n", pPort->rxPackets, pPort->rxDestMeUcast, pPort->rxDestNotMeUcast, pPort->rxDestMcast);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t pkts reserved: %d, fwd ucast: %d, fwd mcast: %d, fwd direct: %d\r\n", pPort->reservedPackets, pPort->fwdUcastPackets, pPort->fwdMcastPackets, pPort->fwdDirectPackets);
    }
}

static void _CommandBridgeShowFDB(SYS_CMD_DEVICE_NODE* pCmdIO, TCPIP_MAC_BRIDGE_HANDLE brH)
{
    // list the FDB
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    int nEntries = TCPIP_MAC_Bridge_FDBEntries(brH);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "FDB entries: %d\r\n", nEntries);

    TCPIP_MAC_FDB_ENTRY fdbEntry;
    int ix;
    for(ix = 0; ix < nEntries; ix++)
    {
        if(TCPIP_MAC_Bridge_FDBIndexRead(brH, ix, &fdbEntry) == TCPIP_MAC_BRIDGE_RES_OK)
        {   // display it
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\n\tEntry number: %d\r\n", ix);
            char addrBuff[20];
            TCPIP_Helper_MACAddressToString(&fdbEntry.destAdd, addrBuff, sizeof(addrBuff));
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tEntry destAdd: %s\r\n", addrBuff);

            char flagsBuff[40];
            if(fdbEntry.flags == 0)
            {   // this should NOT happen
                sprintf(flagsBuff, "%s\r\n", "none");
            }
            else
            {
                int nChars = sprintf(flagsBuff, "%s", (fdbEntry.flags & TCPIP_MAC_FDB_FLAG_STATIC) != 0 ? "static" : "dynamic");
                nChars += sprintf(flagsBuff + nChars, ", %s", (fdbEntry.flags & TCPIP_MAC_FDB_FLAG_HOST) != 0 ? "host" : "ext");
                nChars += sprintf(flagsBuff + nChars, ", port %s", (fdbEntry.flags & TCPIP_MAC_FDB_FLAG_PORT_VALID) != 0 ? "valid" : "invalid");
            }

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tEntry flags: 0x%02x: %s\r\n", fdbEntry.flags, flagsBuff);

            if((fdbEntry.flags & TCPIP_MAC_FDB_FLAG_PORT_VALID) != 0) 
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tEntry learn port: %d\r\n", fdbEntry.learnPort);
            }

            if((fdbEntry.flags & TCPIP_MAC_FDB_FLAG_STATIC) != 0) 
            {   // display the outPortMap 
                int jx, kx;
                const char* controlStr[TCPIP_MAC_BRIDGE_CONTROL_TYPES] = {"def", "fwd", "filt"};
                int mapEntries = sizeof(fdbEntry.outPortMap[0]) / sizeof(*fdbEntry.outPortMap[0]); 
                uint8_t* portMap = fdbEntry.outPortMap[0]; 
                for(jx = 0; jx < mapEntries; jx++)
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tEntry outPortMap[%d]: ", jx);
                    for(kx = 0; kx < mapEntries; kx++)
                    {
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s ", controlStr[*portMap++]);
                    }
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\r\n");
                }
            }
            else
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tEntry tExpire: 0x%08x\r\n", fdbEntry.tExpire);
            }
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tEntry fwdPackets: %lu\r\n", fdbEntry.fwdPackets);
        }
    }

}

#if (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
static void _CommandBridgeResetFDB(SYS_CMD_DEVICE_NODE* pCmdIO, TCPIP_MAC_BRIDGE_HANDLE brH)
{
    // reset the FDB
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    TCPIP_MAC_BRIDGE_RESULT res = TCPIP_MAC_Bridge_FDBReset(brH);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "fdb reset %s: %d\r\n", res == TCPIP_MAC_BRIDGE_RES_OK ? "success" : "failed", res);
}


// data for
// TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBAddEntry(TCPIP_MAC_BRIDGE_HANDLE brHandle, const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPermEntry);
// 

static TCPIP_MAC_BRIDGE_CONTROL_DCPT dcptCtrl1[] =
{
    {0, TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD},
    {1, TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD},
};

static TCPIP_MAC_BRIDGE_CONTROL_DCPT dcptCtrl2[] =
{
    {0, TCPIP_MAC_BRIDGE_CONTROL_TYPE_FILTER},
    {1, TCPIP_MAC_BRIDGE_CONTROL_TYPE_FILTER},
};

static TCPIP_MAC_BRIDGE_CONTROL_ENTRY ctrlEntry[] =
{
    {
    .inIx = 0,
    .dcptMapEntries = sizeof(dcptCtrl1) / sizeof(*dcptCtrl1),
    .pDcptMap = dcptCtrl1,
    },
    {
    .inIx = 1,
    .dcptMapEntries = sizeof(dcptCtrl2) / sizeof(*dcptCtrl2),
    .pDcptMap = dcptCtrl2,
    },
};


static TCPIP_MAC_BRIDGE_PERMANENT_ENTRY permEntry = 
{
    //.destAdd = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},    // filled by the command
    .controlEntries = sizeof(ctrlEntry) / sizeof(*ctrlEntry), 
    .pControlEntry = ctrlEntry,
};


#endif  // (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)

#if (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
uint32_t bridgeEvents = 0;
TCPIP_MAC_BRIDGE_EVENT_HANDLE bridgeEventHandle = 0;


static void _CommandBridgeEventHandler(TCPIP_MAC_BRIDGE_EVENT evType, const void* param)
{
    const TCPIP_MAC_ADDR* pMacAdd;
    char addBuff[20];
    char evBuff[100];

    switch(evType)
    {
        case TCPIP_MAC_BRIDGE_EVENT_FDB_FULL:
            pMacAdd = (const TCPIP_MAC_ADDR*)param;
            TCPIP_Helper_MACAddressToString(pMacAdd, addBuff, sizeof(addBuff));
            sprintf(evBuff, "%s, address: %s\r\n", "fdb full", addBuff);
            break;

        case TCPIP_MAC_BRIDGE_EVENT_FAIL_PKT_ALLOC:
            sprintf(evBuff, "%s, packets: %u\r\n", "fail alloc", (size_t)param);
            break;

        case TCPIP_MAC_BRIDGE_EVENT_FAIL_DCPT_ALLOC:
            sprintf(evBuff, "%s, descriptors: %u\r\n", "fail alloc", (size_t)param);
            break;

        case TCPIP_MAC_BRIDGE_EVENT_FAIL_MTU:
            sprintf(evBuff, "%s, size: %u\r\n", "fail MTU", (size_t)param);
            break;

        case TCPIP_MAC_BRIDGE_EVENT_FAIL_SIZE:
            sprintf(evBuff, "%s, size: %u\r\n", "fail Size", (size_t)param);
            break;

        case TCPIP_MAC_BRIDGE_EVENT_PKT_POOL_EMPTY:
            sprintf(evBuff, "%s\r\n", "pkt pool empty");
            break;

        case TCPIP_MAC_BRIDGE_EVENT_DCPT_POOL_EMPTY:
            sprintf(evBuff, "%s\r\n", "dcpt pool empty");
            break;

        case TCPIP_MAC_BRIDGE_EVENT_FAIL_LOCK:
            sprintf(evBuff, "%s\r\n", "fail lock");
            break;

        case TCPIP_MAC_BRIDGE_EVENT_ENTRY_ADDED:
            pMacAdd = (const TCPIP_MAC_ADDR*)param;
            TCPIP_Helper_MACAddressToString(pMacAdd, addBuff, sizeof(addBuff));
            sprintf(evBuff, "%s, address: %s\r\n", "entry added", addBuff);
            break;

        case TCPIP_MAC_BRIDGE_EVENT_ENTRY_EXPIRED:
            pMacAdd = (const TCPIP_MAC_ADDR*)param;
            TCPIP_Helper_MACAddressToString(pMacAdd, addBuff, sizeof(addBuff));
            sprintf(evBuff, "%s, address: %s\r\n", "entry expired", addBuff);
            break;

        default:
            sprintf(evBuff, "unknown!\r\n");
            break;
    }

    bridgeEvents++;
    SYS_CONSOLE_PRINT("Bridge event: %s, total events: %d\r\n", evBuff, bridgeEvents);
}
#endif  // (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 



static void _CommandBridge(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // bridge stats <clr>
    // bridge status
    // bridge fdb show/reset/add/delete
    // bridge register <param>

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    TCPIP_MAC_BRIDGE_HANDLE brH = TCPIP_MAC_Bridge_Open(0);
    
    while(argc > 1)
    {

        if(strcmp(argv[1], "status") == 0)
        {
            SYS_STATUS brStat = TCPIP_MAC_Bridge_Status(brH);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "status: %d\r\n", brStat);
            return;
        }

        if(strcmp(argv[1], "stats") == 0)
        {
            bool clearStat = false;
            if(argc > 2)
            {
                if(strcmp(argv[2], "clr") == 0)
                {
                    clearStat = true;
                }
            }

            _CommandBridgeShowStats(pCmdIO, brH, clearStat);
            return;
        }

#if (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
        if(strcmp(argv[1], "register") == 0)
        {
            if(bridgeEventHandle == 0)
            {
                bridgeEventHandle = TCPIP_MAC_Bridge_EventHandlerRegister(brH, _CommandBridgeEventHandler);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "register result: 0x%08x\r\n", bridgeEventHandle);
            }
            else
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "registered already: 0x%08x\r\n", bridgeEventHandle);
            }

            return;
        }

        if(strcmp(argv[1], "deregister") == 0)
        {
            if(bridgeEventHandle != 0)
            {
                bool derRes = TCPIP_MAC_Bridge_EventHandlerDeregister(brH, bridgeEventHandle);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "de-register result: %d\r\n", derRes);
                if(derRes)
                {
                    bridgeEventHandle = 0;
                }
            }
            else
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "not registered!\r\n");
            }

            return;
        }

#endif  // (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 

        if(strcmp(argv[1], "fdb") == 0)
        {   
            if(argc > 2)
            {
                if(strcmp(argv[2], "show") == 0)
                {
                    _CommandBridgeShowFDB(pCmdIO, brH);
                    return;
                }

#if (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
                if(strcmp(argv[2], "reset") == 0)
                {
                    _CommandBridgeResetFDB(pCmdIO, brH);
                    return;
                }

                if(strcmp(argv[2], "delete") == 0 || strcmp(argv[2], "add") == 0)
                {
                    if(argc > 3)
                    {
                        if(TCPIP_Helper_StringToMACAddress(argv[3], permEntry.destAdd.v))
                        {
                            TCPIP_MAC_BRIDGE_RESULT res;
                            if(strcmp(argv[2], "delete") == 0)
                            {
                                res = TCPIP_MAC_Bridge_FDBDeleteEntry(brH, &permEntry.destAdd);
                            }
                            else
                            {
                                res = TCPIP_MAC_Bridge_FDBAddEntry(brH, &permEntry);
                            }
                            (*pCmdIO->pCmdApi->print)(cmdIoParam, "fdb %s returned: %d\r\n", argv[2], res);
                            return;
                        }
                    }
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "fdb %s needs valid MAC address\r\n", argv[2]);
                    return;
                }
#endif  // (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
            }
        }

        break;
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: bridge status\r\n");
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: bridge stats <clr>\r\n");
#if (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: bridge register <param>\r\n");
#endif  // (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
#if (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: bridge fdb show/reset/add/delete\r\n");
#else
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: bridge fdb show\r\n");
#endif  // (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
}
#endif // defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)


#endif // defined(TCPIP_STACK_COMMAND_ENABLE)


