/*******************************************************************************
  IPv6 private API for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ipv6_private.h

  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END

#ifndef H_IPV6_PRIVATE_H_
#define H_IPV6_PRIVATE_H_

// data segment header with full data
typedef struct
{
    IPV6_DATA_SEGMENT_HEADER    segHdr;
    uint32_t   data[1];    // Optional buffer space
}IPV6_DATA_SEGMENT_HEADER_FULL;

// Index of address selection rules for IPv6 default address selection
typedef enum
{
    ADDR_INVALID = 0,
    ADDR_USER_SPECIFIED,
    ADDR_INVALID_ADDRESS_SPECIFIED,
    ADDR_UNDEFINED,
    ADDR_STILL_VALID,
    ADDR_SEL_RULE_1,
    ADDR_SEL_RULE_2,
    ADDR_SEL_RULE_3,
    ADDR_SEL_RULE_4,
    ADDR_SEL_RULE_5,
    ADDR_SEL_RULE_6,
    ADDR_SEL_RULE_7,
    ADDR_SEL_RULE_8,
    ADDR_SEL_RULE_9,
    ADDR_SEL_RULE_10,
} IPV6_ADDR_SEL_INDEX;


// IPV6 event registration
typedef struct  S_TAG_IPV6_LIST_NODE
{
    struct S_TAG_IPV6_LIST_NODE*     next;       // next node in list
                                                // makes it valid SGL_LIST_NODE node
    IPV6_EVENT_HANDLER              handler;    // handler to be called for event
    const void*                     hParam;     // handler parameter
    TCPIP_NET_HANDLE                hNet;       // interface that's registered for
                                                // 0 if all
}IPV6_LIST_NODE;


// IPv6 ULA state machine
typedef enum
{
    TCPIP_IPV6_ULA_IDLE,
    TCPIP_IPV6_ULA_PARAM_SET,
    TCPIP_IPV6_ULA_CONN_START,
    TCPIP_IPV6_ULA_TSTAMP_GET,

}TCPIP_IPV6_ULA_STATE;

// VTF 32 bit value in the IPV6_HEADER
typedef union
{
    uint32_t    val;   // compressed Version, Traffic class and Flow Label
    struct
    {
        unsigned int flow_label     : 20;
        unsigned int ecn            : 2;
        unsigned int ds             : 6;
        unsigned int version        : 4;
    };
}IPV6_VTF;

#endif // H_IPV6_PRIVATE_H_



