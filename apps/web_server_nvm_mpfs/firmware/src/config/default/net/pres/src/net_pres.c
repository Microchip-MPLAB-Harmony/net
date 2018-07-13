#include "../net_pres.h"
#include "../net_pres_encryptionproviderapi.h"
#include "../net_pres_socketapi.h"
#include "../net_pres_transportapi.h"
#include "net_pres_local.h"

NET_PRES_InternalData sNetPresData;
NET_PRES_SocketData sNetPresSockets[NET_PRES_NUM_SOCKETS];

SYS_MODULE_OBJ NET_PRES_Initialize( const SYS_MODULE_INDEX index,
                                           const SYS_MODULE_INIT * const init )
{
    if (sNetPresData.initialized || !init)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    NET_PRES_INIT_DATA *pInitData = (NET_PRES_INIT_DATA*)init;
    
    if (pInitData->numLayers > NET_PRES_NUM_INSTANCE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    

    memset(&sNetPresData, 0, sizeof(NET_PRES_InternalData));
    if (OSAL_MUTEX_Create(&sNetPresData.presMutex) != OSAL_RESULT_TRUE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    memset(&sNetPresSockets, 0, sizeof(NET_PRES_SocketData) * NET_PRES_NUM_SOCKETS);
    sNetPresData.initialized = true;
    sNetPresData.numLayers = pInitData->numLayers;
    uint8_t x;
    for (x = 0; x < sNetPresData.numLayers; x++)
    {
        if (pInitData->pInitData[x].pTransObject_ss)
        {
            memcpy(&sNetPresData.transObjectSS[x], pInitData->pInitData[x].pTransObject_ss, sizeof(NET_PRES_TransportObject));
        }
        if (pInitData->pInitData[x].pTransObject_sc)
        {
            memcpy(&sNetPresData.transObjectSC[x], pInitData->pInitData[x].pTransObject_sc, sizeof(NET_PRES_TransportObject));
        }
        if (pInitData->pInitData[x].pTransObject_ds)
        {
            memcpy(&sNetPresData.transObjectDS[x], pInitData->pInitData[x].pTransObject_ds, sizeof(NET_PRES_TransportObject));
        }
        if (pInitData->pInitData[x].pTransObject_dc)
        {
            memcpy(&sNetPresData.transObjectDC[x], pInitData->pInitData[x].pTransObject_dc, sizeof(NET_PRES_TransportObject));
        }
        if (pInitData->pInitData[x].pProvObject_ss)
        {
            memcpy(&sNetPresData.encProvObjectSS[x], pInitData->pInitData[x].pProvObject_ss, sizeof(NET_PRES_EncProviderObject));
        }        
        if (pInitData->pInitData[x].pProvObject_sc)
        {
            memcpy(&sNetPresData.encProvObjectSC[x], pInitData->pInitData[x].pProvObject_sc, sizeof(NET_PRES_EncProviderObject));
        }        
        if (pInitData->pInitData[x].pProvObject_ds)
        {
            memcpy(&sNetPresData.encProvObjectDS[x], pInitData->pInitData[x].pProvObject_ds, sizeof(NET_PRES_EncProviderObject));
        }        
        if (pInitData->pInitData[x].pProvObject_dc)
        {
            memcpy(&sNetPresData.encProvObjectDC[x], pInitData->pInitData[x].pProvObject_dc, sizeof(NET_PRES_EncProviderObject));
        }        
    }
    return (SYS_MODULE_OBJ)&sNetPresData;
}

void NET_PRES_Deinitialize(SYS_MODULE_OBJ obj)
{
    if (!sNetPresData.initialized)
    {
        return;
    }
    
    uint8_t x;
    // Make sure all the sockets are closed down
    for (x = 0; x < NET_PRES_NUM_SOCKETS; x++)
    {
        if (sNetPresSockets[x].inUse)
        {
            if ((sNetPresSockets[x].socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED)
            {
                NET_PRES_EncProviderConnectionClose fpClose = sNetPresSockets[x].provObject->fpClose;
                NET_PRES_TransClose fpTransClose = sNetPresSockets[x].transObject->fpClose;
                if (fpClose != NULL)
                {
                    (*fpClose)(sNetPresSockets[x].providerData);
                }
                if (fpTransClose)
                {
                    (*fpTransClose)(sNetPresSockets[x].transHandle);
                }
                sNetPresSockets[x].inUse = false;
            }
        }
    }
    
    // Make sure all the encryption providers are down
    for (x = 0; x < NET_PRES_NUM_INSTANCE; x++)
    {
        if (sNetPresData.encProvObjectSS[x].fpDeinit != NULL)
        {
            (*sNetPresData.encProvObjectSS[x].fpDeinit)();
        }
        if (sNetPresData.encProvObjectSC[x].fpDeinit != NULL)
        {
            (*sNetPresData.encProvObjectSC[x].fpDeinit)();
        }
        if (sNetPresData.encProvObjectDS[x].fpDeinit != NULL)
        {
            (*sNetPresData.encProvObjectDS[x].fpDeinit)();
        }
        if (sNetPresData.encProvObjectDC[x].fpDeinit != NULL)
        {
            (*sNetPresData.encProvObjectDC[x].fpDeinit)();
        }
    }
    
    if (OSAL_MUTEX_Delete(&sNetPresData.presMutex) != OSAL_RESULT_TRUE)
    {
        
    }
    memset(&sNetPresData, 0, sizeof(NET_PRES_InternalData));
    memset(&sNetPresSockets, 0, sizeof(NET_PRES_SocketData) * NET_PRES_NUM_SOCKETS);
}

void NET_PRES_Reinitialize(SYS_MODULE_OBJ obj, const SYS_MODULE_INIT * const init)
{
    NET_PRES_Deinitialize(obj);
    NET_PRES_Initialize(0, init);
}

void NET_PRES_Tasks(SYS_MODULE_OBJ obj)
{
    uint8_t x;
    for (x = 0; x < NET_PRES_NUM_SOCKETS; x++)
    {
        if (sNetPresSockets[x].inUse && ((sNetPresSockets[x].socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED))
        {
            // Check the state of the socket and then pump it if necessary.
            switch (sNetPresSockets[x].status)
            {
                case NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION:
                {
                    // First thing is to check if the connection is connected.
                    if (!sNetPresSockets[x].transObject->fpIsConnected(sNetPresSockets[x].transHandle))
                    {
                        break;
                    }
                    // Next check to see if the provider has been initialized
                    if (OSAL_MUTEX_Lock(&sNetPresData.presMutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
                    {
                        continue;
                    }
                    if (!(*sNetPresSockets[x].provObject->fpIsInited)())
                    {
                        if (!(*sNetPresSockets[x].provObject->fpInit)(sNetPresSockets[x].transObject))
                        {
                            sNetPresSockets[x].status = NET_PRES_ENC_SS_FAILED;
                            if (OSAL_MUTEX_Unlock(&sNetPresData.presMutex) != OSAL_RESULT_TRUE)
                            {
                                continue;
                            }
                            continue;
                        }
                    }
                    if (OSAL_MUTEX_Unlock(&sNetPresData.presMutex) != OSAL_RESULT_TRUE)
                    {
                        continue;
                    }
                    if (!(*sNetPresSockets[x].provObject->fpOpen)(sNetPresSockets[x].transHandle, &sNetPresSockets[x].providerData))
                    {
                        sNetPresSockets[x].status = NET_PRES_ENC_SS_FAILED;
                        continue;                       
                    }
                    //Intentional fall through to the next state
                }
                case NET_PRES_ENC_SS_CLIENT_NEGOTIATING:
                case NET_PRES_ENC_SS_SERVER_NEGOTIATING:
                    sNetPresSockets[x].status = (*sNetPresSockets[x].provObject->fpConnect)(sNetPresSockets[x].providerData);
                    break;
                default:
                    break;
            }
        }
    }
}

NET_PRES_SKT_HANDLE_T NET_PRES_SocketOpen(NET_PRES_INDEX index, NET_PRES_SKT_T socketType, NET_PRES_SKT_ADDR_T addrType, NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS * addr, NET_PRES_SKT_ERROR_T* error)
{
    NET_PRES_TransportObject * transObject;
    NET_PRES_EncProviderObject * provObject;

    // Check to see if we have a valid index
    if (index >= sNetPresData.numLayers)
    {
        if (error != NULL)
        {
            *error = NET_PRES_SKT_OP_INVALID_INDEX;
        }
        return NET_PRES_INVALID_SOCKET;
    }

    // Check to see if the operation is supported
    if ((socketType & (NET_PRES_SKT_CLIENT | NET_PRES_SKT_STREAM)) ==  (NET_PRES_SKT_CLIENT | NET_PRES_SKT_STREAM))
    {
        transObject = &(sNetPresData.transObjectSC[index]);
        provObject = &(sNetPresData.encProvObjectSC[index]);
    }
    else if ((socketType & (NET_PRES_SKT_SERVER | NET_PRES_SKT_STREAM)) ==  (NET_PRES_SKT_SERVER | NET_PRES_SKT_STREAM))
    {
        transObject = &(sNetPresData.transObjectSS[index]);
        provObject = &(sNetPresData.encProvObjectSS[index]);
    }
    else if ((socketType & (NET_PRES_SKT_CLIENT | NET_PRES_SKT_DATAGRAM)) ==  (NET_PRES_SKT_CLIENT | NET_PRES_SKT_DATAGRAM))
    {
        transObject = &(sNetPresData.transObjectDC[index]);
        provObject = &(sNetPresData.encProvObjectDC[index]);
    }
    else if ((socketType & (NET_PRES_SKT_SERVER | NET_PRES_SKT_DATAGRAM)) ==  (NET_PRES_SKT_SERVER | NET_PRES_SKT_DATAGRAM))
    {
        transObject = &(sNetPresData.transObjectDS[index]);
        provObject = &(sNetPresData.encProvObjectDS[index]);
    }
    else
    {        
        if (error != NULL)
        {
            *error = NET_PRES_SKT_OP_INVALID_INDEX;
        }
        return NET_PRES_INVALID_SOCKET;
    }
    if (transObject->fpOpen == NULL)
    {
        if (error != NULL)
        {
            *error = NET_PRES_SKT_OP_NOT_SUPPORTED;
        }
        return NET_PRES_INVALID_SOCKET;        
    }
    bool encrypted = (socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED;
    if (!encrypted && !((socketType & NET_PRES_SKT_UNENCRYPTED) == NET_PRES_SKT_UNENCRYPTED))
    {
        // We're default
        if ((transObject->fpIsPortDefaultSecure!= NULL) && transObject->fpIsPortDefaultSecure(port))
        {
            encrypted = true;
            socketType |= NET_PRES_SKT_ENCRYPTED;
        }
    }
    
    if (encrypted)
    {
        if (provObject->fpOpen == NULL)
        {
            if (error != NULL)
            {
                *error = NET_PRES_SKT_OP_NOT_SUPPORTED;
            }
            return NET_PRES_INVALID_SOCKET;                    
        }
    }
    
    // The inputs have been validated
    if (OSAL_MUTEX_Lock(&sNetPresData.presMutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        if (error != NULL)
        {
            *error = NET_PRES_SKT_UNKNOWN_ERROR;
        }
        return NET_PRES_INVALID_SOCKET;
    }
    
    // Search for a free socket
    uint8_t sockIndex;
    for (sockIndex = 0 ; sockIndex < NET_PRES_NUM_SOCKETS; sockIndex++)
    {
        if (sNetPresSockets[sockIndex].inUse)
        {
            continue;
        }
        sNetPresSockets[sockIndex].inUse = true;
        // the socket has been soft locked so no longer need the mutex.
        if (OSAL_MUTEX_Unlock(&sNetPresData.presMutex) != OSAL_RESULT_TRUE)
        {
            sNetPresSockets[sockIndex].inUse = false;
            if (error != NULL)
            {
                *error = NET_PRES_SKT_UNKNOWN_ERROR;
            }
            return NET_PRES_INVALID_SOCKET;        
        }
        sNetPresSockets[sockIndex].transHandle = (*transObject->fpOpen)(addrType, port, addr);
        if (sNetPresSockets[sockIndex].transHandle == NET_PRES_INVALID_SOCKET)
        {
            sNetPresSockets[sockIndex].inUse = false;
            if (error != NULL)
            {
                *error = NET_PRES_SKT_UNKNOWN_ERROR;
            }
            return NET_PRES_INVALID_SOCKET;            
        }
        sNetPresSockets[sockIndex].transObject = transObject;
        sNetPresSockets[sockIndex].provObject = provObject;
        sNetPresSockets[sockIndex].socketType = socketType;
        sNetPresSockets[sockIndex].lastError = NET_PRES_SKT_OK;
        if (error != NULL)
        {
            *error = NET_PRES_SKT_OK;
        }
        if (encrypted)
        {
            sNetPresSockets[sockIndex].status = NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION;
        }
        return sockIndex+1; // avoid returning 0 on success.        
    }
    if (OSAL_MUTEX_Unlock(&sNetPresData.presMutex) != OSAL_RESULT_TRUE)
    {
        if (error != NULL)
        {
            *error = NET_PRES_SKT_UNKNOWN_ERROR;
        }
        return NET_PRES_INVALID_SOCKET;        
    }
    if (error != NULL)
    {
        *error = NET_PRES_SKT_OP_OUT_OF_HANDLES;
    }
    return NET_PRES_INVALID_SOCKET;   
}

static inline NET_PRES_SocketData *  _NET_PRES_SocketValidate(NET_PRES_SKT_HANDLE_T handle)
{
    if (handle <= 0 || handle > NET_PRES_NUM_SOCKETS)
    {
        return NULL;
    }
    handle--;
    if (!sNetPresSockets[handle].inUse)
    {
        return NULL;
    }
    return &(sNetPresSockets[handle]);
}

bool NET_PRES_SocketBind(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_ADDR_T addrType, NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS * addr)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }
    NET_PRES_TransBind fp = pSkt->transObject->fpLocalBind;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }

    return (*fp)(pSkt->transHandle, addrType, port, addr);
}


bool NET_PRES_SocketRemoteBind(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_ADDR_T addrType, NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS * addr)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBind fp =  pSkt->transObject->fpRemoteBind;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle, addrType, port, addr);
}

bool NET_PRES_SocketOptionsSet(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_OPTION_TYPE option, void* optParam)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransOption fp = pSkt->transObject->fpOptionSet;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle, option, optParam);
    
}

bool NET_PRES_SocketOptionsGet(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_OPTION_TYPE option, void* optParam)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransOption fp = pSkt->transObject->fpOptionGet;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle, option, optParam);    
}

