/*******************************************************************************
  DynDNS Headers for Microchip TCP/IP Stack API Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    ddns.h

  Summary:
    The Dynamic DNS Client module provides a method for updating a dynamic IP
    address to a public DDNS service.

  Description:
    The Dynamic DNS Client module provides a method for updating a dynamic IP 
    address to a public DDNS service. These services can be used to provide DNS 
    hostname mapping to devices that behind routers, firewalls, and/or on networks 
    that dynamically assign IP addresses.
*******************************************************************************/
//DOM-IGNORE-BEGIN
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








//DOM-IGNORE-END

#ifndef __DYN_DNSCLIENT_H_
#define __DYN_DNSCLIENT_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

//****************************************************************************
//****************************************************************************
//  Section:  Dynamic DNS Enumerations
//****************************************************************************
//****************************************************************************

// Dynamic DNS Services.
// Must support the DynDNS API (Auxlang) and correspond
// to ddnsServiceHosts and ddnsServicePorts in DynDNS.c.
typedef enum
{
    DYNDNS_ORG = 0u,      // www.dyndns.org
    NO_IP_COM,            // www.no-ip.com
    DNSOMATIC_COM         // www.dnsomatic.com
} DDNS_SERVICES;

// Status message for DynDNS client.  GOOD and NOCHG are okay, but
// ABUSE through 911 are fatal.  UNCHANGED through INVALID are locally
// defined.
typedef enum
{
    DDNS_STATUS_GOOD = 0u,        // Update successful, hostname is now updated
    DDNS_STATUS_NOCHG,            // Update changed no setting and is considered abusive.  
	                              // Additional 'nochg' updates will cause hostname to be blocked.

    DDNS_STATUS_ABUSE,            // The hostname specified is blocked for update abuse.
    DDNS_STATUS_BADSYS,           // System parameter not valid. Should be dyndns, statdns or custom.
    DDNS_STATUS_BADAGENT,         // The user agent was blocked or not sent.
    DDNS_STATUS_BADAUTH,          // The username and password pair do not match a real user.
    DDNS_STATUS_NOT_DONATOR,      // An option available only to credited users 
	                              // (such as offline URL) was specified, but the user is not a credited user. 
								  // If multiple hosts were specified, only a single !donator will be returned.
    DDNS_STATUS_NOT_FQDN,         // The hostname specified is not a fully-qualified domain name 
	                              // (not in the form hostname.dyndns.org or domain.com).
    DDNS_STATUS_NOHOST,           // The hostname specified does not exist in this user account 
	                              // (or is not in the service specified in the system parameter).
    DDNS_STATUS_NOT_YOURS,        // The hostname specified does not belong to this user account.
    DDNS_STATUS_NUMHOST,          // Too many hosts specified in an update.
    DDNS_STATUS_DNSERR,           // Unspecified DNS error encountered by the DDNS service.
    DDNS_STATUS_911,              // There is a problem or scheduled maintenance with the DDNS service.

    DDNS_STATUS_UPDATE_ERROR,     // Error communicating with Update service.
    DDNS_STATUS_UNCHANGED,        // The IP Check indicated that no update was necessary.
    DDNS_STATUS_CHECKIP_ERROR,    // Error communicating with CheckIP service.
    DDNS_STATUS_DNS_ERROR,        // DNS error resolving the CheckIP service.
    DDNS_STATUS_SKT_ERROR,        // TCP socket opening error
    DDNS_STATUS_INVALID,          // DDNS Client data is not valid.
    DDNS_STATUS_UNKNOWN           // DDNS client has not yet been executed with this configuration.
} DDNS_STATUS;

