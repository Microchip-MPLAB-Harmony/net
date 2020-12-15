/*******************************************************************************
  HTTP Headers for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    http_net.h

  Summary:
    The HTTP web server module together with a file system (SYS_FS) allow
    the board to act as a web server.
    This module uses the network presentation layer to allow for encrypted 
    connections.
    
  Description:
    The HTTP module runs a web server within the TCP/IP stack.
    This facilitates an easy method to view status information
    and control applications using any standard web browser.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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








//DOM-IGNORE-END

#ifndef __HTTP_NET_H_
#define __HTTP_NET_H_

#include "system/fs/sys_fs.h"
#include "net_pres/pres/net_pres.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Commands and Server Responses
// *****************************************************************************
// *****************************************************************************

//******************************************************************************

//Supported Commands and Server Response Codes
typedef enum
{
    TCPIP_HTTP_NET_STAT_GET = 0u,               // GET command is being processed
    TCPIP_HTTP_NET_STAT_POST,                   // POST command is being processed
    TCPIP_HTTP_NET_STAT_BAD_REQUEST,            // 400 Bad Request will be returned
    TCPIP_HTTP_NET_STAT_UNAUTHORIZED,           // 401 Unauthorized will be returned
    TCPIP_HTTP_NET_STAT_NOT_FOUND,              // 404 Not Found will be returned
    TCPIP_HTTP_NET_STAT_OVERFLOW,               // 414 Request-URI Too Long will be returned
    TCPIP_HTTP_NET_STAT_INTERNAL_SERVER_ERROR,  // 500 Internal Server Error will be returned
    TCPIP_HTTP_NET_STAT_NOT_IMPLEMENTED,        // 501 Not Implemented (not a GET or POST command)
    TCPIP_HTTP_NET_STAT_REDIRECT,               // 302 Redirect will be returned
    TCPIP_HTTP_NET_STAT_TLS_REQUIRED,           // 403 Forbidden is returned, indicating 
                                                // TLS is required
    TCPIP_HTTP_NET_STAT_UPLOAD_FORM,            // Show the Upload form
    TCPIP_HTTP_NET_STAT_UPLOAD_STARTED,         // An upload operation is being processed
    TCPIP_HTTP_NET_STAT_UPLOAD_WRITE,           // An upload operation is currently writing
    TCPIP_HTTP_NET_STAT_UPLOAD_WRITE_WAIT,      // An upload operation is currently waiting for the write completion
    TCPIP_HTTP_NET_STAT_UPLOAD_OK,              // An Upload was successful
    TCPIP_HTTP_NET_STAT_UPLOAD_ERROR,           // An Upload was not a valid image

} TCPIP_HTTP_NET_STATUS;

// *****************************************************************************
// *****************************************************************************
// Section: HTTP Definitions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************

// Result states for execution callbacks
typedef enum
{
    TCPIP_HTTP_NET_IO_RES_DONE = 0u,  // Finished with procedure
    TCPIP_HTTP_NET_IO_RES_NEED_DATA,  // More data needed to continue, call again later
    TCPIP_HTTP_NET_IO_RES_WAITING,    // Waiting for asynchronous process to complete, 
                                      // call again later
    TCPIP_HTTP_NET_IO_RES_ERROR,      // Some error has occurred, operation will be aborted

} TCPIP_HTTP_NET_IO_RESULT;

// Result states for  TCPIP_HTTP_NET_ConnectionPostNameRead, TCPIP_HTTP_NET_ConnectionPostValueRead 
// and TCPIP_HTTP_NET_ConnectionPostReadPair

typedef enum
{
    TCPIP_HTTP_NET_READ_OK = 0,       // Read was successful
    TCPIP_HTTP_NET_READ_TRUNCATED,    // Buffer overflow prevented by truncating value
    TCPIP_HTTP_NET_READ_INCOMPLETE    // Entire object is not yet in the buffer.  
                                      // Try again later.

} TCPIP_HTTP_NET_READ_STATUS;


// *****************************************************************************
/*
  Enumeration:
    TCPIP_HTTP_NET_EVENT_TYPE

  Summary:
    HTTP reported run-time events.

  Description:
    This enumeration defines the types of the HTTP reported events
    when processing dynamic variables, files, etc.
   
  Remarks:
    Multiple warnings can be set simultaneously.

*/
typedef enum
{
    /* no event */
    TCPIP_HTTP_NET_EVENT_NONE                           = 0,

    /* Notification that a FS upload operation was completed successfully */
    TCPIP_HTTP_NET_EVENT_FS_UPLOAD_COMPLETE,

    /* An attempt to open the connection was made, data received.
     * Subject to parsing and other validation, it may result into a valid connection */
    TCPIP_HTTP_NET_EVENT_OPEN,    

    /* The non-persistent connection was done and closed */
    TCPIP_HTTP_NET_EVENT_CLOSE_DONE,    

    /* The connection was closed due to a client timeout */
    TCPIP_HTTP_NET_EVENT_CLOSE_TIMEOUT,    

    /* The connection was closed due to a POST processing error */
    TCPIP_HTTP_NET_EVENT_CLOSE_POST_ERROR,    

    /* The connection was closed by the client */
    TCPIP_HTTP_NET_EVENT_CLOSE_REMOTE,    

    /* a HTTP file was successfully opened */
    TCPIP_HTTP_NET_EVENT_FILE_OPEN,

    /* a HTTP file was closed */
    TCPIP_HTTP_NET_EVENT_FILE_CLOSE,


    /* errors */
    /* an error occurred when opening a HTTP file */
    TCPIP_HTTP_NET_EVENT_FILE_OPEN_ERROR                = -1,

    /* a file name was not specified */
    TCPIP_HTTP_NET_EVENT_FILE_NAME_ERROR                = -2,

    /* a file name was longer than the HTTP storage space */
    TCPIP_HTTP_NET_EVENT_FILE_NAME_SIZE_ERROR           = -3,

    /* file size error */
    TCPIP_HTTP_NET_EVENT_FILE_SIZE_ERROR                = -4,

    /* file read error */
    TCPIP_HTTP_NET_EVENT_FILE_READ_ERROR                = -5,

    /* file parse error: line too long */
    TCPIP_HTTP_NET_EVENT_FILE_PARSE_ERROR               = -6,

    /* the depth allowed for a recursive call was exceeded */
    TCPIP_HTTP_NET_EVENT_DEPTH_ERROR                    = -7,

    /* an error occurred when parsing: dynamic variable name terminator could not be found, for example */
    TCPIP_HTTP_NET_EVENT_DYNVAR_PARSE_ERROR             = -8,

    /* a write error was reported while performing an FS upload */
    TCPIP_HTTP_NET_EVENT_FS_WRITE_ERROR                 = -9,

    /* a write error was reported while mounting after an FS upload */
    TCPIP_HTTP_NET_EVENT_FS_MOUNT_ERROR                 = -10,

    /* the number of retries for getting a chunk from the pool has been exceeded */
    TCPIP_HTTP_NET_EVENT_CHUNK_POOL_ERROR               = -11,

    /* the number of retries for getting a file buffer has been exceeded */
    TCPIP_HTTP_NET_EVENT_FILE_BUFFER_POOL_ERROR         = -12,

    /* out of memory when trying to allocate space for a dynamic variable descriptor */
    /* Note that dynamic allocation is used for the dynamic variable descriptor */
    TCPIP_HTTP_NET_EVENT_DYNVAR_ALLOC_ERROR             = -13,

    /* out of memory when trying to allocate space for a file upload */
    /* Note that dynamic allocation is used for the file upload buffer */
    TCPIP_HTTP_NET_EVENT_UPLOAD_ALLOC_ERROR             = -14,

    /* an error occurred when parsing an SSI command: SSI terminator could not be found, for example */
    TCPIP_HTTP_NET_EVENT_SSI_PARSE_ERROR                = -15,

    /* an unknown/unsupported SSI command */
    TCPIP_HTTP_NET_EVENT_SSI_COMMAND_ERROR              = -16,

    /* a SSI attribute error : command w/o attribute, etc. */
    TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_ERROR               = -17,

    /* out of memory when trying to allocate space for a SSI command descriptor */
    /* Note that dynamic allocation is used for the SSI commands */
    TCPIP_HTTP_NET_EVENT_SSI_ALLOC_DESCRIPTOR_ERROR     = -18,

    /* out of memory when trying to allocate memory for the peek buffer */
    /* the peek operation failed */
    TCPIP_HTTP_NET_EVENT_PEEK_ALLOC_BUFFER_ERROR        = -19,

    /* out of memory when trying to allocate memory for the SSI echo buffer */
    /* the echo operation failed */
    TCPIP_HTTP_NET_EVENT_SSI_ALLOC_ECHO_ERROR           = -20,

    /* Dynamic parsing warnings. Multiple flags could be set */

    /* warning: a dynamic variable argument name too long, truncated */
    TCPIP_HTTP_NET_EVENT_DYNVAR_ARG_NAME_TRUNCATED      = 0x8001,

    /* warning: too many arguments for a dynamic variable, truncated */
    TCPIP_HTTP_NET_EVENT_DYNVAR_ARG_NUMBER_TRUNCATED    = 0x8002,

    /* warning: too many retries for a dynamic variable, stopped */
    TCPIP_HTTP_NET_EVENT_DYNVAR_RETRIES_EXCEEDED        = 0x8003,

    /* warning: too many attributes for a SSI command, truncated */
    TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_NUMBER_TRUNCATED    = 0x8004,

    /* warning: unrecognized/unsupported SSI command attribute */
    TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_UNKNOWN             = 0x8005,

    /* warning: wrong number of SSI command attributes */
    TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_NUMBER_MISMATCH    = 0x8006,

    /* warning: number of SSI set variables exceeded */
    TCPIP_HTTP_NET_EVENT_SSI_VAR_NUMBER_EXCEEDED       = 0x8007,

    /* warning: SSI variable does not exist */
    TCPIP_HTTP_NET_EVENT_SSI_VAR_UNKNOWN               = 0x8008,

    /* warning: SSI variable is void: not echoed */
    TCPIP_HTTP_NET_EVENT_SSI_VAR_VOID                  = 0x8009,

    /* warning: SSI variable hash could not be created, allocation failed */
    /* Ther ewill be no run time SSI variable support */
    TCPIP_HTTP_NET_EVENT_SSI_HASH_CREATE_FAILED       = 0x800a,

    /* event: SSI variable deleted */
    TCPIP_HTTP_NET_EVENT_SSI_VAR_DELETED               = 0x800b,

    /* Other run-time warnings */

    /* warning: allocation from the HTTP chunk pool failed */
    /* (the allocation will be retried) */
    TCPIP_HTTP_NET_EVENT_CHUNK_POOL_EMPTY               = 0x8020,

    /* warning: allocation from the HTTP file buffers pool failed */
    /* (the allocation will be retried) */
    TCPIP_HTTP_NET_EVENT_FILE_BUFFER_POOL_EMPTY         = 0x8021,

    /* warning: the HTTP peek buffer is too small and cannot contain */
    /* all the data available in the transport socket buffer */
    /* and a HTTP search operation may fail */
    /* This will happen only for transport sockets that do not support */
    /* peek operation with an offset parameter */
    TCPIP_HTTP_NET_EVENT_PEEK_BUFFER_SIZE_EXCEEDED      = 0x8030,


}TCPIP_HTTP_NET_EVENT_TYPE;


// HTTP connection identifier, handle of a HTTP connection
typedef const void*     TCPIP_HTTP_NET_CONN_HANDLE;

