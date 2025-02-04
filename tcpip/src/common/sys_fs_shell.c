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
/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/


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
#define M_SYS_FS_SHELL_MTHREAD_PROTECTION 1
#include "osal/osal.h"
#else
#define M_SYS_FS_SHELL_MTHREAD_PROTECTION 0
#endif  // (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)


// local prototypes

static SYS_FS_HANDLE Shell_FileOpen(const SYS_FS_SHELL_OBJ* pObj, const char *fname, SYS_FS_FILE_OPEN_ATTRIBUTES attributes);
static SYS_FS_RESULT Shell_FileStat(const SYS_FS_SHELL_OBJ* pObj, const char *fname, SYS_FS_FSTAT* statBuff);
static SYS_FS_RESULT Shell_FileDelete(const SYS_FS_SHELL_OBJ* pObj, const char *fname);
static SYS_FS_HANDLE Shell_DirOpen(const SYS_FS_SHELL_OBJ* pObj, const char *fname);
static SYS_FS_RESULT Shell_DirMake(const SYS_FS_SHELL_OBJ* pObj, const char *fname);
static SYS_FS_SHELL_RES Shell_Cwd(const SYS_FS_SHELL_OBJ* pObj, const char *path);
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

struct S_tag_SHELL_OBJ_INSTANCE;
static SYS_FS_SHELL_RES Shell_FileAbsPath(struct S_tag_SHELL_OBJ_INSTANCE *pShell, const char* fname, char* absBuff, size_t absBuffSize);

// the constant shell object functions

static const SYS_FS_SHELL_OBJ  default_shell_obj = 
{
    .fileOpen   = &Shell_FileOpen,
    .fileStat   = &Shell_FileStat, 
    .fileDelete = &Shell_FileDelete,
    .dirOpen    = &Shell_DirOpen,
    .dirMake    = &Shell_DirMake,
    .fileClose  = &Shell_FileClose,
    .dirClose   = &Shell_DirClose,
    .fileSize   = &Shell_FileSize,
    .fileSeek   = &Shell_FileSeek,
    .fileTell   = &Shell_FileTell,
    .fileEof    = &Shell_FileEof,
    .fileRead   = &Shell_FileRead,
    .fileWrite  = &Shell_FileWrite,
    // 
    .cwdSet = &Shell_Cwd, 
    .cwdGet = &Shell_GetCwd,
    .rootGet = &Shell_GetRoot,
    //
    .delete = &Shell_Delete,
    .lastError = &Shell_LastError,
    // 
};


