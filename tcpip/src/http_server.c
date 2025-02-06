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
Copyright (C) 2023-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#define TCPIP_HTTP_CRLF_LEN     2U      // size 
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

// conversion functions/helpers
//
static __inline__ TCPIP_HTTP_FILE_BUFF_DCPT* __attribute__((always_inline)) FC_SglNode2BuffDcpt(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_HTTP_FILE_BUFF_DCPT* pBuffDcpt;
    }U_SGL_NODE_BUFF_DCPT;

    U_SGL_NODE_BUFF_DCPT.node = node;
    return U_SGL_NODE_BUFF_DCPT.pBuffDcpt;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_BuffDcpt2SglNode(TCPIP_HTTP_FILE_BUFF_DCPT* pBuffDcpt)
{
    union
    {
        TCPIP_HTTP_FILE_BUFF_DCPT* pBuffDcpt;
        SGL_LIST_NODE*  node;
    }U_BUFF_DCPT_SGL_NODE;

    U_BUFF_DCPT_SGL_NODE.pBuffDcpt = pBuffDcpt;
    return U_BUFF_DCPT_SGL_NODE.node;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_DynVarDcpt2SglNode(TCPIP_HTTP_DYNVAR_BUFF_DCPT* pDynDcpt)
{
    union
    {
        TCPIP_HTTP_DYNVAR_BUFF_DCPT* pDynDcpt;
        SGL_LIST_NODE*  node;
    }U_DYNVAR_DCPT_SGL_NODE;

    U_DYNVAR_DCPT_SGL_NODE.pDynDcpt = pDynDcpt;
    return U_DYNVAR_DCPT_SGL_NODE.node;
}

static __inline__ TCPIP_HTTP_DYNVAR_BUFF_DCPT* __attribute__((always_inline)) FC_SglNode2DynVarDcpt(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_HTTP_DYNVAR_BUFF_DCPT* pBuffDcpt;
    }U_SGL_NODE_DYN_VAR_DCPT;

    U_SGL_NODE_DYN_VAR_DCPT.node = node;
    return U_SGL_NODE_DYN_VAR_DCPT.pBuffDcpt;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_CunkDcpt2SglNode(TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    union
    {
        TCPIP_HTTP_CHUNK_DCPT* pChDcpt;
        SGL_LIST_NODE*  node;
    }U_CHUNK_DCPT_SGL_NODE;

    U_CHUNK_DCPT_SGL_NODE.pChDcpt = pChDcpt;
    return U_CHUNK_DCPT_SGL_NODE.node;
}

static __inline__ TCPIP_HTTP_CHUNK_DCPT* __attribute__((always_inline)) FC_SglNode2CunkDcpt(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_HTTP_CHUNK_DCPT* pChDcpt;
    }U_SGL_NODE_CHUNK_DCPT;

    U_SGL_NODE_CHUNK_DCPT.node = node;
    return U_SGL_NODE_CHUNK_DCPT.pChDcpt;
}

static __inline__ TCPIP_HTTP_CHUNK_DCPT* __attribute__((always_inline)) FC_DynCtxt2CunkDcpt(const void* dynCtxt)
{
    union
    {
        const void* dynCtxt;
        TCPIP_HTTP_CHUNK_DCPT* pChDcpt;
    }U_V_CTXT_CHUNK_DCPT;

    U_V_CTXT_CHUNK_DCPT.dynCtxt = dynCtxt;
    return U_V_CTXT_CHUNK_DCPT.pChDcpt;
}

static __inline__ S_TCPIP_HTTP_ACC_RULE* __attribute__((always_inline)) FC_AccRuleInc(S_TCPIP_HTTP_ACC_RULE* accRule, uint16_t incSize)
{
    union
    {
        S_TCPIP_HTTP_ACC_RULE*  accRule;
        uint8_t*    u8Ptr;
    }U_ACC_RULE_INC;

    U_ACC_RULE_INC.accRule = accRule;
    U_ACC_RULE_INC.u8Ptr += incSize;
    return U_ACC_RULE_INC.accRule;
}

static __inline__ TCPIP_HTTP_CONN* __attribute__((always_inline)) FC_ConnHndl2ConnPtr(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    union
    {
        TCPIP_HTTP_CONN_HANDLE connHandle;
        TCPIP_HTTP_CONN*     pHttpCon;
    }U_CONN_HNDL_CONN_PTR;

    U_CONN_HNDL_CONN_PTR.connHandle = connHandle;
    return U_CONN_HNDL_CONN_PTR.pHttpCon;
}

static __inline__ TCPIP_HTTP_SSI_HASH_ENTRY* __attribute__((always_inline)) FC_OaHash2SsiHash(OA_HASH_ENTRY* hE)
{
    union
    {
        OA_HASH_ENTRY*  hE;
        TCPIP_HTTP_SSI_HASH_ENTRY* ssiHE;
    }U_OA_SSI_HASH;

    U_OA_SSI_HASH.hE = hE;
    return U_OA_SSI_HASH.ssiHE;
}

static __inline__ TCPIP_HTTP_CONN_STATE __attribute__((always_inline)) FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE connState, int incVal)
{
    union
    {
        TCPIP_HTTP_CONN_STATE  connState;
        int     iVal;
    }U_CONN_STATE_INC;

    U_CONN_STATE_INC.connState = connState;
    U_CONN_STATE_INC.iVal += incVal;
    return U_CONN_STATE_INC.connState;
}

static __inline__ uint16_t __attribute__((always_inline)) FC_Min(uint16_t a, uint16_t b)
{
    return (a < b) ? a : b;
}


// functions for handling special header messages  (Corresponding to TCPIP_HTTP_STATUS)

static int F_HTTP_HeaderMsg_Generic(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, size_t bufferSize);
static int F_HTTP_HeaderMsg_NotFound(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, size_t bufferSize);
static int F_HTTP_HeaderMsg_Redirect(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, size_t bufferSize);

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
static int F_HTTP_HeaderMsg_UploadForm(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, size_t bufferSize);
static int F_HTTP_HeaderMsg_UploadError(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, size_t bufferSize);
#endif

// header message function
typedef int(*F_HTTP_HeaderMsgFnc)(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, size_t bufferSize);

// header message printing helper
static int F_HTTP_HeaderMsg_Print(char* dataBuff, size_t bufferSize, const char* fmt, ...);

static const F_HTTP_HeaderMsgFnc HTTPResponseFunctions[] =
{
    
    NULL,                             // TCPIP_HTTP_STAT_GET
    NULL,                             // TCPIP_HTTP_STAT_POST
    &F_HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_BAD_REQUEST
    &F_HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_UNAUTHORIZED
    &F_HTTP_HeaderMsg_NotFound,       // TCPIP_HTTP_STAT_NOT_FOUND
    &F_HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_OVERFLOW
    &F_HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_INTERNAL_SERVER_ERROR
    &F_HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_NOT_IMPLEMENTED
    &F_HTTP_HeaderMsg_Redirect,       // TCPIP_HTTP_STAT_REDIRECT
    &F_HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_TLS_REQUIRED
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    &F_HTTP_HeaderMsg_UploadForm,     // TCPIP_HTTP_STAT_UPLOAD_FORM
    NULL,                             // TCPIP_HTTP_STAT_UPLOAD_STARTED
    NULL,                             // TCPIP_HTTP_STAT_UPLOAD_WRITE
    NULL,                             // TCPIP_HTTP_STAT_UPLOAD_WRITE_WAIT
    &F_HTTP_HeaderMsg_Generic,        // TCPIP_HTTP_STAT_UPLOAD_OK
    &F_HTTP_HeaderMsg_UploadError,    // TCPIP_HTTP_STAT_UPLOAD_ERROR
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

static TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_NET_DefaultIncludeFile(TCPIP_HTTP_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const struct S_tag_TCPIP_HTTP_USER_CALLBACK* pCBack);

static const TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY httpDynVarKeywords[] = 
{
    // keyWord      keyFlags                                        // dynamicPrint
    { "inc",        TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEF_PROCESS,   &TCPIP_HTTP_NET_DefaultIncludeFile},


};

   
/****************************************************************************
  Section:
    HTTP Connection State Global Variables
  ***************************************************************************/
static TCPIP_HTTP_INST** httpInstances = NULL;
static size_t               httpInstNo = 0;         // number of present instances

static int                  httpInitCount = 0;      // module init counter


                                                        
static TCPIP_SIGNAL_HANDLE    httpSignalHandle = NULL;

static const void*          httpMemH = NULL;              // handle to be used in the TCPIP_HEAP_ calls

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
static void* F_HTTP_Default_MallocFnc(size_t bytes); // internal HTTP malloc, thread protection
static void  F_HTTP_Default_FreeFnc(void* ptr);      // internal HTTP free, thread protection

// current HTTP allocation functions
static void* (*http_mallocFnc)(size_t bytes) = NULL;
static void  (*http_freeFnc)(void* ptr) = NULL;

#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)

/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/
static bool F_HTTP_VerifyConfig(const TCPIP_HTTP_SERVER_MODULE_CONFIG* httpInitData);
static TCPIP_HTTP_INST_RES F_HTTP_InitInstance(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg);
static TCPIP_HTTP_INST_RES F_HTTP_InitInstance_Ports(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg);
static TCPIP_HTTP_INST_RES F_HTTP_CreatePortConnections(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_PORT_CONFIG* pCfgPort, TCPIP_HTTP_PORT_DCPT* pPort);
#if (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)
static TCPIP_HTTP_INST_RES F_HTTP_CreateAccessRules(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg);
static void F_HTTP_GetPageAccess(TCPIP_HTTP_CONN* pHttpCon, char* pageName, TCPIP_HTTP_ACCESS_RULE* accRule);
static bool F_HTTP_Get_RedirectURI(const char* accessDir, const char* redirURI, char* redirBuff, size_t buffSize); 
static bool F_HTTP_InstanceOwnsRule(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_ACCESS_RULE* pRule);
#endif  //  // (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)

static bool F_HTTP_HeaderParseLookup(TCPIP_HTTP_CONN* pHttpCon, int reqIx);
#if defined(TCPIP_HTTP_USE_COOKIES)
static bool F_HTTP_HeaderParseCookie(TCPIP_HTTP_CONN* pHttpCon);
#endif
#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
static bool F_HTTP_HeaderParseAuthorization(TCPIP_HTTP_CONN* pHttpCon);
#endif
#if defined(TCPIP_HTTP_USE_POST)
static bool F_HTTP_HeaderParseContentLength(TCPIP_HTTP_CONN* pHttpCon);
static TCPIP_HTTP_READ_STATUS F_HTTP_ReadTo(TCPIP_HTTP_CONN* pHttpCon, char cDelim, uint8_t* cData, uint16_t wLen);
#endif
static uint16_t F_HTTP_SktFifoRxFree(NET_PRES_SKT_HANDLE_T skt);

static bool F_HTTP_DataTryOutput(TCPIP_HTTP_CONN* pHttpCon, const char* data, uint16_t dataLen, uint16_t checkLen);

static uint32_t F_HTTP_ConnectionStringFind(TCPIP_HTTP_CONN* pHttpCon, const char* str, uint16_t startOffs, uint16_t searchLen);

static uint16_t F_HTTP_ConnectionCharFind(TCPIP_HTTP_CONN* pHttpCon, char cFind, uint16_t wStart, uint16_t wSearchLen);

static uint16_t F_HTTP_ConnectionDiscard(TCPIP_HTTP_CONN* pHttpCon, uint16_t discardLen);

static void TCPIP_HTTP_NET_Process(void);

static void TCPIP_HTTP_NET_ProcessConnection(TCPIP_HTTP_CONN* pHttpCon);

static void F_HTTPSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T skt, TCPIP_NET_HANDLE hNet, uint16_t sigType, const void* param);

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_HTTP_Cleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);
static void F_HTTP_CloseConnections(TCPIP_HTTP_PORT_DCPT* pPort, TCPIP_NET_IF* pNetIf);
#else
#define F_HTTP_Cleanup(stackCtrl)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void F_HTTP_ConnCloseFile(TCPIP_HTTP_CONN* pHttpCon);

static TCPIP_HTTP_CHUNK_RES F_HTTP_AddFileChunk(TCPIP_HTTP_CONN* pHttpCon, SYS_FS_HANDLE fH, const char* fName, TCPIP_HTTP_CHUNK_DCPT* pOwnDcpt);

static TCPIP_HTTP_CHUNK_DCPT* F_HTTP_AllocChunk(TCPIP_HTTP_CONN* pHttpCon, uint16_t flags, bool appendTail, TCPIP_HTTP_EVENT_TYPE* pEvType);

static void F_HTTP_FreeChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);

static bool F_HTTP_FileTypeIsDynamic(const char* fName);

static TCPIP_HTTP_CHUNK_RES F_HTTP_ProcessChunks(TCPIP_HTTP_CONN* pHttpCon);

static uint16_t F_HTTP_PrependStartHttpChunk(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, uint32_t chunkSize);

static uint16_t F_HTTP_AppendEndHttpChunk(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, TCPIP_HTTP_CHUNK_END_TYPE endType);

static TCPIP_HTTP_CHUNK_RES F_HTTP_ProcessFileChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);

static void F_HTTP_Report_ConnectionEvent(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_EVENT_TYPE evType, const void* evInfo);

static TCPIP_HTTP_CHUNK_RES F_HTTP_IncludeFile(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, const char* fName);

static char* F_HTTP_ProcessFileLine(TCPIP_HTTP_CHUNK_DCPT* pChDcpt, char* lineBuffer, size_t buffLen, char** pDynStart, bool isEof);

static char* F_HTTP_FileLineParse(TCPIP_HTTP_CHUNK_DCPT* pChDcpt, char* lineBuff, char** pEndProcess, bool verifyOnly);

static bool F_HTTP_ConnCleanDisconnect(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_DISCARD_TYPE discardType);

