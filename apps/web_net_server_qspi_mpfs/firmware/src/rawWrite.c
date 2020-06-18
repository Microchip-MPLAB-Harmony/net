/*****************************************************************************
Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.

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
#include "configuration.h"
#include "definitions.h"
#include "device.h"

#define RAW_WRITE_DEBUG_ON      0       // display debug messages
#define RAW_FORCE_WRITE         0       // write the SQI flash even if the initial verification succeeds
#define RAW_READ_BUFFER_SIZE    4096

#define GEOMETRY_TABLE_READ_ENTRY   (0)
#define GEOMETRY_TABLE_WRITE_ENTRY  (1)
#define GEOMETRY_TABLE_ERASE_ENTRY  (2)

#define BLOCK_START                 0x0

struct MEDIA_STORAGE_PARTITION_INFO
{
    const char *    partName;           // name that identifies the partition
    unsigned long   partStartOffset;    // the absolute starting offset on that media 
    unsigned long   partSize;           // size of the partition, in bytes
};

extern struct MEDIA_STORAGE_PARTITION_INFO MPFS_IMAGE_PARTITION_INFO;

typedef enum
{
    /* Raw Write machine states */
    APP_STATE_INIT,
    APP_STATE_OPEN_DRIVER,
    APP_STATE_VERIFY_CONTENTS,
    APP_STATE_GEOMETRY_GET,
    APP_STATE_ERASE_FLASH,
    APP_STATE_ERASE_WAIT,
    APP_STATE_WRITE_MEMORY,
    APP_STATE_WRITE_WAIT,
    APP_STATE_WRITE_VERIFY,
    APP_STATE_SUCCESS,
    APP_STATE_ERROR,
    //
    RAW_WRITE_APP_STATES_SENTINEL     // must be last
} RAW_WRITE_APP_STATES;

#if (RAW_WRITE_DEBUG_ON != 0)
static const char * stateStr[ RAW_WRITE_APP_STATES_SENTINEL ] = {
    "INIT",
    "OPEN_DRIVER",
    "VERIFY_CONTENTS",
    "GEOMETRY_GET",
    "ERASE_FLASH",
    "ERASE_WAIT",
    "WRITE_MEMORY",
    "WRITE_WAIT",
    "WRITE_VERIFY",
    "SUCCESS",
    "ERROR",
};

static int  lastState =     APP_STATE_ERROR;
#endif  // (RAW_WRITE_DEBUG_ON != 0)

typedef struct
{
    SYS_MEDIA_GEOMETRY      *geometry;

    /* Driver Handle */
    DRV_HANDLE memoryHandle;

    /* Application transfer status */
    volatile bool xfer_done;

    /* Erase/Write/Read Command Handles*/
    DRV_MEMORY_COMMAND_HANDLE eraseHandle;
    DRV_MEMORY_COMMAND_HANDLE writeHandle;
    DRV_MEMORY_COMMAND_HANDLE readHandle;

    /* Number of read blocks*/
    uint32_t numReadBlocks;

    /* Number of read blocks*/
    uint32_t numWriteBlocks;

    /* Number of read blocks*/
    uint32_t numEraseBlocks;

    uint8_t                 readBuffer[ RAW_READ_BUFFER_SIZE ];
} RAW_WRITE_APP_DATA;

static RAW_WRITE_APP_STATES rawState = APP_STATE_OPEN_DRIVER;

static RAW_WRITE_APP_DATA appData;

typedef enum
{
    RAW_VERIFY_RES_OK           =  0,   // verification OK
    RAW_VERIFY_RES_MATCH_ERR    = -1,   // mismatch, data in the flash does not match
    RAW_VERIFY_RES_READ_ERR     = -2,   // error while reading from the QSPI
}RAW_VERIFY_RESULT;

typedef struct
{
    int verifySize;     // total size to be verified
    int blockSize;      // size of the verification block
    int nBlocks;        // number of blocks to be verified
    int failBlock;      // block index wher the fault occurred
    int failOffset;     // exact failure offset
}RAW_VERIFY_DATA;

static RAW_VERIFY_RESULT rawVerify(RAW_VERIFY_DATA* pVerifyData, bool verifyErased);

void appRawWriteTransferHandler
(
    DRV_MEMORY_EVENT event,
    DRV_MEMORY_COMMAND_HANDLE commandHandle,
    uintptr_t context
)
{
    RAW_WRITE_APP_DATA *app_data = (RAW_WRITE_APP_DATA *)context;

    switch(event)
    {
        case DRV_MEMORY_EVENT_COMMAND_COMPLETE:
            app_data->xfer_done = true;
            break;

        case DRV_MEMORY_EVENT_COMMAND_ERROR:
            rawState = APP_STATE_ERROR;
            break;

        default:
            break;
    }
}

