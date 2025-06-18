/*******************************************************************************
  TCP/IP commands implementation

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    TCPIP stack commands entities. 
    Note, this module is based on system command parser
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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_COMMAND

#include "tcpip/src/tcpip_private.h"
#include "tcpip/tcpip_manager.h"

#include "system/debug/sys_debug.h"
#include "system/command/sys_command.h"

#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER) && defined(TCPIP_HTTP_NET_CONSOLE_CMD)
#include "net_pres/pres/net_pres_socketapi.h"
#define M_TCPIP_COMMANDS_HTTP_NET_SERVER 
#elif defined(TCPIP_STACK_USE_HTTP_SERVER_V2) && defined(TCPIP_HTTP_CONSOLE_CMD)
// HTTP server V2 commands
#include "net_pres/pres/net_pres_socketapi.h"
#if defined(HTTP_SERVER_V2_NET_COMPATIBILITY)
// use backward HTTP_NET compatibility
#include "tcpip/http_server_transl.h"
#define M_TCPIP_COMMANDS_HTTP_NET_SERVER 
#else
// new HTTP server commands
#define M_TCPIP_COMMANDS_HTTP_SERVER 
#endif  // defined(HTTP_SERVER_V2_NET_COMPATIBILITY)
#endif  // defined(TCPIP_STACK_USE_HTTP_NET_SERVER) && defined(TCPIP_HTTP_NET_CONSOLE_CMD)

#if defined(TCPIP_STACK_COMMAND_ENABLE)

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ICMP_CLIENT) && (TCPIP_ICMP_COMMAND_ENABLE == true)
#define M_TCPIP_COMMAND_PING4
#define M_TCPIP_COMMAND_PING4_DEBUG      0   // enable/disable extra ping debugging messages
#endif

#if defined(TCPIP_STACK_USE_IPV6) && defined(TCPIP_STACK_USE_ICMPV6_CLIENT) && defined(TCPIP_ICMPV6_CLIENT_CONSOLE_CMD) && (TCPIP_ICMPV6_CLIENT_CONSOLE_CMD != 0)
#define M_TCPIP_COMMAND_PING6
#endif

#if defined(DRV_MIIM_COMMANDS) && (DRV_MIIM_COMMANDS != 0)
#include "driver/miim/drv_miim.h"
#define M_TCPIP_COMMANDS_MIIM
#endif

#if defined(TCPIP_STACK_USE_PPP_INTERFACE) && (TCPIP_STACK_PPP_COMMANDS != 0)
#include "driver/ppp/drv_ppp_mac.h"
#include "driver/ppp/drv_ppp.h"
#include "driver/ppp/drv_hdlc_obj.h"
#define M_TCPIP_STACK_PPP_COMMANDS
#if defined(PPP_ECHO_REQUEST_ENABLE) && (PPP_ECHO_REQUEST_ENABLE != 0)
#define M_TCPIP_STACK_PPP_ECHO_COMMAND 
#endif  // defined(PPP_ECHO_REQUEST_ENABLE) && (PPP_ECHO_REQUEST_ENABLE != 0)
#endif  // defined(TCPIP_STACK_USE_PPP_INTERFACE) && (TCPIP_STACK_PPP_COMMANDS != 0)

#if defined(TCPIP_STACK_USE_PPP_INTERFACE) && (TCPIP_STACK_HDLC_COMMANDS != 0)
#define M_TCPIP_STACK_HDLC_COMMANDS
#endif  // defined(TCPIP_STACK_USE_PPP_INTERFACE) && (TCPIP_STACK_HDLC_COMMANDS != 0)

#if defined(TCPIP_STACK_USE_WS_CLIENT) && (TCPIP_WSC_COMMANDS != 0)
#define M_TCPIP_COMMANDS_WSC
#endif

#if defined(M_TCPIP_COMMAND_PING4) || defined(M_TCPIP_COMMAND_PING6) || defined(TCPIP_STACK_USE_DNS) || defined(M_TCPIP_COMMANDS_MIIM) || defined(M_TCPIP_STACK_PPP_ECHO_COMMAND) || defined(M_TCPIP_COMMANDS_WSC)
#define M_TCPIP_STACK_COMMAND_TASK
#endif // defined(M_TCPIP_COMMAND_PING4) || defined(M_TCPIP_COMMAND_PING6) || defined(TCPIP_STACK_USE_DNS) || defined(M_TCPIP_COMMANDS_MIIM) || defined(M_TCPIP_STACK_PPP_ECHO_COMMAND) || defined(M_TCPIP_COMMANDS_WSC)


#if defined(TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && (TCPIP_STACK_CONFIGURATION_SAVE_RESTORE != 0)
#define M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE
#endif

static size_t  initialNetIfs = 0U;  // Backup interfaces number for stack restart

#if defined(TCPIP_STACK_DOWN_OPERATION) && (TCPIP_STACK_DOWN_OPERATION != 0)
#define M_TCPIP_STACK_DOWN_OPERATION    1
static TCPIP_STACK_INIT        cmdTcpipInitData;        // data that's used for the StackInit
static TCPIP_STACK_INIT*       pCmdTcpipInitData = NULL;   // pointer to this data
#else
#define M_TCPIP_STACK_DOWN_OPERATION    0
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if defined(TCPIP_STACK_IF_UP_DOWN_OPERATION) && (TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
#define M_TCPIP_STACK_IF_UP_DOWN_OPERATION  1
#else
#define M_TCPIP_STACK_IF_UP_DOWN_OPERATION  0
#endif

#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((M_TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))
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

static TCPIP_COMMAND_STG_DCPT*   pCmdStgDcpt = NULL;   // store current interface configuration
static TCPIP_NETWORK_CONFIG*     pCmdNetConf = NULL;   // create the array of configurations needed for stack initialization

static bool                     tcpipCmdPreserveSavedInfo = false; // do not discard the saved data

#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((M_TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))

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


typedef bool(*addSvcFnc)(TCPIP_NET_HANDLE hNet);



static void F_Command_NetInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_DefaultInterfaceSet (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#if defined(TCPIP_STACK_USE_IPV4)
static void F_Command_AddressService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_STACK_ADDR_SRVC_TYPE svcType);
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
static void F_Command_DhcpOptions(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif 
static void F_Command_ZcllOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_DNSAddressSet4(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IPV6)
static void F_Command_DNSAddressSet6(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_STACK_USE_IPV6)
static void F_Command_IPAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_GatewayAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_BIOSNameSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_MACAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#if (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
static void F_Command_NetworkOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
#if (M_TCPIP_STACK_DOWN_OPERATION != 0)
static void F_Command_StackOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // (M_TCPIP_STACK_DOWN_OPERATION != 0)
static void F_Command_HeapInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_ARP_COMMANDS) && (TCPIP_ARP_COMMANDS != 0)
#define M_TCPIP_ARP_COMMANDS    1
static void F_Command_Arp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#else
#define M_TCPIP_ARP_COMMANDS    0
#endif  // (TCPIP_ARP_COMMANDS != 0)
#endif  // defined(TCPIP_STACK_USE_IPV4)
static void F_Command_MacInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#if defined(TCPIP_STACK_USE_TFTP_CLIENT)
static void F_Command_TFTPC_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
#if defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
static void F_Command_Dhcpv6Options(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
#if defined(TCPIP_STACK_USE_TFTP_SERVER)
static void F_Command_TFTPServerOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
#if defined(TCPIP_STACK_USE_DHCP_SERVER)
static void F_Command_DHCPSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_DHCPLeaseInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#elif defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
static void F_Command_DHCPsOptions(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static bool F_Command_DHCPsEnable(TCPIP_NET_HANDLE netH);
static bool F_Command_DHCPsDisable(TCPIP_NET_HANDLE netH);
static void F_Command_DHCPsLeaseList(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_NET_HANDLE netH);
#if defined(TCPIP_DHCPS_DYNAMIC_DB_ACCESS) && (TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0)
#define M_TCPIP_DHCPS_DYNAMIC_DB_ACCESS 1
static void F_Command_DHCPsLeaseRemove(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_NET_HANDLE netH);
#else
#define M_TCPIP_DHCPS_DYNAMIC_DB_ACCESS 0
#endif  // (TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0)
static void F_Command_DHCPsStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_NET_HANDLE netH);
#if defined(M_TCPIP_STACK_DHCPS_CONFIG_EXAMPLE)
static void F_Command_DHCPsConfigure(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_NET_HANDLE netH);
#endif // defined(M_TCPIP_STACK_DHCPS_CONFIG_EXAMPLE)
#endif  //  defined(TCPIP_STACK_USE_DHCP_SERVER)
#if defined(TCPIP_STACK_USE_DNS)
static void F_Command_DNSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_DNS_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_ShowDNSResolvedInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
#if defined(TCPIP_STACK_USE_DNS_SERVER)
static void F_Command_DNSSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_AddDelDNSSrvAddress(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv,DNS_SERVICE_COMD_TYPE dnsCommand);
static void F_Command_ShowDNSServInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_DnsServService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif

#if defined(TCPIP_STACK_USE_TFTP_CLIENT)
static char tftpServerHost[TCPIP_TFTPC_SERVERADDRESS_LEN];     // current target server
static char tftpcFileName[TCPIP_TFTPC_FILENAME_LEN]; // TFTP file name that will be for PUT and GET command
#endif

#if defined(M_TCPIP_COMMANDS_HTTP_NET_SERVER)
static void F_Command_HttpNetInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#if (TCPIP_HTTP_NET_SSI_PROCESS != 0)
static void F_Command_SsiNetInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
#elif defined(M_TCPIP_COMMANDS_HTTP_SERVER)
static void F_Command_HttpInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static size_t http_inst_ix  = 0U;        // current HTTP instance number
static size_t http_port_ix  = 0U;        // current HTTP port number
#if defined(TCPIP_HTTP_SSI_PROCESS) && (TCPIP_HTTP_SSI_PROCESS != 0)
#define M_TCPIP_HTTP_SSI_PROCESS    1
static void F_Command_SsiInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#else
#define M_TCPIP_HTTP_SSI_PROCESS    0
#endif
#endif

#if defined(TCPIP_STACK_USE_SMTPC) && defined(TCPIP_SMTPC_USE_MAIL_COMMAND)
static void F_Command_Mail(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_STACK_USE_SMTPC) && defined(TCPIP_SMTPC_USE_MAIL_COMMAND)


#if defined(TCPIP_UDP_COMMANDS) && (TCPIP_UDP_COMMANDS != 0)
#define M_TCPIP_UDP_COMMANDS 1
static void F_Command_Udp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#else
#define M_TCPIP_UDP_COMMANDS 0
#endif  // (TCPIP_UDP_COMMANDS)

#if defined(TCPIP_TCP_COMMANDS) && (TCPIP_TCP_COMMANDS != 0)
#define M_TCPIP_TCP_COMMANDS    1
static void F_Command_Tcp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_TcpTrace(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#else
#define M_TCPIP_TCP_COMMANDS    0
#endif  // (TCPIP_TCP_COMMANDS)

#if defined(TCPIP_PACKET_LOG_ENABLE) && (TCPIP_PACKET_LOG_ENABLE != 0)
#define M_TCPIP_PACKET_LOG_ENABLE   1
static void F_Command_PktLog(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_PktLogInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_PktLogClear(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_PktLogReset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_PktLogHandler(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_PktLogType(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_PktLogMask(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_PktLogDefHandler(TCPIP_STACK_MODULE moduleId, const TCPIP_PKT_LOG_ENTRY* pLogEntry);

typedef enum
{
    CMD_PKT_XTRACT_RES_OK   = 0,    // all OK
    CMD_PKT_XTRACT_RES_CLR  = 1,    // all OK, 'clr' was requested


    CMD_PKT_XTRACT_RES_ERR  = -1,   // some error occurred

}CMD_PKT_XTRACT_RES;
static CMD_PKT_XTRACT_RES F_Command_PktExtractMasks(int argc, char** argv, uint32_t* pAndMask, uint32_t* pOrMask);

static SYS_CMD_DEVICE_NODE*   p_pktHandlerCmdIo = NULL;

// table with the module names for logger purposes
// only basic modules supported
static const char* T_CommandPktLogModuleNames[] = 
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


#else
#define M_TCPIP_PACKET_LOG_ENABLE   0
#endif  // (TCPIP_PACKET_LOG_ENABLE)

#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
static void F_Command_PktInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
static void F_Command_HeapList(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)
static void F_Command_Announce(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)

#if defined(M_TCPIP_STACK_COMMAND_TASK)
// command task status
typedef enum
{
    TCPIP_CMD_STAT_IDLE = 0,        // command task is idle

    // ping related status
    TCPIP_CMD_STAT_PING_START,      // starting ping commands

    TCPIP_PING_CMD_DNS_WAIT,        // wait for DNS
    TCPIP_PING_CMD_START_PING,      // start ping process
    TCPIP_PING_CMD_DO_PING,         // send pings
    TCPIP_PING6_CMD_DNS_GET,        // send pings
    TCPIP_PING6_CMD_DNS_WAIT,       // wait for DNS    
    TCPIP_SEND_ECHO_REQUEST_IPV6,   // send IPv6 ping request


    // DNS related status
    TCPIP_CMD_STAT_DNS_START,                               // starting DNS commands

    TCPIP_DNS_LOOKUP_CMD_WAIT,                              // wait for DNS

    // PHY commands
    TCPIP_PHY_READ,                 // read a PHY register command
    TCPIP_PHY_WRITE,                // write a PHY register command
    TCPIP_PHY_DUMP,                 // dump a range of PHY registers command
    TCPIP_PHY_READ_SMI,             // read an extended SMI PHY register command
    TCPIP_PHY_WRITE_SMI,            // write an extended SMI PHY register command

    // PPP echo status
    TCPIP_CMD_STAT_PPP_START,       // ppp echo start
    TCPIP_PPP_CMD_DO_ECHO,          // do the job

    // WSC task status
    TCPIP_CMD_STAT_WSC_OPEN,        // WSC has an open connection

}TCPIP_COMMANDS_STAT;
#define TCPIP_PING_CMD_DNS_GET TCPIP_CMD_STAT_PING_START        // get DNS
#define TCPIP_CMD_STAT_PING_STOP TCPIP_SEND_ECHO_REQUEST_IPV6   // stop ping commands
#define TCPIP_DNS_LOOKUP_CMD_GET TCPIP_CMD_STAT_DNS_START       // get DNS
#define TCPIP_CMD_STAT_DNS_STOP TCPIP_DNS_LOOKUP_CMD_WAIT       // stop DNS commands
#define TCPIP_CMD_STAT_PPP_STOP TCPIP_PPP_CMD_DO_ECHO           // pppp echo stop

static SYS_CMD_DEVICE_NODE* pTcpipCmdDevice = NULL;
static TCPIP_SIGNAL_HANDLE     tcpipCmdSignalHandle = NULL;      // tick handle


static TCPIP_COMMANDS_STAT  tcpipCmdStat = TCPIP_CMD_STAT_IDLE;

#endif  // defined(M_TCPIP_STACK_COMMAND_TASK)

static int commandInitCount = 0;        // initialization count
static uint32_t sysFreq = 0U;

#if defined(TCPIP_STACK_USE_DNS)
static char                 dnslookupTargetHost[TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN + 1];     // current target host name
static TCPIP_DNS_RESOLVE_TYPE     dnsType=TCPIP_DNS_TYPE_A;
static const void*          dnsLookupCmdIoParam = NULL;
static uint32_t             dnsLookUpStartTick;

static void                 F_Command_DNSLookUP(SYS_CMD_DEVICE_NODE* pCmdIO, char** argv);

static void                 TCPIPCmdDnsTask(void);
#endif

#if defined(M_TCPIP_COMMAND_PING4)

static void                 F_Command_Ping(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static void                 CommandPingHandler(const  TCPIP_ICMP_ECHO_REQUEST* pEchoReq, TCPIP_ICMP_REQUEST_HANDLE iHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result, const void* param);

static void                 TCPIPCmdPingTask(void);

static void                 F_PingStop(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam);

static IPV4_ADDR            icmpTargetAddr;         // current target address
static uint8_t              icmpPingBuff[TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE];
static uint16_t             icmpPingSize = TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE;
static TCPIP_ICMP_REQUEST_HANDLE icmpReqHandle;     // current transaction handle
#endif  // defined(M_TCPIP_COMMAND_PING4)

#if defined(M_TCPIP_COMMAND_PING6)
static void                 F_Command_IPv6_Ping(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void                 CommandPing6Handler(TCPIP_NET_HANDLE hNetIf,uint8_t type, const IPV6_ADDR * localIP,
                                                                    const IPV6_ADDR * remoteIP, void * data);
static char                 icmpv6TargetAddrStr[42];
static uint32_t             pingPktSize = 0U;
static IPV6_ADDR            icmpv6TargetAddr;
static ICMPV6_HANDLE        hIcmpv6 = NULL;
#endif  // defined(M_TCPIP_COMMAND_PING6)


#if defined(M_TCPIP_COMMAND_PING4) || defined(M_TCPIP_COMMAND_PING6)

#define TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY 5  // minimum delay between successive echo requests

static char                 icmpTargetHost[31];     // current target host name
static char                 icmpTargetAddrStr[16 + 1]; // current target address string
static uint16_t             icmpSequenceNo;         // current sequence number
static uint16_t             icmpIdentifier;         // current ID number

static const void*          icmpCmdIoParam = NULL;
static uint32_t             icmpReqNo;              // number of requests to send
static size_t               icmpReqCount;           // current request counter
static size_t               icmpAckRecv;            // number of acks
static uint32_t             icmpReqDelay;

static uint32_t             icmpStartTick;
static TCPIP_NET_HANDLE     icmpNetH = NULL;
#endif  // defined(M_TCPIP_COMMAND_PING4) || defined(M_TCPIP_COMMAND_PING6)

#if defined(M_TCPIP_COMMANDS_MIIM)
static void     TCPIPCmdMiimTask(void);
static void     F_Command_Miim(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void     F_Command_MiimOp(SYS_CMD_DEVICE_NODE* pCmdIO, uint16_t rIx, uint32_t wData, TCPIP_COMMANDS_STAT miimCmd);
static void     F_Command_MiimSetup(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam);
static DRV_HANDLE F_MiimOpen(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam);
static void     F_MiimClose(bool idleState);

static const DRV_MIIM_OBJECT_BASE*  miimObj = NULL;    // MIIM object associated with the PIC32INT MAC driver
static SYS_MODULE_INDEX             miimObjIx = 0;  // current MIIM object index

static DRV_HANDLE           miimHandle = 0U; // handle to the MIIM driver
static DRV_MIIM_OPERATION_HANDLE miimOpHandle = NULL;  // current operation
static uint16_t             miimRegStart = 0U; // start for a dump
static uint16_t             miimRegEnd = 0U;   // end for a dump
static uint16_t             miimRegIx = 0U;    // current Reg index to read
static uint16_t             miimAdd = 0U;    // PHY address
static uint32_t             miimNetIx = 0U;    // Network Interface Number

static const void*          miimCmdIoParam = NULL;

static const char*          miiOpName_Tbl[] = 
{
    "read",         // TCPIP_PHY_READ
    "write",        // TCPIP_PHY_WRITE
    "dump",         // TCPIP_PHY_DUMP
    "read_smi",     // TCPIP_PHY_READ_SMI
    "write_smi",    // TCPIP_PHY_WRITE_SMI
};

#define         TCPIP_MIIM_COMMAND_TASK_RATE  100   // milliseconds
#endif  // defined(M_TCPIP_COMMANDS_MIIM)

#if defined(M_TCPIP_STACK_PPP_ECHO_COMMAND)

#define TCPIP_COMMAND_PPP_ECHO_REQUEST_MIN_DELAY 5  // minimum delay between successive echo requests

static void                 F_PPPEchoHandler(const PPP_ECHO_REQUEST* pEchoReq, PPP_REQUEST_HANDLE pppHandle, PPP_ECHO_RESULT result, const void* param);

static void                 F_PPPEchoStop(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam);

static PPP_REQUEST_HANDLE   pppReqHandle;     // current transaction handle

static uint16_t             pppSeqNo;         // current sequence number

static const void*          pppCmdIoParam = NULL;
static uint32_t             pppReqNo;              // number of requests to send
static size_t               pppReqCount;           // current request counter
static size_t               pppAckRecv;            // number of acks
static uint32_t             pppReqDelay;

static uint32_t             pppStartTick;

static uint8_t  pppEchoBuff[TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE];
static uint16_t pppEchoSize = (uint16_t)TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE;

#endif



#if defined(TCPIP_STACK_USE_FTP_CLIENT) && defined(TCPIP_FTPC_COMMANDS)
static void F_Command_FTPC_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif

#if defined(TCPIP_STACK_USE_IPV4)  && defined(TCPIP_IPV4_COMMANDS) && (TCPIP_IPV4_COMMANDS != 0)
static void F_Command_Ipv4(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif 

// internal test command. Not MHC configurable
#define M_TCPIP_PKT_ALLOC_COMMANDS    0

#if (M_TCPIP_PKT_ALLOC_COMMANDS != 0)
static void F_Command_Packet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // (M_TCPIP_PKT_ALLOC_COMMANDS != 0)

#if defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)
static void F_Command_Bridge(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif // defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)

#if defined(M_TCPIP_STACK_HDLC_COMMANDS)
static void F_Command_Hdlc(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(M_TCPIP_STACK_HDLC_COMMANDS)
#if defined(M_TCPIP_STACK_PPP_COMMANDS)
static void F_Command_Ppp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#if defined(M_TCPIP_STACK_PPP_ECHO_COMMAND)
static void TCPIPCmd_PppEchoTask(void);
#endif  // defined(M_TCPIP_STACK_PPP_ECHO_COMMAND)
#endif  // defined(M_TCPIP_STACK_PPP_COMMANDS)

#if defined(TCPIP_STACK_RUN_TIME_INIT) && (TCPIP_STACK_RUN_TIME_INIT != 0)
static void F_Command_ModDeinit(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_ModRunning(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_STACK_RUN_TIME_INIT) && (TCPIP_STACK_RUN_TIME_INIT != 0)

#if defined(TCPIP_STACK_USE_SNMPV3_SERVER)  
static void F_Command_SNMPv3USMSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif
#if defined(TCPIP_STACK_USE_SNTP_CLIENT)  
static void F_Command_Sntp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // defined(TCPIP_STACK_USE_SNTP_CLIENT)  

#if defined(M_TCPIP_COMMANDS_WSC)  
static void F_Command_WsHelp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsPreset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WscRate(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsOpen(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsClose(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsRegister(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsPing(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsPong(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsMsg(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsRxInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsRxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsRead(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsForceRead(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_WsAutoRead(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static void Wsc_CmdInitialize(void);
static void Wsc_CmdTask(void);
static const char* Wsc_SNIHostName(NET_PRES_SKT_HANDLE_T handle);
static void Wsc_PrintSettings(SYS_CMD_DEVICE_NODE* pCmdIO, char** argv);
static void Wsc_EventHandler(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_EVENT_TYPE evType, TCPIP_WSC_EV_INFO evInfo, const void* param);
static void Wsc_SendCtrlFrame(SYS_CMD_DEVICE_NODE* pCmdIO, char** argv, const char* message, TCPIP_WS_OP_CODE opCode);
static TCPIP_WSC_RES Wsc_SendMsg(TCPIP_WSC_SEND_MSG_DCPT* txDcpt);
static TCPIP_WSC_RES Wsc_ReadMsg(const void* rxHandle);
static void Wsc_CloseConn(void);


static void F_CommandWsc(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif

// TCPIP stack command table
static const SYS_CMD_DESCRIPTOR    tcpipCmdTbl[]=
{
    {"netinfo",     &F_Command_NetInfo,              ": Get network information"},
    {"defnet",      &F_Command_DefaultInterfaceSet,  ": Set/Get default interface"},
#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
    {"dhcp",        &F_Command_DhcpOptions,           ": DHCP client commands"},
#endif
    {"zcll",        &F_Command_ZcllOnOff,            ": Turn ZCLL on/off"},
    {"setdns4",     &F_Command_DNSAddressSet4,       ": Set DNS IPv4 address"},
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IPV6)
    {"setdns6",     &F_Command_DNSAddressSet6,       ": Set DNS IPv6 address"},
#endif  // defined(TCPIP_STACK_USE_IPV6)
    {"setip",       &F_Command_IPAddressSet,         ": Set IP address and mask"},
    {"setgw",       &F_Command_GatewayAddressSet,    ": Set Gateway address"},
    {"setbios",     &F_Command_BIOSNameSet,          ": Set host's NetBIOS name"},
    {"setmac",      &F_Command_MACAddressSet,        ": Set MAC address"},
#if (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
    {"if",          &F_Command_NetworkOnOff,         ": Bring an interface up/down"},
#endif  // (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
#if (M_TCPIP_STACK_DOWN_OPERATION != 0)
    {"stack",       &F_Command_StackOnOff,           ": Stack turn on/off"},
#endif  // (M_TCPIP_STACK_DOWN_OPERATION != 0)
    {"heapinfo",    &F_Command_HeapInfo,             ": Check heap status"},
#if defined(TCPIP_STACK_USE_DHCP_SERVER)
    {"dhcps",       &F_Command_DHCPSOnOff,           ": Turn DHCP server on/off"},
    {"dhcpsinfo",   &F_Command_DHCPLeaseInfo,        ": Display DHCP Server Lease Details" },
#elif defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
    {"dhcps",       &F_Command_DHCPsOptions,          ": DHCP server commands"},
#endif  //  defined(TCPIP_STACK_USE_DHCP_SERVER)
#if defined(M_TCPIP_COMMAND_PING4)
    {"ping",        &F_Command_Ping,                  ": Ping an IP address"},
#endif  // defined(M_TCPIP_COMMAND_PING4)
#if defined(M_TCPIP_COMMAND_PING6)
    {"ping6",       &F_Command_IPv6_Ping,            ": Ping an IPV6 address"},
#endif  // defined(M_TCPIP_COMMAND_PING6)
#if defined(TCPIP_STACK_USE_IPV4)
#if (M_TCPIP_ARP_COMMANDS != 0)
    {"arp",         &F_Command_Arp,                   ": ARP commands"},
#endif  // (M_TCPIP_ARP_COMMANDS != 0)
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DNS_SERVER)
    {"dnss",        &F_Command_DnsServService,       ": DNS server commands"},
#endif
#if defined(TCPIP_STACK_USE_DNS)
    {"dnsc",        &F_Command_DNS_Service,          ": DNS client commands"},
#endif
    {"macinfo",     &F_Command_MacInfo,              ": Check MAC statistics"},
#if defined(TCPIP_STACK_USE_TFTP_CLIENT)
    {"tftpc",       &F_Command_TFTPC_Service,        ": TFTP client Service"},
#endif
#if defined(TCPIP_STACK_USE_TFTP_SERVER)
    {"tftps",       &F_Command_TFTPServerOnOff,      ": TFTP Server Service"},
#endif
#if defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
    {"dhcp6",      &F_Command_Dhcpv6Options,          ": DHCPV6 client commands"},
#endif
#if defined(M_TCPIP_COMMANDS_HTTP_NET_SERVER)
    {"http",        &F_Command_HttpNetInfo,           ": HTTP information"},
#if (TCPIP_HTTP_NET_SSI_PROCESS != 0)
    {"ssi",         &F_Command_SsiNetInfo,            ": SSI information"},
#endif
#elif defined(M_TCPIP_COMMANDS_HTTP_SERVER)
    {"http",        &F_Command_HttpInfo,              ": HTTP information"},
#if (M_TCPIP_HTTP_SSI_PROCESS != 0)
    {"ssi",         &F_Command_SsiInfo,               ": SSI information"},
#endif
#endif
#if defined(TCPIP_STACK_USE_SMTPC) && defined(TCPIP_SMTPC_USE_MAIL_COMMAND)
    {"mail",        &F_Command_Mail,                  ": Send Mail Message"},
#endif  // defined(TCPIP_STACK_USE_SMTPC) && defined(TCPIP_SMTPC_USE_MAIL_COMMAND)
#if defined(M_TCPIP_COMMANDS_MIIM)
    {"miim",        &F_Command_Miim,                  ": MIIM commands"},
#endif  // defined(M_TCPIP_COMMANDS_MIIM)
#if (M_TCPIP_UDP_COMMANDS)
    {"udp",         &F_Command_Udp,                  ": UDP commands"},
#endif  // (M_TCPIP_UDP_COMMANDS)
#if (M_TCPIP_TCP_COMMANDS)
    {"tcp",         &F_Command_Tcp,                  ": TCP commands"},
    {"tcptrace",    &F_Command_TcpTrace,             ": Enable TCP trace"},
#endif  // (M_TCPIP_TCP_COMMANDS)
#if (M_TCPIP_PACKET_LOG_ENABLE)
    {"plog",        &F_Command_PktLog,               ": PKT flight log"},
#endif  // (M_TCPIP_PACKET_LOG_ENABLE)
#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
    {"pktinfo",     &F_Command_PktInfo,              ": Check PKT allocation"},
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
#if defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
    {"heaplist",    &F_Command_HeapList,             ": List heap"},
#endif  // defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)
    {"announce",    &F_Command_Announce,             ": Announce"},
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)
#if defined(TCPIP_STACK_USE_FTP_CLIENT)  && defined(TCPIP_FTPC_COMMANDS)
    {"ftpc",        &F_Command_FTPC_Service,         ": Connect FTP Client to Server"},
#endif  // (TCPIP_STACK_USE_FTP_CLIENT)    
#if defined(TCPIP_STACK_USE_IPV4)  && defined(TCPIP_IPV4_COMMANDS) && (TCPIP_IPV4_COMMANDS != 0)
    {"ip4",         &F_Command_Ipv4,                   ": IPv4"},
#endif
#if (M_TCPIP_PKT_ALLOC_COMMANDS != 0)
    {"pkt",         &F_Command_Packet,                 ": pkt"},
#endif  // (M_TCPIP_PKT_ALLOC_COMMANDS != 0)
#if defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)
    {"bridge",      &F_Command_Bridge,                 ": Bridge"},
#endif // defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)
#if defined(M_TCPIP_STACK_HDLC_COMMANDS)
    {"hdlc",        &F_Command_Hdlc,                   ": Hdlc"},
#endif  // defined(M_TCPIP_STACK_HDLC_COMMANDS)
#if defined(M_TCPIP_STACK_PPP_COMMANDS)
    {"ppp",         &F_Command_Ppp,                    ": ppp"},
#endif  // defined(M_TCPIP_STACK_PPP_COMMANDS)
#if defined(TCPIP_STACK_RUN_TIME_INIT) && (TCPIP_STACK_RUN_TIME_INIT != 0)
    {"deinit",      &F_Command_ModDeinit,          ": deinit"},
    {"runstat",     &F_Command_ModRunning,          ": runstat"},
#endif  // defined(TCPIP_STACK_RUN_TIME_INIT) && (TCPIP_STACK_RUN_TIME_INIT != 0)
#if defined(TCPIP_STACK_USE_SNMPV3_SERVER)    
    {"snmpv3",  &F_Command_SNMPv3USMSet,     ": snmpv3"},
#endif    
#if defined(TCPIP_STACK_USE_SNTP_CLIENT)  
    {"sntp",    &F_Command_Sntp,     ": sntp"},
#endif  // defined(TCPIP_STACK_USE_SNTP_CLIENT)  
#if defined(M_TCPIP_COMMANDS_WSC)
    {"wsc",        &F_CommandWsc,                  ": WebSocket commands"},
#endif  // defined(M_TCPIP_COMMANDS_WSC)
};

#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((M_TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))
/* MISRA C-2012 Rule 21.3 deviated:2 Deviation record ID -  H3_MISRAC_2012_R_21_3_NET_DR_7 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:2 "MISRA C-2012 Rule 21.3" "H3_MISRAC_2012_R_21_3_NET_DR_7" 
#endif  //  defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((M_TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))
bool TCPIP_Commands_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init
    if(commandInitCount == 0)
    {   // 1st time we run
        initialNetIfs = stackCtrl->nIfs;

        // create command group
        if (!SYS_CMD_ADDGRP(tcpipCmdTbl, (int)(sizeof(tcpipCmdTbl) / sizeof(*tcpipCmdTbl)), "tcpip", ": stack commands"))
        {
            SYS_ERROR(SYS_ERROR_ERROR, "Failed to create TCPIP Commands\r\n");
            return false;
        }

#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((M_TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))
        // get storage for interfaces configuration
        // cannot be taken from the TCPIP-HEAP because we need it persistent after
        // TCPIP_STACK_Deinit() is called!
        if(pCmdStgDcpt == NULL && pCmdNetConf == NULL)
        {
            pCmdStgDcpt = (TCPIP_COMMAND_STG_DCPT*)TCPIP_STACK_CALLOC_FUNC(initialNetIfs, sizeof(*pCmdStgDcpt));
            pCmdNetConf = (TCPIP_NETWORK_CONFIG*)TCPIP_STACK_CALLOC_FUNC(initialNetIfs, sizeof(*pCmdNetConf));
            if(pCmdStgDcpt == NULL || pCmdNetConf == NULL)
            {   // failure is not considered to be catastrophic
                SYS_ERROR(SYS_ERROR_WARNING, "Failed to create TCPIP Commands Storage/Config\r\n");
            }
        }
#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((M_TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))

#if defined(M_TCPIP_COMMAND_PING4)
        icmpAckRecv = 0U;
#endif  // defined(M_TCPIP_COMMAND_PING4)
#if defined(M_TCPIP_COMMAND_PING6)
        hIcmpv6 = NULL;
        icmpAckRecv = 0U;
#endif  // defined(M_TCPIP_COMMAND_PING6)

#if defined(M_TCPIP_STACK_COMMAND_TASK)
        tcpipCmdSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_COMMAND_Task, 0);
        if(tcpipCmdSignalHandle == NULL)
        {   // timer is not active now
            SYS_ERROR(SYS_ERROR_ERROR, "TCPIP commands task registration failed\r\n");
            return false;
        }
        // else the timer will start when we send a query
        tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
#endif  // defined(M_TCPIP_STACK_COMMAND_TASK)

#if defined(M_TCPIP_COMMANDS_MIIM)
        // get the MIIM driver object
        miimObj = &DRV_MIIM_OBJECT_BASE_Default;
        miimObjIx = DRV_MIIM_DRIVER_INDEX_0;
        miimHandle = 0U;
        miimOpHandle = NULL;
#endif  // defined(M_TCPIP_COMMANDS_MIIM)

#if defined(M_TCPIP_COMMANDS_WSC)
        Wsc_CmdInitialize();
#endif  // defined(M_TCPIP_COMMANDS_WSC)

        sysFreq = SYS_TMR_TickCounterFrequencyGet();
    }

    commandInitCount++;

    return true;
}
#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((M_TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))
#pragma coverity compliance end_block "MISRA C-2012 Rule 21.3"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */
#endif // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && ((M_TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0))

