/*******************************************************************************
  HTTP Headers for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    http_net_private.h

  Summary:

  Description:
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








// DOM-IGNORE-END

#ifndef H_HTTP_NET_PRIVATE_H
#define H_HTTP_NET_PRIVATE_H


// HTTP debugging levels/masks
// basic debugging
#define HTTP_NET_DEBUG_MASK_BASIC           (0x0001)
#define HTTP_NET_DEBUG_MASK_FILE            (0x0002)
#define HTTP_NET_DEBUG_MASK_DYNVAR          (0x0004)
#define HTTP_NET_DEBUG_MASK_CHUNK_INFO      (0x0008)
#define HTTP_NET_DEBUG_MASK_CONN_STATE      (0x0010)
#define HTTP_NET_DEBUG_MASK_DYN_CONTROL     (0x0020)
#define HTTP_NET_DEBUG_MASK_SSI_HASH        (0x0040)
#define HTTP_NET_DEBUG_MASK_CONN_TMO        (0x0080)

// enable HTTP debugging levels
#define TCPIP_HTTP_NET_DEBUG_LEVEL          (0x01)


// the maximum length of a chunk header for a 32 bit chunk length:
//  n x hexDigits + CRLF
#define TCPIP_HTTP_CHUNK_HEADER_LEN         10U

// size of a chunk trailer: CRLF
#define TCPIP_HTTP_CHUNK_TRAILER_LEN        2U

// size of the final chunk trailer: 0 CRLF CRLF
#define TCPIP_HTTP_CHUNK_FINAL_TRAILER_LEN  5U

// Buffer overrun protection limit
#define TCPIP_HTTP_NET_DEFAULT_LEN        (10U)


// Mac connection timeout, seconds
#define TCPIP_HTTP_NET_CONN_MAX_TIMEOUT         32767U


/****************************************************************************
Section:
HTTP State Definitions
 ***************************************************************************/
// HTTP file type definitions
// 16 bit values
typedef enum
{
    TCPIP_HTTP_NET_FILE_TYPE_TXT = 0,       // File is a text document
    TCPIP_HTTP_NET_FILE_TYPE_HTM,           // File is HTML (extension .htm)
    TCPIP_HTTP_NET_FILE_TYPE_HTML,          // File is HTML (extension .html)
    TCPIP_HTTP_NET_FILE_TYPE_CGI,           // File is HTML (extension .cgi)
    TCPIP_HTTP_NET_FILE_TYPE_XML,           // File is XML (extension .xml)
    TCPIP_HTTP_NET_FILE_TYPE_CSS,           // File is stylesheet (extension .css)
    TCPIP_HTTP_NET_FILE_TYPE_GIF,           // File is GIF image (extension .gif)
    TCPIP_HTTP_NET_FILE_TYPE_PNG,           // File is PNG image (extension .png)
    TCPIP_HTTP_NET_FILE_TYPE_JPG,           // File is JPG image (extension .jpg)
    TCPIP_HTTP_NET_FILE_TYPE_JS,            // File is java script (extension .js)
    TCPIP_HTTP_NET_FILE_TYPE_JVM,           // File is java vm (extension .class)
    TCPIP_HTTP_NET_FILE_TYPE_JVL,           // File is java language file (extension .java)
    TCPIP_HTTP_NET_FILE_TYPE_WAV,           // File is audio (extension .wav)
    TCPIP_HTTP_NET_FILE_TYPE_UNKNOWN        // File type is unknown

} TCPIP_HTTP_NET_FILE_TYPE;