bool NET_PRES_SocketIsConnected(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBool fp = pSkt->transObject->fpIsConnected;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle);    
} 

bool NET_PRES_SocketWasReset(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBool fp = pSkt->transObject->fpWasReset;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle);    
}  

bool NET_PRES_SocketDisconnect(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBool fp = pSkt->transObject->fpDisconnect;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    bool res = (*fp)(pSkt->transHandle);    


    if(res && (pSkt->socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED)
    {   // let the provide know that we start over
        if(pSkt->status > NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION)
        {
            NET_PRES_EncProviderConnectionClose fp = pSkt->provObject->fpClose;
            if (fp != NULL)
            {
                (*fp)(pSkt->providerData);
            }
            pSkt->status = NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION;
        }
    }

    return res;
}  

bool NET_PRES_SocketConnect(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBool fp = pSkt->transObject->fpConnect;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle);    
}  

void NET_PRES_SocketClose(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return ;
    }

    
    if ((pSkt->socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED)
    {
        NET_PRES_EncProviderConnectionClose fp = pSkt->provObject->fpClose;
        if (fp != NULL)
        {
            (*fp)(pSkt->providerData);
        }
    }
    NET_PRES_TransClose fpc = pSkt->transObject->fpClose;
    if (fpc == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return;
    }
    (*fpc)(pSkt->transHandle);
    if (OSAL_MUTEX_Lock(&sNetPresData.presMutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        pSkt->lastError = NET_PRES_SKT_UNKNOWN_ERROR;
        return;
    }
    memset(pSkt, 0, sizeof(NET_PRES_SocketData));
    if (OSAL_MUTEX_Unlock(&sNetPresData.presMutex) != OSAL_RESULT_TRUE)
    {
        pSkt->lastError = NET_PRES_SKT_UNKNOWN_ERROR;
        return;
    }
}

bool NET_PRES_SocketInfoGet(NET_PRES_SKT_HANDLE_T handle, void * info)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransSocketInfoGet fp = pSkt->transObject->fpSocketInfoGet;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle, info);    
   
}