//****************************************************************************
/*
  Summary:
    Configuration parameters for the Dynamic DNS Client

  Description:
    This structure of pointers configures the Dynamic DNS Client.  Initially,
    all pointers will be null and the client will be disabled.  <c>Set
    DDNSClient.[field name].szRAM</c> to use a string stored in RAM, or
    <c>DDNSClient.[field name].szROM</c> to use a string stored in const.
    Where, <c>[field name]</c>, is one of the following parameters.

    If a const string is specified, <c>DDNSClient.ROMPointers.[field name]</c>
    must also be set to 1 to indicate that this field should be retrieved
    from const instead of RAM.

  Parameters:
    CheckIPServer - The server used to determine the external IP address
    CheckIPPort   - Port on the above server to connect to
    UpdateServer  - The server where updates should be posted
    UpdatePort    - Port on the above server to connect to
    Username      - The user name for the dynamic DNS server
    Password      - The password to supply when making updates
    Host          - The host name you wish to update
    ROMPointers   - Indicates which parameters to read from const instead of RAM.
  ***************************************************************************/
typedef struct
{
    union
    {
        uint8_t *szRAM;
        const uint8_t *szROM;
    } CheckIPServer;

    uint16_t CheckIPPort;

    union
    {
        uint8_t *szRAM;
        const uint8_t *szROM;
    } UpdateServer;

    uint16_t UpdatePort;

    union
    {
        uint8_t *szRAM;
        const uint8_t *szROM;
    } Username;

    union
    {
        uint8_t *szRAM;
        const uint8_t *szROM;
    } Password;

    union
    {
        uint8_t *szRAM;
        const uint8_t *szROM;
    } Host;

    struct
    {
        unsigned char CheckIPServer:1;
        unsigned char UpdateServer:1;
        unsigned char Username:1;
        unsigned char Password:1;
        unsigned char Host:1;

    } ROMPointers;

} DDNS_POINTERS;

// Global DDNS Configuration parameters
extern DDNS_POINTERS DDNSClient;

typedef struct
{
    void* reserved;
} DDNS_MODULE_CONFIG;

//****************************************************************************
//****************************************************************************
//  Section:  Function Prototypes
//****************************************************************************
//****************************************************************************
  
//*****************************************************************************
/*
  Function:
    void TCPIP_DDNS_UpdateForce(void)

  Summary:
    Forces an immediate DDNS update.

  Description:
    This function forces the DDNS Client to execute a full update
    immediately.  Any error message is cleared, and the update will be
    executed whether the IP address has changed or not.  Call this
    function every time the DDNSClient parameters have been modified.

  Precondition:
    TCPIP_DDNS_Initialize must have been called.

  Parameters:
    None.

  Returns:
    None.
*/
void         TCPIP_DDNS_UpdateForce(void);

//*****************************************************************************
/*
  Function:
    void TCPIP_DDNS_ServiceSet(DDNS_SERVICES svc)

  Summary:
    Selects a preconfigured Dynamic DNS service.

  Description:
    This function selects a Dynamic DNS service based on parameters
    configured in ddnsServiceHosts and ddnsServicePorts.  These arrays
    must match the DDNS_SERVICES enumeration.

  Precondition:
    None.

  Parameters:
    svc - one of the DDNS_SERVICES elements to indicate the selected service

  Returns:
    None.
	
*/
void         TCPIP_DDNS_ServiceSet(DDNS_SERVICES svc);

//*****************************************************************************
/*
  Function:
    IPV4_ADDR TCPIP_DDNS_LastIPGet(void)

  Summary:
    Returns the last known external IP address of the device.

  Description:
    This function returns the last known external IP address of the device.

 Precondition:
    None.

  Parameters:
    None.

  Returns:
    The last known external IP address of the device.
	
*/
DDNS_STATUS  TCPIP_DDNS_LastStatusGet(void);

//*****************************************************************************
/*
  Function:
    DDNS_STATUS TCPIP_DDNS_LastStatusGet(void)

  Summary:
    Returns the status of the most recent update.

  Description:
    This function returns the status of the most recent update.  See the
    DDNS_STATUS enumeration for possible codes.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    DDNS_STATUS indicating the status code for the most recent update.
	
*/
IPV4_ADDR    TCPIP_DDNS_LastIPGet(void);

// *****************************************************************************
/*
  Function:
    void  TCPIP_DDNS_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    Performs DDNS module tasks in the TCP/IP stack.

  Precondition:
    DDNS module should have been initialized

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_DDNS_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __DYN_DNSCLIENT_H_
