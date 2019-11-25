/*******************************************************************************
  File System System-Library Interface Implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    sys_fs.c

  Summary:
    This file contains implementation of SYS_FS functions.

  Description:
    This file contains implementation of SYS_FS functions.
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


#include "system/fs/src/sys_fs_local.h"
#include "system/fs/sys_fs_media_manager.h"
#include <stdarg.h>
#include <string.h>

// *****************************************************************************
/* Registration table for each native file system

  Summary:
    Defines the registration table for each native file system.

  Description:
    Defines the registration table for each native file system.

  Remarks:
    None
*/
SYS_FS_REGISTRATION_TABLE gSYSFSObj[SYS_FS_MAX_FILE_SYSTEM_TYPE];

// *****************************************************************************
/* Mount point

  Summary:
    Defines the mount point required for mounting each volume.

  Description:
    Defines the mount point required for mounting each volume.

  Remarks:
    None
*/
SYS_FS_MOUNT_POINT gSYSFSMountPoint[SYS_FS_VOLUME_NUMBER];

// *****************************************************************************
/* Current Mount point

  Summary:
    Defines the mount point currently being used.

  Description:
    Defines the mount point which will be used as default, in case the application
    do not specify the full path name along with the drive name.

  Remarks:
    None
*/
SYS_FS_CURRENT_MOUNT_POINT  gSYSFSCurrentMountPoint;

// *****************************************************************************
/* File object

  Summary:
    Defines the File object for each file opened.

  Description:
    Defines the File object for each file opened on the system.

  Remarks:
    None
*/
SYS_FS_OBJ gSYSFSFileObj[SYS_FS_MAX_FILES];

// *****************************************************************************
/* Directory object

  Summary:
    Defines the Directory object for each directory opened.

  Description:
    Defines the Directorye object for each directory opened on the system.

  Remarks:
    None
*/
SYS_FS_DIR_OBJ  gSYSFSDirObj[SYS_FS_MAX_FILES];

/* File system object instance mutex */
OSAL_MUTEX_DECLARE(gSysFsMutex);
/* Variable to hold the error value */
SYS_FS_ERROR errorValue;

//******************************************************************************
/*Function:
    static bool SYS_FS_GetDisk
    (
        const char *path, 
        SYS_FS_MOUNT_POINT **disk,
        uint8_t *buffer
    )

   Summary:
     Retrieves the disk from the name of the disk.

   Description:
     Retrieves the disk from the name of the disk.

   Remarks:
     None
***************************************************************************/

static bool SYS_FS_GetDisk
(
    const char *path, 
    SYS_FS_MOUNT_POINT **disk,
    uint8_t *buffer
)
{
    SYS_FS_MOUNT_POINT *volume = NULL;
    const char *ptr = NULL;
    uint16_t pathLength = 0; 
    uint8_t index = 0;
    uint8_t mountNameLength = 0;
    bool status = false;

    ptr = path;
    pathLength = strlen (ptr);

    if (strncmp((const char*)path, (const char *)"/mnt/", 5) == 0)
    {
        ptr = path + 5;
        pathLength -= 5;

        /* Start with 0th index and find an index that is free. */
        for (index = 0; index != SYS_FS_VOLUME_NUMBER; index++)
        {
            volume = &gSYSFSMountPoint[index];
            if (volume->inUse == true)
            {
                /* Find the volume with the matching "mountName". Ignore the
                 * first 5 chars --> "/mnt/". */
                mountNameLength = volume->mountNameLength;
                if (strncmp (ptr, volume->mountName, mountNameLength) == 0)
                {
                    /* Found the matching volume. */
                    *disk = volume;

                    ptr += (mountNameLength);
                    pathLength -= (mountNameLength);

                    status = true;
                    break;
                }
            }
        }

        if (!status)
        {
            /* Invalid volume. */
            errorValue = SYS_FS_ERROR_INVALID_NAME;
        }
    }
    else
    {
        /* Since the path does not contain the "mountName", assume the default
           volume. */
        if (gSYSFSCurrentMountPoint.inUse == true)
        {
            *disk = gSYSFSCurrentMountPoint.currentDisk;
            status = true;
        }
        else
        {
            errorValue = SYS_FS_ERROR_NO_FILESYSTEM;
        }
    }

    if ((status) && (buffer != NULL))
    {
        if ((pathLength + 3) > SYS_FS_PATH_LEN_WITH_DISK_NUM)
        {
            errorValue = SYS_FS_ERROR_INVALID_NAME;
            status = false;
        }
        else
        {
            memset(buffer, 0, pathLength + 3);

            /* Append "0:" before the file name. This is required for different disks
             * */
            *buffer++ = (*disk)->diskNumber + '0';
            *buffer++ = ':';

            if (pathLength)
            {
                strncpy((char *)buffer, (const char *)ptr, pathLength);
                pathLength ++;
            }

            buffer[pathLength] = '\0';

            /* The status variable is already set to true. */
        }
    }

    return status;
}

//******************************************************************************
/*Function:
    static bool SYS_FS_StringWildCardCompare
    (
        const char * ptr1, 
        const char *ptr2
    )

  Summary:
    Compares the 2 file names with wild character.

  Description:
    Compares the 2 file names with wild character. This is required for file
    search of a directory.

  Remarks:
    None
***************************************************************************/
static bool SYS_FS_StringWildCardCompare
(
    const char * ptr1,
    const char *ptr2
)
{
    do
    {
        if((*ptr1 != '?') && (*ptr1 != '*') && (*ptr1 != *ptr2))
        {
            return false;
        }
        else
        {
           if(*ptr1 == '*')
            {
                ptr1++;
                if(*ptr1 == '\0')
                    return true;
                else if(*ptr1 == '?')
                    continue;
                else
                {
                    // Find the same character in second string as well
                    while((*ptr1 != *ptr2) && (*ptr2 != '\0'))
                    {
                        ptr2++;
                    }
                    if(*ptr2 == '\0')
                        return false;
                }
            }
            else
            {
                ptr1++;
                ptr2++;
            }
        }
    }while(*ptr1 != '\0');

    if((*ptr1 == '\0') && (*ptr2 == '\0'))
    {
        return true;
    }
    return false;
}


//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_Initialize
    (
        const void* initData
    )

  Summary:
    Initializes the file system abstraction layer (sys_fs layer).

  Description:
    This function initializes the abstraction layer (sys_fs layer) and sets up
    the necessary parameters.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/

SYS_FS_RESULT SYS_FS_Initialize
(
    const void* initData
)
{
    SYS_FS_REGISTRATION_TABLE *init = (SYS_FS_REGISTRATION_TABLE *)initData;
    uint8_t index = 0;
    
    if (init == NULL)
    {
        return SYS_FS_RES_FAILURE;
    }

    for (index = 0; index != SYS_FS_MAX_FILES; index++)
    {
        gSYSFSFileObj[index].inUse = false;
        gSYSFSFileObj[index].mountPoint = NULL;
        gSYSFSFileObj[index].nativeFSFileObj = (uintptr_t)NULL;
        memset(gSYSFSFileObj[index].fileName, 0, FAT_FS_MAX_LFN);
        gSYSFSFileObj[index].errorValue = SYS_FS_ERROR_OK;

        gSYSFSDirObj[index].inUse = false;
        gSYSFSDirObj[index].mountPoint = NULL;
        gSYSFSDirObj[index].nativeFSDirObj = (uintptr_t)NULL;
        gSYSFSDirObj[index].errorValue = SYS_FS_ERROR_OK;
    }
    
    for (index = 0; index != SYS_FS_MAX_FILE_SYSTEM_TYPE; index++)
    {
        gSYSFSObj[index].nativeFileSystemType = init->nativeFileSystemType;
        gSYSFSObj[index].nativeFileSystemFunctions = init->nativeFileSystemFunctions;
        init++;
    }

    if (OSAL_MUTEX_Create(&gSysFsMutex) != OSAL_RESULT_TRUE)
    {
        return SYS_FS_RES_FAILURE;
    }
    
    for (index = 0; index != SYS_FS_VOLUME_NUMBER; index++)
    {
        if (OSAL_MUTEX_Create(&(gSYSFSMountPoint[index].mutexDiskVolume)) != OSAL_RESULT_TRUE)
        {
            return SYS_FS_RES_FAILURE;
        }
    }

    return SYS_FS_RES_SUCCESS;
}

// *****************************************************************************
/* Function:
    void SYS_FS_Tasks
    (
        void
    )

  Summary:
    Maintains the File System tasks and functionalities.

  Description:
    This function is used to run the various tasks and functionalities of
    sys_fs layer.

  Returns:
    See sys_fs.h for usage information.
*/

void SYS_FS_Tasks ( void )
{
    /* Task routine for media manager */
    SYS_FS_MEDIA_MANAGER_Tasks();
}