// Basic HTTP Connection State Machine
// 16 bit values
typedef enum
{
    HTTP_CONN_STATE_IDLE = 0u,                          // Connection is idle
    HTTP_CONN_STATE_PARSE_REQUEST,                      // Parses the first line for a file name and GET args
    HTTP_CONN_STATE_PARSE_FILE_UPLOAD,                  // Parses the file upload line
    HTTP_CONN_STATE_PARSE_FILE_OPEN,                    // Parses and opens the request file
    HTTP_CONN_STATE_PARSE_GET_ARGS,                     // Parses the GET arguments
    HTTP_CONN_STATE_PARSE_HEADERS,                      // Reads and parses headers one at a time
    HTTP_CONN_STATE_AUTHENTICATE,                       // Validates the current authorization state
    HTTP_CONN_STATE_PROCESS_GET,                        // Invokes user callback for GET args or cookies
    HTTP_CONN_STATE_PROCESS_POST,                       // Invokes user callback for POSTed data
    HTTP_CONN_STATE_SERVE_HEADERS,                      // Sends any required headers for the response
    HTTP_CONN_STATE_SERVE_COOKIES,                      // Adds any cookies to the response
    HTTP_CONN_STATE_SERVE_BODY_INIT,                    // body intialization
    HTTP_CONN_STATE_SERVE_BODY,                         // Serves the actual content
    HTTP_CONN_STATE_SERVE_CHUNKS,                       // chunk processing
    HTTP_CONN_STATE_DONE,                               // job done: closes all files and waits for new connections
    HTTP_CONN_STATE_ERROR,                              // Some error occurred. Disconnec the server and closes all files
    HTTP_CONN_STATE_DISCONNECT                          // Non persistent connection, end of processing
                                                              // Disconnects the server and closes all files
} TCPIP_HTTP_NET_CONN_STATE;


// 8 bit values only
typedef enum
{
    TCPIP_HTTP_DYNVAR_BUFF_FLAG_ACK     = 0x01,       // needs buffer acknowledge



}TCPIP_HTTP_DYNVAR_BUFF_FLAGS;


typedef struct S_tag_TCPIP_HTTP_DYNVAR_BUFF_DCPT
{
    // fixed fields
    struct S_tag_TCPIP_HTTP_DYNVAR_BUFF_DCPT*    next;   // valid single list node
    const void* dynBuffer;                              // persistent dynamic variable buffer
                                                        // this buffer will be printed as part of the 
                                                        // dynamic variable callback procedure
    uint16_t    dynBufferSize;                          // size of this buffer
    uint16_t    writeOffset;                            // current write offset: 0 -> dynBufferSize
    uint8_t     dynFlags;                               // flags: TCPIP_HTTP_DYNVAR_BUFF_FLAGS value
    uint8_t     padding[3];                             // not used                                                        
}TCPIP_HTTP_DYNVAR_BUFF_DCPT;

typedef struct
{
    // fixed fields
    void*       next;              // valid single list node
    uint16_t    fileBufferSize;    // size of the chunk buffer: without header/trailer, just the active data: TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE
    uint16_t    fileBufferTotSize;  // TCPIP_HTTP_CHUNK_HEADER_LEN + TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE + TCPIP_HTTP_CHUNK_FINAL_TRAILER_LEN + 1   
    // char        fileBuffer[];       // buffer that's used for storage when a dynamic/binary file is output
                                    // NOTE: this buffer is also used to store the TCPIP_HTTP_DYN_VAR_DCPT and TCPIP_HTTP_DYN_ARG_DCPT[] 
                                    // when a dynamic variable is passed to the user!
}TCPIP_HTTP_FILE_BUFF_DCPT_BARE;

typedef struct S_tag_TCPIP_HTTP_FILE_BUFF_DCPT
{
    // fixed fields
    struct S_tag_TCPIP_HTTP_FILE_BUFF_DCPT*    next;    // valid single list node
    uint16_t    fileBufferSize;    // size of the chunk buffer: without header/trailer, just the active data: TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE
    uint16_t    fileBufferTotSize;  // TCPIP_HTTP_CHUNK_HEADER_LEN + TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE + TCPIP_HTTP_CHUNK_FINAL_TRAILER_LEN + 1   
    char        fileBuffer[4];      // buffer that's used for storage when a dynamic/binary file is output
                                    // NOTE: this buffer is also used to store the TCPIP_HTTP_DYN_VAR_DCPT and TCPIP_HTTP_DYN_ARG_DCPT[] 
                                    // when a dynamic variable is passed to the user!
}TCPIP_HTTP_FILE_BUFF_DCPT;


