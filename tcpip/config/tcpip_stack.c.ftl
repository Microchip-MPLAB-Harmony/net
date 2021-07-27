<#----------------------------------------------------------------------------
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

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
----------------------------------------------------------------------------->

// <editor-fold defaultstate="collapsed" desc="TCP/IP Stack Initialization Data">
// *****************************************************************************
// *****************************************************************************
// Section: TCPIP Data
// *****************************************************************************
// *****************************************************************************
<#--
// tcpip_stack_init.c.ftl: TCP/IP modules initialization data
-->
<#if (tcpipArp.TCPIP_USE_ARP)?has_content && (tcpipArp.TCPIP_USE_ARP) == true>
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
</#if>
<#if (tcpipTelnet.TCPIP_USE_TELNET)?has_content && (tcpipTelnet.TCPIP_USE_TELNET) == true>
/*** telnet Server Initialization Data ***/
const TCPIP_TELNET_MODULE_CONFIG tcpipTelnetInitData =
{ 
    .nConnections   = TCPIP_TELNET_MAX_CONNECTIONS,
    .sktTxBuffSize  = TCPIP_TELNET_SKT_TX_BUFF_SIZE,
    .sktRxBuffSize  = TCPIP_TELNET_SKT_RX_BUFF_SIZE,
    .listenPort     = TCPIP_TELNET_LISTEN_PORT,
    .configFlags    = TCPIP_TELNET_CONFIG_FLAGS,
};
</#if>

<#if (tcpipAnnounce.TCPIP_USE_ANNOUNCE)?has_content && (tcpipAnnounce.TCPIP_USE_ANNOUNCE) == true>
/*** Announce Discovery Initialization Data ***/
const TCPIP_ANNOUNCE_MODULE_CONFIG tcpipAnnounceInitData =
{ 
    0
};
</#if>

<#if (tcpipUdp.TCPIP_USE_UDP)?has_content && (tcpipUdp.TCPIP_USE_UDP) == true>
/*** UDP Sockets Initialization Data ***/
const TCPIP_UDP_MODULE_CONFIG tcpipUDPInitData =
{
    .nSockets       = TCPIP_UDP_MAX_SOCKETS,
    .sktTxBuffSize  = TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE, 
<#if (tcpipUdp.TCPIP_UDP_USE_POOL_BUFFERS)?has_content && (tcpipUdp.TCPIP_UDP_USE_POOL_BUFFERS) == true>
    .poolBuffers    = TCPIP_UDP_SOCKET_POOL_BUFFERS,
    .poolBufferSize = TCPIP_UDP_SOCKET_POOL_BUFFER_SIZE,
</#if>
};
</#if>

<#if (tcpipTcp.TCPIP_USE_TCP)?has_content && (tcpipTcp.TCPIP_USE_TCP) == true>
/*** TCP Sockets Initialization Data ***/
const TCPIP_TCP_MODULE_CONFIG tcpipTCPInitData =
{
    .nSockets       = TCPIP_TCP_MAX_SOCKETS,
    .sktTxBuffSize  = TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE, 
    .sktRxBuffSize  = TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE,
};
</#if>

<#if (tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER)?has_content && (tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER) == true>
/*** HTTP Server Initialization Data ***/
const TCPIP_HTTP_MODULE_CONFIG tcpipHTTPInitData =
{
    .nConnections   = TCPIP_HTTP_MAX_CONNECTIONS,
    .dataLen        = TCPIP_HTTP_MAX_DATA_LEN,
    .sktTxBuffSize  = TCPIP_HTTP_SKT_TX_BUFF_SIZE,
    .sktRxBuffSize  = TCPIP_HTTP_SKT_RX_BUFF_SIZE,
    .configFlags    = TCPIP_HTTP_CONFIG_FLAGS,
    .http_malloc_fnc    = TCPIP_HTTP_MALLOC_FUNC,
    .http_free_fnc      = TCPIP_HTTP_FREE_FUNC,
    .web_dir            = TCPIP_HTTP_WEB_DIR, 
};
</#if>

<#if (tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER)?has_content && (tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER) == true>
/*** HTTP_NET Server Initialization Data ***/
const TCPIP_HTTP_NET_MODULE_CONFIG tcpipHTTPNetInitData =
{
    .nConnections   = TCPIP_HTTP_NET_MAX_CONNECTIONS,
    .dataLen        = TCPIP_HTTP_NET_MAX_DATA_LEN,
    .sktTxBuffSize  = TCPIP_HTTP_NET_SKT_TX_BUFF_SIZE,
    .sktRxBuffSize  = TCPIP_HTTP_NET_SKT_RX_BUFF_SIZE,
    .listenPort     = TCPIP_HTTP_NET_LISTEN_PORT,
<#if (tcpipHttpNet.TCPIP_HTTP_NET_DYNVAR_PROCESS)?has_content && (tcpipHttpNet.TCPIP_HTTP_NET_DYNVAR_PROCESS) == true>
    .nDescriptors   = TCPIP_HTTP_NET_DYNVAR_DESCRIPTORS_NUMBER,
<#else>
    .nDescriptors   = 0,
</#if>
    .nChunks        = TCPIP_HTTP_NET_CHUNKS_NUMBER, 
    .maxRecurseLevel= TCPIP_HTTP_NET_MAX_RECURSE_LEVEL,    
    .configFlags    = TCPIP_HTTP_NET_CONFIG_FLAGS,
    .nFileBuffers   = TCPIP_HTTP_NET_FILE_PROCESS_BUFFERS_NUMBER,
    .fileBufferSize = TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE,
    .chunkPoolRetries = TCPIP_HTTP_NET_CHUNK_RETRIES,
    .fileBufferRetries = TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_RETRIES,
<#if (tcpipHttpNet.TCPIP_HTTP_NET_DYNVAR_PROCESS)?has_content && (tcpipHttpNet.TCPIP_HTTP_NET_DYNVAR_PROCESS) == true>
    .dynVarRetries  = TCPIP_HTTP_NET_DYNVAR_PROCESS_RETRIES,
<#else>
    .dynVarRetries      = 0,
</#if>
    .connTimeout        = TCPIP_HTTP_NET_CONNECTION_TIMEOUT,
    .http_malloc_fnc    = TCPIP_HTTP_NET_MALLOC_FUNC,
    .http_free_fnc      = TCPIP_HTTP_NET_FREE_FUNC,
    .web_dir            = TCPIP_HTTP_NET_WEB_DIR, 
};
</#if>

