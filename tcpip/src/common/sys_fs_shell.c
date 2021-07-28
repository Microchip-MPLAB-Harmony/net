/*******************************************************************************
  Harmony File System Shell for TCPIP

  File Name:
    sys_fs_shell.c

  Summary:
    This file provides shell functions for Harmony SYS_FS calls

  Description:
    This file provides shell functions for Harmony SYS_FS calls
 *******************************************************************************/


// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2019 Microchip Technology Inc. and its subsidiaries.

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


//#define SYS_FS_SHELL_MTHREAD_PROTECTION     0
//#define SYS_FS_SHELL_MALLOC                 malloc
//#define SYS_FS_SHELL_FREE                   free
#define SYS_FS_SHELL_DEBUG                    0

// SYS_FS workaround
// SYS_FS functions like SYS_FS_DirOpen() won't properly open a directory
// if the directory name ends in '/'.
// This workaround removes the trailing '/' from the path!
// Should be fixed in the SYS_FS!
#define SYS_FS_TRAIL_SLASH_WORKAROUND     1


// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include <stdlib.h>

#include "./sys_fs_shell.h"
#include "configuration.h"

#if defined(SYS_FS_SHELL_MTHREAD_PROTECTION) && (SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
#define _SYS_FS_SHELL_MTHREAD_PROTECTION 1
#include "osal/osal.h"
#else
#define _SYS_FS_SHELL_MTHREAD_PROTECTION 0
#endif  // (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)


// local prototypes

static SYS_FS_HANDLE Shell_FileOpen(const SYS_FS_SHELL_OBJ* pObj, const char *fname, SYS_FS_FILE_OPEN_ATTRIBUTES attributes);
static SYS_FS_RESULT Shell_FileStat(const SYS_FS_SHELL_OBJ* pObj, const char *fname, SYS_FS_FSTAT *buf);
static SYS_FS_RESULT Shell_FileDelete(const SYS_FS_SHELL_OBJ* pObj, const char *fname);
static SYS_FS_HANDLE Shell_DirOpen(const SYS_FS_SHELL_OBJ* pObj, const char *fname);
static SYS_FS_HANDLE Shell_DirMake(const SYS_FS_SHELL_OBJ* pObj, const char *fname);
static SYS_FS_SHELL_RES Shell_Cwd(const SYS_FS_SHELL_OBJ* pObj, const char *cwd);
static SYS_FS_SHELL_RES Shell_LastError(const SYS_FS_SHELL_OBJ* pObj);
static SYS_FS_SHELL_RES Shell_Delete(const SYS_FS_SHELL_OBJ* pObj);
static SYS_FS_RESULT Shell_FileClose(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle);
static SYS_FS_RESULT Shell_DirClose(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle);
static int32_t  Shell_FileSize(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle);
static int32_t  Shell_FileSeek(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle, int32_t offset, SYS_FS_FILE_SEEK_CONTROL whence);
static int32_t  Shell_FileTell(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle);
static bool     Shell_FileEof(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle);
static size_t Shell_FileRead(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle, void *buf, size_t nbyte);
static size_t Shell_FileWrite(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle, const void *buf, size_t nbyte);

static SYS_FS_SHELL_RES Shell_GetRoot(const SYS_FS_SHELL_OBJ* pObj, char* rootBuff, size_t rootBuffSize);
static SYS_FS_SHELL_RES Shell_GetCwd(const SYS_FS_SHELL_OBJ* pObj, char* cwdBuff, size_t cwdBuffSize);

struct _tag_SHELL_OBJ_INSTANCE;
static SYS_FS_SHELL_RES Shell_FileAbsPath(struct _tag_SHELL_OBJ_INSTANCE *pShell, const char* fname, char* absBuff, size_t absBuffSize);

// the constant shell object functions

static const SYS_FS_SHELL_OBJ  default_shell_obj = 
{
    .fileOpen   = Shell_FileOpen,
    .fileStat   = Shell_FileStat, 
    .fileDelete = Shell_FileDelete,
    .dirOpen    = Shell_DirOpen,
    .dirMake    = Shell_DirMake,
    .fileClose  = Shell_FileClose,
    .dirClose   = Shell_DirClose,
    .fileSize   = Shell_FileSize,
    .fileSeek   = Shell_FileSeek,
    .fileTell   = Shell_FileTell,
    .fileEof    = Shell_FileEof,
    .fileRead   = Shell_FileRead,
    .fileWrite  = Shell_FileWrite,
    // 
    .cwdSet = Shell_Cwd, 
    .cwdGet = Shell_GetCwd,
    .rootGet = Shell_GetRoot,
    //
    .delete = Shell_Delete,
    .lastError = Shell_LastError,
    // 
};


