/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef _NET_PRES_LOCAL_H_
#define _NET_PRES_LOCAL_H_

#include "../net_pres.h"
#include "osal/osal.h"
#include "../net_pres_transportapi.h"
#include "../net_pres_encryptionproviderapi.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif

// NET_PRES debugging levels/masks
// basic debugging
#define NET_PRES_DEBUG_MASK_BASIC           (0x0001)

// enableNET_PRES debugging levels
#define NET_PRES_DEBUG_LEVEL  (0)


    
    
    typedef struct _NET_PRES_InternalData
    {
        bool initialized;
        OSAL_MUTEX_HANDLE_TYPE presMutex;
        uint8_t numLayers;
        NET_PRES_TransportObject transObjectSS[NET_PRES_NUM_INSTANCE];
        NET_PRES_TransportObject transObjectSC[NET_PRES_NUM_INSTANCE];
        NET_PRES_TransportObject transObjectDS[NET_PRES_NUM_INSTANCE];
        NET_PRES_TransportObject transObjectDC[NET_PRES_NUM_INSTANCE];
        NET_PRES_EncProviderObject encProvObjectSS[NET_PRES_NUM_INSTANCE];
        NET_PRES_EncProviderObject encProvObjectSC[NET_PRES_NUM_INSTANCE];
        NET_PRES_EncProviderObject encProvObjectDS[NET_PRES_NUM_INSTANCE];
        NET_PRES_EncProviderObject encProvObjectDC[NET_PRES_NUM_INSTANCE];
    }NET_PRES_InternalData;
    
    typedef struct _NET_PRES_SocketData
    {
        uint8_t     inUse;
        uint8_t     provOpen;
        uint8_t     status;     // NET_PRES_EncSessionStatus value
        int8_t      lastError;  // NET_PRES_SKT_ERROR_T value
        uint16_t    socketType; // NET_PRES_SKT_T value
        int16_t     transHandle;
        NET_PRES_TransportObject * transObject;
        NET_PRES_EncProviderObject * provObject;
        NET_PRES_SIGNAL_HANDLE    sigHandle;    // registered signal handle
        NET_PRES_SIGNAL_FUNCTION  usrSigFnc;    // user signal function
        const void*               usrSigParam;  // user signal parameter
        uint8_t providerData[8];
    }NET_PRES_SocketData;
    
#ifdef __CPLUSPLUS
}
#endif

#endif