<#if (tcpipSntp.TCPIP_USE_SNTP_CLIENT)?has_content && (tcpipSntp.TCPIP_USE_SNTP_CLIENT) == true>
/*** SNTP Client Initialization Data ***/
const TCPIP_SNTP_MODULE_CONFIG tcpipSNTPInitData =
{
    .ntp_server             = TCPIP_NTP_SERVER,
    .ntp_interface          = TCPIP_NTP_DEFAULT_IF,
    .ntp_connection_type    = TCPIP_NTP_DEFAULT_CONNECTION_TYPE,
    .ntp_reply_timeout      = TCPIP_NTP_REPLY_TIMEOUT,
    .ntp_stamp_timeout      = TCPIP_NTP_TIME_STAMP_TMO,
    .ntp_success_interval   = TCPIP_NTP_QUERY_INTERVAL,
    .ntp_error_interval     = TCPIP_NTP_FAST_QUERY_INTERVAL,
};
</#if>

<#if (tcpipSmtp.TCPIP_USE_SMTP_CLIENT)?has_content && (tcpipSmtp.TCPIP_USE_SMTP_CLIENT) == true>
/*** SMTP client Initialization Data ***/
const TCPIP_SMTP_CLIENT_MODULE_CONFIG tcpipSMTPInitData =
{ 
    0
};
</#if>

<#if (tcpipSmtpc.TCPIP_USE_SMTPC_CLIENT)?has_content && (tcpipSmtpc.TCPIP_USE_SMTPC_CLIENT) == true>
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
</#if>

<#if (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT)?has_content && (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT) == true>
/*** DHCP client Initialization Data ***/
const TCPIP_DHCP_MODULE_CONFIG tcpipDHCPInitData =
{     
    .dhcpEnable     = false,   
    .dhcpTmo        = TCPIP_DHCP_TIMEOUT,
    .dhcpCliPort    = TCPIP_DHCP_CLIENT_CONNECT_PORT,
    .dhcpSrvPort    = TCPIP_DHCP_SERVER_LISTEN_PORT,

};
</#if>

<#if (tcpipBerkeleyApi.TCPIP_STACK_USE_BERKELEY_API)?has_content && (tcpipBerkeleyApi.TCPIP_STACK_USE_BERKELEY_API) == true>
/*** Berkeley API Initialization Data ***/
const BERKELEY_MODULE_CONFIG tcpipBerkeleyInitData = 
{
    .maxSockets     = MAX_BSD_SOCKETS,
};
</#if>

<#if (tcpipIcmp.TCPIP_STACK_USE_ICMP_SERVER)?has_content && (tcpipIcmp.TCPIP_STACK_USE_ICMP_SERVER) == true>
/*** ICMP Server Initialization Data ***/
const TCPIP_ICMP_MODULE_CONFIG tcpipICMPInitData = 
{
    0
};
</#if>

<#if (tcpipNbns.TCPIP_USE_NBNS)?has_content && (tcpipNbns.TCPIP_USE_NBNS) == true>
/*** NBNS Server Initialization Data ***/
const TCPIP_NBNS_MODULE_CONFIG tcpipNBNSInitData =
{ 
    0
};
</#if>

<#if (tcpipIgmp.TCPIP_USE_IGMP)?has_content && (tcpipIgmp.TCPIP_USE_IGMP) == true>
/*** IGMP module Initialization Data ***/
const TCPIP_IGMP_MODULE_CONFIG tcpipIGMPInitData =
{     
    .lowSsmAddress     = 0,   
    .highSsmAddress    = 0,
    .reportInterval    = TCPIP_IGMP_UNSOLICITED_REPORT_INTERVAL,
    .nInterfaces       = TCPIP_IGMP_INTERFACES,
};
</#if>


<#if (tcpipDdns.TCPIP_USE_DDNS)?has_content && (tcpipDdns.TCPIP_USE_DDNS) == true>
/*** DDNS Initialization Data ***/
const DDNS_MODULE_CONFIG tcpipDDNSInitData =
{
    0
};
</#if>

<#if (tcpipZeroConf.TCPIP_USE_LINK_ZERO_CONFIG)?has_content && (tcpipZeroConf.TCPIP_USE_LINK_ZERO_CONFIG) == true>
/*** Zeroconfig initialization data ***/
const ZCLL_MODULE_CONFIG tcpipZCLLInitData =
{
    0
};
</#if>


<#if (tcpipTftpc.TCPIP_USE_TFTPC_MODULE)?has_content && (tcpipTftpc.TCPIP_USE_TFTPC_MODULE) == true>
/*** TFTP Client Initialization Data ***/
const TCPIP_TFTPC_MODULE_CONFIG tcpipTFTPCInitData =
{
    .tftpc_interface        = TCPIP_TFTPC_DEFAULT_IF,
    .tftpc_reply_timeout    = TCPIP_TFTPC_CMD_PROCESS_TIMEOUT,  
};
</#if>

<#if (tcpipTftps.TCPIP_USE_TFTPS_MODULE)?has_content && (tcpipTftps.TCPIP_USE_TFTPS_MODULE) == true>
/*** TFTP Server Initialization Data ***/
const TCPIP_TFTPS_MODULE_CONFIG tcpipTFTPSInitData =
{
    .mountPath              = TCPIP_TFTPS_MOUNT_POINT,
};
</#if>

<#if (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER)?has_content && (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER) == true>
/*** DHCP server initialization data ***/
TCPIP_DHCPS_ADDRESS_CONFIG DHCP_POOL_CONFIG[]=
{
<#if (tcpipDhcps.TCPIP_DHCP_SERVER_IDX0)?has_content && (tcpipDhcps.TCPIP_DHCP_SERVER_IDX0) == true>
    {
        .interfaceIndex     = TCPIP_DHCP_SERVER_INTERFACE_INDEX_IDX0,
		.poolIndex          = TCPIP_DHCP_SERVER_POOL_INDEX_IDX0,
        .serverIPAddress    = TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX0,
        .startIPAddRange    = TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX0,
        .ipMaskAddress      = TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX0,
        .priDNS             = TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX0,
        .secondDNS          = TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX0,
        .poolEnabled        = TCPIP_DHCP_SERVER_POOL_ENABLED_IDX0,
    },
</#if>
<#if (tcpipDhcps.TCPIP_DHCP_SERVER_IDX1)?has_content && (tcpipDhcps.TCPIP_DHCP_SERVER_IDX1) == true>
    {
        .interfaceIndex     = TCPIP_DHCP_SERVER_INTERFACE_INDEX_IDX1,
		.poolIndex          = TCPIP_DHCP_SERVER_POOL_INDEX_IDX1,
        .serverIPAddress    = TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX1,
        .startIPAddRange    = TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX1,
        .ipMaskAddress      = TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX1,
        .priDNS             = TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX1,
        .secondDNS          = TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX1,
        .poolEnabled        = TCPIP_DHCP_SERVER_POOL_ENABLED_IDX1,
    },
</#if>    
};
const TCPIP_DHCPS_MODULE_CONFIG tcpipDHCPSInitData =
{
    .enabled            = true,
    .deleteOldLease     = TCPIP_DHCP_SERVER_DELETE_OLD_ENTRIES,
	.dhcpServerCnt		= TCPIP_DHCPS_MAX_NUMBER_INSTANCES,
    .leaseEntries       = TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT,
    .entrySolvedTmo     = TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO,
    .dhcpServer         = (TCPIP_DHCPS_ADDRESS_CONFIG*)DHCP_POOL_CONFIG,
};
</#if>