// HTTP module configuration flags
// Multiple flags can be OR-ed
typedef enum
{
    TCPIP_HTTP_NET_MODULE_FLAG_DEFAULT              = 0x00,     // Default flags value

    TCPIP_HTTP_NET_MODULE_FLAG_NON_PERSISTENT       = 0x01,     // Use non-persistent connections
                                                                // This flag will cause the HTTP 
                                                                // connections to be non-persistent
                                                                // and closed after serving each request 
                                                                // to the client
                                                                // By default the HTTP connections are persistent
    TCPIP_HTTP_NET_MODULE_FLAG_NO_DELAY             = 0x02,     // Create the HTTP sockets with NO-DELAY option.
                                                                // It will flush data as soon as possible.
    TCPIP_HTTP_NET_MODULE_FLAG_SECURE_ON            = 0x10,     // All HTTP connections have to be secure
                                                                // (supposing the network presentation layer 
                                                                // supports encryption)
                                                                // Cannot be used together with 
                                                                // TCPIP_HTTP_NET_MODULE_FLAG_SECURE_OFF

    TCPIP_HTTP_NET_MODULE_FLAG_SECURE_OFF           = 0x20,     // HTTP connections will be non-secure
                                                                // Cannot be used together with 
                                                                // TCPIP_HTTP_NET_MODULE_FLAG_SECURE_ON

    TCPIP_HTTP_NET_MODULE_FLAG_SECURE_DEFAULT       = 0x00,     //  HTTP security is based on the port numbers

}TCPIP_HTTP_NET_MODULE_FLAGS;

// HTTP module dynamic configuration data
typedef struct
{
    uint16_t    nConnections;   // number of simultaneous HTTP connections allowed
    uint16_t    dataLen;        // size of the data buffer for reading cookie and GET/POST arguments (bytes)
    uint16_t    sktTxBuffSize;  // size of TX buffer for the associated socket; leave 0 for default
    uint16_t    sktRxBuffSize;  // size of RX buffer for the associated socket; leave 0 for default
    uint16_t    listenPort;     // HTTP listening port: 80, 443, etc.
    uint16_t    nDescriptors;   // how many buffers descriptors for dynamic variable processing to create
                                // they are independent of the HTTP connection number
                                // all the HTTP connections use from the dynamic descriptors pool
    uint16_t    nChunks;        // maximum number of chunks that are created
                                // It depends on the TCPIP_HTTP_NET_MAX_RECURSE_LEVEL and
                                // on the number of connections
                                // Maximum number should be TCPIP_HTTP_NET_MAX_CONNECTIONS * TCPIP_HTTP_NET_MAX_RECURSE_LEVEL
                                // All the chunks are in a pool and are used by all connections                                 
    uint16_t    maxRecurseLevel;// The maximum depth of recursive calls for serving a web page:
                                // - files without dynvars: 1
                                // - file including another file: + 1
                                // - file including a dynamic variable: + 1
                                // etc.
    uint16_t    configFlags;    // a TCPIP_HTTP_NET_MODULE_FLAGS value.
    uint16_t    nFileBuffers;   // number of file buffers to be created;
                                // These buffers are used to store data while file processing is done
                                // They are organized in a pool
                                // Each file being processed needs a file buffer and tries to get it from the pool
                                // If a buffer is not available, the HTTP conenction will wait for one to become available.
                                // Once the file is done the file buffer is released and could be used by a different file
                                // The number depends on the number of files that are processed in parallel
                                // To avoid deadlock the number should be >= than the number of maximum files that can be open simultaneously:
                                // i.e. for file1 ->include file2 -> include file3 you'll need >= 3 file process buffers
    uint16_t    fileBufferSize; // size of each of these file buffers
                                // should correspond to TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE
    uint16_t    chunkPoolRetries;  // how many retries to get chunk from the pool before giving up
    uint16_t    fileBufferRetries;  // how many retries to get a fileBuffer before giving up
    uint16_t    dynVarRetries;  // how many retries to have for a dynamic variable dynamicPrint function
                                // before calling it done
    uint16_t    connTimeout;    // persistent connection timeout, seconds
                                // If non 0 and a persistent connection is idle for more than this timeout,
                                // the server will close it.
                                // Maximum supported value is 32767 seconds
    // HTTP specific allocation functions
    // These functions are used for:
    //      - the dynamic variable descriptor allocation
    //      - SSI commands
    //      - file upload buffer allocation
    //  NOTE: if these functions are not provided but are needed, then the allocation functions
    //        passed at the stack initialization are used:
    //        TCPIP_STACK_HEAP_CONFIG.malloc_fnc and TCPIP_STACK_HEAP_CONFIG.free_fnc
    void* (*http_malloc_fnc)(size_t bytes); // malloc style allocation function for HTTP module
    void  (*http_free_fnc)(void* ptr);      // free style allocation function

    // Web pages directory
    // The HTTP server will serve pages located under this top directory.
    // No access is possible above this directory.
    // As a good practice to follow, use something like: "/mnt_point/webpages/"
    // to limit an external user access to this only directory in the file system
    // when using the Web server
    // NOTE: to allow access to the whole file system use the root directory:
    // "/mnt_point/" or "/" depending on your SYS_FS settings/mounting
    // But this usage should be discouraged because it gives HTTP access
    // to all system files.
    const char* web_dir;

} TCPIP_HTTP_NET_MODULE_CONFIG;

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

//*****************************************************************************
/*
  Function:
    uint8_t* TCPIP_HTTP_NET_URLDecode(uint8_t* cData)

  Summary:
    Parses a string from URL encoding to plain text.

  Description:
    This function parses a string from URL encoding to plain text.  The following
    conversions are made: ‘=’ to ‘\0’, ‘&’ to ‘\0’, ‘+’ to ‘ ‘, and
    “%xx” to a single hex byte.

    After completion, the data has been decoded and a null terminator
    signifies the end of a name or value.  A second null terminator (or a
    null name parameter) indicates the end of all the data.

  Precondition:
    The data parameter is null terminated and has at least one extra
    byte free.

  Parameters:
    cData - The string which is to be decoded in place.

  Returns:
    A pointer to the last null terminator in data, which is also the
    first free byte for new data.

  Remarks:
    This function is called by the stack to parse GET arguments and
    cookie data.  User applications can use this function to decode POST
    data, but first need to verify that the string is null-terminated.
 */
uint8_t*  TCPIP_HTTP_NET_URLDecode(uint8_t* cData);


//*****************************************************************************
/*
  Function:
    const uint8_t* TCPIP_HTTP_NET_ArgGet(const uint8_t* cData, const uint8_t* cArg)

  Summary:
    Locates a form field value in a given data array.

  Description:
    This function searches through a data array to find the value associated with a
    given argument.  It can be used to find form field values in data
    received over GET or POST.

    The end of data is assumed to be reached when a null name parameter is
    encountered.  This requires the string to have an even number of
    null-terminated strings, followed by an additional null terminator.

  Precondition:
    The data array has a valid series of null terminated name/value pairs.

  Parameters:
    cData - the buffer to search
    cArg - the name of the argument to find

  Returns:
    A pointer to the argument value, or NULL if not found.

  Example:
  <code>
    template_DynPrint(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT* 
                            varDcpt, const TCPIP_HTTP_NET_USER_CALLBACK* pCBack)
    {
        const uint8_t *ptr;

        ptr = TCPIP_HTTP_NET_ArgGet(TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle), 
                                   (const uint8_t*)"name");
        if(ptr == 0)
        {
            ptr = "not set";
        }
        else
        {
            strncpy(myBuffer, ptr, sizeof(myBuffer));
            ptr = myBuffer;
        }

        TCPIP_HTTP_NET_DynamicWrite(varDcpt, ptr, strlen(ptr), false);
            
    }
  </code>

  Remarks:
    None.
 */
const uint8_t*      TCPIP_HTTP_NET_ArgGet(const uint8_t* cData, const uint8_t* cArg);


//*****************************************************************************
/*
  Function:
    int TCPIP_HTTP_NET_ConnectionDynamicDescriptors(void);

  Summary:
    Returns the number of dynamic variable descriptors
    
  Description:
    This function returns the number of the dynamic variable buffer descriptors 
    that are allocated by the HTTP for dynamic variable processing.

  Precondition:
    None

  Parameters:
    None.

  Returns:
    The number of descriptors allocated by the HTTP module.
     
  Remarks:
    Currently the dynamic variable descriptors are allocated at the HTTP 
    initialization and cannot be changed at run-time.

 */

int TCPIP_HTTP_NET_ConnectionDynamicDescriptors(void);



//*****************************************************************************
/*
  Function:
    TCPIP_HTTP_NET_READ_STATUS TCPIP_HTTP_NET_ConnectionPostNameRead (TCPIP_HTTP_NET_CONN_HANDLE 
                                                      connHandle, uint8_t* cData, uint16_t wLen)

  Summary:
    Reads a name from a URL encoded string in the network transport buffer.

  Description:
    This function reads a name from a URL encoded string in the network transport 
    buffer. This function is meant to be called from an template_ConnectionPostExecute 
    callback to facilitate easier parsing of incoming data.
    This function also prevents buffer overflows by forcing the programmer
    to indicate how many bytes are expected.
    At least two extra bytes are needed in cData over the maximum
    length of data expected to be read.

    This function will read until the next '=' character, which indicates the
    end of a name parameter.  It assumes that the front of the buffer is
    the beginning of the name parameter to be read.

    This function properly updates pHttpCon->byteCount by decrementing it
    by the number of bytes read.  It also removes the delimiting '=' from
    the buffer.

  Precondition:
    The front of the network transport buffer is the beginning of a name parameter, 
    and the rest of the network transport buffer contains a URL-encoded string with 
    a name parameter terminated by a '=' character.

  Parameters:
    connHandle  - HTTP connection handle
    cData - where to store the name once it is read
    wLen - how many bytes can be written to cData

  Returns:
    - TCPIP_HTTP_NET_READ_OK - name was successfully read
    - TCPIP_HTTP_NET_READ_TRUNCTATED - entire name could not fit in the buffer, 
                                       so the value was truncated and data has been lost
    - TCPIP_HTTP_NET_READ_INCOMPLETE - entire name was not yet in the buffer, so call
                                       this function again later to retrieve

  Remarks:
    None.
 */
TCPIP_HTTP_NET_READ_STATUS    TCPIP_HTTP_NET_ConnectionPostNameRead (TCPIP_HTTP_NET_CONN_HANDLE 
                                                    connHandle, uint8_t* cData, uint16_t wLen);


//*****************************************************************************
/*
  Function:
    TCPIP_HTTP_NET_READ_STATUS TCPIP_HTTP_NET_ConnectionPostValueRead (TCPIP_HTTP_NET_CONN_HANDLE 
                                                        connHandle, uint8_t* cData, uint16_t wLen)

  Summary:
    Reads a value from a URL encoded string in the network transport buffer.

  Description:
    This function reads a value from a URL encoded string in the network transport 
    buffer.
    This function is meant to be called from an template_ConnectionPostExecute 
    callback to facilitate easier parsing of incoming data.  This function also 
    prevents buffer overflows by forcing the programmer to indicate how many bytes 
    are expected.  At least two extra bytes are needed in cData above the maximum
    length of data expected to be read.

    This function will read until the next '&' character, which indicates the
    end of a value parameter.  It assumes that the front of the buffer is
    the beginning of the value parameter to be read.  If pHttpCon->byteCount
    indicates that all expected bytes are in the buffer, it assumes that
    all remaining data is the value and acts accordingly.

    This function properly updates pHttpCon->byteCount by decrementing it
    by the number of bytes read.  The terminating '&' character is also
    removed from the buffer.

  Precondition:
    The front of the network transport buffer is the beginning of a name parameter, 
    and the rest of the network transport buffer contains a URL-encoded string with 
    a name parameter terminated by a '=' character.

  Parameters:
    connHandle  - HTTP connection handle
    cData - where to store the value once it is read
    wLen - how many bytes can be written to cData

  Returns:
    - TCPIP_HTTP_NET_READ_OK - value was successfully read
    - TCPIP_HTTP_NET_READ_TRUNCTATED - entire value could not fit in the buffer, 
                                       so the value was truncated and data has been lost
    - TCPIP_HTTP_NET_READ_INCOMPLETE - entire value was not yet in the buffer, so call
                                       this function again later to retrieve

  Remarks:
    None.
 */
