/*******************************************************************************
  File System System-Library Interface Implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    sys_fs_media_manager.c

  Summary:
    This file contains implementation of SYS FS Media Manager functions.

  Description:
    This file contains implementation of SYS FS Media Manager functions.
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

#include "system/fs/src/sys_fs_media_manager_local.h"
#include "system/fs/src/sys_fs_local.h"
#include "system/fs/mpfs/mpfs.h"

const char *gSYSFSVolumeName [] = {
    "nvm",
    "sd",
    "mmcblk",
    "ram",
    "mtd",
};

/**/
const uint16_t gPartitionTypeOffset [4] =
{
    450,
    466,
    482,
    498
};

// *****************************************************************************
/* Media object

  Summary:
    Defines the media object for each media that should be controlled by media manager

  Description:
    This data type defines the media objects that are available on
    the part.
  Remarks:
    None
*/

SYS_FS_MEDIA gSYSFSMediaObject[SYS_FS_MEDIA_NUMBER];

// *****************************************************************************
/* Volume object

  Summary:
    Defines the volume object for each media that should be controlled by media manager

  Description:
     Each partition of the media is assigned as a volume using the volume object.

  Remarks:
    None
*/
SYS_FS_VOLUME gSYSFSVolumeObject[SYS_FS_VOLUME_NUMBER];


// *****************************************************************************
/* Media Page Buffer

  Summary:
    Defines the media page buffer to store the data on media.

  Description:
    This data type defines the media page buffer.
  Remarks:
    None
*/
uint8_t CACHE_ALIGN gSYSFSMediaBlockBuffer[SYS_FS_MEDIA_MANAGER_BUFFER_SIZE] = {0};

// *****************************************************************************
/* Media Mount Table

  Summary:
    Defines the mount table specified by application.

  Description:
    This data type defines the mount table that maps device and mount name.
  Remarks:
    None
*/
extern const SYS_FS_MEDIA_MOUNT_DATA sysfsMountTable[];

uint8_t CACHE_ALIGN gSYSFSMediaBuffer[SYS_FS_MEDIA_MAX_BLOCK_SIZE];

/* Following structure holds the variables for media manager, including the task states */
SYS_FS_MEDIA_MANAGER_OBJ gSYSFSMediaManagerObj =
{
    gSYSFSMediaObject,
    gSYSFSVolumeObject,
    sysfsMountTable,
    NULL,
    gSYSFSMediaBuffer,
    0,
    0,
    false
};

/* Defines the volume to partition translation table, used by MPFS */
MPFS_PARTITION MPFS_VolToPart[SYS_FS_VOLUME_NUMBER];


//*****************************************************************************
/* Function:
    static void _SYS_FS_MEDIA_MANAGER_HandleMediaDetach
    (
        SYS_FS_MEDIA *mediaObj
    );

  Summary:
    Handles the media detach condition.

  Description:
    This function updates the structure members tracking the media attach
    status and also invokes the registered callback function to propagate the
    media detach event.

  Remarks:
    None
*/
static void _SYS_FS_MEDIA_MANAGER_HandleMediaDetach
(
    SYS_FS_MEDIA *mediaObj
)
{
    uint8_t volIndex = 0;
    SYS_FS_VOLUME *volumeObj = NULL;

    for (volIndex = 0; volIndex < SYS_FS_VOLUME_NUMBER; volIndex++)
    {
        volumeObj = &gSYSFSMediaManagerObj.volumeObj[volIndex];
        if ((volumeObj->inUse == false) || (volumeObj->obj != mediaObj))
        {
            continue;
        }

        volumeObj->inUse = false;

    }
}


// *****************************************************************************
/* Function:
    static void _SYS_FS_MEDIA_MANAGER_PopulateVolume
    (
        SYS_FS_MEDIA *mediaObj,
        uint8_t isMBR,
        uint8_t partitionMap,
        uint8_t fsType
    );

  Summary:
    Allocate a volume and populate with the FS details.

  Description:
    After finding a valid file system, a volume is allocated and populated with
    the details of the file system.

  Remarks:
    None.
***************************************************************************/
static void _SYS_FS_MEDIA_MANAGER_PopulateVolume
(
    SYS_FS_MEDIA *mediaObj,
    uint8_t isMBR,
    uint8_t partitionMap,
    uint8_t fsType
)
{
    uint8_t volumeIndex = 0;
    uint8_t volumeNameLen = 0;
    SYS_FS_VOLUME *volumeObj = &gSYSFSMediaManagerObj.volumeObj[0];


    for (volumeIndex = 0; volumeIndex < SYS_FS_VOLUME_NUMBER; volumeIndex++)
    {
        if (volumeObj->inUse == true)
        {
            volumeObj++;
            continue;
        }

        /* Found a free volume */
        mediaObj->numVolumes++;

        volumeNameLen = strlen(gSYSFSVolumeName[mediaObj->mediaType]);
        strncpy (volumeObj->volumeName, gSYSFSVolumeName[mediaObj->mediaType], volumeNameLen);

        /* Store the volume name */
        volumeObj->volumeName[volumeNameLen++] = mediaObj->mediaId;
        volumeObj->volumeName[volumeNameLen++] = mediaObj->numVolumes + '0';
        volumeObj->volumeName[volumeNameLen] = '\0';

        volumeObj->obj = mediaObj;
        volumeObj->fsType = fsType;

        if ('M' == volumeObj->fsType)
        {
            /* MPFS File System */
            volumeObj->numSectors = 0;
            volumeObj->startSector = 0;

            MPFS_VolToPart[volumeIndex].pd = mediaObj->mediaIndex;
            MPFS_VolToPart[volumeIndex].pt = 0;
        }
        /* Update the inUse flag to indicate that the volume is now in use.*/
        volumeObj->inUse = true;


        /* Continue if there is more than one partition on media */
        if (!partitionMap)
        {
            break;
        }

        /* Go to the next volume object */
        volumeObj++;
    }
}