#if (M_TCPIP_STACK_DOWN_OPERATION != 0)
#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
/* MISRA C-2012 Rule 21.3 deviated:2 Deviation record ID -  H3_MISRAC_2012_R_21_3_NET_DR_7 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:2 "MISRA C-2012 Rule 21.3" "H3_MISRAC_2012_R_21_3_NET_DR_7" 
#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
void TCPIP_Commands_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(commandInitCount > 0 && stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
    {   // whole stack is going down
        if(--commandInitCount == 0)
        {   // close all
#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
            if(tcpipCmdPreserveSavedInfo == false)
            {
                TCPIP_STACK_FREE_FUNC(pCmdStgDcpt);
                TCPIP_STACK_FREE_FUNC(pCmdNetConf);
                pCmdStgDcpt = NULL;
                pCmdNetConf = NULL;
            }
#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)

#if defined(M_TCPIP_STACK_COMMAND_TASK)
            if(tcpipCmdSignalHandle != NULL)
            {
                TCPIPStackSignalHandlerDeregister(tcpipCmdSignalHandle);
                tcpipCmdSignalHandle = NULL;
            }
#endif  // defined(M_TCPIP_STACK_COMMAND_TASK)

#if defined(M_TCPIP_COMMAND_PING6)
            if(hIcmpv6 != NULL)
            {
                (void)TCPIP_ICMPV6_CallbackDeregister(hIcmpv6);
            }
#endif  // defined(M_TCPIP_COMMAND_PING6)
        }
    }
}
#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
#pragma coverity compliance end_block "MISRA C-2012 Rule 21.3"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */
#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE) && (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
#endif  // (M_TCPIP_STACK_DOWN_OPERATION != 0)

static void F_Command_NetInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t i;
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
        return;
    }

    for (i=0; i < initialNetIfs; i++)
    {
        netH = TCPIP_STACK_IndexToNet(i);
        (void)TCPIP_STACK_NetAliasNameGet(netH, addrBuff, sizeof(addrBuff));
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
        (void)TCPIP_Helper_IPAddressToString(&ipAddr, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 Address: %s\r\n", addrBuff);

        ipAddr.Val = TCPIP_STACK_NetMask(netH);
        (void)TCPIP_Helper_IPAddressToString(&ipAddr, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Mask: %s\r\n", addrBuff);

        ipAddr.Val = TCPIP_STACK_NetAddressGateway(netH);
        (void)TCPIP_Helper_IPAddressToString(&ipAddr, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Gateway: %s\r\n", addrBuff);

        ipAddr.Val = TCPIP_STACK_NetAddressDnsPrimary(netH);
        (void)TCPIP_Helper_IPAddressToString(&ipAddr, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "DNS1: %s\r\n", addrBuff);

        ipAddr.Val = TCPIP_STACK_NetAddressDnsSecond(netH);
        (void)TCPIP_Helper_IPAddressToString(&ipAddr, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "DNS2: %s\r\n", addrBuff);
#endif  // defined(TCPIP_STACK_USE_IPV4)

        pMac = FC_CUptr2CMacAdd(TCPIP_STACK_NetAddressMac(netH));
        (void)TCPIP_Helper_MACAddressToString(pMac, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "MAC Address: %s\r\n", addrBuff);

#if defined(M_TCPIP_STACK_PPP_COMMANDS)
        TCPIP_MAC_TYPE macType = TCPIP_STACK_NetMACTypeGet(netH);
        if(macType == TCPIP_MAC_TYPE_PPP)
        { 
            DRV_HANDLE hPPP = DRV_PPP_MAC_Open((uint16_t)TCPIP_MODULE_MAC_PPP_0, DRV_IO_INTENT_SHARED);
            ipAddr.Val = PPP_GetRemoteIpv4Addr(hPPP);
            (void)TCPIP_Helper_IPAddressToString(&ipAddr, addrBuff, sizeof(addrBuff));
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Peer address: %s\r\n", addrBuff);
        }
#endif  // defined(M_TCPIP_STACK_PPP_COMMANDS)

        // display IPv6 addresses
#if defined(TCPIP_STACK_USE_IPV6)
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "IPv6 Unicast addresses:\r\n");

        prevHandle = NULL;
        do
        {
            nextHandle = TCPIP_STACK_NetIPv6AddressGet(netH, IPV6_ADDR_TYPE_UNICAST, &currIpv6Add, prevHandle);
            if(nextHandle != NULL)
            {   // have a valid address; display it
                addr6 = currIpv6Add.address;
                (void)TCPIP_Helper_IPv6AddressToString(&addr6, addrBuff, sizeof(addrBuff));
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "    %s\r\n", addrBuff);
                prevHandle = nextHandle;
            }
        }while(nextHandle != NULL);

        if(prevHandle == NULL)
        {   // no valid address
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "    Unknown\r\n");
        }
        
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "IPv6 Multicast addresses:\r\n");
        prevHandle = NULL;
        do
        {
            nextHandle = TCPIP_STACK_NetIPv6AddressGet(netH, IPV6_ADDR_TYPE_MULTICAST, &currIpv6Add, prevHandle);
            if(nextHandle != NULL)
            {   // have a valid address; display it
                addr6 = currIpv6Add.address;
                (void)TCPIP_Helper_IPv6AddressToString(&addr6, addrBuff, sizeof(addrBuff));
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "    %s\r\n", addrBuff);
                prevHandle = nextHandle;
            }
        }while(nextHandle != NULL);

        if(prevHandle == NULL)
        {   // no valid address
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "    Unknown\r\n");
        }

        bool res = TCPIP_STACK_NetDnsIPv6Get(netH, &addr6); 
        if(res)
        {
            (void)TCPIP_Helper_IPv6AddressToString(&addr6, addrBuff, sizeof(addrBuff));
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv6 DNS: %s\r\n", addrBuff);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "IPv6 DNS: None\r\n");
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
#if defined(TCPIP_STACK_USE_DHCP_SERVER) || defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
        if(TCPIP_DHCPS_IsEnabled(netH))
        {
            msgAdd = "dhcps";
        }
        else
#endif  // defined(TCPIP_STACK_USE_DHCP_SERVER) || defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
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
}

#if defined(TCPIP_STACK_USE_DHCP_SERVER)
static void F_Command_DHCPLeaseInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam,"MAC Address       IPAddress       RemainingLeaseTime \r\n",0);

    prevLease = NULL;
    do
    {
        (void)memset((void*)&leaseEntry,0,sizeof(TCPIP_DHCPS_LEASE_ENTRY));
        nextLease = TCPIP_DHCPS_LeaseEntryGet(netH, &leaseEntry, prevLease);
        if(nextLease == NULL)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, " \r\n No more entry present \r\n", 0);
        }
        if(nextLease != NULL)
        {   // valid info
            // display info
            (void)TCPIP_Helper_MACAddressToString(&leaseEntry.hwAdd, addrBuff, sizeof(addrBuff));
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s", addrBuff);
            (void)TCPIP_Helper_IPAddressToString(&leaseEntry.ipAddress, addrBuff, sizeof(addrBuff));
            (*pCmdIO->pCmdApi->print)(cmdIoParam, " %s ", addrBuff);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, " %d Secs\r\n", leaseEntry.leaseTime / sysFreq);

            prevLease = nextLease;
        }
    }while(nextLease != NULL);
}
#elif defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
static void F_Command_DHCPsOptions(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{   
    // dhcps interface {on/off, list, remove ix/all <keepPerm> <keepBusy>, stats}

    const void* cmdIoParam = pCmdIO->cmdIoParam;
    while(argc >= 3)
    {
        TCPIP_NET_HANDLE netH = TCPIP_STACK_NetHandleGet(argv[1]);
        if (netH == NULL)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
            return;
        }

        if(strcmp(argv[2], "on") == 0 || strcmp(argv[2], "off") == 0)
        {
            F_Command_AddressService(pCmdIO, argc, argv, TCPIP_STACK_ADDR_SRVC_DHCPS);
            return;
        }

        if(strcmp(argv[2], "list") == 0)
        {
            F_Command_DHCPsLeaseList(pCmdIO, argc, argv, netH); 
            return;
        }

#if (M_TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0)
        if(strcmp(argv[2], "remove") == 0)
        {
            if(argc >= 4)
            {
                F_Command_DHCPsLeaseRemove(pCmdIO, argc, argv, netH); 
                return;
            }
            break;
        }
#endif  // (M_TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0)

        if(strcmp(argv[2], "stats") == 0)
        {
            F_Command_DHCPsStat(pCmdIO, argc, argv, netH); 
            return;
        }

#if defined(M_TCPIP_STACK_DHCPS_CONFIG_EXAMPLE)
        if(strcmp(argv[2], "configure") == 0)
        {
            F_Command_DHCPsConfigure(pCmdIO, argc, argv, netH); 
            return;
        }
#endif // defined(M_TCPIP_STACK_DHCPS_CONFIG_EXAMPLE)

        break;
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dhcps interface command\r\n");
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Command is one of: {on/off, list, remove ix/all <keepBusy> <keepPerm>, stats} \r\n");

}

static bool F_Command_DHCPsEnable(TCPIP_NET_HANDLE netH)
{
    TCPIP_DHCPS_RES res = TCPIP_DHCPS_Enable(netH);
    return res == TCPIP_DHCPS_RES_OK; 
}

static bool F_Command_DHCPsDisable(TCPIP_NET_HANDLE netH)
{
    TCPIP_DHCPS_RES res = TCPIP_DHCPS_Disable(netH);
    return res == TCPIP_DHCPS_RES_OK; 
}


static void F_Command_DHCPsLeaseList(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_NET_HANDLE netH)
{   
    // dhcps interface list
    
    char   addrBuff[20];
    char   idBuff[TCPIP_DHCPS_CLIENT_ID_MAX_SIZE * 3];

    union
    {
        TCPIP_DHCPS_LEASE_INFO leaseInfo;
        uint8_t id_space[sizeof(TCPIP_DHCPS_LEASE_INFO) + TCPIP_DHCPS_CLIENT_ID_MAX_SIZE];
    }extLeaseInfo;

    const void* cmdIoParam = pCmdIO->cmdIoParam;
    (void)memset(&extLeaseInfo.leaseInfo, 0, sizeof(extLeaseInfo.leaseInfo));

    uint16_t nLeases;
    uint16_t usedLeases;
    TCPIP_DHCPS_RES res = TCPIP_DHCPS_LeaseEntriesGet(netH, &nLeases, &usedLeases);
    if(res != TCPIP_DHCPS_RES_OK)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam,"Failed to get DHCPS leases: %d\r\n", res);
        return;
    }

    uint16_t leaseIx;

    (*pCmdIO->pCmdApi->print)(cmdIoParam,"DHCPS: total leases: %d, used: %d\r\n", nLeases, usedLeases);

    if(usedLeases == 0U)
    {
        return;
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam,"IP Address\tID\r\n");
    for(leaseIx = 0; leaseIx < nLeases; leaseIx++)
    {
        TCPIP_DHCPS_LEASE_INFO* pLeaseInfo = &extLeaseInfo.leaseInfo;
        res = TCPIP_DHCPS_LeaseGetInfo(netH, pLeaseInfo, leaseIx);
        if(res ==  TCPIP_DHCPS_RES_UNUSED_INDEX)
        {
            continue;
        }
        else if ((int)res < 0)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam,"Failure for DHCPS lease: %d, res: %d\r\n", leaseIx, res);
            return;
        }
        else
        {
            // do nothing
        }

        // OK, display
        (void)TCPIP_Helper_IPAddressToString(&pLeaseInfo->ipAddress, addrBuff, sizeof(addrBuff));
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s ", addrBuff);
        size_t jx;
        char* pBuff = idBuff;
        const uint8_t* pId = pLeaseInfo->clientId;
        size_t idLim =  (sizeof(idBuff) / 3U) <  pLeaseInfo->clientIdLen ?  (sizeof(idBuff) / 3U) :  pLeaseInfo->clientIdLen;
        int nUsed = 0;
        for(jx = 0; jx < idLim; jx++)
        {
            nUsed += FC_sprintf(pBuff + nUsed, sizeof(idBuff) - (size_t)nUsed, (jx == idLim - 1U) ? "%.2x" : "%.2x:", *pId);
            pId++;
        }
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s ", idBuff);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, " Time: %d secs, state: %d, index: %d\r\n", pLeaseInfo->leaseTime, pLeaseInfo->leaseState, leaseIx);
    }

}

#if (M_TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0)
static void F_Command_DHCPsLeaseRemove(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_NET_HANDLE netH)
{   
    // dhcps interface remove ix/all <keepPerm> <keepBusy>
    
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool removeAll = false;
    uint16_t leaseIx = 0;
    uint32_t lease32 = 0UL; 
    bool keepBusy = false;
    bool keepPerm = false;

    if(strcmp(argv[3], "all") == 0)
    {
        removeAll = true;
    }
    else
    {
        if(FC_Str2UL(argv[3], 10, &lease32) < 0)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam,"Invalid DHCPS lease index\r\n");
            return;
        }
        leaseIx = (uint16_t)lease32;

    }

    int startIx = 5;
    while(argc >= startIx)
    {

        if(strcmp(argv[startIx - 1], "keepBusy") == 0)
        {
            keepBusy = true;
        }

        if(strcmp(argv[startIx - 1], "keepPerm") == 0)
        {
            keepPerm = true;
        }
        startIx++;
    }

    TCPIP_DHCPS_RES res;
    if(removeAll == false)
    {
        res = TCPIP_DHCPS_LeaseRemove(netH, leaseIx, keepBusy);
    }
    else
    {
        res = TCPIP_DHCPS_LeaseRemoveAll(netH, keepPerm, keepBusy);
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam,"DHCPS remove %s, res: %d\r\n", argv[3], res);
}
#endif  // (M_TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0)

static void F_Command_DHCPsStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_NET_HANDLE netH)
{   
    // dhcps interface stats
    
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    TCPIP_DHCPS_STATISTICS_DATA statData;

    TCPIP_DHCPS_RES res = TCPIP_DHCPS_StatisticsDataGet(netH, &statData);

    if((int)res < 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam,"Failed to get stats: %d\r\n", res);
        return;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam,"\treleasedDelCount: %d, expiredDelCount: %d cacheFullCount: %d \r\n", statData.releasedDelCount, statData.expiredDelCount, statData.cacheFullCount);
    (*pCmdIO->pCmdApi->print)(cmdIoParam,"\tpoolEmptyCount: %d, declinedCount: %d arpFailCount: %d \r\n", statData.poolEmptyCount, statData.declinedCount, statData.arpFailCount);
    (*pCmdIO->pCmdApi->print)(cmdIoParam,"\techoFailCount: %d, icmpFailCount: %d icmpProbeCount: %d \r\n", statData.echoFailCount, statData.icmpFailCount, statData.icmpProbeCount);
    (*pCmdIO->pCmdApi->print)(cmdIoParam,"\tmsgOvflCount: %d, arpInjectCount: %d, sktNotReadyCount: %d\r\n", statData.msgOvflCount, statData.arpInjectCount, statData.sktNotReadyCount);
}

// run-time configuration example 
#if defined(M_TCPIP_STACK_DHCPS_CONFIG_EXAMPLE)
static void F_Command_DHCPsConfigure(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_NET_HANDLE netH)
{   
    // dhcps interface configure <1/2>
    static const TCPIP_DHCPS_CLIENT_OPTION_CONFIG dhcpsOptions1[] =
    {
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_ROUTER,
            .ipStr = "192.168.222.20",
        },
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_DNS,
            .ipStr = "192.168.222.20",
        },
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_DNS,
            .ipStr = "192.168.222.21",
        },
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_T1_RENEWAL,
            .multFact = 2,
            .divFact = 3,
        },
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_T2_REBINDING,
            .multFact = 6,
            .divFact = 7,
        },
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_NAME_SERVER,
            .ipStr = "192.168.222.20",
        },

    };    
    
    static const TCPIP_DHCPS_INTERFACE_CONFIG dhcpsConfig1[] = 
    {
        {
            .ifIndex = 0,
            .configFlags = 0,
            .leaseEntries = 20,
            .leaseDuration = 60,
            .minLeaseDuration = 60,
            .maxLeaseDuration = 120,
            .unreqOfferTmo = 10,
            .serverIPAddress = "192.168.222.1",
            .startIPAddress = "192.168.222.200",
            .prefixLen = 24,
            .pOptConfig = dhcpsOptions1,
            .nOptConfigs = sizeof(dhcpsOptions1) / sizeof(*dhcpsOptions1),
        }
    };

    static const TCPIP_DHCPS_CLIENT_OPTION_CONFIG dhcpsOptions2[] =
    {
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_ROUTER,
            .ipStr = "192.168.111.10",
        },
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_DNS,
            .ipStr = "192.168.111.10",
        },
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_DNS,
            .ipStr = "192.168.111.11",
        },
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_T1_RENEWAL,
            .multFact = 3,
            .divFact = 4,
        },
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_T2_REBINDING,
            .multFact = 5,
            .divFact = 6,
        },
        {
            .optType = TCPIP_DHCPS_CLIENT_OPTION_NTP_SERVER,
            .ipStr = "192.168.111.111",
        },

    };    
    
    static const TCPIP_DHCPS_INTERFACE_CONFIG dhcpsConfig2[] = 
    {
        {
            .ifIndex = 0,
            .configFlags = 0,
            .leaseEntries = 20,
            .leaseDuration = 60,
            .minLeaseDuration = 60,
            .maxLeaseDuration = 120,
            .unreqOfferTmo = 10,
            .serverIPAddress = "192.168.111.100",
            .startIPAddress = "192.168.111.101",
            .prefixLen = 24,
            .pOptConfig = dhcpsOptions2,
            .nOptConfigs = sizeof(dhcpsOptions2) / sizeof(*dhcpsOptions2),
        }
    };

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    const TCPIP_DHCPS_INTERFACE_CONFIG* pConfig = dhcpsConfig1;
    uint16_t nConfigs = sizeof(dhcpsConfig1) / sizeof(*dhcpsConfig1); 

    if(argc >= 4)
    {
        uint32_t cfgNo = 0UL;
        (void)FC_Str2UL(argv[3], 10, &cfgNo);
        if(cfgNo == 2U)
        {
            pConfig = dhcpsConfig2;
            nConfigs = sizeof(dhcpsConfig2) / sizeof(*dhcpsConfig2); 
        }
    }


    TCPIP_DHCPS_RES res = TCPIP_DHCPS_Configure(pConfig, nConfigs);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPS configure res: %d\r\n", res);

}
#endif // defined(M_TCPIP_STACK_DHCPS_CONFIG_EXAMPLE)

#endif  //  defined(TCPIP_STACK_USE_DHCP_SERVER) defined(TCPIP_STACK_USE_DHCP_SERVER_V2)

static void F_Command_DefaultInterfaceSet (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    bool res;
    size_t nameSize;
    TCPIP_NET_HANDLE netH = NULL;
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
            if (netH == NULL)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
                return;
            }
            defaultOp = 1;
        }
        else if(strcmp(argv[1], "get") == 0)
        {
            defaultOp = 2;
        }
        else
        {
            // do nothing
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
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Default interface is: %s\r\n", nameSize != 0U ? nameBuff : "None");
            break;

        default:
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: defnet set/get <interface>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: defnet set eth0\r\n");
            break;
    }


}

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)
static void F_Command_DhcpOptions(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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
        return;
    }

    reqIpAddr.Val = 0;
    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
        return;
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
            return;
        }

        if (!TCPIP_Helper_StringToIPAddress(argv[3], &reqIpAddr))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IP address string \r\n");
            return;
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
                addBuff[0] = '\0';
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP lease start: %d, duration: %ds\r\n", dhcpInfo.leaseStartTime, dhcpInfo.leaseDuration);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP renew time: %d, rebind time: %d\r\n", dhcpInfo.renewTime, dhcpInfo.rebindTime);

                (void)TCPIP_Helper_IPAddressToString(&dhcpInfo.dhcpAddress, addBuff, sizeof(addBuff));
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP address: %s\r\n", addBuff);
                (void)TCPIP_Helper_IPAddressToString(&dhcpInfo.serverAddress, addBuff, sizeof(addBuff));
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP server: %s\r\n", addBuff);
                if(dhcpInfo.bootFileName == NULL || strlen(dhcpInfo.bootFileName) == 0U)
                {
                    bootName = "not given";
                }
                else
                {
                    bootName = dhcpInfo.bootFileName;
                }
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP boot name: %s\r\n", bootName);

                if(dhcpInfo.timeServersNo != 0U)
                {
                    (void)TCPIP_Helper_IPAddressToString(dhcpInfo.timeServers, addBuff, sizeof(addBuff));
                }
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP Time servers: %d, %s\r\n", dhcpInfo.timeServersNo, dhcpInfo.timeServersNo != 0U ? addBuff: "None");

                if(dhcpInfo.ntpServersNo != 0U)
                {
                    (void)TCPIP_Helper_IPAddressToString(dhcpInfo.ntpServers, addBuff, sizeof(addBuff));
                }
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCP NTP servers: %d, %s\r\n", dhcpInfo.ntpServersNo, dhcpInfo.ntpServersNo != 0U ? addBuff :  "None");
            }
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCP: failed to get info\r\n");
        }
        return;
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown option\r\n");
        return;
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

}
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DHCP_CLIENT)

#if defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
static void F_Command_Dhcpv6Options(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    TCPIP_DHCPV6_CLIENT_RES res;
    const char* msg;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    char printBuff[100];

    if (argc < 3)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Usage: %s <interface> on/off/info\r\n", argv[0]);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Usage: %s <interface> ia state ix \r\n", argv[0]);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Usage: %s <interface> release addr\r\n", argv[0]);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Ex: %s eth0 on \r\n", argv[0]);
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
        return;
    }

    if (strcmp(argv[2], "info") == 0)
    {   // DHCPV6 info
        TCPIP_DHCPV6_CLIENT_INFO dhcpv6Info;
        IPV6_ADDR dhcpv6DnsBuff[1];

        dhcpv6Info.statusBuff = (uint8_t*)printBuff;
        dhcpv6Info.statusBuffSize = sizeof(printBuff);
        dhcpv6Info.dnsBuff = dhcpv6DnsBuff;
        dhcpv6Info.dnsBuffSize = sizeof(dhcpv6DnsBuff);
        dhcpv6Info.domainBuff = NULL;
        dhcpv6Info.domainBuffSize = 0U;


        res = TCPIP_DHCPV6_ClientInfoGet(netH, &dhcpv6Info);
        if((int)res >= 0) 
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 status: %d ( %d == Run), tot Buffs: %d, free Buffs: %d, time: %d\r\n", dhcpv6Info.clientState, TCPIP_DHCPV6_CLIENT_STATE_RUN, dhcpv6Info.totBuffers, dhcpv6Info.freeBuffers, dhcpv6Info.dhcpTime);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 tot IANAs: %d, tot IATAs: %d, IANAs: %d, IATAs %d: Free IAs: %d\r\n", dhcpv6Info.totIanas, dhcpv6Info.totIatas, dhcpv6Info.nIanas, dhcpv6Info.nIatas, dhcpv6Info.nFreeIas);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 sol IAs: %d, req IAs: %d, dad IAs %d: decline IAs: %d, bound IAs: %d\r\n", dhcpv6Info.solIas, dhcpv6Info.reqIas, dhcpv6Info.dadIas, dhcpv6Info.declineIas, dhcpv6Info.boundIas);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 renew IAs: %d, rebind IAs: %d, confirm IAs %d: release IAs: %d, trans IAs: %d\r\n", dhcpv6Info.renewIas, dhcpv6Info.rebindIas, dhcpv6Info.confirmIas, dhcpv6Info.releaseIas, dhcpv6Info.transIas);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCPV6: failed to get client info\r\n");
        }
    }
    else if(strcmp(argv[2], "ia") == 0)
    {
        if (argc < 5)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCPV6: provide an IA state and index\r\n");
        }
        else
        {
            int32_t iaState = 0;
            (void)FC_Str2L(argv[3], 10, &iaState);
            int32_t iaIx = 0;
            (void)FC_Str2L(argv[4], 10, &iaIx);
            TCPIP_DHCPV6_IA_INFO iaInfo;

            (void)memset(&iaInfo, 0, sizeof(iaInfo));
            iaInfo.iaState = (TCPIP_DHCPV6_IA_STATE)iaState;
            iaInfo.iaIndex = iaIx;

            res = TCPIP_DHCPV6_IaInfoGet(netH, &iaInfo);
            if((int)res >= 0) 
            {
                const char* typeMsg = (iaInfo.iaType == TCPIP_DHCPV6_IA_TYPE_IANA) ? "iana" : (iaInfo.iaType == TCPIP_DHCPV6_IA_TYPE_IATA) ? "iata" : "unknown";
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 IA type: %s, index: %d, id: %d, next: %d\r\n", typeMsg, iaInfo.iaIndex, iaInfo.iaId, iaInfo.nextIndex);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 IA status: %d ( %d == Bound), sub state: %d\r\n", iaInfo.iaState, TCPIP_DHCPV6_IA_STATE_BOUND, iaInfo.iaSubState);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 IA tAcquire: %d, t1: %d, t2: %d\r\n", iaInfo.tAcquire, iaInfo.t1, iaInfo.t2);
                (void)TCPIP_Helper_IPv6AddressToString(&iaInfo.ipv6Addr, printBuff, sizeof(printBuff));
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 IA address: %s, pref LTime: %d, valid LTime: %d\r\n", printBuff, iaInfo.prefLTime, iaInfo.validLTime);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 IA msgBuffer: 0x%08x\r\n", iaInfo.msgBuffer);
            }
            else if(res == TCPIP_DHCPV6_CLIENT_RES_IX_ERR)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCPV6: no such IA\r\n");
            }
            else
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCPV6: failed to get IA info\r\n");
            }
        }
    }
    else if (strcmp(argv[2], "on") == 0)
    {
        res = TCPIP_DHCPV6_Enable(netH);
        msg = (res == TCPIP_DHCPV6_CLIENT_RES_OK) ? "ok" : (res == TCPIP_DHCPV6_CLIENT_RES_BUSY) ? "busy" : "err";

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 on: %s, res: %d\r\n", msg, res);
    }
    else if (strcmp(argv[2], "off") == 0)
    {
        res = TCPIP_DHCPV6_Disable(netH);
        msg = (res == TCPIP_DHCPV6_CLIENT_RES_OK) ? "ok" : (res == TCPIP_DHCPV6_CLIENT_RES_BUSY) ? "busy" : "err";
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 off: %s, res: %d\r\n", msg, res);
    }
#if defined(TCPIP_DHCPV6_STATISTICS_ENABLE) && (TCPIP_DHCPV6_STATISTICS_ENABLE != 0)
    else if (strcmp(argv[2], "stat") == 0)
    {
        TCPIP_DHCPV6_CLIENT_STATISTICS stat;
        res = TCPIP_DHCPV6_Statistics(netH, &stat);
        if(res == TCPIP_DHCPV6_CLIENT_RES_OK)
        {
            (void)FC_sprintf(printBuff, sizeof(printBuff), "DHCPV6 buffers: %zu, free: %zu, pend rx: %zu, pend tx: %zu, advertise: %zu, reply: %zu\r\n", stat.msgBuffers, stat.freeBuffers, stat.rxMessages, stat.txMessages, stat.advMessages, stat.replyMessages);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s", printBuff);


            (void)FC_sprintf(printBuff, sizeof(printBuff), "DHCPV6 failures - tx Buff: %zu, tx Space: %zu, tx Flush: %zu, rx Buff: %zu, rx Space: %zu\r\n", stat.txBuffFailCnt, stat.txSpaceFailCnt, stat.txSktFlushFailCnt, stat.rxBuffFailCnt, stat.rxBuffSpaceFailCnt);    
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s", printBuff);
        }
        else
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 stats - failed: %d\r\n", res);
        }
    }
#endif  // defined(TCPIP_DHCPV6_STATISTICS_ENABLE) && (TCPIP_DHCPV6_STATISTICS_ENABLE != 0)
    else if (strcmp(argv[2], "release") == 0)
    {
        IPV6_ADDR relAddr;
        if (argc < 4)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCPV6: provide an IPv6 address\r\n");
        }
        else if (!TCPIP_Helper_StringToIPv6Address(argv[3], &relAddr))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCPV6: provide a valid IPv6 address\r\n");
        }
        else
        {
            res = TCPIP_DHCPV6_AddrRelease(netH, &relAddr);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "DHCPV6 release returned: %d\r\n", res);
        }
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "DHCPV6: Unknown option\r\n");
    }

}
#endif  // defined(TCPIP_STACK_USE_DHCPV6_CLIENT)

#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DHCP_SERVER)
static void F_Command_DHCPSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    F_Command_AddressService(pCmdIO, argc, argv, TCPIP_STACK_ADDR_SRVC_DHCPS);
}
#endif  // defined(TCPIP_STACK_USE_DHCP_SERVER)

static void F_Command_ZcllOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    F_Command_AddressService(pCmdIO, argc, argv, TCPIP_STACK_ADDR_SRVC_ZCLL);
}

static void F_Command_AddressService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, TCPIP_STACK_ADDR_SRVC_TYPE svcType)
{ 
    TCPIP_NET_HANDLE netH;
    addSvcFnc        addFnc;
    bool             addRes, svcEnable;
    const char       *msgOK, *msgFail;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 3)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Usage: %s <interface> <on/off> \r\n", argv[0]);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Ex: %s PIC32INT on \r\n", argv[0]);
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
        return;
    }

    if (strcmp(argv[2], "on") == 0)
    {   // turning on a service
        svcEnable = true;
    }
    else if (strcmp(argv[2], "off") == 0)
    {   // turning off a service
        svcEnable = false;
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown option\r\n");
        return;
    }

    switch(svcType)
    {
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
        case TCPIP_STACK_ADDR_SRVC_DHCPC:
            addFnc = svcEnable ? &TCPIP_DHCP_Enable : &TCPIP_DHCP_Disable;
            break;
#endif 
            
#if defined(TCPIP_STACK_USE_DHCP_SERVER)
        case TCPIP_STACK_ADDR_SRVC_DHCPS:
            addFnc = svcEnable ? &TCPIP_DHCPS_Enable : &TCPIP_DHCPS_Disable;
            break;
#elif defined(TCPIP_STACK_USE_DHCP_SERVER_V2)
        case TCPIP_STACK_ADDR_SRVC_DHCPS:
            addFnc = svcEnable? &F_Command_DHCPsEnable : &F_Command_DHCPsDisable;
            break;
#endif  // defined(TCPIP_STACK_USE_DHCP_SERVER)

#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
        case TCPIP_STACK_ADDR_SRVC_ZCLL:
            addFnc = svcEnable ? &TCPIP_ZCLL_Enable : &TCPIP_ZCLL_Disable;
            break;
#endif
        default:
            addFnc = NULL;     // unknown service;
            break;
    }

    if(addFnc != NULL)
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

}
#endif  // defined(TCPIP_STACK_USE_IPV4)


static void F_Command_IPAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    TCPIP_NET_IF*   pNetIf;
    IP_ADDRESS_TYPE addType;

    IPV4_ADDR ipAddr;
#if defined(TCPIP_STACK_USE_IPV4)
    IPV4_ADDR ipMask;
    IPV4_ADDR*  pMask;
#endif  // defined(TCPIP_STACK_USE_IPV4)
    IPV6_ADDR  ipv6Addr;
#if defined(TCPIP_STACK_USE_IPV6)
    uint8_t    prefixLen;
#endif  // defined(TCPIP_STACK_USE_IPV6)
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool     success = false;

    if (argc < 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setip <interface> <ipv4/6 address> <ipv4mask/ipv6 prefix len>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setip PIC32INT 192.168.0.8 255.255.255.0 \r\n");
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return;
    }

    pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No such interface is up\r\n");
        return;
    }

    bool badAddType = false;
    if (TCPIP_Helper_StringToIPAddress(argv[2], &ipAddr))
    {
        addType = IP_ADDRESS_TYPE_IPV4;
#if !defined(TCPIP_STACK_USE_IPV4)
        badAddType = true;
#endif  // defined(TCPIP_STACK_USE_IPV4)
    }
    else if(TCPIP_Helper_StringToIPv6Address (argv[2], &ipv6Addr))
    {
        addType = IP_ADDRESS_TYPE_IPV6;
#if !defined(TCPIP_STACK_USE_IPV6)
        badAddType = true;
#endif  // defined(TCPIP_STACK_USE_IPV6)
    }
    else
    {
        badAddType = true;
    }

    if(badAddType)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IP address string \r\n");
        return;
    }

    (void)addType;

#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IPV6)
    if(addType == IP_ADDRESS_TYPE_IPV4)
    {
#endif  // defined(TCPIP_STACK_USE_IPV6)
        if(TCPIPStackAddressServiceIsRunning(pNetIf) != TCPIP_STACK_ADDR_SRVC_NONE)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "An address service is already running. Stop DHCP, ZCLL, etc. first\r\n");
            return;
        }

        if(argc > 3)
        {   // we have net mask too
            if (!TCPIP_Helper_StringToIPAddress(argv[3], &ipMask))
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IP mask string \r\n");
                return;
            }
            pMask = &ipMask;
        }
        else
        {
            pMask = NULL;
        }

        if(TCPIP_STACK_NetAddressSet(netH, &ipAddr, pMask, true))
        {
            success = true;
        }
#if defined(TCPIP_STACK_USE_IPV6)
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)

#if defined(TCPIP_STACK_USE_IPV4)
    if(addType == IP_ADDRESS_TYPE_IPV6)
    {
#endif  //  defined(TCPIP_STACK_USE_IPV4)
        if(argc > 3)
        {   // we have prefix length
            uint32_t prefix32 = 0UL;
            (void)FC_Str2UL(argv[3], 10, &prefix32);
            prefixLen = (uint8_t)prefix32;
        }
        else
        {
            prefixLen = 0U;
        }

        if(TCPIP_IPV6_UnicastAddressAdd (netH, &ipv6Addr, prefixLen, 0U) != NULL)
        {
            success = true;
        }
#if defined(TCPIP_STACK_USE_IPV4)
    }
#endif  //  defined(TCPIP_STACK_USE_IPV4)

#endif  // defined(TCPIP_STACK_USE_IPV6)


    (*pCmdIO->pCmdApi->msg)(cmdIoParam, success ? "Set ip address OK\r\n" : "Set ip address failed\r\n");
}

static void F_Command_GatewayAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    IP_ADDRESS_TYPE addType;
    IPV4_ADDR ipGateway;
    IPV6_ADDR  ipv6Gateway;
#if defined(TCPIP_STACK_USE_IPV6)
    uint32_t   validTime;