TCPIP_HTTP_NET_READ_STATUS  TCPIP_HTTP_NET_ConnectionPostValueRead (TCPIP_HTTP_NET_CONN_HANDLE 
                                                   connHandle, uint8_t* cData, uint16_t wLen);


//*****************************************************************************
/*
  Function:
    FILE_HANDLE  TCPIP_HTTP_NET_ConnectionFileGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Get handle to current connection's file.

  Description:
    This function returns the handle of the current HTTP connection file.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    Handle to File System file belonging to the connection defined by connHandle

  Example:
  <code>
    uint8_t myBuff[20];

    // Get the file handle and read from that file
    SYS_FS_FileRead(myBuff, sizeof(myBuff), TCPIP_HTTP_NET_ConnectionFileGet(connHandle));
  </code>

  Remarks:
    None.
 */
SYS_FS_HANDLE  TCPIP_HTTP_NET_ConnectionFileGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    uint16_t  TCPIP_HTTP_NET_ConnectionPostSmGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Get the POST state machine state.

  Description:
    This function returns the POST state machine state for the connection defined by 
    connHandle.
    This state is maintained by the HTTP connection and can be used by the user of the
    HTTP to maintain its own POST state machine.
    The values of the POST state machine have significance only for the user of the 
    HTTP connection.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    16-bit integer POST state machine state.

  Example:
  <code>
    #define SM_POST_LCD_READ_NAME   1
    #define SM_POST_LCD_READ_VALUE  2
  
    switch(TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle))
    {
        // Find the name
        case SM_POST_LCD_READ_NAME:
         .
         .
         .
        // Found the value, so store the LCD and return
        case SM_POST_LCD_READ_VALUE:
         .
         .
         .
    }
  </code>

  Remarks:
    None.
 */
uint16_t  TCPIP_HTTP_NET_ConnectionPostSmGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);


//*****************************************************************************
/*
  Function:
    void  TCPIP_HTTP_NET_ConnectionPostSmSet(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                             uint16_t state)

  Summary:
    Set the POST state machine state.

  Description:
    This function sets the POST state machine state for the connection defined by 
    connHandle.
    This state is maintained by the HTTP connection and can be used by the user 
    of the HTTP to maintain its own POST state machine.
    The values of the POST state machine have significance only for the user of 
    the HTTP connection.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    state - 16 bit integer state for POST state machine

  Returns:
    None.

  Example:
  <code>
    uint8_t* httpDataBuff;
    uint16_t httpBuffSize;
    #define SM_POST_LCD_READ_NAME   1
    #define SM_POST_LCD_READ_VALUE  2

    switch(TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle))
    {
        // Find the name
        case SM_POST_LCD_READ_NAME:

            // Read a name
            httpBuffSize = TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(connHandle);
            if(TCPIP_HTTP_NET_ConnectionPostNameRead(connHandle, httpDataBuff, 
               httpBuffSize) == TCPIP_HTTP_NET_READ_INCOMPLETE)
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_POST_LCD_READ_VALUE);
            // No break...continue reading value

        // Found the value, so store the LCD and return
        case SM_POST_LCD_READ_VALUE:
         .
         .
         .
    }
  </code>

  Remarks:
    None.
 */
void  TCPIP_HTTP_NET_ConnectionPostSmSet(TCPIP_HTTP_NET_CONN_HANDLE 
                                         connHandle, uint16_t state);

//*****************************************************************************
/*
  Function:
    uint8_t*  TCPIP_HTTP_NET_ConnectionDataBufferGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Returns pointer to connection general purpose data buffer.

  Description:
    This function returns  a pointer to the HTTP connection internal data buffer.
    This gives access to the application to the data that's stored in the
    HTTP connection buffer.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    Pointer to the connection's general purpose data buffer.

  Example:
  <code>
    template_DynPrint(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
       const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, 
       const TCPIP_HTTP_NET_USER_CALLBACK* pCBack)
    {
        const uint8_t *ptr;

        ptr = TCPIP_HTTP_NET_ArgGet(TCPIP_HTTP_NET_ConnectionDataBufferGet
                                   (connHandle), (const uint8_t*)"name");
        if(ptr == 0)
        {
            ptr = "not set";
        }
        else
        {
            strncpy(myBuffer, ptr, sizeof(myBuffer));
            ptr = myBuffer;
        }
        TCPIP_HTTP_NET_DynamicWrite(varDcpt, ptr, strlen(ptr), false);
    }
  </code>

  Remarks:
    None.
 */
uint8_t*  TCPIP_HTTP_NET_ConnectionDataBufferGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);


//*****************************************************************************
/*
  Function:
    uint16_t  TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

  Summary:
    Returns the size of the connection general purpose data buffer.

  Description:
    This function returns the size of the HTTP connection internal data buffer.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    Size of the connection's general purpose data buffer.

  Example:
  <code>
    // Read a name
    uint8_t* httpDataBuff = TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);
    uint16_t httpDataLen = TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(connHandle);
    if(TCPIP_HTTP_NET_ConnectionPostNameRead(connHandle, httpDataBuff, httpDataLen) == TCPIP_HTTP_NET_READ_INCOMPLETE)
    {
        return TCPIP_HTTP_NET_IO_RES_NEED_DATA;
    }
  </code>

  Remarks:
    This is the parameter that was used for HTTP initialization in
    TCPIP_HTTP_NET_MODULE_CONFIG::dataLen.
    
 */
uint16_t  TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    uint32_t  TCPIP_HTTP_NET_ConnectionCallbackPosGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Returns the callback position indicator.

  Description:
    This function will return the current value of the callback position indicator
    for the HTTP connection identified by connHandle.
    The callback position indicator is used in the processing of the
    HTTP dynamic variables.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    Callback position indicator for connection defined by connHandle.

  Example:
  <code>
  </code>

  Remarks:
    None.
 */
uint32_t  TCPIP_HTTP_NET_ConnectionCallbackPosGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);


//*****************************************************************************
/*
  Function:
    void  TCPIP_HTTP_NET_ConnectionCallbackPosSet(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                                  uint32_t callbackPos)

  Summary:
    Sets the callback position indicator.

  Description:
    This function will set the current value of the callback position indicator
    for the HTTP connection identified by connHandle.
    The callback position indicator is used in the processing of the
    HTTP dynamic variables.
    When set to a value != 0, it indicates to the HTTP server that the application
    has more pending processing that needs to be done.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    callbackPos - new connection callback position value

  Returns:
    None.

  Example:
  <code>
  </code>

  Remarks:
    None.
 */
void  TCPIP_HTTP_NET_ConnectionCallbackPosSet(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                              uint32_t callbackPos);

//*****************************************************************************
/*
  Function:
    TCPIP_HTTP_NET_STATUS TCPIP_HTTP_NET_ConnectionStatusGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

  Summary:
    Gets HTTP status.

  Description:
    This function returns the current HTTP status of the selected HTTP connection.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    A TCPIP_HTTP_NET_STATUS value.

  Example:
  <code>
    TCPIP_HTTP_NET_STATUS currStat =  TCPIP_HTTP_NET_ConnectionStatusGet(connHandle);
  </code>

  Remarks:
    None.
 */
TCPIP_HTTP_NET_STATUS         TCPIP_HTTP_NET_ConnectionStatusGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);



//*****************************************************************************
/*
  Function:
    void TCPIP_HTTP_NET_ConnectionStatusSet(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                            TCPIP_HTTP_NET_STATUS stat)

  Summary:
    Sets HTTP status.

  Description:
    Allows write access to the HTTP status of the selected HTTP connection.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    stat        - new TCPIP_HTTP_NET_STATUS enumeration value.

  Returns:
    None.

  Example:
  <code>
    byteCount = TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle);
    int sktRxSize;
    
    sktRxSize = TCPIP_HTTP_NET_ConnectionReadBufferSize(connHandle);

    if(byteCount > sktRxSize)
    {   // Configuration Failure
        // 302 Redirect will be returned
        TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, TCPIP_HTTP_NET_STAT_REDIRECT);
    }
  </code>

  Remarks:
    None.
 */
void  TCPIP_HTTP_NET_ConnectionStatusSet(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                         TCPIP_HTTP_NET_STATUS stat);


//*****************************************************************************
/*
  Function:
    uint8_t TCPIP_HTTP_NET_ConnectionHasArgsGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Checks whether there are get or cookie arguments.

  Description:
    The function will get the value of the "cookies or get arguments" that are present.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    The current value of the connection hasArgs.

  Example:
  <code>
    uint8_t hasArgs = TCPIP_HTTP_NET_ConnectionHasArgsGet(connHandle);
  </code>

  Remarks:
    None.
 */
uint8_t        TCPIP_HTTP_NET_ConnectionHasArgsGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    void TCPIP_HTTP_NET_ConnectionHasArgsSet(TCPIP_HTTP_NET_CONN_HANDLE connHandle, uint8_t args)

  Summary:
    Sets whether there are get or cookie arguments.

  Description:
    The function sets the value of the "cookies or get arguments" that are present.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    args        - number of arguments

  Returns:
    None.

  Example:
  <code>
    else if(!memcmp(filename, "cookies.htm", 11))
    { 
        TCPIP_HTTP_NET_ConnectionHasArgsSet(connHandle, 1);
    }
  </code>

  Remarks:
    None.
 */
void   TCPIP_HTTP_NET_ConnectionHasArgsSet(TCPIP_HTTP_NET_CONN_HANDLE connHandle, uint8_t args);


//*****************************************************************************
/*
  Function:
    uint32_t TCPIP_HTTP_NET_ConnectionByteCountGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Returns how many bytes are available to be read.

  Description:
    This function returns the current value of the counter showing the number of bytes
    available to be read on the connection.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    Current connection byte count, how many bytes have been read so far.

  Example:
  <code>
    switch(TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle))
    {
        case SM_CFG_SNMP_READ_NAME:
            // If all parameters have been read, end
            if(TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle) == 0u)
            {
                return TCPIP_HTTP_NET_IO_RES_DONE;
            }
         .
         .
         .
    }
  </code>

  Remarks:
    None.
 */
uint32_t  TCPIP_HTTP_NET_ConnectionByteCountGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);


//*****************************************************************************
/*
  Function:
    void  TCPIP_HTTP_NET_ConnectionByteCountSet(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                                uint32_t byteCount)

  Summary:
    Sets how many bytes are available to be read.

  Description:
    This function sets the current value of the counter showing the number of bytes
    available to be read from the connection.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    byteCount   - byte count to be set

  Returns:
    None.

  Remarks:
    None.
 */
void  TCPIP_HTTP_NET_ConnectionByteCountSet(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                            uint32_t byteCount);


//*****************************************************************************
/*
  Function:
    void  TCPIP_HTTP_NET_ConnectionByteCountDec(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                                uint32_t byteCount)

  Summary:
    Decrements the connection byte count.

  Description:
    This function decrements the current value of the counter showing the number of bytes
    available to be read from the connection.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    byteCount   - byte count reduction

  Returns:
    None.

  Remarks:
    None.
 */
void  TCPIP_HTTP_NET_ConnectionByteCountDec(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                            uint32_t byteCount);


//*****************************************************************************
/*
  Function:
    uint8_t   TCPIP_HTTP_NET_ConnectionIsAuthorizedGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Gets the authorized state for the current connection.

  Description:
    This function returns the authorization status for the current HTTP connection.
    This is one of the values returned by the template_ConnectionFileAuthenticate 
    function.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    A uint8_t representing the authorization status.

  Example:
  <code>
    uint8_t isAuth;

    isAuth = TCPIP_HTTP_NET_ConnectionIsAuthorizedGet(connHandle);
  </code>

  Remarks:
    None.
 */