// TODO aa: these empyrical constants are based on values detected during testing
// The proper wolfSSL function should be devised/used!
static uint32_t    transpMulFact = 18;          // transport provider multiplication factor
static uint32_t    transpDivFact = 10;          // transport provider divider factor
static uint16_t    writeReqSizeThres = 100;     // lower threshold for a required size
static uint16_t    writeMinReqSizeThres = 50;   // lower threshold for a minimum requested size

uint16_t NET_PRES_SocketWriteIsReady(NET_PRES_SKT_HANDLE_T handle, uint16_t reqSize, uint16_t minSize)
{
    uint16_t transpSize;
    NET_PRES_SocketData * pSkt;

    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    NET_PRES_TransReady fpTrans = pSkt->transObject->fpReadyToWrite;
    if (fpTrans == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }

    if ((pSkt->socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED)
    {   // encrypted socket
        NET_PRES_EncProviderWriteReady fpEnc =  0;

        if(pSkt->status == NET_PRES_ENC_SS_OPEN)
        {   // IsSecure!
            fpEnc = pSkt->provObject->fpWriteReady;
            if (fpEnc == NULL)
            {
                pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
                return 0;
            }

        }
        if(reqSize < writeReqSizeThres)
        {
            reqSize = writeReqSizeThres;
        }
        if(minSize != 0 && minSize < writeMinReqSizeThres)
        {
            minSize = writeMinReqSizeThres;
        }

        uint16_t encSize = fpEnc ? (*fpEnc)(pSkt->providerData, reqSize, minSize) : 0;
        if(encSize != 0)
        {   // check that transport also available
            transpSize = (*fpTrans)(pSkt->transHandle);
            if(transpSize >= (encSize * transpMulFact) / transpDivFact)
            {
                return encSize;
            }
        }
        return 0;
    }


    transpSize = (*fpTrans)(pSkt->transHandle);
    if(transpSize >= reqSize || (minSize != 0 && transpSize >= minSize))
    {
        return transpSize;
    }

    return 0;
}

uint16_t NET_PRES_SocketReadIsReady(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    if ((pSkt->socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED)
    {   // encrypted socket
        NET_PRES_EncProviderReadReady fp =  0;

        if(pSkt->status == NET_PRES_ENC_SS_OPEN)
        {   // IsSecure!
            fp = pSkt->provObject->fpReadReady;
            if (fp == NULL)
            {
                pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
                return 0;
            }
        }
		return fp ? (*fp)(pSkt->providerData) : 0;
    }
        
    NET_PRES_TransReady fp = pSkt->transObject->fpReadyToRead;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
    }
    return (*fp)(pSkt->transHandle);    
    
}

uint16_t NET_PRES_SocketWrite(NET_PRES_SKT_HANDLE_T handle, const void * buffer, uint16_t size)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    if ((pSkt->socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED)
    {
        NET_PRES_EncProviderWrite fp = pSkt->provObject->fpWrite;
        if (fp == NULL)
        {
            pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
            return 0;
        }
        return (*fp)(pSkt->providerData, buffer, size);    
    }
    NET_PRES_TransWrite fpc = pSkt->transObject->fpWrite;
    if (fpc == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }
    return (*fpc)(pSkt->transHandle, buffer, size);  
}

uint16_t NET_PRES_SocketFlush(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    // TODO aa: is it safe to call the transport layer even if the conenction is encrypted?
    NET_PRES_TransBool fp = pSkt->transObject->fpFlush;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }
    return (*fp)(pSkt->transHandle);        
}

