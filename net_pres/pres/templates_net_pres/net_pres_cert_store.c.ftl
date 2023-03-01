<#--
Copyright (C) 2015-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
--> /*******************************************************************************
 Source file for the Net Pres Certificate Store functions to work with Harmony


  Summary:


  Description:

*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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

#include "net_pres/pres/net_pres_certstore.h"
<#if NET_PRES_BLOB_CERT_REPO>
    <#if NET_PRES_BLOB_CLIENT_SUPPORT>
#define USE_CERT_BUFFERS_2048
#include "${NET_PRES_BLOB_CLIENT_CERT_FILENAME}"
        <#if NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT>
            <#if NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_FILENAME != NET_PRES_BLOB_CLIENT_CERT_FILENAME>
#include "${NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_FILENAME}"
            </#if>
        </#if>
    </#if>
    <#if NET_PRES_BLOB_SERVER_SUPPORT>
#define USE_CERT_BUFFERS_2048
#include "${NET_PRES_BLOB_SERVER_CERT_FILENAME}"
#include "${NET_PRES_BLOB_SERVER_KEY_FILENAME}"
    </#if>
</#if>

<#if NET_PRES_CERT_STORE_STUBS>
    <#if NET_PRES_CERT_STORE_STUBS_CLIENT>
bool NET_PRES_CertStoreGetCACerts(const uint8_t ** certPtr, int32_t * certSize, uint8_t certIndex)
{
    return false;
}
        <#if NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT>
bool NET_PRES_CertStoreGetDeviceTlsParams(const uint8_t ** certPtr, int32_t * certSize, const uint8_t ** pvtKeyPtr, int32_t * pvtKeySize, uint8_t certIndex)
{
    return false;
}
        </#if>
    </#if>
    <#if NET_PRES_CERT_STORE_STUBS_SERVER>
bool NET_PRES_CertStoreGetServerCert(const uint8_t ** serverCertPtr, int32_t * serverCertSize, const uint8_t ** serverKeyPtr, int32_t * serverKeySize, uint8_t certIndex)
{
    return false;
}
    </#if>
</#if>
<#if NET_PRES_BLOB_CERT_REPO>
    <#if NET_PRES_BLOB_CLIENT_SUPPORT>
bool NET_PRES_CertStoreGetCACerts(const uint8_t ** certPtr, int32_t * certSize, uint8_t certIndex)
{
    *certPtr = ${NET_PRES_BLOB_CLIENT_CERT_VARIABLE};
    *certSize = ${NET_PRES_BLOB_CLIENT_CERT_LEN_VARIABLE};
    return true;
}
        <#if NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT>
bool NET_PRES_CertStoreGetDeviceTlsParams(const uint8_t ** certPtr, int32_t * certSize, const uint8_t ** pvtKeyPtr, int32_t * pvtKeySize, uint8_t certIndex)
{
    *certPtr = ${NET_PRES_BLOB_CLIENT_DEVICE_CERT_VARIABLE};
    *certSize = ${NET_PRES_BLOB_CLIENT_DEVICE_CERT_LEN_VARIABLE};
    *pvtKeyPtr = ${NET_PRES_BLOB_CLIENT_DEVICE_PVT_KEY_VARIABLE};
    *pvtKeySize = ${NET_PRES_BLOB_CLIENT_DEVICE_PVT_KEY_LEN_VARIABLE};
    return true;
}
        </#if>
    </#if>
    <#if NET_PRES_BLOB_SERVER_SUPPORT>
bool NET_PRES_CertStoreGetServerCert(const uint8_t ** serverCertPtr, int32_t * serverCertSize, const uint8_t ** serverKeyPtr, int32_t * serverKeySize, uint8_t certIndex)
{
    *serverCertPtr = ${NET_PRES_BLOB_SERVER_CERT_VARIABLE};
    *serverCertSize = ${NET_PRES_BLOB_SERVER_CERT_LEN_VARIABLE};
    *serverKeyPtr = ${NET_PRES_BLOB_SERVER_KEY_VARIABLE};
    *serverKeySize = ${NET_PRES_BLOB_SERVER_KEY_LEN_VARIABLE};
    return true;
}
    </#if>
</#if>