uint8_t   TCPIP_HTTP_NET_ConnectionIsAuthorizedGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);


//*****************************************************************************
/*
  Function:
    void   TCPIP_HTTP_NET_ConnectionIsAuthorizedSet(TCPIP_HTTP_NET_CONN_HANDLE 
                                                      connHandle, uint8_t auth)

  Summary:
    Sets the authorized state for the current connection.

  Description:
    This function sets the authorization status for the current HTTP connection.
    This has to be one of the values in the set returned by the 
    template_ConnectionFileAuthenticate function.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    auth        - new authorization state

  Returns:
    None.

  Example:
  <code>
    uint8_t auth = 0x80;

    TCPIP_HTTP_NET_ConnectionIsAuthorizedSet(connHandle, auth);
  </code>

  Remarks:
    None.
 */
void   TCPIP_HTTP_NET_ConnectionIsAuthorizedSet(TCPIP_HTTP_NET_CONN_HANDLE 
                                                connHandle, uint8_t auth);


//*****************************************************************************
/*
  Function:
    void    TCPIP_HTTP_NET_ConnectionUserDataSet(TCPIP_HTTP_NET_CONN_HANDLE 
                                              connHandle, const void* uData)

  Summary:
    Sets the user data parameter for the current connection.

  Description:
    This function will set the user data value for the current HTTP connection.
    This data belongs to the user and is not used in any way by the HTTP server module.
    It is available to the user by calling TCPIP_HTTP_NET_ConnectionUserDataGet.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    uData       - user supplied data

  Returns:
    None.

  Example:
  <code>
    uint32_t myConnData;

    TCPIP_HTTP_NET_ConnectionUserDataSet(connHandle, (const void*)myConnData);
  </code>

  Remarks:
    None.
 */
void    TCPIP_HTTP_NET_ConnectionUserDataSet(TCPIP_HTTP_NET_CONN_HANDLE 
                                             connHandle, const void* uData);


//*****************************************************************************
/*
  Function:
    const void*    TCPIP_HTTP_NET_ConnectionUserDataGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Gets the user data parameter for the current connection.

  Description:
    This function returns the user data value for the current HTTP connection.
    This data belongs to the user and is not used in any way by the HTTP server module.
    It can be set by the user with TCPIP_HTTP_NET_ConnectionUserDataSet.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    User data that's stored as part of the connection.

  Example:
  <code>
    uint32_t myConnData;

    myConnData = (uint32_t)TCPIP_HTTP_NET_ConnectionUserDataGet(connHandle);
  </code>

  Remarks:
    None.
 */
const void*    TCPIP_HTTP_NET_ConnectionUserDataGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    int TCPIP_HTTP_NET_ConnectionIndexGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

  Summary:
    Gets the index of the HTTP connection.

  Description:
    This function will return the index of the requested HTTP connection.
   
  Precondition:
    None.

  Parameters:
    connHandle      - the HTTP connection handle.

  Returns:
    The connection index.

  Example:
  <code>
    int connIx;

    connIx = TCPIP_HTTP_NET_ConnectionIndexGet(connHandle);
  </code>

  Remarks:
    None

 */
int TCPIP_HTTP_NET_ConnectionIndexGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    TCPIP_HTTP_NET_CONN_HANDLE    TCPIP_HTTP_NET_ConnectionHandleGet(int connIx);

  Summary:
    Gets the connection handle of a HTTP connection.

  Description:
    This function will return the connection handle of the requested HTTP connection index.
   
  Precondition:
    None.

  Parameters:
    connIx      - the HTTP connection ix.

  Returns:
    A valid connection handle if the connection index is valid
    0 if there is no such connection

  Example:
  <code>
    TCPIP_HTTP_NET_CONN_HANDLE connHandle;

    connHandle = TCPIP_HTTP_NET_ConnectionHandleGet(0);
  </code>

  Remarks:
    None

 */
TCPIP_HTTP_NET_CONN_HANDLE    TCPIP_HTTP_NET_ConnectionHandleGet(int connIx);

//*****************************************************************************
/*
  Function:
    int    TCPIP_HTTP_NET_ActiveConnectionCountGet(int* pOpenCount);

  Summary:
    Gets the number of active (and inactive) connections.

  Description:
    This function will return the number of active and total opened HTTP connections
    at the current time.
   
  Precondition:
    None.

  Parameters:
    pOpenCount  - address to store the number of total HTTP opened connections
                  Could be NULL if not needed

  Returns:
    The number of active/total connections.

  Example:
  <code>
    int nConns;

    nConns = TCPIP_HTTP_NET_ActiveConnectionCountGet(0);
  </code>

  Remarks:
    The value returned by this function is informational only.
    The number of active connections changes dynamically.

 */
int    TCPIP_HTTP_NET_ActiveConnectionCountGet(int* pOpenCount);


//*****************************************************************************
/*
  Function:
    TCPIP_HTTP_NET_READ_STATUS TCPIP_HTTP_NET_ConnectionPostReadPair(TCPIP_HTTP_NET_CONN_HANDLE 
                                                     connHandle, uint8_t* cData, uint16_t wLen)

  Summary:
    Reads a name and value pair from a URL encoded string in the network transport 
    buffer.

  Description:
    Reads a name and value pair from a URL encoded string in the network transport 
    buffer.
    This function is meant to be called from an template_ConnectionPostExecute 
    callback to facilitate easier parsing of incoming data.  This function also prevents
    buffer overflows by forcing the programmer to indicate how many bytes are
    expected.  At least 2 extra bytes are needed in cData over the maximum
    length of data expected to be read.

    This function will read until the next '&' character, which indicates the
    end of a value parameter.  It assumes that the front of the buffer is
    the beginning of the name parameter to be read.

    This function properly updates the connection byteCount 
    (see TCPIP_HTTP_NET_ConnectionByteCountGet) by decrementing it by the number 
    of bytes read.  It also removes the delimiting '&' from the buffer.

    Once complete, two strings will exist in the cData buffer.  The first is
    the parameter name that was read, while the second is the associated
    value.

  Precondition:
    The front of the network transport buffer is the beginning of a name parameter, 
    and the rest of the network transport buffer contains a URL-encoded string with 
    a name parameter terminated by a '=' character and a value parameter terminated 
    by a '&'.

  Parameters:
    connHandle  - HTTP connection handle
    cData - where to store the name and value strings once they are read
    wLen - how many bytes can be written to cData

  Returns:
    - TCPIP_HTTP_NET_READ_OK - name and value were successfully read
    - TCPIP_HTTP_NET_READ_TRUNCTATED - entire name and value could not fit in 
                                       the buffer, so input was truncated and 
                                       data has been lost
    - TCPIP_HTTP_NET_READ_INCOMPLETE - entire name and value was not yet in 
                                       the buffer, so call this function again 
                                       later to retrieve

  Remarks:
    This function is aliased to TCPIP_HTTP_NET_ConnectionPostValueRead, 
    since they effectively perform the same task.  The name is provided only 
    for completeness.
 */
#define TCPIP_HTTP_NET_ConnectionPostReadPair(connHandle, cData, wLen) \
        TCPIP_HTTP_NET_ConnectionPostValueRead(connHandle, cData, wLen)

// *****************************************************************************
// Section: HTTP User-implemented Callback data structure
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Enumeration:
    TCPIP_HTTP_DYN_ARG_TYPE

  Summary:
    HTTP supported dynamic variables argument types.

  Description:
    This enumeration defines the types of the HTTP supported dynamic variables
    arguments.
   
  Remarks:
    Currently a dynamic variable can have either string or int32_t parameters.

    Only 16 bit values are currently supported.
*/
typedef enum
{
    /* invalid argument type */
    TCPIP_HTTP_DYN_ARG_TYPE_INVALID      = 0,

    /* the dynamic variable argument is a int32_t */
    TCPIP_HTTP_DYN_ARG_TYPE_INT32,

    /* the dynamic variable argument is a ASCII string */
    TCPIP_HTTP_DYN_ARG_TYPE_STRING,


}TCPIP_HTTP_DYN_ARG_TYPE;

// *****************************************************************************
/*
  Data structure:
    TCPIP_HTTP_DYN_ARG_DCPT

  Summary:
    HTTP dynamic argument descriptor.

  Description:
    This data type defines the structure of a HTTP dynamic variable argument.
    It is used for describing the dynamic variables arguments.


  Remarks:
    The argument flags are currently not used.
    They are meant for further extensions.

*/
typedef struct
{
    /* argument type */
    uint16_t            argType;        // a TCPIP_HTTP_DYN_ARG_TYPE value
    uint16_t            argFlags;       // extra argument flags
    union
    {
        int32_t         argInt32;       // use this member when the arg type is INT32
        const char*     argStr;         // use this member when the arg type is string
    };
}TCPIP_HTTP_DYN_ARG_DCPT;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_HTTP_DYN_VAR_FLAGS

  Summary:
    HTTP supported dynamic variables flags.

  Description:
    This enumeration defines the flags associated with the HTTP supported dynamic 
    variables.
   
  Remarks:
    Multiple flags con be set simultaneously.
    New flags will be eventually added.
    Only 16-bit values are currently supported.

*/
typedef enum
{
    /* no flag associated with this dynamic variable */
    TCPIP_HTTP_DYN_VAR_FLAG_NONE      = 0,

    /* dynamic variable field exceeded available parsing space */
    /* the dynamic variable name has been truncated */
    TCPIP_HTTP_DYN_VAR_FLAG_NAME_TRUNCATED      = 0x01,

    /* dynamic variable field exceeded available parsing space */
    /* the dynamic variable arguments have been truncated */
    TCPIP_HTTP_DYN_VAR_FLAG_ARG_NAME_TRUNCATED  = 0x02,

    /* dynamic variable arguments exceeded available buffering space */
    /* the number of arguments of the dynamic variable has been truncated */
    TCPIP_HTTP_DYN_VAR_FLAG_ARG_NO_TRUNCATED   = 0x04,


}TCPIP_HTTP_DYN_VAR_FLAGS;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_HTTP_DYN_PRINT_RES

  Summary:
    Dynamic print result when a dynamic variable print callback function returns;

  Description:
    This enumeration defines the results associated with the HTTP dynamic variables
    callbacks (template_DynPrint).
   
  Remarks:
    Currently the default action for a user defined dynamic variable is "do nothing".
    Keywords like "inc" have an internal implementation and don't need to be
    processed by the user.

*/
typedef enum
{
    /* dynamic callback is done */
    TCPIP_HTTP_DYN_PRINT_RES_DONE      = 0,

    /* no implementation supported for this dynamic variable, call the default action */
    TCPIP_HTTP_DYN_PRINT_RES_DEFAULT,

    /* process the action of this call and then call again */
    /* data written to the HTTP connection but more data is pending */ 
    TCPIP_HTTP_DYN_PRINT_RES_PROCESS_AGAIN,

    /* do not process any action just call again */
    /* no data was available to be written to the connection */
    /* or the written data should be ignored this turn */
    TCPIP_HTTP_DYN_PRINT_RES_AGAIN,


}TCPIP_HTTP_DYN_PRINT_RES;




