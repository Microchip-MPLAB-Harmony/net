/*******************************************************************************
  TCP/IP notifications implementation file

  Company:
    Microchip Technology Inc.

  File Name:
   tcpip_notify.c

  Summary:
   TCPIP notifications mechanism header file

  Description:
    This source file contains the internal notifications API
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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








//DOM-IGNORE-END

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MANAGER

#include "tcpip/src/tcpip_private.h"


bool TCPIP_Notification_Initialize(PROTECTED_SINGLE_LIST* notifyList)
{
    return TCPIP_Helper_ProtSglListInitialize(notifyList);
}


void TCPIP_Notification_Deinitialize(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_Notification_RemoveAll(notifyList, heapH);
    TCPIP_Helper_ProtSglListDeinitialize(notifyList);
}



SGL_LIST_NODE* TCPIP_Notification_Add(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH, void* pContent, size_t nBytes)
{
    SGL_LIST_NODE* newNode = TCPIP_HEAP_Malloc(heapH, nBytes);

    if(newNode != NULL)
    {
        if(pContent != NULL)
        {
           (void) memcpy((void *)newNode, pContent, nBytes);
        }
        TCPIP_Helper_ProtSglListTailAdd(notifyList, newNode);
    }
    return newNode;
}



bool TCPIP_Notification_Remove(SGL_LIST_NODE* node, PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)
{
    bool status = false;
    
    if(TCPIP_Helper_ProtSglListNodeRemove(notifyList, node) != NULL)
    {
        (void) TCPIP_HEAP_Free(heapH, node);
        status = true;
    }

    return status;

}

bool TCPIP_Notification_CbackRemove(SGL_LIST_NODE* node, PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH, void (*pCback)(SGL_LIST_NODE* node))
{
    bool status = false;
    
    if(TCPIP_Helper_ProtSglListLock(notifyList))
    {
        SGL_LIST_NODE* ret = TCPIP_Helper_SingleListNodeRemove(&notifyList->list, node);
        if(ret != NULL && pCback != NULL)
        {
            (*pCback)(ret);
        }

        (void)TCPIP_Helper_ProtSglListUnlock(notifyList);

        if(ret != NULL)
        {
            (void)TCPIP_HEAP_Free(heapH, node);
            status = true;
        }
    }
    return status;
}

void TCPIP_Notification_RemoveAll(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)
{
    SGL_LIST_NODE* dNode;

    while( (dNode = TCPIP_Helper_ProtSglListHeadRemove(notifyList)) != NULL )
    {
        (void)TCPIP_HEAP_Free(heapH, dNode);
    }

}