// default shell object, containing the data
typedef struct _tag_SHELL_OBJ_INSTANCE
{
    SYS_FS_SHELL_OBJ  obj;
    // private data
    const struct _tag_SHELL_OBJ_INSTANCE* self;     // self reference; quick protection/check
    void (*freeFnc)(void*);                         // free function to use
    SYS_FS_SHELL_RES     opError;
    uint16_t             rootLen;                   // length of the root path
    uint16_t             cwdLen;                    // length of the cwd
#if (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
    OSAL_SEM_HANDLE_TYPE mtSem;
#endif  // (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
    uint8_t              createFlags;               // SYS_FS_SHELL_CREATE_FLAGS
    // root path; can never go higher than this
    char                root[SYS_FS_FILE_NAME_LEN + 1];  // formatted as "/srv/ftp", or "" : no trailing /
    // current working directory
    char                cwd[SYS_FS_FILE_NAME_LEN + 1];  // formatted as "/dir/", or "/", or "/dir/dir/dir/";
                                                        // leading /, trailing /
                                                        // abs path is always: (root + cwd)
    //
    // whatever other data is needed here
}SHELL_OBJ_INSTANCE;

// inlines
// protection

#if (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
static __inline__ bool  __attribute__((always_inline)) _InstanceLockCreate(OSAL_SEM_HANDLE_TYPE* pSem)
{
    // create the shell Lock
    return OSAL_SEM_Create(pSem, OSAL_SEM_TYPE_BINARY, 1, 0) == OSAL_RESULT_TRUE;
}    

static __inline__ void  __attribute__((always_inline)) _InstanceLockDelete(OSAL_SEM_HANDLE_TYPE* pSem)
{
    OSAL_SEM_Delete(pSem);
}    

// locks access to shared shell resources
static __inline__ void  __attribute__((always_inline)) _InstanceLock(SHELL_OBJ_INSTANCE* pShell)
{
    // Shared Data Lock
    OSAL_SEM_Pend(&pShell->mtSem, OSAL_WAIT_FOREVER);
}    

// unlocks access to shared shell resources
static __inline__ void  __attribute__((always_inline)) _InstanceUnlock(SHELL_OBJ_INSTANCE* pShell)
{
    // Shared Data unlock
    OSAL_SEM_Post(&pShell->mtSem);
}

#endif  // (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)

// locks a shell for usage
// all (mutable) operations should happen on a locked object
// returns the locked object if success, 0 otherwise
static SHELL_OBJ_INSTANCE* _Shell_ObjectLock(const SYS_FS_SHELL_OBJ* pObj)
{
    SHELL_OBJ_INSTANCE *pShell = (SHELL_OBJ_INSTANCE*)pObj;
    if(pShell != 0 && pShell->self == pShell)
    {
#if (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
        if((pShell->createFlags & SYS_FS_SHELL_FLAG_MTHREAD) != 0)
        {
            _InstanceLock(pShell);
        }
#endif  // (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)

        return pShell;
    } 

    return 0;
}

// unlocks the shell object
// additionally sets the last operation error is error != SYS_FS_SHELL_RES_OK
// returns the passed in error code 
static SYS_FS_SHELL_RES _Shell_ObjectUnlock(SHELL_OBJ_INSTANCE* pShell, SYS_FS_SHELL_RES error)
{
    if(error != SYS_FS_SHELL_RES_OK)
    {
        pShell->opError = error;
    }
#if (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
    if((pShell->createFlags & SYS_FS_SHELL_FLAG_MTHREAD) != 0)
    {
        _InstanceUnlock(pShell);
    }
#endif  // (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)

    return error;
}

// API
//

