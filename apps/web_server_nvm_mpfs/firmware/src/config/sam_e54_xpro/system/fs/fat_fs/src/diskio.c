/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "system/fs/fat_fs/src/hardware_access/diskio.h"        /* FatFs lower layer API */
#include "system/fs/sys_fs_media_manager.h"
#include <string.h>

#define ALIGN_32_BYTE_MASK  0x0000001F

typedef struct
{
    SYS_FS_MEDIA_COMMAND_STATUS commandStatus;
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE commandHandle;
    uint8_t alignedBuffer[FAT_FS_MAX_SS] __ALIGNED(32);
} SYS_FS_DISK_DATA;

static SYS_FS_DISK_DATA CACHE_ALIGN gSysFsDiskData[SYS_FS_MEDIA_NUMBER];

void diskEventHandler
(
    SYS_FS_MEDIA_BLOCK_EVENT event,
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE commandHandle,
    uintptr_t context
)
{
    switch(event)
    {
        case SYS_FS_MEDIA_EVENT_BLOCK_COMMAND_COMPLETE:
            gSysFsDiskData[context].commandStatus = SYS_FS_MEDIA_COMMAND_COMPLETED;
            break;
        case SYS_FS_MEDIA_EVENT_BLOCK_COMMAND_ERROR:
            gSysFsDiskData[context].commandStatus= SYS_FS_MEDIA_COMMAND_UNKNOWN;
            break;
        default:
            break;
    }
}

static DRESULT disk_checkCommandStatus(uint8_t pdrv)
{
    DRESULT result = RES_ERROR;

    /* Buffer is invalid report error */
    if (gSysFsDiskData[pdrv].commandHandle == SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID)
    {
        result = RES_PARERR;
    }
    else
    {
        /* process the read request by blocking on the task routine that process the
         I/O request */
        while (gSysFsDiskData[pdrv].commandStatus == SYS_FS_MEDIA_COMMAND_IN_PROGRESS)
        {
            SYS_FS_MEDIA_MANAGER_TransferTask (pdrv);
        }


        if (gSysFsDiskData[pdrv].commandStatus == SYS_FS_MEDIA_COMMAND_COMPLETED)
        {
            /* Buffer processed successfully */
            result = RES_OK;
        }
    }



    return result;
}

/* Definitions of physical drive number for each drive */
#define ATA     0   /* Example: Map ATA harddisk to physical drive 0 */
#define MMC     1   /* Example: Map MMC/SD card to physical drive 1 */
#define USB     2   /* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    uint8_t pdrv        /* Physical drive nmuber to identify the drive */
)
{
    return 0;
}



