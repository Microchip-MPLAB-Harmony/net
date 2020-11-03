/*******************************************************************************
  HyperText Transfer Protocol (HTTP) Server

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Serves dynamic pages to web browsers such as Microsoft Internet 
      Explorer, Mozilla Firefox, etc.
    - Reference: RFC 2616
*******************************************************************************/

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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_HTTP_SERVER

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_HTTP_SERVER)



#define HTTP_SEND_DATABUF_SIZE      256
#define HTTP_INC_DATABUF_SIZE       256


#define MPFS_SIGNATURE "MPFS\x02\x01"
// size of the MPFS upload operation write buffer
#define MPFS_UPLOAD_WRITE_BUFFER_SIZE   (4 * 1024)

#include "tcpip/src/common/sys_fs_shell.h"

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
#include "system/fs/sys_fs_media_manager.h"
#endif  // defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)

#include "http_private.h"

extern void TCPIP_HTTP_Print(HTTP_CONN_HANDLE connHandle,uint32_t callbackID);

#define SYS_FS_ATTR_ZIP_COMPRESSED  ((uint16_t)0x0001)
//#define HTTP_FILE_ERR_DEBUG
/****************************************************************************
  Section:
    String Constants
  ***************************************************************************/
    static const uint8_t HTTP_CRLF[] = "\r\n";  // New line sequence
    #define HTTP_CRLF_LEN   2               // Length of above string
        
/****************************************************************************
  Section:
    File and Content Type Settings
  ***************************************************************************/
    // File type extensions corresponding to HTTP_FILE_TYPE
    static const char * const httpFileExtensions[HTTP_UNKNOWN+2] =
    {
        "txt",          // HTTP_TXT
        "htm",          // HTTP_HTM
        "html",         // HTTP_HTML
        "cgi",          // HTTP_CGI
        "xml",          // HTTP_XML
        "css",          // HTTP_CSS
        "gif",          // HTTP_GIF
        "png",          // HTTP_PNG
        "jpg",          // HTTP_JPG
        "js",           // HTTP_JS
        "cla",          // HTTP_JAVA
        "wav",          // HTTP_WAV
        "\0\0\0"        // HTTP_UNKNOWN
    };
    
    // Content-type strings corresponding to HTTP_FILE_TYPE
    static const char * const httpContentTypes[HTTP_UNKNOWN+2] =
    {
        "text/plain",            // HTTP_TXT
        "text/html",             // HTTP_HTM
        "text/html",             // HTTP_HTML
        "text/html",             // HTTP_CGI
        "text/xml",              // HTTP_XML
        "text/css",              // HTTP_CSS
        "image/gif",             // HTTP_GIF
        "image/png",             // HTTP_PNG
        "image/jpeg",            // HTTP_JPG
        "application/x-javascript",   // HTTP_JS
        "application/java-vm",   // HTTP_JAVA
        "audio/x-wave",          // HTTP_WAV
        ""                       // HTTP_UNKNOWN
    };
        
/****************************************************************************
  Section:
    Commands and Server Responses
  ***************************************************************************/

    // Initial response strings (Corresponding to HTTP_STATUS)
    static const char * const HTTPResponseHeaders[] =
    {
        "HTTP/1.1 200 OK\r\nConnection: close\r\n",
        "HTTP/1.1 200 OK\r\nConnection: close\r\n",
        "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n400 Bad Request: can't handle Content-Length\r\n",
        "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"Protected\"\r\nConnection: close\r\n\r\n401 Unauthorized: Password required\r\n",
        #if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
        "HTTP/1.1 404 Not found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n404: File not found<br>Use <a href=\"/" TCPIP_HTTP_FILE_UPLOAD_NAME "\">MPFS Upload</a> to program web pages\r\n",
        #else       
        "HTTP/1.1 404 Not found\r\nConnection: close\r\n\r\n404: File not found\r\n",
        #endif
        "HTTP/1.1 414 Request-URI Too Long\r\nConnection: close\r\n\r\n414 Request-URI Too Long: Buffer overflow detected\r\n",
        "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\n500 Internal Server Error: Expected data not present\r\n",
        "HTTP/1.1 501 Not Implemented\r\nConnection: close\r\n\r\n501 Not Implemented: Only GET and POST supported\r\n",
        "HTTP/1.1 302 Found\r\nConnection: close\r\nLocation: ",
        "HTTP/1.1 403 Forbidden\r\nConnection: close\r\n\r\n403 Forbidden: SSL Required - use HTTPS\r\n",

        #if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
        "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body style=\"margin:100px\"><form method=post action=\"/" TCPIP_HTTP_FILE_UPLOAD_NAME "\" enctype=\"multipart/form-data\"><b>MPFS Image Upload</b><p><input type=file name=i size=40> &nbsp; <input type=submit value=\"Upload\"></form></body></html>",
        "",
        "",
        "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body style=\"margin:100px\"><b>MPFS Update Successful</b><p><a href=\"/\">Site main page</a></body></html>",
        "",
        "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body style=\"margin:100px\"><b>MPFS Image Corrupt or Wrong Version</b><p><a href=\"/" TCPIP_HTTP_FILE_UPLOAD_NAME "\">Try again?</a></body></html>",
        #endif

    };
    
/****************************************************************************
  Section:
    Header Parsing Configuration
  ***************************************************************************/
    
    // Header strings for which we'd like to parse
    static const char * const HTTPRequestHeaders[] =
    {
        "Cookie:",
        "Authorization:",
        "Content-Length:"
    };
    
/****************************************************************************
  Section:
    HTTP Connection State Global Variables
  ***************************************************************************/
static HTTP_CONN*           httpConnCtrl = 0;       // all http connections
static uint8_t*             httpConnData = 0;       // http connections data space
static uint16_t             httpConnDataSize = 0;   // associated data size
static int                  httpConnNo = 0;         // number of HTTP connections
static int                  httpInitCount = 0;      // module init counter
static HTTP_MODULE_FLAGS    httpConfigFlags = 0;    // run time flags

static tcpipSignalHandle       httpSignalHandle = 0;

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
static void* (*http_malloc_fnc)(size_t bytes) = 0; // HTTP malloc
static void  (*http_free_fnc)(void* ptr) = 0;      // HTTP free
#endif  // defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)

// file shell object for file access
static const SYS_FS_SHELL_OBJ*  httpFileShell = 0;

/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/
static void _HTTP_HeaderParseLookup(HTTP_CONN* pHttpCon, int i);
#if defined(TCPIP_HTTP_USE_COOKIES)
static void _HTTP_HeaderParseCookie(HTTP_CONN* pHttpCon);
#endif
#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
static void _HTTP_HeaderParseAuthorization(HTTP_CONN* pHttpCon);
#endif
#if defined(TCPIP_HTTP_USE_POST)
static void _HTTP_HeaderParseContentLength(HTTP_CONN* pHttpCon);
static HTTP_READ_STATUS _HTTP_ReadTo(HTTP_CONN* pHttpCon, uint8_t delim, uint8_t* buf, uint16_t len);
#endif

static void TCPIP_HTTP_Process(void);
static void TCPIP_HTTP_ProcessConnection(HTTP_CONN* pHttpCon);
static bool TCPIP_HTTP_FileSend(HTTP_CONN* pHttpCon);
static bool TCPIP_HTTP_WebPageIsDynamic(HTTP_CONN* pHttpCon);
static void _HTTPSocketRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param);

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _HTTP_Cleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);
static void _HTTP_CloseConnections(TCPIP_NET_IF* pNetIf);
#else
#define _HTTP_Cleanup(stackCtrl)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
static HTTP_IO_RESULT TCPIP_HTTP_MPFSUpload(HTTP_CONN* pHttpCon);
#endif

#define mMIN(a, b)  ((a<b)?a:b)

static int8_t fileErr = 0;

static void _HTTP_FileRdCheck(int condt, const char *file, int32_t line)
{
    if (condt == 0) {
        #if defined(HTTP_FILE_ERR_DEBUG)
        SYS_CONSOLE_PRINT("%s, at line: %d\r\n", file, line);
        #endif
        fileErr = 1;
        //while(1);
    }
}

// if pNetIf == 0, all connections are closed, stack is going down
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _HTTP_CloseConnections(TCPIP_NET_IF* pNetIf)
{
    HTTP_CONN* pHttpCon;
    int  connIx;

    pHttpCon = httpConnCtrl + 0;
    for (connIx = 0; connIx < httpConnNo; connIx++)
    {
        // Close the connections that were associated with this interface
        if (pHttpCon->socket != INVALID_SOCKET)
        {
            if (pNetIf == 0 || TCPIP_TCP_SocketNetGet(pHttpCon->socket) == pNetIf)
            {
                if(pHttpCon->file != SYS_FS_HANDLE_INVALID)
                {
                    (*httpFileShell->fileClose)(httpFileShell, pHttpCon->file);
                    pHttpCon->file = SYS_FS_HANDLE_INVALID;
                }

                if(pNetIf == 0)
                {   // stack going down
                    if(pHttpCon->socketSignal != 0)
                    {
                        TCPIP_TCP_SignalHandlerDeregister(pHttpCon->socket, pHttpCon->socketSignal);
                    }
                    
                    TCPIP_TCP_Close(pHttpCon->socket);
                    pHttpCon->socket = INVALID_SOCKET;
                }
                else 
                {   // TCPIP_STACK_ACTION_IF_DOWN - interface down
                    TCPIP_TCP_Disconnect(pHttpCon->socket);
                }
            }
        }
        pHttpCon++;
    }


}


static void _HTTP_Cleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    if(httpConnData && httpConnCtrl)
    {
        _HTTP_CloseConnections(0);
    }

    if(httpConnData)
    {
        TCPIP_HEAP_Free(stackCtrl->memH, httpConnData);
        httpConnData = 0;
    }
    if(httpConnCtrl)
    {
        TCPIP_HEAP_Free(stackCtrl->memH, httpConnCtrl);
        httpConnCtrl = 0;
    }
    if(httpSignalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(httpSignalHandle);
        httpSignalHandle = 0;
    }

    if(httpFileShell != 0)
    {
        (*httpFileShell->delete)(httpFileShell);
        httpFileShell = 0;
    }

    httpConnNo = 0;
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


/*****************************************************************************
  Function:
    bool TCPIP_HTTP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, 
        const TCPIP_HTTP_MODULE_CONFIG* httpInitData)

  Summary:
    Initializes the HTTP server module.

  Description:
    Sets all HTTP sockets to the listening state, and initializes the
    state machine and file handles for each connection. 

  Precondition:
    TCP must already be initialized.

  Parameters:
    None

  Returns:
    true if initialization succeeded,
    false otherwise
    
  Remarks:
    This function is called only one during lifetime of the application.
  ***************************************************************************/