<#if (tcpipFtps.TCPIP_USE_FTP_MODULE)?has_content && (tcpipFtps.TCPIP_USE_FTP_MODULE) == true>
/*** FTP Server Initialization Data ***/
const TCPIP_FTP_MODULE_CONFIG tcpipFTPInitData =
{ 
    .cmdPort            = TCPIP_FTPS_COMMAND_LISTEN_PORT, 
    .dataPort           = TCPIP_FTPS_DATA_LISTEN_PORT, 
    .nConnections       = TCPIP_FTP_MAX_CONNECTIONS,
    .dataSktTxBuffSize  = TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE,
    .dataSktRxBuffSize  = TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE,
    .mountPath          = TCPIP_FTP_MOUNT_POINT,
<#if (tcpipFtps.TCPIP_FTPS_AUTH_CONFIG) != "Run Time Authentication">
    .userName           = TCPIP_FTP_USER_NAME,
    .password           = TCPIP_FTP_PASSWORD,
</#if>
};
</#if>

<#if (tcpipFtpc.TCPIP_STACK_USE_FTP_CLIENT)?has_content && (tcpipFtpc.TCPIP_STACK_USE_FTP_CLIENT) == true>
/*** FTP Client Initialization Data ***/
const TCPIP_FTPC_MODULE_CONFIG_TYPE tcpipFTPClientInitData =
{ 
    .nMaxClients            = TCPIP_FTPC_MAX_NUM_CLIENT,
    .data_tx_buffsize_dflt  = TCPIP_FTPC_DATA_SKT_TX_BUFF_SIZE_DFLT,
    .data_rx_buffsize_dflt  = TCPIP_FTPC_DATA_SKT_RX_BUFF_SIZE_DFLT,    
    .ftpcTmo                = TCPIP_FTPC_TMO,
};
</#if>

<#if (tcpipDns.TCPIP_USE_DNS_CLIENT)?has_content && (tcpipDns.TCPIP_USE_DNS_CLIENT) == true>
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
</#if>

<#if (tcpipDnss.TCPIP_USE_DNSS)?has_content && (tcpipDnss.TCPIP_USE_DNSS) == true>
/*** DNS Server Initialization Data ***/
const TCPIP_DNSS_MODULE_CONFIG tcpipDNSServerInitData =
{ 
    .deleteOldLease         = TCPIP_DNSS_DELETE_OLD_LEASE,
    .replyBoardAddr         = TCPIP_DNSS_REPLY_BOARD_ADDR,
    .IPv4EntriesPerDNSName  = TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS,
<#if (tcpipIPv6.TCPIP_STACK_USE_IPV6)?has_content && (tcpipIPv6.TCPIP_STACK_USE_IPV6) == true>
    .IPv6EntriesPerDNSName  = TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS,
<#else>
    .IPv6EntriesPerDNSName  = 0,
</#if>
};
</#if>

<#if (tcpipIPv6.TCPIP_STACK_USE_IPV6)?has_content && (tcpipIPv6.TCPIP_STACK_USE_IPV6) == true>
/*** IPv6 Initialization Data ***/
const TCPIP_IPV6_MODULE_CONFIG  tcpipIPv6InitData = 
{
    .rxfragmentBufSize      = TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE,
    .fragmentPktRxTimeout   = TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT,
};

<#if (tcpipDhcpcv6.TCPIP_STACK_USE_DHCPV6_CLIENT)?has_content && (tcpipDhcpcv6.TCPIP_STACK_USE_DHCPV6_CLIENT) == true>
/*** DHCPv6 client Initialization Data ***/
const TCPIP_DHCPV6_MODULE_CONFIG  tcpipDhcpcv6InitData = 
{
    .configFlags                = TCPIP_DHCPV6_CONFIG_STARTUP_FLAG,
    .dhcpCliPort                = TCPIP_DHCPV6_CLIENT_CONNECT_PORT,
    .dhcpSrvPort                = TCPIP_DHCPV6_SERVER_LISTEN_PORT,
    .duidType                   = TCPIP_DHCPV6_DUID_TYPE_CONFIG,
    .nIanaDcpts                 = TCPIP_DHCPV6_IANA_DESCRIPTORS_NO,
    .nIataDcpts                 = TCPIP_DHCPV6_IATA_DESCRIPTORS_NO,
    .nFreeDcpts                 = TCPIP_DHCPV6_IA_FREE_DESCRIPTORS_NO,
    .defaultIanaT1              = TCPIP_DHCPV6_IANA_DEFAULT_T1,
    .defaultIanaT2              = TCPIP_DHCPV6_IANA_DEFAULT_T2,
    .defaultIataT1              = TCPIP_DHCPV6_IATA_DEFAULT_T1,
    .defaultIataT2              = TCPIP_DHCPV6_IATA_DEFAULT_T2,
    .ianaSolicitT1              = TCPIP_DHCPV6_IANA_SOLICIT_T1,
    .ianaSolicitT2              = TCPIP_DHCPV6_IANA_SOLICIT_T2,
    .solicitPrefLTime           = TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_PREF_LTIME,
    .solicitValidLTime          = TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_VALID_LTIME,
    .nMsgBuffers                = TCPIP_DHCPV6_MESSAGE_BUFFERS,
    .msgBufferSize              = TCPIP_DHCPV6_MESSAGE_BUFFER_SIZE,        
};
</#if>
</#if>