uint16_t NET_PRES_SocketRead(NET_PRES_SKT_HANDLE_T handle, void * buffer, uint16_t size)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    if ((pSkt->socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED)
    {
        NET_PRES_EncProviderRead fp = pSkt->provObject->fpRead;
        if (fp == NULL)
        {
            pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
            return 0;
        }
        return (*fp)(pSkt->providerData, buffer, size);    
    }
    NET_PRES_TransRead fpc = pSkt->transObject->fpRead;
    if (fpc == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }
    return (*fpc)(pSkt->transHandle, buffer, size);  
}

uint16_t NET_PRES_SocketPeek(NET_PRES_SKT_HANDLE_T handle, void * buffer, uint16_t size)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    if ((pSkt->socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED)
    {
        NET_PRES_EncProviderRead fp = pSkt->provObject->fpPeek;
        if (fp == NULL)
        {
            pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
            return 0;
        }
        return (*fp)(pSkt->providerData, buffer, size);    
    }
    NET_PRES_TransPeek fpc = pSkt->transObject->fpPeek;
    if (fpc == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }
    return (*fpc)(pSkt->transHandle, buffer, size, 0);  
}

uint16_t NET_PRES_SocketDiscard(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    NET_PRES_TransDiscard fp = pSkt->transObject->fpDiscard;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }
    return (*fp)(pSkt->transHandle);    
    
}