static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessIdle(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ParseRequest(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ParseFileUpload(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ParseFileOpen(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ParseGetArgs(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ParseHeaders(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessAuthenticate(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessGet(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);
    
static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessPost(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ServeHeaders(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ServeCookies(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ServeBodyInit(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ServeBody(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ServeChunks(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessDone(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessError(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessDisconnect(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
static char* F_HTTP_DynVarParse(char* dynVarBuff, char** pEndDyn, bool verifyOnly);
static bool  F_HTTP_DynVarExtract(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pDynChDcpt, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt);
static const TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY* F_HTTP_SearchDynVarKeyEntry(const char* keyword);
static TCPIP_HTTP_CHUNK_RES F_HTTP_ProcessDynVarChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);
static TCPIP_HTTP_CHUNK_RES F_HTTP_DynVarCallback(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);
static bool F_HTTP_DynVarProcess(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);
static TCPIP_HTTP_DYNVAR_BUFF_DCPT* F_HTTP_GetDynBuffDescriptor(TCPIP_HTTP_CHUNK_DCPT* pChDcpt);
static void F_HTTP_ReleaseDynBuffDescriptor(TCPIP_HTTP_INST* pInstance, TCPIP_HTTP_DYNVAR_BUFF_DCPT* pDynDcpt);

static uint16_t F_HTTP_StartHttpChunk(TCPIP_HTTP_CONN* pHttpCon, uint32_t chunkSize);
static uint16_t F_HTTP_EndHttpChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_END_TYPE endType);
#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

#if (TCPIP_HTTP_SSI_PROCESS != 0)
static char*    F_HTTP_SSILineParse(char* lineBuff, char** pEndProcess, bool verifyOnly);
static bool     F_HTTP_SSIExtract(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt);
static const TCPIP_HTTP_SSI_PROCESS_ENTRY*  F_HTTP_SSIFindEntry(const char* ssiCmd);
static TCPIP_HTTP_CHUNK_RES                 F_HTTP_ProcessSSIChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt);
static OA_HASH_DCPT*                        F_HTTP_SSICreateHash(TCPIP_HTTP_INST* pInstance);

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
static TCPIP_HTTP_CHUNK_RES F_HTTP_AddDynChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt);
static TCPIP_HTTP_DYN_ARG_TYPE F_HTTP_ArgType(char* argStr, int32_t* pIntArg);
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0)

// basic HTTP connection process function
// processes a HTTP connection state: TCPIP_HTTP_CONN_STATE
// returns the next connection state needed
// also signals if waiting for resources
// The function should only set *pWait if needed: *pWait is cleared by default;
typedef TCPIP_HTTP_CONN_STATE (*F_HTTP_ConnProcessFunc)(TCPIP_HTTP_CONN* pHttpCon, bool* pWait);

// table with HTTP connection processing functions
static const F_HTTP_ConnProcessFunc  T_HTTP_ConnProcess_Tbl[] =
{
    &F_HTTP_ProcessIdle,                  // TCPIP_HTTP_CONN_STATE_IDLE
    &F_HTTP_ParseRequest,                 // TCPIP_HTTP_CONN_STATE_PARSE_REQUEST,    
    &F_HTTP_ParseFileUpload,              // TCPIP_HTTP_CONN_STATE_PARSE_FUPLOAD,
    &F_HTTP_ParseFileOpen,                // TCPIP_HTTP_CONN_STATE_PARSE_FOPEN,  
    &F_HTTP_ParseGetArgs,                 // TCPIP_HTTP_CONN_STATE_PARSE_GET_ARGS,   
    &F_HTTP_ParseHeaders,                 // TCPIP_HTTP_CONN_STATE_PARSE_HEADERS,    
    &F_HTTP_ProcessAuthenticate,          // TCPIP_HTTP_CONN_STATE_AUTHENTICATE,     
    &F_HTTP_ProcessGet,                   // TCPIP_HTTP_CONN_STATE_PROCESS_GET,      
    &F_HTTP_ProcessPost,                  // TCPIP_HTTP_CONN_STATE_PROCESS_POST,     
    &F_HTTP_ServeHeaders,                 // TCPIP_HTTP_CONN_STATE_SRV_HEADERS,    
    &F_HTTP_ServeCookies,                 // TCPIP_HTTP_CONN_STATE_SRV_COOKIES,    
    &F_HTTP_ServeBodyInit,                // TCPIP_HTTP_CONN_STATE_SRV_BODY_INIT,  
    &F_HTTP_ServeBody,                    // TCPIP_HTTP_CONN_STATE_SRV_BODY,       
    &F_HTTP_ServeChunks,                  // TCPIP_HTTP_CONN_STATE_SRV_CHUNKS,     
    &F_HTTP_ProcessDone,                  // TCPIP_HTTP_CONN_STATE_DONE,             
    &F_HTTP_ProcessError,                 // TCPIP_HTTP_CONN_STATE_ERROR        
    &F_HTTP_ProcessDisconnect,            // TCPIP_HTTP_CONN_STATE_DISCONNECT        
};


#if (TCPIP_HTTP_SSI_PROCESS != 0)

static TCPIP_HTTP_CHUNK_RES F_HTTP_SSIInclude(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, size_t leftAttribs);
static TCPIP_HTTP_CHUNK_RES F_HTTP_SSISet(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, size_t leftAttribs);
static TCPIP_HTTP_CHUNK_RES F_HTTP_SSIEcho(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, size_t leftAttribs);

static const TCPIP_HTTP_SSI_PROCESS_ENTRY  T_HTTP_SSIProc_Tbl[] = 
{
    //  ssiCmd                              // ssiFnc
    {   "include",                          &F_HTTP_SSIInclude},      // SSI line: <!--#include virtual="file_name" -->
                                                                    // or        <!--#include file="file_name" -->
                                                                    // are supported
    {   "set",                              &F_HTTP_SSISet},          // SSI line: <!--#set var="varname" value="varvalue" -->
                                                                    // or        <!--#set var="varname" value="$varvalue" -->  

    {   "echo",                             &F_HTTP_SSIEcho},         // SSI line: <!--#echo var="varname" -->  
};


#endif // (TCPIP_HTTP_SSI_PROCESS != 0)

#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_FILE) != 0)
static void F_HTTP_FileDbgCreate(const char* fname, size_t fsize, const char* type, uint16_t connId)
{
    SYS_CONSOLE_PRINT("File Create: %s, size: %d, type: %s, conn: %d\r\n", fname, fsize, type, connId);
}

static void F_HTTP_FileDbgProcess(const char* fname, size_t fsize, const char* type, uint16_t connId)
{
    SYS_CONSOLE_PRINT("File Processed: %s, size: %d, type: %s, conn: %d\r\n", fname, fsize, type, connId);
}
#else
#define F_HTTP_FileDbgCreate(fname, fsize, type, connId)
#define F_HTTP_FileDbgProcess(fname, fsize, type, connId)
#endif  // (TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_FILE)

#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_DYNVAR) != 0)
static void F_HTTP_DynDbgExtract(const char* dynName, uint8_t nArgs, const char* fileName, uint16_t connId)
{
    SYS_CONSOLE_PRINT("DynVar Extract - name: %s, nArgs: %d, fName: %s, conn: %d\r\n", dynName, nArgs, fileName, connId);
}
static const char* T_HTTP_DYNDBG_RES_TBL[] = 
{
    "done", "default", "proc_again", "again",
};

static void F_HTTP_DynDbgCallback(const char* dynName, TCPIP_HTTP_DYN_PRINT_RES res, uint16_t connId)
{
    SYS_CONSOLE_PRINT("DynVar Callback - name: %s, res: %s, conn: %d\r\n", dynName, T_HTTP_DYNDBG_RES_TBL[(int)res], connId);
}

static void F_HTTP_DynDbgProcess(const char* dynName, size_t buffSize, bool acked, uint16_t connId)
{
    SYS_CONSOLE_PRINT("DynBuff Done - name: %s, size: %d, %s, conn: %d\r\n", dynName, buffSize, acked ? "ack" : "nak", connId);
}
#else
#define F_HTTP_DynDbgExtract(dynName, nArgs, fileName, connId)
#define F_HTTP_DynDbgCallback(dynName, res, connId)
#define F_HTTP_DynDbgProcess(dynName, buffSize, acked, connId)
#endif  // (TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_DYNVAR)

#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_CONN_STATE) != 0)
static const char* const T_HTTP_DbgConnState_Tbl[] = 
{
    "idle",                 // TCPIP_HTTP_CONN_STATE_IDLE,       
    "parse_req",            // TCPIP_HTTP_CONN_STATE_PARSE_REQUEST,   
    "parse_upl",            // TCPIP_HTTP_CONN_STATE_PARSE_FUPLOAD
    "parse_fopen",          // TCPIP_HTTP_CONN_STATE_PARSE_FOPEN, 
    "parse_args",           // TCPIP_HTTP_CONN_STATE_PARSE_GET_ARGS,  
    "parse_head",           // TCPIP_HTTP_CONN_STATE_PARSE_HEADERS,   
    "auth",                 // TCPIP_HTTP_CONN_STATE_AUTHENTICATE,    
    "get",                  // TCPIP_HTTP_CONN_STATE_PROCESS_GET,     
    "post",                 // TCPIP_HTTP_CONN_STATE_PROCESS_POST,    
    "srv_head",             // TCPIP_HTTP_CONN_STATE_SRV_HEADERS,   
    "srv_cook",             // TCPIP_HTTP_CONN_STATE_SRV_COOKIES,   
    "srv_bini",             // TCPIP_HTTP_CONN_STATE_SRV_BODY_INIT, 
    "srv_body",             // TCPIP_HTTP_CONN_STATE_SRV_BODY,      
    "srv_chunks",           // TCPIP_HTTP_CONN_STATE_SRV_CHUNKS,    
    "done",                 // TCPIP_HTTP_CONN_STATE_DONE,            
    "error",                // TCPIP_HTTP_CONN_STATE_ERROR       
    "discon",               // TCPIP_HTTP_CONN_STATE_DISCONNECT       
};

static const char* const T_HTTP_DbgHttpState_Tbl[] = 
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


static uint16_t httpConnState;  // TCPIP_HTTP_CONN_STATE
static void F_HTTP_DbgGetState(TCPIP_HTTP_CONN* pHttpCon)
{
    httpConnState = pHttpCon->connState;
}

static void F_HTTP_DbgNewState(TCPIP_HTTP_CONN* pHttpCon)
{
    if(pHttpCon->connState != httpConnState)
    {
        httpConnState = pHttpCon->connState;
        SYS_CONSOLE_PRINT("HTTP Conn: %d, inst: %d, state: %d-%s, http: %s\r\n", pHttpCon->connIx, pHttpCon->connInstance->instIx, httpConnState, T_HTTP_DbgConnState_Tbl[httpConnState], T_HTTP_DbgHttpState_Tbl[pHttpCon->httpStatus]);
    }
}

#else
#define F_HTTP_DbgGetState(pHttpCon)
#define F_HTTP_DbgNewState(pHttpCon)
#endif  // (TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_CONN_STATE)


#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_DYN_CONTROL) != 0)
static volatile int httpKillDynFiles = 0;
static volatile int httpKillDynVarParser = 0;
static volatile int httpKillUserDynVar = 0;
static volatile int httpKillFlashWrite = 0;

static __inline__ bool __attribute__((always_inline)) F_HTTP_DbgKillDynFiles(void)
{
    return httpKillDynFiles != 0;
} 
static __inline__ bool __attribute__((always_inline)) F_HTTP_DbgKillDynParser(void)
{
    return httpKillDynVarParser != 0;
} 
static __inline__ bool __attribute__((always_inline)) F_HTTP_DbgKillUserDynVar(void)
{
    return httpKillUserDynVar != 0;
} 
static __inline__ bool __attribute__((always_inline)) F_HTTP_DbgKillFlashWrite(void)
{
    return httpKillFlashWrite != 0;
} 
#else
static __inline__ bool __attribute__((always_inline)) F_HTTP_DbgKillDynFiles(void)
{
    return false;
} 
static __inline__ bool __attribute__((always_inline)) F_HTTP_DbgKillDynParser(void)
{
    return false;
} 
static __inline__ bool __attribute__((always_inline)) F_HTTP_DbgKillUserDynVar(void)
{
    return false;
} 
static __inline__ bool __attribute__((always_inline)) F_HTTP_DbgKillFlashWrite(void)
{
    return false;
} 
#endif  // ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_DYN_CONTROL) != 0)

#if (TCPIP_HTTP_SSI_PROCESS != 0) && ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_SSI_HASH) != 0)
static void F_HTTP_DbgSSIHashEntry(TCPIP_HTTP_SSI_HASH_ENTRY* pHE, bool isRef)
{
    if(pHE->varType == (uint8_t)TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        SYS_CONSOLE_PRINT("HTTP Set %s int: %s, value: %s, int: %d\r\n", isRef ? "ref" : "direct", pHE->varName, pHE->varStr, pHE->valInt); 
    }
    else
    {
        SYS_CONSOLE_PRINT("HTTP Set %s str: %s, value: %s\r\n", isRef ? "ref" : "direct", pHE->varName, pHE->varStr);
    }
}
#else
#define F_HTTP_DbgSSIHashEntry(pHE, isRef)
#endif  // (TCPIP_HTTP_SSI_PROCESS != 0) && ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_SSI_HASH) != 0)

#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_CONN_TMO) != 0)
static void F_HTTP_DbgConnTmo(TCPIP_HTTP_CONN* pHttpCon)
{
    SYS_CONSOLE_PRINT("HTTP tmo - conn: %d, active sec: %d, curr sec: %d\r\n", pHttpCon->connIx, pHttpCon->connActiveSec, TCPIP_SecCountGet());
}
#else
#define F_HTTP_DbgConnTmo(pHttpCon)
#endif  // ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_CONN_TMO) != 0)

#if (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)
TCPIP_HTTP_ACCESS_RULE* F_HTTP_TestAccess(size_t instIx, size_t portIx, char* pageName, TCPIP_HTTP_ACCESS_RULE* accRule);
#endif

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
static TCPIP_HTTP_IO_RESULT TCPIP_HTTP_NET_FSUpload(TCPIP_HTTP_CONN* pHttpCon);
#endif

#if (TCPIP_STACK_DOWN_OPERATION != 0)
// if pNetIf == 0, all connections are closed, stack is going down
static void F_HTTP_CloseConnections(TCPIP_HTTP_PORT_DCPT* pPort, TCPIP_NET_IF* pNetIf)
{
    TCPIP_HTTP_CONN* pHttpCon;
    size_t  connIx;
    TCP_SOCKET_INFO sktInfo;
    bool closeSkt;

    pHttpCon = pPort->connCtrl + 0;
    for (connIx = 0; connIx < pPort->connNo; connIx++)
    {
        // Close the connections that were associated with this interface
        if (pHttpCon->netSocket != NET_PRES_INVALID_SOCKET)
        {
            if(pNetIf == NULL)
            {
                closeSkt = true;
            }
            else
            {
                (void)NET_PRES_SocketInfoGet(pHttpCon->netSocket, &sktInfo);
                closeSkt = pNetIf == sktInfo.hNet;
            }

            if(closeSkt)
            {
                F_HTTP_ConnCloseFile(pHttpCon);

                if(pNetIf == NULL)
                {   // stack going down
                    if(pHttpCon->socketSignal != NULL)
                    {
                        (void)NET_PRES_SocketSignalHandlerDeregister(pHttpCon->netSocket, pHttpCon->socketSignal);
                    }
                    NET_PRES_SocketClose(pHttpCon->netSocket);
                    pHttpCon->netSocket = NET_PRES_INVALID_SOCKET;
                }
                else 
                {   // TCPIP_STACK_ACTION_IF_DOWN - interface down
                    (void)NET_PRES_SocketDisconnect(pHttpCon->netSocket);
                }
            }
        }
        pHttpCon++;
    }
}

static void F_HTTP_Cleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    int instIx, portIx;
    TCPIP_HTTP_INST* pInstance;
    TCPIP_HTTP_PORT_DCPT* pPort;
    TCPIP_HTTP_FILE_BUFF_DCPT*  fileBuffDcpt;

    TCPIPStack_Assert(httpMemH == stackCtrl->memH, __FILE__, __func__, __LINE__);

    for(instIx = 0; instIx < httpInstNo; instIx++)
    {
        pInstance = httpInstances[instIx];
        // deallocate ports
        pPort = pInstance->portDcpt;
        for(portIx = 0; portIx < pInstance->nPorts; portIx++)
        {
            if(pPort->connData && pPort->connCtrl)
            {
                F_HTTP_CloseConnections(pPort, NULL);
            }


            if(pPort->connData != NULL)
            {
                (void)TCPIP_HEAP_Free(httpMemH, pPort->connData);
            }
            if(pPort->connCtrl != NULL)
            {
                (void)TCPIP_HEAP_Free(httpMemH, pPort->connCtrl);
            }
            if(pPort->fileShell != NULL)
            {
                (void)(*pPort->fileShell->delete)(pPort->fileShell);
            }
            pPort++;
        }
        (void)TCPIP_HEAP_Free(httpMemH, pInstance->portDcpt);

        // deallocate the dynamic variables
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
        if(pInstance->allocDynDcpt != NULL)
        {
            (void)TCPIP_HEAP_Free(httpMemH, pInstance->allocDynDcpt);
        }
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

        if(pInstance->allocChunks != NULL)
        {
            (void)TCPIP_HEAP_Free(httpMemH, pInstance->allocChunks);
        }

        // deallocate the file buffers
        while((fileBuffDcpt = FC_SglNode2BuffDcpt(TCPIP_Helper_SingleListHeadRemove(&pInstance->fileBuffers))) != NULL)
        {
            (void)TCPIP_HEAP_Free(httpMemH, fileBuffDcpt);
        }

        // deallocate SSI
#if (TCPIP_HTTP_SSI_PROCESS != 0)
        if(pInstance->ssiHashDcpt != NULL)
        {
            TCPIP_OAHASH_EntriesRemoveAll(pInstance->ssiHashDcpt);
            http_freeFnc(pInstance->ssiHashDcpt);
        }
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)

        if(pInstance->rules != NULL)
        {
            (void)TCPIP_HEAP_Free(httpMemH, pInstance->rules);
        }

        // finally delete the instance itself
        (void)TCPIP_HEAP_Free(httpMemH, pInstance);
    }


    // delete httpInstances!
    (void)TCPIP_HEAP_Free(httpMemH, httpInstances);
    httpInstances = NULL;
    httpInstNo = 0;
    httpMemH = NULL;

    if(httpSignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(httpSignalHandle);
        httpSignalHandle = NULL;
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void F_HTTP_ConnCloseFile(TCPIP_HTTP_CONN* pHttpCon)
{
    if(pHttpCon->file != SYS_FS_HANDLE_INVALID)
    {
        const SYS_FS_SHELL_OBJ* fShell = pHttpCon->connPort->fileShell;
        (void)fShell->fileClose(fShell, pHttpCon->file);
        pHttpCon->file = SYS_FS_HANDLE_INVALID;
        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_CLOSE, pHttpCon->fileName);
    }
}

bool TCPIP_HTTP_Server_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
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

        const TCPIP_HTTP_SERVER_MODULE_CONFIG* httpInitData = (const TCPIP_HTTP_SERVER_MODULE_CONFIG*)initData;
        if(!F_HTTP_VerifyConfig(httpInitData))
        {
            SYS_ERROR_PRINT(SYS_ERROR_ERROR, " HTTP: Configuration invalid data: %d\r\n", TCPIP_HTTP_INST_RES_BAD_DATA);
            return false;
        }

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
        // set the allocation functions
        http_mallocFnc = httpInitData->httpMallocFnc;
        http_freeFnc = httpInitData->httpFreeFnc;
        if(http_mallocFnc == NULL || http_freeFnc == NULL)
        {   // use the default stack values
            http_mallocFnc = &F_HTTP_Default_MallocFnc;
            http_freeFnc = &F_HTTP_Default_FreeFnc;
        }
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)

        // allocate instances array
        httpMemH = stackCtrl->memH;
        httpInstances = (TCPIP_HTTP_INST**)TCPIP_HEAP_Calloc(httpMemH, httpInitData->nInstances, sizeof(TCPIP_HTTP_INST*));
        if(httpInstances == NULL)
        {
            SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Instance array allocation failed\r\n");
            return false;
        }

        // allocate instances themselves
        pInstCfg = httpInitData->pInstConfig;
        for(instIx = 0; instIx < httpInitData->nInstances; instIx++)
        {
            pInstance = (TCPIP_HTTP_INST*)TCPIP_HEAP_Calloc(httpMemH, 1, sizeof(*pInstance));
            httpInstances[instIx] = pInstance;
            if(pInstance == NULL)
            {   // failed
                break;
            }
            pInstCfg++;
        }

        if(instIx != httpInitData->nInstances)
        {   // allocation failed
            SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Instance allocation failed\r\n");
            // clean up
            size_t cleanIx;
            for(cleanIx = 0; cleanIx < instIx; cleanIx++)
            {
                pInstance = httpInstances[cleanIx];
                (void)TCPIP_HEAP_Free(httpMemH, pInstance);
            }

            (void)TCPIP_HEAP_Free(httpMemH, httpInstances);
            return false;
        }

        // instances properly allocated
        httpInstNo = httpInitData->nInstances;

        // start populating the instances
        pInstCfg = httpInitData->pInstConfig;
        for(instIx = 0; instIx < httpInstNo; instIx++)
        {
            pInstance = httpInstances[instIx];
            if((instRes = F_HTTP_InitInstance(pInstance, pInstCfg)) != TCPIP_HTTP_INST_RES_OK)
            {   // failed
                SYS_ERROR_PRINT(SYS_ERROR_ERROR, " HTTP: Instance creation %d failed: %d\r\n", instIx, instRes);
                initFail = true;
                break;
            }
            pInstance->instIx = (uint16_t)instIx;
            pInstCfg++;
        }
            
        if(initFail == false)
        {   // create the HTTP timer
            httpSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_HTTP_Server_Task, TCPIP_HTTP_TASK_RATE);
            if(httpSignalHandle == NULL)
            {   // cannot create the HTTP timer
                initFail = true;
            }
        }

        // done
        break;
    }

    if(initFail)
    {
        F_HTTP_Cleanup(stackCtrl);
        return false;
    }

    httpInitCount++;

    TCPIPStack_Condition(true, __FILE__, __func__, __LINE__); // nop; remove not used warning
    return true;    
}

// verify config instances to be OK
static bool F_HTTP_VerifyConfig(const TCPIP_HTTP_SERVER_MODULE_CONFIG* httpInitData)
{
    if(httpInitData == NULL || httpInitData->nInstances == 0 || httpInitData->pInstConfig == NULL)
    {
        return false;
    }

    size_t instIx;
    const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg = httpInitData->pInstConfig;
    for(instIx = 0; instIx < httpInitData->nInstances; instIx++)
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
        if(pInstCfg->portConfig == NULL || pInstCfg->nPorts == 0)
        {
            return false;
        }
        size_t portIx;
        const TCPIP_HTTP_PORT_CONFIG* pPort = pInstCfg->portConfig;

        for(portIx = 0; portIx < pInstCfg->nPorts; portIx++)
        { 
            if(pPort->nConnections == 0 || pPort->webDir == NULL || pPort->dataLen == 0 || (size_t)pPort->addType > IP_ADDRESS_TYPE_IPV6)
            {
                return false;
            }
            pPort++;
        }

        // check the port rules
#if (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)
        size_t ruleIx;
        if(pInstCfg->nRules != 0U)
        {   // have access rules
            if(pInstCfg->accRules == NULL)
            {   // missing rules
                return false;
            }
            const TCPIP_HTTP_ACCESS_RULE* pAccRule = pInstCfg->accRules; 
            for(ruleIx = 0; ruleIx < (size_t)pInstCfg->nRules; ruleIx++)
            {
                if(pAccRule->action != (uint8_t)TCPIP_HTTP_ACCESS_ACTION_NONE)
                {
                    if(pAccRule->dir == NULL || strlen(pAccRule->dir) == 0 || pAccRule->dir[0] != '/')
                    {
                        return false;
                    }

                    if(pAccRule->action == (uint8_t)TCPIP_HTTP_ACCESS_ACTION_REDIRECT)
                    {
                        if(pAccRule->redirServer == NULL || strlen(pAccRule->redirServer) == 0)
                        {
                            return false;
                        }
                        if(pAccRule->redirURI == NULL || strlen(pAccRule->redirURI) == 0 || pAccRule->redirURI[0] != '/')
                        {
                            return false;
                        }
                    }
                }
            }
        }
#endif  //  // (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)
        pInstCfg++;
    }

    // all checks passed
    return true;
}


static TCPIP_HTTP_INST_RES F_HTTP_InitInstance(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg)
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
    fileBufferTotSize = TCPIP_HTTP_CHUNK_HEADER_LEN + pInstCfg->fileBufferSize + TCPIP_HTTP_CHUNK_FINAL_TRAILER_LEN + 1U; 
    for(buffIx = 0; buffIx < pInstCfg->nFileBuffers; buffIx ++)
    {
        fileBuffDcpt = (TCPIP_HTTP_FILE_BUFF_DCPT*)TCPIP_HEAP_Malloc(memH, sizeof(TCPIP_HTTP_FILE_BUFF_DCPT_BARE) + fileBufferTotSize);
        if(fileBuffDcpt != NULL)
        {
            fileBuffDcpt->fileBufferSize = pInstCfg->fileBufferSize;
            fileBuffDcpt->fileBufferTotSize = fileBufferTotSize;
            TCPIP_Helper_SingleListTailAdd(&pInstance->fileBuffers, FC_BuffDcpt2SglNode(fileBuffDcpt));
        }
        else
        {   // failed
            break;
        }
    } 

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    if(pInstance->allocDynDcpt == NULL || pInstance->allocChunks == NULL || buffIx != pInstCfg->nFileBuffers)
#else
    if(pInstance->allocChunks == NULL || buffIx != pInstCfg->nFileBuffers)
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    {   // failed
        SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Dynamic chunks and file buffers allocation failed\r\n");
        return TCPIP_HTTP_INST_RES_ALLOC_ERR;
    }

    pInstance->portDcpt =  (TCPIP_HTTP_PORT_DCPT*)TCPIP_HEAP_Calloc(memH, pInstance->nPorts, sizeof(*pPort));
    if(pInstance->portDcpt == NULL)
    {   // allocation failed
        SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Port descriptor allocation failed\r\n");
        return TCPIP_HTTP_INST_RES_ALLOC_ERR;
    }

    TCPIP_HTTP_INST_RES portRes = F_HTTP_InitInstance_Ports(pInstance, pInstCfg);
    if(portRes != TCPIP_HTTP_INST_RES_OK) 
    {   
        return portRes;
    }

    // create access rules
#if (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)
    TCPIP_HTTP_INST_RES accRes = F_HTTP_CreateAccessRules(pInstance, pInstCfg);
    if(accRes != TCPIP_HTTP_INST_RES_OK)
    {
        return accRes;
    }
#endif  //  // (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)

    // continue initialization 
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    // initialize the dynamic variables buffer pool
    TCPIP_Helper_SingleListInitialize (&pInstance->dynVarPool);
    TCPIP_HTTP_DYNVAR_BUFF_DCPT* pDynDcpt = pInstance->allocDynDcpt; 
    for(connIx = 0; connIx < pInstance->dynDescriptorsNo; connIx++)
    {
        TCPIP_Helper_SingleListTailAdd(&pInstance->dynVarPool, FC_DynVarDcpt2SglNode(pDynDcpt));
        pDynDcpt++;
    } 
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

    // initialize the chunk pool
    TCPIP_Helper_SingleListInitialize (&pInstance->chunkPool);
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt = pInstance->allocChunks; 
    for(connIx = 0; connIx < pInstance->chunksNo; connIx++)
    {
        TCPIP_Helper_SingleListTailAdd(&pInstance->chunkPool, FC_CunkDcpt2SglNode(pChDcpt));
        pChDcpt++;
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
        (void)NET_PRES_SocketInfoGet(pHttpCon->netSocket, &tcpInfo);
        if(tcpInfo.rxSize > maxRxSize)
        {
            maxRxSize = tcpInfo.rxSize;
        }
    }
    pInstance->peekBufferSize = maxRxSize;
#endif  // (TCPIP_HTTP_FIND_PEEK_BUFF_SIZE != 0)

    return TCPIP_HTTP_INST_RES_OK;
}

static TCPIP_HTTP_INST_RES F_HTTP_CreatePortConnections(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_PORT_CONFIG* pCfgPort, TCPIP_HTTP_PORT_DCPT* pPort)
{
    size_t nConns, connIx;
    uint16_t    portFlags; // run time flags
    TCPIP_HTTP_CONN*  pHttpCon;
    uint8_t*    pHttpData;
    TCPIP_NET_HANDLE hNet;
    uint16_t    sktType;

    const void* memH = httpMemH;
    nConns = pPort->connNo;
    pPort->connCtrl = (TCPIP_HTTP_CONN*)TCPIP_HEAP_Calloc(memH, nConns, sizeof(*pPort->connCtrl));
    pPort->connData = (uint8_t*)TCPIP_HEAP_Malloc(memH, nConns *  pPort->connDataSize);
    if(pPort->connCtrl == NULL || pPort->connData == NULL)
    {
        SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Connection control/data allocation failed\r\n");
        return TCPIP_HTTP_INST_RES_ALLOC_ERR;
    }

    // open associated sockets
    if(pCfgPort->listenIfIx < 0)
    {
        hNet = NULL;
    }
    else
    {
        hNet = TCPIP_STACK_IndexToNet((size_t)pCfgPort->listenIfIx);
        if(hNet == NULL)
        {
            return TCPIP_HTTP_INST_RES_BAD_IF; 
        }
    }

    sktType = (uint16_t)NET_PRES_SKT_STREAM | (uint16_t)NET_PRES_SKT_SERVER;
    portFlags = pPort->portFlags;
    if((portFlags & TCPIP_HTTP_PORT_FLAG_SECURE_ON) != 0)
    {   // encrypted
        sktType |= (uint16_t)NET_PRES_SKT_ENCRYPTED;
    }
    else if((portFlags & TCPIP_HTTP_PORT_FLAG_SECURE_OFF) != 0)
    {   // unencrypted
        sktType = (uint16_t)NET_PRES_SKT_UNENCRYPTED;
    }
    else
    {
        // use default port number
    }

    // initialize all connections to a known state
    pHttpCon = pPort->connCtrl + 0;
    for(connIx = 0; connIx < nConns; connIx++)
    {
        pHttpCon->netSocket =  NET_PRES_INVALID_SOCKET;
        pHttpCon->file = SYS_FS_HANDLE_INVALID;
        pHttpCon->connIx = (uint16_t)connIx;
        pHttpCon->connInstance = pInstance;
        pHttpCon->connPort = pPort;

        pHttpCon->connState = (uint16_t)TCPIP_HTTP_CONN_STATE_IDLE;
        pHttpCon++;
    }

    // set sockets and data
    pHttpCon = pPort->connCtrl + 0;
    pHttpData = pPort->connData;
    for(connIx = 0; connIx < nConns; connIx++)
    {
        pHttpCon->netSocket =  NET_PRES_SocketOpen(0, (NET_PRES_SKT_T)sktType, (NET_PRES_SKT_ADDR_T)pCfgPort->addType, pPort->listenPort, NULL, NULL);
        if(pHttpCon->netSocket == NET_PRES_INVALID_SOCKET)
        {   // failed to open the socket
            SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Socket creation failed\r\n");
            return TCPIP_HTTP_INST_RES_SKT_OPEN_ERR; 
        }

        // set socket options
        if((portFlags & TCPIP_HTTP_PORT_FLAG_NO_DELAY) != 0)
        {
            void* tcpForceFlush = FC_Uint2VPtr(1UL);
            (void)NET_PRES_SocketOptionsSet(pHttpCon->netSocket, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_NODELAY, tcpForceFlush);
        }
        if(pCfgPort->sktTxBuffSize != 0)
        {
            void* tcpBuffSize = FC_Uint2VPtr((uint32_t)pCfgPort->sktTxBuffSize);
            if(!NET_PRES_SocketOptionsSet(pHttpCon->netSocket, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, tcpBuffSize))
            {
                SYS_ERROR(SYS_ERROR_WARNING, " HTTP: Setting TX Buffer failed - port: %d, conn: %d\r\n", pPort->portIx, connIx);
                return TCPIP_HTTP_INST_RES_SKT_TX_BUFF_ERR; 
            }
        }
        if(pCfgPort->sktRxBuffSize != 0)
        {
            void* tcpBuffSize = FC_Uint2VPtr((uint32_t)pCfgPort->sktRxBuffSize);
            if(!NET_PRES_SocketOptionsSet(pHttpCon->netSocket, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, tcpBuffSize))
            {
                SYS_ERROR(SYS_ERROR_WARNING, " HTTP: Setting RX Buffer failed - port: %d, conn: %d\r\n", pPort->portIx, connIx);
                return TCPIP_HTTP_INST_RES_SKT_RX_BUFF_ERR; 
            }
        }

        if(hNet != NULL)
        {
            TCP_SOCKET nativeSkt = NET_PRES_SocketGetTransportHandle(pHttpCon->netSocket);
            if(!TCPIP_TCP_SocketNetSet(nativeSkt, hNet, true))
            {
                return TCPIP_HTTP_INST_RES_SKT_NET_ERR; 
            }
        }

        pHttpCon->flags.sktLocalReset = 1;      // socket will start reset

        pHttpCon->socketSignal = NET_PRES_SocketSignalHandlerRegister(pHttpCon->netSocket, (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA, &F_HTTPSocketRxSignalHandler, NULL);
        if(pHttpCon->socketSignal == NULL)
        {
            SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Signal creation failed\r\n");
            return TCPIP_HTTP_INST_RES_SKT_SIGNAL_ERR; 
        }

        pHttpCon->httpData = pHttpData;
        pHttpData += pCfgPort->dataLen;
        pHttpCon++;
    }

    return TCPIP_HTTP_INST_RES_OK; 
}

#if (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)
// checks if a rule applies to the selected instance
static bool F_HTTP_InstanceOwnsRule(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_ACCESS_RULE* pRule)
{
    if(pRule->inPort == 0)
    {   // this rule belongs to all
        return true;
    }

    // browse the ports
    size_t portIx;
    TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt;
    for(portIx = 0; portIx < pInstance->nPorts; portIx++)
    {
        if(pRule->inPort == pPort->listenPort)
        {
            return true;
        }
        pPort++;
    }

    return false;
}

static TCPIP_HTTP_INST_RES F_HTTP_CreateAccessRules(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg)
{
    // calculate the sizes and allocate, populate the rules for this instance
    size_t ruleIx, ruleSz, totRulesSz;
    size_t portIx;
    size_t dirSz, redirSz, srvSz;
    size_t defRules, cfgRules;
    S_TCPIP_HTTP_ACC_RULE* instRule;
    const TCPIP_HTTP_ACCESS_RULE* pCfgRule;
    const TCPIP_HTTP_PORT_CONFIG* pPortCfg;

    const void* memH = httpMemH;

    // check if we have default rules
    defRules = cfgRules = 0;
    pPortCfg = pInstCfg->portConfig;
    for(portIx = 0; portIx < pInstCfg->nPorts; portIx++)
    {
        if((pPortCfg->portFlags & TCPIP_HTTP_PORT_FLAG_NO_DEFAULT_RULE) == 0)
        {
            defRules++;
        }
        pPortCfg++;
    }

    while(((size_t)pInstCfg->nRules + defRules) != 0U)
    {   // calculate the total rules size

        totRulesSz = 0;
        
        if(pInstCfg->nRules != 0U)
        {
            pCfgRule = pInstCfg->accRules;
            for(ruleIx = 0; ruleIx < (size_t)pInstCfg->nRules; ruleIx++)
            {
                if(pCfgRule->action ==  (uint8_t)TCPIP_HTTP_ACCESS_ACTION_NONE)
                {
                    pCfgRule++;
                    continue;
                }

                if(F_HTTP_InstanceOwnsRule(pInstance, pCfgRule))
                {
                    cfgRules++;
                    ruleSz = sizeof(*instRule) + strlen(pCfgRule->dir) + 1;
                    if(pCfgRule->action == (uint8_t)TCPIP_HTTP_ACCESS_ACTION_REDIRECT)
                    {
                        ruleSz += strlen(pCfgRule->redirServer) + 1;    // space for '\0'
                        ruleSz += strlen(pCfgRule->redirURI) + 1;    // space for '\0'
                    }
                    ruleSz = ((ruleSz + sizeof(uint32_t) - 1) / sizeof(uint32_t)) * sizeof(uint32_t);   // make it M4
                    totRulesSz += ruleSz;
                }
                pCfgRule++;
            }
        }

        if(defRules != 0)
        {   // add the default rules with action == TCPIP_HTTP_ACCESS_ACTION_ALLOW: one per port
            pPortCfg = pInstCfg->portConfig;
            for(portIx = 0; portIx < pInstCfg->nPorts; portIx++)
            {
                if((pPortCfg->portFlags & TCPIP_HTTP_PORT_FLAG_NO_DEFAULT_RULE) == 0)
                {
                    ruleSz = sizeof(*instRule) + strlen("/") + 1;
                    ruleSz = ((ruleSz + sizeof(uint32_t) - 1) / sizeof(uint32_t)) * sizeof(uint32_t);   // make it M4
                    totRulesSz += ruleSz;
                }
                pPortCfg++;
            }
        }

        // allocate the rule
        if(totRulesSz == 0)
        {   // only no action rules and no default rules!
            break;
        }

        pInstance->rules = (S_TCPIP_HTTP_ACC_RULE*)TCPIP_HEAP_Malloc(memH, totRulesSz);
        if(pInstance->rules == NULL)
        {
            SYS_ERROR(SYS_ERROR_ERROR, " HTTP: Instance rules allocation failed\r\n");
            return TCPIP_HTTP_INST_RES_ALLOC_ERR; 
        }

        // success; populate the rules
        pCfgRule = pInstCfg->accRules;
        instRule = pInstance->rules;
        for(ruleIx = 0; ruleIx < (size_t)pInstCfg->nRules; ruleIx++)
        {
            if(pCfgRule->action ==  (uint8_t)TCPIP_HTTP_ACCESS_ACTION_NONE)
            {
                pCfgRule++;
                continue;
            }

            if(F_HTTP_InstanceOwnsRule(pInstance, pCfgRule))
            {
                dirSz = strlen(pCfgRule->dir) + 1;
                if(pCfgRule->action ==  (uint8_t)TCPIP_HTTP_ACCESS_ACTION_REDIRECT)
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
                ruleSz = ((ruleSz + sizeof(uint32_t) - 1U) / sizeof(uint32_t)) * sizeof(uint32_t);   // make it M4
                instRule->accRule = *pCfgRule;
                instRule->accRule.ruleSize = (uint16_t)ruleSz;
                char* dirStr = instRule->strSpace;
                (void)strcpy(dirStr, pCfgRule->dir);
                instRule->accRule.dir = dirStr;
                if(redirSz != 0)
                {
                    char* redirStr = dirStr + dirSz;
                    (void)strncpy(redirStr, pCfgRule->redirServer, srvSz);
                    redirStr[srvSz - 1] = '\0'; // end properly
                    instRule->accRule.redirServer = redirStr;
                    redirStr += srvSz;
                    (void)strcpy(redirStr, pCfgRule->redirURI);
                    instRule->accRule.redirURI = redirStr;
                }

                // done; next rule
                instRule = FC_AccRuleInc(instRule, (uint16_t)ruleSz);
            }
            pCfgRule++;
        }

        // add the default rules with action == TCPIP_HTTP_ACCESS_ACTION_ALLOW: one per port
        pPortCfg = pInstCfg->portConfig;
        for(portIx = 0; portIx < pInstCfg->nPorts; portIx++)
        {
            if((pPortCfg->portFlags & TCPIP_HTTP_PORT_FLAG_NO_DEFAULT_RULE) == 0)
            {
                ruleSz = sizeof(*instRule) + strlen("/") + 1;
                ruleSz = ((ruleSz + sizeof(uint32_t) - 1) / sizeof(uint32_t)) * sizeof(uint32_t);   // make it M4
                                                                                                    // populate the default rule
                instRule->accRule.inPort = pPortCfg->listenPort;
                instRule->accRule.inIfIx = pPortCfg->listenIfIx;
                instRule->accRule.inAddType = pPortCfg->addType;
                instRule->accRule.action = (uint8_t)TCPIP_HTTP_ACCESS_ACTION_ALLOW;
                instRule->accRule.ruleSize = (uint16_t)ruleSz;
                char* dirStr = instRule->strSpace;
                (void)strcpy(dirStr, "/");
                instRule->accRule.dir = dirStr;

                // done; next rule
                instRule = FC_AccRuleInc(instRule, (uint16_t)ruleSz);
            }
            pPortCfg++;
        }

        pInstance->nRules = (uint16_t)cfgRules + (uint16_t)defRules;

        return TCPIP_HTTP_INST_RES_OK;
    }

    pInstance->nRules = 0U;
    pInstance->rules = NULL;

    return TCPIP_HTTP_INST_RES_OK;
}

static void F_HTTP_GetPageAccess(TCPIP_HTTP_CONN* pHttpCon, char* pageName, TCPIP_HTTP_ACCESS_RULE* accRule)
{
    size_t ruleIx;
    char pagePath[SYS_FS_FILE_NAME_LEN + 1];
    char rulePath[SYS_FS_FILE_NAME_LEN + 1];
                                //
    TCPIP_HTTP_INST* pInst = pHttpCon->connInstance;
    if(pInst->nRules == 0U)
    {   // if there are no rules, simply grant access!
        accRule->action = (uint8_t)TCPIP_HTTP_ACCESS_ACTION_ALLOW;
        return;
    }

    // when rules are present the default action is block
    accRule->action = (uint8_t)TCPIP_HTTP_ACCESS_ACTION_BLOCK;
    TCPIP_HTTP_PORT_DCPT* pPort = pHttpCon->connPort;

    SYS_FS_SHELL_RES fsRes = SYS_FS_Shell_GetPath(pPort->fileShell, pageName, pagePath, sizeof(pagePath) / sizeof(*pagePath));
    if(fsRes != SYS_FS_SHELL_RES_OK)
    {   // no such page exist?
        return;
    }

    // get the interface this access came from
    TCP_SOCKET_INFO sktInfo;
    (void)NET_PRES_SocketInfoGet(pHttpCon->netSocket, &sktInfo);

    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(sktInfo.hNet);
    uint16_t inIx = TCPIPStackNetIxGet(pNetIf);

    // get the address type
    IP_ADDRESS_TYPE  inAddType = sktInfo.addressType;
    
    
    // evaluate the rules, including the default ones
    S_TCPIP_HTTP_ACC_RULE* pRule = pInst->rules; 
    for(ruleIx = 0; ruleIx < (size_t)pInst->nRules; ruleIx++)
    {
        // evaluate inPort, inIfIx, inAddType
        if(pRule->accRule.inPort == 0 || pRule->accRule.inPort == pPort->listenPort)
        {   // port match
            if(pRule->accRule.inIfIx < 0 || (uint16_t)pRule->accRule.inIfIx == inIx)
            {   // interface match
                if(pRule->accRule.inAddType == 0 || pRule->accRule.inAddType == inAddType)
                {   // add type match
                    // rule applies to this page
                    fsRes = SYS_FS_Shell_GetPath(pPort->fileShell, pRule->accRule.dir, rulePath, sizeof(rulePath) / sizeof(*rulePath));
                    if(fsRes == SYS_FS_SHELL_RES_OK)
                    {   // rule in the page path; consider rule
                        // check if the rulePath and the pagePath intersect: rulePath is part of the pagePath
                        char* subDir = strstr(pagePath, rulePath);
                        if(subDir != NULL)
                        {   // rule does apply
                            *accRule = pRule->accRule;
                            return;
                        }
                    }
                }
            }
        }
        // next rule
        pRule = FC_AccRuleInc(pRule, pRule->accRule.ruleSize);
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
static bool F_HTTP_Get_RedirectURI(const char* accessDir, const char* redirURI, char* redirBuff, size_t buffSize)
{
    union
    {
        int iLen;
        size_t sLen;
    }U_FC_LEN;

    size_t redirLen = strlen(redirURI);
    if(redirURI[redirLen - 1] == '/')
    {   // redirect to a directory
        U_FC_LEN.iLen = FC_sprintf(redirBuff, buffSize, "%s", redirURI);
        // find the last '/' in the accessDir
        
        const char* dirEnd = accessDir + strlen(accessDir);
        const char* endPath = strrchr(accessDir, (int)'/');
        if(endPath != NULL && (endPath + 1) != dirEnd)
        {   // there is something after the last '/'
            U_FC_LEN.iLen = FC_sprintf(redirBuff + U_FC_LEN.iLen, buffSize - U_FC_LEN.sLen, "%s", endPath + 1);
        }
    }
    else
    {   // redirect to a redirURI
        U_FC_LEN.iLen = FC_sprintf(redirBuff, buffSize, "%s", redirURI);
    }
    
    return U_FC_LEN.sLen < buffSize;    // true if the redirBuff is not overflowed!
}
#endif  // (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)

// initializes the ports belonging to an instance
static TCPIP_HTTP_INST_RES F_HTTP_InitInstance_Ports(TCPIP_HTTP_INST* pInstance, const TCPIP_HTTP_INSTANCE_CONFIG* pInstCfg)
{
    size_t portIx;
    TCPIP_HTTP_INST_RES createRes;
    TCPIP_HTTP_PORT_DCPT* pPort;
    const TCPIP_HTTP_PORT_CONFIG* pCfgPort;
    
    // initialize the ports
    pPort = pInstance->portDcpt;
    pCfgPort = pInstCfg->portConfig;
    for(portIx = 0; portIx < pInstance->nPorts; portIx++)
    {
        pPort->connDataSize = pCfgPort->dataLen;
        pPort->listenPort = pCfgPort->listenPort;
        pPort->connNo = pCfgPort->nConnections;
        pPort->portFlags = pCfgPort->portFlags;
        pPort->portIx = (uint16_t)portIx;

        pPort->persistTmo = pCfgPort->connTimeout;
        if(pPort->persistTmo > TCPIP_HTTP_CONN_MAX_TIMEOUT)
        {
            pPort->persistTmo = TCPIP_HTTP_CONN_MAX_TIMEOUT;
        }

        // create the SYS_FS shell
        SYS_FS_SHELL_RES shellRes;
        const char* webDir = pCfgPort->webDir;
        pPort->fileShell = SYS_FS_Shell_Create(webDir, SYS_FS_SHELL_FLAG_NONE, NULL, NULL, &shellRes);
        if(pPort->fileShell == NULL)
        {
            SYS_ERROR(SYS_ERROR_WARNING, " HTTP: FS Shell creation for directory: %s Failed: %d!\r\n", webDir, shellRes);
            return TCPIP_HTTP_INST_RES_FS_SHELL_ERR; 
        }

        // create the HTTP port connections
        createRes = F_HTTP_CreatePortConnections(pInstance, pCfgPort, pPort);
        if(createRes != TCPIP_HTTP_INST_RES_OK)
        {
            return createRes;
        }
        pCfgPort++, pPort++;
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
        size_t instIx, portIx;
        TCPIP_HTTP_INST*    pInstance;
        for(instIx = 0; instIx < httpInstNo; instIx++)
        {
            pInstance = httpInstances[instIx];
            TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt;
            for(portIx = 0; portIx < pInstance->nPorts; portIx++)
            {
                F_HTTP_CloseConnections(pPort, stackCtrl->pNetIf);
                pPort++;
            }
        }

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down
            if(--httpInitCount == 0)
            {   // all closed
                // release resources
                F_HTTP_Cleanup(stackCtrl);
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_HTTP_Server_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { //  some signal occurred
        TCPIP_HTTP_NET_Process();
    }
}

// send a signal to the HTTP module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void F_HTTPSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T skt, TCPIP_NET_HANDLE hNet, uint16_t sigType, const void* param)
{
    if(sigType == TCPIP_TCP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

static void TCPIP_HTTP_NET_Process(void)
{
    int         instIx, portIx;
    TCPIP_HTTP_INST*    pInstance;
    TCPIP_HTTP_PORT_DCPT*   pPort;
    TCPIP_HTTP_CONN* pHttpCon;
    size_t conn;

    // process instances in turn
    for(instIx = 0; instIx < httpInstNo; instIx++)
    {
        pInstance = httpInstances[instIx];

        pPort = pInstance->portDcpt;
        for(portIx = 0; portIx < pInstance->nPorts; portIx++)
        {
            pHttpCon = pPort->connCtrl + 0;
            for(conn = 0; conn < pPort->connNo; conn++)
            {
                if(pHttpCon->netSocket == NET_PRES_INVALID_SOCKET)
                {
                    pHttpCon++;
                    continue;
                }

                // If a socket is reset at any time 
                // forget about it and return to idle state.
                if(NET_PRES_SocketWasReset(pHttpCon->netSocket) || NET_PRES_SocketWasDisconnected(pHttpCon->netSocket))
                {
                    if(pHttpCon->flags.sktLocalReset != 0)
                    {   // http initiated the disconnect; no error
                        pHttpCon->flags.sktLocalReset = 0;
                    }
                    else
                    {   // some disconnect initiated by the remote party
                        pHttpCon->flags.discardRxBuff = 1;
                        pHttpCon->connState = (uint16_t)TCPIP_HTTP_CONN_STATE_ERROR;
                        pHttpCon->closeEvent = (uint8_t)TCPIP_HTTP_EVENT_CLOSE_REMOTE;
                    }
                }

                // Determine if this connection is eligible for processing
                if(pHttpCon->connState != (uint16_t)TCPIP_HTTP_CONN_STATE_IDLE || NET_PRES_SocketReadIsReady(pHttpCon->netSocket))
                {
                    TCPIP_HTTP_NET_ProcessConnection(pHttpCon);
                }
                else if((pPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) == 0 && pPort->persistTmo != 0 && pHttpCon->flags.sktIsConnected != 0)
                {   // check the connection has not timed out
                    uint16_t currSec = (uint16_t)TCPIP_SecCountGet();
                    if((currSec - pHttpCon->connActiveSec) >= pPort->persistTmo)
                    {   // timeout; kill the connection
                        F_HTTP_DbgConnTmo(pHttpCon);
                        pHttpCon->connActiveSec = currSec;  // disconnect may take a while, avoid a new disconnect request
                        pHttpCon->closeEvent = (uint8_t)TCPIP_HTTP_EVENT_CLOSE_TIMEOUT;
                        pHttpCon->connState = (uint16_t)TCPIP_HTTP_CONN_STATE_DISCONNECT;
                    }
                }
                else
                {
                    // OK
                }
                pHttpCon++;
            }
            pPort++;
        }
    }
}


// Performs any pending operations for the currently loaded HTTP connection.
static void TCPIP_HTTP_NET_ProcessConnection(TCPIP_HTTP_CONN* pHttpCon)
{
    bool needWait;

    // mark connection as active
    pHttpCon->connActiveSec = (uint16_t)TCPIP_SecCountGet();

    do
    {
        needWait = false;   // wait disabled by default
        // call the connection status process function
        F_HTTP_DbgGetState(pHttpCon);
        pHttpCon->connState = (uint16_t)(*T_HTTP_ConnProcess_Tbl[pHttpCon->connState])(pHttpCon, &needWait);
        F_HTTP_DbgNewState(pHttpCon);
    }while(needWait == false);

}

/*****************************************************************************
  Function:
    static bool F_HTTP_HeaderParseLookup(TCPIP_HTTP_CONN* pHttpCon, int reqIx)

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
static bool F_HTTP_HeaderParseLookup(TCPIP_HTTP_CONN* pHttpCon, int reqIx)
{
    // reqIx corresponds to an index in HTTPRequestHeaders

#if defined(TCPIP_HTTP_USE_COOKIES)
    if(reqIx == 0u)
    {
        return F_HTTP_HeaderParseCookie(pHttpCon);
    }
#endif

#if defined(TCPIP_HTTP_USE_AUTHENTICATION)    
    if(reqIx == 1u)
    {
        return F_HTTP_HeaderParseAuthorization(pHttpCon);
    }
#endif

#if defined(TCPIP_HTTP_USE_POST)
    if(reqIx == 2u)
    {
        return F_HTTP_HeaderParseContentLength(pHttpCon);
    }
#endif

    return true;

}

/*****************************************************************************
  Function:
    static bool F_HTTP_HeaderParseAuthorization(TCPIP_HTTP_CONN* pHttpCon)

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
static bool F_HTTP_HeaderParseAuthorization(TCPIP_HTTP_CONN* pHttpCon)
{
    uint16_t len, nDec;
    uint8_t inBuff[40];
    char   outBuff[40 + 2];
    char* passStr; 
    char* queryStr;

    // If auth processing is not required, return
    if((pHttpCon->isAuthorized & 0x80U) != 0U)
    {
        return true;
    }

    // Clear the auth type ("BASIC ")
    (void)F_HTTP_ConnectionDiscard(pHttpCon, 6);

    // Find the terminating CRLF, make sure it's a multiple of four and limit the size
    len = (uint16_t)F_HTTP_ConnectionStringFind(pHttpCon, TCPIP_HTTP_CRLF, 0, 0);
    len += 3U;
    len &= 0xfcU;
    len = FC_Min(len, (uint16_t)sizeof(inBuff) - 4U);


    (void)NET_PRES_SocketRead(pHttpCon->netSocket, inBuff, len);
    nDec = TCPIP_Helper_Base64Decode(inBuff, len, (uint8_t*)outBuff, sizeof(outBuff) - 2);
    outBuff[nDec] = '\0';
    queryStr = strstr(outBuff, ":");
    if(queryStr != NULL)
    {
        *queryStr = '\0';
        passStr = queryStr + 1;
    }
    else
    {
        passStr = outBuff + nDec;
        *passStr = '\0';
    }

    // Verify credentials
    const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
    if(httpUserCback->userAuthenticate != NULL)
    {
        pHttpCon->isAuthorized = (*httpUserCback->userAuthenticate)(pHttpCon, outBuff, passStr, httpUserCback);
    }
    else
    {
        pHttpCon->isAuthorized = 0U;
    }

    return true;
}
#endif

/*****************************************************************************
  Function:
    static bool F_HTTP_HeaderParseCookie(TCPIP_HTTP_CONN* pHttpCon)

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
static bool F_HTTP_HeaderParseCookie(TCPIP_HTTP_CONN* pHttpCon)
{
    uint16_t lenA, lenB;

    // Verify there's enough space
    uint16_t connDataSize = pHttpCon->connPort->connDataSize;
    lenB = (uint16_t)F_HTTP_ConnectionStringFind(pHttpCon, TCPIP_HTTP_CRLF, 0, 0);
    if(lenB >= (uint16_t)(pHttpCon->httpData + connDataSize - pHttpCon->ptrData - 2U))
    {   // If not, overflow
        pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_OVERFLOW;
        return false;
    }

    // While a CRLF is not immediate, grab a cookie value
    while(lenB != 0u)
    {
        // Look for a ';' and use the shorter of that or a CRLF
        lenA = F_HTTP_ConnectionCharFind(pHttpCon, ';', 0, 0);

        // Read to the terminator
        pHttpCon->ptrData += NET_PRES_SocketRead(pHttpCon->netSocket, pHttpCon->ptrData, FC_Min(lenA, lenB));

        // Insert an & to anticipate another cookie
        *(pHttpCon->ptrDataCh++) = '&';

        // If semicolon, trash it and whitespace
        if(lenA < lenB)
        {
            (void)F_HTTP_ConnectionDiscard(pHttpCon, 1);
            while(F_HTTP_ConnectionCharFind(pHttpCon, ' ', 0, 0) == 0u)
            {
                (void)F_HTTP_ConnectionDiscard(pHttpCon, 1);
            }
        }

        // Find the new distance to the CRLF
        lenB = (uint16_t)F_HTTP_ConnectionStringFind(pHttpCon, TCPIP_HTTP_CRLF, 0, 0);
    }

    return true;

}
#endif

/*****************************************************************************
  Function:
    static bool F_HTTP_HeaderParseContentLength(TCPIP_HTTP_CONN* pHttpCon)

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
static bool F_HTTP_HeaderParseContentLength(TCPIP_HTTP_CONN* pHttpCon)
{
    uint16_t len;
    uint8_t buf[10];

    // Read up to the CRLF (max 9 bytes)
    len = (uint16_t)F_HTTP_ConnectionStringFind(pHttpCon, TCPIP_HTTP_CRLF, 0, 0);
    if(len >= sizeof(buf))
    {
        pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_BAD_REQUEST;
        pHttpCon->byteCount = 0;
        return false;
    }   
    len = NET_PRES_SocketRead(pHttpCon->netSocket, buf, len);
    buf[len] = 0;

    if(FC_Str2UL((char*)buf, 10, &pHttpCon->byteCount) < 0)
    {
        return false;
    }

    return true;
}
#endif

// process HTTP idle state: TCPIP_HTTP_CONN_STATE_IDLE
// returns the next connection state
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessIdle(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    uint16_t lenA;

    // Check how much data is waiting
    lenA = NET_PRES_SocketReadIsReady(pHttpCon->netSocket);
    if(lenA == 0)
    {   // no data; wait some more
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_IDLE;
    }

    // process the request
    // Clear out state info and move to next state
    pHttpCon->ptrData = pHttpCon->httpData;
    pHttpCon->isAuthorized = 0xffU;
    pHttpCon->hasArgs = 0;
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
    pHttpCon->httpTick = SYS_TMR_TickCountGet() + (uint32_t)TCPIP_HTTP_TIMEOUT * sysFreq;
    pHttpCon->callbackPos = 0xffffffffU;
    pHttpCon->byteCount = 0;
    pHttpCon->smPost = 0x00;
    pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_GET; // show no error condition
    pHttpCon->closeEvent = 0;
    if(pHttpCon->flags.sktIsConnected == 0)
    {   // report it once
        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_OPEN, NULL);
    }
    pHttpCon->flags.val = 0;
    pHttpCon->flags.sktIsConnected = 1;

    return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_IDLE, +1);

}

// parse HTTP request state: TCPIP_HTTP_CONN_STATE_PARSE_REQUEST
// returns the next connection state
// also signals if waiting for resources
// Retrieves the file name in pHttpCon->httpData!
static TCPIP_HTTP_CONN_STATE F_HTTP_ParseRequest(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
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
    if((lenB = F_HTTP_ConnectionCharFind(pHttpCon, TCPIP_HTTP_NET_LINE_END, 0, 0)) == 0xffff)
    {   // First line isn't here yet
        if(F_HTTP_SktFifoRxFree(pHttpCon->netSocket) == 0)
        {   // If the FIFO is full, we overflowed
            pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_OVERFLOW;
            pHttpCon->flags.discardRxBuff = 1;
            pHttpCon->flags.requestError = 1;
            return TCPIP_HTTP_CONN_STATE_SRV_HEADERS;
        }

        if(((int32_t)SYS_TMR_TickCountGet() - (int32_t)pHttpCon->httpTick) > 0)
        {   // A timeout has occurred
            pHttpCon->flags.discardRxBuff = 1;
            pHttpCon->closeEvent = (uint8_t)TCPIP_HTTP_EVENT_CLOSE_TIMEOUT;
            return TCPIP_HTTP_CONN_STATE_ERROR;
        }
        // wait some more
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_PARSE_REQUEST;
    }

    // Reset the watchdog timer
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
    pHttpCon->httpTick = SYS_TMR_TickCountGet() + (uint32_t)TCPIP_HTTP_TIMEOUT * sysFreq;

    // Determine the request method
    lenA = F_HTTP_ConnectionCharFind(pHttpCon, ' ', 0, 0);
    if(lenA > 5u)
    {
        lenA = 5;
    }
    (void)NET_PRES_SocketRead(pHttpCon->netSocket, pHttpCon->httpData, lenA + 1U);

    if (strncmp(pHttpCon->httpDataCh, "GET", 3) == 0)
    {
        pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_GET;
    }
#if defined(TCPIP_HTTP_USE_POST)
    else if (strncmp(pHttpCon->httpDataCh, "POST", 4) == 0)
    {
        pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_POST;
    }
#endif
    else
    {   // Unrecognized method, so return not implemented
        pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_NOT_IMPLEMENTED;
        pHttpCon->flags.requestError = 1;
        return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
    }

    // Find end of filename
    lenA = F_HTTP_ConnectionCharFind(pHttpCon, ' ', 0, 0);
    lenB = F_HTTP_ConnectionCharFind(pHttpCon, '?', 0, lenA);
    lenA = FC_Min(lenA, lenB);

    // If the file name is too long, then reject the request
    if(lenA > pHttpCon->connPort->connDataSize - TCPIP_HTTP_DEFAULT_LEN - 1)
    {
        pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_OVERFLOW;
        pHttpCon->flags.requestError = 1;
        return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
    }

    // Read in the filename and decode
    lenB = NET_PRES_SocketRead(pHttpCon->netSocket, pHttpCon->httpData, lenA);
    pHttpCon->httpDataCh[lenB] = '\0';
    (void)TCPIP_HTTP_URLDecode(pHttpCon->httpData);

    return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_PARSE_REQUEST, +1); // advance 
}

// parse HTTP file upload state: TCPIP_HTTP_CONN_STATE_PARSE_FUPLOAD
// returns the next connection state
// also signals if waiting for resources
// Uses the file name in pHttpCon->httpData!
static TCPIP_HTTP_CONN_STATE F_HTTP_ParseFileUpload(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    // Check if this is an FS Upload
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    if(strncmp(pHttpCon->httpDataCh + 1, TCPIP_HTTP_FILE_UPLOAD_NAME, sizeof(TCPIP_HTTP_FILE_UPLOAD_NAME)) == 0)
    {   // Read remainder of line, and bypass all file opening, etc.
        if(strlen(pHttpCon->httpDataCh + 1) > sizeof(pHttpCon->fileName))
        {
            F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_NAME_SIZE_ERROR, pHttpCon->httpData + 1);
        }
        (void)strncpy(pHttpCon->fileName, pHttpCon->httpDataCh + 1, sizeof(pHttpCon->fileName) - 1);
        pHttpCon->fileName[sizeof(pHttpCon->fileName) -  1U] = '\0';

#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
        const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
        if(httpUserCback->fileAuthenticate != NULL)
        {
            pHttpCon->isAuthorized = (*httpUserCback->fileAuthenticate)(pHttpCon, pHttpCon->fileName, httpUserCback);
        }
        else
        {
            pHttpCon->isAuthorized = 0U;
        }
#endif
        if(pHttpCon->httpStatus == (uint16_t)TCPIP_HTTP_STAT_GET)
        {
            pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_UPLOAD_FORM;
        }
        else
        {
            pHttpCon->flags.uploadPhase = 0;
            pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_UPLOAD_STARTED;
        }

        return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
    }
#endif
    return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_PARSE_FUPLOAD, + 1);
}

// parse HTTP file open state: TCPIP_HTTP_CONN_STATE_PARSE_FOPEN
// returns the next connection state
// also signals if waiting for resources
// Uses the file name in pHttpCon->httpData!
static TCPIP_HTTP_CONN_STATE F_HTTP_ParseFileOpen(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    int ix;
    char *ext;
    uint16_t lenB;

    TCPIP_HTTP_PORT_DCPT* pPort = pHttpCon->connPort;
    const SYS_FS_SHELL_OBJ*  fileShell = pPort->fileShell;

#if (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)
    TCPIP_HTTP_ACCESS_RULE accRule;
    F_HTTP_GetPageAccess(pHttpCon, pHttpCon->httpDataCh, &accRule);
    if(accRule.action == (uint8_t)TCPIP_HTTP_ACCESS_ACTION_BLOCK)
    {
        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_ACCESS_BLOCK, pHttpCon->httpData + 1);
        pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_NOT_FOUND;
        pHttpCon->flags.requestError = 1;
        return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
    } 
    else if(accRule.action == (uint8_t)TCPIP_HTTP_ACCESS_ACTION_REDIRECT)
    {   // get redirect info
        char redirBuff[TCPIP_HTTP_REDIR_BUFF_SIZE];
        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_ACCESS_REDIRECT, pHttpCon->httpData + 1);

        // redirect to accRule.redirURI
        if(!F_HTTP_Get_RedirectURI(pHttpCon->httpDataCh + 1, accRule.redirURI, redirBuff, sizeof(redirBuff)))
        {
            F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_REDIRECT_BUFFER_SIZE_EXCEEDED, pHttpCon->httpData + 1);
        }
#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_REDIRECT) != 0)
        SYS_CONSOLE_PRINT("redirected -%s- to ", pHttpCon->httpDataCh);
#endif  // ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_REDIRECT) != 0)
        (void)FC_sprintf(pHttpCon->httpDataCh, pHttpCon->connPort->connDataSize, "%s%s", accRule.redirServer, redirBuff);
#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_REDIRECT) != 0)
        SYS_CONSOLE_PRINT("%s\r\n", pHttpCon->httpDataCh);
#endif  // ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_REDIRECT) != 0)

        (void)NET_PRES_SocketDiscard(pHttpCon->netSocket);
        pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_REDIRECT;
        pHttpCon->redirType = accRule.redirType;
        return TCPIP_HTTP_CONN_STATE_SRV_HEADERS;
    }
    else
    {
        // else just let it through...
    }

    

#endif  // (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)

    // Decode may have changed the string length - update it here
    lenB = (uint16_t)strlen(pHttpCon->httpDataCh);

    // If the last character is a not a directory delimiter, then try to open the file
    // String starts at 2nd character, because the first is always a '/'
    if(pHttpCon->httpDataCh[lenB-1U] != '/')
    {
        pHttpCon->file = (*fileShell->fileOpen)(fileShell, pHttpCon->httpDataCh + 1, SYS_FS_FILE_OPEN_READ);
    }

    // If the open fails, then add our default name and try again
    if(pHttpCon->file == SYS_FS_HANDLE_INVALID)
    {
        if(pHttpCon->httpDataCh[lenB-1] != '/')
        {   // Add the directory delimiter if needed
            pHttpCon->httpDataCh[lenB++] = '/';
        }

        // Add our default file name
        (void)strncpy(pHttpCon->httpDataCh + lenB, TCPIP_HTTP_DEFAULT_FILE, (size_t)pPort->connDataSize - (size_t)lenB);

        // Try to open again
        pHttpCon->file = (*fileShell->fileOpen)(fileShell, pHttpCon->httpDataCh + 1, SYS_FS_FILE_OPEN_READ);
    }

    if(pHttpCon->file == SYS_FS_HANDLE_INVALID)
    {   // failed
        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_OPEN_ERROR, pHttpCon->httpData + 1);
        pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_NOT_FOUND;
        pHttpCon->flags.requestError = 1;
        return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
    }

    F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_OPEN, pHttpCon->httpData + 1);
    if(strlen(pHttpCon->httpDataCh + 1) > sizeof(pHttpCon->fileName))
    {
        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_NAME_SIZE_ERROR, pHttpCon->httpData + 1);
    }
    (void)strncpy(pHttpCon->fileName, pHttpCon->httpDataCh + 1, sizeof(pHttpCon->fileName) - 1U);
    pHttpCon->fileName[sizeof(pHttpCon->fileName) - 1U] = '\0';

    // Find the extension in the filename
    ext = strrchr(pHttpCon->fileName, (int)TCPIP_HTTP_FILE_EXT_SEP);

    if(ext != NULL)
    {   // Compare to known extensions to determine Content-Type
        ext++;
        for(ix = 0; ix < sizeof(httpFileExtensions) / sizeof(*httpFileExtensions); ix++)
        {
            if(strcmp(ext, httpFileExtensions[ix]) == 0)
            {
                break;
            }
        }
        pHttpCon->fileType = (uint16_t)ix;  // TCPIP_HTTP_FILE_TYPE_UNKNOWN if not found
    }
    else
    {
        pHttpCon->fileType = (uint16_t)TCPIP_HTTP_FILE_TYPE_UNKNOWN; 
    }

    // Perform first round authentication (pass file name only)
#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
    const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
    if(httpUserCback->fileAuthenticate != NULL)
    {
        pHttpCon->isAuthorized = (*httpUserCback->fileAuthenticate)(pHttpCon, pHttpCon->fileName, httpUserCback);
    }
    else
    {
        pHttpCon->isAuthorized = 0U;
    }
#endif

    return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_PARSE_FOPEN, + 1);   // advance

}

// parse HTTP GET arguments: TCPIP_HTTP_CONN_STATE_PARSE_GET_ARGS
// returns the next connection state
// also signals if waiting for resources
// Uses the file name in pHttpCon->httpData!
static TCPIP_HTTP_CONN_STATE F_HTTP_ParseGetArgs(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    uint16_t lenA;
    uint8_t c;

    // Read GET args, up to buffer size - 1
    lenA = F_HTTP_ConnectionCharFind(pHttpCon, ' ', 0, 0);
    if(lenA != 0)
    {
        pHttpCon->hasArgs = 1;

        // Trash the '?'
        (void)NET_PRES_SocketRead(pHttpCon->netSocket, &c, 1);

        // Verify there's enough space
        lenA--;
        if(lenA >= pHttpCon->connPort->connDataSize - 2)
        {
            pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_OVERFLOW;
            pHttpCon->flags.requestError = 1;
            return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
        }

        // Read in the arguments and '&'-terminate in anticipation of cookies
        pHttpCon->ptrData += NET_PRES_SocketRead(pHttpCon->netSocket, pHttpCon->httpData, lenA);
        *(pHttpCon->ptrDataCh++) = '&';
    }

    // Clear the rest of the line
    lenA = F_HTTP_ConnectionCharFind(pHttpCon, TCPIP_HTTP_NET_LINE_END, 0, 0);
    (void)F_HTTP_ConnectionDiscard(pHttpCon, lenA + 1U);

    // Move to parsing the headers
    return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_PARSE_GET_ARGS, + 1);    // advance

}

    
// parse HTTP headers state: TCPIP_HTTP_CONN_STATE_PARSE_HEADERS 
// returns the next connection state
// also signals if waiting for resources
// if requestError == 1, it eats up the header fields only
// it can set requestError by itself
static TCPIP_HTTP_CONN_STATE F_HTTP_ParseHeaders(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    bool clearLine;
    int ix;
    uint16_t lenA, lenB;
    bool parseFail;
    uint8_t dataBuff[TCPIP_HTTP_MAX_HEADER_LEN + 1];

    // Loop over all the headers
    while(true)
    {
        // Make sure entire line is in the FIFO
        lenA = F_HTTP_ConnectionCharFind(pHttpCon, TCPIP_HTTP_NET_LINE_END, 0, 0);
        if(lenA == 0xffffU)
        {   // If not, make sure we can receive more data
            if(F_HTTP_SktFifoRxFree(pHttpCon->netSocket) == 0)
            {   // Overflow
                pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_OVERFLOW;
                pHttpCon->flags.discardRxBuff = 1;
                pHttpCon->flags.requestError = 1;
                return TCPIP_HTTP_CONN_STATE_SRV_HEADERS;
            }

            if(((int32_t)SYS_TMR_TickCountGet() - (int32_t)pHttpCon->httpTick) > 0)
            {   // A timeout has occured
                pHttpCon->flags.discardRxBuff = 1;
                pHttpCon->closeEvent = (uint8_t)TCPIP_HTTP_EVENT_CLOSE_TIMEOUT;
                return TCPIP_HTTP_CONN_STATE_ERROR;
            }

            // wait some more
            *pWait = true;
            return TCPIP_HTTP_CONN_STATE_PARSE_HEADERS;
        }

        // Reset the watchdog timer
        uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
        pHttpCon->httpTick = SYS_TMR_TickCountGet() + (uint32_t)TCPIP_HTTP_TIMEOUT * sysFreq;

        // If a CRLF is immediate, then headers are done
        if(lenA == 1u)
        {   // Remove the CRLF and move to next state
            (void)F_HTTP_ConnectionDiscard(pHttpCon, 2);
            return (pHttpCon->flags.requestError == 1) ? TCPIP_HTTP_CONN_STATE_SRV_HEADERS : FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_PARSE_HEADERS, + 1); // advance
        }

        if(pHttpCon->flags.requestError == 1)
        {   // in error mode just discard the line
            (void)F_HTTP_ConnectionDiscard(pHttpCon, lenA + 1U);
            continue;
        }

        // Find the header name, and use needWait as a flag to indicate a match
        lenB = F_HTTP_ConnectionCharFind(pHttpCon, ':', 0, lenA) + 2U;

        // If name is too long or this line isn't a header, ignore it
        if(lenB > sizeof(dataBuff))
        {
            (void)F_HTTP_ConnectionDiscard(pHttpCon, lenA + 1U);
            continue;
        }

        // Read in the header name
        (void)NET_PRES_SocketRead(pHttpCon->netSocket, dataBuff, lenB);
        dataBuff[lenB-1U] = 0;
        lenA -= lenB;

        clearLine = false;
        // Compare header read to ones we're interested in
        parseFail = false;
        for(ix = 0; ix < sizeof(HTTPRequestHeaders)/sizeof(HTTPRequestHeaders[0]); ix++)
        {
            if(stricmp((char*)dataBuff, (const char *)HTTPRequestHeaders[ix]) == 0)
            {   // Parse the header and stop the loop
                parseFail = F_HTTP_HeaderParseLookup(pHttpCon, ix) == false;
                clearLine = true;
                break;
            }
        }

        // Clear the rest of the line, and call the loop again
        if(clearLine)
        {   // We already know how much to remove unless a header was found
            lenA = F_HTTP_ConnectionCharFind(pHttpCon, TCPIP_HTTP_NET_LINE_END, 0, 0);
        }
        (void)F_HTTP_ConnectionDiscard(pHttpCon, lenA + 1U);
        if(parseFail)
        {   // signal the error
            pHttpCon->flags.requestError = 1;
        } 
    }

}


// process the authenticate state: TCPIP_HTTP_CONN_STATE_AUTHENTICATE
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessAuthenticate(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    uint8_t hasArgs;

#if defined(TCPIP_HTTP_USE_AUTHENTICATION)
    // Check current authorization state
    if(pHttpCon->isAuthorized < 0x80U)
    {   // 401 error
        pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_UNAUTHORIZED;
        return TCPIP_HTTP_CONN_STATE_SRV_HEADERS;
    }
#endif

    // Parse the args string
    *pHttpCon->ptrData = 0;
    pHttpCon->ptrData = TCPIP_HTTP_URLDecode(pHttpCon->httpData);

    // If this is an upload form request, bypass to headers
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    if(pHttpCon->httpStatus == (uint16_t)TCPIP_HTTP_STAT_UPLOAD_FORM)
    {
        return TCPIP_HTTP_CONN_STATE_SRV_HEADERS;
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
static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessGet(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    // Run the application callback TCPIP_HTTP_ConnectionGetExecute()
    const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
    if(httpUserCback->getExecute != NULL)
    {
        if((*httpUserCback->getExecute)(pHttpCon, httpUserCback) == TCPIP_HTTP_IO_RES_WAITING)
        {   // If waiting for asynchronous process, return to main app
            *pWait = true;
            return TCPIP_HTTP_CONN_STATE_PROCESS_GET;   // stay  here
        }
    }

    // Move on to POST data
    return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_PROCESS_GET, + 1);    // advance

}


// process the POST state: TCPIP_HTTP_CONN_STATE_PROCESS_POST
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessPost(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{

#if defined(TCPIP_HTTP_USE_POST)
    TCPIP_HTTP_IO_RESULT    ioRes;

    // See if we have any new data
    if(NET_PRES_SocketReadIsReady(pHttpCon->netSocket) == pHttpCon->callbackPos)
    {
        if(((int32_t)SYS_TMR_TickCountGet() - (int32_t)pHttpCon->httpTick) > 0)
        {   // If a timeout has occured, disconnect
            pHttpCon->closeEvent = (uint8_t)TCPIP_HTTP_EVENT_CLOSE_TIMEOUT;
            return TCPIP_HTTP_CONN_STATE_ERROR;
        }
    }

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    if(pHttpCon->httpStatus == (uint16_t)TCPIP_HTTP_STAT_POST || (pHttpCon->httpStatus >= (uint16_t)TCPIP_HTTP_STAT_UPLOAD_STARTED && pHttpCon->httpStatus <= (uint16_t)TCPIP_HTTP_STAT_UPLOAD_ERROR))
#else
        if(pHttpCon->httpStatus == (uint16_t)TCPIP_HTTP_STAT_POST)
#endif
        {
            // Run the application callback TCPIP_HTTP_ConnectionPostExecute()
#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
            if(pHttpCon->httpStatus >= (uint16_t)TCPIP_HTTP_STAT_UPLOAD_STARTED && pHttpCon->httpStatus <= (uint16_t)TCPIP_HTTP_STAT_UPLOAD_ERROR)
            {
                ioRes = TCPIP_HTTP_NET_FSUpload(pHttpCon);
            }
            else
#endif  // defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
            {
                const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
                if(httpUserCback->postExecute != NULL)
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
                pHttpCon->callbackPos = (uint32_t)NET_PRES_SocketReadIsReady(pHttpCon->netSocket) - 1U;
                *pWait = true;
                return TCPIP_HTTP_CONN_STATE_PROCESS_POST;  // wait
            }
            else if(ioRes == TCPIP_HTTP_IO_RES_NEED_DATA)
            {   // If waiting for more data
                pHttpCon->callbackPos = (uint32_t)NET_PRES_SocketReadIsReady(pHttpCon->netSocket);
                uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
                pHttpCon->httpTick = SYS_TMR_TickCountGet() + (uint32_t)TCPIP_HTTP_TIMEOUT * sysFreq;

                // If more is expected and space is available, return to main app
                if(pHttpCon->byteCount > pHttpCon->callbackPos && F_HTTP_SktFifoRxFree(pHttpCon->netSocket) != 0)
                {
                    *pWait = true;
                    return TCPIP_HTTP_CONN_STATE_PROCESS_POST;  // wait
                }

                // Handle cases where application ran out of data or buffer space
                pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_INTERNAL_SERVER_ERROR;
                return TCPIP_HTTP_CONN_STATE_SRV_HEADERS;  // error
            }
            else if(ioRes == TCPIP_HTTP_IO_RES_ERROR)
            {   // some error, abort
                pHttpCon->closeEvent = (uint8_t)TCPIP_HTTP_EVENT_CLOSE_POST_ERROR;
                return TCPIP_HTTP_CONN_STATE_ERROR;
            }
            else
            {
                // OK
            }

            // else TCPIP_HTTP_IO_RES_DONE and move on; discard whatever the buffer may contain
            (void)NET_PRES_SocketDiscard(pHttpCon->netSocket);
            return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_PROCESS_POST, + 1);  // advance
        }
#endif
    // We're done with POST
    return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_PROCESS_POST, + 1);  // advance
}


// process the serve connection headers state: TCPIP_HTTP_CONN_STATE_SRV_HEADERS
// returns the next connection state:
// also signals if waiting for resources
// Note: all the previous parsing states should branch here after parsing the headers
// when an error detected // (except timeouts/file errors that go to TCPIP_HTTP_CONN_STATE_ERROR)!
// Processing should get here with all headers processed!
//
static TCPIP_HTTP_CONN_STATE F_HTTP_ServeHeaders(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    bool isConnDone;
    bool fileGzipped;
    union
    {
        int         iLen;
        uint16_t    u16Len;
    }headerLen, responseLen, contentLen;
    SYS_FS_FSTAT fs_attr = {0};
    char contentLenBuffer[40];
    char responseBuffer[TCPIP_HTTP_RESPONSE_BUFFER_SIZE];

    if(pHttpCon->flags.procPhase == 0)
    {   // output headers now; Send header corresponding to the current state
        if(pHttpCon->httpStatus == (uint16_t)TCPIP_HTTP_STAT_REDIRECT)
        {   // special case here, this header message takes arguments; 
            uint8_t redirType = pHttpCon->redirType; 
            if(redirType == 0 || redirType > TCPIP_HTTP_REDIR_TYPE_308)
            {   // use default
                redirType = (uint8_t)M_TCPIP_HTTP_REDIR_TYPE_DEFAULT;
            }
            else
            {
                pHttpCon->redirType = 0;        // restore the default
            }
            headerLen.iLen = FC_sprintf(responseBuffer, sizeof(responseBuffer), TCPIP_HTTP_NET_HEADER_PREFIX "%s\r\nLocation: %s \r\n", HTTPRedirectHeaders[redirType - 1], pHttpCon->httpDataCh);
        }
        else
        {
            headerLen.iLen = FC_sprintf(responseBuffer, sizeof(responseBuffer), TCPIP_HTTP_NET_HEADER_PREFIX "%s", HTTPResponseHeaders[pHttpCon->httpStatus]);
        }

        if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) != 0)
        {
            headerLen.iLen += FC_sprintf(responseBuffer + headerLen.iLen, sizeof(responseBuffer) - headerLen.u16Len, "Connection: close\r\n");
        }

        if(!F_HTTP_DataTryOutput(pHttpCon, responseBuffer, headerLen.u16Len, 0))
        {   //  not enough room to send data; wait some more
            *pWait = true;
            return TCPIP_HTTP_CONN_STATE_SRV_HEADERS;
        }
        // success
        pHttpCon->flags.procPhase = 1;
    }

    if(pHttpCon->flags.procPhase == 1)
    {   // output content length now;
        responseLen.iLen = 0;
        // if error or not GET or POST, we're done and we don't output any message body
        isConnDone = pHttpCon->flags.requestError != 0 || (pHttpCon->httpStatus != (uint16_t)TCPIP_HTTP_STAT_GET && pHttpCon->httpStatus != (uint16_t)TCPIP_HTTP_STAT_POST);
        
        if(HTTPResponseFunctions[pHttpCon->httpStatus] != NULL)
        {
            responseLen.iLen = (*HTTPResponseFunctions[pHttpCon->httpStatus])(pHttpCon, responseBuffer, sizeof(responseBuffer));
            if(responseLen.iLen != 0)
            {
                contentLen.iLen = FC_sprintf(contentLenBuffer, sizeof(contentLenBuffer), "Content-Length: %d\r\n", responseLen.iLen);
                if(!F_HTTP_DataTryOutput(pHttpCon, contentLenBuffer, contentLen.u16Len, contentLen.u16Len + responseLen.u16Len + isConnDone ? TCPIP_HTTP_CRLF_LEN : 0))
                {   // not enough room to send data; wait some more
                    *pWait = true;
                    return TCPIP_HTTP_CONN_STATE_SRV_HEADERS;
                }
                // this should succeed now
                (void)F_HTTP_DataTryOutput(pHttpCon, responseBuffer, responseLen.u16Len, 0U);
                if(isConnDone)
                {
                    (void)F_HTTP_DataTryOutput(pHttpCon, TCPIP_HTTP_CRLF, TCPIP_HTTP_CRLF_LEN, 0U);
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

    responseLen.iLen = 0;
    // Output the content type, if known
    if(pHttpCon->fileType != (uint16_t)TCPIP_HTTP_FILE_TYPE_UNKNOWN)
    {
        responseLen.iLen = FC_sprintf(responseBuffer, sizeof(responseBuffer),  "Content-Type: %s\r\n", httpContentTypes[pHttpCon->fileType]);
    }

    // Output the gzip encoding header if needed
    fileGzipped = 0;
    const SYS_FS_SHELL_OBJ* fileShell = pHttpCon->connPort->fileShell;
    if((*fileShell->fileStat)(fileShell, (const char *)&pHttpCon->fileName, &fs_attr) != SYS_FS_HANDLE_INVALID)
    {
        if (fs_attr.fattrib == SYS_FS_ATTR_ZIP_COMPRESSED)
        {
            responseLen.iLen += FC_sprintf(responseBuffer + responseLen.iLen, sizeof(responseBuffer) - responseLen.u16Len,  "Content-Encoding: gzip\r\n");
            fileGzipped = 1;
        }
    }

    // Output the cache-control
    if((pHttpCon->httpStatus == (uint16_t)TCPIP_HTTP_STAT_POST) || (fileGzipped == 0 && F_HTTP_FileTypeIsDynamic(pHttpCon->fileName)))
    {   // This is a dynamic page or a POST request, so no cache
        responseLen.iLen += FC_sprintf(responseBuffer + responseLen.iLen, sizeof(responseBuffer) - responseLen.u16Len,  "Cache-Control: no-cache\r\n");
    }
    else
    {
        responseLen.iLen += FC_sprintf(responseBuffer + responseLen.iLen, sizeof(responseBuffer) - responseLen.u16Len,  "Cache-Control: max-age=" TCPIP_HTTP_CACHE_LEN TCPIP_HTTP_CRLF);
    }


    if(!F_HTTP_DataTryOutput(pHttpCon, responseBuffer, responseLen.u16Len, 0))
    {   // not enough room to send data; wait some more
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_SRV_HEADERS;
    }


    // Check if we should output cookies
    return (pHttpCon->hasArgs != 0) ? FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_SRV_HEADERS, + 1) : TCPIP_HTTP_CONN_STATE_SRV_BODY_INIT;

}


// serve connection cookies state: TCPIP_HTTP_CONN_STATE_SRV_COOKIES
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE F_HTTP_ServeCookies(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
#if defined(TCPIP_HTTP_USE_COOKIES)
    union
    {
        uint8_t u8;
        char    ch;
    }U_U8_CH;
    union
    {
        char*       pCBuff;
        uint8_t*    pU8Buff;
    }U_BUFF;

    char    cookieBuffer[TCPIP_HTTP_COOKIE_BUFFER_SIZE];
    

    // Write cookies one at a time as space permits
    pHttpCon->ptrRead = pHttpCon->httpData;
    while(pHttpCon->hasArgs != 0)
    {
        // Write the header
        (void)strcpy(cookieBuffer, "Set-Cookie: ");
        U_BUFF.pCBuff = cookieBuffer + strlen(cookieBuffer);

        // Write the name, URL encoded, one character at a time
        while((U_U8_CH.u8 = *(pHttpCon->ptrRead++)) != 0U)
        {
            if(U_U8_CH.ch == ' ')
            {
                *U_BUFF.pCBuff++ = '+';
            }
            else if(U_U8_CH.ch < '0' || (U_U8_CH.ch > '9' && U_U8_CH.ch < 'A') || (U_U8_CH.ch > 'Z' && U_U8_CH.ch < 'a') || U_U8_CH.ch > 'z')
            {
                *U_BUFF.pCBuff++ = '%';
                *U_BUFF.pU8Buff++ = btohexa_high(U_U8_CH.u8);
                *U_BUFF.pU8Buff++ = btohexa_low(U_U8_CH.u8);
            }
            else
            {
                *U_BUFF.pCBuff++ = U_U8_CH.ch;
            }
        }

        *U_BUFF.pCBuff++ = '=';

        // Write the value, URL encoded, one character at a time
        while((U_U8_CH.u8 = *(pHttpCon->ptrRead++)) != 0U)
        {
            if(U_U8_CH.ch == ' ')
            {
                *U_BUFF.pCBuff++ = '+';
            }
            else if(U_U8_CH.ch < '0' || (U_U8_CH.ch > '9' && U_U8_CH.ch < 'A') || (U_U8_CH.ch > 'Z' && U_U8_CH.ch < 'a') || U_U8_CH.ch > 'z')
            {
                *U_BUFF.pCBuff++ = '%';
                *U_BUFF.pU8Buff++ = btohexa_high(U_U8_CH.u8);
                *U_BUFF.pU8Buff++ = btohexa_low(U_U8_CH.u8);
            }
            else
            {
                *U_BUFF.pCBuff++ = U_U8_CH.ch;
            }
        }

        // Finish the line
        *U_BUFF.pCBuff++ = '\r';
        *U_BUFF.pCBuff++ = '\n';

        if(!F_HTTP_DataTryOutput(pHttpCon, cookieBuffer, FC_PtrDiff2UI16(U_BUFF.pCBuff - cookieBuffer), 0))
        {   //  not enough room to send data; wait some more
            *pWait = true;
            return TCPIP_HTTP_CONN_STATE_SRV_COOKIES;
        }
        pHttpCon->hasArgs--;
    }
#endif

    return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_SRV_COOKIES, + 1);   // done, advance
}

// process HTTP body init state: TCPIP_HTTP_CONN_STATE_SRV_BODY_INIT
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE F_HTTP_ServeBodyInit(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    int  encodeLen;
    char encodingBuffer[40];

    // Set up the dynamic substitutions
    pHttpCon->byteCount = 0;

    if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) == 0)
    {   // output encoding and end of headers
        encodeLen = FC_sprintf(encodingBuffer, sizeof(encodingBuffer), "Transfer-Encoding: chunked\r\n\r\n");
    }
    else
    {   // just terminate the headers
        encodeLen = FC_sprintf(encodingBuffer, sizeof(encodingBuffer), "\r\n");

    }

    if(!F_HTTP_DataTryOutput(pHttpCon, encodingBuffer, FC_Int322Uint16(encodeLen), 0U))
    {   // not enough room to send data; wait some more
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_SRV_BODY_INIT;
    }

    return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_SRV_BODY_INIT, + 1);   // advance
}

// process HTTP serve body state: TCPIP_HTTP_CONN_STATE_SRV_BODY
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE F_HTTP_ServeBody(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    TCPIP_HTTP_CHUNK_RES chunkRes;

    chunkRes = F_HTTP_AddFileChunk(pHttpCon, pHttpCon->file, pHttpCon->fileName, NULL);
    if(chunkRes == HTTP_CHUNK_RES_WAIT)
    {   // need a break; stay here
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_SRV_BODY;
    }

    // else continue processing
    pHttpCon->file = SYS_FS_HANDLE_INVALID; // it will be closed when its chunk is processed 
    return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_SRV_BODY, + 1);    // advance
}

// process HTTP serve chunks state: TCPIP_HTTP_CONN_STATE_SRV_CHUNKS
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE F_HTTP_ServeChunks(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    TCPIP_HTTP_CHUNK_RES chunkRes;

    chunkRes = F_HTTP_ProcessChunks(pHttpCon);

    if(chunkRes == HTTP_CHUNK_RES_WAIT)
    {   // need a break; wait here
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_SRV_CHUNKS;
    }

    if(chunkRes == HTTP_CHUNK_RES_OK || chunkRes < 0)
    {   // either continue or some error occurred; try another chunk
        return TCPIP_HTTP_CONN_STATE_SRV_CHUNKS;
    }

    // done processing chunks
    return FC_ConnStateAdvance(TCPIP_HTTP_CONN_STATE_SRV_CHUNKS, + 1);
}


// process HTTP disconnect state: TCPIP_HTTP_CONN_STATE_DONE
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessDone(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) == 0)
    {  // keep connection open; Make sure any opened files are closed
        F_HTTP_ConnCloseFile(pHttpCon);
        *pWait = true;
        return TCPIP_HTTP_CONN_STATE_IDLE;
    }

    // else nonpersistent: disconnect
    pHttpCon->closeEvent = (uint8_t)TCPIP_HTTP_EVENT_CLOSE_DONE;
    return TCPIP_HTTP_CONN_STATE_DISCONNECT;
}


// clean ups the connection data and disconnects the socket
// it also discards the socket RX data as instructed by discardType:
//     - TCPIP_HTTP_DISCARD_NOT         - no discard done
//     - TCPIP_HTTP_DISCARD_WAIT_DISCON - discard if disconnect successful
//     - TCPIP_HTTP_DISCARD_WAIT_NOT    - discard anyway
// returns the disconnect operation result
static bool F_HTTP_ConnCleanDisconnect(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_DISCARD_TYPE discardType)
{
    // Make sure any opened files are closed
    F_HTTP_ConnCloseFile(pHttpCon);

    // purge all pending chunks
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt;
    while((pChDcpt = FC_SglNode2CunkDcpt(pHttpCon->chunkList.head)) != NULL)
    {
        F_HTTP_FreeChunk(pHttpCon, pChDcpt);
    } 

    bool disconRes;
    if((disconRes = NET_PRES_SocketDisconnect(pHttpCon->netSocket)) == true)
    {
        pHttpCon->flags.sktLocalReset = 1;
        pHttpCon->flags.sktIsConnected = 0;
    }

    if(discardType != TCPIP_HTTP_DISCARD_NOT)
    {
        if(disconRes == true || discardType == TCPIP_HTTP_DISCARD_WAIT_NOT)
        {
            (void)NET_PRES_SocketDiscard(pHttpCon->netSocket);
        }
    }

    return disconRes;
}

// process HTTP error state: TCPIP_HTTP_CONN_STATE_ERROR
// returns the next connection state:
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessError(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{

    (void)F_HTTP_ConnCleanDisconnect(pHttpCon, pHttpCon->flags.discardRxBuff != 0 ? TCPIP_HTTP_DISCARD_WAIT_NOT : TCPIP_HTTP_DISCARD_NOT);
    F_HTTP_Report_ConnectionEvent(pHttpCon, (TCPIP_HTTP_EVENT_TYPE)pHttpCon->closeEvent, NULL);

    *pWait = true;
    return TCPIP_HTTP_CONN_STATE_IDLE;
}

// process non-persistent HTTP disconnect state: TCPIP_HTTP_CONN_STATE_DISCONNECT
// persistent connection gets here only if timeout
// returns the next connection state: TCPIP_HTTP_CONN_STATE_IDLE
// also signals if waiting for resources
static TCPIP_HTTP_CONN_STATE F_HTTP_ProcessDisconnect(TCPIP_HTTP_CONN* pHttpCon, bool* pWait)
{
    *pWait = true;

    bool disconRes = F_HTTP_ConnCleanDisconnect(pHttpCon, TCPIP_HTTP_DISCARD_WAIT_DISCON);

    if(disconRes)
    {
        F_HTTP_Report_ConnectionEvent(pHttpCon, (TCPIP_HTTP_EVENT_TYPE)pHttpCon->closeEvent, NULL);
        return TCPIP_HTTP_CONN_STATE_IDLE;
    }

    // else retry next time
    return TCPIP_HTTP_CONN_STATE_DISCONNECT;
}

/* MISRA C-2012 Rule 17.1 deviated:3 Deviation record ID -  H3_MISRAC_2012_R_17_1_NET_DR_2 */
/* MISRA C-2012 Rule 21.6 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_21_6_NET_DR_3 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:3 "MISRA C-2012 Rule 17.1" "H3_MISRAC_2012_R_17_1_NET_DR_2" 
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 21.6" "H3_MISRAC_2012_R_21_6_NET_DR_3" 
static int F_HTTP_HeaderMsg_Print(char* dataBuff, size_t bufferSize, const char* fmt, ...)
{
    int nChars;
    va_list args = {NULL};

    va_start( args, fmt );
    nChars = vsnprintf(dataBuff, bufferSize, fmt, args);
    va_end( args );

    if(nChars >= 0)
    {   // successful
        if((size_t)nChars >= bufferSize)
        {   // exceeded
            nChars = (int)bufferSize - 1;
        }
        return nChars;
    }

    return 0;
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 17.1"
#pragma coverity compliance end_block "MISRA C-2012 Rule 21.6"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */


// outputs a response message from the HTTPResponseMessages
// used for messages without parameters
static int F_HTTP_HeaderMsg_Generic(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, size_t bufferSize)
{

    const char* msg = HTTPResponseMessages[pHttpCon->httpStatus];

    return msg ? F_HTTP_HeaderMsg_Print(dataBuff, bufferSize, msg) : 0;
}

static int F_HTTP_HeaderMsg_NotFound(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, size_t bufferSize)
{
    const char* msg;

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
    msg = "\r\n404: File not found<br>Use <a href=\"/" TCPIP_HTTP_FILE_UPLOAD_NAME "\">MPFS Upload</a> to program web pages\r\n";
    return F_HTTP_HeaderMsg_Print(dataBuff, bufferSize, msg);
#else       
    msg = "\r\n404: File: %s not found\r\n";
    return F_HTTP_HeaderMsg_Print(dataBuff, bufferSize, msg, pHttpCon->fileName);
#endif

}

static int F_HTTP_HeaderMsg_Redirect(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, size_t bufferSize)
{
    const char* msg = "\r\nRedirect: %s\r\n";
    return F_HTTP_HeaderMsg_Print(dataBuff, bufferSize, msg, pHttpCon->httpDataCh);
}

#if defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
static int F_HTTP_HeaderMsg_UploadForm(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, size_t bufferSize)
{
    const char* msg = "\r\n<html><body style=\"margin:100px\"><form method=post action=\"/%s\" enctype=\"multipart/form-data\"><b>FS Image Upload</b><p><input type=file name=i size=40> &nbsp; <input type=submit value=\"Upload\"></form></body></html>";
    return F_HTTP_HeaderMsg_Print(dataBuff, bufferSize, msg, pHttpCon->fileName);
}

static int F_HTTP_HeaderMsg_UploadError(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, size_t bufferSize)
{
    // TCPIP_HTTP_STAT_UPLOAD_ERROR
    const char* msg;
    if(pHttpCon->flags.uploadMemError != 0U)
    {
        msg = "\r\n<html><body style=\"margin:100px\"><b>Upload Memory Allocation Error</b><p><a href=\"/%s \">Try again?</a></body></html>";
    }
    else
    {
        msg = "\r\n<html><body style=\"margin:100px\"><b>FS Image Corrupt or Wrong Version</b><p><a href=\"/%s \">Try again?</a></body></html>";
    }
    return F_HTTP_HeaderMsg_Print(dataBuff, bufferSize, msg, pHttpCon->fileName);
}

#endif

uint8_t* TCPIP_HTTP_URLDecode(uint8_t* cData)
{
    uint8_t *pRead, *pWrite;
    uint16_t wLen;
    uint8_t uc;
    char    sc;
    TCPIP_UINT16_VAL hex;

    union
    {
        uint8_t uc;
        char    sc;
    }U_UINT_CH;

    // Determine length of input
    wLen = (uint16_t)strlen((char*)cData);

    // Read all characters in the string
    pRead = pWrite = cData;
    while(wLen != 0U)
    {
        U_UINT_CH.uc = *pRead++;
        uc = U_UINT_CH.uc;
        sc = U_UINT_CH.sc;

        wLen--;

        if(sc == '=' || sc == '&')
        {
            *pWrite++ = 0U;
        }
        else if(sc == '+')
        {
            *pWrite++ = (uint8_t)' ';
        }
        else if(sc == '%')
        {
            if(wLen < 2U)
            {
                wLen = 0U;
            }
            else
            {   // change endianess
                hex.v[1] = *pRead++;
                hex.v[0] = *pRead++;
                wLen--;
                wLen--;
                *pWrite++ = hexatob(hex.Val);
            }
        }
        else
        {
            *pWrite++ = uc;
        }
    }

    // Double null terminate the last value
    *pWrite++ = 0U;
    *pWrite = 0U;

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
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);

    status = F_HTTP_ReadTo(pHttpCon, '=', cData, wLen);

    // Decode the data (if not reading to null or blank) and return
    if(cData && *cData)
    {
        (void)TCPIP_HTTP_URLDecode(cData);
    }
    return status;
}   
#endif

#if defined(TCPIP_HTTP_USE_POST)
TCPIP_HTTP_READ_STATUS TCPIP_HTTP_ConnectionPostValueRead(TCPIP_HTTP_CONN_HANDLE connHandle, uint8_t* cData, uint16_t wLen)
{
    TCPIP_HTTP_READ_STATUS status;
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);

    // Try to read the value
    status = F_HTTP_ReadTo(pHttpCon, '&', cData, wLen);

    // If read was incomplete, check if we're at the end
    if(status == TCPIP_HTTP_READ_INCOMPLETE)
    {
        // If all data has arrived, read all remaining data
        if(pHttpCon->byteCount == NET_PRES_SocketReadIsReady(pHttpCon->netSocket))
        {
            status = F_HTTP_ReadTo(pHttpCon, '\0', cData, wLen);
        }
    }

    // Decode the data (if not reading to null or blank) and return
    if(cData && *cData)
    {
        (void)TCPIP_HTTP_URLDecode(cData);
    }
    return status;
}   
#endif

/*****************************************************************************
  Function:
    static TCPIP_HTTP_READ_STATUS F_HTTP_ReadTo(TCPIP_HTTP_CONN* pHttpCon, uint8_t cDelim, uint8_t* cData, uint16_t wLen)

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
static TCPIP_HTTP_READ_STATUS F_HTTP_ReadTo(TCPIP_HTTP_CONN* pHttpCon, char cDelim, uint8_t* cData, uint16_t wLen)
{
    TCPIP_HTTP_READ_STATUS status;
    uint16_t wPos;

    // Either look for delimiter, or read all available data
    if(cDelim != '\0')
    {
        wPos = F_HTTP_ConnectionCharFind(pHttpCon, cDelim, 0U, 0U);
    }
    else
    {
        wPos = NET_PRES_SocketReadIsReady(pHttpCon->netSocket);
    }

    // If not found, return incomplete
    if(wPos == 0xffffU)
    {
        return TCPIP_HTTP_READ_INCOMPLETE;
    }

    // Read the value
    if(wLen < 2u && cData != NULL)
    {   // Buffer is too small, so read to NULL instead
        pHttpCon->byteCount -= F_HTTP_ConnectionDiscard(pHttpCon, wPos);
        status = TCPIP_HTTP_READ_TRUNCATED;
    }
    else if(cData == NULL)
    {   // Just remove the data
        pHttpCon->byteCount -= F_HTTP_ConnectionDiscard(pHttpCon, wPos);
        status = TCPIP_HTTP_READ_OK;
    }
    else if(wPos > wLen - 2U)
    {   // Read data, but truncate at max length
        pHttpCon->byteCount -= NET_PRES_SocketRead(pHttpCon->netSocket, cData, wLen - 2U);
        pHttpCon->byteCount -= F_HTTP_ConnectionDiscard(pHttpCon, wPos - (wLen - 2U));
        cData[wLen - 2U] = 0U;
        status = TCPIP_HTTP_READ_TRUNCATED;
    }
    else
    {   // Read the data normally
        pHttpCon->byteCount -= NET_PRES_SocketRead(pHttpCon->netSocket, cData, wPos);
        cData[wPos] = 0U;
        status = TCPIP_HTTP_READ_OK;
    }

    // Remove the delimiter
    if(cDelim != '\0')
    {
        pHttpCon->byteCount -= F_HTTP_ConnectionDiscard(pHttpCon, 1U);
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
    uint16_t lenA, lenB;
    uint32_t nSectors;
    uint32_t mpfsAllocSize;
    TCPIP_UINT32_VAL peekRes;
    union
    {
        uint8_t mpfsBuffer[sizeof(MPFS_SIGNATURE) - 1U];  // large enough to hold the MPFS signature
        char    mpfsStr[sizeof(MPFS_SIGNATURE) - 1U];  // large enough to hold the MPFS signature
    }U_MPFS_BUFF;

    TCPIP_HTTP_IO_RESULT ioRes = TCPIP_HTTP_IO_RES_NEED_DATA;
    switch(pHttpCon->httpStatus)
    {
        // New upload, so look for the CRLFCRLF
        case (uint16_t)TCPIP_HTTP_STAT_UPLOAD_STARTED:
            if(pHttpCon->flags.uploadPhase == 0U)
            {   // just starting
                pHttpCon->uploadSectNo = 0;
                peekRes.Val = F_HTTP_ConnectionStringFind(pHttpCon, "\r\n\r\n", 0U, 0U);

                if(peekRes.word.LW == 0xffffU)
                {   // End of line not found, remove as much as possible
                    lenA = (peekRes.word.HW);   // get the # of bytes safe to remove
                    if(lenA > 4)
                    {
                        lenA = F_HTTP_ConnectionDiscard(pHttpCon, lenA - 4U);
                        pHttpCon->byteCount -= lenA;
                    }
                    break;
                }

                // Found end of line, so remove all data up to and including
                lenA = F_HTTP_ConnectionDiscard(pHttpCon, peekRes.word.LW + 4U);
                pHttpCon->byteCount -= (lenA + 4);

                pHttpCon->flags.uploadPhase = 1U;
                break;
            }
            // waiting for the signature
            // Make sure first 6 bytes are also in
            if(NET_PRES_SocketReadIsReady(pHttpCon->netSocket) < (uint16_t)sizeof(MPFS_SIGNATURE) - 1U )
            {
                ioRes = TCPIP_HTTP_IO_RES_NEED_DATA;
                break;
            }

            // reset the phase, we're done
            pHttpCon->flags.uploadPhase = 0U;

            lenA = NET_PRES_SocketRead(pHttpCon->netSocket, U_MPFS_BUFF.mpfsBuffer, sizeof(MPFS_SIGNATURE) - 1U);
            pHttpCon->byteCount -= lenA;
            if(strncmp(U_MPFS_BUFF.mpfsStr, MPFS_SIGNATURE, sizeof(MPFS_SIGNATURE) - 1U) != 0)
            {   // wrong signature
                pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_UPLOAD_ERROR;
                ioRes = TCPIP_HTTP_IO_RES_WAITING;
                break;
            }

            // proper file version
            // allocate the buffer size as a multiple of sector size
            pHttpCon->uploadBufferStart = NULL;
            mpfsAllocSize = ((MPFS_UPLOAD_WRITE_BUFFER_SIZE + (SYS_FS_MEDIA_SECTOR_SIZE - 1)) / SYS_FS_MEDIA_SECTOR_SIZE) * SYS_FS_MEDIA_SECTOR_SIZE;
            if((pHttpCon->uploadBufferStart = (uint8_t*)http_mallocFnc(mpfsAllocSize)) != NULL)
            {
                pHttpCon->uploadBufferEnd = pHttpCon->uploadBufferStart + mpfsAllocSize;
                (void)memcpy(pHttpCon->uploadBufferStart, FC_CStr2CUPtr(MPFS_SIGNATURE), sizeof(MPFS_SIGNATURE) - 1U);
                pHttpCon->uploadBufferCurr = pHttpCon->uploadBufferStart + sizeof(MPFS_SIGNATURE) - 1U;

                (void)SYS_FS_Unmount(MPFS_UPLOAD_MOUNT_PATH);
                pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_UPLOAD_WRITE;
                ioRes = TCPIP_HTTP_IO_RES_WAITING;
                break;
            }

            // memory allocation failed
            pHttpCon->flags.uploadMemError = 1U; // signal out of memory
            F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_UPLOAD_ALLOC_ERROR, pHttpCon->fileName);
            pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_UPLOAD_ERROR;
            ioRes = TCPIP_HTTP_IO_RES_WAITING;
            break;

        case (uint16_t)TCPIP_HTTP_STAT_UPLOAD_WRITE:
            lenA = NET_PRES_SocketReadIsReady(pHttpCon->netSocket);
            if(lenA > pHttpCon->uploadBufferEnd - pHttpCon->uploadBufferCurr)
            {
                lenA = FC_PtrDiff2UI16(pHttpCon->uploadBufferEnd - pHttpCon->uploadBufferCurr);
            }

            if((uint32_t)lenA > pHttpCon->byteCount)
            {
                lenA = (uint16_t)pHttpCon->byteCount;
            }

            if(lenA == 0U)
            {
                ioRes = TCPIP_HTTP_IO_RES_WAITING;
                break;
            }

            lenB = NET_PRES_SocketRead(pHttpCon->netSocket, pHttpCon->uploadBufferCurr, lenA);
            pHttpCon->uploadBufferCurr += lenB;
            pHttpCon->byteCount -= lenB;

            // check that upload buffer is full
            if(pHttpCon->uploadBufferCurr != pHttpCon->uploadBufferEnd && pHttpCon->byteCount != 0U)
            {
                ioRes = TCPIP_HTTP_IO_RES_WAITING;
                break;
            }

            nSectors = ((pHttpCon->uploadBufferCurr - pHttpCon->uploadBufferStart) + SYS_FS_MEDIA_SECTOR_SIZE - 1U) / (SYS_FS_MEDIA_SECTOR_SIZE);

            if(!F_HTTP_DbgKillFlashWrite())
            {   // try to perform the write
                pHttpCon->uploadBuffHandle = SYS_FS_MEDIA_MANAGER_SectorWrite(MPFS_UPLOAD_DISK_NO, pHttpCon->uploadSectNo , pHttpCon->uploadBufferStart,
                        nSectors);
                if(pHttpCon->uploadBuffHandle == SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID)
                {
                    pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_UPLOAD_ERROR;
                    F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FS_WRITE_ERROR, pHttpCon->fileName);
                    ioRes = TCPIP_HTTP_IO_RES_WAITING;
                    break;
                }
            }
            else
            {   // advance without performing the write
                pHttpCon->uploadBuffHandle = SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID;
            }

            // everything fine
            pHttpCon->uploadSectNo += (uint16_t)nSectors;
            pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_UPLOAD_WRITE_WAIT;
            ioRes = TCPIP_HTTP_IO_RES_WAITING;
            break;

        case (uint16_t)TCPIP_HTTP_STAT_UPLOAD_WRITE_WAIT:
            if(pHttpCon->uploadBuffHandle != SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID)
            {
                if(SYS_FS_MEDIA_MANAGER_CommandStatusGet(MPFS_UPLOAD_DISK_NO, pHttpCon->uploadBuffHandle) != SYS_FS_MEDIA_COMMAND_COMPLETED)
                {
                    ioRes = TCPIP_HTTP_IO_RES_WAITING;
                    break;
                }
            }

            pHttpCon->uploadBufferCurr = pHttpCon->uploadBufferStart;

            if(pHttpCon->byteCount == 0U)
            {   // we're done
                http_freeFnc(pHttpCon->uploadBufferStart);
                pHttpCon->uploadBufferStart = NULL;

                if(SYS_FS_Mount(MPFS_UPLOAD_NVM_VOL, MPFS_UPLOAD_MOUNT_PATH, MPFS2, 0, NULL)  != SYS_FS_RES_FAILURE)
                {
                    F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FS_UPLOAD_COMPLETE, pHttpCon->fileName);
                    pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_UPLOAD_OK;
                    ioRes = TCPIP_HTTP_IO_RES_DONE;
                    break;
                }
                else
                {
                    F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FS_MOUNT_ERROR, pHttpCon->fileName);
                    pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_UPLOAD_ERROR;
                    ioRes = TCPIP_HTTP_IO_RES_ERROR;
                    break;
                }
            }

            // else, more data to come
            pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_UPLOAD_WRITE;
            ioRes = TCPIP_HTTP_IO_RES_WAITING;
            break;

        case (uint16_t)TCPIP_HTTP_STAT_UPLOAD_ERROR:
            if(pHttpCon->uploadBufferStart != NULL)
            {
                http_freeFnc(pHttpCon->uploadBufferStart);
                pHttpCon->uploadBufferStart = NULL;
            }
            pHttpCon->byteCount -= NET_PRES_SocketReadIsReady(pHttpCon->netSocket);
            (void)NET_PRES_SocketDiscard(pHttpCon->netSocket);
            if(pHttpCon->byteCount < 100U || pHttpCon->byteCount > 0x80000000U)
            {   // If almost all data was read, or if we overflowed, then return
                ioRes = TCPIP_HTTP_IO_RES_DONE;
            }
            break;

        default:
            ioRes = TCPIP_HTTP_IO_RES_NEED_DATA;
            break;
    }

    return ioRes;
}
#endif //defined (TCPIP_HTTP_FILE_UPLOAD_ENABLE)

// the default file include dynamic variable HTTP operation
static TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_NET_DefaultIncludeFile(TCPIP_HTTP_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const struct S_tag_TCPIP_HTTP_USER_CALLBACK* pCBack)
{
    TCPIP_HTTP_CONN* pHttpCon;
    const char* fName;
    TCPIP_HTTP_DYN_ARG_DCPT* pArgDcpt;
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt = FC_DynCtxt2CunkDcpt(varDcpt->dynContext);

    pHttpCon = FC_ConnHndl2ConnPtr(connHandle); 

    // some basic sanity check
    pArgDcpt = varDcpt->dynArgs;
    if(pArgDcpt->argType == TCPIP_HTTP_DYN_ARG_TYPE_STRING)
    {
        fName = pArgDcpt->argStr;
    }
    else
    {
        fName = NULL;
    }

    (void)F_HTTP_IncludeFile(pHttpCon, pChDcpt, fName);

    // one way or another we're done
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

static TCPIP_HTTP_CHUNK_RES F_HTTP_IncludeFile(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, const char* fName)
{
    SYS_FS_HANDLE fp = SYS_FS_HANDLE_INVALID;


    // avoid creating a new chunk for a file that cannot be opened
    if(fName != NULL)
    {
        const SYS_FS_SHELL_OBJ* fileShell = pHttpCon->connPort->fileShell;
        fp = (*fileShell->fileOpen)(fileShell, fName, SYS_FS_FILE_OPEN_READ);
    }

    if(fp == SYS_FS_HANDLE_INVALID)
    {   // File not found, so abort
        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_OPEN_ERROR, fName);
        return HTTP_CHUNK_RES_FILE_ERR;
    }

    F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_OPEN, fName);
    // add valid file for processing
    return F_HTTP_AddFileChunk(pHttpCon, fp, fName, pChDcpt);
}


SYS_FS_HANDLE TCPIP_HTTP_ConnectionFileGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return pHttpCon->file;
}

uint16_t TCPIP_HTTP_ConnectionPostSmGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return pHttpCon->smPost;
}

void TCPIP_HTTP_ConnectionPostSmSet(TCPIP_HTTP_CONN_HANDLE connHandle, uint16_t state)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    pHttpCon->smPost = state;
}

uint8_t* TCPIP_HTTP_ConnectionDataBufferGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return pHttpCon->httpData;
}

/* MISRA C-2012 Rule 5.1 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_5_1_NET_DR_10 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:9 "MISRA C-2012 Rule 5.1" "H3_MISRAC_2012_R_5_1_NET_DR_10" 
uint16_t TCPIP_HTTP_ConnectionDataBufferSizeGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return pHttpCon->connPort->connDataSize;
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 5.1"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */


uint32_t TCPIP_HTTP_ConnectionCallbackPosGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return pHttpCon->callbackPos;
}

/* MISRA C-2012 Rule 5.1 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_5_1_NET_DR_10 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:9 "MISRA C-2012 Rule 5.1" "H3_MISRAC_2012_R_5_1_NET_DR_10" 
void TCPIP_HTTP_ConnectionCallbackPosSet(TCPIP_HTTP_CONN_HANDLE connHandle, uint32_t callbackPos)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    pHttpCon->callbackPos = callbackPos;
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 5.1"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */


void TCPIP_HTTP_ConnectionStatusSet(TCPIP_HTTP_CONN_HANDLE connHandle, TCPIP_HTTP_STATUS stat)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    pHttpCon->httpStatus = (uint16_t)stat;
}

void  TCPIP_HTTP_ConnectionRedirectionSet(TCPIP_HTTP_CONN_HANDLE connHandle, TCPIP_HTTP_REDIR_TYPE redirType)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    pHttpCon->httpStatus = (uint16_t)TCPIP_HTTP_STAT_REDIRECT;
    pHttpCon->redirType = (uint8_t)redirType;
}

TCPIP_HTTP_STATUS TCPIP_HTTP_ConnectionStatusGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return (TCPIP_HTTP_STATUS)pHttpCon->httpStatus;
}



void TCPIP_HTTP_ConnectionHasArgsSet(TCPIP_HTTP_CONN_HANDLE connHandle, uint8_t args)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    pHttpCon->hasArgs = args;
}

uint8_t TCPIP_HTTP_ConnectionHasArgsGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return pHttpCon->hasArgs;
}

uint32_t TCPIP_HTTP_ConnectionByteCountGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return pHttpCon->byteCount;
}

void TCPIP_HTTP_ConnectionByteCountSet(TCPIP_HTTP_CONN_HANDLE connHandle, uint32_t byteCount)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    pHttpCon->byteCount = byteCount;
}

void TCPIP_HTTP_ConnectionByteCountDec(TCPIP_HTTP_CONN_HANDLE connHandle, uint32_t byteCount)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    pHttpCon->byteCount -= byteCount;
}

