/*
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
