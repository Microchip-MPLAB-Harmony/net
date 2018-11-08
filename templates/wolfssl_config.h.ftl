<#--
/*******************************************************************************
  Application Header

  File Name:
  wolfssl_config.h.ftl

  Summary:
 config file for wolfSSL to avoid custom build options

  Description:
 config file for wolfSSL to avoid custom build options
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2018 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
//DOM-IGNORE-END

-->
<#if wolfssl>
/*** wolfSSL Library Configuration ***/
#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#if defined(INLINE)
#undef INLINE
#define INLINE inline
#endif

#define MICROCHIP_MPLAB_HARMONY
#define WOLFSSL_IGNORE_FILE_WARN

#define CONFIG_USE_3RDPARTY_WOLFSSL
#define MICROCHIP_PIC32
#define MICROCHIP_PIC32_RNG
#define WC_NO_HARDEN
#define NO_DSA
#define HAVE_MCAPI

#define NEED_AES_TABLES

#define SIZEOF_LONG_LONG 8
#define WOLFSSL_USER_IO
#define NO_WRITEV
#define NO_DEV_RANDOM
#define NO_FILESYSTEM
#define WOLFSSL_STATIC_RSA

<#if wolfsslMULTITHREAD == false>
#define SINGLE_THREADED
</#if>
<#if wolfsslFASTMATH>
#define USE_FAST_MATH
</#if>
#define TFM_TIMING_RESISTANT
<#if !wolfsslClient>
#define NO_WOLFSSL_CLIENT
</#if>
<#if !wolfsslServer>
#define NO_WOLFSSL_SERVER
</#if>
<#if !wolfsslDES3>
#define NO_DES3
</#if>
<#if !wolfsslDH>
#define NO_DH
</#if>
<#if wolfsslAESCTR == true>
#define WOLFSSL_AES_COUNTER
</#if>
<#if wolfsslAESCTR == false && wolfsslAESCBC == false && wolfsslAESCCM == false && wolfsslAESGCM == false>
#define NO_AES
</#if>
<#if !wolfsslDSA>
#define NO_DSA
</#if>
<#if !wolfsslErrStr>
#define NO_ERROR_STRINGS
</#if>
<#if !wolfsslHMAC>
#define NO_HMAC
</#if>
<#if !wolfsslMD4>
#define NO_MD4
</#if>
<#if !wolfsslSHA256>
#define NO_SHA256
</#if>
<#if !wolfsslPSK>
#define NO_PSK
</#if>
<#if !wolfsslPWDBASED>
#define NO_PWDBASED
</#if>
<#if !wolfsslRC4>
#define NO_RC4
</#if>
<#if !wolfsslRABBIT>
#define NO_RABBIT
</#if>
<#if !wolfsslHC128>
#define NO_HC128
</#if>
<#if !wolfsslCache>
#define NO_SESSION_CACHE
</#if>
<#if !wolfsslTLS>
#define NO_TLS
</#if>
<#if wolfsslSmallCache>
#define SMALL_SESSION_CACHE
</#if>
<#if wolfsslCertGen>
#define WOLFSSL_CERT_GEN
</#if>
<#if wolfsslDERLoad>
#define WOLFSSL_DER_LOAD
</#if>
<#if wolfsslDTLS>
#define WOLFSSL_DTLS
</#if>
<#if wolfsslRSAKeyGen>
#define WOLFSSL_KEY_GEN
</#if>
<#if wolfsslRIPEMD>
#define WOLFSSL_RIPEMD
</#if>
<#if wolfsslSHA384>
#define WOLFSSL_SHA384
</#if>
<#if wolfsslSHA512>
#define WOLFSSL_SHA512
</#if>
<#if wolfsslAESCCM>
#define HAVE_AESCCM
</#if>
<#if wolfsslAESGCM>
#define HAVE_AESGCM
</#if>
<#if wolfsslCAMELLIA>
#define HAVE_CAMELLIA
</#if>
<#if wolfsslCRL>
#define HAVE_CRL
</#if>
<#if wolfsslECC>
#define HAVE_ECC
</#if>
<#if wolfsslCompress>
#define HAVE_LIBZ
</#if>
<#if wolfsslOCSP>
#define HAVE_OCSP
</#if>
<#if wolfsslOpenSSL_API>
#define OPENSSL_EXTRA
</#if>

<#if wolfsslDebug>
#define DEBUG_WOLFSSL
</#if>

<#if !wolfsslOldTLS>
#define NO_OLD_TLS
</#if>

<#if wolfsslTLS13>
#define WOLFSSL_TLS13
#define HAVE_TLS_EXTENSIONS
</#if>


<#if wolfsslMQTT == true>
#define WOLFMQTT_NONBLOCK
<#if wolfsslMQTTTLS == true>
#define ENABLE_MQTT_TLS
</#if>
#ifdef	__cplusplus
}
#endif
</#if>

<#-- CONFIG_H --> 
#endif
<#-- end if wolfssl --> 
</#if>

<#-- 
/*******************************************************************************
 End of File
*/
-->