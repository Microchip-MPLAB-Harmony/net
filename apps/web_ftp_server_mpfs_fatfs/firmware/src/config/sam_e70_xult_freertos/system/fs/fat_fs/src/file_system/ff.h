/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module include R0.11a    (C)ChaN, 2015
/----------------------------------------------------------------------------/
/ FatFs module is a free software that opened under license policy of
/ following conditions.
/
/ Copyright (C) 2015, ChaN, all right reserved.
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/---------------------------------------------------------------------------*/


#ifndef _FATFS
#define _FATFS	64180	/* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "configuration.h"

#include "ffconf.h"		/* FatFs configuration options */
#include "system/fs/src/sys_fs_media_manager_local.h"
    
extern const SYS_FS_FUNCTIONS FatFsFunctions;
#if _FATFS != _FFCONF
#error Wrong configuration file (ffconf.h).
#endif



/* Definitions of volume management */

#if _MULTI_PARTITION		/* Multiple partition configuration */
typedef struct {
	uint8_t pd;	/* Physical drive number */
	uint8_t pt;	/* Partition: 0:Auto detect, 1-4:Forced partition) */
} PARTITION;

#if !defined(SYS_FS_VOLUME_NUMBER)
#define SYS_FS_VOLUME_NUMBER 5
#endif

extern PARTITION VolToPart[SYS_FS_VOLUME_NUMBER];	/* Volume - Partition resolution table */

#define LD2PD(vol) (VolToPart[vol].pd)	/* Get physical drive number */
#define LD2PT(vol) (VolToPart[vol].pt)	/* Get partition index */

#else							/* Single partition configuration */
#define LD2PD(vol) (uint8_t)(vol)	/* Each logical drive is bound to the same physical drive number */
#define LD2PT(vol) 0			/* Find first valid partition or in SFD */

#endif



/* Type of path name strings on FatFs API */

#if _LFN_UNICODE			/* Unicode string */
#if !FAT_FS_USE_LFN
#error _LFN_UNICODE must be 0 at non-LFN cfg.
#endif
#ifndef _INC_TCHAR
typedef uint16_t TCHAR;
#define _T(x) L ## x
#define _TEXT(x) L ## x
#endif

#else						/* ANSI/OEM string */
#ifndef _INC_TCHAR
typedef char TCHAR;
#define _T(x) x
#define _TEXT(x) x
#endif

#endif



/* File system object structure (FATFS) */

typedef struct {
	uint8_t	fs_type;		/* FAT sub-type (0:Not mounted) */
	uint8_t	drv;			/* Physical drive number */
	uint8_t	csize;			/* Sectors per cluster (1,2,4...128) */
	uint8_t	n_fats;			/* Number of FAT copies (1 or 2) */
	uint8_t	wflag;			/* win[] flag (b0:dirty) */
	uint8_t	fsi_flag;		/* FSINFO flags (b7:disabled, b0:dirty) */
	uint16_t	id;				/* File system mount ID */
	uint16_t	n_rootdir;		/* Number of root directory entries (FAT12/16) */
#if FAT_FS_MAX_SS != _MIN_SS
	uint16_t	ssize;			/* Bytes per sector (512, 1024, 2048 or 4096) */
#endif
#if _FS_REENTRANT
	_SYNC_t	sobj;			/* Identifier of sync object */
#endif
#if !_FS_READONLY
	uint32_t	last_clust;		/* Last allocated cluster */
	uint32_t	free_clust;		/* Number of free clusters */
#endif
#if _FS_RPATH
	uint32_t	cdir;			/* Current directory start cluster (0:root) */
#endif
	uint32_t	n_fatent;		/* Number of FAT entries, = number of clusters + 2 */
	uint32_t	fsize;			/* Sectors per FAT */
	uint32_t	volbase;		/* Volume start sector */
	uint32_t	fatbase;		/* FAT start sector */
	uint32_t	dirbase;		/* Root directory start sector (FAT32:Cluster#) */
	uint32_t	database;		/* Data start sector */
	uint32_t	winsect;		/* Current sector appearing in the win[] */
	uint8_t CACHE_ALIGN win[FAT_FS_MAX_SS];	/* Disk access window for Directory, FAT (and file data at tiny cfg) */
} FATFS;



/* File object structure (FIL) */