#if (SYS_FS_AUTOMOUNT_ENABLE == true)
//******************************************************************************
/*Function:
    void SYS_FS_EventHandlerSet
    (
        const void* eventHandler, 
        const uintptr_t context
    )

  Summary:
    Allows a client to identify an event handling function for the file system
    to call back when mount/unmount operation has completed.

  Description:
    This function allows a client to identify an event handling function for
    the File System to call back when mount/unmount operation has completed.
    The file system will pass mount name back to the client by calling
    "eventHandler".

  Returns:
    None

    See sys_fs.h for usage information.
***************************************************************************/
void SYS_FS_EventHandlerSet
(
    const void * eventHandler,
    const uintptr_t context
)
{
    SYS_FS_MEDIA_MANAGER_EventHandlerSet(eventHandler, context);
}

#endif // SYS_FS_AUTOMOUNT_ENABLE == true

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_Mount
    (
        const char *devName,
        const char *mountName,
        SYS_FS_FILE_SYSTEM_TYPE filesystemtype,
        unsigned long mountflags,
        const void *data
    );

  Summary:
    Mounts the file system.

  Description:
    The mount command attaches the file system specified to a volume. The call
    to the mount should be non blocking in nature.  The application code has to
    allow the SYS_FS_Tasks to run periodically while calling the SYS_FS_Mount
    function. If the SYS_FS_Mount is called in a blocking mode, then the
    SYS_Tasks() never gets a chance to run and therefore, the media will not be
    analyzed and finally, the SYS_FS_Mount will never succeed.  This will
    result in a deadlock.

    There is no mechanism available for the application to know if the
    specified volume (devName) is really attached or not. The only available
    possibility is to keep trying to mount the volume (with the devname), until
    success is achieved.
      
    It is prudent that the application code implements a time-out mechanism
    while trying to mount a volume (by calling SYS_FS_Mount). The trial for
    mount should continue at least 10 times before before assuming that the
    mount will never succeed. This has to be done for every new volume to be
    mounted.

    The standard names for volumes (devName) used in the MPLAB Harmony file
    system is as follows:
        NVM       - "nvm"    "media number" "volume number"
        SD card   - "mmcblk" "media number" "volume number"
        MSD       - "sd"     "media number" "volume number"

    Where, "media number" a, b, c... depends on the number of the type of
    connected media, and where, "volume number" 1, 2, 3... depends on the
    number of partitions in that media.

    The convention for assigning names to volumes is further described below
    with examples:

    If a SD card (with four partitions) is attached to the system, and assuming
    all four partitions are recognized, there will be four devNames:
      1. mmcblka1
      2. mmcblka2
      3. mmcblka3 and 
      4. mmcblka4

    Subsequently, if NVM media is attached that has only one partition, the
    devname will be: nvma1.

    Later, if another SD card is attached to the system that has one partition,
    the devname will be mmcblkb1.

    Finally, there will be six volume names (or devNames), which are available
    for the application to be mounted and used for the file system.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/

SYS_FS_RESULT SYS_FS_Mount
(
    const char *devName,
    const char *mountName,
    SYS_FS_FILE_SYSTEM_TYPE filesystemtype,
    unsigned long mountflags,
    const void *data
)
{
    int fileStatus = -1;
    SYS_FS_MOUNT_POINT *disk = NULL;
    SYS_FS_VOLUME_PROPERTY volumeProperty;
    uint8_t index = 0;
    uint8_t mountPoint = 0;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;
    bool status = false;

    const char *ptr = NULL;

    (void)mountflags;
    (void)data;
    /* Validate the parameters. */
    if ((devName == NULL) || (mountName == NULL) || ((filesystemtype != FAT) && (filesystemtype != MPFS2)))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    /* Check if the media has been attached, analyzed and allocated a volume. */
    if (SYS_FS_MEDIA_MANAGER_MediaStatusGet(devName) != true)
    {
        errorValue = SYS_FS_ERROR_NOT_READY;
        return SYS_FS_RES_FAILURE;
    }

    /* Clear the error value when mount is sucessful */
    errorValue = SYS_FS_ERROR_OK;

    /* Verify if the requested file system is supported by SYS_FS */
    for (index = 0; index != SYS_FS_MAX_FILE_SYSTEM_TYPE; index++)
    {
        if (gSYSFSObj[index].nativeFileSystemType == filesystemtype)
        {
            break;
        }
    }

    if (index >= SYS_FS_MAX_FILE_SYSTEM_TYPE)
    {
        /* The requested file system is not supported. */
        errorValue = SYS_FS_ERROR_FS_NOT_SUPPORTED;
        return SYS_FS_RES_FAILURE;
    }

    /* Fetch the properties of the volume from the media manager. */
    status = SYS_FS_MEDIA_MANAGER_VolumePropertyGet(devName, &volumeProperty);
    if (status == false)
    {
        errorValue = SYS_FS_ERROR_DISK_ERR;
        return SYS_FS_RES_FAILURE;
    }

    /* Check if the volume property matches. */
    if ((volumeProperty.fsType != filesystemtype) && (volumeProperty.fsType != UNSUPPORTED_FS))
    {
        errorValue = SYS_FS_ERROR_FS_NOT_MATCH_WITH_VOLUME;
        return SYS_FS_RES_FAILURE;
    }

    /* Acquire a mutex. */
    osalResult = OSAL_MUTEX_Lock(&gSysFsMutex, OSAL_WAIT_FOREVER);
    if (osalResult != OSAL_RESULT_TRUE)
    {
        errorValue = SYS_FS_ERROR_DENIED;
        return SYS_FS_RES_FAILURE;
    }

    /* Find a volume/mount point that is not in use. */
    status = false;
    for (mountPoint = 0; mountPoint != SYS_FS_VOLUME_NUMBER; mountPoint++)
    {
        disk = &gSYSFSMountPoint[mountPoint];
        if (disk->inUse == false)
        {
            status = true;
            break;
        }
    }

    if (status == false)
    {
        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock (&gSysFsMutex);

        errorValue = SYS_FS_ERROR_NOT_ENOUGH_FREE_VOLUME;
        return SYS_FS_RES_FAILURE;
    }

    /* Populate the disk members. */
    disk->fsType = filesystemtype;
    disk->fsFunctions = gSYSFSObj[index].nativeFileSystemFunctions;

    /* Find the mount name length and save it for later use. */
    disk->mountNameLength = 0;
    ptr = (mountName + 5);
    while ((*ptr != '/') && (*ptr != '\0'))
    {
        ptr++;
        disk->mountNameLength ++;
    }

    disk->mountName = (mountName + 5);
    disk->diskNumber = volumeProperty.volNumber;

    disk->inUse = true;

    /* Put the recently assigned disk as the current disk */
    gSYSFSCurrentMountPoint.inUse = true;
    gSYSFSCurrentMountPoint.currentDisk = disk;

    /* Release the acquired mutex. */
    OSAL_MUTEX_Unlock (&gSysFsMutex);

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult != OSAL_RESULT_TRUE)
    {
        errorValue = SYS_FS_ERROR_DENIED;
        return SYS_FS_RES_FAILURE;
    }

    if (disk->fsFunctions->chdrive != NULL)
    {
        disk->fsFunctions->chdrive(disk->diskNumber);
        fileStatus = SYS_FS_ERROR_OK;
    }
    else
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
    }

    if (disk->fsFunctions->mount != NULL)
    {
        fileStatus = disk->fsFunctions->mount(disk->diskNumber);
                errorValue = (SYS_FS_ERROR)fileStatus;
        if (fileStatus == SYS_FS_ERROR_NO_FILESYSTEM)
        {
            fileStatus = 0;
        }        
    }
    else
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        fileStatus = -1;
    }

    /* Release the acquired mutex. */
    OSAL_MUTEX_Unlock (&(disk->mutexDiskVolume));

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_Unmount
    (
        const char *fname
    );

  Summary:
    Unmounts the file system.

  Description:
    This function removes (unmounts) the attachment of the volume from the file
    system.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/

