/*******************************************************************************
  HTTP Headers for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    http.h

  Summary:
    The HTTP web server module together with a file system (SYS_FS) allow
    the board to act as a web server.
    
  Description:
    The HTTP module runs a web server within the TCP/IP stack.
    This facilitates an easy method to view status information
    and control applications using any standard web browser.

*******************************************************************************/
//DOM-IGNORE-BEGIN
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








//DOM-IGNORE-END

#ifndef __HTTP_H_
#define __HTTP_H_

#include "system/fs/sys_fs.h"

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
    HTTP_GET = 0u,               // GET command is being processed
    HTTP_POST,                   // POST command is being processed
    HTTP_BAD_REQUEST,            // 400 Bad Request will be returned
    HTTP_UNAUTHORIZED,           // 401 Unauthorized will be returned
    HTTP_NOT_FOUND,              // 404 Not Found will be returned
    HTTP_OVERFLOW,               // 414 Request-URI Too Long will be returned
    HTTP_INTERNAL_SERVER_ERROR,  // 500 Internal Server Error will be returned
    HTTP_NOT_IMPLEMENTED,        // 501 Not Implemented (not a GET or POST command)
    HTTP_REDIRECT,               // 302 Redirect will be returned
    HTTP_SSL_REQUIRED,           // 403 Forbidden is returned, indicating SSL is required

    HTTP_MPFS_FORM,              // Show the MPFS Upload form
    HTTP_MPFS_UP,                // An MPFS Upload is being processed
    HTTP_MPFS_SIGNATURE,         // MPFS Upload looking for signature
    HTTP_MPFS_OK,                // An MPFS Upload was successful  
	HTTP_MPFS_WAIT,              // An MPFS Upload waiting for the write operation to complete
	HTTP_MPFS_ERROR,             // An MPFS Upload was not a valid image

} HTTP_STATUS;

// *****************************************************************************
// *****************************************************************************
// Section: HTTP Definitions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************

// Result states for execution callbacks
typedef enum
{
    HTTP_IO_DONE = 0u,  // Finished with procedure
    HTTP_IO_NEED_DATA,  // More data needed to continue, call again later
    HTTP_IO_WAITING     // Waiting for asynchronous process to complete, call again later

} HTTP_IO_RESULT;

// Result states for  TCPIP_HTTP_PostNameRead, TCPIP_HTTP_PostValueRead and TCPIP_HTTP_PostReadPair 
typedef enum
{
    HTTP_READ_OK = 0u,      // Read was successful
    HTTP_READ_TRUNCATED,    // Buffer overflow prevented by truncating value
    HTTP_READ_INCOMPLETE    // Entire object is not yet in the buffer.  Try again later.

} HTTP_READ_STATUS;

// File type definitions
typedef enum
{
    HTTP_TXT = 0u,      // File is a text document
    HTTP_HTM,           // File is HTML (extension .htm)
    HTTP_HTML,          // File is HTML (extension .html)
    HTTP_CGI,           // File is HTML (extension .cgi)
    HTTP_XML,           // File is XML (extension .xml)
    HTTP_CSS,           // File is stylesheet (extension .css)
    HTTP_GIF,           // File is GIF image (extension .gif)
    HTTP_PNG,           // File is PNG image (extension .png)
    HTTP_JPG,           // File is JPG image (extension .jpg)
    HTTP_JS,            // File is java script (extension .js)
    HTTP_JAVA,          // File is java (extension .class)
    HTTP_WAV,           // File is audio (extension .wav)
    HTTP_UNKNOWN        // File type is unknown

} HTTP_FILE_TYPE;

// HTTP connection identifier, handle of a HTTP connection
typedef const void*     HTTP_CONN_HANDLE;

// HTTP module configuration flags
// Multiple flags can be OR-ed
typedef enum
{
    HTTP_MODULE_FLAG_DEFAULT             = 0x00, // Default flags value

    HTTP_MODULE_FLAG_ADJUST_SKT_FIFOS    = 0x01, // Adjust corresponding socket FIFO at run time.
                                                 // Improves throughput when the socket buffers are small.
    HTTP_MODULE_FLAG_NO_DELAY            = 0x02, // Create the HTTP sockets with NO-DELAY option.
                                                 // It will flush data as soon as possible.
}HTTP_MODULE_FLAGS;

// HTTP module dynamic configuration data
typedef struct
{
    uint16_t    nConnections;   // number of simultaneous HTTP connections allowed
    uint16_t    nTlsConnections; // Not used in the current implementation;
                                // Number of simultaneous HTTPS connections allowed
    uint16_t    dataLen;        // size of the data buffer for reading cookie and GET/POST arguments (bytes)
    uint16_t    sktTxBuffSize;  // size of TX buffer for the associated socket; leave 0 for default
    uint16_t    sktRxBuffSize;  // size of RX buffer for the associated socket; leave 0 for default
    uint16_t    tlsSktTxBuffSize;  // Not used in the current implementation;
                                // Size of TLS TX buffer for the associated socket; leave 0 for default (min 512 bytes)
    uint16_t    tlsSktRxBuffSize;  // Not used in the current implementation;
                                // Size of TLS RX buffer for the associated socket; leave 0 for default (min 512 bytes)
    uint16_t    configFlags;    // a HTTP_MODULE_FLAGS value.
    // HTTP specific allocation functions
    // These functions are used only for:
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

} TCPIP_HTTP_MODULE_CONFIG;

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

