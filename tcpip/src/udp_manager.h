/*******************************************************************************
  UDP Module manager - private stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    udp_manager.h
    
  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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








// DOM-IGNORE-END

#ifndef __UDP__MANAGER_H_
#define __UDP__MANAGER_H_


bool TCPIP_UDP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, const TCPIP_UDP_MODULE_CONFIG* pUdpInit);
void TCPIP_UDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit);


// BSD sockets support
typedef enum
{
    UDP_OPEN_SERVER         = 0x01,        // create a server socket
    UDP_OPEN_CLIENT         = 0x02,        // create a client socket
    // extra creation flags
    UDP_OPEN_TX_SPLIT       = 0x80,        // create a ZC BSD socket, the payload is external
                                           // No PUT operations available for this socket
    UDP_OPEN_CONFIG_SERVICE = 0x100,       // the owner of this socket is a stack configuration service

}UDP_OPEN_TYPE;

// Stores the header of a UDP packet
typedef struct
{
    UDP_PORT    SourcePort;             // Source UDP port
    UDP_PORT    DestinationPort;        // Destination UDP port
    uint16_t    Length;                 // Length of data
    uint16_t    Checksum;               // UDP checksum of the data
} UDP_HEADER;


UDP_SOCKET TCPIP_UDP_OpenServerSkt(IP_ADDRESS_TYPE addType, UDP_PORT localPort,  IP_MULTI_ADDRESS* localAddress, UDP_OPEN_TYPE opType);


UDP_SOCKET TCPIP_UDP_OpenClientSkt(IP_ADDRESS_TYPE addType, UDP_PORT remotePort, IP_MULTI_ADDRESS* remoteAddress, UDP_OPEN_TYPE opType);



// sets the payload for a TX_SPLIT/BSD created IPv4 socket
bool TCPIP_UDP_SetSplitPayload(UDP_SOCKET s, void* pLoad, uint16_t loadSize);


// returns the current write pointer into the socket TX buffer
// note that TCPIP_UDP_PutIsReady() should return not 0 
// use TCPIP_UDP_TxOffsetSet() to set the write pointer
// where is needed before and after this call
uint8_t*    TCPIP_UDP_TxPointerGet(UDP_SOCKET s);


#endif // __UDP__MANAGER_H_


