/*******************************************************************************
  HyperText Transfer Protocol (HTTP) Server

  File Name:
    http_server.c

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Serves dynamic pages to web browsers such as Microsoft Internet 
      Explorer, Mozilla Firefox, etc.
    - Reference: RFC 2616
*******************************************************************************/

/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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



#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_HTTP_SERVER_V2

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_HTTP_SERVER_V2)


#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)

#define MPFS_SIGNATURE "MPFS\x02\x01"
// size of the MPFS upload operation write buffer
#define MPFS_UPLOAD_WRITE_BUFFER_SIZE   (4 * 1024)

#include "system/fs/sys_fs_media_manager.h"
#endif  // defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)

#include "tcpip/src/common/sys_fs_shell.h"

#include "http_server_private.h"

#include "net_pres/pres/net_pres_socketapi.h"

#define SYS_FS_ATTR_ZIP_COMPRESSED  ((uint16_t)0x0001)
//#define TCPIP_HTTP_NET_FILE_ERR_DEBUG
/****************************************************************************
  Section:
    String Constants
  ***************************************************************************/
#define TCPIP_HTTP_CRLF         "\r\n"  // New line sequence
#define TCPIP_HTTP_CRLF_LEN     2       // size 
#define TCPIP_HTTP_NET_LINE_END     '\n'  // end of line
        

/****************************************************************************
Section:
File and Content Type Settings
 ***************************************************************************/
// File type extensions corresponding to TCPIP_HTTP_FILE_TYPE
static const char * const httpFileExtensions[] =
{
    "txt",          // TCPIP_HTTP_FILE_TYPE_TXT
    "htm",          // TCPIP_HTTP_FILE_TYPE_HTM
    "html",         // TCPIP_HTTP_FILE_TYPE_HTML
    "cgi",          // TCPIP_HTTP_FILE_TYPE_CGI
    "xml",          // TCPIP_HTTP_FILE_TYPE_XML
    "css",          // TCPIP_HTTP_FILE_TYPE_CSS
    "gif",          // TCPIP_HTTP_FILE_TYPE_GIF
    "png",          // TCPIP_HTTP_FILE_TYPE_PNG
    "jpg",          // TCPIP_HTTP_FILE_TYPE_JPG
    "js",           // TCPIP_HTTP_FILE_TYPE_JS
    "class",        // TCPIP_HTTP_FILE_TYPE_JVM
    "java",         // TCPIP_HTTP_FILE_TYPE_JVL
    "wav",          // TCPIP_HTTP_FILE_TYPE_WAV
};

// Content-type strings corresponding to TCPIP_HTTP_FILE_TYPE
static const char * const httpContentTypes[] =
{
    "text/plain",            // TCPIP_HTTP_FILE_TYPE_TXT
    "text/html",             // TCPIP_HTTP_FILE_TYPE_HTM
    "text/html",             // TCPIP_HTTP_FILE_TYPE_HTML
    "text/html",             // TCPIP_HTTP_FILE_TYPE_CGI
    "text/xml",              // TCPIP_HTTP_FILE_TYPE_XML
    "text/css",              // TCPIP_HTTP_FILE_TYPE_CSS
    "image/gif",             // TCPIP_HTTP_FILE_TYPE_GIF
    "image/png",             // TCPIP_HTTP_FILE_TYPE_PNG
    "image/jpeg",            // TCPIP_HTTP_FILE_TYPE_JPG
    "application/x-javascript",   // TCPIP_HTTP_FILE_TYPE_JS
    "application/java-vm",   // TCPIP_HTTP_FILE_TYPE_JVM
    "application/java-vm",   // TCPIP_HTTP_FILE_TYPE_JVL
    "audio/x-wave",          // TCPIP_HTTP_FILE_TYPE_WAV
};

// File type extensions that can carry dynamic content
static const char* httpDynFileExtensions[] =
{
    "inc",          // include file
    "htm",          // TCPIP_HTTP_FILE_TYPE_HTM
    "html",         // TCPIP_HTTP_FILE_TYPE_HTML
    "cgi",          // TCPIP_HTTP_FILE_TYPE_CGI
    "xml",          // TCPIP_HTTP_FILE_TYPE_XML
};

// connection discard type
typedef enum
{
    TCPIP_HTTP_DISCARD_NOT = 0,    // no discard needed
    TCPIP_HTTP_DISCARD_WAIT_DISCON, // discard after disconnect is done
    TCPIP_HTTP_DISCARD_WAIT_NOT,    // discard immediately
}TCPIP_HTTP_DISCARD_TYPE;

/****************************************************************************
  Section:
    Commands and Server Responses
  ***************************************************************************/
// the string that will always be prepended to the server'`s response
#define TCPIP_HTTP_NET_HEADER_PREFIX    "HTTP/1.1 "

// header for non persistent connections
#define TCPIP_HTTP_CONNECTION_CLOSE "Connection: close\r\n"

// Initial response strings (Corresponding to TCPIP_HTTP_STATUS)
static const char * const HTTPResponseHeaders[] =
{
    "200 OK\r\n",                               // TCPIP_HTTP_STAT_GET
    "200 OK\r\n",                               // TCPIP_HTTP_STAT_POST
    "400 Bad Request\r\n",                      // TCPIP_HTTP_STAT_BAD_REQUEST
    "401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"Protected\"\r\n",  // TCPIP_HTTP_STAT_UNAUTHORIZED
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    "404 Not found\r\nContent-Type: text/html\r\n", // TCPIP_HTTP_STAT_NOT_FOUND
#else       
    "404 Not found\r\n",                        // TCPIP_HTTP_STAT_NOT_FOUND
#endif
    "414 Request-URI Too Long\r\n",             // TCPIP_HTTP_STAT_OVERFLOW
    "500 Internal Server Error\r\n",            // TCPIP_HTTP_STAT_INTERNAL_SERVER_ERROR
    "501 Not Implemented\r\n",                  // TCPIP_HTTP_STAT_NOT_IMPLEMENTED
    0,                                          // TCPIP_HTTP_STAT_REDIRECT
    "403 Forbidden\r\n",                        // TCPIP_HTTP_STAT_TLS_REQUIRED
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    "200 OK\r\nContent-Type: text/html\r\n",    // TCPIP_HTTP_STAT_UPLOAD_FORM
    0,                                          // TCPIP_HTTP_STAT_UPLOAD_STARTED
    0,                                          // TCPIP_HTTP_STAT_UPLOAD_WRITE
    0,                                          // TCPIP_HTTP_STAT_UPLOAD_WRITE_WAIT
    "200 OK\r\nContent-Type: text/html\r\n",    // TCPIP_HTTP_STAT_UPLOAD_OK
    "500 Internal Server Error\r\nContent-Type: text/html\r\n", // TCPIP_HTTP_STAT_UPLOAD_ERROR
#endif

};

// correspond to the [TCPIP_HTTP_REDIR_TYPE_301, TCPIP_HTTP_REDIR_TYPE_308]
static const char* const HTTPRedirectHeaders[] =
{
    "301 Moved Permanently",        // TCPIP_HTTP_REDIR_TYPE_301
    "302 Found",                    // TCPIP_HTTP_REDIR_TYPE_302
    "303 See Other",                // TCPIP_HTTP_REDIR_TYPE_303
    "307 Temporary Redirect",       // TCPIP_HTTP_REDIR_TYPE_307
    "308 Permanent Redirect",       // TCPIP_HTTP_REDIR_TYPE_308
};

// Message strings (Corresponding to TCPIP_HTTP_STATUS)
static const char * const HTTPResponseMessages[] =
{
    
    0,                                                                  // TCPIP_HTTP_STAT_GET
    0,                                                                  // TCPIP_HTTP_STAT_POST
    "\r\n400 Bad Request: can't handle Content-Length\r\n",             // TCPIP_HTTP_STAT_BAD_REQUEST
    "\r\n401 Unauthorized: Password required\r\n",                      // TCPIP_HTTP_STAT_UNAUTHORIZED
    0,                                                                  // TCPIP_HTTP_STAT_NOT_FOUND
    "\r\n414 Request-URI Too Long: Buffer overflow detected\r\n",       // TCPIP_HTTP_STAT_OVERFLOW
    "\r\n500 Internal Server Error: Expected data not present\r\n",     // TCPIP_HTTP_STAT_INTERNAL_SERVER_ERROR
    "\r\n501 Not Implemented: Only GET and POST supported\r\n",         // TCPIP_HTTP_STAT_NOT_IMPLEMENTED
    0,                                                                  // TCPIP_HTTP_STAT_REDIRECT
    "\r\n403 Forbidden: TLS Required - use HTTPS\r\n",                  // TCPIP_HTTP_STAT_TLS_REQUIRED

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    0,                                                                  // TCPIP_HTTP_STAT_UPLOAD_FORM
    0,                                                                  // TCPIP_HTTP_STAT_UPLOAD_STARTED
    0,                                                                  // TCPIP_HTTP_STAT_UPLOAD_WRITE
    0,                                                                  // TCPIP_HTTP_STAT_UPLOAD_WRITE_WAIT
                                                                        // TCPIP_HTTP_STAT_UPLOAD_OK
    "\r\n<html><body style=\"margin:100px\"><b>FS Update Successful</b><p><a href=\"/\">Site main page</a></body></html>",
    0,                                                                  // TCPIP_HTTP_STAT_UPLOAD_ERROR
#endif
};

// functions for handling special header messages  (Corresponding to TCPIP_HTTP_STATUS)

static int _HTTP_HeaderMsg_Generic(TCPIP_HTTP_CONN* pHttpCon, char* buffer, size_t bufferSize);
static int _HTTP_HeaderMsg_NotFound(TCPIP_HTTP_CONN* pHttpCon, char* buffer, size_t bufferSize);
static int _HTTP_HeaderMsg_Redirect(TCPIP_HTTP_CONN* pHttpCon, char* buffer, size_t bufferSize);

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
static int _HTTP_HeaderMsg_UploadForm(TCPIP_HTTP_CONN* pHttpCon, char* buffer, size_t bufferSize);
static int _HTTP_HeaderMsg_UploadError(TCPIP_HTTP_CONN* pHttpCon, char* buffer, size_t bufferSize);
#endif

// header message function
typedef int(*_HTTP_HeaderMsgFnc)(TCPIP_HTTP_CONN* pHttpCon, char* buffer, size_t bufferSize);

// header message printing helper
static int _HTTP_HeaderMsg_Print(char* buffer, size_t bufferSize, const char* fmt, ...);

static const _HTTP_HeaderMsgFnc HTTPResponseFunctions[] =
{
    
    0,                              // TCPIP_HTTP_STAT_GET
    0,                              // TCPIP_HTTP_STAT_POST
    _HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_BAD_REQUEST
    _HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_UNAUTHORIZED
    _HTTP_HeaderMsg_NotFound,       // TCPIP_HTTP_STAT_NOT_FOUND
    _HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_OVERFLOW
    _HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_INTERNAL_SERVER_ERROR
    _HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_NOT_IMPLEMENTED
    _HTTP_HeaderMsg_Redirect,       // TCPIP_HTTP_STAT_REDIRECT
    _HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_TLS_REQUIRED
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    _HTTP_HeaderMsg_UploadForm,     // TCPIP_HTTP_STAT_UPLOAD_FORM
    0,                              // TCPIP_HTTP_STAT_UPLOAD_STARTED
    0,                              // TCPIP_HTTP_STAT_UPLOAD_WRITE
    0,                              // TCPIP_HTTP_STAT_UPLOAD_WRITE_WAIT
    _HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_UPLOAD_OK
    _HTTP_HeaderMsg_UploadError,    // TCPIP_HTTP_STAT_UPLOAD_ERROR
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
    HTTP Dynamic variables parsing
  ***************************************************************************/
// dynamic variables terminator 
#define TCPIP_HTTP_DYNVAR_DELIM     '~'

// dynamic variables keyword argument start
// usually arguments are in format: "variable(arg1, arg2, ...)"
// however some old keywords still use: "inc:fname"
#define TCPIP_HTTP_DYNVAR_KWORD_OLD_ARG_START     ':'

// dynamic variables keyword argument start
// arguments are in format: "variable(arg1, arg2, ...)"
#define TCPIP_HTTP_DYNVAR_ARG_START     '('

// dynamic variables keyword argument end
// arguments are in format: "variable(arg1, arg2, ...)"
#define TCPIP_HTTP_DYNVAR_ARG_END     ')'

// dynamic variables keyword argument separator
// arguments are in format: "variable(arg1, arg2, ...)"
// any space or , is a valid separator
#define TCPIP_HTTP_DYNVAR_ARG_SEP     " ,"

/****************************************************************************
  Section:
    HTTP SSI parsing
  ***************************************************************************/
// SSI keywords start
#define TCPIP_HTTP_SSI_COMMAND_START        "<!--#"
#define TCPIP_HTTP_SSI_COMMAND_START_CHAR   '<'
#define TCPIP_HTTP_SSI_COMMAND_END_CHAR     '>'
#define TCPIP_HTTP_SSI_COMMENT_DELIM        "--"
#define TCPIP_HTTP_SSI_ATTRIB_DELIM         '"'
// SSI command separator: space
#define TCPIP_HTTP_SSI_CMD_SEP              " "     // separates command from attribute pairs
#define TCPIP_HTTP_SSI_ATTR_SEP             "= "    // separate the attribute from value within a pair
#define TCPIP_HTTP_SSI_VALUE_SEP            "\""    // value delimiter within a pair

// dynamic variables/SSI, etc. ignore pattern start and end
// any dynamic variable within these keywords are ignored
#define TCPIP_HTTP_PARSE_IGNORE_BEGIN  "<code>"
#define TCPIP_HTTP_PARSE_IGNORE_END    "</code>"

// file extension separator
#define TCPIP_HTTP_FILE_EXT_SEP         '.'

// file path separator
#define TCPIP_HTTP_FILE_PATH_SEP         '/'

// list of dynamic variables that are keywords and can be processed internally 

static TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_NET_DefaultIncludeFile(TCPIP_HTTP_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const struct _tag_TCPIP_HTTP_USER_CALLBACK* pCBack);

static const TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY httpDynVarKeywords[] = 
{
    // keyWord      keyFlags                                        // dynamicPrint
    { "inc",        TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEFAULT_PROCESS,   TCPIP_HTTP_NET_DefaultIncludeFile},


};

   
/****************************************************************************
  Section:
    HTTP Connection State Global Variables
  ***************************************************************************/
static TCPIP_HTTP_INST** httpInstances = 0;
static size_t               httpInstNo = 0;         // number of present instances

static int                  httpInitCount = 0;      // module init counter


                                                        
static tcpipSignalHandle    httpSignalHandle = 0;

static const void*          httpMemH = 0;              // handle to be used in the TCPIP_HEAP_ calls

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
static void* _HTTP_Default_MallocFnc(size_t bytes); // internal HTTP malloc, thread protection
static void  _HTTP_Default_FreeFnc(void* ptr);      // internal HTTP free, thread protection

// current HTTP allocation functions
void* (*http_mallocFnc)(size_t bytes);
void  (*http_freeFnc)(void* ptr);

#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)

/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/
static bool _HTTP_VerifyConfig(const TCPIP_HTTP_SERVER_MODULE_CONFIG* httpInitData);
static TCPIP_HTTP_INST_RES _HTTP_InitInstance(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg);
static TCPIP_HTTP_INST_RES _HTTP_InitInstance_Ports(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg);
static TCPIP_HTTP_INST_RES _HTTP_CreatePortConnections(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_PORT_CONFIG* pCfgPort, TCPIP_HTTP_PORT_DCPT* pPort);
#if (_TCPIP_HTTP_USE_ACCESS_RULES != 0)
static TCPIP_HTTP_INST_RES _HTTP_CreateAccessRules(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg);
static void _HTTP_GetPageAccess(TCPIP_HTTP_CONN* pHttpCon, char* pageName, TCPIP_HTTP_ACCESS_RULE* accRule);
static bool _HTTP_Get_RedirectURI(const char* accessDir, const char* redirURI, char* redirBuff, size_t buffSize); 
static bool _HTTP_InstanceOwnsRule(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_ACCESS_RULE* pRule);
#endif  //  // (_TCPIP_HTTP_USE_ACCESS_RULES != 0)

static bool _HTTP_HeaderParseLookup(TCPIP_HTTP_CONN* pHttpCon, int reqIx);
#if defined(TCPIP_HTTP_USE_COOKIES)
static bool _HTTP_HeaderParseCookie(TCPIP_HTTP_CONN* pHttpCon);
#endif
#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
static bool _HTTP_HeaderParseAuthorization(TCPIP_HTTP_CONN* pHttpCon);
#endif
#if defined(TCPIP_HTTP_USE_POST)
static bool _HTTP_HeaderParseContentLength(TCPIP_HTTP_CONN* pHttpCon);
static TCPIP_HTTP_READ_STATUS _HTTP_ReadTo(TCPIP_HTTP_CONN* pHttpCon, uint8_t delim, uint8_t* buf, uint16_t len);
#endif
static uint16_t _HTTP_SktFifoRxFree(NET_PRES_SKT_HANDLE_T skt);

static bool _HTTP_DataTryOutput(TCPIP_HTTP_CONN* pHttpCon, const char* data, uint16_t dataLen, uint16_t checkLen);

static uint32_t _HTTP_ConnectionStringFind(TCPIP_HTTP_CONN* pHttpCon, const char* findStr, uint16_t wStart, uint16_t wSearchLen);

static uint16_t _HTTP_ConnectionCharFind(TCPIP_HTTP_CONN* pHttpCon, uint8_t cFind, uint16_t wStart, uint16_t wSearchLen);

static uint16_t _HTTP_ConnectionDiscard(TCPIP_HTTP_CONN* pHttpCon, uint16_t discardLen);

static void TCPIP_HTTP_NET_Process(void);

static void TCPIP_HTTP_NET_ProcessConnection(TCPIP_HTTP_CONN* pHttpCon);

static void _HTTPSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T skt, TCPIP_NET_HANDLE hNet, uint16_t sigType, const void* param);

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _HTTP_Cleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);
static void _HTTP_CloseConnections(TCPIP_HTTP_PORT_DCPT* pPort, TCPIP_NET_IF* pNetIf);
#else
#define _HTTP_Cleanup(stackCtrl)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void _HTTP_ConnCloseFile(TCPIP_HTTP_CONN* pHttpCon);

static TCPIP_HTTP_CHUNK_RES _HTTP_AddFileChunk(TCPIP_HTTP_CONN* pHttpCon, SYS_FS_HANDLE fH, const char* fName, TCPIP_HTTP_CHUNK_DCPT* pOwnDcpt);

static TCPIP_HTTP_CHUNK_DCPT* _HTTP_AllocChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_FLAGS flags, bool appendTail, TCPIP_HTTP_EVENT_TYPE* pEvType);

static void _HTTP_FreeChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);

static bool _HTTP_FileTypeIsDynamic(const char* fName);

static TCPIP_HTTP_CHUNK_RES _HTTP_ProcessChunks(TCPIP_HTTP_CONN* pHttpCon);

static uint16_t _HTTP_PrependStartHttpChunk(TCPIP_HTTP_CONN* pHttpCon, char* buffer, uint32_t chunkSize);

static uint16_t _HTTP_AppendEndHttpChunk(TCPIP_HTTP_CONN* pHttpCon, char* buffer, TCPIP_HTTP_CHUNK_END_TYPE endType);

static TCPIP_HTTP_CHUNK_RES _HTTP_ProcessFileChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);

static void _HTTP_Report_ConnectionEvent(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_EVENT_TYPE evType, const void* evInfo);

static TCPIP_HTTP_CHUNK_RES _HTTP_IncludeFile(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, const char* fName);

static char* _HTTP_ProcessFileLine(TCPIP_HTTP_CHUNK_DCPT* pChDcpt, char* lineBuffer, size_t buffLen, char** pDynStart, bool isEof);

static char* _HTTP_FileLineParse(TCPIP_HTTP_CHUNK_DCPT* pChDcpt, char* lineBuff, char** pEndProcess, bool verifyOnly);

static bool _HTTP_ConnCleanDisconnect(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_DISCARD_TYPE discardType);

