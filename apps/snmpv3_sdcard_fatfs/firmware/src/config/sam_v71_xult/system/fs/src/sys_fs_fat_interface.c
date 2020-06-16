/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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

#include "system/fs/sys_fs_fat_interface.h"
#include "system/fs/sys_fs.h"

typedef struct
{
    uint8_t inUse;
    FATFS volObj;
} FATFS_VOLUME_OBJECT;

typedef struct
{
    uint8_t inUse;
    FIL fileObj;
} FATFS_FILE_OBJECT;

typedef struct
{
    uint8_t inUse;
    DIR dirObj;
} FATFS_DIR_OBJECT;

static FATFS_VOLUME_OBJECT CACHE_ALIGN FATFSVolume[SYS_FS_VOLUME_NUMBER];
static FATFS_FILE_OBJECT CACHE_ALIGN FATFSFileObject[SYS_FS_MAX_FILES];
static FATFS_DIR_OBJECT CACHE_ALIGN FATFSDirObject[SYS_FS_MAX_FILES];
static uint8_t startupflag = 0;

typedef UINT(*STREAM_FUNC)(const BYTE*,UINT);

int FATFS_mount ( uint8_t vol )
{
    FATFS *fs = NULL;
    FRESULT res = FR_INT_ERR;
    TCHAR path[3];
    BYTE opt = 1; /* 0:Do not mount (delayed mount), 1:Mount immediately */

    uint8_t index = 0;

    if(0 == startupflag)
    {
        startupflag = 1;
        for(index = 0; index != SYS_FS_VOLUME_NUMBER ; index++ )
        {
            FATFSVolume[index].inUse = false;
        }
        for(index = 0; index != SYS_FS_MAX_FILES ; index++ )
        {
            FATFSFileObject[index].inUse = false;
            FATFSDirObject[index].inUse = false;
        }
    }

    /* Check if the drive number is valid */
    if (vol >= SYS_FS_VOLUME_NUMBER)
    {
        return FR_INVALID_DRIVE;
    }

    /* If the volume specified is already in use, then return failure, as we cannot mount it again */
    if(FATFSVolume[vol].inUse == true)
    {
        return FR_INVALID_DRIVE;
    }
    else
    {
        fs = &FATFSVolume[vol].volObj;
    }

    path[0] = '0' + vol;
    path[1] = ':';
    path[2] = '\0';

    res = f_mount(fs, (const TCHAR *)&path, opt);

    if (res == FR_OK)
    {
        FATFSVolume[vol].inUse = true;
    }

    return ((int)res);
}

int FATFS_unmount ( uint8_t vol )
{
    FRESULT res = FR_INT_ERR;
    TCHAR path[3];
    BYTE opt = 0;
    uint32_t hFATfs =0;

    if (vol >= SYS_FS_VOLUME_NUMBER)
    {
        return FR_INVALID_DRIVE;
    }

    /* If the volume specified not in use, then return failure, as we cannot unmount mount a free volume */
    if(FATFSVolume[vol].inUse == false)
    {
        return FR_INVALID_DRIVE;
    }
    
    // free the volume
    FATFSVolume[vol].inUse = false;

    for(hFATfs = 0; hFATfs < SYS_FS_MAX_FILES; hFATfs++)
    {
        if(FATFSFileObject[hFATfs].inUse)
        {
            if (FATFSFileObject[hFATfs].fileObj.obj.fs == NULL)
            {
                FATFSFileObject[hFATfs].inUse = false;
            }
            else if(VolToPart[vol].pd == FATFSFileObject[hFATfs].fileObj.obj.fs->pdrv)
            {
                FATFSFileObject[hFATfs].inUse = false;
            }
        }
        if(FATFSDirObject[hFATfs].inUse)
        {
            if (FATFSDirObject[hFATfs].dirObj.obj.fs == NULL)
            {
                FATFSDirObject[hFATfs].inUse = false;
            }
            else if(VolToPart[vol].pd == FATFSDirObject[hFATfs].dirObj.obj.fs->pdrv)
            {
                FATFSDirObject[hFATfs].inUse = false;
            }
        }
    }

    path[0] = '0' + vol;
    path[1] = ':';
    path[2] = '\0';

    res = f_mount(NULL, (const TCHAR *)&path, opt);

    return ((int)res);
}