// *****************************************************************************
/* Function:
    static uint8_t _SYS_FS_MEDIA_MANAGER_FindNextMedia
    (
        SYS_FS_MEDIA *mediaObj,
        uint8_t *index
    );

  Summary:
    Returns the index of the media that needs to be processed.

  Description:
    This function returns the index of the media that will be processed next by
    the media manager task routine.

  Remarks:
    None.
***************************************************************************/
static uint8_t _SYS_FS_MEDIA_MANAGER_FindNextMedia
(
    SYS_FS_MEDIA *mediaObj,
    uint8_t *index
)
{
    uint8_t indexLow = 0;

    indexLow = *index;
    while (*index < SYS_FS_MEDIA_NUMBER)
    {
        if (mediaObj[*index].inUse == true)
        {
            /* Media found. Return the index. */
            indexLow = *index;
            _SYS_FS_MEDIA_MANAGER_UPDATE_MEDIA_INDEX(*index);
            return indexLow;
        }

        (*index)++;
    }

    if (indexLow == 0)
    {
        /* No media. Reset the media index. */
        *index = 0;
        return 0xFF;
    }

    /* No media found in the upper range. Start searching from 0 till *index.
     * */
    indexLow = 0;
    while (indexLow < *index)
    {
        if (mediaObj[indexLow].inUse == true)
        {
            /* Media found. Return the media index. */
            *index = indexLow;
            _SYS_FS_MEDIA_MANAGER_UPDATE_MEDIA_INDEX(*index);
            return indexLow;
        }

        indexLow++;
    }

    /* No media. */
    return 0xFF;
}


// *****************************************************************************
/* Function:
    static uint8_t _SYS_FS_MEDIA_MANAGER_AnalyzeFileSystem
    (
        uint8_t *firstSector,
        uint8_t *numPartition,
        uint8_t *isMBR,
        uint8_t *partitionMap
    );

  Summary:
    Analyzes the MBR/VBR and identifies the type of filesystem present.

  Description:
    This function analyzes the contents of the MBR or the VBR to find the type
    of file system present on the media/partition.

  Remarks:
    None.
***************************************************************************/
static uint8_t _SYS_FS_MEDIA_MANAGER_AnalyzeFileSystem
(
    uint8_t *firstSector,
    uint8_t *numPartition,
    uint8_t *isMBR,
    uint8_t *partitionMap
)
{
    uint8_t fsType = 0xFF;

    *partitionMap = 0;
    *numPartition = 0;

    /* Check for the Boot Signature */
    if (0 == memcmp(firstSector, "MPFS", 4))
    {
        /* No partitions in MPFS, hence, go to other state */
        /* allocate a new volume to each partition */
        (*numPartition) = 1;
        /* This is 0x4D which also mean file system Primary QNX POSIX volume on disk */
        /* Need to find an unused value from the partition type*/
        fsType = 'M';
    }
    else /* If MBR is not detected, make media as unsupported */
    {
        /* File system either not present or not supported */
        fsType = 0xFF;
    }

    return fsType;
}

