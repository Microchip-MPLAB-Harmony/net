/*******************************************************************************
  Harmony File System Wrapper for TCPIP

  File Name:
    sys_fs_wrapper.c

  Summary:
    This file provides wrapping functions for Harmony SYS_FS calls (unmount,
    open and stat)

  Description:
    This file provides wrapping functions for Harmony SYS_FS calls (unmount,
    open and stat) to add the local site path to web path before passing to
    SYS_FS for processing. This is a temporary file and need to be fixed in the
    long run.
 *******************************************************************************/


// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

//Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "tcpip/src/common/sys_fs_wrapper.h"
#include "system_config.h"

// the file name will be formatted in nameBuffer and the path will be added, if needed
// buffSize is the buffer size, including the terminating \0.
static const char* SYS_FS_FileNameFormat(const char* fname, char* nameBuffer, size_t buffSize)
{

    int localLen = strlen(LOCAL_WEBSITE_PATH);
    if(buffSize - 1 < localLen)
    {   // no room to contain/append path
        return 0;
    }

    /* Check the file name to see if it already contains local website name*/
    if (strncmp(fname, LOCAL_WEBSITE_PATH, localLen) == 0)
    {
        // already contains the path
        return fname;
    }

    /*Append local path to web path*/
    memset(nameBuffer, 0, buffSize);
    strncpy(nameBuffer, LOCAL_WEBSITE_PATH, buffSize - 1);
    strncat(nameBuffer, fname, buffSize - 1 - localLen);
    return nameBuffer;

}

//******************************************************************************
/*Function:
  SYS_FS_HANDLE SYS_FS_FileOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

***************************************************************************/
SYS_FS_HANDLE SYS_FS_FileOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes)
{
    char localSitePath[SYS_FS_MAX_PATH + 1];

    const char *fnameBuf = SYS_FS_FileNameFormat(fname, localSitePath, sizeof(localSitePath));

    return (fnameBuf == 0) ? SYS_FS_HANDLE_INVALID : SYS_FS_FileOpen(fnameBuf, attributes);
}

//******************************************************************************
/*Function:
  SYS_FS_HANDLE SYS_FS_FileOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

***************************************************************************/
SYS_FS_RESULT SYS_FS_FileStat_Wrapper(const char *fname,
                                      SYS_FS_FSTAT *buf)
{
    char localSitePath[SYS_FS_MAX_PATH + 1];
    const char *fnameBuf = SYS_FS_FileNameFormat(fname, localSitePath, sizeof(localSitePath));

    return (fnameBuf == 0) ? SYS_FS_RES_FAILURE : SYS_FS_FileStat(fnameBuf, buf);
}

//******************************************************************************
/*Function:
  SYS_FS_HANDLE SYS_FS_FileOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

***************************************************************************/
SYS_FS_RESULT SYS_FS_Unmount_Wrapper(const char *fname)
{
    char localSitePath[SYS_FS_MAX_PATH + 1];
    const char *fnameBuf = SYS_FS_FileNameFormat(fname, localSitePath, sizeof(localSitePath));

    return (fnameBuf == 0) ? SYS_FS_RES_FAILURE : SYS_FS_Unmount(fnameBuf);
}

//******************************************************************************
/*Function:
  SYS_FS_HANDLE SYS_FS_DirOpen_Wrapper(const char *fname);

***************************************************************************/
SYS_FS_HANDLE SYS_FS_DirOpen_Wrapper(const char *fname)
{
    char localSitePath[SYS_FS_MAX_PATH + 1];
    const char *fnameBuf = SYS_FS_FileNameFormat(fname, localSitePath, sizeof(localSitePath));

    return (fnameBuf == 0) ? SYS_FS_HANDLE_INVALID : SYS_FS_DirOpen(fnameBuf);
}
/*******************************************************************************
 End of File
*/
