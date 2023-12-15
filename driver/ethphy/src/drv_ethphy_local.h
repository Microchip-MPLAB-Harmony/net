/*******************************************************************************
  Ethernet PHY Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_ethphy_local.h

  Summary:
    Ethernet PHY driver local declarations and definitions.

  Description:
    This file contains the Ethernet PHY driver's local declarations and definitions.
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

#ifndef _DRV_ETHPHY_LOCAL_H
#define _DRV_ETHPHY_LOCAL_H


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
#include "driver/ethphy/drv_ethphy.h"
#include "driver/ethphy/src/dynamic/drv_extphy_regs.h"

// debugging
#define DRV_PHY_DEBUG_MASK_BASIC            (0x0001)    // basic assert/condition
#define DRV_PHY_DEBUG_MASK_OP_FAIL          (0x0002)    // display failed operations
#define DRV_PHY_DEBUG_MASK_DETECT_PHASE     (0x0004)    // display detect phases/states
#define DRV_PHY_DEBUG_MASK_DETECT_VALUES    (0x0008)    // display detect register read/write values
#define DRV_PHY_DEBUG_MASK_SETUP            (0x0010)    // display setup process
#define DRV_PHY_DEBUG_MASK_MDIX             (0x0020)    // display MDIX process
#define DRV_PHY_DEBUG_MASK_LINK             (0x0040)    // display link process


// enable IPV4 debugging levels
#define DRV_PHY_DEBUG_LEVEL  (0x03)


// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* ETHPHY Driver Client Object Operation types

  Summary:
    Defines the internal operations performed by the PHY driver client

  Description:
    Defines the internal operations performed by the PHY driver client

  Remarks:
    None.
*/

typedef enum
{
    DRV_ETHPHY_CLIENT_OP_TYPE_NONE = 0,         // no operation currently in progress
    DRV_ETHPHY_CLIENT_OP_TYPE_SETUP,            // set up operation
    DRV_ETHPHY_CLIENT_OP_TYPE_LINK_STAT,        // link status get operation
    DRV_ETHPHY_CLIENT_OP_TYPE_NEG_COMPLETE,     // wait negotiation complete operation
    DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESULT,       // get negotiation result operation
    DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESTART,      // ask for a negotiation restart
    DRV_ETHPHY_CLIENT_OP_TYPE_RESET,            // ask for a PHY reset
    DRV_ETHPHY_CLIENT_OP_SMI_READ,              // SMI read operation scheduled
    DRV_ETHPHY_CLIENT_OP_SMI_WRITE,             // SMI write operation scheduled
} DRV_ETHPHY_CLIENT_OP_TYPE;



// *****************************************************************************
/* ETHPHY Driver Client Object Internal Set up Phases

  Summary:
    Defines the internal set up operation phases of a Driver Client Object.

  Description:
    Defines the internal set up operation phases of a Driver Client Object.

  Remarks:
    None.
*/

typedef enum
{
    DRV_ETHPHY_SETUP_PHASE_IDLE = 0,
    DRV_ETHPHY_SETUP_PHASE_DETECT,
    DRV_ETHPHY_SETUP_PHASE_READ_ID,        
    DRV_ETHPHY_SETUP_PHASE_RESET,
    DRV_ETHPHY_SETUP_PHASE_NEGOTIATE,
    DRV_ETHPHY_SETUP_PHASE_DONE,
    DRV_ETHPHY_SETUP_PHASE_ERROR,       // failed
} DRV_ETHPHY_SETUP_PHASE;

// *****************************************************************************
/* ETHPHY Driver Client Object Internal Link Status Phases

  Summary:
    Defines the internal link status operation phases of a Driver Client Object.

  Description:
    Defines the internal link status operation phases of a Driver Client Object.

  Remarks:
    None.
*/

typedef enum
{
    DRV_ETHPHY_LINK_STAT_PHASE_IDLE = 0,
    DRV_ETHPHY_LINK_STAT_PHASE_READ,
    DRV_ETHPHY_LINK_STAT_PHASE_REFRESH,
    DRV_ETHPHY_LINK_STAT_PHASE_ERROR,       // failed

} DRV_ETHPHY_LINK_STAT_PHASE;


