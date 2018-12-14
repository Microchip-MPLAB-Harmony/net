/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    logging.h
  
  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

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



/* submitted by eof */


#ifndef WOLFSSL_LOGGING_H
#define WOLFSSL_LOGGING_H

#include "crypto/src/types.h"

#ifdef __cplusplus
    extern "C" {
#endif


enum  CYA_Log_Levels {
    ERROR_LOG = 0,
    INFO_LOG,
    ENTER_LOG,
    LEAVE_LOG,
    OTHER_LOG
};

typedef void (*wolfSSL_Logging_cb)(const int logLevel,
                                  const char *const logMessage);

WOLFSSL_API int wolfSSL_SetLoggingCb(wolfSSL_Logging_cb log_function);

#if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)
    WOLFSSL_LOCAL int wc_LoggingInit(void);
    WOLFSSL_LOCAL int wc_LoggingCleanup(void);
    WOLFSSL_LOCAL int wc_AddErrorNode(int error, int line, char* buf,
            char* file);
    WOLFSSL_LOCAL int wc_PeekErrorNode(int index, const char **file,
            const char **reason, int *line);
    WOLFSSL_LOCAL void wc_RemoveErrorNode(int index);
    WOLFSSL_LOCAL void wc_ClearErrorNodes(void);
    WOLFSSL_API   int wc_SetLoggingHeap(void* h);
    #if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)
        WOLFSSL_API   void wc_ERR_print_errors_fp(FILE* fp);
    #endif
#endif /* defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE) */

#ifdef DEBUG_WOLFSSL
    #if defined(_WIN32)
        #if defined(INTIME_RTOS)
            #define __func__ NULL
        #else
            #define __func__ __FUNCTION__
        #endif
    #endif

    /* a is prepended to m and b is appended, creating a log msg a + m + b */
    #define WOLFSSL_LOG_CAT(a, m, b) #a " " m " "  #b

    void WOLFSSL_ENTER(const char* msg);
    void WOLFSSL_LEAVE(const char* msg, int ret);
    #define WOLFSSL_STUB(m) \
        WOLFSSL_MSG(WOLFSSL_LOG_CAT(wolfSSL Stub, m, not implemented))

    void WOLFSSL_MSG(const char* msg);
    void WOLFSSL_BUFFER(const byte* buffer, word32 length);

#else /* DEBUG_WOLFSSL   */

    #define WOLFSSL_ENTER(m)
    #define WOLFSSL_LEAVE(m, r)
    #define WOLFSSL_STUB(m)

    #define WOLFSSL_MSG(m)
    #define WOLFSSL_BUFFER(b, l)

#endif /* DEBUG_WOLFSSL  */

#if (defined(DEBUG_WOLFSSL) || defined(WOLFSSL_NGINX)) || defined(WOLFSSL_HAPROXY)
    #if (defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE))
    void WOLFSSL_ERROR_LINE(int err, const char* func, unsigned int line,
            const char* file, void* ctx);
    #define WOLFSSL_ERROR(x) WOLFSSL_ERROR_LINE((x), __func__, __LINE__, __FILE__,NULL)
    #else
    void WOLFSSL_ERROR(int);
    #endif
#else
    #define WOLFSSL_ERROR(e)
#endif

#ifdef __cplusplus
}
#endif
#endif /* WOLFSSL_LOGGING_H */