SYS_FS_RESULT SYS_FS_Unmount
(
    const char *fname
)
{
    int fileStatus = -1;
    uint32_t index = 0;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;
    SYS_FS_MOUNT_POINT *disk = NULL;

    if (fname == NULL)
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(fname, &disk, NULL) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    /* Acquire the mutex. */
    osalResult = OSAL_MUTEX_Lock (&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult != OSAL_RESULT_TRUE)
    {
        errorValue = SYS_FS_ERROR_DENIED;
        return SYS_FS_RES_FAILURE;
    }
         
    if (disk->fsFunctions->unmount != NULL)
    {
        fileStatus = disk->fsFunctions->unmount(disk->diskNumber);
        errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
    }

    OSAL_MUTEX_Unlock (&(disk->mutexDiskVolume));

    if (fileStatus == 0)
    {
        /* Acquire the mutex. */
        osalResult = OSAL_MUTEX_Lock (&gSysFsMutex, OSAL_WAIT_FOREVER);
        if (osalResult != OSAL_RESULT_TRUE)
        {
            errorValue = SYS_FS_ERROR_DENIED;
            return SYS_FS_RES_FAILURE;
        }

        for (index = 0; index != SYS_FS_MAX_FILES; index++)
        {
            if (gSYSFSFileObj[index].mountPoint == disk)
            {
                gSYSFSFileObj[index].inUse = false;
            }

            if (gSYSFSDirObj[index].mountPoint == disk)
            {
                gSYSFSDirObj[index].inUse = false;
            }            
        }

        disk->inUse = false;
        disk->mountName = NULL;
        disk->fsFunctions = NULL;
        disk->mountNameLength = 0;
        
        /* Reset the current mount point if it is set to the current disk. */
        if ((gSYSFSCurrentMountPoint.inUse == true) && (gSYSFSCurrentMountPoint.currentDisk == disk))
        {
            gSYSFSCurrentMountPoint.inUse = false;
        }
        
        /* Release the mutex. */
        OSAL_MUTEX_Unlock (&gSysFsMutex);
        
        return SYS_FS_RES_SUCCESS;
    }
    else
    {
        return SYS_FS_RES_FAILURE;
    }
}

//******************************************************************************
/* Function:
    SYS_FS_HANDLE SYS_FS_FileOpen
    (
        const char* fname, 
        SYS_FS_FILE_OPEN_ATTRIBUTES attributes
    );

  Summary:
    Opens a file

  Description:
    This function opens a file with the requested attributes.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/

SYS_FS_HANDLE SYS_FS_FileOpen
(
    const char *fname,
    SYS_FS_FILE_OPEN_ATTRIBUTES attributes
)
{
    int fileStatus = -1;
    uint16_t index = 0;
    uint8_t mode = 0;
    SYS_FS_OBJ *fileObj = NULL;
    SYS_FS_MOUNT_POINT *disk = NULL;
    uint8_t pathWithDiskNo[SYS_FS_PATH_LEN_WITH_DISK_NUM] = { 0 };
    uint8_t *ptr = NULL;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Get the disk from the name, and also append the disk number prior to the
     * file name like this "0:file.txt" */
    if (SYS_FS_GetDisk(fname, &disk, pathWithDiskNo) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_HANDLE_INVALID;
    }

    /* For MPFS file system, opening a file is possible only in "READ" mode.
     * TODO: These type of checks should be done by the native file system. */
    if ((disk->fsType == MPFS2) && (attributes != SYS_FS_FILE_OPEN_READ))
    {
        errorValue = SYS_FS_ERROR_DENIED;
        return SYS_FS_HANDLE_INVALID;
    }

    /* Acquire the mutex. */
    osalResult = OSAL_MUTEX_Lock(&gSysFsMutex, OSAL_WAIT_FOREVER);
    if (osalResult != OSAL_RESULT_TRUE)
    {
        errorValue = SYS_FS_ERROR_DENIED;
        return SYS_FS_HANDLE_INVALID;
    }

    /* Find a free file object. */
    for (index = 0; index < SYS_FS_MAX_FILES; index ++)
    {
        fileObj = &gSYSFSFileObj[index];
        if (fileObj->inUse == false)
        {
            fileObj->inUse = true;
            fileObj->mountPoint = disk;
            break;
        }
    }

    /* Release the mutex. */
    OSAL_MUTEX_Unlock(&gSysFsMutex);

    if (index >= SYS_FS_MAX_FILES)
    {
        /* Could not find a free file object. */
        errorValue = SYS_FS_ERROR_TOO_MANY_OPEN_FILES;
        return SYS_FS_HANDLE_INVALID;
    }

    /* Save the file name for future use. Ignore the "0:/" */
    ptr = &pathWithDiskNo[3];
    if (pathWithDiskNo[2] != '/')
    {
        /* Ignore the "0:" */
        ptr = &pathWithDiskNo[2];
    }

    for (index = 0; index < FAT_FS_MAX_LFN; index ++)
    {
        fileObj->fileName[index] = *ptr;
        if(*ptr++ == '\0')
        {
            break;
        }
    }

    /* Convert the SYS_FS file open attributes to FAT FS attributes */
    switch(attributes)
    {
        case SYS_FS_FILE_OPEN_READ:
            mode = FA_READ;
            break;
        case SYS_FS_FILE_OPEN_WRITE:
             mode = FA_WRITE | FA_CREATE_ALWAYS;
            break;
        case SYS_FS_FILE_OPEN_APPEND:
            mode = FA_WRITE | FA_OPEN_ALWAYS;
            break;
        case SYS_FS_FILE_OPEN_READ_PLUS:
            mode = FA_READ | FA_WRITE;
            break;
        case SYS_FS_FILE_OPEN_WRITE_PLUS:
            mode = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;
            break;
        case SYS_FS_FILE_OPEN_APPEND_PLUS:
            mode = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;
            break;
        default:
            /** TODO */
            //mode = FA__ERROR;
            break;
    }

    /* Acquire the mutex. */
    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult != OSAL_RESULT_TRUE)
    {
        errorValue = SYS_FS_ERROR_DENIED;
        /* Mark the file object as free. */
        fileObj->inUse = false;
        return SYS_FS_HANDLE_INVALID;
    }

    errorValue = SYS_FS_ERROR_OK;
    if (disk->fsFunctions->open != NULL)
    {
        fileStatus = disk->fsFunctions->open((uintptr_t)&fileObj->nativeFSFileObj, (const char *)pathWithDiskNo, mode);
        if ((fileStatus == 0) && ((SYS_FS_FILE_OPEN_APPEND == attributes) || (SYS_FS_FILE_OPEN_APPEND_PLUS == attributes)))
        {
            //fileStatus = SYS_FS_FileSeek((SYS_FS_HANDLE)fileObj, 0, SYS_FS_SEEK_END);
            uint32_t size = 0;
            size = fileObj->mountPoint->fsFunctions->size(fileObj->nativeFSFileObj);
            fileStatus = fileObj->mountPoint->fsFunctions->seek(fileObj->nativeFSFileObj, size);

            if (fileStatus != 0)
            {
                /* Close the file if the seek fails. */
                fileObj->mountPoint->fsFunctions->close(fileObj->nativeFSFileObj);
            }
        }
        errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
    }

    /* Release the mutex. */
    OSAL_MUTEX_Unlock (&(disk->mutexDiskVolume));

    if (fileStatus != 0)
    {
        /* Mark the file object as free. */
        fileObj->inUse = false;
    }

    return (fileStatus == 0) ? ((SYS_FS_HANDLE)fileObj) : SYS_FS_HANDLE_INVALID;
}

//******************************************************************************
/* Function:
    SYS_FS_RESULT SYS_FS_FileClose
    (
        SYS_FS_HANDLE handle
    );

  Summary:
    Closes a file.

  Description:
    This function closes an opened file.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_FileClose
(
    SYS_FS_HANDLE handle
)
{
    int fileStatus = -1;
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Check if the handle is valid. */
    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->inUse == false)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->mountPoint->fsFunctions->close == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    /* Clear the error. */
    fileObj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult != OSAL_RESULT_TRUE)
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
        return SYS_FS_RES_FAILURE;
    }

    fileStatus = fileObj->mountPoint->fsFunctions->close(fileObj->nativeFSFileObj);

    /* Release the acquired mutex. */
    OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

    if (fileStatus == 0)
    {
        /* Mark the file object as free. */
        fileObj->inUse = false;
        return SYS_FS_RES_SUCCESS;
    }
    else
    {
        fileObj->errorValue = (SYS_FS_ERROR)fileStatus;
        return SYS_FS_RES_FAILURE;
    }
}

//******************************************************************************
/*Function:
    bool SYS_FS_FileNameGet
    (
        SYS_FS_HANDLE handle, 
        uint8_t* cName, 
        uint16_t wLen
    );

  Summary:
    Reads the file name.

  Description:
    This function reads the file name of a file that is already open.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
bool SYS_FS_FileNameGet
(
    SYS_FS_HANDLE handle,
    uint8_t* cName,
    uint16_t wLen
)
{
    SYS_FS_OBJ *obj = (SYS_FS_OBJ *)handle;
    uint32_t index;

    if(handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return false;
    }

    if(obj->inUse == false)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return false;
    }

    if(wLen > FAT_FS_MAX_LFN)
    {
        wLen = FAT_FS_MAX_LFN;
    }

    for(index = 0;  index < wLen; index ++)
    {
        cName[index] = obj->fileName[index];
        /* break, in case end of string reached */
        if(obj->fileName[index] == '\0')
            break;
    }

    return true;

}