int FATFS_open (
    uintptr_t handle,   /* Pointer to the blank file object */
    const char * path,  /* Pointer to the file name */
    uint8_t mode        /* Access mode and file open mode flags */
)
{
    FRESULT res = FR_INT_ERR;
    uint32_t index = 0;
    FIL *fp = NULL;

    /* Convert the SYS_FS file open attributes to FAT FS attributes */
    switch(mode)
    {
        case SYS_FS_FILE_OPEN_READ:
            mode = FA_READ;
            break;
        case SYS_FS_FILE_OPEN_WRITE:
            mode = FA_WRITE | FA_CREATE_ALWAYS;
            break;
        case SYS_FS_FILE_OPEN_APPEND:
            mode = FA_WRITE | FA_OPEN_APPEND;
            break;
        case SYS_FS_FILE_OPEN_READ_PLUS:
            mode = FA_READ | FA_WRITE;
            break;
        case SYS_FS_FILE_OPEN_WRITE_PLUS:
            mode = FA_READ | FA_WRITE | FA_CREATE_ALWAYS;
            break;
        case SYS_FS_FILE_OPEN_APPEND_PLUS:
            mode = FA_READ | FA_WRITE | FA_OPEN_APPEND;
            break;
        default:
            return ((int)res);
            break;
    }

    for (index = 0; index < SYS_FS_MAX_FILES; index++)
    {
        if(FATFSFileObject[index].inUse == false)
        {
            FATFSFileObject[index].inUse = true;
            fp = &FATFSFileObject[index].fileObj;
            *(uintptr_t *)handle = (uintptr_t)&FATFSFileObject[index];
            break;
        }
    }

    res = f_open(fp, (const TCHAR *)path, mode);

    if (res != FR_OK)
    {
        FATFSFileObject[index].inUse = false;
    }

    return ((int)res);
}

int FATFS_read (
    uintptr_t handle, /* Pointer to the file object */
    void* buff,       /* Pointer to data buffer */
    uint32_t btr,     /* Number of bytes to read */
    uint32_t* br      /* Pointer to number of bytes read */
)
{
    FRESULT res = FR_INT_ERR;
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    res = f_read(fp, buff, (UINT)btr, (UINT *)br);

    return ((int)res);
}

int FATFS_close (
    uintptr_t handle /* Pointer to the file object to be closed */
)
{
    FRESULT res;
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    if(ptr->inUse == false)
    {
        return FR_INVALID_OBJECT;
    }

    res = f_close(fp);

    if (res == FR_OK)
    {
        ptr->inUse = false;
    }
    return ((int)res);
}

int FATFS_lseek (
    uintptr_t handle,   /* Pointer to the file object */
    uint32_t ofs        /* File pointer from top of file */
)
{
    FRESULT res;
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    res = f_lseek(fp, (FSIZE_t)ofs);

    return ((int)res);
}

int FATFS_stat (
    const char* path,   /* Pointer to the file path */
    uintptr_t fileInfo  /* Pointer to file information to return */
)
{
    FRESULT res;
    FILINFO *finfo = (FILINFO *)fileInfo;

    res = f_stat((const TCHAR *)path, finfo);

    SYS_FS_FSTAT *fileStat = (SYS_FS_FSTAT *)fileInfo;

    if ((res == FR_OK) && (fileStat->lfname != NULL))
    {
        /* Use fileStat->fname instead */
        fileStat->lfname[0] = '\0';
    }

    return ((int)res);
}

int FATFS_getlabel (
    const char* path,  /* Path name of the logical drive number */
    char* label,       /* Pointer to a buffer to return the volume label */
    uint32_t* vsn           /* Pointer to a variable to return the volume serial number */
)
{
    FRESULT res;

    res = f_getlabel((const TCHAR *)path, (TCHAR *)label, (DWORD *)vsn);

    return ((int)res);
}

int FATFS_getcwd (
    char* buff,    /* Pointer to the directory path */
    uint32_t len    /* Size of path */
)
{
    FRESULT res;
    res = f_getcwd((TCHAR *)buff, (UINT)len);

    return ((int)res);
}

char* FATFS_gets (
    char* buff,    /* Pointer to the string buffer to read */
    int len,        /* Size of string buffer (characters) */
    uintptr_t handle/* Pointer to the file object */
)
{
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    return ((char *)f_gets((TCHAR *)buff, len, fp));
}