// returns: -1 => Failure, 0 => inProcess, 1 => Success
int rawWrite( void )
{
    RAW_VERIFY_RESULT           verifyRes;
    RAW_VERIFY_DATA             verifyData;
    int                         retval = 0;
    
#if (RAW_WRITE_DEBUG_ON != 0)
    // too fast/too often for proper display
    if( (lastState != rawState) &&
        (APP_STATE_ERASE_WAIT != rawState) && 
        (APP_STATE_WRITE_WAIT != rawState)
    )
    {
        lastState = rawState;
        SYS_CONSOLE_PRINT("   RAW_%s\r\n", stateStr[ rawState ] );
    }
#endif

    /* Check the application's current state. */
    switch ( rawState )
    {
        case APP_STATE_OPEN_DRIVER:
            appData.memoryHandle = DRV_MEMORY_Open(DRV_MEMORY_INDEX_0, DRV_IO_INTENT_READWRITE);

            if (appData.memoryHandle != DRV_HANDLE_INVALID)
            {
                DRV_MEMORY_TransferHandlerSet(appData.memoryHandle, (const void *)appRawWriteTransferHandler, (uintptr_t)&appData);

                rawState = APP_STATE_GEOMETRY_GET;
            }
            else
            {
                return -1;
            }

            break;

        case APP_STATE_GEOMETRY_GET:
            appData.geometry = DRV_MEMORY_GeometryGet(appData.memoryHandle);

            if (appData.geometry == NULL)
            {
                rawState = APP_STATE_ERROR;
                break;
            }

            appData.numReadBlocks  = (MPFS_IMAGE_PARTITION_INFO.partSize / appData.geometry->geometryTable[GEOMETRY_TABLE_READ_ENTRY].blockSize);
            appData.numWriteBlocks = (MPFS_IMAGE_PARTITION_INFO.partSize / appData.geometry->geometryTable[GEOMETRY_TABLE_WRITE_ENTRY].blockSize);
            appData.numEraseBlocks = (MPFS_IMAGE_PARTITION_INFO.partSize / appData.geometry->geometryTable[GEOMETRY_TABLE_ERASE_ENTRY].blockSize);

            rawState = APP_STATE_VERIFY_CONTENTS;

            break;

        case APP_STATE_VERIFY_CONTENTS:
            /* Check if the requested data it's already written to the QSPI memory */
            verifyRes = rawVerify(&verifyData, false);

            if(verifyRes == RAW_VERIFY_RES_OK) 
            {   // looks like everything is OK
#if (RAW_WRITE_DEBUG_ON != 0)
                SYS_CONSOLE_MESSAGE("   Raw Memory Verify Contents OK!\r\n");
#endif
#if (RAW_FORCE_WRITE != 0)
#if (RAW_WRITE_DEBUG_ON != 0)
                SYS_CONSOLE_MESSAGE("   Forcing Raw Memory Write...\r\n");
#endif
                rawState = APP_STATE_ERASE_FLASH;
#else
                rawState = APP_STATE_SUCCESS;
#endif
            }
            else
            {   // overwrite whatever is stored on that memory
#if (RAW_WRITE_DEBUG_ON != 0)
                SYS_CONSOLE_PRINT("   Raw Memory Verify failed: %d\r\n", verifyRes);
                SYS_CONSOLE_PRINT("   Verify size: %d, block size: %d, blocks: %d, fail block: %d\r\n", verifyData.verifySize, verifyData.blockSize, verifyData.nBlocks, verifyData.failBlock);
#endif
                rawState = APP_STATE_ERASE_FLASH;
            }

            break;

        case APP_STATE_ERASE_FLASH:
            appData.xfer_done = false;

            DRV_MEMORY_AsyncErase(appData.memoryHandle, &appData.eraseHandle, BLOCK_START, appData.numEraseBlocks);

            if (DRV_MEMORY_COMMAND_HANDLE_INVALID == appData.eraseHandle)
            {
                rawState = APP_STATE_ERROR;
            }
            else
            {
                rawState = APP_STATE_ERASE_WAIT;
            }

            break;

        case APP_STATE_ERASE_WAIT:
            if(appData.xfer_done == false)
            {
                /* Wait for Erase to complete */
                break;
            }

            appData.xfer_done = false;

            verifyRes = rawVerify(&verifyData, true);

            if(verifyRes != RAW_VERIFY_RES_OK)
            {
#if (RAW_WRITE_DEBUG_ON != 0)
                SYS_CONSOLE_PRINT("   Raw Memory Erase Verify failed: %d\r\n", verifyRes);
                SYS_CONSOLE_PRINT("   Erase size: %d, block size: %d, blocks: %d, fail block: %d\r\n", verifyData.verifySize, verifyData.blockSize, verifyData.nBlocks, verifyData.failBlock);
#endif
                rawState = APP_STATE_ERROR;
            }
            else
            {
#if (RAW_WRITE_DEBUG_ON != 0)
                SYS_CONSOLE_PRINT("   Raw Memory Erase Verified OK \r\n");
#endif
                rawState = APP_STATE_WRITE_MEMORY;
            }

            break;

        case APP_STATE_WRITE_MEMORY:
            appData.xfer_done = false;

            DRV_MEMORY_AsyncWrite(appData.memoryHandle,
                                  &appData.writeHandle,
                                  (void *)MPFS_IMAGE_PARTITION_INFO.partStartOffset,
                                  BLOCK_START,
                                  appData.numWriteBlocks);

            if (DRV_MEMORY_COMMAND_HANDLE_INVALID == appData.writeHandle)
            {
                rawState = APP_STATE_ERROR;
            }
            else
            {
                rawState = APP_STATE_WRITE_WAIT;
            }

            break;

        case APP_STATE_WRITE_WAIT:
            if(appData.xfer_done == false)
            {
                /* Wait for Write to complete */
                break;
            }

            appData.xfer_done = false;

            rawState = APP_STATE_WRITE_VERIFY;

#if (RAW_WRITE_DEBUG_ON != 0)
            SYS_CONSOLE_PRINT("   Raw Memory Write Complete \r\n");
#endif

            break;

        case APP_STATE_WRITE_VERIFY:
            verifyRes = rawVerify(&verifyData, false);

            if(verifyRes == RAW_VERIFY_RES_OK)
            {
                rawState = APP_STATE_SUCCESS;
            } 
            else
            {
#if (RAW_WRITE_DEBUG_ON != 0)
                SYS_CONSOLE_PRINT("   Raw Memory Write Verify failed: %d\r\n", verifyRes);
                SYS_CONSOLE_PRINT("   Verify size: %d, block size: %d, blocks: %d, fail block: %d\r\n", verifyData.verifySize, verifyData.blockSize, verifyData.nBlocks, verifyData.failBlock);
#endif
                rawState = APP_STATE_ERROR;
            }
            break;

        case APP_STATE_SUCCESS:
            {
                DRV_MEMORY_Close(appData.memoryHandle);
                retval = 1;
                break;
            }

        case APP_STATE_ERROR:
            DRV_MEMORY_Close(appData.memoryHandle);
        default:
            retval = -1;
    }
    return( retval );
}