// *****************************************************************************
/* Function:
    SYS_FS_MEDIA_HANDLE SYS_FS_MEDIA_MANAGER_Register
    (
        SYS_MODULE_OBJ obj,
        SYS_MODULE_INDEX index,
        const SYS_FS_MEDIA_FUNCTIONS *mediaFunctions,
        SYS_FS_MEDIA_TYPE mediaType
    );

  Summary:
    Function to register media drivers with the media manager.

  Description:
    This function is called by the media driver to register the functionalities
    with the media manager. For static media, such as NVM or a SD card, the
    media drivers register with this function at the time of initialization.
    For dynamic media, such as MSD, this register function is called
    dynamically, once the MSD media is connected.

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
SYS_FS_MEDIA_HANDLE SYS_FS_MEDIA_MANAGER_Register
(
    SYS_MODULE_OBJ obj,
    SYS_MODULE_INDEX index,
    const SYS_FS_MEDIA_FUNCTIONS *mediaFunctions,
    SYS_FS_MEDIA_TYPE mediaType
)
{
    uint8_t mediaIndex = 0;
    uint8_t mediaId = 'a';

    SYS_FS_MEDIA *mediaObj = NULL;

    mediaObj = &gSYSFSMediaManagerObj.mediaObj[0];
    for (mediaIndex = 0; mediaIndex < SYS_FS_MEDIA_NUMBER; mediaIndex++)
    {
         if ((mediaObj->inUse) && (mediaType == mediaObj->mediaType))
         {
             /* For every type of media, increment the mediaID as a, b, c... */
             mediaId++;
         }
         mediaObj++;
    }

    mediaObj = &gSYSFSMediaManagerObj.mediaObj[0];
    /* Start with 0th disk and find a disk which is free */
    for (mediaIndex = 0; mediaIndex < SYS_FS_MEDIA_NUMBER; mediaIndex++)
    {
        if (mediaObj->inUse == false)
        {
            mediaObj->inUse = true;

            mediaObj->driverFunctions = mediaFunctions;
            mediaObj->driverObj = obj;
            mediaObj->driverIndex = index;
            mediaObj->driverHandle = DRV_HANDLE_INVALID;

            mediaObj->mediaState = SYS_FS_MEDIA_STATE_REGISTERED;
            mediaObj->mediaType = mediaType;

            mediaObj->numPartitions = 0;
            mediaObj->numVolumes = 0;

            mediaObj->mediaIndex = mediaIndex;
            mediaObj->mediaId = mediaId;
            mediaObj->attachStatus = SYS_FS_MEDIA_DETACHED;

            return (SYS_FS_MEDIA_HANDLE)mediaObj;
        }

        mediaObj++;
    }

    return SYS_FS_MEDIA_HANDLE_INVALID;
}

//*****************************************************************************
/*Function:
    void SYS_FS_MEDIA_MANAGER_DeRegister
    (
        SYS_FS_MEDIA_HANDLE    handle
    )

  Description:
    Function called by a media to deregister itself to the media manager. For
    static media, (like NVM or SD card), this "deregister function" is never
    called, since static media never gets deregistered once they are
    initialized.  For dynamic media (like MSD), this register function is
    called dynamically, once the MSD media is connected.

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
void SYS_FS_MEDIA_MANAGER_DeRegister
(
    SYS_FS_MEDIA_HANDLE handle
)
{
    SYS_FS_MEDIA *mediaObj = (SYS_FS_MEDIA *)handle;

    if (handle == SYS_FS_MEDIA_HANDLE_INVALID)
    {
        return;
    }

    mediaObj->isMediaDisconnected = true;
}

//*****************************************************************************
/* Function:
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_SectorRead
    (
        uint16_t diskNo,
        uint8_t * dataBuffer,
        uint32_t sector,
        uint32_t noSectors
    );

  Summary:
    Reads a specified media sector.

  Description:
    This function reads a specified media (disk) sector. This is the function
    in the media manager layer. This function in turn calls the specific sector
    read function from the list of function pointers of the media driver.

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_SectorRead
(
    uint16_t diskNum,
    uint8_t *dataBuffer,
    uint32_t sector,
    uint32_t numSectors
)
{
    SYS_FS_MEDIA *mediaObj = NULL;
    uint32_t blocksPerSector = 0;
    uint32_t mediaReadBlockSize = 0;

    if (diskNum >= SYS_FS_MEDIA_NUMBER)
    {
        SYS_ASSERT(false, "Invalid Disk");
        return SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID;
    }

    mediaObj = &gSYSFSMediaManagerObj.mediaObj[diskNum];

    if (mediaObj->driverHandle == DRV_HANDLE_INVALID)
    {
        return SYS_FS_MEDIA_HANDLE_INVALID;
    }

    mediaReadBlockSize = mediaObj->mediaGeometry->geometryTable[0].blockSize;

    if (mediaReadBlockSize < 512)
    {
        /* Find the number of blocks per sector */
        blocksPerSector = 512 / mediaReadBlockSize;
        /* Perform sector to block translation */
        sector *= blocksPerSector;
        numSectors *= blocksPerSector;
    }
    else
    {
        /* TODO: Handle cases where the block size is greater than 512 bytes.
         * */
    }


    mediaObj->commandStatus = SYS_FS_MEDIA_COMMAND_IN_PROGRESS;
    mediaObj->driverFunctions->sectorRead (mediaObj->driverHandle, &(mediaObj->commandHandle), dataBuffer, sector, numSectors);

    return (mediaObj->commandHandle);
}