static TCPIP_HTTP_CONN_STATE _HTTP_ProcessIdle(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ParseRequest(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ParseFileUpload(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ParseFileOpen(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ParseGetArgs(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ParseHeaders(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ProcessAuthenticate(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ProcessGet(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);
    
static TCPIP_HTTP_CONN_STATE _HTTP_ProcessPost(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ServeHeaders(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ServeCookies(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ServeBodyInit(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ServeBody(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ServeChunks(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ProcessDone(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ProcessError(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE _HTTP_ProcessDisconnect(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
static char* _HTTP_DynVarParse(char* dynVarBuff, char** pEndDyn, bool verifyOnly);
static bool  _HTTP_DynVarExtract(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pDynChDcpt, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt);
static const TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY* _HTTP_SearchDynVarKeyEntry(const char* keyword);
static TCPIP_HTTP_CHUNK_RES _HTTP_ProcessDynVarChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);
static TCPIP_HTTP_CHUNK_RES _HTTP_DynVarCallback(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);
static bool _HTTP_DynVarProcess(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);
static TCPIP_HTTP_DYNVAR_BUFF_DCPT* _HTTP_GetDynBuffDescriptor(TCPIP_HTTP_CHUNK_DCPT* pChDcpt);
static void _HTTP_ReleaseDynBuffDescriptor(TCPIP_HTTP_INST* pInstance, TCPIP_HTTP_DYNVAR_BUFF_DCPT* pDynDcpt);

static uint16_t _HTTP_StartHttpChunk(TCPIP_HTTP_CONN* pHttpCon, uint32_t chunkSize);
static uint16_t _HTTP_EndHttpChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_END_TYPE endType);
#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

#if (TCPIP_HTTP_SSI_PROCESS != 0)
static char*                                _HTTP_SSILineParse(char* lineBuff, char** pEndProcess, bool verifyOnly);
static bool                                 _HTTP_SSIExtract(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pDynChDcpt, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt);
static const TCPIP_HTTP_SSI_PROCESS_ENTRY*  _HTTP_SSIFindEntry(const char* ssiCmd);
static TCPIP_HTTP_CHUNK_RES                 _HTTP_ProcessSSIChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);
static OA_HASH_DCPT*                        _HTTP_SSICreateHash(TCPIP_HTTP_INST* pInstance);

#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
// dynamic manipulation should be enabled by default
static size_t TCPIP_HTTP_SSI_HashKeyHash(OA_HASH_DCPT* pOH, const void* key);
#if defined(OA_DOUBLE_HASH_PROBING)
static size_t TCPIP_HTTP_SSI_HashProbeHash(OA_HASH_DCPT* pOH, const void* key);
#endif  // defined(OA_DOUBLE_HASH_PROBING)
static int TCPIP_HTTP_SSI_HashKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
static void TCPIP_HTTP_SSI_HashKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

#endif // (TCPIP_HTTP_SSI_PROCESS != 0)

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0)
static TCPIP_HTTP_CHUNK_RES _HTTP_AddDynChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt);
static TCPIP_HTTP_DYN_ARG_TYPE _HTTP_ArgType(char* argStr, int32_t* pIntArg);
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0)

// basic HTTP connection process function
// processes a HTTP connection state: TCPIP_HTTP_CONN_STATE
// returns the next connection state needed
// also signals if waiting for resources
// The function should only set *pWait if needed: *pWait is cleared by default;
typedef TCPIP_HTTP_CONN_STATE (*_HTTP_ConnProcessFunc)(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

// table with HTTP connection processing functions
static const _HTTP_ConnProcessFunc  _HTTP_ConnProcess_Tbl[] =
{
    _HTTP_ProcessIdle,                  // TCPIP_HTTP_CONN_STATE_IDLE
    _HTTP_ParseRequest,                 // TCPIP_HTTP_CONN_STATE_PARSE_REQUEST,    
    _HTTP_ParseFileUpload,              // TCPIP_HTTP_CONN_STATE_PARSE_FILE_UPLOAD,
    _HTTP_ParseFileOpen,                // TCPIP_HTTP_CONN_STATE_PARSE_FILE_OPEN,  
    _HTTP_ParseGetArgs,                 // TCPIP_HTTP_CONN_STATE_PARSE_GET_ARGS,   
    _HTTP_ParseHeaders,                 // TCPIP_HTTP_CONN_STATE_PARSE_HEADERS,    
    _HTTP_ProcessAuthenticate,          // TCPIP_HTTP_CONN_STATE_AUTHENTICATE,     
    _HTTP_ProcessGet,                   // TCPIP_HTTP_CONN_STATE_PROCESS_GET,      
    _HTTP_ProcessPost,                  // TCPIP_HTTP_CONN_STATE_PROCESS_POST,     
    _HTTP_ServeHeaders,                 // TCPIP_HTTP_CONN_STATE_SERVE_HEADERS,    
    _HTTP_ServeCookies,                 // TCPIP_HTTP_CONN_STATE_SERVE_COOKIES,    
    _HTTP_ServeBodyInit,                // TCPIP_HTTP_CONN_STATE_SERVE_BODY_INIT,  
    _HTTP_ServeBody,                    // TCPIP_HTTP_CONN_STATE_SERVE_BODY,       
    _HTTP_ServeChunks,                  // TCPIP_HTTP_CONN_STATE_SERVE_CHUNKS,     
    _HTTP_ProcessDone,                  // TCPIP_HTTP_CONN_STATE_DONE,             
    _HTTP_ProcessError,                 // TCPIP_HTTP_CONN_STATE_ERROR        
    _HTTP_ProcessDisconnect,            // TCPIP_HTTP_CONN_STATE_DISCONNECT        
};


#if (TCPIP_HTTP_SSI_PROCESS != 0)

static TCPIP_HTTP_CHUNK_RES _HTTP_SSIInclude(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, int leftAttribs);
static TCPIP_HTTP_CHUNK_RES _HTTP_SSISet(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, int leftAttribs);
static TCPIP_HTTP_CHUNK_RES _HTTP_SSIEcho(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, int leftAttribs);

static const TCPIP_HTTP_SSI_PROCESS_ENTRY  _HTTP_SSIProc_Tbl[] = 
{
    //  ssiCmd                              // ssiFnc
    {   "include",                          _HTTP_SSIInclude},      // SSI line: <!--#include virtual="file_name" -->
                                                                    // or        <!--#include file="file_name" -->
                                                                    // are supported
    {   "set",                              _HTTP_SSISet},          // SSI line: <!--#set var="varname" value="varvalue" -->
                                                                    // or        <!--#set var="varname" value="$varvalue" -->  

    {   "echo",                             _HTTP_SSIEcho},         // SSI line: <!--#echo var="varname" -->  
};


#endif // (TCPIP_HTTP_SSI_PROCESS != 0)



#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_BASIC) != 0)
volatile int _HTTPStayAssertLoop = 0;
static void _HTTPAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("HTTP Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_HTTPStayAssertLoop != 0);
    }
}
// a debug condition, not really assertion
volatile int _HTTPStayCondLoop = 0;
static void _HTTPDbgCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("HTTP Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_HTTPStayCondLoop != 0);
    }
}

#else
#define _HTTPAssertCond(cond, message, lineNo)
#define _HTTPDbgCond(cond, message, lineNo)
#endif  // (TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_BASIC)


#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_FILE) != 0)
static void _HTTP_FileDbgCreate(const char* fname, size_t fsize, const char* type, int connId)
{
    SYS_CONSOLE_PRINT("File Create: %s, size: %d, type: %s, conn: %d\r\n", fname, fsize, type, connId);
}

static void _HTTP_FileDbgProcess(const char* fname, size_t fsize, const char* type, int connId)
{
    SYS_CONSOLE_PRINT("File Processed: %s, size: %d, type: %s, conn: %d\r\n", fname, fsize, type, connId);
}
#else
#define _HTTP_FileDbgCreate(fname, fsize, type, connId)
#define _HTTP_FileDbgProcess(fname, fsize, type, connId)
#endif  // (TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_FILE)

#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_DYNVAR) != 0)
static void _HTTP_DynDbgExtract(const char* dynName, int nArgs, const char* fileName, int connId)
{
    SYS_CONSOLE_PRINT("DynVar Extract - name: %s, nArgs: %d, fName: %s, conn: %d\r\n", dynName, nArgs, fileName, connId);
}
static const char* _HTTP_DYNDBG_RES_TBL[] = 
{
    "done", "default", "proc_again", "again",
};

static void _HTTP_DynDbgCallback(const char* dynName, int res, int connId)
{
    SYS_CONSOLE_PRINT("DynVar Callback - name: %s, res: %s, conn: %d\r\n", dynName, _HTTP_DYNDBG_RES_TBL[res], connId);
}

static void _HTTP_DynDbgProcess(const char* dynName, size_t buffSize, bool acked, int connId)
{
    SYS_CONSOLE_PRINT("DynBuff Done - name: %s, size: %d, %s, conn: %d\r\n", dynName, buffSize, acked ? "ack" : "nak", connId);
}
#else
#define _HTTP_DynDbgExtract(dynName, nArgs, fileName, connId)
#define _HTTP_DynDbgCallback(dynName, res, connId)
#define _HTTP_DynDbgProcess(dynName, buffSize, acked, connId)
#endif  // (TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_DYNVAR)

#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_CONN_STATE) != 0)
static const char* const _HTTP_DbgConnState_Tbl[] = 
{
    "idle",                 // TCPIP_HTTP_CONN_STATE_IDLE,       
    "parse_req",            // TCPIP_HTTP_CONN_STATE_PARSE_REQUEST,   
    "parse_upl",            // TCPIP_HTTP_CONN_STATE_PARSE_FILE_UPLOAD
    "parse_fopen",          // TCPIP_HTTP_CONN_STATE_PARSE_FILE_OPEN, 
    "parse_args",           // TCPIP_HTTP_CONN_STATE_PARSE_GET_ARGS,  
    "parse_head",           // TCPIP_HTTP_CONN_STATE_PARSE_HEADERS,   
    "auth",                 // TCPIP_HTTP_CONN_STATE_AUTHENTICATE,    
    "get",                  // TCPIP_HTTP_CONN_STATE_PROCESS_GET,     
    "post",                 // TCPIP_HTTP_CONN_STATE_PROCESS_POST,    
    "srv_head",             // TCPIP_HTTP_CONN_STATE_SERVE_HEADERS,   
    "srv_cook",             // TCPIP_HTTP_CONN_STATE_SERVE_COOKIES,   
    "srv_bini",             // TCPIP_HTTP_CONN_STATE_SERVE_BODY_INIT, 
    "srv_body",             // TCPIP_HTTP_CONN_STATE_SERVE_BODY,      
    "srv_chunks",           // TCPIP_HTTP_CONN_STATE_SERVE_CHUNKS,    
    "done",                 // TCPIP_HTTP_CONN_STATE_DONE,            
    "error",                // TCPIP_HTTP_CONN_STATE_ERROR       
    "discon",               // TCPIP_HTTP_CONN_STATE_DISCONNECT       
};

static const char* const _HTTP_DbgHttpState_Tbl[] = 
{
    "get",              // TCPIP_HTTP_STAT_GET,             
    "post",             // TCPIP_HTTP_STAT_POST,                 
    "400",              // TCPIP_HTTP_STAT_BAD_REQUEST,          
    "401",              // TCPIP_HTTP_STAT_UNAUTHORIZED,         
    "404",              // TCPIP_HTTP_STAT_NOT_FOUND,            
    "414",              // TCPIP_HTTP_STAT_OVERFLOW,             
    "500",              // TCPIP_HTTP_STAT_INTERNAL_SERVER_ERROR,
    "501",              // TCPIP_HTTP_STAT_NOT_IMPLEMENTED,      
    "redir",            // TCPIP_HTTP_STAT_REDIRECT,             
    "403",              // TCPIP_HTTP_STAT_TLS_REQUIRED,         
    "upl",              // TCPIP_HTTP_STAT_UPLOAD_FORM,                                            
    "upl_start",        // TCPIP_HTTP_STAT_UPLOAD_STARTED,      
    "upl_write",        // TCPIP_HTTP_STAT_UPLOAD_WRITE,      
    "upl_wait",         // TCPIP_HTTP_STAT_UPLOAD_WRITE_WAIT,      
    "upl_ok",           // TCPIP_HTTP_STAT_UPLOAD_OK,           
    "upl_err",          // TCPIP_HTTP_STAT_UPLOAD_ERROR,        
                                  
};


static TCPIP_HTTP_CONN_STATE httpConnState;
static void _HTTP_DbgGetState(TCPIP_HTTP_CONN* pHttpCon)
{
    httpConnState = pHttpCon->connState;
}

static void _HTTP_DbgNewState(TCPIP_HTTP_CONN* pHttpCon)
{
    if(pHttpCon->connState != httpConnState)
    {
        httpConnState = pHttpCon->connState;
        SYS_CONSOLE_PRINT("HTTP Conn: %d, inst: %d, state: %d-%s, http: %s\r\n", pHttpCon->connIx, pHttpCon->connInstance->instIx, httpConnState, _HTTP_DbgConnState_Tbl[httpConnState], _HTTP_DbgHttpState_Tbl[pHttpCon->httpStatus]);
    }
}

#else
#define _HTTP_DbgGetState(pHttpCon)
#define _HTTP_DbgNewState(pHttpCon)
#endif  // (TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_CONN_STATE)


#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_DYN_CONTROL) != 0)
static volatile int httpKillDynFiles = 0;
static volatile int httpKillDynVarParser = 0;
static volatile int httpKillUserDynVar = 0;
static volatile int httpKillFlashWrite = 0;

static __inline__ bool __attribute__((always_inline)) _HTTP_DbgKillDynFiles(void)
{
    return httpKillDynFiles != 0;
} 
static __inline__ bool __attribute__((always_inline)) _HTTP_DbgKillDynParser(void)
{
    return httpKillDynVarParser != 0;
} 
static __inline__ bool __attribute__((always_inline)) _HTTP_DbgKillUserDynVar(void)
{
    return httpKillUserDynVar != 0;
} 
static __inline__ bool __attribute__((always_inline)) _HTTP_DbgKillFlashWrite(void)
{
    return httpKillFlashWrite != 0;
} 
#else
#define _HTTP_DbgKillDynFiles()   (false)
#define _HTTP_DbgKillDynParser()  (false)
#define _HTTP_DbgKillUserDynVar() (false)
#define _HTTP_DbgKillFlashWrite() (false)
#endif  // ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_DYN_CONTROL) != 0)

#if (TCPIP_HTTP_SSI_PROCESS != 0) && ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_SSI_HASH) != 0)
static void _HTTP_DbgSSIHashEntry(TCPIP_HTTP_SSI_HASH_ENTRY* pHE, bool isRef)
{
    if(pHE->varType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        SYS_CONSOLE_PRINT("HTTP Set %s int: %s, value: %s, int: %d\r\n", isRef ? "ref" : "direct", pHE->varName, pHE->varStr, pHE->valInt); 
    }
    else
    {
        SYS_CONSOLE_PRINT("HTTP Set %s str: %s, value: %s\r\n", isRef ? "ref" : "direct", pHE->varName, pHE->varStr);
    }
}
#else
#define _HTTP_DbgSSIHashEntry(pHE, isRef)
#endif  // (TCPIP_HTTP_SSI_PROCESS != 0) && ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_SSI_HASH) != 0)

#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_CONN_TMO) != 0)
static void _HTTP_DbgConnTmo(TCPIP_HTTP_CONN* pHttpCon)
{
    SYS_CONSOLE_PRINT("HTTP tmo - conn: %d, active sec: %d, curr sec: %d\r\n", pHttpCon->connIx, pHttpCon->connActiveSec, _TCPIP_SecCountGet());
}
#else
#define _HTTP_DbgConnTmo(pHttpCon)
#endif  // ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_CONN_TMO) != 0)


#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
static TCPIP_HTTP_IO_RESULT TCPIP_HTTP_NET_FSUpload(TCPIP_HTTP_CONN* pHttpCon);
#endif

#define mMIN(a, b)  ((a<b)?a:b)

#if (TCPIP_STACK_DOWN_OPERATION != 0)
// if pNetIf == 0, all connections are closed, stack is going down
static void _HTTP_CloseConnections(TCPIP_HTTP_PORT_DCPT* pPort, TCPIP_NET_IF* pNetIf)
{
    TCPIP_HTTP_CONN* pHttpCon;
    int  connIx;
    TCP_SOCKET_INFO sktInfo;
    bool closeSkt;

    pHttpCon = pPort->connCtrl + 0;
    for (connIx = 0; connIx < pPort->connNo; connIx++, pHttpCon++)
    {
        // Close the connections that were associated with this interface
        if (pHttpCon->socket != NET_PRES_INVALID_SOCKET)
        {
            if(pNetIf == 0)
            {
                closeSkt = true;
            }
            else
            {
                NET_PRES_SocketInfoGet(pHttpCon->socket, &sktInfo);
                closeSkt = pNetIf == sktInfo.hNet;
            }

            if(closeSkt)
            {
                _HTTP_ConnCloseFile(pHttpCon);

                if(pNetIf == 0)
                {   // stack going down
                    if(pHttpCon->socketSignal != 0)
                    {
                        NET_PRES_SocketSignalHandlerDeregister(pHttpCon->socket, pHttpCon->socketSignal);
                    }
                    NET_PRES_SocketClose(pHttpCon->socket);
                    pHttpCon->socket = NET_PRES_INVALID_SOCKET;
                }
                else 
                {   // TCPIP_STACK_ACTION_IF_DOWN - interface down
                    NET_PRES_SocketDisconnect(pHttpCon->socket);
                }
            }
        }
    }
}

static void _HTTP_Cleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    int instIx, portIx;
    TCPIP_HTTP_INST* pInstance;
    TCPIP_HTTP_PORT_DCPT* pPort;
    TCPIP_HTTP_FILE_BUFF_DCPT*  fileBuffDcpt;

    _HTTPAssertCond(httpMemH == stackCtrl->memH, __func__, __LINE__);

    for(instIx = 0; instIx < httpInstNo; instIx++)
    {
        pInstance = httpInstances[instIx];
        // deallocate ports
        pPort = pInstance->portDcpt;
        for(portIx = 0; portIx < pInstance->nPorts; portIx++, pPort++)
        {
            if(pPort->connData && pPort->connCtrl)
            {
                _HTTP_CloseConnections(pPort, 0);
            }


            if(pPort->connData)
            {
                TCPIP_HEAP_Free(httpMemH, pPort->connData);
            }
            if(pPort->connCtrl)
            {
                TCPIP_HEAP_Free(httpMemH, pPort->connCtrl);
            }
            if(pPort->fileShell != 0)
            {
                (*pPort->fileShell->delete)(pPort->fileShell);
            }
        }
        TCPIP_HEAP_Free(httpMemH, pInstance->portDcpt);

        // deallocate the dynamic variables
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
        if(pInstance->allocDynDcpt)
        {
            TCPIP_HEAP_Free(httpMemH, pInstance->allocDynDcpt);
        }
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

        if(pInstance->allocChunks)
        {
            TCPIP_HEAP_Free(httpMemH, pInstance->allocChunks);
        }

        // deallocate the file buffers
        while((fileBuffDcpt = (TCPIP_HTTP_FILE_BUFF_DCPT*)TCPIP_Helper_SingleListHeadRemove(&pInstance->fileBuffers)) != 0)
        {
            TCPIP_HEAP_Free(httpMemH, fileBuffDcpt);
        }

        // deallocate SSI
#if (TCPIP_HTTP_SSI_PROCESS != 0)
        if(pInstance->ssiHashDcpt)
        {
            TCPIP_OAHASH_EntriesRemoveAll(pInstance->ssiHashDcpt);
            http_freeFnc(pInstance->ssiHashDcpt);
        }
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)

        if(pInstance->rules)
        {
            TCPIP_HEAP_Free(httpMemH, pInstance->rules);
        }

        // finally delete the instance itself
        TCPIP_HEAP_Free(httpMemH, pInstance);
    }


    // delete httpInstances!
    TCPIP_HEAP_Free(httpMemH, httpInstances);
    httpInstances = 0;
    httpInstNo = 0;
    httpMemH = 0;

    if(httpSignalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(httpSignalHandle);
        httpSignalHandle = 0;
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void _HTTP_ConnCloseFile(TCPIP_HTTP_CONN* pHttpCon)
{
    if(pHttpCon->file != SYS_FS_HANDLE_INVALID)
    {
        const SYS_FS_SHELL_OBJ* fShell = pHttpCon->connPort->fileShell;
        fShell->fileClose(fShell, pHttpCon->file);
        pHttpCon->file = SYS_FS_HANDLE_INVALID;
        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_CLOSE, pHttpCon->fileName);
    }
}

bool TCPIP_HTTP_Server_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl,
              const TCPIP_HTTP_SERVER_MODULE_CONFIG* httpInitData)
{
    bool        initFail;
    size_t      instIx;
    const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg;
    TCPIP_HTTP_INST* pInstance;
    TCPIP_HTTP_INST_RES instRes;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init
    initFail = false;
    while(httpInitCount == 0)
    {   // first time we're run

        if(!_HTTP_VerifyConfig(httpInitData))
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, " HTTP: Configuration invalid data: %d\r\n", TCPIP_HTTP_INST_RES_BAD_DATA);
            return false;
        }

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
        // set the allocation functions
        http_mallocFnc = httpInitData->httpMallocFnc;
        http_freeFnc = httpInitData->httpFreeFnc;
        if(http_mallocFnc == 0 || http_freeFnc == 0)
        {   // use the default stack values
            http_mallocFnc = _HTTP_Default_MallocFnc;
            http_freeFnc = _HTTP_Default_FreeFnc;
        }
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)

        // allocate instances array
        httpMemH = stackCtrl->memH;
        httpInstances = (TCPIP_HTTP_INST**)TCPIP_HEAP_Calloc(httpMemH, httpInitData->nInstances, sizeof(TCPIP_HTTP_INST*));
        if(httpInstances == 0)
        {
            SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Instance array allocation failed\r\n");
            return false;
        }

        // allocate instances themselves
        pInstCfg = httpInitData->pInstConfig;
        for(instIx = 0; instIx < httpInitData->nInstances; instIx++, pInstCfg++)
        {
            pInstance = (TCPIP_HTTP_INST*)TCPIP_HEAP_Calloc(httpMemH, 1, sizeof(*pInstance));
            httpInstances[instIx] = pInstance;
            if(pInstance == 0)
            {   // failed
                break;
            }
        }

        if(instIx != httpInitData->nInstances)
        {   // allocation failed
            SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Instance allocation failed\r\n");
            // clean up
            size_t cleanIx;
            for(cleanIx = 0; cleanIx < instIx; cleanIx++)
            {
                pInstance = httpInstances[cleanIx];
                TCPIP_HEAP_Free(httpMemH, pInstance);
            }

            TCPIP_HEAP_Free(httpMemH, httpInstances);
            return false;
        }

        // instances properly allocated
        httpInstNo = httpInitData->nInstances;

        // start populating the instances
        for(instIx = 0, pInstCfg = httpInitData->pInstConfig; instIx < httpInstNo; instIx++, pInstCfg++)
        {
            pInstance = httpInstances[instIx];
            if((instRes = _HTTP_InitInstance(pInstance, pInstCfg)) != TCPIP_HTTP_INST_RES_OK)
            {   // failed
                SYS_ERROR_PRINT(SYS_ERROR_ERROR, " HTTP: Instance creation %d failed: %d\r\n", instIx, instRes);
                initFail = true;
                break;
            }
            pInstance->instIx = instIx;
        }
            
        if(initFail == false)
        {   // create the HTTP timer
            httpSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_HTTP_Server_Task, TCPIP_HTTP_TASK_RATE);
            if(httpSignalHandle == 0)
            {   // cannot create the HTTP timer
                initFail = true;
            }
        }

        // done
        break;
    }

    if(initFail)
    {
        _HTTP_Cleanup(stackCtrl);
        return false;
    }

    httpInitCount++;

    _HTTPDbgCond(true, __func__, __LINE__); // nop; remove not used warning
    return true;    
}

// verify config instances to be OK
static bool _HTTP_VerifyConfig(const TCPIP_HTTP_SERVER_MODULE_CONFIG* httpInitData)
{
    if(httpInitData == 0 || httpInitData->nInstances == 0 || httpInitData->pInstConfig == 0)
    {
        return false;
    }

    size_t instIx;
    const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg = httpInitData->pInstConfig;
    for(instIx = 0; instIx < httpInitData->nInstances; instIx++, pInstCfg++)
    {
        // sanity check
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
        if(pInstCfg->nDescriptors == 0 || pInstCfg->nChunks == 0 || pInstCfg->nFileBuffers == 0 || pInstCfg->fileBufferSize == 0)
#else
            if(pInstCfg->nChunks == 0 || pInstCfg->nFileBuffers == 0 || pInstCfg->fileBufferSize == 0)
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0)
            {
                return false;
            }
        // check ports
        if(pInstCfg->portConfig == 0 || pInstCfg->nPorts == 0)
        {
            return false;
        }
        size_t portIx;
        const TCPIP_HTTP_PORT_CONFIG* pPort = pInstCfg->portConfig;

        for(portIx = 0; portIx < pInstCfg->nPorts; portIx++, pPort++)
        { 
            if(pPort->nConnections == 0 || pPort->webDir == 0 || pPort->dataLen == 0 || (size_t)pPort->addType > IP_ADDRESS_TYPE_IPV6)
            {
                return false;
            }
        }

        // check the port rules
#if (_TCPIP_HTTP_USE_ACCESS_RULES != 0)
        size_t ruleIx;
        if(pInstCfg->nRules != 0)
        {   // have access rules
            if(pInstCfg->accRules == 0)
            {   // missing rules
                return false;
            }
            const TCPIP_HTTP_ACCESS_RULE* pAccRule = pInstCfg->accRules; 
            for(ruleIx = 0; ruleIx < pInstCfg->nRules; ruleIx++)
            {
                if(pAccRule->action != TCPIP_HTTP_ACCESS_ACTION_NONE)
                {
                    if(pAccRule->dir == 0 || strlen(pAccRule->dir) == 0 || pAccRule->dir[0] != '/')
                    {
                        return false;
                    }

                    if(pAccRule->action == TCPIP_HTTP_ACCESS_ACTION_REDIRECT)
                    {
                        if(pAccRule->redirServer == 0 || strlen(pAccRule->redirServer) == 0)
                        {
                            return false;
                        }
                        if(pAccRule->redirURI == 0 || strlen(pAccRule->redirURI) == 0 || pAccRule->redirURI[0] != '/')
                        {
                            return false;
                        }
                    }
                }
            }
        }
#endif  //  // (_TCPIP_HTTP_USE_ACCESS_RULES != 0)
    }

    // all checks passed
    return true;
}


static TCPIP_HTTP_INST_RES _HTTP_InitInstance(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg)
{
    size_t      connIx, buffIx;
    uint16_t    fileBufferTotSize;
    TCPIP_HTTP_FILE_BUFF_DCPT*  fileBuffDcpt;
    TCPIP_HTTP_PORT_DCPT* pPort;
    const void* memH = httpMemH;

    pInstance->chunksDepth = pInstCfg->maxRecurseLevel;
    pInstance->chunksNo = pInstCfg->nChunks;
    pInstance->chunkPoolRetries = pInstCfg->chunkPoolRetries;
    pInstance->fileBufferRetries = pInstCfg->fileBufferRetries;
    TCPIP_Helper_SingleListInitialize (&pInstance->fileBuffers);
    pInstance->nPorts = pInstCfg->nPorts;

    pInstance->allocChunks =  (TCPIP_HTTP_CHUNK_DCPT*)TCPIP_HEAP_Calloc(memH, pInstance->chunksNo, sizeof(TCPIP_HTTP_CHUNK_DCPT)); 

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    pInstance->dynVarRetries = pInstCfg->dynVarRetries;
    // allocate dynamic variable descriptors
    pInstance->dynDescriptorsNo = pInstCfg->nDescriptors;
    pInstance->allocDynDcpt =  (TCPIP_HTTP_DYNVAR_BUFF_DCPT*)TCPIP_HEAP_Calloc(memH, pInstance->dynDescriptorsNo, sizeof(TCPIP_HTTP_DYNVAR_BUFF_DCPT)); 
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

    // try to allocate the file buffers
    fileBufferTotSize = TCPIP_HTTP_CHUNK_HEADER_LEN + pInstCfg->fileBufferSize + TCPIP_HTTP_CHUNK_FINAL_TRAILER_LEN + 1; 
    for(buffIx = 0; buffIx < pInstCfg->nFileBuffers; buffIx ++)
    {
        fileBuffDcpt = (TCPIP_HTTP_FILE_BUFF_DCPT*)TCPIP_HEAP_Malloc(memH, sizeof(TCPIP_HTTP_FILE_BUFF_DCPT) + fileBufferTotSize);
        if(fileBuffDcpt)
        {
            fileBuffDcpt->fileBufferSize = pInstCfg->fileBufferSize;
            fileBuffDcpt->fileBufferTotSize = fileBufferTotSize;
            TCPIP_Helper_SingleListTailAdd(&pInstance->fileBuffers, (SGL_LIST_NODE*)fileBuffDcpt);
        }
        else
        {   // failed
            break;
        }
    } 

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    if(pInstance->allocDynDcpt == 0 || pInstance->allocChunks == 0 || buffIx != pInstCfg->nFileBuffers)
#else
    if(pInstance->allocChunks == 0 || buffIx != pInstCfg->nFileBuffers)
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    {   // failed
        SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Dynamic chunks and file buffers allocation failed\r\n");
        return TCPIP_HTTP_INST_RES_ALLOC_ERR;
    }

    pInstance->portDcpt =  (TCPIP_HTTP_PORT_DCPT*)TCPIP_HEAP_Calloc(memH, pInstance->nPorts, sizeof(*pPort));
    if(pInstance->portDcpt == 0)
    {   // allocation failed
        SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Port descriptor allocation failed\r\n");
        return TCPIP_HTTP_INST_RES_ALLOC_ERR;
    }

    TCPIP_HTTP_INST_RES portRes = _HTTP_InitInstance_Ports(pInstance, pInstCfg);
    if(portRes != TCPIP_HTTP_INST_RES_OK) 
    {   
        return portRes;
    }

    // create access rules
#if (_TCPIP_HTTP_USE_ACCESS_RULES != 0)
    TCPIP_HTTP_INST_RES accRes = _HTTP_CreateAccessRules(pInstance, pInstCfg);
    if(accRes != TCPIP_HTTP_INST_RES_OK)
    {
        return accRes;
    }
#endif  //  // (_TCPIP_HTTP_USE_ACCESS_RULES != 0)

    // continue initialization 
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    // initialize the dynamic variables buffer pool
    TCPIP_Helper_SingleListInitialize (&pInstance->dynVarPool);
    TCPIP_HTTP_DYNVAR_BUFF_DCPT* pDynDcpt = pInstance->allocDynDcpt; 
    for(connIx = 0; connIx < pInstance->dynDescriptorsNo; connIx++, pDynDcpt++)
    {
        TCPIP_Helper_SingleListTailAdd(&pInstance->dynVarPool, (SGL_LIST_NODE*)pDynDcpt);
    } 
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

    // initialize the chunk pool
    TCPIP_Helper_SingleListInitialize (&pInstance->chunkPool);
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt = pInstance->allocChunks; 
    for(connIx = 0; connIx < pInstance->chunksNo; connIx++, pChDcpt++)
    {
        TCPIP_Helper_SingleListTailAdd(&pInstance->chunkPool, (SGL_LIST_NODE*)pChDcpt);
    } 

    // postpone the SSI variables initialization

    // get the peek buffer size
#if (TCPIP_HTTP_FIND_PEEK_BUFF_SIZE != 0)
    pInstance->peekBufferSize = TCPIP_HTTP_FIND_PEEK_BUFF_SIZE;
#else
    // when multiple connections and each has its own sktRxBuffSize
    // use the socket RX buffer size
    TCP_SOCKET_INFO tcpInfo;
    size_t portIx;
    uint16_t maxRxSize = 0;

    pPort = pInstance->portDcpt;
    for(portIx = 0; portIx < pInstance->nPorts; portIx++, pPort++)
    {
        TCPIP_HTTP_CONN* pHttpCon = pPort->connCtrl + 0;   // all sockets in a connection are the same; using just port 0
        NET_PRES_SocketInfoGet(pHttpCon->socket, &tcpInfo);
        if(tcpInfo.rxSize > maxRxSize)
        {
            maxRxSize = tcpInfo.rxSize;
        }
    }
    pInstance->peekBufferSize = maxRxSize;
#endif  // (TCPIP_HTTP_FIND_PEEK_BUFF_SIZE != 0)

    return TCPIP_HTTP_INST_RES_OK;
}

static TCPIP_HTTP_INST_RES _HTTP_CreatePortConnections(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_PORT_CONFIG* pCfgPort, TCPIP_HTTP_PORT_DCPT* pPort)
{
    size_t nConns, connIx;
    uint16_t    portFlags; // run time flags
    TCPIP_HTTP_CONN*  pHttpCon;
    uint8_t*    pHttpData;
    TCPIP_NET_HANDLE hNet;
    NET_PRES_SKT_T  sktType;

    const void* memH = httpMemH;
    nConns = pPort->connNo;
    pPort->connCtrl = (TCPIP_HTTP_CONN*)TCPIP_HEAP_Calloc(memH, nConns, sizeof(*pPort->connCtrl));
    pPort->connData = (uint8_t*)TCPIP_HEAP_Malloc(memH, nConns *  pPort->connDataSize);
    if(pPort->connCtrl == 0 || pPort->connData == 0)
    {
        SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Connection control/data allocation failed\r\n");
        return TCPIP_HTTP_INST_RES_ALLOC_ERR;
    }

    // open associated sockets
    if(pCfgPort->listenIfIx < 0)
    {
        hNet = 0;
    }
    else
    {
        hNet = TCPIP_STACK_IndexToNet(pCfgPort->listenIfIx);
        if(hNet == 0)
        {
            return TCPIP_HTTP_INST_RES_BAD_IF; 
        }
    }

    sktType = NET_PRES_SKT_STREAM | NET_PRES_SKT_SERVER;
    portFlags = pPort->portFlags;
    if((portFlags & TCPIP_HTTP_PORT_FLAG_SECURE_ON) != 0)
    {   // encrypted
        sktType |= NET_PRES_SKT_ENCRYPTED;
    }
    else if((portFlags & TCPIP_HTTP_PORT_FLAG_SECURE_OFF) != 0)
    {   // unencrypted
        sktType = NET_PRES_SKT_UNENCRYPTED;
    }
    else
    {
        // use default port number
    }

    // initialize all connections to a known state
    pHttpCon = pPort->connCtrl + 0;
    for(connIx = 0; connIx < nConns; connIx++, pHttpCon++)
    {
        pHttpCon->socket =  NET_PRES_INVALID_SOCKET;
        pHttpCon->file = SYS_FS_HANDLE_INVALID;
        pHttpCon->connIx = (uint16_t)connIx;
        pHttpCon->connInstance = pInstance;
        pHttpCon->connPort = pPort;

        pHttpCon->connState = TCPIP_HTTP_CONN_STATE_IDLE;
    }

    // set sockets and data
    pHttpCon = pPort->connCtrl + 0;
    pHttpData = pPort->connData;
    for(connIx = 0; connIx < nConns; connIx++, pHttpCon++)
    {
        pHttpCon->socket =  NET_PRES_SocketOpen(0, sktType, pCfgPort->addType, pPort->listenPort, 0, 0);
        if(pHttpCon->socket == NET_PRES_INVALID_SOCKET)
        {   // failed to open the socket
            SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Socket creation failed\r\n");
            return TCPIP_HTTP_INST_RES_SKT_OPEN_ERR; 
        }

        // set socket options
        if((portFlags & TCPIP_HTTP_PORT_FLAG_NO_DELAY) != 0)
        {
            void* tcpForceFlush = (void*)1;
            NET_PRES_SocketOptionsSet(pHttpCon->socket, TCP_OPTION_NODELAY, tcpForceFlush);
        }
        if(pCfgPort->sktTxBuffSize != 0)
        {
            void* tcpBuffSize = (void*)(unsigned int)pCfgPort->sktTxBuffSize;
            if(!NET_PRES_SocketOptionsSet(pHttpCon->socket, TCP_OPTION_TX_BUFF, tcpBuffSize))
            {
                SYS_ERROR(SYS_ERROR_WARNING, " HTTP: Setting TX Buffer failed - port: %d, conn: %d\r\n", pPort->portIx, connIx);
                return TCPIP_HTTP_INST_RES_SKT_TX_BUFF_ERR; 
            }
        }
        if(pCfgPort->sktRxBuffSize != 0)
        {
            void* tcpBuffSize = (void*)(unsigned int)pCfgPort->sktRxBuffSize;
            if(!NET_PRES_SocketOptionsSet(pHttpCon->socket, TCP_OPTION_RX_BUFF, tcpBuffSize))
            {
                SYS_ERROR(SYS_ERROR_WARNING, " HTTP: Setting RX Buffer failed - port: %d, conn: %d\r\n", pPort->portIx, connIx);
                return TCPIP_HTTP_INST_RES_SKT_RX_BUFF_ERR; 
            }
        }

        if(hNet != 0)
        {
            TCP_SOCKET nativeSkt = NET_PRES_SocketGetTransportHandle(pHttpCon->socket);
            if(!TCPIP_TCP_SocketNetSet(nativeSkt, hNet, true))
            {
                return TCPIP_HTTP_INST_RES_SKT_NET_ERR; 
            }
        }

        pHttpCon->flags.sktLocalReset = 1;      // socket will start reset

        pHttpCon->socketSignal = NET_PRES_SocketSignalHandlerRegister(pHttpCon->socket, TCPIP_TCP_SIGNAL_RX_DATA, _HTTPSocketRxSignalHandler, 0);
        if(pHttpCon->socketSignal == 0)
        {
            SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Signal creation failed\r\n");
            return TCPIP_HTTP_INST_RES_SKT_SIGNAL_ERR; 
        }

        pHttpCon->httpData = pHttpData;
        pHttpData += pCfgPort->dataLen;
    }

    return TCPIP_HTTP_INST_RES_OK; 
}

#if (_TCPIP_HTTP_USE_ACCESS_RULES != 0)
// checks if a rule applies to the selected instance
static bool _HTTP_InstanceOwnsRule(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_ACCESS_RULE* pRule)
{
    if(pRule->inPort == 0)
    {   // this rule belongs to all
        return true;
    }

    // browse the ports
    size_t portIx;
    TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt;
    for(portIx = 0; portIx < pInstance->nPorts; portIx++, pPort++)
    {
        if(pRule->inPort == pPort->listenPort)
        {
            return true;
        }
    }

    return false;
}

static TCPIP_HTTP_INST_RES _HTTP_CreateAccessRules(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg)
{
    // calculate the sizes and allocate, populate the rules for this instance
    size_t ruleIx, ruleSz, totRulesSz;
    size_t portIx;
    size_t dirSz, redirSz, srvSz;
    size_t defRules, cfgRules;
    _TCPIP_HTTP_ACC_RULE* instRule;
    const TCPIP_HTTP_ACCESS_RULE* pCfgRule;
    const TCPIP_HTTP_PORT_CONFIG* pPortCfg;

    const void* memH = httpMemH;

    // check if we have default rules
    defRules = cfgRules = 0;
    pPortCfg = pInstCfg->portConfig;
    for(portIx = 0; portIx < pInstCfg->nPorts; portIx++, pPortCfg++)
    {
        if((pPortCfg->portFlags & TCPIP_HTTP_PORT_FLAG_NO_DEFAULT_RULE) == 0)
        {
            defRules++;
        }
    }

    while((pInstCfg->nRules + defRules) != 0)
    {   // calculate the total rules size

        totRulesSz = 0;
        
        if(pInstCfg->nRules != 0)
        {
            pCfgRule = pInstCfg->accRules;
            for(ruleIx = 0; ruleIx < pInstCfg->nRules; ruleIx++, pCfgRule++)
            {
                if(pCfgRule->action ==  TCPIP_HTTP_ACCESS_ACTION_NONE)
                {
                    continue;
                }

                if(_HTTP_InstanceOwnsRule(pInstance, pCfgRule))
                {
                    cfgRules++;
                    ruleSz = sizeof(*instRule) + strlen(pCfgRule->dir) + 1;
                    if(pCfgRule->action == TCPIP_HTTP_ACCESS_ACTION_REDIRECT)
                    {
                        ruleSz += strlen(pCfgRule->redirServer) + 1;    // space for '\0'
                        ruleSz += strlen(pCfgRule->redirURI) + 1;    // space for '\0'
                    }
                    ruleSz = ((ruleSz + sizeof(uint32_t) - 1) / sizeof(uint32_t)) * sizeof(uint32_t);   // make it M4
                    totRulesSz += ruleSz;
                }
            }
        }

        if(defRules != 0)
        {   // add the default rules with action == TCPIP_HTTP_ACCESS_ACTION_ALLOW: one per port
            pPortCfg = pInstCfg->portConfig;
            for(portIx = 0; portIx < pInstCfg->nPorts; portIx++, pPortCfg++)
            {
                if((pPortCfg->portFlags & TCPIP_HTTP_PORT_FLAG_NO_DEFAULT_RULE) == 0)
                {
                    ruleSz = sizeof(*instRule) + strlen("/") + 1;
                    ruleSz = ((ruleSz + sizeof(uint32_t) - 1) / sizeof(uint32_t)) * sizeof(uint32_t);   // make it M4
                    totRulesSz += ruleSz;
                }
            }
        }

        // allocate the rule
        if(totRulesSz == 0)
        {   // only no action rules and no default rules!
            break;
        }

        pInstance->rules = (_TCPIP_HTTP_ACC_RULE*)TCPIP_HEAP_Malloc(memH, totRulesSz);
        if(pInstance->rules == 0)
        {
            SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Instance rules allocation failed\r\n");
            return TCPIP_HTTP_INST_RES_ALLOC_ERR; 
        }

        // success; populate the rules
        pCfgRule = pInstCfg->accRules;
        instRule = pInstance->rules;
        for(ruleIx = 0; ruleIx < pInstCfg->nRules; ruleIx++, pCfgRule++)
        {
            if(pCfgRule->action ==  TCPIP_HTTP_ACCESS_ACTION_NONE)
            {
                continue;
            }

            if(_HTTP_InstanceOwnsRule(pInstance, pCfgRule))
            {
                dirSz = strlen(pCfgRule->dir) + 1;
                if(pCfgRule->action ==  TCPIP_HTTP_ACCESS_ACTION_REDIRECT)
                {
                    srvSz = strlen(pCfgRule->redirServer);
                    if(pCfgRule->redirServer[srvSz - 1] == '/')
                    {   // remove the trailing '/'
                        srvSz -= 1;
                    }
                    srvSz += 1;  // space for '\0'
                    redirSz = srvSz + strlen(pCfgRule->redirURI) + 1;     // space for '\0'
                }
                else
                {
                    redirSz = 0;
                }
                ruleSz = sizeof(*instRule) + dirSz + redirSz;
                ruleSz = ((ruleSz + sizeof(uint32_t) - 1) / sizeof(uint32_t)) * sizeof(uint32_t);   // make it M4
                instRule->accRule = *pCfgRule;
                instRule->accRule.ruleSize = ruleSz;
                char* dirStr = instRule->strSpace;
                strcpy(dirStr, pCfgRule->dir);
                instRule->accRule.dir = dirStr;
                if(redirSz != 0)
                {
                    char* redirStr = dirStr + dirSz;
                    strncpy(redirStr, pCfgRule->redirServer, srvSz);
                    redirStr[srvSz - 1] = '\0'; // end properly
                    instRule->accRule.redirServer = redirStr;
                    redirStr += srvSz;
                    strcpy(redirStr, pCfgRule->redirURI);
                    instRule->accRule.redirURI = redirStr;
                }

                // done; next rule
                instRule = (_TCPIP_HTTP_ACC_RULE*)((uint8_t*)instRule + ruleSz);
            }
        }

        // add the default rules with action == TCPIP_HTTP_ACCESS_ACTION_ALLOW: one per port
        pPortCfg = pInstCfg->portConfig;
        for(portIx = 0; portIx < pInstCfg->nPorts; portIx++, pPortCfg++)
        {
            if((pPortCfg->portFlags & TCPIP_HTTP_PORT_FLAG_NO_DEFAULT_RULE) == 0)
            {
                ruleSz = sizeof(*instRule) + strlen("/") + 1;
                ruleSz = ((ruleSz + sizeof(uint32_t) - 1) / sizeof(uint32_t)) * sizeof(uint32_t);   // make it M4
                                                                                                    // populate the default rule
                instRule->accRule.inPort = pPortCfg->listenPort;
                instRule->accRule.inIfIx = pPortCfg->listenIfIx;
                instRule->accRule.inAddType = pPortCfg->addType;
                instRule->accRule.action = TCPIP_HTTP_ACCESS_ACTION_ALLOW;
                instRule->accRule.ruleSize = ruleSz;
                char* dirStr = instRule->strSpace;
                strcpy(dirStr, "/");
                instRule->accRule.dir = dirStr;

                // done; next rule
                instRule = (_TCPIP_HTTP_ACC_RULE*)((uint8_t*)instRule + ruleSz);
            }
        }

        pInstance->nRules = cfgRules + defRules;

        return TCPIP_HTTP_INST_RES_OK;
    }

    pInstance->nRules = 0;
    pInstance->rules = 0;

    return TCPIP_HTTP_INST_RES_OK;
}

static void _HTTP_GetPageAccess(TCPIP_HTTP_CONN* pHttpCon, char* pageName, TCPIP_HTTP_ACCESS_RULE* accRule)
{
    size_t ruleIx;
    char pagePath[SYS_FS_FILE_NAME_LEN + 1];
    char rulePath[SYS_FS_FILE_NAME_LEN + 1];
                                //
    TCPIP_HTTP_INST* pInst = pHttpCon->connInstance;
    if(pInst->nRules == 0)
    {   // if there are no rules, simply grant access!
        accRule->action = TCPIP_HTTP_ACCESS_ACTION_ALLOW;
        return;
    }

    // when rules are present the default action is block
    accRule->action = TCPIP_HTTP_ACCESS_ACTION_BLOCK;
    TCPIP_HTTP_PORT_DCPT* pPort = pHttpCon->connPort;

    SYS_FS_SHELL_RES fsRes = SYS_FS_Shell_GetPath(pPort->fileShell, pageName, pagePath, sizeof(pagePath) / sizeof(*pagePath));
    if(fsRes != SYS_FS_SHELL_RES_OK)
    {   // no such page exist?
        return;
    }

    // get the interface this access came from
    TCP_SOCKET_INFO sktInfo;
    NET_PRES_SocketInfoGet(pHttpCon->socket, &sktInfo);

    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(sktInfo.hNet);
    int16_t inIx = _TCPIPStackNetIxGet(pNetIf);

    // get the address type
    IP_ADDRESS_TYPE  inAddType = sktInfo.addressType;
    
    
    // evaluate the rules, including the default ones
    TCPIP_HTTP_ACCESS_RULE* pRule = (TCPIP_HTTP_ACCESS_RULE*)pInst->rules; 
    for(ruleIx = 0; ruleIx < pInst->nRules; ruleIx++)
    {
        // evaluate inPort, inIfIx, inAddType
        if(pRule->inPort == 0 || pRule->inPort == pPort->listenPort)
        {   // port match
            if(pRule->inIfIx < 0 || pRule->inIfIx == inIx)
            {   // interface match
                if(pRule->inAddType == 0 || pRule->inAddType == inAddType)
                {   // add type match
                    // rule applies to this page
                    fsRes = SYS_FS_Shell_GetPath(pPort->fileShell, pRule->dir, rulePath, sizeof(rulePath) / sizeof(*rulePath));
                    if(fsRes == SYS_FS_SHELL_RES_OK)
                    {   // rule in the page path; consider rule
                        // check if the rulePath and the pagePath intersect: rulePath is part of the pagePath
                        char* subDir = strstr(pagePath, rulePath);
                        if(subDir != 0)
                        {   // rule does apply
                            *accRule = *pRule;
                            return;
                        }
                    }
                }
            }
        }
        // next rule
        pRule = (TCPIP_HTTP_ACCESS_RULE*)((uint8_t*)pRule + pRule->ruleSize);
    }
        
    // no rule matched
    // the default rule is block
}

// returns the redirection URI result for an access page that needs redirection
// accessDir - the dir/page that was accessed
// redirURI - the redirection dir/page set by the access rule
// redirBuff - buffer for depositing the result
//
// dir/page redirection rules:
// redirURI == '/redir/'
//      the redirect string will try to replicate the accessDir as much as possible:
//          '/dir/page.x' will redirect to '/redir/page.x'
//          '/dir/' or '/dir' will redirect to '/redir/'
// else
//  redirURI == '/redir' or redirURI == '/redir/rpage.y'
//      the redirect string will be 'redirURI no matter what the access page/dir is
//      ('/dir/page.x', '/dir/', '/dir' will redirect to '/redir')
//      ('/dir/page.x', '/dir/', '/dir' will redirect to '/redir/rpage.y')
//      
static bool _HTTP_Get_RedirectURI(const char* accessDir, const char* redirURI, char* redirBuff, size_t buffSize)
{
    size_t outLen;

    size_t redirLen = strlen(redirURI);
    if(redirURI[redirLen - 1] == '/')
    {   // redirect to a directory
        outLen = snprintf(redirBuff, buffSize, "%s", redirURI);
        // find the last '/' in the accessDir
        
        const char* dirEnd = accessDir + strlen(accessDir);
        const char* endPath = strrchr(accessDir, '/');
        if(endPath != 0 && (endPath + 1) != dirEnd)
        {   // there is something after the last '/'
            outLen = snprintf(redirBuff + outLen, buffSize - outLen, "%s", endPath + 1);
        }
    }
    else
    {   // redirect to a redirURI
        outLen = snprintf(redirBuff, buffSize, "%s", redirURI);
    }
    
    return outLen < buffSize;    // true if the redirBuff is not overflowed!
}
#endif  // (_TCPIP_HTTP_USE_ACCESS_RULES != 0)

// initializes the ports belonging to an instance
static TCPIP_HTTP_INST_RES _HTTP_InitInstance_Ports(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg)
{
    size_t portIx;
    TCPIP_HTTP_INST_RES createRes;
    TCPIP_HTTP_PORT_DCPT* pPort;
    const TCPIP_HTTP_PORT_CONFIG* pCfgPort;
    
    // initialize the ports
    pPort = pInstance->portDcpt;
    pCfgPort = pInstCfg->portConfig;
    for(portIx = 0; portIx < pInstance->nPorts; portIx++, pCfgPort++, pPort++)
    {
        pPort->connDataSize = pCfgPort->dataLen;
        pPort->listenPort = pCfgPort->listenPort;
        pPort->connNo = pCfgPort->nConnections;
        pPort->portFlags = pCfgPort->portFlags;
        pPort->portIx = portIx;

        pPort->persistTmo = pCfgPort->connTimeout;
        if(pPort->persistTmo > TCPIP_HTTP_CONN_MAX_TIMEOUT)
        {
            pPort->persistTmo = TCPIP_HTTP_CONN_MAX_TIMEOUT;
        }

        // create the SYS_FS shell
        SYS_FS_SHELL_RES shellRes;
        const char* webDir = pCfgPort->webDir;
        pPort->fileShell = SYS_FS_Shell_Create(webDir, 0, 0, 0, &shellRes);
        if(pPort->fileShell == 0)
        {
            SYS_ERROR(SYS_ERROR_WARNING, " HTTP: FS Shell creation for directory: %s Failed: %d!\r\n", webDir, shellRes);
            return TCPIP_HTTP_INST_RES_FS_SHELL_ERR; 
        }

        // create the HTTP port connections
        createRes = _HTTP_CreatePortConnections(pInstance, pCfgPort, pPort);
        if(createRes != TCPIP_HTTP_INST_RES_OK)
        {
            return createRes;
        }
    }

    return TCPIP_HTTP_INST_RES_OK; 
}


#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_HTTP_Server_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    // one way or another this interface is going down

    if(httpInitCount > 0)
    {   // we're up and running
        int instIx, portIx;
        TCPIP_HTTP_INST*    pInstance;
        for(instIx = 0; instIx < httpInstNo; instIx++)
        {
            pInstance = httpInstances[instIx];
            TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt;
            for(portIx = 0; portIx < pInstance->nPorts; portIx++, pPort++)
            {
                _HTTP_CloseConnections(pPort, stackCtrl->pNetIf);
            }
        }

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

void TCPIP_HTTP_Server_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { //  some signal occurred
        TCPIP_HTTP_NET_Process();
    }
}

// send a signal to the HTTP module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void _HTTPSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T skt, TCPIP_NET_HANDLE hNet, uint16_t sigType, const void* param)
{
    if(sigType == TCPIP_TCP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

static void TCPIP_HTTP_NET_Process(void)
{
    int         instIx, portIx;
    TCPIP_HTTP_INST*    pInstance;
    TCPIP_HTTP_PORT_DCPT*   pPort;
    TCPIP_HTTP_CONN* pHttpCon;
    int conn;

    // process instances in turn
    for(instIx = 0; instIx < httpInstNo; instIx++)
    {
        pInstance = httpInstances[instIx];

        pPort = pInstance->portDcpt;
        for(portIx = 0; portIx < pInstance->nPorts; portIx++, pPort++)
        {
            pHttpCon = pPort->connCtrl + 0;
            for(conn = 0; conn < pPort->connNo; conn++, pHttpCon++)
            {
                if(pHttpCon->socket == NET_PRES_INVALID_SOCKET)
                {
                    continue;
                }

                // If a socket is reset at any time 
                // forget about it and return to idle state.
                if(NET_PRES_SocketWasReset(pHttpCon->socket) || NET_PRES_SocketWasDisconnected(pHttpCon->socket))
                {
                    if(pHttpCon->flags.sktLocalReset != 0)
                    {   // http initiated the disconnect; no error
                        pHttpCon->flags.sktLocalReset = 0;
                    }
                    else
                    {   // some disconnect initiated by the remote party
                        pHttpCon->flags.discardRxBuff = 1;
                        pHttpCon->connState = TCPIP_HTTP_CONN_STATE_ERROR;
                        pHttpCon->closeEvent = TCPIP_HTTP_EVENT_CLOSE_REMOTE;
                    }
                }

                // Determine if this connection is eligible for processing
                if(pHttpCon->connState != TCPIP_HTTP_CONN_STATE_IDLE || NET_PRES_SocketReadIsReady(pHttpCon->socket))
                {
                    TCPIP_HTTP_NET_ProcessConnection(pHttpCon);
                }
                else if((pPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) == 0 && pPort->persistTmo != 0 && pHttpCon->flags.sktIsConnected != 0)
                {   // check the connection has not timed out
                    uint16_t currSec = (uint16_t)_TCPIP_SecCountGet();
                    if((currSec - pHttpCon->connActiveSec) >= pPort->persistTmo)
                    {   // timeout; kill the connection
                        _HTTP_DbgConnTmo(pHttpCon);
                        pHttpCon->connActiveSec = currSec;  // disconnect may take a while, avoid a new disconnect request
                        pHttpCon->closeEvent = TCPIP_HTTP_EVENT_CLOSE_TIMEOUT;
                        pHttpCon->connState = TCPIP_HTTP_CONN_STATE_DISCONNECT;
                    }
                }
            }
        }
    }
}


// Performs any pending operations for the currently loaded HTTP connection.
static void TCPIP_HTTP_NET_ProcessConnection(TCPIP_HTTP_CONN* pHttpCon)
{
    bool needWait;

    // mark connection as active
    pHttpCon->connActiveSec = (uint16_t)_TCPIP_SecCountGet();

    do
    {
        needWait = false;   // wait disabled by default
        // call the connection status process function
        _HTTP_DbgGetState(pHttpCon);
        pHttpCon->connState = (*_HTTP_ConnProcess_Tbl[pHttpCon->connState])(pHttpCon, &needWait);
        _HTTP_DbgNewState(pHttpCon);
    }while(needWait == false);

}

/*****************************************************************************
  Function:
    static bool _HTTP_HeaderParseLookup(TCPIP_HTTP_CONN* pHttpCon, int reqIx)

  Description:
    Calls the appropriate header parser based on the index of the header
    that was read from the request.

  Precondition:
    None

  Parameters:
    i - the index of a HTTP request (entry into the HTTPRequestHeaders table)

  Return Values:
    true if parsing succeeded
    false if some error
  ***************************************************************************/
static bool _HTTP_HeaderParseLookup(TCPIP_HTTP_CONN* pHttpCon, int reqIx)
{
    // reqIx corresponds to an index in HTTPRequestHeaders

#if defined(TCPIP_HTTP_USE_COOKIES)
    if(reqIx == 0u)
    {
        return _HTTP_HeaderParseCookie(pHttpCon);
    }
#endif

#if defined(TCPIP_HTTP_USE_AUTHENTICATION)    
    if(reqIx == 1u)
    {
        return _HTTP_HeaderParseAuthorization(pHttpCon);
    }
#endif

#if defined(TCPIP_HTTP_USE_POST)
    if(reqIx == 2u)
    {
        return _HTTP_HeaderParseContentLength(pHttpCon);
    }
#endif

    return true;

}

/*****************************************************************************
  Function:
    static bool _HTTP_HeaderParseAuthorization(TCPIP_HTTP_CONN* pHttpCon)

  Summary:
    Parses the "Authorization:" header for a request and verifies the
    credentials.

  Description:
    Parses the "Authorization:" header for a request.  For example, 
    "BASIC YWRtaW46cGFzc3dvcmQ=" is decoded to a user name of "admin" and
    a password of "password".  Once read, TCPIP_HTTP_ConnectionUserAuthenticate is called from
    custom_http_app.c to determine if the credentials are acceptable.

    The return value of TCPIP_HTTP_ConnectionUserAuthenticate is saved in pHttpCon->isAuthorized for
    later use by the application.

  Precondition:
    None

  Parameters:
    connection pointer

  Returns:
    true if parsing succeeded
    false if some error

  Remarks:
    This function is ony available when TCPIP_HTTP_USE_AUTHENTICATION is defined.
  ***************************************************************************/
#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
static bool _HTTP_HeaderParseAuthorization(TCPIP_HTTP_CONN* pHttpCon)
{
    uint16_t len, nDec;
    uint8_t inBuff[40];
    char   outBuff[40 + 2];
    char* passStr; 
    char* queryStr;

    // If auth processing is not required, return
    if(pHttpCon->isAuthorized & 0x80)
    {
        return true;
    }

    // Clear the auth type ("BASIC ")
    _HTTP_ConnectionDiscard(pHttpCon, 6);

    // Find the terminating CRLF, make sure it's a multiple of four and limit the size
    len = (uint16_t)_HTTP_ConnectionStringFind(pHttpCon, TCPIP_HTTP_CRLF, 0, 0);
    len += 3;
    len &= 0xfc;
    len = mMIN(len, sizeof(inBuff)-4);


    NET_PRES_SocketRead(pHttpCon->socket, inBuff, len);
    nDec = TCPIP_Helper_Base64Decode(inBuff, len, (uint8_t*)outBuff, sizeof(outBuff) - 2);
    outBuff[nDec] = 0;
    queryStr = strstr(outBuff, ":");
    if(queryStr)
    {
        *queryStr = 0;
        passStr = queryStr + 1;
    }
    else
    {
        passStr = outBuff + nDec;
        *passStr = 0;
    }

    // Verify credentials
    const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
    if(httpUserCback->userAuthenticate)
    {
        pHttpCon->isAuthorized = (*httpUserCback->userAuthenticate)(pHttpCon, outBuff, passStr, httpUserCback);
    }
    else
    {
        pHttpCon->isAuthorized = 0;
    }

    return true;
}
#endif

/*****************************************************************************
  Function:
    static bool _HTTP_HeaderParseCookie(TCPIP_HTTP_CONN* pHttpCon)

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
    connection pointer

  Returns:
    true if parsing succeeded
    false if some error

  Remarks:
    This function is ony available when TCPIP_HTTP_USE_COOKIES is defined.
  ***************************************************************************/
#if defined(TCPIP_HTTP_USE_COOKIES)
static bool _HTTP_HeaderParseCookie(TCPIP_HTTP_CONN* pHttpCon)
{
    uint16_t lenA, lenB;

    // Verify there's enough space
    uint16_t connDataSize = pHttpCon->connPort->connDataSize;
    lenB = (uint16_t)_HTTP_ConnectionStringFind(pHttpCon, TCPIP_HTTP_CRLF, 0, 0);
    if(lenB >= (uint16_t)(pHttpCon->httpData + connDataSize - pHttpCon->ptrData - 2))
    {   // If not, overflow
        pHttpCon->httpStatus = TCPIP_HTTP_STAT_OVERFLOW;
        return false;
    }

    // While a CRLF is not immediate, grab a cookie value
    while(lenB != 0u)
    {
        // Look for a ';' and use the shorter of that or a CRLF
        lenA = _HTTP_ConnectionCharFind(pHttpCon, ';', 0, 0);

        // Read to the terminator
        pHttpCon->ptrData += NET_PRES_SocketRead(pHttpCon->socket, pHttpCon->ptrData, mMIN(lenA, lenB));

        // Insert an & to anticipate another cookie
        *(pHttpCon->ptrData++) = '&';

        // If semicolon, trash it and whitespace
        if(lenA < lenB)
        {
            _HTTP_ConnectionDiscard(pHttpCon, 1);
            while(_HTTP_ConnectionCharFind(pHttpCon, ' ', 0, 0) == 0u)
            {
                _HTTP_ConnectionDiscard(pHttpCon, 1);
            }
        }

        // Find the new distance to the CRLF
        lenB = (uint16_t)_HTTP_ConnectionStringFind(pHttpCon, TCPIP_HTTP_CRLF, 0, 0);
    }

    return true;

}
#endif

/*****************************************************************************
  Function:
    static bool _HTTP_HeaderParseContentLength(TCPIP_HTTP_CONN* pHttpCon)

  Summary:
    Parses the "Content-Length:" header for a request.

  Description:
    Parses the "Content-Length:" header to determine how many bytes of
    POST data to expect after the request.  This value is stored in 
    pHttpCon->byteCount.

  Precondition:
    None

  Parameters:
    connection pointer

  Returns:
    true if parsing succeeded
    false if some error

  Remarks:
    This function is ony available when TCPIP_HTTP_USE_POST is defined.
  ***************************************************************************/
#if defined(TCPIP_HTTP_USE_POST)
static bool _HTTP_HeaderParseContentLength(TCPIP_HTTP_CONN* pHttpCon)
{
    uint16_t len;
    uint8_t buf[10];

    // Read up to the CRLF (max 9 bytes)
    len = (uint16_t)_HTTP_ConnectionStringFind(pHttpCon, TCPIP_HTTP_CRLF, 0, 0);
    if(len >= sizeof(buf))
    {
        pHttpCon->httpStatus = TCPIP_HTTP_STAT_BAD_REQUEST;
        pHttpCon->byteCount = 0;
        return false;
    }   
    len = NET_PRES_SocketRead(pHttpCon->socket, buf, len);
    buf[len] = '\0';

    pHttpCon->byteCount = atol((char*)buf);

    return true;
}
#endif

// process HTTP idle state: TCPIP_HTTP_CONN_STATE_IDLE
// returns the next connection state
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE _HTTP_ProcessIdle(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    uint16_t lenA;

    // Check how much data is waiting
    lenA = NET_PRES_SocketReadIsReady(pHttpCon->socket);
    if(lenA == 0)
    {   // no data; wait some more
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_IDLE;
    }

    // process the request
    // Clear out state info and move to next state
    pHttpCon->ptrData = pHttpCon->httpData;
    pHttpCon->isAuthorized = 0xff;
    pHttpCon->hasArgs = 0;
    pHttpCon->httpTick = SYS_TMR_TickCountGet() + TCPIP_HTTP_TIMEOUT * SYS_TMR_TickCounterFrequencyGet();
    pHttpCon->callbackPos = 0xffffffff;
    pHttpCon->byteCount = 0;
    pHttpCon->smPost = 0x00;
    pHttpCon->httpStatus = TCPIP_HTTP_STAT_GET; // show no error condition
    pHttpCon->closeEvent = 0;
    if(pHttpCon->flags.sktIsConnected == 0)
    {   // report it once
        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_OPEN, 0);
    }
    pHttpCon->flags.val = 0;
    pHttpCon->flags.sktIsConnected = 1;

    return TCPIP_HTTP_CONN_STATE_IDLE + 1;

}

// parse HTTP request state: TCPIP_HTTP_CONN_STATE_PARSE_REQUEST
// returns the next connection state
// also signals if waiting for resources
// Retrieves the file name in pHttpCon->httpData!
static TCPIP_HTTP_CONN_STATE _HTTP_ParseRequest(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    uint16_t lenA, lenB;

    // check that there is some registered HTTP app
    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance;
    if((pInstance->instanceFlags & TCPIP_HTTP_EXT_FLAG_USR_REG) == 0)
    {
        pHttpCon->flags.discardRxBuff = 1;
        return TCPIP_HTTP_CONN_STATE_ERROR;
    }

    // Verify the entire first line is in the FIFO
    if((lenB = _HTTP_ConnectionCharFind(pHttpCon, TCPIP_HTTP_NET_LINE_END, 0, 0)) == 0xffff)
    {   // First line isn't here yet
        if(_HTTP_SktFifoRxFree(pHttpCon->socket) == 0)
        {   // If the FIFO is full, we overflowed
            pHttpCon->httpStatus = TCPIP_HTTP_STAT_OVERFLOW;
            pHttpCon->flags.discardRxBuff = 1;
            pHttpCon->flags.requestError = 1;
            return TCPIP_HTTP_CONN_STATE_SERVE_HEADERS;
        }

        if((int32_t)(SYS_TMR_TickCountGet() - pHttpCon->httpTick) > 0)
        {   // A timeout has occurred
            pHttpCon->flags.discardRxBuff = 1;
            pHttpCon->closeEvent = TCPIP_HTTP_EVENT_CLOSE_TIMEOUT;
            return TCPIP_HTTP_CONN_STATE_ERROR;
        }
        // wait some more
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_PARSE_REQUEST;
    }

    // Reset the watchdog timer
    pHttpCon->httpTick = SYS_TMR_TickCountGet() + TCPIP_HTTP_TIMEOUT * SYS_TMR_TickCounterFrequencyGet();

    // Determine the request method
    lenA = _HTTP_ConnectionCharFind(pHttpCon, ' ', 0, 0);
    if(lenA > 5u)
    {
        lenA = 5;
    }
    NET_PRES_SocketRead(pHttpCon->socket, pHttpCon->httpData, lenA + 1);

    if ( memcmp(pHttpCon->httpData, (const void*)"GET", 3) == 0)
    {
        pHttpCon->httpStatus = TCPIP_HTTP_STAT_GET;
    }
#if defined(TCPIP_HTTP_USE_POST)
    else if ( memcmp(pHttpCon->httpData, (const void*)"POST", 4) == 0)
    {
        pHttpCon->httpStatus = TCPIP_HTTP_STAT_POST;
    }
#endif
    else
    {   // Unrecognized method, so return not implemented
        pHttpCon->httpStatus = TCPIP_HTTP_STAT_NOT_IMPLEMENTED;
        pHttpCon->flags.requestError = 1;
        return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
    }

    // Find end of filename
    lenA = _HTTP_ConnectionCharFind(pHttpCon, ' ', 0, 0);
    lenB = _HTTP_ConnectionCharFind(pHttpCon, '?', 0, lenA);
    lenA = mMIN(lenA, lenB);

    // If the file name is too long, then reject the request
    if(lenA > pHttpCon->connPort->connDataSize - TCPIP_HTTP_DEFAULT_LEN - 1)
    {
        pHttpCon->httpStatus = TCPIP_HTTP_STAT_OVERFLOW;
        pHttpCon->flags.requestError = 1;
        return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
    }

    // Read in the filename and decode
    lenB = NET_PRES_SocketRead(pHttpCon->socket, pHttpCon->httpData, lenA);
    pHttpCon->httpData[lenB] = '\0';
    TCPIP_HTTP_URLDecode(pHttpCon->httpData);

    return TCPIP_HTTP_CONN_STATE_PARSE_REQUEST + 1; // advance 
}

// parse HTTP file upload state: TCPIP_HTTP_CONN_STATE_PARSE_FILE_UPLOAD
// returns the next connection state
// also signals if waiting for resources
// Uses the file name in pHttpCon->httpData!
static TCPIP_HTTP_CONN_STATE _HTTP_ParseFileUpload(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    // Check if this is an FS Upload
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    if(memcmp(&pHttpCon->httpData[1], TCPIP_HTTP_FILE_UPLOAD_NAME, sizeof(TCPIP_HTTP_FILE_UPLOAD_NAME)) == 0)
    {   // Read remainder of line, and bypass all file opening, etc.
        if(strlen((char*)pHttpCon->httpData + 1) > sizeof(pHttpCon->fileName))
        {
            _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_NAME_SIZE_ERROR, pHttpCon->httpData + 1);
        }
        strncpy(pHttpCon->fileName, (char*)pHttpCon->httpData + 1, sizeof(pHttpCon->fileName) - 1);
        pHttpCon->fileName[sizeof(pHttpCon->fileName) -  1] = 0;

#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
        const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
        if(httpUserCback->fileAuthenticate)
        {
            pHttpCon->isAuthorized = (*httpUserCback->fileAuthenticate)(pHttpCon, pHttpCon->fileName, httpUserCback);
        }
        else
        {
            pHttpCon->isAuthorized = 0;
        }
#endif
        if(pHttpCon->httpStatus == TCPIP_HTTP_STAT_GET)
        {
            pHttpCon->httpStatus = TCPIP_HTTP_STAT_UPLOAD_FORM;
        }
        else
        {
            pHttpCon->flags.uploadPhase = 0;
            pHttpCon->httpStatus = TCPIP_HTTP_STAT_UPLOAD_STARTED;
        }

        return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
    }
#endif
    return TCPIP_HTTP_CONN_STATE_PARSE_FILE_UPLOAD + 1;
}

// parse HTTP file open state: TCPIP_HTTP_CONN_STATE_PARSE_FILE_OPEN
// returns the next connection state
// also signals if waiting for resources
// Uses the file name in pHttpCon->httpData!
static TCPIP_HTTP_CONN_STATE _HTTP_ParseFileOpen(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    int ix;
    char *ext;
    uint16_t lenB;

    TCPIP_HTTP_PORT_DCPT* pPort = pHttpCon->connPort;
    const SYS_FS_SHELL_OBJ*  fileShell = pPort->fileShell;

#if (_TCPIP_HTTP_USE_ACCESS_RULES != 0)
    TCPIP_HTTP_ACCESS_RULE accRule;
    _HTTP_GetPageAccess(pHttpCon, (char*)pHttpCon->httpData, &accRule);
    if(accRule.action == TCPIP_HTTP_ACCESS_ACTION_BLOCK)
    {
        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_ACCESS_BLOCK, pHttpCon->httpData + 1);
        pHttpCon->httpStatus = TCPIP_HTTP_STAT_NOT_FOUND;
        pHttpCon->flags.requestError = 1;
        return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
    } 
    else if(accRule.action == TCPIP_HTTP_ACCESS_ACTION_REDIRECT)
    {   // get redirect info
        char redirBuff[TCPIP_HTTP_REDIR_BUFF_SIZE];
        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_ACCESS_REDIRECT, pHttpCon->httpData + 1);

        // redirect to accRule.redirURI
        if(!_HTTP_Get_RedirectURI((char*)(pHttpCon->httpData + 1), accRule.redirURI, redirBuff, sizeof(redirBuff)))
        {
            _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_REDIRECT_BUFFER_SIZE_EXCEEDED, pHttpCon->httpData + 1);
        }
#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_REDIRECT) != 0)
        SYS_CONSOLE_PRINT("redirected -%s- to ", pHttpCon->httpData);
#endif  // ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_REDIRECT) != 0)
        sprintf((char*)pHttpCon->httpData, "%s%s", accRule.redirServer, redirBuff);
#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_REDIRECT) != 0)
        SYS_CONSOLE_PRINT("%s\r\n", pHttpCon->httpData);
#endif  // ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_REDIRECT) != 0)

        NET_PRES_SocketDiscard(pHttpCon->socket);
        pHttpCon->httpStatus = TCPIP_HTTP_STAT_REDIRECT;
        pHttpCon->redirType = accRule.redirType;
        return TCPIP_HTTP_CONN_STATE_SERVE_HEADERS;
    }

    // else just let it through...
    