NET_PRES_SKT_HANDLE_T TCPIP_HTTP_ConnectionSocketGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return pHttpCon->netSocket;
}

uint8_t TCPIP_HTTP_ConnectionIsAuthorizedGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return pHttpCon->isAuthorized;
}

/* MISRA C-2012 Rule 5.1 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_5_1_NET_DR_10 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:9 "MISRA C-2012 Rule 5.1" "H3_MISRAC_2012_R_5_1_NET_DR_10" 
void TCPIP_HTTP_ConnectionIsAuthorizedSet(TCPIP_HTTP_CONN_HANDLE connHandle, uint8_t auth)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    pHttpCon->isAuthorized = auth;
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 5.1"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */


void TCPIP_HTTP_ConnectionUserDataSet(TCPIP_HTTP_CONN_HANDLE connHandle, const void* uData)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    pHttpCon->userData = uData;
}

const void* TCPIP_HTTP_ConnectionUserDataGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return pHttpCon->userData;
}

size_t TCPIP_HTTP_ConnectionIndexGet(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
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

    return NULL;
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
            for(connIx = 0; connIx < pPort->connNo; connIx++)
            {
                if(pHttpCon->netSocket != NET_PRES_INVALID_SOCKET)
                {
                    openCount++;
                    if(NET_PRES_SocketIsConnected(pHttpCon->netSocket))
                    {
                        activeCount++;
                    }
                }
                pHttpCon++;
            }
        }
    }

    if(pOpenCount != NULL)
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
            return (ssize_t)pInstance->nRules;
        }

        // count rules for portIx
        if(portIx < pInstance->nPorts)
        {
            TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt + portIx;
            ssize_t nRules = 0;
            S_TCPIP_HTTP_ACC_RULE* pRule = pInstance->rules;
            for(ruleIx = 0; ruleIx < (size_t)pInstance->nRules; ruleIx++)
            {
                if(pRule->accRule.inPort == pPort->listenPort)
                {
                    nRules++;
                }
                // next rule
                pRule = FC_AccRuleInc(pRule, pRule->accRule.ruleSize);
            }
            return nRules;
        }
    }

    return -1;
}