#endif  // defined(TCPIP_STACK_USE_IPV6)
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool     success = false;

    if (argc < 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setgw <interface> <ipv4/6 address> <validTime> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setgw PIC32INT 192.168.0.1 \r\n");
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return;
    }

    bool badAddType = false;
    if (TCPIP_Helper_StringToIPAddress(argv[2], &ipGateway))
    {
        addType = IP_ADDRESS_TYPE_IPV4;
#if !defined(TCPIP_STACK_USE_IPV4)
        badAddType = true;
#endif  // defined(TCPIP_STACK_USE_IPV4)
    }
    else if(TCPIP_Helper_StringToIPv6Address (argv[2], &ipv6Gateway))
    {
        addType = IP_ADDRESS_TYPE_IPV6;
#if !defined(TCPIP_STACK_USE_IPV6)
        badAddType = true;
#endif  // defined(TCPIP_STACK_USE_IPV6)
    }
    else
    {
        badAddType = true;
    }

    if(badAddType)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IP address string \r\n");
        return;
    }


    (void)addType;
#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IPV6)
    if(addType == IP_ADDRESS_TYPE_IPV4)
    {
#endif  // defined(TCPIP_STACK_USE_IPV6)
        success = TCPIP_STACK_NetAddressGatewaySet(netH, &ipGateway);
#if defined(TCPIP_STACK_USE_IPV6)
    }
#endif  // defined(TCPIP_STACK_USE_IPV6)
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
#if defined(TCPIP_STACK_USE_IPV4)
    if(addType == IP_ADDRESS_TYPE_IPV6)
    {
#endif  // defined(TCPIP_STACK_USE_IPV4)
        validTime = 0UL;
        if(argc > 3)
        {   // we have validity time
            (void)FC_Str2UL(argv[3], 10, &validTime);
        }
        success = TCPIP_IPV6_RouterAddressAdd(netH, &ipv6Gateway, validTime, 0);
#if defined(TCPIP_STACK_USE_IPV4)
    }
#endif  // defined(TCPIP_STACK_USE_IPV4)
#endif  // defined(TCPIP_STACK_USE_IPV6)


    (*pCmdIO->pCmdApi->msg)(cmdIoParam, success ? "Set gateway address OK\r\n" : "Set gateway address failed\r\n");
}

#if defined(TCPIP_STACK_USE_IPV4)
static void F_Command_DNSAddressSet4(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    IPV4_ADDR ipDNS;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setdns4 if add <1/2>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setdns4 eth0 192.168.1.1\r\n");
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified\r\n");
        return;
    }

    if (!TCPIP_Helper_StringToIPAddress(argv[2], &ipDNS))
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IPv4 address string\r\n");
        return;
    }

    uint32_t dnsIx = 1UL;
    if(argc > 3)
    {
        (void)FC_Str2UL(argv[3], 10, &dnsIx);
        if(dnsIx != 1U && dnsIx != 2U)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown DNS index\r\n");
            return;
        }
    }

    bool res = dnsIx == 1U ? TCPIP_STACK_NetDnsPrimarySet(netH, &ipDNS) : TCPIP_STACK_NetDnsSecondSet(netH, &ipDNS); 
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Set DNS %d address %s\r\n", dnsIx, res ? "success" : "failed");
}
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
static void F_Command_DNSAddressSet6(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    IPV6_ADDR ipDNS;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setdns6 if ipv6Add\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setdns6 eth0 2001::3:2:1\r\n");
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified\r\n");
        return;
    }

    if (!TCPIP_Helper_StringToIPv6Address(argv[2], &ipDNS))
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IPv6 address string\r\n");
        return;
    }


    bool res = TCPIP_STACK_NetDnsIPv6Set(netH, &ipDNS);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Set DNS6 address %s\r\n", res ? "success" : "failed");
}
#endif  // defined(TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_TFTP_CLIENT)
static void F_Command_TFTPC_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    TCPIP_TFTP_CMD_TYPE cmdType=TFTP_CMD_NONE;
    size_t  serverIPStrLen = 0U;
    size_t  fileNameLen = 0U;
    IP_MULTI_ADDRESS mAddr;
    IP_ADDRESS_TYPE ipType;
    
    if (argc != 4) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: tftpc <server IP address> <command> <filename>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: Now only supports IPv4 Address\r\n");
        return;
    }
    serverIPStrLen = strlen(argv[1]);
    if(serverIPStrLen >= sizeof(tftpServerHost))
    {
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "TFTPC: Server name is too long. Retry.\r\n");
        return;
    }
    (void)strcpy(tftpServerHost, argv[1]);
    
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
        return;
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
        return;
    }

    // Process file name
    fileNameLen = strlen(argv[3]);
    if(fileNameLen < sizeof(tftpcFileName))
    {
        (void)strcpy(tftpcFileName, argv[3]);
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "TFTPC:File size should be less than [ %d ] .\r\n", sizeof(tftpcFileName) - 1U);
        return;
    }
   
    if(TCPIP_TFTPC_SetCommand(&mAddr,ipType,cmdType,tftpcFileName) != TFTPC_ERROR_NONE)
    {
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "TFTPC:Command processing error.\r\n");
        return;
    }
}
#endif
#if defined(TCPIP_STACK_USE_DNS)
static void F_Command_DNSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[2]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
        return;
    }

    if (strcmp(argv[1], "on") == 0)
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
            else
            {
                // do nothing
            }
        }
        
    }
    else if (strcmp(argv[1], "off") == 0)
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
        return;
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
    return;
}
static void F_Command_DNS_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t i;
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

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnsc <del/info/on/off/lookup> \r\n");
        return;
    }

    for(i=0; i < (sizeof(dnssComnd) / sizeof(DNSS_COMMAND_MAP)); i++)
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
            F_Command_DNSOnOff(pCmdIO,argc,argv);
            break;

        case DNS_SERVICE_COMD_LOOKUP:
            if (argc != 4)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnsc lookup <type> <hostName> \r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: <hostName>(URL) - look up for hostname\r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: <type> : a or A for IPv4 address lookup\r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: <type> : aaaa or AAAA for IPv6 address lookup\r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: <type> : any for both IPv4 and IPv6 address lookup\r\n");
            }
            else
            {
                F_Command_DNSLookUP(pCmdIO,argv);
            }
            break;

        case DNS_SERVICE_COMD_DEL:
            if (argc != 3)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnsc del <hostName>|all \r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: <hostName>(URL) - Remove the entry if exists \r\n");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: all - Remove all the resolved entry \r\n");
            }
            else
            {
                hostName = (uint8_t*)argv[2];
                if (hostName == NULL)
                {
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown option\r\n");
                    break;
                }
                if(strcmp((char*)hostName,(char*)"all")==0)
                {
                    (void)TCPIP_DNS_RemoveAll();
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
                        break;
                    case TCPIP_DNS_RES_NO_SERVICE:
                        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Incomplete command \r\n");
                        break;
                    case TCPIP_DNS_RES_OK:
                        break;
                    default:
                        // do nothing
                        break;
                }
            }
            break;

        case DNS_SERVICE_COMD_INFO:
            F_Command_ShowDNSResolvedInfo(pCmdIO,argc,argv);
            break;

        default:
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Invalid Input Command :[ %s ] \r\n", argv[1]);
            break;
    }
}


static void F_Command_DNSLookUP(SYS_CMD_DEVICE_NODE* pCmdIO, char** argv)
{
    if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE)
    {
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "dnsc lookup: command in progress. Retry later.\r\n");
        return;
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
        return;
    }

    if(strlen(argv[3]) > sizeof(dnslookupTargetHost) - 1U)
    {
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "dnsc lookup: Host name too long. Retry.\r\n");
        return;
    }
    (void)strcpy(dnslookupTargetHost, argv[3]);

    dnsLookupCmdIoParam = pCmdIO->cmdIoParam;
    (*pCmdIO->pCmdApi->print)(pCmdIO, "dnsc lookup: resolving host: %s for type: %s \r\n", dnslookupTargetHost,argv[2]);
    tcpipCmdStat = TCPIP_DNS_LOOKUP_CMD_GET;
    pTcpipCmdDevice = pCmdIO;
    (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, (int16_t)TCPIP_DNS_CLIENT_TASK_PROCESS_RATE);

    return;
}

static void F_Command_ShowDNSResolvedInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_DNS_ENTRY_QUERY dnsQuery;
    TCPIP_DNS_CLIENT_INFO clientInfo;

    IPV4_ADDR       ipv4Addr[TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS];
    char            hostName[TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN + 1];
    size_t          index, ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    TCPIP_DNS_RESULT res;
    size_t      nEntries;
    IPV6_ADDR   ipv6Addr[TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS];
    char        addrPrintBuff[44];
    const char* strictName, *prefName;

    if (argc != 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnsc info \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: display the DNS cache entry details \r\n");
        return;
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
        return;
    }

    strictName = TCPIP_STACK_NetNameGet(clientInfo.strictNet);
    if(strictName == NULL)
    {
        strictName = "none";
    }
    prefName = TCPIP_STACK_NetNameGet(clientInfo.prefNet);
    if(prefName == NULL)
    {
        prefName = "none";
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "DNS Client IF - Strict: %s, Preferred: %s\r\n", strictName, prefName);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "DNS Client - time: %d, pending: %d, current: %d, total: %d\r\n", clientInfo.dnsTime, clientInfo.pendingEntries, clientInfo.currentEntries, clientInfo.totalEntries);

    index = 0U;
    nEntries = 0U;

    while(true)
    {
        res = TCPIP_DNS_EntryQuery(&dnsQuery, index);
        if(res == TCPIP_DNS_RES_OK)
        {
            nEntries++;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Hostname = %s \r\nTimeout = %d \r\n", hostName, dnsQuery.ttlTime);
            if(dnsQuery.nIPv4ValidEntries > 0U)
            {
                for(ix = 0; ix < dnsQuery.nIPv4ValidEntries; ix++)
                {                    
                    (void)TCPIP_Helper_IPAddressToString(dnsQuery.ipv4Entry + ix, addrPrintBuff, sizeof(addrPrintBuff)); 
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 =%s\r\n", addrPrintBuff);
                }
            }
            if(dnsQuery.nIPv6Entries > 0U)
            {
                for(ix = 0; ix < dnsQuery.nIPv6ValidEntries; ix++)
                {
                    (void)TCPIP_Helper_IPv6AddressToString(dnsQuery.ipv6Entry + ix, addrPrintBuff, sizeof(addrPrintBuff));                   
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
        else
        {
            break;
        }
    }

    if(nEntries == 0U)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No DNS Client Cache entries \r\n");
    }
}
#endif

#if defined(TCPIP_STACK_USE_DNS_SERVER)
static void F_Command_DNSSOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    addSvcFnc        addFnc;
    bool             addRes, svcEnable;
    const char       *msgOK, *msgFail;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 4)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage:dnss service <interface> <on/off> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: dnss service PIC32INT on \r\n");
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[2]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface\r\n");
        return;
    }

    if (strcmp(argv[3], "on") == 0)
    {   // turning on a service
        svcEnable = true;
    }
    else if (strcmp(argv[3], "off") == 0)
    {   // turning off a service
        svcEnable = false;
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown option\r\n");
        return;
    }
    addFnc = svcEnable ? &TCPIP_DNSS_Enable : &TCPIP_DNSS_Disable;

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
}

static void F_Command_AddDelDNSSrvAddress(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv,DNS_SERVICE_COMD_TYPE dnsCommand)
{
    IP_ADDRESS_TYPE     addrType;
    uint8_t             *hostName;
    IP_MULTI_ADDRESS    ipDNS;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint32_t        entryTimeout=0;
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
            return;
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
            return;
        }
    }
    else
    {
        return;
    }

    if(strlen(argv[2]) > (size_t)TCPIP_DNSS_HOST_NAME_LEN)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, " Hostname length should not be more than [%d]\r\n",TCPIP_DNSS_HOST_NAME_LEN);
        return;
    }
    hostName = (uint8_t*)argv[2];

    if(*argv[3] == '4')
    {   // turning on a service
        addrType = IP_ADDRESS_TYPE_IPV4;
    }
#if defined(TCPIP_STACK_USE_IPV6)
    else if (*argv[3] == '6')
    {   // turning off a service
        addrType = IP_ADDRESS_TYPE_IPV6;
    }
#endif
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown option\r\n");
        return;
    }
    if(addrType == IP_ADDRESS_TYPE_IPV4)
    {
        if (!TCPIP_Helper_StringToIPAddress(argv[4], &ipDNS.v4Add)) {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IPv4 address string \r\n");
            return;
        }
    }
#if defined(TCPIP_STACK_USE_IPV6)
    if(addrType == IP_ADDRESS_TYPE_IPV6)
    {
        (void)strncpy((char*)addrBuf, argv[4], sizeof(addrBuf) - 1U);
        addrBuf[sizeof(addrBuf) - 1U] = 0;
        if (!TCPIP_Helper_StringToIPv6Address((char*)addrBuf, &ipDNS.v6Add)) {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid IPv6 address string \r\n");
            return;
        }
    }
#endif

    if(dnsCommand == DNS_SERVICE_COMD_DEL)
    {
        res = TCPIP_DNSS_CacheEntryRemove((const char*)hostName,addrType,&ipDNS);
    }
    else if(dnsCommand == DNS_SERVICE_COMD_ADD)
    {
        (void)FC_Str2UL(argv[5], 10, &entryTimeout);
        res = TCPIP_DNSS_EntryAdd((const char*)hostName, addrType, &ipDNS, entryTimeout);
    }
    else
    {
        // do nothing
    }

    switch(res)
    {
        case TCPIP_DNSS_RES_NO_ENTRY:
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "The Address is not part of the DNS Cache entry \r\n");
            break;

        case TCPIP_DNSS_RES_MEMORY_FAIL:
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No memory available \r\n");
            break;

        case TCPIP_DNSS_RES_CACHE_FULL:
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "No space to add [%s] entry \r\n",hostName);
            break;

        case TCPIP_DNSS_RES_OK:
            break;

        default:
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Failed to add [%s] entry \r\n",hostName);
            break;
    }
}

static void F_Command_DnsServService(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t i;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    DNS_SERVICE_COMD_TYPE val=DNS_SERVICE_COMD_NONE;
    DNSS_COMMAND_MAP dnssComnd[]=
    {
        {"service",DNS_SERVICE_COMD_ENABLE_INTF},
        {"add", DNS_SERVICE_COMD_ADD,},
        {"del",DNS_SERVICE_COMD_DEL,},
        {"info",DNS_SERVICE_COMD_INFO,},
    }; 
    
    
    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnss <service/add/del/info> \r\n");
         return;
    }
    
    for(i=0; i < (sizeof(dnssComnd) / sizeof(DNSS_COMMAND_MAP)); i++)
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
            F_Command_DNSSOnOff(pCmdIO,argc,argv);
            break;
        case DNS_SERVICE_COMD_ADD:
            F_Command_AddDelDNSSrvAddress(pCmdIO,argc,argv,val);
            break;
        case DNS_SERVICE_COMD_DEL:
            F_Command_AddDelDNSSrvAddress(pCmdIO,argc,argv,val);
            break;
        case DNS_SERVICE_COMD_INFO:
            F_Command_ShowDNSServInfo(pCmdIO,argc,argv);
            break;
        default:
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Invalid Input Command :[ %s ] \r\n", argv[1]);
            break;
    }
}

static void F_Command_ShowDNSServInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    IP_MULTI_ADDRESS ipDNS;
    IP_ADDRESS_TYPE addrType;
    uint8_t         *hostName;
    size_t          ipcount=0;
    size_t          index;
    size_t          nEntries;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    TCPIP_DNSS_RESULT res;
    uint32_t    ttlTime=0;
    char        hostBuff[16];
#if defined(TCPIP_STACK_USE_IPV6)
    uint8_t     addrBuf[44];
#endif    

    if (argc != 3) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dnsserv info <hostname> | <all>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Help: display the DNS cache entry details \r\n");
        return;
    }
    hostName = (uint8_t*)argv[2];
    if(strcmp((char*)argv[2],"all")==0)
    {
        index = 0U;
        nEntries = 0U;

        while(true)
        {
            res = TCPIP_DNSS_AddressCntGet(index, hostBuff, sizeof(hostBuff), &ipcount);
            if(res == TCPIP_DNSS_RES_OK)
            {
                if(nEntries == 0U)
                {   // 1st entry, show header
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam,"HostName        IPv4/IPv6Count\r\n");
                }

                (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s       %d\r\n", hostBuff, ipcount);
                nEntries++;
                index++;
            }
            else
            {
                break;
            }
        }

        if(nEntries == 0U)
        {   // no entries found
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No DNS Server Cache entry \r\n");
        }
        return;
    }

    addrType = IP_ADDRESS_TYPE_IPV4;
    index = 0U;
    nEntries = 0U;
    while(true)
    {
        res = TCPIP_DNSS_EntryGet((uint8_t*)hostName, addrType, index, &ipDNS, &ttlTime);
        if(res == TCPIP_DNSS_RES_OK)
        {
            if(nEntries == 0U)
            {   // for the 1st entry print the header
                (*pCmdIO->pCmdApi->msg)(cmdIoParam,"HostName\t\tIPv4 Address\t\tTTLTime \r\n");
            }
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s\t\t%d.%d.%d.%d\t\t%d\r\n",hostName,ipDNS.v4Add.v[0],ipDNS.v4Add.v[1],
                    ipDNS.v4Add.v[2],ipDNS.v4Add.v[3],ttlTime);
            nEntries++;
            index++;
        }
        else
        {
            break;
        }
    }

    if(nEntries == 0U)
    {   // no entries found
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No IPv4 DNS Server Cache entry \r\n");
    }

#if defined(TCPIP_STACK_USE_IPV6)
    addrType = IP_ADDRESS_TYPE_IPV6;
    index = 0U;
    nEntries = 0U;

    while(true)
    {
        res = TCPIP_DNSS_EntryGet((uint8_t*)hostName, addrType, index, &ipDNS, &ttlTime);
        if(res == TCPIP_DNSS_RES_OK)
        {
            if(nEntries == 0U)
            {   // for the 1st entry print the header
                (*pCmdIO->pCmdApi->msg)(cmdIoParam,"HostName\t\tIPv6 Address\t\tTTLTime \r\n");
            }

            (void)TCPIP_Helper_IPv6AddressToString(&ipDNS.v6Add,(char*)addrBuf,sizeof(addrBuf));
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s       %s      %d\r\n",hostName,addrBuf,ttlTime);
            nEntries++;
            index++;
        }
        else
        {
            break;
        }
    }
    if(nEntries == 0U)
    {   // no entries found
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No IPv6 DNS Server Cache entry \r\n");
    }

#endif
}
#endif

static void F_Command_BIOSNameSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    const char* msg;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setbios <interface> <x.x.x.x> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setbios PIC32INT MCHPBOARD_29 \r\n");
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return;
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
}

static void F_Command_MACAddressSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    TCPIP_NET_HANDLE netH;
    TCPIP_MAC_ADDR macAddr;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 3) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: setmac <interface> <x:x:x:x:x:x> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: setmac PIC32INT aa:bb:cc:dd:ee:ff \r\n");
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "argv[2]: %s\r\n", argv[2]);

    if (!TCPIP_Helper_StringToMACAddress(argv[2], macAddr.v)) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid MAC address string \r\n");
        return;
    }

    if(!TCPIP_STACK_NetAddressMacSet(netH, &macAddr)) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Set MAC address failed\r\n");
        return;
    }

}

#if defined(TCPIP_STACK_USE_TFTP_SERVER)
static void F_Command_TFTPServerOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // tftps <interface> <start/stop> <add-type>
    // tftps status

    int  opCode = 0;        // 0- none; 1 - start; 2 - stop
    bool opRes;
    IP_ADDRESS_TYPE ipType = IP_ADDRESS_TYPE_ANY;
    TCPIP_NET_HANDLE netH = NULL;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool printUsage = true;

    while(argc >= 2)
    {
        if((strcmp(argv[1], "status") == 0))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "TFTPS - Number of clients running: %d \r\n", TCPIP_TFTPS_ClientsNumber());
            return;
        }

        if(argc < 3)
        {
            break;
        }

        netH = TCPIP_STACK_NetHandleGet(argv[1]);
        if (netH == NULL)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "TFTPS - Unknown interface\r\n");
            return;
        }

        if (strcmp(argv[2], "start") == 0)
        {
            if(TCPIP_TFTPS_IsEnabled())
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "TFTPS - already running\r\n");
                return;
            }

            opCode = 1;
        }
        else if (strcmp(argv[2], "stop") == 0)
        {
            if(!TCPIP_TFTPS_IsEnabled())
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "TFTPS - already stopped\r\n");
                return;
            }
            opCode = 2;
        }
        else
        {
            break;
        }

        if(argc > 3)
        {
            uint32_t type = 0UL;
            (void)FC_Str2UL(argv[3], 10, &type);
            if(type == 4U)
            {
                ipType = IP_ADDRESS_TYPE_IPV4;
            }
            else if(type == 6U)
            {
                ipType = IP_ADDRESS_TYPE_IPV6;
            }
            else if(type == 0U)
            {
                ipType = IP_ADDRESS_TYPE_ANY;
            }
            else
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "TFTPS - Invalid address type\r\n");
                return;
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

}
#endif  

#if (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)
static void F_Command_NetworkOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    bool res = false;
    TCPIP_NET_HANDLE netH;
#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
    TCPIP_COMMAND_STG_DCPT*   pDcpt;
    TCPIP_NETWORK_CONFIG*     pNetConf;
#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
    TCPIP_NETWORK_CONFIG ifConf, *pIfConf;
    SYS_MODULE_OBJ      tcpipStackObj;
    TCPIP_STACK_INIT    tcpip_init_data = {{0}};
    uint16_t net_ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: if <interface> <down/up> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: if PIC32INT down \r\n");
        return;
    }

    netH = TCPIP_STACK_NetHandleGet(argv[1]);

    if (netH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Unknown interface specified \r\n");
        return;
    }

    net_ix = (uint16_t)TCPIP_STACK_NetIndexGet(netH);

    if (strcmp(argv[2], "up") == 0)
    {
        if(TCPIP_STACK_NetIsUp(netH))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "This interface already up\r\n");
            return;
        }

        // get the data passed at initialization
        tcpipStackObj = TCPIP_STACK_Initialize(0, NULL);
        (void)TCPIP_STACK_InitializeDataGet(tcpipStackObj, &tcpip_init_data);
        if(tcpip_init_data.pNetConf == NULL)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Operation failed. No configuration\r\n");
            return;
        }

        pIfConf = &ifConf;
        (void)memcpy(pIfConf, tcpip_init_data.pNetConf + net_ix, sizeof(*pIfConf));

#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
        if(pCmdStgDcpt != NULL) 
        {
            // get the saved network configuration
            pDcpt = pCmdStgDcpt + net_ix;
            if(pDcpt->stgSize != 0U)
            {   // saved config is valid; restore
                pNetConf = TCPIP_STACK_NetConfigSet(&pDcpt->netDcptStg, pDcpt->restoreBuff, sizeof(pDcpt->restoreBuff), NULL);
                if(pNetConf != NULL)
                {   // use the saved data
                    pIfConf = pNetConf;
                }
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Interface up: configuration " );
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, pNetConf != NULL ? "restored\r\n" : "restore failed!\r\n");
            }
        }
#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)

        // change the power mode to FULL
        pIfConf->powerMode = TCPIP_STACK_IF_POWER_FULL;
        res = TCPIP_STACK_NetUp(netH, pIfConf);
    }
    else if (strcmp(argv[2], "down") == 0)
    {
        if(TCPIP_STACK_NetIsUp(netH) == false)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "This interface already down\r\n");
            return;
        }

#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
        if(pCmdStgDcpt != NULL) 
        {
            // get the last network configuration so we use it when
            // restart the stack/interface 
            pDcpt = pCmdStgDcpt + net_ix;
            pDcpt->stgSize = TCPIP_STACK_NetConfigGet(netH, &pDcpt->netDcptStg, sizeof(pDcpt->netDcptStg), NULL);

            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Interface down: configuration saved\r\n");
        }
#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)

        res = TCPIP_STACK_NetDown(netH);
    } 
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Wrong parameter specified \r\n");
        return;
    }

    if (res == true)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Operation successful!\r\n");
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Operation failed!\r\n");
    }

}
#endif  // (M_TCPIP_STACK_IF_UP_DOWN_OPERATION != 0)

#if (M_TCPIP_STACK_DOWN_OPERATION != 0)
static void F_Command_StackOnOff(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
    TCPIP_NET_HANDLE netH;
    size_t           netIx;
    TCPIP_COMMAND_STG_DCPT  *pDcpt;
    TCPIP_NETWORK_CONFIG    *pCurrConf, *pDstConf;
#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
    SYS_MODULE_OBJ          tcpipStackObj;     // stack handle
    const char              *msg;
    TCPIP_STACK_INIT        tcpipInitData;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: stack <up/down> <preserve>\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: stack down preserve\r\n");
        return;
    }


    if (strcmp(argv[1], "up") == 0)
    {
        // try to get a stack handle
        tcpipStackObj = TCPIP_STACK_Initialize(0, NULL);
        if ( tcpipStackObj != SYS_MODULE_OBJ_INVALID)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Stack already up!\r\n");
            return;
        }
        // check the saved init data when the stack went down
        if(pCmdTcpipInitData == NULL)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Turn Stack down and then up!\r\n");
            return;
        }

        // copy of the init data; use as default
        tcpipInitData = *pCmdTcpipInitData;

#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
        if(pCmdStgDcpt != NULL && pCmdNetConf != NULL) 
        {
            // get the saved network configuration
            pDcpt = pCmdStgDcpt + 0;
            pDstConf = pCmdNetConf + 0; 
            pCurrConf = NULL;
            for (netIx = 0; netIx < initialNetIfs; netIx++)
            {
                if(pDcpt->stgSize != 0U)
                {   // saved config is valid; restore
                    pCurrConf = TCPIP_STACK_NetConfigSet(&pDcpt->netDcptStg, pDcpt->restoreBuff, sizeof(pDcpt->restoreBuff), NULL);
                }
                else
                {   // don't have a config to restore
                    pCurrConf = NULL;
                }

                if(pCurrConf == NULL)
                {   // restore failed
                    break;
                }
                else
                {   // save into array for the stack initialization
                    // force the interface start with power up
                    pCurrConf->powerMode = TCPIP_STACK_IF_POWER_FULL;
                    (void)memcpy(pDstConf, pCurrConf, sizeof(*pDstConf));
                }

                pDcpt++;
                pDstConf++;
            }

            if(pCurrConf != NULL)
            {   // success
                tcpipInitData.pNetConf = pCmdNetConf;
                tcpipInitData.nNets = initialNetIfs;
                msg = "Stack up: configuration restored\r\n";
            }
            else
            {
                msg = "Stack up: configuration restore failed\r\n";
            }

            (*pCmdIO->pCmdApi->msg)(cmdIoParam, msg);
        }
#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Restarting the stack with %d interface(s)\r\n", tcpipInitData.nNets);

        tcpipStackObj = TCPIP_STACK_Initialize(0, &tcpipInitData);     // init the stack
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
    else if (strcmp(argv[1], "down") == 0)
    {
        // try to get a handle
        tcpipStackObj = TCPIP_STACK_Initialize(0, NULL);
        if ( tcpipStackObj == SYS_MODULE_OBJ_INVALID)
        {
            msg = "Stack down: cannot get a stack handle\r\n";
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, msg);
        }
        else
        {
            // store the data passed at initialization
            (void)TCPIP_STACK_InitializeDataGet(tcpipStackObj, &cmdTcpipInitData);
            pCmdTcpipInitData = &cmdTcpipInitData;

#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
            tcpipCmdPreserveSavedInfo = false;
            if(argc == 3 && strcmp(argv[2], "preserve") == 0)
            {
                if(pCmdStgDcpt != NULL) 
                {
                    // get the last network configuration so we use it when
                    // restart the stack/interface 
                    pDcpt = pCmdStgDcpt + 0;
                    for (netIx = 0; netIx < initialNetIfs; netIx++)
                    {
                        netH = TCPIP_STACK_IndexToNet(netIx);
                        pDcpt->stgSize = TCPIP_STACK_NetConfigGet(netH, &pDcpt->netDcptStg, sizeof(pDcpt->netDcptStg), NULL);
                        pDcpt++;
                    }

                    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Stack down: configuration saved\r\n");
                    tcpipCmdPreserveSavedInfo = true;
                }
            }
#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)

            TCPIP_STACK_Deinitialize(tcpipStackObj);
#if defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
            tcpipCmdPreserveSavedInfo = false;          // make sure it doesn't work the next time
#endif  // defined(M_TCPIP_STACK_COMMANDS_STORAGE_ENABLE)
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Stack down done\r\n");
        }
    }
    else
    {
        // do nothing
    }
}
#endif  // (M_TCPIP_STACK_DOWN_OPERATION != 0)

static void F_Command_HeapInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)    
    size_t  ix, nEntries;
    TCPIP_HEAP_TRACE_ENTRY    tEntry;
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)    
    size_t  heapSize;
    TCPIP_STACK_HEAP_HANDLE heapH;
    const char* typeMsg;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint32_t hType, startType, endType;
    bool hasArgs = false;
    static const char* heapTypeStr[TCPIP_STACK_HEAP_TYPES] = 
    {
        0,              // TCPIP_STACK_HEAP_TYPE_NONE
        "internal",     // TCPIP_STACK_HEAP_TYPE_INTERNAL
        "pool",         // TCPIP_STACK_HEAP_TYPE_POOL
        "external",     // TCPIP_STACK_HEAP_TYPE_EXTERNAL
    };


    if (argc > 1)
    {   // there is an arg
        hType = 0UL;
        (void)FC_Str2UL(argv[1], 10, &hType);
        if(hType == (unsigned int)TCPIP_STACK_HEAP_TYPE_NONE || hType >= (unsigned int)TCPIP_STACK_HEAP_TYPES)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Unknown heap type. Use: [1, %d]\r\n", (unsigned int)TCPIP_STACK_HEAP_TYPES - 1U);
            return;
        }
        // valid
        startType = hType;
        endType = hType + 1U;
        hasArgs = true;
    }
    else
    {   // consider all types
        startType = (unsigned int)TCPIP_STACK_HEAP_TYPE_NONE + 1U;
        endType = (unsigned int)TCPIP_STACK_HEAP_TYPES;
    }

    // display info for each type
    for(hType = startType; hType < endType; hType++)
    {
        typeMsg = heapTypeStr[hType];
        heapH = TCPIP_STACK_HeapHandleGet((TCPIP_STACK_HEAP_TYPE)hType, 0U);
        if(heapH == NULL)
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
        size_t nTraces = TCPIP_HEAP_TraceGetEntriesNo(heapH, true);
        if(nTraces != 0U)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Trace info: \r\n");
            nEntries = TCPIP_HEAP_TraceGetEntriesNo(heapH, false);
            for(ix = 0; ix < nEntries; ix++)
            {
                if(TCPIP_HEAP_TraceGetEntry(heapH, ix, &tEntry))
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tModule: %4d, nAllocs: %6d, nFrees: %6d\r\n", tEntry.moduleId, tEntry.nAllocs, tEntry.nFrees);
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t\ttotAllocated: %6d, currAllocated: %6d, totFailed: %6d, maxFailed: %6d\r\n", tEntry.totAllocated, tEntry.currAllocated, tEntry.totFailed, tEntry.maxFailed);
                }

            }
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No Trace info exists.\r\n");
        }
#else
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No Trace info exists.\r\n");
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)    

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
        nEntries = TCPIP_HEAP_DistGetEntriesNo(heapH);
        if(nEntries != 0U)
        {
            size_t  modIx;
            TCPIP_HEAP_DIST_ENTRY distEntry;
            size_t currLowHitMem = 0U;
            size_t currHiHitMem = 0U;

            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "TCPIP Heap distribution: \r\n");

            for(ix = 0; ix < nEntries; ix++)
            {
                (void)TCPIP_HEAP_DistGetEntry(heapH, ix, &distEntry);

                int entryPrint = 0;
                struct moduleDist* pMDist = distEntry.modDist;
                for(modIx = 0; modIx < sizeof(distEntry.modDist)/sizeof(*distEntry.modDist); modIx++)
                {
                    if(pMDist->modHits != 0U)
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
                    pMDist++;
                }
                if(distEntry.gHits != 0U)
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t mod: xx \thits: %d, \r\n", distEntry.gHits);
                }
            }

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "curr Low Lim: %d, curr Hi Lim: %d, max Free: %d, min Free: %d\r\n", currLowHitMem, currHiHitMem, heapSize - currLowHitMem, heapSize - currHiHitMem);
        }
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

    }

}

static void F_Command_MacInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t                  netNo, netIx;
    TCPIP_NET_HANDLE        netH;
    TCPIP_MAC_RX_STATISTICS rxStatistics;
    TCPIP_MAC_TX_STATISTICS txStatistics;
    TCPIP_MAC_STATISTICS_REG_ENTRY  regEntries[50];
    TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntry;
    size_t                  jx, hwEntries;
    char                    entryName[sizeof(pRegEntry->registerName) + 1U];
    const char*             netName;

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc != 1) {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: macinfo \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: macinfo \r\n");
        return;
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
            entryName[sizeof(entryName) - 1U] = '\0';
            pRegEntry = regEntries;
            for(jx = 0; jx < hwEntries && jx < sizeof(regEntries)/sizeof(*regEntries); jx++)
            {
                (void)strncpy(entryName, pRegEntry->registerName, sizeof(entryName) - 1U);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t %s: 0x%x\r\n", entryName, pRegEntry->registerValue);
                pRegEntry++;
            }
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\tnot supported\r\n");
        }

    }

}