bool TCPIP_HTTP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl,
              const TCPIP_HTTP_MODULE_CONFIG* httpInitData)
{
    bool        initFail;
    int         connIx, nConns;
    HTTP_CONN*  pHttpCon;
    uint8_t*    pHttpData;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init
    initFail = false;
    while(httpInitCount == 0)
    {   // first time we're run

        if(httpInitData == 0 || httpInitData->nConnections == 0)
        {
            return false;
        }
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
        http_malloc_fnc = httpInitData->http_malloc_fnc;
        http_free_fnc = httpInitData->http_free_fnc;
        if(http_malloc_fnc == 0 || http_free_fnc == 0)
        {   // use the default stack values
            http_malloc_fnc = _TCPIPStackHeapConfig()->malloc_fnc;
            http_free_fnc = _TCPIPStackHeapConfig()->free_fnc;
        }
        if(http_malloc_fnc == 0 || http_free_fnc == 0)
        {   // failed; should not happen
            return false;
        }

#endif  // defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)

        nConns = httpInitData->nConnections;
        httpConfigFlags = httpInitData->configFlags;

        httpConnCtrl = (HTTP_CONN*)TCPIP_HEAP_Calloc(stackCtrl->memH, nConns, sizeof(*httpConnCtrl));
        httpConnData = (uint8_t*)TCPIP_HEAP_Malloc(stackCtrl->memH, nConns * httpInitData->dataLen);
        if(httpConnCtrl == 0 || httpConnData == 0)
        {   // failed
            SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Dynamic allocation failed");
            initFail = true;
            break;
        }

        // create the HTTP timer
        httpSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_HTTP_Task, TCPIP_HTTP_TASK_RATE);
        if(httpSignalHandle == 0)
        {   // cannot create the HTTP timer
            initFail = true;
            break;
        }

        // create the SYS_FS shell
        SYS_FS_SHELL_RES shellRes;
        const char* web_dir = httpInitData->web_dir;
        httpFileShell = SYS_FS_Shell_Create(web_dir, 0, 0, 0, &shellRes);
        if(httpFileShell == 0)
        {
            SYS_ERROR(SYS_ERROR_WARNING, " HTTP: FS Shell creation for directory: %s Failed: %d!\r\n", web_dir, shellRes);
            initFail = true;
            break;
        }

        // initialize all connections
        httpConnDataSize = httpInitData->dataLen;

        pHttpCon = httpConnCtrl + 0;
        for(connIx = 0; connIx < nConns; connIx++, pHttpCon++)
        {
            pHttpCon->socket =  INVALID_SOCKET;
            pHttpCon->file = SYS_FS_HANDLE_INVALID;
        }

        httpConnNo = nConns;
        pHttpCon = httpConnCtrl + 0;
        pHttpData = httpConnData;
        for(connIx = 0; connIx < nConns; connIx++)
        {
            pHttpCon->sm = SM_HTTP_IDLE;
            pHttpCon->socket = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_ANY, TCPIP_HTTP_SERVER_PORT, 0);
            if( pHttpCon->socket == INVALID_SOCKET)
            {   // failed to open the socket
                SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Socket creation failed");
                initFail = true;
                break;
            }

#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
            // set socket options
            if((httpConfigFlags & HTTP_MODULE_FLAG_NO_DELAY) != 0)
            {
                void* tcpForceFlush = (void*)1;
                TCPIP_TCP_OptionsSet(pHttpCon->socket, TCP_OPTION_NODELAY, tcpForceFlush);
            }
            if(httpInitData->sktTxBuffSize != 0)
            {
                void* tcpBuffSize = (void*)(unsigned int)httpInitData->sktTxBuffSize;
                TCPIP_TCP_OptionsSet(pHttpCon->socket, TCP_OPTION_TX_BUFF, tcpBuffSize);
            }
            if(httpInitData->sktRxBuffSize != 0)
            {
                void* tcpBuffSize = (void*)(unsigned int)httpInitData->sktRxBuffSize;
                TCPIP_TCP_OptionsSet(pHttpCon->socket, TCP_OPTION_RX_BUFF, tcpBuffSize);
            }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)

            pHttpCon->socketSignal = TCPIP_TCP_SignalHandlerRegister(pHttpCon->socket, TCPIP_TCP_SIGNAL_RX_DATA, _HTTPSocketRxSignalHandler, 0);
            if(pHttpCon->socketSignal == 0)
            {
                SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Signal creation failed");
                initFail = true;
                break;
            }

            pHttpCon->data = pHttpData;
            pHttpCon->connIx = (uint16_t)connIx;

            pHttpCon++;
            pHttpData += httpInitData->dataLen;
        }

        break;
    }

    if(initFail)
    {
        _HTTP_Cleanup(stackCtrl);
        return false;
    }

    httpInitCount++;
    return true;    
}

/*****************************************************************************
  Function:
    bool TCPIP_HTTP_Deinitialize(void)

  Summary:
    DeInitializes the HTTP server module.

  Description:
    Takes down all HTTP sockets, the state machine and file handles for 
    each connection. 

  Precondition:
    None

  Parameters:
    None

  Returns:
    None
    
  Remarks:
    This function is called only one during lifetime of the application.
  ***************************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_HTTP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    // one way or another this interface is going down

    if(httpInitCount > 0)
    {   // we're up and running
        _HTTP_CloseConnections(stackCtrl->pNetIf);

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down
            if(--httpInitCount == 0)
            {   // all closed
                // release resources
                _HTTP_Cleanup(stackCtrl);
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_HTTP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { //  some signal occurred
        TCPIP_HTTP_Process();
    }
}

// send a signal to the HTTP module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void _HTTPSocketRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_TCP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}



static void TCPIP_HTTP_Process(void)
{
    HTTP_CONN* pHttpCon;
    int conn;

    pHttpCon = httpConnCtrl + 0;
    for(conn = 0; conn < httpConnNo; conn++, pHttpCon++)
    {
        if(pHttpCon->socket == INVALID_SOCKET)
            continue;

        // If a socket is disconnected at any time 
        // forget about it and return to idle state.
        if(TCPIP_TCP_WasReset(pHttpCon->socket) || TCPIP_TCP_WasDisconnected(pHttpCon->socket))
        {
            pHttpCon->sm = SM_HTTP_IDLE;
            pHttpCon->file_sm = SM_IDLE;

            // Make sure any opened files are closed
            if(pHttpCon->file != SYS_FS_HANDLE_INVALID)
            {
                (*httpFileShell->fileClose)(httpFileShell, pHttpCon->file);
                pHttpCon->file = SYS_FS_HANDLE_INVALID;
                // Important to clear related control variables,
                // or serving continuous refresh(F5) by IE etc... will meet issue
                memset((void *)&pHttpCon->TxFile, 0, sizeof(FILE_CTRL));
            }
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
            if((httpConfigFlags & HTTP_MODULE_FLAG_ADJUST_SKT_FIFOS) != 0)
            {
                // Adjust FIFO sizes to half and half.
                TCPIP_TCP_FifoSizeAdjust(pHttpCon->socket, 1, 0, TCP_ADJUST_PRESERVE_RX);
            }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
        }

        // Determine if this connection is eligible for processing
        if(pHttpCon->sm != SM_HTTP_IDLE || TCPIP_TCP_GetIsReady(pHttpCon->socket))
        {
            TCPIP_HTTP_ProcessConnection(pHttpCon);
        }
    }
}

/*****************************************************************************
  Function:
    static void TCPIP_HTTP_ProcessConnection(HTTP_CONN* pHttpCon)

  Description:
    Performs any pending operations for the currently loaded HTTP connection.

  Precondition:
    TCPIP_HTTP_Initialize() has been called.

  Parameters:
    None

  Returns:
    None
  ***************************************************************************/

