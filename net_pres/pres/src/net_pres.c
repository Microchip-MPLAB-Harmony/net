/*
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#include "../net_pres.h"
#include "../net_pres_encryptionproviderapi.h"
#include "../net_pres_socketapi.h"
#include "../net_pres_transportapi.h"
#include "system/debug/sys_debug.h"
#include "net_pres_local.h"


// local data
static NET_PRES_InternalData sNetPresData;
static NET_PRES_SocketData sNetPresSockets[NET_PRES_NUM_SOCKETS];


// basic level  debugging
#if ((NET_PRES_DEBUG_LEVEL & NET_PRES_DEBUG_MASK_BASIC) != 0)
static volatile int NET_PRES_StayAssertLoop = 0;
static void F_NET_PRES_AssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("NET_PRES Assert: %s, in line: %d, \r\n", message, lineNo);
        while(NET_PRES_StayAssertLoop != 0);
    }
}

#else
#define F_NET_PRES_AssertCond(cond, message, lineNo)
#endif  // (NET_PRES_DEBUG_LEVEL)


// local prototypes
static void NET_PRES_SignalHandler(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param);


SYS_MODULE_OBJ NET_PRES_Initialize( const SYS_MODULE_INDEX index, const void* initData )
{
    if (sNetPresData.initialized || initData == NULL)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    const NET_PRES_INIT_DATA *pInitData = (const NET_PRES_INIT_DATA*)initData;
    
    if (pInitData->numLayers > (uint8_t)NET_PRES_NUM_INSTANCE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    

    (void)memset(&sNetPresData, 0, sizeof(NET_PRES_InternalData));
    if (OSAL_MUTEX_Create(&sNetPresData.presMutex) != OSAL_RESULT_SUCCESS)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    (void)memset(&sNetPresSockets, 0, sizeof(NET_PRES_SocketData) * (size_t)NET_PRES_NUM_SOCKETS);
    sNetPresData.initialized = true;
    sNetPresData.numLayers = pInitData->numLayers;
    uint8_t x;
    for (x = 0; x < sNetPresData.numLayers; x++)
    {
        if (pInitData->pInitData[x].pTransObject_ss != NULL)
        {
            (void)memcpy(&sNetPresData.transObjectSS[x], pInitData->pInitData[x].pTransObject_ss, sizeof(NET_PRES_TransportObject));
        }
        if (pInitData->pInitData[x].pTransObject_sc != NULL)
        {
            (void)memcpy(&sNetPresData.transObjectSC[x], pInitData->pInitData[x].pTransObject_sc, sizeof(NET_PRES_TransportObject));
        }
        if (pInitData->pInitData[x].pTransObject_ds != NULL)
        {
            (void)memcpy(&sNetPresData.transObjectDS[x], pInitData->pInitData[x].pTransObject_ds, sizeof(NET_PRES_TransportObject));
        }
        if (pInitData->pInitData[x].pTransObject_dc != NULL)
        {
            (void)memcpy(&sNetPresData.transObjectDC[x], pInitData->pInitData[x].pTransObject_dc, sizeof(NET_PRES_TransportObject));
        }
        if (pInitData->pInitData[x].pProvObject_ss != NULL)
        {
            (void)memcpy(&sNetPresData.encProvObjectSS[x], pInitData->pInitData[x].pProvObject_ss, sizeof(Net_ProvObject));
        }        
        if (pInitData->pInitData[x].pProvObject_sc != NULL)
        {
            (void)memcpy(&sNetPresData.encProvObjectSC[x], pInitData->pInitData[x].pProvObject_sc, sizeof(Net_ProvObject));
        }        
        if (pInitData->pInitData[x].pProvObject_ds != NULL)
        {
            (void)memcpy(&sNetPresData.encProvObjectDS[x], pInitData->pInitData[x].pProvObject_ds, sizeof(Net_ProvObject));
        }        
        if (pInitData->pInitData[x].pProvObject_dc != NULL)
        {
            (void)memcpy(&sNetPresData.encProvObjectDC[x], pInitData->pInitData[x].pProvObject_dc, sizeof(Net_ProvObject));
        }        
    }
    return (SYS_MODULE_OBJ)&sNetPresData;
}

SYS_MODULE_OBJ NET_PRES_ModuleObjGet(const SYS_MODULE_INDEX index)
{
    if(sNetPresData.initialized == false)
    {
        return SYS_MODULE_OBJ_INVALID;
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
    for (x = 0; x < (uint8_t)NET_PRES_NUM_SOCKETS; x++)
    {
        if (sNetPresSockets[x].inUse != 0U)
        {
            if ((sNetPresSockets[x].socketType & (uint16_t)NET_PRES_SKT_ENCRYPTED) == (uint16_t)NET_PRES_SKT_ENCRYPTED)
            {
                Net_ProvConnectionClose fpClose = sNetPresSockets[x].provObject->fpClose;
                NET_PRES_TransClose fpTransClose = sNetPresSockets[x].transObject->fpClose;
                if (fpClose != NULL)
                {
                    (void)(*fpClose)(sNetPresSockets[x].providerData);
                }
                if (fpTransClose != NULL)
                {
                    (*fpTransClose)(sNetPresSockets[x].transHandle);
                }
                sNetPresSockets[x].inUse = 0U;
            }
        }
    }
    
    // Make sure all the encryption providers are down
    for (x = 0; x < (uint8_t)NET_PRES_NUM_INSTANCE; x++)
    {
        if (sNetPresData.encProvObjectSS[x].fpDeinit != NULL)
        {
            (void)(*sNetPresData.encProvObjectSS[x].fpDeinit)();
        }
        if (sNetPresData.encProvObjectSC[x].fpDeinit != NULL)
        {
            (void)(*sNetPresData.encProvObjectSC[x].fpDeinit)();
        }
        if (sNetPresData.encProvObjectDS[x].fpDeinit != NULL)
        {
            (void)(*sNetPresData.encProvObjectDS[x].fpDeinit)();
        }
        if (sNetPresData.encProvObjectDC[x].fpDeinit != NULL)
        {
            (void)(*sNetPresData.encProvObjectDC[x].fpDeinit)();
        }
    }
    
    if (OSAL_MUTEX_Delete(&sNetPresData.presMutex) != OSAL_RESULT_SUCCESS)
    {
        
    }
    (void)memset(&sNetPresData, 0, sizeof(NET_PRES_InternalData));
    (void)memset(&sNetPresSockets, 0, sizeof(NET_PRES_SocketData) * (size_t)NET_PRES_NUM_SOCKETS);
}

void NET_PRES_Reinitialize(SYS_MODULE_OBJ obj, const void* initData)
{
    NET_PRES_Deinitialize(obj);
    (void)NET_PRES_Initialize(0, initData);
}

void NET_PRES_Tasks(SYS_MODULE_OBJ obj)
{
    uint8_t x, status;
    for (x = 0; x < (uint8_t)NET_PRES_NUM_SOCKETS; x++)
    {
        if (sNetPresSockets[x].inUse != 0U && ((sNetPresSockets[x].socketType & (uint16_t)NET_PRES_SKT_ENCRYPTED) == (uint16_t)NET_PRES_SKT_ENCRYPTED))
        {
            // Check the state of the socket and then pump it if necessary.
            status = sNetPresSockets[x].status;
            if (status == (uint8_t)NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION || status == (uint8_t)NET_PRES_ENC_SS_CLIENT_NEGOTIATING || status == (uint8_t)NET_PRES_ENC_SS_SERVER_NEGOTIATING)
            {
                if (status == (uint8_t)NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION)
                {
                    // First thing is to check if the connection is connected.
                    if (!sNetPresSockets[x].transObject->fpIsConnected(sNetPresSockets[x].transHandle))
                    {
                        continue;
                    }
                    // Next check to see if the provider has been initialized
                    if (OSAL_MUTEX_Lock(&sNetPresData.presMutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
                    {
                        continue;
                    }
                    if (!(*sNetPresSockets[x].provObject->fpIsInited)())
                    {
                        if (!(*sNetPresSockets[x].provObject->fpInit)(sNetPresSockets[x].transObject))
                        {
                            sNetPresSockets[x].status = (uint8_t)NET_PRES_ENC_SS_FAILED;
                            if (OSAL_MUTEX_Unlock(&sNetPresData.presMutex) != OSAL_RESULT_SUCCESS)
                            {
                            }
                            continue;
                        }
                    }
                    if (OSAL_MUTEX_Unlock(&sNetPresData.presMutex) != OSAL_RESULT_SUCCESS)
                    {
                        continue;
                    }
                    if (!(*sNetPresSockets[x].provObject->fpOpen)(obj, (uintptr_t)((uint32_t)x + 1U), sNetPresSockets[x].transHandle, &sNetPresSockets[x].providerData))
                    {
                        sNetPresSockets[x].status = (uint8_t)NET_PRES_ENC_SS_FAILED;
                        continue;                       
                    }
                    sNetPresSockets[x].provOpen = 1U;
                }

                sNetPresSockets[x].status = (uint8_t)(*sNetPresSockets[x].provObject->fpConnect)(sNetPresSockets[x].providerData);
            }
        }
    }
}

NET_PRES_SKT_HANDLE_T NET_PRES_SocketOpen(NET_PRES_INDEX index, NET_PRES_SKT_T socketType, NET_PRES_SKT_ADDR_T addrType, NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS * addr, NET_PRES_SKT_ERROR_T* error)
{
    NET_PRES_TransportObject * transObject;
    Net_ProvObject * provObject;

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
    if (((uint16_t)socketType & ((uint16_t)NET_PRES_SKT_CLIENT | (uint16_t)NET_PRES_SKT_STREAM)) ==  ((uint16_t)NET_PRES_SKT_CLIENT | (uint16_t)NET_PRES_SKT_STREAM))
    {
        transObject = &(sNetPresData.transObjectSC[index]);
        provObject = &(sNetPresData.encProvObjectSC[index]);
    }
    else if (((uint16_t)socketType & ((uint16_t)NET_PRES_SKT_SERVER | (uint16_t)NET_PRES_SKT_STREAM)) ==  ((uint16_t)NET_PRES_SKT_SERVER | (uint16_t)NET_PRES_SKT_STREAM))
    {
        transObject = &(sNetPresData.transObjectSS[index]);
        provObject = &(sNetPresData.encProvObjectSS[index]);
    }
    else if (((uint16_t)socketType & ((uint16_t)NET_PRES_SKT_CLIENT | (uint16_t)NET_PRES_SKT_DATAGRAM)) ==  ((uint16_t)NET_PRES_SKT_CLIENT | (uint16_t)NET_PRES_SKT_DATAGRAM))
    {
        transObject = &(sNetPresData.transObjectDC[index]);
        provObject = &(sNetPresData.encProvObjectDC[index]);
    }
    else if (((uint16_t)socketType & ((uint16_t)NET_PRES_SKT_SERVER | (uint16_t)NET_PRES_SKT_DATAGRAM)) ==  ((uint16_t)NET_PRES_SKT_SERVER |(uint16_t) NET_PRES_SKT_DATAGRAM))
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
    bool encrypted = ((uint16_t)socketType & (uint16_t)NET_PRES_SKT_ENCRYPTED) == (uint16_t)NET_PRES_SKT_ENCRYPTED;
    if (!encrypted && !(((uint16_t)socketType & (uint16_t)NET_PRES_SKT_UNENCRYPTED) == (uint16_t)NET_PRES_SKT_UNENCRYPTED))
    {
        // We're default
        if ((transObject->fpIsPortDefaultSecure!= NULL) && transObject->fpIsPortDefaultSecure(port))
        {
            encrypted = true;
            uint16_t type16 = (uint16_t)socketType;
            type16 |= (uint16_t)NET_PRES_SKT_ENCRYPTED;
            socketType = (NET_PRES_SKT_T)type16;
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
    if (OSAL_MUTEX_Lock(&sNetPresData.presMutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
    {
        if (error != NULL)
        {
            *error = NET_PRES_SKT_UNKNOWN_ERROR;
        }
        return NET_PRES_INVALID_SOCKET;
    }
    
    // Search for a free socket
    uint8_t sockIndex;
    for (sockIndex = 0 ; sockIndex < (uint8_t)NET_PRES_NUM_SOCKETS; sockIndex++)
    {
        if (sNetPresSockets[sockIndex].inUse != 0U)
        {
            continue;
        }
        sNetPresSockets[sockIndex].inUse = 1U;
        // the socket has been soft locked so no longer need the mutex.
        if (OSAL_MUTEX_Unlock(&sNetPresData.presMutex) != OSAL_RESULT_SUCCESS)
        {
            sNetPresSockets[sockIndex].inUse = 0U;
            if (error != NULL)
            {
                *error = NET_PRES_SKT_UNKNOWN_ERROR;
            }
            return NET_PRES_INVALID_SOCKET;        
        }
        sNetPresSockets[sockIndex].transHandle = (*transObject->fpOpen)(addrType, port, addr);
        if (sNetPresSockets[sockIndex].transHandle == NET_PRES_INVALID_SOCKET)
        {
            sNetPresSockets[sockIndex].inUse = 0U;
            if (error != NULL)
            {
                *error = NET_PRES_SKT_UNKNOWN_ERROR;
            }
            return NET_PRES_INVALID_SOCKET;            
        }
        sNetPresSockets[sockIndex].transObject = transObject;
        sNetPresSockets[sockIndex].provObject = provObject;
        sNetPresSockets[sockIndex].socketType = (uint16_t)socketType;
        sNetPresSockets[sockIndex].lastError = (int8_t)NET_PRES_SKT_OK;
        if (error != NULL)
        {
            *error = NET_PRES_SKT_OK;
        }
        if (encrypted)
        {
            sNetPresSockets[sockIndex].status = (uint8_t)NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION;
        }
        return (NET_PRES_SKT_HANDLE_T)sockIndex + (NET_PRES_SKT_HANDLE_T)1U; // avoid returning 0 on success.        
    }
    if (OSAL_MUTEX_Unlock(&sNetPresData.presMutex) != OSAL_RESULT_SUCCESS)
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

static inline NET_PRES_SocketData *  F_NET_PRES_SocketValidate(NET_PRES_SKT_HANDLE_T handle)
{
    if ((int8_t)handle <= 0 || (uint8_t)handle > (uint8_t)NET_PRES_NUM_SOCKETS)
    {
        return NULL;
    }
    handle--;
    if (sNetPresSockets[handle].inUse == 0U)
    {
        return NULL;
    }
    return &(sNetPresSockets[handle]);
}

bool NET_PRES_SocketBind(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_ADDR_T addrType, NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS * addr)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }
    NET_PRES_TransBind fp = pSkt->transObject->fpLocalBind;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }

    return (*fp)(pSkt->transHandle, addrType, port, addr);
}


bool NET_PRES_SocketRemoteBind(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_ADDR_T addrType, NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS * addr)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBind fp =  pSkt->transObject->fpRemoteBind;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle, addrType, port, addr);
}

bool NET_PRES_SocketOptionsSet(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_OPTION_TYPE option, void* optParam)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransOption fp = pSkt->transObject->fpOptionSet;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle, option, optParam);
    
}

bool NET_PRES_SocketOptionsGet(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_OPTION_TYPE option, void* optParam)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransOption fp = pSkt->transObject->fpOptionGet;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle, option, optParam);    
}

bool NET_PRES_SocketIsConnected(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBool fp = pSkt->transObject->fpIsConnected;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle);    
} 

bool NET_PRES_SocketWasReset(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBool fp = pSkt->transObject->fpWasReset;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle);    
}  

bool NET_PRES_SocketWasDisconnected(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBool fp = pSkt->transObject->fpWasDisconnected;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle);    
}  


bool NET_PRES_SocketDisconnect(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBool fpDiscon = pSkt->transObject->fpDisconnect;
    if (fpDiscon == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    bool res = (*fpDiscon)(pSkt->transHandle);    


    if(res && (pSkt->socketType & (uint16_t)NET_PRES_SKT_ENCRYPTED) == (uint16_t)NET_PRES_SKT_ENCRYPTED)
    {   // let the provide know that we start over
        if(pSkt->status > (uint8_t)NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION)
        {
            if(pSkt->provOpen != 0U)
            {
                Net_ProvConnectionClose fpClose = pSkt->provObject->fpClose;
                if (fpClose != NULL)
                {
                    (void)(*fpClose)(pSkt->providerData);
                    pSkt->provOpen = 0U;
                }
            }
            pSkt->status = (uint8_t)NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION;
        }
    }

    return res;
}  

bool NET_PRES_SocketConnect(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransBool fp = pSkt->transObject->fpConnect;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle);    
}  

void NET_PRES_SocketClose(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return ;
    }

    
    if ((pSkt->socketType & (uint16_t)NET_PRES_SKT_ENCRYPTED) == (uint16_t)NET_PRES_SKT_ENCRYPTED)
    {
        Net_ProvConnectionClose fp = pSkt->provObject->fpClose;
        if (fp != NULL)
        {
            (void)(*fp)(pSkt->providerData);
        }
    }
    NET_PRES_TransClose fpc = pSkt->transObject->fpClose;
    if (fpc == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return;
    }
    (*fpc)(pSkt->transHandle);
    if (OSAL_MUTEX_Lock(&sNetPresData.presMutex, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_UNKNOWN_ERROR;
        return;
    }
    (void)memset(pSkt, 0, sizeof(NET_PRES_SocketData));
    if (OSAL_MUTEX_Unlock(&sNetPresData.presMutex) != OSAL_RESULT_SUCCESS)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_UNKNOWN_ERROR;
        return;
    }
}

bool NET_PRES_SocketInfoGet(NET_PRES_SKT_HANDLE_T handle, void * info)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransSocketInfoGet fp = pSkt->transObject->fpSocketInfoGet;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }
    return (*fp)(pSkt->transHandle, info);    
   
}

uint16_t NET_PRES_SocketWriteIsReady(NET_PRES_SKT_HANDLE_T handle, uint16_t reqSize, uint16_t minSize)
{
    uint16_t transpSpace;
    uint16_t encAvlblSize, encOutSize; 
    NET_PRES_SocketData * pSkt;

    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    NET_PRES_TransReady fpTrans = pSkt->transObject->fpReadyToWrite;
    if (fpTrans == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }

    if ((pSkt->socketType & (uint16_t)NET_PRES_SKT_ENCRYPTED) == (uint16_t)NET_PRES_SKT_ENCRYPTED)
    {   // encrypted socket

        Net_ProvWriteReady fpWriteReady = pSkt->provObject->fpWriteReady;
        Net_ProvOutputSize fpOutputSize = pSkt->provObject->fpOutputSize;

        if(pSkt->status != (uint8_t)NET_PRES_ENC_SS_OPEN || fpWriteReady == NULL || fpOutputSize == NULL)
        {   // data cannot be sent/received
            pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
            return 0;
        }

        transpSpace = (*fpTrans)(pSkt->transHandle);
        encAvlblSize = (*fpWriteReady)(pSkt->providerData, reqSize, 0);
        if(encAvlblSize != 0U)
        {   // check that transport also available
            encOutSize = (uint16_t)(*fpOutputSize)(pSkt->providerData, (int32_t)reqSize); 
            if(transpSpace >= encOutSize)
            {
                return reqSize;
            }
        }
        // failed; check for min space
        if(minSize != 0U)
        {
            encAvlblSize = (*fpWriteReady)(pSkt->providerData, minSize, 0);
            if(encAvlblSize != 0U)
            {   // check that transport also available
                encOutSize = (uint16_t)(*fpOutputSize)(pSkt->providerData, (int32_t)minSize); 
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
    if(transpSpace >= reqSize || (minSize != 0U && transpSpace >= minSize))
    {
        return transpSpace;
    }

    return 0U;
}

uint16_t NET_PRES_SocketReadIsReady(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    if ((pSkt->socketType & (uint16_t)NET_PRES_SKT_ENCRYPTED) == (uint16_t)NET_PRES_SKT_ENCRYPTED)
    {   // encrypted socket

        if(pSkt->status == (uint8_t)NET_PRES_ENC_SS_OPEN)
        {   // IsSecure!
            Net_ProvReadReady fprov = pSkt->provObject->fpReadReady;
            if (fprov == NULL)
            {
                pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
                return 0;
            }
            return (uint16_t)(*fprov)(pSkt->providerData);
        }
        // not secure yet
        return 0;
    }
        
    NET_PRES_TransReady ftrans = pSkt->transObject->fpReadyToRead;
    if (ftrans == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }
    return (*ftrans)(pSkt->transHandle);    
}

uint16_t NET_PRES_SocketWrite(NET_PRES_SKT_HANDLE_T handle, const void * dataBuffer, uint16_t size)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    if ((pSkt->socketType & (uint16_t)NET_PRES_SKT_ENCRYPTED) == (uint16_t)NET_PRES_SKT_ENCRYPTED)
    {
        Net_ProvWrite fp = pSkt->provObject->fpWrite;
        if (fp == NULL)
        {
            pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
            return 0;
        }
        return (uint16_t)(*fp)(pSkt->providerData, dataBuffer, size);    
    }
    NET_PRES_TransWrite fpc = pSkt->transObject->fpWrite;
    if (fpc == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }
    return (*fpc)(pSkt->transHandle, dataBuffer, size);  
}

uint16_t NET_PRES_SocketFlush(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    NET_PRES_TransBool fp = pSkt->transObject->fpFlush;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }
    return (uint16_t)(*fp)(pSkt->transHandle);        
}

uint16_t NET_PRES_SocketRead(NET_PRES_SKT_HANDLE_T handle, void * dataBuffer, uint16_t size)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    if ((pSkt->socketType & (uint16_t)NET_PRES_SKT_ENCRYPTED) == (uint16_t)NET_PRES_SKT_ENCRYPTED)
    {
        Net_ProvRead fp = pSkt->provObject->fpRead;
        if (fp == NULL)
        {
            pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
            return 0;
        }

        if(dataBuffer == NULL)
        {   // this means discard for the TCP/IP
            // but the provider might not support it
            uint16_t ix;
            uint8_t discard_buff[100];

            uint16_t nDiscards = size / (uint16_t)sizeof(discard_buff);
            uint16_t nLeft = size % (uint16_t)sizeof(discard_buff);
            uint16_t discardBytes = 0U;

            for(ix = 0; ix < nDiscards; ix++)
            {
                discardBytes += (uint16_t)(*fp)(pSkt->providerData, discard_buff, sizeof(discard_buff));    
            }

            if(nLeft != 0U)
            {
                discardBytes += (uint16_t)(*fp)(pSkt->providerData, discard_buff, nLeft);    
            }
            return discardBytes;
        }
        return (uint16_t)(*fp)(pSkt->providerData, dataBuffer, size);    
    }
    NET_PRES_TransRead fpc = pSkt->transObject->fpRead;
    if (fpc == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }
    return (*fpc)(pSkt->transHandle, dataBuffer, size);  
}

uint16_t NET_PRES_SocketPeek(NET_PRES_SKT_HANDLE_T handle, void * dataBuffer, uint16_t size)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    if ((pSkt->socketType & (uint16_t)NET_PRES_SKT_ENCRYPTED) == (uint16_t)NET_PRES_SKT_ENCRYPTED)
    {
        Net_ProvRead fp = pSkt->provObject->fpPeek;
        if (fp == NULL)
        {
            pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
            return 0;
        }
        return (uint16_t)(*fp)(pSkt->providerData, dataBuffer, size);    
    }
    NET_PRES_TransPeek fpc = pSkt->transObject->fpPeek;
    if (fpc == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }
    return (uint16_t)(*fpc)(pSkt->transHandle, dataBuffer, size, 0);  
}

uint16_t NET_PRES_SocketDiscard(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return 0;
    }

    NET_PRES_TransDiscard fp = pSkt->transObject->fpDiscard;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return 0;
    }
    return (*fp)(pSkt->transHandle);    
    
}

// socket handle it's a transport handle, actually
static void NET_PRES_SignalHandler(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    const NET_PRES_SocketData * pSkt = (const NET_PRES_SocketData*)param;
    F_NET_PRES_AssertCond(pSkt->transHandle == handle,  __func__, __LINE__);
    F_NET_PRES_AssertCond(pSkt->sigHandle != 0,  __func__, __LINE__);
    F_NET_PRES_AssertCond(pSkt->usrSigFnc != 0,  __func__, __LINE__);

    // call the user handler
    if(pSkt->usrSigFnc != NULL)
    {
        ptrdiff_t sktDiff = pSkt - sNetPresSockets;
        uint16_t sktIx = (uint16_t)sktDiff + 1U;
        (*pSkt->usrSigFnc)(sktIx, hNet, sigType, pSkt->usrSigParam);
    }

}


NET_PRES_SIGNAL_HANDLE NET_PRES_SocketSignalHandlerRegister(NET_PRES_SKT_HANDLE_T handle, uint16_t sigMask, NET_PRES_SIGNAL_FUNCTION handler, const void* hParam)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return NULL;
    }

    if (handler == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_HANDLER_ERROR;
        return NULL;
    }

    NET_PRES_TransHandlerRegister fp = pSkt->transObject->fpHandlerRegister;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return NULL;
    }

    if(pSkt->sigHandle != NULL)
    {   // one socket has just one signal handler
        pSkt->lastError = (int8_t)NET_PRES_SKT_HANDLER_BUSY;
        return NULL;
    }

    pSkt->usrSigFnc = handler;
    pSkt->usrSigParam = hParam;
    pSkt->sigHandle = (*fp)(pSkt->transHandle, sigMask, &NET_PRES_SignalHandler, pSkt);    

    if(pSkt->sigHandle == NULL)
    {   // failed
        pSkt->lastError = (int8_t)NET_PRES_SKT_HANDLER_TRANSP_ERROR;
    }


    return pSkt->sigHandle;

}

bool NET_PRES_SocketSignalHandlerDeregister(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SIGNAL_HANDLE hSig)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    NET_PRES_TransSignalHandlerDeregister fp = pSkt->transObject->fpHandlerDeregister;
    if (fp == NULL)
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_OP_NOT_SUPPORTED;
        return false;
    }

    if(pSkt->sigHandle == NULL || pSkt->sigHandle != hSig)
    {   // no such signal handler
        pSkt->lastError = (int8_t)NET_PRES_SKT_HANDLER_ERROR;
        return false;
    }

    bool res = (*fp)(pSkt->transHandle, pSkt->sigHandle);
    if(res)
    {   // done
        pSkt->sigHandle = NULL;
    }
    else
    {
        pSkt->lastError = (int8_t)NET_PRES_SKT_HANDLER_TRANSP_ERROR;
    }

    return res;
}

bool NET_PRES_SocketIsNegotiatingEncryption(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }
    return ((pSkt->status == (uint8_t)NET_PRES_ENC_SS_CLIENT_NEGOTIATING) ||  
            (pSkt->status == (uint8_t)NET_PRES_ENC_SS_SERVER_NEGOTIATING) || 
            (pSkt->status == (uint8_t)NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION));
}
bool NET_PRES_SocketIsSecure(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    return pSkt->status == (uint8_t)NET_PRES_ENC_SS_OPEN;
}
bool NET_PRES_SocketEncryptSocket(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return false;
    }

    if ((pSkt->socketType & ((uint16_t)NET_PRES_SKT_UNENCRYPTED | (uint16_t)NET_PRES_SKT_ENCRYPTED)) != (uint16_t)NET_PRES_SKT_UNENCRYPTED)
    {
        return false;
    }
    if(pSkt->provObject->fpInit == NULL || pSkt->provObject->fpOpen == NULL || pSkt->provObject->fpIsInited == NULL)
    {   // cannot start negotiation
        return false;
    }

    pSkt->socketType ^= (uint16_t)NET_PRES_SKT_UNENCRYPTED | (uint16_t)NET_PRES_SKT_ENCRYPTED;
    pSkt->status = (uint8_t)NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION;
    return true;
}

bool NET_PRES_SocketIsOpenModeSupported(NET_PRES_INDEX index, NET_PRES_SKT_T socketType)
{
    NET_PRES_TransOpen fpTransOpen = NULL;
    Net_ProvOpen fpProvOpen= NULL;

    // Check to see if we have a valid index
    if (index >= sNetPresData.numLayers)
    {
        return false;
    }

    // Check to see if the operation is supported
    if (((uint16_t)socketType & ((uint16_t)NET_PRES_SKT_CLIENT | (uint16_t)NET_PRES_SKT_STREAM)) ==  ((uint16_t)NET_PRES_SKT_CLIENT | (uint16_t)NET_PRES_SKT_STREAM))
    {
        fpProvOpen = sNetPresData.encProvObjectSC[index].fpOpen;
        fpTransOpen = sNetPresData.transObjectSC[index].fpOpen;
    }
    else if (((uint16_t)socketType & ((uint16_t)NET_PRES_SKT_SERVER | (uint16_t)NET_PRES_SKT_STREAM)) ==  ((uint16_t)NET_PRES_SKT_SERVER | (uint16_t)NET_PRES_SKT_STREAM))
    {
        fpProvOpen = sNetPresData.encProvObjectSS[index].fpOpen;
        fpTransOpen = sNetPresData.transObjectSS[index].fpOpen;
    }
    else if (((uint16_t)socketType & ((uint16_t)NET_PRES_SKT_CLIENT | (uint16_t)NET_PRES_SKT_DATAGRAM)) ==  ((uint16_t)NET_PRES_SKT_CLIENT | (uint16_t)NET_PRES_SKT_DATAGRAM))
    {
        fpProvOpen = sNetPresData.encProvObjectDC[index].fpOpen;
        fpTransOpen = sNetPresData.transObjectDC[index].fpOpen;
    }
    else if (((uint16_t)socketType & ((uint16_t)NET_PRES_SKT_SERVER | (uint16_t)NET_PRES_SKT_DATAGRAM)) ==  ((uint16_t)NET_PRES_SKT_SERVER | (uint16_t)NET_PRES_SKT_DATAGRAM))
    {
        fpProvOpen = sNetPresData.encProvObjectDS[index].fpOpen;
        fpTransOpen = sNetPresData.transObjectDS[index].fpOpen;
    }
    else
    {
        // do nothing
    }

    if (fpTransOpen == NULL)
    {
        return false;        
    }
    bool encrypted = ((uint16_t)socketType & (uint16_t)NET_PRES_SKT_ENCRYPTED) == (uint16_t)NET_PRES_SKT_ENCRYPTED;
    
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
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return NET_PRES_SKT_INVALID_SOCKET;
    }

    NET_PRES_SKT_ERROR_T lastError = (NET_PRES_SKT_ERROR_T)pSkt->lastError;
    pSkt->lastError = (int8_t)NET_PRES_SKT_OK;

    return lastError;
}

NET_PRES_SKT_HANDLE_T NET_PRES_SocketGetTransportHandle(NET_PRES_SKT_HANDLE_T handle)
{
    NET_PRES_SocketData * pSkt;
    if ((pSkt = F_NET_PRES_SocketValidate(handle)) == NULL)
    {
        return (NET_PRES_SKT_HANDLE_T)NET_PRES_SKT_INVALID_SOCKET;
    }

    pSkt->lastError = (int8_t)NET_PRES_SKT_OK;
    return pSkt->transHandle;
    
}
NET_PRES_CBACK_HANDLE NET_PRES_SniCallbackRegister(SYS_MODULE_OBJ obj, NET_PRES_SNI_CALLBACK cBack)
{
    if(cBack == NULL)
    {
        return NULL;
    }

    NET_PRES_InternalData * pData = (NET_PRES_InternalData*)obj;
    if (pData != &sNetPresData || pData->initialized == false)
    {
        return NULL;
    }

    union
    {
        NET_PRES_CBACK_HANDLE cbHandle;
        NET_PRES_SNI_CALLBACK sniCback;
    }U_SNI_CBACK_HANDLE;

    U_SNI_CBACK_HANDLE.cbHandle = NULL;
    (void)OSAL_MUTEX_Lock(&pData->presMutex, OSAL_WAIT_FOREVER);
    if(pData->sniCback == NULL)
    {
        U_SNI_CBACK_HANDLE.sniCback = cBack;
        pData->sniCback = U_SNI_CBACK_HANDLE.cbHandle;
    }

    (void)OSAL_MUTEX_Unlock(&pData->presMutex);

    return U_SNI_CBACK_HANDLE.cbHandle;
}

bool NET_PRES_SniCallbackDeregister(SYS_MODULE_OBJ obj, NET_PRES_CBACK_HANDLE cBackHandle)
{
    NET_PRES_InternalData * pData = (NET_PRES_InternalData*)obj;
    if (pData != &sNetPresData || pData->initialized == false)
    {
        return false;
    }

    bool res = false;
    (void)OSAL_MUTEX_Lock(&pData->presMutex, OSAL_WAIT_FOREVER);
    if(pData->sniCback == cBackHandle)
    {
        pData->sniCback = NULL;
        res = true;
    }

    (void)OSAL_MUTEX_Unlock(&pData->presMutex);

    return res;
}

NET_PRES_SNI_CALLBACK NET_PRES_SniCallbackGet(SYS_MODULE_OBJ obj)
{
    NET_PRES_InternalData * pData = (NET_PRES_InternalData*)obj;
    if (pData != &sNetPresData || pData->initialized == false)
    {
        return NULL;
    }

    union
    {
        NET_PRES_CBACK_HANDLE cbHandle;
        NET_PRES_SNI_CALLBACK sniCback;
    }U_SNI_CBACK_HANDLE;

    U_SNI_CBACK_HANDLE.cbHandle = pData->sniCback;
    return U_SNI_CBACK_HANDLE.sniCback;
}
