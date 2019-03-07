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
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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








// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
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

//******************************************************************************
/*Function:
  SYS_FS_HANDLE SYS_FS_FileDelete_Wrapper(const char *fname);

***************************************************************************/
SYS_FS_RESULT SYS_FS_FileDelete_Wrapper(const char *fname)
{
    char localSitePath[SYS_FS_MAX_PATH + 1];

    const char *fnameBuf = SYS_FS_FileNameFormat(fname, localSitePath, sizeof(localSitePath));

    return (fnameBuf == 0) ? SYS_FS_HANDLE_INVALID : SYS_FS_FileDirectoryRemove(fnameBuf);
}
/*******************************************************************************
 End of File
*/