bool TCPIP_HTTP_PortRuleGet(size_t instIx, ssize_t portIx, size_t ruleIx, TCPIP_HTTP_ACCESS_RULE* pAccRule)
{
#if (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)
    size_t rIx;
    S_TCPIP_HTTP_ACC_RULE* pRule, *foundRule;
    if(instIx < httpInstNo)
    {
        const TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);

        if(portIx < 0)
        {   // all rules are counted
            if(ruleIx < (size_t)pInstance->nRules)
            {
                if(pAccRule != NULL)
                {   // traverse the rules
                    pRule = pInstance->rules;
                    for(rIx = 0; rIx < ruleIx; rIx++)
                    {
                        pRule = FC_AccRuleInc(pRule, pRule->accRule.ruleSize);
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
            foundRule = NULL;
            pRule = pInstance->rules;
            for(rIx = 0; rIx < (size_t)pInstance->nRules; rIx++)
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
                pRule = FC_AccRuleInc(pRule, pRule->accRule.ruleSize);
            }

            if(foundRule != NULL)
            {
                if(pAccRule != NULL)
                {   // traverse the rules
                    *pAccRule = foundRule->accRule;
                }
                return true;
            }
        }
        else
        {
            // OK
        }
    }

    return false;
#else
    return false;
#endif  //  // (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)
}

uint16_t TCPIP_HTTP_ConnectionStringFind(TCPIP_HTTP_CONN_HANDLE connHandle, const char* str, uint16_t startOffs, uint16_t searchLen)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return (uint16_t)F_HTTP_ConnectionStringFind(pHttpCon, str, startOffs, searchLen);
}


