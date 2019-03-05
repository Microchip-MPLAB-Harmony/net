#include "configuration.h"
#include "definitions.h"
#include "device.h"

#define RAW_WRITE_DEBUG_ON      0       // display debug messages
#define RAW_FORCE_WRITE         0       // write the SQI flash even if the initial verification succeeds
#define RAW_READ_BUFFER_SIZE    2048

struct MEDIA_STORAGE_PARTITION_INFO
{
    const char*     partName;           // name that identifies the partition
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
    DRV_HANDLE              handle;
    DRV_MX25L_GEOMETRY      geometry;
    uint32_t                jedec_id;
    uint8_t *               writeImagePtr;
    unsigned long           writeImageSize;
    unsigned long           writeImageIndex;
    unsigned long           eraseIndex;
    unsigned long           eraseIndexStep;
    uint8_t                 readBuffer[ RAW_READ_BUFFER_SIZE ];
} RAW_WRITE_APP_DATA;

static RAW_WRITE_APP_STATES rawState = APP_STATE_INIT;

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

// returns: -1 => Failure, 0 => inProcess, 1 => Success
int rawWrite( void )
{
    DRV_MX25L_TRANSFER_STATUS   transferStatus;
    RAW_VERIFY_RESULT           verifyRes;
    RAW_VERIFY_DATA             verifyData;
    int                         retval = 0;
    
#if (RAW_WRITE_DEBUG_ON != 0)
    if(     (lastState != rawState)
        &&  (APP_STATE_WRITE_WAIT != rawState) &&  (APP_STATE_ERASE_WAIT != rawState) // too fast/too often for proper display
      )
    {
        lastState = rawState;
        SYS_CONSOLE_PRINT("   RAW_%s\r\n", stateStr[ rawState ] );
    }
#endif  // (RAW_WRITE_DEBUG_ON != 0)

    /* Check the application's current state. */
    switch ( rawState )
    {
        case APP_STATE_INIT:
            if (DRV_MX25L_Status(DRV_MX25L_INDEX) == SYS_STATUS_READY)
            {
                rawState = APP_STATE_OPEN_DRIVER;
            }
            break;

        case APP_STATE_OPEN_DRIVER:
            appData.handle = DRV_MX25L_Open(DRV_MX25L_INDEX, DRV_IO_INTENT_READWRITE);
            if (appData.handle != DRV_HANDLE_INVALID)
            {
                rawState = APP_STATE_VERIFY_CONTENTS;
            }
            else
            {
                rawState = APP_STATE_ERROR;
            }
            break;

        case APP_STATE_VERIFY_CONTENTS:
            /* Check if the requested data it's already written to the QSPI memory */
            verifyRes = rawVerify(&verifyData, false);
            if(verifyRes == RAW_VERIFY_RES_OK) 
            {   // looks like everything is OK
#if (RAW_WRITE_DEBUG_ON != 0)
                SYS_CONSOLE_MESSAGE("   Raw Memory Verify Contents OK!\r\n");
#endif  // (RAW_WRITE_DEBUG_ON != 0)
#if (RAW_FORCE_WRITE != 0)
#if (RAW_WRITE_DEBUG_ON != 0)
                SYS_CONSOLE_MESSAGE("   Forcing Raw Memory Write...\r\n");
#endif  // (RAW_WRITE_DEBUG_ON != 0)
                rawState = APP_STATE_GEOMETRY_GET;
#else
                rawState = APP_STATE_SUCCESS;
#endif  // (RAW_FORCE_WRITE != 0)
            }
            else
            {   // overwrite whatever is stored on that memory
#if (RAW_WRITE_DEBUG_ON != 0)
                SYS_CONSOLE_PRINT("   Raw Memory Verify failed: %d\r\n", verifyRes);
                SYS_CONSOLE_PRINT("   Verify size: %d, block size: %d, blocks: %d, fail block: %d\r\n", verifyData.verifySize, verifyData.blockSize, verifyData.nBlocks, verifyData.failBlock);
#endif  // (RAW_WRITE_DEBUG_ON != 0)
                rawState = APP_STATE_GEOMETRY_GET;
            }
            break;

        case APP_STATE_GEOMETRY_GET:
            if( true != DRV_MX25L_UnlockFlash( appData.handle ) )
            {
                rawState = APP_STATE_ERROR;
            }
            else if( true == DRV_MX25L_GeometryGet(appData.handle, &appData.geometry) )
            {
                appData.writeImageSize = MPFS_IMAGE_PARTITION_INFO.partSize;
                appData.eraseIndex = 0;
                appData.eraseIndexStep = appData.geometry.erase_blockSize; 
                rawState = APP_STATE_ERASE_FLASH;
            }
            else
            {
                rawState = APP_STATE_ERROR;
            }
            break;

        case APP_STATE_ERASE_FLASH:
            if( true == DRV_MX25L_SectorErase(appData.handle, appData.eraseIndex ))
            {
                rawState = APP_STATE_ERASE_WAIT;
            }
            else
            {
                rawState = APP_STATE_ERROR;
            }
            break;

        case APP_STATE_ERASE_WAIT:
            transferStatus = DRV_MX25L_TransferStatusGet( appData.handle );
            if(transferStatus == DRV_MX25L_TRANSFER_COMPLETED)
            {
                appData.eraseIndex += appData.eraseIndexStep;
                if (appData.eraseIndex < appData.writeImageSize)
                {
                    rawState = APP_STATE_ERASE_FLASH;
                }
                else if(verifyRes = rawVerify(&verifyData, true) != RAW_VERIFY_RES_OK)
                {
#if (RAW_WRITE_DEBUG_ON != 0)
                    SYS_CONSOLE_PRINT("   Raw Memory Erase Verify failed: %d\r\n", verifyRes);
                    SYS_CONSOLE_PRINT("   Erase size: %d, block size: %d, blocks: %d, fail block: %d\r\n", verifyData.verifySize, verifyData.blockSize, verifyData.nBlocks, verifyData.failBlock);
#endif  // (RAW_WRITE_DEBUG_ON != 0)
                    rawState = APP_STATE_ERROR;
                }
                else
                {
#if (RAW_WRITE_DEBUG_ON != 0)
                    SYS_CONSOLE_PRINT("   Raw Memory Erase Verified OK, last erase: %d, last block: %d\r\n", appData.eraseIndex, verifyData.failBlock);
#endif  // (RAW_WRITE_DEBUG_ON != 0)
                    appData.writeImageSize = MPFS_IMAGE_PARTITION_INFO.partSize;
                    appData.writeImagePtr = (uint8_t *) MPFS_IMAGE_PARTITION_INFO.partStartOffset;
                    appData.writeImageIndex = 0;
                    rawState = APP_STATE_WRITE_MEMORY;
                }
            }
            else if (transferStatus == DRV_MX25L_TRANSFER_ERROR_UNKNOWN)
            {
                rawState = APP_STATE_ERROR;
            }
            break;

        case APP_STATE_WRITE_MEMORY:
            if( true == DRV_MX25L_PageWrite(    appData.handle, 
                        (uint32_t *) (appData.writeImagePtr + appData.writeImageIndex),
                        appData.writeImageIndex
                        )
              )
            {
                rawState = APP_STATE_WRITE_WAIT;
            }
            else
            {
                rawState = APP_STATE_ERROR;
            }
            break;

        case APP_STATE_WRITE_WAIT:
            transferStatus = DRV_MX25L_TransferStatusGet(appData.handle);
            if(transferStatus == DRV_MX25L_TRANSFER_COMPLETED)
            {
                appData.writeImageIndex += appData.geometry.write_blockSize;
                if (appData.writeImageIndex < appData.writeImageSize)
                {
                    rawState = APP_STATE_WRITE_MEMORY;
                }
                else
                {
                    rawState = APP_STATE_WRITE_VERIFY;
#if (RAW_WRITE_DEBUG_ON != 0)
                    SYS_CONSOLE_PRINT("   Last write at %d\r\n", appData.writeImageIndex );
#endif  // (RAW_WRITE_DEBUG_ON != 0)
                }
            }
            else if (transferStatus == DRV_MX25L_TRANSFER_ERROR_UNKNOWN)
            {
                rawState = APP_STATE_ERROR;
            }
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
#endif  // (RAW_WRITE_DEBUG_ON != 0)
                rawState = APP_STATE_ERROR;
            }
            break;

        case APP_STATE_SUCCESS:
            {
                retval = 1;
                break;
            }

        case APP_STATE_ERROR:
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
    RAW_VERIFY_RESULT verifyRes = RAW_VERIFY_RES_OK;

    for(ix = 0; ix < nCycles; ix++)
    {
        if(ix == nChunks)
        {
            chunkSize = nLeft;
        }

        memset(appData.readBuffer, 0, chunkSize);
        if(!DRV_MX25L_Read(appData.handle, (uint32_t *) appData.readBuffer, chunkSize, readOffs ) )
        {
            verifyRes = RAW_VERIFY_RES_READ_ERR;
            break;
        }

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

        readOffs += chunkSize;
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