// *****************************************************************************
/*
  Data structure:
    TCPIP_HTTP_DYN_VAR_DCPT

  Summary:
    HTTP dynamic variable descriptor.

  Description:
    This data type defines the structure of a HTTP dynamic variable descriptor.
    When the user registers a template_DynPrint function
    for dynamic variable processing, this callback will receive
    the dynamic variable descriptor as a parameter.

  Remarks:
    None.
*/
typedef struct
{
    const char*                     dynName;    // ASCII string storing the dynamic 
                                                // variable name
    const char*                     fileName;   // ASCII string storing the file name 
                                                // the dynamic variable belongs to
    uint16_t                        callbackID; // Call back ID: the dynamic variable 
                                                // index within the file
    uint8_t                         dynFlags;   // a TCPIP_HTTP_DYN_VAR_FLAGS value
    uint8_t                         nArgs;      // number of arguments that the variable has
    TCPIP_HTTP_DYN_ARG_DCPT*        dynArgs;    // array of argument descriptors carrying 
                                                // the dynamic variable arguments
    const void*                     dynContext; // dynamic context of the callback.
                                                // This context is used by the HTTP server 
                                                // and is irrelevant to the user.
                                                // Should NOT be modified in any way by the user!
}TCPIP_HTTP_DYN_VAR_DCPT;

// *****************************************************************************
/*
  Data structure:
    TCPIP_HTTP_SSI_ATTR_DCPT

  Summary:
    HTTP SSI attribute descriptor.

  Description:
    This data type defines the structure of a SSI command attribute descriptor.
    When the user registers a template_SSINotification function
    for SSI processing, this callback will receive info about the SSI attribute
    descriptors.

  Remarks:
    The SSI commands consist of pairs of (attribute, value) tokens.
    For example:
    <!--#set var="varname" value="varvalue" --> 
    has 2 pairs:
    pair 1 - attribute = var
             value = varname
    pair 2 - attribute = value
             value = varvalue

*/

typedef struct
{
    const char* attribute;      // the SSI attribute of the command
    char* value;                // the SSI value 
}TCPIP_HTTP_SSI_ATTR_DCPT;

// *****************************************************************************
/*
  Data structure:
    TCPIP_HTTP_SSI_NOTIFY_DCPT

  Summary:
    HTTP SSI notification descriptor.

  Description:
    This data type defines the structure of a SSI notification descriptor.
    When the user registers a template_SSINotification function
    for SSI processing, this callback will receive a SSI descriptor as a parameter.

  Remarks:
    None.

*/

typedef struct
{

    const char*                 fileName;       // the file containing the SSI command
    char*                       ssiCommand;     // the SSI command parsed from the command line
    int                         nAttribs;       // number of attributes descriptors in the command
    TCPIP_HTTP_SSI_ATTR_DCPT*   pAttrDcpt;      // pointer to an array of descriptors parsed from this SSI command
}TCPIP_HTTP_SSI_NOTIFY_DCPT;


// *****************************************************************************
/*
  Structure:
    TCPIP_HTTP_NET_USER_CALLBACK

  Summary:
    HTTP user implemented callback data structure.

  Description:
    This data structure defines the user callbacks that are implemented by the 
    user and the HTTP server calls at run-time.

  Remarks:
    See the detailed explanation for each callback in the callback templates section.  

    The extra pCBack parameter is passed back for allowing the user to store 
    additional info in the supplied TCPIP_HTTP_NET_USER_CALLBACK data structure that could be used at run-time.
*/
typedef struct _tag_TCPIP_HTTP_NET_USER_CALLBACK
{
    /*  template_ConnectionGetExecute GET process function */ 
    TCPIP_HTTP_NET_IO_RESULT (*getExecute)(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                         const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);
    /*  template_ConnectionPostExecute POST process function */ 
    TCPIP_HTTP_NET_IO_RESULT (*postExecute)(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                          const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);
    /*  template_ConnectionFileAuthenticate File Authenticate function */ 
    uint8_t (*fileAuthenticate)(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const char* cFile, 
              const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);
    /*  template_ConnectionUserAuthenticate User Authenticate function */ 
    uint8_t (*userAuthenticate)(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const char* cUser, 
             const char* cPass, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);
    /*  template_DynPrint Dynamic variable process function */ 
    TCPIP_HTTP_DYN_PRINT_RES (*dynamicPrint)(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                              const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, 
                              const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);
    /*  template_DynAcknowledge Dynamic variable acknowledge function */ 
    void (*dynamicAck)(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const void* buffer, 
          const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);
    /* template_EventReport run-time HTTP processing report */
    void (*eventReport)(TCPIP_HTTP_NET_CONN_HANDLE connHandle, TCPIP_HTTP_NET_EVENT_TYPE evType, 
          const void* evInfo, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);
    /* template_SSINotification run-time HTTP SSI processing */
    bool (*ssiNotify)(TCPIP_HTTP_NET_CONN_HANDLE connHandle, TCPIP_HTTP_SSI_NOTIFY_DCPT* pSSINotifyDcpt, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);

}TCPIP_HTTP_NET_USER_CALLBACK;

// *****************************************************************************
/*
  Type:
    TCPIP_HTTP_NET_USER_HANDLE

  Summary:
    HTTP user handle.

  Description:
    A handle that a client can use after the HTTP user callback has been registered.
 */

typedef const void* TCPIP_HTTP_NET_USER_HANDLE;

//*****************************************************************************
/*
  Function:
    TCPIP_HTTP_NET_USER_HANDLE  TCPIP_HTTP_NET_UserHandlerRegister 
                              (const TCPIP_HTTP_NET_USER_CALLBACK* userCallback);

  Summary:
    Registers a user callback structure with the HTTP server

  Description:
    The function registers a user callback data structure with the HTTP server.
    The HTTP server will call the user supplied callbacks at run-time when 
    processing the web pages.


  Precondition:
    The HTTP server module properly initialized.

  Parameters:
    userCallback    - user callback to be registered with the HTTP server  

  Returns:
    Returns a valid handle if the call succeeds, or a null handle if
    the call failed (out of memory, for example).

  Example:
  <code>
  </code>

  Remarks:
    Currently only one user callback structure could be registered.
    The call will fail if a user callback structure is already registered.

 */
TCPIP_HTTP_NET_USER_HANDLE    TCPIP_HTTP_NET_UserHandlerRegister 
                              (const TCPIP_HTTP_NET_USER_CALLBACK* userCallback);

// *****************************************************************************
/* Function:
    TCPIP_HTTP_NET_UserHandlerDeregister(TCPIP_HTTP_NET_USER_HANDLE hHttp)

  Summary:
    Deregisters a previously registered HTTP user handler.
    
  Description:
    This function deregisters a HTTP user callback handler.

  Precondition:
    The HTTP server module properly initialized.

  Parameters:
    hHttp   - A handle returned by a previous call to TCPIP_HTTP_NET_UserHandlerRegister

  Returns:
    - true	- if the call succeeds
    - false - if no such handler is registered
              or there are active connections

  Remarks:
    The call will fail if there is active HTTP traffic.
    The handler cannot be deregistered while HTTP traffic is in progress.
 */

bool             TCPIP_HTTP_NET_UserHandlerDeregister(TCPIP_HTTP_NET_USER_HANDLE hHttp);

// *****************************************************************************
// Section: Templates for User-implemented Callback Function Prototypes
// *****************************************************************************
// *****************************************************************************

//*****************************************************************************
/*
  Function:
    TCPIP_HTTP_NET_IO_RESULT template_ConnectionGetExecute 
                          (TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                          const TCPIP_HTTP_NET_USER_CALLBACK* pCBack)

  Summary:
    Processes GET form field variables and cookies.

  Description:
    This function is implemented by the application developer.
    Its purpose is to parse the data received from URL parameters (GET method forms) 
    and cookies and perform any application-specific tasks in response to these inputs.  
    Any required authentication has already been validated.

    When this function is called, the connection data buffer
    (see TCPIP_HTTP_NET_ConnectionDataBufferGet) contains sequential name/value pairs 
    of strings representing the data received. In this format, TCPIP_HTTP_NET_ArgGet 
    can be used to search for specific variables in the input.  If data buffer space 
    associated with this connection is required, connection data buffer may be overwritten
    here once the application is done with the values.  Any data placed there will be 
    available to future callbacks for this connection, including 
    TCPIP_HTTP_NET_ConnectionPostExecute and any TCPIP_HTTP_NET_Print_varname dynamic
    substitutions.

    This function may also issue redirections by setting the connection data
    buffer to the destination file name or URL, and the connection
    httpStatus (TCPIP_HTTP_NET_ConnectionStatusSet()) to TCPIP_HTTP_NET_STAT_REDIRECT.

    Finally, this function may set cookies.  Set connection data buffer to a series
    of name/value string pairs (in the same format in which parameters
    arrive) and then set the connection hasArgs (TCPIP_HTTP_NET_ConnectionHasArgsSet)
    equal to the number of cookie name/value pairs.
    The cookies will be transmitted to the browser, and any future requests
    will have those values available in the connection data buffer.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    pCBack      - user TCPIP_HTTP_NET_USER_CALLBACK pointer

  Returns:
    - TCPIP_HTTP_NET_IO_RES_DONE - application is done processing
    - TCPIP_HTTP_NET_IO_RES_NEED_DATA - this value may not be returned because 
                                        more data will not become available
    - TCPIP_HTTP_NET_IO_RES_WAITING - the application is waiting for an asynchronous
                                      process to complete, and this function should be
                                      called again later

  Remarks:
    This function is only called if variables are received via URL
    parameters or Cookie arguments.
  
    This function may NOT write to the network transport buffer.

    This function may service multiple HTTP requests simultaneously.
    Exercise caution when using global or static variables inside this
    routine.
    Use the connection callbackPos (TCPIP_HTTP_NET_ConnectionCallbackPosGet)
    or the connection data buffer for storage associated with individual requests.
 */
TCPIP_HTTP_NET_IO_RESULT template_ConnectionGetExecute 
                      (TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                      const TCPIP_HTTP_NET_USER_CALLBACK* pCBack);

//*****************************************************************************
/*
  Function:
    TCPIP_HTTP_NET_IO_RESULT template_ConnectionPostExecute 
                      (TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                      const TCPIP_HTTP_NET_USER_CALLBACK* pCBack)

  Summary:
    Processes POST form variables and data.

  Description:
    This function is implemented by the application developer.
    Its purpose is to parse the data received from
    POST forms and perform any application-specific tasks in response
    to these inputs.  Any required authentication has already been
    validated before this function is called.

    When this function is called, POST data will be waiting in the
    network transport buffer.
    The connection byteCount (see TCPIP_HTTP_NET_ConnectionByteCountGet
    will indicate the number of bytes remaining to be read before the 
	browser request is complete.

    Since data is still in the network transport buffer, the application must call
    TCPIP_HTTP_NET_ConnectionRead in order to retrieve bytes.  When this is done,
    connection byteCount MUST be updated to reflect how many bytes now
    remain.

    In general, data submitted from web forms via POST is URL encoded.
    The TCPIP_HTTP_NET_URLDecode function can be used to decode this information
    back to a standard string if required.  If data buffer space
    associated with this connection is required, the connection data buffer 
    (see TCPIP_HTTP_NET_ConnectionDataBufferGet) may be
    overwritten here once the application is done with the values.
    Any data placed there will be available to future callbacks for
    this connection,  including template_ConnectionPostExecute and any
    TCPIP_HTTP_NET_Print_varname dynamic substitutions.

    Whenever a POST form is processed it is recommended to issue a
    redirect back to the browser, either to a status page or to
    the same form page that was posted.  This prevents accidental
    duplicate submissions (by clicking refresh or back/forward) and
    avoids browser warnings about "resubmitting form data".  Redirects
    may be issued to the browser by setting the connection data buffer to the
    destination file or URL, and the connection httpStatus
    (TCPIP_HTTP_NET_ConnectionStatusSet) to TCPIP_HTTP_NET_STAT_REDIRECT.

    Finally, this function may set cookies.  Set the connection data buffer
    to a series of name/value string pairs (in the same format in which parameters
    arrive), and then set the connection hasArgs (TCPIP_HTTP_NET_ConnectionHasArgsSet)
    equal to the number of cookie name/value pairs.
    The cookies will be transmitted to the browser, and any future requests
    will have those values available in the connection data buffer.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    pCBack      - user TCPIP_HTTP_NET_USER_CALLBACK pointer

  Returns:
    - TCPIP_HTTP_NET_IO_RES_DONE - application is done processing
    - TCPIP_HTTP_NET_IO_RES_NEED_DATA - more data is needed to continue, and this
                                        function should be called again later
    - TCPIP_HTTP_NET_IO_RES_WAITING - the application is waiting for an asynchronous
                                      process to complete, and this function should
                                      be called again later

  Remarks:
    This function is only called when the request method is POST, and is
    only used when TCPIP_HTTP_NET_USE_POST is defined.
  
    This method may NOT write to the network transport buffer.

    This function may service multiple HTTP requests simultaneously.
    Exercise caution when using global or static variables inside this
    routine.
    Use the connection callbackPos (TCPIP_HTTP_NET_ConnectionCallbackPosGet)
    or connection data buffer for storage associated with individual requests.
 */