static void TCPIP_HTTP_ProcessConnection(HTTP_CONN* pHttpCon)
{
    uint16_t lenA, lenB;
    int i;
    uint8_t c;
    bool isDone;
    uint8_t * ptr = NULL;
    uint8_t *ext;
    uint8_t buffer[TCPIP_HTTP_MAX_HEADER_LEN+1];
    SYS_FS_FSTAT fs_attr = {0};

    do
    {
        isDone = true;

        switch(pHttpCon->sm)
        {

            case SM_HTTP_IDLE:

                // Check how much data is waiting
                lenA = TCPIP_TCP_GetIsReady(pHttpCon->socket);

                // If a connection has been made, then process the request
                if(lenA)
                {// Clear out state info and move to next state
                    pHttpCon->ptrData = pHttpCon->data;
                    pHttpCon->sm = SM_HTTP_PARSE_REQUEST;
                    pHttpCon->isAuthorized = 0xff;
                    pHttpCon->hasArgs = 0;
                    pHttpCon->httpTick = SYS_TMR_TickCountGet() + TCPIP_HTTP_TIMEOUT * SYS_TMR_TickCounterFrequencyGet();
                    pHttpCon->callbackPos = 0xffffffff;
                    pHttpCon->byteCount = 0;
                    pHttpCon->nameHash = 0;
#if defined(TCPIP_HTTP_USE_POST)
                    pHttpCon->smPost = 0x00;
#endif
                    pHttpCon->TxFile.fileTxDone = 0;
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
                    if((httpConfigFlags & HTTP_MODULE_FLAG_ADJUST_SKT_FIFOS) != 0)
                    {
                        // Adjust the TCP FIFOs for optimal reception of 
                        // the next HTTP request from the browser
                        TCPIP_TCP_FifoSizeAdjust(pHttpCon->socket, 1, 0, TCP_ADJUST_PRESERVE_RX | TCP_ADJUST_GIVE_REST_TO_RX);
                    }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
                }
                else
                    // Don't break for new connections.  There may be
                    // an entire request in the buffer already.
                    break;

            case SM_HTTP_PARSE_REQUEST:

                // Verify the entire first line is in the FIFO
                if(TCPIP_TCP_Find(pHttpCon->socket, '\n', 0, 0, false) == 0xffff)
                {// First line isn't here yet
                    if(TCPIP_TCP_FifoRxFreeGet(pHttpCon->socket) == 0u)
                    {// If the FIFO is full, we overflowed
                        pHttpCon->httpStatus = HTTP_OVERFLOW;
                        pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
                        isDone = false;
                    }
                    if((int32_t)(SYS_TMR_TickCountGet() - pHttpCon->httpTick) > 0)
                    {// A timeout has occurred
                        TCPIP_TCP_Disconnect(pHttpCon->socket);
                        pHttpCon->sm = SM_HTTP_DISCONNECT;
                        isDone = false;
                    }
                    break;
                }

                // Reset the watchdog timer
                pHttpCon->httpTick = SYS_TMR_TickCountGet() + TCPIP_HTTP_TIMEOUT * SYS_TMR_TickCounterFrequencyGet();

                // Determine the request method
                lenA = TCPIP_TCP_Find(pHttpCon->socket, ' ', 0, 0, false);
                if(lenA > 5u)
                    lenA = 5;
                TCPIP_TCP_ArrayGet(pHttpCon->socket, pHttpCon->data, lenA+1);

                if ( memcmp(pHttpCon->data, (const void*)"GET", 3) == 0)
                    pHttpCon->httpStatus = HTTP_GET;
#if defined(TCPIP_HTTP_USE_POST)
                else if ( memcmp(pHttpCon->data, (const void*)"POST", 4) == 0)
                    pHttpCon->httpStatus = HTTP_POST;
#endif
                else
                {// Unrecognized method, so return not implemented
                    pHttpCon->httpStatus = HTTP_NOT_IMPLEMENTED;
                    pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
                    isDone = false;
                    break;
                }

                // Find end of filename
                lenA = TCPIP_TCP_Find(pHttpCon->socket, ' ', 0, 0, false);
                lenB = TCPIP_TCP_Find(pHttpCon->socket, '?', 0, lenA, false);
                lenA = mMIN(lenA, lenB);

                // If the file name is too long, then reject the request
                if(lenA > httpConnDataSize - TCPIP_HTTP_DEFAULT_LEN - 1)
                {
                    pHttpCon->httpStatus = HTTP_OVERFLOW;
                    pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
                    isDone = false;
                    break;
                }

                // Read in the filename and decode
                lenB = TCPIP_TCP_ArrayGet(pHttpCon->socket, pHttpCon->data, lenA);
                pHttpCon->data[lenB] = '\0';
                TCPIP_HTTP_URLDecode(pHttpCon->data);

                // Decode may have changed the string length - update it here
                lenB = strlen((char*)pHttpCon->data);

                // Check if this is an MPFS Upload
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
                if(memcmp(&pHttpCon->data[1], TCPIP_HTTP_FILE_UPLOAD_NAME, sizeof(TCPIP_HTTP_FILE_UPLOAD_NAME)) == 0)
                {// Read remainder of line, and bypass all file opening, etc.
#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
                    pHttpCon->isAuthorized = TCPIP_HTTP_FileAuthenticate(pHttpCon, &pHttpCon->data[1]);
#endif
                    if(pHttpCon->httpStatus == HTTP_GET)
                        pHttpCon->httpStatus = HTTP_MPFS_FORM;
                    else
                        pHttpCon->httpStatus = HTTP_MPFS_UP;

                    pHttpCon->sm = SM_HTTP_PARSE_HEADERS;
                    isDone = false;
                    break;
                }
#endif

                // If the last character is a not a directory delimiter, then try to open the file
                // String starts at 2nd character, because the first is always a '/'
                if(pHttpCon->data[lenB-1] != '/') {
                    pHttpCon->file = (*httpFileShell->fileOpen)(httpFileShell, (char *)&pHttpCon->data[1], SYS_FS_FILE_OPEN_READ);
                    if(strlen((char*)pHttpCon->data + 1) > sizeof(pHttpCon->fileName))
                    {
                        SYS_ERROR(SYS_ERROR_WARNING, " HTTP: URL exceeds allocated space!");
                    }
                    strncpy(pHttpCon->fileName, (char*)pHttpCon->data + 1, sizeof(pHttpCon->fileName));
                }

                // If the open fails, then add our default name and try again
                if(pHttpCon->file == SYS_FS_HANDLE_INVALID)
                {
                    // Add the directory delimiter if needed
                    if(pHttpCon->data[lenB-1] != '/')
                        pHttpCon->data[lenB++] = '/';

                    // Add our default file name
                    strncpy((char*)pHttpCon->data + lenB, TCPIP_HTTP_DEFAULT_FILE, httpConnDataSize - lenB);
                    lenB += strlen(TCPIP_HTTP_DEFAULT_FILE);

                    // Try to open again
                    pHttpCon->file = (*httpFileShell->fileOpen)(httpFileShell, (char *)&pHttpCon->data[1],SYS_FS_FILE_OPEN_READ);
                    strncpy(pHttpCon->fileName, (char*)pHttpCon->data + 1, sizeof(pHttpCon->fileName));
                }

                //Calculate 2 Bytes HashIndex for  pHttpCon->file->name
                // Calculate the name hash to speed up searching
                for(pHttpCon->nameHash = 0, ptr = (uint8_t*)(&pHttpCon->data[1]); ptr !=(uint8_t*) (&pHttpCon->data[lenB]); ptr++)
                {
                    if(*ptr != 0x20)
                    {
                        pHttpCon->nameHash += *ptr;
                        pHttpCon->nameHash <<= 1;
                    }
                }

                // Find the extension in the filename
                for(ext = pHttpCon->data + lenB-1; ext != pHttpCon->data; ext--)
                    if(*ext == '.')
                        break;

                // Compare to known extensions to determine Content-Type
                ext++;
                for(pHttpCon->fileType = HTTP_TXT; pHttpCon->fileType < HTTP_UNKNOWN; pHttpCon->fileType++)
                    if(!strcmp((char *)ext, (const void*)httpFileExtensions[pHttpCon->fileType]))
                        break;

                // Perform first round authentication (pass file name only)
#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
                pHttpCon->isAuthorized = TCPIP_HTTP_FileAuthenticate(pHttpCon, &pHttpCon->data[1]);
#endif

                // If the file was found, see if it has an index
                // Index file is not needed any more, because of FileRcrd.bin and DynRcrd.bin used

                // Read GET args, up to buffer size - 1
                lenA = TCPIP_TCP_Find(pHttpCon->socket, ' ', 0, 0, false);
                if(lenA != 0)
                {
                    pHttpCon->hasArgs = 1;

                    // Trash the '?'
                    TCPIP_TCP_Get(pHttpCon->socket, &c);

                    // Verify there's enough space
                    lenA--;
                    if(lenA >= httpConnDataSize - 2)
                    {
                        pHttpCon->httpStatus = HTTP_OVERFLOW;
                        pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
                        isDone = false;
                        break;
                    }

                    // Read in the arguments and '&'-terminate in anticipation of cookies
                    pHttpCon->ptrData += TCPIP_TCP_ArrayGet(pHttpCon->socket, pHttpCon->data, lenA);
                    *(pHttpCon->ptrData++) = '&';

                }

                // Clear the rest of the line
                lenA = TCPIP_TCP_Find(pHttpCon->socket, '\n', 0, 0, false);
                TCPIP_TCP_ArrayGet(pHttpCon->socket, NULL, lenA + 1);

                // Move to parsing the headers
                pHttpCon->sm = SM_HTTP_PARSE_HEADERS;

                // No break, continue to parsing headers

            case SM_HTTP_PARSE_HEADERS:

                // Loop over all the headers
                while(1)
                {
                    // Make sure entire line is in the FIFO
                    lenA = TCPIP_TCP_Find(pHttpCon->socket, '\n', 0, 0, false);
                    if(lenA == 0xffff)
                    {// If not, make sure we can receive more data
                        if(TCPIP_TCP_FifoRxFreeGet(pHttpCon->socket) == 0u)
                        {// Overflow
                            pHttpCon->httpStatus = HTTP_OVERFLOW;
                            pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
                            isDone = false;
                        }
                        if((int32_t)(SYS_TMR_TickCountGet() - pHttpCon->httpTick) > 0)
                        {// A timeout has occured
                            TCPIP_TCP_Disconnect(pHttpCon->socket);
                            pHttpCon->sm = SM_HTTP_DISCONNECT;
                            isDone = false;
                        }
                        break;
                    }

                    // Reset the watchdog timer
                    pHttpCon->httpTick = SYS_TMR_TickCountGet() + TCPIP_HTTP_TIMEOUT * SYS_TMR_TickCounterFrequencyGet();

                    // If a CRLF is immediate, then headers are done
                    if(lenA == 1u)
                    {// Remove the CRLF and move to next state
                        TCPIP_TCP_ArrayGet(pHttpCon->socket, NULL, 2);
                        pHttpCon->sm = SM_HTTP_AUTHENTICATE;
                        isDone = false;
                        break;
                    }

                    // Find the header name, and use isDone as a flag to indicate a match
                    lenB = TCPIP_TCP_Find(pHttpCon->socket, ':', 0, lenA, false) + 2;
                    isDone = false;

                    // If name is too long or this line isn't a header, ignore it
                    if(lenB > sizeof(buffer))
                    {
                        TCPIP_TCP_ArrayGet(pHttpCon->socket, NULL, lenA+1);
                        continue;
                    }

                    // Read in the header name
                    TCPIP_TCP_ArrayGet(pHttpCon->socket, buffer, lenB);
                    buffer[lenB-1] = '\0';
                    lenA -= lenB;

                    // Compare header read to ones we're interested in
                    for(i = 0; i < sizeof(HTTPRequestHeaders)/sizeof(HTTPRequestHeaders[0]); i++)
                    {
                        if(stricmp((char*)buffer, (const char *)HTTPRequestHeaders[i]) == 0)
                        {// Parse the header and stop the loop
                        _HTTP_HeaderParseLookup(pHttpCon, i);
                            isDone = true;
                            break;
                        }
                    }

                    // Clear the rest of the line, and call the loop again
                    if(isDone)
                    {// We already know how much to remove unless a header was found
                        lenA = TCPIP_TCP_Find(pHttpCon->socket, '\n', 0, 0, false);
                    }
                    TCPIP_TCP_ArrayGet(pHttpCon->socket, NULL, lenA+1);
                }

                break;

            case SM_HTTP_AUTHENTICATE:

#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
                // Check current authorization state
                if(pHttpCon->isAuthorized < 0x80)
                {// 401 error
                    pHttpCon->httpStatus = HTTP_UNAUTHORIZED;
                    pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
                    isDone = false;

                    break;
                }
#endif

                // Parse the args string
                *pHttpCon->ptrData = '\0';
                pHttpCon->ptrData = TCPIP_HTTP_URLDecode(pHttpCon->data);

                // If this is an MPFS upload form request, bypass to headers
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
                if(pHttpCon->httpStatus == HTTP_MPFS_FORM)
                {
                    pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
                    isDone = false;
                    break;
                }
#endif

                // Move on to GET args, unless there are none
                pHttpCon->sm = SM_HTTP_PROCESS_GET;
                if(pHttpCon->hasArgs == 0)
                {
                    pHttpCon->sm = SM_HTTP_PROCESS_POST;
                }
                isDone = false;
                pHttpCon->hasArgs = 0;
                break;

            case SM_HTTP_PROCESS_GET:

                // Run the application callback TCPIP_HTTP_GetExecute()
                if(TCPIP_HTTP_GetExecute(pHttpCon) == HTTP_IO_WAITING)
                {// If waiting for asynchronous process, return to main app
                    break;
                }

                // Move on to POST data
                pHttpCon->sm = SM_HTTP_PROCESS_POST;

            case SM_HTTP_PROCESS_POST:

#if defined(TCPIP_HTTP_USE_POST)

                // See if we have any new data
                if(TCPIP_TCP_GetIsReady(pHttpCon->socket) == pHttpCon->callbackPos)
                {
                    if((int32_t)(SYS_TMR_TickCountGet() - pHttpCon->httpTick) > 0)
                    {// If a timeout has occured, disconnect
                        TCPIP_TCP_Disconnect(pHttpCon->socket);
                        pHttpCon->sm = SM_HTTP_DISCONNECT;
                        isDone = false;
                        break;
                    }
                }

                if(pHttpCon->httpStatus == HTTP_POST
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
                        || (pHttpCon->httpStatus >= HTTP_MPFS_UP && pHttpCon->httpStatus <= HTTP_MPFS_ERROR)
#endif
                  )
                {
                    // Run the application callback TCPIP_HTTP_PostExecute()
#if  defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
                    if(pHttpCon->httpStatus >= HTTP_MPFS_UP && pHttpCon->httpStatus <= HTTP_MPFS_ERROR)
                    {
                    c = TCPIP_HTTP_MPFSUpload(pHttpCon);
                        if(c == (uint8_t)HTTP_IO_DONE)
                        {
                            pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
                            isDone = false;
                            break;
                        }
                    }
                    else
#endif  // defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
                        c = TCPIP_HTTP_PostExecute(pHttpCon);
					 // If waiting for asynchronous process, return to main app
					if(c == (uint8_t)HTTP_IO_WAITING)	
                    {// return to main app and make sure we don't get stuck by the watchdog
                        pHttpCon->callbackPos = TCPIP_TCP_GetIsReady(pHttpCon->socket) - 1;
                        break;
                    }
                    else if(c == (uint8_t)HTTP_IO_NEED_DATA)
                    {// If waiting for more data
                        pHttpCon->callbackPos = TCPIP_TCP_GetIsReady(pHttpCon->socket);
                        pHttpCon->httpTick = SYS_TMR_TickCountGet() + TCPIP_HTTP_TIMEOUT * SYS_TMR_TickCounterFrequencyGet();

                        // If more is expected and space is available, return to main app
                        if(pHttpCon->byteCount > pHttpCon->callbackPos && TCPIP_TCP_FifoRxFreeGet(pHttpCon->socket) != 0u)
                            break;

                        // Handle cases where application ran out of data or buffer space
                        pHttpCon->httpStatus = HTTP_INTERNAL_SERVER_ERROR;
                        pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
                        isDone = false;
                        break;
                    }
                }
#endif

                // We're done with POST
                pHttpCon->sm = SM_HTTP_PROCESS_REQUEST;
                // No break, continue to sending request

            
            case SM_HTTP_PROCESS_REQUEST:

                // Check for 404
                if(pHttpCon->file == SYS_FS_HANDLE_INVALID)
                {
                    pHttpCon->httpStatus = HTTP_NOT_FOUND;
                    pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
                    isDone = false;
                    break;
                }

                // Set up the dynamic substitutions
                pHttpCon->byteCount = 0;

                // Move to next state
                pHttpCon->sm = SM_HTTP_SERVE_HEADERS;

            case SM_HTTP_SERVE_HEADERS:

                // We're in write mode now:
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
                if((httpConfigFlags & HTTP_MODULE_FLAG_ADJUST_SKT_FIFOS) != 0)
                {
                    // Adjust the TCP FIFOs for optimal transmission of 
                    // the HTTP response to the browser
                    TCPIP_TCP_FifoSizeAdjust(pHttpCon->socket, 1, 0, TCP_ADJUST_GIVE_REST_TO_TX);
                }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
                // Send headers
                TCPIP_TCP_StringPut(pHttpCon->socket, (const uint8_t*)HTTPResponseHeaders[pHttpCon->httpStatus]);

                // If this is a redirect, print the rest of the Location: header
                if(pHttpCon->httpStatus == HTTP_REDIRECT)
                {
                    TCPIP_TCP_StringPut(pHttpCon->socket, pHttpCon->data);
                    TCPIP_TCP_StringPut(pHttpCon->socket, (const uint8_t*)"\r\n\r\n304 Redirect: ");
                    TCPIP_TCP_StringPut(pHttpCon->socket, pHttpCon->data);
                    TCPIP_TCP_StringPut(pHttpCon->socket, (const uint8_t*)HTTP_CRLF);
                }

                // If not GET or POST, we're done
                if(pHttpCon->httpStatus != HTTP_GET && pHttpCon->httpStatus != HTTP_POST)
                {// Disconnect
                    pHttpCon->sm = SM_HTTP_DISCONNECT;
                    break;
                }

                // Output the content type, if known
                if(pHttpCon->fileType != HTTP_UNKNOWN)
                {
                    TCPIP_TCP_StringPut(pHttpCon->socket, (const uint8_t*)"Content-Type: ");
                    TCPIP_TCP_StringPut(pHttpCon->socket, (const uint8_t*)httpContentTypes[pHttpCon->fileType]);
                    TCPIP_TCP_StringPut(pHttpCon->socket, HTTP_CRLF);
                }

                // Output the gzip encoding header if needed
                if((*httpFileShell->fileStat)(httpFileShell, (const char *)&pHttpCon->fileName, &fs_attr) != SYS_FS_HANDLE_INVALID) {
                    if (fs_attr.fattrib == SYS_FS_ATTR_ZIP_COMPRESSED)
                    {
                        TCPIP_TCP_StringPut(pHttpCon->socket, (const uint8_t*)"Content-Encoding: gzip\r\n");
                    }
                }

                // Output the cache-control
                TCPIP_TCP_StringPut(pHttpCon->socket, (const uint8_t*)"Cache-Control: ");
                if((pHttpCon->httpStatus == HTTP_POST) || (TCPIP_HTTP_WebPageIsDynamic(pHttpCon)))
                {// This is a dynamic page or a POST request, so no cache
                    TCPIP_TCP_StringPut(pHttpCon->socket, (const uint8_t*)"no-cache");
                }
                else
                {// This is a static page, so save it for the specified amount of time
                    TCPIP_TCP_StringPut(pHttpCon->socket, (const uint8_t*)"max-age=");
                    TCPIP_TCP_StringPut(pHttpCon->socket, (const uint8_t*)TCPIP_HTTP_CACHE_LEN);
                }
                TCPIP_TCP_StringPut(pHttpCon->socket, HTTP_CRLF);

                // Check if we should output cookies
                if(pHttpCon->hasArgs != 0)
                {
                    pHttpCon->sm = SM_HTTP_SERVE_COOKIES;
                }
                else
                {// Terminate the headers
                    TCPIP_TCP_StringPut(pHttpCon->socket, HTTP_CRLF);
                    pHttpCon->sm = SM_HTTP_SERVE_BODY;
                }

                // Move to next stage
                isDone = false;
                break;

            case SM_HTTP_SERVE_COOKIES:

#if defined(TCPIP_HTTP_USE_COOKIES)
                // If the TX FIFO runs out of space, the client will never get CRLFCRLF
                // Avoid writing huge cookies - keep it under a hundred bytes max

                // Write cookies one at a time as space permits
                for(pHttpCon->ptrRead = pHttpCon->data; pHttpCon->hasArgs != 0; pHttpCon->hasArgs--)
                {
                    // Write the header
                    TCPIP_TCP_StringPut(pHttpCon->socket, (const uint8_t*)"Set-Cookie: ");

                    // Write the name, URL encoded, one character at a time
                    while((c = *(pHttpCon->ptrRead++)))
                    {
                        if(c == ' ')
                            TCPIP_TCP_Put(pHttpCon->socket, '+');
                        else if(c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z')
                        {
                            TCPIP_TCP_Put(pHttpCon->socket, '%');
                            TCPIP_TCP_Put(pHttpCon->socket, btohexa_high(c));
                            TCPIP_TCP_Put(pHttpCon->socket, btohexa_low(c));
                        }
                        else
                            TCPIP_TCP_Put(pHttpCon->socket, c);
                    }

                    TCPIP_TCP_Put(pHttpCon->socket, '=');

                    // Write the value, URL encoded, one character at a time
                    while((c = *(pHttpCon->ptrRead++)))
                    {
                        if(c == ' ')
                            TCPIP_TCP_Put(pHttpCon->socket, '+');
                        else if(c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z')
                        {
                            TCPIP_TCP_Put(pHttpCon->socket, '%');
                            TCPIP_TCP_Put(pHttpCon->socket, btohexa_high(c));
                            TCPIP_TCP_Put(pHttpCon->socket, btohexa_low(c));
                        }
                        else
                            TCPIP_TCP_Put(pHttpCon->socket, c);
                    }

                    // Finish the line
                    TCPIP_TCP_StringPut(pHttpCon->socket, HTTP_CRLF);

                }
#endif

                // We're done, move to next state
                TCPIP_TCP_StringPut(pHttpCon->socket, HTTP_CRLF);
                pHttpCon->sm = SM_HTTP_SERVE_BODY;

            case SM_HTTP_SERVE_BODY:

                isDone = false;

                // Try to send next packet
                if(pHttpCon->TxFile.fileTxDone)
                {// If EOF, then we're done so close and disconnect
                    //SYS_FS_close(pHttpCon->file);
                    //pHttpCon->file = SYS_FS_HANDLE_INVALID;
                    pHttpCon->sm = SM_HTTP_DISCONNECT;
                    isDone = true;
                }
                else
                {
                    isDone = TCPIP_HTTP_FileSend(pHttpCon);
                }

                // If the TX FIFO is full, then return to main app loop
                if(!isDone && TCPIP_TCP_PutIsReady(pHttpCon->socket) == 0u)
                {
                    isDone = true;
                }
                break;

            case SM_HTTP_DISCONNECT:
                // Make sure any opened files are closed
                if(pHttpCon->file != SYS_FS_HANDLE_INVALID)
                {
                    (*httpFileShell->fileClose)(httpFileShell, pHttpCon->file);
                    pHttpCon->file = SYS_FS_HANDLE_INVALID;
                }

                if(TCPIP_TCP_Disconnect(pHttpCon->socket))
                {
                    pHttpCon->sm = SM_HTTP_IDLE;
                }
                // else retry next time
                break;
        }
    } while(!isDone);

}

/*****************************************************************************
  Function:
    static void IsDynamicWebPage(HTTP_CONN* pHttpCon)

  Description:
    Identify if currrent requested web page is dynamic(containing dynamic
        variable) or not. This is judged by if the file name hash is within the
        FileRcrd.bin or not, because all web pages, which has dynamic variable,
        will be recored in to the FileRcrd.bin.

  Precondition:
        None

  Parameters:
    pHttpCon

  Returns:
    true - dynamic page;
        false - static page;
  ***************************************************************************/
static bool TCPIP_HTTP_WebPageIsDynamic(HTTP_CONN* pHttpCon)
{
    SYS_FS_HANDLE fp = SYS_FS_HANDLE_INVALID;
    uint16_t nameHash = 0;
    uint32_t numFile = 0;
    bool ret = false;

    fp = (*httpFileShell->fileOpen)(httpFileShell, "FileRcrd.bin", SYS_FS_FILE_OPEN_READ);
    if (fp == SYS_FS_HANDLE_INVALID)
    {
        return false;
    }
    (*httpFileShell->fileRead)(httpFileShell, fp, &numFile, 4); //Reading Number of files in record

    while(numFile)
    {
        (*httpFileShell->fileRead)(httpFileShell, fp, &nameHash, 2); //Reading HashName record
        if(pHttpCon->nameHash == nameHash) 
        {
            ret = true;
            break;
        }
        else //Seek to next name hash position
        {
            (*httpFileShell->fileSeek)(httpFileShell, fp, 8, SYS_FS_SEEK_CUR);
        }
        numFile-=1;
    }
    // Close file
    if (fp != SYS_FS_HANDLE_INVALID) {
        (*httpFileShell->fileClose)(httpFileShell, fp);
    }

    return ret;
}

/*****************************************************************************
  Function:
    static bool TCPIP_HTTP_FileSend(HTTP_CONN* pHttpCon)

  Description:
    Serves up the next chunk of curHTTP's file, up to:
  a) the available TX FIFO space or
  b) the next callback index, whichever comes first.

  Precondition:
    pHttpCon->file has been opened for reading.

  Parameters:
    None

  Return Values:
    true  - no more data can be processed for this call and the big TCPIP_HTTP_ProcessConnection loop has to be broken
            this is due to the dynamic variable processing
            when the application notifies that it has more data to send to this current HTTP connection
    false - TCPIP_HTTP_ProcessConnection can continue, no break needed

  Note:
    the function sets the pHttpCon->TxFile.fileTxDone flag which also breaks the TCPIP_HTTP_ProcessConnection loop

  ***************************************************************************/

static bool TCPIP_HTTP_FileSend(HTTP_CONN* pHttpCon)
{
    uint32_t len;
    uint32_t cntr=0;
    //uint32_t dynVarCallBackID;
    uint32_t UInt32DataFromBinFile;
    uint16_t nameHashRcrd;
    uint32_t recrdcntr=0;
    uint8_t sendDataBuffer[HTTP_SEND_DATABUF_SIZE];
    SYS_FS_HANDLE FileRcrdPtr=SYS_FS_HANDLE_INVALID, DynVarFilePtr = SYS_FS_HANDLE_INVALID;
    bool needBreak = false;
    int16_t bytesPut;

    switch(pHttpCon->file_sm)
    {
        case SM_IDLE:

            pHttpCon->TxFile.numBytes = (*httpFileShell->fileSize)(httpFileShell, pHttpCon->file);
            if ((pHttpCon->TxFile.numBytes == SYS_FS_HANDLE_INVALID) || (pHttpCon->TxFile.numBytes == 0)) {
                pHttpCon->TxFile.fileTxDone = 1;
                return false;
            }
            pHttpCon->TxFile.bytesReadCount=0;
            pHttpCon->callbackPos = 0;

        case SM_GET_NO_OF_FILES:
            FileRcrdPtr = (*httpFileShell->fileOpen)(httpFileShell, "FileRcrd.bin", SYS_FS_FILE_OPEN_READ);
            if(FileRcrdPtr == SYS_FS_HANDLE_INVALID)
            {   // No dynamic variables, so default the flag to 1
                pHttpCon->file_sm = SM_SERVE_TEXT_DATA ;
                pHttpCon->TxFile.EndOfCallBackFileFlag = true;
                break;
            }

            cntr = (*httpFileShell->fileRead)(httpFileShell, FileRcrdPtr,&recrdcntr, 4); //Reading Number of files in record        
            _HTTP_FileRdCheck(cntr==4, __FILE__, __LINE__);

            //Continue to next state
            pHttpCon->file_sm = SM_GET_HASH_RCRD;

        case SM_GET_HASH_RCRD:

            while(recrdcntr)
            {
                cntr = (*httpFileShell->fileRead)(httpFileShell, FileRcrdPtr, &nameHashRcrd, 2); //Reading HashName record
                _HTTP_FileRdCheck(cntr==2, __FILE__, __LINE__);
                if(pHttpCon->nameHash == nameHashRcrd) // Check if namHash calculation contains dir delimiter '/'
                {
                    pHttpCon->TxFile.nameHashMatched = true;
                    cntr = (*httpFileShell->fileRead)(httpFileShell, FileRcrdPtr, &UInt32DataFromBinFile, 4);
                    _HTTP_FileRdCheck(cntr==4, __FILE__, __LINE__);
                    pHttpCon->TxFile.DynRcrdRdCount = UInt32DataFromBinFile;
                    cntr = (*httpFileShell->fileRead)(httpFileShell, FileRcrdPtr, &pHttpCon->TxFile.dynVarCntr, 4);
                    _HTTP_FileRdCheck(cntr==4, __FILE__, __LINE__);
                    break;
                }
                else //Dummy Read..to hop to next hash name or HOP by "FileSeek" in file handle
                {
                    cntr = (*httpFileShell->fileRead)(httpFileShell, FileRcrdPtr, &UInt32DataFromBinFile,4);
                    _HTTP_FileRdCheck(cntr==4, __FILE__, __LINE__);
                    cntr = (*httpFileShell->fileRead)(httpFileShell, FileRcrdPtr, &UInt32DataFromBinFile,4);
                    _HTTP_FileRdCheck(cntr==4, __FILE__, __LINE__);
                }
                recrdcntr-=1;
            }
            if(FileRcrdPtr != SYS_FS_HANDLE_INVALID) {
                (*httpFileShell->fileClose)(httpFileShell, FileRcrdPtr);
                FileRcrdPtr = SYS_FS_HANDLE_INVALID;
            }

            if(pHttpCon->TxFile.nameHashMatched == true)
            {
                //Continue to next state
                pHttpCon->file_sm = SM_GET_DYN_VAR_FILE_RCRD;
            }

            //If NO HashName record matched means the requrested webpage do not have dynamic variables
            if(recrdcntr == 0 && pHttpCon->TxFile.nameHashMatched !=true )
            {
                pHttpCon->file_sm=SM_SERVE_TEXT_DATA ;
                pHttpCon->TxFile.EndOfCallBackFileFlag = true; // No variable, so default the flag to 1

                break;
            }

        case SM_GET_DYN_VAR_FILE_RCRD:
            pHttpCon->callbackPos = 0;
            // Open DynRcrd.bin if not opened
            DynVarFilePtr = (*httpFileShell->fileOpen)(httpFileShell, "DynRcrd.bin", SYS_FS_FILE_OPEN_READ);
            if (DynVarFilePtr == SYS_FS_HANDLE_INVALID)
                return false;
            if(pHttpCon->TxFile.lock_dynrcd == 0)
            {
                (*httpFileShell->fileSeek)(httpFileShell, DynVarFilePtr, pHttpCon->TxFile.DynRcrdRdCount+6, SYS_FS_SEEK_SET);
                pHttpCon->TxFile.lock_dynrcd = 1;
            } else {
                (*httpFileShell->fileSeek)(httpFileShell, DynVarFilePtr, pHttpCon->TxFile.DynRcrdRdCount, SYS_FS_SEEK_SET);
            }
            cntr = (*httpFileShell->fileRead)(httpFileShell, DynVarFilePtr, &pHttpCon->TxFile.dynVarRcrdOffset, 4);//Reading dynamic variable offset in webpage
            _HTTP_FileRdCheck(cntr==4, __FILE__, __LINE__);
            cntr = (*httpFileShell->fileRead)(httpFileShell, DynVarFilePtr, &pHttpCon->TxFile.dynVarCallBackID, 4);//Reading dynamic variable call back ID
            _HTTP_FileRdCheck(cntr==4, __FILE__, __LINE__);
            
            //Continue to next state
            pHttpCon->file_sm =SM_PARSE_TILL_DYN_VAR;

        case SM_PARSE_TILL_DYN_VAR:
            // Check if it is re-entering service
            if (DynVarFilePtr == SYS_FS_HANDLE_INVALID) {
                DynVarFilePtr = (*httpFileShell->fileOpen)(httpFileShell, "DynRcrd.bin", SYS_FS_FILE_OPEN_READ);
                (*httpFileShell->fileSeek)(httpFileShell, DynVarFilePtr, pHttpCon->TxFile.DynRcrdRdCount, SYS_FS_SEEK_SET);
            }
            if( pHttpCon->TxFile.dynVarRcrdOffset== 0x00)
            {
                pHttpCon->file_sm = SM_PARSE_DYN_VAR_STRING;
            }
            else
            {
                TCPIP_TCP_PutIsReady(pHttpCon->socket);
                
                if((pHttpCon->TxFile.dynVarRcrdOffset-pHttpCon->TxFile.bytesReadCount) >= sizeof(sendDataBuffer))
                {
                    cntr = sizeof(sendDataBuffer);
                }
                else
                {
                    cntr = pHttpCon->TxFile.dynVarRcrdOffset - pHttpCon->TxFile.bytesReadCount;
                }
                len = (*httpFileShell->fileRead)(httpFileShell, pHttpCon->file, sendDataBuffer, cntr);
                _HTTP_FileRdCheck((len==cntr), __FILE__, __LINE__);
                bytesPut = TCPIP_TCP_ArrayPut(pHttpCon->socket, sendDataBuffer, len);
                //SYS_CMD_PRINT("cntr %d len %d BP %d\r\n", cntr, len, bytesPut);
                if (bytesPut != len)
                {
                    // we didn't transmit the entire buffer, so we have to seek backwards.
                    (*httpFileShell->fileSeek)(httpFileShell, pHttpCon->file, 0 - (len - bytesPut), SYS_FS_SEEK_CUR);
                }
                pHttpCon->TxFile.numBytes -= bytesPut;
                pHttpCon->TxFile.bytesReadCount+=bytesPut;
            }
            pHttpCon->TxFile.DynRcrdRdCount = (*httpFileShell->fileTell)(httpFileShell, DynVarFilePtr);
            if(pHttpCon->TxFile.dynVarRcrdOffset == pHttpCon->TxFile.bytesReadCount)
            {
                pHttpCon->file_sm = SM_PARSE_DYN_VAR_STRING;
            }
            (*httpFileShell->fileClose)(httpFileShell, DynVarFilePtr);
            break;

        case SM_PARSE_DYN_VAR_STRING:
            len = (*httpFileShell->fileRead)(httpFileShell, pHttpCon->file, sendDataBuffer, 1);
            _HTTP_FileRdCheck(len==1, __FILE__, __LINE__);
            pHttpCon->TxFile.numBytes-=1;
            pHttpCon->TxFile.bytesReadCount+=1;
            if(sendDataBuffer[0]=='~')
            {
                do
                {
                    len = (*httpFileShell->fileRead)(httpFileShell, pHttpCon->file, sendDataBuffer, 1);
                    _HTTP_FileRdCheck(len==1, __FILE__, __LINE__);
                    pHttpCon->TxFile.numBytes-=1;
                    pHttpCon->TxFile.bytesReadCount+=1;
                }
                while(sendDataBuffer[0]!='~');
            }

            //Continue to next state to process the dynamic variable callback
            pHttpCon->file_sm =SM_PROCESS_DYN_VAR_CALLBACK;
            pHttpCon->TxFile.EndOfCallBackFileFlag = true;

        case SM_PROCESS_DYN_VAR_CALLBACK:

            TCPIP_HTTP_Print(pHttpCon, pHttpCon->TxFile.dynVarCallBackID);

            if(pHttpCon->TxFile.EndOfCallBackFileFlag == true)
            {
                pHttpCon->TxFile.dynVarCntr-=1;
                pHttpCon->file_sm=SM_GET_DYN_VAR_FILE_RCRD;
                if(pHttpCon->TxFile.dynVarCntr == 0)
                {       
                    if (pHttpCon->TxFile.numBytes != 0)
                    {
                        pHttpCon->file_sm =SM_SERVE_TEXT_DATA;
                    }
                    pHttpCon->TxFile.lock_dynrcd = 0;
                }
            }
            else if (pHttpCon->callbackPos != 0 && pHttpCon->callbackPos != -1)
            {
                needBreak = true;
            }

            break;

        case SM_SERVE_TEXT_DATA:

            // If HashIndex do not match,that means no entry in the "FilRcrd.bin", means no dynamic variables for this wepage,
            //then proceed to serve the page as normal HTML text

            TCPIP_TCP_PutIsReady(pHttpCon->socket);
            
            if (pHttpCon->TxFile.numBytes >= sizeof(sendDataBuffer))
            {
                cntr = sizeof(sendDataBuffer);
            }
            else
            {
                cntr = pHttpCon->TxFile.numBytes;
            }
            len = (*httpFileShell->fileRead)(httpFileShell, pHttpCon->file, sendDataBuffer, cntr);
            _HTTP_FileRdCheck(len==cntr, __FILE__, __LINE__);
            bytesPut = TCPIP_TCP_ArrayPut(pHttpCon->socket, sendDataBuffer, len);
            if (bytesPut != len)
            {
                // we didn't transmit the entire buffer, so we have to seek backwards.
                (*httpFileShell->fileSeek)(httpFileShell, pHttpCon->file, 0 - (len - bytesPut), SYS_FS_SEEK_CUR);
            }
            pHttpCon->TxFile.numBytes -=bytesPut;
            pHttpCon->TxFile.bytesReadCount+=bytesPut;

            break;

        default:
            return false;
    }

    if((((pHttpCon->TxFile.numBytes == 0)) && (pHttpCon->TxFile.EndOfCallBackFileFlag == true)) \
        || (fileErr==1))    // Exception on file reading
    {
        TCPIP_TCP_Flush(pHttpCon->socket);

        pHttpCon->TxFile.nameHashMatched = false;
        pHttpCon->TxFile.lock_dynrcd = 0;
        pHttpCon->TxFile.dynVarCallBackID = 0;
        pHttpCon->TxFile.DynRcrdRdCount = 0;
        
        pHttpCon->TxFile.bytesReadCount=0;
        pHttpCon->file_sm=SM_IDLE;
        pHttpCon->TxFile.fileTxDone = 1;

        fileErr = 0;
        return false;
    }

    return needBreak ? true : false;

}

/*****************************************************************************
  Function:
    static void HTTPHeaderParseLookup(HTTP_CONN* pHttpCon, int i)

  Description:
    Calls the appropriate header parser based on the index of the header
    that was read from the request.

  Precondition:
    None

  Parameters:
    i - the index of the string found in HTTPRequestHeaders

  Return Values:
    true - the end of the file was reached and reading is done
    false - more data remains to be read
  ***************************************************************************/
static void _HTTP_HeaderParseLookup(HTTP_CONN* pHttpCon, int i)
{
    // i corresponds to an index in HTTPRequestHeaders

#if defined(TCPIP_HTTP_USE_COOKIES)
    if(i == 0u)
    {
        _HTTP_HeaderParseCookie(pHttpCon);
        return;
    }
#endif

#if defined(TCPIP_HTTP_USE_AUTHENTICATION)    
    if(i == 1u)
    {
        _HTTP_HeaderParseAuthorization(pHttpCon);
        return;
    }
#endif

#if defined(TCPIP_HTTP_USE_POST)
    if(i == 2u)
    {
        _HTTP_HeaderParseContentLength(pHttpCon);
        return;
    }
#endif
}

/*****************************************************************************
  Function:
    static void HTTPHeaderParseAuthorization(HTTP_CONN* pHttpCon)

  Summary:
    Parses the "Authorization:" header for a request and verifies the
    credentials.

  Description:
    Parses the "Authorization:" header for a request.  For example, 
    "BASIC YWRtaW46cGFzc3dvcmQ=" is decoded to a user name of "admin" and
    a password of "password".  Once read, TCPIP_HTTP_UserAuthenticate is called from
    custom_http_app.c to determine if the credentials are acceptable.

    The return value of TCPIP_HTTP_UserAuthenticate is saved in pHttpCon->isAuthorized for
    later use by the application.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is ony available when TCPIP_HTTP_USE_AUTHENTICATION is defined.
  ***************************************************************************/
#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
static void _HTTP_HeaderParseAuthorization(HTTP_CONN* pHttpCon)
{
    uint16_t len;
    uint8_t buf[40];
    uint8_t *ptrBuf;

    // If auth processing is not required, return
    if(pHttpCon->isAuthorized & 0x80)
        return;

    // Clear the auth type ("BASIC ")
    TCPIP_TCP_ArrayGet(pHttpCon->socket, NULL, 6);

    // Find the terminating CRLF and make sure it's a multiple of four
    len = TCPIP_TCP_ArrayFind(pHttpCon->socket, HTTP_CRLF, HTTP_CRLF_LEN, 0, 0, false);
    len += 3;
    len &= 0xfc;
    len = mMIN(len, sizeof(buf)-4);

    // Read in 4 bytes at a time and decode (slower, but saves RAM)
    for(ptrBuf = buf; len > 0u; len-=4, ptrBuf+=3)
    {
        TCPIP_TCP_ArrayGet(pHttpCon->socket, ptrBuf, 4);
        TCPIP_Helper_Base64Decode(ptrBuf, 4, ptrBuf, 3);
    }

    // Null terminate both, and make sure there's at least two terminators
    *ptrBuf = '\0';
    for(len = 0, ptrBuf = buf; len < sizeof(buf); len++, ptrBuf++)
        if(*ptrBuf == ':')
            break;
    *(ptrBuf++) = '\0';

    // Verify credentials
    pHttpCon->isAuthorized = TCPIP_HTTP_UserAuthenticate(pHttpCon, buf, ptrBuf);

    return;
}
#endif

/*****************************************************************************
  Function:
    static void HTTPHeaderParseCookie(HTTP_CONN* pHttpCon)

  Summary:
    Parses the "Cookie:" headers for a request and stores them as GET
    variables.

  Description:
    Parses the "Cookie:" headers for a request.  For example, 
    "Cookie: name=Wile+E.+Coyote; order=ROCKET_LAUNCHER" is decoded to 
    "name=Wile+E.+Coyote&order=ROCKET_LAUNCHER&" and stored as any other 
    GET variable in pHttpCon->data.

    The user application can easily access these values later using the
    TCPIP_HTTP_ArgGet() and TCPIP_HTTP_ArgGet() functions.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is ony available when TCPIP_HTTP_USE_COOKIES is defined.
  ***************************************************************************/
#if defined(TCPIP_HTTP_USE_COOKIES)
static void _HTTP_HeaderParseCookie(HTTP_CONN* pHttpCon)
{
    uint16_t lenA, lenB;

    // Verify there's enough space
    lenB = TCPIP_TCP_ArrayFind(pHttpCon->socket, HTTP_CRLF, HTTP_CRLF_LEN, 0, 0, false);
    if(lenB >= (uint16_t)(pHttpCon->data + httpConnDataSize - pHttpCon->ptrData - 2))
    {// If not, overflow
        pHttpCon->httpStatus = HTTP_OVERFLOW;
        pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
        return;
    }

    // While a CRLF is not immediate, grab a cookie value
    while(lenB != 0u)
    {
        // Look for a ';' and use the shorter of that or a CRLF
        lenA = TCPIP_TCP_Find(pHttpCon->socket, ';', 0, 0, false);

        // Read to the terminator
        pHttpCon->ptrData += TCPIP_TCP_ArrayGet(pHttpCon->socket, pHttpCon->ptrData, mMIN(lenA, lenB));

        // Insert an & to anticipate another cookie
        *(pHttpCon->ptrData++) = '&';

        // If semicolon, trash it and whitespace
        if(lenA < lenB)
        {
            TCPIP_TCP_Get(pHttpCon->socket, NULL);
            while(TCPIP_TCP_Find(pHttpCon->socket, ' ', 0, 0, false) == 0u)
                TCPIP_TCP_Get(pHttpCon->socket, NULL);
        }

        // Find the new distance to the CRLF
        lenB = TCPIP_TCP_ArrayFind(pHttpCon->socket, HTTP_CRLF, HTTP_CRLF_LEN, 0, 0, false);
    }

    return;

}
#endif

/*****************************************************************************
  Function:
    static void HTTPHeaderParseContentLength(HTTP_CONN* pHttpCon)

  Summary:
    Parses the "Content-Length:" header for a request.

  Description:
    Parses the "Content-Length:" header to determine how many bytes of
    POST data to expect after the request.  This value is stored in 
    pHttpCon->byteCount.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is ony available when TCPIP_HTTP_USE_POST is defined.
  ***************************************************************************/
#if defined(TCPIP_HTTP_USE_POST)
static void _HTTP_HeaderParseContentLength(HTTP_CONN* pHttpCon)
{
    uint16_t len;
    uint8_t buf[10];

    // Read up to the CRLF (max 9 bytes or ~1GB)
    len = TCPIP_TCP_ArrayFind(pHttpCon->socket, HTTP_CRLF, HTTP_CRLF_LEN, 0, 0, false);
    if(len >= sizeof(buf))
    {
        pHttpCon->httpStatus = HTTP_BAD_REQUEST;
        pHttpCon->byteCount = 0;
        return;
    }   
    len = TCPIP_TCP_ArrayGet(pHttpCon->socket, buf, len);
    buf[len] = '\0';

    pHttpCon->byteCount = atol((char*)buf);
}
#endif

/*****************************************************************************
  Function:
    uint8_t* TCPIP_HTTP_URLDecode(uint8_t* cData)

  Summary:
    Parses a string from URL encoding to plain-text.

  Description:
    Parses a string from URL encoding to plain-text.  The following
    conversions are made: ??to ?0? ??to ?0? ??to ?? and
    ?xx?to a single hex byte.
 
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
  ***************************************************************************/
uint8_t* TCPIP_HTTP_URLDecode(uint8_t* cData)
{
    uint8_t *pRead, *pWrite;
    uint16_t wLen;
    uint8_t c;
    uint16_t hex;

    // Determine length of input
    wLen = strlen((char*)cData);

    // Read all characters in the string
    for(pRead = pWrite = cData; wLen != 0u; )
    {
        c = *pRead++;
        wLen--;

        if(c == '=' || c == '&')
            *pWrite++ = '\0';
        else if(c == '+')
            *pWrite++ = ' ';
        else if(c == '%')
        {
            if(wLen < 2u)
                wLen = 0;
            else
            {
                ((uint8_t*)&hex)[1] = *pRead++;
                ((uint8_t*)&hex)[0] = *pRead++;
                wLen--;
                wLen--;
                *pWrite++ = hexatob(hex);
            }
        }
        else
            *pWrite++ = c;
    }

    // Double null terminate the last value
    *pWrite++ = '\0';
    *pWrite = '\0';

    return pWrite;
}

/*****************************************************************************
  Function:
    const uint8_t* TCPIP_HTTP_ArgGet(const uint8_t* cData, const uint8_t* cArg)

  Summary:
    Locates a form field value in a given data array.

  Description:
    Searches through a data array to find the value associated with a
    given argument.  It can be used to find form field values in data
    received over GET or POST.
    
    The end of data is assumed to be reached when a null name parameter is
    encountered.  This requires the string to have an even number of 
    null-terminated strings, followed by an additional null terminator.

  Precondition:
    The data array has a valid series of null terminated name/value pairs.

  Parameters:
    data - the buffer to search
    arg - the name of the argument to find

  Returns:
    A pointer to the argument value, or NULL if not found.
  ***************************************************************************/
const uint8_t* TCPIP_HTTP_ArgGet(const uint8_t* cData, const uint8_t* cArg)
{
    // Search through the array while bytes remain
    while(*cData != '\0')
    { 
        // Look for arg at current position
        if(!strcmp((const char*)cArg, (const char*)cData))
        {// Found it, so return parameter
            return cData + strlen((const char*)cArg) + 1;
        }

        // Skip past two strings (NUL bytes)
        cData += strlen((const char*)cData) + 1;
        cData += strlen((const char*)cData) + 1;
    }

    // Return NULL if not found
    return NULL;
}


/*****************************************************************************
  Function:
    HTTP_READ_STATUS TCPIP_HTTP_PostNameRead(uint8_t* cData, uint16_t wLen)

  Summary:
    Reads a name from a URL encoded string in the TCP buffer.

  Description:
    Reads a name from a URL encoded string in the TCP buffer.  This function
    is meant to be called from an TCPIP_HTTP_PostExecute callback to facilitate
    easier parsing of incoming data.  This function also prevents buffer
    overflows by forcing the programmer to indicate how many bytes are
    expected.  At least 2 extra bytes are needed in cData over the maximum
    length of data expected to be read.
    
    This function will read until the next '=' character, which indicates the
    end of a name parameter.  It assumes that the front of the buffer is
    the beginning of the name paramter to be read.
    
    This function properly updates pHttpCon->byteCount by decrementing it
    by the number of bytes read.  It also removes the delimiting '=' from
    the buffer.

  Precondition:
    Front of TCP buffer is the beginning of a name parameter, and the rest of
    the TCP buffer contains a URL-encoded string with a name parameter 
    terminated by a '=' character.

  Parameters:
    cData - where to store the name once it is read
    wLen - how many bytes can be written to cData

  Return Values:
    HTTP_READ_OK - name was successfully read
    HTTP_READ_TRUNCTATED - entire name could not fit in the buffer, so the
                            value was truncated and data has been lost
    HTTP_READ_INCOMPLETE - entire name was not yet in the buffer, so call
                            this function again later to retrieve
  ***************************************************************************/
#if defined(TCPIP_HTTP_USE_POST)
HTTP_READ_STATUS TCPIP_HTTP_PostNameRead(HTTP_CONN_HANDLE connHandle, uint8_t* cData, uint16_t wLen)
{
    HTTP_READ_STATUS status;
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;

    status = _HTTP_ReadTo(pHttpCon, '=', cData, wLen);

    // Decode the data (if not reading to null or blank) and return
    if(cData && *cData)
    {
        TCPIP_HTTP_URLDecode(cData);
    }
    return status;
}   
#endif

/*****************************************************************************
  Function:
    HTTP_READ_STATUS TCPIP_HTTP_PostValueRead(uint8_t* cData, uint16_t wLen)

  Summary:
    Reads a value from a URL encoded string in the TCP buffer.

  Description:
    Reads a value from a URL encoded string in the TCP buffer.  This function
    is meant to be called from an TCPIP_HTTP_PostExecute callback to facilitate
    easier parsing of incoming data.  This function also prevents buffer
    overflows by forcing the programmer to indicate how many bytes are
    expected.  At least 2 extra bytes are needed in cData above the maximum
    length of data expected to be read.
    
    This function will read until the next '&' character, which indicates the
    end of a value parameter.  It assumes that the front of the buffer is
    the beginning of the value paramter to be read.  If pHttpCon->byteCount
    indicates that all expected bytes are in the buffer, it assumes that 
    all remaining data is the value and acts accordingly.
    
    This function properly updates pHttpCon->byteCount by decrementing it
    by the number of bytes read.  The terminating '&' character is also 
    removed from the buffer.
    
  Precondition:
    Front of TCP buffer is the beginning of a name parameter, and the rest of
    the TCP buffer contains a URL-encoded string with a name parameter 
    terminated by a '=' character.

  Parameters:
    cData - where to store the value once it is read
    wLen - how many bytes can be written to cData

  Return Values:
    HTTP_READ_OK - value was successfully read
    HTTP_READ_TRUNCTATED - entire value could not fit in the buffer, so the
                            value was truncated and data has been lost
    HTTP_READ_INCOMPLETE - entire value was not yet in the buffer, so call
                            this function again later to retrieve
  ***************************************************************************/
#if defined(TCPIP_HTTP_USE_POST)
HTTP_READ_STATUS TCPIP_HTTP_PostValueRead(HTTP_CONN_HANDLE connHandle, uint8_t* cData, uint16_t wLen)
{
    HTTP_READ_STATUS status;
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;

    // Try to read the value
    status = _HTTP_ReadTo(pHttpCon, '&', cData, wLen);

    // If read was incomplete, check if we're at the end
    if(status == HTTP_READ_INCOMPLETE)
    {
        // If all data has arrived, read all remaining data
        if(pHttpCon->byteCount == TCPIP_TCP_GetIsReady(pHttpCon->socket))
            status = _HTTP_ReadTo(pHttpCon, '\0', cData, wLen);
    }

    // Decode the data (if not reading to null or blank) and return
    if(cData && *cData)
        TCPIP_HTTP_URLDecode(cData);
    return status;
}   
#endif

/*****************************************************************************
  Function:
    static HTTP_READ_STATUS HTTPReadTo(HTTP_CONN* pHttpCon, uint8_t cDelim, uint8_t* cData, uint16_t wLen)

  Summary:
    Reads to a buffer until a specified delimiter character.

  Description:
    Reads from the TCP buffer to cData until either cDelim is reached, or
    until wLen - 2 bytes have been read.  The value read is saved to cData and 
    null terminated.  (wLen - 2 is used so that the value can be passed to
    TCPIP_HTTP_URLDecode later, which requires a null terminator plus one extra free
    byte.)
    
    The delimiter character is removed from the buffer, but not saved to 
    cData. If all data cannot fit into cData, it will still be removed from 
    the buffer but will not be saved anywhere.

    This function properly updates pHttpCon->byteCount by decrementing it
    by the number of bytes read. 

  Precondition:
    None

  Parameters:
    cDelim - the character at which to stop reading, or NULL to read to
             the end of the buffer
    cData - where to store the data being read
    wLen - how many bytes can be written to cData

  Return Values:
    HTTP_READ_OK - data was successfully read
    HTTP_READ_TRUNCTATED - entire data could not fit in the buffer, so the
                            data was truncated and data has been lost
    HTTP_READ_INCOMPLETE - delimiter character was not found
  ***************************************************************************/
#if defined(TCPIP_HTTP_USE_POST)
static HTTP_READ_STATUS _HTTP_ReadTo(HTTP_CONN* pHttpCon, uint8_t cDelim, uint8_t* cData, uint16_t wLen)
{
    HTTP_READ_STATUS status;
    uint16_t wPos;

    // Either look for delimiter, or read all available data
    if(cDelim)
        wPos = TCPIP_TCP_Find(pHttpCon->socket, cDelim, 0, 0, false);
    else
        wPos = TCPIP_TCP_GetIsReady(pHttpCon->socket);

    // If not found, return incomplete
    if(wPos == 0xffff)
        return HTTP_READ_INCOMPLETE;

    // Read the value
    if(wLen < 2u && cData != NULL)
    {// Buffer is too small, so read to NULL instead
        pHttpCon->byteCount -= TCPIP_TCP_ArrayGet(pHttpCon->socket, NULL, wPos);
        status = HTTP_READ_TRUNCATED;
    }
    else if(cData == NULL)
    {// Just remove the data
        pHttpCon->byteCount -= TCPIP_TCP_ArrayGet(pHttpCon->socket, NULL, wPos);
        status = HTTP_READ_OK;
    }
    else if(wPos > wLen - 2)
    {// Read data, but truncate at max length
        pHttpCon->byteCount -= TCPIP_TCP_ArrayGet(pHttpCon->socket, cData, wLen - 2);
        pHttpCon->byteCount -= TCPIP_TCP_ArrayGet(pHttpCon->socket, NULL, wPos - (wLen - 2));
        cData[wLen - 2] = '\0';
        status = HTTP_READ_TRUNCATED;
    }
    else
    {// Read the data normally
        pHttpCon->byteCount -= TCPIP_TCP_ArrayGet(pHttpCon->socket, cData, wPos);
        cData[wPos] = '\0';
        status = HTTP_READ_OK;
    }

    // Remove the delimiter
    if(cDelim)
        pHttpCon->byteCount -= TCPIP_TCP_Get(pHttpCon->socket, NULL);

    return status;
}   
#endif

/*****************************************************************************
  Function:
    HTTP_IO_RESULT HTTPMPFSUpload(HTTP_CONN* pHttpCon)

  Summary:
    Saves a file uploaded via POST as the new MPFS image in EEPROM or 
    external Flash.

  Description:
    Allows the MPFS image in EEPROM or external Flash to be updated via a 
    web page by accepting a file upload and storing it to the external memory.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    HTTP_IO_DONE - on success
    HTTP_IO_NEED_DATA - if more data is still expected

  Remarks:
    This function is only available when MPFS uploads are enabled and
    the MPFS image is stored in EEPROM.

  ***************************************************************************/
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
#define     SYS_FS_MEDIA_SECTOR_SIZE        512     
#define MPFS_UPLOAD_WRITE_BUFFER_SIZE       (4 * 1024)
static HTTP_IO_RESULT TCPIP_HTTP_MPFSUpload(HTTP_CONN* pHttpCon)
{
    uint8_t nvmBuffer[sizeof(MPFS_SIGNATURE) - 1];  // large enough to hold the MPFS signature
    uint16_t lenA, lenB;
    uint32_t nSectors=0;
    uint32_t mpfsAllocSize;

    switch(pHttpCon->httpStatus)
    {
        // New upload, so look for the CRLFCRLF
        case HTTP_MPFS_UP:
            pHttpCon->uploadSectNo = 0;
            lenA = TCPIP_TCP_ArrayFind(pHttpCon->socket, (const uint8_t*)"\r\n\r\n", 4, 0, 0, false);

            if(lenA == 0xffff)
            {// no EOL found, remove as much as possible
                lenA = TCPIP_TCP_ArrayGet(pHttpCon->socket, NULL, TCPIP_TCP_GetIsReady(pHttpCon->socket) - 4);
                pHttpCon->byteCount -= lenA;
                break;
            }

            // Found the EOL separator, remove all data up to and including
            lenA = TCPIP_TCP_ArrayGet(pHttpCon->socket, NULL, lenA + 4);
            pHttpCon->byteCount -= (lenA + 4);

            pHttpCon->httpStatus = HTTP_MPFS_SIGNATURE;
            // intentional no break

        case HTTP_MPFS_SIGNATURE:

            // Make sure first 6 bytes are also in
            if(TCPIP_TCP_GetIsReady(pHttpCon->socket) < sizeof(MPFS_SIGNATURE) - 1 )
            {
                return HTTP_IO_NEED_DATA;
            }
            lenA = TCPIP_TCP_ArrayGet(pHttpCon->socket, nvmBuffer, sizeof(MPFS_SIGNATURE) - 1);
            pHttpCon->byteCount -= lenA;
            if(memcmp(nvmBuffer, (const void*)MPFS_SIGNATURE, sizeof(MPFS_SIGNATURE)-1) == 0)
            {   // Read as Ver 2.1
                // allocate the buffer size as a multiple of sector size
                mpfsAllocSize = ((MPFS_UPLOAD_WRITE_BUFFER_SIZE + (SYS_FS_MEDIA_SECTOR_SIZE - 1)) / SYS_FS_MEDIA_SECTOR_SIZE) * SYS_FS_MEDIA_SECTOR_SIZE;
                if(pHttpCon->uploadBufferStart == 0)
                {
                    pHttpCon->uploadBufferStart = (uint8_t*)(*http_malloc_fnc)(mpfsAllocSize);
                    SYS_FS_Unmount(MPFS_UPLOAD_MOUNT_PATH);
                }

                if(pHttpCon->uploadBufferStart != 0)
                {
                    pHttpCon->uploadBufferEnd = pHttpCon->uploadBufferStart + mpfsAllocSize;
                    memcpy(pHttpCon->uploadBufferStart, MPFS_SIGNATURE, sizeof(MPFS_SIGNATURE) - 1);
                    pHttpCon->uploadBufferCurr = pHttpCon->uploadBufferStart + sizeof(MPFS_SIGNATURE) - 1;
                    pHttpCon->httpStatus = HTTP_MPFS_OK;
                    return HTTP_IO_WAITING;
                }
            }
            pHttpCon->httpStatus = HTTP_MPFS_ERROR;
            return HTTP_IO_WAITING;

        case HTTP_MPFS_ERROR:
            if(pHttpCon->uploadBufferStart != 0)
            {
                (*http_free_fnc)(pHttpCon->uploadBufferStart);
                pHttpCon->uploadBufferStart = 0;
            }
            pHttpCon->byteCount -= TCPIP_TCP_GetIsReady(pHttpCon->socket);
            TCPIP_TCP_Discard(pHttpCon->socket);
            if(pHttpCon->byteCount < 100u || pHttpCon->byteCount > 0x80000000u)
            {// If almost all data was read, or if we overflowed, then return
                pHttpCon->sm = SM_HTTP_SERVE_HEADERS;
                return HTTP_IO_DONE;
            }
            break;

        case HTTP_MPFS_OK:
            lenA = TCPIP_TCP_GetIsReady(pHttpCon->socket);
            if(lenA > pHttpCon->uploadBufferEnd - pHttpCon->uploadBufferCurr)
            {
                lenA = pHttpCon->uploadBufferEnd - pHttpCon->uploadBufferCurr;
            }

            if(lenA > pHttpCon->byteCount)
            {
                lenA = pHttpCon->byteCount;
            }

            if(lenA == 0 )
            {
                return HTTP_IO_WAITING;
            }

            lenB = TCPIP_TCP_ArrayGet(pHttpCon->socket, pHttpCon->uploadBufferCurr, lenA);
            pHttpCon->uploadBufferCurr += lenB;
            pHttpCon->byteCount -= lenB;

            // check that upload buffer is full
            if(pHttpCon->uploadBufferCurr != pHttpCon->uploadBufferEnd && pHttpCon->byteCount != 0)
            {
                return HTTP_IO_WAITING;
            }

            nSectors = ((pHttpCon->uploadBufferCurr - pHttpCon->uploadBufferStart) + SYS_FS_MEDIA_SECTOR_SIZE - 1) / (SYS_FS_MEDIA_SECTOR_SIZE);

            pHttpCon->uploadBuffHandle = SYS_FS_MEDIA_MANAGER_SectorWrite(MPFS_UPLOAD_DISK_NO, pHttpCon->uploadSectNo , pHttpCon->uploadBufferStart,
                        nSectors);
            if ( pHttpCon->uploadBuffHandle == SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID)
            {
                pHttpCon->httpStatus = HTTP_MPFS_ERROR;
                SYS_CONSOLE_MESSAGE(" Sector Write: failed!\r\n");
                return HTTP_IO_WAITING;
            }

            // everything fine
            pHttpCon->uploadSectNo += nSectors;
            pHttpCon->httpStatus = HTTP_MPFS_WAIT;
            return HTTP_IO_WAITING;

        case HTTP_MPFS_WAIT:
            // wait for the write transaction to complete
            if(SYS_FS_MEDIA_MANAGER_CommandStatusGet(MPFS_UPLOAD_DISK_NO, pHttpCon->uploadBuffHandle) != SYS_FS_MEDIA_COMMAND_COMPLETED)
            {   // wait some more
                return HTTP_IO_WAITING;
            }

            pHttpCon->httpStatus = HTTP_MPFS_OK;
            pHttpCon->uploadBufferCurr = pHttpCon->uploadBufferStart;

            if(pHttpCon->byteCount == 0u)
            {   // we're done
                (*http_free_fnc)(pHttpCon->uploadBufferStart);
                pHttpCon->uploadBufferStart = 0;

                if(SYS_FS_Mount(MPFS_UPLOAD_NVM_VOL, MPFS_UPLOAD_MOUNT_PATH, MPFS2, 0, NULL)  != SYS_FS_RES_FAILURE)
                {
                    pHttpCon->sm = SM_HTTP_PROCESS_REQUEST;
                    SYS_CONSOLE_MESSAGE("\r\nMPFSUPLOAD completed\r\n");
                    return HTTP_IO_DONE;
                }
                else
                {
                    pHttpCon->sm = SM_HTTP_DISCONNECT;
                    SYS_CONSOLE_MESSAGE("\r\nMPFSUPLOAD failed: cannot mount\r\n");
                    pHttpCon->httpStatus = HTTP_MPFS_ERROR;
                    return HTTP_MPFS_ERROR;
                }
            }

            // else, more data to come
            return HTTP_IO_WAITING;

        default:
            break;
    }

    return HTTP_IO_NEED_DATA;
}//TCPIP_HTTP_MPFSUpload

#endif //defined (TCPIP_HTTP_FILE_UPLOAD_ENABLE)

/*****************************************************************************
  Function:
    void TCPIP_HTTP_FileInclude(const uint8_t* cFile)

  Summary:
    Writes a file byte-for-byte to the currently loaded TCP socket.

  Description:
    Allows an entire file to be included as a dynamic variable, providing
    a basic templating system for HTML web pages.  This reduces unneeded
    duplication of visual elements such as headers, menus, etc.

    When pHttpCon->callbackPos is 0, the file is opened and as many bytes
    as possible are written.  The current position is then saved to 
    pHttpCon->callbackPos and the file is closed.  On subsequent calls, 
    reading begins at the saved location and continues.  Once the end of
    the input file is reached, pHttpCon->callbackPos is set back to 0 to 
    indicate completion.

  Precondition:
    None

  Parameters:
    cFile - the name of the file to be sent

  Returns:
    None
    
  Remarks:
    Users should not call this function directly, but should instead add
    dynamic variables in the form of ~inc:filename.ext~ in their HTML code
    to include (for example) the file "filename.ext" at that specified
    location.  The MPFS2 Generator utility will handle the rest.
  ***************************************************************************/
void TCPIP_HTTP_FileInclude(HTTP_CONN_HANDLE connHandle, const uint8_t* cFile)
{
    SYS_FS_HANDLE fp;
    uint32_t cntr=0;
    uint8_t incDataBuffer[HTTP_INC_DATABUF_SIZE];
    uint32_t availbleTcpBuffSize,len;
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    uint16_t bytesPut;


    if((fp = (*httpFileShell->fileOpen)(httpFileShell, (const char *)cFile, SYS_FS_FILE_OPEN_READ)) == SYS_FS_HANDLE_INVALID)
    {// File not found, so abort
        pHttpCon->TxFile.incFileRdCnt = 0;
        return;
    }
    else
    {// The file opened successfully, so seek the file
        if(pHttpCon->TxFile.lock_hdr == 0)
        {
            pHttpCon->TxFile.numBytesHdrFile = (*httpFileShell->fileSize)(httpFileShell, fp);
            pHttpCon->TxFile.incFileRdCnt = 0x00;
            if ((pHttpCon->TxFile.numBytesHdrFile == -1) || (pHttpCon->TxFile.numBytesHdrFile == 0)) {
                (*httpFileShell->fileClose)(httpFileShell, fp);
                pHttpCon->TxFile.EndOfCallBackFileFlag=0x01;
                return;   
            }
            pHttpCon->TxFile.lock_hdr=1;
        }
        (*httpFileShell->fileSeek)(httpFileShell, fp, pHttpCon->TxFile.incFileRdCnt, SYS_FS_SEEK_SET);
    }

    availbleTcpBuffSize = TCPIP_TCP_PutIsReady(pHttpCon->socket);

    if(availbleTcpBuffSize == 0)
    {
        // Save the new address and close the file
        pHttpCon->TxFile.incFileRdCnt = (*httpFileShell->fileTell)(httpFileShell, fp);
        (*httpFileShell->fileClose)(httpFileShell, fp);
        pHttpCon->TxFile.EndOfCallBackFileFlag=0x00;
        return;
    }

    if(pHttpCon->TxFile.numBytesHdrFile <= sizeof(incDataBuffer))
    {
        cntr = pHttpCon->TxFile.numBytesHdrFile;
    }
    else
    {
        cntr = sizeof(incDataBuffer);
    }
    len = (*httpFileShell->fileRead)(httpFileShell, fp, incDataBuffer, cntr);
    _HTTP_FileRdCheck(len==cntr, __FILE__, __LINE__);
    bytesPut = TCPIP_TCP_ArrayPut(pHttpCon->socket, incDataBuffer, len);
    if (bytesPut != len)
    {
        // we didn't transmit the entire buffer, so we have to seek backwards.
        (*httpFileShell->fileSeek)(httpFileShell, fp, 0 - (len - bytesPut), SYS_FS_SEEK_CUR);
    }
    pHttpCon->TxFile.numBytesHdrFile -= bytesPut;

    if(pHttpCon->TxFile.numBytesHdrFile == 0)
    {// If no bytes were read, an EOF was reached
        (*httpFileShell->fileClose)(httpFileShell, fp);
        pHttpCon->TxFile.incFileRdCnt = 0x00;
        pHttpCon->TxFile.EndOfCallBackFileFlag=0x01;
        pHttpCon->TxFile.lock_hdr=0;
        return;
    }

    // Save the new address and close the file
    pHttpCon->TxFile.incFileRdCnt = (*httpFileShell->fileTell)(httpFileShell, fp);
    (*httpFileShell->fileClose)(httpFileShell, fp);
    pHttpCon->TxFile.EndOfCallBackFileFlag=-1;
}

SYS_FS_HANDLE TCPIP_HTTP_CurrentConnectionFileGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->file;
}

uint16_t TCPIP_HTTP_CurrentConnectionPostSmGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->smPost;
}

