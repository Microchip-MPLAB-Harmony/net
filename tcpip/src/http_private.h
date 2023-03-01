/*******************************************************************************
  HTTP Headers for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    http_private.h

  Summary:

  Description:
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

#ifndef __HTTP_PRIVATE_H
#define __HTTP_PRIVATE_H


/****************************************************************************
Section:
HTTP State Definitions
 ***************************************************************************/

// Basic HTTP Connection State Machine
typedef enum
{
    SM_HTTP_IDLE = 0u,                          // Socket is idle
    SM_HTTP_PARSE_REQUEST,                      // Parses the first line for a file name and GET args
    SM_HTTP_PARSE_HEADERS,                      // Reads and parses headers one at a time
    SM_HTTP_AUTHENTICATE,                       // Validates the current authorization state
    SM_HTTP_PROCESS_GET,                        // Invokes user callback for GET args or cookies
    SM_HTTP_PROCESS_POST,                       // Invokes user callback for POSTed data
    SM_HTTP_PROCESS_REQUEST,                    // Begins the process of returning data
    SM_HTTP_SERVE_HEADERS,                      // Sends any required headers for the response
    SM_HTTP_SERVE_COOKIES,                      // Adds any cookies to the response
    SM_HTTP_SERVE_BODY,                         // Serves the actual content
    //SM_HTTP_SEND_FROM_CALLBACK,               // Invokes a dynamic variable callback
    SM_HTTP_DISCONNECT                          // Disconnects the server and closes all files
} SM_HTTP2;


typedef enum
{
    SM_IDLE = 0u,
    SM_GET_NO_OF_FILES,
    SM_GET_HASH_RCRD,
    SM_COMPARE_HASH_RCRD,
    SM_GET_DYN_VAR_FILE_RCRD,
    SM_PARSE_TILL_DYN_VAR,
    SM_PARSE_DYN_VAR_STRING,
    SM_PROCESS_DYN_VAR_CALLBACK,
    SM_SERVE_TEXT_DATA,
} SM_FILETX;


typedef struct
{
    // TOP level file control parameters
    uint32_t    dynVarRcrdOffset;               // Dynamic variable offset in web page
    uint32_t    dynVarCallBackID;               // Call back ID
    uint32_t    dynVarCntr;                     // Number of dynamic variable
    uint32_t    bytesReadCount;                 // Counter of current file already reading
    uint32_t    incFileRdCnt;                   // Position of current including file
    size_t      numBytes;                       // Number of bytes of the current file
    size_t      numBytesHdrFile;                // Number of bytes of included header file
    uint16_t    DynRcrdRdCount;                 // Dynamic variable file, read position
    int8_t      lock_dynrcd;                    // Flag - If first read on this record
    int8_t      nameHashMatched;                // Name hash match flag
    // Including file or variable file
    uint8_t     EndOfCallBackFileFlag;          // Flag - if current call back service finished
    uint8_t     lock_hdr;                       // Flag - If first read of current header file
    uint8_t     fileTxDone; 
    uint8_t     padding;                        // padding field to have structure multiple of 32 bits 
} FILE_CTRL;

// Stores extended state data for each connection
typedef struct
{
    uint32_t        byteCount;                      // How many bytes have been read so far
    uint32_t        nextCallback;                   // Byte index of the next callback
    union
    {
        uint32_t    httpTick;                       // watchdog timer
        uint32_t    callbackID;                     // Callback ID to execute
    };
    uint32_t        callbackPos;                    // Callback position indicator
    uint8_t*        ptrData;                        // Points to first free byte in data
    uint8_t*        ptrRead;                        // Points to current read location
    SYS_FS_HANDLE   file;                           // File pointer for the file being served
    FILE_CTRL       TxFile;                         // Current sending file stub
    HTTP_STATUS     httpStatus;                     // Request method/status
    HTTP_FILE_TYPE  fileType;                       // File type to return with Content-Type
    SM_HTTP2        sm;                             // Current connection state
    SM_FILETX       file_sm;                        // Current file sending state
    uint8_t*        data;                           // General purpose data buffer
    uint16_t        nameHash;                       // Current file name hash
    uint16_t        connIx;                         // index of this connection in the HTTP server
    const void*     userData;                       // user supplied data; not used by the HTTP module
    uint8_t         hasArgs;                        // True if there were get or cookie arguments
    uint8_t         isAuthorized;                   // 0x00-0x79 on fail, 0x80-0xff on pass
    uint16_t        smPost;                         // POST state machine variable  

    TCP_SOCKET      socket;                         // Socket being served
    uint16_t        uploadSectNo;                   // current sector number for upload
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    uint8_t*        uploadBufferStart;              // buffer used for the mpfs upload operation
    uint8_t*        uploadBufferEnd;                // end of buffer used for the mpfs upload operation
    uint8_t*        uploadBufferCurr;               // current pointer
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE uploadBuffHandle; // current SYS_MEDIA write buffer handle
#endif  // defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    
    TCPIP_TCP_SIGNAL_HANDLE socketSignal;           // socket signal handler
    char            fileName[SYS_FS_FILE_NAME_LEN + 1]; // file name storage

} HTTP_CONN;


#endif // __HTTP_PRIVATE_H