const SYS_FS_SHELL_OBJ* SYS_FS_Shell_Create(const char* rootDir, SYS_FS_SHELL_CREATE_FLAGS flags, void*(*shell_malloc_fnc)(size_t), void(*shell_free_fnc)(void*), SYS_FS_SHELL_RES* pRes)
{
    int rootLen;
    bool needsRoot = false;
    SYS_FS_SHELL_RES res = SYS_FS_SHELL_RES_OK;
    SHELL_OBJ_INSTANCE* newObj = 0;
#if (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
    OSAL_SEM_HANDLE_TYPE shellSem = 0;
#endif  // (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)

    while(true)
    {
        if(rootDir == 0 || (rootLen = strlen(rootDir)) == 0)
        {
            res = SYS_FS_SHELL_RES_BAD_ARG;
            break; 
        }

        if(rootDir[0] != '/')
        {   // need to add it
            rootLen++;
            needsRoot = true;
        } 

        if(rootLen > sizeof(newObj->root) - 1)
        {
            res = SYS_FS_SHELL_RES_LENGTH_ERROR;
            break;
        }

        // create the synchronization object
#if (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
        if((flags & SYS_FS_SHELL_FLAG_MTHREAD) != 0)
        { 
            if(!_InstanceLockCreate(&shellSem))
            {
                res = SYS_FS_SHELL_RES_LOCK_ERROR;
                break;
            }
        }
#endif  // (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)

        // create the object
        if(shell_malloc_fnc == 0)
        {
#if defined(SYS_FS_SHELL_MALLOC)
            shell_malloc_fnc = SYS_FS_SHELL_MALLOC;
#else
            shell_malloc_fnc = malloc;
#endif  //  defined(SYS_FS_SHELL_MALLOC)
        }

        newObj = (SHELL_OBJ_INSTANCE*)(*shell_malloc_fnc)(sizeof(*newObj));

        if(newObj == 0)
        {
            res = SYS_FS_SHELL_RES_MEM_ERROR;
            break;
        }

        // init the object
        memset(newObj, 0, sizeof(*newObj));
        newObj->obj = default_shell_obj;
        newObj->self = newObj;
        char* pRoot = newObj->root;
        if(needsRoot)
        {
            *pRoot++ = '/';
        }
        strncpy(pRoot, rootDir, sizeof(newObj->root) - 1);
        if(rootDir[rootLen - 1] == '/')
        {   // remove the ending '/'
            newObj->root[rootLen - 1] = 0;
            rootLen--;
        }
        newObj->rootLen = rootLen;
        newObj->cwd[0] = '/';
        newObj->cwdLen = 1;
#if (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
        newObj->mtSem = shellSem;
#endif  // (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
        newObj->createFlags = flags;

        if(shell_free_fnc == 0)
        {
#if defined(SYS_FS_SHELL_FREE)
            shell_free_fnc = SYS_FS_SHELL_FREE;
#else
            shell_free_fnc = free;
#endif  // defined(SYS_FS_SHELL_FREE)
        }
        newObj->freeFnc = shell_free_fnc; 

        res = SYS_FS_SHELL_RES_OK;
        break;
    }

    if(pRes)
    {
        *pRes = res;
    }

    return newObj ? &newObj->obj : 0;
}

// local object functions implementation
//

static SYS_FS_SHELL_RES Shell_Cwd(const SYS_FS_SHELL_OBJ* pObj, const char *path)
{
    SYS_FS_HANDLE fsHandle = SYS_FS_HANDLE_INVALID;
    SHELL_OBJ_INSTANCE *pShell = _Shell_ObjectLock(pObj);
    if(!pShell)
    {
        return SYS_FS_SHELL_RES_NO_OBJECT;
    }

    int len_path;
    if(path == 0 || (len_path = strlen(path)) == 0)
    {
        return _Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_BAD_ARG);
    }

    bool need_end = false;
    char path_buff[SYS_FS_FILE_NAME_LEN + 1];
    char abs_buff[SYS_FS_FILE_NAME_LEN + 1];

    if(path[len_path - 1] != '/')
    {   // we need to add this
        need_end = true;
        len_path++;
    }
    
    if(len_path > sizeof(path_buff) - 1)
    {
        return _Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_LENGTH_ERROR);
    }

    strcpy(path_buff, path);
    if(need_end)
    {
        path_buff[len_path - 1] = '/';
        path_buff[len_path] = 0;
    }


    SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, path_buff, abs_buff, sizeof(abs_buff));

    if(absRes != SYS_FS_SHELL_RES_OK)
    {   // failed
        return _Shell_ObjectUnlock(pShell, absRes);
    }
    
    // do not allow an invalid directory as the cwd
    // check if the new cwd is a valid directory.
    fsHandle = SYS_FS_DirOpen(abs_buff);
    if(fsHandle == SYS_FS_HANDLE_INVALID)
    {
        return _Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_DIR_ERROR);
    }
    // OK, close the directory
    SYS_FS_DirClose(fsHandle);        
    
    strncpy(pShell->cwd, abs_buff + pShell->rootLen, sizeof(pShell->cwd) - 1);
    pShell->cwd[sizeof(pShell->cwd) - 1] = 0;
    int len_cwd = strlen(pShell->cwd);
    char* end_cwd = pShell->cwd + len_cwd - 1;
    if(*end_cwd != '/')
    {
        *++end_cwd = '/';
        *++end_cwd = 0;
        len_cwd++;
    }
    
    pShell->cwdLen = len_cwd;
    return _Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_OK);
}

