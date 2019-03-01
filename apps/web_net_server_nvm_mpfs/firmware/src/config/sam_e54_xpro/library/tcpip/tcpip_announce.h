/**************************************************************************
Announce Service Module API Definitions Header File

  Company:
    Microchip Technology Inc.
	
  File Name:
    tcpip_announce.h
	
  Summary:
    Announce provides a simple discovery mechanism for Microchip TCP/IP hosts.
	
  Description:
    Announce Service Module for Microchip TCP/IP Stack.
    It is a simple, proprietary discovery mechanism for Microchip TCP/IP hosts.
    It works in conjunction with a GUI tool running on a PC.

  **************************************************************************/
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

#ifndef __TCPIP_ANNOUNCE_H
#define __TCPIP_ANNOUNCE_H

#include <stdint.h>
#include <stdbool.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// *****************************************************************************
/* Enumeration: TCPIP_ANNOUNCE_FIELD_PAYLOAD

  Summary:
    Announce payload types

  Description:
    Describes the current types supported by the announce module payload.
 */
typedef enum
{
    TCPIP_ANNOUNCE_FIELD_NONE   = 0,                // invalid
    TCPIP_ANNOUNCE_FIELD_TRUNCATED,                 // the payload is the truncated type
    TCPIP_ANNOUNCE_FIELD_MAC_ADDR,                  // the payload is the MAC address
    TCPIP_ANNOUNCE_FIELD_MAC_NAME,                  // the payload is the MAC name
    TCPIP_ANNOUNCE_FIELD_HOST_NAME,                 // the payload is the host name
    TCPIP_ANNOUNCE_FIELD_IPV4_ADDRESS,              // the payload is the IPv4 address
    TCPIP_ANNOUNCE_FIELD_IPV6_UNICAST,              // the payload is the IPv6 unicast address
    TCPIP_ANNOUNCE_FIELD_IPV6_MULTICAST,            // the payload is the IPv6 multicast address
    TCPIP_ANNOUNCE_FIELD_IPV6_DEFAULT_ROUTER,       // the payload is the IPv6 router address
    TCPIP_ANNOUNCE_FIELD_IPV6_DEFAULT_GATEWAY,      // the payload is the IPv6 gateway address
} TCPIP_ANNOUNCE_FIELD_PAYLOAD;


// *****************************************************************************
/* Structure: TCPIP_ANNOUNCE_TRUNC_DCPT

  Summary:
     Announce truncation data structure

  Description:
    This structure is used for the Announce module truncation payload.
*/
typedef struct __attribute__((packed))
{
    uint8_t     type;           // TCPIP_ANNOUNCE_FIELD_TRUNCATED
    char        separator[2];   // CRLF terminator
}TCPIP_ANNOUNCE_TRUNC_DCPT;

// *****************************************************************************
/* Structure: TCPIP_ANNOUNCE_MAC_ADDR_DCPT

  Summary:
     Announce MAC address data structure

  Description:
    This structure is used for the Announce module MAC address payload.
*/
typedef struct __attribute__((packed))
{
    uint8_t     type;       // TCPIP_ANNOUNCE_FIELD_MAC_ADDR
    uint8_t     macAddr[6]; // MAC address, binary
    char        separator[2];   // CRLF terminator
}TCPIP_ANNOUNCE_MAC_ADDR_DCPT;

// *****************************************************************************
/* Structure: TCPIP_ANNOUNCE_MAC_NAME_DCPT

  Summary:
     Announce MAC name data structure

  Description:
    This structure is used for the Announce module MAC name payload.
*/
typedef struct __attribute__((packed))
{
    uint8_t     type;       // TCPIP_ANNOUNCE_FIELD_MAC_NAME
    char        macName[2]; // variable size MAC interface name, ASCII + CRLF separator;
}TCPIP_ANNOUNCE_MAC_NAME_DCPT;