//******************************************************************************
/* Function:
    size_t SYS_FS_FileRead
    (
        SYS_FS_HANDLE handle, 
        void *buf, 
        size_t nbyte
    );

  Summary:
    Read data from the file.

  Description:
    This function attempts to read nbyte bytes of data from the file associated
    with the file handle into the buffer pointed to by buf.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/

size_t SYS_FS_FileRead
(
    SYS_FS_HANDLE handle,
    void *buffer,
    size_t nbyte
)
{
    int fileStatus = -1;
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    uint32_t bytesRead = -1;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Check if the handle is valid. */
    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return bytesRead;
    }

    /* Check if the file object is in use. */
    if (fileObj->inUse == false)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return bytesRead;
    }

    if (fileObj->mountPoint->fsFunctions->read == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return bytesRead;
    }

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult != OSAL_RESULT_TRUE)
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }
    else
    {
        fileStatus = fileObj->mountPoint->fsFunctions->read(
                fileObj->nativeFSFileObj,
                buffer,
                nbyte, 
                &bytesRead);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        if (fileStatus != 0)
        {
            /* There was an error with the read operation. Update the error value.
             * */
            fileObj->errorValue = (SYS_FS_ERROR)fileStatus;
            bytesRead = -1;
        }
    }

    return bytesRead;
}

//******************************************************************************
/* Function:
    size_t SYS_FS_FileWrite
    (
        SYS_FS_HANDLE handle,
        const void *buf,
        size_t nbyte
    );

  Summary:
    Writes data to the file.

  Description:
    This function attempts to write nbyte bytes from the buffer pointed to by
    buf to the file associated with the file handle.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/

size_t SYS_FS_FileWrite
(
    SYS_FS_HANDLE handle,
    const void *buffer,
    size_t nbyte
)
{
    int fileStatus = -1;
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    uint32_t bytesWritten = -1;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Check if the handle is valid. */
    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return bytesWritten;
    }

    /* Check if the fil object is in use. */
    if (fileObj->inUse == false)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return bytesWritten;
    }

    if (fileObj->mountPoint->fsFunctions->write == NULL)
    {
        /* The write operation is not supported by the Native FS. */
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return bytesWritten;
    }

    /* Clear out the error. */
    fileObj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult != OSAL_RESULT_TRUE)
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }
    else
    {
        fileStatus = fileObj->mountPoint->fsFunctions->write(
                fileObj->nativeFSFileObj,
                buffer, 
                nbyte, 
                &bytesWritten);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        if (fileStatus != 0)
        {
            /* There was an error while writing to the file. Update the error
             * value. */
            fileObj->errorValue = (SYS_FS_ERROR)fileStatus;
            bytesWritten = -1;
        }
    }

    return bytesWritten;
}

//******************************************************************************
/* Function:
    int SYS_FS_FileSeek
    (
        SYS_FS_HANDLE handle,
        int offset,
        SYS_FS_FILE_SEEK_CONTROL whence
    );

  Summary:
    Moves the file pointer by the requested offset.

  Description:
    This function sets the file pointer for a open file associated with the
    file handle, as follows:
      whence = SYS_FS_SEEK_SET - File offset is set to offset bytes from the
                                 beginning.
      whence = SYS_FS_SEEK_CUR - File offset is set to its current location
                                 plus offset.
      whence = SYS_FS_SEEK_END - File offset is set to the size of the file
                                 plus offset. The offset specified for this
                                 option should be negative for the file pointer
                                 to be valid.

    Trying to move the file pointer using SYS_FS_FileSeek, beyond the range of
    file will only cause the pointer to be moved to the last location of the
    file.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/

int32_t SYS_FS_FileSeek
(
    SYS_FS_HANDLE handle,
    int32_t offset,
    SYS_FS_FILE_SEEK_CONTROL whence
)
{
    int fileStatus = -1;
    SYS_FS_OBJ *obj = (SYS_FS_OBJ *)handle;
    long tell = 0;
    uint32_t size = 0;
    int temp = 0;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Check if the handle is valid. */
    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return -1;
    }

    /* Check if the file object is in use. */
    if (obj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return -1;
    }

    if ((obj->mountPoint->fsFunctions->seek == NULL) || 
        (obj->mountPoint->fsFunctions->tell == NULL) ||
        (obj->mountPoint->fsFunctions->size == NULL))
    {
        /* The function is not supported in the native file system. */
        obj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return -1;
    }

    /* Clear out the error. */
    obj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(obj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        /* SYS_FS_SEEK_SET case. */
        temp = offset;

        if (whence == SYS_FS_SEEK_CUR)
        {
            tell = obj->mountPoint->fsFunctions->tell(obj->nativeFSFileObj);
            temp = (offset + tell);
        }
        else if (whence == SYS_FS_SEEK_END)
        {
            size = obj->mountPoint->fsFunctions->size(obj->nativeFSFileObj);
            temp = (offset + size);
        }

        fileStatus = obj->mountPoint->fsFunctions->seek(obj->nativeFSFileObj, temp);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(obj->mountPoint->mutexDiskVolume));

        if (fileStatus != 0)
        {
            obj->errorValue = (SYS_FS_ERROR)fileStatus;
            offset = -1;
        }
    }
    else
    {
        obj->errorValue = SYS_FS_ERROR_DENIED;
        offset = -1;
    }

    return offset;
}

//******************************************************************************
/*Function:
    int32_t SYS_FS_FileTell
    (
        SYS_FS_HANDLE handle
    );

  Summary:
    Obtains the file pointer position.

  Description:
    Obtains the current value of the file position indicator for the file
    pointed to by handle.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/

int32_t SYS_FS_FileTell
(
    SYS_FS_HANDLE handle
)
{
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    int fileStatus = -1;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Check if the handle is valid. */
    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return -1;
    }

    /* Check if the file object is in use. */
    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return -1;
    }

    if (fileObj->mountPoint->fsFunctions->tell == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return -1;
    }

    /* Clear out the error. */
    fileObj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = fileObj->mountPoint->fsFunctions->tell(fileObj->nativeFSFileObj);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        if (fileStatus < 0)
        {
            fileObj->errorValue = SYS_FS_ERROR_DISK_ERR;
        }
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return fileStatus;
}

//******************************************************************************
/*Function:
    int32_t SYS_FS_FileSize
    (
        SYS_FS_HANDLE handle
    );

  Summary:
    Returns the size of the file in bytes.

  Description:
    This function returns the size of the file as pointed by the handle.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
int32_t SYS_FS_FileSize
(
    SYS_FS_HANDLE handle
)
{
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    int fileStatus = -1;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Check if the handle is valid. */
    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return -1;
    }

    /* Check if the file object is in use. */
    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return -1;
    }

    if (fileObj->mountPoint->fsFunctions->size == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return -1;
    }

    /* Clear out the error. */
    fileObj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = fileObj->mountPoint->fsFunctions->size(fileObj->nativeFSFileObj);
        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        if (fileStatus < 0)
        {
            fileObj->errorValue = SYS_FS_ERROR_DISK_ERR;
        }
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return fileStatus;
}

//******************************************************************************
/*Function:
    bool SYS_FS_FileEOF
    (
        SYS_FS_HANDLE handle
    );

  Summary:
    Checks for end of file.

  Description:
    Checks whether or not the file position indicator is at the end of the
    file.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
bool SYS_FS_FileEOF
(
    SYS_FS_HANDLE handle
)
{
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    bool status = false;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Check if the handle is valid. */
    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return -1;
    }

    /* Check if the file fileObject is in use. */
    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return false;
    }

    if (fileObj->mountPoint->fsFunctions->eof == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return false;
    }

    /* Clear out the error. */
    fileObj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        status = fileObj->mountPoint->fsFunctions->eof(fileObj->nativeFSFileObj);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return status;
}