/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    uint8_t pdrv                /* Physical drive nmuber to identify the drive */
)
{
    switch( pdrv ) {
    case 0:
    default:
        break;
    }

    SYS_FS_MEDIA_MANAGER_RegisterTransferHandler( (void *) diskEventHandler );
    return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read
(
    uint8_t pdrv,   /* Physical drive nmuber (0..) */
    uint8_t *buff,  /* Data buffer to store read data */
    uint32_t sector,/* Sector address (LBA) */
    uint32_t count   /* Number of sectors to read (1..128) */
)
{
    uint32_t i;
    DRESULT result = RES_ERROR;

    gSysFsDiskData[pdrv].commandHandle = SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID;

    /* Use aligned buffer to read if the received buffer address is not Aligned to 32 Bytes */
    if (((uint32_t)buff & ALIGN_32_BYTE_MASK) != 0)
    {
        /* Read One Sector at a Time */
        for (i = 0; i < count; i++)
        {
            gSysFsDiskData[pdrv].commandStatus = SYS_FS_MEDIA_COMMAND_IN_PROGRESS;

            /* submit the read request */
            gSysFsDiskData[pdrv].commandHandle = SYS_FS_MEDIA_MANAGER_SectorRead(pdrv /* DISK 0 */ ,
                    gSysFsDiskData[pdrv].alignedBuffer /* Destination Sector*/,
                    sector,
                    1 /* Number of Sectors */);

            result = disk_checkCommandStatus(pdrv);

            if (result != RES_OK)
            {
                break;
            }

            /* Copy the received data from aligned buffer to actual buffer */
            memcpy(buff, gSysFsDiskData[pdrv].alignedBuffer, FAT_FS_MAX_SS);

            buff += FAT_FS_MAX_SS;
            sector++;
        }
    }
    else
    {
        gSysFsDiskData[pdrv].commandStatus = SYS_FS_MEDIA_COMMAND_IN_PROGRESS;

        /* submit the read request */
        gSysFsDiskData[pdrv].commandHandle = SYS_FS_MEDIA_MANAGER_SectorRead(pdrv /* DISK 0 */ ,
                buff /* Destination Sector*/,
                sector,
                count /* Number of Sectors */);

        result = disk_checkCommandStatus(pdrv);
    }

    return result;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write
(
    uint8_t pdrv,       /* Physical drive nmuber (0..) */
    const uint8_t *buff,/* Data to be written */
    uint32_t sector,    /* Sector address (LBA) */
    uint32_t count       /* Number of sectors to write (1..128) */
)
{
    uint32_t i;
    DRESULT result = RES_ERROR;

    gSysFsDiskData[pdrv].commandHandle = SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID;

    /* Use aligned buffer to write if the received buffer address is not Aligned to 32 Bytes */
    if (((uint32_t)buff & ALIGN_32_BYTE_MASK) != 0)
    {
        for (i = 0; i < count; i++)
        {
            /* Copy the actual buffer data into aligned buffer */
            memcpy(gSysFsDiskData[pdrv].alignedBuffer, buff, FAT_FS_MAX_SS);

            gSysFsDiskData[pdrv].commandStatus = SYS_FS_MEDIA_COMMAND_IN_PROGRESS;

            /* Submit the write request to media */
            gSysFsDiskData[pdrv].commandHandle = SYS_FS_MEDIA_MANAGER_SectorWrite(pdrv /* DISK 0 */ ,
                sector /* Destination Sector*/,
                gSysFsDiskData[pdrv].alignedBuffer,
                1 /* Number of Sectors */);

            result = disk_checkCommandStatus(pdrv);

            if (result != RES_OK)
            {
                break;
            }

            buff += FAT_FS_MAX_SS;
            sector++;
        }
    }
    else
    {
        gSysFsDiskData[pdrv].commandStatus = SYS_FS_MEDIA_COMMAND_IN_PROGRESS;

        /* Submit the write request to media */
        gSysFsDiskData[pdrv].commandHandle = SYS_FS_MEDIA_MANAGER_SectorWrite(pdrv /* DISK 0 */ ,
            sector /* Destination Sector*/,
            (uint8_t *)buff,
            count /* Number of Sectors */);

        result = disk_checkCommandStatus(pdrv);
    }

    return result;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
        uint8_t pdrv,       /* Physical drive nmuber (0..) */
        uint8_t cmd,        /* Control code */
        void *buff      /* Buffer to send/receive control data */
        )
{
    if (cmd == GET_SECTOR_COUNT)
    {
        uint32_t numSectors = 0;
        uint32_t mediaBlockSize = 0;
        uint32_t numBlocksPerSector = 1;
        SYS_FS_MEDIA_GEOMETRY *mediaGeometry = NULL;

        mediaGeometry = SYS_FS_MEDIA_MANAGER_GetMediaGeometry (pdrv);
        if (mediaGeometry == NULL)
        {
            return RES_ERROR;
        }
        mediaBlockSize = mediaGeometry->geometryTable[0].blockSize;

        if (mediaBlockSize < 512)
        {
            /* Perform block to sector translation */
            numBlocksPerSector = (512 / mediaBlockSize);
        }

        numSectors = mediaGeometry->geometryTable[0].numBlocks / numBlocksPerSector;

        *(uint32_t *)buff = numSectors;
    }

    return RES_OK;
}
#endif

/****************************************************************************
 The get_fattime function is used to know the present time, which is used by
 FAT FS code. The present time should ideally be updated by a Real time clock
 hardware module. Since this module is not integrated with Harmony FS framework,
 a fixed time is set as given by the implementation of the function below   */
uint32_t get_fattime(void)
{
    /* RTC should return time here */
    /* For now, just a value */
    SYS_FS_TIME time;
    time.packedTime = 0;

    // All FAT FS times are calculated based on 0 = 1980
    time.discreteTime.year = (2013 - 1980); // year is 2013
    time.discreteTime.month = 8;    // August
    time.discreteTime.day = 9;     // 9th date
    time.discreteTime.hour = 15;    // 3pm afternoon
    time.discreteTime.minute = 06;  // 06 min
    time.discreteTime.second = 00;  // 00 sec

    return (time.packedTime);
}
