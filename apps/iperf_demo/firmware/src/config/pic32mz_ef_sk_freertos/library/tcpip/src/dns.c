/*******************************************************************************
  Domain Name System (DNS) Client 
  Module for Microchip TCP/IP Stack

  Summary:
    DNS client implementation file
    
  Description:
    This source file contains the functions of the 
    DNS client routines
    
    Provides  hostname to IP address translation
    Reference: RFC 1035
*******************************************************************************/

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

#include "tcpip/src/tcpip_private.h"

bool TCPIP_DNS_IsEnabled(TCPIP_NET_HANDLE hNet){return false;}
bool TCPIP_DNS_Enable(TCPIP_NET_HANDLE hNet, TCPIP_DNS_ENABLE_FLAGS flags){return false;}
bool TCPIP_DNS_Disable(TCPIP_NET_HANDLE hNet, bool clearCache){return false;}

TCPIP_DNS_RESULT  TCPIP_DNS_Resolve(const char* HostName, TCPIP_DNS_RESOLVE_TYPE Type)
{
    return TCPIP_DNS_RES_NO_SERVICE; 
}

TCPIP_DNS_RESULT  TCPIP_DNS_IsResolved(const char* hostName, IP_MULTI_ADDRESS* hostIP, IP_ADDRESS_TYPE type)
{
    return TCPIP_DNS_RES_NO_SERVICE; 
}

TCPIP_DNS_RESULT  TCPIP_DNS_IsNameResolved(const char* hostName, IPV4_ADDR* hostIPv4, IPV6_ADDR* hostIPv6)
{
    return TCPIP_DNS_RES_NO_SERVICE; 
}

TCPIP_DNS_RESULT TCPIP_DNS_RemoveEntry(const char *hostName)
{
    return TCPIP_DNS_RES_NO_SERVICE;
}

TCPIP_DNS_RESULT TCPIP_DNS_RemoveAll(void)
{
    return TCPIP_DNS_RES_NO_SERVICE;
}

TCPIP_DNS_RESULT TCPIP_DNS_ClientInfoGet(TCPIP_DNS_CLIENT_INFO* pClientInfo)
{
    return TCPIP_DNS_RES_NO_SERVICE;
}

TCPIP_DNS_RESULT TCPIP_DNS_EntryQuery(TCPIP_DNS_ENTRY_QUERY *pDnsQuery, int queryIndex)
{
    return TCPIP_DNS_RES_NO_SERVICE;
}


