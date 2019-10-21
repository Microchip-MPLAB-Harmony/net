/*******************************************************************************
  MEMORY Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_memory_local.h

  Summary:
    MEMORY driver local declarations and definitions

  Description:
    This file contains the MEMORY driver's local declarations and definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
//DOM-IGNORE-END

#ifndef _DRV_MEMORY_LOCAL_H
#define _DRV_MEMORY_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "driver/memory/drv_memory.h"

#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Local Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MEMORY Driver Buffer/Client Handle Macros

  Summary:
    MEMORY driver Buffer/Client Handle Macros

  Description:
    Buffer/Client handle related utility macros. Memory buffer/client handle is
    a combination of client/buffer index (8-bit), instance index (8-bit) and
    token (16-bit). The token is incremented for every new driver open request Or
    a transfer request.

  Remarks:
    None
*/
#define DRV_MEMORY_INDEX_MASK                           (0x000000FF)
#define DRV_MEMORY_INSTANCE_INDEX_MASK                  (0x0000FF00)
#define DRV_MEMORY_TOKEN_MASK                           (0xFFFF0000)
#define DRV_MEMORY_TOKEN_MAX                            (DRV_MEMORY_TOKEN_MASK >> 16)
#define DRV_MEMORY_MAKE_HANDLE(token, instance, index)  ((token) << 16 | (instance << 8) | (index))

/* MEMORY Driver operations. */
typedef enum
{
    /* Request is read operation. */
    DRV_MEMORY_OPERATION_TYPE_READ = 0,

    /* Request is write operation. */
    DRV_MEMORY_OPERATION_TYPE_WRITE,

    /* Request is erase operation. */
    DRV_MEMORY_OPERATION_TYPE_ERASE,

    /* Request is erase write operation. */
    DRV_MEMORY_OPERATION_TYPE_ERASE_WRITE

} DRV_MEMORY_OPERATION_TYPE;

/* MEMORY Driver write states. */
typedef enum
{
    /* Read init state */
    DRV_MEMORY_READ_INIT = 0,

    /* Read command state */
    DRV_MEMORY_READ_MEM,

    /* Read command status state */
    DRV_MEMORY_READ_MEM_STATUS

} DRV_MEMORY_READ_STATE;

/* MEMORY Driver write states. */
typedef enum
{
    /* Write init state */
    DRV_MEMORY_WRITE_INIT = 0,

    /* Write command state */
    DRV_MEMORY_WRITE_MEM,

    /* Write command status state */
    DRV_MEMORY_WRITE_MEM_STATUS

} DRV_MEMORY_WRITE_STATE;

/* MEMORY Driver erase states. */
typedef enum
{
    /* Erase init state */
    DRV_MEMORY_ERASE_INIT = 0,

    /* Erase command state */
    DRV_MEMORY_ERASE_CMD,

    /* Erase command status state */
    DRV_MEMORY_ERASE_CMD_STATUS

} DRV_MEMORY_ERASE_STATE;

/* MEMORY Driver erasewrite states. */
typedef enum
{
    /* Erase write init state. */
    DRV_MEMORY_EW_INIT = 0,

    /* Erase write read state */
    DRV_MEMORY_EW_READ_SECTOR,

    /* Erase write erase state */
    DRV_MEMORY_EW_ERASE_SECTOR,

    /* Erase write write state */
    DRV_MEMORY_EW_WRITE_SECTOR

} DRV_MEMORY_EW_STATE;

/**************************************
 * MEMORY Driver Client
 **************************************/
typedef struct DRV_MEMORY_CLIENT_OBJ_STRUCT
{
    /* The hardware instance index associate with the client */
    uint8_t drvIndex;

    /* The intent with which the client was opened */
    DRV_IO_INTENT intent;

    /* Flag to indicate in use */
    bool inUse;

    /* Client specific event handler */
    DRV_MEMORY_TRANSFER_HANDLER transferHandler;

    /* Client handle assigned to this client object when it was opened */
    DRV_HANDLE clientHandle;

    /* Client specific context */
    uintptr_t context;

} DRV_MEMORY_CLIENT_OBJECT;

/*******************************************
 * MEMORY Driver Buffer Object that services
 * a driver request.
 ******************************************/