bool TCPIP_HTTP_DynamicFileInclude(TCPIP_HTTP_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const char* fileName)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt = FC_DynCtxt2CunkDcpt(varDcpt->dynContext);
    TCPIP_HTTP_CHUNK_RES chunkRes = F_HTTP_IncludeFile(pHttpCon, pChDcpt, fileName);

    return chunkRes == HTTP_CHUNK_RES_OK; 
}


uint16_t TCPIP_HTTP_ConnectionFlush(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return NET_PRES_SocketFlush(pHttpCon->netSocket);
}

uint16_t TCPIP_HTTP_ConnectionReadIsReady(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return NET_PRES_SocketReadIsReady(pHttpCon->netSocket);
}

uint16_t TCPIP_HTTP_ConnectionRead(TCPIP_HTTP_CONN_HANDLE connHandle, void * dataBuff, uint16_t size)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return NET_PRES_SocketRead(pHttpCon->netSocket, dataBuff, size);
}

uint16_t TCPIP_HTTP_ConnectionPeek(TCPIP_HTTP_CONN_HANDLE connHandle,  void * dataBuff, uint16_t size)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return NET_PRES_SocketPeek(pHttpCon->netSocket, dataBuff, size);
}

uint16_t TCPIP_HTTP_ConnectionDiscard(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);
    return NET_PRES_SocketDiscard(pHttpCon->netSocket);
}

uint16_t TCPIP_HTTP_ConnectionReadBufferSize(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    uint16_t sktRxSize;
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);

    if(NET_PRES_SocketOptionsGet(pHttpCon->netSocket, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, &sktRxSize))
    {
        return sktRxSize;
    }

    return 0;
}

TCPIP_NET_HANDLE TCPIP_HTTP_ConnectionNetHandle(TCPIP_HTTP_CONN_HANDLE connHandle)
{
    TCP_SOCKET_INFO sktInfo;
    TCPIP_HTTP_CONN* pHttpCon = FC_ConnHndl2ConnPtr(connHandle);

    if(NET_PRES_SocketInfoGet(pHttpCon->netSocket, &sktInfo))
    {
        return sktInfo.hNet;
    }

    return NULL;
}


static uint16_t F_HTTP_SktFifoRxFree(NET_PRES_SKT_HANDLE_T skt)
{
    uint16_t sktRxSize;
    uint16_t sktReadySize;

    if(NET_PRES_SocketOptionsGet(skt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, &sktRxSize))
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
static bool F_HTTP_DataTryOutput(TCPIP_HTTP_CONN* pHttpCon, const char* data, uint16_t dataLen, uint16_t checkLen)
{
    uint16_t socketSpace;

    if(checkLen == 0 || checkLen < dataLen)
    {
        checkLen = dataLen;
    }
    socketSpace = NET_PRES_SocketWriteIsReady(pHttpCon->netSocket, checkLen, 0);
    if(socketSpace >= checkLen)
    {
        if(data != NULL)
        {
            (void)NET_PRES_SocketWrite(pHttpCon->netSocket, (const uint8_t*)data, dataLen);
        }
        return true;
    }

    return false;
}


// Note that the search will fail if there's more data in the TCP socket than could be read at once.
// returns a 32 bit word:
//      - hi 16 bits hold the peeked at data, i.e. the number of bytes to safely remove, if needed
//      - low 16 bits is 0xffff if not found or offset where the string was found
static uint32_t F_HTTP_ConnectionStringFind(TCPIP_HTTP_CONN* pHttpCon, const char* str, uint16_t startOffs, uint16_t searchLen)
{
    uint16_t    peekOffs, peekReqLen, peekSize, avlblBytes;
    char*   queryStr;

    uint16_t peekBufferSize = pHttpCon->connInstance->peekBufferSize;
    char* srchBuff = (char*)http_mallocFnc(peekBufferSize + 1);
    if(srchBuff == NULL)
    {
        char eventBuff[16];
        (void)FC_sprintf(eventBuff, sizeof(eventBuff), "%d", peekBufferSize + 1);
        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_PEEK_ALLOC_BUFFER_ERROR, eventBuff);
        return 0xffff;
    }

    const char* findStr = (const char*)str;

    size_t findLen = strlen(findStr);

    // peek sanity check
    if((avlblBytes = NET_PRES_SocketReadIsReady(pHttpCon->netSocket)) > peekBufferSize)
    {   // peek buffer too small!
        (void)FC_sprintf(srchBuff, (size_t)peekBufferSize, "%d", (int)avlblBytes - (int)peekBufferSize);
        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_PEEK_BUFFER_SIZE_EXCEEDED, srchBuff);
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
            peekSize =  NET_PRES_SocketPeek(pHttpCon->netSocket, srchBuff, peekReqLen);
            if(peekSize >= findLen)
            {   // enough data present
                srchBuff[peekSize] = '\0';     // end string properly
                queryStr = strstr(srchBuff, findStr);
                if(queryStr != NULL)
                {
                    retCode = peekOffs + FC_PtrDiff2UI16(queryStr - srchBuff);
                }
            }
        }
    }


    http_freeFnc(srchBuff);
    return ((uint32_t)peekSize << 16) | retCode;
}

static uint16_t F_HTTP_ConnectionCharFind(TCPIP_HTTP_CONN* pHttpCon, char cFind, uint16_t wStart, uint16_t wSearchLen)
{
    char srchBuff[2];
    srchBuff[0] = cFind;
    srchBuff[1] = '\0';

    return (uint16_t)F_HTTP_ConnectionStringFind(pHttpCon, srchBuff, wStart, wSearchLen);
}

TCPIP_HTTP_USER_HANDLE TCPIP_HTTP_UserHandlerRegister(size_t instIx, const TCPIP_HTTP_USER_CALLBACK* userCallback)
{
    if(instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);

        if(pInstance->portDcpt != NULL)
        {   // we're up and running; allow only one registration for now
            if(userCallback && (pInstance->instanceFlags & TCPIP_HTTP_EXT_FLAG_USR_REG) == 0)
            {   // OK, empty
                pInstance->httpRegistry = *userCallback;
                pInstance->instanceFlags |= TCPIP_HTTP_EXT_FLAG_USR_REG;
                return (TCPIP_HTTP_USER_HANDLE)&pInstance->httpRegistry;
            }
        }
    }

    return NULL;

}

