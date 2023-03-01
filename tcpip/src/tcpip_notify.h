/*******************************************************************************
  TCP/IP notifications Header file

  Company:
    Microchip Technology Inc.

  File Name:
   tcpip_notify.h

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

#ifndef __TCPIP_NOTIFY_H_
#define __TCPIP_NOTIFY_H_

#include <stdint.h>
#include <stdbool.h>


// *****************************************************************************
// *****************************************************************************
// Section: API definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
Function:
    bool      TCPIP_Notification_Initialize(PROTECTED_SINGLE_LIST* notifyList)

  Summary:
    Initializes a notification list

  Description:
    Initializes a notification list and makes it ready for use.

  Precondition:
    notifyList is a valid list

  Parameters:
    notifyList  - address of the list to be initialized    

  Returns:
        On Success - true
        On Failure - false

  Example:
     None

  Remarks:
    This call creates the protection object (semaphire) associated with this list.
*/
bool        TCPIP_Notification_Initialize(PROTECTED_SINGLE_LIST* notifyList);



// *****************************************************************************
/*
Function:
    void      TCPIP_Notification_Deinitialize(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)

  Summary:
    Deinitializes a notification list

  Description:
    Deinitializes a notification list and frees associated resources

  Precondition:
    notifyList is a valid initialized list

  Parameters:
    notifyList  - address of the list to be deinitialized    
    heapH       - Heap to be used for freeing node resources

  Returns:
        None

  Example:
     None

  Remarks:
    This call deletes the protection object (semaphire) associated with this list.

    heapH has to match the one that was used for adding nodes
*/
void        TCPIP_Notification_Deinitialize(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH);




// *****************************************************************************
/*
Function:
    SGL_LIST_NODE*      TCPIP_Notification_Add(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH, void* pContent, size_t nBytes)

  Summary:
    Adds a new notification

  Description:
    Tries to create a new SGL_LIST_NODE and add it to the tail of the notifyList.

  Precondition:
    the module notifyList should have been initialized

  Parameters:
    notifyList  - address of the list where the new entry is to be added    
    heapH       - Heap to be used for adding the new node.
                  This could be module specific.
    pContent    - the contents to be copied in the new entry, if allocation is successful
                  exactly nBytes will be copied
                  Could be NULL if not needed 
    nBytes      - size of the entry - module specific

  Returns:
    SGL_LIST_NODE pointer to the created node 
        On Success - Returns a valid pointer
        On Failure - null pointer if memory call failed

  Example:
     None

  Remarks:
    It is up to each module to set the specific data associated with this entry.
    This function only creates a new node and inserts it properly in the notification list.

    pContent refers to the whole node size, including the 'next' field.
    However, the 'next' field will be overwritten when the node is interted into the list.
    
*/
SGL_LIST_NODE*      TCPIP_Notification_Add(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH, void* pContent, size_t nBytes);


// *****************************************************************************
/*
Function:
    bool      TCPIP_Notification_Remove(SGL_LIST_NODE* node, PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)

  Summary:
    Removes a notification 

  Description:
    Tries to remove a SGL_LIST_NODE from the notifyList.

  Precondition:
    the node should have been added to the notifyList with TCPIP_Notification_Add()

  Parameters:
    node        - node to be deregistered
    notifyList  - address of the list from where the new entry is to be removed    
    heapH       - Heap to be used for freeing up memory
                  This could be module specific.

  Returns:
        true  - for success
        false - if failure

  Example:
     None

  Remarks:
    It is up to each module to remove/free the specific data associated with this entry.
    This function only removes the node from the notification list and then frees the associated memory
*/
bool      TCPIP_Notification_Remove(SGL_LIST_NODE* node, PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH);

// *****************************************************************************

/*
Function:
    bool      TCPIP_Notification_CbackRemove(SGL_LIST_NODE* node, PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH, void (*pCback)(SGL_LIST_NODE* node));

  Summary:
    Removes a notification and calls a user function

  Description:
    Tries to remove a SGL_LIST_NODE from the notifyList.
    If needed, it will call a user function with the list safely locked

  Precondition:
    the node should have been added to the notifyList with TCPIP_Notification_Add()

  Parameters:
    node        - node to be deregistered
    notifyList  - address of the list from where the new entry is to be removed    
    heapH       - Heap to be used for freeing up memory
                  This could be module specific.
    pCback      - function to be called in safe context if the node was successfuly removed from the list
                  Node contents is still valid at that time.
                  Should be a quick function
                  Could be NULL if not needed 
                

  Returns:
        true  - for success
        false - if failure

  Example:
     None

  Remarks:
    It is up to each module to remove/free the specific data associated with this entry.
    This function only removes the node from the notification list and then frees the associated memory
*/
bool      TCPIP_Notification_CbackRemove(SGL_LIST_NODE* node, PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH, void (*pCback)(SGL_LIST_NODE* node));

// *****************************************************************************
/*
Function:
    void      TCPIP_Notification_RemoveAll(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)

  Summary:
    Removes all notifications from a list 

  Description:
    Tries to remove all notifications from the notifyList.

  Precondition:
    the nodes should have been added to the notifyList with TCPIP_Notification_Add()

  Parameters:
    heapH       - Heap to be used for freeing up memory
                  This could be module specific.
    notifyList  - address of the list from where the new entry is to be removed    

  Returns:
        None

  Example:
     None

  Remarks:
    It is up to each module to remove/free the specific data associated with this entry.
    This function only removes the node from the notification list and then frees the associated memory
*/
void      TCPIP_Notification_RemoveAll(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH);


// *****************************************************************************
/*
Function:
    void      TCPIP_Notification_Lock(PROTECTED_SINGLE_LIST* notifyList)

  Summary:
    Locks access to a notification list 

  Description:
    Locks access to a notification list 
    The list can be traversed safely.

  Precondition:
    The notifyList should have been properly initialized

  Parameters:
    notifyList  - list to lock 

  Returns:
        None

  Example:
     None

  Remarks:
    None
*/
static __inline__ void __attribute__((always_inline)) TCPIP_Notification_Lock(PROTECTED_SINGLE_LIST* notifyList)
{
    TCPIP_Helper_ProtectedSingleListLock(notifyList);
}

// *****************************************************************************
/*
Function:
    void      TCPIP_Notification_Unlock(PROTECTED_SINGLE_LIST* notifyList)

  Summary:
    Unlocks access to a notification list 

  Description:
    Unlocks access to a notification list 

  Precondition:
    The notifyList should have been properly initialized and locked
    before this call

  Parameters:
    notifyList  - list to unlock 

  Returns:
        None

  Example:
     None

  Remarks:
    None
*/
static __inline__ void __attribute__((always_inline)) TCPIP_Notification_Unlock(PROTECTED_SINGLE_LIST* notifyList)
{
    TCPIP_Helper_ProtectedSingleListUnlock(notifyList);
}



#endif  // __TCPIP_NOTIFY_H_