<#if (tcpipIPv4.TCPIP_STACK_USE_IPV4)?has_content && (tcpipIPv4.TCPIP_STACK_USE_IPV4) == true>
/*** IPv4 Initialization Data ***/

<#if (tcpipIPv4.TCPIP_IPV4_FORWARD) == true>
TCPIP_IPV4_FORWARD_ENTRY_ASCII tcpipIPv4AsciiForwardTbl[TCPIP_IPV4_FORWARDING_TABLE_ENTRIES] =
{
    // destination     mask    gateway   in interface   out interface   metric        
<#if (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX0)?has_content && (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX0) == true>
    {  "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX0)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX0)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX0)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX0)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX0)}", ${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_METRIC_IDX0)} },
</#if>                                                                                                                                                                                                                      
<#if (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX1)?has_content && (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX1) == true>                                                                                                                
    {  "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX1)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX1)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX1)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX1)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX1)}", ${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_METRIC_IDX1)} },
</#if>                                                                                                                                                                                                                      
<#if (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX2)?has_content && (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX2) == true>                                                                                                                
    {  "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX2)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX2)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX2)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX2)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX2)}", ${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_METRIC_IDX2)} },
</#if>                                                                                                                                                                                                                      
<#if (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX3)?has_content && (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX3) == true>                                                                                                                
    {  "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX3)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX3)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX3)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX3)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX3)}", ${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_METRIC_IDX3)} },
</#if>                                                                                                                                                                                                                      
<#if (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX4)?has_content && (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX4) == true>                                                                                                                
    {  "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX4)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX4)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX4)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX4)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX4)}", ${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_METRIC_IDX4)} },
</#if>                                                                                                                                                                                                                      
<#if (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX5)?has_content && (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX5) == true>                                                                                                                
    {  "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX5)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX5)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX5)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX5)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX5)}", ${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_METRIC_IDX5)} },
</#if>                                                                                                                                                                                                                      
<#if (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX6)?has_content && (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX6) == true>                                                                                                                
    {  "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX6)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX6)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX6)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX6)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX6)}", ${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_METRIC_IDX6)} },
</#if>                                                                                                                                                                                                                      
<#if (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX7)?has_content && (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX7) == true>                                                                                                                
    {  "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX7)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX7)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX7)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX7)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX7)}", ${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_METRIC_IDX7)} },
</#if>                                                                                                                                                                                                                      
<#if (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX8)?has_content && (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX8) == true>                                                                                                                
    {  "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX8)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX8)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX8)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX8)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX8)}", ${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_METRIC_IDX8)} },
</#if>                                                                                                                                                                                                                      
<#if (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX9)?has_content && (tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_BOOLX9) == true>                                                                                                                
    {  "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX9)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX9)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX9)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX9)}", "${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX9)}", ${(tcpipIPv4.TCPIP_IPV4_FWD_ENTRY_METRIC_IDX9)} },
</#if>
};
</#if>

const TCPIP_IPV4_MODULE_CONFIG  tcpipIPv4InitData = 
{
    .arpEntries = TCPIP_IPV4_ARP_SLOTS, 
<#if (tcpipIPv4.TCPIP_IPV4_FORWARD)?has_content && (tcpipIPv4.TCPIP_IPV4_FORWARD) == true>
    // forwarding values
    .forwardFlags = TCPIP_IPV4_FWD_FLAGS,
    .forwardTxQueueSize = TCPIP_IPV4_FWD_TX_SLOTS,
    .forwardTableMaxEntries = TCPIP_IPV4_FORWARDING_TABLE_MAX_SIZE,
    .forwardTableSize = TCPIP_IPV4_FORWARDING_TABLE_ENTRIES,
    .forwardTable = (const TCPIP_IPV4_FORWARD_ENTRY*)tcpipIPv4AsciiForwardTbl,

</#if>
};
</#if>

<#if (tcpipSnmp.TCPIP_USE_SNMP)?has_content && (tcpipSnmp.TCPIP_USE_SNMP) == true>
TCPIP_SNMP_COMMUNITY_CONFIG tcpipSNMPInitReadcommunity[] =
{
<#if (tcpipSnmp.TCPIP_SNMP_STACK_CONFIG_IDX0)>
/*** SNMP Configuration Index 0 ***/
    {
        TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDX0,
    },
</#if>
<#if (tcpipSnmp.TCPIP_SNMP_STACK_CONFIG_IDX1)>
/*** SNMP Configuration Index 1 ***/
    {
        TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDX1,
    },
</#if>  
<#if (tcpipSnmp.TCPIP_SNMP_STACK_CONFIG_IDX2)>
/*** SNMP Configuration Index 2 ***/    
    {
        TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDX2,
    },
</#if>  
};

TCPIP_SNMP_COMMUNITY_CONFIG tcpipSNMPInitWritecommunity[] =
{
<#if (tcpipSnmp.TCPIP_SNMP_STACK_CONFIG_IDX0)>
/*** SNMP Configuration Index 0 ***/
    {
        TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDX0,
    },
</#if>  
<#if (tcpipSnmp.TCPIP_SNMP_STACK_CONFIG_IDX1)>
/*** SNMP Configuration Index 1 ***/
    {
        TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDX1,
    },
</#if>  
<#if (tcpipSnmp.TCPIP_SNMP_STACK_CONFIG_IDX2)>
/*** SNMP Configuration Index 2 ***/
    {
        TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDX2,
    },
</#if>  
};