#endif  // (_TCPIP_HTTP_USE_ACCESS_RULES != 0)

    // Decode may have changed the string length - update it here
    lenB = strlen((char*)pHttpCon->httpData);

    // If the last character is a not a directory delimiter, then try to open the file
    // String starts at 2nd character, because the first is always a '/'
    if(pHttpCon->httpData[lenB-1] != '/')
    {
        pHttpCon->file = (*fileShell->fileOpen)(fileShell, (char *)&pHttpCon->httpData[1], SYS_FS_FILE_OPEN_READ);
    }

    // If the open fails, then add our default name and try again
    if(pHttpCon->file == SYS_FS_HANDLE_INVALID)
    {
        if(pHttpCon->httpData[lenB-1] != '/')
        {   // Add the directory delimiter if needed
            pHttpCon->httpData[lenB++] = '/';
        }

        // Add our default file name
        strncpy((char*)pHttpCon->httpData + lenB, TCPIP_HTTP_DEFAULT_FILE, pPort->connDataSize - lenB);
        lenB += strlen(TCPIP_HTTP_DEFAULT_FILE);

        // Try to open again
        pHttpCon->file = (*fileShell->fileOpen)(fileShell, (char *)&pHttpCon->httpData[1], SYS_FS_FILE_OPEN_READ);
    }

    if(pHttpCon->file == SYS_FS_HANDLE_INVALID)
    {   // failed
        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_OPEN_ERROR, pHttpCon->httpData + 1);
        pHttpCon->httpStatus = TCPIP_HTTP_STAT_NOT_FOUND;
        pHttpCon->flags.requestError = 1;
        return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
    }

    _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_OPEN, pHttpCon->httpData + 1);
    if(strlen((char*)pHttpCon->httpData + 1) > sizeof(pHttpCon->fileName))
    {
        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_NAME_SIZE_ERROR, pHttpCon->httpData + 1);
    }
    strncpy(pHttpCon->fileName, (char*)pHttpCon->httpData + 1, sizeof(pHttpCon->fileName) - 1);
    pHttpCon->fileName[sizeof(pHttpCon->fileName) - 1] = 0;

    // Find the extension in the filename
    ext = strrchr(pHttpCon->fileName, TCPIP_HTTP_FILE_EXT_SEP);

    if(ext)
    {   // Compare to known extensions to determine Content-Type
        ext++;
        for(ix = 0; ix < sizeof(httpFileExtensions) / sizeof(*httpFileExtensions); ix++)
        {
            if(strcmp(ext, httpFileExtensions[ix]) == 0)
            {
                break;
            }
        }
        pHttpCon->fileType = (TCPIP_HTTP_FILE_TYPE)ix;  // TCPIP_HTTP_FILE_TYPE_UNKNOWN if not found
    }
    else
    {
        pHttpCon->fileType = TCPIP_HTTP_FILE_TYPE_UNKNOWN; 
    }

    // Perform first round authentication (pass file name only)