#if defined(TCPIP_STACK_USE_DNS)
static void TCPIPCmdDnsTask(void)
{
    TCPIP_DNS_RESULT  dnsRes;
    size_t ipv4Index = 0U, ipv6Index = 0U;
    size_t      nIPv4Entries;
    IPV4_ADDR   ip4Address;
    size_t      nIPv6Entries;
    IPV6_ADDR   ip6Address;
    uint8_t     addrBuf[44];
    uint32_t    timeout = 0U;

    switch(tcpipCmdStat)
    {
        case TCPIP_DNS_LOOKUP_CMD_GET:
            dnsRes = TCPIP_DNS_Resolve(dnslookupTargetHost, dnsType);
            if(dnsRes != TCPIP_DNS_RES_OK && dnsRes != TCPIP_DNS_RES_PENDING && dnsRes != TCPIP_DNS_RES_NAME_IS_IPADDRESS)
            {   // some other error
                (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "DNS Lookup: DNS failure for %s, err: %d\r\n", dnslookupTargetHost, dnsRes);
                tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
            }
            else
            {   // wait some more
                tcpipCmdStat = TCPIP_DNS_LOOKUP_CMD_WAIT;
                dnsLookUpStartTick = SYS_TMR_TickCountGet();
            }
            break;

        case TCPIP_DNS_LOOKUP_CMD_WAIT:
            dnsRes = TCPIP_DNS_IsResolved(dnslookupTargetHost, NULL, IP_ADDRESS_TYPE_ANY);
            timeout = (SYS_TMR_TickCountGet() - dnsLookUpStartTick) / sysFreq;
            if(timeout >= ((uint32_t)TCPIP_DNS_CLIENT_SERVER_TMO / 2U))
            {   // timeout
                (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "DNS Lookup: request timeout.\r\n");
                tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
                break;
            }
            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {   // operation in progress
                break;
            }
            else if((int)dnsRes < 0 )
            {   // timeout or some other DNS error
                (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "DNS Lookup: DNS failure for %s, err: %d\r\n", dnslookupTargetHost, dnsRes);
                tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
                break;
            }
            else
            {
                // got the DNS result
            }
            (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, 0);
            tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
            // success
            (*pTcpipCmdDevice->pCmdApi->msg)(dnsLookupCmdIoParam, "Lookup Answer:\r\n----------------------\r\n");
            nIPv4Entries = TCPIP_DNS_GetIPAddressesNumber(dnslookupTargetHost,IP_ADDRESS_TYPE_IPV4);
            nIPv6Entries = TCPIP_DNS_GetIPAddressesNumber(dnslookupTargetHost,IP_ADDRESS_TYPE_IPV6);
            if((nIPv4Entries == 0U) && (nIPv6Entries == 0U))
            {
                (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "No Lookup entry for [%s]\r\n",dnslookupTargetHost);
                break;
            }
            while(true)
            {
                if(ipv4Index < nIPv4Entries)
                {
                    ip4Address.Val = 0U;
                    (void)TCPIP_DNS_GetIPv4Addresses(dnslookupTargetHost, ipv4Index, &ip4Address, 1U);
                    (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "[%s] A IPv4 Address: %d.%d.%d.%d\r\n",dnslookupTargetHost,ip4Address.v[0],
                            ip4Address.v[1],ip4Address.v[2],ip4Address.v[3]);
                    ipv4Index++;
                }
                else if(ipv6Index < nIPv6Entries)
                {
                    (void)TCPIP_DNS_GetIPv6Addresses(dnslookupTargetHost, ipv6Index, &ip6Address, 1U);
                    (void)memset(addrBuf,0,sizeof(addrBuf));
                    (void)TCPIP_Helper_IPv6AddressToString(&ip6Address,(char*)addrBuf,sizeof(addrBuf));
                    (*pTcpipCmdDevice->pCmdApi->print)(dnsLookupCmdIoParam, "[%s] AAAA IPv6 Address: %s\r\n",dnslookupTargetHost,addrBuf);
                    ipv6Index++;
                }
                else
                {
                    break;
                }
            }
            break;

        default:
            // do nothing
            break;
    }
}
#endif

#if defined(M_TCPIP_COMMAND_PING4)
static void F_Command_Ping(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int     currIx;    
    TCPIP_COMMANDS_STAT  newCmdStat;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ping Usage: ping <stop>/<name/address> <i interface> <n nPings> <t msPeriod> <s size>\r\n");
        return;
    }

    if(strcmp(argv[1], "stop") == 0)
    {
        if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE)
        {
            F_PingStop(pCmdIO, cmdIoParam);
        }
        return;
    }

    if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ping: command in progress. Retry later.\r\n");
        return;
    }

    // get the host
    if(TCPIP_Helper_StringToIPAddress(argv[1], &icmpTargetAddr))
    {
        (void)strncpy(icmpTargetAddrStr, argv[1], sizeof(icmpTargetAddrStr) - 1U);
        icmpTargetAddrStr[sizeof(icmpTargetAddrStr) - 1U] = '\0';
        icmpTargetHost[0] = '\0';
        newCmdStat = TCPIP_PING_CMD_START_PING;
    }
    else
    {   // assume host address
        if(strlen(argv[1]) > sizeof(icmpTargetHost) - 1U)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ping: Host name too long. Retry.\r\n");
            return;
        }
        (void)strcpy(icmpTargetHost, argv[1]);
        newCmdStat = TCPIP_PING_CMD_DNS_GET;
    }

    // get additional parameters, if any
    //
    icmpReqNo = 0U;
    icmpReqDelay = 0U;

    currIx = 2;

    while(currIx + 1 < argc)
    { 
        char* param = argv[currIx];
        char* paramVal = argv[currIx + 1];

        if(strcmp(param, "i") == 0)
        {
            if((icmpNetH = TCPIP_STACK_NetHandleGet(paramVal)) == NULL)
            {   // use default interface
                icmpNetH = TCPIP_STACK_NetDefaultGet();
            }
        }
        else if(strcmp(param, "n") == 0)
        {
             (void)FC_Str2UL(paramVal, 10, &icmpReqNo);
        }
        else if(strcmp(param, "t") == 0)
        {
            (void)FC_Str2UL(paramVal, 10, &icmpReqDelay);
        }
        else if(strcmp(param, "s") == 0)
        {
            uint32_t pingSize = 0UL;
            (void)FC_Str2UL(paramVal, 10, &pingSize);
            if(pingSize > 0UL && pingSize <= sizeof(icmpPingBuff))
            {
                icmpPingSize = (uint16_t)pingSize;
            }
            else
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "Ping: Data size too big. Max: %d. Retry\r\n", sizeof(icmpPingBuff));
                return;
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

    icmpSequenceNo = (uint16_t)SYS_RANDOM_PseudoGet();
    icmpIdentifier = (uint16_t)SYS_RANDOM_PseudoGet();

    if(icmpReqNo == 0U)
    {
        icmpReqNo = (size_t)TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS;
    }
    if(icmpReqDelay == 0U)
    {
        icmpReqDelay = (uint32_t)TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY;
    }

    // convert to ticks
    if(icmpReqDelay < (uint32_t)TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY)
    {
        icmpReqDelay = (uint32_t)TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY;
    }

    pTcpipCmdDevice = pCmdIO;
    icmpCmdIoParam = cmdIoParam; 
    icmpAckRecv = 0U;
    icmpReqCount = 0U;

    (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, (int16_t)icmpReqDelay);

}

static void CommandPingHandler(const  TCPIP_ICMP_ECHO_REQUEST* pEchoReq, TCPIP_ICMP_REQUEST_HANDLE iHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result, const void* param)
{
    char addBuff[20];

    if(result == TCPIP_ICMP_ECHO_REQUEST_RES_OK)
    {   // reply has been received
        uint32_t errorMask = 0U;     // error mask:
        // 0x1: wrong id
        // 0x2: wrong seq
        // 0x4: wrong target
        // 0x8: wrong size
        // 0x10: wrong data
        //
        if(pEchoReq->identifier != icmpIdentifier)
        {
            errorMask |= 0x1U;
        }

        if(pEchoReq->sequenceNumber != icmpSequenceNo)
        {
            errorMask |= 0x2U;
        }

        if(pEchoReq->dataSize != icmpPingSize)
        {
            errorMask |= 0x8U;
        }

        // check the data
        size_t ix;
        uint16_t checkSize = pEchoReq->dataSize < icmpPingSize ? pEchoReq->dataSize : icmpPingSize;
        uint8_t* pSrc = icmpPingBuff;
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
            (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: wrong reply received. Mask: 0x%2x\r\n", errorMask);
        }
        else
        {   // good reply
            uint32_t pingTicks = SYS_TMR_TickCountGet() - icmpStartTick;
            uint32_t pingMs = (pingTicks * 1000U) / sysFreq;
            if(pingMs == 0U)
            {
                pingMs = 1U;
            }

            addBuff[0] = '\0';
            (void)TCPIP_Helper_IPAddressToString(&pEchoReq->targetAddr, addBuff, sizeof(addBuff));

            (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: reply[%d] from %s: time = %dms\r\n", ++icmpAckRecv, addBuff, pingMs);
        }
    }
    else
    {
#if (M_TCPIP_COMMAND_PING4_DEBUG != 0)
        (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: request aborted by ICMP with result %d\r\n", result);
#endif  // (M_TCPIP_COMMAND_PING4_DEBUG != 0)
    }
    // one way or the other, request is done
    icmpReqHandle = NULL;
}

#endif  // defined(M_TCPIP_COMMAND_PING4)

#if defined(M_TCPIP_COMMAND_PING6)
static void F_Command_IPv6_Ping(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint32_t size = 0U;
    TCPIP_NET_HANDLE netH;
    int     argIx;

    if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ping6: command in progress. Retry later.\r\n");
        return;
    }

    if((argc < 2) ||(argc > 4))
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ping6 <net> <x::x:x:x:x> <size> \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ping6 fe80::23:2222:3333:1234 1500\r\n");
        return;
    }
 // check the 1st parameter type
    netH = TCPIP_STACK_NetHandleGet(argv[1]);
    if(netH == NULL)
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
            return;
        }
    }

    if(TCPIP_Helper_StringToIPv6Address(argv[argIx], &icmpv6TargetAddr))
    {
        (void)strncpy(icmpTargetAddrStr, argv[argIx], sizeof(icmpTargetAddrStr) - 1U);
        icmpTargetAddrStr[sizeof(icmpTargetAddrStr) - 1U] = '\0';
        icmpTargetHost[0] = '\0';
        tcpipCmdStat = TCPIP_SEND_ECHO_REQUEST_IPV6;
        (void)memset(icmpv6TargetAddrStr,0,sizeof(icmpv6TargetAddrStr));
        if(strlen(argv[argIx]) <= sizeof(icmpv6TargetAddrStr))
        {
            (void)strcpy(icmpv6TargetAddrStr,argv[argIx]);
        }
    }
     else
    {   // assume host address
        if(strlen(argv[argIx]) > sizeof(icmpTargetHost) - 1U)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ping6: Host name too long. Retry.\r\n");
            return;
        }
        (void)strcpy(icmpTargetHost, argv[argIx]);
        tcpipCmdStat = TCPIP_PING6_CMD_DNS_GET;
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "ping6: resolving host: %s\r\n", icmpTargetHost);
    }
    
     size = 0U;
    if(argv[argIx + 1] != NULL)
    {
        (void)FC_Str2UL(argv[argIx+1], 10, &size);
    }
    
     pingPktSize = size;

    if(hIcmpv6 == NULL)
    {
        if((hIcmpv6 = TCPIP_ICMPV6_CallbackRegister( &CommandPing6Handler)) == NULL)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ping6: Failed to register ICMP handler\r\n");
            return;
        }
    }

    icmpSequenceNo = (uint16_t)SYS_RANDOM_PseudoGet();
    icmpIdentifier = (uint16_t)SYS_RANDOM_PseudoGet();
    icmpReqNo = 0U;
    icmpReqDelay = 0U;
    if(icmpReqNo == 0U)
    {
        icmpReqNo = (size_t)TCPIP_STACK_COMMANDS_ICMPV6_ECHO_REQUESTS;
    }
    if(icmpReqDelay == 0U)
    {
        icmpReqDelay = (uint32_t)TCPIP_STACK_COMMANDS_ICMPV6_ECHO_REQUEST_DELAY;
    }

    // convert to ticks
    if(icmpReqDelay < (uint32_t)TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY)
    {
        icmpReqDelay = (uint32_t)TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY;
    }

    pTcpipCmdDevice = pCmdIO;
    icmpCmdIoParam = cmdIoParam;
    icmpAckRecv = 0U;
    icmpReqCount = 0U;

    (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, (int16_t)icmpReqDelay);
}

static void CommandPing6Handler(TCPIP_NET_HANDLE hNetIf,uint8_t type, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, void * data)
{
    char addBuff[42];

    if(tcpipCmdStat == TCPIP_CMD_STAT_IDLE)
    {
        return; // not our reply?
    }

    if(type != (uint8_t)ICMPV6_INFO_ECHO_REPLY)
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
        uint32_t pingMs = (pingTicks * 1000U) / sysFreq;
        if(pingMs == 0U)
        {
            pingMs = 1U;
        }
        addBuff[0] = '\0';
        (void)TCPIP_Helper_IPv6AddressToString(remoteIP, addBuff, sizeof(addBuff));

        (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "ping6: reply from [%s] time = %dms\r\n", addBuff, pingMs);
        icmpAckRecv++;
    }

}
#endif  // defined(M_TCPIP_COMMAND_PING6)


#if defined(M_TCPIP_COMMAND_PING4) || defined(M_TCPIP_COMMAND_PING6)
static void F_PingStop(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam)
{
#if defined(M_TCPIP_COMMAND_PING4)
    if(icmpReqHandle != NULL)
    {
#if (M_TCPIP_COMMAND_PING4_DEBUG == 0)
        (void)TCPIP_ICMP_EchoRequestCancel(icmpReqHandle);
#else
        if(TCPIP_ICMP_EchoRequestCancel(icmpReqHandle) != ICMP_ECHO_OK)
        {   // this should NOT happen!
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ping stop failed!\r\n");
        }
        else
        { 
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ping: request aborted by tcpip CMD: stop!\r\n");
        }
#endif  // (M_TCPIP_COMMAND_PING4_DEBUG == 0)

        icmpReqHandle = NULL;
    }
#endif  // defined(M_TCPIP_COMMAND_PING4)

    (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, 0);
    tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
    if(pCmdIO != NULL)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Ping: done. Sent %d requests, received %d replies.\r\n", icmpReqCount, icmpAckRecv);
    }
    pTcpipCmdDevice = NULL;
}


static void TCPIPCmdPingTask(void)
{
#if defined(M_TCPIP_COMMAND_PING4)
    ICMP_ECHO_RESULT echoRes;
    TCPIP_ICMP_ECHO_REQUEST echoRequest;
    bool cancelReq, newReq;
#endif  // defined(M_TCPIP_COMMAND_PING4)
#if defined(M_TCPIP_COMMAND_PING6)
    bool ipv6EchoRes=false;
#endif
    TCPIP_DNS_RESULT  dnsRes;
    bool killIcmp = false;
       
    switch(tcpipCmdStat)
    {
#if defined(M_TCPIP_COMMAND_PING4)
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
            dnsRes = TCPIP_DNS_IsNameResolved(icmpTargetHost, &icmpTargetAddr, NULL);
            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {   // operation in progress
                break;
            }
            else if((int)dnsRes < 0 )
            {   // timeout or some other DNS error
                (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: DNS failure for %s\r\n", icmpTargetHost);
                killIcmp = true;
                break;
            }
            else
            {
                // success
            }
 
            (void)TCPIP_Helper_IPAddressToString(&icmpTargetAddr, icmpTargetAddrStr, sizeof(icmpTargetAddrStr));
            tcpipCmdStat = TCPIP_PING_CMD_START_PING;            
            break;

        case TCPIP_PING_CMD_START_PING:
            icmpStartTick = 0;  // try to start as quickly as possible
            tcpipCmdStat = TCPIP_PING_CMD_DO_PING;            
            break;

        case TCPIP_PING_CMD_DO_PING:
            if(icmpReqCount == icmpReqNo)
            {   // no more requests to send
                killIcmp = true;
                break;
            }

            // check if time for another request
            cancelReq = newReq = false;

            if(SYS_TMR_TickCountGet() - icmpStartTick > (sysFreq * icmpReqDelay) / 1000U)
            {
                cancelReq = icmpReqCount != icmpAckRecv && icmpReqHandle != NULL;    // cancel if there is another one ongoing
                newReq = true;
            }
            else if(icmpReqCount != icmpAckRecv)
            {   // no reply received to the last ping 
                if(SYS_TMR_TickCountGet() - icmpStartTick > (sysFreq * (uint32_t)TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT) / 1000U)
                {   // timeout
#if (M_TCPIP_COMMAND_PING4_DEBUG != 0)
                    (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: request timeout.\r\n");
#endif  // (M_TCPIP_COMMAND_PING4_DEBUG != 0)
                    cancelReq = icmpReqHandle != NULL;    // cancel if there is another one ongoing
                    newReq = true;
                }
                // else wait some more
            }
            else
            {
                // do nothing
            }

            if(cancelReq)
            {
#if (M_TCPIP_COMMAND_PING4_DEBUG != 0)
                if(TCPIP_ICMP_EchoRequestCancel(icmpReqHandle) != ICMP_ECHO_OK)
                {   // this should NOT happen!
                    (*pTcpipCmdDevice->pCmdApi->msg)(icmpCmdIoParam, "Ping cancel failed!!!\r\n");
                }
                else
                {
                    (*pTcpipCmdDevice->pCmdApi->msg)(icmpCmdIoParam, "Ping: request aborted by tcpip CMD: tmo!\r\n");
                }
#else
                (void)TCPIP_ICMP_EchoRequestCancel(icmpReqHandle);
#endif  // (M_TCPIP_COMMAND_PING4_DEBUG != 0)
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
            echoRequest.callback =  &CommandPingHandler;
            echoRequest.param = NULL;

            {
                size_t ix;
                uint8_t* pBuff = icmpPingBuff;
                for(ix = 0; ix < icmpPingSize; ix++)
                {
                    *pBuff++ = (uint8_t)SYS_RANDOM_PseudoGet();
                }
            }

            echoRes = TCPIP_ICMP_EchoRequest (&echoRequest, &icmpReqHandle);

            if((int)echoRes >= 0 )
            {
                icmpStartTick = SYS_TMR_TickCountGet();
                icmpReqCount++;
#if (M_TCPIP_COMMAND_PING4_DEBUG != 0)
                (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: sent request %d to: %s [%s]\r\n", icmpReqCount, icmpTargetHost, icmpTargetAddrStr);
#endif  // (M_TCPIP_COMMAND_PING4_DEBUG != 0)
            }
            else
            {
#if (M_TCPIP_COMMAND_PING4_DEBUG != 0)
                (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: failed to send request %d to: %s, error %d\r\n", icmpReqCount, icmpTargetAddrStr, echoRes);
#endif  // (M_TCPIP_COMMAND_PING4_DEBUG != 0)
                killIcmp = true;
            }

            break;
#endif  // defined(M_TCPIP_COMMAND_PING4)
#if defined(M_TCPIP_COMMAND_PING6)
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
            dnsRes = TCPIP_DNS_IsNameResolved(icmpTargetHost, NULL, &icmpv6TargetAddr);
            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {   // operation in progress
                break;
            }
            else if((int)dnsRes < 0 )
            {   // timeout or some other DNS error
                (*pTcpipCmdDevice->pCmdApi->print)(icmpCmdIoParam, "Ping: DNS failure for %s\r\n", icmpTargetHost);
                killIcmp = true;
                break;
            }
            else
            {
                // success
            }

            (void)TCPIP_Helper_IPv6AddressToString(&icmpv6TargetAddr, icmpv6TargetAddrStr, sizeof(icmpv6TargetAddrStr));
            tcpipCmdStat = TCPIP_SEND_ECHO_REQUEST_IPV6;
            break;
        case TCPIP_SEND_ECHO_REQUEST_IPV6:
            if(icmpReqCount != 0U && icmpAckRecv == 0U)
            {   // no reply received
                if(SYS_TMR_TickCountGet() - icmpStartTick > (sysFreq * (uint32_t)TCPIP_STACK_COMMANDS_ICMPV6_ECHO_TIMEOUT) / 1000U)
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
            ipv6EchoRes = TCPIP_ICMPV6_EchoRequestSend (icmpNetH, &icmpv6TargetAddr, ++icmpSequenceNo, icmpIdentifier, pingPktSize);

            if(ipv6EchoRes != false )
            {
                icmpStartTick = SYS_TMR_TickCountGet();
                if(icmpReqCount++ == 0U)
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
#endif  // defined(M_TCPIP_COMMAND_PING6)

        default:
            killIcmp = true;
            break;

    }

    if(killIcmp)
    {
        F_PingStop(pTcpipCmdDevice, icmpCmdIoParam);
    }

}

#endif  // defined(M_TCPIP_COMMAND_PING4) || defined(M_TCPIP_COMMAND_PING6)

void TCPIP_COMMAND_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred

#if  defined(M_TCPIP_COMMAND_PING4) || defined(M_TCPIP_COMMAND_PING6)
        if(TCPIP_CMD_STAT_PING_START <= tcpipCmdStat && tcpipCmdStat <= TCPIP_CMD_STAT_PING_STOP)
        {
            TCPIPCmdPingTask();
        }
#endif  // defined(M_TCPIP_COMMAND_PING4) || defined(M_TCPIP_COMMAND_PING6)

#if defined(TCPIP_STACK_USE_DNS)
        if(TCPIP_CMD_STAT_DNS_START <= tcpipCmdStat && tcpipCmdStat <= TCPIP_CMD_STAT_DNS_STOP)
        {
            TCPIPCmdDnsTask();
        }
#endif  // defined(TCPIP_STACK_USE_DNS)
#if defined(M_TCPIP_COMMANDS_MIIM)
        if(TCPIP_PHY_READ <= tcpipCmdStat && tcpipCmdStat <= TCPIP_PHY_WRITE_SMI)
        {
            TCPIPCmdMiimTask();
        }
#endif  // defined(M_TCPIP_COMMANDS_MIIM)
#if defined(M_TCPIP_STACK_PPP_ECHO_COMMAND)
        if(TCPIP_CMD_STAT_PPP_START <= tcpipCmdStat && tcpipCmdStat <= TCPIP_CMD_STAT_PPP_STOP)
        {
            TCPIPCmd_PppEchoTask();
        }
#endif  // defined(M_TCPIP_STACK_PPP_ECHO_COMMAND)
#if defined(M_TCPIP_COMMANDS_WSC)  
        if(tcpipCmdStat == TCPIP_CMD_STAT_WSC_OPEN)
        {
            Wsc_CmdTask();
        }
#endif  //  defined(M_TCPIP_COMMANDS_WSC)  
    }
}



#if defined(TCPIP_STACK_USE_IPV4)
#if (M_TCPIP_ARP_COMMANDS != 0)
static void F_Command_Arp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // arp <interface> <req/query/del/list/insert> <ipAddr> <macAddr>\r\n");
    //
    TCPIP_NET_HANDLE netH;
    IPV4_ADDR ipAddr;
    TCPIP_ARP_RESULT  arpRes;
    TCPIP_MAC_ADDR    macAddr;
    const char*       message;
    char        addrBuff[20];
    size_t      arpEntries, ix;
    TCPIP_ARP_ENTRY_QUERY arpQuery;
    
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    while(argc > 2)
    {
        netH = TCPIP_STACK_NetHandleGet(argv[1]);
        if (netH == NULL)
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
                (void)TCPIP_ARP_EntryQuery(netH, ix, &arpQuery);
                if(arpQuery.entryType == ARP_ENTRY_TYPE_PERMANENT || arpQuery.entryType == ARP_ENTRY_TYPE_COMPLETE)
                {
                    (void)TCPIP_Helper_IPAddressToString(&arpQuery.entryIpAdd, addrBuff, sizeof(addrBuff));
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "arp: IPv4 address: %s", addrBuff);
                    (void)TCPIP_Helper_MACAddressToString(&arpQuery.entryHwAdd, addrBuff, sizeof(addrBuff));
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
                    (void)TCPIP_Helper_IPAddressToString(&arpQuery.entryIpAdd, addrBuff, sizeof(addrBuff));
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "arp: IPv4 address: %s, queued\r\n", addrBuff);
                }
                else
                {
                    // do nothing
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
            bool skipMsg = false;
            switch(arpRes)
            {
                case ARP_RES_ENTRY_SOLVED:

                    (void)TCPIP_Helper_MACAddressToString(&macAddr, addrBuff, sizeof(addrBuff));
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "arp: resolved - IPv4 address: %s, MAC Address: %s\r\n", argv[3], addrBuff);
                    skipMsg = true;
                    break;

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
            if(skipMsg == false)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, message);
            }

            return;
        }

        if (strcmp(argv[2], "query") == 0)
        {   // query for an address
            arpRes = TCPIP_ARP_EntryGet(netH, &ipAddr, &macAddr, false);
            if(arpRes == ARP_RES_ENTRY_SOLVED)
            {
                (void)TCPIP_Helper_MACAddressToString(&macAddr, addrBuff, sizeof(addrBuff));
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

        if (strcmp(argv[2], "insert") == 0)
        {   // insert an address
            if (argc < 5 || !TCPIP_Helper_StringToMACAddress(argv[4], macAddr.v))
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Invalid MAC address string \r\n");
                return;
            }


            arpRes = TCPIP_ARP_EntrySet(netH, &ipAddr, &macAddr, true);
            if((int)arpRes >= 0)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "arp: Added MAC address %s for %s (%d)\r\n", argv[4], argv[3], arpRes);
            }
            else
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "arp: Failed to insert MAC address!\r\n");
            }
            return;
        }

        break;
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: arp interface list\r\n");
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: arp interface req/query/del/insert <ipAddr> <macAddr>\r\n");
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: arp eth0 req 192.168.1.105 \r\n");
}
#endif  // (M_TCPIP_ARP_COMMANDS != 0)
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(M_TCPIP_COMMANDS_HTTP_NET_SERVER)
static void F_Command_HttpNetInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t httpActiveConn, connIx, chunkIx;
    size_t httpOpenConn;
    TCPIP_HTTP_NET_CONN_INFO    httpInfo;
    TCPIP_HTTP_NET_CHUNK_INFO   httpChunkInfo[6];
    TCPIP_HTTP_NET_CHUNK_INFO*  pChunkInfo;
    TCPIP_HTTP_NET_STAT_INFO    httpStat;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: http info/stat/chunk/disconnect\r\n");
        return;
    }

    httpActiveConn = TCPIP_HTTP_NET_ActiveConnectionCountGet(&httpOpenConn);

    if(strcmp(argv[1], "info") == 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP connections - active: %d, open: %d\r\n", httpActiveConn, httpOpenConn);

        for(connIx = 0; connIx < httpOpenConn; connIx++)
        {
            if(TCPIP_HTTP_NET_InfoGet(connIx, &httpInfo))
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP conn: %d status: 0x%4x, port: %d, sm: 0x%4x, chunks: %d, chunk empty: %d, file empty: %d\r\n",
                       connIx, httpInfo.httpStatus, httpInfo.listenPort, httpInfo.connStatus, httpInfo.nChunks, httpInfo.chunkPoolEmpty, httpInfo.fileBufferPoolEmpty);
            }
            else
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP: failed to get info for conn: %d\r\n", connIx);
            }
        }
    }
    else if(strcmp(argv[1], "chunk") == 0)
    {
        for(connIx = 0; connIx < httpOpenConn; connIx++)
        {
            if(TCPIP_HTTP_NET_InfoGet(connIx, &httpInfo))
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP conn: %d, chunks: %d\r\n",  connIx, httpInfo.nChunks);
                if(TCPIP_HTTP_NET_ChunkInfoGet(connIx, httpChunkInfo, sizeof(httpChunkInfo)/sizeof(*httpChunkInfo)))
                {
                    pChunkInfo = httpChunkInfo;
                    for(chunkIx = 0; chunkIx < httpInfo.nChunks; chunkIx++)
                    {
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tHTTP chunk: %d flags: 0x%4x, status: 0x%4x, fName: %s\r\n", chunkIx, pChunkInfo->flags, pChunkInfo->status, pChunkInfo->chunkFName);
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tHTTP chunk: dyn buffers: %d, var Name: %s\r\n", pChunkInfo->nDynBuffers, pChunkInfo->dynVarName);
                        pChunkInfo++;
                    }
                    continue;
                }
            }

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP: failed to get info for conn: %d\r\n", connIx);
        }
    }
    else if(strcmp(argv[1], "stat") == 0)
    {
        if(TCPIP_HTTP_NET_StatGet(&httpStat))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP connections: %d, active: %d, open: %d\r\n", httpStat.nConns, httpStat.nActiveConns, httpStat.nOpenConns);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP pool empty: %d, max depth: %d, parse retries: %d\r\n", httpStat.dynPoolEmpty, httpStat.maxRecurseDepth, httpStat.dynParseRetry);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "HTTP: Failed to get status!\r\n");
        }
    }
    else if(strcmp(argv[1], "disconnect") == 0)
    {
        for(connIx = 0; connIx < httpOpenConn; connIx++)
        {
            TCPIP_HTTP_NET_CONN_HANDLE connHandle = TCPIP_HTTP_NET_ConnectionHandleGet(connIx);
            NET_PRES_SKT_HANDLE_T skt_h = TCPIP_HTTP_NET_ConnectionSocketGet(connHandle);
            (void)NET_PRES_SocketDisconnect(skt_h);
        }

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP disconnected %d connections, active: %d\r\n", httpOpenConn, httpActiveConn);
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "HTTP: unknown parameter\r\n");
    }




}
#if (TCPIP_HTTP_NET_SSI_PROCESS != 0)
static void F_Command_SsiNetInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t ssiEntries, ix;
    size_t nSSIVars;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    const char* varStr;
    const char* varName;
    TCPIP_HTTP_DYN_ARG_TYPE varType;

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ssi info\r\n");
        return;
    }

    if(strcmp(argv[1], "info") == 0)
    {
        ssiEntries = TCPIP_HTTP_NET_SSIVariablesNumberGet(&nSSIVars);

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "SSI variable slots - active: %d, total: %d\r\n", ssiEntries, nSSIVars);

        for(ix = 0; ix < nSSIVars; ix++)
        {
            varStr = TCPIP_HTTP_NET_SSIVariableGetByIndex(ix, &varName, &varType, NULL);
            if(varStr != NULL)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "SSI variable %d name: %s, value: %s, type: %d\r\n", ix, varName, varStr, varType);
            }
        }
    }
}
#endif  // (TCPIP_HTTP_NET_SSI_PROCESS != 0)
#endif // defined(M_TCPIP_COMMANDS_HTTP_NET_SERVER)

#if defined(M_TCPIP_COMMANDS_HTTP_SERVER)
static void F_Command_HttpInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t argIx, argStep;
    size_t instCount, portCount, instIx, ruleIx;
    size_t connIx, chunkIx, httpOpenConn;
    ssize_t httpActiveConn, ruleCount;
    TCPIP_HTTP_CONN_INFO    httpInfo;
    TCPIP_HTTP_CHUNK_INFO   httpChunkInfo[6];
    TCPIP_HTTP_CHUNK_INFO*  pChunkInfo;
    TCPIP_HTTP_STATISTICS   httpStat;
    TCPIP_HTTP_ACCESS_RULE accRule;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: http <inst n> <port n> info/stat/chunk/disconnect/rules\r\n");
        return;
    }

    argIx = 1;
    while(argIx < argc)
    { 
        char* cmd = argv[argIx];

        if(strcmp(cmd, "inst") == 0 && (argIx + 1) < argc)
        {
            http_inst_ix = 0UL;
            (void)FC_Str2UL(argv[argIx + 1], 10, &http_inst_ix); 
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "http: Set the instance to: %d\r\n", http_inst_ix);
            argStep = 2;
        }
        else if(strcmp(cmd, "port") == 0 && (argIx + 1) < argc)
        {
            http_port_ix = 0UL;
            (void)FC_Str2UL(argv[argIx + 1], 10, &http_port_ix);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "http: Set the port to: %d\r\n", http_port_ix);
            argStep = 2;
        }
        else
        {
            httpActiveConn = TCPIP_HTTP_ActiveConnectionCountGet(http_inst_ix, http_port_ix, &httpOpenConn);

            if(strcmp(cmd, "info") == 0)
            {
                instCount = TCPIP_HTTP_Instance_CountGet();
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP instances: %d\r\n", instCount);
                for(instIx = 0; instIx < instCount; instIx++)
                {
                    portCount = TCPIP_HTTP_Instance_PortCountGet(instIx); 
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP instance: %d, has %d port(s)\r\n", instIx, portCount);
                }

                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP instance: %d, port: %d connections info - active: %d, open: %d\r\n", http_inst_ix, http_port_ix, httpActiveConn, httpOpenConn);

                for(connIx = 0; connIx < httpOpenConn; connIx++)
                {
                    if(TCPIP_HTTP_InfoGet(http_inst_ix, http_port_ix, connIx, &httpInfo))
                    {
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP conn: %d status: 0x%4x, port: %d, sm: 0x%4x, chunks: %d, chunk empty: %d, file empty: %d\r\n",
                                connIx, httpInfo.httpStatus, httpInfo.listenPort, httpInfo.connStatus, httpInfo.nChunks, httpInfo.chunkPoolEmpty, httpInfo.fileBufferPoolEmpty);
                    }
                    else
                    {
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP: failed to get info for conn: %d\r\n", connIx);
                    }
                }
            }
            else if(strcmp(cmd, "chunk") == 0)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP instance: %d, port: %d chunk info:\r\n", http_inst_ix, http_port_ix);
                for(connIx = 0; connIx < httpOpenConn; connIx++)
                {
                    if(TCPIP_HTTP_InfoGet(http_inst_ix, http_port_ix, connIx, &httpInfo))
                    {
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP conn: %d, chunks: %d\r\n",  connIx, httpInfo.nChunks);
                        if(TCPIP_HTTP_ChunkInfoGet(http_inst_ix, http_port_ix, connIx, httpChunkInfo, sizeof(httpChunkInfo)/sizeof(*httpChunkInfo)))
                        {
                            pChunkInfo = httpChunkInfo;
                            for(chunkIx = 0; chunkIx < httpInfo.nChunks; chunkIx++)
                            {
                                (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tHTTP chunk: %d flags: 0x%4x, status: 0x%4x, fName: %s\r\n", chunkIx, pChunkInfo->flags, pChunkInfo->status, pChunkInfo->chunkFName);
                                (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tHTTP chunk: dyn buffers: %d, var Name: %s\r\n", pChunkInfo->nDynBuffers, pChunkInfo->dynVarName);
                                pChunkInfo++;
                            }
                            continue;
                        }
                    }

                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP: failed to get info for conn: %d\r\n", connIx);
                }
            }
            else if(strcmp(cmd, "stat") == 0)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP instance: %d, port: %d statistics info:\r\n", http_inst_ix, http_port_ix);
                if(TCPIP_HTTP_StatsticsGet(http_inst_ix, http_port_ix, &httpStat))
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP connections: %d, active: %d, open: %d\r\n", httpStat.nConns, httpStat.nActiveConns, httpStat.nOpenConns);
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP pool empty: %d, max depth: %d, parse retries: %d\r\n", httpStat.dynPoolEmpty, httpStat.maxRecurseDepth, httpStat.dynParseRetry);
                }
                else
                {
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "HTTP: Failed to get status!\r\n");
                }
            }
            else if(strcmp(cmd, "disconnect") == 0)
            {
                for(connIx = 0; connIx < httpOpenConn; connIx++)
                {
                    TCPIP_HTTP_CONN_HANDLE connHandle = TCPIP_HTTP_ConnectionHandleGet(http_inst_ix, http_port_ix, connIx);
                    NET_PRES_SKT_HANDLE_T skt_h = TCPIP_HTTP_ConnectionSocketGet(connHandle);
                    (void)NET_PRES_SocketDisconnect(skt_h);
                }

                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP instance %d, port %d, disconnected %d connections, active: %d\r\n", http_inst_ix, http_port_ix, httpOpenConn, httpActiveConn);
            }
            else if(strcmp(cmd, "rules") == 0)
            {
                ruleCount = TCPIP_HTTP_PortRules_CountGet(http_inst_ix, (ssize_t)http_port_ix); 
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP instance: %d, port: %d, rules: %d\r\n", http_inst_ix, http_port_ix, ruleCount);
                for(ruleIx = 0; ruleIx < ruleCount; ruleIx++)
                {
                    if(TCPIP_HTTP_PortRuleGet(http_inst_ix, (ssize_t)http_port_ix, ruleIx, &accRule))
                    {
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "rule: %d\r\n", ruleIx);
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tinPort: %d, intIfIx: %d, addType: %d\r\n", accRule.inPort, accRule.inIfIx, accRule.inAddType);
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\taction: %d, ruleSize: %d\r\n", accRule.action, accRule.ruleSize);
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tdir: %s\r\n", accRule.dir);
                        if(accRule.action == TCPIP_HTTP_ACCESS_ACTION_REDIRECT)
                        {
                            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tredirURI: %s, redirServer: %s\r\n", accRule.redirURI, accRule.redirServer);
                        }
                    }
                    else
                    {
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Failed to get rule: %d\r\n", ruleIx);
                    }
                }
            }
            else
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP: unknown parameter '%s'\r\n", cmd);
            }

            argStep = 1;
        }
        argIx += argStep;
    }
}
#if (M_TCPIP_HTTP_SSI_PROCESS != 0)
static void F_Command_SsiInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t argIx, argStep;
    size_t ssiEntries, ssiIx, nSSIVars;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    const char* varName = NULL;
    TCPIP_HTTP_DYN_ARG_DCPT varDcpt;

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ssi <inst n> info\r\n");
        return;
    }

    argIx = 1;
    argStep = 1;
    while(argIx < argc)
    { 
        char* cmd = argv[argIx];

        if(strcmp(cmd, "inst") == 0 && (argIx + 1) < argc)
        {
            http_inst_ix = 0UL;
            (void)FC_Str2UL(argv[argIx + 1], 10, &http_inst_ix); 
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "http: Set the instance to: %d\r\n", http_inst_ix);
            argStep = 2;
        }
        else if(strcmp(cmd, "info") == 0)
        {
            ssiEntries = TCPIP_HTTP_SSIVariablesNumberGet(http_inst_ix, &nSSIVars);

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "HTTP instance %d SSI variable slots - active: %d, total: %d\r\n", http_inst_ix, ssiEntries, nSSIVars);

            for(ssiIx = 0; ssiIx < nSSIVars; ssiIx++)
            {
                bool varRes = TCPIP_HTTP_SSIVariableGetByIndex(http_inst_ix, ssiIx, &varName, &varDcpt);
                if(varRes)
                {
                    if(varDcpt.argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
                    {
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "SSI variable %d name: %s, type: integer, value: %d\r\n", ssiIx, varName, varDcpt.argInt32);
                    } 
                    else
                    {
                        (*pCmdIO->pCmdApi->print)(cmdIoParam, "SSI variable %d name: %s, type: string, value: %s\r\n", ssiIx, varName, varDcpt.argStr);
                    }
                }
                else
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "SSI info - failed to get variable: %s\r\n", varName);
                }
            }
            argStep = 1;
        }
        else
        {   // ignore unknown command
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "SSI: unknown parameter '%s'\r\n", cmd);
            argStep = 1;
        }

        argIx += argStep;
    }
}
#endif  // (M_TCPIP_HTTP_SSI_PROCESS != 0)
#endif // defined(M_TCPIP_COMMANDS_HTTP_SERVER)

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
    (const uint8_t*)"This is yet another text attachment also supplied as a buffer.\r\n.Adjust as needed.\r\
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
    .messageCallback = &tcpipMailCallback,
    .replyCallback = &tcpipReplyCallback,
};

