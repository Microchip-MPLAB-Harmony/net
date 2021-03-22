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
// hdrlen is the length of the IPv4 header
// needed because the packet pTransportLayer and segLen are not updated yet!
// pNetLayer is valid only! 
// The IPV4_HEADER is NOT converted to host endianess!
typedef bool    (*IPV4_FILTER_FUNC)(TCPIP_MAC_PACKET* pRxPkt, uint8_t hdrlen);

// a handle that a client can use
// after the filter handler has been registered
typedef const void* IPV4_FILTER_HANDLE;

// Register an IPv4 filter handler
// Returns a valid handle if the call succeeds,
// or a null handle if the call failed.
// Function has to be called after the IPv4 is initialized
// active specifies if the filter is active or not
IPV4_FILTER_HANDLE      IPv4RegisterFilter(IPV4_FILTER_FUNC handler, bool active);

// activates/de-activates an IPv4 filter
bool                    Ipv4FilterSetActive(IPV4_FILTER_HANDLE hFilter, bool active);

// deregister the filter handler
// returns true or false if no such handler registered
bool                    Ipv4DeRegisterFilter(IPV4_FILTER_HANDLE hFilter);


// helper to transform a RX packet to a TX packet
// IPv4 header:
//          - the destination addresses is set as the packet source address
//          - the source address is the IP address of the coresponding packet interface (which should be set!) 
//          - total length and fragment info are converted to network order
//          - data segment is re-adjusted with the IPv4 header length
// setChecksum:
//          - if true, the IPv4 header checksum is updated for the IPv4 header
// if setMac == true, then the MAC header is adjusted too: 
//          - the destination addresses is set as the MAC packet source address
//          - the source address is the MAC address of the coresponding packet interface (which should be set!) 
//          - data segment is re-adjusted with the MAC header length
//
void                    TCPIP_IPV4_MacPacketSwitchTxToRx(TCPIP_MAC_PACKET* pRxPkt, bool setChecksum, bool setMac);

// helper to transmit a TX packet
// isPersistent specifies that pPkt is persistent and can be queued,
// if needed, for ARP operations
// Otherwise, the pMacPkt will be used if ARP queuing needed
bool TCPIP_IPV4_PktTx(IPV4_PACKET* pPkt, TCPIP_MAC_PACKET* pMacPkt, bool isPersistent);

#endif // _IPV4_MANAGER_H_