NET_PRES_SIGNAL_HANDLE NET_PRES_SocketSignalHandlerRegister(NET_PRES_SKT_HANDLE_T handle, uint16_t sigMask, NET_PRES_SIGNAL_FUNCTION handler, const void* hParam)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    NET_PRES_TransHandlerRegister fp = pSkt->transObject->fpHandlerRegister;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return NULL;
    }
    return (*fp)(pSkt->transHandle, sigMask, handler, hParam);    

}
bool NET_PRES_SocketSignalHandlerDeregister(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SIGNAL_HANDLE hSig)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransSignalHandlerDeregister fp = pSkt->transObject->fpHandlerDeregister;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(handle, hSig);    

}

bool NET_PRES_SocketIsNegotiatingEncryption(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }
    return ((pSkt->status == NET_PRES_ENC_SS_CLIENT_NEGOTIATING) ||  
            (pSkt->status == NET_PRES_ENC_SS_SERVER_NEGOTIATING) || 
            (pSkt->status == NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION));
}
bool NET_PRES_SocketIsSecure(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    return pSkt->status == NET_PRES_ENC_SS_OPEN;
}
bool NET_PRES_SocketEncryptSocket(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    if ((pSkt->socketType & (NET_PRES_SKT_UNENCRYPTED | NET_PRES_SKT_ENCRYPTED)) != NET_PRES_SKT_UNENCRYPTED)
    {
        return false;
    }
    pSkt->socketType ^= NET_PRES_SKT_UNENCRYPTED | NET_PRES_SKT_ENCRYPTED;
    pSkt->status = NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION;
    return true;
}