//*****************************************************************************
/* Function:
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_Read
    (
        uint16_t diskNo,
        uint8_t* destination,
        uint8_t * source,
        const unsigned int nBytes
    );

   Summary:
     Gets data from a specific media address.

   Description:
     This function gets data from a specific address of media. This function is
     intended to work with NVM media only, which can have byte level
     addressing.  For other media, such as a SD card, byte addressing is not
     possible and this function will not work. Also, this function is intended
     to work with the MPFS2 file system only, which uses byte addressing.

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_Read
(
    uint16_t diskNum,
    uint8_t *destination,
    uint8_t *source,
    const uint32_t nBytes
)
{
    SYS_FS_MEDIA *mediaObj = NULL;
    uint32_t startAddress = 0;
    uint32_t address = 0;

    if (diskNum >= SYS_FS_MEDIA_NUMBER)
    {
        SYS_ASSERT(false, "Invalid Disk");
        return SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID;
    }

    mediaObj = &gSYSFSMediaManagerObj.mediaObj[diskNum];

    if (mediaObj->driverHandle == DRV_HANDLE_INVALID)
    {
        return SYS_FS_MEDIA_HANDLE_INVALID;
    }

    startAddress = mediaObj->driverFunctions->addressGet(mediaObj->driverHandle);
    address = (uint32_t)source - (uint32_t)startAddress;


    mediaObj->commandStatus = SYS_FS_MEDIA_COMMAND_IN_PROGRESS;
    mediaObj->driverFunctions->Read(mediaObj->driverHandle, &(mediaObj->commandHandle), destination, address, nBytes);

    return (mediaObj->commandHandle);
}

//*****************************************************************************
/* Function:
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_SectorWrite
    (
        uint16_t diskNo,
        uint32_t sector,
        uint8_t * dataBuffer,
        uint32_t noSectors
    );

    Summary:
      Writes a sector to the specified media.

    Description:
      This function writes to a sector of the specified media (disk). This is
      the function in the media manager layer. This function in turn calls the
      specific sector write function from the list of function pointers of the
      media driver.

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_SectorWrite
(
    uint16_t diskNum,
    uint32_t sector,
    uint8_t *dataBuffer,
    uint32_t numSectors
)
{
    SYS_FS_MEDIA *mediaObj = NULL;
    uint8_t *data = dataBuffer;
    uint32_t sectorOffsetInBlock = 0;
    uint32_t memoryBlock = 0;
    uint32_t sectorsPerBlock = 0;
    uint32_t numSectorsToWrite = 0;
    uint32_t mediaWriteBlockSize = 0;
    uint32_t blocksPerSector = 0;

    if(diskNum >= SYS_FS_MEDIA_NUMBER)
    {
        SYS_ASSERT(false, "Invalid Disk");
        return SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID;
    }

    mediaObj = &gSYSFSMediaManagerObj.mediaObj[diskNum];

    if (mediaObj->driverHandle == DRV_HANDLE_INVALID)
    {
        return SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID;
    }

    mediaWriteBlockSize = mediaObj->mediaGeometry->geometryTable[1].blockSize;

    if (mediaWriteBlockSize > 512)
    {
        sectorsPerBlock = mediaWriteBlockSize / 512;
    }
    else if (mediaWriteBlockSize == 512)
    {
        sectorsPerBlock = 1;
        blocksPerSector = 1;
    }
    else
    {
        blocksPerSector = 512 / mediaWriteBlockSize;
        sector *= blocksPerSector;
        numSectors *= blocksPerSector;
    }

    if ((sectorsPerBlock == 1) || (blocksPerSector > 0))
    {
        mediaObj->commandStatus = SYS_FS_MEDIA_COMMAND_IN_PROGRESS;
        mediaObj->driverFunctions->sectorWrite (mediaObj->driverHandle, &(mediaObj->commandHandle), dataBuffer, sector, numSectors);
        return (mediaObj->commandHandle);
    }
    else
    {
        /* Mute the event notification */
        gSYSFSMediaManagerObj.muteEventNotification = true;

        while (numSectors > 0)
        {
            /* Find the memory block for the starting sector */
            memoryBlock = sector / sectorsPerBlock;

            /* Find the number of sectors to be updated in this block. */
            sectorOffsetInBlock = (sector % sectorsPerBlock);
            numSectorsToWrite = (sectorsPerBlock - sectorOffsetInBlock);

            if (numSectors < numSectorsToWrite)
            {
                numSectorsToWrite = numSectors;
            }

            if (numSectorsToWrite != sectorsPerBlock)
            {
                /* Read the memory block from the media. Update the media data. */
                mediaObj->commandStatus = SYS_FS_MEDIA_COMMAND_IN_PROGRESS;

                mediaObj->driverFunctions->sectorRead(mediaObj->driverHandle, &(mediaObj->commandHandle), gSYSFSMediaBlockBuffer, memoryBlock * mediaWriteBlockSize, SYS_FS_MEDIA_MANAGER_BUFFER_SIZE);

                while (mediaObj->commandStatus == SYS_FS_MEDIA_COMMAND_IN_PROGRESS)
                {
                    if(mediaObj->driverFunctions->tasks != NULL)
                    {
                        mediaObj->driverFunctions->tasks(mediaObj->driverObj);
                    }
                }

                if (mediaObj->commandStatus != SYS_FS_MEDIA_COMMAND_COMPLETED)
                {
                    /* Unmute the event notification */
                    gSYSFSMediaManagerObj.muteEventNotification = false;

                    /* Media read operation failed. */
                    return SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID;
                }

                /* Multiply by the sector size */
                sectorOffsetInBlock <<= 9;
                memcpy ((void *)&gSYSFSMediaBlockBuffer[sectorOffsetInBlock], (const void *)dataBuffer, numSectorsToWrite << 9);

                data = gSYSFSMediaBlockBuffer;
            }
            else
            {
                /* Since the whole block is being updated, there is no need to
                 * perform a read-modify-write operation of the block. */
                data = dataBuffer;
            }

            if ((numSectors - numSectorsToWrite) == 0)
            {
                /* This is the last write operation. */
                break;
            }

            /* Write the block to the media */
            mediaObj->commandStatus = SYS_FS_MEDIA_COMMAND_IN_PROGRESS;
            mediaObj->driverFunctions->sectorWrite (mediaObj->driverHandle, &(mediaObj->commandHandle), data, memoryBlock, 1);
            while (mediaObj->commandStatus == SYS_FS_MEDIA_COMMAND_IN_PROGRESS)
            {
                if(mediaObj->driverFunctions->tasks != NULL)
                {
                    mediaObj->driverFunctions->tasks(mediaObj->driverObj);
                }
            }

            if (mediaObj->commandStatus != SYS_FS_MEDIA_COMMAND_COMPLETED)
            {
                /* Unmute the event notification */
                gSYSFSMediaManagerObj.muteEventNotification = false;

                /* Media write operation failed. */
                return SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID;
            }

            /* Update the number of block still to be written, sector address
             * and the buffer pointer */
            numSectors -= numSectorsToWrite;
            sector += numSectorsToWrite;
            dataBuffer += (numSectorsToWrite << 9);
        }
    }

    /* Unmute the event notification */
    gSYSFSMediaManagerObj.muteEventNotification = false;

    mediaObj->commandStatus = SYS_FS_MEDIA_COMMAND_IN_PROGRESS;
    mediaObj->driverFunctions->sectorWrite (mediaObj->driverHandle, &(mediaObj->commandHandle), data, memoryBlock, 1);

    return (mediaObj->commandHandle);
}