bool TCPIP_HTTP_UserHandlerDeregister(size_t instIx, TCPIP_HTTP_USER_HANDLE hHttp)
{
    uint16_t  connIx, portIx;

    if(instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);

        if(pInstance->portDcpt != NULL)
        {   // we're up and running
            // abort all connections
            if(hHttp == &pInstance->httpRegistry && (pInstance->instanceFlags & TCPIP_HTTP_EXT_FLAG_USR_REG) != 0)
            {

                TCPIP_HTTP_PORT_DCPT* pPort = pInstance->portDcpt;
                for(portIx = 0; portIx < pInstance->nPorts; portIx++)
                {
                    TCPIP_HTTP_CONN* pHttpCon = pPort->connCtrl + 0;
                    for (connIx = 0; connIx < pPort->connNo; connIx++)
                    {
                        if (pHttpCon->netSocket != NET_PRES_INVALID_SOCKET)
                        {
                            TCP_SOCKET tcp_skt = NET_PRES_SocketGetTransportHandle(pHttpCon->netSocket);
                            TCPIP_TCP_Abort(tcp_skt, false);
                        }

                        F_HTTP_ConnCloseFile(pHttpCon);
                        pHttpCon++;
                    }
                    pPort++;
                }

                pInstance->instanceFlags &= ~TCPIP_HTTP_EXT_FLAG_USR_REG;
                (void)memset(&pInstance->httpRegistry, 0, sizeof(pInstance->httpRegistry));
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
static uint16_t F_HTTP_StartHttpChunk(TCPIP_HTTP_CONN* pHttpCon, uint32_t chunkSize)
{
    char chunkHdrBuff[TCPIP_HTTP_CHUNK_HEADER_LEN + 1];     

    int iLen = FC_sprintf(chunkHdrBuff, sizeof(chunkHdrBuff), "%x\r\n", (unsigned int)chunkSize);
    TCPIPStack_Assert(iLen >= 0 && iLen <= (int)TCPIP_HTTP_CHUNK_HEADER_LEN, __FILE__, __func__, __LINE__);
    uint16_t hdrLen = (uint16_t)iLen;

    if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) == 0)
    {   // write to the socket
        uint16_t avlblBytes;
        avlblBytes = NET_PRES_SocketWriteIsReady(pHttpCon->netSocket, hdrLen, 0);
        if(avlblBytes < hdrLen)
        {   // not enough space
            return 0;
        }

        // write to output
        avlblBytes = NET_PRES_SocketWrite(pHttpCon->netSocket, chunkHdrBuff, hdrLen);
        TCPIPStack_Assert(avlblBytes == hdrLen, __FILE__, __func__, __LINE__);
        return avlblBytes;
    }
    // else non persistent and fake it
    return hdrLen;
}

// generates the CRLF at the end of the chunk
// Returns: number of bytes written as the chunk trailer
//          0 if retry needed
// the output goes directly to the socket, if possible
static uint16_t F_HTTP_EndHttpChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_END_TYPE endType)
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

    uint16_t trailLen = (uint16_t)strlen(endStr);

    if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) == 0)
    {
        uint16_t avlblBytes;
        avlblBytes = NET_PRES_SocketWriteIsReady(pHttpCon->netSocket, trailLen, 0);
        if(avlblBytes < trailLen)
        {   // not enough space
            return 0;
        }

        // write to output
        avlblBytes =  NET_PRES_SocketWrite(pHttpCon->netSocket, endStr, trailLen);
        TCPIPStack_Assert(avlblBytes == trailLen, __FILE__, __func__, __LINE__);
        return avlblBytes;
    }

    // fake it
    return trailLen;
}

#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

// prepends the start HTTP chunk to the buffer
// if buffer is null, it just calculates the size
// returns the size taken by the header
static uint16_t F_HTTP_PrependStartHttpChunk(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, uint32_t chunkSize)
{
    if((pHttpCon->connPort->portFlags & TCPIP_HTTP_PORT_FLAG_NON_PERSISTENT) != 0)
    {   // no chunk header
        return 0;
    }

    char chunkHdrBuff[TCPIP_HTTP_CHUNK_HEADER_LEN + 1];     

    int iLen = FC_sprintf(chunkHdrBuff, sizeof(chunkHdrBuff), "%x\r\n", (unsigned int)chunkSize);

    TCPIPStack_Assert(iLen >= 0 && iLen <= TCPIP_HTTP_CHUNK_HEADER_LEN, __FILE__, __func__, __LINE__);
    uint16_t hdrLen = (uint16_t)iLen;

    if(dataBuff != NULL)
    {
        (void)memcpy(dataBuff - hdrLen, chunkHdrBuff, hdrLen);
    }

    return hdrLen;
}

// appends the end chunk to the end of buffer
// if buffer is null, it just calculates the size
// returns the size taken by the trailer
static uint16_t F_HTTP_AppendEndHttpChunk(TCPIP_HTTP_CONN* pHttpCon, char* dataBuff, TCPIP_HTTP_CHUNK_END_TYPE endType)
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

    uint16_t trailLen = (uint16_t)strlen(endStr);

    if(dataBuff != NULL)
    {
        (void)memcpy(dataBuff, endStr, trailLen);
    }

    return trailLen;
}




// adds a file chunk to this connection
// the file handle should be provided
// pOwnDcpt shows the chunk that owns this new file. If 0, then it's a root file.
static TCPIP_HTTP_CHUNK_RES F_HTTP_AddFileChunk(TCPIP_HTTP_CONN* pHttpCon, SYS_FS_HANDLE fH, const char* fName, TCPIP_HTTP_CHUNK_DCPT* pOwnDcpt)
{
    uint16_t chunkFlags;
    TCPIP_HTTP_CHUNK_RES chunkRes = HTTP_CHUNK_RES_OK;
    TCPIP_HTTP_EVENT_TYPE evType = TCPIP_HTTP_EVENT_NONE;
    const void* evInfo = fName;
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt = NULL;

    const SYS_FS_SHELL_OBJ* fileShell = pHttpCon->connPort->fileShell;

    while(true)
    {
        if(fH == SYS_FS_HANDLE_INVALID)
        {   // do nothing for invalid files
            TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
            evType = TCPIP_HTTP_EVENT_FILE_OPEN_ERROR;
            chunkRes = HTTP_CHUNK_RES_FILE_ERR;
            break;
        }
        

        if(TCPIP_Helper_SingleListCount(&pHttpCon->chunkList) >= pHttpCon->connInstance->chunksDepth)
        {   // already at max depth
            evType = TCPIP_HTTP_EVENT_DEPTH_ERROR;
            chunkRes = HTTP_CHUNK_RES_DEPTH_ERR;
            break;
        }

        chunkFlags = (pOwnDcpt == NULL) ? ((uint16_t)TCPIP_HTTP_CHUNK_FLAG_TFILE | (uint16_t)TCPIP_HTTP_CHUNK_FLAG_TFILE_ROOT) : (uint16_t)TCPIP_HTTP_CHUNK_FLAG_TFILE;
        if(fName != NULL)
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

            if(!fileGzipped && F_HTTP_FileTypeIsDynamic(fName))
            {
                if(!F_HTTP_DbgKillDynFiles())
                {
                    chunkFlags |= (uint16_t)TCPIP_HTTP_CHUNK_FLAG_TFILE_DYN;
                }
            }
        }

        int32_t fileSize =  (*fileShell->fileSize)(fileShell, fH);
        if(fileSize == 0 || fileSize == -1)
        {   // still invalid
            evType = TCPIP_HTTP_EVENT_FILE_SIZE_ERROR;
            chunkRes = HTTP_CHUNK_RES_FILE_ERR;
            break;
        }

        // valid file, try to get a chunk
        pChDcpt = F_HTTP_AllocChunk(pHttpCon, chunkFlags, (pOwnDcpt == NULL), &evType);

        if(pChDcpt == NULL)
        { 
            chunkRes = (evType < 0) ? HTTP_CHUNK_RES_RETRY_ERR : HTTP_CHUNK_RES_WAIT;
            break;
        }


        pChDcpt->pRootDcpt = (pOwnDcpt == NULL) ? pChDcpt : pOwnDcpt->pRootDcpt; 
        pChDcpt->fileChDcpt.fSize = (size_t)fileSize;
        pChDcpt->fileChDcpt.fHandle = fH;
        if(fName != NULL)
        {
            char* path = strrchr(fName, (int)TCPIP_HTTP_FILE_PATH_SEP);
            if(path != NULL)
            {   // save a truncated version of the file name, no path 
                fName = path + 1;
            }
            (void)strncpy(pChDcpt->chunkFName, fName, sizeof(pChDcpt->chunkFName) - 1U);
            pChDcpt->chunkFName[sizeof(pChDcpt->chunkFName) - 1U] = '\0';
        }
        else
        {
            pChDcpt->chunkFName[0] = '\0';
        }

        break;
    }

    if((int)chunkRes < 0)
    {   // some error occurred
        if(fH != SYS_FS_HANDLE_INVALID)
        {
            (void)(*fileShell->fileClose)(fileShell, fH);
            F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_CLOSE, fName);
        }
    }
    else if(chunkRes == HTTP_CHUNK_RES_OK) 
    {
        F_HTTP_FileDbgCreate(pChDcpt->chunkFName, pChDcpt->fileChDcpt.fSize, (chunkFlags & (uint16_t)TCPIP_HTTP_CHUNK_FLAG_TFILE_DYN) != 0U ? "dyn" : "bin", pHttpCon->connIx);
    }
    else
    {
        // do nothing
    }

    if(evType != TCPIP_HTTP_EVENT_NONE)
    {
        F_HTTP_Report_ConnectionEvent(pHttpCon, evType, evInfo);
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
static TCPIP_HTTP_CHUNK_DCPT* F_HTTP_AllocChunk(TCPIP_HTTP_CONN* pHttpCon, uint16_t flags, bool appendTail, TCPIP_HTTP_EVENT_TYPE* pEvType)
{
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt;
    TCPIP_HTTP_FILE_BUFF_DCPT*  fileBuffDcpt = NULL;

    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 
    SINGLE_LIST* httpFileBuffers = &pInstance->fileBuffers;
    if((flags & (uint16_t)TCPIP_HTTP_CHUNK_FLAG_TFILE) != 0U)
    {   // grab a file buffer
        fileBuffDcpt = FC_SglNode2BuffDcpt(TCPIP_Helper_SingleListHeadRemove(httpFileBuffers));
        if(fileBuffDcpt == NULL)
        {   // failed
            pHttpCon->fileBufferPoolEmpty++;
            if(++pHttpCon->fileBufferRetry > pInstance->fileBufferRetries)
            {
                *pEvType = TCPIP_HTTP_EVENT_FBUFF_POOL_ERROR;
            }
            else
            {
                *pEvType = TCPIP_HTTP_EVENT_FBUFF_POOL_EMPTY;
            }
            return NULL;
        }
        else
        {   // success
            pHttpCon->fileBufferRetry = 0;
        }
    }

    pChDcpt = FC_SglNode2CunkDcpt(TCPIP_Helper_SingleListHeadRemove(&pInstance->chunkPool));

    if(pChDcpt != NULL)
    {   // success
        size_t currDepth;

        pHttpCon->chunkPoolRetry = 0;

        (void)memset(pChDcpt, 0, sizeof(*pChDcpt));
        // set the chunk connection
        pChDcpt->pOwnerCon = pHttpCon;

        if((flags & (uint16_t)TCPIP_HTTP_CHUNK_FLAG_TFILE) != 0U)
        {
            pChDcpt->fileChDcpt.fHandle = SYS_FS_HANDLE_INVALID;
            pChDcpt->fileChDcpt.fileBuffDcpt = fileBuffDcpt;
        }
        else
        {   // data chunk
            TCPIP_Helper_SingleListInitialize (&pChDcpt->dynChDcpt.dynBuffList);
        }
        pChDcpt->flags = flags | (uint16_t)TCPIP_HTTP_CHUNK_FLAG_BEG_CHUNK;
        pChDcpt->status = (uint8_t)TCPIP_HTTP_CHUNK_STATE_BEG;

        if(appendTail)
        {
            TCPIP_Helper_SingleListTailAdd(&pHttpCon->chunkList, FC_CunkDcpt2SglNode(pChDcpt));
        }
        else
        {
            TCPIP_Helper_SingleListHeadAdd(&pHttpCon->chunkList, FC_CunkDcpt2SglNode(pChDcpt));
        }

        currDepth = TCPIP_Helper_SingleListCount(&pHttpCon->chunkList);
        if((uint16_t)currDepth > pInstance->maxRecurseDepth)
        {
            pInstance->maxRecurseDepth = (uint16_t)currDepth;
        }

    }
    else
    {   // failed
        if(fileBuffDcpt != NULL)
        {   // return it to pool
            TCPIP_Helper_SingleListTailAdd(httpFileBuffers, FC_BuffDcpt2SglNode(fileBuffDcpt));
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
static void F_HTTP_FreeChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    TCPIPStack_Assert(pChDcpt == FC_SglNode2CunkDcpt(pHttpCon->chunkList.head), __FILE__, __func__, __LINE__);

    TCPIP_HTTP_CHUNK_DCPT* pHead = FC_SglNode2CunkDcpt(TCPIP_Helper_SingleListHeadRemove(&pHttpCon->chunkList));
    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 

    if((pHead->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE) != 0)
    {
        if(pHead->fileChDcpt.fHandle != SYS_FS_HANDLE_INVALID)
        {
            const SYS_FS_SHELL_OBJ* fileShell = pHttpCon->connPort->fileShell;
            (void)(*fileShell->fileClose)(fileShell, pHead->fileChDcpt.fHandle);
            F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_FILE_CLOSE, pHead->chunkFName);
        }

        if(pHead->fileChDcpt.fileBuffDcpt != NULL)
        {
            TCPIP_Helper_SingleListTailAdd(&pInstance->fileBuffers, FC_BuffDcpt2SglNode(pHead->fileChDcpt.fileBuffDcpt));
        }
    }
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    else if((pHead->flags & (TCPIP_HTTP_CHUNK_FLAG_TFILE | TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI)) == 0)
    {
        if(pHead->dynChDcpt.pDynAllocDcpt != NULL)
        {   // dynamic variable chunk with allocated dyn var user space
            http_freeFnc(pHead->dynChDcpt.pDynAllocDcpt);
        }
        // check if there are dynamic buffers that are not freed
        TCPIP_HTTP_DYNVAR_BUFF_DCPT* pDynDcpt;
        while((pDynDcpt = FC_SglNode2DynVarDcpt(TCPIP_Helper_SingleListHeadRemove(&pChDcpt->dynChDcpt.dynBuffList))) != NULL)
        {
            if((pDynDcpt->dynFlags & TCPIP_HTTP_DYNVAR_BUFF_FLAG_ACK) != 0)
            {   // needs acknowlegment
                if(pInstance->httpRegistry.dynamicAck != NULL)
                {
                    (*pInstance->httpRegistry.dynamicAck)(pHttpCon, pDynDcpt->dynBuffer, &pInstance->httpRegistry);
                }
            }
            F_HTTP_ReleaseDynBuffDescriptor(pInstance, pDynDcpt);
        }
    }
#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)
#if (TCPIP_HTTP_SSI_PROCESS != 0)
    else if((pHead->flags & (TCPIP_HTTP_CHUNK_FLAG_TFILE | TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI)) == TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI)
    {
        if(pHead->ssiChDcpt.pAllocAttribs != NULL)
        {   // SSI chunk with allocated processing space
            http_freeFnc(pHead->ssiChDcpt.pAllocAttribs);
        }
    }
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)
    else
    {
        // OK
    }

    TCPIP_Helper_SingleListTailAdd(&pInstance->chunkPool, FC_CunkDcpt2SglNode(pHead));
}

static bool F_HTTP_FileTypeIsDynamic(const char* fName)
{
    if(fName != NULL)
    {
        char* ext = strchr(fName, (int)TCPIP_HTTP_FILE_EXT_SEP);
        if(ext != NULL)
        {
            size_t ix;
            ext++;
            const char** dynExt = httpDynFileExtensions;
            for(ix = 0; ix < sizeof(httpDynFileExtensions) / sizeof(*httpDynFileExtensions); ix++)
            {
                if(strcmp(ext, *dynExt) == 0)
                {
                    return true;
                }
                dynExt++;
            }
        }
    }

    return false;
}

static TCPIP_HTTP_CHUNK_RES F_HTTP_ProcessChunks(TCPIP_HTTP_CONN* pHttpCon)
{
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt;
    TCPIP_HTTP_CHUNK_RES chunkRes;

    pChDcpt = FC_SglNode2CunkDcpt(pHttpCon->chunkList.head);

    if(pChDcpt == NULL)
    {   // empty; nothing to do
        return HTTP_CHUNK_RES_DONE;
    }

    // process list; could become empty!
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    if((pChDcpt->flags & (TCPIP_HTTP_CHUNK_FLAG_TFILE | TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI)) == 0)
    {
        chunkRes = F_HTTP_ProcessDynVarChunk(pHttpCon, pChDcpt);
    }
    else
#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)
#if (TCPIP_HTTP_SSI_PROCESS != 0)
    if((pChDcpt->flags & (TCPIP_HTTP_CHUNK_FLAG_TFILE | TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI)) == TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI)
    {
        chunkRes = F_HTTP_ProcessSSIChunk(pHttpCon, pChDcpt);
    }
    else
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)
    {
        chunkRes = F_HTTP_ProcessFileChunk(pHttpCon, pChDcpt);
    }

    // ignore errors and, as long as there is something to do, keep going
    if(chunkRes != HTTP_CHUNK_RES_WAIT)
    {   // no break is needed
        chunkRes = TCPIP_Helper_SingleListIsEmpty(&pHttpCon->chunkList) ? HTTP_CHUNK_RES_DONE : HTTP_CHUNK_RES_OK;
    }

    return chunkRes;
}

//
// Processes a chunk for a dynamic variable or binary file
static TCPIP_HTTP_CHUNK_RES F_HTTP_ProcessFileChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
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
        if(outSize != 0U)
        {   // data pending in the chunk buffer; send it out
            outSize = NET_PRES_SocketWrite(pHttpCon->netSocket, chunkBuffer + pChDcpt->fileChDcpt.chunkOffset, outSize);
            // global indicator that something went out of this file
            pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_OUT_DATA; 
            
            if( (pChDcpt->fileChDcpt.chunkOffset += outSize) != pChDcpt->fileChDcpt.chunkEnd)
            {   // more data; wait some more
                return HTTP_CHUNK_RES_WAIT;
            }
        }

        // chunk buffer empty and ready for another line
        if(pChDcpt->status == (uint8_t)TCPIP_HTTP_CHUNK_STATE_END)
        {   // we're done
            break;
        }

        procError = false;
        // if we have a dynStart process it
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0)
        if(pChDcpt->fileChDcpt.dynStart != NULL)
        {   // start the dynamic variable processing
            TCPIP_HTTP_CHUNK_RES dynRes = F_HTTP_AddDynChunk(pHttpCon, pChDcpt);
            if(dynRes == HTTP_CHUNK_RES_WAIT)
            {   // wait for resources...
                return HTTP_CHUNK_RES_WAIT;
            }

            pChDcpt->fileChDcpt.dynStart = NULL;
            if(dynRes == HTTP_CHUNK_RES_OK)
            {   // it went through and it needs processing
                return HTTP_CHUNK_RES_OK;
            }
            else
            {   // dynRes < 0: some error occurred; abort processing this chunk
                pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_TFILE_ERROR | TCPIP_HTTP_CHUNK_FLAG_TFILE_PARSE_ERROR;
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
            if((fileBytes = pChDcpt->fileChDcpt.fSize - pChDcpt->fileChDcpt.fOffset) != 0U)
            {   // more data to read
                if(fileBytes > chunkBufferSize)
                {
                    fileBytes = chunkBufferSize;
                }

                fileReadBytes = (*pPort->fileShell->fileRead)(pPort->fileShell, pChDcpt->fileChDcpt.fHandle, fileBuffer, fileBytes);

                if(fileReadBytes != fileBytes)
                {   // one chunk at a time. can abort if error because no new chunk was written!
                    pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_TFILE_ERROR;
                }
                else if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_DYN) != 0)
                {   // check for dynamic variables
                    endLine = F_HTTP_ProcessFileLine(pChDcpt, fileBuffer, fileBytes, &dynStart, (pChDcpt->fileChDcpt.fOffset + fileBytes) == pChDcpt->fileChDcpt.fSize);
                    if(endLine == NULL)
                    {   // cannot fit one line in our buffer
                        pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_TFILE_ERROR | TCPIP_HTTP_CHUNK_FLAG_TFILE_PARSE_ERROR;
                    }
                    else
                    {
                        fileBytes = (size_t)FC_PtrDiff2UI16(endLine - fileBuffer);
                        pChDcpt->fileChDcpt.dynStart = dynStart;
                    }
                }
                else
                {
                    // OK
                }
            }
        }

        endOfFile = false;
        if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_ERROR) != 0)
        {
            if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_DYN) != 0)
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
        if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_DYN) != 0)
        {   // for a dynamic file we insert the header for every chunk
            if(fileBytes != 0 || (pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_ERROR) != 0)
            {
                prependHeader = true;
                headerLen = fileBytes;
            }
        }
        else if(pChDcpt->status == (uint8_t)TCPIP_HTTP_CHUNK_STATE_BEG)
        {   // binary file: if we just started, the start chunk header needs to be inserted
            prependHeader = true;
            headerLen = pChDcpt->fileChDcpt.fSize;
            pChDcpt->status = (uint8_t)TCPIP_HTTP_CHUNK_STATE_DATA;
        }
        else
        {
            // OK
        }

        // need trailer?
        appendTrailer = false;
        trailType = TCPIP_HTTP_CHUNK_END_NONE;
        if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_DYN) != 0)
        {   // for a dynamic file we end each chunk 
            if(fileBytes != 0 || (pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_ERROR) != 0)
            {   // transmit smth or error
                appendTrailer = true;
                trailType = TCPIP_HTTP_CHUNK_END_CURRENT;
            }

            if(endOfFile && (pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_ROOT) != 0)
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
            if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_ROOT) != 0)
            {
                trailType += TCPIP_HTTP_CHUNK_END_FINAL;
            }
        }
        else
        {
            // OK
        }
        
        // construct the chunk
        // header
        if(prependHeader != 0)
        {   // output the chunk data
            hdrBytes = F_HTTP_PrependStartHttpChunk(pHttpCon, fileBuffer, headerLen);
        }
        else
        {
            hdrBytes = 0;
        }

        // data already in there
        pChDcpt->fileChDcpt.chunkOffset = FC_PtrDiff2UI16(fileBuffer - chunkBuffer) - hdrBytes;
        pChDcpt->fileChDcpt.chunkEnd = pChDcpt->fileChDcpt.chunkOffset + hdrBytes + (uint16_t)fileBytes;

        // trailer
        if(appendTrailer)
        {
            pChDcpt->fileChDcpt.chunkEnd += F_HTTP_AppendEndHttpChunk(pHttpCon, chunkBuffer + pChDcpt->fileChDcpt.chunkEnd, trailType);
        }

        if(endOfFile)
        {
            pChDcpt->status = (uint8_t)TCPIP_HTTP_CHUNK_STATE_END;
        }

        // finally update the file offset
        if(fileReadBytes != fileBytes)
        {   // readjust if we read too much
            (void)(*pPort->fileShell->fileSeek)(pPort->fileShell, pChDcpt->fileChDcpt.fHandle, -(fileReadBytes - fileBytes), SYS_FS_SEEK_CUR);
        }

        // continue: either done, or send out data and do more, or process the dynStart
    }


    // once we're here the file is done

    if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_ERROR) != 0)
    {
        F_HTTP_Report_ConnectionEvent(pHttpCon, (pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_PARSE_ERROR) != 0 ? TCPIP_HTTP_EVENT_FILE_PARSE_ERROR : TCPIP_HTTP_EVENT_FILE_READ_ERROR, pChDcpt->chunkFName);
    }

    // done with this file
    F_HTTP_FileDbgProcess(pChDcpt->chunkFName, pChDcpt->fileChDcpt.fSize, ((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_DYN) != 0) ? "dyn" : "bin", pHttpCon->connIx);
    F_HTTP_FreeChunk(pHttpCon, pChDcpt);

    return (pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_ERROR) != 0 ? HTTP_CHUNK_RES_FILE_ERR : HTTP_CHUNK_RES_DONE;
        
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
static char* F_HTTP_ProcessFileLine(TCPIP_HTTP_CHUNK_DCPT* pChDcpt, char* lineBuffer, size_t buffLen, char** pDynStart, bool isEof)
{
    char *endPattern, *startPattern;
    char *dynStart, *dynEnd, *endLine;
    char* searchStart = lineBuffer;

    *pDynStart = NULL;

    lineBuffer[buffLen] = '\0';
    endLine = strrchr(lineBuffer, (int)'\n');
    if(endLine == NULL)
    {   // no proper end of line detected
        if(!isEof)
        {   // error: cannot fit one line in our buffer
            return NULL;
        }
        // acceptable at the EOF
        endLine = lineBuffer + buffLen - 1;
    }

    endLine++;
    if(F_HTTP_DbgKillDynParser())
    {
        return endLine;
    }

    while(true)
    {
        if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TFILE_SKIP) != 0)
        {
            endPattern = strstr(searchStart, TCPIP_HTTP_PARSE_IGNORE_END);
            if(endPattern == NULL)
            {
                break;
            }

            // ending the ignore mode
            pChDcpt->flags &= ~TCPIP_HTTP_CHUNK_FLAG_TFILE_SKIP;
            searchStart = endPattern + strlen(TCPIP_HTTP_PARSE_IGNORE_END);
            continue;
        }

        // check for start of dynamic variables or other internal processing
        dynStart = F_HTTP_FileLineParse(pChDcpt, searchStart, &dynEnd, true);
        if(dynStart == NULL)
        {   // not found dynamic variable
            break;
        }

        // make sure the dyn var is not within the ignore pattern
        startPattern = strstr(searchStart, TCPIP_HTTP_PARSE_IGNORE_BEGIN);
        if(startPattern == NULL || (dynStart < startPattern && dynEnd <= startPattern))
        {
            *pDynStart = dynStart;
            endLine = dynStart;
            break;
        }

        // starting the ignore mode
        pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_TFILE_SKIP;
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
static char* F_HTTP_FileLineParse(TCPIP_HTTP_CHUNK_DCPT* pChDcpt, char* lineBuff, char** pEndProcess, bool verifyOnly)
{
    char    *procStart = NULL;
    char    *dynStart = NULL;
    char    *procEnd, *dynEnd;  
#if (TCPIP_HTTP_SSI_PROCESS != 0)
    procStart = F_HTTP_SSILineParse(lineBuff, &procEnd, verifyOnly);
#else
    procStart = NULL;
    procEnd = NULL;
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)

    dynEnd = NULL;
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
    dynStart = F_HTTP_DynVarParse(lineBuff, &dynEnd, verifyOnly);
#else
    dynStart = 0;
#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

    if(procStart != NULL && (dynStart == NULL || procStart < dynStart))
    {
        pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI;
        *pEndProcess = procEnd;
        return procStart;
    }

    pChDcpt->flags &= ~TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI;
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
static char* F_HTTP_DynVarParse(char* dynVarBuff, char** pEndDyn, bool verifyOnly)
{
    char *dynStart, *dynEnd;
    int nDelims;


    // check for start of dynamic variables
    dynStart = strchr(dynVarBuff, (int)TCPIP_HTTP_DYNVAR_DELIM);
    if(dynStart == NULL)
    {   // not found dynamic variable
        return NULL;
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
            *dynEnd++ = '\0';
        }
    }

    if(nDelims != 0)
    {   // not ending properly
        return NULL;
    }


    *pEndDyn = dynEnd;
    return dynStart;
}