static TCPIP_SMTPC_MESSAGE_HANDLE tcpipMailHandle = NULL;


static char        tcpipMailServer[40] = "";    // IPv4 mail server address string
static uint16_t    tcpipServerPort = 587;
static char        tcpipAuthUser[80 + 1] = "";
static char        tcpipAuthPass[80 + 1] = "";
static char        tcpipMailFrom[80 + 1] = "";
static char        tcpipMailTo[80 + 1] = "";
static uint32_t    tcpipTlsFlag = 0U;
static bool        tcpipAuthPlain = false;
static bool        tcpipForceAuth = false;
static bool        tcpipHeloGreet = false;


// returns:
//      1 for success
//      0 if already in progress
//     -1 if failure
static int tcpipSendMail(void)
{

    if(tcpipMailHandle != NULL)
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
    myMailTestMsg.messageFlags = (tcpipTlsFlag == 1U) ? (uint16_t)TCPIP_SMTPC_MAIL_FLAG_CONNECT_TLS : (tcpipTlsFlag == 2U) ? (uint16_t)TCPIP_SMTPC_MAIL_FLAG_SKIP_TLS : (tcpipTlsFlag == 3U) ? (uint16_t)TCPIP_SMTPC_MAIL_FLAG_FORCE_TLS : 0U;
    if(tcpipAuthPlain)
    {
        myMailTestMsg.messageFlags |= (uint16_t)TCPIP_SMTPC_MAIL_FLAG_AUTH_PLAIN;
    }
    if(tcpipForceAuth)
    {
        myMailTestMsg.messageFlags |= (uint16_t)TCPIP_SMTPC_MAIL_FLAG_FORCE_AUTH;
    }
    if(tcpipHeloGreet)
    {
        myMailTestMsg.messageFlags |= (uint16_t)TCPIP_SMTPC_MAIL_FLAG_GREET_HELO;
    }

    size_t nBuffs = sizeof(smtpBuffAttachTbl) / sizeof(*smtpBuffAttachTbl);
    size_t ix;
    TCPIP_SMTPC_ATTACH_BUFFER* pAttachBuff = smtpBuffAttachTbl;
    for(ix = 0; ix < nBuffs; ix++)
    {
        pAttachBuff->attachSize = strlen((const char*)pAttachBuff->attachBuffer);
        pAttachBuff++;
    }
    
    myMailTestMsg.attachBuffers = smtpBuffAttachTbl;
    myMailTestMsg.nBuffers = (uint16_t)nBuffs;

    size_t nFiles = sizeof(smtpFileAttachTbl) / sizeof(*smtpFileAttachTbl);
    myMailTestMsg.nFiles = (uint16_t)nFiles;
    myMailTestMsg.attachFiles = smtpFileAttachTbl;


    tcpipMailHandle = TCPIP_SMTPC_MailMessage(&myMailTestMsg, &mailRes);

    return tcpipMailHandle == NULL ? -1 : 1;
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

    tcpipMailHandle = NULL;
}

static void tcpipReplyCallback(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, TCPIP_SMTPC_MESSAGE_STATUS currStat, const char* serverReply)
{
    // Note: You could add some monitoring of the server replies during the mail transaction like:
    // SYS_CONSOLE_PRINT("app: Mail server reply - stat: %d, msg: %s\r\n", currStat, serverReply);
}

static void F_Command_Mail(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int currIx;    
    uint32_t temp32;
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
        return;
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
        return;  // no more parameters
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
        return;  // no more parameters
    }
    else
    {
        // do nothing
    }


    currIx = 1;
    while(currIx + 1 < argc)
    { 
        char* param = argv[currIx];

        if(strcmp(param, "srv") == 0)
        {
            (void)strcpy(tcpipMailServer, argv[currIx + 1]);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set the server to: %s\r\n", tcpipMailServer);
        }
        else if(strcmp(param, "port") == 0)
        {
            temp32 = 0U;
            (void)FC_Str2UL(argv[currIx + 1], 10, &temp32); 
            tcpipServerPort = (uint16_t)temp32;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set the server port to: %d\r\n", tcpipServerPort);
        }
        else if(strcmp(param, "user") == 0)
        {
            (void)strncpy(tcpipAuthUser, argv[currIx + 1], sizeof(tcpipAuthUser) - 1U);
            tcpipAuthUser[sizeof(tcpipAuthUser) - 1U] = '\0';
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set auth user to: %s\r\n", tcpipAuthUser);
        }
        else if(strcmp(param, "pass") == 0)
        {
            (void)strncpy(tcpipAuthPass, argv[currIx + 1], sizeof(tcpipAuthPass) - 1U);
            tcpipAuthPass[sizeof(tcpipAuthPass) - 1U] = '\0';
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set auth pass to: %s\r\n", tcpipAuthPass);
        }
        else if(strcmp(param, "from") == 0)
        {
            (void)strncpy(tcpipMailFrom, argv[currIx + 1], sizeof(tcpipMailFrom) - 1U);
            tcpipMailFrom[sizeof(tcpipMailFrom) - 1U] = '\0';
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set mail-from to: %s\r\n", tcpipMailFrom);
        }
        else if(strcmp(param, "to") == 0)
        {
            (void)strncpy(tcpipMailTo, argv[currIx + 1], sizeof(tcpipMailTo) - 1U);
            tcpipMailTo[sizeof(tcpipMailTo) - 1U] = '\0';
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Set mail-to to: %s\r\n", tcpipMailTo);
        }
        else if(strcmp(param, "tls") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(argv[currIx + 1], 10, &temp32);
            tcpipTlsFlag = temp32;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: TLS flag set to: %s\r\n", (tcpipTlsFlag == 1U) ? "conn" : (tcpipTlsFlag == 2U) ? "skip" : (tcpipTlsFlag == 3U) ? "force" : "none");
        }
        else if(strcmp(param, "auth") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(argv[currIx + 1], 10, &temp32);
            tcpipAuthPlain = temp32 != 0U;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Authentication set to: %s\r\n", tcpipAuthPlain ? "plain" : "login");
        }
        else if(strcmp(param, "force") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(argv[currIx + 1], 10, &temp32);
            tcpipForceAuth = temp32 != 0U;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: Forced Auth set to: %d\r\n", tcpipForceAuth);
        }
        else if(strcmp(param, "helo") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(argv[currIx + 1], 10, &temp32);
            tcpipHeloGreet = temp32 != 0U;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "mail: HELO greet set to: %d\r\n", tcpipHeloGreet);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "mail: Unknown option\r\n");
        }

        currIx += 2;
    }



}

#endif  // defined(TCPIP_STACK_USE_SMTPC) && defined(TCPIP_SMTPC_USE_MAIL_COMMAND)

#if defined(M_TCPIP_COMMANDS_MIIM)
static void  F_Command_Miim(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int currIx;
    uint32_t temp32;
    TCPIP_COMMANDS_STAT writeCmd, readCmd;
    uint16_t rIx;

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <netix n> <add a> - Set the network interface index and PHY address\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <start r> <end r> - Sets the start and end register (decimal) for a dump op\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <read rix> - Reads register rix (decimal)\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <write rix wdata> - Writes register rix (decimal) with 16 bit data (hex)\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <dump> - Dumps all registers [rStart, rEnd]\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim <setup> - Performs the PHY setup\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim write_smi rix wdata - Extended 32 bit SMI write using rix (hex) and wdata(hex)\r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: miim read_smi rix - Extended 32 bit SMI read using rix (hex)\r\n");
        return;
    }

    // no parameter commands 

    if(strcmp(argv[1], "dump") == 0)
    {   // perform dump
        F_Command_MiimOp(pCmdIO, 0, 0, TCPIP_PHY_DUMP);
        return;  // no more parameters
    }

    if(strcmp(argv[1], "setup") == 0)
    {   // perform setup
        F_Command_MiimSetup(pCmdIO, cmdIoParam);
        return;  // no more parameters
    }

    // parameter commands 
    currIx = 1;
    while(currIx + 1 < argc)
    { 
        char* param = argv[currIx];
        char* paramVal = argv[currIx + 1];

        if(strcmp(param, "read") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(paramVal, 10, &temp32);
            rIx = (uint16_t)temp32;
            readCmd = TCPIP_PHY_READ;
        }
        else if(strcmp(param, "read_smi") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(paramVal, 16, &temp32);
            rIx = (uint16_t)temp32;
            readCmd = TCPIP_PHY_READ_SMI;
        }
        else
        {
            readCmd = TCPIP_CMD_STAT_IDLE;
        }

        if(readCmd != TCPIP_CMD_STAT_IDLE)
        {   // read operation
            F_Command_MiimOp(pCmdIO, rIx, 0, readCmd);
            return;  // no more parameters
        }

        if(strcmp(param, "write") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(paramVal, 10, &temp32);
            rIx = (uint16_t)temp32;
            writeCmd = TCPIP_PHY_WRITE;
        }
        else if(strcmp(param, "write_smi") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(paramVal, 16, &temp32);
            rIx = (uint16_t)temp32;
            writeCmd = TCPIP_PHY_WRITE_SMI;
        }
        else
        {
            writeCmd = TCPIP_CMD_STAT_IDLE;
        }
        
        if(writeCmd != TCPIP_CMD_STAT_IDLE)
        {   // write operation
            if(currIx + 2 >= argc)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim write: missing wData\r\n");
                return;
            }
            uint32_t wData =  0UL;
            (void)FC_Str2UL(argv[currIx + 2], 16, &wData);
            F_Command_MiimOp(pCmdIO, rIx, wData, writeCmd);
            return;  // no more parameters
        }

        if(strcmp(param, "add") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(paramVal, 10, &temp32);
            miimAdd = (uint16_t)temp32;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim: Set Add to: %d\r\n", miimAdd);
        }
        else if(strcmp(param, "netix") == 0)
        {
            miimNetIx = 0UL;
            (void)FC_Str2UL(paramVal, 10, &miimNetIx);
            if (miimNetIx >= (unsigned int)DRV_MIIM_INSTANCES_NUMBER)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "miim: Incorrect Interface index\r\n");
            }
            else
            {
                miimNetIx = miimObjIx;
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim: Set Network Interface index to: %d\r\n", miimNetIx);
            }
        }
        else if(strcmp(param, "start") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(paramVal, 10, &temp32);
            miimRegStart = (uint16_t)temp32;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim: Set Start Reg to: %d\r\n", miimRegStart);
        }
        else if(strcmp(param, "end") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(paramVal, 10, &temp32);
            miimRegEnd = (uint16_t)temp32;
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim: Set End Reg to: %d\r\n", miimRegEnd);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "miim: Unknown option\r\n");
        }

        currIx += 2;
    }

}

static void F_Command_MiimOp(SYS_CMD_DEVICE_NODE* pCmdIO, uint16_t rIx, uint32_t wData, TCPIP_COMMANDS_STAT miimCmd)
{
    DRV_MIIM_OPERATION_HANDLE opHandle;
    DRV_MIIM_RESULT miimRes;
    const char* opName = "unknown";
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE || miimOpHandle != NULL || miimHandle != 0U)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "miim: Another operation ongoing. Retry!\r\n");
        return;
    }

    if(F_MiimOpen(pCmdIO, cmdIoParam) == 0U)
    {
        return;
    }

    if(TCPIP_PHY_READ <= miimCmd && miimCmd <= TCPIP_PHY_WRITE_SMI)
    {
       opName = miiOpName_Tbl[(int)miimCmd - (int)TCPIP_PHY_READ];
    }

    miimRegIx = rIx;

    switch(miimCmd)
    {
        case TCPIP_PHY_READ:
            opHandle = miimObj->miim_Read(miimHandle, rIx, miimAdd, DRV_MIIM_OPERATION_FLAG_NONE, &miimRes);
            break;

        case TCPIP_PHY_WRITE:
            opHandle = miimObj->miim_Write(miimHandle, rIx, miimAdd, (uint16_t)wData, DRV_MIIM_OPERATION_FLAG_NONE, &miimRes);
            break;

        case TCPIP_PHY_DUMP:
            opHandle = miimObj->miim_Read(miimHandle, (miimRegIx = miimRegStart), miimAdd, DRV_MIIM_OPERATION_FLAG_NONE, &miimRes);
            break;

        case TCPIP_PHY_READ_SMI:
            opHandle = miimObj->miim_ReadExt(miimHandle, rIx, miimAdd, DRV_MIIM_OPERATION_FLAG_EXT_SMI_SLAVE, &miimRes);
            break;

        case TCPIP_PHY_WRITE_SMI:
            opHandle = miimObj->miim_WriteExt(miimHandle, rIx, miimAdd, wData, DRV_MIIM_OPERATION_FLAG_EXT_SMI_SLAVE, &miimRes);
            break;

        default:
            opHandle = NULL;
            miimRes = DRV_MIIM_RES_OP_HANDLE_ERR;
            break;
    }


    if(opHandle != NULL)
    {   // operation started
        miimOpHandle = opHandle;
        tcpipCmdStat = miimCmd;
        pTcpipCmdDevice = pCmdIO;
        miimCmdIoParam = cmdIoParam; 
        (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, (int16_t)TCPIP_MIIM_COMMAND_TASK_RATE);
    }
    else
    {
        F_MiimClose(true);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim %s: an error occurred: %d!\r\n", opName, miimRes);
    }
        
}

static void F_Command_MiimSetup(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam)
{
    DRV_MIIM_SETUP miimSetup;
    DRV_MIIM_RESULT res;

    if(miimHandle != 0U || miimOpHandle != NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "miim: Another operation ongoing. Retry!\r\n");
        return;
    }

    if(F_MiimOpen(pCmdIO, cmdIoParam) == 0U)
    {
        return;
    }

    miimSetup.hostClockFreq = (uint32_t)TCPIP_INTMAC_PERIPHERAL_CLK;
    miimSetup.maxBusFreq = 2000000U;
    miimSetup.setupFlags = DRV_MIIM_SETUP_FLAG_NONE;

    
    res = miimObj->miim_Setup(miimHandle, &miimSetup);

    if((int)res < 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim setup failed: %d!\r\n", res);
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "miim setup success:%d\r\n", res);
    }
    F_MiimClose(false);
}

static DRV_HANDLE F_MiimOpen(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam)
{
    DRV_HANDLE hMiim = miimObj->miim_Open(miimNetIx, DRV_IO_INTENT_SHARED);
    if(hMiim == DRV_HANDLE_INVALID || hMiim == 0U)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "miim open: failed!\r\n");
        hMiim = 0U;
    }

    return (miimHandle = hMiim);
}

static void F_MiimClose(bool idleState)
{
    miimObj->miim_Close(miimHandle);
    miimHandle = 0U;
    miimOpHandle = NULL;
    if(idleState)
    {
        (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, 0);
        tcpipCmdStat = TCPIP_CMD_STAT_IDLE; 
    }
}

// MIIM commands state machine
static void TCPIPCmdMiimTask(void)
{
    DRV_MIIM_RESULT  opRes;
    const char* opName;
    bool    opContinue;
    uint32_t opData;
    
    opRes = miimObj->miim_OperationResult(miimHandle, miimOpHandle, &opData);

    if(opRes == DRV_MIIM_RES_PENDING)
    {   // ongoing...
        return;
    }

    opContinue = false;
    if(TCPIP_PHY_READ <= tcpipCmdStat && tcpipCmdStat <= TCPIP_PHY_WRITE_SMI)
    {
       opName = miiOpName_Tbl[(int)tcpipCmdStat - (int)TCPIP_PHY_READ];
    }
    else
    {
        opName = "unknown";
    }

    if(opRes == DRV_MIIM_RES_OK)
    {   // success
        const char* fmtStr;
        if(tcpipCmdStat == TCPIP_PHY_READ_SMI || tcpipCmdStat == TCPIP_PHY_WRITE_SMI)
        {
            fmtStr = "Miim %s: 0x%04x, netIx: %d, add: %d, val: 0x%08x\r\n"; 
        }
        else
        {
            fmtStr = "Miim %s: %d, netIx: %d, add: %d, val: 0x%4x\r\n"; 
        }

        (*pTcpipCmdDevice->pCmdApi->print)(miimCmdIoParam, fmtStr, opName, miimRegIx, miimNetIx, miimAdd, opData);
         
        if(tcpipCmdStat == TCPIP_PHY_DUMP)
        {
            if(miimRegIx != miimRegEnd)
            {   // initiate another read
                miimOpHandle = miimObj->miim_Read(miimHandle, ++miimRegIx, miimAdd, DRV_MIIM_OPERATION_FLAG_NONE, &opRes);
                opContinue = true;
            }
        }
    }

    if((int)opRes < 0)
    {   // error occurred
        (*pTcpipCmdDevice->pCmdApi->print)(miimCmdIoParam, "Miim %s error: %d\r\n", opName, opRes);
    } 

    if((int)opRes < 0 || opContinue == false)
    {
        F_MiimClose(true);
    }

}

#endif  // defined(M_TCPIP_COMMANDS_MIIM)

#if (M_TCPIP_UDP_COMMANDS)
static void F_Command_Udp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // udp info
    uint32_t sktNo, ix, startIx, stopIx;
    UDP_SOCKET_INFO sktInfo;
    char flagsBuff[80];

    const void* cmdIoParam = pCmdIO->cmdIoParam;
 
    if(argc > 1)
    {
        if(strcmp("info", argv[1]) == 0)
        {
            sktNo = TCPIP_UDP_SocketsNumberGet();
            if(argc > 2)
            {
                startIx = 0UL;
                (void)FC_Str2UL(argv[2], 10, &startIx);
                stopIx = startIx + 1U;
            }
            else
            {
                startIx = 0U;
                stopIx = sktNo;
            }

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "UDP sockets: %d \r\n", sktNo);
            for(ix = startIx; ix < stopIx; ix++)
            {
                if(TCPIP_UDP_SocketInfoGet((UDP_SOCKET)ix, &sktInfo))
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tsktIx: %d, addType: %d, remotePort: %d, localPort: %d, rxQueueSize: %d, txSize: %d\r\n",
                            ix, sktInfo.addressType, sktInfo.remotePort, sktInfo.localPort, sktInfo.rxQueueSize, sktInfo.txSize);

                    static const char* sticky_tbl[] = {"Non-", " "};
                    static const char* strict_tbl[] = {"Loose", "Strict"};
                    int n = 0;
                    uint8_t sktFlags = (uint8_t)sktInfo.flags; 
                    n += FC_sprintf(flagsBuff + n, sizeof(flagsBuff) - (size_t)n, "'%sSticky ", sticky_tbl[(sktFlags & (uint8_t)UDP_SOCKET_FLAG_STICKY_PORT) != 0U ? 1 : 0]);
                    n += FC_sprintf(flagsBuff + n, sizeof(flagsBuff) - (size_t)n, "%s Port', ", strict_tbl[(sktFlags & (uint8_t)UDP_SOCKET_FLAG_STRICT_PORT) != 0U ? 1 : 0]);

                    n += FC_sprintf(flagsBuff + n, sizeof(flagsBuff) - (size_t)n, "'%sSticky ", sticky_tbl[(sktFlags & (uint8_t)UDP_SOCKET_FLAG_STICKY_NET) != 0U ? 1 : 0]);
                    n += FC_sprintf(flagsBuff + n, sizeof(flagsBuff) - (size_t)n, "%s Net',", strict_tbl[(sktFlags & (uint8_t)UDP_SOCKET_FLAG_STRICT_NET) != 0U ? 1 : 0]);

                    n += FC_sprintf(flagsBuff + n, sizeof(flagsBuff) - (size_t)n, "'%sSticky ", sticky_tbl[(sktFlags & (uint8_t)UDP_SOCKET_FLAG_STICKY_ADD) != 0U ? 1 : 0]);
                    (void)FC_sprintf(flagsBuff + n, sizeof(flagsBuff) - (size_t)n, "%s Add'", strict_tbl[(sktFlags & (uint8_t)UDP_SOCKET_FLAG_STRICT_ADD) != 0U ? 1 : 0]);
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t flags: %s\r\n", flagsBuff);
                }
            }
            return;
        }
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: udp info <n>\r\n");
}

#endif  // (M_TCPIP_UDP_COMMANDS)


#if (M_TCPIP_TCP_COMMANDS)
static void F_Command_Tcp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{   // tcp info <n>
    uint32_t sktNo, ix, startIx, stopIx;
    TCP_SOCKET_INFO sktInfo;

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc > 1)
    {
        if(strcmp("info", argv[1]) == 0)
        {
            sktNo = TCPIP_TCP_SocketsNumberGet();
    
            if(argc > 2)
            {
                startIx = 0UL;
                (void)FC_Str2UL(argv[2], 10, &startIx);
                stopIx = startIx + 1U;
            }
            else
            {
                startIx = 0U;
                stopIx = sktNo;
            }

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "TCP sockets: %d \r\n", sktNo);
            for(ix = startIx; ix < stopIx; ix++)
            {
                if(TCPIP_TCP_SocketInfoGet((TCP_SOCKET)ix, &sktInfo))
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tsktIx: %d, addType: %d, remotePort: %d, localPort: %d, flags: 0x%02x\r\n",
                            ix, sktInfo.addressType, sktInfo.remotePort, sktInfo.localPort, sktInfo.flags);
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\trxSize: %d, txSize: %d, state: %d, rxPend: %d, txPend: %d\r\n",
                            sktInfo.rxSize, sktInfo.txSize, sktInfo.state, sktInfo.rxPending, sktInfo.txPending);
                }
            }

            return;
        }
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: tcp info <n>\r\n");
}

static void F_Command_TcpTrace(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{   // tcptrace on/off n

    uint32_t temp32;
    uint16_t sktNo;
    bool    traceOn;

    const void* cmdIoParam = pCmdIO->cmdIoParam;
    
    while(argc >= 3)
    {
        temp32 = 0UL;
        (void)FC_Str2UL(argv[2], 10, &temp32);
        sktNo = (uint16_t)temp32;
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

        bool res = TCPIP_TCP_SocketTraceSet((TCP_SOCKET)sktNo, traceOn);

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "tcp trace %s for socket: %d %s\r\n", argv[1], sktNo, res ? "success" : "failed");
        return;
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: tcptrace on/off n\r\n");
}

#endif  // (M_TCPIP_TCP_COMMANDS)

#if (M_TCPIP_PACKET_LOG_ENABLE)
static void F_Command_PktLog(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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
        return;
    }

    if(strcmp(argv[1], "show") == 0)
    {
        F_Command_PktLogInfo(pCmdIO, argc, argv);
    }
    else if(strcmp(argv[1], "clear") == 0)
    {
        F_Command_PktLogClear(pCmdIO, argc, argv);
    }
    else if(strcmp(argv[1], "reset") == 0)
    {
        F_Command_PktLogReset(pCmdIO, argc, argv);
    }
    else if(strcmp(argv[1], "handler") == 0)
    {
        F_Command_PktLogHandler(pCmdIO, argc, argv);
    }
    else if(strcmp(argv[1], "type") == 0)
    {
        F_Command_PktLogType(pCmdIO, argc, argv);
    }
    else
    {
        F_Command_PktLogMask(pCmdIO, argc, argv);
    }

}


static void F_Command_PktLogInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // "Usage: plog show <all/unack/ack/err>"
    uint16_t ix;
    uint16_t jx;
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

    (void)strcpy(printBuff, logInfo.logType == TCPIP_PKT_LOG_TYPE_RX_ONLY ? "RX" : logInfo.logType == TCPIP_PKT_LOG_TYPE_TX_ONLY ? "TX" : "RXTX");
    if(((uint16_t)logInfo.logType & (uint16_t)TCPIP_PKT_LOG_TYPE_SKT_ONLY) != 0U)
    {
        (void)strcat(printBuff, "_SKT");
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog entries: %d, used: %d, persistent: %d, failed: %d\r\n", logInfo.nEntries, logInfo.nUsed, logInfo.nPersistent, logInfo.nFailed);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog persist mask: 0x%4x, module log mask: 0x%4x, log type: %s\r\n", logInfo.persistMask, logInfo.logModuleMask, printBuff);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog net log mask 0x%4x, socket log mask: 0x%8x, handler: 0x%8x\r\n\n", logInfo.netLogMask, logInfo.sktLogMask, logInfo.logHandler);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog for -%s- entries\r\n", argc > 2 ? argv[2] : "unack");

    for(ix = 0; ix < logInfo.nEntries; ix++)
    {
        if(!TCPIP_PKT_FlightLogGetEntry((size_t)ix, &logEntry))
        {
            continue;
        }

        if(showMask == 0)
        {   // show only unacknowledged ones
            if((logEntry.logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_DONE) != 0U)
            {   // entry done
                continue;
            }
        }
        else if(showMask == 1)
        {   // show only error ones
            if((logEntry.logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_DONE) == 0U || logEntry.ackRes > 0)
            {   // not done or good ack
                continue;
            }
        }
        else if(showMask == 2)
        {   // show only properly acknowledged ones
            if((logEntry.logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_DONE) == 0U || logEntry.ackRes < 0)
            {   // not done or bad ack
                continue;
            }
        }
        else
        {
            // show all
        }

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog entry: %d\r\n", ix);
        if((logEntry.logFlags & ((uint16_t)TCPIP_PKT_LOG_FLAG_RX | (uint16_t)TCPIP_PKT_LOG_FLAG_TX)) == ((uint16_t)TCPIP_PKT_LOG_FLAG_RX | (uint16_t)TCPIP_PKT_LOG_FLAG_TX))
        {
            rxtxMsg = "RX/TX";
        }
        else if((logEntry.logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_RX) != 0U)
        {
            rxtxMsg = "RX";
        }
        else 
        {
            rxtxMsg = "TX";
        }
        (void)TCPIP_STACK_NetAliasNameGet(logEntry.pPkt->pktIf, printBuff, sizeof(printBuff));


        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tpkt: 0x%8x, if: %s, type: %s %s\r\n", logEntry.pPkt, printBuff, rxtxMsg, (logEntry.logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_PERSISTENT) != 0U ? "p" : " ");
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\towner: %d, mac: %d, acker: %d, ackRes: %d\r\n\tmodules:\t", logEntry.pktOwner, logEntry.macId, logEntry.pktAcker, logEntry.ackRes);

        modPrint = false;
        for(jx = 1; jx <= (uint16_t)TCPIP_MODULE_LAYER3; jx++)
        {
            if((logEntry.moduleLog & ( 1UL << jx)) != 0U)
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s(0x%08x) ", T_CommandPktLogModuleNames[jx], logEntry.moduleStamp[jx - 1U]);
                modPrint = true;
            }
        }
        if(modPrint)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\n\tMAC stamp: 0x%08x, ACK stamp: 0x%08x\r\n", logEntry.macStamp, logEntry.ackStamp);
        }

        if((logEntry.logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_SKT_PARAM) != 0U)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tskt: %d, lport: %d, rport: %d\r\n", logEntry.sktNo, logEntry.lclPort, logEntry.remPort);
        }

    }

}

static void F_Command_PktLogClear(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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

static void F_Command_PktLogReset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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

static void F_Command_PktLogHandler(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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
            (void)TCPIP_PKT_FlightLogRegister(&F_Command_PktLogDefHandler, logAll);
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Registered the default handler\r\n");
            p_pktHandlerCmdIo = pCmdIO;
            return;
        }
        else if(strcmp(argv[2], "off") == 0)
        {
            (void)TCPIP_PKT_FlightLogRegister(NULL, false);
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Deregistered the default handler\r\n");
            p_pktHandlerCmdIo = NULL;
            return;
        }
        else
        {
            // do nothing
        }
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Unknown parameter\r\n");

}

static void F_Command_PktLogType(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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

        (void)TCPIP_PKT_FlightLogTypeSet(logType, clrPersist);
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
// 8 bit values only
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
    { .cmdName = "and",    .cmdFlags = (uint8_t)CMD_PKT_XTRACT_FLAG_AND | (uint8_t)CMD_PKT_XTRACT_FLAG_NEEDED | (uint8_t)CMD_PKT_XTRACT_FLAG_NEED_PARAMS},
    { .cmdName = "or",     .cmdFlags = (uint8_t)CMD_PKT_XTRACT_FLAG_OR  | (uint8_t)CMD_PKT_XTRACT_FLAG_NEEDED | (uint8_t)CMD_PKT_XTRACT_FLAG_NEED_PARAMS},
    { .cmdName = "clr",    .cmdFlags = (uint8_t)CMD_PKT_XTRACT_FLAG_CLR },
};

static CMD_PKT_XTRACT_OP xtract_op_tbl[sizeof(const_xtract_op_tbl) / sizeof(*const_xtract_op_tbl)];