// default shell object, containing the data
typedef struct S_tag_SHELL_OBJ_INSTANCE
{
    SYS_FS_SHELL_OBJ  obj;
    // private data
    const struct S_tag_SHELL_OBJ_INSTANCE* self;     // self reference; quick protection/check
    void (*freeFnc)(void* ptr);                      // free function to use
    SYS_FS_SHELL_RES     opError;
    uint16_t             rootLen;                   // length of the root path
    uint16_t             cwdLen;                    // length of the cwd
#if (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
    OSAL_SEM_HANDLE_TYPE mtSem;
#endif  // (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
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
//

// conversion functions/helpers
//
static __inline__ int __attribute__((always_inline)) FC_PtrDiff2Int(const char* ptr1, const char* ptr2)
{
    return (int)(ptr1 - ptr2);
}

static __inline__ SHELL_OBJ_INSTANCE* __attribute__((always_inline)) FC_ShObj2ShInst(const SYS_FS_SHELL_OBJ* pShObj)
{
    union
    {
        const SYS_FS_SHELL_OBJ* pShObj;
        SHELL_OBJ_INSTANCE* pObjInst;
    }U_SH_OBJ_SH_INST;

    U_SH_OBJ_SH_INST.pShObj = pShObj;
    return U_SH_OBJ_SH_INST.pObjInst;
}

// protection

#if (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
static __inline__ bool  __attribute__((always_inline)) F_InstanceLockCreate(OSAL_SEM_HANDLE_TYPE* pSem)
{
    // create the shell Lock
    return OSAL_SEM_Create(pSem, OSAL_SEM_TYPE_BINARY, 1, 0) == OSAL_RESULT_SUCCESS;
}    

static __inline__ void  __attribute__((always_inline)) F_InstanceLockDelete(OSAL_SEM_HANDLE_TYPE* pSem)
{
    (void)OSAL_SEM_Delete(pSem);
}    

// locks access to shared shell resources
static __inline__ void  __attribute__((always_inline)) F_InstanceLock(SHELL_OBJ_INSTANCE* pShell)
{
    // Shared Data Lock
    (void)OSAL_SEM_Pend(&pShell->mtSem, OSAL_WAIT_FOREVER);
}    

// unlocks access to shared shell resources
static __inline__ void  __attribute__((always_inline)) F_InstanceUnlock(SHELL_OBJ_INSTANCE* pShell)
{
    // Shared Data unlock
    (void)OSAL_SEM_Post(&pShell->mtSem);
}

#endif  // (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)

// locks a shell for usage
// all (mutable) operations should happen on a locked object
// returns the locked object if success, 0 otherwise
static SHELL_OBJ_INSTANCE* F_Shell_ObjectLock(const SYS_FS_SHELL_OBJ* pObj)
{
    SHELL_OBJ_INSTANCE *pShell = FC_ShObj2ShInst(pObj);
    if(pShell != NULL && pShell->self == pShell)
    {
#if (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
        if((pShell->createFlags & (uint8_t)SYS_FS_SHELL_FLAG_MTHREAD) != 0U)
        {
            F_InstanceLock(pShell);
        }
#endif  // (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)

        return pShell;
    } 

    return NULL;
}

// unlocks the shell object
// additionally sets the last operation error is error != SYS_FS_SHELL_RES_OK
// returns the passed in error code 
static void F_Shell_ObjectUnlock(SHELL_OBJ_INSTANCE* pShell, SYS_FS_SHELL_RES error)
{
    if(error != SYS_FS_SHELL_RES_OK)
    {
        pShell->opError = error;
    }
#if (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
    if((pShell->createFlags & (uint8_t)SYS_FS_SHELL_FLAG_MTHREAD) != 0U)
    {
        F_InstanceUnlock(pShell);
    }
#endif  // (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)

}

// API
//

/* MISRA C-2012 Rule 21.3 deviated:2 Deviation record ID -  H3_MISRAC_2012_R_21_3_NET_DR_7 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:2 "MISRA C-2012 Rule 21.3" "H3_MISRAC_2012_R_21_3_NET_DR_7" 
const SYS_FS_SHELL_OBJ* SYS_FS_Shell_Create(const char* rootDir, SYS_FS_SHELL_CREATE_FLAGS flags, void*(*shell_malloc_fnc)(size_t nBytes), void(*shell_free_fnc)(void* ptr), SYS_FS_SHELL_RES* pRes)
{
    size_t rootLen;
    bool needsRoot = false;
    SYS_FS_SHELL_RES res = SYS_FS_SHELL_RES_OK;
    SHELL_OBJ_INSTANCE* newObj = NULL;
#if (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
    OSAL_SEM_HANDLE_TYPE shellSem = 0;
#endif  // (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
    uint8_t uFlags = (uint8_t)flags;

    while(true)
    {
        if(rootDir == NULL)
        {
            res = SYS_FS_SHELL_RES_BAD_ARG;
            break; 
        }
        else if((rootLen = strlen(rootDir)) == 0U)
        {
            res = SYS_FS_SHELL_RES_BAD_ARG;
            break; 
        }
        else
        {
            // OK
        }

        if(rootDir[0] != '/')
        {   // need to add it
            rootLen++;
            needsRoot = true;
        } 

        if(rootLen > sizeof(newObj->root) - 1U)
        {
            res = SYS_FS_SHELL_RES_LENGTH_ERROR;
            break;
        }

        // create the synchronization object
#if (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
        if((uFlags & (uint8_t)SYS_FS_SHELL_FLAG_MTHREAD) != 0U)
        { 
            if(!F_InstanceLockCreate(&shellSem))
            {
                res = SYS_FS_SHELL_RES_LOCK_ERROR;
                break;
            }
        }
#endif  // (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)

        // create the object
        if(shell_malloc_fnc == NULL)
        {
#if defined(SYS_FS_SHELL_MALLOC)
            shell_malloc_fnc = &SYS_FS_SHELL_MALLOC;
#else
            shell_malloc_fnc = &malloc;
#endif  //  defined(SYS_FS_SHELL_MALLOC)
        }

        newObj = (SHELL_OBJ_INSTANCE*)(*shell_malloc_fnc)(sizeof(*newObj));

        if(newObj == NULL)
        {
            res = SYS_FS_SHELL_RES_MEM_ERROR;
            break;
        }

        // init the object
        (void)memset(newObj, 0, sizeof(*newObj));
        newObj->obj = default_shell_obj;
        newObj->self = newObj;
        char* pRoot = newObj->root;
        if(needsRoot)
        {
            *pRoot++ = '/';
        }
        (void)strncpy(pRoot, rootDir, sizeof(newObj->root) - 1U);
        if(rootDir[rootLen - 1U] == '/')
        {   // remove the ending '/'
            newObj->root[rootLen - 1U] = '\0';
            rootLen--;
        }
        newObj->rootLen = (uint16_t)rootLen;
        newObj->cwd[0] = '/';
        newObj->cwdLen = 1;
#if (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
        newObj->mtSem = shellSem;
#endif  // (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
        newObj->createFlags = uFlags;

        if(shell_free_fnc == NULL)
        {
#if defined(SYS_FS_SHELL_FREE)
            shell_free_fnc = &SYS_FS_SHELL_FREE;
#else
            shell_free_fnc = &free;
#endif  // defined(SYS_FS_SHELL_FREE)
        }
        newObj->freeFnc = shell_free_fnc; 

        res = SYS_FS_SHELL_RES_OK;
        break;
    }

    if(pRes != NULL)
    {
        *pRes = res;
    }

    return newObj != NULL ? &newObj->obj : NULL;
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 21.3"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

// local object functions implementation
//

static SYS_FS_SHELL_RES Shell_Cwd(const SYS_FS_SHELL_OBJ* pObj, const char *path)
{
    SYS_FS_HANDLE fsHandle = SYS_FS_HANDLE_INVALID;
    SHELL_OBJ_INSTANCE *pShell = F_Shell_ObjectLock(pObj);
    if(pShell == NULL)
    {
        return SYS_FS_SHELL_RES_NO_OBJECT;
    }

    size_t len_path;
    SYS_FS_SHELL_RES shRes;
    if(path == NULL)
    {
        shRes = SYS_FS_SHELL_RES_BAD_ARG;
    }
    else if((len_path = strlen(path)) == 0U)
    {
        shRes = SYS_FS_SHELL_RES_BAD_ARG;
    }
    else
    {   // OK
        shRes = SYS_FS_SHELL_RES_OK; 
    }

    if(shRes != SYS_FS_SHELL_RES_OK)
    {   // failed
        F_Shell_ObjectUnlock(pShell, shRes);
        return shRes;
    }

    bool need_end = false;
    char path_buff[SYS_FS_FILE_NAME_LEN + 1];
    char abs_buff[SYS_FS_FILE_NAME_LEN + 1];

    if(path[len_path - 1U] != '/')
    {   // we need to add this
        need_end = true;
        len_path++;
    }
    
    if(len_path > sizeof(path_buff) - 1U)
    {
        F_Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_LENGTH_ERROR);
        return SYS_FS_SHELL_RES_LENGTH_ERROR;
    }

    (void)strcpy(path_buff, path);
    if(need_end)
    {
        path_buff[len_path - 1U] = '/';
        path_buff[len_path] = '\0';
    }


    shRes = Shell_FileAbsPath(pShell, path_buff, abs_buff, sizeof(abs_buff));

    if(shRes != SYS_FS_SHELL_RES_OK)
    {   // failed
        F_Shell_ObjectUnlock(pShell, shRes);
        return shRes;
    }
    
    // do not allow an invalid directory as the cwd
    // check if the new cwd is a valid directory.
    fsHandle = SYS_FS_DirOpen(abs_buff);
    if(fsHandle == SYS_FS_HANDLE_INVALID)
    {
        F_Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_DIR_ERROR);
        return SYS_FS_SHELL_RES_DIR_ERROR;
    }
    // OK, close the directory
    (void)SYS_FS_DirClose(fsHandle);        
    
    (void)strncpy(pShell->cwd, abs_buff + pShell->rootLen, sizeof(pShell->cwd) - 1U);
    pShell->cwd[sizeof(pShell->cwd) - 1U] = '\0';
    size_t len_cwd = strlen(pShell->cwd);
    char* end_cwd = pShell->cwd + len_cwd - 1U;
    if(*end_cwd != '/')
    {
        *++end_cwd = '/';
        *++end_cwd = '\0';
        len_cwd++;
    }
    
    pShell->cwdLen = (uint16_t)len_cwd;
    F_Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_OK);
    return SYS_FS_SHELL_RES_OK;
}

static SYS_FS_HANDLE Shell_FileOpen(const SYS_FS_SHELL_OBJ* pObj, const char *fname, SYS_FS_FILE_OPEN_ATTRIBUTES attributes)
{
    SYS_FS_HANDLE fsHandle = SYS_FS_HANDLE_INVALID;
    SHELL_OBJ_INSTANCE *pShell = F_Shell_ObjectLock(pObj);
    if(pShell != NULL)
    {
        char absBuff[SYS_FS_FILE_NAME_LEN + 1];
        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, absBuff, sizeof(absBuff));
        F_Shell_ObjectUnlock(pShell, absRes);

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
    SHELL_OBJ_INSTANCE *pShell = F_Shell_ObjectLock(pObj);
    if(pShell != NULL)
    {
        char absBuff[SYS_FS_FILE_NAME_LEN + 1];
        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, absBuff, sizeof(absBuff));
        F_Shell_ObjectUnlock(pShell, absRes);

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
    SHELL_OBJ_INSTANCE *pShell = F_Shell_ObjectLock(pObj);
    if(pShell != NULL)
    {
        char absBuff[SYS_FS_FILE_NAME_LEN + 1];

        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, absBuff, sizeof(absBuff));
        F_Shell_ObjectUnlock(pShell, absRes);

        if(absRes == SYS_FS_SHELL_RES_OK)
        {
            fsRes = SYS_FS_FileDirectoryRemove(absBuff);
        }
    }
    return fsRes;
}

static SYS_FS_RESULT Shell_DirMake(const SYS_FS_SHELL_OBJ* pObj, const char *fname)
{
    SYS_FS_RESULT fsRes = SYS_FS_RES_FAILURE;
    SHELL_OBJ_INSTANCE *pShell = F_Shell_ObjectLock(pObj);
    if(pShell != NULL)
    {
        char absBuff[SYS_FS_FILE_NAME_LEN + 1];
        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, absBuff, sizeof(absBuff));
        F_Shell_ObjectUnlock(pShell, absRes);

        if(absRes == SYS_FS_SHELL_RES_OK)
        {
            fsRes = SYS_FS_DirectoryMake(absBuff);
        }
    }
    return fsRes;
}

static SYS_FS_HANDLE Shell_DirOpen(const SYS_FS_SHELL_OBJ* pObj, const char *fname)
{
    SYS_FS_HANDLE fsHandle = SYS_FS_HANDLE_INVALID;
    SHELL_OBJ_INSTANCE *pShell = F_Shell_ObjectLock(pObj);
    if(pShell != NULL)
    {
        char absBuff[SYS_FS_FILE_NAME_LEN + 1];
        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, absBuff, sizeof(absBuff));
        F_Shell_ObjectUnlock(pShell, absRes);

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
    SHELL_OBJ_INSTANCE *pShell = F_Shell_ObjectLock(pObj);

    if(pShell != NULL)
    {
        res = pShell->opError;
        pShell->opError = SYS_FS_SHELL_RES_OK;
        F_Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_OK);
    }
    return res;
}

static SYS_FS_SHELL_RES Shell_Delete(const SYS_FS_SHELL_OBJ* pObj)
{
    SHELL_OBJ_INSTANCE *pShell = F_Shell_ObjectLock(pObj);
    if(pShell == NULL)
    {
        return SYS_FS_SHELL_RES_NO_OBJECT;
    }
    
    void(*shell_free_fnc)(void* ptr) = pShell->freeFnc;
#if (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)
    OSAL_SEM_HANDLE_TYPE sem = pShell->mtSem;
    uint8_t flags = pShell->createFlags;
    (void)memset(pShell, 0, sizeof(*pShell));
    if((flags & (uint8_t)SYS_FS_SHELL_FLAG_MTHREAD) != 0U)
    {
        F_InstanceLockDelete(&sem);
    }
#else
    (void)memset(pShell, 0, sizeof(*pShell));
#endif  // (M_SYS_FS_SHELL_MTHREAD_PROTECTION != 0)


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
    if(rootBuff == NULL || rootBuffSize == 0U)
    {
        return SYS_FS_SHELL_RES_BAD_ARG;
    }

    SHELL_OBJ_INSTANCE *pShell = F_Shell_ObjectLock(pObj);
    if(pShell == NULL)
    {
        return SYS_FS_SHELL_RES_NO_OBJECT;
    }

    
    SYS_FS_SHELL_RES res;
    if(pShell->rootLen > rootBuffSize - 1U)
    {
        res = SYS_FS_SHELL_RES_LENGTH_ERROR; 
    }
    else
    {
        res = SYS_FS_SHELL_RES_OK;
    }

    (void)strncpy(rootBuff, pShell->root, rootBuffSize - 1U);
    rootBuff[rootBuffSize - 1U] = '\0';

    F_Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_OK);
    return res;
}

static SYS_FS_SHELL_RES Shell_GetCwd(const SYS_FS_SHELL_OBJ* pObj, char* cwdBuff, size_t cwdBuffSize)
{
    if(cwdBuff == NULL || cwdBuffSize == 0U)
    {
        return SYS_FS_SHELL_RES_BAD_ARG;
    }

    SHELL_OBJ_INSTANCE *pShell = F_Shell_ObjectLock(pObj);
    if(pShell == NULL)
    {
        return SYS_FS_SHELL_RES_NO_OBJECT;
    }

    SYS_FS_SHELL_RES res;
    if(pShell->cwdLen > cwdBuffSize - 1U)
    {
        res = SYS_FS_SHELL_RES_LENGTH_ERROR; 
    }
    else
    {
        res = SYS_FS_SHELL_RES_OK;
    }

    (void)strncpy(cwdBuff, pShell->cwd, cwdBuffSize - 1U);
    cwdBuff[cwdBuffSize - 1U] = '\0';

    F_Shell_ObjectUnlock(pShell, SYS_FS_SHELL_RES_OK);
    return res;

}

// local helpers
// calculates the absolute path for the passed in fname into the supplied absBuff buffer
// buffSize is the total buffer size, including the \0
// returns 0 (SYS_FS_SHELL_RES_OK) if success
// otherwise an error code
static SYS_FS_SHELL_RES Shell_FileAbsPath(struct S_tag_SHELL_OBJ_INSTANCE *pShell, const char* fname, char* absBuff, size_t absBuffSize)
{
    size_t f_len, cwd_len;
    char *end_cwd;

    if(fname == NULL)
    {
        return SYS_FS_SHELL_RES_BAD_ARG;
    }
    else if((f_len = strlen(fname)) == 0U)
    {
        return SYS_FS_SHELL_RES_BAD_ARG;
    }
    else
    {
        // OK
    }

    cwd_len = (size_t)pShell->cwdLen; 
    end_cwd = pShell->cwd + cwd_len;

    if(fname[0] == '.')
    {   // relative path
        if(fname[1] == '/')
        {   // OK, starting with ./ start with the current cwd
            fname += 2;
        }
        else if(fname[1] == '\0')
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
                    if(FC_PtrDiff2Int(end_cwd, pShell->cwd) <= 0)
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
            cwd_len = (size_t)FC_PtrDiff2Int(end_cwd, pShell->cwd); 
        }
    }
    else
    { // ok, regular file name; just append whatever to the cwd
        // except if the full path is given
        if(fname[0] == '/')
        {   
            if((pShell->createFlags & (uint8_t)SYS_FS_SHELL_FLAG_ABS_ROOT) != 0U)
            {   // check that it's under our root so we allow access
                (void)strcpy(absBuff, pShell->root);
                (void)strcat(absBuff, "/");   // root doesn't have trailing /
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
    if(pShell->rootLen + cwd_len + f_len > absBuffSize - 1U)
    {    // too long
        return SYS_FS_SHELL_RES_LENGTH_ERROR;
    }

    // start with root
    (void)strcpy(absBuff, pShell->root);

    if(cwd_len != 0U)
    {   // add cwd
        (void)strncpy(absBuff + pShell->rootLen, pShell->cwd, cwd_len + 1U);
    }

    // add the file name
    (void)strcpy(absBuff + pShell->rootLen + cwd_len, fname);
    absBuff[absBuffSize - 1U] = '\0';

#if (SYS_FS_TRAIL_SLASH_WORKAROUND != 0)
    size_t absLen = strlen(absBuff);
    char* pEnd = absBuff + absLen - 1U;
    if(*pEnd == '/')
    {
        *pEnd = '\0';
    }
#endif  // (SYS_FS_TRAIL_SLASH_WORKAROUND != 0)

    return SYS_FS_SHELL_RES_OK;
}

SYS_FS_SHELL_RES SYS_FS_Shell_GetPath(const struct S_tag_SYS_FS_SHELL_OBJ* pObj, const char *fname, char* resBuff, size_t resBuffSize)
{
    if(fname == NULL || resBuff == NULL || resBuffSize == 0U)
    {
        return SYS_FS_SHELL_RES_BAD_ARG; 
    }

    SHELL_OBJ_INSTANCE *pShell = F_Shell_ObjectLock(pObj);
    if(pShell != NULL)
    {
        SYS_FS_SHELL_RES absRes = Shell_FileAbsPath(pShell, fname, resBuff, resBuffSize);
        F_Shell_ObjectUnlock(pShell, absRes);

        return absRes;
    }

    return SYS_FS_SHELL_RES_NO_OBJECT;
}

