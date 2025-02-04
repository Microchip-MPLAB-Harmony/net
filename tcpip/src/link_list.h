/*******************************************************************************
  Linked list helper file

  Company:
    Microchip Technology Inc.
    
  File Name:
    link_list.h

  Summary:
    Linked lists manipulation Interface Header
    
  Description:
    This header file contains the function prototypes and definitions of the 
    linked lists manipulation routines
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END

#ifndef H_LINK_LISTS_H_
#define H_LINK_LISTS_H_

#include "osal/osal.h"
#include <stdbool.h>

typedef struct S_TAG_SGL_LIST_NODE
{
    struct S_TAG_SGL_LIST_NODE*      next;       // next node in list
    // void*                        data[];     // generic payload    
}SGL_LIST_NODE; // generic linked list node definition


typedef struct
{
    SGL_LIST_NODE*  head;   // list head
    SGL_LIST_NODE*  tail;
    size_t          nNodes; // number of nodes in the list

}SINGLE_LIST;   // single linked list


/////  single linked lists manipulation ///////////
//


void  TCPIP_Helper_SingleListInitialize(SINGLE_LIST* pL);


static __inline__ bool __attribute__((always_inline)) TCPIP_Helper_SingleListIsEmpty(SINGLE_LIST* pL)
{
    return pL->head == NULL;
}


static __inline__ size_t __attribute__((always_inline)) TCPIP_Helper_SingleListCount(SINGLE_LIST* pL)
{
    return pL->nNodes;
}

void  TCPIP_Helper_SingleListHeadAdd(SINGLE_LIST* pL, SGL_LIST_NODE* pN);

void  TCPIP_Helper_SingleListTailAdd(SINGLE_LIST* pL, SGL_LIST_NODE* pN);


// insertion in the middle, not head or tail
// Note: no verification is made for 'after'. It should be a valid list node!
void  TCPIP_Helper_SingleListMidAdd(SINGLE_LIST* pL, SGL_LIST_NODE* pN, SGL_LIST_NODE* after);

// insertion at head, at tail or in the middle
// If after == NULL, it will insert at head
// Note: no verification is made for 'after'. It should be a valid list node!
void    TCPIP_Helper_SingleListAdd(SINGLE_LIST* pL, SGL_LIST_NODE* pN, SGL_LIST_NODE* after);

// removes the head node
SGL_LIST_NODE*  TCPIP_Helper_SingleListHeadRemove(SINGLE_LIST* pL);

// removes the next node (following prev) in the list
// if prev == 0 removed the head
SGL_LIST_NODE*  TCPIP_Helper_SingleListNextRemove(SINGLE_LIST* pL, SGL_LIST_NODE* prev);


// removes a node anywhere in the list
// Note: this is lengthy!
// Use a double linked list if faster operation needed!
SGL_LIST_NODE*  TCPIP_Helper_SingleListNodeRemove(SINGLE_LIST* pL, SGL_LIST_NODE* pN);

// concatenates the append list to the destination one
void  TCPIP_Helper_SingleListAppend(SINGLE_LIST* pDstL, SINGLE_LIST* pAList);



// no memory de-allocation is performed, just removes the nodes from the list
static __inline__ void __attribute__((always_inline)) TCPIP_Helper_SingleListRemoveAll(SINGLE_LIST* pL)
{
    while(TCPIP_Helper_SingleListHeadRemove(pL) != NULL)
    {
        // do nothing
    }
}

// returns true if pN belongs to pL; false otherwise
// expensive, traverses the list
bool        TCPIP_Helper_SingleListFind(SINGLE_LIST* pL, SGL_LIST_NODE* pN);

// Single linked protected list /////
typedef struct
{
    SINGLE_LIST list;
    OSAL_SEM_HANDLE_TYPE semaphore;
    bool   semValid;
} PROTECTED_SINGLE_LIST;

// creates an empty single linked list and associated semaphore
// the list should NOT be used if the initialization failed!
// However, TCPIP_Helper_ProtSglListDeinitialize() can be safely called
bool  TCPIP_Helper_ProtSglListInitialize(PROTECTED_SINGLE_LIST* pL);

// removes all nodes from a single linked list and deletes the associated semaphore
// no memory de-allocation is performed, just removes the nodes from the list
void  TCPIP_Helper_ProtSglListDeinitialize(PROTECTED_SINGLE_LIST* pL);

static __inline__ bool __attribute__((always_inline)) TCPIP_Helper_ProtSglListIsEmpty(PROTECTED_SINGLE_LIST* pL)
{
    return TCPIP_Helper_SingleListIsEmpty(&pL->list);
}


static __inline__ size_t __attribute__((always_inline)) TCPIP_Helper_ProtSglListCount(PROTECTED_SINGLE_LIST* pL)
{
    return TCPIP_Helper_SingleListCount(&pL->list);
}

void  TCPIP_Helper_ProtSglListHeadAdd(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* pN);

void  TCPIP_Helper_ProtSglListTailAdd(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* pN);


// insertion in the middle, not head or tail
// Note: no verification is made for 'after'. It should be a valid list node!
void  TCPIP_Helper_ProtSglListMidAdd(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* pN, SGL_LIST_NODE* after);

// insertion at head, at tail or in the middle
// If after == NULL, it will insert at head
//
// Note: no verification is made for 'after'. It should be a valid list node!
void    TCPIP_Helper_ProtSglListAdd(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* pN, SGL_LIST_NODE* after);

// removes the head node
SGL_LIST_NODE*  TCPIP_Helper_ProtSglListHeadRemove(PROTECTED_SINGLE_LIST* pL);

// removes the next node (following prev) in the list
// if prev == 0 removed the head
SGL_LIST_NODE*  TCPIP_Helper_ProtSglListNextRemove(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* prev);


// removes a node anywhere in the list
// Note: this is lengthy!
// Use a double linked list if faster operation needed!
SGL_LIST_NODE*  TCPIP_Helper_ProtSglListNodeRemove(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* pN);


// concatenates the append list to the destination one
void  TCPIP_Helper_ProtSglListAppend(PROTECTED_SINGLE_LIST* pDstL, SINGLE_LIST* pAList);


// removes all nodes from a single linked list
// no memory de-allocation is performed, just removes the nodes from the list
// after this operation the list is valid, i.e. the operation
// does not delete the associated semaphore
void TCPIP_Helper_ProtSglListRemoveAll(PROTECTED_SINGLE_LIST* pL);


// locks access to a protected single list
// the list should have been properly initialized
bool TCPIP_Helper_ProtSglListLock(PROTECTED_SINGLE_LIST* pL);

// unlocks access to a protected single list
// the list should have been properly initialized and lock acquired
bool TCPIP_Helper_ProtSglListUnlock(PROTECTED_SINGLE_LIST* pL);

/////  double linked lists manipulation ///////////
//

typedef struct S_TAG_DBL_LIST_NODE
{
    struct S_TAG_DBL_LIST_NODE*      next;       // next node in list
    struct S_TAG_DBL_LIST_NODE*      prev;       // prev node in list
    // void*                        data[];     // generic payload    
}DBL_LIST_NODE; // generic linked list node definition


typedef struct
{
    DBL_LIST_NODE*  head;   // list head
    DBL_LIST_NODE*  tail;   // list tail;
    size_t          nNodes; // number of nodes in the list 
}DOUBLE_LIST;   // double linked list


void  TCPIP_Helper_DoubleListInitialize(DOUBLE_LIST* pL);


static __inline__ bool __attribute__((always_inline)) TCPIP_Helper_DoubleListIsEmpty(DOUBLE_LIST* pL)
{
    return pL->head == NULL;
}

static __inline__ size_t __attribute__((always_inline)) TCPIP_Helper_DoubleListCount(DOUBLE_LIST* pL)
{
    return pL->nNodes;
}

void  TCPIP_Helper_DoubleListHeadAdd(DOUBLE_LIST* pL, DBL_LIST_NODE* pN);

void  TCPIP_Helper_DoubleListTailAdd(DOUBLE_LIST* pL, DBL_LIST_NODE* pN);

// add node pN in the middle, after existing node "after"
// Note: no verification is made for 'after'. It should be a valid list node!
void  TCPIP_Helper_DoubleListMidAdd(DOUBLE_LIST* pL, DBL_LIST_NODE* pN, DBL_LIST_NODE* after);

// insertion at head, at tail or in the middle
// If after == NULL, it will insert at head
// Note: no verification is made for 'after'. It should be a valid list node!
void  TCPIP_Helper_DoubleListAdd(DOUBLE_LIST* pL, DBL_LIST_NODE* pN, DBL_LIST_NODE* after);

DBL_LIST_NODE*  TCPIP_Helper_DoubleListHeadRemove(DOUBLE_LIST* pL);

DBL_LIST_NODE*  TCPIP_Helper_DoubleListTailRemove(DOUBLE_LIST* pL);

// remove existing node, neither head, nor tail
void  TCPIP_Helper_DoubleListMidRemove(DOUBLE_LIST* pL, DBL_LIST_NODE* pN);

void  TCPIP_Helper_DoubleListNodeRemove(DOUBLE_LIST* pL, DBL_LIST_NODE* pN);

// no memory de-allocation is performed, just removes the nodes from the list
static __inline__ void __attribute__((always_inline)) TCPIP_Helper_DoubleListRemoveAll(DOUBLE_LIST* pL)
{
    while(TCPIP_Helper_DoubleListHeadRemove(pL) != NULL)
    {
        // do nothing
    }
}

// returns true if pN belongs to pL; false otherwise
// expensive, traverses the list
bool        TCPIP_Helper_DoubleListFind(DOUBLE_LIST* pL, DBL_LIST_NODE* pN);


typedef struct
{
    DOUBLE_LIST list;
    OSAL_SEM_HANDLE_TYPE semaphore;
    bool semValid;
}PROTECTED_DOUBLE_LIST; // double linked list


// creates an empty double linked list and associated semaphore
// the list should NOT be used if the initialization failed!
// However, TCPIP_Helper_ProtDblListDeinitialize() can be safely called
bool  TCPIP_Helper_ProtDblListInitialize(PROTECTED_DOUBLE_LIST* pL);

// removes all nodes from a double linked list and deletes the associated semaphore
// no memory de-allocation is performed, just removes the nodes from the list
void  TCPIP_Helper_ProtDblListDeinitialize(PROTECTED_DOUBLE_LIST* pL);

static __inline__ bool __attribute__((always_inline)) TCPIP_Helper_ProtDblListIsEmpty(PROTECTED_DOUBLE_LIST* pL)
{
    return TCPIP_Helper_DoubleListIsEmpty(&pL->list);
}

static __inline__ size_t __attribute__((always_inline)) TCPIP_Helper_ProtDblListCount(PROTECTED_DOUBLE_LIST* pL)
{
    return TCPIP_Helper_DoubleListCount(&pL->list);
}

void  TCPIP_Helper_ProtDblListHeadAdd(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN);

void  TCPIP_Helper_ProtDblListTailAdd(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN);

// add node pN in the middle, after existing node "after"
// Note: no verification is made for 'after'. It should be a valid list node!
void  TCPIP_Helper_ProtDblListMidAdd(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN, DBL_LIST_NODE* after);

// insertion at head, at tail or in the middle
// If after == NULL, it will insert at head
// Note: no verification is made for 'after'. It should be a valid list node!
void  TCPIP_Helper_ProtDblListAdd(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN, DBL_LIST_NODE* after);

DBL_LIST_NODE*  TCPIP_Helper_ProtDblListHeadRemove(PROTECTED_DOUBLE_LIST* pL);

DBL_LIST_NODE*  TCPIP_Helper_ProtDblListTailRemove(PROTECTED_DOUBLE_LIST* pL);

// remove existing node, neither head, nor tail
void  TCPIP_Helper_ProtDblListMidRemove(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN);

void  TCPIP_Helper_ProtDblListNodeRemove(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN);

// removes all nodes from a double linked list
// no memory de-allocation is performed, just removes the nodes from the list
// after this operation the list is valid, i.e. the operation
// does not delete the associated semaphore
void TCPIP_Helper_ProtDblListRemoveAll(PROTECTED_DOUBLE_LIST* pL);


// locks access to a protected double list
// the list should have been properly initialized
void TCPIP_Helper_ProtDblListLock(PROTECTED_DOUBLE_LIST* pL);

// unlocks access to a protected double list
// the list should have been properly initialized and lock acquired
void TCPIP_Helper_ProtDblListUnlock(PROTECTED_DOUBLE_LIST* pL);

#endif //  H_LINK_LISTS_H_