#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
    const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
    if(httpUserCback->fileAuthenticate)
    {
        pHttpCon->isAuthorized = (*httpUserCback->fileAuthenticate)(pHttpCon, pHttpCon->fileName, httpUserCback);
    }
    else
    {
        pHttpCon->isAuthorized = 0;
    }
#endif

    return TCPIP_HTTP_CONN_STATE_PARSE_FILE_OPEN + 1;   // advance

}

// parse HTTP GET arguments: TCPIP_HTTP_CONN_STATE_PARSE_GET_ARGS
// returns the next connection state
// also signals if waiting for resources
// Uses the file name in pHttpCon->httpData!
static TCPIP_HTTP_CONN_STATE _HTTP_ParseGetArgs(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    uint16_t lenA;
    uint8_t c;

    // Read GET args, up to buffer size - 1
    lenA = _HTTP_ConnectionCharFind(pHttpCon, ' ', 0, 0);
    if(lenA != 0)
    {
        pHttpCon->hasArgs = 1;

        // Trash the '?'
        NET_PRES_SocketRead(pHttpCon->socket, &c, 1);

        // Verify there's enough space
        lenA--;
        if(lenA >= pHttpCon->connPort->connDataSize - 2)
        {
            pHttpCon->httpStatus = TCPIP_HTTP_STAT_OVERFLOW;
            pHttpCon->flags.requestError = 1;
            return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
        }

        // Read in the arguments and '&'-terminate in anticipation of cookies
        pHttpCon->ptrData += NET_PRES_SocketRead(pHttpCon->socket, pHttpCon->httpData, lenA);
        *(pHttpCon->ptrData++) = '&';
    }

    // Clear the rest of the line
    lenA = _HTTP_ConnectionCharFind(pHttpCon, TCPIP_HTTP_NET_LINE_END, 0, 0);
    _HTTP_ConnectionDiscard(pHttpCon, lenA + 1);

    // Move to parsing the headers
    return TCPIP_HTTP_CONN_STATE_PARSE_GET_ARGS + 1;    // advance

}

    
// parse HTTP headers state: TCPIP_HTTP_CONN_STATE_PARSE_HEADERS 
// returns the next connection state
// also signals if waiting for resources
// if requestError == 1, it eats up the header fields only
// it can set requestError by itself
static TCPIP_HTTP_CONN_STATE _HTTP_ParseHeaders(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    bool clearLine;
    int ix;
    uint16_t lenA, lenB;
    bool parseFail;
    uint8_t buffer[TCPIP_HTTP_MAX_HEADER_LEN + 1];

    // Loop over all the headers
    while(1)
    {
        // Make sure entire line is in the FIFO
        lenA = _HTTP_ConnectionCharFind(pHttpCon, TCPIP_HTTP_NET_LINE_END, 0, 0);
        if(lenA == 0xffff)
        {   // If not, make sure we can receive more data
            if(_HTTP_SktFifoRxFree(pHttpCon->socket) == 0)
            {   // Overflow
                pHttpCon->httpStatus = TCPIP_HTTP_STAT_OVERFLOW;
                pHttpCon->flags.discardRxBuff = 1;
                pHttpCon->flags.requestError = 1;
                return TCPIP_HTTP_CONN_STATE_SERVE_HEADERS;
            }

            if((int32_t)(SYS_TMR_TickCountGet() - pHttpCon->httpTick) > 0)
            {   // A timeout has occured
                pHttpCon->flags.discardRxBuff = 1;
                pHttpCon->closeEvent = TCPIP_HTTP_EVENT_CLOSE_TIMEOUT;
                return TCPIP_HTTP_CONN_STATE_ERROR;
            }

            // wait some more
            *pWait = true;
            return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
        }

        // Reset the watchdog timer
        pHttpCon->httpTick = SYS_TMR_TickCountGet() + TCPIP_HTTP_TIMEOUT * SYS_TMR_TickCounterFrequencyGet();

        // If a CRLF is immediate, then headers are done
        if(lenA == 1u)
        {   // Remove the CRLF and move to next state
            _HTTP_ConnectionDiscard(pHttpCon, 2);
            return (pHttpCon->flags.requestError == 1) ? TCPIP_HTTP_CONN_STATE_SERVE_HEADERS : TCPIP_HTTP_CONN_STATE_PARSE_HEADERS + 1; // advance
        }

        if(pHttpCon->flags.requestError == 1)
        {   // in error mode just discard the line
            _HTTP_ConnectionDiscard(pHttpCon, lenA + 1);
            continue;
        }

        // Find the header name, and use needWait as a flag to indicate a match
        lenB = _HTTP_ConnectionCharFind(pHttpCon, ':', 0, lenA) + 2;

        // If name is too long or this line isn't a header, ignore it
        if(lenB > sizeof(buffer))
        {
            _HTTP_ConnectionDiscard(pHttpCon, lenA + 1);
            continue;
        }

        // Read in the header name
        NET_PRES_SocketRead(pHttpCon->socket, buffer, lenB);
        buffer[lenB-1] = '\0';
        lenA -= lenB;

        clearLine = false;
        // Compare header read to ones we're interested in
        parseFail = false;
        for(ix = 0; ix < sizeof(HTTPRequestHeaders)/sizeof(HTTPRequestHeaders[0]); ix++)
        {
            if(stricmp((char*)buffer, (const char *)HTTPRequestHeaders[ix]) == 0)
            {   // Parse the header and stop the loop
                parseFail = _HTTP_HeaderParseLookup(pHttpCon, ix) == false;
                clearLine = true;
                break;
            }
        }

        // Clear the rest of the line, and call the loop again
        if(clearLine)
        {   // We already know how much to remove unless a header was found
            lenA = _HTTP_ConnectionCharFind(pHttpCon, TCPIP_HTTP_NET_LINE_END, 0, 0);
        }
        _HTTP_ConnectionDiscard(pHttpCon, lenA + 1);
        if(parseFail)
        {   // signal the error
            pHttpCon->flags.requestError = 1;
        } 
    }

}


// process the authenticate state: TCPIP_HTTP_CONN_STATE_AUTHENTICATE
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE _HTTP_ProcessAuthenticate(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    uint8_t hasArgs;

#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
    // Check current authorization state
    if(pHttpCon->isAuthorized < 0x80)
    {   // 401 error
        pHttpCon->httpStatus = TCPIP_HTTP_STAT_UNAUTHORIZED;
        return TCPIP_HTTP_CONN_STATE_SERVE_HEADERS;
    }
#endif

    // Parse the args string
    *pHttpCon->ptrData = '\0';
    pHttpCon->ptrData = TCPIP_HTTP_URLDecode(pHttpCon->httpData);

    // If this is an upload form request, bypass to headers
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    if(pHttpCon->httpStatus == TCPIP_HTTP_STAT_UPLOAD_FORM)
    {
        return TCPIP_HTTP_CONN_STATE_SERVE_HEADERS;
    }
#endif

    hasArgs = pHttpCon->hasArgs;
    pHttpCon->hasArgs = 0;

    // Move on to GET args, unless there are none
    return (hasArgs != 0) ? TCPIP_HTTP_CONN_STATE_PROCESS_GET : TCPIP_HTTP_CONN_STATE_PROCESS_POST;
}



// process the GET state: TCPIP_HTTP_CONN_STATE_PROCESS_GET
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE _HTTP_ProcessGet(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    // Run the application callback TCPIP_HTTP_ConnectionGetExecute()
    const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
    if(httpUserCback->getExecute)
    {
        if((*httpUserCback->getExecute)(pHttpCon, httpUserCback) == TCPIP_HTTP_IO_RES_WAITING)
        {   // If waiting for asynchronous process, return to main app
            *pWait = true;
            return TCPIP_HTTP_CONN_STATE_PROCESS_GET;   // stay  here
        }
    }

    // Move on to POST data
    return TCPIP_HTTP_CONN_STATE_PROCESS_GET + 1;    // advance

}


// process the POST state: TCPIP_HTTP_CONN_STATE_PROCESS_POST
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE _HTTP_ProcessPost(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{

#if defined(TCPIP_HTTP_USE_POST)
    TCPIP_HTTP_IO_RESULT    ioRes;

    // See if we have any new data
    if(NET_PRES_SocketReadIsReady(pHttpCon->socket) == pHttpCon->callbackPos)
    {
        if((int32_t)(SYS_TMR_TickCountGet() - pHttpCon->httpTick) > 0)
        {   // If a timeout has occured, disconnect
            pHttpCon->closeEvent = TCPIP_HTTP_EVENT_CLOSE_TIMEOUT;
            return TCPIP_HTTP_CONN_STATE_ERROR;
        }
    }

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    if(pHttpCon->httpStatus == TCPIP_HTTP_STAT_POST || (pHttpCon->httpStatus >= TCPIP_HTTP_STAT_UPLOAD_STARTED && pHttpCon->httpStatus <= TCPIP_HTTP_STAT_UPLOAD_ERROR))
#else
        if(pHttpCon->httpStatus == TCPIP_HTTP_STAT_POST)
#endif
        {
            // Run the application callback TCPIP_HTTP_ConnectionPostExecute()
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
            if(pHttpCon->httpStatus >= TCPIP_HTTP_STAT_UPLOAD_STARTED && pHttpCon->httpStatus <= TCPIP_HTTP_STAT_UPLOAD_ERROR)
            {
                ioRes = TCPIP_HTTP_NET_FSUpload(pHttpCon);
            }
            else
#endif  // defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
            {
                const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
                if(httpUserCback->postExecute)
                {
                    ioRes = (*httpUserCback->postExecute)(pHttpCon, httpUserCback);
                }
                else
                {
                    ioRes = TCPIP_HTTP_IO_RES_DONE;
                }
            }

            // If waiting for asynchronous process, return to main app
            if(ioRes == TCPIP_HTTP_IO_RES_WAITING)
            {   // return to main app and make sure we don't get stuck by the watchdog
                pHttpCon->callbackPos = NET_PRES_SocketReadIsReady(pHttpCon->socket) - 1;
                *pWait = true;
                return TCPIP_HTTP_CONN_STATE_PROCESS_POST;  // wait
            }
            else if(ioRes == TCPIP_HTTP_IO_RES_NEED_DATA)
            {   // If waiting for more data
                pHttpCon->callbackPos = NET_PRES_SocketReadIsReady(pHttpCon->socket);
                pHttpCon->httpTick = SYS_TMR_TickCountGet() + TCPIP_HTTP_TIMEOUT * SYS_TMR_TickCounterFrequencyGet();

                // If more is expected and space is available, return to main app
                if(pHttpCon->byteCount > pHttpCon->callbackPos && _HTTP_SktFifoRxFree(pHttpCon->socket) != 0)
                {
                    *pWait = true;
                    return TCPIP_HTTP_CONN_STATE_PROCESS_POST;  // wait
                }

                // Handle cases where application ran out of data or buffer space
                pHttpCon->httpStatus = TCPIP_HTTP_STAT_INTERNAL_SERVER_ERROR;
                return TCPIP_HTTP_CONN_STATE_SERVE_HEADERS;  // error
            }
            else if(ioRes == TCPIP_HTTP_IO_RES_ERROR)
            {   // some error, abort
                pHttpCon->closeEvent = TCPIP_HTTP_EVENT_CLOSE_POST_ERROR;
                return TCPIP_HTTP_CONN_STATE_ERROR;
            }

            // else TCPIP_HTTP_IO_RES_DONE and move on; discard whatever the buffer may contain
            NET_PRES_SocketDiscard(pHttpCon->socket);
            return TCPIP_HTTP_CONN_STATE_PROCESS_POST + 1;  // advance
        }
#endif
    // We're done with POST
    return TCPIP_HTTP_CONN_STATE_PROCESS_POST + 1;  // advance
}


// process the serve connection headers state: TCPIP_HTTP_CONN_STATE_SERVE_HEADERS
// returns the next connection state:
// also signals if waiting for resources
// Note: all the previous parsing states should branch here after parsing the headers
// when an error detected // (except timeouts/file errors that go to TCPIP_HTTP_CONN_STATE_ERROR)!
// Processing should get here with all headers processed!
//
static TCPIP_HTTP_CONN_STATE _HTTP_ServeHeaders(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    bool isConnDone;
    bool fileGzipped;
    int  headerLen, responseLen, contentLen;
    SYS_FS_FSTAT fs_attr = {0};
    char contentLenBuffer[40];
    char responseBuffer[TCPIP_HTTP_RESPONSE_BUFFER_SIZE];

    if(pHttpCon->flags.procPhase == 0)
    {   // output headers now; Send header corresponding to the current state
        if(pHttpCon->httpStatus == TCPIP_HTTP_STAT_REDIRECT)
        {   // special case here, this header message takes arguments; 
            uint8_t redirType = pHttpCon->redirType; 
            if(redirType == 0 || redirType > TCPIP_HTTP_REDIR_TYPE_308)
            {   // use default
                redirType = _TCPIP_HTTP_REDIR_TYPE_DEFAULT;
            }
            else
            {
                pHttpCon->redirType = 0;        // restore the default
            }
            headerLen = sprintf(responseBuffer, TCPIP_HTTP_NET_HEADER_PREFIX "%s\r\nLocation: %s \r\n", HTTPRedirectHeaders[redirType - 1], (char*)pHttpCon->httpData);
        }
        else
        {
            headerLen = sprintf(responseBuffer, TCPIP_HTTP_NET_HEADER_PREFIX "%s", HTTPResponseHeaders[pHttpCon->httpStatus]);
        }

        if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) != 0)
        {
            headerLen += sprintf(responseBuffer + headerLen, "Connection: close\r\n");
        }

        if(!_HTTP_DataTryOutput(pHttpCon, responseBuffer, headerLen, 0))
        {   //  not enough room to send data; wait some more
            *pWait = true;
            return TCPIP_HTTP_CONN_STATE_SERVE_HEADERS;
        }
        // success
        pHttpCon->flags.procPhase = 1;
    }

    if(pHttpCon->flags.procPhase == 1)
    {   // output content length now;
        responseLen = 0;
        // if error or not GET or POST, we're done and we don't output any message body
        isConnDone = pHttpCon->flags.requestError != 0 || (pHttpCon->httpStatus != TCPIP_HTTP_STAT_GET && pHttpCon->httpStatus != TCPIP_HTTP_STAT_POST);
        
        if(HTTPResponseFunctions[pHttpCon->httpStatus] != 0)
        {
            responseLen = (*HTTPResponseFunctions[pHttpCon->httpStatus])(pHttpCon, responseBuffer, sizeof(responseBuffer));
            if(responseLen != 0)
            {
                contentLen = sprintf(contentLenBuffer, "Content-Length: %d\r\n", responseLen);
                if(!_HTTP_DataTryOutput(pHttpCon, contentLenBuffer, contentLen, contentLen + responseLen + isConnDone ? TCPIP_HTTP_CRLF_LEN : 0))
                {   // not enough room to send data; wait some more
                    *pWait = true;
                    return TCPIP_HTTP_CONN_STATE_SERVE_HEADERS;
                }
                // this should succeed now
                _HTTP_DataTryOutput(pHttpCon, responseBuffer, responseLen, 0);
                if(isConnDone)
                {
                    _HTTP_DataTryOutput(pHttpCon, TCPIP_HTTP_CRLF, TCPIP_HTTP_CRLF_LEN, 0);
                }
                pHttpCon->flags.procPhase = 2;
            }
        }

        if(isConnDone)
        {   // no message body; we're done;
            pHttpCon->flags.procPhase = 0;    // clear our traces
            return TCPIP_HTTP_CONN_STATE_DONE;
        }
        // continue
        pHttpCon->flags.procPhase = 2;
    }


    // pHttpCon->flags.procPhase == 2;

    // process a GET or POST - something that will have a message body

    responseLen = 0;
    // Output the content type, if known
    if(pHttpCon->fileType != TCPIP_HTTP_FILE_TYPE_UNKNOWN)
    {
        responseLen = sprintf(responseBuffer,  "Content-Type: %s\r\n", httpContentTypes[pHttpCon->fileType]);
    }

    // Output the gzip encoding header if needed
    fileGzipped = 0;
    const SYS_FS_SHELL_OBJ* fileShell = pHttpCon->connPort->fileShell;
    if((*fileShell->fileStat)(fileShell, (const char *)&pHttpCon->fileName, &fs_attr) != SYS_FS_HANDLE_INVALID)
    {
        if (fs_attr.fattrib == SYS_FS_ATTR_ZIP_COMPRESSED)
        {
            responseLen += sprintf(responseBuffer + responseLen,  "Content-Encoding: gzip\r\n");
            fileGzipped = 1;
        }
    }

    // Output the cache-control
    if((pHttpCon->httpStatus == TCPIP_HTTP_STAT_POST) || (fileGzipped == 0 && _HTTP_FileTypeIsDynamic(pHttpCon->fileName)))
    {   // This is a dynamic page or a POST request, so no cache
        responseLen += sprintf(responseBuffer + responseLen,  "Cache-Control: no-cache\r\n");
    }
    else
    {
        responseLen += sprintf(responseBuffer + responseLen,  "Cache-Control: max-age=" TCPIP_HTTP_CACHE_LEN TCPIP_HTTP_CRLF);
    }


    if(!_HTTP_DataTryOutput(pHttpCon, responseBuffer, responseLen, 0))
    {   // not enough room to send data; wait some more
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_SERVE_HEADERS;
    }


    // Check if we should output cookies
    return (pHttpCon->hasArgs != 0) ? TCPIP_HTTP_CONN_STATE_SERVE_HEADERS + 1 : TCPIP_HTTP_CONN_STATE_SERVE_BODY_INIT;

}


// serve connection cookies state: TCPIP_HTTP_CONN_STATE_SERVE_COOKIES
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE _HTTP_ServeCookies(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
#if defined(TCPIP_HTTP_USE_COOKIES)
    uint8_t c;
    char*   pBuff;
    char    cookieBuffer[TCPIP_HTTP_COOKIE_BUFFER_SIZE];
    

    // Write cookies one at a time as space permits
    for(pHttpCon->ptrRead = pHttpCon->httpData; pHttpCon->hasArgs != 0; pHttpCon->hasArgs--)
    {
        // Write the header
        strcpy(cookieBuffer, "Set-Cookie: ");
        pBuff = cookieBuffer + strlen(cookieBuffer);

        // Write the name, URL encoded, one character at a time
        while((c = *(pHttpCon->ptrRead++)))
        {
            if(c == ' ')
            {
                *pBuff++ = '+';
            }
            else if(c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z')
            {
                *pBuff++ = '%';
                *pBuff++ = btohexa_high(c);
                *pBuff++ = btohexa_low(c);
            }
            else
            {
                *pBuff++ = c;
            }
        }

        *pBuff++ = '=';

        // Write the value, URL encoded, one character at a time
        while((c = *(pHttpCon->ptrRead++)))
        {
            if(c == ' ')
            {
                *pBuff++ = '+';
            }
            else if(c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z')
            {
                *pBuff++ = '%';
                *pBuff++ = btohexa_high(c);
                *pBuff++ = btohexa_low(c);
            }
            else
            {
                *pBuff++ = c;
            }
        }

        // Finish the line
        *pBuff++ = '\r';
        *pBuff++ = '\n';

        if(!_HTTP_DataTryOutput(pHttpCon, cookieBuffer, pBuff - cookieBuffer, 0))
        {   //  not enough room to send data; wait some more
            *pWait = true;
            return TCPIP_HTTP_CONN_STATE_SERVE_COOKIES;
        }
    }
#endif

    return TCPIP_HTTP_CONN_STATE_SERVE_COOKIES + 1;   // done, advance
}

// process HTTP body init state: TCPIP_HTTP_CONN_STATE_SERVE_BODY_INIT
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE _HTTP_ServeBodyInit(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    int  encodeLen;
    char encodingBuffer[40];

    // Set up the dynamic substitutions
    pHttpCon->byteCount = 0;

    if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) == 0)
    {   // output encoding and end of headers
        encodeLen = sprintf(encodingBuffer, "Transfer-Encoding: chunked\r\n\r\n");
    }
    else
    {   // just terminate the headers
        encodeLen = sprintf(encodingBuffer, "\r\n");

    }

    if(!_HTTP_DataTryOutput(pHttpCon, encodingBuffer, encodeLen, 0))
    {   // not enough room to send data; wait some more
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_SERVE_BODY_INIT;
    }

    return TCPIP_HTTP_CONN_STATE_SERVE_BODY_INIT + 1;   // advance
}

// process HTTP serve body state: TCPIP_HTTP_CONN_STATE_SERVE_BODY
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE _HTTP_ServeBody(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    TCPIP_HTTP_CHUNK_RES chunkRes;

    chunkRes = _HTTP_AddFileChunk(pHttpCon, pHttpCon->file, pHttpCon->fileName, 0);
    if(chunkRes == TCPIP_HTTP_CHUNK_RES_WAIT)
    {   // need a break; stay here
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_SERVE_BODY;
    }

    // else continue processing
    pHttpCon->file = SYS_FS_HANDLE_INVALID; // it will be closed when its chunk is processed 
    return TCPIP_HTTP_CONN_STATE_SERVE_BODY + 1;    // advance
}

// process HTTP serve chunks state: TCPIP_HTTP_CONN_STATE_SERVE_CHUNKS
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE _HTTP_ServeChunks(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    TCPIP_HTTP_CHUNK_RES chunkRes;

    chunkRes = _HTTP_ProcessChunks(pHttpCon);

    if(chunkRes == TCPIP_HTTP_CHUNK_RES_WAIT)
    {   // need a break; wait here
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_SERVE_CHUNKS;
    }

    if(chunkRes == TCPIP_HTTP_CHUNK_RES_OK || chunkRes < 0)
    {   // either continue or some error occurred; try another chunk
        return TCPIP_HTTP_CONN_STATE_SERVE_CHUNKS;
    }

    // done processing chunks
    return TCPIP_HTTP_CONN_STATE_SERVE_CHUNKS + 1;
}


// process HTTP disconnect state: TCPIP_HTTP_CONN_STATE_DONE
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE _HTTP_ProcessDone(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) == 0)
    {  // keep connection open; Make sure any opened files are closed
        _HTTP_ConnCloseFile(pHttpCon);
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_IDLE;
    }

    // else nonpersistent: disconnect
    pHttpCon->closeEvent = TCPIP_HTTP_EVENT_CLOSE_DONE;
    return TCPIP_HTTP_CONN_STATE_DISCONNECT;
}


// clean ups the connection data and disconnects the socket
// it also discards the socket RX data as instructed by discardType:
//     - TCPIP_HTTP_DISCARD_NOT         - no discard done
//     - TCPIP_HTTP_DISCARD_WAIT_DISCON - discard if disconnect successful
//     - TCPIP_HTTP_DISCARD_WAIT_NOT    - discard anyway
// returns the disconnect operation result
static bool _HTTP_ConnCleanDisconnect(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_DISCARD_TYPE discardType)
{
    // Make sure any opened files are closed
    _HTTP_ConnCloseFile(pHttpCon);

    // purge all pending chunks
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt;
    while((pChDcpt = (TCPIP_HTTP_CHUNK_DCPT*)pHttpCon->chunkList.head) != 0)
    {
        _HTTP_FreeChunk(pHttpCon, pChDcpt);
    } 

    bool disconRes;
    if((disconRes = NET_PRES_SocketDisconnect(pHttpCon->socket)) == true)
    {
        pHttpCon->flags.sktLocalReset = 1;
        pHttpCon->flags.sktIsConnected = 0;
    }

    if(discardType != TCPIP_HTTP_DISCARD_NOT)
    {
        if(disconRes == true || discardType == TCPIP_HTTP_DISCARD_WAIT_NOT)
        {
            NET_PRES_SocketDiscard(pHttpCon->socket);
        }
    }

    return disconRes;
}

// process HTTP error state: TCPIP_HTTP_CONN_STATE_ERROR
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE _HTTP_ProcessError(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{

    _HTTP_ConnCleanDisconnect(pHttpCon, pHttpCon->flags.discardRxBuff != 0 ? TCPIP_HTTP_DISCARD_WAIT_NOT : TCPIP_HTTP_DISCARD_NOT);
    _HTTP_Report_ConnectionEvent(pHttpCon, (TCPIP_HTTP_EVENT_TYPE)pHttpCon->closeEvent, 0);

    *pWait = true;
    return TCPIP_HTTP_CONN_STATE_IDLE;
}

// process non-persistent HTTP disconnect state: TCPIP_HTTP_CONN_STATE_DISCONNECT
// persistent connection gets here only if timeout
// returns the next connection state: TCPIP_HTTP_CONN_STATE_IDLE
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE _HTTP_ProcessDisconnect(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    *pWait = true;

    bool disconRes = _HTTP_ConnCleanDisconnect(pHttpCon, TCPIP_HTTP_DISCARD_WAIT_DISCON);

    if(disconRes)
    {
        _HTTP_Report_ConnectionEvent(pHttpCon, (TCPIP_HTTP_EVENT_TYPE)pHttpCon->closeEvent, 0);
        return TCPIP_HTTP_CONN_STATE_IDLE;
    }

    // else retry next time
    return TCPIP_HTTP_CONN_STATE_DISCONNECT;
}

static int _HTTP_HeaderMsg_Print(char* buffer, size_t bufferSize, const char* fmt, ...)
{
    int nChars;
    va_list args = {0};

    va_start( args, fmt );
    nChars = vsnprintf(buffer, bufferSize, fmt, args);
    va_end( args );

    if(nChars >= 0)
    {   // successful
        if(nChars >= bufferSize)
        {   // exceeded
            nChars = bufferSize - 1;
        }
        return nChars;
    }

    return 0;
}




// outputs a response message from the HTTPResponseMessages
// used for messages without parameters
static int _HTTP_HeaderMsg_Generic(TCPIP_HTTP_CONN* pHttpCon, char* buffer, size_t bufferSize)
{

    const char* msg = HTTPResponseMessages[pHttpCon->httpStatus];

    return msg ? _HTTP_HeaderMsg_Print(buffer, bufferSize, msg) : 0;
}

static int _HTTP_HeaderMsg_NotFound(TCPIP_HTTP_CONN* pHttpCon, char* buffer, size_t bufferSize)
{
    const char* msg;

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    msg = "\r\n404: File not found<br>Use <a href=\"/" TCPIP_HTTP_FILE_UPLOAD_NAME "\">MPFS Upload</a> to program web pages\r\n";
    return _HTTP_HeaderMsg_Print(buffer, bufferSize, msg);
#else       
    msg = "\r\n404: File: %s not found\r\n";
    return _HTTP_HeaderMsg_Print(buffer, bufferSize, msg, pHttpCon->fileName);
#endif

}

static int _HTTP_HeaderMsg_Redirect(TCPIP_HTTP_CONN* pHttpCon, char* buffer, size_t bufferSize)
{
    const char* msg = "\r\nRedirect: %s\r\n";
    return _HTTP_HeaderMsg_Print(buffer, bufferSize, msg, (char*)pHttpCon->httpData);
}

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
static int _HTTP_HeaderMsg_UploadForm(TCPIP_HTTP_CONN* pHttpCon, char* buffer, size_t bufferSize)
{
    const char* msg = "\r\n<html><body style=\"margin:100px\"><form method=post action=\"/%s\" enctype=\"multipart/form-data\"><b>FS Image Upload</b><p><input type=file name=i size=40> &nbsp; <input type=submit value=\"Upload\"></form></body></html>";
    return _HTTP_HeaderMsg_Print(buffer, bufferSize, msg, pHttpCon->fileName);
}

static int _HTTP_HeaderMsg_UploadError(TCPIP_HTTP_CONN* pHttpCon, char* buffer, size_t bufferSize)
{
    // TCPIP_HTTP_STAT_UPLOAD_ERROR
    const char* msg;
    if(pHttpCon->flags.uploadMemError)
    {
        msg = "\r\n<html><body style=\"margin:100px\"><b>Upload Memory Allocation Error</b><p><a href=\"/%s \">Try again?</a></body></html>";
    }
    else
    {
        msg = "\r\n<html><body style=\"margin:100px\"><b>FS Image Corrupt or Wrong Version</b><p><a href=\"/%s \">Try again?</a></body></html>";
    }
    return _HTTP_HeaderMsg_Print(buffer, bufferSize, msg, pHttpCon->fileName);
}

#endif

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
        {
            *pWrite++ = '\0';
        }
        else if(c == '+')
        {
            *pWrite++ = ' ';
        }
        else if(c == '%')
        {
            if(wLen < 2u)
            {
                wLen = 0;
            }
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
        {
            *pWrite++ = c;
        }
    }

    // Double null terminate the last value
    *pWrite++ = '\0';
    *pWrite = '\0';

    return pWrite;
}

const uint8_t* TCPIP_HTTP_ArgGet(const uint8_t* cData, const uint8_t* cArg)
{
    // Search through the array while bytes remain
    while(*cData != '\0')
    { 
        // Look for arg at current position
        if(!strcmp((const char*)cArg, (const char*)cData))
        {   // Found it, so return parameter
            return cData + strlen((const char*)cArg) + 1;
        }

        // Skip past two strings (NUL bytes)
        cData += strlen((const char*)cData) + 1;
        cData += strlen((const char*)cData) + 1;
    }

    // Return NULL if not found
    return NULL;
}


#if defined(TCPIP_HTTP_USE_POST)
TCPIP_HTTP_READ_STATUS TCPIP_HTTP_ConnectionPostNameRead(TCPIP_HTTP_CONN_HANDLE connHandle, uint8_t* cData, uint16_t wLen)
{
    TCPIP_HTTP_READ_STATUS status;
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;

    status = _HTTP_ReadTo(pHttpCon, '=', cData, wLen);

    // Decode the data (if not reading to null or blank) and return
    if(cData && *cData)
    {
        TCPIP_HTTP_URLDecode(cData);
    }
    return status;
}   
#endif