// 16 bits only
typedef enum
{
    // chunk flags
    HTTP_CHUNK_FLAG_NONE                      = 0x0000,   // invalid
    HTTP_CHUNK_FLAG_BEG_CHUNK                 = 0x0001,   // beginning of a chunk
    HTTP_CHUNK_FLAG_END_CHUNK                 = 0x0002,   // end of a chunk
                                                                // if neither beg/end are set, it is an intermediary chunk
    HTTP_CHUNK_FLAG_OUT_DATA                  = 0x0004,   // chunk has output data (either file or dynVar)
    HTTP_CHUNK_FLAG_TYPE_FILE                 = 0x0008,   // file chunk/else dyn var chunk
    HTTP_CHUNK_FLAG_TYPE_DATA                 = 0x0000,   // dyn var chunk
    HTTP_CHUNK_FLAG_TYPE_DATA_SSI             = 0x0010,   // dynamically interpreted chunk contains SSI code, else regular dynamic variables
    HTTP_CHUNK_FLAG_TYPE_FILE_ROOT            = 0x0020,   // root file, beginning the list of chunks
    HTTP_CHUNK_FLAG_TYPE_FILE_DYN             = 0x0040,   // file contains dynamic variables to be interpreted, else plain/bin
    HTTP_CHUNK_FLAG_TYPE_FILE_SKIP            = 0x0080,   // dynamic file currently skipping dynamic variables evaluation
    HTTP_CHUNK_FLAG_TYPE_FILE_ERROR           = 0x0100,   // error occurred while processing the file
    HTTP_CHUNK_FLAG_TYPE_FILE_PARSE_ERROR     = 0x0200,   // error occurred while parsing the file

    // dyn variable chunk specific flags
    HTTP_CHUNK_FLAG_DYNVAR_VALID              = 0x1000,   // the descriptor is valid, the dynamic variable parameters are updated 
    HTTP_CHUNK_FLAG_DYNVAR_DEFAULT_PROCESS    = 0x2000,   // the dynamic variable needs to be processed internally (the default processing)
    HTTP_CHUNK_FLAG_DYNVAR_AGAIN              = 0x4000,   // the dynamicPrint function needs to be called again 

    /* all dyn variable flags mask */
    HTTP_CHUNK_FLAG_DYNVAR_MASK               = (HTTP_CHUNK_FLAG_DYNVAR_VALID | HTTP_CHUNK_FLAG_DYNVAR_DEFAULT_PROCESS | HTTP_CHUNK_FLAG_DYNVAR_AGAIN ),


}TCPIP_HTTP_CHUNK_FLAGS;

typedef enum
{
    // OK codes
    HTTP_CHUNK_RES_OK             = 0,    // chunk processing OK, continue
    HTTP_CHUNK_RES_DONE,                  // processing done

    // wait codes
    HTTP_CHUNK_RES_WAIT,                  // waiting for a resource and a retry/break is needed
                                                // could also mean that could not allocate a chunk, pool is empty
    // errors
    HTTP_CHUNK_RES_DEPTH_ERR      = -1,   // max depth exceeded 
    HTTP_CHUNK_RES_FILE_ERR       = -2,   // invalid file in the chunk 
    HTTP_CHUNK_RES_DYNVAR_ERR     = -3,   // dynamic variable parsing failed 
    HTTP_CHUNK_RES_SSI_ERR        = -4,   // SSI parsing failed 
    HTTP_CHUNK_RES_RETRY_ERR      = -5,   // maximum retries number exceeded 
    HTTP_CHUNK_RES_SSI_ATTRIB_ERR = -6,   // SSI attribute error 
    HTTP_CHUNK_RES_SSI_CACHE_FAIL = -7,   // SSI cache failure 
    HTTP_CHUNK_RES_SSI_CACHE_FULL = -8,   // SSI variable name cache full 
    HTTP_CHUNK_RES_SSI_ALLOC_ERR  = -9,   // SSI allocation failed


    HTTP_CHUNK_RES_INTERNAL_ERR    = -10,  // internal error, shouldn't happen

}TCPIP_HTTP_CHUNK_RES;


// processing phases for a dynamic variable chunk
// 8 bit values
typedef enum
{
    TCPIP_HTTP_DYNVAR_PHASE_START       = 0,        // the chunk is starting: chunk size is calculated
    TCPIP_HTTP_DYNVAR_PHASE_DATA,                   // the chunk is spitting out data
    TCPIP_HTTP_DYNVAR_PHASE_END,                    // the end of chunk is being output
    TCPIP_HTTP_DYNVAR_PHASE_DONE,                   // the chunk is done
}TCPIP_HTTP_DYNVAR_PROCESS_PHASE;


typedef struct
{
    const char*     keyWord;    // variable keyword
    uint16_t        keyFlags;   // keyword flags: TCPIP_HTTP_CHUNK_FLAGS
    uint16_t        padding;
    // keyword default processing function
    TCPIP_HTTP_DYN_PRINT_RES (*dynamicPrint)(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const struct S_tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);
}TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY;