// *****************************************************************************
/* ETHPHY Driver Client Object Internal Negotiation Complete Status Phases

  Summary:
    Defines the negotiation complete operation phases of a Driver Client Object.

  Description:
    Defines the negotiation complete operation phases of a Driver Client Object.

  Remarks:
    None.
*/

typedef enum
{
    DRV_ETHPHY_NEG_COMPLETE_PHASE_IDLE = 0,
    DRV_ETHPHY_NEG_COMPLETE_PHASE_READ1,
    DRV_ETHPHY_NEG_COMPLETE_PHASE_READ2,
    DRV_ETHPHY_NEG_COMPLETE_PHASE_RESULT_NOWAIT,
    DRV_ETHPHY_NEG_COMPLETE_PHASE_AN_RESTART,
    DRV_ETHPHY_NEG_COMPLETE_PHASE_AN_COMPLETE,
    DRV_ETHPHY_NEG_COMPLETE_PHASE_ERROR,       // failed

} DRV_ETHPHY_NEG_COMPLETE_PHASE;

// *****************************************************************************
/* ETHPHY Driver Client Object Internal Negotiation Result Status Phases

  Summary:
    Defines the negotiation get result operation phases of a Driver Client Object.

  Description:
    Defines the negotiation get result operation phases of a Driver Client Object.

  Remarks:
    None.
*/

typedef enum
{
    DRV_ETHPHY_NEG_RESULT_PHASE_IDLE = 0,
    DRV_ETHPHY_NEG_RESULT_PHASE_BMSTAT,
    DRV_ETHPHY_NEG_RESULT_PHASE_EXTSTAT,
    DRV_ETHPHY_NEG_RESULT_PHASE_1000BASECTRL,
    DRV_ETHPHY_NEG_RESULT_PHASE_1000BASESTAT,
    DRV_ETHPHY_NEG_RESULT_PHASE_ANEXP,
    DRV_ETHPHY_NEG_RESULT_PHASE_ANLPAD,
    DRV_ETHPHY_NEG_RESULT_PHASE_ANAD,
    DRV_ETHPHY_NEG_RESULT_PHASE_ERROR,       // failed

} DRV_ETHPHY_NEG_RESULT_PHASE;


// *****************************************************************************
/* ETHPHY Driver Client Object Internal Negotiation Restart Status Phases

  Summary:
    Defines the negotiation restart operation phases of a Driver Client Object.

  Description:
    Defines the negotiation restart operation phases of a Driver Client Object.

  Remarks:
    None.
*/

typedef enum
{
    DRV_ETHPHY_NEG_RESTART_PHASE_IDLE = 0,
    DRV_ETHPHY_NEG_RESTART_PHASE_READ,
    DRV_ETHPHY_NEG_RESTART_PHASE_WRITE,
    DRV_ETHPHY_NEG_RESTART_PHASE_ERROR,       // failed

} DRV_ETHPHY_NEG_RESTART_PHASE;

// *****************************************************************************
/* ETHPHY Driver Client Object Internal Reset Status Phases

  Summary:
    Defines the reset operation phases of a Driver Client Object.

  Description:
    Defines the reset operation phases of a Driver Client Object.

  Remarks:
    None.
*/

typedef enum
{
    DRV_ETHPHY_RESET_PHASE_IDLE = 0,
    DRV_ETHPHY_RESET_PHASE_WRITE,
    DRV_ETHPHY_RESET_PHASE_READ,
    DRV_ETHPHY_RESET_PHASE_ERROR,       // failed

} DRV_ETHPHY_RESET_PHASE;


// *****************************************************************************
/* ETHPHY Driver Client SMI Operation Status

  Summary:
    Defines the status of a Driver Client initiated SMI operation

  Description:
    Defines the status of a Driver Client initiated SMI operation

  Remarks:
    None.
*/

