/*******************************************************************************
  Microchip TCP/IP Stack Include File

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_common_ports.h

  Summary:
    List of the common UDP and TCP port numbers that are used
    in the stack implementation.

  Description:
    This is the list of the well-known IANA allocated ports
    that are used in the current stack implementation.

    See RFC 6335 and 
    www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml
    for details.

    Also some private port numbers (and ranges) used by the stack modules
    are listed in here. Update the values as needed.

*******************************************************************************/
//DOM-IGNORE-BEGIN
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








//DOM-IGNORE-END

#ifndef H_TCPIP_COMMON_PORTS_H__
#define H_TCPIP_COMMON_PORTS_H__

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// UDP dynamic port numbers
// *****************************************************************************
// *****************************************************************************

// The dynamic port range defined by IANA consists of the 49152-65535 range
// and is meant for the selection of ephemeral ports (RFC 6056).
// Adjust to your needs but stay within the IANA range 
// First port number for randomized local port number selection
#define TCPIP_UDP_LOCAL_PORT_START_NUMBER         (49152)
// Last port number for randomized local port number selection
#define TCPIP_UDP_LOCAL_PORT_END_NUMBER           (65535)



// *****************************************************************************
// *****************************************************************************
// TCP dynamic port numbers
// *****************************************************************************
// *****************************************************************************


// First port number for randomized local port number selection
// Use the dynamic port range defined by IANA consists of the 49152-65535 range
// and is meant for the selection of ephemeral ports (RFC 6056).
// Adjust to your needs but stay within the IANA range 
#define TCPIP_TCP_LOCAL_PORT_START_NUMBER         (49152)

// Last port number for randomized local port number selection
#define TCPIP_TCP_LOCAL_PORT_END_NUMBER           (65535)


// *****************************************************************************
// *****************************************************************************
// Stack modules port numbers
// *****************************************************************************
// *****************************************************************************


// ************************************************************
// HTTP server listening TCP port
#define TCPIP_HTTP_SERVER_PORT                  (80)
#define TCPIP_HTTP_NET_SERVER_PORT              (80)

// HTTPS server listening TCP port
#define TCPIP_HTTP_NET_SERVER_SECURE_PORT       (443)


// ************************************************************
// Unsecured Telnet server TCP port
#define TCPIP_TELNET_SERVER_PORT                (23)

// Secured Telnet server TCP port
#define TCPIP_TELNET_SERVER_SECURE_PORT         (992)   


// ************************************************************
// DHCP Client transactions UDP local port
#define TCPIP_DHCP_CLIENT_PORT                  (68)

// DHCP Server UDP remote port
#define TCPIP_DHCP_SERVER_PORT                  (67)



// DNS server UDP listening port
#define TCPIP_DNS_SERVER_PORT                   (53)


// ************************************************************
// Dynamic DNS TCP remote port
#define TCPIP_DDNS_SERVER_REMOTE_PORT           (80)



// ************************************************************
// NetBIOS Name UDP Service (server) port
#define TCPIP_NBNS_SERVER_PORT                  (137)

// ************************************************************
// SMTP server remote TCP port 
#define TCPIP_SMTP_SERVER_REMOTE_PORT           (25)
#define TCPIP_SMTP_SERVER_SECURE_SSL_PORT       (465)
#define TCPIP_SMTP_SERVER_SECURE_TLS_PORT       (587)

// ************************************************************
// NTP server remote TCP port
#define TCPIP_NTP_SERVER_REMOTE_PORT            (123)


// ************************************************************
// Reboot server listening UDP port
#define TCPIP_REBOOT_SERVER_PORT                (69)


// ************************************************************
// MCHP Announce service UDP port (server and client)
#define TCPIP_ANNOUNCE_PORT                     (30303)


// ************************************************************
// Iperf server listening port (TCP and UDP)
#define TCPIP_IPERF_SERVER_PORT                 (5001)

// Iperf client local TCP port
#define TCPIP_IPERF_TCP_LOCAL_PORT_START_NUMBER (1024)

// ************************************************************
// TFTP Client local UDP port
#define TCPIP_TFTP_CLIENT_PORT                  (65352)

// TFTP Server remote UDP Port
#define TCPIP_TFTP_SERVER_PORT                  (69)

// ************************************************************
// FTP ports
#define TCPIP_FTP_COMMAND_PORT                  (21)

#define TCPIP_FTP_DATA_PORT                     (20)

#define TCPIP_FTP_DATA_SECURE_PORT              (989)
#define TCPIP_FTP_COMMAND_SECURE_PORT           (990)

// ************************************************************
// DHCPv6 clients listening UDP port:
#define  TCPIP_DHCPV6_CLIENT_PORT               (546)

// DHCPv6 servers and relay agents listening UDP port:
#define  TCPIP_DHCPV6_SERVER_PORT               (547)

// ************************************************************
// NNTP servers port:
#define  TCPIP_NNTP_SERVER_SECURE_PORT          (563)

// ************************************************************
// LDAP port:
#define  TCPIP_LDAP_SERVER_SECURE_PORT          (636)


// ************************************************************
// IMAP port:
#define  TCPIP_IMAP_SERVER_PORT                 (143)
#define  TCPIP_IMAP_SERVER_SECURE_PORT          (993)

// ************************************************************
// MQTT broker TCP port
#define TCPIP_MQTT_BROKER_PORT                  (1883)

// MQTT broker secure TCP port
#define TCPIP_MQTT_BROKER_SECURE_PORT           (8883)


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif  // H_TCPIP_COMMON_PORTS_H__