static CMD_PKT_XTRACT_RES F_Command_PktExtractMasks(int argc, char** argv, uint32_t* pAndMask, uint32_t* pOrMask)
{
    size_t ix;
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
    (void)memset(xtract_op_tbl, 0, sizeof(xtract_op_tbl));
    pXtOp = xtract_op_tbl;
    pCtOp = const_xtract_op_tbl;
    for(ix = 0; ix < sizeof(xtract_op_tbl) / sizeof(*xtract_op_tbl); ix++)
    {
        pXtOp->cmdName = pCtOp->cmdName;
        pXtOp->cmdFlags = pCtOp->cmdFlags;
        pXtOp++;
        pCtOp++;
    }

    orMask = 0;
    andMask = 0xffffffffU;

    size_t argIx = 2U;
    argc -= 2;
    argBuff[sizeof(argBuff) - 1U] = '\0';
    pCurrOp = NULL;
    size_t notOptCount = 0U;

    while(argc != 0)
    {
        if(pCurrOp == NULL)
        {   // extract new command
            pXtOp = xtract_op_tbl;
            pNewOp = NULL;
            for(ix = 0; ix < sizeof(xtract_op_tbl) / sizeof(*xtract_op_tbl); ix++)
            {
                if(strcmp(argv[argIx], pXtOp->cmdName) == 0)
                {   // found command
                    pNewOp = pXtOp;
                    break;
                }
                pXtOp++;
            }

            if(pNewOp == NULL)
            {   // no such command ?
                return CMD_PKT_XTRACT_RES_ERR;
            }

            // set the new command
            pCurrOp = pNewOp;
            pCurrOp->cmdCount++;
            pCurrOp->cmdFlags |= (uint8_t)CMD_PKT_XTRACT_FLAG_BUSY;
            if((pCurrOp->cmdFlags & (uint8_t)CMD_PKT_XTRACT_FLAG_NEEDED) != 0U)
            {
                notOptCount++;   // got one mandatory command i.e. or/and
            }
            if((pCurrOp->cmdFlags & (uint8_t)CMD_PKT_XTRACT_FLAG_NEED_PARAMS) == 0U) 
            {   // no params; stop this op
                pCurrOp = NULL;
            }
        }
        else
        {   // ongoing operation; extract parameters
            if(strcmp(argv[argIx], "none") == 0)
            {   // 'none' should be the only parameter 
                if(pCurrOp->cmdParams != 0U)
                {   
                    return CMD_PKT_XTRACT_RES_ERR;
                }
                pCurrOp->cmdMask = 0U;
                pCurrOp->cmdParams++;
                pCurrOp = NULL;    // no params, done 
            }
            else if(strcmp(argv[argIx], "all") == 0)
            {   // 'all' should be the only parameter 
                if(pCurrOp->cmdParams != 0U)
                {
                    return CMD_PKT_XTRACT_RES_ERR;
                }
                pCurrOp->cmdMask = 0xffffffffU;
                pCurrOp->cmdParams++; 
                pCurrOp = NULL;    // no params, done 
            }
            else
            {   // should be a number
                bool argInc = false;
                (void)strncpy(argBuff, argv[argIx], sizeof(argBuff) - 1U);
                size_t len = strlen(argBuff);
                if(argBuff[len - 1U] == '0')
                {
                    argBuff[len - 1U] += 1;
                    argInc = true;
                }

                uint32_t argInt = 0UL;
                (void)FC_Str2UL(argBuff, 10, &argInt);
                if(argInt == 0U)
                {   // not a number?; done with this operation
                    pCurrOp = NULL; 
                    continue;
                }
                else
                {   // valid number
                    if(argInc)
                    {
                        argInt--;
                    }
                    pCurrOp->cmdMask |= 1UL << argInt;
                    pCurrOp->cmdParams++; 
                }
            }
        }

        argIx++;
        argc--;
    }

    // we're done; collect the result
    if(notOptCount == 0U)
    {   // mandatory command not found
        return CMD_PKT_XTRACT_RES_ERR;
    }

    xtractRes = CMD_PKT_XTRACT_RES_OK;
    pXtOp = xtract_op_tbl;
    for(ix = 0; ix < sizeof(xtract_op_tbl) / sizeof(*xtract_op_tbl); ix++)
    {
        if((pXtOp->cmdFlags & (uint8_t)CMD_PKT_XTRACT_FLAG_BUSY) != 0U)
        {   // in use entry
            if((pXtOp->cmdFlags & (uint8_t)CMD_PKT_XTRACT_FLAG_NEED_PARAMS) != 0U && pXtOp->cmdParams == 0U)
            {   // command without parameters
                return CMD_PKT_XTRACT_RES_ERR;
            }

            if((pXtOp->cmdFlags & (uint8_t)CMD_PKT_XTRACT_FLAG_AND) != 0U)
            {
                andMask &= pXtOp->cmdMask;
            }
            else if((pXtOp->cmdFlags & (uint8_t)CMD_PKT_XTRACT_FLAG_OR) != 0U)
            {
                orMask |= pXtOp->cmdMask;
            }
            else if((pXtOp->cmdFlags & (uint8_t)CMD_PKT_XTRACT_FLAG_CLR) != 0U)
            {
                if(pXtOp->cmdCount != 0U)
                {   // 'clr' was mentioned
                    xtractRes = CMD_PKT_XTRACT_RES_CLR;
                }
            }
            else
            {
                // do nothing
            }
        }
        pXtOp++;
    }

    *pOrMask = orMask;
    *pAndMask = andMask;
    return xtractRes;
}

static void F_Command_PktLogMask(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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
    else
    {
        // do nothing
    }

    if(logMaskOp == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Unknown command\r\n");
        return;
    }

    xtRes = F_Command_PktExtractMasks(argc, argv, &andMask, &orMask);
    if(xtRes == CMD_PKT_XTRACT_RES_ERR)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "pktlog: Wrong command parameters\r\n");
        return;
    }

    switch(logMaskOp)
    {
        case 1:
            TCPIP_PKT_FlightLogUpdateNetMask(andMask, orMask, ((uint16_t)xtRes & (uint16_t)CMD_PKT_XTRACT_RES_CLR) != 0U);
            break;

        case 2:
            TCPIP_PKT_FlightLogUpdatePersistMask(andMask, orMask, ((uint16_t)xtRes & (uint16_t)CMD_PKT_XTRACT_RES_CLR) != 0U);
            break;

        case 3:
            TCPIP_PKT_FlightLogUpdateModuleMask(andMask, orMask, ((uint16_t)xtRes & (uint16_t)CMD_PKT_XTRACT_RES_CLR) != 0U);
            break;

        default:    // 4
            TCPIP_PKT_FlightLogUpdateSocketMask(andMask, orMask, ((uint16_t)xtRes & (uint16_t)CMD_PKT_XTRACT_RES_CLR) != 0U);
            break;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pktlog: %s and with: 0x%4x, or with: 0x%4x, %scleared\r\n", argv[1], andMask, orMask, ((uint16_t)xtRes & (uint16_t)CMD_PKT_XTRACT_RES_CLR) != 0U ? " " : " not ");

}

static void F_Command_PktLogDefHandler(TCPIP_STACK_MODULE moduleId, const TCPIP_PKT_LOG_ENTRY* pLogEntry)
{
    if(p_pktHandlerCmdIo != NULL)
    {
        const char* logType = (pLogEntry->logFlags & ((uint16_t)TCPIP_PKT_LOG_FLAG_RX | (uint16_t)TCPIP_PKT_LOG_FLAG_TX)) == ((uint16_t)TCPIP_PKT_LOG_FLAG_RX | (uint16_t)TCPIP_PKT_LOG_FLAG_TX) ? "RXTX" : (pLogEntry->logFlags & (uint16_t)TCPIP_PKT_LOG_FLAG_RX) != 0U ? "RX" : "TX";
        (*p_pktHandlerCmdIo->pCmdApi->print)( p_pktHandlerCmdIo->cmdIoParam, "logger - module : %d, pkt: 0x%8x %s\r\n", moduleId, pLogEntry->pPkt, logType);
    }
}


#endif  // (M_TCPIP_PACKET_LOG_ENABLE)


#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
static void F_Command_PktInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t ix;
    TCPIP_PKT_TRACE_ENTRY tEntry;
    TCPIP_PKT_TRACE_INFO  tInfo;

    const void* cmdIoParam = pCmdIO->cmdIoParam;


    if(TCPIP_PKT_TraceGetEntriesNo(&tInfo) == 0U)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No packet info available\r\n");
        return;
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

}
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
static void F_Command_HeapList(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{

    TCPIP_STACK_HEAP_HANDLE heapH;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    size_t ix;
    TCPIP_HEAP_POOL_ENTRY_LIST entryList = {0U};

    heapH = TCPIP_STACK_HeapHandleGet(TCPIP_STACK_HEAP_TYPE_POOL, 0);
    if(heapH == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No pool heap exists!\r\n");
        return;
    }

    uint16_t nEntries = TCPIP_HEAP_POOL_Entries(heapH);

    if(nEntries == 0U)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "No entries in this pool heap!\r\n");
        return;
    }

    size_t totSize = 0U;
    size_t totFreeSize = 0U;
    uint16_t expansionSize = 0U;
    for(ix = 0; ix < (size_t)nEntries; ix++)
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

}
#endif  // defined(TCPIP_STACK_USE_INTERNAL_HEAP_POOL)

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)
static void F_Command_Announce(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // announce 0/1 for limited/network directed broadcast

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: announce 0/1\r\n");
        return;
    }
    
    int32_t param = 0;
    (void)FC_Str2L(argv[1], 10, &param);
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

}
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)

#if defined(TCPIP_STACK_USE_FTP_CLIENT) && defined(TCPIP_FTPC_COMMANDS)

//Disable the following define to retrieve file for transmission from file system
//#define FTPC_CALLBACK_TX_PROCESSING

//Disable the following define to store file received in file system
//#define FTPC_CALLBACK_RX_PROCESSING

#ifdef FTPC_CALLBACK_TX_PROCESSING  
static char ftpcTransmitData[] = "This is a text file for testing";
#endif
#ifdef FTPC_CALLBACK_RX_PROCESSING  
static char ftpcDataBuffer[2048];
#endif


static void ctrlSktHandler(TCPIP_FTPC_CONN_HANDLE_TYPE ftpCliHandle, TCPIP_FTPC_CTRL_EVENT_TYPE ftpcEvent,
                                            TCPIP_FTPC_CMD cmd, char * ctrlbuff, uint16_t ctrllen)
{
    char ctrl_buffer[150];
    
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
        case TCPIP_FTPC_CTRL_SEND:
        default:
            // do nothing
            break;
    }
        
    if(ctrllen != 0U)
    {
        (void)memcpy (ctrl_buffer, ctrlbuff, ctrllen);
        ctrl_buffer[ctrllen] = '\0';
        SYS_CONSOLE_PRINT("%s\rLength = %d\r\n\n", ctrl_buffer, ctrllen); 
    }
}

//This callback function returns 'true' when Data-Socket Rx/Tx data is handled in this callback itself.
//Then, FTP Client function won't store/retrieve data to/from FileSystem.
//When it returns 'false', the FTP Client function will store/retrieve data to/from FileSystem.
static bool dataSktHandler(TCPIP_FTPC_CONN_HANDLE_TYPE ftpCliHandle, TCPIP_FTPC_DATA_EVENT_TYPE ftpcEvent,
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
            (void)memcpy (ftpcDataBuffer, databuff, *datalen);
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
            len = strlen(&ftpcTransmitData[buff_index]);
            if(*datalen <= len)
            {
                (void)strncpy(databuff,&ftpcTransmitData[buff_index], *datalen);
                buff_index += *datalen ;
                buffCount++;
            }
            else
            {
                if(len)
                {
                    (void)strncpy(databuff,&ftpcTransmitData[buff_index], len);
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

        default:
            // do nothing
            break;
    }   
    return callback_processing;
    
}

static void ftpc_res_print(SYS_CMD_DEVICE_NODE* pCmdIO, TCPIP_FTPC_RETURN_TYPE ftpcRes)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if(ftpcRes == TCPIP_FTPC_RET_OK)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Command Started\r\n");
    }
    else if(ftpcRes == TCPIP_FTPC_RET_BUSY)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Not Ready\r\n");
    }        
    else if(ftpcRes == TCPIP_FTPC_RET_NOT_CONNECT)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Not Connected\r\n");
    }
    else if(ftpcRes == TCPIP_FTPC_RET_NOT_LOGIN)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Not Logged In\r\n");
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "FTPC - Failure\r\n");
    }
}

static void F_Command_FTPC_Service(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int i;
    size_t opt_count;
    uint32_t temp32;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    TCPIP_FTPC_RETURN_TYPE res = TCPIP_FTPC_RET_FAILURE;
    TCPIP_FTPC_STATUS_TYPE ftpcStatus;
    char ftpc_username[15];
    char ftpc_password[15];
    char ftpc_account[15];
    char ftpc_src_pathname[20];
    static TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle = NULL;
    
    if(strcmp("connect",argv[1])==0)
    {
        TCPIP_FTPC_CTRL_CONN_TYPE ftpcConn;
        static IP_MULTI_ADDRESS serverIpAddr;
        static IP_ADDRESS_TYPE serverIpAddrType;
        static uint16_t    ftpcServerPort = 0U;

        if ((argc < 3)||(argc > 4))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc connect <server ip address> <server port>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc connect 192.168.0.8 0 \r\n");
            return;
        }
        
        ftpcServerPort = 0U;
        if (argc == 4)
        {
            if(strcmp("0",argv[3]) != 0)
            {        
                temp32 = 0UL;
                (void)FC_Str2UL(argv[3], 10, &temp32);
                ftpcServerPort = (uint16_t)temp32;
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
            return;
        }        
        
        
        ftpcConn.ftpcServerAddr = &serverIpAddr;
        ftpcConn.ftpcServerIpAddrType = serverIpAddrType;
        ftpcConn.serverCtrlPort = ftpcServerPort;
        
        ftpcHandle = TCPIP_FTPC_Connect(&ftpcConn, &ctrlSktHandler, &res);
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
            return;
        }
        
        ftpcStatus.isConnected = false;
        TCPIP_FTPC_Get_Status(ftpcHandle, &ftpcStatus);
        if(ftpcStatus.isConnected)
        {
            if(TCPIP_FTPC_Disconnect(ftpcHandle) != TCPIP_FTPC_RET_OK)
            {
                ftpc_res_print(pCmdIO,res);
            }
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
            return;
        }
        
        (void)strcpy(ftpc_username, argv[2]);
        (void)strcpy(ftpc_password, argv[3]);
        (void)strcpy(ftpc_account, "0");
        if (argc == 5)
        {
            if(strcmp("0",argv[4]) != 0)
            {        
                (void)strcpy(ftpc_account, argv[4]);
            }            
        }        
        ftpcStatus.isConnected = false;
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
            return;
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
            return;
        }        
        (void)strcpy(ftpc_src_pathname, argv[2]);        
        res = TCPIP_FTPC_MakeDir(ftpcHandle, ftpc_src_pathname);
        ftpc_res_print(pCmdIO,res);

    }
    else if(strcmp("cd",argv[1])==0)
    {       
        if (argc != 3)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc cd <pathname>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc cd test\r\n");
            return;
        }        
        (void)strcpy(ftpc_src_pathname, argv[2]);
        res = TCPIP_FTPC_Change_Dir(ftpcHandle, ftpc_src_pathname);
        ftpc_res_print(pCmdIO,res);
        
    }
    else if(strcmp("cdup",argv[1])==0)
    {       
        if (argc != 2)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc cdup\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc cdup \r\n");
            return;
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
            return;
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
            return;
        }
        
        (void)strcpy(ftpc_src_pathname, argv[2]);
        res = TCPIP_FTPC_RemoveDir(ftpcHandle, ftpc_src_pathname);
        ftpc_res_print(pCmdIO,res);         
    }
    else if(strcmp("dele",argv[1])==0)
    {       
        if (argc != 3)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc dele <pathname>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc dele test.txt\r\n");
            return;
        }
        
        (void)strcpy(ftpc_src_pathname, argv[2]);
        res = TCPIP_FTPC_DeleteFile(ftpcHandle, ftpc_src_pathname);
        ftpc_res_print(pCmdIO,res);         
    }
    else if(strcmp("pasv",argv[1])==0)
    {    
        if (argc != 2)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc pasv\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc pasv\r\n");
            return;
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
            return;
        }
        
        if(TCPIP_Helper_StringToIPAddress(argv[2], &dataServerIpAddr.v4Add))
        {
            ftpcDataConn.dataServerIpAddrType = IP_ADDRESS_TYPE_IPV4;
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "FTPC: Invalid DataServer IP address.\r\n");
            return;
        } 
        (void)memcpy(&(ftpcDataConn.dataServerAddr), &(dataServerIpAddr), sizeof(IP_MULTI_ADDRESS));
           
        temp32 = 0UL;
        (void)FC_Str2UL(argv[3], 10, &temp32);
        ftpcDataConn.dataServerPort = (uint16_t)temp32;
        
        res = TCPIP_FTPC_SetActiveMode(ftpcHandle,&ftpcDataConn);
        ftpc_res_print(pCmdIO,res);         
    }
    else if(strcmp("get",argv[1])==0)
    {
        TCPIP_FTPC_DATA_CONN_TYPE ftpcDataConn;
        TCPIP_FTPC_FILE_OPT_TYPE fileOptions;
        static char serverFilename[20];
        static char clientFilename[20];
        
        if ((argc < 3)||(argc > 6))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc get <-a> <-p> <server_filename><client_filename>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc get -a -p test.txt 0 \r\n");
            return;
        }        
        ftpcDataConn.ftpcIsPassiveMode = false;  
        ftpcDataConn.ftpcDataType = TCPIP_FTPC_DATA_REP_ASCII;
        ftpcDataConn.ftpcDataTxBuffSize = 0;
        ftpcDataConn.ftpcDataRxBuffSize = 0;
        opt_count = 0U;
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
            else
            {
                // do nothing
            }
        }
        
        (void)strcpy(serverFilename, argv[opt_count + 2U]);
        if ((size_t)argc == opt_count + 4U)
        {
            if(strcmp("0",argv[opt_count + 3U]) != 0)
            {
                (void)strcpy(clientFilename, argv[opt_count + 3U]);
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
        res = TCPIP_FTPC_GetFile(ftpcHandle, &ftpcDataConn, &fileOptions, &dataSktHandler);
        ftpc_res_print(pCmdIO,res);
    }
    else if(strcmp("put",argv[1])==0)
    {
        TCPIP_FTPC_DATA_CONN_TYPE ftpcDataConn;
        TCPIP_FTPC_FILE_OPT_TYPE fileOptions;
        //TCPIP_FTPC_DATA_REP_TYPE ftpcDataType;
        static char serverFilename[20];
        static char clientFilename[20];
        
        if ((argc < 3)||(argc > 7))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc put <-a> <-p> <-u> <client_filename><server_filename>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc put -a -p test.txt 0 \r\n");
            return;
        }
        
        fileOptions.store_unique = false;
        ftpcDataConn.ftpcIsPassiveMode = false;        
        ftpcDataConn.ftpcDataType = TCPIP_FTPC_DATA_REP_ASCII;
        ftpcDataConn.ftpcDataTxBuffSize = 0;
        ftpcDataConn.ftpcDataRxBuffSize = 0;
        
        opt_count = 0U;
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
            else
            {
                // do nothing
            }
        }
        
        (void)strcpy(clientFilename, argv[opt_count +  2U]);        
        if ((size_t)argc == opt_count + 4U)
        {
            if(strcmp("0",argv[opt_count + 3U]) != 0)
            {
                (void)strcpy(serverFilename, argv[opt_count + 3U]);
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
        res = TCPIP_FTPC_PutFile(ftpcHandle,&ftpcDataConn,&fileOptions, &dataSktHandler);
        ftpc_res_print(pCmdIO,res);
    } 
    else if(strcmp("type",argv[1])==0)
    {
        TCPIP_FTPC_DATA_REP_TYPE ftpcDataType;
       
        if (argc != 3)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc type <representation type : a,e,i>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc type a\r\n");
            return;
        }
        
        if(strcmp("a",argv[2])==0)
        {
            ftpcDataType =  TCPIP_FTPC_DATA_REP_ASCII;
        }
        else if(strcmp("e",argv[2])==0)
        {
            ftpcDataType =  TCPIP_FTPC_DATA_REP_EBCDIC;
        }
        else if(strcmp("i",argv[2])==0)
        {
            ftpcDataType =  TCPIP_FTPC_DATA_REP_IMAGE;
        }
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
            return;
        }
        
        if(strcmp("f",argv[2])==0)
        {
            ftpcFileStruct =  TCPIP_FTPC_STRUCT_FILE;
        }
        else if(strcmp("r",argv[2])==0)
        {
            ftpcFileStruct =  TCPIP_FTPC_STRUCT_RECORD;
        }
        else if(strcmp("p",argv[2])==0)
        {
            ftpcFileStruct =  TCPIP_FTPC_STRUCT_PAGE;
        }
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
            return;
        }
        
        if(strcmp("s",argv[2])==0)
        {
           ftpcTranMode =  TCPIP_FTPC_TRANS_STREAM_MODE;
        }
        else if(strcmp("b",argv[2])==0)
        {
            ftpcTranMode =  TCPIP_FTPC_TRANS_BLOCK_MODE;
        }
        else if(strcmp("c",argv[2])==0)
        {
            ftpcTranMode =  TCPIP_FTPC_TRANS_COMPRESS_MODE;
        }
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

        if ((argc < 2)||(argc > 5))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc nlist -p <server_pathname><filename_to_savelist>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc nlist -p test.txt\r\n");
            return;
        }

        ftpcDataConn.ftpcIsPassiveMode = false;
        ftpcDataConn.ftpcDataTxBuffSize = 0;
        ftpcDataConn.ftpcDataRxBuffSize = 0;
        opt_count = 0U;
        for(i = 1; i < argc; i++)
        {
            if(strcmp("-p",argv[i])==0)
            {
                ftpcDataConn.ftpcIsPassiveMode = true;
                opt_count++;
            }
        }
        
        fileOptions.serverPathName = (char *)0;
        if((size_t)argc >= opt_count + 3U)
        {
            if(strcmp("0",argv[opt_count + 2U]) != 0)
            {
                (void)strcpy(serverPathname, argv[opt_count + 2U]);
                fileOptions.serverPathName = serverPathname;
            }       
        }

        fileOptions.clientPathName = (char *)"name_list.txt";
        if ((size_t)argc == opt_count + 4U)
        {
            if(strcmp("0",argv[opt_count + 3U]) != 0)
            {
                (void)strcpy(clientFilename, argv[opt_count + 3U]);
                fileOptions.clientPathName = clientFilename;
            } 
        }
        
        res = TCPIP_FTPC_NameList(ftpcHandle, &ftpcDataConn, &fileOptions, &dataSktHandler); 
        ftpc_res_print(pCmdIO,res); 
    }
    else if(strcmp("ls",argv[1])==0)
    {
        TCPIP_FTPC_DATA_CONN_TYPE ftpcDataConn;
        TCPIP_FTPC_FILE_OPT_TYPE fileOptions;
        static char serverPathname[20];        
        static char clientFilename[20];

        if ((argc < 2)||(argc > 5))
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ftpc ls -p <server_pathname><filename_to_savelist>\r\n");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: ftpc ls -p test list.txt\r\n");
            return;
        }

        ftpcDataConn.ftpcIsPassiveMode = false;
        ftpcDataConn.ftpcDataTxBuffSize = 0;
        ftpcDataConn.ftpcDataRxBuffSize = 0;
        opt_count = 0U;
        
        for(i = 1; i < argc; i++)
        {
            if(strcmp("-p",argv[i])==0)
            {
                ftpcDataConn.ftpcIsPassiveMode = true;
                opt_count++;
            }
        }

        fileOptions.serverPathName = (char *)0;
        if((size_t)argc == opt_count + 3U)
        {
            if(strcmp("0",argv[opt_count + 2U]) != 0)
            {
                (void)strcpy(serverPathname, argv[opt_count + 2U]);
                fileOptions.serverPathName = serverPathname;
            }       
        }
        fileOptions.clientPathName = (char *)"list.txt";
        if ((size_t)argc == opt_count + 4U)
        {
            if(strcmp("0",argv[opt_count + 3U]) != 0)
            {
                (void)strcpy(clientFilename, argv[opt_count + 3U]);
                fileOptions.clientPathName = clientFilename;
            } 
        }
                
        res = TCPIP_FTPC_List(ftpcHandle, &ftpcDataConn, &fileOptions, &dataSktHandler); 
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
}

#endif // defined(TCPIP_STACK_USE_FTP_CLIENT)


#if defined(TCPIP_STACK_USE_IPV4)  && defined(TCPIP_IPV4_COMMANDS) && (TCPIP_IPV4_COMMANDS != 0)
static void F_Command_Ipv4Arp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
static void F_Command_Ipv4Fwd(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void F_Command_Ipv4Table(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)

static void F_Command_Ipv4(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // ip4 arp/fwd/table ...

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    bool usage = true;
    while(argc > 1)
    {
        usage = false;
        if(strcmp(argv[1], "arp") == 0)
        {
            F_Command_Ipv4Arp(pCmdIO, argc, argv);
        }
#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
        else if(strcmp(argv[1], "fwd") == 0)
        {
            F_Command_Ipv4Fwd(pCmdIO, argc, argv);
        }
        else if(strcmp(argv[1], "table") == 0)
        {
            F_Command_Ipv4Table(pCmdIO, argc, argv);
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

static void F_Command_Ipv4Arp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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

    bool arpRes = TCPIP_IPv4_ArpStatGet(&arpStat, statClear);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 ARP Stat: %s\r\n", arpRes ? "success" : "Failed");
    if(arpRes)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "pool: %d, pend: %d, txSubmit: %d, fwdSubmit: %d\r\n", arpStat.nPool, arpStat.nPend, arpStat.txSubmit, arpStat.fwdSubmit);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "txSolved: %d, fwdSolved: %d, totSolved: %d, totFailed: %d\r\n", arpStat.txSolved, arpStat.fwdSolved, arpStat.totSolved, arpStat.totFailed);
    }
}

#if (TCPIP_IPV4_FORWARDING_ENABLE != 0)
static void F_Command_Ipv4Fwd(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // ip fwd ix clr
    
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint32_t index = 0UL;
    bool clear = false;

    if(argc > 2)
    {
        (void)FC_Str2UL(argv[2], 10, &index);
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

static void F_Command_Ipv4Table(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // ip table index
    size_t ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    TCPIP_IPV4_FORWARD_ENTRY_BIN fwdEntry = {0};
    uint32_t index = 0UL;

    if(argc > 2)
    {
        (void)FC_Str2UL(argv[2], 10, &index);
    }

    TCPIP_NET_HANDLE netH = TCPIP_STACK_IndexToNet(index);
    if(netH == NULL)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "table - no such iface: %d\r\n", index);
        return;
    }

    size_t usedEntries;
    size_t tableEntries = TCPIP_IPV4_ForwadTableSizeGet(netH, &usedEntries);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 Fwd Table for iface: %d, entries: %d, used: %d\r\n", index, tableEntries, usedEntries);

    for(ix = 0; ix < tableEntries; ix++)
    {
        (void)TCPIP_IPV4_ForwadTableEntryGet(netH, ix, &fwdEntry);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "IPv4 Fwd Entry: %d\r\n", ix);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tnetAdd: 0x%08x, netMask: 0x%08x, gwAdd: 0x%08x\r\n", fwdEntry.netAddress, fwdEntry.netMask, fwdEntry.gwAddress);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\toutIfIx: %d, inIfIx: %d, metric: %d, nOnes: %d\r\n", fwdEntry.outIfIx, fwdEntry.inIfIx, fwdEntry.metric, fwdEntry.nOnes);
    }

}
#endif  // (TCPIP_IPV4_FORWARDING_ENABLE != 0)


#endif  // defined(TCPIP_STACK_USE_IPV4)  && defined(TCPIP_IPV4_COMMANDS) && (TCPIP_IPV4_COMMANDS != 0)

#if (M_TCPIP_PKT_ALLOC_COMMANDS != 0)
static TCPIP_MAC_PACKET* allocPktList[10] = {0};

#define TCPIP_MAC_SEGMENT_GAP_TEST 0
#if (TCPIP_MAC_SEGMENT_GAP_TEST != 0)
extern uint32_t    u_tcpip_mac_segment_gap;
#endif  // (TCPIP_MAC_SEGMENT_GAP_TEST != 0)

static void F_Command_Packet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // pkt alloc pktLen segLoadLen
    // pkt free pktIx
    // pkt show pktIx
    // pkt gap gapSz
    // pkt list

    size_t ix;
    TCPIP_MAC_PACKET* pkt;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    while(argc >= 2)
    {

        if(strcmp(argv[1], "list") == 0)
        {
            int nPkts = 0;
            TCPIP_MAC_PACKET** ppPkt = allocPktList;
            for(ix = 0; ix < sizeof(allocPktList) / sizeof(*allocPktList); ix++)
            {
                if((pkt = *ppPkt) != NULL)
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "packet: 0x%08x, ix: %d\r\n", pkt, ix);
                    nPkts++;
                } 
                ppPkt++;
            }
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "allocated packets: %d\r\n", nPkts);
            return;
        }

#if (TCPIP_MAC_SEGMENT_GAP_TEST != 0)
        if(strcmp(argv[1], "gap") == 0)
        {
            if(argc >= 3)
            {
                u_tcpip_mac_segment_gap = 0UL;
                (void)FC_Str2UL(argv[2], 10, &u_tcpip_mac_segment_gap);
            }

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "MAC segment gap: %d\r\n", u_tcpip_mac_segment_gap);
            return;
        }
#endif  // (TCPIP_MAC_SEGMENT_GAP_TEST != 0)

        if(argc < 3)
        {
            break;
        }

        if(strcmp(argv[1], "free") == 0)
        {
            ix = 0UL;
            (void)FC_Str2UL(argv[2], 10, &ix);
            if(ix < sizeof(allocPktList) / sizeof(*allocPktList))
            {
                pkt = allocPktList[ix];
                if(pkt != NULL)
                {
                    TCPIP_PKT_PacketFree(pkt);
                    allocPktList[ix] = NULL;
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "freed packet: 0x%08x, index: %d\r\n", pkt, ix);
                    return;
                }
            }
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "free: no such packet index: %d\r\n", ix);
            return;
        }

        if(strcmp(argv[1], "show") == 0)
        {
            ix = 0UL;
            (void)FC_Str2UL(argv[2], 10, &ix);
            if(ix < sizeof(allocPktList) / sizeof(*allocPktList))
            {
                pkt = allocPktList[ix];
                if(pkt != NULL)
                {
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "showing packet: 0x%08x, index: %d\r\n", pkt, ix);
                    TCPIP_MAC_DATA_SEGMENT* pSeg = pkt->pDSeg;
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "pDSeg: 0x%08x, segLoad: 0x%08x\r\n", pSeg, pSeg->segLoad);
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "segLen: %d, segSize: %d, segAllocSize: %d\r\n", pSeg->segLen, pSeg->segSize, pSeg->segAllocSize);
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

            uint32_t len32 = 0UL;
            (void)FC_Str2UL(argv[3], 10, &len32);
            uint16_t segLoadLen = (uint16_t)len32;
            len32 = 0UL;
            (void)FC_Str2UL(argv[2], 10, &len32);
            uint16_t pktLen = (uint16_t)len32;

            pkt = TCPIP_PKT_PacketAlloc(pktLen, segLoadLen, TCPIP_MAC_PKT_FLAG_NONE);
            if(pkt == NULL)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Failed to allocate packet!\r\n");
                return;
            }

            // find a spot
            TCPIP_MAC_PACKET** ppPkt = allocPktList;
            for(ix = 0; ix < sizeof(allocPktList) / sizeof(*allocPktList); ix++)
            {
                if(*ppPkt == NULL)
                {
                    *ppPkt = pkt;
                    (*pCmdIO->pCmdApi->print)(cmdIoParam, "packet: 0x%08x, added to ix: %d\r\n", pkt, ix);
                    return;
                } 
                ppPkt++;
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
#endif  // (M_TCPIP_PKT_ALLOC_COMMANDS != 0)

#if defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)

static void F_Command_BridgeShowStats(SYS_CMD_DEVICE_NODE* pCmdIO, TCPIP_MAC_BRIDGE_HANDLE brH, bool clearStat)
{
    int ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    TCPIP_MAC_BRIDGE_STAT stat;
    TCPIP_MAC_BRIDGE_PORT_STAT* pPort;
    bool res = TCPIP_MAC_Bridge_StatisticsGet(brH, &stat, clearStat);

    if(res == false)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "failed to get stats!\r\n");
        return;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "stats returned: %d\r\n", res);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t failPktAlloc: %d, failDcptAlloc: %d, failLocks: %d, fdbFull: %d\r\n", stat.failPktAlloc, stat.failDcptAlloc, stat.failLocks, stat.fdbFull);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t failMac: %d, failMtu: %d, failSize: %d\r\n", stat.failMac, stat.failMtu, stat.failSize);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t allocPackets: %d, freedPackets: %d, ackPackets: %d, delayPackets: %d\r\n", stat.allocPackets, stat.freedPackets, stat.ackPackets, stat.delayPackets);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t pktPoolSize: %d, pktPoolLowSize: %d, pktPoolEmpty: %d\r\n", stat.pktPoolSize, stat.pktPoolLowSize, stat.pktPoolEmpty);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t dcptPoolSize: %d, dcptPoolLowSize: %d, dcptPoolEmpty: %d\r\n", stat.dcptPoolSize, stat.dcptPoolLowSize, stat.dcptPoolEmpty);

    pPort = stat.portStat;
    for(ix = 0; ix < TCPIP_MAC_BRIDGE_MAX_PORTS_NO; ix++)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t port %d stats:\r\n", ix);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t\t pkts received: %d, dest me-ucast: %d, dest notme-ucast: %d, dest mcast: %d\r\n", pPort->rxPackets, pPort->rxDestMeUcast, pPort->rxDestNotMeUcast, pPort->rxDestMcast);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t\t pkts reserved: %d, fwd ucast: %d, fwd mcast: %d, fwd direct: %d\r\n", pPort->reservedPackets, pPort->fwdUcastPackets, pPort->fwdMcastPackets, pPort->fwdDirectPackets);
        pPort++;
    }
}