#if defined(TCPIP_HTTP_USE_POST)
TCPIP_HTTP_READ_STATUS TCPIP_HTTP_ConnectionPostValueRead(TCPIP_HTTP_CONN_HANDLE connHandle, uint8_t* cData, uint16_t wLen)
{
    TCPIP_HTTP_READ_STATUS status;
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;

    // Try to read the value
    status = _HTTP_ReadTo(pHttpCon, '&', cData, wLen);

    // If read was incomplete, check if we're at the end
    if(status == TCPIP_HTTP_READ_INCOMPLETE)
    {
        // If all data has arrived, read all remaining data
        if(pHttpCon->byteCount == NET_PRES_SocketReadIsReady(pHttpCon->socket))
        {
            status = _HTTP_ReadTo(pHttpCon, '\0', cData, wLen);
        }
    }

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
    static TCPIP_HTTP_READ_STATUS _HTTP_ReadTo(TCPIP_HTTP_CONN* pHttpCon, uint8_t cDelim, uint8_t* cData, uint16_t wLen)

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
    TCPIP_HTTP_READ_OK - data was successfully read
    TCPIP_HTTP_READ_TRUNCTATED - entire data could not fit in the buffer, so the
                            data was truncated and data has been lost
    TCPIP_HTTP_READ_INCOMPLETE - delimiter character was not found
  ***************************************************************************/
#if defined(TCPIP_HTTP_USE_POST)
static TCPIP_HTTP_READ_STATUS _HTTP_ReadTo(TCPIP_HTTP_CONN* pHttpCon, uint8_t cDelim, uint8_t* cData, uint16_t wLen)
{
    TCPIP_HTTP_READ_STATUS status;
    uint16_t wPos;

    // Either look for delimiter, or read all available data
    if(cDelim)
    {
        wPos = _HTTP_ConnectionCharFind(pHttpCon, cDelim, 0, 0);
    }
    else
    {
        wPos = NET_PRES_SocketReadIsReady(pHttpCon->socket);
    }

    // If not found, return incomplete
    if(wPos == 0xffff)
    {
        return TCPIP_HTTP_READ_INCOMPLETE;
    }

    // Read the value
    if(wLen < 2u && cData != NULL)
    {   // Buffer is too small, so read to NULL instead
        pHttpCon->byteCount -= _HTTP_ConnectionDiscard(pHttpCon, wPos);
        status = TCPIP_HTTP_READ_TRUNCATED;
    }
    else if(cData == NULL)
    {   // Just remove the data
        pHttpCon->byteCount -= _HTTP_ConnectionDiscard(pHttpCon, wPos);
        status = TCPIP_HTTP_READ_OK;
    }
    else if(wPos > wLen - 2)
    {   // Read data, but truncate at max length
        pHttpCon->byteCount -= NET_PRES_SocketRead(pHttpCon->socket, cData, wLen - 2);
        pHttpCon->byteCount -= _HTTP_ConnectionDiscard(pHttpCon, wPos - (wLen - 2));
        cData[wLen - 2] = '\0';
        status = TCPIP_HTTP_READ_TRUNCATED;
    }
    else
    {   // Read the data normally
        pHttpCon->byteCount -= NET_PRES_SocketRead(pHttpCon->socket, cData, wPos);
        cData[wPos] = '\0';
        status = TCPIP_HTTP_READ_OK;
    }

    // Remove the delimiter
    if(cDelim)
    {
        pHttpCon->byteCount -= _HTTP_ConnectionDiscard(pHttpCon, 1);
    }

    return status;
}   
#endif

/*****************************************************************************
  Function:
    static TCPIP_HTTP_IO_RESULT TCPIP_HTTP_NET_FSUpload(TCPIP_HTTP_CONN* pHttpCon)

  Summary:
    Saves a file uploaded via POST as the new image in EEPROM or 
    external Flash.

  Description:
    Allows the FS image in EEPROM or external Flash to be updated via a 
    web page by accepting a file upload and storing it to the external memory.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    TCPIP_HTTP_IO_RES_DONE - on success
    TCPIP_HTTP_IO_RES_NEED_DATA - if more data is still expected

  Remarks:
    This function is only available when FS uploads are enabled and
    the FS image could be stored (EEPROM, flash, etc.)

  ***************************************************************************/
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
#define     SYS_FS_MEDIA_SECTOR_SIZE        512     
static TCPIP_HTTP_IO_RESULT TCPIP_HTTP_NET_FSUpload(TCPIP_HTTP_CONN* pHttpCon)
{
    uint8_t mpfsBuffer[sizeof(MPFS_SIGNATURE) - 1];  // large enough to hold the MPFS signature
    uint16_t lenA, lenB;
    uint32_t nSectors;
    uint32_t mpfsAllocSize;

    switch(pHttpCon->httpStatus)
    {
        // New upload, so look for the CRLFCRLF
        case TCPIP_HTTP_STAT_UPLOAD_STARTED:
            if(pHttpCon->flags.uploadPhase == 0)
            {   // just starting
                pHttpCon->uploadSectNo = 0;
                uint32_t peekRes = _HTTP_ConnectionStringFind(pHttpCon, "\r\n\r\n", 0, 0);

                if((uint16_t)peekRes == 0xffff)
                {   // End of line not found, remove as much as possible
                    lenA = peekRes >> 16;   // get the # of bytes safe to remove
                    if(lenA > 4)
                    {
                        lenA = _HTTP_ConnectionDiscard(pHttpCon, lenA - 4);
                        pHttpCon->byteCount -= lenA;
                    }
                    break;
                }

                // Found end of line, so remove all data up to and including
                lenA = _HTTP_ConnectionDiscard(pHttpCon, (uint16_t)peekRes + 4);
                pHttpCon->byteCount -= (lenA + 4);

                pHttpCon->flags.uploadPhase = 1;
                break;
            }
            // waiting for the signature
            // Make sure first 6 bytes are also in
            if(NET_PRES_SocketReadIsReady(pHttpCon->socket) < sizeof(MPFS_SIGNATURE) - 1 )
            {
                return TCPIP_HTTP_IO_RES_NEED_DATA;
            }

            // reset the phase, we're done
            pHttpCon->flags.uploadPhase = 0;

            lenA = NET_PRES_SocketRead(pHttpCon->socket, mpfsBuffer, sizeof(MPFS_SIGNATURE) - 1);
            pHttpCon->byteCount -= lenA;
            if(memcmp(mpfsBuffer, (const void*)MPFS_SIGNATURE, sizeof(MPFS_SIGNATURE) - 1) != 0)
            {   // wrong signature
                pHttpCon->httpStatus = TCPIP_HTTP_STAT_UPLOAD_ERROR;
                return TCPIP_HTTP_IO_RES_WAITING;
            }

            // proper file version
            // allocate the buffer size as a multiple of sector size
            pHttpCon->uploadBufferStart = 0;
            mpfsAllocSize = ((MPFS_UPLOAD_WRITE_BUFFER_SIZE + (SYS_FS_MEDIA_SECTOR_SIZE - 1)) / SYS_FS_MEDIA_SECTOR_SIZE) * SYS_FS_MEDIA_SECTOR_SIZE;
            if((pHttpCon->uploadBufferStart = (uint8_t*)http_mallocFnc(mpfsAllocSize)) != 0)
            {
                pHttpCon->uploadBufferEnd = pHttpCon->uploadBufferStart + mpfsAllocSize;
                memcpy(pHttpCon->uploadBufferStart, MPFS_SIGNATURE, sizeof(MPFS_SIGNATURE) - 1);
                pHttpCon->uploadBufferCurr = pHttpCon->uploadBufferStart + sizeof(MPFS_SIGNATURE) - 1;

                SYS_FS_Unmount(MPFS_UPLOAD_MOUNT_PATH);
                pHttpCon->httpStatus = TCPIP_HTTP_STAT_UPLOAD_WRITE;
                return TCPIP_HTTP_IO_RES_WAITING;
            }

            // memory allocation failed
            pHttpCon->flags.uploadMemError = 1; // signal out of memory
            _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_UPLOAD_ALLOC_ERROR, pHttpCon->fileName);
            pHttpCon->httpStatus = TCPIP_HTTP_STAT_UPLOAD_ERROR;
            return TCPIP_HTTP_IO_RES_WAITING;

        case TCPIP_HTTP_STAT_UPLOAD_WRITE:
            lenA = NET_PRES_SocketReadIsReady(pHttpCon->socket);
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
                return TCPIP_HTTP_IO_RES_WAITING;
            }

            lenB = NET_PRES_SocketRead(pHttpCon->socket, pHttpCon->uploadBufferCurr, lenA);
            pHttpCon->uploadBufferCurr += lenB;
            pHttpCon->byteCount -= lenB;

            // check that upload buffer is full
            if(pHttpCon->uploadBufferCurr != pHttpCon->uploadBufferEnd && pHttpCon->byteCount != 0)
            {
                return TCPIP_HTTP_IO_RES_WAITING;
            }

            nSectors = ((pHttpCon->uploadBufferCurr - pHttpCon->uploadBufferStart) + SYS_FS_MEDIA_SECTOR_SIZE - 1) / (SYS_FS_MEDIA_SECTOR_SIZE);

            if(!_HTTP_DbgKillFlashWrite())
            {   // try to perform the write
                pHttpCon->uploadBuffHandle = SYS_FS_MEDIA_MANAGER_SectorWrite(MPFS_UPLOAD_DISK_NO, pHttpCon->uploadSectNo , pHttpCon->uploadBufferStart,
                        nSectors);
                if(pHttpCon->uploadBuffHandle == SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID)
                {
                    pHttpCon->httpStatus = TCPIP_HTTP_STAT_UPLOAD_ERROR;
                    _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FS_WRITE_ERROR, pHttpCon->fileName);
                    return TCPIP_HTTP_IO_RES_WAITING;
                }
            }
            else
            {   // advance without performing the write
                pHttpCon->uploadBuffHandle = SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID;
            }

            // everything fine
            pHttpCon->uploadSectNo += nSectors;
            pHttpCon->httpStatus = TCPIP_HTTP_STAT_UPLOAD_WRITE_WAIT;
            return TCPIP_HTTP_IO_RES_WAITING;

        case TCPIP_HTTP_STAT_UPLOAD_WRITE_WAIT:
            if(pHttpCon->uploadBuffHandle != SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID)
            {
                if(SYS_FS_MEDIA_MANAGER_CommandStatusGet(MPFS_UPLOAD_DISK_NO, pHttpCon->uploadBuffHandle) != SYS_FS_MEDIA_COMMAND_COMPLETED)
                {
                    return TCPIP_HTTP_IO_RES_WAITING;
                }
            }

            pHttpCon->uploadBufferCurr = pHttpCon->uploadBufferStart;

            if(pHttpCon->byteCount == 0u)
            {   // we're done
                http_freeFnc(pHttpCon->uploadBufferStart);
                pHttpCon->uploadBufferStart = 0;

                if(SYS_FS_Mount(MPFS_UPLOAD_NVM_VOL, MPFS_UPLOAD_MOUNT_PATH, MPFS2, 0, NULL)  != SYS_FS_RES_FAILURE)
                {
                    _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FS_UPLOAD_COMPLETE, pHttpCon->fileName);
                    pHttpCon->httpStatus = TCPIP_HTTP_STAT_UPLOAD_OK;
                    return TCPIP_HTTP_IO_RES_DONE;
                }
                else
                {
                    _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FS_MOUNT_ERROR, pHttpCon->fileName);
                    pHttpCon->httpStatus = TCPIP_HTTP_STAT_UPLOAD_ERROR;
                    return TCPIP_HTTP_IO_RES_ERROR;
                }
            }

            // else, more data to come
            pHttpCon->httpStatus = TCPIP_HTTP_STAT_UPLOAD_WRITE;
            return TCPIP_HTTP_IO_RES_WAITING;

        case TCPIP_HTTP_STAT_UPLOAD_ERROR:
            if(pHttpCon->uploadBufferStart != 0)
            {
                http_freeFnc(pHttpCon->uploadBufferStart);
                pHttpCon->uploadBufferStart = 0;
            }
            pHttpCon->byteCount -= NET_PRES_SocketReadIsReady(pHttpCon->socket);
            NET_PRES_SocketDiscard(pHttpCon->socket);
            if(pHttpCon->byteCount < 100u || pHttpCon->byteCount > 0x80000000u)
            {   // If almost all data was read, or if we overflowed, then return
                return TCPIP_HTTP_IO_RES_DONE;
            }
            break;

        default:
            break;
    }

    return TCPIP_HTTP_IO_RES_NEED_DATA;
}

#endif //defined (TCPIP_HTTP_FILE_UPLOAD_ENABLE)

// the default file include dynamic variable HTTP operation
static TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_NET_DefaultIncludeFile(TCPIP_HTTP_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const struct _tag_TCPIP_HTTP_USER_CALLBACK* pCBack)
{
    TCPIP_HTTP_CONN* pHttpCon;
    const char* fName;
    TCPIP_HTTP_DYN_ARG_DCPT* pArgDcpt;
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt = (TCPIP_HTTP_CHUNK_DCPT*)varDcpt->dynContext;

    pHttpCon = (TCPIP_HTTP_CONN*)connHandle;

    // some basic sanity check
    pArgDcpt = varDcpt->dynArgs;
    if(pArgDcpt->argType == TCPIP_HTTP_DYN_ARG_TYPE_STRING)
    {
        fName = pArgDcpt->argStr;
    }
    else
    {
        fName = 0;
    }

    _HTTP_IncludeFile(pHttpCon, pChDcpt, fName);

    // one way or another we're done
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

static TCPIP_HTTP_CHUNK_RES _HTTP_IncludeFile(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, const char* fName)
{
    SYS_FS_HANDLE fp;


    // avoid creating a new chunk for a file that cannot be opened
    const SYS_FS_SHELL_OBJ* fileShell = pHttpCon->connPort->fileShell;
    if(fName == 0 || (fp = (*fileShell->fileOpen)(fileShell, fName, SYS_FS_FILE_OPEN_READ)) == SYS_FS_HANDLE_INVALID)
    {   // File not found, so abort
        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_OPEN_ERROR, fName);
        return TCPIP_HTTP_CHUNK_RES_FILE_ERR;
    }

    _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_OPEN, fName);
    // add valid file for processing
    return _HTTP_AddFileChunk(pHttpCon, fp, fName, pChDcpt);
}


SYS_FS_HANDLE TCPIP_HTTP_ConnectionFileGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->file;
}

uint16_t TCPIP_HTTP_ConnectionPostSmGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->smPost;
}

void TCPIP_HTTP_ConnectionPostSmSet(TCPIP_HTTP_CONN_HANDLE connHandle, uint16_t stat)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    pHttpCon->smPost = stat;
}

uint8_t* TCPIP_HTTP_ConnectionDataBufferGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->httpData;
}

uint16_t TCPIP_HTTP_ConnectionDataBufferSizeGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->connPort->connDataSize;
}

uint32_t TCPIP_HTTP_ConnectionCallbackPosGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->callbackPos;
}

void TCPIP_HTTP_ConnectionCallbackPosSet(TCPIP_HTTP_CONN_HANDLE connHandle, uint32_t pos)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    pHttpCon->callbackPos = pos;
}

void TCPIP_HTTP_ConnectionStatusSet(TCPIP_HTTP_CONN_HANDLE connHandle, TCPIP_HTTP_STATUS stat)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    pHttpCon->httpStatus = stat;
}

void  TCPIP_HTTP_ConnectionRedirectionSet(TCPIP_HTTP_CONN_HANDLE connHandle, TCPIP_HTTP_REDIR_TYPE redirType)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    pHttpCon->httpStatus = TCPIP_HTTP_STAT_REDIRECT;
    pHttpCon->redirType = redirType;
}

TCPIP_HTTP_STATUS TCPIP_HTTP_ConnectionStatusGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->httpStatus;
}



void TCPIP_HTTP_ConnectionHasArgsSet(TCPIP_HTTP_CONN_HANDLE connHandle, uint8_t args)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    pHttpCon->hasArgs = args;
}

uint8_t TCPIP_HTTP_ConnectionHasArgsGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->hasArgs;
}

uint32_t TCPIP_HTTP_ConnectionByteCountGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->byteCount;
}

void TCPIP_HTTP_ConnectionByteCountSet(TCPIP_HTTP_CONN_HANDLE connHandle, uint32_t byteCount)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    pHttpCon->byteCount = byteCount;
}

void TCPIP_HTTP_ConnectionByteCountDec(TCPIP_HTTP_CONN_HANDLE connHandle, uint32_t byteCount)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    pHttpCon->byteCount -= byteCount;
}

NET_PRES_SKT_HANDLE_T TCPIP_HTTP_ConnectionSocketGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->socket;
}

uint8_t TCPIP_HTTP_ConnectionIsAuthorizedGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->isAuthorized;
}

void TCPIP_HTTP_ConnectionIsAuthorizedSet(TCPIP_HTTP_CONN_HANDLE connHandle, uint8_t auth)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    pHttpCon->isAuthorized = auth;
}

void TCPIP_HTTP_ConnectionUserDataSet(TCPIP_HTTP_CONN_HANDLE connHandle, const void* uData)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    pHttpCon->userData = uData;
}

const void* TCPIP_HTTP_ConnectionUserDataGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->userData;
}

size_t TCPIP_HTTP_ConnectionIndexGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return pHttpCon->connIx;
}

TCPIP_HTTP_CONN_HANDLE TCPIP_HTTP_ConnectionHandleGet(size_t instIx, size_t portIx, size_t connIx)
{
    if(instIx < httpInstNo)
    {
        const TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);
        if(portIx < pInstance->nPorts)
        {
            TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt + portIx;
            
            if(connIx < pPort->connNo)
            {
                return pPort->connCtrl + connIx;
            }
        }
    }

    return 0;
}

size_t TCPIP_HTTP_Instance_CountGet(void)
{
    return httpInstNo; 
}

size_t TCPIP_HTTP_Instance_PortCountGet(size_t instIx)
{
    if(instIx < httpInstNo)
    {
        const TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);
        return pInstance->nPorts;
    }

    return 0;
}


ssize_t TCPIP_HTTP_ActiveConnectionCountGet(size_t instIx, size_t portIx, size_t* pOpenCount)
{
    TCPIP_HTTP_CONN* pHttpCon;
    size_t connIx;
    ssize_t activeCount;
    size_t openCount;

    activeCount = -1;
    openCount = 0;

    if(instIx < httpInstNo)
    {
        const TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);

        if(portIx < pInstance->nPorts)
        {
            activeCount = 0;
            TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt + portIx;

            pHttpCon = pPort->connCtrl + 0;
            for(connIx = 0; connIx < pPort->connNo; connIx++, pHttpCon++)
            {
                if(pHttpCon->socket != NET_PRES_INVALID_SOCKET)
                {
                    openCount++;
                    if(NET_PRES_SocketIsConnected(pHttpCon->socket))
                    {
                        activeCount++;
                    }
                }
            }
        }
    }

    if(pOpenCount)
    {
        *pOpenCount = openCount;
    }

    return activeCount;
}

ssize_t  TCPIP_HTTP_PortRules_CountGet(size_t instIx, ssize_t portIx)
{
    size_t ruleIx;

    if(instIx < httpInstNo)
    {
        const TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);
        if(portIx < 0)
        {
            return pInstance->nRules;
        }

        // count rules for portIx
        if(portIx < pInstance->nPorts)
        {
            TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt + portIx;
            ssize_t nRules = 0;
            _TCPIP_HTTP_ACC_RULE* pRule = pInstance->rules;
            for(ruleIx = 0; ruleIx < pInstance->nRules; ruleIx++)
            {
                if(pRule->accRule.inPort == pPort->listenPort)
                {
                    nRules++;
                }
                // next rule
                pRule = (_TCPIP_HTTP_ACC_RULE*)((uint8_t*)pRule + pRule->accRule.ruleSize);
            }
            return nRules;
        }
    }

    return -1;
}

bool TCPIP_HTTP_PortRuleGet(size_t instIx, ssize_t portIx, size_t ruleIx, TCPIP_HTTP_ACCESS_RULE* pAccRule)
{
#if (_TCPIP_HTTP_USE_ACCESS_RULES != 0)
    size_t rIx;
    _TCPIP_HTTP_ACC_RULE* pRule, *foundRule;
    if(instIx < httpInstNo)
    {
        const TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);

        if(portIx < 0)
        {   // all rules are counted
            if(ruleIx < pInstance->nRules)
            {
                if(pAccRule)
                {   // traverse the rules
                    pRule = pInstance->rules;
                    for(rIx = 0; rIx < ruleIx; rIx++)
                    {
                        pRule = (_TCPIP_HTTP_ACC_RULE*)((uint8_t*)pRule + pRule->accRule.ruleSize);
                    }

                    *pAccRule = pRule->accRule;
                }
                return true;
            }
        }
        else if(portIx < pInstance->nPorts)
        {
            TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt + portIx;
            // get to the selected ruleIx

            int portRuleNo = 0;
            foundRule = 0;
            pRule = pInstance->rules;
            for(rIx = 0; rIx < pInstance->nRules; rIx++)
            {
                if(pRule->accRule.inPort == pPort->listenPort)
                {
                    if(portRuleNo == ruleIx)
                    {   // found the rule
                        foundRule = pRule;
                        break;
                    }
                    portRuleNo++;
                }
                // next rule
                pRule = (_TCPIP_HTTP_ACC_RULE*)((uint8_t*)pRule + pRule->accRule.ruleSize);
            }

            if(foundRule)
            {
                if(pAccRule)
                {   // traverse the rules
                    *pAccRule = foundRule->accRule;
                }
                return true;
            }
        }
    }

    return false;
#else
    return false;
#endif  //  // (_TCPIP_HTTP_USE_ACCESS_RULES != 0)
}

uint16_t TCPIP_HTTP_ConnectionStringFind(TCPIP_HTTP_CONN_HANDLE connHandle, const char* str, uint16_t startOffs, uint16_t searchLen)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return (uint16_t)_HTTP_ConnectionStringFind(pHttpCon, str, startOffs, searchLen);
}


bool TCPIP_HTTP_DynamicFileInclude(TCPIP_HTTP_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const char* fileName)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt = (TCPIP_HTTP_CHUNK_DCPT*)varDcpt->dynContext;
    TCPIP_HTTP_CHUNK_RES chunkRes = _HTTP_IncludeFile(pHttpCon, pChDcpt, fileName);

    return chunkRes == TCPIP_HTTP_CHUNK_RES_OK; 
}


uint16_t TCPIP_HTTP_ConnectionFlush(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return NET_PRES_SocketFlush(pHttpCon->socket);
}

uint16_t TCPIP_HTTP_ConnectionReadIsReady(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return NET_PRES_SocketReadIsReady(pHttpCon->socket);
}

uint16_t TCPIP_HTTP_ConnectionRead(TCPIP_HTTP_CONN_HANDLE connHandle, void * buffer, uint16_t size)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return NET_PRES_SocketRead(pHttpCon->socket, buffer, size);
}

uint16_t TCPIP_HTTP_ConnectionPeek(TCPIP_HTTP_CONN_HANDLE connHandle,  void * buffer, uint16_t size)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return NET_PRES_SocketPeek(pHttpCon->socket, buffer, size);
}

uint16_t TCPIP_HTTP_ConnectionDiscard(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;
    return NET_PRES_SocketDiscard(pHttpCon->socket);
}

uint16_t TCPIP_HTTP_ConnectionReadBufferSize(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    uint16_t sktRxSize;
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;

    if(NET_PRES_SocketOptionsGet(pHttpCon->socket, TCP_OPTION_RX_BUFF, &sktRxSize))
    {
        return sktRxSize;
    }

    return 0;
}

TCPIP_NET_HANDLE TCPIP_HTTP_ConnectionNetHandle(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCP_SOCKET_INFO sktInfo;
    TCPIP_HTTP_CONN* pHttpCon = (TCPIP_HTTP_CONN*)connHandle;

    if(NET_PRES_SocketInfoGet(pHttpCon->socket, &sktInfo))
    {
        return sktInfo.hNet;
    }

    return 0;
}


static uint16_t _HTTP_SktFifoRxFree(NET_PRES_SKT_HANDLE_T skt)
{
    uint16_t sktRxSize;
    uint16_t sktReadySize;

    if(NET_PRES_SocketOptionsGet(skt, TCP_OPTION_RX_BUFF, &sktRxSize))
    {
        sktReadySize = NET_PRES_SocketReadIsReady(skt);
        return sktRxSize - sktReadySize;
    }

    return 0;
}


// tries to output the data to the transport socket
// it checks first for available space and starts the operation
// only if there's enough space
// checkLen is the size to check for; could be different than dataLen
// if data == 0; then nothing is written
static bool _HTTP_DataTryOutput(TCPIP_HTTP_CONN* pHttpCon, const char* data, uint16_t dataLen, uint16_t checkLen)
{
    uint16_t socketSpace;

    if(checkLen == 0 || checkLen < dataLen)
    {
        checkLen = dataLen;
    }
    socketSpace = NET_PRES_SocketWriteIsReady(pHttpCon->socket, checkLen, 0);
    if(socketSpace >= checkLen)
    {
        if(data)
        {
            NET_PRES_SocketWrite(pHttpCon->socket, (const uint8_t*)data, dataLen);
        }
        return true;
    }

    return false;
}


// Note that the search will fail if there's more data in the TCP socket than could be read at once.
// returns a 32 bit word:
//      - hi 16 bits hold the peeked at data, i.e. the number of bytes to safely remove, if needed
//      - low 16 bits is 0xffff if not found or offset where the string was found
static uint32_t _HTTP_ConnectionStringFind(TCPIP_HTTP_CONN* pHttpCon, const char* str, uint16_t startOffs, uint16_t searchLen)
{
    uint16_t    peekOffs, peekReqLen, peekSize, avlblBytes;
    char*   queryStr;

    uint16_t peekBufferSize = pHttpCon->connInstance->peekBufferSize;
    char* srchBuff = (char*)http_mallocFnc(peekBufferSize + 1);
    if(srchBuff == 0)
    {
        char eventBuff[16];
        sprintf(eventBuff, "%d", peekBufferSize + 1);
        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_PEEK_ALLOC_BUFFER_ERROR, eventBuff);
        return 0xffff;
    }

    const char* findStr = (const char*)str;

    size_t findLen = strlen(findStr);

    // peek sanity check
    if((avlblBytes = NET_PRES_SocketReadIsReady(pHttpCon->socket)) > peekBufferSize)
    {   // peek buffer too small!
        sprintf(srchBuff, "%d", (int)(avlblBytes - (peekBufferSize)));
        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_PEEK_BUFFER_SIZE_EXCEEDED, srchBuff);
    }

    uint16_t retCode = 0xffff;
    peekSize = 0;
    // sanity check
    if(findLen < peekBufferSize)
    {   // make sure enough room to find such string
        if(searchLen == 0 || searchLen >= findLen)
        {
            peekOffs = startOffs;
            peekReqLen = searchLen == 0 ? peekBufferSize : searchLen;
            peekSize =  NET_PRES_SocketPeek(pHttpCon->socket, srchBuff, peekReqLen);
            if(peekSize >= findLen)
            {   // enough data present
                srchBuff[peekSize] = 0;     // end string properly
                queryStr = strstr(srchBuff, findStr);
                if(queryStr != 0)
                {
                    retCode = peekOffs + queryStr - srchBuff;
                }
            }
        }
    }


    http_freeFnc(srchBuff);
    return ((uint32_t)peekSize << 16) | retCode;
}

static uint16_t _HTTP_ConnectionCharFind(TCPIP_HTTP_CONN* pHttpCon, uint8_t cFind, uint16_t wStart, uint16_t wSearchLen)
{
    char srchBuff[2];
    srchBuff[0] = cFind;
    srchBuff[1] = 0;

    return (uint16_t)_HTTP_ConnectionStringFind(pHttpCon, srchBuff, wStart, wSearchLen);
}

TCPIP_HTTP_USER_HANDLE TCPIP_HTTP_UserHandlerRegister(size_t instIx, const TCPIP_HTTP_USER_CALLBACK* userCallback)
{
    if(instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);

        if(pInstance->portDcpt)
        {   // we're up and running; allow only one registration for now
            if(userCallback && (pInstance->instanceFlags & TCPIP_HTTP_EXT_FLAG_USR_REG) == 0)
            {   // OK, empty
                pInstance->httpRegistry = *userCallback;
                pInstance->instanceFlags |= TCPIP_HTTP_EXT_FLAG_USR_REG;
                return (TCPIP_HTTP_USER_HANDLE)&pInstance->httpRegistry;
            }
        }
    }

    return 0;

}

bool TCPIP_HTTP_UserHandlerDeregister(size_t instIx, TCPIP_HTTP_USER_HANDLE hHttp)
{
    int  connIx, portIx;

    if(instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);

        if(pInstance->portDcpt)
        {   // we're up and running
            // abort all connections
            if(hHttp == &pInstance->httpRegistry && (pInstance->instanceFlags & TCPIP_HTTP_EXT_FLAG_USR_REG) != 0)
            {

                TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt;
                for(portIx = 0; portIx < pInstance->nPorts; portIx++, pPort++)
                {
                    TCPIP_HTTP_CONN* pHttpCon = pPort->connCtrl + 0;
                    for (connIx = 0; connIx < pPort->connNo; connIx++, pHttpCon++)
                    {
                        if (pHttpCon->socket != NET_PRES_INVALID_SOCKET)
                        {
                            TCP_SOCKET tcp_skt = NET_PRES_SocketGetTransportHandle(pHttpCon->socket);
                            TCPIP_TCP_Abort(tcp_skt, false);
                        }

                        _HTTP_ConnCloseFile(pHttpCon);
                    }
                }

                pInstance->instanceFlags &= ~TCPIP_HTTP_EXT_FLAG_USR_REG;
                memset(&pInstance->httpRegistry, 0, sizeof(pInstance->httpRegistry));
                return true;
            }
        }
    }

    return false;
}

// generates a HTTP chunk of the requested size 
// a HTTP chunk structure:
//      - number of bytes in hex ASCII
//      - optional params - not supported
//      - CRLF
//      --------- chunk data following this chunk header
//
//      - CRLF
// Returns: number of bytes needed/written as the chunk header
//          0 if retry needed
// the output goes directly to the socket, if possible
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
static uint16_t _HTTP_StartHttpChunk(TCPIP_HTTP_CONN* pHttpCon, uint32_t chunkSize)
{
    char chunkHdrBuff[TCPIP_HTTP_CHUNK_HEADER_LEN + 1];     

    uint16_t hdrLen = sprintf(chunkHdrBuff, "%x\r\n", (unsigned int)chunkSize);

    _HTTPAssertCond(hdrLen <= TCPIP_HTTP_CHUNK_HEADER_LEN, __func__, __LINE__);

    if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) == 0)
    {   // write to the socket
        uint16_t avlblBytes;
        avlblBytes = NET_PRES_SocketWriteIsReady(pHttpCon->socket, hdrLen, 0);
        if(avlblBytes < hdrLen)
        {   // not enough space
            return 0;
        }

        // write to output
        avlblBytes = NET_PRES_SocketWrite(pHttpCon->socket, chunkHdrBuff, hdrLen);
        _HTTPAssertCond(avlblBytes == hdrLen, __func__, __LINE__);
        return avlblBytes;
    }
    // else non persistent and fake it
    return hdrLen;
}

// generates the CRLF at the end of the chunk
// Returns: number of bytes written as the chunk trailer
//          0 if retry needed
// the output goes directly to the socket, if possible
static uint16_t _HTTP_EndHttpChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_END_TYPE endType)
{
    const char* endStr;
    if(endType == TCPIP_HTTP_CHUNK_END_CURRENT)
    {
        endStr = TCPIP_HTTP_CRLF;
    }
    else if(endType == TCPIP_HTTP_CHUNK_END_FINAL)
    {
        endStr =  "0" TCPIP_HTTP_CRLF TCPIP_HTTP_CRLF;
    }
    else
    {   // all: current + final
        endStr = TCPIP_HTTP_CRLF "0" TCPIP_HTTP_CRLF TCPIP_HTTP_CRLF;
    }

    uint16_t trailLen = strlen(endStr);

    if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) == 0)
    {
        uint16_t avlblBytes;
        avlblBytes = NET_PRES_SocketWriteIsReady(pHttpCon->socket, trailLen, 0);
        if(avlblBytes < trailLen)
        {   // not enough space
            return 0;
        }

        // write to output
        avlblBytes =  NET_PRES_SocketWrite(pHttpCon->socket, endStr, trailLen);
        _HTTPAssertCond(avlblBytes == trailLen, __func__, __LINE__);
        return avlblBytes;
    }

    // fake it
    return trailLen;
}

#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

// prepends the start HTTP chunk to the buffer
// if buffer is null, it just calculates the size
// returns the size taken by the header
static uint16_t _HTTP_PrependStartHttpChunk(TCPIP_HTTP_CONN* pHttpCon, char* buffer, uint32_t chunkSize)
{
    if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) != 0)
    {   // no chunk header
        return 0;
    }

    char chunkHdrBuff[TCPIP_HTTP_CHUNK_HEADER_LEN + 1];     

    uint16_t hdrLen = sprintf(chunkHdrBuff, "%x\r\n", (unsigned int)chunkSize);

    _HTTPAssertCond(hdrLen <= TCPIP_HTTP_CHUNK_HEADER_LEN, __func__, __LINE__);

    if(buffer)
    {
        memcpy(buffer - hdrLen, chunkHdrBuff, hdrLen);
    }

    return hdrLen;
}