<#if (tcpipSnmpv3.TCPIP_USE_SNMPv3)?has_content && (tcpipSnmpv3.TCPIP_USE_SNMPv3) == true>
// SNMPv3 USM configuration
TCPIP_SNMPV3_USM_USER_CONFIG tcpipSNMPv3InitUSM[] =
{
<#if (tcpipSnmpv3.TCPIP_SNMPV3_STACK_CONFIG_IDX0)>
/*** SNMPV3 Configuration Index 0 ***/
    {
        TCPIP_SNMPV3_STACK_USM_NAME_IDX0,                       /*** securityName ***/
        TCPIP_SNMPV3_STACK_SECURITY_LEVEL_IDX0,                 /*** authentication and privacy security-level ***/
        /*** auth ***/
        TCPIP_SNMPV3_STACK_AUTH_PROTOCOL_IDX0,                  /*** MD5 auth protocol ***/
        TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDX0,                  /*** auth passphrase ***/
        /*** priv ***/
        TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX0,                  /*** AES priv protocol ***/
        TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX0,                  /*** priv passphrase ***/
    },
</#if>  
<#if (tcpipSnmpv3.TCPIP_SNMPV3_STACK_CONFIG_IDX1)>
/*** SNMPV3 Configuration Index 1 ***/  
    {
        TCPIP_SNMPV3_STACK_USM_NAME_IDX1,                       /*** securityName ***/
        TCPIP_SNMPV3_STACK_SECURITY_LEVEL_IDX1,                 /*** authentication and privacy security-level ***/
        /*** auth ***/
        TCPIP_SNMPV3_STACK_AUTH_PROTOCOL_IDX1,                  /*** MD5 auth protocol ***/
        TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDX1,                  /*** auth passphrase ***/
        /*** priv ***/
        TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX1,                  /*** AES priv protocol ***/
        TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX1,                  /*** priv passphrase ***/
    },
</#if>  
<#if (tcpipSnmpv3.TCPIP_SNMPV3_STACK_CONFIG_IDX2)>
/*** SNMPV3 Configuration Index 2 ***/  
    {
        TCPIP_SNMPV3_STACK_USM_NAME_IDX2,                       /*** securityName ***/
        TCPIP_SNMPV3_STACK_SECURITY_LEVEL_IDX2,                 /*** authentication and privacy security-level ***/
        /*** auth ***/
        TCPIP_SNMPV3_STACK_AUTH_PROTOCOL_IDX2,                  /*** MD5 auth protocol ***/
        TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDX2,                  /*** auth passphrase ***/
        /*** priv ***/
        TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX2,                  /*** AES priv protocol ***/
        TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX2,                  /*** priv passphrase ***/
    },
</#if>  
};

// SNMPv3 USM based Trap configuration
// User name should be exacly same to the above USM table.
TCPIP_SNMPV3_TARGET_ENTRY_CONFIG tcpipSNMPv3InitTargetTrap[]=
{
<#if (tcpipSnmpv3.TCPIP_SNMPV3_STACK_CONFIG_IDX0)>
/*** SNMPV3 Configuration Index 0 ***/
    {
        TCPIP_SNMPV3_TARGET_ENTRY_SEC_NAME_IDX0,                    /*** securityName ***/
        TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE_IDX0,       /*** Message processing model ***/
        TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE_IDX0,              /*** Security Model ***/
        TCPIP_SNMPV3_TARGET_ENTRY_SEC_LEVEL_IDX0,                   /*** Security-level ***/
    },
</#if>  
<#if (tcpipSnmpv3.TCPIP_SNMPV3_STACK_CONFIG_IDX1)>
/*** SNMPV3 Configuration Index 1 ***/  
     {
        TCPIP_SNMPV3_TARGET_ENTRY_SEC_NAME_IDX1,                    /*** securityName ***/
        TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE_IDX1,       /*** Message processing model ***/
        TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE_IDX1,              /*** Security Model ***/
        TCPIP_SNMPV3_TARGET_ENTRY_SEC_LEVEL_IDX1,                   /*** Security-level ***/
    },
</#if>  
<#if (tcpipSnmpv3.TCPIP_SNMPV3_STACK_CONFIG_IDX2)>
/*** SNMPV3 Configuration Index 2 ***/  
    {
        TCPIP_SNMPV3_TARGET_ENTRY_SEC_NAME_IDX2,                    /*** securityName ***/
        TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE_IDX2,       /*** Message processing model ***/
        TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE_IDX2,              /*** Security Model ***/
        TCPIP_SNMPV3_TARGET_ENTRY_SEC_LEVEL_IDX2,                   /*** Security-level ***/
    },
</#if>
};
</#if>

const TCPIP_SNMP_MODULE_CONFIG tcpipSNMPInitData =
{
    .trapEnable             = TCPIP_SNMP_USE_TRAP_SUPPORT,
    .snmp_trapv2_use        = TCPIP_SNMP_STACK_USE_V2_TRAP,
<#if (tcpipSnmpv3.TCPIP_USE_SNMPv3)?has_content && (tcpipSnmpv3.TCPIP_USE_SNMPv3) == true>
    .snmpv3_trapv1v2_use    = TCPIP_SNMPV3_STACK_USE_V1_V2_TRAP,
<#else>
    .snmpv3_trapv1v2_use    = false,
</#if>  
    .snmp_bib_file          = TCPIP_SNMP_BIB_FILE_NAME,
    .read_community_config  = (TCPIP_SNMP_COMMUNITY_CONFIG*)tcpipSNMPInitReadcommunity,
    .write_community_config = (TCPIP_SNMP_COMMUNITY_CONFIG*)tcpipSNMPInitWritecommunity,
<#if (tcpipSnmpv3.TCPIP_USE_SNMPv3)?has_content && (tcpipSnmpv3.TCPIP_USE_SNMPv3) == true>
    .usm_config             = (TCPIP_SNMPV3_USM_USER_CONFIG*)tcpipSNMPv3InitUSM,
    .trap_target_config     = (TCPIP_SNMPV3_TARGET_ENTRY_CONFIG*)tcpipSNMPv3InitTargetTrap,
<#else>
    .usm_config             = NULL,
    .trap_target_config     = NULL,
</#if>
    .mountPath              = TCPIP_SNMP_MOUNT_POINT,
};
</#if>

<#if (tcpipNetConfig.TCPIP_STACK_USE_MAC_BRIDGE)?has_content && (tcpipNetConfig.TCPIP_STACK_USE_MAC_BRIDGE) == true>
TCPIP_MAC_BRIDGE_ENTRY_BIN tcpipMacbridgeTable[${tcpipNetConfig.TCPIP_STACK_NETWORK_MAC_BRIDGE_COUNT}] = 
{
<#if (tcpipNetConfig.TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX)?has_content>
<#list 0..(tcpipNetConfig.TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX - 1) as i>
<#assign netConfigEnabled = "tcpipNetConfig_${i}">
<#if .vars[netConfigEnabled]?has_content>
<#assign network_config_idx = "tcpipNetConfig_${i}.TCPIP_STACK_NETWORK_CONFIG_IDX${i}"?eval>
<#assign mac_bridge_enable = "tcpipNetConfig_${i}.TCPIP_NETWORK_MACBRIDGE_ADD_IDX${i}"?eval>
<#if network_config_idx??>
<#if network_config_idx == true>
<#if mac_bridge_enable == true>
	{${i}},
</#if>
</#if>
</#if>
</#if>
</#list>
</#if>
};
</#if>

