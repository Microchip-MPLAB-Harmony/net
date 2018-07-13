/*******************************************************************************
  IPV4 private manager API for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ipv4_manager.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _IPV4_MANAGER_H_
#define _IPV4_MANAGER_H_


// Stack structures

// IP Pseudo header as defined by RFC 793 (needed for TCP and UDP 
// checksum calculations/verification)
typedef struct
{
    IPV4_ADDR SourceAddress;
    IPV4_ADDR DestAddress;
    uint8_t Zero;
    uint8_t Protocol;
    uint16_t Length;
} IPV4_PSEUDO_HEADER;


// stack private API
// 
   

bool TCPIP_IPV4_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, const TCPIP_IPV4_MODULE_CONFIG* pIpInit);
void TCPIP_IPV4_DeInitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit);


// Interface functions


// prototype of a IPv4 filter function/handler
// stack modules can register a packet filter with the IPv4
// Returns true if the module filter wants the packet to be accepted
// (the IPv4 will comply)
// Otherwise the packet is subject to standard IPv4 filtering mechanism
// (matching IP interface address, broadcast, multicast, etc.)
typedef bool    (*IPV4_FILTER_FUNC)(TCPIP_MAC_PACKET* pRxPkt, const void* param);

// a handle that a client can use
// after the filter handler has been registered
typedef const void* IPV4_FILTER_HANDLE;

// Register an IPv4 filter handler
// Returns a valid handle if the call succeeds,
// or a null handle if the call failed.
// Function has to be called after the IPv4 is initialized
// The hParam is passed by the client and will be used by the IPv4
// when the filter call is made.
IPV4_FILTER_HANDLE      IPv4RegisterFilter(IPV4_FILTER_FUNC handler, const void* hParam);

// deregister the filter handler
// returns true or false if no such handler registered
bool                    Ipv4DeRegisterFilter(IPV4_FILTER_HANDLE hFilter);


// helper to transform a RX packet to a TX packet
// IPv4 header:
//          - source and destination addresses are switched
//          - total length and fragment info are converted to network order
//          - data segment is re-adjusted with the IPv4 header length
// MAC header:
//          - source and destination addresses are switched
//          - data segment is re-adjusted with the MAC header length
//
void                    TCPIP_IPV4_MacPacketSwitchTxToRx(TCPIP_MAC_PACKET* pRxPkt);


// 

#endif // _IPV4_MANAGER_H_