//*****************************************************************************
/* Function:
    uintptr_t SYS_FS_MEDIA_MANAGER_AddressGet
    (
        uint16_t diskNo
    );

  Summary:
    Gets the starting media address based on a disk number.

  Description:
    This function gets the starting address of a media. This function is
    intended to work only with MPFS2, which does byte addressing and needs a
    memory address (not disk number).

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
uintptr_t SYS_FS_MEDIA_MANAGER_AddressGet
(
    uint16_t diskNum
)
{
    SYS_FS_MEDIA *mediaObj = NULL;

    if (diskNum >= SYS_FS_MEDIA_NUMBER)
    {
        SYS_ASSERT(false, "Invalid Disk");
        return 0;
    }

    mediaObj = &gSYSFSMediaManagerObj.mediaObj[diskNum];
    return (mediaObj->driverFunctions->addressGet(mediaObj->driverHandle));
}

//*****************************************************************************
/* Function:
    SYS_FS_MEDIA_COMMAND_STATUS SYS_FS_MEDIA_MANAGER_CommandStatusGet
    (
        uint16_t diskNo,
        SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE bufferHandle
    );

  Summary:
    Gets the command status.

  Description:
    This function gets the command status. The sector read and sector write are
    non-blocking functions. Therefore, this interface is provided where the
    code should periodically poll for the buffer status. If status is
    completed, the read/write operation is considered to be complete.

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
SYS_FS_MEDIA_COMMAND_STATUS SYS_FS_MEDIA_MANAGER_CommandStatusGet
(
    uint16_t diskNum,
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE commandHandle
)
{
    SYS_FS_MEDIA *mediaObj = NULL;

    if (diskNum >= SYS_FS_MEDIA_NUMBER)
    {
        SYS_ASSERT(false, "Invalid Disk");
        return SYS_FS_MEDIA_COMMAND_UNKNOWN;
    }

    mediaObj = &gSYSFSMediaManagerObj.mediaObj[diskNum];

    if (mediaObj->driverHandle == DRV_HANDLE_INVALID)
    {
        return SYS_FS_MEDIA_COMMAND_UNKNOWN;
    }

    return (mediaObj->driverFunctions->commandStatusGet(mediaObj->driverHandle, commandHandle));
}

//*****************************************************************************
/* Function:
    bool SYS_FS_MEDIA_MANAGER_MediaStatusGet
    (
        const char *devName
    );

  Summary:
    Gets the media status.

  Description:
    This function gets the media status. This function is used by higher layers
    (sys_fs layer) to know the status of the media(whether the media is attached
    or detached).

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
bool SYS_FS_MEDIA_MANAGER_MediaStatusGet
(
    const char *volumeName
)
{
    SYS_FS_VOLUME * volumeObj =  NULL;
    uint8_t volumeIndex = 0;

    for (volumeIndex = 0; volumeIndex < SYS_FS_VOLUME_NUMBER; volumeIndex++)
    {
        volumeObj = &gSYSFSMediaManagerObj.volumeObj[volumeIndex];
        if (volumeObj->inUse == true)
        {
            if (strncmp("/dev/", volumeName, 5))
            {
                if (strcmp((const char*)(volumeName), (const char *)volumeObj->volumeName) == 0)
                {
                    return (volumeObj->obj->driverFunctions->mediaStatusGet(volumeObj->obj->driverHandle));
                }
            }
            else
            {
                if (strcmp((const char*)(volumeName + 5), (const char *)volumeObj->volumeName) == 0)
                {
                    return (volumeObj->obj->driverFunctions->mediaStatusGet(volumeObj->obj->driverHandle));
                }
            }
        }
    }

    return SYS_FS_MEDIA_DETACHED;
}

// *****************************************************************************
/* Function:
    bool SYS_FS_MEDIA_MANAGER_VolumePropertyGet
    (
        const char *volumeName,
        SYS_FS_VOLUME_PROPERTY *property
    );

  Summary:
    Gets the volume property.

  Description:
    This function gets the property of the volume. This function is used by
    higher layers (sys_fs layer) to know the property of the volume as
    specified in the SYS_FS_VOLUME_PROPERTY structure.

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
bool SYS_FS_MEDIA_MANAGER_VolumePropertyGet
(
    const char *volumeName,
    SYS_FS_VOLUME_PROPERTY *property
)
{
    SYS_FS_VOLUME *volumeObj = NULL;
    uint8_t volumeIndex = 0;

    for (volumeIndex = 0; volumeIndex < SYS_FS_VOLUME_NUMBER; volumeIndex++)
    {
        volumeObj = &gSYSFSMediaManagerObj.volumeObj[volumeIndex];

        if (volumeObj->inUse == true)
        {
            if (strcmp((const char*)(volumeName + 5), (const char *)volumeObj->volumeName) == 0)
            {
                if (volumeObj->fsType == 'M')
                {
                    /* MPFS File System */
                    property->fsType = MPFS2;
                }
                else
                {
                    /* Unsupported file system or no file system */
                    property->fsType = UNSUPPORTED_FS;
                }

                property->volNumber = volumeIndex;
                return true;
            }
        }
    }

    return false;
}