bool NET_PRES_SocketIsOpenModeSupported(NET_PRES_INDEX index, NET_PRES_SKT_T socketType)
{
    NET_PRES_TransOpen fpTransOpen = NULL;
    NET_PRES_EncProviderOpen fpProvOpen= NULL;

    // Check to see if we have a valid index
    if (index >= sNetPresData.numLayers)
    {
        return false;
    }

    // Check to see if the operation is supported
    if ((socketType & (NET_PRES_SKT_CLIENT | NET_PRES_SKT_STREAM)) ==  (NET_PRES_SKT_CLIENT | NET_PRES_SKT_STREAM))
    {
        fpProvOpen = sNetPresData.encProvObjectSC[index].fpOpen;
        fpTransOpen = sNetPresData.transObjectSC[index].fpOpen;
    }
    else if ((socketType & (NET_PRES_SKT_SERVER | NET_PRES_SKT_STREAM)) ==  (NET_PRES_SKT_SERVER | NET_PRES_SKT_STREAM))
    {
        fpProvOpen = sNetPresData.encProvObjectSS[index].fpOpen;
        fpTransOpen = sNetPresData.transObjectSS[index].fpOpen;
    }
    else if ((socketType & (NET_PRES_SKT_CLIENT | NET_PRES_SKT_DATAGRAM)) ==  (NET_PRES_SKT_CLIENT | NET_PRES_SKT_DATAGRAM))
    {
        fpProvOpen = sNetPresData.encProvObjectDC[index].fpOpen;
        fpTransOpen = sNetPresData.transObjectDC[index].fpOpen;
    }
    else if ((socketType & (NET_PRES_SKT_SERVER | NET_PRES_SKT_DATAGRAM)) ==  (NET_PRES_SKT_SERVER | NET_PRES_SKT_DATAGRAM))
    {
        fpProvOpen = sNetPresData.encProvObjectDS[index].fpOpen;
        fpTransOpen = sNetPresData.transObjectDS[index].fpOpen;
    }
    if (fpTransOpen == NULL)
    {
        return false;        
    }
    bool encrypted = (socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED;
    
    if (encrypted)
    {
        if (fpProvOpen == NULL)
        {
            return false;                    
        }
    }
    return true;
}

SYS_STATUS NET_PRES_Status ( SYS_MODULE_OBJ object )
{
    NET_PRES_InternalData * pData = (NET_PRES_InternalData*)object;
    if (pData != &sNetPresData)
    {
        return SYS_STATUS_ERROR;
    }
    if (pData->initialized)
    {
        return SYS_STATUS_READY;
    }
    else
    {
        return SYS_STATUS_UNINITIALIZED;
    }
}

NET_PRES_SKT_ERROR_T NET_PRES_SocketLastError(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return NET_PRES_SKT_INVALID_SOCKET;
    }

    NET_PRES_SKT_ERROR_T lastError = pSkt->lastError;
    pSkt->lastError = NET_PRES_SKT_OK;

    return lastError;
}

NET_PRES_SKT_HANDLE_T NET_PRES_SocketGetTransportHandle(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return NET_PRES_SKT_INVALID_SOCKET;
    }

    pSkt->lastError = NET_PRES_SKT_OK;
    return pSkt->transHandle;
    
}