//******************************************************************************
/* Function:
    SYS_FS_RESULT SYS_FS_FileStat
    (
        const char *fname,
        SYS_FS_FSTAT *buf
    );

  Summary:
    Gets file status.

  Description:
    This function obtains information about a file associated with the file
    name, and populates the information in the structure pointed to by buf.
    This function can read the status of file regardless of whether a file is
    opened or not.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_FileStat
(
    const char *fname,
    SYS_FS_FSTAT *buf
)
{
    int fileStatus = -1;
    uint8_t pathWithDiskNo[SYS_FS_PATH_LEN_WITH_DISK_NUM] = { 0 };
    SYS_FS_MOUNT_POINT *disk = (SYS_FS_MOUNT_POINT *) NULL;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if ((fname == NULL) || (buf == NULL))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(fname, &disk, pathWithDiskNo) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    if (disk->fsFunctions->fstat == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    /* Clear out the error. */
    errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->fstat((const char *)pathWithDiskNo, (uintptr_t)buf);
        errorValue = (SYS_FS_ERROR)fileStatus;

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));
    }
    else
    {
        errorValue = SYS_FS_ERROR_DENIED;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_ERROR SYS_FS_Error
    (
        void
    );

  Summary:
    Returns the last error.

  Description:
    When a file system operation fails, the application can know the reason of
    failure by calling the SYS_FS_Error. This function only reports the errors
    which are not file (or file handle) specific. For example, for functions
    such as SYS_FS_Mount and SYS_FS_FileOpen, which do not take handle, any
    errors happening inside such function calls could be reported using
    SYS_FS_Error function. Even for functions, which take handle as its input
    parameters, the SYS_FS_Error function can be used to report the type of
    error for cases where the passed handle itself is invalid.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_ERROR SYS_FS_Error
(
    void
)
{
    return errorValue;
}

//******************************************************************************
/*Function:
    SYS_FS_ERROR SYS_FS_FileError
    (
        SYS_FS_HANDLE handle
    );

  Summary:
    Returns the file specific error.

  Description:
    For file system functions which accepts valid handle, any error happening
    in those functions could be retrieved with SYS_FS_FileError. This function
    returns errors which are file specific.

    Please note that if an invalid handle is passed to a file system function,
    in such a case, SYS_FS_FileError will not return the correct type of error,
    as the handle was invalid. Therefore, it would be prudent to check the
    errors using the SYS_FS_Error function.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_ERROR SYS_FS_FileError
(
    SYS_FS_HANDLE handle
)
{
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;

    if (handle == SYS_FS_HANDLE_INVALID)
    {
        return SYS_FS_ERROR_INVALID_OBJECT;
    }

    if (fileObj->inUse == false)
    {
        return SYS_FS_ERROR_INVALID_OBJECT;
    }

    return fileObj->errorValue;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_FileSync
    (
        SYS_FS_HANDLE handle
    );

  Summary:
    Flushes the cached information when writing to a file.

  Description:
    This function flushes the cached information when writing to a file. The
    SYS_FS_FileSync function performs the same process as SYS_FS_FileClose
    function; however, the file is left open and can continue read/write/seek
    operations to the file.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/

SYS_FS_RESULT SYS_FS_FileSync
(
    SYS_FS_HANDLE handle
)
{
    int fileStatus = -1;
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Check if the handle is valid. */
    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    /* Check if the file object is in use. */
    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->mountPoint->fsFunctions->sync == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    /* Clear the error */
    fileObj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = fileObj->mountPoint->fsFunctions->sync(fileObj->nativeFSFileObj);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        fileObj->errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_FileTruncate
    (
        SYS_FS_HANDLE handle
    );

  Summary:
    Truncates a file

  Description:
    This function truncates the file size to the current file read/write
    pointer. This function has no effect if the file read/write pointer is
    already pointing to end of the file.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_FileTruncate
(
    SYS_FS_HANDLE handle
)
{
    int fileStatus = -1;
    SYS_FS_OBJ *obj = (SYS_FS_OBJ *)handle;

    if(handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if(obj->inUse == false)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    /* Now, call the real file open function */
    if(obj->mountPoint->fsFunctions->truncate == NULL)
    {
        obj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }
    if(OSAL_MUTEX_Lock(&(obj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER)
                                                        == OSAL_RESULT_TRUE)
    {
        fileStatus = obj->mountPoint->fsFunctions->truncate(obj->nativeFSFileObj);

        OSAL_MUTEX_Unlock(&(obj->mountPoint->mutexDiskVolume));
    }

    if(fileStatus == 0)
    {
        return SYS_FS_RES_SUCCESS;
    }
    else
    {
        obj->errorValue = (SYS_FS_ERROR)fileStatus;
        return SYS_FS_RES_FAILURE;
    }
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_FileStringGet
    (
        SYS_FS_HANDLE handle, 
        char* buff,
        uint32_t len
    );

  Summary:
    Reads a string from the file into a buffer.

  Description:
    This function reads a string of specified length from the file into a
    buffer. The read operation continues until:
      1. '\n' is stored 
      2. reached end of the file or 
      3. the buffer is filled with len - 1 characters.
      The read string is terminated with a '\0'.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_FileStringGet
(
    SYS_FS_HANDLE handle, 
    char* buff, 
    uint32_t len
)
{
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    char *ptr = NULL;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Check if the parameters are valid. */
    if ((handle == SYS_FS_HANDLE_INVALID) || (buff == NULL) || (len == 0))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }
    
    /* Check if the file object is in use. */
    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->mountPoint->fsFunctions->getstrn == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    /* Clear the error. */
    fileObj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        ptr = fileObj->mountPoint->fsFunctions->getstrn(buff, len, fileObj->nativeFSFileObj);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        if (ptr != NULL) //(buff == ptr)
        {
            return SYS_FS_RES_SUCCESS;
        }
        else
        {
            fileObj->errorValue = SYS_FS_ERROR_DISK_ERR;
        }
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_FileCharacterPut
    (
        SYS_FS_HANDLE handle,
        char data
    );

  Summary:
    Writes a character to a file.

  Description:
    This function writes a character to a file.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_FileCharacterPut
(
    SYS_FS_HANDLE handle, 
    char data
)
{
    int fileStatus = -1;
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    int res = 0;

    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Check if the handle is valid. */
    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->mountPoint->fsFunctions->putchr == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    /* Clear the error. */
    fileObj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        res = fileObj->mountPoint->fsFunctions->putchr(data, fileObj->nativeFSFileObj);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        if (res == 1)
        {
            return SYS_FS_RES_SUCCESS;
        }
        else
        {
            fileObj->errorValue = (SYS_FS_ERROR)fileStatus;
        }
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_FileStringPut
    (
        SYS_FS_HANDLE handle,
        const char *string
    );

  Summary:
    Writes a string to a file.

  Description:
    This function writes a string into a file. The string to be written should
    be NULL terminated. The terminator character will not be written.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_FileStringPut
(
    SYS_FS_HANDLE handle, 
    const char *string
)
{
    int fileStatus = -1;
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    int res = 0;

    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Validate the input parameters. */
    if ((handle == SYS_FS_HANDLE_INVALID) || (string == NULL))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->mountPoint->fsFunctions->putstrn == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    /* Clear the error. */
    fileObj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        res = fileObj->mountPoint->fsFunctions->putstrn(string, fileObj->nativeFSFileObj);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        if (res == EOF)
        {
            fileObj->errorValue = (SYS_FS_ERROR)fileStatus;
        }
        else
        {
            return SYS_FS_RES_SUCCESS;
        }
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_FilePrintf
    (
        SYS_FS_HANDLE handle,
        const char *string,
        ...
    );

  Summary:
    Writes a formatted string into a file.

  Description:
    This function writes a formatted string into a file.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_FilePrintf
(
    SYS_FS_HANDLE handle, 
    const char *string, 
    ...
)
{
    int fileStatus = SYS_FS_ERROR_NOT_READY;
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    int res = 0;
    va_list ap = (va_list){0};
    const char *ptr;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    /* Validate the parameters. */
    if ((handle == SYS_FS_HANDLE_INVALID) || (string == NULL))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->mountPoint->fsFunctions->formattedprint == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    /* Clear the error. */
    fileObj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        va_start (ap, string);
        ptr = va_arg(ap, const char*);
        res = fileObj->mountPoint->fsFunctions->formattedprint(fileObj->nativeFSFileObj, string, ptr);
        va_end (ap);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        if (res == EOF)
        {
            fileObj->errorValue = (SYS_FS_ERROR)fileStatus;
        }
        else
        {
            return SYS_FS_RES_SUCCESS;
        }
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    bool SYS_FS_FileTestError
    (
        SYS_FS_HANDLE handle
    );

  Summary:
    Checks for errors in the file.

  Description:
    This function checks whether or not file has any errors.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
bool SYS_FS_FileTestError
(
    SYS_FS_HANDLE handle
)
{
    SYS_FS_OBJ *fileObj = (SYS_FS_OBJ *)handle;
    bool status = false;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return true;
    }

    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return true;
    }

    if (fileObj->mountPoint->fsFunctions->testerror == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return true;
    }

    /* Clear the error. */
    fileObj->errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        status = fileObj->mountPoint->fsFunctions->testerror(fileObj->nativeFSFileObj);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return status;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_DirectoryMake
    (
        const char* path
    );

  Summary:
    Makes a directory.

  Description:
    This function makes a new directory as per the specified path.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_DirectoryMake
(
    const char* path
)
{
    int fileStatus = -1;
    uint8_t pathWithDiskNo[SYS_FS_PATH_LEN_WITH_DISK_NUM] = { 0 };
    SYS_FS_MOUNT_POINT *disk = NULL;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (path == NULL)
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(path, &disk, pathWithDiskNo) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    if (disk->fsFunctions->mkdir == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->mkdir((const char *)pathWithDiskNo);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));

        errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        errorValue = SYS_FS_ERROR_DENIED;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_DirectoryChange
    (
        const char* path
    );

  Summary:
    Changes to a the directory specified.

  Description:
    This function changes the present directory to a new directory.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_DirectoryChange
(
    const char* path
)
{
    int fileStatus = -1;
    uint8_t pathWithDiskNo[SYS_FS_PATH_LEN_WITH_DISK_NUM] = { 0 };
    SYS_FS_MOUNT_POINT *disk = (SYS_FS_MOUNT_POINT *) NULL;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (path == NULL)
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(path, &disk, pathWithDiskNo) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    if (disk->fsFunctions->chdir == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->chdir((const char *)pathWithDiskNo);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));
        errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        errorValue = SYS_FS_ERROR_DENIED;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_FileDirectoryRemove
    (
        const char* path
    );

  Summary:
    Removes a file or directory.

  Description:
    This function removes a file or directory as specified by the path.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_FileDirectoryRemove
(
    const char* path
)
{
    int fileStatus = -1;
    uint8_t pathWithDiskNo[SYS_FS_PATH_LEN_WITH_DISK_NUM] = { 0 };
    SYS_FS_MOUNT_POINT *disk = (SYS_FS_MOUNT_POINT *) NULL;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (path == NULL)
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(path, &disk, pathWithDiskNo) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    if (disk->fsFunctions->remove == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    errorValue = SYS_FS_ERROR_OK;

    /* Acquire the volume mutex. */
    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->remove((const char *)pathWithDiskNo);

        /* Release the acquired mutex. */
        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));
        errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        errorValue = SYS_FS_ERROR_DENIED;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/* Function:
    SYS_FS_HANDLE SYS_FS_DirOpen
    (
        const char* path
    );

  Summary:
    Open a directory

  Description:
    This function opens the requested directory.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_HANDLE SYS_FS_DirOpen
(
    const char* path
)
{
    int fileStatus = SYS_FS_ERROR_NOT_READY;
    uint8_t pathWithDiskNo[SYS_FS_PATH_LEN_WITH_DISK_NUM] = { 0 };
    SYS_FS_MOUNT_POINT *disk = (SYS_FS_MOUNT_POINT *) NULL;
    uint32_t index = 0;
    SYS_FS_DIR_OBJ *fileObj = NULL;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (path == NULL)
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_HANDLE_INVALID;
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(path, &disk, pathWithDiskNo) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_HANDLE_INVALID;
    }

    if (disk->fsFunctions->openDir == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_HANDLE_INVALID;
    }

    osalResult = OSAL_MUTEX_Lock((OSAL_MUTEX_HANDLE_TYPE *)&gSysFsMutex, OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        /* The data should be protected from simulateous access */
        for (index = 0; index < SYS_FS_MAX_FILES; index ++)
        {
            if (gSYSFSDirObj[index].inUse == false)
            {
                gSYSFSDirObj[index].inUse = true;
                gSYSFSDirObj[index].mountPoint = disk;
                fileObj = &gSYSFSDirObj[index];
                break;
            }
        }
        OSAL_MUTEX_Unlock(&gSysFsMutex);
    }

    if (fileObj == NULL)
    {
        errorValue = SYS_FS_ERROR_TOO_MANY_OPEN_FILES;
        return SYS_FS_HANDLE_INVALID;
    }

    errorValue = SYS_FS_ERROR_OK;
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->openDir((uintptr_t)&fileObj->nativeFSDirObj,
                                                        (const char *)pathWithDiskNo);

        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        if (fileStatus == 0)
        {
            return ((SYS_FS_HANDLE)fileObj);
        }
        else
        {
            errorValue = (SYS_FS_ERROR)fileStatus;
            fileObj->inUse = false;
        }
    }

    return SYS_FS_HANDLE_INVALID;
}

//******************************************************************************
/* Function:
    SYS_FS_RESULT SYS_FS_DirClose
    (
        SYS_FS_HANDLE handle
    );

  Summary:
    Closes an opened directory.

  Description:
    This function closes a directory that was opened earlier opened with the
    SYS_FS_DirOpen function.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_DirClose
(
    SYS_FS_HANDLE handle
)
{
    int fileStatus = -1;
    SYS_FS_DIR_OBJ *fileObj = (SYS_FS_DIR_OBJ *)handle;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->inUse == false)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        SYS_ASSERT(false,"File fileObject is not in use");
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->mountPoint->fsFunctions->closeDir == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    fileObj->errorValue = SYS_FS_ERROR_OK;

    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = fileObj->mountPoint->fsFunctions->closeDir(fileObj->nativeFSDirObj);
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        if (fileStatus == 0)
        {
            /* Return the SYS_FS file system object. */
            fileObj->inUse = false;
            return SYS_FS_RES_SUCCESS;
        }
        else
        {
            fileObj->errorValue = (SYS_FS_ERROR)fileStatus;
        }
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return SYS_FS_RES_FAILURE;
}

//******************************************************************************
/* Function:
    SYS_FS_RESULT SYS_FS_DirRead
    (
        SYS_FS_HANDLE handle,
        SYS_FS_FSTAT stat
    );

  Summary:
    Reads the files and directories of the specified directory.

  Description:
    This function reads the files and directories specified in the open
    directory.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_DirRead
(
    SYS_FS_HANDLE handle, 
    SYS_FS_FSTAT *stat
)
{
    int fileStatus = -1;
    SYS_FS_DIR_OBJ *fileObj = (SYS_FS_DIR_OBJ *)handle;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if ((handle == SYS_FS_HANDLE_INVALID) || (stat == NULL))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->mountPoint->fsFunctions->readDir == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    fileObj->errorValue = SYS_FS_ERROR_OK;
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if(osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = fileObj->mountPoint->fsFunctions->readDir(fileObj->nativeFSDirObj, (uintptr_t)stat);

        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        fileObj->errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/* Function:
    SYS_FS_RESULT SYS_FS_DirRewind
    (
        SYS_FS_HANDLE handle
    );

  Summary:
    Rewinds to the beginning of the directory.

  Description:
    This function rewinds the directory to the start. Once a search of
    directory or directory read is completed, the rewind function is used to
    begin searching the directory from the start.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_DirRewind
(
    SYS_FS_HANDLE handle
)
{
    int fileStatus = -1;
    SYS_FS_DIR_OBJ *fileObj = (SYS_FS_DIR_OBJ *)handle;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (handle == SYS_FS_HANDLE_INVALID)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->mountPoint->fsFunctions->readDir == NULL)
    {
        fileObj->errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    fileObj->errorValue = SYS_FS_ERROR_OK;
    osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if(osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = fileObj->mountPoint->fsFunctions->readDir(fileObj->nativeFSDirObj, (uintptr_t)NULL);
        OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

        fileObj->errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        fileObj->errorValue = SYS_FS_ERROR_DENIED;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/* Function:
    SYS_FS_RESULT SYS_FS_DirSearch
    (
        SYS_FS_HANDLE handle,
        const char * name,
        SYS_FS_FILE_DIR_ATTR attr,
        SYS_FS_FSTAT *stat
    );

  Summary:
    Searches for a file or directory.

  Description:
    This function searches for the requested file or directory. The file or
    directory is specified in the attr parameter, which is of type
    SYS_FS_FILE_DIR_ATTR.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_DirSearch
(
    SYS_FS_HANDLE handle, 
    const char * name, 
    SYS_FS_FILE_DIR_ATTR attr, 
    SYS_FS_FSTAT *stat
)
{
    bool workToDo = true;           // avoid unreachable code warning
    int fileStatus = -1;
    SYS_FS_DIR_OBJ *fileObj = (SYS_FS_DIR_OBJ *)handle;
    char *fileName = NULL;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if ((handle == SYS_FS_HANDLE_INVALID) || (name == NULL) || (stat == NULL))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->inUse == 0)
    {
        errorValue = SYS_FS_ERROR_INVALID_OBJECT;
        return SYS_FS_RES_FAILURE;
    }

    if (fileObj->mountPoint->fsFunctions->readDir == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    while( workToDo )
    {
        errorValue = SYS_FS_ERROR_OK;
        osalResult = OSAL_MUTEX_Lock(&(fileObj->mountPoint->mutexDiskVolume), OSAL_WAIT_FOREVER);
        if (osalResult == OSAL_RESULT_TRUE)
        {
            fileStatus = fileObj->mountPoint->fsFunctions->readDir(fileObj->nativeFSDirObj, (uintptr_t)stat);
            OSAL_MUTEX_Unlock(&(fileObj->mountPoint->mutexDiskVolume));

            if(fileStatus != 0)
            {
                errorValue = (SYS_FS_ERROR)fileStatus;
                return SYS_FS_RES_FAILURE;
            }
        }
        else
        {
            errorValue = SYS_FS_ERROR_DENIED;
            return SYS_FS_RES_FAILURE;
        }

        /* If we have come to end of directory */
        if (stat->lfname != NULL)
        {
            /* If LFN is Enabled but filename fits into Short file name */
            if ((stat->lfname[0] == '\0') && (stat->fname[0] == '\0'))
            {
                errorValue = SYS_FS_ERROR_NO_FILE;
                return SYS_FS_RES_FAILURE;
            }
        }
        else if (stat->fname[0] == '\0')
        {
            errorValue = SYS_FS_ERROR_NO_FILE;
            return SYS_FS_RES_FAILURE;
        }

        /* Firstly, match the file attribute with the requested attribute */
        if (stat->fattrib & attr)
        {
            if((stat->lfname != NULL) && (stat->lfname[0] != '\0'))
            {
                /* File name is LFN */
                fileName = stat->lfname;
            }
            else
            {
                /* File name fits in 8.3 format */
                fileName = stat->fname;
            }

            if(SYS_FS_StringWildCardCompare(name, fileName) == true)
            {
                return SYS_FS_RES_SUCCESS;
            }
        }
    }

    errorValue = SYS_FS_ERROR_NO_FILE;
    return SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_CurrentWorkingDirectoryGet
    (
        char *buff,
        uint32_t len
    );

  Summary:
    Gets the current working directory

  Description:
    This function gets the current working directory path along with the
    working drive.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_CurrentWorkingDirectoryGet