typedef enum
{
    DRV_ETHPHY_SMI_TXFER_OP_NONE = 0,      // no operation in progress
    DRV_ETHPHY_SMI_TXFER_OP_START,         // command to start the operation
    DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE, // operation waiting to complete
} DRV_ETHPHY_SMI_TXFER_OP_STATUS;

// *****************************************************************************
/* ETHPHY Driver Client SMI Operation result

  Summary:
    Defines the result of an initiated SMI operation

  Description:
    Defines the result of an initiated SMI operation

  Remarks:
    None.
*/

typedef enum
{
    DRV_ETHPHY_SMI_TXFER_RES_DONE,      // operation completed successfully
    DRV_ETHPHY_SMI_TXFER_RES_SCHEDULED, // operation scheduled succesfuly, not yet complete
    DRV_ETHPHY_SMI_TXFER_RES_WAIT,      // operation not yet complete, call again
    DRV_ETHPHY_SMI_TXFER_RES_BUSY,      // operation failed - another operation in progress; retry
    // errors
    DRV_ETHPHY_SMI_TXFER_RES_ILLEGAL    = -1,   // illegal operation attempted
    DRV_ETHPHY_SMI_TXFER_RES_MIIM_ERROR = -2,   // MIIM operation failed - MIIM couldn't start operation 
} DRV_ETHPHY_SMI_TXFER_RES;


// *****************************************************************************
/* ETHPHY Driver PHY MIIM transfer type

  Summary:
    Defines the type of a PHY MIIM transfer operation

  Description:
    Defines the type of a PHY MIIM transfer operation

  Remarks:
    None.
*/

typedef enum
{
    DRV_ETHPHY_SMI_XFER_TYPE_NONE = 0,          // no transfer in progress
    DRV_ETHPHY_SMI_XFER_TYPE_WRITE,             // write operation
    DRV_ETHPHY_SMI_XFER_TYPE_WRITE_COMPLETE,    // write with completion
    DRV_ETHPHY_SMI_XFER_TYPE_READ,              // read operation
} DRV_ETHPHY_SMI_XFER_TYPE;

// *****************************************************************************
/* ETHPHY Driver Client Object

  Summary:
    Defines the object required for the maintainence of the software clients.

  Description:
    This defines the object required for the maintainence of the software
    clients instance. This object exists once per client instance.

  Remarks:
    None.
*/

typedef struct 
{
    uint8_t                     clientInUse;    // True if in use
    uint8_t                     reserved;       // padding, not used                                                
    int16_t                     status;         // Client Status: DRV_ETHPHY_CLIENT_STATUS value

    uintptr_t                   ethphyId;       // The peripheral Id associated with the object
    struct _DRV_ETHPHY_INSTANCE* hDriver;       // Handle of driver that owns the client
    const DRV_MIIM_OBJECT_BASE* pMiimBase;      // MIIM driver base object to use   
    DRV_HANDLE                  miimHandle;     // MMIM client handle
    DRV_MIIM_OPERATION_HANDLE   miimOpHandle;   // current MIIM op in progress; 

    // current operation performed by the driver
    uint16_t                    operType;       // DRV_ETHPHY_CLIENT_OP_TYPE: current operation type
    uint16_t                    operPhase;      // current phase; operation dependent
                                                // - DRV_ETHPHY_SETUP_PHASE  for set up
                                                //
    uint16_t                    operSubPhase;   // extra sub-phase counter
    int16_t                     operRes;        // DRV_ETHPHY_RESULT: last operation result
    uint16_t                    operReg[4];     // scratch operation registers 
    uintptr_t                   operParam;      // operation parameter
    uint32_t                    operTStamp;     // tick value for timing purposes 


    // low level SMI transfer operations data
    uint8_t                     smiTxferStatus; // DRV_ETHPHY_SMI_TXFER_OP_STATUS: current operation status
    uint8_t                     smiTxferType;   // DRV_ETHPHY_SMI_XFER_TYPE: current operation type
    uint16_t                    smiRIx;         // current SMI operation register involved
    uint16_t                    smiData;        // current SMI operation data: I/O for read/write
    uint16_t                    smiPhyAddress;  // PHY SMI address to use for the transaction
                                                
    // vendor specific data
    uint16_t                    detectMask;     // the pPhyObj->bmconDetectMask value 
    uint16_t                    capabMask;      // the pPhyObj->bmstatCpblMask value 
    uintptr_t                   vendorData;
    DRV_ETHPHY_VENDOR_DETECT    vendorDetect;    
        
    uint16_t                    phyId_1;        // PHYY Identifier 1 register value
    uint16_t                    phyId_2;        // PHYY Identifier 2 register value

} DRV_ETHPHY_CLIENT_OBJ;