<#if (tcpipNetConfig.TCPIP_STACK_USE_MAC_BRIDGE)?has_content && (tcpipNetConfig.TCPIP_STACK_USE_MAC_BRIDGE) == true>
/*** TCPIP MAC Bridge Initialization Data ***/
const TCPIP_MAC_BRIDGE_CONFIG  tcpipBridgeInitData = 
{
	.purgeTimeout = TCPIP_MAC_BRIDGE_ENTRY_TIMEOUT,
    .transitDelay = TCPIP_MAC_BRIDGE_MAX_TRANSIT_DELAY,
	.fdbEntries = TCPIP_MAC_BRIDGE_FDB_TABLE_ENTRIES,
    .pktPoolSize = TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE,
    .pktSize = TCPIP_MAC_BRIDGE_PACKET_SIZE,
    .dcptPoolSize = TCPIP_MAC_BRIDGE_DCPT_POOL_SIZE,
    .pktReplenish = TCPIP_MAC_BRIDGE_PACKET_POOL_REPLENISH,
    .dcptReplenish = TCPIP_MAC_BRIDGE_DCPT_POOL_REPLENISH,
    .bridgeFlags = TCPIP_MC_BRIDGE_INIT_FLAGS,
    .bridgeTableSize = ${tcpipNetConfig.TCPIP_STACK_NETWORK_MAC_BRIDGE_COUNT},
    .bridgeTable = (const TCPIP_MAC_BRIDGE_ENTRY*)tcpipMacbridgeTable,
    // advanced
	.bridgePermTableSize = 0,
    .bridgePermTable = 0,

};
</#if>



<#if (TCPIP_STACK_USE_HEAP_CONFIG)?has_content>
<#if (TCPIP_STACK_USE_HEAP_CONFIG) == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP">
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
<#elseif (TCPIP_STACK_USE_HEAP_CONFIG) == "TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP">
TCPIP_STACK_HEAP_EXTERNAL_CONFIG tcpipHeapConfig =
{
    .heapType = TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP,
    .heapFlags = TCPIP_STACK_HEAP_USE_FLAGS,
    .heapUsage = TCPIP_STACK_HEAP_USAGE_CONFIG,
    .malloc_fnc = TCPIP_STACK_MALLOC_FUNC,
    .calloc_fnc = TCPIP_STACK_CALLOC_FUNC,
    .free_fnc = TCPIP_STACK_FREE_FUNC,
};
<#elseif (TCPIP_STACK_USE_HEAP_CONFIG) == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL">
    <#lt>TCPIP_STACK_HEAP_POOL_ENTRY tcpipHeapPoolEntryTbl[TCPIP_HEAP_POOL_ENTRIES_NUMBER] =
    <#lt>{
    <#lt>   // entrySize    //nBlocks   // expansion Blocks
    <#if TCPIP_HEAP_POOL_ENTRIES_NUMBER gte 1>
        <#list 0 ..(TCPIP_HEAP_POOL_ENTRIES_NUMBER -1) as i >
            <#assign blkSize = "TCPIP_HEAP_POOL_ENTRY_SIZE_IDX" + i>
            <#assign preAllocBlkNum = "TCPIP_HEAP_POOL_ENTRY_BLOCKS_IDX" + i>
            <#assign dynExpBlkNum= "TCPIP_HEAP_POOL_ENTRY_EXP_BLOCKS_IDX" + i>
    <#lt>   {${.vars[blkSize]},     ${.vars[preAllocBlkNum]},       ${.vars[dynExpBlkNum]}},
        </#list>
    </#if>  
    <#lt>};



TCPIP_STACK_HEAP_POOL_CONFIG tcpipHeapConfig =
{
    .heapType = TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL,
    .heapFlags = TCPIP_STACK_HEAP_USE_FLAGS,
    .heapUsage = TCPIP_STACK_HEAP_USAGE_CONFIG,
    .malloc_fnc = TCPIP_STACK_MALLOC_FUNC,
    .calloc_fnc = TCPIP_STACK_CALLOC_FUNC,
    .free_fnc = TCPIP_STACK_FREE_FUNC,
    .nPoolEntries = TCPIP_HEAP_POOL_ENTRIES_NUMBER,
    .pEntries = tcpipHeapPoolEntryTbl,
    .expansionHeapSize = TCPIP_STACK_INTERNAL_HEAP_POOL_EXPANSION_SIZE,
};
</#if>
</#if>


const TCPIP_NETWORK_CONFIG __attribute__((unused))  TCPIP_HOSTS_CONFIGURATION[] =
{
<#if (tcpipNetConfig.TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX)?has_content>
<#list 0..(tcpipNetConfig.TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX - 1) as i>
<#assign netConfigEnabled = "tcpipNetConfig_${i}">
<#if .vars[netConfigEnabled]?has_content>
<#assign network_config_idx = "tcpipNetConfig_${i}.TCPIP_STACK_NETWORK_CONFIG_IDX${i}"?eval>
<#if .vars["TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX${i?string}"]?has_content >
<#assign network_ipv6_idx = "tcpipNetConfig_${i}.TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX${i}"?eval>
</#if>
<#if network_config_idx??>
<#if network_config_idx == true>
    /*** Network Configuration Index ${i} ***/
    {
        .interface = TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${i},
        .hostName = TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX${i},
        .macAddr = TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX${i},
        .ipAddr = TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX${i},
        .ipMask = TCPIP_NETWORK_DEFAULT_IP_MASK_IDX${i},
        .gateway = TCPIP_NETWORK_DEFAULT_GATEWAY_IDX${i},
        .priDNS = TCPIP_NETWORK_DEFAULT_DNS_IDX${i},
        .secondDNS = TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX${i},
        .powerMode = TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX${i},
        .startFlags = TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX${i},
        .pMacObject = &TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX${i},
<#if network_ipv6_idx??>
<#if network_ipv6_idx == true>
        .ipv6Addr = TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX${i},
        .ipv6PrefixLen = TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX${i},
        .ipv6Gateway = TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX${i},
</#if>
</#if>
    },
</#if>
</#if>
</#if>
</#list>
</#if>
};

const size_t TCPIP_HOSTS_CONFIGURATION_SIZE = sizeof (TCPIP_HOSTS_CONFIGURATION) / sizeof (*TCPIP_HOSTS_CONFIGURATION);