static SYS_FS_HANDLE Shell_FileOpen(const SYS_FS_SHELL_OBJ* pObj, const char *fname, SYS_FS_FILE_OPEN_ATTRIBUTES attributes)
{
    SYS_FS_HANDLE fsHandle = SYS_FS_HANDLE_INVALID;
    SHELL_OBJ_INSTANCE *pShell = _Shell_ObjectLock(pObj);
    if(pShell)
    {
        char absBuff[SYS_FS_FILE_NAME_LEN + 1];
        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, absBuff, sizeof(absBuff));
        _Shell_ObjectUnlock(pShell, absRes);

        if(absRes == SYS_FS_SHELL_RES_OK)
        {
            fsHandle = SYS_FS_FileOpen(absBuff, attributes);
        }
    }

    return fsHandle;
}

static SYS_FS_RESULT Shell_FileStat(const SYS_FS_SHELL_OBJ* pObj, const char *fname, SYS_FS_FSTAT* statBuff)
{
    SYS_FS_RESULT fsRes = SYS_FS_RES_FAILURE;
    SHELL_OBJ_INSTANCE *pShell = _Shell_ObjectLock(pObj);
    if(pShell)
    {
        char absBuff[SYS_FS_FILE_NAME_LEN + 1];
        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, absBuff, sizeof(absBuff));
        _Shell_ObjectUnlock(pShell, absRes);

        if(absRes == SYS_FS_SHELL_RES_OK)
        {
            fsRes = SYS_FS_FileStat(absBuff, statBuff);
        }
    }
    return fsRes;
}

static SYS_FS_RESULT Shell_FileDelete(const SYS_FS_SHELL_OBJ* pObj, const char *fname)
{
    SYS_FS_RESULT fsRes = SYS_FS_RES_FAILURE;
    SHELL_OBJ_INSTANCE *pShell = _Shell_ObjectLock(pObj);
    if(pShell)
    {
        char absBuff[SYS_FS_FILE_NAME_LEN + 1];

        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, absBuff, sizeof(absBuff));
        _Shell_ObjectUnlock(pShell, absRes);

        if(absRes == SYS_FS_SHELL_RES_OK)
        {
            fsRes = SYS_FS_FileDirectoryRemove(absBuff);
        }
    }
    return fsRes;
}

static SYS_FS_HANDLE Shell_DirMake(const SYS_FS_SHELL_OBJ* pObj, const char *fname)
{
    SYS_FS_HANDLE fsHandle = SYS_FS_HANDLE_INVALID;
    SHELL_OBJ_INSTANCE *pShell = _Shell_ObjectLock(pObj);
    if(pShell)
    {
        char absBuff[SYS_FS_FILE_NAME_LEN + 1];
        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, absBuff, sizeof(absBuff));
        _Shell_ObjectUnlock(pShell, absRes);

        if(absRes == SYS_FS_SHELL_RES_OK)
        {
            fsHandle = SYS_FS_DirectoryMake(absBuff);
        }
    }
    return fsHandle;
}

static SYS_FS_HANDLE Shell_DirOpen(const SYS_FS_SHELL_OBJ* pObj, const char *fname)
{
    SYS_FS_HANDLE fsHandle = SYS_FS_HANDLE_INVALID;
    SHELL_OBJ_INSTANCE *pShell = _Shell_ObjectLock(pObj);
    if(pShell)
    {
        char absBuff[SYS_FS_FILE_NAME_LEN + 1];
        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, absBuff, sizeof(absBuff));
        _Shell_ObjectUnlock(pShell, absRes);

        if(absRes == SYS_FS_SHELL_RES_OK)
        {
            fsHandle = SYS_FS_DirOpen(absBuff);
        }
    }
    return fsHandle;
}

