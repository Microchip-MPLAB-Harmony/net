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

#include "../net_pres.h"
#include "../net_pres_encryptionproviderapi.h"
#include "../net_pres_socketapi.h"
#include "../net_pres_transportapi.h"
#include "system/debug/sys_debug.h"
#include "net_pres_local.h"


// local data
NET_PRES_InternalData sNetPresData;
NET_PRES_SocketData sNetPresSockets[NET_PRES_NUM_SOCKETS];


// basic level  debugging
#if ((NET_PRES_DEBUG_LEVEL & NET_PRES_DEBUG_MASK_BASIC) != 0)
static volatile int _NET_PRES_StayAssertLoop = 0;
static void _NET_PRES_AssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("NET_PRES Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_NET_PRES_StayAssertLoop != 0);
    }
}

#else
#define _NET_PRES_AssertCond(cond, message, lineNo)
#endif  // (NET_PRES_DEBUG_LEVEL)


// local prototypes
static void NET_PRES_SignalHandler(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param);


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
                    sNetPresSockets[x].provOpen = true;
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

bool NET_PRES_SocketWasDisconnected(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBool fp = pSkt->transObject->fpWasDisconnected;
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

    NET_PRES_TransBool fpDiscon = pSkt->transObject->fpDisconnect;
    if (fpDiscon == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    bool res = (*fpDiscon)(pSkt->transHandle);    


    if(res && (pSkt->socketType & NET_PRES_SKT_ENCRYPTED) == NET_PRES_SKT_ENCRYPTED)
    {   // let the provide know that we start over
        if(pSkt->status > NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION)
        {
            if(pSkt->provOpen)
            {
                NET_PRES_EncProviderConnectionClose fpClose = pSkt->provObject->fpClose;
                if (fpClose != NULL)
                {
                    (*fpClose)(pSkt->providerData);
                    pSkt->provOpen = 0;
                }
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

uint16_t NET_PRES_SocketWriteIsReady(NET_PRES_SKT_HANDLE_T handle, uint16_t reqSize, uint16_t minSize)
{
    uint16_t transpSpace;
    uint16_t encAvlblSize, encOutSize; 
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

        NET_PRES_EncProviderWriteReady fpWriteReady = pSkt->provObject->fpWriteReady;
        NET_PRES_EncProviderOutputSize fpOutputSize = pSkt->provObject->fpOutputSize;

        if(pSkt->status != NET_PRES_ENC_SS_OPEN || fpWriteReady == NULL || fpOutputSize == NULL)
        {   // data cannot be sent/received
            pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
            return 0;
        }

        transpSpace = (*fpTrans)(pSkt->transHandle);
        encAvlblSize = (*fpWriteReady)(pSkt->providerData, reqSize, 0);
        if(encAvlblSize != 0)
        {   // check that transport also available
            encOutSize = (*fpOutputSize)(pSkt->providerData, reqSize); 
            if(transpSpace >= encOutSize)
            {
                return reqSize;
            }
        }
        // failed; check for min space
        if(minSize != 0)
        {
            encAvlblSize = (*fpWriteReady)(pSkt->providerData, minSize, 0);
            if(encAvlblSize != 0)
            {   // check that transport also available
                encOutSize = (*fpOutputSize)(pSkt->providerData, minSize); 
                if(transpSpace >= encOutSize)
                {
                    return minSize;
                }
            }
        }

        // failed
        return 0;
    }


    // non secured socket
    transpSpace = (*fpTrans)(pSkt->transHandle);
    if(transpSpace >= reqSize || (minSize != 0 && transpSpace >= minSize))
    {
        return transpSpace;
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

        if(buffer == 0)
        {   // this means discard for the TCP/IP
            // but the provider might not support it
            int ix;
            uint8_t discard_buff[100];

            uint16_t nDiscards = size / sizeof(discard_buff);
            uint16_t nLeft = size % sizeof(discard_buff);
            uint16_t discardBytes = 0;

            for(ix = 0; ix < nDiscards; ix++)
            {
                discardBytes += (*fp)(pSkt->providerData, discard_buff, sizeof(discard_buff));    
            }

            if(nLeft)
            {
                discardBytes += (*fp)(pSkt->providerData, discard_buff, nLeft);    
            }
            return discardBytes;
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

// socket handle it's a transport handle, actually
static void NET_PRES_SignalHandler(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    NET_PRES_SocketData * pSkt = (NET_PRES_SocketData*)param;
    _NET_PRES_AssertCond(pSkt->transHandle == handle,  __func__, __LINE__);
    _NET_PRES_AssertCond(pSkt->sigHandle != 0,  __func__, __LINE__);
    _NET_PRES_AssertCond(pSkt->usrSigFnc != 0,  __func__, __LINE__);

    // call the user handler
    if(pSkt->usrSigFnc != 0)
    {
        uint16_t sktIx = (pSkt - sNetPresSockets) + 1; 
        (*pSkt->usrSigFnc)(sktIx, hNet, sigType, pSkt->usrSigParam);
    }

}


NET_PRES_SIGNAL_HANDLE NET_PRES_SocketSignalHandlerRegister(NET_PRES_SKT_HANDLE_T handle, uint16_t sigMask, NET_PRES_SIGNAL_FUNCTION handler, const void* hParam)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = _NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    if (handler == 0)
    {
        pSkt->lastError = NET_PRES_SKT_HANDLER_ERROR;
        return 0;
    }

    NET_PRES_TransHandlerRegister fp = pSkt->transObject->fpHandlerRegister;
    if (fp == NULL)
    {
        pSkt->lastError = NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }

    if(pSkt->sigHandle != 0)
    {   // one socket has just one signal handler
        pSkt->lastError = NET_PRES_SKT_HANDLER_BUSY;
        return 0;
    }

    pSkt->usrSigFnc = handler;
    pSkt->usrSigParam = hParam;
    pSkt->sigHandle = (*fp)(pSkt->transHandle, sigMask, NET_PRES_SignalHandler, pSkt);    

    if(pSkt->sigHandle == 0)
    {   // failed
        pSkt->lastError = NET_PRES_SKT_HANDLER_TRANSP_ERROR;
    }


    return pSkt->sigHandle;

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

    if(pSkt->sigHandle == 0 || pSkt->sigHandle != hSig)
    {   // no such signal handler
        pSkt->lastError = NET_PRES_SKT_HANDLER_ERROR;
        return false;
    }

    bool res = (*fp)(pSkt->transHandle, pSkt->sigHandle);
    if(res)
    {   // done
        pSkt->sigHandle = 0;
    }
    else
    {
        pSkt->lastError = NET_PRES_SKT_HANDLER_TRANSP_ERROR;
    }

    return res;
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
    if(pSkt->provObject->fpInit == 0 || pSkt->provObject->fpOpen == 0 || pSkt->provObject->fpIsInited == 0)
    {   // cannot start negotiation
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