// extracts the dynamic variable name and parameters from the connection current file
// stores all the dynamic variable parameters
// Note: strings are kept in the fileBuffer belonging to the parent file!
// returns true for success, false for error
static bool  F_HTTP_DynVarExtract(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pDynChDcpt, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt)
{
    int32_t extraLen;
    size_t  fileLen, len;
    char    *argStart, *argEnd, *argStr;
    char    *startDynName, *endDynName = NULL;
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
    uint8_t dynFlags = (uint8_t)TCPIP_HTTP_DYN_VAR_FLAG_NONE; 
    uint16_t dynEvent = (uint16_t)TCPIP_HTTP_EVENT_NONE;
    TCPIP_HTTP_DYN_ARG_DCPT  dynArgDcpt[TCPIP_HTTP_DYNVAR_ARG_MAX_NUMBER];

    // NOTE: the buffer belonging to the parent file is used!
    dynVarBuff = pFileChDcpt->fileChDcpt.fileBuffDcpt->fileBuffer;
    TCPIPStack_Assert(TCPIP_HTTP_DYNVAR_MAX_LEN < pFileChDcpt->fileChDcpt.fileBuffDcpt->fileBufferSize, __FILE__, __func__, __LINE__);

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
            TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
            dynEvent = (uint16_t)TCPIP_HTTP_EVENT_FILE_READ_ERROR;
            dynInfo = pDynChDcpt->chunkFName;
            endDynName = dynVarBuff + len;  // eat up all buffer if error
            fileLen = len;
            break;
        }

        dynVarBuff[fileLen] = '\0';
        TCPIPStack_Assert(dynVarBuff[0] == TCPIP_HTTP_DYNVAR_DELIM, __FILE__, __func__, __LINE__);

        // parse the dynamic variable
        startDynName = F_HTTP_DynVarParse(dynVarBuff, &endDynName, false);

        if(startDynName == NULL)
        {   // some parsing error
            TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
            dynEvent = (uint16_t)TCPIP_HTTP_EVENT_DYNVAR_PARSE_ERROR;
            dynInfo = dynVarBuff;
            break;
        }

        // address the old keyword format
        argStart = strchr(startDynName, (int)TCPIP_HTTP_DYNVAR_KWORD_OLD_ARG_START);
        if(argStart != NULL)
        {   // replace with standard arg delimiter
            *argStart = TCPIP_HTTP_DYNVAR_ARG_START;
            // this style of argument doesn't end properly
            argEnd = startDynName + strlen(startDynName);
            *argEnd++ = TCPIP_HTTP_DYNVAR_ARG_END;
            *argEnd++ = '\0';
        }

        // get the variable name and the arguments
        nArgs = 0;
        argStart = strchr(startDynName, (int)TCPIP_HTTP_DYNVAR_ARG_START);
        dynInfo = startDynName;

        if(argStart != NULL)
        {   // start getting the arguments
            *argStart++ = '\0';    // end the dynName properly
            argEnd = strchr(argStart, (int)TCPIP_HTTP_DYNVAR_ARG_END);
            if(argEnd != NULL)
            {
                *argEnd = '\0';
            }
            else
            {
                dynFlags |= (uint8_t)TCPIP_HTTP_DYN_VAR_FLAG_ARG_NAME_TRUNCATED;
                dynEvent |= (uint16_t)TCPIP_HTTP_EVENT_DYNVAR_ARG_NAME_TRUNCATED;
            }

            (void)memset(dynArgDcpt, 0, sizeof(dynArgDcpt));
            pArgDcpt = dynArgDcpt;
            while(true)
            {
                argStr = strtok(argStart, TCPIP_HTTP_DYNVAR_ARG_SEP);
                argStart = NULL;
                if(argStr == NULL)
                {   // done
                    break;
                }

                if(nArgs < sizeof(dynArgDcpt) / sizeof(*dynArgDcpt))
                {
                    argInt = 0;
                    argType = F_HTTP_ArgType(argStr, &argInt);
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
                dynFlags |= (uint8_t)TCPIP_HTTP_DYN_VAR_FLAG_ARG_NO_TRUNCATED; 
                dynEvent |= (uint16_t)TCPIP_HTTP_EVENT_DYNVAR_ARG_N_TRUNCATED; 
                nArgs = sizeof(dynArgDcpt) / sizeof(*dynArgDcpt);
            }
        }

        // allocate space for this dynVar descriptor and include the dynamic arguments
        pAllocDcpt = (TCPIP_HTTP_DYNVAR_ALLOC_DCPT*)http_mallocFnc(sizeof(TCPIP_HTTP_DYNVAR_ALLOC_DCPT) + (nArgs - 1) * sizeof(TCPIP_HTTP_DYN_ARG_DCPT));

        if(pAllocDcpt == NULL)
        {   // allocation failed
            dynEvent = (uint16_t)TCPIP_HTTP_EVENT_DYNVAR_ALLOC_ERROR;
            break;
        }
        // construct the info to be passed to the user
        pDestDcpt = &pAllocDcpt->dynDcpt;
        pDestDcpt->dynName = startDynName;
        pDestDcpt->fileName = pFileChDcpt->chunkFName;
        pDestDcpt->dynFlags = dynFlags;
        pDestDcpt->dynContext = pDynChDcpt;

        // set the args
        TCPIPStack_Assert(nArgs < 256U, __FILE__, __func__, __LINE__);
        if((pDestDcpt->nArgs = (uint8_t)nArgs) != 0)
        {
            pDestDcpt->dynArgs = pAllocDcpt->dynArgs;
            (void)memcpy(pDestDcpt->dynArgs, dynArgDcpt, nArgs * sizeof(TCPIP_HTTP_DYN_ARG_DCPT));
        }
        else
        {
            pDestDcpt->dynArgs = NULL;
        }

        pDynChDcpt->dynChDcpt.pDynAllocDcpt = pAllocDcpt;
    

        pKEntry = F_HTTP_SearchDynVarKeyEntry(pDestDcpt->dynName);
        if(pKEntry != NULL && (pKEntry->keyFlags & TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEF_PROCESS) != 0)
        {
            pDynChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEF_PROCESS;
        }
        pDynChDcpt->dynChDcpt.pKEntry = pKEntry;

        // success; mark it valid!
        pDynChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_DYNVAR_VALID;
        F_HTTP_DynDbgExtract(pDestDcpt->dynName, pDestDcpt->nArgs, pDestDcpt->fileName, pHttpCon->connIx);

        success = true;
        break;
    }
    
    if(dynEvent != (uint16_t)TCPIP_HTTP_EVENT_NONE)
    {
        F_HTTP_Report_ConnectionEvent(pHttpCon, (TCPIP_HTTP_EVENT_TYPE)dynEvent, dynInfo);
    }

    // adjust the file
    extraLen = (int32_t)fileLen - (endDynName - dynVarBuff);
    if(extraLen != 0)
    {
        (void)(*pPort->fileShell->fileSeek)(pPort->fileShell, pFileChDcpt->fileChDcpt.fHandle, -extraLen, SYS_FS_SEEK_CUR);
    }

    // adjust the file byte counters with read characters
    TCPIPStack_Assert(endDynName - dynVarBuff >= 0, __FILE__, __func__, __LINE__);

    pFileChDcpt->fileChDcpt.fOffset += endDynName - dynVarBuff;


    return success;
}

static const TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY* F_HTTP_SearchDynVarKeyEntry(const char* keyword)
{
    size_t ix;

    const TCPIP_HTTP_DYN_VAR_KEYWORD_ENTRY* pEntry = httpDynVarKeywords;
    for(ix = 0; ix < sizeof(httpDynVarKeywords) / sizeof(*httpDynVarKeywords); ix++)
    {
        if(strcmp(pEntry->keyWord, keyword) == 0)
        {
            return pEntry;
        }
        pEntry++;
    }

    return NULL;
}

static TCPIP_HTTP_CHUNK_RES F_HTTP_ProcessDynVarChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    TCPIP_HTTP_CHUNK_RES chunkRes;

    TCPIPStack_Assert((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_DYNVAR_VALID) != 0, __FILE__, __func__, __LINE__);
    
    if(pChDcpt->status == (uint8_t)TCPIP_HTTP_CHUNK_STATE_BEG)
    {   // call the dynamic variable print function (again)
        chunkRes = F_HTTP_DynVarCallback(pHttpCon, pChDcpt);
        if(chunkRes == HTTP_CHUNK_RES_WAIT)
        {   // user needs a break
            return HTTP_CHUNK_RES_WAIT;
        }
        pChDcpt->status = (uint8_t)TCPIP_HTTP_CHUNK_STATE_DATA;
        pChDcpt->phase = (uint8_t)TCPIP_HTTP_DYNVAR_PHASE_START;
    }


    if(pChDcpt->status == (uint8_t)TCPIP_HTTP_CHUNK_STATE_DATA)
    {   // process the user's data
        if(!F_HTTP_DynVarProcess(pHttpCon, pChDcpt))
        {
            return HTTP_CHUNK_RES_WAIT;
        }

        // done with the data
        // check if we need to call again
        if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_DYNVAR_AGAIN) != 0)
        {   // yep, run another turn
            pChDcpt->status = (uint8_t)TCPIP_HTTP_CHUNK_STATE_BEG;
            return HTTP_CHUNK_RES_OK;
        }

        // we're done
        pChDcpt->status = (uint8_t)TCPIP_HTTP_CHUNK_STATE_DONE;
    }

    // when we're done, and need to delete ourselves
    // make sure we're executing now
    if(pChDcpt == FC_SglNode2CunkDcpt(pHttpCon->chunkList.head))
    {   // we're done
        F_HTTP_FreeChunk(pHttpCon, pChDcpt);
        return HTTP_CHUNK_RES_DONE; 
    } 

    return HTTP_CHUNK_RES_OK; 
}

// performs the calling of a callback for a dynamic variable
// detects if default needs to be called, etc.
// returns: HTTP_CHUNK_RES_WAIT, HTTP_CHUNK_RES_OK or HTTP_CHUNK_RES_DONE
static TCPIP_HTTP_CHUNK_RES F_HTTP_DynVarCallback(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
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

    if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEF_PROCESS) == 0)
    {   // call the user processing
        if(!F_HTTP_DbgKillUserDynVar())
        {
            if(httpUserCback->dynamicPrint == NULL)
            {
                pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEF_PROCESS;
            }
            else
            {
                printRes = (*httpUserCback->dynamicPrint)(pHttpCon, pDynDcpt, httpUserCback);
                F_HTTP_DynDbgCallback(pDynDcpt->dynName, printRes, pHttpCon->connIx);
                if(printRes == TCPIP_HTTP_DYN_PRINT_RES_DEFAULT)
                {
                    pChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEF_PROCESS;
                }
                else if(printRes == TCPIP_HTTP_DYN_PRINT_RES_AGAIN || printRes == TCPIP_HTTP_DYN_PRINT_RES_PROCESS_AGAIN)
                {
                    if(++pDynChunkDcpt->dynRetries > pInstance->dynVarRetries)
                    {
                        printRes = TCPIP_HTTP_DYN_PRINT_RES_DONE; 
                        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_DYNVAR_RETRIES_EXCEEDED, pDynDcpt->dynName);
                    }
                }
                else
                {
                    // OK
                }
            }
        }
    }

    if((pChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEF_PROCESS) != 0)
    {   // call the default implementation!
        if((pKEntry = pDynChunkDcpt->pKEntry) != NULL)
        {
            printRes = (*pKEntry->dynamicPrint)(pHttpCon, pDynDcpt, httpUserCback);
            F_HTTP_DynDbgCallback(pDynDcpt->dynName, printRes, pHttpCon->connIx);
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
        return printRes == TCPIP_HTTP_DYN_PRINT_RES_AGAIN ? HTTP_CHUNK_RES_WAIT : HTTP_CHUNK_RES_OK;
    }

    // don't need calling again
    return HTTP_CHUNK_RES_DONE;
}

// process the data that's passed during the dynamic variable callback
// returns true if done
// false if needs to be called again
static bool F_HTTP_DynVarProcess(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    uint32_t chunkSize;
    uint16_t writeSize, outSize;
    uint8_t* writeBuff;
    bool     needAck;
    const void* userBuff;
    TCPIP_HTTP_DYNVAR_BUFF_DCPT*    pDynDcpt;

    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 
    const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pInstance->httpRegistry;

    if(pChDcpt->phase == (uint8_t)TCPIP_HTTP_DYNVAR_PHASE_START)
    {   // just starting, get the chunk size
        chunkSize = 0;
        for(pDynDcpt = FC_SglNode2DynVarDcpt(pChDcpt->dynChDcpt.dynBuffList.head); pDynDcpt != NULL; pDynDcpt = pDynDcpt->next)
        {
            chunkSize += pDynDcpt->dynBufferSize;
        }
        if(chunkSize == 0)
        {   // nothing to do
            pChDcpt->phase = (uint8_t)TCPIP_HTTP_DYNVAR_PHASE_DONE;
            return true;
        }
        // show the dynVar outputs some data
        pChDcpt->pRootDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_OUT_DATA;

        if(F_HTTP_StartHttpChunk(pHttpCon, chunkSize) == 0)
        {   // wait some more... and recalculate (avoid chunk storage for the size)!
            return false;
        }
        // start sending data
        pChDcpt->phase = (uint8_t)TCPIP_HTTP_DYNVAR_PHASE_DATA;
    }

    while(pChDcpt->phase == (uint8_t)TCPIP_HTTP_DYNVAR_PHASE_DATA)
    {   // ok, there's some work to be done; try to fill the socket buffer
        pDynDcpt = FC_SglNode2DynVarDcpt(pChDcpt->dynChDcpt.dynBuffList.head);

        if(pDynDcpt == NULL)
        {   // done; signal end chunk
            pChDcpt->phase = (uint8_t)TCPIP_HTTP_DYNVAR_PHASE_END;
            break;
        }
    
        writeSize = pDynDcpt->dynBufferSize - pDynDcpt->writeOffset;
        TCPIPStack_Condition(writeSize != 0, __FILE__, __func__, __LINE__);
        writeBuff = FC_CUPtr2UPtr((const uint8_t*)pDynDcpt->dynBuffer + pDynDcpt->writeOffset);
        outSize = NET_PRES_SocketWrite(pHttpCon->netSocket, writeBuff, writeSize);
        pDynDcpt->writeOffset += outSize;
        if(outSize != writeSize)
        {   // couldn't write all of it; wait some more
            return false;
        }

        if(pDynDcpt->writeOffset == pDynDcpt->dynBufferSize)
        {   // buffer done
            pDynDcpt = FC_SglNode2DynVarDcpt(TCPIP_Helper_SingleListHeadRemove(&pChDcpt->dynChDcpt.dynBuffList));
            needAck = (pDynDcpt->dynFlags & TCPIP_HTTP_DYNVAR_BUFF_FLAG_ACK) != 0;
            userBuff = pDynDcpt->dynBuffer;
            F_HTTP_DynDbgProcess(pChDcpt->dynChDcpt.pDynAllocDcpt->dynDcpt.dynName, pDynDcpt->dynBufferSize, needAck, pHttpCon->connIx);
            F_HTTP_ReleaseDynBuffDescriptor(pInstance, pDynDcpt);
            if(needAck)
            {
                if(httpUserCback->dynamicAck != NULL)
                {
                    (*httpUserCback->dynamicAck)(pHttpCon, userBuff, httpUserCback);
                }
            }
        }
    }

    if(pChDcpt->phase == (uint8_t)TCPIP_HTTP_DYNVAR_PHASE_END)
    {
        if(F_HTTP_EndHttpChunk(pHttpCon, TCPIP_HTTP_CHUNK_END_CURRENT) == 0)
        {
            return false;
        }
    }
    
    pChDcpt->phase = (uint8_t)TCPIP_HTTP_DYNVAR_PHASE_DONE;
    return true;

}

// gets a new buffer descriptor and adds it to the existing chunk list
// assumes the chunk is a dynamic variable one!
static TCPIP_HTTP_DYNVAR_BUFF_DCPT* F_HTTP_GetDynBuffDescriptor(TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    TCPIP_HTTP_DYNVAR_BUFF_DCPT*    pDynDcpt;

    TCPIP_HTTP_INST* pInstance = pChDcpt->pOwnerCon->connInstance; 
    SINGLE_LIST* httpDynVarPool = &pInstance->dynVarPool;
    pDynDcpt = FC_SglNode2DynVarDcpt(TCPIP_Helper_SingleListHeadRemove(httpDynVarPool));
    if(pDynDcpt != NULL)
    {   // initialize the buffer
        (void)memset(pDynDcpt, 0, sizeof(*pDynDcpt));
        TCPIP_Helper_SingleListTailAdd(&pChDcpt->dynChDcpt.dynBuffList, FC_DynVarDcpt2SglNode(pDynDcpt));
    }
    else
    {
        pInstance->dynPoolEmpty++;
    }

    return pDynDcpt;
}

static void F_HTTP_ReleaseDynBuffDescriptor(TCPIP_HTTP_INST* pInstance, TCPIP_HTTP_DYNVAR_BUFF_DCPT* pDynDcpt)
{
    TCPIP_Helper_SingleListTailAdd(&pInstance->dynVarPool, FC_DynVarDcpt2SglNode(pDynDcpt));
}

// dynamic variable API functions
bool TCPIP_HTTP_DynamicWrite(const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const void * dataBuff, uint16_t size, bool needAck)
{

    if(dataBuff == NULL)
    {
        return false;   // do nothing
    }

    TCPIP_HTTP_DYNVAR_BUFF_DCPT*    pDynBuffDcpt;
    TCPIP_HTTP_CHUNK_DCPT* pChDcpt = FC_DynCtxt2CunkDcpt(varDcpt->dynContext);
    TCPIP_HTTP_INST* pInstance = pChDcpt->pOwnerCon->connInstance; 

    // this should be a dynamic data chunk!
    TCPIPStack_Assert((pChDcpt->flags & (TCPIP_HTTP_CHUNK_FLAG_TFILE | TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI)) == 0, __FILE__, __func__, __LINE__);

    if(size == 0)
    {   // buffer done!
        const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pInstance->httpRegistry;
        if(needAck && httpUserCback->dynamicAck != NULL)
        {
            (*httpUserCback->dynamicAck)(pChDcpt->pOwnerCon, dataBuff, httpUserCback);
        }
        return true;
    }

    // get a dynamic descriptor
    pDynBuffDcpt = F_HTTP_GetDynBuffDescriptor(pChDcpt);

    if(pDynBuffDcpt != NULL)
    {   // no more buffers
        pDynBuffDcpt->dynBuffer = dataBuff;
        pDynBuffDcpt->dynBufferSize = size;
        if(needAck)
        {
            pDynBuffDcpt->dynFlags |= TCPIP_HTTP_DYNVAR_BUFF_FLAG_ACK;
        }
    }

    return pDynBuffDcpt != NULL;
}

bool TCPIP_HTTP_DynamicWriteString(const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const char* str, bool needAck)
{
    return str != NULL ? TCPIP_HTTP_DynamicWrite(varDcpt, str, (uint16_t)strlen(str), needAck) : false;
}

#else
// dynamic variable API functions
bool TCPIP_HTTP_DynamicWrite(const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, const void * dataBuff, uint16_t size, bool needAck)
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
static char* F_HTTP_SSILineParse(char* lineBuff, char** pEndProcess, bool verifyOnly)
{
    char* ssiStart = strstr(lineBuff, TCPIP_HTTP_SSI_COMMAND_START);

    while(ssiStart != NULL && (ssiStart == lineBuff || (*(ssiStart - 1) == TCPIP_HTTP_NET_LINE_END)))
    {   // SSI comment allowed only at line start
        // check the line is formatted OK
        char* cmdStart = ssiStart + strlen(TCPIP_HTTP_SSI_COMMAND_START);
        char* lineEnd = strchr(lineBuff, (int)TCPIP_HTTP_NET_LINE_END);  // check if there is EOL 

        char* ssiEnd = strstr(cmdStart, TCPIP_HTTP_SSI_COMMENT_DELIM);
        if(ssiEnd == NULL)
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

        if(lineEnd != NULL && (ssiStart < lineEnd && lineEnd < ssiEnd))
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
                *ssiStop = '\0';
                return cmdStart;
            }
            return ssiStart;
        }

        // done; and failed
        break;
    }

    return NULL;
}