//*****************************************************************************
/*
  Function:
    uint8_t* TCPIP_HTTP_URLDecode(uint8_t* cData)

  Summary:
    Parses a string from URL encoding to plain-text.

  Description:
    This function parses a string from URL encoding to plain-text.  The following
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
uint8_t*  TCPIP_HTTP_URLDecode(uint8_t* cData);


//*****************************************************************************
/*
  Function:
    const uint8_t* TCPIP_HTTP_ArgGet(const uint8_t* cData, const uint8_t* cArg)

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
    void TCPIP_HTTP_Print_cookiename(HTTP_CONN_HANDLE connHandle)
    {
        const uint8_t *ptr;
        TCP_SOCKET sktHTTP;

        ptr = TCPIP_HTTP_ArgGet(TCPIP_HTTP_CurrentConnectionDataBufferGet(connHandle), (const uint8_t*)"name");
        sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
        if(ptr)
            TCPIP_TCP_StringPut(sktHTTP, ptr);
        else
            TCPIP_TCP_StringPut(sktHTTP, (const uint8_t*)"not set");
    }
  </code>

  Remarks:
    None.
 */
const uint8_t*      TCPIP_HTTP_ArgGet(const uint8_t* cData, const uint8_t* cArg);


//*****************************************************************************
/*
  Function:
    void TCPIP_HTTP_FileInclude(HTTP_CONN_HANDLE connHandle, const uint8_t* cFile)

  Summary:
    Writes a file byte-for-byte to the currently loaded TCP socket.

  Description:
    This function allows an entire file to be included as a dynamic variable, providing
    a basic templating system for HTML web pages.  This reduces unneeded
    duplication of visual elements such as headers, menus, etc.

    When pHttpCon->callbackPos is 0, the file is opened and as many bytes
    as possible are written.  The current position is then saved to
    pHttpCon->callbackPos and the file is closed.  On subsequent calls,
    reading begins at the saved location and continues.  Once the end of
    the input file is reached, pHttpCon->callbackPos is set back to 0 to
    indicate completion.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    cFile - the name of the file to be sent

  Returns:
    None.

  Remarks:
    Users should not call this function directly, but should instead add
    dynamic variables in the form of <c>~inc:filename.ext~</c> in their HTML code
    to include (for example) the file "filename.ext" at that specified
    location.  The mpfs2.jar utility will handle the rest.
 */
void  TCPIP_HTTP_FileInclude(HTTP_CONN_HANDLE connHandle, const uint8_t* cFile);


//*****************************************************************************
/*
  Function:
    HTTP_READ_STATUS TCPIP_HTTP_PostNameRead(HTTP_CONN_HANDLE connHandle, uint8_t* cData, uint16_t wLen)

  Summary:
    Reads a name from a URL encoded string in the TCP buffer.

  Description:
    This function reads a name from a URL encoded string in the TCP buffer.  This function
    is meant to be called from an TCPIP_HTTP_PostExecute callback to facilitate
    easier parsing of incoming data.  This function also prevents buffer
    overflows by forcing the programmer to indicate how many bytes are
    expected.  At least two extra bytes are needed in cData over the maximum
    length of data expected to be read.

    This function will read until the next '=' character, which indicates the
    end of a name parameter.  It assumes that the front of the buffer is
    the beginning of the name parameter to be read.

    This function properly updates pHttpCon->byteCount by decrementing it
    by the number of bytes read.  It also removes the delimiting '=' from
    the buffer.

  Precondition:
    The front of the TCP buffer is the beginning of a name parameter, and the rest of
    the TCP buffer contains a URL-encoded string with a name parameter
    terminated by a '=' character.

  Parameters:
    connHandle  - HTTP connection handle
    cData - where to store the name once it is read
    wLen - how many bytes can be written to cData

  Returns:
    - HTTP_READ_OK - name was successfully read
    - HTTP_READ_TRUNCTATED - entire name could not fit in the buffer, so the
                            value was truncated and data has been lost
    - HTTP_READ_INCOMPLETE - entire name was not yet in the buffer, so call
                            this function again later to retrieve

  Remarks:
    None.
 */
HTTP_READ_STATUS    TCPIP_HTTP_PostNameRead(HTTP_CONN_HANDLE connHandle, uint8_t* cData, uint16_t wLen);


//*****************************************************************************
/*
  Function:
    HTTP_READ_STATUS TCPIP_HTTP_PostValueRead(HTTP_CONN_HANDLE connHandle, uint8_t* cData, uint16_t wLen)

  Summary:
    Reads a value from a URL encoded string in the TCP buffer.

  Description:
    This function reads a value from a URL encoded string in the TCP buffer.  This function
    is meant to be called from an TCPIP_HTTP_PostExecute callback to facilitate
    easier parsing of incoming data.  This function also prevents buffer
    overflows by forcing the programmer to indicate how many bytes are
    expected.  At least 2 extra bytes are needed in cData above the maximum
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
    The front of the TCP buffer is the beginning of a name parameter, and the rest of
    the TCP buffer contains a URL-encoded string with a name parameter
    terminated by a '=' character.

  Parameters:
    connHandle  - HTTP connection handle
    cData - where to store the value once it is read
    wLen - how many bytes can be written to cData

  Returns:
    - HTTP_READ_OK - value was successfully read
    - HTTP_READ_TRUNCTATED - entire value could not fit in the buffer, so the
                            value was truncated and data has been lost
    - HTTP_READ_INCOMPLETE - entire value was not yet in the buffer, so call
                            this function again later to retrieve

  Remarks:
    None.
 */