static SYS_FS_SHELL_RES Shell_LastError(const SYS_FS_SHELL_OBJ* pObj)
{
    SYS_FS_SHELL_RES res = SYS_FS_SHELL_RES_NO_OBJECT;
    SHELL_OBJ_INSTANCE *pShell = _Shell_ObjectLock(pObj);

    if(pShell)
    {
        res = pShell->opError;
        pShell->opError = 0;
        _Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_OK);
    }
    return res;
}

static SYS_FS_SHELL_RES Shell_Delete(const SYS_FS_SHELL_OBJ* pObj)
{
    SHELL_OBJ_INSTANCE *pShell = _Shell_ObjectLock(pObj);
    if(!pShell)
    {
        return SYS_FS_SHELL_RES_NO_OBJECT;
    }
    
    void(*shell_free_fnc)(void*) = pShell->freeFnc;
#if (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
    OSAL_SEM_HANDLE_TYPE sem = pShell->mtSem;
    uint8_t flags = pShell->createFlags;
    memset(pShell, 0, sizeof(*pShell));
    if((flags & SYS_FS_SHELL_FLAG_MTHREAD) != 0)
    {
        _InstanceLockDelete(&sem);
    }
#else
    memset(pShell, 0, sizeof(*pShell));
#endif  // (_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)


    (*shell_free_fnc)(pShell);
    return SYS_FS_SHELL_RES_OK;
}

static SYS_FS_RESULT Shell_FileClose(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle)
{
    (void)pObj;
    return SYS_FS_FileClose(handle);
}

static SYS_FS_RESULT Shell_DirClose(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle)
{
    (void)pObj;
    return SYS_FS_DirClose(handle);
}

static int32_t Shell_FileSize(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle)
{
    (void)pObj;
    return SYS_FS_FileSize(handle);
}

static int32_t  Shell_FileSeek(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle, int32_t offset, SYS_FS_FILE_SEEK_CONTROL whence)
{
    (void)pObj;
    return SYS_FS_FileSeek(handle, offset, whence);
}

static int32_t  Shell_FileTell(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle)
{
    (void)pObj;
    return SYS_FS_FileTell(handle);
}

static bool Shell_FileEof(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle)
{
    (void)pObj;
    return SYS_FS_FileEOF(handle);
}

static size_t Shell_FileRead(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle, void *buf, size_t nbyte)
{
    (void)pObj;
    return SYS_FS_FileRead(handle, buf, nbyte);
}

static size_t Shell_FileWrite(const SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle, const void *buf, size_t nbyte)
{
    (void)pObj;
    return SYS_FS_FileWrite(handle, buf, nbyte);
}

static SYS_FS_SHELL_RES Shell_GetRoot(const SYS_FS_SHELL_OBJ* pObj, char* rootBuff, size_t rootBuffSize)
{
    if(rootBuff == 0 || rootBuffSize == 0)
    {
        return SYS_FS_SHELL_RES_BAD_ARG;
    }

    SHELL_OBJ_INSTANCE *pShell = _Shell_ObjectLock(pObj);
    if(!pShell)
    {
        return SYS_FS_SHELL_RES_NO_OBJECT;
    }

    
    SYS_FS_SHELL_RES res;
    if(pShell->rootLen > rootBuffSize - 1)
    {
        res = SYS_FS_SHELL_RES_LENGTH_ERROR; 
    }
    else
    {
        res = SYS_FS_SHELL_RES_OK;
    }

    strncpy(rootBuff, pShell->root, rootBuffSize - 1);
    rootBuff[rootBuffSize - 1] = 0;

    _Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_OK);
    return res;
}

static SYS_FS_SHELL_RES Shell_GetCwd(const SYS_FS_SHELL_OBJ* pObj, char* cwdBuff, size_t cwdBuffSize)
{
    if(cwdBuff == 0 || cwdBuffSize == 0)
    {
        return SYS_FS_SHELL_RES_BAD_ARG;
    }

    SHELL_OBJ_INSTANCE *pShell = _Shell_ObjectLock(pObj);
    if(!pShell)
    {
        return SYS_FS_SHELL_RES_NO_OBJECT;
    }

    SYS_FS_SHELL_RES res;
    if(pShell->cwdLen > cwdBuffSize - 1)
    {
        res = SYS_FS_SHELL_RES_LENGTH_ERROR; 
    }
    else
    {
        res = SYS_FS_SHELL_RES_OK;
    }

    strncpy(cwdBuff, pShell->cwd, cwdBuffSize - 1);
    cwdBuff[cwdBuffSize - 1] = 0;

    _Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_OK);
    return res;

}