//*****************************************************************************
/* Function:
    void SYS_FS_MEDIA_MANAGER_RegisterTransferHandler
    (
        const void *eventHandler
    );

  Summary:
    Register the event handler for data transfer events.

  Description:
    This function is used to send the command status for the disk operation.

***************************************************************************/
void SYS_FS_MEDIA_MANAGER_RegisterTransferHandler
(
    const void *eventHandler
)
{
    gSYSFSMediaManagerObj.eventHandler = (SYS_FS_EVENT_HANDLER) eventHandler;
}

//*****************************************************************************
/* Function:
    void SYS_FS_MEDIA_MANAGER_EventHandler
    (
        SYS_FS_MEDIA_BLOCK_EVENT event,
        SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE commandHandle,
        uintptr_t context
    );

  Summary:
    Registers the event handler with media drivers.

  Description:
    This function registers the event handler with media drivers. The event
    handler will be called by media drivers whenever a media command has
    executed successfully or if it resulted in an error.

  Remarks:
    None.
*/
void SYS_FS_MEDIA_MANAGER_EventHandler
(
    SYS_FS_MEDIA_BLOCK_EVENT event,
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE commandHandle,
    uintptr_t context
)
{
    switch(event)
    {
        case SYS_FS_MEDIA_EVENT_BLOCK_COMMAND_COMPLETE:
            ((SYS_FS_MEDIA*)context)->commandStatus = SYS_FS_MEDIA_COMMAND_COMPLETED;
            break;
        case SYS_FS_MEDIA_EVENT_BLOCK_COMMAND_ERROR:
            ((SYS_FS_MEDIA*)context)->commandStatus = SYS_FS_MEDIA_COMMAND_UNKNOWN;
            break;
        default:
            break;
    }

    if ((gSYSFSMediaManagerObj.eventHandler != NULL) && (gSYSFSMediaManagerObj.muteEventNotification == false))
    {
        gSYSFSMediaManagerObj.eventHandler ((SYS_FS_EVENT)event, (void *)commandHandle, ((SYS_FS_MEDIA*)context)->mediaIndex);
    }
}