(
    char *buffer,
    uint32_t len
)
{
    SYS_FS_MOUNT_POINT *disk = NULL;
    int fileStatus = -1;
    char cwd[SYS_FS_CWD_STRING_LEN] = { 0 };
    char *ptr = NULL;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if ((buffer == NULL) || (len == 0))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    if (gSYSFSCurrentMountPoint.inUse == false)
    {
        errorValue = SYS_FS_ERROR_NO_FILESYSTEM;
        return SYS_FS_RES_FAILURE;
    }

    /* This is the current drive in sys_fs. For this current drive, get the
     * current working directory */
    disk = gSYSFSCurrentMountPoint.currentDisk;
    if (disk->fsFunctions->currWD == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult != OSAL_RESULT_TRUE)
    {
        errorValue = SYS_FS_ERROR_DENIED;
        return SYS_FS_RES_FAILURE;
    }

    fileStatus = disk->fsFunctions->currWD(buffer, len);
    OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));

    errorValue = (SYS_FS_ERROR)fileStatus;

    if (fileStatus == 0)
    {
        ptr = buffer;

        /* Check if the first character of the returned buffer is a ASCII for digit */
        if ((*ptr >= '0') && (*ptr <= '9'))
        {
            /* Check if the current drive in native file system matches with
             * the current drive in sys_fs layer */
            if (disk->diskNumber == (*ptr - '0'))
            {
                /* Ignore the first two characters as they contain the driver number and the ':" */
                ptr += 2;
            }
            else
            {
                errorValue = SYS_FS_ERROR_INVALID_NAME;
                return SYS_FS_RES_FAILURE;
            }
        }

        if (len < (5 + disk->mountNameLength + strlen(ptr)))
        {
            errorValue = SYS_FS_ERROR_NOT_ENOUGH_CORE;
            return SYS_FS_RES_FAILURE;
        }

        /* Copy the cwd name in a temporary buffer. */
        strcpy (cwd, ptr);

        ptr = buffer;
        memset (ptr, 0, len);

        strncpy (ptr, "/mnt/", 5);
        ptr += 5;

        strncpy (ptr, disk->mountName, disk->mountNameLength);
        ptr += disk->mountNameLength;

        strncpy (ptr, cwd, strlen(cwd));

        return SYS_FS_RES_SUCCESS;
    }
    else
    {
        return SYS_FS_RES_FAILURE;
    }
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_FileDirectoryModeSet
    (
        const char* path,
        SYS_FS_FILE_DIR_ATTR attr,
        SYS_FS_FILE_DIR_ATTR mask
    );

  Summary:
    Sets the mode for the file or directory.

  Description:
    This function sets the mode for a file or directory from the specified list
    of attributes.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_FileDirectoryModeSet