// structure to be allocated for a TCPIP_HTTP_DYN_VAR_DCPT that's passed to the user
// contains the dynVar parameters
typedef struct
{
    TCPIP_HTTP_DYN_VAR_DCPT         dynDcpt;        // this is what's passed to the user
                                                    // also stores the dynamic variable context across multiple calls

    // TCPIP_HTTP_DYN_ARG_DCPT         dynArgs[];      // storing the dynamic variable argument descriptors  
                                                    // strings are stored in the fileBuffer
}TCPIP_HTTP_DYNVAR_ALLOC_DCPT_BARE;   

typedef struct
{
    TCPIP_HTTP_DYN_VAR_DCPT         dynDcpt;        // this is what's passed to the user
                                                    // also stores the dynamic variable context across multiple calls

    TCPIP_HTTP_DYN_ARG_DCPT         dynArgs[1];     // storing the dynamic variable argument descriptors  
                                                    // strings are stored in the fileBuffer
}TCPIP_HTTP_DYNVAR_ALLOC_DCPT;   

// dynamic variables processing: chunk descriptor
typedef struct
{
    TCPIP_HTTP_DYNVAR_ALLOC_DCPT*   pDynAllocDcpt;   // this contains what's passed to the user
                                                     // also stores the dynamic variable context across multiple calls

    const TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY* pKEntry;// the keyword (if any) it corresponds to
    SINGLE_LIST                     dynBuffList;    // current list of dynamic variable descriptors: TCPIP_HTTP_DYNVAR_BUFF_DCPT 
    struct S_tag_TCPIP_HTTP_NET_CONN* pOwnerCon;     // connection that owns this dynamic chunk
    uint16_t                        dynRetries;     // current number of retries
    uint16_t                        padding;        // unused
}TCPIP_HTTP_DYNVAR_CHUNK_DCPT;   
    

#if (TCPIP_HTTP_NET_SSI_PROCESS != 0)
struct S_tag_TCPIP_HTTP_CHUNK_DCPT;
struct S_tag_TCPIP_HTTP_NET_CONN;
// SSI command process function
// returns:
//      HTTP_CHUNK_RES_OK - if a new chunk is spawned and needs to be executed
//      HTTP_CHUNK_RES_WAIT - waiting for resources needed
//      HTTP_CHUNK_RES_DONE - processing round completed
//      < 0 some error when executing this SSI attribute
//
// function is required to update the pChDcpt->nCurrAttrib!
typedef TCPIP_HTTP_CHUNK_RES (*TCPIP_SSI_COMMAND_FNC)(struct S_tag_TCPIP_HTTP_NET_CONN* pHttpCon, struct S_tag_TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, size_t leftAttribs);

typedef struct
{
    const char*             ssiCmd;         // corresponding SSI command
    TCPIP_SSI_COMMAND_FNC   ssiFnc;         // SSI processing function
}TCPIP_HTTP_SSI_PROCESS_ENTRY;

// SSI cache entry for supporting SSI variables
typedef struct
{
    OA_HASH_ENTRY           hEntry;         // hash header;
    int32_t                 valInt;                                                     // integer variable value
    uint8_t                 varType;        // string/int/etc. TCPIP_HTTP_DYN_ARG_TYPE value
    char                    varName[TCPIP_HTTP_NET_SSI_VARIABLE_NAME_MAX_LENGTH + 1];   // the hash key: the variable name
    char                    varStr[TCPIP_HTTP_NET_SSI_VARIABLE_STRING_MAX_LENGTH + 1];  // string variable value
                                                                                        // if variable is int, then it is the string representation
                                                                                                    
}TCPIP_HTTP_SSI_HASH_ENTRY;

// hash probing step 
#define TCPIP_HTTP_SSI_HASH_PROBE_STEP     1U


// SSI chunk

