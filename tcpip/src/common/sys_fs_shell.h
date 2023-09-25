/*******************************************************************************
  Header file for Harmony File System Shell for TCPIP

  File Name:
    sys_fs_shell.h

  Summary:
    Header file for file system shell functions

  Description:
    This file system shell allows access to the SYS_FS files in a shell manner type.
    The settings for the root and current working directory of the shell are private for
    this shell so multiple users can have different roots and working directories
    without influencing themselves.
    This is useful for multiple command prompts/consoles, multiple FTP/HTTP server clients, etc.
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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








// DOM-IGNORE-END


#ifndef _SYS_FS_SHELL_H_
#define _SYS_FS_SHELL_H_

#include "system/fs/sys_fs.h"

// file shell results
typedef enum
{
    SYS_FS_SHELL_RES_OK              = 0,    // operation successful

    // errors
    SYS_FS_SHELL_RES_BAD_ARG         = -1,   // bad argument
    SYS_FS_SHELL_RES_LENGTH_ERROR    = -2,   // argument is too long / not enough room in buffer
    SYS_FS_SHELL_RES_MEM_ERROR       = -3,   // out of memory
    SYS_FS_SHELL_RES_LEVEL_ERROR     = -4,   // cannot go up so many levels
    SYS_FS_SHELL_RES_DIR_ERROR       = -5,   // no such directory exists
    SYS_FS_SHELL_RES_NO_OBJECT       = -6,   // no such shell object/invalid
    SYS_FS_SHELL_RES_LOCK_ERROR      = -7,   // error creating the multi-thread lock object
}SYS_FS_SHELL_RES;

// definition of a file shell object
typedef struct _tag_SYS_FS_SHELL_OBJ
{
    // file access routines based on the file name
    // file name access rules:
    // fname starting with:
    //      "/" :
    //          - If the shell was created with the SYS_FS_SHELL_FLAG_REL_ROOT flag (default setting) then this refers to the
    //            root of the shell
    //            See SYS_FS_Shell_Create and SYS_FS_SHELL_CREATE_FLAGS  
    //            Example: SYS_FS_Shell_Create("/srv/ftp", ...); and then access file "/filename" is OK
    //            translating to "/srv/ftp/filename"
    //
    //          - If the shell was created with SYS_FS_SHELL_FLAG_ABS_ROOT flag, then this means absolute path access;
    //            For access to be allowed, this MUST be under the root with which the object was created!
    //            See SYS_FS_Shell_Create and SYS_FS_SHELL_CREATE_FLAGS  
    //            Example: SYS_FS_Shell_Create("/srv/ftp", ...); and then access file "/srv/ftp/dir/file" is OK
    //            but file "/srv/dir/file" will fail!
    //
    //      "./" - means relative to the cwd (current working directory). 
    //              Absolute path will be: "root + cwd + name"
    //
    //      "../../dir/file" - go up n levels from the cwd and open the file
    //          Cannot go higher than the root specified in SYS_FS_Shell_Create!
    //
    //      "name" - the name refers to the cwd. Absolute path will be: "root + cwd + name"
    //
    SYS_FS_HANDLE (*fileOpen)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, const char *fname, SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

    // file status - default redirects to SYS_FS
    SYS_FS_RESULT (*fileStat)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, const char *fname, SYS_FS_FSTAT *buf);


    // file delete
    SYS_FS_RESULT (*fileDelete)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, const char *fname);

    // directory open 
    SYS_FS_HANDLE (*dirOpen)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, const char *fname);
    
    // directory make 
    SYS_FS_HANDLE (*dirMake)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, const char *fname);

    // common file/dir operations, based on a handle
    // the file shell object is not intended as a complete replacement of the SYS_FS file handle operations
    // it just implements some of the most common ones
    // by default these redirect to the SYS_FS
    // however different implementations could do this differently 

    // file close
    SYS_FS_RESULT (*fileClose)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle);

    // dir close
    SYS_FS_RESULT (*dirClose)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle);

    // file size
    int32_t       (*fileSize)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle);

    // file seek
    int32_t       (*fileSeek)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle, int32_t offset, SYS_FS_FILE_SEEK_CONTROL whence);

    // file tell
    int32_t       (*fileTell)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle);

    // file EOF
    bool          (*fileEof)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle);

    // file read/write operations
    size_t (*fileRead)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle, void *buf, size_t nbyte);
    size_t (*fileWrite)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, SYS_FS_HANDLE handle, const void *buf, size_t nbyte);

    // change the current working directory
    // cwd starting with:
    //      "/" - relative/absolute based on the value of SYS_FS_SHELL_FLAG_REL_ROOT/SYS_FS_SHELL_FLAG_ABS_ROOT flag.
    //          Always checked against the root directory
    //          New cwd = cwd parameter  
    //
    //      "./" - relative to the existent cwd
    //          New cwd = old cwd + cwd
    //
    //      "../../" - go up n levels from the existent cwd.
    //          Cannot go higher than the root specified in SYS_FS_Shell_Create!
    //
    //      "name" - refers to the existent cwd:
    //       New cwd = old cwd + name
    SYS_FS_SHELL_RES (*cwdSet)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, const char *path);

    // copies the cwd directory of the shell
    // returns SYS_FS_SHELL_RES_LENGTH_ERROR if the supplied buffer is not long enough and the cwd had to be truncated
    SYS_FS_SHELL_RES (*cwdGet)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, char* cwdBuff, size_t cwdBuffSize);

    // copies the root directory the shell was created with
    // the root directory cannot change!
    // returns SYS_FS_SHELL_RES_LENGTH_ERROR if the supplied buffer is not long enough and the root had to be truncated
    SYS_FS_SHELL_RES (*rootGet)(const struct _tag_SYS_FS_SHELL_OBJ* pObj, char* rootBuff, size_t rootBuffSize);

    // self delete
    SYS_FS_SHELL_RES (*delete)(const struct _tag_SYS_FS_SHELL_OBJ* pObj);

    // diagnostics
    SYS_FS_SHELL_RES (*lastError)(const struct _tag_SYS_FS_SHELL_OBJ* pObj);

}SYS_FS_SHELL_OBJ;

// file shell creation flags
typedef enum
{
    SYS_FS_SHELL_FLAG_NONE           = 0,       // no flag, default

    SYS_FS_SHELL_FLAG_REL_ROOT       = 0x00,    // the root is relative
                                                // i.e. a file specified like "/fname" is interpreted as
                                                // "shell_root/fname"
                                                // This is the default setting

    SYS_FS_SHELL_FLAG_ABS_ROOT       = 0x01,    // the root is absolute
                                                // i.e. a file specified like "/fname" is interpreted as "/fname"
                                                // from the absolute root of the file system
                                                //

    SYS_FS_SHELL_FLAG_MTHREAD        = 0x10,    // create the shell with multi-threaded protection
                                                // Note: this should not be needed in most usage cases
                                                // It's better to create separate shell objects in each thread
                                                // However, there are special cases where the threads need to share the same shell object
                                                // This is handled by setting this flag and synchronization objects will be created for shell access
                                                //
                                                // Note: the multi threaded protection is for the shell manipulation of current working directory
                                                // and other internal data/parameters.
                                                // It does not refer to SYS_FS access protection!
                                                // The SYS_FS protection is handled by the SYS_FS itself
                                                //
                                                // Note: there is a build time option to enable the multi-threaded option!
                                                //
    //
    //  other flags may be eventually added
    //
}SYS_FS_SHELL_CREATE_FLAGS;

// rootDir - could be something like:
//      "/" - to allow access to the whole file system
//      "/srv/ftp" - to allow access only to any directory below this path 
// flags - creation flags SYS_FS_SHELL_CREATE_FLAGS; see above
// malloc_func - standard malloc style allocation function to be used
//      if 0, the build time SYS_FS_SHELL_MALLOC symbol is used
// free_func - standard free style deallocation function to be used
//      if 0, the build time SYS_FS_SHELL_FREE symbol is used
// pRes - address to store the operation result. Could be 0 if not needed. 
const SYS_FS_SHELL_OBJ* SYS_FS_Shell_Create(const char* rootDir, SYS_FS_SHELL_CREATE_FLAGS flags, void*(*malloc_func)(size_t), void(*free_fnc)(void*), SYS_FS_SHELL_RES* pRes);


// returns a temporary pointer to the resulted file absolute path
SYS_FS_SHELL_RES SYS_FS_Shell_GetPath(const struct _tag_SYS_FS_SHELL_OBJ* pObj, const char *fname, char* resBuff, size_t resBuffSize);


#endif  /* _SYS_FS_SHELL_H_ */