(
    const char* fname,
    SYS_FS_FILE_DIR_ATTR attr,
    SYS_FS_FILE_DIR_ATTR mask
)
{
    int fileStatus = -1;
    uint8_t pathWithDiskNo[SYS_FS_PATH_LEN_WITH_DISK_NUM] = { 0 };
    SYS_FS_MOUNT_POINT *disk = (SYS_FS_MOUNT_POINT *) NULL;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (fname == NULL)
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER; 
        return SYS_FS_RES_FAILURE;
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(fname, &disk, pathWithDiskNo) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    if (disk->fsFunctions->chmode == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return (SYS_FS_RES_FAILURE);
    }

    errorValue = SYS_FS_ERROR_OK;
    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->chmode((const char *)pathWithDiskNo, attr, mask);
        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));
        errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        errorValue = SYS_FS_ERROR_DENIED;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_FileDirectoryTimeSet
    (
        const char* path,
        SYS_FS_TIME *time
    );

  Summary:
    Sets or changes the time for a file or directory.

  Description:
    This function sets or change the time for a file or directory.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_FileDirectoryTimeSet
(
    const char* fname, 
    SYS_FS_TIME *time
)
{
    int fileStatus = -1;
    uint8_t pathWithDiskNo[SYS_FS_PATH_LEN_WITH_DISK_NUM] = { 0 };
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;
    SYS_FS_MOUNT_POINT *disk = (SYS_FS_MOUNT_POINT *) NULL;
    SYS_FS_FSTAT stat;

    if ((fname == NULL) || (time == NULL))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return (SYS_FS_RES_FAILURE);
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(fname, &disk, pathWithDiskNo) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    if (disk->fsFunctions->chtime == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return (SYS_FS_RES_FAILURE);
    }

    /* Now, transfer the time from elements of "SYS_FS_TIME" to elements of "SYS_FS_FSTAT" */
    stat.fdate = time->timeDate.date;
    stat.ftime = time->timeDate.time;

    errorValue = SYS_FS_ERROR_OK;
    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->chtime((const char *)pathWithDiskNo, (uintptr_t)&stat);
        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));
        errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        errorValue = SYS_FS_ERROR_DENIED;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_FileDirectoryRenameMove
    (
        const char *oldPath,
        const char *newPath
    );

  Summary:
    Renames or moves a file or directory.

  Description:
    This function renames or moves a file or directory.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_FileDirectoryRenameMove
(
    const char *oldPath, 
    const char *newPath
)
{
    int fileStatus = SYS_FS_ERROR_NOT_READY;
    uint8_t oldPathWithDiskNo[SYS_FS_PATH_LEN_WITH_DISK_NUM] = { 0 };
    uint8_t newPathWithDiskNo[SYS_FS_PATH_LEN_WITH_DISK_NUM] = { 0 };
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;
    SYS_FS_MOUNT_POINT *disk = (SYS_FS_MOUNT_POINT *) NULL;

    if ((oldPath == NULL) || (newPath == NULL))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return (SYS_FS_RES_FAILURE);
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(oldPath, &disk, oldPathWithDiskNo) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    if (SYS_FS_GetDisk(newPath, &disk, newPathWithDiskNo) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    if (disk->fsFunctions->rename == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return (SYS_FS_RES_FAILURE);
    }

    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        /* This function does not take disk number (like "0:"). Hence use from index "2" onwards */
        fileStatus = disk->fsFunctions->rename((const char *)&oldPathWithDiskNo[2], (const char *)&newPathWithDiskNo[2]);
        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));
        errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        errorValue = SYS_FS_ERROR_DENIED;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_CurrentDriveGet
    (
        char* buffer
    );

  Summary:
    Gets the current drive

  Description:
    This function gets the present drive being used. The drive information is
    populated in the buffer.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_CurrentDriveGet
(
    char* buffer
)
{
    if (buffer == NULL)
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    if(gSYSFSCurrentMountPoint.inUse == false)
    {
        errorValue = SYS_FS_ERROR_NO_FILESYSTEM;
        return SYS_FS_RES_FAILURE;
    }

    strcpy(buffer, "/mnt/");
    strncpy(buffer + 5, gSYSFSCurrentMountPoint.currentDisk->mountName, gSYSFSCurrentMountPoint.currentDisk->mountNameLength);

    return SYS_FS_RES_SUCCESS;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_CurrentDriveSet
    (
        const char* path
    );

  Summary:
    Sets the drive.

  Description:
    This function sets the present drive to the one as specified by the path.
    By default, the drive mounted last becomes the current drive for the
    system. This is useful for applications where only one drive (volume) is
    used. In such an application, there is no need to call the
    SYS_FS_CurrentDriveSet function. However, in the case of an application
    where there are multiple volumes, the user can select the current drive for
    the application by calling this function.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_CurrentDriveSet
(
    const char* path
)
{
    SYS_FS_MOUNT_POINT *disk = (SYS_FS_MOUNT_POINT *) NULL;
    int fileStatus = -1;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (path == NULL)
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(path, &disk, NULL) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    if(gSYSFSCurrentMountPoint.inUse == false)
    {
        SYS_ASSERT(false, "Invalid mount point. Was the disk mounted?");
        errorValue = SYS_FS_ERROR_NO_FILESYSTEM;
        return SYS_FS_RES_FAILURE;
    }

    if (disk->fsType == MPFS2)
    {
        gSYSFSCurrentMountPoint.currentDisk = disk;
        return SYS_FS_RES_SUCCESS;
    }

    if (disk->fsFunctions->chdrive == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->chdrive(disk->diskNumber);

        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));
        errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        errorValue = SYS_FS_ERROR_DENIED;
    }

    if (fileStatus == 0)
    {
        gSYSFSCurrentMountPoint.currentDisk = disk;
        return SYS_FS_RES_SUCCESS;
    }
    else
    {
        return SYS_FS_RES_FAILURE;
    }
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_DriveLabelGet
    (
        const char* drive,
        char *buff,
        uint32_t *sn
    );

  Summary:
    Gets the drive label.

  Description:
    This function gets the label for the drive specified. If no drive is
    specified, the label for the current drive is obtained.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_DriveLabelGet