// local helpers
// calculates the absolute path for the passed in fname into the supplied absBuff buffer
// buffSize is the total buffer size, including the \0
// returns 0 (SYS_FS_SHELL_RES_OK) if success
// otherwise an error code
static SYS_FS_SHELL_RES Shell_FileAbsPath(SHELL_OBJ_INSTANCE *pShell, const char* fname, char* absBuff, size_t absBuffSize)
{
    int f_len, cwd_len;
    char *end_cwd;

    if(fname == 0 || (f_len = strlen(fname)) == 0)
    {
        return SYS_FS_SHELL_RES_BAD_ARG;
    }

    cwd_len = pShell->cwdLen; 
    end_cwd = pShell->cwd + cwd_len;

    if(fname[0] == '.')
    {   // relative path
        if(fname[1] == '/')
        {   // OK, starting with ./ start with the current cwd
            fname += 2;
        }
        else if(fname[1] == 0)
        {   // OK, "." should still mean current directory
            fname += 1;
        }
        else if(fname[1] != '.' || fname[2] != '/')
        {
            return SYS_FS_SHELL_RES_BAD_ARG;
        }
        else
        {   // ../ access
            while(strstr(fname, "../") == fname)
            {
                fname += 3;
                int up_lev = 0;
                while(true)
                {
                    if(end_cwd <= pShell->cwd)
                    {   // cannot go up
                        return SYS_FS_SHELL_RES_LEVEL_ERROR;
                    }

                    // skip 2 /'s; the 1st one is the current level
                    if(*(end_cwd - 1) == '/')
                    {
                        if(++up_lev == 2)
                        {
                            break;
                        }
                    }
                    end_cwd--;
                }
            }
            cwd_len = end_cwd - pShell->cwd; 
        }
    }
    else
    { // ok, regular file name; just append whatever to the cwd
        // except if the full path is given
        if(fname[0] == '/')
        {   
            if((pShell->createFlags & SYS_FS_SHELL_FLAG_ABS_ROOT) != 0)
            {   // check that it's under our root so we allow access
                strcpy(absBuff, pShell->root);
                strcat(absBuff, "/");   // root doesn't have trailing /
                if(strstr(fname, absBuff) != fname)
                {
                    return SYS_FS_SHELL_RES_DIR_ERROR;
                }
                // ok, it's under our root; remove the root part
                fname += strlen(absBuff);
            }
            else
            {   // skip the shell->cwd
                cwd_len = 0;
            }
        }
    }

    // regular checks
    f_len = strlen(fname);
    if(pShell->rootLen + cwd_len + f_len > absBuffSize - 1)
    {    // too long
        return SYS_FS_SHELL_RES_LENGTH_ERROR;
    }

    // start with root
    strcpy(absBuff, pShell->root);

    if(cwd_len != 0)
    {   // add cwd
        strncpy(absBuff + pShell->rootLen, pShell->cwd, cwd_len + 1);
    }

    // add the file name
    strcpy(absBuff + pShell->rootLen + cwd_len, fname);
    absBuff[absBuffSize - 1] = 0;

#if (SYS_FS_TRAIL_SLASH_WORKAROUND != 0)
    int absLen = strlen(absBuff);
    char* pEnd = absBuff + absLen - 1;
    if(*pEnd == '/')
    {
        *pEnd = 0;
    }
#endif  // (SYS_FS_TRAIL_SLASH_WORKAROUND != 0)

    return SYS_FS_SHELL_RES_OK;
}

#if (SYS_FS_SHELL_DEBUG != 0)
SYS_FS_SHELL_RES SYS_FS_Shell_GetPath(const struct _tag_SYS_FS_SHELL_OBJ* pObj, const char *fname, char* resBuff, size_t resBuffSize)
{
    if(fname == 0 || resBuff == 0 || resBuffSize == 0)
    {
        return SYS_FS_SHELL_RES_BAD_ARG; 
    }

    SHELL_OBJ_INSTANCE *pShell = _Shell_ObjectLock(pObj);
    if(pShell)
    {
        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, resBuff, resBuffSize);
        _Shell_ObjectUnlock(pShell, absRes);

        return absRes;
    }

    return SYS_FS_SHELL_RES_NO_OBJECT;
}
#endif  // (SYS_FS_SHELL_DEBUG != 0)