// appends the end chunk to the end of buffer
// if buffer is null, it just calculates the size
// returns the size taken by the trailer
static uint16_t _HTTP_AppendEndHttpChunk(TCPIP_HTTP_CONN* pHttpCon, char* buffer, TCPIP_HTTP_CHUNK_END_TYPE endType)
{
    if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) != 0)
    {   // no chunk trailer
        return 0;
    }

    const char* endStr;
    if(endType == TCPIP_HTTP_CHUNK_END_CURRENT)
    {
        endStr = TCPIP_HTTP_CRLF;
    }
    else if(endType == TCPIP_HTTP_CHUNK_END_FINAL)
    {
        endStr =  "0" TCPIP_HTTP_CRLF TCPIP_HTTP_CRLF;
    }
    else
    {   // all: current + final
        endStr = TCPIP_HTTP_CRLF "0" TCPIP_HTTP_CRLF TCPIP_HTTP_CRLF;
    }

    uint16_t trailLen = strlen(endStr);

    if(buffer)
    {
        memcpy(buffer, endStr, trailLen);
    }

    return trailLen;
}




// adds a file chunk to this connection
// the file handle should be provided
// pOwnDcpt shows the chunk that owns this new file. If 0, then it's a root file.
static TCPIP_HTTP_CHUNK_RES _HTTP_AddFileChunk(TCPIP_HTTP_CONN* pHttpCon, SYS_FS_HANDLE fH, const char* fName, TCPIP_HTTP_CHUNK_DCPT* pOwnDcpt)
{
    TCPIP_HTTP_CHUNK_FLAGS chunkFlags;
    TCPIP_HTTP_CHUNK_RES chunkRes = TCPIP_HTTP_CHUNK_RES_OK;
    TCPIP_HTTP_EVENT_TYPE evType = TCPIP_HTTP_EVENT_NONE;
    const void* evInfo = fName;
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt = 0;

    const SYS_FS_SHELL_OBJ* fileShell = pHttpCon->connPort->fileShell;

    while(true)
    {
        if(fH == SYS_FS_HANDLE_INVALID)
        {   // do nothing for invalid files
            _HTTPAssertCond(false, __func__, __LINE__);
            evType = TCPIP_HTTP_EVENT_FILE_OPEN_ERROR;
            chunkRes = TCPIP_HTTP_CHUNK_RES_FILE_ERR;
            break;
        }
        

        if(TCPIP_Helper_SingleListCount(&pHttpCon->chunkList) >= pHttpCon->connInstance->chunksDepth)
        {   // already at max depth
            evType = TCPIP_HTTP_EVENT_DEPTH_ERROR;
            chunkRes = TCPIP_HTTP_CHUNK_RES_DEPTH_ERR;
            break;
        }

        chunkFlags = (pOwnDcpt == 0) ? (TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE | TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_ROOT) : TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE;
        if(fName != 0)
        {
            SYS_FS_FSTAT fs_attr = {0};
            bool fileGzipped = false;

            if((*fileShell->fileStat)(fileShell, fName, &fs_attr) != SYS_FS_HANDLE_INVALID)
            {
                if (fs_attr.fattrib == SYS_FS_ATTR_ZIP_COMPRESSED)
                {
                    fileGzipped = true;
                }
            }

            if(!fileGzipped && _HTTP_FileTypeIsDynamic(fName))
            {
                if(!_HTTP_DbgKillDynFiles())
                {
                    chunkFlags |= TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_DYN;
                }
            }
        }

        int32_t fileSize =  (*fileShell->fileSize)(fileShell, fH);
        if(fileSize == 0 || fileSize == -1)
        {   // still invalid
            evType = TCPIP_HTTP_EVENT_FILE_SIZE_ERROR;
            chunkRes = TCPIP_HTTP_CHUNK_RES_FILE_ERR;
            break;
        }

        // valid file, try to get a chunk
        pChDcpt = _HTTP_AllocChunk(pHttpCon, chunkFlags, (pOwnDcpt == 0), &evType);

        if(pChDcpt == 0)
        { 
            chunkRes = (evType < 0) ? TCPIP_HTTP_CHUNK_RES_RETRY_ERR : TCPIP_HTTP_CHUNK_RES_WAIT;
            break;
        }


        pChDcpt->pRootDcpt = (pOwnDcpt == 0) ? pChDcpt : pOwnDcpt->pRootDcpt; 
        pChDcpt->fileChDcpt.fSize = fileSize;
        pChDcpt->fileChDcpt.fHandle = fH;
        if(fName != 0)
        {
            char* path = strrchr(fName, TCPIP_HTTP_FILE_PATH_SEP);
            if(path)
            {   // save a truncated version of the file name, no path 
                fName = path + 1;
            }
            strncpy(pChDcpt->chunkFName, fName, sizeof(pChDcpt->chunkFName) - 1);
            pChDcpt->chunkFName[sizeof(pChDcpt->chunkFName) - 1] = 0;
        }
        else
        {
            pChDcpt->chunkFName[0] = 0;
        }

        break;
    }

    if(chunkRes < 0)
    {   // some error occurred
        if(fH != SYS_FS_HANDLE_INVALID)
        {
            (*fileShell->fileClose)(fileShell, fH);
            _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_CLOSE, fName);
        }
        if(pChDcpt != 0)
        {
            pChDcpt->fileChDcpt.fHandle = SYS_FS_HANDLE_INVALID;
            _HTTP_FreeChunk(pHttpCon, pChDcpt);
        }
    }
    else if(chunkRes == TCPIP_HTTP_CHUNK_RES_OK) 
    {
        _HTTP_FileDbgCreate(pChDcpt->chunkFName, pChDcpt->fileChDcpt.fSize, (chunkFlags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_DYN) != 0 ? "dyn" : "bin", pHttpCon->connIx);
    }

    if(evType != TCPIP_HTTP_EVENT_NONE)
    {
        _HTTP_Report_ConnectionEvent(pHttpCon, evType, evInfo);
    }

    return chunkRes;
}


// allocates (extracts from the pool) a new chunk descriptor of the specified type
// and adds it to the head of the HTTP connection!
// the flags carry the type info!
// sets the TCPIP_HTTP_CHUNK_FLAG_BEG_CHUNK flag
// if appendTail set, it appends to the tail
// else inserts at front
// it also allocates a fileBuffer, if needed
// returns 0 if failed and updates the pEvType
static TCPIP_HTTP_CHUNK_DCPT* _HTTP_AllocChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_FLAGS flags, bool appendTail, TCPIP_HTTP_EVENT_TYPE* pEvType)
{
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt;
    TCPIP_HTTP_FILE_BUFF_DCPT*  fileBuffDcpt = 0;

    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 
    SINGLE_LIST* httpFileBuffers = &pInstance->fileBuffers;
    if((flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE) != 0)
    {   // grab a file buffer
        fileBuffDcpt = (TCPIP_HTTP_FILE_BUFF_DCPT*)TCPIP_Helper_SingleListHeadRemove(httpFileBuffers);
        if(fileBuffDcpt == 0)
        {   // failed
            pHttpCon->fileBufferPoolEmpty++;
            if(++pHttpCon->fileBufferRetry > pInstance->fileBufferRetries)
            {
                *pEvType = TCPIP_HTTP_EVENT_FILE_BUFFER_POOL_ERROR;
            }
            else
            {
                *pEvType = TCPIP_HTTP_EVENT_FILE_BUFFER_POOL_EMPTY;
            }
            return 0;
        }
        else
        {   // success
            pHttpCon->fileBufferRetry = 0;
        }
    }

    pChDcpt = (TCPIP_HTTP_CHUNK_DCPT*)TCPIP_Helper_SingleListHeadRemove(&pInstance->chunkPool);

    if(pChDcpt != 0)
    {   // success
        int currDepth;

        pHttpCon->chunkPoolRetry = 0;

        memset(pChDcpt, 0, sizeof(*pChDcpt));
        // set the chunk connection
        pChDcpt->pOwnerCon = pHttpCon;

        if((flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE) != 0)
        {
            pChDcpt->fileChDcpt.fHandle = SYS_FS_HANDLE_INVALID;
            pChDcpt->fileChDcpt.fileBuffDcpt = fileBuffDcpt;
        }
        else
        {   // data chunk
            TCPIP_Helper_SingleListInitialize (&pChDcpt->dynChDcpt.dynBuffList);
        }
        pChDcpt->flags = flags | TCPIP_HTTP_CHUNK_FLAG_BEG_CHUNK;
        pChDcpt->status = TCPIP_HTTP_CHUNK_STATE_BEG;

        if(appendTail)
        {
            TCPIP_Helper_SingleListTailAdd(&pHttpCon->chunkList, (SGL_LIST_NODE*)pChDcpt);
        }
        else
        {
            TCPIP_Helper_SingleListHeadAdd(&pHttpCon->chunkList, (SGL_LIST_NODE*)pChDcpt);
        }

        currDepth = TCPIP_Helper_SingleListCount(&pHttpCon->chunkList);
        if(currDepth > pInstance->maxRecurseDepth)
        {
            pInstance->maxRecurseDepth = currDepth;
        }

    }
    else
    {   // failed
        if(fileBuffDcpt != 0)
        {   // return it to pool
            TCPIP_Helper_SingleListTailAdd(httpFileBuffers, (SGL_LIST_NODE*)fileBuffDcpt);
        }
        pHttpCon->chunkPoolEmpty++;

        if(++pHttpCon->chunkPoolRetry > pInstance->chunkPoolRetries)
        {
            *pEvType = TCPIP_HTTP_EVENT_CHUNK_POOL_ERROR;
        }
        else
        {
            *pEvType = TCPIP_HTTP_EVENT_CHUNK_POOL_EMPTY;
        }

    }

    return pChDcpt;
}

// frees (re-inserts into the pool) a chunk after it's done
// the chunk should be at the head of the chunkList!
static void _HTTP_FreeChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    _HTTPAssertCond(pChDcpt == (TCPIP_HTTP_CHUNK_DCPT*)pHttpCon->chunkList.head, __func__, __LINE__);

    TCPIP_HTTP_CHUNK_DCPT* pHead = (TCPIP_HTTP_CHUNK_DCPT*)TCPIP_Helper_SingleListHeadRemove(&pHttpCon->chunkList);
    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 

    if((pHead->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE) != 0)
    {
        if(pHead->fileChDcpt.fHandle != SYS_FS_HANDLE_INVALID)
        {
            const SYS_FS_SHELL_OBJ* fileShell = pHttpCon->connPort->fileShell;
            (*fileShell->fileClose)(fileShell, pHead->fileChDcpt.fHandle);
            _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_CLOSE, pHead->chunkFName);
        }

        if(pHead->fileChDcpt.fileBuffDcpt)
        {
            TCPIP_Helper_SingleListTailAdd(&pInstance->fileBuffers, (SGL_LIST_NODE*)pHead->fileChDcpt.fileBuffDcpt);
        }
    }
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    else if((pHead->flags & (TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE | TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI)) == 0)
    {
        if(pHead->dynChDcpt.pDynAllocDcpt != 0)
        {   // dynamic variable chunk with allocated dyn var user space
            http_freeFnc(pHead->dynChDcpt.pDynAllocDcpt);
        }
        // check if there are dynamic buffers that are not freed
        TCPIP_HTTP_DYNVAR_BUFF_DCPT* pDynDcpt;
        while((pDynDcpt = (TCPIP_HTTP_DYNVAR_BUFF_DCPT*)TCPIP_Helper_SingleListHeadRemove(&pChDcpt->dynChDcpt.dynBuffList)) != 0)
        {
            if((pDynDcpt->dynFlags & TCPIP_HTTP_DYNVAR_BUFF_FLAG_ACK) != 0)
            {   // needs acknowlegment
                if(pInstance->httpRegistry.dynamicAck != 0)
                {
                    (*pInstance->httpRegistry.dynamicAck)(pHttpCon, pDynDcpt->dynBuffer, &pInstance->httpRegistry);
                }
            }
            _HTTP_ReleaseDynBuffDescriptor(pInstance, pDynDcpt);
        }
    }
#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)
#if (TCPIP_HTTP_SSI_PROCESS != 0)
    else if((pHead->flags & (TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE | TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI)) == TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI)
    {
        if(pHead->ssiChDcpt.pAllocAttribs != 0)
        {   // SSI chunk with allocated processing space
            http_freeFnc(pHead->ssiChDcpt.pAllocAttribs);
        }
    }
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)

    TCPIP_Helper_SingleListTailAdd(&pInstance->chunkPool, (SGL_LIST_NODE*)pHead);
}

static bool _HTTP_FileTypeIsDynamic(const char* fName)
{
    if(fName)
    {
        char* ext = strchr(fName, TCPIP_HTTP_FILE_EXT_SEP);
        if(ext)
        {
            int ix;
            ext++;
            const char** dynExt = httpDynFileExtensions;
            for(ix = 0; ix < sizeof(httpDynFileExtensions)/sizeof(*httpDynFileExtensions); ix++, dynExt++)
            {
                if(strcmp(ext, *dynExt) == 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

static TCPIP_HTTP_CHUNK_RES _HTTP_ProcessChunks(TCPIP_HTTP_CONN* pHttpCon)
{
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt;
    TCPIP_HTTP_CHUNK_RES chunkRes;

    pChDcpt = (TCPIP_HTTP_CHUNK_DCPT*)pHttpCon->chunkList.head;

    if(pChDcpt == 0)
    {   // empty; nothing to do
        return TCPIP_HTTP_CHUNK_RES_DONE;
    }

    // process list; could become empty!
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    if((pChDcpt->flags & (TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE | TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI)) == 0)
    {
        chunkRes = _HTTP_ProcessDynVarChunk(pHttpCon, pChDcpt);
    }
    else
#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)
#if (TCPIP_HTTP_SSI_PROCESS != 0)
    if((pChDcpt->flags & (TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE | TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI)) == TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI)
    {
        chunkRes = _HTTP_ProcessSSIChunk(pHttpCon, pChDcpt);
    }
    else
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)
    {
        chunkRes = _HTTP_ProcessFileChunk(pHttpCon, pChDcpt);
    }

    // ignore errors and, as long as there is something to do, keep going
    if(chunkRes != TCPIP_HTTP_CHUNK_RES_WAIT)
    {   // no break is needed
        chunkRes = TCPIP_Helper_SingleListIsEmpty(&pHttpCon->chunkList) ? TCPIP_HTTP_CHUNK_RES_DONE : TCPIP_HTTP_CHUNK_RES_OK;
    }

    return chunkRes;
}

//
// Processes a chunk for a dynamic variable or binary file
static TCPIP_HTTP_CHUNK_RES _HTTP_ProcessFileChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    size_t fileBytes, fileReadBytes;
    char* dynStart, *endLine;
    uint16_t hdrBytes;
    uint16_t outSize;
    uint16_t chunkBufferSize;
    bool    endOfFile;
    bool    prependHeader, appendTrailer;
    bool    procError;
    size_t  headerLen;
    char* fileBuffer;
    char* chunkBuffer;
    TCPIP_HTTP_CHUNK_END_TYPE trailType;


    chunkBuffer = pChDcpt->fileChDcpt.fileBuffDcpt->fileBuffer;
    chunkBufferSize = pChDcpt->fileChDcpt.fileBuffDcpt->fileBufferSize;
    TCPIP_HTTP_PORT_DCPT* pPort = pHttpCon->connPort;

    while(true)
    {  
        // check if there's any transmission pending
        outSize = pChDcpt->fileChDcpt.chunkEnd - pChDcpt->fileChDcpt.chunkOffset;
        if(outSize)
        {   // data pending in the chunk buffer; send it out
            outSize = NET_PRES_SocketWrite(pHttpCon->socket, chunkBuffer + pChDcpt->fileChDcpt.chunkOffset, outSize);
            // global indicator that something went out of this file
            pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_OUT_DATA; 
            
            if( (pChDcpt->fileChDcpt.chunkOffset += outSize) != pChDcpt->fileChDcpt.chunkEnd)
            {   // more data; wait some more
                return TCPIP_HTTP_CHUNK_RES_WAIT;
            }
        }

        // chunk buffer empty and ready for another line
        if(pChDcpt->status == TCPIP_HTTP_CHUNK_STATE_END)
        {   // we're done
            break;
        }

        procError = false;
        // if we have a dynStart process it
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0)
        if(pChDcpt->fileChDcpt.dynStart)
        {   // start the dynamic variable processing
            TCPIP_HTTP_CHUNK_RES dynRes = _HTTP_AddDynChunk(pHttpCon, pChDcpt);
            if(dynRes == TCPIP_HTTP_CHUNK_RES_WAIT)
            {   // wait for resources...
                return TCPIP_HTTP_CHUNK_RES_WAIT;
            }

            pChDcpt->fileChDcpt.dynStart = 0;
            if(dynRes == TCPIP_HTTP_CHUNK_RES_OK)
            {   // it went through and it needs processing
                return TCPIP_HTTP_CHUNK_RES_OK;
            }
            else
            {   // dynRes < 0: some error occurred; abort processing this chunk
                pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_ERROR | TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_PARSE_ERROR;
                procError = true;
            }
        }
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0)
        // or a binary file that doesn't have dynVars
        
        fileBuffer = chunkBuffer + TCPIP_HTTP_CHUNK_HEADER_LEN; // point to data
        fileReadBytes = 0;
        fileBytes = 0;
        if(!procError)
        {
            // check if there's data left in file
            if((fileBytes = pChDcpt->fileChDcpt.fSize - pChDcpt->fileChDcpt.fOffset) != 0)
            {   // more data to read
                if(fileBytes > chunkBufferSize)
                {
                    fileBytes = chunkBufferSize;
                }

                fileReadBytes = (*pPort->fileShell->fileRead)(pPort->fileShell, pChDcpt->fileChDcpt.fHandle, fileBuffer, fileBytes);

                if(fileReadBytes != fileBytes)
                {   // one chunk at a time. can abort if error because no new chunk was written!
                    pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_ERROR;
                }
                else if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_DYN) != 0)
                {   // check for dynamic variables
                    endLine = _HTTP_ProcessFileLine(pChDcpt, fileBuffer, fileBytes, &dynStart, (pChDcpt->fileChDcpt.fOffset + fileBytes) == pChDcpt->fileChDcpt.fSize);
                    if(endLine == 0)
                    {   // cannot fit one line in our buffer
                        pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_ERROR | TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_PARSE_ERROR;
                    }
                    else
                    {
                        fileBytes = endLine - fileBuffer;
                        pChDcpt->fileChDcpt.dynStart = dynStart;
                    }
                }
            }
        }

        endOfFile = false;
        if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_ERROR) != 0)
        {
            if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_DYN) != 0)
            {   // dynamic file: abort if error
                fileBytes = 0;
                fileReadBytes = 0;
                endOfFile = true;
            }
            else
            {   // for a binary file error send the whole of the file, because of the pre-calculated chunk size!
                fileReadBytes = fileBytes;
            }
        }

        if(!endOfFile)
        {
            endOfFile = (pChDcpt->fileChDcpt.fOffset += fileBytes) == pChDcpt->fileChDcpt.fSize;
        }

        // need header?
        prependHeader = false;
        headerLen = 0;
        if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_DYN) != 0)
        {   // for a dynamic file we insert the header for every chunk
            if(fileBytes != 0 || (pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_ERROR) != 0)
            {
                prependHeader = true;
                headerLen = fileBytes;
            }
        }
        else if(pChDcpt->status == TCPIP_HTTP_CHUNK_STATE_BEG)
        {   // binary file: if we just started, the start chunk header needs to be inserted
            prependHeader = true;
            headerLen = pChDcpt->fileChDcpt.fSize;
            pChDcpt->status = TCPIP_HTTP_CHUNK_STATE_DATA;
        }

        // need trailer?
        appendTrailer = false;
        trailType = TCPIP_HTTP_CHUNK_END_NONE;
        if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_DYN) != 0)
        {   // for a dynamic file we end each chunk 
            if(fileBytes != 0 || (pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_ERROR) != 0)
            {   // transmit smth or error
                appendTrailer = true;
                trailType = TCPIP_HTTP_CHUNK_END_CURRENT;
            }

            if(endOfFile && (pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_ROOT) != 0)
            {   // EOF for a root file
                if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_OUT_DATA) != 0 || fileBytes != 0)
                {   // file sent/sends out some data
                    appendTrailer = true;
                    trailType += TCPIP_HTTP_CHUNK_END_FINAL;
                }
            }
        }
        else if(endOfFile)
        {   // a binary file sends the end of chunk only when endOfFile
            appendTrailer = true;
            trailType = TCPIP_HTTP_CHUNK_END_CURRENT;
            if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_ROOT) != 0)
            {
                trailType += TCPIP_HTTP_CHUNK_END_FINAL;
            }
        }
        
        // construct the chunk
        // header
        if(prependHeader != 0)
        {   // output the chunk data
            hdrBytes = _HTTP_PrependStartHttpChunk(pHttpCon, fileBuffer, headerLen);
        }
        else
        {
            hdrBytes = 0;
        }

        // data already in there
        pChDcpt->fileChDcpt.chunkOffset = (fileBuffer - hdrBytes) - chunkBuffer;
        pChDcpt->fileChDcpt.chunkEnd = pChDcpt->fileChDcpt.chunkOffset + hdrBytes + fileBytes;

        // trailer
        if(appendTrailer)
        {
            pChDcpt->fileChDcpt.chunkEnd += _HTTP_AppendEndHttpChunk(pHttpCon, chunkBuffer + pChDcpt->fileChDcpt.chunkEnd, trailType);
        }

        if(endOfFile)
        {
            pChDcpt->status = TCPIP_HTTP_CHUNK_STATE_END;
        }

        // finally update the file offset
        if(fileReadBytes != fileBytes)
        {   // readjust if we read too much
            (*pPort->fileShell->fileSeek)(pPort->fileShell, pChDcpt->fileChDcpt.fHandle, -(fileReadBytes - fileBytes), SYS_FS_SEEK_CUR);
        }

        // continue: either done, or send out data and do more, or process the dynStart
    }


    // once we're here the file is done

    if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_ERROR) != 0)
    {
        _HTTP_Report_ConnectionEvent(pHttpCon, (pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_PARSE_ERROR) != 0 ? TCPIP_HTTP_EVENT_FILE_PARSE_ERROR : TCPIP_HTTP_EVENT_FILE_READ_ERROR, pChDcpt->chunkFName);
    }

    // done with this file
    _HTTP_FileDbgProcess(pChDcpt->chunkFName, pChDcpt->fileChDcpt.fSize, ((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_DYN) != 0) ? "dyn" : "bin", pHttpCon->connIx);
    _HTTP_FreeChunk(pHttpCon, pChDcpt);

    return (pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_ERROR) != 0 ? TCPIP_HTTP_CHUNK_RES_FILE_ERR : TCPIP_HTTP_CHUNK_RES_DONE;
        
}

// returns a pointer within the current buffer just past the end of line
// this pointer reflects the characters that can be processed within this line
// returns 0 if error, line cannot be processed
// the pDynStart is updated with where a dynamic variable starts
// or 0 if no dynamic variable found
// Ignores dynamic variables withing key fields like <code>...</code>
//
// we always read a full fileBuffer if possible (i.e. more than available space in the transport socket)
// because we want to work on line boundaries and a file line should always be <= TCPIP_HTTP_FILE_PROCESS_BUFFER_SIZE! 
// isEof signals the end of file, so proper end of line can be ignored
static char* _HTTP_ProcessFileLine(TCPIP_HTTP_CHUNK_DCPT* pChDcpt, char* lineBuffer, size_t buffLen, char** pDynStart, bool isEof)
{
    char *endPattern, *startPattern;
    char *dynStart, *dynEnd, *endLine;
    char* searchStart = lineBuffer;

    *pDynStart = 0;

    lineBuffer[buffLen] = 0;
    endLine = strrchr(lineBuffer, '\n');
    if(endLine == 0)
    {   // no proper end of line detected
        if(!isEof)
        {   // error: cannot fit one line in our buffer
            return 0;
        }
        // acceptable at the EOF
        endLine = lineBuffer + buffLen - 1;
    }

    endLine++;
    if(_HTTP_DbgKillDynParser())
    {
        return endLine;
    }

    while(true)
    {
        if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_SKIP) != 0)
        {
            endPattern = strstr(searchStart, TCPIP_HTTP_PARSE_IGNORE_END);
            if(endPattern == 0)
            {
                break;
            }

            // ending the ignore mode
            pChDcpt->flags &= ~TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_SKIP;
            searchStart = endPattern + strlen(TCPIP_HTTP_PARSE_IGNORE_END);
            continue;
        }

        // check for start of dynamic variables or other internal processing
        dynStart = _HTTP_FileLineParse(pChDcpt, searchStart, &dynEnd, true);
        if(dynStart == 0)
        {   // not found dynamic variable
            break;
        }

        // make sure the dyn var is not within the ignore pattern
        startPattern = strstr(searchStart, TCPIP_HTTP_PARSE_IGNORE_BEGIN);
        if(startPattern == 0 || (dynStart < startPattern && dynEnd <= startPattern))
        {
            *pDynStart = dynStart;
            endLine = dynStart;
            break;
        }

        // starting the ignore mode
        pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE_SKIP;
        searchStart = startPattern + strlen(TCPIP_HTTP_PARSE_IGNORE_BEGIN);

    }

    return endLine;
}

// parses the lineBuff for a valid dynamic variable or other command that needs to be processed by the server
// - lineBuff should be properly ended with \0
// - returns where the processing should start
//  or 0 if not found or some parsing error
// - pEndProcess stores where it ends 
// - if verifyOnly is true, the position of the front delim is returned and the buffer is not changed
// - else the position past the delim is returned and zeroes are stored in the buffer where the variable ends 
static char* _HTTP_FileLineParse(TCPIP_HTTP_CHUNK_DCPT* pChDcpt, char* lineBuff, char** pEndProcess, bool verifyOnly)
{
    char    *procStart = 0;
    char    *dynStart = 0;
    char    *procEnd, *dynEnd;  
#if (TCPIP_HTTP_SSI_PROCESS != 0)
    procStart = _HTTP_SSILineParse(lineBuff, &procEnd, verifyOnly);
#else
    procStart = 0;
    procEnd = 0;
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)

    dynEnd = 0;
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    dynStart = _HTTP_DynVarParse(lineBuff, &dynEnd, verifyOnly);
#else
    dynStart = 0;
#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

    if(procStart != 0 && (dynStart == 0 || procStart < dynStart))
    {
        pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI;
        *pEndProcess = procEnd;
        return procStart;
    }

    pChDcpt->flags &= ~TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI;
    *pEndProcess = dynEnd;
    return dynStart;
}

// - parses the dynVarBuff for a valid dynamicVariable: "delim\+ .* delim\+"
// - dynVarBuff should be properly ended with \0
// - returns where the dynamic variable name starts
//  or 0 if not found or some parsing error
// - pEndDyn stores where it ends 
// - if verifyOnly is true, the position of the front delim is returned and the buffer is not changed
// - else the position past the delim is returned and zeroes are stored in the buffer where the variable ends 
//
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
static char* _HTTP_DynVarParse(char* dynVarBuff, char** pEndDyn, bool verifyOnly)
{
    char *dynStart, *dynEnd;
    int nDelims;


    // check for start of dynamic variables
    dynStart = strchr(dynVarBuff, TCPIP_HTTP_DYNVAR_DELIM);
    if(dynStart == 0)
    {   // not found dynamic variable
        return 0;
    }

    // eat up multiple front delimiters
    nDelims = 0;
    dynEnd = dynStart;
    while(*dynEnd != 0 && *dynEnd == TCPIP_HTTP_DYNVAR_DELIM)
    {
        nDelims++;
        dynEnd++;
    }
    if(verifyOnly == false)
    {   // skip the front delims
        dynStart = dynEnd;
    }

    // skip the body
    while(*dynEnd != 0 && *dynEnd != TCPIP_HTTP_DYNVAR_DELIM)
    {
        dynEnd++;
    }
    // eat up multiple back delimiters
    while(*dynEnd != 0 && *dynEnd == TCPIP_HTTP_DYNVAR_DELIM)
    {
        nDelims--;
        if(verifyOnly)
        {
            dynEnd++;
        }
        else
        {
            *dynEnd++ = 0;
        }
    }

    if(nDelims != 0)
    {   // not ending properly
        return 0;
    }


    *pEndDyn = dynEnd;
    return dynStart;
}

