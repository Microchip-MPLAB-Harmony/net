/*******************************************************************************
  MIIM Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim_local.h

  Summary:
    MIIM driver local declarations and definitions.

  Description:
    This file contains the MIIM driver's local declarations and definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2013-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _DRV_MIIM_LOCAL_H
#define _DRV_MIIM_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "system_config.h"
#include "driver/miim/drv_miim.h"

#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// maximum register value that can be accessed.
// This driver uses standard MMIM routines, and has 5 bits for register index
#define DRV_MIIM_MAX_REG_INDEX_VALUE        0x1f

// maximum PHY address value that can be used.
// This driver uses standard MMIM routines, and has 5 bits for PHY address
#define DRV_MIIM_MAX_ADDRESS_VALUE        0x1f


// debugging
#define DRV_MIIM_DEBUG_MASK_BASIC           (0x0001)
#define DRV_MIIM_DEBUG_MASK_RW              (0x0002)   // debug Read/Write operations
#define DRV_MIIM_DEBUG_MASK_OPER            (0x0004)   // debug MIIM operations
#define DRV_MIIM_DEBUG_MASK_EXT_SMI_RW      (0x0008)   // debug extended SMI Read/Write operations

// enable MIIM debugging levels
#define DRV_MIIM_DEBUG_LEVEL  (0x01)



// private object definitions
//

typedef struct _TAG_SGL_LIST_NODE
{
    struct _TAG_SGL_LIST_NODE*      next;       // next node in list
    void*                           data[];     // generic payload    
}SGL_LIST_NODE; // generic linked list node definition


typedef struct
{
    SGL_LIST_NODE*  head;   // list head
    SGL_LIST_NODE*  tail;
    int             nNodes; // number of nodes in the list

}SINGLE_LIST;   // single linked list


#if (DRV_MIIM_CLIENT_OP_PROTECTION)
// client identification structure
typedef struct _tag_DRV_MIIM_CLI_OP_STAMP
{
    struct _tag_DRV_MIIM_CLI_OP_STAMP* next;       // safe cast to single list node
    struct _tag_DRV_MIIM_OP_DCPT*   pOpDcpt;    // descriptor describing the operation
    struct _tag_DRV_MIIM_CLIENT_DCPT* pClientDcpt;   // owner client 
    uint32_t                        tStamp;     // time at which operation was started
}DRV_MIIM_CLI_OP_STAMP;

#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)
// MIIM operation types
typedef enum
{
    DRV_MIIM_OP_NONE    = 0,        // no operation started
    DRV_MIIM_OP_READ,               // read operation 
    DRV_MIIM_OP_WRITE,              // write operation 
    DRV_MIIM_OP_SCAN,               // scan operation 
    DRV_MIIM_OP_READ_EXT,           // extended read operation 
    DRV_MIIM_OP_WRITE_EXT,          // extended write operation 
}DRV_MIIM_OP_TYPE;


// MIIM operation transfer status
typedef enum
{
    DRV_MIIM_TXFER_NONE = 0,      // operation not started
    DRV_MIIM_TXFER_START,         // command to start the operation
    DRV_MIIM_TXFER_RDWR_WAIT_COMPLETE, // read/write operation waiting to complete
    DRV_MIIM_TXFER_EXT_WAIT_PHASE1, // extended operation waiting to complete phase 1
    DRV_MIIM_TXFER_SCAN_STALE,    // scan operation ongoing, stale result
    DRV_MIIM_TXFER_REPORT_STATE,  // from this state on reports are needed
    DRV_MIIM_TXFER_SCAN_VALID = DRV_MIIM_TXFER_REPORT_STATE,    // scan operation ongoing, fresh result available
    DRV_MIIM_TXFER_DONE,          // operation completed successfully
    DRV_MIIM_TXFER_ERROR,         // unexpected error has occurred

} DRV_MIIM_TXFER_STAT;

// MIIM operation report actions
typedef enum
{
    DRV_MIIM_REPORT_ACT_NONE = 0,   // no action is required
    DRV_MIIM_REPORT_ACT_DELETE,     // operation needs to be deleted
    DRV_MIIM_REPORT_ACT_WAIT_COMPLETE, // operation needs to be stored in the complete list
} DRV_MIIM_REPORT_ACT;

//  queue to which an operation may belong
typedef enum
{
    DRV_MIIM_QTYPE_NONE = 0,        // invalid
    DRV_MIIM_QTYPE_FREE,            // free, available
    DRV_MIIM_QTYPE_BUSY,            // busy, scheduled
    DRV_MIIM_QTYPE_COMPLETE,        // completed but waiting for client acknowledge
}DRV_MIIM_QUEUE_TYPE;

// MIIM operation descriptor
typedef struct _tag_DRV_MIIM_OP_DCPT
{
    struct _tag_DRV_MIIM_OP_DCPT*   next;       // safe cast to a SGL_LIST_NODE
    uint16_t                        regIx;      // register for the operation
    uint16_t                        phyAdd;     // PHY address to use for the operation
    uint32_t                        opData;     // associated data
    uint8_t                         opType;     // DRV_MIIM_OP_TYPE: type of operation in progress
    uint8_t                         opFlags;    // DRV_MIIM_OPERATION_FLAGS: flags associated with the operation
    uint8_t                         txferStat;  // DRV_MIIM_TXFER_STAT value: current operation transfer status
    uint8_t                         qType;      // DRV_MIIM_QUEUE_TYPE value: current queue the operation is in
    struct _tag_DRV_MIIM_CLIENT_DCPT* pOwner;    // owner of this operation 
#if (DRV_MIIM_CLIENT_OP_PROTECTION)
    DRV_MIIM_CLI_OP_STAMP*          pCliStamp;  // client stamp to match
    uint32_t                        tStamp;     // time at which operation was started
#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)
}DRV_MIIM_OP_DCPT;


typedef struct _tag_DRV_MIIM_CLIENT_DCPT
{
    uint16_t                            clientInUse;    // True if in use
    uint16_t                            clientIx;       // client number
    struct _tag_DRV_MIIM_OBJ_STRUCT*    parentObj;      // parent object to which this client belongs
    DRV_MIIM_OPERATION_CALLBACK         cbackHandler;   // client notification when operation is done
    DRV_MIIM_CLIENT_STATUS              cliStatus;      // Client Status
}DRV_MIIM_CLIENT_DCPT;

//  MIIM Object Flags
//  Currently 16 bits only
typedef enum
{
    DRV_MIIM_OBJ_FLAG_NONE          = 0x00,     // default, no flag set
    DRV_MIIM_OBJ_FLAG_IN_USE        = 0x01,     // object properly initialized and in use
    DRV_MIIM_OBJ_FLAG_SETUP_DONE    = 0x02,     // at least one client called the setup routine
    DRV_MIIM_OBJ_FLAG_EXCLUSIVE     = 0x04,     // some client opened in exclusive mode
    DRV_MIIM_OBJ_FLAG_IS_SCANNING   = 0x08,     // object is in scan mode

    // add other flags here

}DRV_MIIM_OBJ_FLAGS;

// MIIM Driver Hardware Instance Object
typedef struct _tag_DRV_MIIM_OBJ_STRUCT
{
    OSAL_SEM_HANDLE_TYPE objSem;  // synchronization object: protection for access to the IGMP
                                          // lists between user threads and task thread
    uint16_t            objFlags;       // DRV_MIIM_OBJ_FLAGS: object associated flags
    uint16_t            numClients;     // Number of active clients
    SYS_STATUS          objStatus;      // Status of module
    SYS_MODULE_INDEX    iModule;        // Module instance number
    uintptr_t           miimId;       // The peripheral Id associated with the object
    DRV_MIIM_CLIENT_DCPT objClients[DRV_MIIM_INSTANCE_CLIENTS]; // array of clients
    DRV_MIIM_OP_DCPT    opPool[DRV_MIIM_INSTANCE_OPERATIONS];      // pool of operations
    SINGLE_LIST         freeOpList;     // available operations
    SINGLE_LIST         busyOpList;     // scheduled operations
    SINGLE_LIST         completeOpList; // completed operations that need to be polled by the clients
#if (DRV_MIIM_CLIENT_OP_PROTECTION)
    DRV_MIIM_CLI_OP_STAMP stampPool[DRV_MIIM_INSTANCE_OPERATIONS];      // pool of stamps, matching operations
    SINGLE_LIST         freeStampList;   // list with available client stamps
    SINGLE_LIST         busyStampList;   // list with scheduled client stamps

#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)

}DRV_MIIM_OBJ;


// operation processing function 
typedef void (*DRV_MIIM_PROCESS_OP_FNC)(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt);


// helpers
//
void  Helper_SingleListInitialize(SINGLE_LIST* pL);



// adds node to tail
void  Helper_SingleListTailAdd(SINGLE_LIST* pL, SGL_LIST_NODE* pN);


// removes the head node
SGL_LIST_NODE*  Helper_SingleListHeadRemove(SINGLE_LIST* pL);

// removes a node anywhere in the list
// Note: this is lengthy!
SGL_LIST_NODE*  Helper_SingleListNodeRemove(SINGLE_LIST* pL, SGL_LIST_NODE* pN);

#endif //#ifndef _DRV_MIIM_LOCAL_H

/*******************************************************************************
 End of File
*/

