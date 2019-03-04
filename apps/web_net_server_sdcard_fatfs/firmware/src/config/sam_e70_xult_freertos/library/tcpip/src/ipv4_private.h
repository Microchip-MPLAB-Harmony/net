/*******************************************************************************
  IPv4 private API for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ipv4_private.h

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

#ifndef _IPV4_PRIVATE_H_
#define _IPV4_PRIVATE_H_

// internal

// debugging
#define TCPIP_IPV4_DEBUG_MASK_BASIC           (0x0001)
#define TCPIP_IPV4_DEBUG_MASK_FRAGMENT        (0x0002)
#define TCPIP_IPV4_DEBUG_MASK_RX_CHECK        (0x0004)

// enable IPV4 debugging levels
#define TCPIP_IPV4_DEBUG_LEVEL  (TCPIP_IPV4_DEBUG_MASK_BASIC | TCPIP_IPV4_DEBUG_MASK_RX_CHECK)


// IPv4 packet filter registration

typedef struct  _TAG_IPV4_FILTER_LIST_NODE
{
	struct _TAG_IPV4_FILTER_LIST_NODE*  next;       // next node in list
                                                    // makes it valid SGL_LIST_NODE node
    IPV4_FILTER_FUNC                    handler;    // handler to be called for the filter
    const void*                         hParam;     // handler parameter
}IPV4_FILTER_LIST_NODE;



// IPv4 fragment reassembly

typedef struct _TAG_IPV4_FRAGMENT_NODE
{
    struct _TAG_IPV4_FRAGMENT_NODE* next;       // next fragment node: ipv4FragmentQueue 
    TCPIP_MAC_PACKET*               fragHead;   // head fragments list; connected with pkt_next;
    uint32_t                        fragTStart; // fragment occurring tick 
    uint16_t                        nFrags;     // number of fragments in this node
    uint16_t                        fragTmo;    // fragment expiration timeout, seconds
    
}IPV4_FRAGMENT_NODE;


// IPv4 fragment transmission
typedef struct
{
    TCPIP_MAC_PACKET        macPkt;         // standard MAC packet containing the MAC + IPv4 headers
                                            // always on 1st position to safe cast to SGL_LIST_NODE!
    TCPIP_MAC_DATA_SEGMENT  fragSeg;        // segment carying the IPv4 payload 
                                            // allows zc functionality
}IPV4_FRAG_TX_PKT;                                           


typedef enum
{
    IPV4_FRAG_TX_ACK_HEAD   = 0x01,         // acknowledge the head of a frgmented TX packet
    IPV4_FRAG_TX_ACK_FRAGS  = 0x02,         // acknowledge the fragments within a frgmented TX packet

}IPV4_FRAG_TX_ACK;


// IPv4 fixed options

// mask of internal supported options
// multiple can be set
// 16 bits only
typedef enum
{
    IPV4_OPTION_MASK_NONE           = 0x0000,
    IPV4_OPTION_MASK_ROUTER_ALERT   = 0x0001,

    // other options will be added here
}IPV4_OPTION_TYPE_MASK;


#define IPV4_ROUTER_ALERT_OPTION_SIZE       4   // fixed option requiring 4 bytes
#define IPV4_ROUTER_ALERT_OPTION_COPIED     1 
#define IPV4_ROUTER_ALERT_OPTION_CLASS      0
#define IPV4_ROUTER_ALERT_OPTION_DATA       0
typedef struct
{
    struct
    {   // option type
        uint8_t     optNumber:  5;      // TCPIP_IPV4_OPTION_ROUTER_ALERT
        uint8_t     optClass:   2;      // option class: 0 - control; 2 - debugging and measurement; 1, 3 - reserved
        uint8_t     optCopied:  1;      // option copied to all fragments
    };
    uint8_t         optLength;          // size of the entire option
    uint16_t        optValue;           // option value
}IPV4_ROUTER_ALERT_OPTION;


// maximum size of options in an IPv4 packet
#define IPV4_OPTIONS_MAXIMUM_SIZE           40  // 40 bytes + 20 bytes header == 60 bytes maximum packet

// maximum size ofIPv4 header
#define IPV4_HEADER_MAXIMUM_SIZE            60  // 20 bytes header + 40 bytes max options

#endif // _IPV4_PRIVATE_H_