// extracts the dynamic variable name and parameters from the connection current file
// stores all the dynamic variable parameters
// Note: strings are kept in the fileBuffer belonging to the parent file!
// returns true for success, false for error
static bool  _HTTP_DynVarExtract(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pDynChDcpt, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt)
{
    int32_t fileLen, extraLen, len;
    char    *argStart, *argEnd, *argStr;
    char    *startDynName, *endDynName = 0;
    uint16_t nArgs;
    int32_t  argInt;
    TCPIP_HTTP_DYN_VAR_DCPT* pDestDcpt;
    TCPIP_HTTP_DYN_ARG_DCPT* pArgDcpt;
    TCPIP_HTTP_DYN_ARG_TYPE  argType;
    char*                    dynVarBuff;
    const TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY* pKEntry; 
    TCPIP_HTTP_DYNVAR_ALLOC_DCPT*       pAllocDcpt;
    bool                     success;
    const void*              dynInfo;
    TCPIP_HTTP_DYN_VAR_FLAGS dynFlags = TCPIP_HTTP_DYN_VAR_FLAG_NONE; 
    TCPIP_HTTP_EVENT_TYPE dynEvent = TCPIP_HTTP_EVENT_NONE;
    TCPIP_HTTP_DYN_ARG_DCPT  dynArgDcpt[TCPIP_HTTP_DYNVAR_ARG_MAX_NUMBER];

    // NOTE: the buffer belonging to the parent file is used!
    dynVarBuff = pFileChDcpt->fileChDcpt.fileBuffDcpt->fileBuffer;
    _HTTPAssertCond(TCPIP_HTTP_DYNVAR_MAX_LEN < pFileChDcpt->fileChDcpt.fileBuffDcpt->fileBufferSize, __func__, __LINE__);

    TCPIP_HTTP_PORT_DCPT* pPort = pHttpCon->connPort;

    // mark it invalid
    pDynChDcpt->flags &= ~TCPIP_HTTP_CHUNK_FLAG_DYNVAR_MASK;

    success = false;

    while(true)
    {
        len = pFileChDcpt->fileChDcpt.fSize - pFileChDcpt->fileChDcpt.fOffset;
        if(len > TCPIP_HTTP_DYNVAR_MAX_LEN)
        {
            len = TCPIP_HTTP_DYNVAR_MAX_LEN;
        }
        fileLen = (*pPort->fileShell->fileRead)(pPort->fileShell, pFileChDcpt->fileChDcpt.fHandle, dynVarBuff, len);

        if(fileLen != len)
        {   // couldn't read data?
            _HTTPDbgCond(false, __func__, __LINE__);
            dynEvent = TCPIP_HTTP_EVENT_FILE_READ_ERROR;
            dynInfo = pDynChDcpt->chunkFName;
            endDynName = dynVarBuff + len;  // eat up all buffer if error
            fileLen = len;
            break;
        }

        dynVarBuff[fileLen] = 0;
        _HTTPAssertCond(dynVarBuff[0] == TCPIP_HTTP_DYNVAR_DELIM, __func__, __LINE__);

        // parse the dynamic variable
        startDynName = _HTTP_DynVarParse(dynVarBuff, &endDynName, false);

        if(startDynName == 0)
        {   // some parsing error
            _HTTPDbgCond(false, __func__, __LINE__);
            dynEvent = TCPIP_HTTP_EVENT_DYNVAR_PARSE_ERROR;
            dynInfo = dynVarBuff;
            break;
        }

        // address the old keyword format
        argStart = strchr(startDynName, TCPIP_HTTP_DYNVAR_KWORD_OLD_ARG_START);
        if(argStart)
        {   // replace with standard arg delimiter
            *argStart = TCPIP_HTTP_DYNVAR_ARG_START;
            // this style of argument doesn't end properly
            argEnd = startDynName + strlen(startDynName);
            *argEnd++ = TCPIP_HTTP_DYNVAR_ARG_END;
            *argEnd++ = 0;
        }

        // get the variable name and the arguments
        nArgs = 0;
        argStart = strchr(startDynName, TCPIP_HTTP_DYNVAR_ARG_START);
        dynInfo = startDynName;

        if(argStart != 0)
        {   // start getting the arguments
            *argStart++ = 0;    // end the dynName properly
            argEnd = strchr(argStart, TCPIP_HTTP_DYNVAR_ARG_END);
            if(argEnd != 0)
            {
                *argEnd = 0;
            }
            else
            {
                dynFlags |= TCPIP_HTTP_DYN_VAR_FLAG_ARG_NAME_TRUNCATED;
                dynEvent |= TCPIP_HTTP_EVENT_DYNVAR_ARG_NAME_TRUNCATED;
            }

            memset(dynArgDcpt, 0, sizeof(dynArgDcpt));
            pArgDcpt = dynArgDcpt;
            while(true)
            {
                argStr = strtok(argStart, TCPIP_HTTP_DYNVAR_ARG_SEP);
                argStart = 0;
                if(argStr == 0)
                {   // done
                    break;
                }

                if(nArgs < sizeof(dynArgDcpt) / sizeof(*dynArgDcpt))
                {
                    argType = _HTTP_ArgType(argStr, &argInt);
                    if(argType == TCPIP_HTTP_DYN_ARG_TYPE_INVALID)
                    {   // ignore void arguments
                        continue;
                    }

                    pArgDcpt->argType = (uint16_t)argType;
                    if(argType == TCPIP_HTTP_DYN_ARG_TYPE_STRING)
                    {
                        pArgDcpt->argStr = argStr;
                    }
                    else
                    {   // TCPIP_HTTP_DYN_ARG_TYPE_INT32
                        pArgDcpt->argInt32 = argInt;
                    }
                    pArgDcpt++;
                }
                // ignore excess arguments

                nArgs++;
            }

            if(nArgs > sizeof(dynArgDcpt) / sizeof(*dynArgDcpt))
            {
                dynFlags |= TCPIP_HTTP_DYN_VAR_FLAG_ARG_NO_TRUNCATED; 
                dynEvent |= TCPIP_HTTP_EVENT_DYNVAR_ARG_NUMBER_TRUNCATED; 
                nArgs = sizeof(dynArgDcpt) / sizeof(*dynArgDcpt);
            }
        }

        // allocate space for this dynVar descriptor and include the dynamic arguments
        pAllocDcpt = (TCPIP_HTTP_DYNVAR_ALLOC_DCPT*)http_mallocFnc(sizeof(TCPIP_HTTP_DYNVAR_ALLOC_DCPT) + nArgs * sizeof(TCPIP_HTTP_DYN_ARG_DCPT));

        if(pAllocDcpt == 0)
        {   // allocation failed
            dynEvent = TCPIP_HTTP_EVENT_DYNVAR_ALLOC_ERROR;
            break;
        }
        // construct the info to be passed to the user
        pDestDcpt = &pAllocDcpt->dynDcpt;
        pDestDcpt->dynName = startDynName;
        pDestDcpt->fileName = pFileChDcpt->chunkFName;
        pDestDcpt->dynFlags = dynFlags;
        pDestDcpt->dynContext = pDynChDcpt;

        // set the args
        if((pDestDcpt->nArgs = nArgs) != 0)
        {
            pDestDcpt->dynArgs = pAllocDcpt->dynArgs;
            memcpy(pDestDcpt->dynArgs, dynArgDcpt, nArgs * sizeof(TCPIP_HTTP_DYN_ARG_DCPT));
        }
        else
        {
            pDestDcpt->dynArgs = 0;
        }

        pDynChDcpt->dynChDcpt.pDynAllocDcpt = pAllocDcpt;
    

        pKEntry = _HTTP_SearchDynVarKeyEntry(pDestDcpt->dynName);
        if(pKEntry != 0 && (pKEntry->keyFlags & TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEFAULT_PROCESS) != 0)
        {
            pDynChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEFAULT_PROCESS;
        }
        pDynChDcpt->dynChDcpt.pKEntry = pKEntry;

        // success; mark it valid!
        pDynChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_DYNVAR_VALID;
        _HTTP_DynDbgExtract(pDestDcpt->dynName, pDestDcpt->nArgs, pDestDcpt->fileName, pHttpCon->connIx);

        success = true;
        break;
    }
    
    if(dynEvent != TCPIP_HTTP_EVENT_NONE)
    {
        _HTTP_Report_ConnectionEvent(pHttpCon, dynEvent, dynInfo);
    }

    // adjust the file
    extraLen = fileLen - (endDynName - dynVarBuff);
    if(extraLen)
    {
        (*pPort->fileShell->fileSeek)(pPort->fileShell, pFileChDcpt->fileChDcpt.fHandle, -extraLen, SYS_FS_SEEK_CUR);
    }

    // adjust the file byte counters with read characters
    pFileChDcpt->fileChDcpt.fOffset += endDynName - dynVarBuff;


    return success;
}

static const TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY* _HTTP_SearchDynVarKeyEntry(const char* keyword)
{
    int ix;

    const TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY* pEntry = httpDynVarKeywords;
    for(ix = 0; ix < sizeof(httpDynVarKeywords) / sizeof(*httpDynVarKeywords); ix++, pEntry++)
    {
        if(strcmp(pEntry->keyWord, keyword) == 0)
        {
            return pEntry;
        }
    }

    return 0;
}

static TCPIP_HTTP_CHUNK_RES _HTTP_ProcessDynVarChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    TCPIP_HTTP_CHUNK_RES chunkRes;

    _HTTPAssertCond((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_DYNVAR_VALID) != 0, __func__, __LINE__);
    
    if(pChDcpt->status == TCPIP_HTTP_CHUNK_STATE_BEG)
    {   // call the dynamic variable print function (again)
        chunkRes = _HTTP_DynVarCallback(pHttpCon, pChDcpt);
        if(chunkRes == TCPIP_HTTP_CHUNK_RES_WAIT)
        {   // user needs a break
            return TCPIP_HTTP_CHUNK_RES_WAIT;
        }
        pChDcpt->status = TCPIP_HTTP_CHUNK_STATE_DATA;
        pChDcpt->phase = TCPIP_HTTP_DYNVAR_PHASE_START;
    }


    if(pChDcpt->status == TCPIP_HTTP_CHUNK_STATE_DATA)
    {   // process the user's data
        if(!_HTTP_DynVarProcess(pHttpCon, pChDcpt))
        {
            return TCPIP_HTTP_CHUNK_RES_WAIT;
        }

        // done with the data
        // check if we need to call again
        if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_DYNVAR_AGAIN) != 0)
        {   // yep, run another turn
            pChDcpt->status = TCPIP_HTTP_CHUNK_STATE_BEG;
            return TCPIP_HTTP_CHUNK_RES_OK;
        }

        // we're done
        pChDcpt->status = TCPIP_HTTP_CHUNK_STATE_DONE;
    }

    // when we're done, and need to delete ourselves
    // make sure we're executing now
    if(pChDcpt == (TCPIP_HTTP_CHUNK_DCPT*)pHttpCon->chunkList.head)
    {   // we're done
        _HTTP_FreeChunk(pHttpCon, pChDcpt);
        return TCPIP_HTTP_CHUNK_RES_DONE; 
    } 

    return TCPIP_HTTP_CHUNK_RES_OK; 
}

// performs the calling of a callback for a dynamic variable
// detects if default needs to be called, etc.
// returns: TCPIP_HTTP_CHUNK_RES_WAIT, TCPIP_HTTP_CHUNK_RES_OK or TCPIP_HTTP_CHUNK_RES_DONE
static TCPIP_HTTP_CHUNK_RES _HTTP_DynVarCallback(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    TCPIP_HTTP_DYN_PRINT_RES printRes;
    const TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY* pKEntry; 
    TCPIP_HTTP_DYNVAR_CHUNK_DCPT* pDynChunkDcpt;
    TCPIP_HTTP_DYN_VAR_DCPT*      pDynDcpt;

    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 
    const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pInstance->httpRegistry;
    pDynChunkDcpt = &pChDcpt->dynChDcpt;
    pDynDcpt = &pDynChunkDcpt->pDynAllocDcpt->dynDcpt;

    printRes = TCPIP_HTTP_DYN_PRINT_RES_DONE;
    
    if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_DYNVAR_AGAIN) == 0)
    {   // 1st pass
        pHttpCon->callbackPos = 0;
    }

    if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEFAULT_PROCESS) == 0)
    {   // call the user processing
        if(!_HTTP_DbgKillUserDynVar())
        {
            if(httpUserCback->dynamicPrint == 0)
            {
                pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEFAULT_PROCESS;
            }
            else
            {
                printRes = (*httpUserCback->dynamicPrint)(pHttpCon, pDynDcpt, httpUserCback);
                _HTTP_DynDbgCallback(pDynDcpt->dynName, printRes, pHttpCon->connIx);
                if(printRes == TCPIP_HTTP_DYN_PRINT_RES_DEFAULT)
                {
                    pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEFAULT_PROCESS;
                }
                else if(printRes == TCPIP_HTTP_DYN_PRINT_RES_AGAIN || printRes == TCPIP_HTTP_DYN_PRINT_RES_PROCESS_AGAIN)
                {
                    if(++pDynChunkDcpt->dynRetries > pInstance->dynVarRetries)
                    {
                        printRes = TCPIP_HTTP_DYN_PRINT_RES_DONE; 
                        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_DYNVAR_RETRIES_EXCEEDED, pDynDcpt->dynName);
                    }
                }
            }
        }
    }

    if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEFAULT_PROCESS) != 0)
    {   // call the default implementation!
        if((pKEntry = pDynChunkDcpt->pKEntry) != 0)
        {
            printRes = (*pKEntry->dynamicPrint)(pHttpCon, pDynDcpt, httpUserCback);
            _HTTP_DynDbgCallback(pDynDcpt->dynName, printRes, pHttpCon->connIx);
        }
    }

    if(pHttpCon->callbackPos != 0)
    {   // user messing with the callbackPos; ignore the printRes
        printRes = TCPIP_HTTP_DYN_PRINT_RES_PROCESS_AGAIN;
    }

    pChDcpt->flags &= ~TCPIP_HTTP_CHUNK_FLAG_DYNVAR_AGAIN; 
    if(printRes == TCPIP_HTTP_DYN_PRINT_RES_AGAIN || printRes == TCPIP_HTTP_DYN_PRINT_RES_PROCESS_AGAIN)
    {
        pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_DYNVAR_AGAIN; 
        return printRes == TCPIP_HTTP_DYN_PRINT_RES_AGAIN ? TCPIP_HTTP_CHUNK_RES_WAIT : TCPIP_HTTP_CHUNK_RES_OK;
    }

    // don't need calling again
    return TCPIP_HTTP_CHUNK_RES_DONE;
}

// process the data that's passed during the dynamic variable callback
// returns true if done
// false if needs to be called again
static bool _HTTP_DynVarProcess(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    uint32_t chunkSize;
    uint16_t writeSize, outSize;
    uint8_t* writeBuff;
    bool     needAck;
    const void* userBuff;
    TCPIP_HTTP_DYNVAR_BUFF_DCPT*    pDynDcpt;

    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 
    const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pInstance->httpRegistry;

    if(pChDcpt->phase == TCPIP_HTTP_DYNVAR_PHASE_START)
    {   // just starting, get the chunk size
        chunkSize = 0;
        for(pDynDcpt = (TCPIP_HTTP_DYNVAR_BUFF_DCPT*)pChDcpt->dynChDcpt.dynBuffList.head; pDynDcpt != 0; pDynDcpt = pDynDcpt->next)
        {
            chunkSize += pDynDcpt->dynBufferSize;
        }
        if(chunkSize == 0)
        {   // nothing to do
            pChDcpt->phase = TCPIP_HTTP_DYNVAR_PHASE_DONE;
            return true;
        }
        // show the dynVar outputs some data
        pChDcpt->pRootDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_OUT_DATA;

        if(_HTTP_StartHttpChunk(pHttpCon, chunkSize) == 0)
        {   // wait some more... and recalculate (avoid chunk storage for the size)!
            return false;
        }
        // start sending data
        pChDcpt->phase = TCPIP_HTTP_DYNVAR_PHASE_DATA;
    }

    while(pChDcpt->phase == TCPIP_HTTP_DYNVAR_PHASE_DATA)
    {   // ok, there's some work to be done; try to fill the socket buffer
        pDynDcpt = (TCPIP_HTTP_DYNVAR_BUFF_DCPT*)pChDcpt->dynChDcpt.dynBuffList.head;

        if(pDynDcpt == 0)
        {   // done; signal end chunk
            pChDcpt->phase = TCPIP_HTTP_DYNVAR_PHASE_END;
            break;
        }
    
        writeSize = pDynDcpt->dynBufferSize - pDynDcpt->writeOffset;
        _HTTPDbgCond(writeSize != 0, __func__, __LINE__);
        writeBuff = (uint8_t*)pDynDcpt->dynBuffer + pDynDcpt->writeOffset;
        outSize = NET_PRES_SocketWrite(pHttpCon->socket, writeBuff, writeSize);
        pDynDcpt->writeOffset += outSize;
        if(outSize != writeSize)
        {   // couldn't write all of it; wait some more
            return false;
        }

        if(pDynDcpt->writeOffset == pDynDcpt->dynBufferSize)
        {   // buffer done
            pDynDcpt = (TCPIP_HTTP_DYNVAR_BUFF_DCPT*)TCPIP_Helper_SingleListHeadRemove(&pChDcpt->dynChDcpt.dynBuffList);
            needAck = (pDynDcpt->dynFlags & TCPIP_HTTP_DYNVAR_BUFF_FLAG_ACK) != 0;
            userBuff = pDynDcpt->dynBuffer;
            _HTTP_DynDbgProcess(pChDcpt->dynChDcpt.pDynAllocDcpt->dynDcpt.dynName, pDynDcpt->dynBufferSize, needAck, pHttpCon->connIx);
            _HTTP_ReleaseDynBuffDescriptor(pInstance, pDynDcpt);
            if(needAck)
            {
                if(httpUserCback->dynamicAck != 0)
                {
                    (*httpUserCback->dynamicAck)(pHttpCon, userBuff, httpUserCback);
                }
            }
        }
    }

    if(pChDcpt->phase == TCPIP_HTTP_DYNVAR_PHASE_END)
    {
        if(_HTTP_EndHttpChunk(pHttpCon, TCPIP_HTTP_CHUNK_END_CURRENT) == 0)
        {
            return false;
        }
    }
    
    pChDcpt->phase = TCPIP_HTTP_DYNVAR_PHASE_DONE;
    return true;

}

// gets a new buffer descriptor and adds it to the existing chunk list
// assumes the chunk is a dynamic variable one!
static TCPIP_HTTP_DYNVAR_BUFF_DCPT* _HTTP_GetDynBuffDescriptor(TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    TCPIP_HTTP_DYNVAR_BUFF_DCPT*    pDynDcpt;

    TCPIP_HTTP_INST* pInstance = pChDcpt->pOwnerCon->connInstance; 
    SINGLE_LIST* httpDynVarPool = &pInstance->dynVarPool;
    pDynDcpt = (TCPIP_HTTP_DYNVAR_BUFF_DCPT*)TCPIP_Helper_SingleListHeadRemove(httpDynVarPool);
    if(pDynDcpt != 0)
    {   // initialize the buffer
        memset(pDynDcpt, 0, sizeof(*pDynDcpt));
        TCPIP_Helper_SingleListTailAdd(&pChDcpt->dynChDcpt.dynBuffList, (SGL_LIST_NODE*)pDynDcpt);
    }
    else
    {
        pInstance->dynPoolEmpty++;
    }

    return pDynDcpt;
}

static void _HTTP_ReleaseDynBuffDescriptor(TCPIP_HTTP_INST* pInstance, TCPIP_HTTP_DYNVAR_BUFF_DCPT* pDynDcpt)
{
    TCPIP_Helper_SingleListTailAdd(&pInstance->dynVarPool, (SGL_LIST_NODE*)pDynDcpt);
}

// dynamic variable API functions
bool TCPIP_HTTP_DynamicWrite(const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const void * buffer, uint16_t size, bool needAck)
{

    if(buffer == 0)
    {
        return false;   // do nothing
    }

    TCPIP_HTTP_DYNVAR_BUFF_DCPT*    pDynBuffDcpt;
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt = (TCPIP_HTTP_CHUNK_DCPT*)varDcpt->dynContext;
    TCPIP_HTTP_INST* pInstance = pChDcpt->pOwnerCon->connInstance; 

    // this should be a dynamic data chunk!
    _HTTPAssertCond((pChDcpt->flags & (TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE | TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI)) == 0, __func__, __LINE__);

    if(size == 0)
    {   // buffer done!
        const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pInstance->httpRegistry;
        if(needAck && httpUserCback->dynamicAck != 0)
        {
            (*httpUserCback->dynamicAck)(pChDcpt->pOwnerCon, buffer, httpUserCback);
        }
        return true;
    }

    // get a dynamic descriptor
    pDynBuffDcpt = _HTTP_GetDynBuffDescriptor(pChDcpt);

    if(pDynBuffDcpt != 0)
    {   // no more buffers
        pDynBuffDcpt->dynBuffer = buffer;
        pDynBuffDcpt->dynBufferSize = size;
        if(needAck)
        {
            pDynBuffDcpt->dynFlags |= TCPIP_HTTP_DYNVAR_BUFF_FLAG_ACK;
        }
    }

    return pDynBuffDcpt != 0;
}

bool TCPIP_HTTP_DynamicWriteString(const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const char* str, bool needAck)
{
    return str ? TCPIP_HTTP_DynamicWrite(varDcpt, str, strlen(str), needAck) : false;
}

#else
// dynamic variable API functions
bool TCPIP_HTTP_DynamicWrite(const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const void * buffer, uint16_t size, bool needAck)
{
    return false;
}

bool TCPIP_HTTP_DynamicWriteString(const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const char* str, bool needAck)
{
    return false;
}

#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)


#if (TCPIP_HTTP_SSI_PROCESS != 0)
// parses the lineBuff for a valid SSI directive that needs to be processed by the server
// - lineBuff should be properly ended with \0
// - returns where the processing should start
//  or 0 if not found or some parsing error
// - pEndProcess stores where processing ends 
// - if verifyOnly is true, the position of the front delim is returned and the buffer is not changed
// - else the position past the delim is returned and zeroes are stored in the buffer where the variable ends 
static char* _HTTP_SSILineParse(char* lineBuff, char** pEndProcess, bool verifyOnly)
{
    char* ssiStart = strstr(lineBuff, TCPIP_HTTP_SSI_COMMAND_START);

    while(ssiStart != 0 && (ssiStart == lineBuff || (*(ssiStart - 1) == TCPIP_HTTP_NET_LINE_END)))
    {   // SSI comment allowed only at line start
        // check the line is formatted OK
        char* cmdStart = ssiStart + strlen(TCPIP_HTTP_SSI_COMMAND_START);
        char* lineEnd = strchr(lineBuff, TCPIP_HTTP_NET_LINE_END);  // check if there is EOL 

        char* ssiEnd = strstr(cmdStart, TCPIP_HTTP_SSI_COMMENT_DELIM);
        if(ssiEnd == 0)
        {   // no valid end found
            break;
        }

        char* ssiStop = ssiEnd; // store location to end the string

        // found valid end --; 
        ssiEnd += strlen(TCPIP_HTTP_SSI_COMMENT_DELIM);
        while(*ssiEnd != 0 && *ssiEnd == ' ')
        {   // spaces are allowed , "--   >" is a valid end
            ssiEnd++;
        }

        if(*ssiEnd != TCPIP_HTTP_SSI_COMMAND_END_CHAR)
        {   // not ended properly!
            break;
        }

        if(lineEnd != 0 && (ssiStart < lineEnd && lineEnd < ssiEnd))
        {   // SSI command shouldn't span multiple lines!
            break;
        }

        // ended ok; check that the attribute separators are an even number
        char* p = cmdStart;
        int attrDelim = 0;
        while(p != ssiEnd)
        {
            if(*p++ == TCPIP_HTTP_SSI_ATTRIB_DELIM)
            {
                attrDelim++;
            } 
        }

        if(attrDelim == 0 || (attrDelim & 1) == 0)
        {   // either no delimiter or even number
            // parsed ok
            *pEndProcess = ssiEnd + 1;
            if(verifyOnly == false)
            {
                *ssiStop = 0;
                return cmdStart;
            }
            return ssiStart;
        }

        // done; and failed
        break;
    }

    return 0;
}

// 
// extracts the SSI command and attributes from the connection current file
// stores all the SSI command attribute/value pairs
// Note: strings are kept in the fileBuffer belonging to the parent file!
// returns true for success, false for error
static bool  _HTTP_SSIExtract(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt)
{
    int32_t fileLen, extraLen, len;
    char    *ssiBuff;
    char    *startSsiCmd, *endSsiCmd;
    char    *ssiCmd;
    char    *attrStr, *attrVal;
    int     nAttribs, nStaticAttribs, nAllocAttribs;
    const TCPIP_HTTP_SSI_PROCESS_ENTRY* pEntry;
    TCPIP_HTTP_SSI_ATTR_DCPT* pAttrDcpt;
    TCPIP_HTTP_SSI_ATTR_DCPT* pAllocAttrib; 
    TCPIP_HTTP_SSI_NOTIFY_DCPT  notifyDcpt;
    bool                        success;
    const void*                 evInfo = 0;
    TCPIP_HTTP_EVENT_TYPE   evType = TCPIP_HTTP_EVENT_NONE;
    
    TCPIP_HTTP_SSI_ATTR_DCPT  ssiAttribTbl[TCPIP_HTTP_SSI_ATTRIBUTES_MAX_NUMBER];


    ssiBuff = pFileChDcpt->fileChDcpt.fileBuffDcpt->fileBuffer;
    _HTTPAssertCond(TCPIP_HTTP_SSI_CMD_MAX_LEN < pFileChDcpt->fileChDcpt.fileBuffDcpt->fileBufferSize, __func__, __LINE__);

    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 
    const SYS_FS_SHELL_OBJ* fileShell = pHttpCon->connPort->fileShell;
    success = false;
    endSsiCmd = 0;

    while(true)
    {
        len = pFileChDcpt->fileChDcpt.fSize - pFileChDcpt->fileChDcpt.fOffset;
        if(len > TCPIP_HTTP_SSI_CMD_MAX_LEN)
        {
            len = TCPIP_HTTP_SSI_CMD_MAX_LEN;
        }
        fileLen = (*fileShell->fileRead)(fileShell, pFileChDcpt->fileChDcpt.fHandle, ssiBuff, len);
        if(fileLen != len)
        {   // couldn't read data?
            evInfo = pFileChDcpt->chunkFName;
            evType = TCPIP_HTTP_EVENT_FILE_READ_ERROR;
            endSsiCmd = ssiBuff + len; // eat up all buffer if error
            fileLen = len;
            break;
        }

        ssiBuff[fileLen] = 0;
        _HTTPAssertCond(ssiBuff[0] == TCPIP_HTTP_SSI_COMMAND_START_CHAR, __func__, __LINE__);

        // parse the dynamic variable
        startSsiCmd = _HTTP_SSILineParse(ssiBuff, &endSsiCmd, false);

        if(startSsiCmd == 0)
        {   // some parsing error; should not happen since it was parsed properly before
            _HTTPDbgCond(false, __func__, __LINE__);
            evInfo = ssiBuff;
            evType = TCPIP_HTTP_EVENT_SSI_PARSE_ERROR;
            break;
        }

        // get the command
        ssiCmd = strtok(startSsiCmd, TCPIP_HTTP_SSI_CMD_SEP);

        // get the command attributes
        nAttribs = 0;
        pAttrDcpt = ssiAttribTbl;
        while(true)
        {
            if((attrStr = strtok(0, TCPIP_HTTP_SSI_ATTR_SEP)) == 0)
            {   // done
                break;
            }
            if((attrVal = strtok(0, TCPIP_HTTP_SSI_VALUE_SEP)) != 0)
            {
                if(strcspn(attrVal, " ") == 0)
                {   // if all blanks, ignore it
                    attrVal = 0;
                }
            }

            if(nAttribs < sizeof(ssiAttribTbl) / sizeof(*ssiAttribTbl))
            {
                pAttrDcpt->attribute = attrStr;
                pAttrDcpt->value = attrVal;
                pAttrDcpt++;
            }

            nAttribs++;
        }

        // notify the user of this valid SSI line
        const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pInstance->httpRegistry;
        if(httpUserCback->ssiNotify != 0)
        {
            notifyDcpt.fileName = pFileChDcpt->chunkFName;
            notifyDcpt.ssiCommand = ssiCmd;
            notifyDcpt.nAttribs = (nAttribs > sizeof(ssiAttribTbl) / sizeof(*ssiAttribTbl)) ? sizeof(ssiAttribTbl) / sizeof(*ssiAttribTbl) : nAttribs;
            notifyDcpt.pAttrDcpt = ssiAttribTbl;

            if((*httpUserCback->ssiNotify)(pHttpCon, &notifyDcpt, httpUserCback))
            {   // no processing needed; suppress the SSI line from the output, indicate as error and continue
                break; 
            } 
            // else just continue normally
        }

        // sanity check
        pEntry = _HTTP_SSIFindEntry(ssiCmd);
        evInfo = ssiCmd;

        if(pEntry == 0)
        {   // no such command
            evType = TCPIP_HTTP_EVENT_SSI_COMMAND_ERROR;
            break;
        }

    
        if(nAttribs == 0)
        {   // couldn't get the command attributes
            evType = TCPIP_HTTP_EVENT_SSI_ATTRIB_ERROR;
            break;
        }
        else if(nAttribs > sizeof(ssiAttribTbl) / sizeof(*ssiAttribTbl))
        {
            evType = TCPIP_HTTP_EVENT_SSI_ATTRIB_NUMBER_TRUNCATED;
            nAttribs = sizeof(ssiAttribTbl) / sizeof(*ssiAttribTbl);
        }

        pAllocAttrib = 0;
        nStaticAttribs = nAttribs;
        if(nStaticAttribs > sizeof(pChDcpt->ssiChDcpt.staticAttribs) / sizeof(*pChDcpt->ssiChDcpt.staticAttribs))
        {
            nStaticAttribs = sizeof(pChDcpt->ssiChDcpt.staticAttribs) / sizeof(*pChDcpt->ssiChDcpt.staticAttribs);
        }
        nAllocAttribs = nAttribs - nStaticAttribs;

        if(nAllocAttribs != 0)
        {   // SSI command with excess attributes
            // allocate space for the SSI descriptor  
            pAllocAttrib = (TCPIP_HTTP_SSI_ATTR_DCPT*)http_mallocFnc(nAllocAttribs * sizeof(TCPIP_HTTP_SSI_ATTR_DCPT));

            if(pAllocAttrib == 0)
            {   // allocation failed
                evType = TCPIP_HTTP_EVENT_SSI_ALLOC_DESCRIPTOR_ERROR;
                break;
            } 
        }

        // copy the SSI attributes info
        if(nStaticAttribs != 0)
        {
            memcpy(pChDcpt->ssiChDcpt.staticAttribs, ssiAttribTbl, nStaticAttribs * sizeof(TCPIP_HTTP_SSI_ATTR_DCPT)); 
        }
        if(nAllocAttribs != 0)
        {
            memcpy(pAllocAttrib, ssiAttribTbl + nStaticAttribs, nAllocAttribs * sizeof(TCPIP_HTTP_SSI_ATTR_DCPT)); 
        }

        // construct the SSI info
        pChDcpt->ssiChDcpt.ssiCmd = ssiCmd;
        pChDcpt->ssiChDcpt.fileName = pFileChDcpt->chunkFName;
        pChDcpt->ssiChDcpt.ssiFnc = pEntry->ssiFnc;
        pChDcpt->ssiChDcpt.nStaticAttribs = nStaticAttribs;
        pChDcpt->ssiChDcpt.nAllocAttribs = nAllocAttribs;
        pChDcpt->ssiChDcpt.pAllocAttribs = pAllocAttrib;

        // success;
        success = true;
        break;
    }

    if(evType != TCPIP_HTTP_EVENT_NONE)
    {
       _HTTP_Report_ConnectionEvent(pHttpCon, evType, evInfo);
    }

    // adjust the file
    extraLen = fileLen - (endSsiCmd - ssiBuff);
    if(extraLen)
    {
        (*fileShell->fileSeek)(fileShell, pFileChDcpt->fileChDcpt.fHandle, -extraLen, SYS_FS_SEEK_CUR);
    }

    // adjust the file byte counters with read characters
    pFileChDcpt->fileChDcpt.fOffset += endSsiCmd - ssiBuff;

    return success;
}

static TCPIP_HTTP_CHUNK_RES _HTTP_ProcessSSIChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    int                     currAttrib, leftAttribs;
    TCPIP_HTTP_CHUNK_RES    chunkRes;
    TCPIP_HTTP_SSI_ATTR_DCPT *pAttr;

    _HTTPAssertCond(pChDcpt->ssiChDcpt.ssiFnc != 0, __func__, __LINE__);

    // basic sanity check
    if(pChDcpt->ssiChDcpt.nStaticAttribs == 0)
    {   // we should have had at least one attribute pair
        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_SSI_ATTRIB_NUMBER_MISMATCH, pChDcpt->ssiChDcpt.ssiCmd);
        return TCPIP_HTTP_CHUNK_RES_SSI_ATTRIB_ERR;
    }

    // check if there's attributes to be processed
    // the current attribute number is update by the processing function
    while(true)
    {   
        // get the current attribute to be processed
        currAttrib = pChDcpt->ssiChDcpt.nCurrAttrib;

        if(currAttrib < pChDcpt->ssiChDcpt.nStaticAttribs)
        {
            pAttr = pChDcpt->ssiChDcpt.staticAttribs + currAttrib;
        }
        else if(currAttrib < pChDcpt->ssiChDcpt.nStaticAttribs + pChDcpt->ssiChDcpt.nAllocAttribs)
        {   // excess args
            pAttr = pChDcpt->ssiChDcpt.pAllocAttribs + (currAttrib - pChDcpt->ssiChDcpt.nStaticAttribs);
        }
        else
        {   // we're done
            break;
        }

        leftAttribs = pChDcpt->ssiChDcpt.nStaticAttribs + pChDcpt->ssiChDcpt.nAllocAttribs - currAttrib;

        // have arguments to execute
        // execute the SSI process function
        chunkRes = (*pChDcpt->ssiChDcpt.ssiFnc)(pHttpCon, pChDcpt, pAttr, leftAttribs);

        if(chunkRes == TCPIP_HTTP_CHUNK_RES_WAIT)
        {   // return back to caller needed
            return TCPIP_HTTP_CHUNK_RES_WAIT;
        }
        else if(chunkRes == TCPIP_HTTP_CHUNK_RES_OK) 
        {   // some new chunk spawned that needs to execute
            return TCPIP_HTTP_CHUNK_RES_OK;
        }

        // else either done or failed
        // even if some error occurred: ignore and continue processing the command attributes
    }

    // when we're done, delete ourselves
    // but only if we're executing now
    if(pChDcpt == (TCPIP_HTTP_CHUNK_DCPT*)pHttpCon->chunkList.head)
    {   // we're done
        _HTTP_FreeChunk(pHttpCon, pChDcpt);
        return TCPIP_HTTP_CHUNK_RES_DONE; 
    }

    return TCPIP_HTTP_CHUNK_RES_OK; 
}

// processes an SSI include directive
// returns: see TCPIP_SSI_COMMAND_FNC definition
static TCPIP_HTTP_CHUNK_RES _HTTP_SSIInclude(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, int leftAttribs)
{
    // process one attribute pair at a time
    pChDcpt->ssiChDcpt.nCurrAttrib++;
    if(strcmp(pAttr->attribute, "virtual") == 0 || strcmp(pAttr->attribute, "file") == 0)
    {   // open the requested file
        return _HTTP_IncludeFile(pHttpCon, pChDcpt, pAttr->value);
    }

    _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_SSI_ATTRIB_UNKNOWN, pAttr->attribute);
    return TCPIP_HTTP_CHUNK_RES_DONE;
}