TCPIP_HTTP_NET_IO_RESULT template_ConnectionPostExecute 
                            (TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                            const TCPIP_HTTP_NET_USER_CALLBACK* pCBack);

//*****************************************************************************
/*
  Function:
    uint8_t template_ConnectionFileAuthenticate 
                       (TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                        const char* cFile, const TCPIP_HTTP_NET_USER_CALLBACK* pCBack)

  Summary:
    Determines if a given file name requires authentication

  Description:
    This function is implemented by the application developer.
    Its function is to determine if a file being
    requested requires authentication to view.  The user name and password,
    if supplied, will arrive later with the request headers, and will be
    processed at that time.

    Return values 0x80 - 0xff indicate that authentication is not required,
    while values from 0x00 to 0x79 indicate that a user name and password
    are required before proceeding.  While most applications will only use a
    single value to grant access and another to require authorization, the
    range allows multiple "realms" or sets of pages to be protected, with
    different credential requirements for each.

    The return value of this function is saved for the current connection
    and can be read using TCPIP_HTTP_NET_ConnectionIsAuthorizedGet.
    It will be available to future callbacks, including 
    template_ConnectionUserAuthenticate and any of the 
    template_ConnectionGetExecute, template_ConnectionPostExecute, 
    or TCPIP_HTTP_NET_Print_varname callbacks.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    cFile       - the name of the file being requested
    pCBack      - user TCPIP_HTTP_NET_USER_CALLBACK pointer

  Returns:
    - <= 0x79 - valid authentication is required
    - >= 0x80 - access is granted for this connection

  Remarks:
    This function may NOT write to the network transport buffer.
 */
uint8_t template_ConnectionFileAuthenticate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                     const char* cFile, const TCPIP_HTTP_NET_USER_CALLBACK* pCBack);

//*****************************************************************************
/*
  Function:
    uint8_t template_ConnectionUserAuthenticate(TCPIP_HTTP_NET_CONN_HANDLE 
                          connHandle, const char* cUser, const char* cPass, 
                          const TCPIP_HTTP_NET_USER_CALLBACK* pCBack)

  Summary:
    Performs validation on a specific user name and password.

  Description:
    This function is implemented by the application developer.
    Its function is to determine if the user name and
    password supplied by the client are acceptable for this resource.
    This callback function can thus to determine if only specific
    user names or passwords will be accepted for this resource.

    Return values 0x80 - 0xff indicate that the credentials were accepted,
    while values from 0x00 to 0x79 indicate that authorization failed.
    While most applications will only use a single value to grant access,
    flexibility is provided to store multiple values in order to
    indicate which user (or user's group) logged in.

    The value returned by this function is stored in the corresponding 
    connection data and will be available with 
    TCPIP_HTTP_NET_ConnectionIsAuthorizedGet in any of the 
    template_ConnectionGetExecute, template_ConnectionPostExecute, or 
	TCPIP_HTTP_NET_Print_varname callbacks.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    cUser       - the user name supplied by the client
    cPass       - the password supplied by the client
    pCBack      - user TCPIP_HTTP_NET_USER_CALLBACK pointer

  Returns:
    - <= 0x79 - the credentials were rejected
    - >= 0x80 - access is granted for this connection

  Remarks:
    This function is only called when an Authorization header is
    encountered.

    This function may NOT write to the network transport buffer.
 */
uint8_t template_ConnectionUserAuthenticate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                          const char* cUser, const char* cPass, 
                                          const TCPIP_HTTP_NET_USER_CALLBACK* pCBack);

//*****************************************************************************
/*
  Function:
    TCPIP_HTTP_DYN_PRINT_RES template_DynPrint(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                           const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, 
                                           const TCPIP_HTTP_NET_USER_CALLBACK* pCBack);


  Summary:
    Inserts dynamic content into a web page

  Description:
    Functions in this style are implemented by the application developer.
    This function generates dynamic content to be
    inserted into web pages.

    The function of this type is called when a dynamic variable is located
    in a web page.  (i.e., ~varname~)  The name between the tilde '~'
    characters is passed as the dynamic variable name.

    The function currently supports int32_t and string parameters.
    The dynamic variable parameters are included in the varDcpt data structure
    that's passed at the time of the call.
    For example, the variable "~myArray(2,6)~" will generate the
    call with:
        varDcpt.dynName = "myArray";
        varDcpt.nArgs = 2;
        varDcpt.dynArgs->argType = TCPIP_HTTP_DYN_ARG_TYPE_INT32;
        varDcpt.dynArgs->argInt32 = 2;
        (varDcpt.dynArgs + 1)->argType = TCPIP_HTTP_DYN_ARG_TYPE_INT32;
        (varDcpt.dynArgs + 1)->argInt32 = 6;

    When called, this function should write its output to the HTTP connection
    using the TCPIP_HTTP_NET_DynamicWrite/TCPIP_HTTP_NET_DynamicWriteString 
    functions.

    In situations where the dynamic variable print function needs to perform 
    additional write operations, or simply needs to be called again, it must 
    return: TCPIP_HTTP_DYN_PRINT_RES_PROCESS_AGAIN/TCPIP_HTTP_DYN_PRINT_RES_AGAIN.
    Typically this is used when outputting large amounts of data that cannot fit 
    into one single buffer write operation or when the data is not available 
    all at once.
    
    An alternative (legacy) mechanism for that, is using the connection callbackPos 
    calls: (TCPIP_HTTP_NET_ConnectionCallbackPosGet/TCPIP_HTTP_NET_ConnectionCallbackPosSet).
    This value will be set to zero before the function is called.
    If the function is managing its output state, it must set this to a non-zero 
    value before returning.
    If the connection callbackPos is non-zero, the function will be called again 
    after the current data is processed (equivalent to returning 
    TCPIP_HTTP_DYN_PRINT_RES_PROCESS_AGAIN).
    Once the callback completes, set the callback value back to zero and return
    TCPIP_HTTP_DYN_PRINT_RES_DONE to resume normal servicing of the request.

    If the function does not process this dynamic variable callback (or it wants 
    the default action to be performed), it should return TCPIP_HTTP_DYN_PRINT_RES_DEFAULT.
    Then the default action for that variable will be performed by the HTTP module.

    Note that this is an advanced feature and will be used for processing of HTTP 
    dynamic variable keywords. No implementation currently exists for user added 
    variables and the default action in this case will do nothing.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    varDcpt     - pointer to a variable descriptor containing the dynamic variable name 
                  and arguments valid within this call context only.
    pCBack      - user TCPIP_HTTP_NET_USER_CALLBACK pointer


  Returns:
    A TCPIP_HTTP_DYN_PRINT_RES specifying the action to be taken. See the 
    TCPIP_HTTP_DYN_PRINT_RES definition for a list of allowed actions

  Remarks:
    This function may service multiple HTTP requests simultaneously,
    especially when managing its output state.  Exercise caution when using
    global or static variables inside this routine.  Use the connection
    callbackPos or the connection data buffer for storage associated with 
	individual requests.

    The varDcpt variable descriptor is valid only in the context of this call.
    Any parameters that are needed for further processing should be copied
    to user storage.

    The varDcpt variable descriptor is constant and should not be modified in any way
    by the callback.

    If the callback returned TCPIP_HTTP_DYN_PRINT_RES_DEFAULT, no further calls 
    will be made to the callback function.

    Returning TCPIP_HTTP_DYN_PRINT_RES_PROCESS_AGAIN/TCPIP_HTTP_DYN_PRINT_RES_AGAIN
    is an alternative mechanism to manipulation the connection status with
    TCPIP_HTTP_NET_ConnectionCallbackPosSet/TCPIP_HTTP_NET_ConnectionCallbackPosGet 
    functions (which mechanism is still valid).
    - If the template_DynPrint() returned TCPIP_HTTP_DYN_PRINT_RES_PROCESS_AGAIN, 
      the HTTP server will start sending the dynamic variable buffer
      (invoked by TCPIP_HTTP_NET_DynamicWrite/TCPIP_HTTP_NET_DynamicWriteString).
      Once sending is done, the HTTP server will again call template_DynPrint 
      regardless of the value set with the callback position.
    - If the template_DynPrint() returned TCPIP_HTTP_DYN_PRINT_RES_AGAIN, 
      there will be no processing of dynamic variable data (potentially written to 
      the connection) but the template_DynPrint will be invoked again.
      Returning TCPIP_HTTP_DYN_PRINT_RES_AGAIN is needed when 
      TCPIP_HTTP_NET_DynamicWrite failed or some condition is not satisfied and the 
      template_DynPrint needs to be called again.

    The callback position mechanism will be evaluated after the template_DynPrint
    returns TCPIP_HTTP_DYN_PRINT_RES_DONE.


 */
TCPIP_HTTP_DYN_PRINT_RES template_DynPrint(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                      const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, 
                                      const TCPIP_HTTP_NET_USER_CALLBACK* pCBack);

//*****************************************************************************
/*
  Function:
    void template_DynAcknowledge(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                  const void* buffer, 
                                  const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);


  Summary:
    Dynamic variable acknowledge function

  Description:
    Functions in this style are implemented by the application developer.
    This function reports that a buffer that was passed as part of a dynamic 
    variable callback (dynamicPrint) was processed and can be reused, freed, etc.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    buffer      - the processed buffer
    pCBack      - user TCPIP_HTTP_NET_USER_CALLBACK pointer


  Returns:
    None.

  Remarks:
    The function is called only when TCPIP_HTTP_NET_DynamicWrite or 
    TCPIP_HTTP_NET_DynamicWriteString was specified with a true needAck 
    parameter. 

    The function is called from withing HTTP context.
    It should be kept as short as possible.

 */
void template_DynAcknowledge(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                          const void* buffer, 
                          const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);

