/*******************************************************************************
 Source file for the Net Pres Certificate Store functions to work with Harmony


  Summary:


  Description:

*******************************************************************************/

/*******************************************************************************
File Name: net_pres_cert_stroe.c
Copyright (c) 2015 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND,
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
#include "net/pres/net_pres_certstore.h"
<#if CONFIG_NET_PRES_BLOB_CERT_REPO>
    <#if CONFIG_NET_PRES_BLOB_CLIENT_SUPPORT>
#define USE_CERT_BUFFERS_2048
#include "${CONFIG_NET_PRES_BLOB_CLIENT_CERT_FILENAME}"
    </#if>
    <#if CONFIG_NET_PRES_BLOB_SERVER_SUPPORT>
#define USE_CERT_BUFFERS_2048
#include "${CONFIG_NET_PRES_BLOB_SERVER_CERT_FILENAME}"
#include "${CONFIG_NET_PRES_BLOB_SERVER_KEY_FILENAME}"
    </#if>
</#if>

<#if CONFIG_NET_PRES_CERT_STORE_STUBS>
    <#if CONFIG_NET_PRES_CERT_STORE_STUBS_CLIENT>
bool NET_PRES_CertStoreGetCACerts(const uint8_t ** certPtr, int32_t * certSize, uint8_t certIndex)
{
    return false;
}
    </#if>
    <#if CONFIG_NET_PRES_CERT_STORE_STUBS_SERVER>
bool NET_PRES_CertStoreGetServerCert(const uint8_t ** serverCertPtr, int32_t * serverCertSize, const uint8_t ** serverKeyPtr, int32_t * serverKeySize, uint8_t certIndex)
{
    return false;
}
    </#if>
</#if>
<#if CONFIG_NET_PRES_BLOB_CERT_REPO>
    <#if CONFIG_NET_PRES_BLOB_CLIENT_SUPPORT>
bool NET_PRES_CertStoreGetCACerts(const uint8_t ** certPtr, int32_t * certSize, uint8_t certIndex)
{
    *certPtr = ${CONFIG_NET_PRES_BLOB_CLIENT_CERT_VARIABLE};
    *certSize = ${CONFIG_NET_PRES_BLOB_CLIENT_CERT_LEN_VARIABLE};
    return true;
}
    </#if>
    <#if CONFIG_NET_PRES_BLOB_SERVER_SUPPORT>
bool NET_PRES_CertStoreGetServerCert(const uint8_t ** serverCertPtr, int32_t * serverCertSize, const uint8_t ** serverKeyPtr, int32_t * serverKeySize, uint8_t certIndex)
{
    *serverCertPtr = ${CONFIG_NET_PRES_BLOB_SERVER_CERT_VARIABLE};
    *serverCertSize = ${CONFIG_NET_PRES_BLOB_SERVER_CERT_LEN_VARIABLE};
    *serverKeyPtr = ${CONFIG_NET_PRES_BLOB_SERVER_KEY_VARIABLE};
    *serverKeySize = ${CONFIG_NET_PRES_BLOB_SERVER_KEY_LEN_VARIABLE};
    return true;
}
    </#if>
</#if>