// *****************************************************************************
/* ETHPHY Driver Hardware Instance Object

  Summary:
    Defines the object required for the maintainence of the hardware instance.

  Description:
    This defines the object required for the maintainence of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

typedef struct _DRV_ETHPHY_INSTANCE
{
    uint8_t                     objInUse;       // True if in use
    uint8_t                     macPauseType;   // TCPIP_ETH_PAUSE_TYPE: MAC supported pause type
    uint16_t                    phyAddress;     // PHY SMI address

    int16_t                     status;         // SYS_STATUS: Status of module
    uint16_t                    miimIndex;      // SYS_MODULE_INDEX: MIIM object index 
    uint16_t                    iModule;        // SYS_MODULE_INDEX: Module instance number
    uint16_t                    configFlags;    // DRV_ETHPHY_CONFIG_FLAGS: ETHPHY MII/RMII configuration flags

    
    uintptr_t                   ethphyId;       // The peripheral Id associated with the object
    uint32_t                    openFlags;      // TCPIP_ETH_OPEN_FLAGS: flags required at open time
    const DRV_ETHPHY_OBJECT*    pPhyObj;        // PHY object, vendor specific functions    
    const DRV_ETHPHY_OBJECT_BASE* pBaseObj;     // PHY base: this object
    const DRV_MIIM_OBJECT_BASE* pMiimBase;      // MIIM driver base object to use   
    DRV_ETHPHY_TMO *            ethphyTmo;      // PHY Initialization Time-outs 
    DRV_ETHPHY_CLIENT_OBJ       objClient;      // the one and only PHY client: the MAC driver
} DRV_ETHPHY_INSTANCE;



// driver functions that could be used by derived objects

// return an DRV PHY instance from a handle
DRV_ETHPHY_INSTANCE* _DRV_ETHPHY_HandleToInst(void * handle);

// starts a driver operations and sets the right state
void _DRV_ETHPHY_SetOperStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE opType, DRV_ETHPHY_RESULT res);


// sets an operation completion state and result
void _DRV_ETHPHY_SetOperDoneResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, DRV_ETHPHY_RESULT res);

// debug support
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_BASIC) != 0)
void _DRV_ETHPHY_AssertCond(bool cond, const char* message, int lineNo);
#else
#define _DRV_ETHPHY_AssertCond(cond, message, lineNo)
#endif  // (DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_BASIC)

#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_PHASE) != 0)
void _DRV_ETHPHY_Dbg_DetectPhase(uint16_t detectPhase);
#else
#define _DRV_ETHPHY_Dbg_DetectPhase(detectPhase)
#endif  // (DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_PHASE)

#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_VALUES) != 0)
void _DRV_ETHPHY_Dbg_DetectWriteValue(int rIx, uint16_t rVal);
void _DRV_ETHPHY_Dbg_DetectReadValue(int rIx, uint16_t rVal, uint16_t valMask, uint16_t chkMask);
#else
#define _DRV_ETHPHY_Dbg_DetectWriteValue(rIx, rVal)
#define _DRV_ETHPHY_Dbg_DetectReadValue(rIx, rVal, valMask, chkMask)
#endif  // (DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_VALUES)

#endif //#ifndef _DRV_ETHPHY_LOCAL_H

/*******************************************************************************
 End of File
*/

