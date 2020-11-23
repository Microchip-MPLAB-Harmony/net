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
/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

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
#define DRV_PHY_DEBUG_MASK_BASIC        (0x0001)

// enable IPV4 debugging levels
#define DRV_PHY_DEBUG_LEVEL  (0)


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

typedef struct _DRV_ETHPHY_CLIENT_OBJ_STRUCT
{
    uint16_t                    clientInUse;// True if in use
    uint16_t                    clientIx;   // client number
    uintptr_t                   ethphyId;   // The peripheral Id associated with the object
    DRV_ETHPHY_CLIENT_STATUS    status;     // Client Status
    struct _DRV_ETHPHY_OBJ_STRUCT* hDriver; // Handle of driver that owns the client
    const DRV_MIIM_OBJECT_BASE* pMiimBase;  // MIIM driver base object to use   
    DRV_HANDLE                  miimHandle; // MMIM client handle
    DRV_MIIM_OPERATION_HANDLE   miimOpHandle;   // current MIIM op in progress; 
    // current operation performed by the driver
    uint16_t                    operType;     // DRV_ETHPHY_CLIENT_OP_TYPE: current operation type
    uint16_t                    operPhase;    // current phase; operation dependent
                                              // - DRV_ETHPHY_SETUP_PHASE  for set up
                                              //
    uint16_t                    operSubPhase; // extra sub-phase counter
    uint16_t                    operReg[3];   // scratch operation registers 
    uint32_t                    operTStamp;   // tick value for timing purposes 
    DRV_ETHPHY_RESULT           operRes;      // last operation result
    uintptr_t                   operParam;    // operation parameter


    // low level SMI transfer operations data
    uint16_t                    smiTxferStatus;   // DRV_ETHPHY_SMI_TXFER_OP_STATUS: current operation status
    uint16_t                    smiTxferType;     // DRV_ETHPHY_SMI_XFER_TYPE: current operation type
    uint16_t                    smiRIx;      // current SMI operation register involved
    uint16_t                    smiData;     // current SMI operation data: I/O for read/write
    int                         smiPhyAddress;  // PHY SMI address to use for the transaction

    // vendor specific data
    uintptr_t                   vendorData;

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

typedef struct _DRV_ETHPHY_OBJ_STRUCT
{
    uint8_t             objInUse;       // True if in use
    uint8_t             busInUse;       // True if SMI bus in use;
    uint16_t            numClients;     // Number of active clients
    SYS_STATUS          status;         // Status of module
    SYS_MODULE_INDEX    iModule;        // Module instance number
    uintptr_t       ethphyId;       // The peripheral Id associated with the object
    TCPIP_ETH_OPEN_FLAGS      openFlags;      // flags required at open time
    DRV_ETHPHY_CONFIG_FLAGS configFlags;    // ETHPHY MII/RMII configuration flags
    TCPIP_ETH_PAUSE_TYPE      macPauseType;   // MAC supported pause type
    int                 phyAddress;     // PHY SMI address
    const DRV_ETHPHY_OBJECT*    pPhyObj; // PHY object, vendor specififc functions
    DRV_ETHPHY_CLIENT_OBJ       objClients[DRV_ETHPHY_CLIENTS_NUMBER]; // array of clients
    const DRV_MIIM_OBJECT_BASE* pMiimBase;  // MIIM driver base object to use   
    SYS_MODULE_INDEX            miimIndex;  // MIIM object index 
} DRV_ETHPHY_OBJ;


#endif //#ifndef _DRV_ETHPHY_LOCAL_H

/*******************************************************************************
 End of File
*/

