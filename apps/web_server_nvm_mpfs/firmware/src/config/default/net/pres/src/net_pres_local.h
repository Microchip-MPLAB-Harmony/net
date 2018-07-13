#ifndef _NET_PRES_LOCAL_H_
#define _NET_PRES_LOCAL_H_

#include "../net_pres.h"
#include "osal/osal.h"
#include "../net_pres_transportapi.h"
#include "../net_pres_encryptionproviderapi.h"

#ifdef __CPLUSPLUS
extern "C" {
#endif
    
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
        bool inUse;
        NET_PRES_SKT_ERROR_T lastError;
        NET_PRES_SKT_T socketType;
        uint16_t transHandle;
        uint8_t providerData[8];
        NET_PRES_EncSessionStatus status;
        NET_PRES_TransportObject * transObject;
        NET_PRES_EncProviderObject * provObject;
    }NET_PRES_SocketData;
    
#ifdef __CPLUSPLUS
}
#endif

#endif