void TCPIP_HTTP_CurrentConnectionPostSmSet(HTTP_CONN_HANDLE connHandle, uint16_t stat)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->smPost = stat;
}

uint8_t* TCPIP_HTTP_CurrentConnectionDataBufferGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->data;
}

uint32_t TCPIP_HTTP_CurrentConnectionCallbackPosGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->callbackPos;
}

uint16_t  TCPIP_HTTP_CurrentConnectionDataBufferSizeGet(HTTP_CONN_HANDLE connHandle)
{
    return httpConnDataSize;
}

void TCPIP_HTTP_CurrentConnectionCallbackPosSet(HTTP_CONN_HANDLE connHandle, uint32_t pos)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->callbackPos = pos;
    pHttpCon->TxFile.EndOfCallBackFileFlag = (pos == 0);
}

void TCPIP_HTTP_CurrentConnectionStatusSet(HTTP_CONN_HANDLE connHandle, HTTP_STATUS stat)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->httpStatus = stat;
}

HTTP_STATUS TCPIP_HTTP_CurrentConnectionStatusGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->httpStatus;
}



void TCPIP_HTTP_CurrentConnectionHasArgsSet(HTTP_CONN_HANDLE connHandle, uint8_t args)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->hasArgs = args;
}

uint8_t TCPIP_HTTP_CurrentConnectionHasArgsGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->hasArgs;
}