static RAW_VERIFY_RESULT rawVerify(RAW_VERIFY_DATA* pVerifyData, bool verifyErased)
{
    int             ix, jx;
    uint8_t*        pChk;
    const uint8_t*  pVerifySrc = (const uint8_t *) MPFS_IMAGE_PARTITION_INFO.partStartOffset;
    uint32_t        verifySize = MPFS_IMAGE_PARTITION_INFO.partSize;
    int nChunks =   verifySize / RAW_READ_BUFFER_SIZE;
    int nLeft =     verifySize % RAW_READ_BUFFER_SIZE;
    int nCycles =   nLeft ? nChunks + 1 : nChunks;
    int chunkSize = RAW_READ_BUFFER_SIZE;
    int cycle =     0;
    uint32_t readOffs = 0;
    int failOffset =    0;
    bool verifyFail =   false;
    uint32_t numReadBlocks = 0;
    RAW_VERIFY_RESULT verifyRes = RAW_VERIFY_RES_OK;

    numReadBlocks = (chunkSize / appData.geometry->geometryTable[GEOMETRY_TABLE_READ_ENTRY].blockSize);

    for(ix = 0; ix < nCycles; ix++)
    {
        if(ix == nChunks)
        {
            chunkSize = nLeft;
        }

        memset(appData.readBuffer, 0, chunkSize);

        appData.xfer_done = false;

        DRV_MEMORY_AsyncRead(appData.memoryHandle, &appData.readHandle, (void *)appData.readBuffer, (BLOCK_START + readOffs), numReadBlocks);

        if (DRV_MEMORY_COMMAND_HANDLE_INVALID == appData.readHandle)
        {
            verifyRes = RAW_VERIFY_RES_READ_ERR;
            break;
        }

        /* Wait for Read to complete */
        while(appData.xfer_done == false)
        {
            DRV_MEMORY_Tasks(sysObj.drvMemory0);
        }

        appData.xfer_done = false;

        pChk = appData.readBuffer;

        if(verifyErased)
        {
            for(jx = 0; jx < chunkSize; jx++)
            {
                if(*pChk++ != 0xff)
                {
                    verifyFail = true;
                    failOffset = jx;
                    break;
                }
            }
        }
        else
        {
            for(jx = 0; jx < chunkSize; jx++)
            {
                if(*pChk++ != *pVerifySrc++)
                {
                    verifyFail = true;
                    break;
                }
            }
        }

        if(verifyFail)
        {
            failOffset = jx;
            verifyRes = RAW_VERIFY_RES_MATCH_ERR;
            break;
        }

        readOffs += numReadBlocks;
        cycle++;
    }

    if(pVerifyData)
    {
        pVerifyData->verifySize = verifySize;
        pVerifyData->blockSize = RAW_READ_BUFFER_SIZE;
        pVerifyData->nBlocks = nChunks;
        pVerifyData->failBlock = cycle;
        pVerifyData->failOffset = failOffset;
    }
    return verifyRes;
}