typedef struct {
	FATFS*	fs;				/* Pointer to the related file system object (**do not change order**) */
	uint16_t	id;				/* Owner file system mount ID (**do not change order**) */
	uint8_t	flag;			/* Status flags */
	uint8_t	err;			/* Abort flag (error code) */
	uint32_t	fptr;			/* File read/write pointer (Zeroed on file open) */
	uint32_t	fsize;			/* File size */
	uint32_t	sclust;			/* File start cluster (0:no cluster chain, always 0 when fsize is 0) */
	uint32_t	clust;			/* Current cluster of fpter (not valid when fprt is 0) */
	uint32_t	dsect;			/* Sector number appearing in buf[] (0:invalid) */
#if !_FS_READONLY
	uint32_t	dir_sect;		/* Sector number containing the directory entry */
	uint8_t*	dir_ptr;		/* Pointer to the directory entry in the win[] */
#endif
#if _USE_FASTSEEK
	uint32_t*	cltbl;			/* Pointer to the cluster link map table (Nulled on file open) */
#endif
#if _FS_LOCK
	uint32_t	lockid;			/* File lock ID origin from 1 (index of file semaphore table Files[]) */
#endif
#if !_FS_TINY
	uint8_t	CACHE_ALIGN buf[FAT_FS_MAX_SS];	/* File private data read/write window */
#endif
} FIL;



/* Directory object structure (DIR) */

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object (**do not change order**) */
	uint16_t	id;				/* Owner file system mount ID (**do not change order**) */
	uint16_t	index;			/* Current read/write index number */
	uint32_t	sclust;			/* Table start cluster (0:Root dir) */
	uint32_t	clust;			/* Current cluster */
	uint32_t	sect;			/* Current sector */
	uint8_t*	dir;			/* Pointer to the current SFN entry in the win[] */
	uint8_t*	fn;				/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
#if _FS_LOCK
	uint32_t	lockid;			/* File lock ID (index of file semaphore table Files[]) */
#endif
#if FAT_FS_USE_LFN
	uint16_t*	lfn;			/* Pointer to the LFN working buffer */
	uint16_t	lfn_idx;		/* Last matched LFN index number (0xFFFF:No LFN) */
#endif
#if _USE_FIND
	const TCHAR*	pat;	/* Pointer to the name matching pattern */
#endif
} DIR;



/* File information structure (FILINFO) */

typedef struct {
	uint32_t	fsize;			/* File size */
	uint16_t	fdate;			/* Last modified date */
	uint16_t	ftime;			/* Last modified time */
	uint8_t	fattrib;		/* Attribute */
	TCHAR	fname[13];		/* Short file name (8.3 format) */
#if FAT_FS_USE_LFN
	TCHAR*	lfname;			/* Pointer to the LFN buffer */
	uint32_t 	lfsize;			/* Size of LFN buffer in TCHAR */
#endif
} FILINFO;



/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Access denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_LOCK */
	FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs module application interface                           */
//int f_mount (FATFS* fs, const TCHAR* path, uint8_t opt);			/* Mount/Unmount a logical drive */
int f_mount (uint8_t vol);								/* Mount a logical drive */
int f_unmount (uint8_t vol);
int f_open (uintptr_t handle, const char* path, uint8_t mode);				/* Open or create a file */
int f_read (uintptr_t handle, void* buff, uint32_t btr, uint32_t* br);			/* Read data from a file */
int f_lseek (uintptr_t handle, uint32_t ofs);								/* Move file pointer of a file object */
int f_close (uintptr_t handle);											/* Close an open file object */
int f_opendir (uintptr_t handle, const TCHAR* path);						/* Open a directory */
int f_readdir (uintptr_t handle, uintptr_t fno);							/* Read a directory item */
int f_closedir (uintptr_t handle);										/* Close an open directory */
int f_stat (const char* path, uintptr_t ptr);					/* Get file status */
int f_write (uintptr_t handle, const void* buff, uint32_t btw, uint32_t* bw);	/* Write data to a file */
int f_getfree (const TCHAR* path, uint32_t* nclst, FATFS** fatfs);	/* Get number of free clusters on the drive */
int f_truncate (uintptr_t handle);										/* Truncate file */
int f_sync (uintptr_t handle);											/* Flush cached data of a writing file */
int f_unlink (const TCHAR* path);								/* Delete an existing file or directory */
int f_mkdir (const TCHAR* path);								/* Create a sub directory */
int f_chmod (const TCHAR* path, uint8_t attr, uint8_t mask);			/* Change attribute of the file/dir */
int f_utime (const TCHAR* path, const uintptr_t fno);			/* Change times-tamp of the file/dir */
int f_rename (const TCHAR* path_old, const TCHAR* path_new);	/* Rename/Move a file or directory */
int f_chdrive (uint8_t drv);								/* Change current drive */
int f_chdir (const TCHAR* path);								/* Change current directory */
int f_getcwd (TCHAR* buff, uint32_t len);							/* Get current directory */
int f_getlabel (const TCHAR* path, TCHAR* label, uint32_t* vsn);	/* Get volume label */
int f_setlabel (const TCHAR* label);							/* Set volume label */
int f_forward (uintptr_t handle, uint32_t(*func)(const uint8_t*, uint32_t), uint32_t btf, uint32_t* bf);	/* Forward data to the stream */
int f_mkfs (uint8_t vol, uint8_t sfd, uint32_t au);				/* Create a file system on the volume */
int f_fdisk (uint8_t pdrv, const uint32_t szt[], void* work);			/* Divide a physical drive into some partitions */
int f_putc (TCHAR c, uintptr_t handle);										/* Put a character to the file */
int f_puts (const TCHAR* str, uintptr_t handle);								/* Put a string to the file */
int f_printf (uintptr_t handle, const TCHAR* str, va_list argList);				/* Put a formatted string to the file */
TCHAR* f_gets (TCHAR* buff, int len, uintptr_t handle);						/* Get a string from the file */
int f_findfirst (uintptr_t handle, uintptr_t fno, const TCHAR* path, const TCHAR* pattern);	/* Find first file */
FRESULT f_findnext (uintptr_t handle, uintptr_t fno);							/* Find next file */
int f_getclusters (const char *path, uint32_t *tot_sec, uint32_t *free_sec);