// *****************************************************************************
/* Structure: TCPIP_ANNOUNCE_HOST_NAME_DCPT

  Summary:
     Announce host name data structure

  Description:
    This structure is used for the Announce module host name payload.
*/
typedef struct __attribute__((packed))
{
    uint8_t     type;       // TCPIP_ANNOUNCE_FIELD_HOST_NAME
    char        hostName[2];// variable size interface host name, ASCII + CRLF separator
}TCPIP_ANNOUNCE_HOST_NAME_DCPT;


// *****************************************************************************
/* Structure: TCPIP_ANNOUNCE_IPV4_ADDR_DCPT

  Summary:
     Announce IPv4 address data structure

  Description:
    This structure is used for the Announce module IPv4 address payload.
*/
typedef struct __attribute__((packed))
{
    uint8_t     type;       // TCPIP_ANNOUNCE_FIELD_IPV4_ADDRESS
    uint8_t     ipv4Addr[4]; // IPv4 address, binary
    char        separator[2];   // CRLF terminator
}TCPIP_ANNOUNCE_IPV4_ADDR_DCPT;


// *****************************************************************************
/* Structure: TCPIP_ANNOUNCE_IPV6_ADDR_DCPT

  Summary:
     Announce IPv6 address data structure

  Description:
    This structure is used for the Announce module IPv6 address payload.
*/
typedef struct __attribute__((packed))
{
    uint8_t     type;       // TCPIP_ANNOUNCE_FIELD_IPV6_UNICAST/TCPIP_ANNOUNCE_FIELD_IPV6_MULTICAST/
                            // TCPIP_ANNOUNCE_FIELD_IPV6_DEFAULT_ROUTER/TCPIP_ANNOUNCE_FIELD_IPV6_DEFAULT_GATEWAY
    uint8_t     ipv6Addr[16]; // IPv6 address, binary
    char        separator[2];   // CRLF terminator
}TCPIP_ANNOUNCE_IPV6_ADDR_DCPT;

// *****************************************************************************
/* Structure: TCPIP_ANNOUNCE_USER_START

  Summary:
     Announce user separator data structure

  Description:
    This structure is used for the Announce module user separator payload.
    The extra separator is used only when user data is present.
*/
typedef struct __attribute__((packed))
{
    char        separator[2];   // CRLF terminator
}TCPIP_ANNOUNCE_USER_START;


// *****************************************************************************
/* Structure: TCPIP_ANNOUNCE_MESSAGE

  Summary:
     Announce message data structure

  Description:
    This structure is used for the Announce module messages.
*/
typedef struct __attribute__((packed))
{
    // only if the message is truncated
    TCPIP_ANNOUNCE_TRUNC_DCPT       truncDcpt;
    TCPIP_ANNOUNCE_MAC_ADDR_DCPT    macAddDcpt;
    TCPIP_ANNOUNCE_MAC_NAME_DCPT    macNameDcpt;
    TCPIP_ANNOUNCE_HOST_NAME_DCPT   hostNameDcpt;

    // only when IPv4 is enabled
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT   ipv4AddDcpt;

    // only if IPv6 is enabled
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT   ipv6UnicastDcpt[1];
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT   ipv6MulticastDcpt[1];
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT   ipv6RouterDcpt[1];
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT   ipv6GatewayDcpt[1];

    // only when user data present
    TCPIP_ANNOUNCE_USER_START       userStart;
}TCPIP_ANNOUNCE_MESSAGE;

// *****************************************************************************
/* Enumeration: TCPIP_ANNOUNCE_BROADCAST_TYPE

  Summary:
    Announce message type

  Description:
    Describes the current message types supported by the announce module.
 */
typedef enum
{
    TCPIP_ANNOUNCE_BCAST_NONE,              // invalid, not specified
    TCPIP_ANNOUNCE_BCAST_NET_LIMITED,       // the Announce messages will be sent
                                            // using the IPv4 network limited broadcast type
                                            // i.e. 255.255.255.255
                                            // This is the default
    TCPIP_ANNOUNCE_BCAST_NET_DIRECTED,      // the Announce messages will be sent
                                            // using the IPv4 network directed broadcast type
                                            // i.e. 192.168.1.255, for example
}TCPIP_ANNOUNCE_BROADCAST_TYPE;