HTTP_READ_STATUS  TCPIP_HTTP_PostValueRead(HTTP_CONN_HANDLE connHandle, uint8_t* cData, uint16_t wLen);


//*****************************************************************************
/*
  Function:
    FILE_HANDLE  TCPIP_HTTP_CurrentConnectionFileGet(HTTP_CONN_HANDLE connHandle)

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
    SYS_FS_FileRead(myBuff, sizeof(myBuff), TCPIP_HTTP_CurrentConnectionFileGet(connHandle));
  </code>

  Remarks:
    None.
 */
SYS_FS_HANDLE  TCPIP_HTTP_CurrentConnectionFileGet(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    uint16_t  TCPIP_HTTP_CurrentConnectionPostSmGet(HTTP_CONN_HANDLE connHandle)

  Summary:
    Get the POST state machine state.

  Description:
    This function returns the POST state machine state for the connection defined by connHandle.
    This state is maintained by the HTTP connection and can be used by the user of the
    HTTP to maintain its own POST state machine.
    The values of the POST state machine have significance only for the user of the HTTP connection.

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
  
    switch(TCPIP_HTTP_CurrentConnectionPostSmGet(connHandle))
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
uint16_t  TCPIP_HTTP_CurrentConnectionPostSmGet(HTTP_CONN_HANDLE connHandle);


//*****************************************************************************
/*
  Function:
    void  TCPIP_HTTP_CurrentConnectionPostSmSet(HTTP_CONN_HANDLE connHandle, uint16_t state)

  Summary:
    Set the POST state machine state.

  Description:
    This function sets the POST state machine state for the connection defined by connHandle.
    This state is maintained by the HTTP connection and can be used by the user of the
    HTTP to maintain its own POST state machine.
    The values of the POST state machine have significance only for the user of the HTTP connection.

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

    switch(TCPIP_HTTP_CurrentConnectionPostSmGet(connHandle))
    {
        // Find the name
        case SM_POST_LCD_READ_NAME:

            // Read a name
            httpBuffSize = TCPIP_HTTP_CurrentConnectionDataBufferSizeGet(connHandle);
            if(TCPIP_HTTP_PostNameRead(connHandle, httpDataBuff, httpBuffSize) == HTTP_READ_INCOMPLETE)
                return HTTP_IO_NEED_DATA;

            TCPIP_HTTP_CurrentConnectionPostSmSet(connHandle, SM_POST_LCD_READ_VALUE);
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
void  TCPIP_HTTP_CurrentConnectionPostSmSet(HTTP_CONN_HANDLE connHandle, uint16_t state);


//*****************************************************************************
/*
  Function:
    uint8_t*  TCPIP_HTTP_CurrentConnectionDataBufferGet(HTTP_CONN_HANDLE connHandle)

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
    void TCPIP_HTTP_Print_cookiename(HTTP_CONN_HANDLE connHandle)
    {
        const uint8_t *ptr;
        TCP_SOCKET sktHTTP;

        ptr = TCPIP_HTTP_ArgGet(TCPIP_HTTP_CurrentConnectionDataBufferGet(connHandle), (const uint8_t*)"name");
        sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
        if(ptr)
            TCPIP_TCP_StringPut(sktHTTP, ptr);
        else
            TCPIP_TCP_StringPut(sktHTTP, (const uint8_t*)"not set");
    }
  </code>

  Remarks:
    None.
 */
uint8_t*  TCPIP_HTTP_CurrentConnectionDataBufferGet(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    uint16_t  TCPIP_HTTP_CurrentConnectionDataBufferSizeGet(HTTP_CONN_HANDLE connHandle)

  Summary:
    Returns the size of the connection general purpose data buffer.

  Description:
    This function returns the size of the HTTP connection internal data buffer.
    This buffer is created at the HTTP initialization using the size specified in 
    TCPIP_HTTP_MODULE_CONFIG.dataLen .

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    Size of the connection's general purpose data buffer.

  Example:
  <code>
  </code>

  Remarks:
    Currently the size of the buffer is the same for all connections.
    The connHandle parameter is not used.

 */
uint16_t  TCPIP_HTTP_CurrentConnectionDataBufferSizeGet(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    uint32_t  TCPIP_HTTP_CurrentConnectionCallbackPosGet(HTTP_CONN_HANDLE connHandle)

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
    uint32_t callbackPos;
    callbackPos = TCPIP_HTTP_CurrentConnectionCallbackPosGet(connHandle);
    if(callbackPos == 0x00u)
        callbackPos = (uint32_t)DDNSClient.Host.szRAM;
    callbackPos = (uint32_t)TCPIP_TCP_StringPut(TCPIP_HTTP_CurrentConnectionSocketGet(connHandle), (uint8_t*)callbackPos);
    if(*(uint8_t*)callbackPos == '\0')
        callbackPos = 0x00;
    TCPIP_HTTP_CurrentConnectionCallbackPosSet(connHandle, callbackPos);
  </code>

  Remarks:
    None.
 */
uint32_t  TCPIP_HTTP_CurrentConnectionCallbackPosGet(HTTP_CONN_HANDLE connHandle);


//*****************************************************************************
/*
  Function:
    void  TCPIP_HTTP_CurrentConnectionCallbackPosSet(HTTP_CONN_HANDLE connHandle, uint32_t callbackPos)

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
    void TCPIP_HTTP_Print_builddate(HTTP_CONN_HANDLE connHandle)
    {
        TCP_SOCKET sktHTTP;
        sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);

        TCPIP_HTTP_CurrentConnectionCallbackPosSet(connHandle, 0x01);
        if(TCPIP_TCP_PutIsReady(sktHTTP) < strlen((const char*)__DATE__" "__TIME__))
        { // Don't have room to output build date and time
            return;
        }
        TCPIP_HTTP_CurrentConnectionCallbackPosSet(connHandle, 0x00);
        TCPIP_TCP_StringPut(sktHTTP, (const void*)__DATE__" "__TIME__);
    }
  </code>

  Remarks:
    None.
 */
void  TCPIP_HTTP_CurrentConnectionCallbackPosSet(HTTP_CONN_HANDLE connHandle, uint32_t callbackPos);

//*****************************************************************************
/*
  Function:
    HTTP_STATUS TCPIP_HTTP_CurrentConnectionStatusGet(HTTP_CONN_HANDLE connHandle);

  Summary:
    Gets HTTP status.

  Description:
    This function returns the current HTTP status of the selected HTTP connection.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    A HTTP_STATUS value.

  Example:
  <code>
    HTTP_STATUS currStat =  TCPIP_HTTP_CurrentConnectionStatusGet(connHandle);
  </code>

  Remarks:
    None.
 */
HTTP_STATUS         TCPIP_HTTP_CurrentConnectionStatusGet(HTTP_CONN_HANDLE connHandle);



//*****************************************************************************
/*
  Function:
    void TCPIP_HTTP_CurrentConnectionStatusSet(HTTP_CONN_HANDLE connHandle, HTTP_STATUS stat)

  Summary:
    Sets HTTP status.

  Description:
    Allows write access to the HTTP status of the selected HTTP connection.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    stat        - new HTTP_STATUS enumeration value.

  Returns:
    None.

  Example:
  <code>
    byteCount = TCPIP_HTTP_CurrentConnectionByteCountGet(connHandle);
    sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    if(byteCount > TCPIP_TCP_GetIsReady(sktHTTP) + TCPIP_TCP_FifoRxFreeGet(sktHTTP))
    {   // Configuration Failure
        // 302 Redirect will be returned
        TCPIP_HTTP_CurrentConnectionStatusSet(connHandle, HTTP_REDIRECT);
    }
  </code>

  Remarks:
    None.
 */
void  TCPIP_HTTP_CurrentConnectionStatusSet(HTTP_CONN_HANDLE connHandle, HTTP_STATUS stat);


//*****************************************************************************
/*
  Function:
    uint8_t TCPIP_HTTP_CurrentConnectionHasArgsGet(HTTP_CONN_HANDLE connHandle)

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
    uint8_t hasArgs = TCPIP_HTTP_CurrentConnectionHasArgsGet(connHandle);
  </code>

  Remarks:
    None.
 */
uint8_t        TCPIP_HTTP_CurrentConnectionHasArgsGet(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    void TCPIP_HTTP_CurrentConnectionHasArgsSet(HTTP_CONN_HANDLE connHandle, uint8_t args)

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
        TCPIP_HTTP_CurrentConnectionHasArgsSet(connHandle, 1);
    }
  </code>

  Remarks:
    None.
 */
void                TCPIP_HTTP_CurrentConnectionHasArgsSet(HTTP_CONN_HANDLE connHandle, uint8_t args);


//*****************************************************************************
/*
  Function:
    uint32_t TCPIP_HTTP_CurrentConnectionByteCountGet(HTTP_CONN_HANDLE connHandle)

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
    switch(TCPIP_HTTP_CurrentConnectionPostSmGet(connHandle))
    {
        case SM_CFG_SNMP_READ_NAME:
            // If all parameters have been read, end
            if(TCPIP_HTTP_CurrentConnectionByteCountGet(connHandle) == 0u)
            {
                return HTTP_IO_DONE;
            }
         .
         .
         .
    }
  </code>

  Remarks:
    None.
 */
uint32_t  TCPIP_HTTP_CurrentConnectionByteCountGet(HTTP_CONN_HANDLE connHandle);


//*****************************************************************************
/*
  Function:
    void  TCPIP_HTTP_CurrentConnectionByteCountSet(HTTP_CONN_HANDLE connHandle, uint32_t byteCount)

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
void  TCPIP_HTTP_CurrentConnectionByteCountSet(HTTP_CONN_HANDLE connHandle, uint32_t byteCount);


//*****************************************************************************
/*
  Function:
    void  TCPIP_HTTP_CurrentConnectionByteCountDec(HTTP_CONN_HANDLE connHandle, uint32_t byteCount)

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
void  TCPIP_HTTP_CurrentConnectionByteCountDec(HTTP_CONN_HANDLE connHandle, uint32_t byteCount);


//*****************************************************************************
/*
  Function:
    TCP_SOCKET  TCPIP_HTTP_CurrentConnectionSocketGet(HTTP_CONN_HANDLE connHandle)

  Summary:
    Get the socket for the current connection.

  Description:
    The function returns the TCP socket of the specified HTTP connection.
    The user gets access to the connection socket which it can use
    for sending/reading data.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    TCP_SOCKET for the connection defined by connHandle.

  Example:
  <code>
    uint32_t byteCount;
    TCP_SOCKET sktHTTP;

    byteCount = TCPIP_HTTP_CurrentConnectionByteCountGet(connHandle);
    sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    if(byteCount > TCPIP_TCP_GetIsReady(sktHTTP) + TCPIP_TCP_FifoRxFreeGet(sktHTTP))
    {   // Configuration Failure
        TCPIP_HTTP_CurrentConnectionStatusSet(connHandle, HTTP_REDIRECT);
        return HTTP_IO_DONE;
    }
  </code>

  Remarks:
    None.
 */
TCP_SOCKET  TCPIP_HTTP_CurrentConnectionSocketGet(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    uint8_t   TCPIP_HTTP_CurrentConnectionIsAuthorizedGet(HTTP_CONN_HANDLE connHandle)

  Summary:
    Gets the authorized state for the current connection.

  Description:
    This function returns the authorization status for the current HTTP connection.
    This is one of the values returned by the TCPIP_HTTP_FileAuthenticate() function.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    A uint8_t representing the authorization status.

  Example:
  <code>
    uint8_t isAuth;

    isAuth = TCPIP_HTTP_CurrentConnectionIsAuthorizedGet(connHandle);
  </code>

  Remarks:
    None.
 */
uint8_t   TCPIP_HTTP_CurrentConnectionIsAuthorizedGet(HTTP_CONN_HANDLE connHandle);


//*****************************************************************************
/*
  Function:
    void   TCPIP_HTTP_CurrentConnectionIsAuthorizedSet(HTTP_CONN_HANDLE connHandle, uint8_t auth)

  Summary:
    Sets the authorized state for the current connection.

  Description:
    This function sets the authorization status for the current HTTP connection.
    This has to be one of the values in the set returned by the TCPIP_HTTP_FileAuthenticate() function.

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

    TCPIP_HTTP_CurrentConnectionIsAuthorizedSet(connHandle, auth);
  </code>

  Remarks:
    None.
 */
void   TCPIP_HTTP_CurrentConnectionIsAuthorizedSet(HTTP_CONN_HANDLE connHandle, uint8_t auth);


//*****************************************************************************
/*
  Function:
    void    TCPIP_HTTP_CurrentConnectionUserDataSet(HTTP_CONN_HANDLE connHandle, const void* uData)

  Summary:
    Sets the user data parameter for the current connection.

  Description:
    This function will set the user data value for the current HTTP connection.
    This data belongs to the user and is not used in any way by the HTTP server module.
    It is available to the user by calling TCPIP_HTTP_CurrentConnectionUserDataGet.

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

    TCPIP_HTTP_CurrentConnectionUserDataSet(connHandle, (const void*)myConnData);
  </code>

  Remarks:
    None.
 */
void    TCPIP_HTTP_CurrentConnectionUserDataSet(HTTP_CONN_HANDLE connHandle, const void* uData);


//*****************************************************************************
/*
  Function:
    const void*    TCPIP_HTTP_CurrentConnectionUserDataGet(HTTP_CONN_HANDLE connHandle)

  Summary:
    Gets the user data parameter for the current connection.

  Description:
    This function returns the user data value for the current HTTP connection.
    This data belongs to the user and is not used in any way by the HTTP server module.
    It can be set by the user with TCPIP_HTTP_CurrentConnectionUserDataSet().

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    User data that's stored as part of the connection.

  Example:
  <code>
    uint32_t myConnData;

    myConnData = (uint32_t)TCPIP_HTTP_CurrentConnectionUserDataGet(connHandle);
  </code>

  Remarks:
    None.
 */
const void*    TCPIP_HTTP_CurrentConnectionUserDataGet(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    int    TCPIP_HTTP_CurrentConnectionIndexGet(HTTP_CONN_HANDLE connHandle);

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

    connIx = TCPIP_HTTP_CurrentConnectionIndexGet(connHandle);
  </code>

  Remarks:
    None

 */
int    TCPIP_HTTP_CurrentConnectionIndexGet(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    HTTP_CONN_HANDLE    TCPIP_HTTP_CurrentConnectionHandleGet(int connIx);

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
    HTTP_CONN_HANDLE connHandle;

    connHandle = TCPIP_HTTP_CurrentConnectionHandleGet(0);
  </code>

  Remarks:
    None

 */
HTTP_CONN_HANDLE    TCPIP_HTTP_CurrentConnectionHandleGet(int connIx);

//*****************************************************************************
/*
  Function:
    int    TCPIP_HTTP_ActiveConnectionCountGet(int* pOpenCount);

  Summary:
    Gets the number of active connections.

  Description:
    This function will return the number of active and total HTTP connections
    at the current time.
   
  Precondition:
    None.

  Parameters:
    pOpenCount  - address to store the number of total opened connections
                  Could be NULL if not needed.

  Returns:
    The number of active and total connections.

  Example:
  <code>
    int nConns;

    nConns = TCPIP_HTTP_ActiveConnectionCountGet(0);
  </code>

  Remarks:
    The value returned by this function is informational only.
    The number of active connections changes dynamically.

 */
int    TCPIP_HTTP_ActiveConnectionCountGet(int* pOpenCount);
//*****************************************************************************
/*
  Function:
    const void* TCPIP_HTTP_CurrentConnectionUploadBufferStartGet(HTTP_CONN_HANDLE connHandle);

  Summary:
    Gets the start address of current connection upload buffer.

  Description:
    This function will return the start address of current connection upload buffer.
   
  Precondition:
    None.

  Parameters:
	connHandle  - HTTP connection handle

  Returns:
    the start address of current connection upload buffer..

  Remarks:
    None

 */
 
const void* TCPIP_HTTP_CurrentConnectionUploadBufferStartGet(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    const void* TCPIP_HTTP_CurrentConnectionUploadBufferEndGet(HTTP_CONN_HANDLE connHandle);

  Summary:
    Gets the end address of current connection upload buffer.

  Description:
    This function will return the end address of current connection upload buffer.
   
  Precondition:
    None.

  Parameters:
	connHandle  - HTTP connection handle

  Returns:
    the end address of current connection upload buffer..

  Remarks:
    None

 */

const void* TCPIP_HTTP_CurrentConnectionUploadBufferEndGet(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    const void* TCPIP_HTTP_CurrentConnectionUploadBufferCurrGet(HTTP_CONN_HANDLE connHandle);

  Summary:
    Gets the current address of current connection upload buffer.

  Description:
    This function will return the Current address of current connection upload buffer.
   
  Precondition:
    None.

  Parameters:
	connHandle  - HTTP connection handle

  Returns:
    the Current address of current connection upload buffer..

  Remarks:
    None

 */

const void* TCPIP_HTTP_CurrentConnectionUploadBufferCurrGet(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    void TCPIP_HTTP_CurrentConnectionUploadBufferStartSet(HTTP_CONN_HANDLE connHandle, uint8_t* uploadBufferStart);

  Summary:
    Set the start address of current connection upload buffer.

  Description:
    This function will return the set Current address of current connection upload buffer.
   
  Precondition:
    None.

  Parameters:
	connHandle  - HTTP connection handle
	uploadBufferStart - start upload buffer.

  Returns:
    None.

  Remarks:
    None.

 */
void TCPIP_HTTP_CurrentConnectionUploadBufferStartSet(HTTP_CONN_HANDLE connHandle, uint8_t* uploadBufferStart);

//*****************************************************************************
/*
  Function:
    void TCPIP_HTTP_CurrentConnectionUploadBufferEndSet(HTTP_CONN_HANDLE connHandle, uint8_t* uploadBufferEnd);

  Summary:
    Set the end address of current connection upload buffer.

  Description:
    This function Set the end address of current connection upload buffer.
   
  Precondition:
    None.

  Parameters:
	connHandle  - HTTP connection handle
	uploadBufferEnd - end upload buffer.

  Returns:
    None.

  Remarks:
    None.

 */
void TCPIP_HTTP_CurrentConnectionUploadBufferEndSet(HTTP_CONN_HANDLE connHandle, uint8_t* uploadBufferEnd);

//*****************************************************************************
/*
  Function:
    void TCPIP_HTTP_CurrentConnectionUploadBufferCurrSet(HTTP_CONN_HANDLE connHandle, uint8_t* uploadBufferCurr);

  Summary:
    Set the current address of current connection upload buffer.

  Description:
    This function Set the current address of current connection upload buffer..
   
  Precondition:
    None.

  Parameters:
	connHandle  - HTTP connection handle
	uploadBufferCurr - current upload buffer 

  Returns:
    None.

  Remarks:
    None.

 */

void TCPIP_HTTP_CurrentConnectionUploadBufferCurrSet(HTTP_CONN_HANDLE connHandle, uint8_t* uploadBufferCurr);

//*****************************************************************************
/*
  Function:
    void TCPIP_HTTP_CurrentConnectionSMSet(HTTP_CONN_HANDLE connHandle, uint8_t stat);

  Summary:
    Set the HTTP current connection state Machine.

  Description:
    This function set the HTTP current connection state Machine..
   
  Precondition:
    None.

  Parameters:
	connHandle  - HTTP connection handle
	stat - current state.

  Returns:
    None.

  Remarks:
    None.

 */

void TCPIP_HTTP_CurrentConnectionSMSet(HTTP_CONN_HANDLE connHandle, uint8_t stat);

//*****************************************************************************
/*
  Function:
    HTTP_READ_STATUS TCPIP_HTTP_PostReadPair(HTTP_CONN_HANDLE connHandle, uint8_t* cData, uint16_t wLen)

  Summary:
    Reads a name and value pair from a URL encoded string in the TCP buffer.

  Description:
    Reads a name and value pair from a URL encoded string in the TCP buffer.
    This function is meant to be called from an TCPIP_HTTP_PostExecute callback to
    facilitate easier parsing of incoming data.  This function also prevents
    buffer overflows by forcing the programmer to indicate how many bytes are
    expected.  At least 2 extra bytes are needed in cData over the maximum
    length of data expected to be read.

    This function will read until the next '&' character, which indicates the
    end of a value parameter.  It assumes that the front of the buffer is
    the beginning of the name parameter to be read.

    This function properly updates the connection byteCount (see TCPIP_HTTP_CurrentConnectionByteCountGet())
    by decrementing it by the number of bytes read.  It also removes the delimiting '&' from
    the buffer.

    Once complete, two strings will exist in the cData buffer.  The first is
    the parameter name that was read, while the second is the associated
    value.

  Precondition:
    The front of the TCP buffer is the beginning of a name parameter, and the rest of
    the TCP buffer contains a URL-encoded string with a name parameter
    terminated by a '=' character and a value parameter terminated by a '&'.

  Parameters:
    connHandle  - HTTP connection handle
    cData - where to store the name and value strings once they are read
    wLen - how many bytes can be written to cData

  Returns:
    - HTTP_READ_OK - name and value were successfully read
    - HTTP_READ_TRUNCTATED - entire name and value could not fit in the buffer,
                            so input was truncated and data has been lost
    - HTTP_READ_INCOMPLETE - entire name and value was not yet in the buffer,
                            so call this function again later to retrieve

  Remarks:
    This function is aliased to TCPIP_HTTP_PostValueRead, since they effectively
    perform the same task.  The name is provided only for completeness.
 */
#define TCPIP_HTTP_PostReadPair(connHandle, cData, wLen) TCPIP_HTTP_PostValueRead(connHandle, cData, wLen)

// *****************************************************************************
// *****************************************************************************
// Section: User-implemented Callback Function Prototypes
// *****************************************************************************
// *****************************************************************************

//*****************************************************************************
/*
  Function:
    HTTP_IO_RESULT TCPIP_HTTP_GetExecute(HTTP_CONN_HANDLE connHandle)

  Summary:
    Processes GET form field variables and cookies.

  Description:
    This function is implemented by the application developer.
    Its purpose is to parse the data received from
    URL parameters (GET method forms) and cookies and perform any
    application-specific tasks in response to these inputs.  Any
    required authentication has already been validated.

    When this function is called, the connection data buffer
    (see TCPIP_HTTP_CurrentConnectionDataBufferGet())
    contains sequential name/value pairs of strings representing the data received.
    In this format, TCPIP_HTTP_ArgGet can be used to search for
    specific variables in the input.  If data buffer space associated
    with this connection is required, connection data buffer may be overwritten
    here once the application is done with the values.  Any data placed
    there will be available to future callbacks for this connection,
    including TCPIP_HTTP_PostExecute and any TCPIP_HTTP_Print_varname dynamic
    substitutions.

    This function may also issue redirections by setting the connection data
    buffer to the destination file name or URL, and the connection
    httpStatus (TCPIP_HTTP_CurrentConnectionStatusSet()) to HTTP_REDIRECT.

    Finally, this function may set cookies.  Set connection data buffer to a series
    of name/value string pairs (in the same format in which parameters
    arrive) and then set the connection hasArgs (TCPIP_HTTP_CurrentConnectionHasArgsSet())
    equal to the number of cookie name/value pairs.
    The cookies will be transmitted to the browser, and any future requests
    will have those values available in the connection data buffer.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    - HTTP_IO_DONE - application is done processing
    - HTTP_IO_NEED_DATA - this value may not be returned because more data
                        will not become available
    - HTTP_IO_WAITING - the application is waiting for an asynchronous
                      process to complete, and this function should be
                      called again later

  Remarks:
    This function is only called if variables are received via URL
    parameters or Cookie arguments.  This function may NOT write to the
    TCP buffer.

    This function may service multiple HTTP requests simultaneously.
    Exercise caution when using global or static variables inside this
    routine.  Use the connection callbackPos (TCPIP_HTTP_CurrentConnectionCallbackPosGet())
    or the connection data buffer for storage associated with individual requests.
 */
HTTP_IO_RESULT TCPIP_HTTP_GetExecute(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    HTTP_IO_RESULT TCPIP_HTTP_PostExecute(HTTP_CONN_HANDLE connHandle)

  Summary:
    Processes POST form variables and data.

  Description:
    This function is implemented by the application developer.
    Its purpose is to parse the data received from
    POST forms and perform any application-specific tasks in response
    to these inputs.  Any required authentication has already been
    validated before this function is called.

    When this function is called, POST data will be waiting in the
    TCP buffer.
    The connection byteCount (see TCPIP_HTTP_CurrentConnectionByteCountGet
    will indicate the number of bytes remaining to be read before the 
	browser request is complete.

    Since data is still in the TCP buffer, the application must call
    TCPIP_TCP_ArrayGet in order to retrieve bytes.  When this is done,
    connection byteCount MUST be updated to reflect how many bytes now
    remain.  The functions TCPIP_TCP_ArrayFind and TCPIP_TCP_Find 
    may be helpful to locate data in the TCP buffer.

    In general, data submitted from web forms via POST is URL encoded.
    The TCPIP_HTTP_URLDecode function can be used to decode this information
    back to a standard string if required.  If data buffer space
    associated with this connection is required, the connection data buffer 
    (see TCPIP_HTTP_CurrentConnectionDataBufferGet()) may be
    overwritten here once the application is done with the values.
    Any data placed there will be available to future callbacks for
    this connection,  including TCPIP_HTTP_PostExecute and any
    TCPIP_HTTP_Print_varname dynamic substitutions.

    Whenever a POST form is processed it is recommended to issue a
    redirect back to the browser, either to a status page or to
    the same form page that was posted.  This prevents accidental
    duplicate submissions (by clicking refresh or back/forward) and
    avoids browser warnings about "resubmitting form data".  Redirects
    may be issued to the browser by setting the connection data buffer to the
    destination file or URL, and the connection httpStatus
    (TCPIP_HTTP_CurrentConnectionStatusSet()) to HTTP_REDIRECT.

    Finally, this function may set cookies.  Set the connection data buffer
    to a series of name/value string pairs (in the same format in which parameters
    arrive), and then set the connection hasArgs (TCPIP_HTTP_CurrentConnectionHasArgsSet)
    equal to the number of cookie name/value pairs.
    The cookies will be transmitted to the browser, and any future requests
    will have those values available in the connection data buffer.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle

  Returns:
    - HTTP_IO_DONE - application is done processing
    - HTTP_IO_NEED_DATA - more data is needed to continue, and this
                        function should be called again later
    - HTTP_IO_WAITING - the application is waiting for an asynchronous
                      process to complete, and this function should
                      be called again later

  Remarks:
    This function is only called when the request method is POST, and is
    only used when HTTP_USE_POST is defined.  This method may NOT write
    to the TCP buffer.

    This function may service multiple HTTP requests simultaneously.
    Exercise caution when using global or static variables inside this
    routine.  Use the connection callbackPos (TCPIP_HTTP_CurrentConnectionCallbackPosGet)
    or connection data buffer for storage associated with individual requests.
 */
HTTP_IO_RESULT TCPIP_HTTP_PostExecute(HTTP_CONN_HANDLE connHandle);

//*****************************************************************************
/*
  Function:
    uint8_t TCPIP_HTTP_FileAuthenticate(HTTP_CONN_HANDLE connHandle, uint8_t* cFile)

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
    and can be read using TCPIP_HTTP_CurrentConnectionIsAuthorizedGet().
    It will be available to future callbacks, including TCPIP_HTTP_UserAuthenticate and any
    of the TCPIP_HTTP_GetExecute, TCPIP_HTTP_PostExecute, or TCPIP_HTTP_Print_varname callbacks.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    cFile - the name of the file being requested

  Returns:
    - <= 0x79 - valid authentication is required
    - >= 0x80 - access is granted for this connection

  Remarks:
    This function may NOT write to the TCP buffer.
 */
uint8_t TCPIP_HTTP_FileAuthenticate(HTTP_CONN_HANDLE connHandle, uint8_t* cFile);

//*****************************************************************************
/*
  Function:
    uint8_t TCPIP_HTTP_UserAuthenticate(HTTP_CONN_HANDLE connHandle, uint8_t* cUser, uint8_t* cPass)

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

    The value returned by this function is stored in the corresponding connection data
    and will be available with TCPIP_HTTP_CurrentConnectionIsAuthorizedGet
    in any of the TCPIP_HTTP_GetExecute, TCPIP_HTTP_PostExecute, or 
	TCPIP_HTTP_Print_varname callbacks.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    cUser - the user name supplied by the client
    cPass - the password supplied by the client

  Returns:
    - <= 0x79 - the credentials were rejected
    - >= 0x80 - access is granted for this connection

  Remarks:
    This function is only called when an Authorization header is
    encountered.

    This function may NOT write to the TCP buffer.
 */
uint8_t TCPIP_HTTP_UserAuthenticate(HTTP_CONN_HANDLE connHandle, uint8_t* cUser, uint8_t* cPass);

//*****************************************************************************
/*
  Function:
    void TCPIP_HTTP_Print_varname(HTTP_CONN_HANDLE connHandle)
    void TCPIP_HTTP_Print_varname(HTTP_CONN_HANDLE connHandle, uint16_t wParam1)
    void TCPIP_HTTP_Print_varname(HTTP_CONN_HANDLE connHandle, uint16_t wParam1, 
	                              uint16_t wParam2, ...)

  Summary:
    Inserts dynamic content into a web page

  Description:
    Functions in this style are implemented by the application developer.
    These functions generate dynamic content to be
    inserted into web pages and other files returned by the HTTP server.

    Functions of this type are called when a dynamic variable is located
    in a web page.  (i.e., ~varname~)  The name between the tilde '~'
    characters is appended to the base function name.  In this example, the
    callback would be named TCPIP_HTTP_Print_varname.

    The function prototype is located in your project's http_print.h, which
    is automatically generated by the mpfs2.jar utility.  The prototype will
    have uint16_t parameters included for each parameter passed in the dynamic
    variable.  For example, the variable "~myArray(2,6)~" will generate the
    prototype "void TCPIP_HTTP_Print_varname(uint16_t, uint16_t);".

    When called, this function should write its output directly to the TCP
    socket using any combination of TCPIP_TCP_PutIsReady, TCPIP_TCP_Put, TCPIP_TCP_ArrayPut,
    TCPIP_TCP_StringPut, TCPIP_TCP_ArrayPut, and TCPIP_TCP_StringPut.

    Before calling, the HTTP server guarantees that at least
    HTTP_MIN_CALLBACK_FREE bytes (defaults to 16 bytes) are free in the
    output buffer.  If the function is writing less than this amount, it
    should simply write the data to the socket and return.

    In situations where a function needs to write more this amount, it
    must manage its output state using the connection callbackPos
    (TCPIP_HTTP_CurrentConnectionCallbackPosGet/TCPIP_HTTP_CurrentConnectionCallbackPosSet).
    This value will be set to zero before the function is called.
    If the function is managing its output state, it must set this to a non-zero value before
    returning.  Typically this is used to track how many bytes have been
    written, or how many remain to be written.  If the connection callbackPos is
    non-zero, the function will be called again when more buffer space is
    available.  Once the callback completes, set this value back to zero
    to resume normal servicing of the request.

  Precondition:
    None.

  Parameters:
    connHandle  - HTTP connection handle
    wParam1 - first parameter passed in the dynamic variable (if any)
    wParam2 - second parameter passed in the dynamic variable (if any)
    ... - additional parameters as necessary

  Returns:
    None.

  Remarks:
    This function may service multiple HTTP requests simultaneously,
    especially when managing its output state.  Exercise caution when using
    global or static variables inside this routine.  Use the connection
    callbackPos or the connection data buffer for storage associated with 
	individual requests.
 */
void TCPIP_HTTP_Print_varname(HTTP_CONN_HANDLE connHandle,  uint16_t wParam1, uint16_t wParam2, ...);


// *****************************************************************************
/*
  Function:
    void  TCPIP_HTTP_Task(void)

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
void  TCPIP_HTTP_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __HTTP_H_