//#define f_tell(fp) ((fp)->fptr)
//#define f_eof(fp) ((int)((fp)->fptr == (fp)->fsize))
//#define f_size(fp) ((fp)->fsize)
//#define f_error(fp) ((fp)->err)
//#define f_rewind(fp) f_lseek((fp), 0)
//#define f_rewinddir(dp) f_readdir((dp), 0)

uint32_t f_tell(uintptr_t handle);
bool f_eof(uintptr_t handle);
uint32_t f_size(uintptr_t handle);
bool f_error(uintptr_t handle);


#ifndef EOF
#define EOF (-1)
#endif




/*--------------------------------------------------------------*/
/* Additional user defined functions                            */

/* RTC function */
#if !_FS_READONLY && !_FS_NORTC
uint32_t get_fattime (void);
#endif

/* Unicode support functions */
#if FAT_FS_USE_LFN							/* Unicode - OEM code conversion */
uint16_t ff_convert (uint16_t chr, uint32_t dir);	/* OEM-Unicode bidirectional conversion */
uint16_t ff_wtoupper (uint16_t chr);			/* Unicode upper-case conversion */
#if FAT_FS_USE_LFN == 3						/* Memory functions */
void* ff_memalloc (uint32_t msize);			/* Allocate memory block */
void ff_memfree (void* mblock);			/* Free memory block */
#endif
#endif

/* Sync functions */
#if _FS_REENTRANT
int ff_cre_syncobj (uint8_t vol, _SYNC_t* sobj);	/* Create a sync object */
int ff_req_grant (_SYNC_t sobj);				/* Lock sync object */
void ff_rel_grant (_SYNC_t sobj);				/* Unlock sync object */
int ff_del_syncobj (_SYNC_t sobj);				/* Delete a sync object */
#endif




/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access control and file status flags (FIL.flag) */

#define	FA_READ				0x01
#define	FA_OPEN_EXISTING	0x00

#if !_FS_READONLY
#define	FA_WRITE			0x02
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define FA__WRITTEN			0x20
#define FA__DIRTY			0x40
#endif


/* FAT sub type (FATFS.fs_type) */

#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3


/* File attribute bits for directory entry */

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_MASK	0x3F	/* Mask of defined bits */


/* Fast seek feature */
#define CREATE_LINKMAP	0xFFFFFFFF



/*--------------------------------*/
/* Multi-byte word access macros  */

#if _WORD_ACCESS == 1	/* Enable word access to the FAT structure */
#define	LD_WORD(ptr)		(uint16_t)(*(uint16_t*)(uint8_t*)(ptr))
#define	LD_DWORD(ptr)		(uint32_t)(*(uint32_t*)(uint8_t*)(ptr))
#define	ST_WORD(ptr,val)	*(uint16_t*)(uint8_t*)(ptr)=(uint16_t)(val)
#define	ST_DWORD(ptr,val)	*(uint32_t*)(uint8_t*)(ptr)=(uint32_t)(val)
#else					/* Use byte-by-byte access to the FAT structure */
#define	LD_WORD(ptr)		(uint16_t)(((uint16_t)*((uint8_t*)(ptr)+1)<<8)|(uint16_t)*(uint8_t*)(ptr))
#define	LD_DWORD(ptr)		(uint32_t)(((uint32_t)*((uint8_t*)(ptr)+3)<<24)|((uint32_t)*((uint8_t*)(ptr)+2)<<16)|((uint16_t)*((uint8_t*)(ptr)+1)<<8)|*(uint8_t*)(ptr))
#define	ST_WORD(ptr,val)	*(uint8_t*)(ptr)=(uint8_t)(val); *((uint8_t*)(ptr)+1)=(uint8_t)((uint16_t)(val)>>8)
#define	ST_DWORD(ptr,val)	*(uint8_t*)(ptr)=(uint8_t)(val); *((uint8_t*)(ptr)+1)=(uint8_t)((uint16_t)(val)>>8); *((uint8_t*)(ptr)+2)=(uint8_t)((uint32_t)(val)>>16); *((uint8_t*)(ptr)+3)=(uint8_t)((uint32_t)(val)>>24)
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* _FATFS */