// *****************************************************************************
/*
  Type:
    TCPIP_ANNOUNCE_MESSAGE_CALLBACK

  Summary:
    Announce message callback function.

  Description:
    Prototype of an announce callback function that can be registered by the user.

    This callback will be called by the announce module when sending
    a broadcast message.

 Remarks:
    The callback function could use regular UDP calls to add its own data to the message.

    The UDP socket TX buffer has limited space and the standard announce message is in the buffer.
    The amount of data that can be pushed is limited by the TCPIP_ANNOUNCE_MAX_PAYLOAD symbol.

    If the UDP socket TX buffer was not sufficient for storing the internal
    announce message, then the user part will be skipped.
*/

typedef void    (*TCPIP_ANNOUNCE_MESSAGE_CALLBACK)(TCPIP_NET_HANDLE hNet, UDP_SOCKET s);


// *****************************************************************************
/* Structure: TCPIP_ANNOUNCE_MODULE_CONFIG

  Summary:
     Announce Configuration structure placeholder

  Description:
    This structure is used for the Announce module initialization data.
*/
typedef struct
{
    void* reserved;
}TCPIP_ANNOUNCE_MODULE_CONFIG;

// *****************************************************************************
/*
  Function:
    bool  TCPIP_ANNOUNCE_MessageRequest(TCPIP_NET_HANDLE hNet, TCPIP_ANNOUNCE_BROADCAST_TYPE bcastType);

  Summary:
   
  Description:
    This function instructs the module to send an announce message 
    on the specified interface using the selected broadcast type.

    If successful, the message is triggered and will be sent to the output.

  Precondition:
    The Announce module should have been initialized.

  Parameters:
    hNet      - handle of the network to send announce message on
                If 0, the announce message will be sent on all interfaces
    bcastType - type of broadcast to be used
                This type becomes the current broadcast type
                If == TCPIP_ANNOUNCE_BCAST_NONE, the broadcast type is not changed


  Returns:
    true    - if the selected network is valid and the announce message is initiated
    false   - if no such network, or network is down

  Remarks:
    The sending of the message does not occur in the context of this function.
    This function just initiates an announce message request.

*/
bool  TCPIP_ANNOUNCE_MessageRequest(TCPIP_NET_HANDLE hNet, TCPIP_ANNOUNCE_BROADCAST_TYPE bcastType);

// *****************************************************************************
/*
  Function:
    bool  TCPIP_ANNOUNCE_CallbackRegister(TCPIP_ANNOUNCE_MESSAGE_CALLBACK callback);

  Summary:
    Allows the registration of a user callback.

  Description:
    This function registers an application callback.
    The callback will be called when the Announce module needs to send a message.
    The user can insert its own message as part of a standard announce message.

  Precondition:
    The Announce module should have been initialized.

  Parameters:
    callback    - user function to be called by the annopunce module.
                  If callback == 0 then any previous callback will be erased.

  Returns:
    true  -  if the call succeeded.
    false - if failed; module i snot up, for example

  Remarks:
    Only one callback is supported for now.
    To delete the callback simply call the register function with callback = 0.

    The callback function could use regular UDP calls to add its own data to the message.

    If the UDP socket TX buffer was not sufficient for storing the internal
    announce message, then the user part will be skipped.
*/
bool  TCPIP_ANNOUNCE_CallbackRegister(TCPIP_ANNOUNCE_MESSAGE_CALLBACK callback);
    
// *****************************************************************************
/*
  Function:
    void  TCPIP_ANNOUNCE_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs Announce module tasks in the TCP/IP stack.

  Precondition:
    The Announce module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_ANNOUNCE_Task(void);
    
//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif	//#ifndef __TCPIP_ANNOUNCE_H