static void F_Command_BridgeShowFDB(SYS_CMD_DEVICE_NODE* pCmdIO, TCPIP_MAC_BRIDGE_HANDLE brH)
{
    // list the FDB
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    size_t nEntries = TCPIP_MAC_Bridge_FDBEntries(brH);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "FDB entries: %d\r\n", nEntries);

    TCPIP_MAC_FDB_ENTRY fdbEntry;
    size_t ix;
    for(ix = 0; ix < nEntries; ix++)
    {
        if(TCPIP_MAC_Bridge_FDBIndexRead(brH, ix, &fdbEntry) == TCPIP_MAC_BRIDGE_RES_OK)
        {   // display it
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\n\tEntry number: %d\r\n", ix);
            char addrBuff[20];
            (void)TCPIP_Helper_MACAddressToString(&fdbEntry.destAdd, addrBuff, sizeof(addrBuff));
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tEntry destAdd: %s\r\n", addrBuff);

            char flagsBuff[40];
            if(fdbEntry.flags == 0U)
            {   // this should NOT happen
                (void)FC_sprintf(flagsBuff, sizeof(flagsBuff), "%s\r\n", "none");
            }
            else
            {
                int nChars = FC_sprintf(flagsBuff, sizeof(flagsBuff), "%s", (fdbEntry.flags & (uint8_t)TCPIP_MAC_FDB_FLAG_STATIC) != 0U ? "static" : "dynamic");
                nChars += FC_sprintf(flagsBuff + nChars, sizeof(flagsBuff) - (size_t)nChars, ", %s", (fdbEntry.flags & (uint8_t)TCPIP_MAC_FDB_FLAG_HOST) != 0U ? "host" : "ext");
                (void)FC_sprintf(flagsBuff + nChars, sizeof(flagsBuff) - (size_t)nChars, ", port %s", (fdbEntry.flags & (uint8_t)TCPIP_MAC_FDB_FLAG_PORT_VALID) != 0U ? "valid" : "invalid");
            }

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tEntry flags: 0x%02x: %s\r\n", fdbEntry.flags, flagsBuff);

            if((fdbEntry.flags & (uint8_t)TCPIP_MAC_FDB_FLAG_PORT_VALID) != 0U) 
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tEntry learn port: %d\r\n", fdbEntry.learnPort);
            }

            if((fdbEntry.flags & (uint8_t)TCPIP_MAC_FDB_FLAG_STATIC) != 0U) 
            {   // display the outPortMap 
                size_t jx, kx;
                const char* controlStr[TCPIP_MAC_BRIDGE_CONTROL_TYPES] = {"def", "fwd", "filt"};
                size_t mapEntries = sizeof(fdbEntry.outPortMap[0]) / sizeof(*fdbEntry.outPortMap[0]); 
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

#if defined(TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS ) && (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
static void F_Command_BridgeResetFDB(SYS_CMD_DEVICE_NODE* pCmdIO, TCPIP_MAC_BRIDGE_HANDLE brH)
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
    {0U, (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD},
    {1U, (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FORWARD},
};

static TCPIP_MAC_BRIDGE_CONTROL_DCPT dcptCtrl2[] =
{
    {0U, (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FILTER},
    {1U, (uint8_t)TCPIP_MAC_BRIDGE_CONTROL_FILTER},
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

#if defined (TCPIP_MAC_BRIDGE_EVENT_NOTIFY) && (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
static uint32_t bridgeEvents = 0;
static TCPIP_MAC_BRIDGE_EVENT_HANDLE bridgeEventHandle = NULL;


static void F_Command_BridgeEventHandler(TCPIP_MAC_BRIDGE_EVENT evType, const void* param)
{
    const TCPIP_MAC_ADDR* pMacAdd;
    char addBuff[20];
    char evBuff[100];

    addBuff[0] = evBuff[0] = '\0';

    switch(evType)
    {
        case TCPIP_MAC_BRIDGE_EVENT_FDB_FULL:
            pMacAdd = (const TCPIP_MAC_ADDR*)param;
            (void)TCPIP_Helper_MACAddressToString(pMacAdd, addBuff, sizeof(addBuff));
            (void)FC_sprintf(evBuff, sizeof(evBuff), "%s, address: %s\r\n", "fdb full", addBuff);
            break;

        case TCPIP_MAC_BRIDGE_EVENT_FAIL_PKT_ALLOC:
            (void)FC_sprintf(evBuff, sizeof(evBuff), "%s, packets: %lu\r\n", "fail alloc", FC_CvPtr2U32(param));
            break;

        case TCPIP_MAC_BRIDGE_EVENT_FAIL_DCPT_ALLOC:
            (void)FC_sprintf(evBuff, sizeof(evBuff), "%s, descriptors: %lu\r\n", "fail alloc", FC_CvPtr2U32(param));
            break;

        case TCPIP_MAC_BRIDGE_EVENT_FAIL_MTU:
            (void)FC_sprintf(evBuff, sizeof(evBuff), "%s, size: %lu\r\n", "fail MTU", FC_CvPtr2U32(param));
            break;

        case TCPIP_MAC_BRIDGE_EVENT_FAIL_SIZE:
            (void)FC_sprintf(evBuff, sizeof(evBuff), "%s, size: %lu\r\n", "fail Size", FC_CvPtr2U32(param));
            break;

        case TCPIP_MAC_BRIDGE_EVENT_PKT_POOL_EMPTY:
            (void)FC_sprintf(evBuff, sizeof(evBuff), "%s\r\n", "pkt pool empty");
            break;

        case TCPIP_MAC_BRIDGE_EVENT_DCPT_POOL_EMPTY:
            (void)FC_sprintf(evBuff, sizeof(evBuff), "%s\r\n", "dcpt pool empty");
            break;

        case TCPIP_MAC_BRIDGE_EVENT_FAIL_LOCK:
            (void)FC_sprintf(evBuff, sizeof(evBuff), "%s\r\n", "fail lock");
            break;

        case TCPIP_MAC_BRIDGE_EVENT_ENTRY_ADDED:
            pMacAdd = FC_CVptr2MacAdd(param);
            (void)TCPIP_Helper_MACAddressToString(pMacAdd, addBuff, sizeof(addBuff));
            (void)FC_sprintf(evBuff, sizeof(evBuff), "%s, address: %s\r\n", "entry added", addBuff);
            break;

        case TCPIP_MAC_BRIDGE_EVENT_ENTRY_EXPIRED:
            pMacAdd = FC_CVptr2MacAdd(param);
            (void)TCPIP_Helper_MACAddressToString(pMacAdd, addBuff, sizeof(addBuff));
            (void)FC_sprintf(evBuff, sizeof(evBuff), "%s, address: %s\r\n", "entry expired", addBuff);
            break;

        default:
            (void)FC_sprintf(evBuff, sizeof(evBuff), "unknown!\r\n");
            break;
    }

    bridgeEvents++;
    SYS_CONSOLE_PRINT("Bridge event: %s, total events: %d\r\n", evBuff, bridgeEvents);
}
#endif  // (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 



static void F_Command_Bridge(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
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

            F_Command_BridgeShowStats(pCmdIO, brH, clearStat);
            return;
        }

#if defined (TCPIP_MAC_BRIDGE_EVENT_NOTIFY) && (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0)         
        if(strcmp(argv[1], "register") == 0)
        {
            if(bridgeEventHandle == NULL)
            {
                bridgeEventHandle = TCPIP_MAC_Bridge_EventHandlerRegister(brH, &F_Command_BridgeEventHandler);
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
            if(bridgeEventHandle != NULL)
            {
                bool derRes = TCPIP_MAC_Bridge_EventHandlerDeregister(brH, bridgeEventHandle);
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "de-register result: %d\r\n", derRes);
                if(derRes)
                {
                    bridgeEventHandle = NULL;
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
                    F_Command_BridgeShowFDB(pCmdIO, brH);
                    return;
                }

#if defined(TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS ) && (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
                
                if(strcmp(argv[2], "reset") == 0)
                {
                    F_Command_BridgeResetFDB(pCmdIO, brH);
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
#if defined (TCPIP_MAC_BRIDGE_EVENT_NOTIFY) && (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: bridge register <param>\r\n");
#endif  // (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
#if defined(TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS ) && (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)    
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: bridge fdb show/reset/add/delete\r\n");
#else
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: bridge fdb show\r\n");
#endif  // (TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
}
#endif // defined(TCPIP_STACK_USE_MAC_BRIDGE) && (TCPIP_STACK_MAC_BRIDGE_COMMANDS != 0)


#if defined(M_TCPIP_STACK_PPP_COMMANDS)

// PPP commands
static const char* pppStatNames[] = 
{
    "lcpPkts",        
    "ipcpPkts",
    "ipPkts",         
    "tcpPkts",        
    "pppQueued",
    "netQueued",
    "echoReqFree",
    "echoReqQueued",
    "echoDiscardPkts",
    "echoReqPkts",
    "echoReplyPkts",
    "discardPkts", 
    "protoErr",   
    "lengthErr", 
    "mruErr",   
    "codeErr", 
    "formatErr",      
    "rcaMatchErr",
    "rcrIdentErr",
    "rucErr",         
    "rxrErr",    
    "rxjErr",   
    "rxjProtoErr",
    "rxjCodeErr",
    "crossedErr",
    "peerMagicErr", 
    "loopbackErr", 
    "lcpCodeErr", 
    "optionErr", 
    "hdlcWriteErr",   
    "illegalEvents",
    "buffFail", 
};

static union
{
    PPP_STATISTICS  stat;
    uint32_t        statReg[sizeof(PPP_STATISTICS) / sizeof(uint32_t)];
}
pppStatValues;



static void DoPppStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    bool statClr = false;
    bool statShort = false;

    int currIx = 2;

    while(currIx + 1 < argc)
    { 
        char* param = argv[currIx];
        if(strcmp(param, "clr") == 0)
        {
            statClr = true;
        }
        else if(strcmp(param, "short") == 0)
        {
            statShort = true;
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ppp stat: Unknown parameter\r\n");
            return;
        }

        currIx += 1;
    }

    DRV_HANDLE hPPP = DRV_PPP_MAC_Open((uint16_t)TCPIP_MODULE_MAC_PPP_0, DRV_IO_INTENT_SHARED);
    bool res = PPP_StatisticsGet(hPPP, &pppStatValues.stat, statClr);
    if(res == false)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ppp stat: failed to get statistics!\r\n");
        return;
    }

    // if statShort display only members != 0
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ppp stats: \r\n");
    size_t ix;
    uint32_t* pStat = pppStatValues.statReg;
    for(ix = 0; ix < sizeof(pppStatValues.statReg) / sizeof(*pppStatValues.statReg); ix++)
    {
        uint32_t statVal = *pStat;
        if(statShort == false || statVal != 0U)
        {
            const char* statName = pppStatNames[ix];
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tstat %s: %ld\r\n", statName, statVal);
        }
        pStat++;
    }
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ppp stats end\r\n");

}

// normally event should be Open/Close only!
static void DoPppAdmin(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv, PPP_EVENT event)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    PPP_CTRL_PROTO ctlProt = PPP_CTRL_PROTO_LCP;
    const char* ctlName = "lcp";

    if(argc > 2)
    {
        ctlName = argv[2];
        if(strcmp(argv[2], "lcp") == 0)
        {
            ctlProt = PPP_CTRL_PROTO_LCP;
        }
        else if(strcmp(argv[2], "ipcp") == 0)
        {
            ctlProt = PPP_CTRL_PROTO_IPCP;
        }
        else
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "ppp unknown protocol: %s\r\n", argv[2]);
            return;
        }
    }
    DRV_HANDLE hPPP = DRV_PPP_MAC_Open((uint16_t)TCPIP_MODULE_MAC_PPP_0, DRV_IO_INTENT_SHARED);
    
    bool res = PPP_SendAdminEvent(hPPP, event, ctlProt);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "ppp sent event %s, to: %s, res: %d\r\n", argv[1], ctlName, res);
}


static void DoPppAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    char lclAddrBuff[20];
    char remAddrBuff[20];

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    DRV_HANDLE hPPP = DRV_PPP_MAC_Open((uint16_t)TCPIP_MODULE_MAC_PPP_0, DRV_IO_INTENT_SHARED);
    
    IPV4_ADDR lclAddr, remAddr;
    lclAddr.Val = PPP_GetLocalIpv4Addr(hPPP);
    remAddr.Val = PPP_GetRemoteIpv4Addr(hPPP);

    (void)TCPIP_Helper_IPAddressToString(&lclAddr, lclAddrBuff, sizeof(lclAddrBuff));
    (void)TCPIP_Helper_IPAddressToString(&remAddr, remAddrBuff, sizeof(remAddrBuff));

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "ppp local address: %s, remote: %s\r\n", lclAddrBuff, remAddrBuff);
}

static void DoPppState(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    PPP_STATE state[2];

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    DRV_HANDLE hPPP = DRV_PPP_MAC_Open((uint16_t)TCPIP_MODULE_MAC_PPP_0, DRV_IO_INTENT_SHARED);
    
    bool res = PPP_GetState(hPPP, state);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "ppp state - LCP: %d, IPCP: %d, res: %d\r\n", state[0], state[1], res);
}

static void DoPppEcho(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int     currIx;    
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if (argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Echo Usage: ppp echo <stop> <n nEchoes> <t msPeriod> <s size>\r\n");
        return;
    }

    if(argc > 2 && strcmp(argv[2], "stop") == 0)
    {
        if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE)
        {
            F_PPPEchoStop(pCmdIO, cmdIoParam);
        }
        return;
    }

    if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ppp echo: command in progress. Retry later.\r\n");
        return;
    }

    // get additional parameters, if any
    //
    pppReqNo = 0UL;
    pppReqDelay = 0UL;

    currIx = 2;

    while(currIx + 1 < argc)
    { 
        char* param = argv[currIx];
        char* paramVal = argv[currIx + 1];

        if(strcmp(param, "n") == 0)
        {
            (void)FC_Str2UL(paramVal, 10, &pppReqNo);
        }
        else if(strcmp(param, "t") == 0)
        {
            (void)FC_Str2UL(paramVal, 10, &pppReqDelay);
        }
        else if(strcmp(param, "s") == 0)
        {
            uint32_t echoSize = 0UL;
            (void)FC_Str2UL(paramVal, 10, &echoSize);
            if(echoSize <= sizeof(pppEchoBuff))
            {
                pppEchoSize = (uint16_t)echoSize;
            }
            else
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "ppp echo: Data size too big. Max: %d. Retry\r\n", sizeof(pppEchoBuff));
                return;
            }

        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "ppp echo: Unknown parameter\r\n");
        }

        currIx += 2;
    }


    tcpipCmdStat = TCPIP_CMD_STAT_PPP_START;
    pppSeqNo = (uint16_t)SYS_RANDOM_PseudoGet();

    if(pppReqNo == 0U)
    {
        pppReqNo = (size_t)TCPIP_STACK_COMMANDS_PPP_ECHO_REQUESTS;
    }
    if(pppReqDelay == 0U)
    {
        pppReqDelay = (uint32_t)TCPIP_STACK_COMMANDS_PPP_ECHO_REQUEST_DELAY;
    }

    // convert to ticks
    if(pppReqDelay < (uint32_t)TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY)
    {
        pppReqDelay = (uint32_t)TCPIP_COMMAND_ICMP_ECHO_REQUEST_MIN_DELAY;
    }

    pTcpipCmdDevice = pCmdIO;
    pppCmdIoParam = cmdIoParam; 
    pppAckRecv = 0U;
    pppReqCount = 0U;

    (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, (int16_t)pppReqDelay);

}

static void F_Command_Ppp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // ppp stat <short> <clr> 
    // ppp open/close/addr/state
    // ppp echo n x

    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if(argc > 1)
    {
        if(strcmp(argv[1], "stat") == 0)
        {
            DoPppStat( pCmdIO, argc, argv);
            return;
        }
        else if(strcmp(argv[1], "open") == 0)
        {
            DoPppAdmin( pCmdIO, argc, argv, PPP_EVENT_OPEN);
            return;
        }
        else if(strcmp(argv[1], "close") == 0)
        {
            DoPppAdmin( pCmdIO, argc, argv, PPP_EVENT_CLOSE);
            return;
        }
        else if(strcmp(argv[1], "addr") == 0)
        {
            DoPppAddr( pCmdIO, argc, argv);
            return;
        }
        else if(strcmp(argv[1], "state") == 0)
        {
            DoPppState( pCmdIO, argc, argv);
            return;
        }
        else if(strcmp(argv[1], "echo") == 0)
        {
            DoPppEcho( pCmdIO, argc, argv);
            return;
        }
        else
        {
            // do nothing
        }
    }


   (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: ppp open/close/addr/stat/state <short> <clr>\r\n");

}
#if defined(M_TCPIP_STACK_PPP_ECHO_COMMAND)
static void TCPIPCmd_PppEchoTask(void)
{
    PPP_ECHO_RESULT echoRes;
    PPP_ECHO_REQUEST echoRequest;
    bool cancelReq, newReq;
    size_t ix;
    uint8_t*    pBuff;
    DRV_HANDLE  hPPP;
    bool killPpp = false;
       
    switch(tcpipCmdStat)
    {
        case TCPIP_CMD_STAT_PPP_START:
            pppStartTick = 0U;  // try to start as quickly as possible
            tcpipCmdStat = TCPIP_PPP_CMD_DO_ECHO;            
            break;

        case TCPIP_PPP_CMD_DO_ECHO:
            if(pppReqCount == pppReqNo)
            {   // no more requests to send
                killPpp = true;
                break;
            }

            // check if time for another request
            cancelReq = newReq = false;
            if(SYS_TMR_TickCountGet() - pppStartTick > (sysFreq * pppReqDelay) / 1000U)
            {
                cancelReq = pppReqCount != pppAckRecv && pppReqHandle != NULL;    // cancel if there is another one ongoing
                newReq = true;
            }
            else if(pppReqCount != pppAckRecv)
            {   // no reply received to the last ping 
                if(SYS_TMR_TickCountGet() - pppStartTick > (sysFreq * (uint32_t)TCPIP_STACK_COMMANDS_PPP_ECHO_TIMEOUT) / 1000U)
                {   // timeout
                    cancelReq = pppReqHandle != NULL;    // cancel if there is another one ongoing
                    newReq = true;
                }
            }
            else
            {
                // else wait some more
            }

            if(cancelReq)
            {
                (void)PPP_EchoRequestCancel(pppReqHandle);
            }

            if(!newReq)
            {   // nothing else to do
                break;
            }

            // send another request
            echoRequest.pData = pppEchoBuff;
            echoRequest.dataSize = pppEchoSize;
            echoRequest.seqNumber = ++pppSeqNo;
            echoRequest.callback = &F_PPPEchoHandler;
            echoRequest.param = NULL;

            // fill the buffer
            pBuff = pppEchoBuff;
            for(ix = 0; ix < (size_t)pppEchoSize; ix++)
            {
                *pBuff++ = (uint8_t)SYS_RANDOM_PseudoGet();
            }

            hPPP = DRV_PPP_MAC_Open((uint16_t)TCPIP_MODULE_MAC_PPP_0, DRV_IO_INTENT_SHARED);
            echoRes = PPP_EchoRequest (hPPP, &echoRequest, &pppReqHandle);

            if((int)echoRes >= 0 )
            {
                pppStartTick = SYS_TMR_TickCountGet();
                pppReqCount++;
            }
            else
            {
                killPpp = true;
            }

            break;

        default:
            killPpp = true;
            break;

    }

    if(killPpp)
    {
        F_PPPEchoStop(pTcpipCmdDevice, icmpCmdIoParam);
    }
}

static void F_PPPEchoHandler(const PPP_ECHO_REQUEST* pEchoReq, PPP_REQUEST_HANDLE pppHandle, PPP_ECHO_RESULT result, const void* param)
{
    if(result == PPP_ECHO_OK)
    {   // reply has been received
        uint32_t errorMask = 0U;     // error mask:
        // 0x1: wrong seq
        // 0x2: wrong size
        // 0x4: wrong data
        //
        if(pEchoReq->seqNumber != pppSeqNo)
        {
            errorMask |= 0x1U;
        }

        if(pEchoReq->dataSize != pppEchoSize)
        {
            errorMask |= 0x2U;
        }

        // check the data
        size_t ix;
        uint16_t checkSize = pEchoReq->dataSize < pppEchoSize ? pEchoReq->dataSize : pppEchoSize;
        uint8_t* pSrc = pppEchoBuff;
        uint8_t* pDst = pEchoReq->pData;
        for(ix = 0; ix < (size_t)checkSize; ix++)
        {
            if(*pSrc++ != *pDst++)
            {
                errorMask |= 0x04U;
                break;
            }
        }

        if(errorMask != 0U)
        {   // some errors
            (*pTcpipCmdDevice->pCmdApi->print)(pppCmdIoParam, "Echo: wrong reply received. Mask: 0x%2x\r\n", errorMask);
        }
        else
        {   // good reply
            uint32_t echoTicks = SYS_TMR_TickCountGet() - pppStartTick;
            uint32_t echoMs = (echoTicks * 1000U) / sysFreq;
            if(echoMs == 0U)
            {
                echoMs = 1U;
            }

            (*pTcpipCmdDevice->pCmdApi->print)(pppCmdIoParam, "Echo: reply[%d], time = %dms\r\n", ++pppAckRecv, echoMs);
        }
    }
    else
    {
        pTcpipCmdDevice->pCmdApi->print(pppCmdIoParam, "Echo error: %d\r\n", result);
    }
    // one way or the other, request is done
    pppReqHandle = NULL;
}

static void F_PPPEchoStop(SYS_CMD_DEVICE_NODE* pCmdIO, const void* cmdIoParam)
{
    if(pppReqHandle != NULL)
    {
        (void)PPP_EchoRequestCancel(pppReqHandle);

        pppReqHandle = NULL;
    }

    (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, 0);
    tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
    if(pCmdIO != NULL)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "PPP Echo: done. Sent %d requests, received %d replies.\r\n", pppReqCount, pppAckRecv);
    }
    pTcpipCmdDevice = NULL;
}


#endif  // defined(M_TCPIP_STACK_PPP_ECHO_COMMAND)
#endif  // defined(M_TCPIP_STACK_PPP_COMMANDS)

#if defined(M_TCPIP_STACK_HDLC_COMMANDS)
static const char* hdlcStatNames[] =
{
    "txFrames",
    "txTotChars",
    "rxFrames",
    "rxChainedFrames",
    "rxTotChars",
    "freeFrames",
    "busyFrames",
    "pendFrames",
    "rxBuffNA",
    "txAllocErr",
    "serialWrSpaceErr",
    "serialWrErr",
    "rxShortFrames",
    "rxLongFrames",
    "rxFormatErr",
    "rxFcsErr",
};

static union
{
    DRV_HDLC_STATISTICS stat;
    uint32_t            statReg[sizeof(DRV_HDLC_STATISTICS) / sizeof(uint32_t)];
}hdlcStatValues;


static void DoHdlcStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // extern const DRV_HDLC_OBJECT DRV_HDLC_AsyncObject;   // defined by the HDLC driver
    
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool statClr = false;
    if(argc > 2)
    {
        if(strcmp(argv[2], "clr") == 0)
        {
            statClr = true;
        }
    }

    DRV_HANDLE hHdlc = DRV_HDLC_AsyncObject.open(0, 0);
    
    bool res = DRV_HDLC_AsyncObject.getStatistics(hHdlc, &hdlcStatValues.stat, statClr);

    if(res == false)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "hdlc stat: failed to get statistics!\r\n");
        return;
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "hdlc stats: \r\n");
    size_t ix;
    uint32_t* pStat = hdlcStatValues.statReg;
    for(ix = 0; ix < sizeof(hdlcStatValues.statReg) / sizeof(*hdlcStatValues.statReg); ix++)
    {
        const char* statName = hdlcStatNames[ix];
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tstat %s: %ld\r\n", statName, *pStat);
        pStat++;
    }
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "hdlc stats end\r\n");
    

}

static void F_Command_Hdlc(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // hdlc stat <clr>; HDLC statistics

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc > 1)
    {
        if(strcmp(argv[1], "stat") == 0)
        {
            DoHdlcStat(pCmdIO, argc, argv);
            return;
        }

    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "HDLC usage: hdlc stat <clr>\r\n");
}
#endif  // defined(M_TCPIP_STACK_HDLC_COMMANDS)

#if defined(TCPIP_STACK_RUN_TIME_INIT) && (TCPIP_STACK_RUN_TIME_INIT != 0)
static void F_Command_ModDeinit(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // deinit moduleId

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc > 1)
    {
        uint32_t modId = 0UL;
        (void)FC_Str2UL(argv[1], 10, &modId);
        bool res = TCPIP_MODULE_Deinitialize((TCPIP_STACK_MODULE)modId);

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "deinit module: %d returned: %d\r\n", modId, res);
    }
}

static void F_Command_ModRunning(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // runstat moduleId

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc > 1)
    {
        uint32_t modId = 0UL;
        (void)FC_Str2UL(argv[1], 10, &modId);
        bool res = TCPIP_MODULE_IsRunning((TCPIP_STACK_MODULE)modId);

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "runstat module: %d returned: %d\r\n", modId, res);
    }
}
#endif  // defined(TCPIP_STACK_RUN_TIME_INIT) && (TCPIP_STACK_RUN_TIME_INIT != 0)

#if defined(TCPIP_STACK_USE_SNMPV3_SERVER)  
static uint8_t SNMPV3_USM_ERROR_STR[SNMPV3_USM_NO_ERROR][100]=
{
    /* SNMPV3_USM_SUCCESS=0 */
    "\r\n",
    /* SNMPV3_USM_INVALID_INPUTCONFIG */
    "Error! Invalid input parameter \r\n", 
    /* SNMPV3_USM_INVALID_USER */
    "Error! User index position value exceeds the user configuration \r\n",
    /* SNMPV3_USM_INVALID_USERNAME */
    "Error! Invalid user name \r\n",
   /* SNMPV3_USM_INVALID_USER_NAME_LENGTH */
    "Error!Invalid User name length \r\n",
    /* SNMPV3_USM_INVALID_PRIVAUTH_PASSWORD_LEN */
    "Error!Invalid Auth and Priv password length \r\n",
    /* SNMPV3_USM_INVALID_PRIVAUTH_LOCALIZED_PASSWORD_LEN */
    "Error!Invalid Auth and Priv localized password length \r\n",
    /* SNMPV3_USM_INVALID_PRIVAUTH_TYPE */
    "Error!Privacy Authentication security level configuration not allowed \r\n",
    /* SNMPV3_USM_INVALID_AUTH_CONFIG_NOT_ALLOWED */
    "Error! Authentication security level configuration not allowed \r\n",
    /* SNMPV3_USM_INVALID_PRIV_CONFIG_NOT_ALLOWED */
    "Error! Privacy security level configuration not allowed \r\n",
    /*SNMPV3_USM_INVALID_SECURITY_LEVEL */
    "Error! Invalid USM Security level type  \r\n",
    /*SNMPV3_USM_NOT_SUPPORTED */
    "Error! USM Set configuration not allowed \r\n",
};
/*
 "Usage: snmpv3 usm <pos> <name> <security-level> <authpass> <privpass>"
 * pos - USM config table 
 * 
 * usmOpcode = 0 ; only user name configuration
 * usmOpcode = 1 ; both username and security level configuration
 * usmOpcode = 0 ; only user name configuration
 * usmOpcode = 1 ; both username and security level configuration
 */

static void F_Command_SNMPv3USMSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint8_t  usmPos=TCPIP_SNMPV3_USM_MAX_USER;
    bool     usmUserNameOpcode=false;
    bool     usmSecLevelOpcode=false;
    bool     usmAuthPasswdOpcode=false;
    bool     usmPrivPasswdOpcode=false;
    bool     usmUserInfo=false;
    char     userNameBuf[TCPIP_SNMPV3_USER_SECURITY_NAME_LEN+1];
    char     authPwBuf[TCPIP_SNMPV3_PRIVAUTH_PASSWORD_LEN+1];
    char     privPwBuf[TCPIP_SNMPV3_PRIVAUTH_PASSWORD_LEN+1];
    SNMPV3_PRIV_PROT_TYPE privType=SNMPV3_NO_PRIV;
    SNMPV3_HMAC_HASH_TYPE hashType=SNMPV3_NO_HMAC_AUTH;
    STD_BASED_SNMPV3_SECURITY_LEVEL  secLev = NO_AUTH_NO_PRIV;
    int  configArgs=0;
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE result=SNMPV3_USM_NO_ERROR;
    
    if(argc < 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: snmpv3 usm <pos> <u name> <l security-level> <a type authpass> <p type privpass> <info>\r\n");
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "SNMPv3 USM position range - 0 to %d \r\n", TCPIP_SNMPV3_USM_MAX_USER-1);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "SNMPv3 USM security level NoAuthNoPriv- %d, AuthNoPriv- %d, AuthPriv- %d \r\n", NO_AUTH_NO_PRIV, AUTH_NO_PRIV, AUTH_PRIV);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "SNMPv3 USM authentication supported type md5- %d sha- %d \r\n", SNMPV3_HMAC_MD5, SNMPV3_HMAC_SHA1);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "SNMPv3 USM privacy supported type DES- %d AES- %d \r\n", SNMPV3_DES_PRIV, SNMPV3_AES_PRIV);
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: snmpv3 usm 2 u mchp l 3 a 0 auth12345 p 1 priv12345 \r\n");
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Ex: snmpv3 usm info \r\n");
        return;
    }

    configArgs = 1;
    if(strcmp(argv[configArgs], "usm") == 0)
    {
        configArgs = configArgs +1;
        if(strcmp("info", argv[configArgs]) == 0)
        {
            usmUserInfo = true;
        }
        else
        {
            usmPos = (uint8_t)FC_Str2UL(argv[configArgs], 10, NULL);
            if(usmPos>= (uint8_t)TCPIP_SNMPV3_USM_MAX_USER)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam,"Invalid USM configuration position\r\n");
                return;
            }
        }
    }

    // more than position field 
    (void)memset(userNameBuf, 0, sizeof(userNameBuf));
    (void)memset(authPwBuf, 0, sizeof(authPwBuf));
    (void)memset(privPwBuf, 0, sizeof(privPwBuf));

    configArgs = configArgs + 1;
    // verify there are enough commands for this SNMPv3 configuration
    if(argc == configArgs)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam,"SNMPv3 insufficient user inputs\r\n");
        return;
    }
    
    if(usmUserInfo != true)
    {
        while(argc > 3)
        {
            if(strncmp("u",argv[configArgs],1) == 0)
            {
                if(argc == configArgs+1)
                {
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam,"SNMPv3 User name is missing\r\n");
                    return;
                }
                (void)strncpy(userNameBuf,argv[configArgs+1],TCPIP_SNMPV3_USER_SECURITY_NAME_LEN);
                usmUserNameOpcode = true;
                configArgs += 2;
            }
            else if(strncmp("l",argv[configArgs],1) == 0)
            {
                if(argc == configArgs+1)
                {
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam,"SNMPv3 security level is missing\r\n");
                    return;
                }
                secLev = (STD_BASED_SNMPV3_SECURITY_LEVEL)FC_Str2UL(argv[configArgs+1], 10, NULL);
                usmSecLevelOpcode = true;
                configArgs += 2;
            }
            else if(strncmp("a",argv[configArgs],1) == 0)
            {
                if(argc == configArgs+1)
                {
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam,"SNMPv3 Authentication Hash type is missing\r\n");
                    return;
                }
                hashType = (SNMPV3_HMAC_HASH_TYPE)FC_Str2UL(argv[configArgs+1], 10, NULL);
                if(argc == configArgs+2)
                {
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam,"SNMPv3 Authentication password is missing\r\n");
                    return;
                }
                (void)strncpy(authPwBuf,argv[configArgs+2],TCPIP_SNMPV3_PRIVAUTH_PASSWORD_LEN);
                usmAuthPasswdOpcode = true;
                configArgs += 3;
            }
            else if(strncmp("p",argv[configArgs],1) == 0)
            {
                if(argc == configArgs+1)
                {
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam,"SNMPv3 Privacy Hash type is missing\r\n");
                    return;
                }
                privType = (SNMPV3_PRIV_PROT_TYPE)FC_Str2UL(argv[configArgs+1], 10, NULL);
                if(argc == configArgs+2)
                {
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam,"SNMPv3 Privacy password is missing\r\n");
                    return;
                }
                (void)strncpy(privPwBuf,argv[configArgs+2],TCPIP_SNMPV3_PRIVAUTH_PASSWORD_LEN);
                usmPrivPasswdOpcode = true;
                configArgs += 3;
            }
            else
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam,"invalid number of arguments\r\n");
                return;
            }
            if(configArgs>=argc)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam,"End of arguments\r\n");
                break;
            }
        }
    }
    
    if(usmUserNameOpcode)
    {
        uint8_t userNameLen = 0;
        userNameLen = (uint8_t)strlen((char*)userNameBuf);
        result = TCPIP_SNMPV3_SetUSMUserName(userNameBuf,userNameLen,usmPos);
        if(result != SNMPV3_USM_SUCCESS)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam,(char*)SNMPV3_USM_ERROR_STR[result]);
            return;
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam,"USM user name configured successfully\r\n");
        }
    }
    if(usmSecLevelOpcode)
    {
        uint8_t userNameLen = 0;
        userNameLen = (uint8_t)strlen((char*)userNameBuf);
        result = TCPIP_SNMPV3_SetUSMSecLevel(userNameBuf,userNameLen,secLev);
        if(result != SNMPV3_USM_SUCCESS)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam,(char*)SNMPV3_USM_ERROR_STR[result]);
            return;
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam,"USM Security level configured successfully\r\n");
        }
    }
    
    if(usmAuthPasswdOpcode)
    {
        uint8_t userNameLen = 0;
        uint8_t authPwLen=0;
        userNameLen = (uint8_t)strlen((char*)userNameBuf);
        authPwLen = (uint8_t)strlen((char*)authPwBuf);
        result = TCPIP_SNMPV3_SetUSMAuth(userNameBuf,userNameLen,authPwBuf,authPwLen,hashType);
        if(result != SNMPV3_USM_SUCCESS)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam,(char*)SNMPV3_USM_ERROR_STR[result]);
            return;
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam,"USM Authentication password configured successfully\r\n");
        }
    }
    
    if(usmPrivPasswdOpcode)
    {
        uint8_t userNameLen = 0;
        uint8_t privPwLen=0;
        userNameLen = (uint8_t)strlen((char*)userNameBuf);
        privPwLen = (uint8_t)strlen((char*)privPwBuf);
        result = TCPIP_SNMPV3_SetUSMPrivacy(userNameBuf,userNameLen,privPwBuf,privPwLen,privType);
        if(result != SNMPV3_USM_SUCCESS)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam,(char*)SNMPV3_USM_ERROR_STR[result]);
            return;
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam,"USM Privacy password configured successfully\r\n");
        }
    }
    if(usmAuthPasswdOpcode || usmPrivPasswdOpcode)
    {
        (void)TCPIP_SNMPV3_USMAuthPrivLocalization(usmPos);
    }
    if(usmUserInfo)
    {
        uint8_t usmUserLen=0;
        uint8_t usmUserAuthLen=0;
        uint8_t usmUserPrivLen=0;
        STD_BASED_SNMPV3_SECURITY_LEVEL securityLevel=NO_AUTH_NO_PRIV;
        uint8_t i=0;
        
        (*pCmdIO->pCmdApi->msg)(cmdIoParam,"SNMPv3 USM CONFIGURATION DETAILS\r\n");
        
        for(i=0;i<(uint8_t)TCPIP_SNMPV3_USM_MAX_USER;i++)
        {
            (void)memset(userNameBuf,0,sizeof(userNameBuf));
            result = TCPIP_SNMPV3_GetUSMUserName(userNameBuf,&usmUserLen,i);
            if(result != SNMPV3_USM_SUCCESS)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam,(char*)SNMPV3_USM_ERROR_STR[result]);
            }
             
            (void)memset(authPwBuf,0,sizeof(authPwBuf));
            result = TCPIP_SNMPV3_GetUSMAuth(userNameBuf,usmUserLen,authPwBuf,&usmUserAuthLen,&hashType);
            if(result != SNMPV3_USM_SUCCESS)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam,(char*)SNMPV3_USM_ERROR_STR[result]);
            }

            (void)memset(privPwBuf,0,sizeof(privPwBuf));
            result = TCPIP_SNMPV3_GetUSMPrivacy(userNameBuf,usmUserLen,privPwBuf,&usmUserPrivLen,&privType);
            if(result != SNMPV3_USM_SUCCESS)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam,(char*)SNMPV3_USM_ERROR_STR[result]);
            }

            result = TCPIP_SNMPV3_GetUSMSecLevel(userNameBuf,usmUserLen,&securityLevel);
            if(result != SNMPV3_USM_SUCCESS)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam,(char*)SNMPV3_USM_ERROR_STR[result]);
            }
            
            (*pCmdIO->pCmdApi->print)(cmdIoParam,"index:%d  username: %s  secLevel: %d authType: %d authpw: %s  privType: %d privpw: %s \r\n", i,userNameBuf, securityLevel,hashType,authPwBuf,privType,privPwBuf );
        }
    }
    
}
#endif

#if defined(TCPIP_STACK_USE_SNTP_CLIENT)  
static void F_Command_Sntp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    uint32_t utcSeconds = TCPIP_SNTP_UTCSecondsGet();
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "UTCSecondsGet: %d\r\n", utcSeconds);

    uint32_t utcMs; 
    TCPIP_SNTP_RESULT res = TCPIP_SNTP_TimeGet(&utcSeconds, &utcMs);
    if(res == SNTP_RES_OK)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "TimeGet - sec: %d, ms: %d\r\n", utcSeconds, utcMs);
    } 
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "TimeGet failed: %d\r\n", res);
    }

    TCPIP_SNTP_TIME_STAMP tStamp;
    uint32_t lastUpdate;
    res = TCPIP_SNTP_TimeStampGet(&tStamp, &lastUpdate);

    if(res == SNTP_RES_OK)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "TimeStamp - sec: %d, fract: %d\r\n", tStamp.tStampSeconds, tStamp.tStampFraction);
    } 
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "TimeStamp failed: %d\r\n", res);
    }
    
}
#endif  // defined(TCPIP_STACK_USE_SNTP_CLIENT)  