//*****************************************************************************
/*
  Function:
    void template_EventReport(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                           TCPIP_HTTP_NET_EVENT_TYPE evType, const void* evInfo, 
                           const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);


  Summary:
    Reports an event that occurred in the processing of a HTTP web page

  Description:
    Functions in this style are implemented by the application developer.
    This function reports an event that occurred when processing a file, 
    dynamic variable, etc.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    evType      - the HTTP event that occurred
    evInfo      - additional info for that particular event
                    - TCPIP_HTTP_NET_EVENT_FS_UPLOAD_COMPLETE: file name pointer
                    - TCPIP_HTTP_NET_EVENT_OPEN: 0    
                    - TCPIP_HTTP_NET_EVENT_CLOSE_DONE: 0    
                    - TCPIP_HTTP_NET_EVENT_CLOSE_TIMEOUT: 0    
                    - TCPIP_HTTP_NET_EVENT_CLOSE_POST_ERROR: 0    
                    - TCPIP_HTTP_NET_EVENT_CLOSE_REMOTE: 0    
                    - TCPIP_HTTP_NET_EVENT_FILE_OPEN: file name pointer
                    - TCPIP_HTTP_NET_EVENT_FILE_CLOSE: file name pointer

                    - TCPIP_HTTP_NET_EVENT_FILE_OPEN_ERROR: file name pointer
                    - TCPIP_HTTP_NET_EVENT_FILE_NAME_ERROR: 0
                    - TCPIP_HTTP_NET_EVENT_FILE_NAME_SIZE_ERROR: file name pointer
                    - TCPIP_HTTP_NET_EVENT_FILE_SIZE_ERROR: file name pointer
                    - TCPIP_HTTP_NET_EVENT_FILE_READ_ERROR: file name pointer
                    - TCPIP_HTTP_NET_EVENT_FILE_PARSE_ERROR: file name pointer
                    - TCPIP_HTTP_NET_EVENT_DEPTH_ERROR: file name pointer
                    - TCPIP_HTTP_NET_EVENT_DYNVAR_PARSE_ERROR: dynamic variable name pointer
                    - TCPIP_HTTP_NET_EVENT_FS_WRITE_ERROR: file name pointer
                    - TCPIP_HTTP_NET_EVENT_FS_MOUNT_ERROR: file name pointer
                    - TCPIP_HTTP_NET_EVENT_CHUNK_POOL_ERROR: file name pointer
                    - TCPIP_HTTP_NET_EVENT_FILE_BUFFER_POOL_ERROR: file name pointer
                    - TCPIP_HTTP_NET_EVENT_DYNVAR_ALLOC_ERROR: dynamic variable name pointer
                    - TCPIP_HTTP_NET_EVENT_UPLOAD_ALLOC_ERROR: file name pointer
                    - TCPIP_HTTP_NET_EVENT_SSI_PARSE_ERROR: SSI command pointer
                    - TCPIP_HTTP_NET_EVENT_SSI_COMMAND_ERROR: SSI command pointer
                    - TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_ERROR: SSI command pointer
                    - TCPIP_HTTP_NET_EVENT_SSI_ALLOC_DESCRIPTOR_ERROR: SSI command pointer
                    - TCPIP_HTTP_NET_EVENT_PEEK_ALLOC_BUFFER_ERROR: string containing allocation size that failed
                    - TCPIP_HTTP_NET_EVENT_SSI_ALLOC_ECHO_ERROR: string containing allocation size that failed
                    
                    - TCPIP_HTTP_NET_EVENT_DYNVAR_ARG_NAME_TRUNCATED: dynamic variable name pointer
                    - TCPIP_HTTP_NET_EVENT_DYNVAR_ARG_NUMBER_TRUNCATED: dynamic variable name pointer
                    - TCPIP_HTTP_NET_EVENT_DYNVAR_RETRIES_EXCEEDED: dynamic variable name pointer
                    - TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_NUMBER_TRUNCATED: SSI argument pointer
                    - TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_UNKNOWN: SSI attribute pointer
                    - TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_NUMBER_MISMATCH: SSI command pointer
                    - TCPIP_HTTP_NET_EVENT_SSI_VAR_NUMBER_EXCEEDED: SSI variable pointer
                    - TCPIP_HTTP_NET_EVENT_SSI_VAR_UNKNOWN: SSI variable pointer
                    - TCPIP_HTTP_NET_EVENT_SSI_VAR_VOID: SSI variable pointer
                    - TCPIP_HTTP_NET_EVENT_SSI_VAR_DELETED: SSI variable pointer
                    - TCPIP_HTTP_NET_EVENT_CHUNK_POOL_EMPTY: file name pointer 
                    - TCPIP_HTTP_NET_EVENT_FILE_BUFFER_POOL_EMPTY: file name pointer 
                    - TCPIP_HTTP_NET_EVENT_PEEK_BUFFER_SIZE_EXCEEDED: string with excess 
                      number of bytes that cannot be read 

                  Valid within this call context only!
    pCBack      - user TCPIP_HTTP_NET_USER_CALLBACK pointer


  Returns:
    None.

  Remarks:
    The evInfo parameter is valid only in the context of this call.
    Any data that is needed for further processing should be copied
    to user storage.

    This function may NOT write to the network transport buffer.

 */
void template_EventReport(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                 TCPIP_HTTP_NET_EVENT_TYPE evType, const void* evInfo, 
                                 const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);

//*****************************************************************************
/*
  Function:
    bool template_SSINotification(TCPIP_HTTP_NET_CONN_HANDLE connHandle, TCPIP_HTTP_SSI_NOTIFY_DCPT* pSSINotifyDcpt, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);

  Summary:
    Reports an SSI processing event that occurs in the processing of a HTTP web page

  Description:
    Functions in this style are implemented by the application developer.
    This function is used by the HTTP server to report that an SSI
    command line was encountered while processing a file.
    The user has a chance to inspect the line and to provide custom action by using the 
    SSI API functions.

  Precondition:
    SSI processing should be enabled.

  Parameters:
    connHandle      - HTTP connection handle
    pSSINotifyDcpt  - pointer to a SSI notification descriptor containing:
                        fileName    - the name of the file the SSI command belongs to
                        ssiCommand  - the SSI command parsed from the command line
                        nAttribs    - number of attribute descriptors in the command
                        pAttrDcpt   - pointer to an array of descriptors parsed from this SSI command
    pCBack      - user TCPIP_HTTP_NET_USER_CALLBACK pointer


  Returns:
    true    - if the SSI processing is complete and no further action needs to be taken by the HTTP server
    false   - the HTTP should process the SSI command

  Remarks:
    The ssiLine parameter is valid only in the context of this call.
    Any data that is needed for further processing should be copied
    to user storage.

    This function could use the SSI API functions to alter/examine the value of
    SSI variables that are part of the ssiLine.

    This function may NOT write to the network transport buffer.
    Possiblity to write to the transport channel could be eventually added,
    based on the dynamic variable model TCPIP_HTTP_NET_DynamicWrite. 

    The default return value should be false:
    the user function examines/adjusts value of SSI variables
    and instructs the HTTP module continue normally.
    However, the user has the option of supressing the output (SSI "echo")
    or override the value of a variable (SSI "set"), etc.

 */
   
 bool template_SSINotification(TCPIP_HTTP_NET_CONN_HANDLE connHandle, TCPIP_HTTP_SSI_NOTIFY_DCPT* pSSINotifyDcpt, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK* pCBack);

//*****************************************************************************
/*
  Function:
    bool TCPIP_HTTP_NET_DynamicFileInclude(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt,
                                              const char* fileName)

  Summary:
    Dynamically includes/writes a file to the HTTP connection.

  Description:
    This function allows an entire file to be included as part of a dynamic 
    variable processing. This reduces unneeded duplication of visual elements 
    such as headers, menus, etc.
    
    The function is NOT meant for the processing of the "inc" keyword in the web pages!
    (see the Remarks section below).


  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    fileName    - the name of the file to be included

  Returns:
    true if the call succeeded,
    false if the file could not be opened

  Remarks:
    The function allows to insert a file, as part of the dynamic variable processing.
    Should be called ONLY from within a dynamic variable processing function context
    (i.e. from within a dynamicPrint function).
    The connHandle and varDcpt need to be valid handles.

    The included file can contain dynamic variables.
    However there's a limit for recursive calls (see TCPIP_HTTP_NET_MAX_RECURSE_LEVEL).

    The file is just added to the processing queue.
    Returning true does not mean that the whole file has been already sent to the 
    connection.

    If the function returns true but an error occurs during the file processing
    an event will be reported using the template_EventReport callback.

    If the function returns false an event will be reported using the
    template_EventReport callback with additional info.

    Please note that the processing of HTTP dynamic keywords in the HTML code
    such as <c>~inc:filename.ext~</c> is processed internally by the HTTP module!
    For such a dynamic variable, control is not passed to the user.

 */
bool  TCPIP_HTTP_NET_DynamicFileInclude(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, 
                                           const char* fileName);




// *****************************************************************************
// *****************************************************************************
// Section: HTTP Connection Data Manipulation API Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    bool TCPIP_HTTP_NET_DynamicWrite(const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, 
                              const void * buffer, uint16_t size, bool needAck);

  Summary:
    Writes a data buffer to the current connection
    
  Description:
    This function takes a buffer and sends it over the HTTP connection
    as part of the HTTP dynamic variable processing

  Precondition:
    varDcpt     - a valid dynamic variable descriptor.

  Parameters:
    varDcpt    - dynamic variable descriptor as passed in the 
                 template_DynPrint function
    buffer     - The pointer to the persistent buffer to be written to the HTTP 
                 connection as part of this dynamic variable callback
    size       - The number of bytes to be written
    needAck    - if true, once the buffer is processed internally,  
                 template_DynAcknowledge will be called  

  Returns:
    True if the data buffer has been queued for output.
    False if an invalid buffer was provided or the buffer could not be queued
    because of the lack of resources (descriptors).

  Remarks:
    The buffer passed in by the user with this call is queued internally
    using an available dynamic variable buffer descriptor.
    That means that the buffer has to be persistent.
    Once the buffer is processed and sent to output, the dynamicAck callback
    will be called, to inform the user that the corresponding buffer can be
    reused/freed.

    When multiple connections output their dynamic content concurrently
    the HTTP may run out of dynamic variable buffer descriptors that are used 
    in queuing the requests and the call may fail.
    If the call failed, because the buffer could not be queued,
    it may be retried using by returning
    TCPIP_HTTP_DYN_PRINT_RES_AGAIN in the template_DynPrint callback.

    If sequential write calls are done from within the same template_DynPrint 
    call the HTTP module will try to append the new dynamic data to the existent one.

    The number of internal HTTP dynamic variables buffer descriptors is controlled by
    TCPIP_HTTP_NET_DYNVAR_DESCRIPTORS_NUMBER.
    It can be obtained at run-time using the TCPIP_HTTP_NET_ConnectionDynamicDescriptors function.

 */

bool TCPIP_HTTP_NET_DynamicWrite(const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, 
                            const void * buffer, uint16_t size, bool needAck);


//*****************************************************************************
/*
  Function:
    bool TCPIP_HTTP_NET_DynamicWriteString(const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, 
                                           const char* str, bool needAck);

  Summary:
    Helper for writing a string within a dynamic variable context.
    
  Description:
    This function takes a 0 terminated ACII string and sends it to the HTTP 
    connection as part of the HTTP dynamic variable processing

  Precondition:
    varDcpt     - a valid dynamic variable descriptor.

  Parameters:
    varDcpt    - dynamic variable descriptor as passed in the 
                 template_DynPrint function
    str        - The string to be written
    needAck    - if true, once the buffer is processed internally,  
                 template_DynAcknowledge will be called  

  Returns:
    True if the data buffer has been queued for output.
    False if an invalid buffer was provided or the buffer could not be queued
    because of the lack of resources (descriptors).
     
  Remarks:
    This is just a helper.
    The actual function called is still TCPIP_HTTP_NET_DynamicWrite.
    That means that the supplied string has to be persistent.

    See the remarks for TCPIP_HTTP_NET_DynamicWrite.

 */

bool TCPIP_HTTP_NET_DynamicWriteString(const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, 
                                       const char* str, bool needAck);


//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_HTTP_NET_ConnectionFlush(TCPIP_HTTP_NET_CONN_HANDLE connHandle);
  
  Summary:
    Immediately transmits all connection pending TX data.

  Description:
    This function calls the transport layer's flush function

  Precondition:
    connHandle - a valid HTTP connection.

  Parameters:
    connHandle - connection handle

  Returns:
    - number of flushed bytes
    - 0 if no flushed bytes or some error

 */