const TCPIP_STACK_MODULE_CONFIG TCPIP_STACK_MODULE_CONFIG_TBL [] =
{
<#if (tcpipIPv4.TCPIP_STACK_USE_IPV4)?has_content &&(tcpipIPv4.TCPIP_STACK_USE_IPV4) == true>
    {TCPIP_MODULE_IPV4,             &tcpipIPv4InitData},
</#if>

<#if (tcpipIcmp.TCPIP_STACK_USE_ICMP_CLIENT)?has_content >
<#if (tcpipIcmp.TCPIP_STACK_USE_ICMP_CLIENT) == true || (tcpipIcmp.TCPIP_STACK_USE_ICMP_SERVER) == true>
    {TCPIP_MODULE_ICMP,             0},                             // TCPIP_MODULE_ICMP
</#if>
</#if>

<#if (tcpipArp.TCPIP_USE_ARP)?has_content && (tcpipArp.TCPIP_USE_ARP) == true>
    {TCPIP_MODULE_ARP,              &tcpipARPInitData},             // TCPIP_MODULE_ARP
</#if>
<#if (tcpipIPv6.TCPIP_STACK_USE_IPV6)?has_content && (tcpipIPv6.TCPIP_STACK_USE_IPV6) == true>
    {TCPIP_MODULE_IPV6,             &tcpipIPv6InitData},            // TCPIP_MODULE_IPV6
    {TCPIP_MODULE_ICMPV6,           0},                             // TCPIP_MODULE_ICMPV6
    {TCPIP_MODULE_NDP,              0},                             // TCPIP_MODULE_NDP
<#if (tcpipDhcpcv6.TCPIP_STACK_USE_DHCPV6_CLIENT)?has_content && (tcpipDhcpcv6.TCPIP_STACK_USE_DHCPV6_CLIENT) == true>	
	{TCPIP_MODULE_DHCPV6_CLIENT,    &tcpipDhcpcv6InitData},         // TCPIP_MODULE_DHCPV6_CLEINT
</#if>	
</#if>
<#if (tcpipUdp.TCPIP_USE_UDP)?has_content && (tcpipUdp.TCPIP_USE_UDP) == true>
    {TCPIP_MODULE_UDP,              &tcpipUDPInitData},             // TCPIP_MODULE_UDP
</#if>
<#if (tcpipTcp.TCPIP_USE_TCP)?has_content && (tcpipTcp.TCPIP_USE_TCP) == true>
    {TCPIP_MODULE_TCP,              &tcpipTCPInitData},             // TCPIP_MODULE_TCP
</#if>
<#if (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT)?has_content && (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT) == true>
    {TCPIP_MODULE_DHCP_CLIENT,      &tcpipDHCPInitData},            // TCPIP_MODULE_DHCP_CLIENT
</#if>
<#if (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER)?has_content && (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER) == true>
    {TCPIP_MODULE_DHCP_SERVER,      &tcpipDHCPSInitData},           // TCPIP_MODULE_DHCP_SERVER
</#if>
<#if (tcpipAnnounce.TCPIP_USE_ANNOUNCE)?has_content && (tcpipAnnounce.TCPIP_USE_ANNOUNCE) == true>
    {TCPIP_MODULE_ANNOUNCE,         &tcpipAnnounceInitData},        // TCPIP_MODULE_ANNOUNCE
</#if>
<#if (tcpipDns.TCPIP_USE_DNS_CLIENT)?has_content && (tcpipDns.TCPIP_USE_DNS_CLIENT) == true>
    {TCPIP_MODULE_DNS_CLIENT,       &tcpipDNSClientInitData},       // TCPIP_MODULE_DNS_CLIENT
</#if>
<#if (tcpipDnss.TCPIP_USE_DNSS)?has_content && (tcpipDnss.TCPIP_USE_DNSS) == true>
    {TCPIP_MODULE_DNS_SERVER,       &tcpipDNSServerInitData},       // TCPIP_MODULE_DNS_SERVER
</#if>
<#if (tcpipNbns.TCPIP_USE_NBNS)?has_content && (tcpipNbns.TCPIP_USE_NBNS) == true>
    {TCPIP_MODULE_NBNS,             &tcpipNBNSInitData},            // TCPIP_MODULE_NBNS
</#if>
<#if (tcpipSntp.TCPIP_USE_SNTP_CLIENT)?has_content && (tcpipSntp.TCPIP_USE_SNTP_CLIENT) == true>
    {TCPIP_MODULE_SNTP,             &tcpipSNTPInitData},            // TCPIP_MODULE_SNTP
</#if>

<#if (tcpipBerkeleyApi.TCPIP_STACK_USE_BERKELEY_API)?has_content && (tcpipBerkeleyApi.TCPIP_STACK_USE_BERKELEY_API) == true>
    {TCPIP_MODULE_BERKELEY,         &tcpipBerkeleyInitData},        // TCPIP_MODULE_BERKELEY
</#if>
<#if (tcpipFtps.TCPIP_USE_FTP_MODULE)?has_content && (tcpipFtps.TCPIP_USE_FTP_MODULE) == true>
    {TCPIP_MODULE_FTP_SERVER,       &tcpipFTPInitData},             // TCPIP_MODULE_FTP
</#if>
<#if (tcpipFtpc.TCPIP_STACK_USE_FTP_CLIENT)?has_content && (tcpipFtpc.TCPIP_STACK_USE_FTP_CLIENT) == true>
    {TCPIP_MODULE_FTP_CLIENT,       &tcpipFTPClientInitData},       // TCPIP_MODULE_FTP_CLIENT
</#if>
<#if (tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER)?has_content && (tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER) == true>
    {TCPIP_MODULE_HTTP_SERVER,      &tcpipHTTPInitData},            // TCPIP_MODULE_HTTP_SERVER
</#if>
<#if (tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER)?has_content && (tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER) == true>
    {TCPIP_MODULE_HTTP_NET_SERVER,  &tcpipHTTPNetInitData},         // TCPIP_MODULE_HTTP_NET_SERVER
</#if>
<#if (tcpipTelnet.TCPIP_USE_TELNET)?has_content &&  (tcpipTelnet.TCPIP_USE_TELNET) == true>
    {TCPIP_MODULE_TELNET_SERVER,    &tcpipTelnetInitData},          // TCPIP_MODULE_TELNET_SERVER
</#if>
<#if (tcpipSnmp.TCPIP_USE_SNMP)?has_content && (tcpipSnmp.TCPIP_USE_SNMP) == true>
    {TCPIP_MODULE_SNMP_SERVER,      &tcpipSNMPInitData},            // TCPIP_MODULE_SNMP_SERVER
</#if>
<#if (tcpipSmtpc.TCPIP_USE_SMTPC_CLIENT)?has_content && (tcpipSmtpc.TCPIP_USE_SMTPC_CLIENT) == true>
    {TCPIP_MODULE_SMTPC, &tcpipSMTPCInitData},                                  // TCPIP_MODULE_SMTPC,
</#if>
<#if (tcpipDdns.TCPIP_USE_DDNS)?has_content && (tcpipDdns.TCPIP_USE_DDNS) == true>
    {TCPIP_MODULE_DYNDNS_CLIENT,    &tcpipDDNSInitData},            // TCPIP_MODULE_DYNDNS_CLIENT,
</#if>
<#if (tcpipReboot.TCPIP_USE_REBOOT_SERVER)?has_content && (tcpipReboot.TCPIP_USE_REBOOT_SERVER) == true>
    {TCPIP_MODULE_REBOOT_SERVER,    0},                             // TCPIP_MODULE_REBOOT_SERVER,
</#if>
<#if (tcpipZeroConf.TCPIP_USE_LINK_ZERO_CONFIG)?has_content && (tcpipZeroConf.TCPIP_USE_LINK_ZERO_CONFIG) == true>
    {TCPIP_MODULE_ZCLL,             0},                             // TCPIP_MODULE_ZCLL,
</#if>
<#if (tcpipZeroConf.TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG)?has_content && (tcpipZeroConf.TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG) == true>
    {TCPIP_MODULE_MDNS,             0},                             // TCPIP_MODULE_MDNS,
</#if>
<#if (tcpipTftpc.TCPIP_USE_TFTPC_MODULE)?has_content && (tcpipTftpc.TCPIP_USE_TFTPC_MODULE) == true>
    {TCPIP_MODULE_TFTP_CLIENT,      &tcpipTFTPCInitData},           // TCPIP_MODULE_TFTP_CLIENT
</#if>
<#if (tcpipTftps.TCPIP_USE_TFTPS_MODULE)?has_content && (tcpipTftps.TCPIP_USE_TFTPS_MODULE) == true>
    {TCPIP_MODULE_TFTP_SERVER,      &tcpipTFTPSInitData},           // TCPIP_MODULE_TFTP_SERVER
</#if>
<#if (tcpipIgmp.TCPIP_USE_IGMP)?has_content && (tcpipIgmp.TCPIP_USE_IGMP) == true>
    {TCPIP_MODULE_IGMP, &tcpipIGMPInitData},            // TCPIP_MODULE_IGMP
</#if>
    { TCPIP_MODULE_MANAGER,         &tcpipHeapConfig },             // TCPIP_MODULE_MANAGER

// MAC modules
<#if (tcpipNetConfig.TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX)?has_content>
    <#function checkInterface interface>
    <#list 0..(tcpipNetConfig.TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX - 1) as i>
        <#assign netConfigEnabled = "tcpipNetConfig_${i}">
        <#if .vars[netConfigEnabled]?has_content>
            <#assign network_interface_name_idx = "tcpipNetConfig_${i}.TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX${i}"?eval>
            <#if network_interface_name_idx??>
                <#if network_interface_name_idx == interface>
                    <#return true>
                </#if>
            </#if>
        </#if>
    </#list>
    <#return false>
    </#function>
<#if checkInterface("ETHMAC")>
    {TCPIP_MODULE_MAC_PIC32INT,     &tcpipMACPIC32INTInitData},     // TCPIP_MODULE_MAC_PIC32INT
</#if>
<#if checkInterface("GMAC")>
    {TCPIP_MODULE_MAC_PIC32C,     &tcpipMACPIC32CINTInitData},     // TCPIP_MODULE_MAC_PIC32C
</#if>
<#if checkInterface("EMAC0")>
    {TCPIP_MODULE_MAC_SAM9X60_0,    &tcpipEMAC0InitData},           // TCPIP_MODULE_MAC_SAM9X60_0
</#if>
<#if checkInterface("EMAC1")>
    {TCPIP_MODULE_MAC_SAM9X60_1,    &tcpipEMAC1InitData},           // TCPIP_MODULE_MAC_SAM9X60_1
</#if>
<#if checkInterface("WINC")>
    {TCPIP_MODULE_MAC_WINC,         0},                             // TCPIP_MODULE_MAC_WINC
</#if>
<#if checkInterface("ENCX24J600")>
    {TCPIP_MODULE_MAC_ENCJ600,      drvEncX24j600InitData},    // TCPIP_MODULE_MAC_ENCJ600
</#if>

<#if checkInterface("ENC28J60")>
    {TCPIP_MODULE_MAC_ENCJ60,       &drvEnc28j60InitData},      // TCPIP_MODULE_MAC_ENCJ60
</#if>
</#if>
<#if (tcpipNetConfig.TCPIP_STACK_USE_MAC_BRIDGE)?has_content && (tcpipNetConfig.TCPIP_STACK_USE_MAC_BRIDGE) == true>
    {TCPIP_MODULE_MAC_BRIDGE,       &tcpipBridgeInitData},      // TCPIP_MODULE_MAC_BRIDGE 
</#if>
};