uint32_t TCPIP_HTTP_CurrentConnectionByteCountGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->byteCount;
}

void TCPIP_HTTP_CurrentConnectionByteCountSet(HTTP_CONN_HANDLE connHandle, uint32_t byteCount)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->byteCount = byteCount;
}

void TCPIP_HTTP_CurrentConnectionByteCountDec(HTTP_CONN_HANDLE connHandle, uint32_t byteCount)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->byteCount -= byteCount;
}

TCP_SOCKET TCPIP_HTTP_CurrentConnectionSocketGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->socket;
}

uint8_t TCPIP_HTTP_CurrentConnectionIsAuthorizedGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->isAuthorized;
}

void TCPIP_HTTP_CurrentConnectionIsAuthorizedSet(HTTP_CONN_HANDLE connHandle, uint8_t auth)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->isAuthorized = auth;
}

void TCPIP_HTTP_CurrentConnectionUserDataSet(HTTP_CONN_HANDLE connHandle, const void* uData)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->userData = uData;
}

const void* TCPIP_HTTP_CurrentConnectionUserDataGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->userData;
}

void TCPIP_HTTP_CurrentConnectionSMSet(HTTP_CONN_HANDLE connHandle, uint8_t stat)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->sm = stat;
}

HTTP_STATUS TCPIP_HTTP_CurrentConnectionSMGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->sm;
}
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
void TCPIP_HTTP_CurrentConnectionUploadBufferStartSet(HTTP_CONN_HANDLE connHandle, uint8_t* uploadBufferStart)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->uploadBufferStart = uploadBufferStart;
}

