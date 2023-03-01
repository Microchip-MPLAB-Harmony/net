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
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
    return TCPIP_Helper_ProtectedSingleListInitialize(notifyList);
}


void TCPIP_Notification_Deinitialize(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_Notification_RemoveAll(notifyList, heapH);
    TCPIP_Helper_ProtectedSingleListDeinitialize(notifyList);
}



SGL_LIST_NODE* TCPIP_Notification_Add(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH, void* pContent, size_t nBytes)
{
    SGL_LIST_NODE* newNode = TCPIP_HEAP_Malloc(heapH, nBytes);

    if(newNode)
    {
        if(pContent)
        {
            memcpy(newNode, pContent, nBytes);
        }
        TCPIP_Helper_ProtectedSingleListTailAdd(notifyList, newNode);
    }
    return newNode;
}



bool TCPIP_Notification_Remove(SGL_LIST_NODE* node, PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)
{
    if(TCPIP_Helper_ProtectedSingleListNodeRemove(notifyList, node))
    {
        TCPIP_HEAP_Free(heapH, node);
        return true;
    }

    return false;

}

bool TCPIP_Notification_CbackRemove(SGL_LIST_NODE* node, PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH, void (*pCback)(SGL_LIST_NODE* node))
{
    if(TCPIP_Helper_ProtectedSingleListLock(notifyList))
    {
        SGL_LIST_NODE* ret = TCPIP_Helper_SingleListNodeRemove(&notifyList->list, node);
        if(ret && pCback)
        {
            (*pCback)(ret);
        }

        TCPIP_Helper_ProtectedSingleListUnlock(notifyList);

        if(ret)
        {
            TCPIP_HEAP_Free(heapH, node);
            return true;
        }
    }
    return false;
}

void TCPIP_Notification_RemoveAll(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)
{
    SGL_LIST_NODE* dNode;

    while( (dNode = TCPIP_Helper_ProtectedSingleListHeadRemove(notifyList)) != 0 )
    {
        TCPIP_HEAP_Free(heapH, dNode);
    }

}