int FATFS_opendir (
    uintptr_t handle,           /* Pointer to directory object to create */
    const char* path   /* Pointer to the directory path */
)
{
    FRESULT res;
    uint32_t index = 0;
    DIR *dp = NULL;

    for(index = 0; index < SYS_FS_MAX_FILES; index++)
    {
        if(FATFSDirObject[index].inUse == false)
        {
            FATFSDirObject[index].inUse = true;
            dp = &FATFSDirObject[index].dirObj;
            *(uintptr_t *)handle = (uintptr_t)&FATFSDirObject[index];
            break;
        }
    }

    if(index >= SYS_FS_MAX_FILES)
    {
        return FR_INVALID_OBJECT;
    }

    res = f_opendir(dp, (const TCHAR *)path);

    if (res != FR_OK)
    {
        FATFSDirObject[index].inUse = false;
    }

    return ((int)res);
}

int FATFS_readdir (
    uintptr_t handle,   /* Pointer to the open directory object */
    uintptr_t fileInfo  /* Pointer to file information to return */
)
{
    FRESULT res;
    FATFS_DIR_OBJECT *ptr = (FATFS_DIR_OBJECT *)handle;
    DIR *dp = &ptr->dirObj;

    FILINFO *finfo = (FILINFO *)fileInfo;

    res = f_readdir(dp, finfo);

    SYS_FS_FSTAT *fileStat = (SYS_FS_FSTAT *)fileInfo;

    if ((res == FR_OK) && (fileStat->lfname != NULL))
    {
        /* Use fileStat->fname instead */
        fileStat->lfname[0] = '\0';
    }

    return ((int)res);
}

int FATFS_closedir (
    uintptr_t handle /* Pointer to the directory object to be closed */
)
{
    FRESULT res;
    FATFS_DIR_OBJECT *ptr = (FATFS_DIR_OBJECT *)handle;
    DIR *dp = &ptr->dirObj;

    if(ptr->inUse == false)
    {
        return FR_INVALID_OBJECT;
    }

    res = f_closedir(dp);

    if (res == FR_OK)
    {
        ptr->inUse = false;
    }

    return ((int)res);
}

int FATFS_chdir (
    const char* path   /* Pointer to the directory path */
)
{
    FRESULT res;

    res = f_chdir((const TCHAR *)path);

    return ((int)res);
}

int FATFS_chdrive (
    uint8_t drv     /* Drive number */
)
{
    FRESULT res;
    TCHAR path[3];

    path[0] = '0' + drv;
    path[1] = ':';
    path[2] = '\0';

    res = f_chdrive((const TCHAR *)&path);

    return ((int)res);
}


int FATFS_write (
    uintptr_t handle,   /* Pointer to the file object */
    const void *buff,   /* Pointer to the data to be written */
    uint32_t btw,       /* Number of bytes to write */
    uint32_t* bw        /* Pointer to number of bytes written */
)
{
    FRESULT res = FR_INT_ERR;
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    res = f_write(fp, buff, (UINT)btw, (UINT *)bw);

    return ((int)res);
}

uint32_t FATFS_tell(uintptr_t handle)
{
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    return(f_tell(fp));
}

bool FATFS_eof(uintptr_t handle)
{
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    return ((bool)f_eof(fp));
}

uint32_t FATFS_size(uintptr_t handle)
{
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    return (f_size(fp));
}

int FATFS_mkdir (
    const char* path       /* Pointer to the directory path */
)
{
    FRESULT res;

    res = f_mkdir((const TCHAR *)path);

    return ((int)res);
}

int FATFS_unlink (
    const char* path       /* Pointer to the file or directory path */
)
{
    FRESULT res;

    res = f_unlink((const TCHAR *)path);

    return ((int)res);
}

int FATFS_setlabel (
    const char* label  /* Volume label to set with heading logical drive number */
)
{
    FRESULT res;

    res = f_setlabel((const TCHAR *)label);

    return ((int)res);
}

int FATFS_truncate (
    uintptr_t handle /* Pointer to the file object */
)
{
    FRESULT res;

    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    res = f_truncate(fp);

    return ((int)res);
}