const void* TCPIP_HTTP_CurrentConnectionUploadBufferStartGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->uploadBufferStart;
}

void TCPIP_HTTP_CurrentConnectionUploadBufferEndSet(HTTP_CONN_HANDLE connHandle, uint8_t* uploadBufferEnd)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->uploadBufferEnd = uploadBufferEnd;
}

const void* TCPIP_HTTP_CurrentConnectionUploadBufferEndGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->uploadBufferEnd;
}

void TCPIP_HTTP_CurrentConnectionUploadBufferCurrSet(HTTP_CONN_HANDLE connHandle, uint8_t* uploadBufferCurr)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    pHttpCon->uploadBufferCurr = uploadBufferCurr;
}

const void* TCPIP_HTTP_CurrentConnectionUploadBufferCurrGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->uploadBufferCurr;
}
#endif  // defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
int TCPIP_HTTP_CurrentConnectionIndexGet(HTTP_CONN_HANDLE connHandle)
{
    HTTP_CONN* pHttpCon = (HTTP_CONN*)connHandle;
    return pHttpCon->connIx;
}

HTTP_CONN_HANDLE TCPIP_HTTP_CurrentConnectionHandleGet(int connIx)
{
    if(connIx < httpConnNo)
    {
        return httpConnCtrl + connIx;
    }

    return 0;
}

int TCPIP_HTTP_ActiveConnectionCountGet(int* pOpenCount)
{
    HTTP_CONN* pHttpCon;
    int connIx;
    int connCount, openCount;

    connCount = openCount = 0;

    pHttpCon = httpConnCtrl + 0;
    for(connIx = 0; connIx < httpConnNo; connIx++, pHttpCon++)
    {
        if(pHttpCon->socket != INVALID_SOCKET)
        {
            openCount++;
            if(pHttpCon->sm != SM_HTTP_IDLE)
            {
                connCount++;
            }
        }
    }

    if(pOpenCount)
    {
        *pOpenCount = openCount;
    }

    return connCount;
}



#endif  // defined(TCPIP_STACK_USE_HTTP_SERVER)
