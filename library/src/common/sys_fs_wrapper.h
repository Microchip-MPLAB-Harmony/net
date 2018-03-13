/*******************************************************************************
  Header file for Harmony File System Wrapper for TCPIP

  File Name:
    sys_fs_wrapper.h

  Summary:
    Header file for file system wrapper functions

  Description:
    This file is a header.
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


#ifndef SYS_FS_WRAPPER_H
#define	SYS_FS_WRAPPER_H

#include "system/fs/src/sys_fs_local.h"

/* Enumeration for File operations*/
typedef enum
{
    /* UNMOUNT */
    FS_UNMOUNT		/*DOM-IGNORE-BEGIN*/ = (0x00u)	 /*DOM-IGNORE-END*/,

    /* OPEN */
    FS_OPEN,

    /*STAT */
    FS_STAT

} SYS_FS_WRAPPER_FUNCTIONS;

//******************************************************************************
/*Function:
  uint32_t SYS_FS_FileOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

***************************************************************************/
SYS_FS_HANDLE SYS_FS_FileOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

//******************************************************************************
/*Function:
  SYS_FS_RESULT SYS_FS_FileOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

***************************************************************************/
SYS_FS_RESULT SYS_FS_FileStat_Wrapper(const char *fname,
                                      SYS_FS_FSTAT *buf);

//******************************************************************************
/*Function:
  uint32_t SYS_FS_FileOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

***************************************************************************/
SYS_FS_RESULT SYS_FS_Unmount_Wrapper(const char *mountName);

//******************************************************************************
/*Function:
  SYS_FS_HANDLE SYS_FS_FileOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

***************************************************************************/
SYS_FS_HANDLE SYS_FS_FileNameGet_Wrapper(SYS_FS_HANDLE handle, uint8_t* cName, uint16_t wLen);

//******************************************************************************
/*Function:
  uint32_t SYS_FS_DirOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

***************************************************************************/
SYS_FS_HANDLE SYS_FS_DirOpen_Wrapper(const char *fname);

#endif	/* SYS_FS_WRAPPER_H */