// processes an SSI include directive
// returns: see TCPIP_SSI_COMMAND_FNC definition
static TCPIP_HTTP_CHUNK_RES _HTTP_SSISet(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, int leftAttribs)
{
    int     procAttribs;
    TCPIP_HTTP_SSI_ATTR_DCPT    *pAttrName, *pAttrVal;
    TCPIP_HTTP_SSI_HASH_ENTRY   *pHE, *pHRef;
    TCPIP_HTTP_DYN_ARG_TYPE     argType;
    int32_t                     intArg;
    const void*                 evInfo = 0;
    TCPIP_HTTP_EVENT_TYPE   evType = TCPIP_HTTP_EVENT_NONE;
    TCPIP_HTTP_CHUNK_RES        chunkRes = TCPIP_HTTP_CHUNK_RES_DONE;

    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 
    // process two attribute pairs at a time
    while(true)
    {
        procAttribs = 0;
        if(leftAttribs < 2)
        {   // we should have had at least one pair or an even number; skip all remaining attributes
            procAttribs = leftAttribs;
            evType = TCPIP_HTTP_EVENT_SSI_ATTRIB_NUMBER_MISMATCH;
            evInfo = pChDcpt->ssiChDcpt.ssiCmd;
            chunkRes = TCPIP_HTTP_CHUNK_RES_SSI_ATTRIB_ERR;
            break;
        }

        // process remaining attributes
        pAttrName = pAttr;
        pAttrVal = pAttrName + 1;
        procAttribs = 2;

        if(strcmp(pAttrName->attribute, "var") != 0 || strcmp(pAttrVal->attribute, "value") != 0)
        {   // unknown attributes
            evType = TCPIP_HTTP_EVENT_SSI_ATTRIB_UNKNOWN;
            evInfo =  pAttrName->attribute;
            chunkRes = TCPIP_HTTP_CHUNK_RES_SSI_ATTRIB_ERR;
            break;
        }

        // check that the SSI hash is created
        OA_HASH_DCPT* ssiHashDcpt = pInstance->ssiHashDcpt;
        if(ssiHashDcpt == 0)
        {
            if((ssiHashDcpt = _HTTP_SSICreateHash(pInstance)) == 0)
            {
                evType = TCPIP_HTTP_EVENT_SSI_HASH_CREATE_FAILED;
                evInfo =  pChDcpt->chunkFName;
                chunkRes = TCPIP_HTTP_CHUNK_RES_SSI_CACHE_FAIL;
                break;
            }
            pInstance->ssiHashDcpt = ssiHashDcpt;
        }

        argType = _HTTP_ArgType(pAttrVal->value, &intArg);
        if(argType == TCPIP_HTTP_DYN_ARG_TYPE_INVALID)
        {   // an invalid type, i.e. empty string; we delete this variable
            pHE = (TCPIP_HTTP_SSI_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(ssiHashDcpt, pAttrName->value);
            if(pHE)
            {   // variable exists
                pHE->varName[0] = 0;
                TCPIP_OAHASH_EntryRemove(ssiHashDcpt, &pHE->hEntry);
                evType = TCPIP_HTTP_EVENT_SSI_VAR_DELETED;
            }
            else
            {   // no such variable
                evType = TCPIP_HTTP_EVENT_SSI_VAR_UNKNOWN;
            }

            evInfo = pAttrName->value;
            break;
        }

        // creating a new variable or updating an existent one
        if(pAttrVal->value[0] == '$')
        {   // it's a value reference
            pHRef = (TCPIP_HTTP_SSI_HASH_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(ssiHashDcpt, pAttrVal->value + 1);
            if(pHRef == 0)
            {   // no such variable
                evType = TCPIP_HTTP_EVENT_SSI_VAR_UNKNOWN;
                evInfo = pAttrName->value + 1;
                break;
            }
        }
        else
        {
            pHRef = 0;
        }

        // search for the variable to be updated
        pHE = (TCPIP_HTTP_SSI_HASH_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(ssiHashDcpt, pAttrName->value);
        if(pHE == 0)
        {   // failed, no more slots available
            evType = TCPIP_HTTP_EVENT_SSI_VAR_NUMBER_EXCEEDED;
            evInfo = pAttrName->value;
            chunkRes = TCPIP_HTTP_CHUNK_RES_SSI_ATTRIB_ERR;
            break;
        }

        if(pHRef)
        {   // reference
            pHE->varType = pHRef->varType;
            pHE->valInt = pHRef->valInt;
            strcpy(pHE->varStr, pHRef->varStr);
            _HTTP_DbgSSIHashEntry(pHE, true);
        }
        else
        {
            pHE->varType = argType;
            if(pAttrVal->value[0] == '\\' && pAttrVal->value[1] == '$')
            {   // bypass the escape $ sequence
                pAttrVal->value += 1;
            }

            strncpy(pHE->varStr, pAttrVal->value, sizeof(pHE->varStr) - 1);
            pHE->varStr[sizeof(pHE->varStr) - 1] = 0;

            if(argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
            {   // int value
                pHE->valInt = intArg;
            }
            // else regular string
            _HTTP_DbgSSIHashEntry(pHE, false);
        }

        break;
    }
    
    pChDcpt->ssiChDcpt.nCurrAttrib += procAttribs;

    if(evType != TCPIP_HTTP_EVENT_NONE)
    {
       _HTTP_Report_ConnectionEvent(pHttpCon, evType, evInfo);
    }

    return chunkRes;

}

// returns: see TCPIP_SSI_COMMAND_FNC definition
static TCPIP_HTTP_CHUNK_RES _HTTP_SSIEcho(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, int leftAttribs)
{
    uint16_t  echoLen, trailBytes, hdrBytes;
    uint16_t outBytes, socketBytes;
    TCPIP_HTTP_SSI_HASH_ENTRY*  pHE;
    char*                       pEcho;
    const void*                 evInfo = 0;
    TCPIP_HTTP_EVENT_TYPE   evType = TCPIP_HTTP_EVENT_NONE;

    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 
    OA_HASH_DCPT* ssiHashDcpt = pInstance->ssiHashDcpt;

    // construct the echo response here    
#if defined(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE)
    echoLen = (strlen(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE) > sizeof(pHE->varStr)) ? strlen(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE) : sizeof(pHE->varStr);
    echoLen += strlen(pAttr->value);    // we append the variable name to the message
#else
    echoLen = sizeof(pHE->varStr);
#endif // defined(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE)

    char* echoBuffer = (char*)http_mallocFnc(TCPIP_HTTP_CHUNK_HEADER_LEN + echoLen + TCPIP_HTTP_CHUNK_FINAL_TRAILER_LEN + 1);
    if(echoBuffer == 0)
    {   // allocation failed
        char eventBuff[16];
        sprintf(eventBuff, "%d", (TCPIP_HTTP_CHUNK_HEADER_LEN + echoLen + TCPIP_HTTP_CHUNK_FINAL_TRAILER_LEN + 1));
        _HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_SSI_ALLOC_ECHO_ERROR, eventBuff);
        return TCPIP_HTTP_CHUNK_RES_SSI_ALLOC_ERR; 
    }

    // process one attribute pair at a time
    while(true)
    {   
        pHE = 0;
        if(pChDcpt->phase == TCPIP_HTTP_DYNVAR_PHASE_START)
        {   // minimum sanity check
            if(strcmp(pAttr->attribute, "var") != 0)
            {
                evType = TCPIP_HTTP_EVENT_SSI_ATTRIB_UNKNOWN;
                evInfo = pAttr->attribute;
                break;
            }

            // find the requested variable
            if(ssiHashDcpt == 0 || (pHE = (TCPIP_HTTP_SSI_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(ssiHashDcpt, pAttr->value)) == 0)
            {   // the hash was not yet created or no such variable exists
                evType = TCPIP_HTTP_EVENT_SSI_VAR_UNKNOWN;
                evInfo = pAttr->value;
            }
            else if(strlen(pHE->varStr) == 0)
            {   // a void variable
                evType = TCPIP_HTTP_EVENT_SSI_VAR_VOID;
                evInfo = pAttr->value;
            }
            pChDcpt->phase = TCPIP_HTTP_DYNVAR_PHASE_DATA;
        }
        else if(pHE == 0 && ssiHashDcpt != 0)
        {   // a retry
            pHE = (TCPIP_HTTP_SSI_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(ssiHashDcpt, pAttr->value);
        }

        // echo it
        pEcho = echoBuffer + TCPIP_HTTP_CHUNK_HEADER_LEN;   // point to data

        if(pHE)
        {
            strcpy(pEcho, pHE->varStr);
            echoLen = strlen(pHE->varStr);
        }
#if defined(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE)
        else if(strlen(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE) != 0)
        {   // insert a not found message
            sprintf(pEcho, TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE "%s", pAttr->value);
            echoLen = strlen(pEcho);
        }
#endif // defined(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE)
        else
        {   // no output
            break;
        }

        hdrBytes = _HTTP_PrependStartHttpChunk(pHttpCon, pEcho, echoLen);
        trailBytes = _HTTP_AppendEndHttpChunk(pHttpCon, pEcho + echoLen, TCPIP_HTTP_CHUNK_END_CURRENT);
        outBytes = hdrBytes + echoLen + trailBytes;
        socketBytes = NET_PRES_SocketWriteIsReady(pHttpCon->socket, outBytes, 0);
        if(socketBytes < outBytes)
        {
            http_freeFnc(echoBuffer);
            return TCPIP_HTTP_CHUNK_RES_WAIT;
        }

        socketBytes = NET_PRES_SocketWrite(pHttpCon->socket, pEcho - hdrBytes, outBytes);
        _HTTPAssertCond(socketBytes == outBytes, __func__, __LINE__);

        break;
    }

    if(evType != TCPIP_HTTP_EVENT_NONE)
    {
        _HTTP_Report_ConnectionEvent(pHttpCon, evType, evInfo);
    }

    http_freeFnc(echoBuffer);

    pChDcpt->ssiChDcpt.nCurrAttrib++;
    return TCPIP_HTTP_CHUNK_RES_DONE;
}

bool TCPIP_HTTP_SSIVariableGet(size_t instIx, const char* varName, TCPIP_HTTP_DYN_ARG_DCPT* pVarDcpt)
{

    if(pVarDcpt != 0 && instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);

        if(pInstance->ssiHashDcpt != 0)
        {
            TCPIP_HTTP_SSI_HASH_ENTRY*  pHE = (TCPIP_HTTP_SSI_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(pInstance->ssiHashDcpt, varName);
            if(pHE)
            {   // found variable
                if(pVarDcpt)
                {
                    pVarDcpt->argType = (TCPIP_HTTP_DYN_ARG_TYPE)pHE->varType;
                    if(pHE->varType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
                    {
                        pVarDcpt->argInt32 = pHE->valInt;
                    }
                    else
                    {
                        pVarDcpt->argStr = pHE->varStr;
                    }
                }
                return true;
            }
        }
    }

    return false;
}

bool TCPIP_HTTP_SSIVariableSet(size_t instIx, const char* varName, const TCPIP_HTTP_DYN_ARG_DCPT* pVarDcpt)
{
    if(pVarDcpt != 0 && instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);
        OA_HASH_DCPT* ssiHashDcpt = pInstance->ssiHashDcpt;
        TCPIP_HTTP_SSI_HASH_ENTRY*  pHE = 0;

        if(ssiHashDcpt == 0)
        {
            if((ssiHashDcpt = _HTTP_SSICreateHash(pInstance)) == 0)
            {
                _HTTP_Report_ConnectionEvent(0, TCPIP_HTTP_EVENT_SSI_HASH_CREATE_FAILED, 0);
            }
            pInstance->ssiHashDcpt = ssiHashDcpt;
        }

        if(ssiHashDcpt != 0)
        {
            pHE = (TCPIP_HTTP_SSI_HASH_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(ssiHashDcpt, varName);
            if(pHE)
            {   // found/created variable
                pHE->varType = pVarDcpt->argType;
                strncpy(pHE->varStr, pVarDcpt->argStr, sizeof(pHE->varStr) - 1);
                pHE->varStr[sizeof(pHE->varStr) - 1] = 0;
                if(pHE->varType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
                {
                    pHE->valInt = pVarDcpt->argInt32;
                }
                return true;
            }
        }
    }

    return false;
}

bool TCPIP_HTTP_SSIVariableDelete(size_t instIx, const char* varName)
{
    if(varName != 0 && instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);
        OA_HASH_DCPT* ssiHashDcpt = pInstance->ssiHashDcpt;

        if(ssiHashDcpt != 0)
        {
            TCPIP_HTTP_SSI_HASH_ENTRY*  pHE = (TCPIP_HTTP_SSI_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(ssiHashDcpt, varName);
            if(pHE)
            {   // found variable
                pHE->varName[0] = 0;
                TCPIP_OAHASH_EntryRemove(ssiHashDcpt, &pHE->hEntry);
                return true;
            }
        }
    }

    return false;
}

size_t TCPIP_HTTP_SSIVariablesNumberGet(size_t instIx, size_t* pMaxNo)
{
    if(instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);
        OA_HASH_DCPT* ssiHashDcpt = pInstance->ssiHashDcpt;

        if(ssiHashDcpt != 0)
        {
            if(pMaxNo)
            {
                *pMaxNo = ssiHashDcpt->hEntries;
            }

            return ssiHashDcpt->fullSlots;
        }
    }

    if(pMaxNo)
    {
        *pMaxNo = 0;
    }
    return 0;

}

bool TCPIP_HTTP_SSIVariableGetByIndex(size_t instIx, size_t varIndex, const char** pVarName, TCPIP_HTTP_DYN_ARG_DCPT* pVarDcpt)
{
    if(instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);
        OA_HASH_DCPT* ssiHashDcpt = pInstance->ssiHashDcpt;

        if(ssiHashDcpt != 0)
        {
            TCPIP_HTTP_SSI_HASH_ENTRY*  pHE = (TCPIP_HTTP_SSI_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(ssiHashDcpt, varIndex);
            if(pHE && pHE->hEntry.flags.busy != 0)
            {   // found variable
                if(pVarDcpt)
                {
                    pVarDcpt->argType = (TCPIP_HTTP_DYN_ARG_TYPE)pHE->varType;
                    if(pHE->varType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
                    {
                        pVarDcpt->argInt32 = pHE->valInt;
                    }
                    else
                    { 
                        pVarDcpt->argStr = pHE->varName;
                    }
                }
                return true;
            }
        }
    }

    return false;
}


// performs a simple linear search for the supported SSI commands
// returns a valid pointer if the entry corresponding to the SSI command was found
// 0 otherwise
static const TCPIP_HTTP_SSI_PROCESS_ENTRY*  _HTTP_SSIFindEntry(const char* ssiCmd)
{

    int ix;

    const TCPIP_HTTP_SSI_PROCESS_ENTRY* pEntry = _HTTP_SSIProc_Tbl;

    for(ix = 0; ix < sizeof(_HTTP_SSIProc_Tbl) / sizeof(*_HTTP_SSIProc_Tbl); ix++, pEntry++)
    {
        if(strcmp(ssiCmd, pEntry->ssiCmd) == 0)
        {   // found
            return pEntry;
        }
    }

    return 0;
}

static OA_HASH_DCPT* _HTTP_SSICreateHash(TCPIP_HTTP_INST* pInstance)
{
    OA_HASH_DCPT*   pSsiHash;

    pSsiHash = (OA_HASH_DCPT*)http_mallocFnc(sizeof(OA_HASH_DCPT) + TCPIP_HTTP_SSI_VARIABLES_NUMBER * sizeof(TCPIP_HTTP_SSI_HASH_ENTRY));
    if(pSsiHash != 0)
    {   // success; populate the entries
        pSsiHash->memBlk = pSsiHash + 1;
        pSsiHash->hParam = 0;    // not used for now
        pSsiHash->hEntrySize = sizeof(TCPIP_HTTP_SSI_HASH_ENTRY);
        pSsiHash->hEntries = TCPIP_HTTP_SSI_VARIABLES_NUMBER;
        pSsiHash->probeStep = TCPIP_HTTP_SSI_HASH_PROBE_STEP;

#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
        // dynamic manipulation should be enabled by default
        pSsiHash->hashF = TCPIP_HTTP_SSI_HashKeyHash;
#if defined(OA_DOUBLE_HASH_PROBING)
        pSsiHash->probeHash = TCPIP_HTTP_SSI_HashProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
        pSsiHash->delF = 0;      // we don't delete variables unless requested
        pSsiHash->cmpF = TCPIP_HTTP_SSI_HashKeyCompare;
        pSsiHash->cpyF = TCPIP_HTTP_SSI_HashKeyCopy; 
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
        TCPIP_OAHASH_Initialize(pSsiHash);
    }

    return pSsiHash;
}

// dynamic manipulation should be enabled by default
#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
static size_t TCPIP_HTTP_SSI_HashKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32_hash((const char*)key, strlen((const char*)key)) % (pOH->hEntries);
}

#if defined(OA_DOUBLE_HASH_PROBING)
static size_t TCPIP_HTTP_SSI_HashProbeHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32a_hash((const char*)key, strlen((const char*)key)) % (pOH->hEntries);
}
#endif  // defined(OA_DOUBLE_HASH_PROBING)
static int TCPIP_HTTP_SSI_HashKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    TCPIP_HTTP_SSI_HASH_ENTRY* pHE = (TCPIP_HTTP_SSI_HASH_ENTRY*)hEntry;
    
    return strcmp(pHE->varName, (const char*)key);
}

static void TCPIP_HTTP_SSI_HashKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    TCPIP_HTTP_SSI_HASH_ENTRY* pHE = (TCPIP_HTTP_SSI_HASH_ENTRY*)dstEntry;

    strncpy(pHE->varName, (const char*)key, sizeof(pHE->varName) - 1);
    pHE->varName[sizeof(pHE->varName) - 1] = 0;
}

#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )


#endif // (TCPIP_HTTP_SSI_PROCESS != 0)

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0)
// adds a dynamic data chunk to this connection
// returns: 
//  TCPIP_HTTP_CHUNK_RES_OK - everything OK, needs to be processed
//  TCPIP_HTTP_CHUNK_RES_WAIT - chunk pool is empty, wait needed
//  < 0         - error, the dynamic chunk is abandoned
// uses the file descriptor from where this originated...
static TCPIP_HTTP_CHUNK_RES _HTTP_AddDynChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt)
{
    TCPIP_HTTP_CHUNK_FLAGS dataFlags;
    TCPIP_HTTP_CHUNK_DCPT* pDynChDcpt = 0;
    TCPIP_HTTP_CHUNK_RES chunkRes = TCPIP_HTTP_CHUNK_RES_OK;
    TCPIP_HTTP_EVENT_TYPE evType = TCPIP_HTTP_EVENT_NONE;
    const void* evInfo = pFileChDcpt->chunkFName;
    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 

    while(true)
    {

        if(TCPIP_Helper_SingleListCount(&pHttpCon->chunkList) >= pHttpCon->connInstance->chunksDepth)
        {   // already at max depth
            evType = TCPIP_HTTP_EVENT_DEPTH_ERROR;
            chunkRes = TCPIP_HTTP_CHUNK_RES_DEPTH_ERR;
            break;
        }

        // valid dynamic variable, try to get a chunk
        dataFlags = TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA;
#if (TCPIP_HTTP_SSI_PROCESS != 0)
        if((pFileChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI) != 0)
        {
            dataFlags |= TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI;
        }
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)

        pDynChDcpt = _HTTP_AllocChunk(pHttpCon, dataFlags, false, &evType);

        if(pDynChDcpt == 0)
        {
            chunkRes = (evType < 0) ? TCPIP_HTTP_CHUNK_RES_RETRY_ERR : TCPIP_HTTP_CHUNK_RES_WAIT;
            break;
        }

        pDynChDcpt->pRootDcpt = pFileChDcpt->pRootDcpt;   // share the same root

        strncpy(pDynChDcpt->chunkFName, pFileChDcpt->chunkFName, sizeof(pDynChDcpt->chunkFName) - 1);
        pDynChDcpt->chunkFName[sizeof(pDynChDcpt->chunkFName) - 1] = 0;

#if (TCPIP_HTTP_SSI_PROCESS != 0)
        if((dataFlags & TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI) != 0)
        {
            if(!_HTTP_SSIExtract(pHttpCon, pDynChDcpt, pFileChDcpt))
            {
                chunkRes = TCPIP_HTTP_CHUNK_RES_SSI_ERR;
            }
        }
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
        if((dataFlags & TCPIP_HTTP_CHUNK_FLAG_TYPE_DATA_SSI) == 0)
        {   // dynamic data chunk; store the owner
            if(pInstance->httpRegistry.dynamicPrint == 0)
            {   // use the default processing, if any;
                // don't allow changing in the middle of the variable processing!
                pDynChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEFAULT_PROCESS;
            }

            if(_HTTP_DynVarExtract(pHttpCon, pDynChDcpt, pFileChDcpt))
            {
                pDynChDcpt->dynChDcpt.pDynAllocDcpt->dynDcpt.callbackID = pFileChDcpt->fileChDcpt.fDynCount++;
            }
            else
            {   // processing failed
                chunkRes = TCPIP_HTTP_CHUNK_RES_DYNVAR_ERR;
            }
        }
#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

        break;
    }

    if(chunkRes < 0)
    {   // some error
        if(pDynChDcpt != 0)
        {
            _HTTP_FreeChunk(pHttpCon, pDynChDcpt);
        }
    }

    if(evType != TCPIP_HTTP_EVENT_NONE)
    {
        _HTTP_Report_ConnectionEvent(pHttpCon, evType, evInfo);
    }

    return chunkRes;
}

// detects the type of an argument to be a TCPIP_HTTP_DYN_ARG_TYPE_INT32 or TCPIP_HTTP_DYN_ARG_TYPE_STRING
// returns the detected type
// updates the pIntArg if the argument is of type int
static TCPIP_HTTP_DYN_ARG_TYPE _HTTP_ArgType(char* argStr, int32_t* pIntArg)
{
    int     len;
    bool    incAdj;
    int32_t argInt;

    if(argStr == 0 || (len = strlen(argStr)) == 0)
    {   // nothing there
        return TCPIP_HTTP_DYN_ARG_TYPE_INVALID;
    }

    len--;
    if(argStr[len] == '0')
    {   // detect a 0 returned by the atoi
        argStr[len] = '1';
        incAdj = true;
    }
    else
    {
        incAdj = false;
    }

    argInt = atoi(argStr);
    if(incAdj)
    {   // restore the original value 
        argStr[len] = '0';
    }

    if(argInt == 0)
    {   // string: not an int
        return TCPIP_HTTP_DYN_ARG_TYPE_STRING;
    }

    // int arg
    if(incAdj)
    {
        argInt += argInt >= 0 ? -1 : 1;
    }
    *pIntArg = argInt;
    return TCPIP_HTTP_DYN_ARG_TYPE_INT32;
} 

#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0)


size_t TCPIP_HTTP_ConnectionDynamicDescriptors(size_t instIx)
{
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    if(instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);
        return pInstance->portDcpt ? pInstance->dynDescriptorsNo : 0;
    }
    return 0;
#else
    return 0;
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0)
}


static void _HTTP_Report_ConnectionEvent(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_EVENT_TYPE evType, const void* evInfo)
{
    const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
    if(httpUserCback->eventReport != 0)
    {
        (*httpUserCback->eventReport)(pHttpCon, evType, evInfo, httpUserCback);
    }
}


bool TCPIP_HTTP_InfoGet(size_t instIx, size_t portIx, size_t connIx, TCPIP_HTTP_CONN_INFO* pHttpInfo)
{
    TCPIP_HTTP_CONN* pHttpCon;

    if(instIx >= httpInstNo)
    {
        return false;
    }

    TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);
    if(portIx >= pInstance->nPorts)
    {
        return false;
    }

    TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt + portIx;
    if(connIx >= pPort->connNo)
    {
        return false;
    }

    pHttpCon = pPort->connCtrl + connIx;
    if(pHttpInfo)
    {
        pHttpInfo->httpStatus = pHttpCon->httpStatus;
        pHttpInfo->listenPort = pPort->listenPort;
        pHttpInfo->connStatus = pHttpCon->connState;
        pHttpInfo->nChunks = TCPIP_Helper_SingleListCount(&pHttpCon->chunkList);
        pHttpInfo->chunkPoolEmpty = pHttpCon->chunkPoolEmpty;
        pHttpInfo->fileBufferPoolEmpty = pHttpCon->fileBufferPoolEmpty;
    }

    return true;
}

#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_CHUNK_INFO) != 0)
bool TCPIP_HTTP_ChunkInfoGet(size_t instIx, size_t portIx, size_t connIx, TCPIP_HTTP_CHUNK_INFO* pChunkInfo, int nInfos)
{
    TCPIP_HTTP_PORT_DCPT* pPort;
    TCPIP_HTTP_CONN* pHttpCon;
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt;
    TCPIP_HTTP_DYN_VAR_DCPT* pDynDcpt;

    if(instIx >= httpInstNo)
    {
        return false;
    }

    TCPIP_HTTP_INST* pInstance = httpInstances[instIx];
    if(portIx >= pInstance->nPorts)
    {
        return false;
    }

    pPort = pInstance->portDcpt + portIx;

    if(connIx >= pPort->connNo)
    {
        return false;
    }

    pHttpCon = pPort->connCtrl + connIx;

    if(pChunkInfo)
    {   // fill data for each chunk
        for(pChDcpt = (TCPIP_HTTP_CHUNK_DCPT*)pHttpCon->chunkList.head; pChDcpt != 0 && nInfos != 0; pChDcpt = pChDcpt->next, nInfos--, pChunkInfo++)
        {
            pChunkInfo->flags = pChDcpt->flags;
            pChunkInfo->status =  pChDcpt->status;
            strncpy(pChunkInfo->chunkFName, pChDcpt->chunkFName, sizeof(pChunkInfo->chunkFName));

            pChunkInfo->dynVarName[0] = 0;
            pChunkInfo->nDynBuffers = 0;
            if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TYPE_FILE) == 0)
            {   // data chunk
                if(pChDcpt->dynChDcpt.pDynAllocDcpt != 0)
                {
                    pDynDcpt = &pChDcpt->dynChDcpt.pDynAllocDcpt->dynDcpt;
                    if(pDynDcpt->dynName != 0)
                    {
                        strncpy(pChunkInfo->dynVarName, pDynDcpt->dynName, sizeof(pChunkInfo->dynVarName));
                    }
                }
                pChunkInfo->nDynBuffers = TCPIP_Helper_SingleListCount(&pChDcpt->dynChDcpt.dynBuffList);
            }
        }
    }

    return true;
}
#else
bool TCPIP_HTTP_ChunkInfoGet(size_t instIx, size_t portIx, size_t connIx, TCPIP_HTTP_CHUNK_INFO* pChunkInfo, int nInfos)
{
    return false;
}
#endif  // (TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_CHUNK_INFO)

bool TCPIP_HTTP_StatsticsGet(size_t instIx, size_t portIx, TCPIP_HTTP_STATISTICS* pStatInfo)
{
    if(pStatInfo && instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = httpInstances[instIx];
        if(portIx < pInstance->nPorts)
        {
            TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt + portIx;

            pStatInfo->nConns = pPort->connNo;
            pStatInfo->nActiveConns = TCPIP_HTTP_ActiveConnectionCountGet(instIx, portIx, &pStatInfo->nOpenConns);
            pStatInfo->dynPoolEmpty = pInstance->dynPoolEmpty;
            pStatInfo->maxRecurseDepth = pInstance->maxRecurseDepth;
            pStatInfo->dynParseRetry = pInstance->dynVarRetries;

            return true;
        }
    }
    return false;
}


// discards pending data for a HTTP connection
static uint16_t _HTTP_ConnectionDiscard(TCPIP_HTTP_CONN* pHttpCon, uint16_t discardLen)
{
    uint8_t discardBuff[TCPIP_HTTP_MAX_DATA_LEN];

    int nDiscards = discardLen / sizeof(discardBuff);
    int nLeft = discardLen - nDiscards * sizeof(discardBuff);
    uint16_t totDiscard = 0;


    while(nDiscards--)
    {
        totDiscard += NET_PRES_SocketRead(pHttpCon->socket, discardBuff, sizeof(discardBuff));
    }
    
    if(nLeft)
    {
        totDiscard += NET_PRES_SocketRead(pHttpCon->socket, discardBuff, nLeft);
    }

    return totDiscard;
}

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
// internal HTTP malloc, thread protection
static void* _HTTP_Default_MallocFnc(size_t bytes)
{
    return httpMemH == 0 ? 0 : TCPIP_HEAP_Malloc(httpMemH, bytes);
}

// internal HTTP free, thread protection
static void  _HTTP_Default_FreeFnc(void* ptr)
{
    if(httpMemH != 0)
    {
        TCPIP_HEAP_Free(httpMemH, ptr);
    }
}
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)

#if (_TCPIP_HTTP_USE_ACCESS_RULES != 0)
// TODO: debug test function
TCPIP_HTTP_ACCESS_RULE* _HTTP_TestAccess(size_t instIx, size_t portIx, char* pageName, TCPIP_HTTP_ACCESS_RULE* accRule)
{
    size_t ruleIx;
    char pagePath[SYS_FS_FILE_NAME_LEN + 1];
    char rulePath[SYS_FS_FILE_NAME_LEN + 1];
                                //

    if(instIx >= httpInstNo)
    {
        return 0;
    }


    const TCPIP_HTTP_INST* pInst = *(httpInstances + instIx);
    if(portIx >= pInst->nPorts)
    {
        return 0;
    }

    if(pInst->nRules == 0)
    {   // if there are no rules, simply grant access!
        accRule->action = TCPIP_HTTP_ACCESS_ACTION_ALLOW;
        return accRule;
    }

    // when are rules present the default action is block
    accRule->action = TCPIP_HTTP_ACCESS_ACTION_BLOCK;
    TCPIP_HTTP_PORT_DCPT* pPort = pInst->portDcpt + portIx;

    SYS_FS_SHELL_RES fsRes = SYS_FS_Shell_GetPath(pPort->fileShell, pageName, pagePath, sizeof(pagePath) / sizeof(*pagePath));
    if(fsRes != SYS_FS_SHELL_RES_OK)
    {   // no such page exist?
        return accRule;
    }

    // evaluate the rules, including the default ones
    TCPIP_HTTP_ACCESS_RULE* pRule = (TCPIP_HTTP_ACCESS_RULE*)pInst->rules; 
    for(ruleIx = 0; ruleIx < pInst->nRules; ruleIx++)
    {
        // ignore inPort, inIfIx, inAddType
#if 0
        if(pRule->inPort == 0 || pRule->inPort == pPort->listenPort)
        {   // port match
            if(pRule->inIfIx < 0 || pRule->inIfIx == inIx)
            {   // interface match
                if(pRule->inAddType == 0 || pRule->inAddType == inAddType)
                {   // add type match
#endif
                    // rule applies to this page
                    fsRes = SYS_FS_Shell_GetPath(pPort->fileShell, pRule->dir, rulePath, sizeof(rulePath) / sizeof(*rulePath));
                    if(fsRes == SYS_FS_SHELL_RES_OK)
                    {   // rule in the page path; consider rule
                        // check if the rulePath and the pagePath intersect: rulePath is part of the pagePath
                        char* subDir = strstr(pagePath, rulePath);
                        if(subDir != 0)
                        {   // rule does apply
                            *accRule = *pRule;
                            return accRule;
                        }
                    }
#if 0
                }
            }
        }
#endif
        // next rule
        pRule = (TCPIP_HTTP_ACCESS_RULE*)((uint8_t*)pRule + pRule->ruleSize);
    }
        
    // no rule matched
    // the default rule is block
    return accRule;
    
}
#endif  // (_TCPIP_HTTP_USE_ACCESS_RULES != 0)

#endif  // defined(TCPIP_STACK_USE_HTTP_SERVER_V2)