// not less than 2 attributes per SSI command
// and always an even number for commands like "set" that need
// to work on 2 attributes at a time
#define M_TCPIP_HTTP_NET_SSI_STATIC_MIN_ATTTRIB_NUMBER 2
#if (TCPIP_HTTP_NET_SSI_STATIC_ATTTRIB_NUMBER > M_TCPIP_HTTP_NET_SSI_STATIC_MIN_ATTTRIB_NUMBER)
#define M_TCPIP_HTTP_NET_SSI_STATIC_ATTTRIB_NUMBER  TCPIP_HTTP_NET_SSI_STATIC_ATTTRIB_NUMBER
#else
#define M_TCPIP_HTTP_NET_SSI_STATIC_ATTTRIB_NUMBER  M_TCPIP_HTTP_NET_SSI_STATIC_MIN_ATTTRIB_NUMBER
#endif

typedef struct 
{
    const char*             ssiCmd;             // the SSI command: include, exec, etc.
    const char*             fileName;           // ASCII string storing the file name 
                                                // the SSI command belongs to
    TCPIP_SSI_COMMAND_FNC   ssiFnc;             // SSI processing function                                                
    uint16_t                nStaticAttribs;     // number of static attributes in this command
    uint16_t                nAllocAttribs;      // number of allocated attributes in pAllocAttrib
    TCPIP_HTTP_SSI_ATTR_DCPT staticAttribs[M_TCPIP_HTTP_NET_SSI_STATIC_ATTTRIB_NUMBER];      // array of static SSI attribute descriptors 
    TCPIP_HTTP_SSI_ATTR_DCPT* pAllocAttribs;    // array of extra allocated SSI attribute descriptors 
    volatile uint16_t       nCurrAttrib;        // currently processing attribute
                                                // Note: this is updated by the processing function!
                                                // Some process attrib pairs one by one, other multiple at a time.
    uint16_t                padding;            // not used
}TCPIP_HTTP_SSI_CHUNK_DCPT;
#endif // (TCPIP_HTTP_NET_SSI_PROCESS != 0)


// 8 bit values
typedef enum
{
    TCPIP_HTTP_CHUNK_STATE_BEG          = 0,        // chunk is begining
    TCPIP_HTTP_CHUNK_STATE_DATA,                    // chunk is sending data
    TCPIP_HTTP_CHUNK_STATE_END,                     // chunk is finishing its data
    TCPIP_HTTP_CHUNK_STATE_DONE,                    // chunk is done
}TCPIP_HTTP_CHUNK_STATE;

typedef enum
{
    TCPIP_HTTP_CHUNK_END_NONE       = 0,        // invalid
    TCPIP_HTTP_CHUNK_END_CURRENT    = 0x01,     // signals the end of the current chunk
    TCPIP_HTTP_CHUNK_END_FINAL      = 0x02,     // signals the end of all chunks
    TCPIP_HTTP_CHUNK_END_ALL        = 0x03,     // signals the end of both current and all chunks
}TCPIP_HTTP_CHUNK_END_TYPE;


// descriptor of a file chunk
typedef struct
{
    SYS_FS_HANDLE           fHandle;    // file handle
    size_t                  fSize;      // size of the file
    size_t                  fOffset;    // current file offset: read pointer
    char*                   dynStart;   // pointer in the current line buffer where dynamic variable processing starts
    TCPIP_HTTP_FILE_BUFF_DCPT* fileBuffDcpt;    // associated file buffer descriptor
    uint16_t                fDynCount;  // current dynamic variable count in this file
    uint16_t                chunkOffset;// current chunk offset: read pointer    
    uint16_t                chunkEnd;   // end pointer of data in the chunk buffer
    uint16_t                padding;    // unused
}TCPIP_HTTP_FILE_CHUNK_DCPT;


typedef struct S_tag_TCPIP_HTTP_CHUNK_DCPT
{
    struct S_tag_TCPIP_HTTP_CHUNK_DCPT*  next;       // valid single list node
    struct S_tag_TCPIP_HTTP_CHUNK_DCPT*  pRootDcpt;  // root (file) descriptor it comes from 
    uint16_t                flags;                  // TCPIP_HTTP_CHUNK_FLAGS value
    uint8_t                 status;                 // TCPIP_HTTP_CHUNK_STATE value
    uint8_t                 phase;                  // TCPIP_HTTP_DYNVAR_PROCESS_PHASE value: current processing phase
    union
    {   
        TCPIP_HTTP_FILE_CHUNK_DCPT      fileChDcpt; // file descriptor, if the chunk is a file type chunk
        TCPIP_HTTP_DYNVAR_CHUNK_DCPT    dynChDcpt;  // dynVar descriptor: if the chunk is a dyn var chunk
#if (TCPIP_HTTP_NET_SSI_PROCESS != 0)
        TCPIP_HTTP_SSI_CHUNK_DCPT       ssiChDcpt;  // SSI descriptor: if chunk is a SSI data chunk 
#endif // (TCPIP_HTTP_NET_SSI_PROCESS != 0)
    };
    char                    chunkFName[TCPIP_HTTP_NET_FILENAME_MAX_LEN + 1U];  // truncated file name: either the file that's processed
                                                                                // or the file that contained the dynVar
}TCPIP_HTTP_CHUNK_DCPT;