(
    const char* drive, 
    char *buff, 
    uint32_t *sn
)
{
    int fileStatus = -1;
    SYS_FS_MOUNT_POINT *disk = NULL;
    uint8_t pathWithDiskNo[3] = { 0 };
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (drive != NULL)
    {
        /* Get disk number */
        if (SYS_FS_GetDisk(drive, &disk, NULL) == false)
        {
            /* "errorValue" contains the reason for failure. */
            return SYS_FS_RES_FAILURE;
        }
    }
    else
    {
        if (gSYSFSCurrentMountPoint.inUse == false)
        {
            errorValue = SYS_FS_ERROR_NO_FILESYSTEM;
            return SYS_FS_RES_FAILURE;
        }

        disk = gSYSFSCurrentMountPoint.currentDisk;
    }

    if (disk->fsFunctions->getlabel == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    /* Append "0:" before the file name. This is required for different disks
     * */
    pathWithDiskNo[0] = (uint8_t)disk->diskNumber + '0';
    pathWithDiskNo[1] = ':';
    pathWithDiskNo[2] = '\0';

    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->getlabel((const char *)pathWithDiskNo, buff, sn);
        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));
        errorValue = (SYS_FS_ERROR)fileStatus;
    }
    else
    {
        errorValue = SYS_FS_ERROR_DENIED;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_DriveLabelSet
    (
        const char* drive,
        const char *label
    );

  Summary:
    Sets the drive label

  Description:
    This function sets the label for the drive specified. If no drive is
    specified, the label for the current drive is set.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_DriveLabelSet
(
    const char *drive, 
    const char *label
)
{
    int fileStatus = SYS_FS_ERROR_NOT_READY;
    SYS_FS_MOUNT_POINT *disk = (SYS_FS_MOUNT_POINT *) NULL;
    char pathWithDiskNo[SYS_FS_PATH_LEN_WITH_DISK_NUM] = { 0 };
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (label == NULL)
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    if (drive != NULL)
    {
        /* Get disk number */
        if(SYS_FS_GetDisk(drive, &disk, NULL) == false)
        {
            /* "errorValue" contains the reason for failure. */
            return SYS_FS_RES_FAILURE;
        }
    }
    else    /* if(drive == NULL */
    {
        if(gSYSFSCurrentMountPoint.inUse == false)
        {
            SYS_ASSERT(false, "Invalid mount point. Was the disk mounted?");
            errorValue = SYS_FS_ERROR_NO_FILESYSTEM;
            return SYS_FS_RES_FAILURE;
        }

        disk = gSYSFSCurrentMountPoint.currentDisk;
    }

    if(disk->fsFunctions->setlabel == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    /* Append "0:" before the file name. This is required for different disks
     * */
    pathWithDiskNo[0] = (uint8_t)disk->diskNumber + '0';
    pathWithDiskNo[1] = ':';

    /* Form the name with the drive letter */
    strcpy((char *)&pathWithDiskNo[2], (const char *)label);

    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->setlabel((const char *)pathWithDiskNo);
        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));

        errorValue = (SYS_FS_ERROR)fileStatus;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_DriveFormat
    (
        const char* drive,
        SYS_FS_FORMAT fmt,
        uint32_t clusterSize
    );

  Summary:
    Formats a drive.

  Description:
    This function formats a logic drive (create a FAT file system on the
    logical drive), as per the format specified.

    If the logical drive that has to be formatted has been bound to any
    partition (1-4) by multiple partition feature, the FAT volume is created
    into the specified partition. In this case, the second argument fmt is
    ignored. The physical drive must have been partitioned prior to using this
    function.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_DriveFormat
(
    const char* drive, 
    SYS_FS_FORMAT fmt, 
    uint32_t clusterSize
)
{
    int fileStatus = SYS_FS_ERROR_NOT_READY;
    SYS_FS_MOUNT_POINT *disk = (SYS_FS_MOUNT_POINT *) NULL;
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if (drive == NULL)
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(drive, &disk, NULL) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    if (disk->fsFunctions->formatDisk == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return (SYS_FS_RES_FAILURE);
    }

    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->formatDisk((uint8_t)disk->diskNumber, fmt, clusterSize);
        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));

        errorValue = (SYS_FS_ERROR)fileStatus;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/* Function:
    SYS_FS_RESULT SYS_FS_DrivePartition
    (
        const char *path,
        const uint32_t partition[],
        void *work
    );

  Summary:
    Partitions a physical drive (media).

  Description:
    This function partitions a physical drive (media) into requested partition
    sizes. This function will alter the MBR of the physical drive and make it
    into multi partitions. Windows operating systems do not support multi
    partitioned removable media. Maximum 4 partitions can be created on a
    media.

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_DrivePartition
(
    const char *path,
    const uint32_t partition[],
    void *work
)
{
    int fileStatus = SYS_FS_ERROR_NOT_READY;
    SYS_FS_MOUNT_POINT *disk = (SYS_FS_MOUNT_POINT *) NULL;

    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if ((path == NULL) || (work == NULL))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    /* Get disk number */
    if (SYS_FS_GetDisk(path, &disk, NULL) == false)
    {
        /* "errorValue" contains the reason for failure. */
        return SYS_FS_RES_FAILURE;
    }

    if(disk->fsFunctions->partitionDisk == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return (SYS_FS_RES_FAILURE);
    }

    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
        fileStatus = disk->fsFunctions->partitionDisk((uint8_t)disk->diskNumber, partition, work);
        OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));

        errorValue = (SYS_FS_ERROR)fileStatus;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

//******************************************************************************
/*Function:
    SYS_FS_RESULT SYS_FS_DriveSectorGet
    (
        const char* path,
        uint32_t *totalSectors,
        uint32_t *freeSectors
    );

  Summary:
    Obtains total number of sectors and number of free sectors for the
    specified drive.

  Description:
    Function to obtain the total number of sectors and number of free sectors
    in a drive (media).

  Remarks:
    See sys_fs.h for usage information.
***************************************************************************/
SYS_FS_RESULT SYS_FS_DriveSectorGet
(
    const char* path,
    uint32_t *totalSectors,
    uint32_t *freeSectors
)
{
    int fileStatus = -1;
    SYS_FS_MOUNT_POINT *disk = NULL;
    uint8_t pathWithDiskNo[3] = { 0 };
    OSAL_RESULT osalResult = OSAL_RESULT_FALSE;

    if ((totalSectors == NULL) || (freeSectors == NULL))
    {
        errorValue = SYS_FS_ERROR_INVALID_PARAMETER;
        return SYS_FS_RES_FAILURE;
    }

    if (path != NULL)
    {
        /* Get disk number */
        if(SYS_FS_GetDisk(path, &disk, NULL) == false)
        {
            /* "errorValue" contains the reason for failure. */
            return SYS_FS_RES_FAILURE;
        }
    }
    else
    {
        if(gSYSFSCurrentMountPoint.inUse == false)
        {
            errorValue = SYS_FS_ERROR_NO_FILESYSTEM;
            return SYS_FS_RES_FAILURE;
        }

        disk = gSYSFSCurrentMountPoint.currentDisk;
    }

    if(disk->fsFunctions->getCluster == NULL)
    {
        errorValue = SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS;
        return SYS_FS_RES_FAILURE;
    }

    /* Append "0:" before the file name. This is required for different disks
     * */
    pathWithDiskNo[0] = (uint8_t)disk->diskNumber + '0';
    pathWithDiskNo[1] = ':';
    pathWithDiskNo[2] = '\0';

    osalResult = OSAL_MUTEX_Lock(&(disk->mutexDiskVolume), OSAL_WAIT_FOREVER);
    if (osalResult == OSAL_RESULT_TRUE)
    {
       fileStatus = disk->fsFunctions->getCluster((const char *)pathWithDiskNo, totalSectors, freeSectors);
       OSAL_MUTEX_Unlock(&(disk->mutexDiskVolume));
       errorValue = (SYS_FS_ERROR)fileStatus;
    }

    return (fileStatus == 0) ? SYS_FS_RES_SUCCESS : SYS_FS_RES_FAILURE;
}

/*************************************************************************
* END OF sys_fs.c
***************************************************************************/

