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
typedef struct __attribute__((aligned(2), packed))
{
    UDP_PORT    SourcePort;				// Source UDP port
    UDP_PORT    DestinationPort;		// Destination UDP port
    uint16_t    Length;					// Length of data
    uint16_t    Checksum;				// UDP checksum of the data
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


