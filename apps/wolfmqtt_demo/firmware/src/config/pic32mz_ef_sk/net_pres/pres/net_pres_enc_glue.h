 /*******************************************************************************
 Header file for the wolfSSL glue functions to work with Harmony


  Summary:


  Description:

*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

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


#ifndef _NET_TLS_WOLFSSL_GLUE_H_
#define _NET_TLS_WOLFSSL_GLUE_H_

#include "configuration.h"
#include "net_pres/pres/net_pres.h"
#include "net_pres/pres/net_pres_encryptionproviderapi.h"
#ifdef __CPLUSPLUS
extern "C" {
#endif
extern NET_PRES_EncProviderObject net_pres_EncProviderStreamServer0;
extern NET_PRES_EncProviderObject net_pres_EncProviderStreamClient0;
bool NET_PRES_EncProviderStreamServerInit0(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderStreamServerDeinit0();
bool NET_PRES_EncProviderStreamServerOpen0(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderStreamServerIsInited0();
bool NET_PRES_EncProviderStreamClientInit0(struct _NET_PRES_TransportObject * transObject);
bool NET_PRES_EncProviderStreamClientDeinit0();
bool NET_PRES_EncProviderStreamClientOpen0(uintptr_t transHandle, void * providerData);
bool NET_PRES_EncProviderStreamClientIsInited0();
NET_PRES_EncSessionStatus NET_PRES_EncProviderServerAccept0(void * providerData);
NET_PRES_EncSessionStatus NET_PRES_EncProviderClientConnect0(void * providerData);
NET_PRES_EncSessionStatus NET_PRES_EncProviderConnectionClose0(void * providerData);
int32_t NET_PRES_EncProviderWrite0(void * providerData, const uint8_t * buffer, uint16_t size);
uint16_t  NET_PRES_EncProviderWriteReady0(void * providerData, uint16_t reqSize, uint16_t minSize);
int32_t NET_PRES_EncProviderRead0(void * providerData, uint8_t * buffer, uint16_t size);
int32_t NET_PRES_EncProviderReadReady0(void * providerData);
int32_t NET_PRES_EncProviderPeek0(void * providerData, uint8_t * buffer, uint16_t size);
int32_t NET_PRES_EncProviderOutputSize0(void * providerData, int32_t inSize);
int32_t NET_PRES_EncProviderMaxOutputSize0(void * providerData);
#ifdef __CPLUSPLUS
}
#endif
#endif //_NET_TLS_WOLFSSL_GLUE_H_