typedef union
{
    uint16_t    val;
    struct
    {
        unsigned int    procPhase:      3;         // simple phase counter for processing functions
        unsigned int    requestError:   1;         // an eror occurred while processing the requests (invalid file, buffer overflow, etc.)
                                                   // the header parsing will just remove from the socket buffer, don't process
        unsigned int    discardRxBuff:  1;         // socket RX buffer needs to be discarded before listening to a new request
                                                   // set because of an error
        unsigned int    uploadMemError: 1;         // an out of memory occurred during an upload operation
        unsigned int    sktLocalReset:  1;         // socket reset/disconnect was initiated locally 
        unsigned int    sktIsConnected: 1;         // socket connect persistent flag 
        unsigned int    uploadPhase:    1;         // the upload procedure phase: is waiting for the signature
        unsigned int    reserved:       7;         // not used
    };
}TCPIP_HTTP_NET_CONN_FLAGS;


// Stores extended state data for each connection
typedef struct S_tag_TCPIP_HTTP_NET_CONN
{
    uint32_t                    byteCount;                      // How many bytes have been read so far
    uint32_t                    httpTick;                       // timeout counter
    uint32_t                    callbackPos;                    // Callback position indicator
    uint8_t*                    ptrData;                        // Points to first free byte in data
    uint8_t*                    ptrRead;                        // Points to current read location
    SYS_FS_HANDLE               file;                           // File pointer for the file being served; 
    uint8_t*                    httpData;                       // General purpose data buffer
    const void*                 userData;                       // user supplied data; not used by the HTTP module
    NET_PRES_SIGNAL_HANDLE      socketSignal;                   // socket signal handler
#if defined(TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE)
    uint8_t*                    uploadBufferStart;              // buffer used for the fs upload operation
    uint8_t*                    uploadBufferEnd;                // end of buffer used for the fs upload operation
    uint8_t*                    uploadBufferCurr;               // current pointer
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE uploadBuffHandle;         // current SYS_MEDIA write buffer handle
#endif  // defined(TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE)
    SINGLE_LIST                 chunkList;                      // current list of chunk jobs: TCPIP_HTTP_CHUNK_DCPT
    // manually aligned members
    uint16_t                    httpStatus;                     // TCPIP_HTTP_NET_STATUS: Request method/status
    uint16_t                    connState;                      // TCPIP_HTTP_NET_CONN_STATE: Current connection state
    uint16_t                    fileType;                       // TCPIP_HTTP_NET_FILE_TYPE: File type to return with Content-Type
    NET_PRES_SKT_HANDLE_T       netSocket;                      // Socket being served
    uint16_t                    connIx;                         // index of this connection in the HTTP server
    uint16_t                    uploadSectNo;                   // current sector number for upload
    uint16_t                    smPost;                         // POST state machine variable  
    uint16_t                    listenPort;                     // server listening port
    uint8_t                     hasArgs;                        // True if there were get or cookie arguments
    uint8_t                     isAuthorized;                   // 0x00-0x79 on fail, 0x80-0xff on pass
    uint16_t                    chunkPoolEmpty;                 // counter for dynamic chunks empty condition
    uint16_t                    fileBufferPoolEmpty;            // counter for file buffer empty condition
    uint16_t                    chunkPoolRetry;                 // retry counter for chunk pool empty condition
    uint16_t                    fileBufferRetry;                // retry counter for file buffer empty condition
    uint16_t                    connActiveSec;                  // last second the connection was active                  
    TCPIP_HTTP_NET_CONN_FLAGS   flags;                          // connection flags
    uint16_t                    closeEvent;                     // the event for the reported connection close
    // 
    char                        fileName[SYS_FS_FILE_NAME_LEN + 1];  // file name storage

} TCPIP_HTTP_NET_CONN;


#endif // H_HTTP_NET_PRIVATE_H