// 
// extracts the SSI command and attributes from the connection current file
// stores all the SSI command attribute/value pairs
// Note: strings are kept in the fileBuffer belonging to the parent file!
// returns true for success, false for error
static bool  F_HTTP_SSIExtract(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt)
{
    size_t  fileLen, len;
    int32_t extraLen;
    char    *ssiBuff;
    char    *startSsiCmd, *endSsiCmd;
    char    *ssiCmd;
    char    *attrStr, *attrVal;
    size_t  nAttribs, nStaticAttribs, nAllocAttribs;
    const TCPIP_HTTP_SSI_PROCESS_ENTRY* pEntry;
    TCPIP_HTTP_SSI_ATTR_DCPT* pAttrDcpt;
    TCPIP_HTTP_SSI_ATTR_DCPT* pAllocAttrib; 
    TCPIP_HTTP_SSI_NOTIFY_DCPT  notifyDcpt;
    bool                        success;
    const void*                 evInfo = NULL;
    TCPIP_HTTP_EVENT_TYPE   evType = TCPIP_HTTP_EVENT_NONE;
    
    TCPIP_HTTP_SSI_ATTR_DCPT  ssiAttribTbl[TCPIP_HTTP_SSI_ATTRIBUTES_MAX_NUMBER];


    ssiBuff = pFileChDcpt->fileChDcpt.fileBuffDcpt->fileBuffer;
    TCPIPStack_Assert(TCPIP_HTTP_SSI_CMD_MAX_LEN < pFileChDcpt->fileChDcpt.fileBuffDcpt->fileBufferSize, __FILE__, __func__, __LINE__);

    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 
    const SYS_FS_SHELL_OBJ* fileShell = pHttpCon->connPort->fileShell;
    success = false;
    endSsiCmd = NULL;

    while(true)
    {
        len = pFileChDcpt->fileChDcpt.fSize - pFileChDcpt->fileChDcpt.fOffset;
        if(len > (size_t)TCPIP_HTTP_SSI_CMD_MAX_LEN)
        {
            len = (size_t)TCPIP_HTTP_SSI_CMD_MAX_LEN;
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

        ssiBuff[fileLen] = '\0';
        TCPIPStack_Assert(ssiBuff[0] == TCPIP_HTTP_SSI_COMMAND_START_CHAR, __FILE__, __func__, __LINE__);

        // parse the dynamic variable
        startSsiCmd = F_HTTP_SSILineParse(ssiBuff, &endSsiCmd, false);

        if(startSsiCmd == NULL)
        {   // some parsing error; should not happen since it was parsed properly before
            TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
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
            if((attrStr = strtok(NULL, TCPIP_HTTP_SSI_ATTR_SEP)) == NULL)
            {   // done
                break;
            }
            if((attrVal = strtok(NULL, TCPIP_HTTP_SSI_VALUE_SEP)) != NULL)
            {
                if(strcspn(attrVal, " ") == 0)
                {   // if all blanks, ignore it
                    attrVal = NULL;
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
        if(httpUserCback->ssiNotify != NULL)
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
        pEntry = F_HTTP_SSIFindEntry(ssiCmd);
        evInfo = ssiCmd;

        if(pEntry == NULL)
        {   // no such command
            evType = TCPIP_HTTP_EVENT_SSI_COMMAND_ERROR;
            break;
        }

    
        if(nAttribs == 0U)
        {   // couldn't get the command attributes
            evType = TCPIP_HTTP_EVENT_SSI_ATTR_ERROR;
            break;
        }
        else if(nAttribs > sizeof(ssiAttribTbl) / sizeof(*ssiAttribTbl))
        {
            evType = TCPIP_HTTP_EVENT_SSI_ATTR_N_TRUNCATED;
            nAttribs = sizeof(ssiAttribTbl) / sizeof(*ssiAttribTbl);
        }
        else
        {
            // OK
        }

        pAllocAttrib = NULL;
        nStaticAttribs = nAttribs;
        if(nStaticAttribs > sizeof(pChDcpt->ssiChDcpt.staticAttribs) / sizeof(*pChDcpt->ssiChDcpt.staticAttribs))
        {
            nStaticAttribs = sizeof(pChDcpt->ssiChDcpt.staticAttribs) / sizeof(*pChDcpt->ssiChDcpt.staticAttribs);
        }
        nAllocAttribs = nAttribs - nStaticAttribs;

        if(nAllocAttribs != 0U)
        {   // SSI command with excess attributes
            // allocate space for the SSI descriptor  
            pAllocAttrib = (TCPIP_HTTP_SSI_ATTR_DCPT*)http_mallocFnc(nAllocAttribs * sizeof(TCPIP_HTTP_SSI_ATTR_DCPT));

            if(pAllocAttrib == NULL)
            {   // allocation failed
                evType = TCPIP_HTTP_EVENT_SSI_ALLOC_DESCRIPTOR_ERROR;
                break;
            } 
        }

        // copy the SSI attributes info
        if(nStaticAttribs != 0U)
        {
            (void)memcpy(pChDcpt->ssiChDcpt.staticAttribs, ssiAttribTbl, nStaticAttribs * sizeof(TCPIP_HTTP_SSI_ATTR_DCPT)); 
        }
        if(nAllocAttribs != 0U)
        {
            (void)memcpy(pAllocAttrib, ssiAttribTbl + nStaticAttribs, nAllocAttribs * sizeof(TCPIP_HTTP_SSI_ATTR_DCPT)); 
        }

        // construct the SSI info
        pChDcpt->ssiChDcpt.ssiCmd = ssiCmd;
        pChDcpt->ssiChDcpt.fileName = pFileChDcpt->chunkFName;
        pChDcpt->ssiChDcpt.ssiFnc = pEntry->ssiFnc;
        pChDcpt->ssiChDcpt.nStaticAttribs = (uint16_t)nStaticAttribs;
        pChDcpt->ssiChDcpt.nAllocAttribs = (uint16_t)nAllocAttribs;
        pChDcpt->ssiChDcpt.pAllocAttribs = pAllocAttrib;

        // success;
        success = true;
        break;
    }

    if(evType != TCPIP_HTTP_EVENT_NONE)
    {
       F_HTTP_Report_ConnectionEvent(pHttpCon, evType, evInfo);
    }

    // adjust the file
    extraLen = (int32_t)fileLen - (endSsiCmd - ssiBuff);
    if(extraLen != 0)
    {
        (void)(*fileShell->fileSeek)(fileShell, pFileChDcpt->fileChDcpt.fHandle, -extraLen, SYS_FS_SEEK_CUR);
    }

    // adjust the file byte counters with read characters
    TCPIPStack_Assert(endSsiCmd - ssiBuff >= 0, __FILE__, __func__, __LINE__);
    pFileChDcpt->fileChDcpt.fOffset += endSsiCmd - ssiBuff;

    return success;
}

static TCPIP_HTTP_CHUNK_RES F_HTTP_ProcessSSIChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt)
{
    uint16_t                currAttrib, leftAttribs;
    TCPIP_HTTP_CHUNK_RES    chunkRes;
    TCPIP_HTTP_SSI_ATTR_DCPT *pAttr;

    TCPIPStack_Assert(pChDcpt->ssiChDcpt.ssiFnc != NULL, __FILE__, __func__, __LINE__);

    // basic sanity check
    if(pChDcpt->ssiChDcpt.nStaticAttribs == 0)
    {   // we should have had at least one attribute pair
        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_SSI_ATTR_N_MISMATCH, pChDcpt->ssiChDcpt.ssiCmd);
        return HTTP_CHUNK_RES_SSI_ATTRIB_ERR;
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

        if(chunkRes == HTTP_CHUNK_RES_WAIT)
        {   // return back to caller needed
            return HTTP_CHUNK_RES_WAIT;
        }
        else if(chunkRes == HTTP_CHUNK_RES_OK) 
        {   // some new chunk spawned that needs to execute
            return HTTP_CHUNK_RES_OK;
        }
        else
        {
            // OK
        }

        // else either done or failed
        // even if some error occurred: ignore and continue processing the command attributes
    }

    // when we're done, delete ourselves
    // but only if we're executing now
    if(pChDcpt == FC_SglNode2CunkDcpt(pHttpCon->chunkList.head))
    {   // we're done
        F_HTTP_FreeChunk(pHttpCon, pChDcpt);
        return HTTP_CHUNK_RES_DONE; 
    }

    return HTTP_CHUNK_RES_OK; 
}

// processes an SSI include directive
// returns: see TCPIP_SSI_COMMAND_FNC definition
static TCPIP_HTTP_CHUNK_RES F_HTTP_SSIInclude(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, size_t leftAttribs)
{
    // process one attribute pair at a time
    pChDcpt->ssiChDcpt.nCurrAttrib++;
    if(strcmp(pAttr->attribute, "virtual") == 0 || strcmp(pAttr->attribute, "file") == 0)
    {   // open the requested file
        return F_HTTP_IncludeFile(pHttpCon, pChDcpt, pAttr->value);
    }

    F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_SSI_ATTR_UNKNOWN, pAttr->attribute);
    return HTTP_CHUNK_RES_DONE;
}

// processes an SSI include directive
// returns: see TCPIP_SSI_COMMAND_FNC definition
static TCPIP_HTTP_CHUNK_RES F_HTTP_SSISet(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, size_t leftAttribs)
{
    size_t  procAttribs;
    TCPIP_HTTP_SSI_ATTR_DCPT    *pAttrName, *pAttrVal;
    TCPIP_HTTP_SSI_HASH_ENTRY   *pHE, *pHRef;
    TCPIP_HTTP_DYN_ARG_TYPE     argType;
    int32_t                     intArg;
    const void*                 evInfo = NULL;
    TCPIP_HTTP_EVENT_TYPE   evType = TCPIP_HTTP_EVENT_NONE;
    TCPIP_HTTP_CHUNK_RES        chunkRes = HTTP_CHUNK_RES_DONE;

    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 
    // process two attribute pairs at a time
    while(true)
    {
        if(leftAttribs < 2U)
        {   // we should have had at least one pair or an even number; skip all remaining attributes
            procAttribs = leftAttribs;
            evType = TCPIP_HTTP_EVENT_SSI_ATTR_N_MISMATCH;
            evInfo = pChDcpt->ssiChDcpt.ssiCmd;
            chunkRes = HTTP_CHUNK_RES_SSI_ATTRIB_ERR;
            break;
        }

        // process remaining attributes
        pAttrName = pAttr;
        pAttrVal = pAttrName + 1;
        procAttribs = 2U;

        if(strcmp(pAttrName->attribute, "var") != 0 || strcmp(pAttrVal->attribute, "value") != 0)
        {   // unknown attributes
            evType = TCPIP_HTTP_EVENT_SSI_ATTR_UNKNOWN;
            evInfo =  pAttrName->attribute;
            chunkRes = HTTP_CHUNK_RES_SSI_ATTRIB_ERR;
            break;
        }

        // check that the SSI hash is created
        OA_HASH_DCPT* ssiHashDcpt = pInstance->ssiHashDcpt;
        if(ssiHashDcpt == NULL)
        {
            if((ssiHashDcpt = F_HTTP_SSICreateHash(pInstance)) == NULL)
            {
                evType = TCPIP_HTTP_EVENT_SSI_HASH_CREATE_FAILED;
                evInfo =  pChDcpt->chunkFName;
                chunkRes = HTTP_CHUNK_RES_SSI_CACHE_FAIL;
                break;
            }
            pInstance->ssiHashDcpt = ssiHashDcpt;
        }

        argType = F_HTTP_ArgType(pAttrVal->value, &intArg);
        if(argType == TCPIP_HTTP_DYN_ARG_TYPE_INVALID)
        {   // an invalid type, i.e. empty string; we delete this variable
            pHE = FC_OaHash2SsiHash(TCPIP_OAHASH_EntryLookup(ssiHashDcpt, pAttrName->value));
            if(pHE != NULL)
            {   // variable exists
                pHE->varName[0] = '\0';
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
            pHRef = FC_OaHash2SsiHash(TCPIP_OAHASH_EntryLookupOrInsert(ssiHashDcpt, pAttrVal->value + 1));
            if(pHRef == NULL)
            {   // no such variable
                evType = TCPIP_HTTP_EVENT_SSI_VAR_UNKNOWN;
                evInfo = pAttrName->value + 1;
                break;
            }
        }
        else
        {
            pHRef = NULL;
        }

        // search for the variable to be updated
        pHE = FC_OaHash2SsiHash(TCPIP_OAHASH_EntryLookupOrInsert(ssiHashDcpt, pAttrName->value));
        if(pHE == NULL)
        {   // failed, no more slots available
            evType = TCPIP_HTTP_EVENT_SSI_VAR_N_EXCEEDED;
            evInfo = pAttrName->value;
            chunkRes = HTTP_CHUNK_RES_SSI_ATTRIB_ERR;
            break;
        }

        if(pHRef != NULL)
        {   // reference
            pHE->varType = pHRef->varType;
            pHE->valInt = pHRef->valInt;
            (void)strcpy(pHE->varStr, pHRef->varStr);
            F_HTTP_DbgSSIHashEntry(pHE, true);
        }
        else
        {
            pHE->varType = (uint8_t)argType;
            if(pAttrVal->value[0] == '\\' && pAttrVal->value[1] == '$')
            {   // bypass the escape $ sequence
                pAttrVal->value += 1;
            }

            (void)strncpy(pHE->varStr, pAttrVal->value, sizeof(pHE->varStr) - 1);
            pHE->varStr[sizeof(pHE->varStr) - 1U] = '\0';

            if(argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
            {   // int value
                pHE->valInt = intArg;
            }
            // else regular string
            F_HTTP_DbgSSIHashEntry(pHE, false);
        }

        break;
    }
    
    pChDcpt->ssiChDcpt.nCurrAttrib += (uint16_t)procAttribs;

    if(evType != TCPIP_HTTP_EVENT_NONE)
    {
       F_HTTP_Report_ConnectionEvent(pHttpCon, evType, evInfo);
    }

    return chunkRes;

}

// returns: see TCPIP_SSI_COMMAND_FNC definition
static TCPIP_HTTP_CHUNK_RES F_HTTP_SSIEcho(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pChDcpt, TCPIP_HTTP_SSI_ATTR_DCPT* pAttr, size_t leftAttribs)
{
    uint16_t  echoLen, trailBytes, hdrBytes;
    uint16_t outBytes, socketBytes;
    TCPIP_HTTP_SSI_HASH_ENTRY*  pHE;
    char*                       pEcho;
    const void*                 evInfo = NULL;
    TCPIP_HTTP_EVENT_TYPE   evType = TCPIP_HTTP_EVENT_NONE;

    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 
    OA_HASH_DCPT* ssiHashDcpt = pInstance->ssiHashDcpt;

    // construct the echo response here    
#if defined(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE)
    echoLen = (strlen(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE) > sizeof(pHE->varStr)) ? (uint16_t)strlen(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE) : (uint16_t)sizeof(pHE->varStr);
    echoLen += (uint16_t)strlen(pAttr->value);    // we append the variable name to the message
#else
    echoLen = (uint16_t)sizeof(pHE->varStr);
#endif // defined(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE)

    size_t echoBuffLen = (size_t)TCPIP_HTTP_CHUNK_HEADER_LEN + (size_t)echoLen + (size_t)TCPIP_HTTP_CHUNK_FINAL_TRAILER_LEN + 1UL;
    char* echoBuffer = (char*)http_mallocFnc(echoBuffLen);
    if(echoBuffer == NULL)
    {   // allocation failed
        char eventBuff[16];
        (void)FC_sprintf(eventBuff, sizeof(eventBuff), "%d", (TCPIP_HTTP_CHUNK_HEADER_LEN + echoLen + TCPIP_HTTP_CHUNK_FINAL_TRAILER_LEN + 1U));
        F_HTTP_Report_ConnectionEvent(pHttpCon, TCPIP_HTTP_EVENT_SSI_ALLOC_ECHO_ERROR, eventBuff);
        return HTTP_CHUNK_RES_SSI_ALLOC_ERR; 
    }

    // process one attribute pair at a time
    while(true)
    {   
        pHE = NULL;
        if(pChDcpt->phase == (uint8_t)TCPIP_HTTP_DYNVAR_PHASE_START)
        {   // minimum sanity check
            if(strcmp(pAttr->attribute, "var") != 0)
            {
                evType = TCPIP_HTTP_EVENT_SSI_ATTR_UNKNOWN;
                evInfo = pAttr->attribute;
                break;
            }

            // find the requested variable
            if(ssiHashDcpt != NULL)
            {
                pHE = FC_OaHash2SsiHash(TCPIP_OAHASH_EntryLookup(ssiHashDcpt, pAttr->value));
            }

            if(pHE == NULL)
            {   // the hash was not yet created or no such variable exists
                evType = TCPIP_HTTP_EVENT_SSI_VAR_UNKNOWN;
                evInfo = pAttr->value;
            }
            else if(strlen(pHE->varStr) == 0)
            {   // a void variable
                evType = TCPIP_HTTP_EVENT_SSI_VAR_VOID;
                evInfo = pAttr->value;
            }
            else
            {
                // do nothing
            }
            pChDcpt->phase = (uint8_t)TCPIP_HTTP_DYNVAR_PHASE_DATA;
        }
        else if(ssiHashDcpt != NULL)
        {   // pHE == NULL; a retry
            pHE = FC_OaHash2SsiHash(TCPIP_OAHASH_EntryLookup(ssiHashDcpt, pAttr->value));
        }
        else
        {
            // OK
        }

        // echo it
        pEcho = echoBuffer + TCPIP_HTTP_CHUNK_HEADER_LEN;   // point to data

        if(pHE != NULL)
        {
            (void)strcpy(pEcho, pHE->varStr);
            echoLen = (uint16_t)strlen(pHE->varStr);
        }
#if defined(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE)
        else if(strlen(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE) != 0)
        {   // insert a not found message
            (void)FC_sprintf(pEcho, echoBuffLen - (size_t)TCPIP_HTTP_CHUNK_HEADER_LEN, TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE "%s", pAttr->value);
            echoLen = (uint16_t)strlen(pEcho);
        }
#endif // defined(TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE)
        else
        {   // no output
            break;
        }

        hdrBytes = F_HTTP_PrependStartHttpChunk(pHttpCon, pEcho, echoLen);
        trailBytes = F_HTTP_AppendEndHttpChunk(pHttpCon, pEcho + echoLen, TCPIP_HTTP_CHUNK_END_CURRENT);
        outBytes = hdrBytes + echoLen + trailBytes;
        socketBytes = NET_PRES_SocketWriteIsReady(pHttpCon->netSocket, outBytes, 0U);
        if(socketBytes < outBytes)
        {
            http_freeFnc(echoBuffer);
            return HTTP_CHUNK_RES_WAIT;
        }

        socketBytes = NET_PRES_SocketWrite(pHttpCon->netSocket, pEcho - hdrBytes, outBytes);
        TCPIPStack_Assert(socketBytes == outBytes, __FILE__, __func__, __LINE__);

        break;
    }

    if(evType != TCPIP_HTTP_EVENT_NONE)
    {
        F_HTTP_Report_ConnectionEvent(pHttpCon, evType, evInfo);
    }

    http_freeFnc(echoBuffer);

    pChDcpt->ssiChDcpt.nCurrAttrib++;
    return HTTP_CHUNK_RES_DONE;
}

bool TCPIP_HTTP_SSIVariableGet(size_t instIx, const char* varName, TCPIP_HTTP_DYN_ARG_DCPT* pVarDcpt)
{

    if(pVarDcpt != NULL && instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);

        if(pInstance->ssiHashDcpt != NULL)
        {
            TCPIP_HTTP_SSI_HASH_ENTRY*  pHE = FC_OaHash2SsiHash(TCPIP_OAHASH_EntryLookup(pInstance->ssiHashDcpt, varName));
            if(pHE != NULL)
            {   // found variable
                pVarDcpt->argType = (uint16_t)pHE->varType;
                if(pHE->varType == (uint8_t)TCPIP_HTTP_DYN_ARG_TYPE_INT32)
                {
                    pVarDcpt->argInt32 = pHE->valInt;
                }
                else
                {
                    pVarDcpt->argStr = pHE->varStr;
                }
                return true;
            }
        }
    }

    return false;
}

bool TCPIP_HTTP_SSIVariableSet(size_t instIx, const char* varName, const TCPIP_HTTP_DYN_ARG_DCPT* pVarDcpt)
{
    if(pVarDcpt != NULL && instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);
        OA_HASH_DCPT* ssiHashDcpt = pInstance->ssiHashDcpt;
        TCPIP_HTTP_SSI_HASH_ENTRY*  pHE = NULL;

        if(ssiHashDcpt == NULL)
        {
            if((ssiHashDcpt = F_HTTP_SSICreateHash(pInstance)) == NULL)
            {
                F_HTTP_Report_ConnectionEvent(NULL, TCPIP_HTTP_EVENT_SSI_HASH_CREATE_FAILED, NULL);
            }
            pInstance->ssiHashDcpt = ssiHashDcpt;
        }

        if(ssiHashDcpt != NULL)
        {
            pHE = FC_OaHash2SsiHash(TCPIP_OAHASH_EntryLookupOrInsert(ssiHashDcpt, varName));
            if(pHE != NULL)
            {   // found/created variable
                pHE->varType = (uint8_t)pVarDcpt->argType;
                (void)strncpy(pHE->varStr, pVarDcpt->argStr, sizeof(pHE->varStr) - 1);
                pHE->varStr[sizeof(pHE->varStr) - 1] = '\0';
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
    if(varName != NULL && instIx < httpInstNo)
    {
        TCPIP_HTTP_INST* pInstance = *(httpInstances + instIx);
        OA_HASH_DCPT* ssiHashDcpt = pInstance->ssiHashDcpt;

        if(ssiHashDcpt != NULL)
        {
            TCPIP_HTTP_SSI_HASH_ENTRY*  pHE = FC_OaHash2SsiHash(TCPIP_OAHASH_EntryLookup(ssiHashDcpt, varName));
            if(pHE != NULL)
            {   // found variable
                pHE->varName[0] = '\0';
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

        if(ssiHashDcpt != NULL)
        {
            if(pMaxNo != NULL)
            {
                *pMaxNo = ssiHashDcpt->hEntries;
            }

            return ssiHashDcpt->fullSlots;
        }
    }

    if(pMaxNo != NULL)
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

        if(ssiHashDcpt != NULL)
        {
            TCPIP_HTTP_SSI_HASH_ENTRY*  pHE = FC_OaHash2SsiHash(TCPIP_OAHASH_EntryGet(ssiHashDcpt, varIndex));
            if(pHE && pHE->hEntry.flags.busy != 0)
            {   // found variable
                if(pVarDcpt != NULL)
                {
                    pVarDcpt->argType = (uint8_t)pHE->varType;
                    if(pHE->varType == (uint8_t)TCPIP_HTTP_DYN_ARG_TYPE_INT32)
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
static const TCPIP_HTTP_SSI_PROCESS_ENTRY*  F_HTTP_SSIFindEntry(const char* ssiCmd)
{

    size_t ix;

    const TCPIP_HTTP_SSI_PROCESS_ENTRY* pEntry = T_HTTP_SSIProc_Tbl;

    for(ix = 0; ix < sizeof(T_HTTP_SSIProc_Tbl) / sizeof(*T_HTTP_SSIProc_Tbl); ix++)
    {
        if(strcmp(ssiCmd, pEntry->ssiCmd) == 0)
        {   // found
            return pEntry;
        }
        pEntry++;
    }

    return NULL;
}

static OA_HASH_DCPT* F_HTTP_SSICreateHash(TCPIP_HTTP_INST* pInstance)
{
    OA_HASH_DCPT*   pSsiHash;

    pSsiHash = (OA_HASH_DCPT*)http_mallocFnc(sizeof(OA_HASH_DCPT) + TCPIP_HTTP_SSI_VARIABLES_NUMBER * sizeof(TCPIP_HTTP_SSI_HASH_ENTRY));
    if(pSsiHash != NULL)
    {   // success; populate the entries
        pSsiHash->memBlk = pSsiHash + 1;
        pSsiHash->hParam = NULL;    // not used for now
        pSsiHash->hEntrySize = sizeof(TCPIP_HTTP_SSI_HASH_ENTRY);
        pSsiHash->hEntries = TCPIP_HTTP_SSI_VARIABLES_NUMBER;
        pSsiHash->probeStep = TCPIP_HTTP_SSI_HASH_PROBE_STEP;

#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
        // dynamic manipulation should be enabled by default
        pSsiHash->hashF = &TCPIP_HTTP_SSI_HashKeyHash;
#if defined(OA_DOUBLE_HASH_PROBING)
        pSsiHash->probeHash = &TCPIP_HTTP_SSI_HashProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
        pSsiHash->delF = NULL;      // we don't delete variables unless requested
        pSsiHash->cmpF = &TCPIP_HTTP_SSI_HashKeyCompare;
        pSsiHash->cpyF = &TCPIP_HTTP_SSI_HashKeyCopy; 
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
    TCPIP_HTTP_SSI_HASH_ENTRY* pHE = FC_OaHash2SsiHash(hEntry);
    
    return strcmp(pHE->varName, (const char*)key);
}

static void TCPIP_HTTP_SSI_HashKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    TCPIP_HTTP_SSI_HASH_ENTRY* pHE = FC_OaHash2SsiHash(dstEntry);

    (void)strncpy(pHE->varName, (const char*)key, sizeof(pHE->varName) - 1);
    pHE->varName[sizeof(pHE->varName) - 1] = '\0';
}

#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )


#endif // (TCPIP_HTTP_SSI_PROCESS != 0)

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0)
// adds a dynamic data chunk to this connection
// returns: 
//  HTTP_CHUNK_RES_OK - everything OK, needs to be processed
//  HTTP_CHUNK_RES_WAIT - chunk pool is empty, wait needed
//  < 0         - error, the dynamic chunk is abandoned
// uses the file descriptor from where this originated...
static TCPIP_HTTP_CHUNK_RES F_HTTP_AddDynChunk(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_CHUNK_DCPT* pFileChDcpt)
{
    uint16_t    dataFlags;  // TCPIP_HTTP_CHUNK_FLAGS value
    TCPIP_HTTP_CHUNK_DCPT* pDynChDcpt = NULL;
    TCPIP_HTTP_CHUNK_RES chunkRes = HTTP_CHUNK_RES_OK;
    TCPIP_HTTP_EVENT_TYPE evType = TCPIP_HTTP_EVENT_NONE;
    const void* evInfo = pFileChDcpt->chunkFName;
    TCPIP_HTTP_INST* pInstance = pHttpCon->connInstance; 

    while(true)
    {

        if(TCPIP_Helper_SingleListCount(&pHttpCon->chunkList) >= pHttpCon->connInstance->chunksDepth)
        {   // already at max depth
            evType = TCPIP_HTTP_EVENT_DEPTH_ERROR;
            chunkRes = HTTP_CHUNK_RES_DEPTH_ERR;
            break;
        }

        // valid dynamic variable, try to get a chunk
        dataFlags = (uint16_t)TCPIP_HTTP_CHUNK_FLAG_TDATA;
#if (TCPIP_HTTP_SSI_PROCESS != 0)
        if((pFileChDcpt->flags & TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI) != 0)
        {
            dataFlags |= (uint16_t)TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI;
        }
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)

        pDynChDcpt = F_HTTP_AllocChunk(pHttpCon, dataFlags, false, &evType);

        if(pDynChDcpt == NULL)
        {
            chunkRes = (evType < 0) ? HTTP_CHUNK_RES_RETRY_ERR : HTTP_CHUNK_RES_WAIT;
            break;
        }

        pDynChDcpt->pRootDcpt = pFileChDcpt->pRootDcpt;   // share the same root

        (void)strncpy(pDynChDcpt->chunkFName, pFileChDcpt->chunkFName, sizeof(pDynChDcpt->chunkFName) - 1);
        pDynChDcpt->chunkFName[sizeof(pDynChDcpt->chunkFName) - 1] = '\0';

#if (TCPIP_HTTP_SSI_PROCESS != 0)
        if((dataFlags & (uint16_t)TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI) != 0U)
        {
            if(!F_HTTP_SSIExtract(pHttpCon, pDynChDcpt, pFileChDcpt))
            {
                chunkRes = HTTP_CHUNK_RES_SSI_ERR;
            }
        }
#endif // (TCPIP_HTTP_SSI_PROCESS != 0)
#if (TCPIP_HTTP_DYNVAR_PROCESS != 0)
        if((dataFlags & (uint16_t)TCPIP_HTTP_CHUNK_FLAG_TDATA_SSI) == 0U)
        {   // dynamic data chunk; store the owner
            if(pInstance->httpRegistry.dynamicPrint == NULL)
            {   // use the default processing, if any;
                // don't allow changing in the middle of the variable processing!
                pDynChDcpt->flags |= TCPIP_HTTP_CHUNK_FLAG_DYNVAR_DEF_PROCESS;
            }

            if(F_HTTP_DynVarExtract(pHttpCon, pDynChDcpt, pFileChDcpt))
            {
                pDynChDcpt->dynChDcpt.pDynAllocDcpt->dynDcpt.callbackID = pFileChDcpt->fileChDcpt.fDynCount++;
            }
            else
            {   // processing failed
                chunkRes = HTTP_CHUNK_RES_DYNVAR_ERR;
            }
        }
#endif // (TCPIP_HTTP_DYNVAR_PROCESS != 0)

        break;
    }

    if(chunkRes < 0)
    {   // some error
        if(pDynChDcpt != NULL)
        {
            F_HTTP_FreeChunk(pHttpCon, pDynChDcpt);
        }
    }

    if(evType != TCPIP_HTTP_EVENT_NONE)
    {
        F_HTTP_Report_ConnectionEvent(pHttpCon, evType, evInfo);
    }

    return chunkRes;
}

// detects the type of an argument to be a TCPIP_HTTP_DYN_ARG_TYPE_INT32 or TCPIP_HTTP_DYN_ARG_TYPE_STRING
// returns the detected type
// updates the pIntArg if the argument is of type int
static TCPIP_HTTP_DYN_ARG_TYPE F_HTTP_ArgType(char* argStr, int32_t* pIntArg)
{
    int32_t argInt;

    if(argStr == NULL || strlen(argStr) == 0)
    {   // nothing there
        return TCPIP_HTTP_DYN_ARG_TYPE_INVALID;
    }

    if(FC_Str2L(argStr, 10, &argInt) < 0)
    {   // string: not an int
        return TCPIP_HTTP_DYN_ARG_TYPE_STRING;
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
        return pInstance->portDcpt ? (size_t)pInstance->dynDescriptorsNo : 0U;
    }
    return 0U;
#else
    return 0U;
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0)
}


static void F_HTTP_Report_ConnectionEvent(TCPIP_HTTP_CONN* pHttpCon, TCPIP_HTTP_EVENT_TYPE evType, const void* evInfo)
{
    const TCPIP_HTTP_USER_CALLBACK* httpUserCback = &pHttpCon->connInstance->httpRegistry;
    if(httpUserCback->eventReport != NULL)
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
    if(pHttpInfo != NULL)
    {
        pHttpInfo->httpStatus = pHttpCon->httpStatus;
        pHttpInfo->listenPort = pPort->listenPort;
        pHttpInfo->connStatus = pHttpCon->connState;
        pHttpInfo->nChunks = (uint16_t)TCPIP_Helper_SingleListCount(&pHttpCon->chunkList);
        pHttpInfo->chunkPoolEmpty = pHttpCon->chunkPoolEmpty;
        pHttpInfo->fileBufferPoolEmpty = pHttpCon->fileBufferPoolEmpty;
    }

    return true;
}

#if ((TCPIP_HTTP_DEBUG_LEVEL & TCPIP_HTTP_DEBUG_MASK_CHUNK_INFO) != 0)
bool TCPIP_HTTP_ChunkInfoGet(size_t instIx, size_t portIx, size_t connIx, TCPIP_HTTP_CHUNK_INFO* pChunkInfo, size_t nInfos)
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

    if(pChunkInfo != NULL)
    {   // fill data for each chunk
        pChDcpt = FC_SglNode2CunkDcpt(pHttpCon->chunkList.head);
        while(pChDcpt != NULL)
        {
            if(nInfos == 0U)
            {
                break;
            } 
            pChunkInfo->flags = pChDcpt->flags;
            pChunkInfo->status =  pChDcpt->status;
            (void)strncpy(pChunkInfo->chunkFName, pChDcpt->chunkFName, sizeof(pChunkInfo->chunkFName));

            pChunkInfo->dynVarName[0] = '\0';
            pChunkInfo->nDynBuffers = 0U;
            if((pChDcpt->flags & (uint16_t)TCPIP_HTTP_CHUNK_FLAG_TFILE) == 0U)
            {   // data chunk
                if(pChDcpt->dynChDcpt.pDynAllocDcpt != NULL)
                {
                    pDynDcpt = &pChDcpt->dynChDcpt.pDynAllocDcpt->dynDcpt;
                    if(pDynDcpt->dynName != NULL)
                    {
                        (void)strncpy(pChunkInfo->dynVarName, pDynDcpt->dynName, sizeof(pChunkInfo->dynVarName));
                    }
                }
                pChunkInfo->nDynBuffers = (uint16_t)TCPIP_Helper_SingleListCount(&pChDcpt->dynChDcpt.dynBuffList);
            }
            nInfos--;
            pChunkInfo++;
            pChDcpt = pChDcpt->next;
        }
    }

    return true;
}
#else
bool TCPIP_HTTP_ChunkInfoGet(size_t instIx, size_t portIx, size_t connIx, TCPIP_HTTP_CHUNK_INFO* pChunkInfo, size_t nInfos)
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
static uint16_t F_HTTP_ConnectionDiscard(TCPIP_HTTP_CONN* pHttpCon, uint16_t discardLen)
{
    uint8_t discardBuff[TCPIP_HTTP_MAX_DATA_LEN];

    uint16_t nDiscards = discardLen / (uint16_t)sizeof(discardBuff);
    uint16_t nLeft = discardLen - nDiscards * (uint16_t)sizeof(discardBuff);
    uint16_t totDiscard = 0U;


    while(nDiscards-- != 0U)
    {
        totDiscard += NET_PRES_SocketRead(pHttpCon->netSocket, discardBuff, (uint16_t)sizeof(discardBuff));
    }
    
    if(nLeft != 0U)
    {
        totDiscard += NET_PRES_SocketRead(pHttpCon->netSocket, discardBuff, nLeft);
    }

    return totDiscard;
}

#if (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)
// internal HTTP malloc, thread protection
static void* F_HTTP_Default_MallocFnc(size_t bytes)
{
    return httpMemH == NULL ? NULL : TCPIP_HEAP_Malloc(httpMemH, bytes);
}

// internal HTTP free, thread protection
static void  F_HTTP_Default_FreeFnc(void* ptr)
{
    if(httpMemH != NULL)
    {
        (void)TCPIP_HEAP_Free(httpMemH, ptr);
    }
}
#endif  // (TCPIP_HTTP_DYNVAR_PROCESS != 0) || (TCPIP_HTTP_SSI_PROCESS != 0) || defined(TCPIP_HTTP_FILE_UPLOAD_ENABLE)

#if (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)
// debug test function
TCPIP_HTTP_ACCESS_RULE* F_HTTP_TestAccess(size_t instIx, size_t portIx, char* pageName, TCPIP_HTTP_ACCESS_RULE* accRule)
{
    size_t ruleIx;
    char pagePath[SYS_FS_FILE_NAME_LEN + 1];
    char rulePath[SYS_FS_FILE_NAME_LEN + 1];
                                //

    if(instIx >= httpInstNo)
    {
        return NULL;
    }


    const TCPIP_HTTP_INST* pInst = *(httpInstances + instIx);
    if(portIx >= pInst->nPorts)
    {
        return NULL;
    }

    if(pInst->nRules == 0U)
    {   // if there are no rules, simply grant access!
        accRule->action = (uint8_t)TCPIP_HTTP_ACCESS_ACTION_ALLOW;
        return accRule;
    }

    // when are rules present the default action is block
    accRule->action = (uint8_t)TCPIP_HTTP_ACCESS_ACTION_BLOCK;
    TCPIP_HTTP_PORT_DCPT* pPort = pInst->portDcpt + portIx;

    SYS_FS_SHELL_RES fsRes = SYS_FS_Shell_GetPath(pPort->fileShell, pageName, pagePath, sizeof(pagePath) / sizeof(*pagePath));
    if(fsRes != SYS_FS_SHELL_RES_OK)
    {   // no such page exist?
        return accRule;
    }

    // evaluate the rules, including the default ones
    S_TCPIP_HTTP_ACC_RULE* pRule = pInst->rules; 
    for(ruleIx = 0; ruleIx < (size_t)pInst->nRules; ruleIx++)
    {
        // ignore inPort, inIfIx, inAddType
#if 0
        if(pRule->accRule.inPort == 0 || pRule->accRule.inPort == pPort->listenPort)
        {   // port match
            if(pRule->accRule.inIfIx < 0 || pRule->accRule.inIfIx == inIx)
            {   // interface match
                if(pRule->accRule.inAddType == 0 || pRule->accRule.inAddType == inAddType)
                {   // add type match
#endif
                    // rule applies to this page
                    fsRes = SYS_FS_Shell_GetPath(pPort->fileShell, pRule->accRule.dir, rulePath, sizeof(rulePath) / sizeof(*rulePath));
                    if(fsRes == SYS_FS_SHELL_RES_OK)
                    {   // rule in the page path; consider rule
                        // check if the rulePath and the pagePath intersect: rulePath is part of the pagePath
                        char* subDir = strstr(pagePath, rulePath);
                        if(subDir != NULL)
                        {   // rule does apply
                            *accRule = pRule->accRule;
                            return accRule;
                        }
                    }
#if 0
                }
            }
        }
#endif
        // next rule
        pRule = FC_AccRuleInc(pRule, pRule->accRule.ruleSize);
    }
        
    // no rule matched
    // the default rule is block
    return accRule;
    
}
#endif  // (M_TCPIP_HTTP_USE_ACCESS_RULES != 0)

#endif  // defined(TCPIP_STACK_USE_HTTP_SERVER_V2)

