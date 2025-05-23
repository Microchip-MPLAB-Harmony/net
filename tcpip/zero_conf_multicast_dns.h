/*******************************************************************************
  Zero Configuration (Zeroconf) Multicast DNS and 
  Service Discovery Module for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    zero_conf_multicast_dns.h

  Summary:
    Zero configuration (Zeroconf), provides a mechanism to ease the configuration
    of a device on a network.

  Description:
    Zero configuration (Zeroconf), provides a mechanism to ease the configuration 
    of a device on a network. It also provides for a more human-like naming 
    convention, instead of relying on IP addresses alone. Zeroconf also goes by 
    the names Bonjour (Apple) and Avahi (Linux), and is an IETF standard.
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

#ifndef H_ZEROCONF_MULTICAST_DNS_H_
#define H_ZEROCONF_MULTICAST_DNS_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: MDNS Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MDNS Module Configuration Data Structure

  Summary:
    Placeholder for MDNS module configuration data.

  Description:
    Provides a placeholder for MDNS module configuration data.

  Remarks:
    None.
*/
typedef struct
{
    void*   reserved;
} TCPIP_MDNS_MODULE_CONFIG;


typedef enum
{
    MDNSD_SUCCESS       = 0,
    MDNSD_ERR_BUSY      = 1,    /* Already Being used for another Service */
    MDNSD_ERR_CONFLICT  = 2,    /* Name Conflict */
    MDNSD_ERR_INVAL     = 3,    /* Invalid Parameter */
}MDNSD_ERR_CODE;

// ****************************************************************************
// ****************************************************************************
// Section: MDNS Service Discovery Functions
// ****************************************************************************
// ****************************************************************************

//***************************************************************
/*
  Function:
    MDNSD_ERR_CODE TCPIP_MDNS_ServiceRegister( ...)

  Summary:
   DNS Service Discovery function for end-users to register a
   service advertisement. The service is associated with all interfaces.

  Description:
   This function is used by end-user application to announce its
    service on local network. All peer machines that are compliant
    with Multicast DNS and DNS Service Discovery protocol can detect
    the announcement and lists out an entry in the Service Browser list.
    The end-user selects an entry to connect to this service. So
    ultimately this is an aid to end-user to discover any service
    of interest on a local network.

    This is the first function that needs to be invoked by end-user
    application. Presently  Multicast-DNS and Service discovery
    stack supports only single service advertisement. Once the
    application wants to terminate the service it has to invoke
    TCPIP_MDNS_ServiceDeregister function to free the DNS SD stack for
    some other application.

  Precondition:
   None.

  Parameters:
    netH      - handle of the network to be registered
    srv_name  - Service Name, which is being advertised
    srv_type  - For a HTTP-Service its "_http._tcp.local"
                _http: is application protocol proceeded with an underscore
                _tcp: is lower-layer protocol on which service runs
                local: is to represent service is on local-network

                For a iTunes Music Sharing "_daap._tcp.local"
                For a Printing Service "_ipp._tcp.local"
                Refer to  www.dns-sd.org/ServiceTypes.html
                for more service types.

    port      - Port number on which service is running

    txt_len   - For additional information about service like
                default page (e.g., "index.htm") for HTTP-service.
                Length of such additional information
   txt_record - String of additional information (e.g., "index.htm")
                for HTTP service.

   auto_rename- A flag to indicate DNS-SD stack, whether to rename
                the service automatically or not.
                If this is set to '0' Callback parameter will be used
                to indicate the conflict error and user has to select
                different name and re-register with this function.
                If this is set to '1' service-name will be automatically
                renamed with numerical suffix.

    callback  - Callback function, which is user-application defined.
                This callback gets invoked on completion of service 
                advertisement. If an service name-conflict error is
                detected and auto_rename is set to '0' callback gets
                invoked with MDNSD_ERR_CONFLICT as error-code.

    context   - Opaque context (pointer to opaque data), which needs
                to be used in callback function.

  Returns:
    MDNSD_ERR_CODE - Returns Error-code to indicate whether or not registration 
    is successful:
    - MDNSD_SUCCESS   - returns on success of call
    - MDNSD_ERR_BUSY  - When already some other service is being advertised using 
                        this DNS SD stack
    - MDNSD_ERR_INVAL - Invalid parameter
 */
MDNSD_ERR_CODE
TCPIP_MDNS_ServiceRegister( TCPIP_NET_HANDLE netH
                    ,const char *srv_name
                    ,const char *srv_type
                    ,uint16_t port
                    ,const uint8_t *txt_record
                    ,uint8_t auto_rename
                    ,void (*call_back)(char *name, MDNSD_ERR_CODE err, void *context)
                    ,void *context);

//***************************************************************
/*
  Function:
    MDNSD_ERR_CODE TCPIP_MDNS_ServiceDeregister()

  Summary:
    DNS Service Discovery function for end-user to deregister a
    service advertisement, which was previously registered with the
    TCPIP_MDNS_ServiceRegister function.

  Description:
    This function is used by end-user application to deregister
    DNS Service Discovery on a local network. When this gets invoked
    by end-user DNS SD stack sends out Good-Bye packets to update
    all peer machines that service will no longer be present. All
    peer machines remove the corresponding entry from the Browser list.

    This is the last functions that needs to be invoked by the end-user
    application to free the DNS SD stack for some other application.

  Precondition:
   TCPIP_MDNS_ServiceRegister must be invoked before this call.

  Parameters:
    netH - handle of the network to be deregistered

  Returns:
    MDNSD_ERR_CODE - Returns an error code to indicate whether or not 
    registration is successful:
    - MDNSD_SUCCESS   - Returns on success of call
    - MDNSD_ERR_INVAL - When the input parameters are invalid or if the function
                        is invoked in an invalid state
 */
MDNSD_ERR_CODE TCPIP_MDNS_ServiceDeregister(TCPIP_NET_HANDLE netH);


//***************************************************************
/*
  Function:
    MDNSD_ERR_CODE TCPIP_MDNS_ServiceUpdate(
        TCPIP_NET_HANDLE netH,
        uint16_t port,
        const uint8_t *txt_record)

  Summary:
   DNS-Service Discovery function for end-user to update the service 
   advertisement, which was previously registered with TCPIP_MDNS_ServiceRegister.

  Description:
   This function is used by the end-user application to update its service,
    which was previously registered. With this function, the end-user application
    updates the port number on which the service is running. It can also
    update the additional information of service. For example, the
    default page can be updated to new page and corresponding page
    name can be input to this function to update all peer machines. The
    modified service will be announced with new contents on local
    network.

    This is an optional function and should be invoked only if it is
    necessary.

  Precondition:
   TCPIP_MDNS_ServiceRegister must be invoked before this call.

  Parameters:
    netH       - Handle of the network to perform the service update
    port       - Port number on which service is running
    txt_record - String of additional information (e.g., "index.htm")
                 for HTTP-service.

  Returns:
    MDNSD_ERR_CODE - Returns an error code to indicate whether or not registration 
                      is successful:
    - MDNSD_SUCCESS   - returns on success of call
    - MDNSD_ERR_INVAL - When the input parameters are invalid or
                        if the function is invoked in invalid state
 */
MDNSD_ERR_CODE TCPIP_MDNS_ServiceUpdate(
        TCPIP_NET_HANDLE netH,
        uint16_t port,
        const uint8_t *txt_record);

// *****************************************************************************
/*
  Function:
    void  TCPIP_MDNS_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs MDNS module tasks in the TCP/IP stack.

  Precondition:
    MDNS module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_MDNS_Task(void);


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // H_ZEROCONF_MULTICAST_DNS_H_