uint16_t TCPIP_HTTP_NET_ConnectionFlush(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

//******************************************************************************
/*
  Function:
    uint16_t TCPIP_HTTP_NET_ConnectionReadIsReady(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

  Summary:
    Determines how many bytes can be read from the connection RX buffer.

  Description:
    This function determines how many bytes can be read from the
    connection RX buffer.
  
  Precondition:
    connHandle - a valid HTTP connection.

  Parameters:
    connHandle - connection handle

  Returns:
    The number of bytes available to be read from the connection RX buffer.
	
  Remarks:
    When using an encrypted connection the number of available unencrypted bytes
    may turn out to be different than what this function returns.

  */
uint16_t TCPIP_HTTP_NET_ConnectionReadIsReady(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_HTTP_NET_ConnectionRead(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                           void * buffer, uint16_t size);

  Summary:
    Reads an array of data bytes from a connection's RX buffer.

  Description:
    This function reads an array of data bytes from the connection RX buffer.  
	The data is removed from the FIFO in the process.

  Precondition:
    connHandle - a valid HTTP connection.

  Parameters:
    connHandle - connection handle
    buffer - The pointer to the array to store data that was read
    size   - The number of bytes to be read.

  Returns:
    The number of bytes read from the socket.
    If less than len, the connection RX buffer became empty
    or the underlying socket is not connected.

  Remarks:
    If the supplied buffer is null, the data is simply discarded.

 */
uint16_t TCPIP_HTTP_NET_ConnectionRead(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                                      void * buffer, uint16_t size);

//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_HTTP_NET_ConnectionPeek(TCPIP_HTTP_NET_CONN_HANDLE connHandle,  
                void * buffer, uint16_t size);

  Summary:
    Reads a specified number of data bytes from the connection RX buffer
    without removing them from the buffer.

  Description:
    The function allows peeking into the connection buffer.
    The data will still be available for a next read operation.

  Precondition:
    connHandle - a valid HTTP connection.

  Parameters:
    connHandle - connection handle
    buffer - Destination to write the peeked data bytes
    size   - Length of bytes to peek from the connection RX buffer 
  

  Return Values:
    The number of bytes actually peeked from the stream and copied to the buffer.

  Remarks:
    None
 */

uint16_t TCPIP_HTTP_NET_ConnectionPeek(TCPIP_HTTP_NET_CONN_HANDLE connHandle,  
                                       void * buffer, uint16_t size);

//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_HTTP_NET_ConnectionDiscard(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

  Summary:
    Discards any pending data in the connection RX buffer.
  
  Description:
    This function discards data from the connection RX buffer.

  Precondition:
    connHandle - a valid HTTP connection.

  Parameters:
    connHandle - connection handle

  Returns:
    The number of bytes that have been discarded from the RX buffer.

 */
uint16_t TCPIP_HTTP_NET_ConnectionDiscard(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_HTTP_NET_ConnectionReadBufferSize(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

  Summary:
    Returns the size of the connection RX buffer.
  
  Description:
    This function discards data from the connection RX buffer.

  Precondition:
    connHandle - a valid HTTP connection.

  Parameters:
    connHandle - connection handle

  Returns:
    The size of the connection RX buffer.

 */
uint16_t TCPIP_HTTP_NET_ConnectionReadBufferSize(TCPIP_HTTP_NET_CONN_HANDLE connHandle);


//*****************************************************************************
/*
  Function:
    TCPIP_NET_HANDLE TCPIP_HTTP_NET_ConnectionNetHandle(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

  Summary:
    Returns the network handle of the current connection.
  
  Description:
    This function returns the network handle over which the current HTTP connection 
    communicates.

  Precondition:
    connHandle - a valid HTTP connection.

  Parameters:
    connHandle - connection handle

  Returns:
    The connection network handle.

 */
TCPIP_NET_HANDLE TCPIP_HTTP_NET_ConnectionNetHandle(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    NET_PRES_SKT_HANDLE_T  TCPIP_HTTP_NET_ConnectionSocketGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Get the socket for the current connection.

  Description:
    The function returns the network transport socket of the specified HTTP 
    connection.
    The user gets access to the connection socket which it can use for debugging 
    or directly sending/reading data.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    NET_PRES_SKT_HANDLE_T for the connection defined by connHandle.

  Example:
  <code>
    uint32_t byteCount;
    int sktRxSize;

    byteCount = TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle);
    sktRxSize = TCPIP_HTTP_NET_ConnectionReadBufferSize(connHandle);

    if(byteCount > sktRxSize)
    {   // Configuration Failure
        TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, TCPIP_HTTP_NET_STAT_REDIRECT);
        return TCPIP_HTTP_NET_IO_RES_DONE;
    }
  </code>

  Remarks:
    This function gives direct access to the underlying transport socket.
    It is meant for test/advanced usage only.
    The regular connection functions should be used for manipulation of the connection data.
    Using the socket directly for data manipulation will disrupt the HTTP server functionality.

 */
NET_PRES_SKT_HANDLE_T  TCPIP_HTTP_NET_ConnectionSocketGet(TCPIP_HTTP_NET_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_HTTP_NET_ConnectionStringFind(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                               const char* str, uint16_t startOffs, uint16_t searchLen);

  Summary:
    Helper to find a string of characters in an incoming connection buffer.

  Description:
    This function searches for an ASCIIZ string in the RX buffer of the connection.
    It does the search by performing a peek operation in the RX buffer,
    (i.e., the RX data in the buffer is not consumed and it is available for further read
    operations).
    It works for both encrypted and unencrypted connections.
   
  Precondition:
    connHandle - a valid HTTP connection.

  Parameters:
    connHandle  - HTTP connection handle
    str         - 0 terminated ASCII string to search for
    startOffs   - offset in the RX buffer to start the search from
    searchLen   - if !0 it is the length of buffer to search into (starting from startOffs);
                  if 0, the whole buffer is searched


  Returns:
    - If string was found - a zero-indexed position in the RX buffer of the string 
                            occurrence
    - 0xFFFF - Search array not found

  Remarks:
    This function currently performs the search by doing a whole buffer peek into one single operation.
    Note that the search will fail if there's more data in the TCP socket than could be read at once.

 */
uint16_t TCPIP_HTTP_NET_ConnectionStringFind(TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
                          const char* str, uint16_t startOffs, uint16_t searchLen);



// *****************************************************************************
// *****************************************************************************
// Section: HTTP SSI Variables Manipulation API Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    const char*   TCPIP_HTTP_NET_SSIVariableGet(const char* varName, TCPIP_HTTP_DYN_ARG_TYPE* pVarType, int32_t* pVarInt);

  Summary:
    Function to get access to an existing SSI variable.

  Description:
    This function performs a search for the corresponding SSI variable name
    and returns a pointer to the variable string representation.


  Precondition:
    The HTTP module should have been initialized.
    SSI should be enabled.

  Parameters:
    varName     - the SSI variable to search for
    pVarType    - address to store the type of the SSI variable
    pVarInt     - address to store the integer value of this variable, if the variable exists and is an integer
                  else this address won't be modified
                  Could be NULL if not needed

  Returns:
    a valid pointer to the SSI variable string representation if the variable was found
    0 if there is no variable with such name

  Remarks:
    The returned value points to the internal SSI variable representation.
    This pointer should not be used for changing the SSI variable value.

*/
const char*   TCPIP_HTTP_NET_SSIVariableGet(const char* varName, TCPIP_HTTP_DYN_ARG_TYPE* pVarType, int32_t* pVarInt);

// *****************************************************************************
/*
  Function:
    bool   TCPIP_HTTP_NET_SSIVariableSet(const char* varName, TCPIP_HTTP_DYN_ARG_TYPE varType, const char* strValue, int32_t intValue);

  Summary:
    Function to set an SSI variable.

  Description:
    This function sets the new values for an SSI variable.
    If a variable with such name does not exist, it is created.


  Precondition:
    The HTTP module should have been initialized.
    SSI should be enabled.

  Parameters:
    varName     - the SSI variable name
    varType     - the type of the SSI variable
    strValue    - pointer to the string representation of the variable
    intValue    - the integer value of the variable, if the type is integer.


  Returns:
    true    - if the variable was updated or created successfully
    false   - the variable didn't exist and the attempt to create it failed
              (all slots already in use. Increase TCPIP_HTTP_NET_SSI_VARIABLES_NUMBER).


  Remarks:
    The string variable interpretation is needed even if the variable is of integer type.
    HTTP module will use that representation instead of doing a conversion to string of the intValue.

    The string representation should not exceed TCPIP_HTTP_NET_SSI_VARIABLE_STRING_MAX_LENGTH.
    Any excess variables will be truncated.

*/
bool   TCPIP_HTTP_NET_SSIVariableSet(const char* varName, TCPIP_HTTP_DYN_ARG_TYPE varType, const char* strValue, int32_t intValue);

// *****************************************************************************
/*
  Function:
    bool   TCPIP_HTTP_NET_SSIVariableDelete(const char* varName);

  Summary:
    Function to delete an SSI variable.

  Description:
    This function deletes an SSI variable if it exists.


  Precondition:
    The HTTP module should have been initialized.
    SSI should be enabled.

  Parameters:
    varName     - the SSI variable name

  Returns:
    true    - if the variable existed and was deleted 
    false   - the variable didn't exist

  Remarks:
    None.

*/
bool   TCPIP_HTTP_NET_SSIVariableDelete(const char* varName);

// *****************************************************************************
/*
  Function:
    int   TCPIP_HTTP_NET_SSIVariablesNumberGet(int* pMaxNo);

  Summary:
    Function to get the number of the current SSI variables.

  Description:
    This function returns the number of SSI variables that currently exist.
    It will also return the maximum number of variables that the SSI can hold.


  Precondition:
    The HTTP module should have been initialized.
    SSI should be enabled.

  Parameters:
    pMaxNo    - pointer to store the maximum number of SSI variables
                that can exist
                Can be NULL if not needed 


  Returns:
    The number of current SSI variables that are in use


  Remarks:
    None.

*/
int   TCPIP_HTTP_NET_SSIVariablesNumberGet(int* pMaxNo);

// *****************************************************************************
/*
  Function:
    const char*   TCPIP_HTTP_NET_SSIVariableGetByIndex(int varIndex, const char** pVarName, TCPIP_HTTP_DYN_ARG_TYPE* pVarType, int32_t* pVarInt);

  Summary:
    Function to get access to an existing SSI variable.

  Description:
    This function accesses the corresponding SSI variable by its index
    and returns a pointer to the variable string representation.


  Precondition:
    The HTTP module should have been initialized.
    SSI should be enabled.

  Parameters:
    varIndex    - the index of the SSI variable to search for
                  Should be < TCPIP_HTTP_NET_SSIVariablesNumberGet()
    pVarName    - address to store a pointer to the variable name
    pVarType    - address to store the type of the SSI variable
    pVarInt     - address to store the integer value of this variable, if the variable exists and is an integer
                  else this address won't be modified
                  Could be NULL if not needed

  Returns:
    a valid pointer to the SSI variable string representation if the variable was found
    0 if there is no variable with such name

  Remarks:
    The returned value points to the internal SSI variable representation.
    This pointer should not be used for changing the SSI variable value.

    The pVarName points to the internal SSI variable name representation.
    This pointer MUST NOT be used for changing the SSI variable name.

  Remarks:
    None.

*/
const char*   TCPIP_HTTP_NET_SSIVariableGetByIndex(int varIndex, const char** pVarName, TCPIP_HTTP_DYN_ARG_TYPE* pVarType, int32_t* pVarInt);


// *****************************************************************************
// *****************************************************************************
// Section: HTTP Service Task function Prototype
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    void  TCPIP_HTTP_NET_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs HTTP module tasks in the TCP/IP stack.

  Precondition:
    The HTTP module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_HTTP_NET_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __HTTP_NET_H_

