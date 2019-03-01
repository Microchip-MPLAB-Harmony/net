/*******************************************************************************
  File System System-Library Local Types and Defintions

  Company:
    Microchip Technology Inc.

  File Name:
    sys_fs_local.h

  Summary:
    Contains local types and defintions required by the SYS_FS functions.

  Description:
    This file contains local types and defintions required by the SYS_FS 
    functions. These types are internal to the SYS_FS function implementation
    and should not be used directly by the application. 
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

#ifndef _SYS_FS_PRIVATE_H
#define _SYS_FS_PRIVATE_H

#include "configuration.h"
#include "system/system.h"
#include "system/fs/sys_fs.h"
#include "system/fs/fat_fs/src/file_system/ff.h"
#include "system/fs/sys_fs_media_manager.h"
#include "osal/osal.h"
#include <string.h>

#define SYS_FS_PATH_LEN_WITH_DISK_NUM (FAT_FS_MAX_LFN + 3)
#ifndef SYS_FS_CWD_STRING_LEN
#define SYS_FS_CWD_STRING_LEN (1024)
#endif

// *****************************************************************************
/* Mount point

  Summary:
    Defines the mount point for each volume.

  Description:
    This structure defines the mount point that is utilized when a volume is
 * mounted by application code. This mount point is one per volume mounted.

  Remarks:
    None.
*/
typedef struct
{
    /* If the mount point is in use or is it free */
    bool inUse;
    /* Type of native file system implemented on the volume mounted */
    SYS_FS_FILE_SYSTEM_TYPE fsType;
    /* List of function pointers associated with the native file system */
    const SYS_FS_FUNCTIONS * fsFunctions;
    /* Mount name. The format for mount name should follow: "/mnt/myDrive1" */
    const char *mountName;
    uint8_t mountNameLength;
    /* Volume number */
    uint8_t diskNumber;
   /* Mount/Volume instance mutex */
    OSAL_MUTEX_DECLARE(mutexDiskVolume);
}
SYS_FS_MOUNT_POINT;

// *****************************************************************************
/* File Object

  Summary:
    Defines the file object for each file opened.

  Description:
    This structure defines the file object which is allocated when a file is
 * opened by application code.

  Remarks:
    None.
*/
typedef struct
{
    /* If the file object is in use or is it free */
    bool                inUse;
    /* pointer to the mount point associated with the volume from where file
       is being opened */
    SYS_FS_MOUNT_POINT  *mountPoint;
    /* File object as obtained from the native file system */
    uintptr_t            nativeFSFileObj;
    
    /* File specific error value */
    SYS_FS_ERROR errorValue;
    
    /* Name of file is stored in a buffer for future use */
    uint8_t fileName[FAT_FS_MAX_LFN + 1] CACHE_ALIGN;

}SYS_FS_OBJ;

// *****************************************************************************
/* Directory Object

  Summary:
    Defines the directory object for each directory opened.

  Description:
    This structure defines the directory object which is allocated when a directory is
    opened by application code.

  Remarks:
    None.
*/
typedef struct
{
    /* If the file object is in use or is it free */
    bool                inUse;
    /* pointer to the mount point associated with the volume from where file
       is being opened */
    SYS_FS_MOUNT_POINT  *mountPoint;
    /* Directory object as obtained from the native file system */
    uintptr_t            nativeFSDirObj;
    /* Directory specific error value */
    SYS_FS_ERROR errorValue;

}SYS_FS_DIR_OBJ;

// *****************************************************************************
/* Current Mount point

  Summary:
    Defines the mount point currently being used.

  Description:
  This structure defines the mount which is used currently. If the application
  do not specify the full path name (along with the drive name), then this mount
  point will be taken as the default value.

  Remarks:
    None.
*/
typedef struct
{
    /* If the current mount point is valid or not */
    bool                inUse;
    /* Pointer to the current disk object */
    SYS_FS_MOUNT_POINT *currentDisk;
}
SYS_FS_CURRENT_MOUNT_POINT;

//******************************************************************************
/*Function:
    bool _SYS_FS_StringWildCardCompare(const char * ptr1, const char *ptr2)

    Summary:
    Compares the 2 file names with wild character.

    Description:
        Compares the 2 file names with wild character. This is required for
        file search of a directory.

    Precondition:
        Atlesat, one disk has to be mounted before this funcion could be set.


    Parameters:
        ptr1            - First file name. This can have wild characters (*, ?).

        ptr2     	- Second file name. This should be complete file name.
                          No wild char should be passed here.


    Returns:
        If comparison is success	-	true

        If comparison is failure	-	false

  Remarks:
	None
***************************************************************************/
bool _SYS_FS_StringWildCardCompare(const char * ptr1, const char *ptr2);


#endif // _SYS_FS_PRIVATE_H