#if defined(M_TCPIP_COMMANDS_WSC)  
typedef struct
{
    const char* cmdName;    // name of the WSC command
    SYS_CMD_FNC cmdFnc;     // command function
    const char* cmdHelp;    // brief command explanation
}WSC_COMMAND_DCPT;

// list of supported WSC commands
static const WSC_COMMAND_DCPT wscCmdTbl[] = 
{
    {"help",    &F_Command_WsHelp,       "WSC sub-command help"},
    {"set",     &F_Command_WsSet,        "sets the server parameters"},
    {"preset",  &F_Command_WsPreset,     "uses a preset parameter set"},
    {"rate",    &F_Command_WscRate,      "changes the WSC task rate, ms"},
    {"open",    &F_Command_WsOpen,       "opens a connection"},
    {"close",   &F_Command_WsClose,      "closes a connection"},
    {"evreg",   &F_Command_WsRegister,   "registers an event handler"},
    {"stat",    &F_Command_WsStat,       "current connection status"},
    {"ping",    &F_Command_WsPing,       "pings the server"},
    {"pong",    &F_Command_WsPong,       "pongs the server"},
    {"msg",     &F_Command_WsMsg,        "message to the server"},
    {"rxinfo",  &F_Command_WsRxInfo,     "current RX info"},
    {"rxsize",  &F_Command_WsRxSize,     "current RX buffer size"},
    {"read",    &F_Command_WsRead,       "read from the connection"},
    {"fread",   &F_Command_WsForceRead,  "forced read from the connection"},
    {"auto-read", &F_Command_WsAutoRead,   "sets the auto-read option"},
};

// current WS server we want to connect to.
// Used for preset servers:
typedef struct
{
    const char* server;
    const char* resource;
    const char* proto;
    TCPIP_WSC_AUTH_HANDLER  authHandler;
    uint16_t    port;
    uint16_t    flags;
}WSC_TEST_PRESET;

// list of preset servers
static const WSC_TEST_PRESET wsc_presets[] = 
{
// { server, resource, proto, port}
    {"ws.ifelse.io", 0, 0, NULL, 80, (uint16_t)TCPIP_WSC_CONN_FLAG_SECURE_OFF},
    {"ws.ifelse.io", 0, 0, NULL, 443, (uint16_t)TCPIP_WSC_CONN_FLAG_SECURE_DEFAULT},
    {"echo.websocket.org", 0, 0, NULL, 443, (uint16_t)TCPIP_WSC_CONN_FLAG_SECURE_DEFAULT},
    {"497877863b54bfd9.octt.openchargealliance.org", "Mchp", "ocpp1.6", NULL, 16968, (uint16_t)TCPIP_WSC_CONN_FLAG_SECURE_ON},
};

// message to be sent for a connection close
static TCPIP_WSC_CLOSE_CODE ws_close_code = TCPIP_WSC_CLOSE_GOING_AWAY;
static const char* ws_close_msg = "The user has closed the connection!";    

// messages used for pinging the server
static const char* ws_ping_tbl[] =
{
    "Hello There, everybody!",
    "This is a slightly longer message, just for testing the functionality",
    "This is the longest message for a ping to the server. Test that it goes through without problems. Makes sure everything is OK",
};

// messages for sending to the server
static const char* ws_msg_tbl[] =
{
    "Hello There, everybody!",
    "Slightly longer message, just for the fun/test of it",
    "This is a really long message to test it it goes through without problems. Needs to be tested\
        Just to make sure everything is OK",
    "Long message, i.e. beyond 125 characters. Something like >= 250 chars would do. Testing this too\
        but from multiple write operations, possibly. Give it a try, see if it is long enough for this test!\
        Well, let's add some more lines, just to be sure...\
        And this is the last line!",
    " Now a really long message combination of multiple lines. For a really long message it'd have to send it in multiple chunks!\r\n\
        Line number 1 of the long message\r\n\
        Line number 2 of the long message\r\n\
        Line number 3 of the long message\r\n\
        Line number 4 of the long message\r\n\
        Line number 5 of the long message\r\n\
        Line number 6 of the long message\r\n\
        Line number 7 of the long message\r\n\
        Line number 8 of the long message\r\n\
        Line number 9 of the long message\r\n\
        Line number 10 of the long message\r\n\
        Line number 11 of the long message\r\n\
        Line number 12 of the long message\r\n\
        Line number 13 of the long message\r\n\
        Line number 14 of the long message\r\n\
        Line number 15 of the long message\r\n\
        Line number 16 of the long message\r\n\
        Line number 17 of the long message\r\n\
        Line number 18 of the long message\r\n\
        Line number 19 of the long message\r\n\
        Last line (number 20) of the long message\r\n",
};


// dynamic data for the WSC commands

static int wscTaskInited = 0;   // 0 - needs to be initialized
                                // 1 - initialized OK
                                // < 0 - initialization failed

#define WSC_CMD_TASK_RATE_MIN       5   // minimum value 5 ms
#define WSC_CMD_TASK_RATE_DEFAULT   100 // default value 100 ms

static uint32_t wscCmdTaskRate;     // WSC task rate

static bool     wscDisAutoRead = false; // enable/disable WSC task automatically read of the pending messages

static TCPIP_WSC_CONN_HANDLE wscConnHandle = NULL;      // current WSC connection handle


// current server to connect to
static char wsc_server[64 + 1] = "";
// current resource on the server
static char wsc_resource[64 + 1] = "";
// current prototype to request
static char wsc_proto[16 + 1] = "";
// current authentication  handler
static TCPIP_WSC_AUTH_HANDLER wsc_authHandler = NULL; 
// current port to connect to
static uint16_t wsc_port = 80U;
// if proto usage is enforced
static int32_t wsc_proto_enforced = 1;
// current base settings for the connection flags
static uint16_t wsc_flags = (uint16_t)TCPIP_WSC_CONN_FLAG_NONE;

// current event handle
static TCPIP_WSC_EV_HANDLE wsc_EvHandle = NULL;
static TCPIP_WSC_CONN_HANDLE wsc_EvConnHandle = NULL;   // the connection handle used for the event registration
                                                        // copy of the wscConnHandle

// current pending message to TX
static TCPIP_WSC_SEND_MSG_DCPT wsc_TxMsgDcpt;
static TCPIP_WSC_SEND_MSG_DCPT* pWsc_TxMsgDcpt = NULL;

// buffer for reading messages from the server
static union
{
    uint8_t uBuffer[512];
    char    cBuffer[513];
}U_WSC_RD_BUFF;

// current read message handle
static const void* wsc_RxMsgHandle = NULL;
static size_t wsc_bReadSize = 100;      // size of the buffer to use for a read operation

static void Wsc_CmdInitialize(void)
{
    wscCmdTaskRate = WSC_CMD_TASK_RATE_DEFAULT;

    wsc_server[sizeof(wsc_server) - 1U] = '\0';
    wsc_resource[sizeof(wsc_resource) - 1U] = '\0';
    wsc_proto[sizeof(wsc_proto) - 1U] = '\0';

    SYS_MODULE_OBJ presObj = NET_PRES_ModuleObjGet(0);
    NET_PRES_CBACK_HANDLE sniCback = NET_PRES_SniCallbackRegister(presObj, &Wsc_SNIHostName); 
    if(sniCback == NULL)
    {
        SYS_CONSOLE_MESSAGE("WSC: Failed to register SNI callback!\r\n");
        wscTaskInited = -1;
        return;
    }

    wscTaskInited = 1;
    SYS_CONSOLE_MESSAGE("WSC: Initialized WSC Task!\r\n");
}


// WSC Task function
static void Wsc_CmdTask(void)
{
    if(wscTaskInited > 0)
    {
        // check for pending TX tasks
        if(pWsc_TxMsgDcpt != NULL)
        {
            TCPIP_WSC_RES res = Wsc_SendMsg(pWsc_TxMsgDcpt);
            if((int)res <= 0)
            {   // done one way or another
                pWsc_TxMsgDcpt = NULL;
            }
            // else keep sending
        }

        // check for pending RX tasks
        if(!wscDisAutoRead && wsc_RxMsgHandle != NULL)
        {
            TCPIP_WSC_RES res = Wsc_ReadMsg(wsc_RxMsgHandle);
            if((int)res <= 0)
            {   // done one way or another
                wsc_RxMsgHandle = NULL;
            }
            // else keep reading
        }
    }
}

// function for registering the client SNI host name with wolfSSL
static const char* Wsc_SNIHostName(NET_PRES_SKT_HANDLE_T handle)
{
    return wsc_server;
}

// WSC command functions
static void F_CommandWsc(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(wscTaskInited <= 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc commands not initialized. Restart!\r\n");
    }

    size_t ix;
    const WSC_COMMAND_DCPT* pCDcpt;

    if(argc < 2)
    {
        F_Command_WsHelp(pCmdIO, argc, argv);
        return;
    }

    // execute the command
    pCDcpt = wscCmdTbl; 
    for(ix = 0; ix < sizeof(wscCmdTbl) / sizeof(*wscCmdTbl); ix++)
    {
        if(strcmp(argv[1], pCDcpt->cmdName) == 0)
        {   // found it
            pCDcpt->cmdFnc(pCmdIO, argc, argv);
            return;
        }
        pCDcpt++;
    }
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc - unknown command: %s\r\n", argv[1]);

}

static void F_Command_WsHelp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    const WSC_COMMAND_DCPT* pCDcpt = wscCmdTbl; 
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc supported sub-commands: \r\n");
    for(ix = 0; ix < sizeof(wscCmdTbl) / sizeof(*wscCmdTbl); ix++)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t'wsc %s - %s'\r\n", pCDcpt->cmdName, pCDcpt->cmdHelp);
        pCDcpt++;
    }
}

// updates the settings for a connection
static void F_Command_WsSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc set
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    bool printUsage = false;
    bool printSettings = true;

    while(true)
    {
        if(argc == 2)
        {
            printUsage = true;
            break;
        }


        int argIx = 2;
        argc -= 2;

        if(((unsigned int)argc & 0x1U) != 0U)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc set : an even number of arguments is required. Retry!\r\n");
            printUsage = true;
            printSettings = false;
            break;
        }

        // set params in turn
        while(argc != 0)
        {
            char* kword = argv[argIx];
            char* param = argv[argIx + 1];

            if(strcmp(kword, "server") == 0)
            {
                if(strcmp(param, "none") == 0)
                {
                    wsc_server[0] = '\0';
                }
                else
                {
                    (void)strncpy(wsc_server, param, sizeof(wsc_server) - 1U);
                }
            }
            else if(strcmp(kword, "resource") == 0)
            {
                if(strcmp(param, "none") == 0)
                {
                    wsc_resource[0] = '\0';
                }
                else
                {
                    (void)strncpy(wsc_resource, param, sizeof(wsc_resource) - 1U);
                }
            }
            else if(strcmp(kword, "proto") == 0)
            {
                if(strcmp(param, "none") == 0)
                {
                    wsc_proto[0] = '\0';
                }
                else
                {
                    (void)strncpy(wsc_proto, param, sizeof(wsc_proto) - 1U);
                }
            }
            else if(strcmp(kword, "port") == 0)
            {
                uint32_t port32;
                (void)FC_Str2UL(param, 10, &port32);
                wsc_port = (uint16_t)port32;
            }
            else if(strcmp(kword, "proto_enforced") == 0)
            {
                (void)FC_Str2L(param, 10, &wsc_proto_enforced);
            }
            else
            {
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc set - unknown setting: %sRetry!\r\n", kword);
                break;
            }
            argIx += 2;
            argc -= 2;
        }

        break;
    }

    if(printUsage)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "usage: 'wsc set  <server name/none> <resource path/none> <proto name/none> <port n> <proto_enforced 0/1>'\r\n");
    }

    if(printSettings)
    {
        Wsc_PrintSettings(pCmdIO, argv);
    }


}

// updates the settings for a connection
static void F_Command_WsPreset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc preset index
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc < 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc preset - provide a preset index!\r\n");
        return;
    }

    uint32_t presIx; 
    (void)FC_Str2UL(argv[2], 10, &presIx);
    if(presIx >= sizeof(wsc_presets) / sizeof(*wsc_presets))
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc preset - wrong preset index! Maxim '%d'\r\n", sizeof(wsc_presets) / sizeof(*wsc_presets) - 1U);
        return;
    }

    // update the settings
    const WSC_TEST_PRESET* preset = wsc_presets + presIx;

    if(preset->server != NULL)
    {
        (void)strncpy(wsc_server, preset->server, sizeof(wsc_server) - 1U);
    }
    else
    {
        wsc_server[0] = '\0';
    }

    if(preset->resource != NULL)
    {
        (void)strncpy(wsc_resource, preset->resource, sizeof(wsc_resource) - 1U);
    }
    else
    {
        wsc_resource[0] = '\0';
    }

    if(preset->proto != NULL)
    {
        (void)strncpy(wsc_proto, preset->proto, sizeof(wsc_proto) - 1U);
    }
    else
    {
        wsc_proto[0] = '\0';
    }

    wsc_authHandler = preset->authHandler;
    wsc_port = preset->port;
    wsc_flags = preset->flags;

    Wsc_PrintSettings(pCmdIO, argv);
}

static void Wsc_PrintSettings(SYS_CMD_DEVICE_NODE* pCmdIO, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc %s - settings: \r\n", argv[1]);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tserver: '%s'\r\n", wsc_server[0] == '\0' ? "none" : wsc_server);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tresource: '%s'\r\n", wsc_resource[0] == '\0' ? "none" : wsc_resource);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tproto: '%s'\r\n", wsc_proto[0] == '\0' ? "none" : wsc_proto);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tauth: '0x%08x'\r\n", wsc_authHandler);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tport: %d\r\n", wsc_port);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tflags: 0x%02x\r\n", wsc_flags);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\tproto_enforced: '%d'\r\n", wsc_proto_enforced);
} 

// change the rate of the WSC task
static void F_Command_WscRate(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc rate ms
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if(argc > 2)
    {
        uint32_t rateMs;
        (void)FC_Str2UL(argv[2], 10, &rateMs);
        if(rateMs < (uint32_t)WSC_CMD_TASK_RATE_MIN) 
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc rate - bad value: %d. Minimum: %d\r\n", rateMs, WSC_CMD_TASK_RATE_MIN);
            return;
        }
        wscCmdTaskRate = rateMs;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc Task rate: %d ms\r\n", wscCmdTaskRate);
}


static void F_Command_WsOpen(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc open 
    const char* protoTbl[] = {wsc_proto};
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(tcpipCmdStat != TCPIP_CMD_STAT_IDLE || wscConnHandle != NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc open - Another operation ongoing. Retry!\r\n");
        return;
    }

    if(wsc_server[0] == '\0')
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc open - no server set. Retry!\r\n");
        return;
    }

    Wsc_PrintSettings(pCmdIO, argv);

    const char** openProtos;
    uint16_t nProto;
    if(wsc_proto[1] == '\0')
    {
        openProtos = NULL;
        nProto = 0;
    }
    else
    {
        openProtos = protoTbl;
        nProto = 1;
    }

    TCPIP_WSC_RES res;
    TCPIP_WSC_CONN_DCPT connDcpt; 
    connDcpt.server = wsc_server;
    connDcpt.resource = wsc_resource;
    connDcpt.port = wsc_port;
    connDcpt.connFlags = wsc_flags | ((wsc_proto_enforced != 0) ? (uint16_t)TCPIP_WSC_CONN_FLAG_PROTO_ENFORCED : (uint16_t)TCPIP_WSC_CONN_FLAG_PROTO_OPTIONAL);
    connDcpt.protocols = openProtos;
    connDcpt.nProtocols = nProto;
    connDcpt.extensions = NULL;
    connDcpt.authHandler = wsc_authHandler;

    wscConnHandle = TCPIP_WSC_ConnOpen(&connDcpt, &res);
    
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc open - wscConnHandle: 0x%08x, res: %d\r\n", wscConnHandle, res);
    if(wscConnHandle != NULL)
    {   // success
        tcpipCmdStat = TCPIP_CMD_STAT_WSC_OPEN;
        pTcpipCmdDevice = pCmdIO;
        (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, (int16_t)wscCmdTaskRate);
    }
}

static void F_Command_WsClose(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc close
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if(wscConnHandle == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc close - wscConnHandle is NULL\r\n");
    }

    TCPIP_WSC_RES res = TCPIP_WSC_ConnClose(wscConnHandle, ws_close_code, ws_close_msg);
    Wsc_CloseConn();
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc close - res: %d\r\n", res);
}

static void F_Command_WsRegister(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc evreg 0/1 - deregister, register
    
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    const char* regMsg;

    while(argc > 2)
    {
        uint32_t regVal = 0;
        (void)FC_Str2UL(argv[2], 10, &regVal);
        bool doReg = regVal != 0U;

        if(doReg && wsc_EvHandle != NULL)
        {   // nothing to do
            break;
        }
        else if(doReg == false && wsc_EvHandle == NULL)
        {   // nothing to do
            break;
        }
        else
        {
            // OK
        }
        bool res;
        if(doReg)
        {
            wsc_EvConnHandle = wscConnHandle;
            wsc_EvHandle = TCPIP_WSC_HandlerRegister(wsc_EvConnHandle, &Wsc_EventHandler, NULL);
            res = wsc_EvHandle != NULL;
            regMsg = " ";
        }
        else
        {
            res = TCPIP_WSC_HandlerDeRegister(wsc_EvConnHandle, wsc_EvHandle);
            wsc_EvHandle = NULL;
            regMsg = " de-";
        }

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc evreg -%sregister result: %d\r\n", regMsg, res);
        break;
    }


    // show status
    if(wsc_EvHandle != NULL)
    {
        regMsg = " ";
    }
    else
    {
        regMsg = " de-";
    }
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc evreg -%sregistered (wsc_EvConnHandle = 0x%08x) - evHandle: 0x%0x\r\n", regMsg, wsc_EvConnHandle, wsc_EvHandle);
}

// connection status
static void F_Command_WsStat(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc stat
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if(wscConnHandle == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc stat - wscConnHandle NULL\r\n");
        return;
    }
    TCPIP_WSC_CONN_STATUS connStat = TCPIP_WSC_ConnStatus(wscConnHandle);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc stat - conn stat is: %d\r\n", connStat);
}


// ping the server
static void F_Command_WsPing(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc ping <ix>
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint32_t pingIx = 0;
    bool msgSel = false;
    if(argc > 2)
    {
        (void)FC_Str2UL(argv[2], 10, &pingIx);
        if(pingIx >= sizeof(ws_ping_tbl) / sizeof(*ws_ping_tbl))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc ping - bad ping ix: %d\r\n", pingIx);
            return;
        }
        msgSel = true;
    }

    const char* ws_ping_data = msgSel ? ws_ping_tbl[pingIx] : NULL;
    Wsc_SendCtrlFrame(pCmdIO, argv, ws_ping_data, TCPIP_WS_CTRL_CODE_PING);
}

static void F_Command_WsPong(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc pong <ix>
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint32_t pongIx = 0;
    bool msgSel = false;
    if(argc > 2)
    {
        (void)FC_Str2UL(argv[2], 10, &pongIx);
        if(pongIx >= sizeof(ws_ping_tbl) / sizeof(*ws_ping_tbl))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc pong - bad pong ix: %d\r\n", pongIx);
            return;
        }
        msgSel = true;
    }

    const char* ws_pong_data = msgSel ? ws_ping_tbl[pongIx] : NULL;
    Wsc_SendCtrlFrame(pCmdIO, argv, ws_pong_data, TCPIP_WS_CTRL_CODE_PONG);
}

// sends a message to the server
static void F_Command_WsMsg(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc msg <ix>
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if(pWsc_TxMsgDcpt != NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc msg - another message ongoing. Retry later!\r\n");
        return;
    }

    // brand new message to send
    if(wscConnHandle == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc msg - wscConnHandle NULL\r\n");
        return;
    }
    TCPIP_WSC_CONN_STATUS connStat = TCPIP_WSC_ConnStatus(wscConnHandle);
    if(connStat != TCPIP_WSC_CONN_STAT_OPEN)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc msg - stat not open: %d. Retry!\r\n", connStat);
        return;
    }

    uint32_t msgIx = 0;
    if(argc > 2)
    {
        (void)FC_Str2UL(argv[2], 10, &msgIx);
        if(msgIx >= sizeof(ws_msg_tbl) / sizeof(*ws_msg_tbl))
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc msg - bad msg ix: %d\r\n", msgIx);
            return;
        }
    }

    wsc_TxMsgDcpt.msgData = (const uint8_t*)ws_msg_tbl[msgIx]; 
    wsc_TxMsgDcpt.msgSize = strlen(ws_msg_tbl[msgIx]);
    wsc_TxMsgDcpt.msgFlags = (uint8_t)TCPIP_WSC_MSG_FLAG_TEXT; 
    wsc_TxMsgDcpt.msgId = 0; 

    pWsc_TxMsgDcpt = &wsc_TxMsgDcpt;
}

static void F_Command_WsRxInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc rxinfo
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(wscConnHandle == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc rxinfo - wscConnHandle NULL\r\n");
        return;
    }
    TCPIP_WSC_PEND_MSG_DCPT msgDcpt, *pDcpt;
    msgDcpt.msgHandle = NULL;
    pDcpt = &msgDcpt;
    TCPIP_WSC_RES res = TCPIP_WSC_MessageInfo(wscConnHandle, wsc_RxMsgHandle, pDcpt);

    pCmdIO->pCmdApi->print(cmdIoParam, "wsc rxinfo (wsc_RxMsgHandle == 0x%08x) - handle: 0x%08x, res = %d\r\n", wsc_RxMsgHandle, pDcpt->msgHandle, res);
    if(res == TCPIP_WSC_RES_OK)
    {
        pCmdIO->pCmdApi->print(cmdIoParam, "\tmsg flags: 0x%02x, opcode: 0x%02x, ftype: %d, flen: %d\r\n", pDcpt->info.flags, pDcpt->info.opCode, pDcpt->info.frameType, pDcpt->info.frameLen);
        pCmdIO->pCmdApi->print(cmdIoParam, "\tmsg payLen: %d, rendLen: %d, skt pendLen: %d, srv avlbl: %d\r\n", pDcpt->payloadLen, pDcpt->renderedLen, pDcpt->sktPendLen, pDcpt->srvAvlblLen);
        if(wsc_RxMsgHandle == NULL)
        {
            wsc_RxMsgHandle = pDcpt->msgHandle;
            pCmdIO->pCmdApi->print(cmdIoParam, "wsc rxinfo set wsc_RxMsgHandle to: 0x%08x\r\n", wsc_RxMsgHandle);
        }
    }
}

static void F_Command_WsRxSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc rxsize <bSize>  // default size is 100 bytes
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(wsc_RxMsgHandle != NULL)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc rxsize - Another read message ongoing: 0x%08x. Retry later!\r\n", wsc_RxMsgHandle);
        return;
    }

    if(argc > 2)
    {
        uint32_t bSize;
        (void)FC_Str2UL(argv[2], 10, &bSize);
        if(bSize == 0U || bSize > sizeof(U_WSC_RD_BUFF.uBuffer))
        {
            pCmdIO->pCmdApi->print(cmdIoParam, "wsc rxsize - invalid bSize: %d\r\n", bSize);
            return;
        }
        wsc_bReadSize = bSize;
    }

    pCmdIO->pCmdApi->print(cmdIoParam, "wsc rxsize - read buffer size: %d\r\n", wsc_bReadSize);
}

static void F_Command_WsRead(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc read
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(wscConnHandle == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc read - wscConnHandle NULL\r\n");
        return;
    }

    if(wsc_RxMsgHandle == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc read - No pending read message. Use 'fread'!\r\n");
        return;
    }

    TCPIP_WSC_RES res = Wsc_ReadMsg(wsc_RxMsgHandle);
    if((int)res <= 0)
    {   // done one way or another
        wsc_RxMsgHandle = NULL;
    }
    else if(res == TCPIP_WSC_RES_MORE_DATA)
    {
        pCmdIO->pCmdApi->msg(cmdIoParam, "wsc read: more data pending. Call again!\r\n");
    }
    else
    {
        // nothing to do
    }
}

static void F_Command_WsForceRead(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc fread
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(wscConnHandle == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "wsc fread - wscConnHandle NULL\r\n");
        return;
    }

    // read using a NULL handle
    TCPIP_WSC_RES res = Wsc_ReadMsg(NULL);

    if(res == TCPIP_WSC_RES_MORE_DATA)
    {
        pCmdIO->pCmdApi->msg(cmdIoParam, "wsc fread: more data pending. Call again!\r\n");
    }
}

static void F_Command_WsAutoRead(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // wsc auto-read 0/1
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc > 2)
    {
        uint32_t autoVal = 0;
        (void)FC_Str2UL(argv[2], 10, &autoVal);
        wscDisAutoRead = autoVal == 0U;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc auto-read is: %d\r\n", !wscDisAutoRead);
}

// sends a control message to the server
static void Wsc_SendCtrlFrame(SYS_CMD_DEVICE_NODE* pCmdIO, char** argv, const char* message, TCPIP_WS_OP_CODE opCode)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    if(wscConnHandle == NULL)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc %s - wscConnHandle NULL\r\n", argv[1]);
        return;
    }
    TCPIP_WSC_CONN_STATUS connStat = TCPIP_WSC_ConnStatus(wscConnHandle);
    if(connStat != TCPIP_WSC_CONN_STAT_OPEN)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc %s - stat not open: %d. Retry!\r\n", argv[1], connStat);
        return;
    }

    size_t dataSize = (message == NULL) ? 0U : strlen(message);

    TCPIP_WSC_RES res;

    size_t (*WSC_CtrlFnc)(TCPIP_WSC_CONN_HANDLE hConn, const uint8_t* dataBuff, size_t dataSize, TCPIP_WSC_RES* pRes);

    if(opCode == TCPIP_WS_CTRL_CODE_PING)
    {
        WSC_CtrlFnc = &TCPIP_WSC_ConnPing;
    } 
    else if(opCode == TCPIP_WS_CTRL_CODE_PONG)
    {
        WSC_CtrlFnc = &TCPIP_WSC_ConnPong;
    } 
    else
    {   // if using the function for other control codes
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc ctrl message - unknown code: %d\r\n", opCode);
        return;
    }

    size_t sentSize = WSC_CtrlFnc(wscConnHandle, (const uint8_t*)message, dataSize, &res);

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "wsc %s - sent: %d, expected: %d, res: %d\r\n", argv[1], sentSize, dataSize, res);

}

// returns the result of the operation
static TCPIP_WSC_RES Wsc_SendMsg(TCPIP_WSC_SEND_MSG_DCPT* txDcpt)
{
    const void* cmdIoParam = pTcpipCmdDevice->cmdIoParam;
    // send the message
    TCPIP_WSC_RES res;

    size_t sentSize = TCPIP_WSC_MessageSend(wscConnHandle, txDcpt, &res);
    if(sentSize != 0U)
    {
        (*pTcpipCmdDevice->pCmdApi->print)(cmdIoParam, "wsc msg - sent: %d, expected: %d, res: %d\r\n", sentSize, txDcpt->msgSize, res);
    }

    if((int)res < 0)
    {
        (*pTcpipCmdDevice->pCmdApi->print)(cmdIoParam, "wsc msg - failed with res: %d. Aborted!\r\n", res);
    }
    else if(res == TCPIP_WSC_RES_OK)
    {
        (*pTcpipCmdDevice->pCmdApi->msg)(cmdIoParam, "wsc msg - Completed OK\r\n");
    }
    else
    {
        // continue sending data
    }
    return res;

}

static TCPIP_WSC_RES Wsc_ReadMsg(const void* rxHandle)
{
    TCPIP_WSC_RES res;
    const void* cmdIoParam = pTcpipCmdDevice->cmdIoParam;

    size_t readSize = TCPIP_WSC_MessageRead(wscConnHandle, rxHandle, U_WSC_RD_BUFF.uBuffer, wsc_bReadSize, &res);
    if(readSize != 0U)
    {
        pTcpipCmdDevice->pCmdApi->print(cmdIoParam, "wsc read - readSize: %d, res: %d\r\n", readSize, res);
    }

    if((int)res < 0)
    {
        (*pTcpipCmdDevice->pCmdApi->print)(cmdIoParam, "wsc read - failed with res: %d. Aborted!\r\n", res);
    }
    else
    {
        // check if smth was read
        if(readSize != 0U)
        {   // display 100 chars...
            U_WSC_RD_BUFF.cBuffer[readSize] = '\0';
            U_WSC_RD_BUFF.cBuffer[100] = '\0';   // limit to 100 chars
            pTcpipCmdDevice->pCmdApi->print(cmdIoParam, "wsc read message dump: %s\r\n", U_WSC_RD_BUFF.cBuffer);
        }

        if(res == TCPIP_WSC_RES_OK)
        {   // done
            (*pTcpipCmdDevice->pCmdApi->msg)(cmdIoParam, "wsc read - Completed OK\r\n");
        }
        // else continue reading data
    }

    return res;
}
// WSC event handler
static void Wsc_EventHandler(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_EVENT_TYPE evType, TCPIP_WSC_EV_INFO evInfo, const void* param)
{
    const TCPIP_WSC_OPEN_INFO* openInfo;
    const TCPIP_WSC_PEND_MSG_DCPT* msgDcpt;
    const TCPIP_WSC_ERR_MSG_DCPT* errDcpt;
    int netIx;
    const char* evMsg;
    char addBuff[44];

    bool  closeConn = false;
    switch(evType)
    {
        case WSC_EVENT_RAW_ESTABLISHED:
        case WSC_EVENT_RAW_RX_DATA:
        case WSC_EVENT_RAW_RX_FIN:
        case WSC_EVENT_RAW_RX_RST:
        case WSC_EVENT_RAW_UNKNOWN:
            if(evType == WSC_EVENT_RAW_ESTABLISHED)
            {
                evMsg = "Established";
            }
            else if(evType == WSC_EVENT_RAW_RX_DATA)
            {
                evMsg = "RX data";
            }
            else if(evType == WSC_EVENT_RAW_RX_FIN)
            {
                evMsg = "FIN";
                closeConn = true;
            }
            else if(evType == WSC_EVENT_RAW_RX_RST)
            {
                evMsg = "RST";
                closeConn = true;
            }
            else
            {
                evMsg = "unknown";
            }
            SYS_CONSOLE_PRINT(" WSC Event RAW - %s\r\n", evMsg);
            break;
        
        case WSC_EVENT_OPEN:
            openInfo = evInfo.openInfo;

            addBuff[0] = '\0';
            if(openInfo->ipType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
            {
                (void)TCPIP_Helper_IPAddressToString(&openInfo->srvAddress.v4Add, addBuff, sizeof(addBuff));
            }
            else
            {
                (void)TCPIP_Helper_IPv6AddressToString(&openInfo->srvAddress.v6Add, addBuff, sizeof(addBuff));
            }
            netIx = TCPIP_STACK_NetIndexGet(openInfo->hNet);
            SYS_CONSOLE_PRINT(" WSC Event - open: address: %s, port: %d, network: %d\r\n", addBuff, openInfo->srvPort, netIx);
            break;
            
        case WSC_EVENT_MSG_READY:
            msgDcpt = evInfo.pendMsgDcpt;
            SYS_CONSOLE_PRINT(" WSC Event - msg rdy: ftype: %d, flen: %d, payLen: %d, skt pendLen: %d, srv avlbl: %d, handle: 0x%08x\r\n", msgDcpt->info.frameType, msgDcpt->info.frameLen, msgDcpt->payloadLen, msgDcpt->sktPendLen, msgDcpt->srvAvlblLen, msgDcpt->msgHandle);
            wsc_RxMsgHandle = msgDcpt->msgHandle;
            break;

        case WSC_EVENT_CTRL_CLOSE:
        case WSC_EVENT_CTRL_PING:
        case WSC_EVENT_CTRL_PONG:
        case WSC_EVENT_CTRL_UNKNOWN:
            if(evType == WSC_EVENT_CTRL_CLOSE)
            {
                evMsg = "close";
                closeConn = true;
            }
            else if(evType == WSC_EVENT_CTRL_PING)
            {
                evMsg = "ping";
            }
            else if(evType == WSC_EVENT_CTRL_PONG)
            {
                evMsg = "pong";
            }
            else
            {
                evMsg = "unknown";
            }

            SYS_CONSOLE_PRINT(" WSC Event - ctrl: %s, message: %s\r\n", evMsg, evInfo.evCtrlMsg);
            break;

        case WSC_EVENT_CONN_CLOSE:
            SYS_CONSOLE_MESSAGE(" WSC Event - conn closed!\r\n");
            closeConn = true;
            break;

        case WSC_EVENT_MSG_ERROR:
        case WSC_EVENT_MSG_DISCARD_USR_TMO:
        case WSC_EVENT_MSG_DISCARD_CLOSE:
            errDcpt = evInfo.errMsgDcpt;
            if(evType == WSC_EVENT_MSG_ERROR)
            {
                evMsg = "err";
            }
            else if(evType == WSC_EVENT_MSG_DISCARD_USR_TMO)
            {
                evMsg = "discard tmo";
            }
            else
            {
                evMsg = "discard close";
            }
            SYS_CONSOLE_PRINT(" WSC Event - msg %s: ftype: %d, flen: %d, payLen: %llu\r\n", evMsg, errDcpt->info.frameType, errDcpt->info.frameLen, errDcpt->payloadLen);
            break;

        case WSC_EVENT_FAIL_CONN:
            SYS_CONSOLE_PRINT(" WSC Event - fail conn: failRes: %d\r\n", evInfo.failInfo->failRes);
            closeConn = true;
            break;

        default:
            SYS_CONSOLE_PRINT(" WSC Event unk - %d\r\n", evType);
            break;
    }

    if(closeConn && wscConnHandle != NULL)
    {
        Wsc_CloseConn();
    }

}

static void Wsc_CloseConn(void)
{
    wscConnHandle = NULL;
    pWsc_TxMsgDcpt = NULL;
    wsc_RxMsgHandle = NULL;
    tcpipCmdStat = TCPIP_CMD_STAT_IDLE;
    (void)TCPIPStackSignalHandlerSetParams(TCPIP_THIS_MODULE_ID, tcpipCmdSignalHandle, 0);
}

#endif // defined(M_TCPIP_COMMANDS_WSC)  

#endif // defined(TCPIP_STACK_COMMAND_ENABLE)