const size_t TCPIP_STACK_MODULE_CONFIG_TBL_SIZE = sizeof (TCPIP_STACK_MODULE_CONFIG_TBL) / sizeof (*TCPIP_STACK_MODULE_CONFIG_TBL);
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

<#if (TCPIP_STACK_ENABLE_INIT_CALLBACK) == true && (TCPIP_STACK_INIT_CALLBACK)?has_content >
extern int ${TCPIP_STACK_INIT_CALLBACK}( const struct TCPIP_STACK_INIT** ppStackInit );
</#if>

SYS_MODULE_OBJ TCPIP_STACK_Init(void)
{
    TCPIP_STACK_INIT    tcpipInit;

    tcpipInit.pNetConf = TCPIP_HOSTS_CONFIGURATION;
    tcpipInit.nNets = TCPIP_HOSTS_CONFIGURATION_SIZE;
    tcpipInit.pModConfig = TCPIP_STACK_MODULE_CONFIG_TBL;
    tcpipInit.nModules = TCPIP_STACK_MODULE_CONFIG_TBL_SIZE;
<#if (TCPIP_STACK_ENABLE_INIT_CALLBACK) == true && (TCPIP_STACK_INIT_CALLBACK)?has_content >
    tcpipInit.initCback = ${TCPIP_STACK_INIT_CALLBACK};
<#else>
    tcpipInit.initCback = 0;
</#if>

    return TCPIP_STACK_Initialize(0, &tcpipInit.moduleInit);
}
// </editor-fold>
<#--
/*******************************************************************************
 End of File
*/
-->