int FATFS_chmod (
    const char* path,  /* Pointer to the file path */
    uint8_t attr,       /* Attribute bits */
    uint8_t mask        /* Attribute mask to change */
)
{
    FRESULT res;

    res = f_chmod((const TCHAR *)path, (BYTE)attr, (BYTE)mask);

    return ((int)res);
}

int FATFS_utime (
    const char* path,  /* Pointer to the file/directory name */
    const uintptr_t ptr /* Pointer to the time stamp to be set */
)
{
    FRESULT res;

    FILINFO *finfo = (FILINFO *)ptr;

    res = f_utime((const TCHAR *)path, finfo);

    return ((int)res);
}

int FATFS_rename (
    const char* path_old,  /* Pointer to the object to be renamed */
    const char* path_new   /* Pointer to the new name */
)
{
    FRESULT res;

    res = f_rename((const TCHAR *)path_old, (const TCHAR *)path_new);

    return ((int)res);
}

int FATFS_sync (
    uintptr_t handle /* Pointer to the file object */
)
{
    FRESULT res = FR_INT_ERR;
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    res = f_sync(fp);

    return ((int)res);
}

int FATFS_putc (
    char c,    /* A character to be output */
    uintptr_t handle/* Pointer to the file object */
)
{
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    return (f_putc((TCHAR)c, fp));
}

int FATFS_puts (
    const char* str,   /* Pointer to the string to be output */
    uintptr_t handle    /* Pointer to the file object */
)
{
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    return (f_puts((const TCHAR *)str, fp));
}

int FATFS_printf (
    uintptr_t handle,           /* Pointer to the file object */
    const char* fmt,   /* Pointer to the format string */
    va_list argList                 /* Optional arguments... */
)
{
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    return (f_printf(fp, (const TCHAR *)fmt, argList));
}

bool FATFS_error(uintptr_t handle)
{
    FATFS_FILE_OBJECT *ptr = (FATFS_FILE_OBJECT *)handle;
    FIL *fp = &ptr->fileObj;

    return ((bool)f_error(fp));
}

int FATFS_mkfs (
    uint8_t vol,            /* Logical drive number */
    const MKFS_PARM* opt,   /* Format options */
    void* work,             /* Pointer to working buffer (null: use heap memory) */
    uint32_t len            /* Size of working buffer [byte] */
)
{
    FRESULT res;
    TCHAR path[3];

    /* Check if the drive number is valid */
    if (vol >= SYS_FS_VOLUME_NUMBER)
    {
        return FR_INVALID_DRIVE;
    }

    path[0] = '0' + vol;
    path[1] = ':';
    path[2] = '\0';

    res = f_mkfs((const TCHAR *)&path, opt, work, (UINT)len);

    return ((int)res);
}

int FATFS_fdisk (
    uint8_t pdrv,           /* Physical drive number */
    const uint32_t ptbl[],  /* Pointer to the size table for each partitions */
    void* work          /* Pointer to the working buffer */
)
{
    FRESULT res;

    res = f_fdisk((BYTE)pdrv, (const LBA_t *)ptbl, work);

    return ((int)res);
}

/*-----------------------------------------------------------------------*/
/* Function below written separately.                                    */
/* Not from standard FAT FS code. It is added to allow compatibility with*/
/* other FS, as this function does not need "FATFS **fatfs"              */
/*-----------------------------------------------------------------------*/

int FATFS_getclusters (
    const char *path,
    uint32_t *tot_sec,
    uint32_t *free_sec
)
{
    FATFS *fs = (FATFS *)NULL;
    DWORD clst = 0;
    FRESULT res = FR_OK;

    res = f_getfree((const TCHAR *)path, &clst, &fs);

    if(res != FR_OK)
    {
        *tot_sec = 0;
        *free_sec = 0;
        return (int)res;
    }

    /* Get total sectors and free sectors */
    *tot_sec = (fs->n_fatent - 2) * fs->csize;
    *free_sec = clst * fs->csize;

    return ((int)res);
}

int FATFS_getfree (
    const char* path,  /* Path name of the logical drive number */
    uint32_t* nclst,        /* Pointer to a variable to return number of free clusters */
    FATFS** fatfs       /* Pointer to return pointer to corresponding file system object */
)
{
    FRESULT res;

    res = f_getfree((const TCHAR *)path, (DWORD *)nclst, fatfs);

    return ((int)res);
}