//*****************************************************************************
/* Function:
    SYS_FS_MEDIA_GEOMETRY * SYS_FS_MEDIA_MANAGER_GetMediaGeometry
    (
        uint16_t diskNum
    );

  Summary:
    Gets the media geometry information.

  Description:
    This function gets the media geometry information.

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
SYS_FS_MEDIA_GEOMETRY * SYS_FS_MEDIA_MANAGER_GetMediaGeometry
(
    uint16_t diskNum
)
{
    SYS_FS_MEDIA *mediaObj = NULL;

    if (diskNum >= SYS_FS_MEDIA_NUMBER)
    {
        SYS_ASSERT(false, "Invalid Disk");
        return NULL;
    }

    mediaObj = &gSYSFSMediaManagerObj.mediaObj[diskNum];

    if (mediaObj->attachStatus == SYS_FS_MEDIA_DETACHED)
    {
        return NULL;
    }

    return mediaObj->mediaGeometry;
}

//*****************************************************************************
/* Function:
    void SYS_FS_MEDIA_MANAGER_TransferTask
    (
        uint8_t mediaIndex
    );

  Summary:
    Media manager transfer task function.

  Description:
    This is the media manager transfer task function. This task is repeatedly
    called by the disk io layer of the native file system for driving the
    current disk read/write operation to completion.

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
void SYS_FS_MEDIA_MANAGER_TransferTask
(
    uint8_t mediaIndex
)
{
    SYS_FS_MEDIA *mediaObj;

    if (mediaIndex >= SYS_FS_MEDIA_NUMBER)
    {
        return;
    }

    mediaObj = &gSYSFSMediaManagerObj.mediaObj[mediaIndex];

    if (mediaObj->inUse == false)
    {
        return;
    }

    if (mediaObj->driverFunctions->tasks != NULL)
    {
        mediaObj->driverFunctions->tasks(mediaObj->driverObj);
    }
}

//*****************************************************************************
/* Function:
    void SYS_FS_MEDIA_MANAGER_Tasks(void)

  Summary:
    Media manager task function.

  Description:
    This is the media manager task function. This task must be called
    repeatedly from the main loop.

  Remarks:
    See sys_fs_media_manager.h for usage information.
***************************************************************************/
void SYS_FS_MEDIA_MANAGER_Tasks
(
    void
)
{
    uint8_t mediaIndex = 0;
    uint8_t fsType = 0xFF;
    uint8_t partitionMap = 0;
    uint8_t isMBR = 0;
    uint32_t numSectors = 0;
    uint32_t mediaReadBlockSize = 0;

    SYS_FS_MEDIA *mediaObj = NULL;

    /* Find the next media to be processed */
    mediaIndex = _SYS_FS_MEDIA_MANAGER_FindNextMedia (&gSYSFSMediaManagerObj.mediaObj[0], &gSYSFSMediaManagerObj.mediaIndex);
    if (mediaIndex == 0xFF)
    {
        /* No media attached. Do nothing. */
        return;
    }

    mediaObj = &gSYSFSMediaManagerObj.mediaObj[mediaIndex];

    if (mediaObj->isMediaDisconnected == true)
    {
        /* If the media driver was de-registered in this state, then the media
         * had use of the media buffer and no longer needs it. */
         if(SYS_FS_MEDIA_ANALYZE_FS == mediaObj->mediaState)
            gSYSFSMediaManagerObj.bufferInUse =false;

        /* If media driver has de-registered then switch to the DEREGISTERED
         * state and handle the media detach. */
        mediaObj->mediaState = SYS_FS_MEDIA_STATE_DEREGISTERED;
    }

    switch (mediaObj->mediaState)
    {
        case SYS_FS_MEDIA_STATE_REGISTERED:
            {
                /* Initial state. Open the media driver. */
                mediaObj->driverHandle = mediaObj->driverFunctions->open(mediaObj->driverIndex, DRV_IO_INTENT_READWRITE);
                if (mediaObj->driverHandle != DRV_HANDLE_INVALID)
                {
                    /* Media driver open successful. Register an event handler for
                     * the media driver events. */
                    mediaObj->driverFunctions->eventHandlerset(
                                                mediaObj->driverHandle,
                                                (const void *) SYS_FS_MEDIA_MANAGER_EventHandler,
                                                (uintptr_t) mediaObj
                                                );

                    /* Transition to the next state. */
                    mediaObj->mediaState = SYS_FS_MEDIA_CHECK_ATTACH_STATUS;
                }
                else
                {
                    /* Media driver open failed. Stay in the same state and
                     * retry. */
                }

                break;
            }

        case SYS_FS_MEDIA_CHECK_ATTACH_STATUS:
            {
                /* Check if the Media is attached. */
                if (mediaObj->driverFunctions->mediaStatusGet(mediaObj->driverHandle) == true)
                {
                    if (mediaObj->attachStatus == SYS_FS_MEDIA_DETACHED)
                    {
                        /* The media was earlier detached. But now it is
                         * attached. Read the media geometry. */
                        mediaObj->mediaGeometry = mediaObj->driverFunctions->mediaGeometryGet(mediaObj->driverHandle);

                        /* Transition to the next state to kick start the
                         * analysis of the FS on the media. */
                        mediaObj->mediaState = SYS_FS_MEDIA_READ_FIRST_SECTOR;
                    }

                    /* Update the media status */
                    mediaObj->attachStatus = SYS_FS_MEDIA_ATTACHED;
                }
                else
                {
                    if (mediaObj->attachStatus == SYS_FS_MEDIA_ATTACHED)
                    {
                        /* The media was earlier attached. But now it is
                         * detached. Handle the media detach. */
                        _SYS_FS_MEDIA_MANAGER_HandleMediaDetach (mediaObj);

                        /* Reset the media's number of volumes field */
                        mediaObj->numVolumes = 0;
                    }

                    /* Update the media status */
                    mediaObj->attachStatus = SYS_FS_MEDIA_DETACHED;

                    /* Stay in the same state */
                }

                break;
            }

        case SYS_FS_MEDIA_READ_FIRST_SECTOR:
            {
                if (gSYSFSMediaManagerObj.bufferInUse == true)
                {
                    /* Stay in the same state till the buffer becomes free. */
                    break;
                }

                gSYSFSMediaManagerObj.bufferInUse = true;

                numSectors = 1;

                mediaReadBlockSize = mediaObj->mediaGeometry->geometryTable[0].blockSize;

                if (mediaReadBlockSize < 512)
                {
                    /* Perform sector to block translation */
                    numSectors *= (512 / mediaReadBlockSize);
                }

                memset (gSYSFSMediaManagerObj.mediaBuffer, 0, SYS_FS_MEDIA_MAX_BLOCK_SIZE);

                /* Update the command status */
                mediaObj->commandStatus = SYS_FS_MEDIA_COMMAND_IN_PROGRESS;

                /* Read the first sector of the media */
                mediaObj->driverFunctions->sectorRead(mediaObj->driverHandle, &(mediaObj->commandHandle), gSYSFSMediaManagerObj.mediaBuffer, 0, numSectors);

                if (mediaObj->commandHandle != SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID)
                {
                    mediaObj->mediaState = SYS_FS_MEDIA_ANALYZE_FS;
                }
                else
                {
                    /* Failed to queue the Media read operation. Retry the
                     * operation. Clear the buffer in use flag. */
                    gSYSFSMediaManagerObj.bufferInUse = false;
                }

                break;
            }

        case SYS_FS_MEDIA_ANALYZE_FS:
            {
                if (SYS_FS_MEDIA_COMMAND_IN_PROGRESS == mediaObj->commandStatus)
                {
                    /* The request is not complete yet. Stay in the same state.
                     * */
                    if(mediaObj->driverFunctions->tasks != NULL)
                    {
                        mediaObj->driverFunctions->tasks(mediaObj->driverObj);
                    }
                    break;
                }

                if (mediaObj->commandStatus != SYS_FS_MEDIA_COMMAND_COMPLETED)
                {
                    /* Clear the buffer in use flag. */
                    gSYSFSMediaManagerObj.bufferInUse = false;

                    /* Media read operation has failed. Retry the read
                     * operation. */
                    mediaObj->mediaState = SYS_FS_MEDIA_READ_FIRST_SECTOR;

                    break;
                }

                fsType = _SYS_FS_MEDIA_MANAGER_AnalyzeFileSystem(gSYSFSMediaManagerObj.mediaBuffer, &mediaObj->numPartitions, &isMBR, &partitionMap);

                if (fsType == 0xFF)
                {
                    /* File system not found or found an unsupported file
                     * system. Allocate a volume so as to allow for formatting
                     * of the disk. */
                    if (isMBR)
                    {
                        partitionMap = 0x01;
                    }
                    else
                    {
                        partitionMap = 0x00;
                    }
                }

                _SYS_FS_MEDIA_MANAGER_PopulateVolume (mediaObj, isMBR, partitionMap, fsType);

                /* Clear the buffer in use flag. */
                gSYSFSMediaManagerObj.bufferInUse = false;

                /* Transition state to check for media attach/detach */
                mediaObj->mediaState = SYS_FS_MEDIA_CHECK_ATTACH_STATUS;

                break;
            }

        case SYS_FS_MEDIA_STATE_DEREGISTERED:
            {
                if (mediaObj->attachStatus == SYS_FS_MEDIA_ATTACHED)
                {
                    /* The media was earlier attached. But now it is
                     * detached. Handle the media detach. */
                    _SYS_FS_MEDIA_MANAGER_HandleMediaDetach (mediaObj);
                }

                mediaObj->inUse = false;
                mediaObj->attachStatus = SYS_FS_MEDIA_DETACHED;
                mediaObj->isMediaDisconnected = false;
                mediaObj->mediaId = 0;
                mediaObj->mediaIndex = 0;
                mediaObj->numPartitions = 0;
                mediaObj->numVolumes = 0;
                break;
            }

        default:
            break;
    }
}

/*************************************************************************
* END OF sys_fs_media_manager.c
***************************************************************************/