typedef struct _DRV_MEMORY_BUFFER_OBJECT
{
    /* Client that owns this buffer */
    DRV_MEMORY_CLIENT_OBJECT *hClient;

    /* Present status of this command */
    DRV_MEMORY_COMMAND_STATUS status;

    /* Current command handle of this buffer object */
    DRV_MEMORY_COMMAND_HANDLE commandHandle;

    /* Pointer to the source/destination buffer */
    uint8_t *buffer;

    /* Start address of the operation. */
    uint32_t blockStart;

    /* Number of blocks */
    uint32_t nBlocks;

    /* Operation type - read/write/erase/erasewrite */
    DRV_MEMORY_OPERATION_TYPE opType;

} DRV_MEMORY_BUFFER_OBJECT;

/**************************************
 * MEMORY Driver Hardware Instance Object
 **************************************/
typedef struct
{
    /* The status of the driver */
    SYS_STATUS status;

    /* Read state */
    DRV_MEMORY_READ_STATE readState;

    /* Write state */
    DRV_MEMORY_WRITE_STATE writeState;

    /* Erase state */
    DRV_MEMORY_ERASE_STATE eraseState;

    /* Erase write state */
    DRV_MEMORY_EW_STATE ewState;

    /* Flag to check transfer status */
    volatile bool isTransferDone;

    /* Flag to indicate in use  */
    bool inUse;

    /* Flag to indicate that the driver is used in exclusive access mode */
    bool isExclusive;

    /* Pointer to the Erase Write buffer */
    uint8_t *ewBuffer;

    /* This instances flash start address */
    uint32_t blockStartAddress;

    /* Tracks the current block address for the write operation. */
    uint32_t blockAddress;

    /* Tracks the current number of blocks of the write operation. */
    uint32_t nBlocks;

    /* erase write sector number */
    uint32_t sectorNumber;

    /* Block offset within the current sector. */
    uint32_t blockOffsetInSector;

    /* Number of blocks to write. */
    uint32_t nBlocksToWrite;

    /* Pointer to user write buffer */
    uint8_t *writePtr;

    /* Write Block size */
    uint32_t writeBlockSize;

    /* Erase Block size */
    uint32_t eraseBlockSize;

    /* This is an instance specific token counter used to generate unique client
     * handles
     */
    uint16_t clientToken;

    /* This is an instance specific token counter used to generate unique buffer
     * handles
     */
    uint16_t bufferToken;

    /* Attached Memory Device index */
    SYS_MODULE_INDEX memDevIndex;

    /* Attached Memory Device Handle */
    DRV_HANDLE memDevHandle;

    /* Flag to indicate if attached memory device configured to interrupt mode */
    bool isMemDevInterruptEnabled;

    /* Number of milliseconds to poll for transfer status check */
    uint32_t memDevStatusPollUs;

    /* Attached Memory Device functions */
    const DRV_MEMORY_DEVICE_INTERFACE *memoryDevice;

    /* Pointer to the current buffer object */
    DRV_MEMORY_BUFFER_OBJECT currentBufObj;

    /* Memory pool for Client Objects */
    DRV_MEMORY_CLIENT_OBJECT *clientObjPool;

    /* Number of clients connected to the hardware instance */
    uint8_t numClients;

    /* Maximum number of clients */
    size_t nClientsMax;

    /* MEMORY driver geometry object */
    SYS_MEDIA_GEOMETRY mediaGeometryObj;

    /* MEMORY driver media geometry table. */
    SYS_MEDIA_REGION_GEOMETRY mediaGeometryTable[3];

    /* Mutex to serialize access to the underlying media */
    OSAL_MUTEX_DECLARE(transferMutex);

    /* Mutex to protect the client object pool */
    OSAL_MUTEX_DECLARE(clientMutex);

    /* Semaphore to wait for transfer request to complete. This will be released
     * from the System timer handler at regular interval expiry.
    */
    OSAL_SEM_DECLARE(transferDone);

} DRV_MEMORY_OBJECT;

typedef MEMORY_DEVICE_TRANSFER_STATUS (*DRV_MEMORY_TransferOperation)(
    DRV_MEMORY_OBJECT *dObj,
    uint8_t *data,
    uint32_t blockStart,
    uint32_t nBlocks
);

#endif //#ifndef _DRV_MEMORY_LOCAL_H

/*******************************************************************************
 End of File
*/

