/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    ssl.h

  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright © 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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
//DOM-IGNORE-END

#ifndef WOLFSSL_SSL_H
#define WOLFSSL_SSL_H


/* for users not using preprocessor flags*/
#include "crypto/src/settings.h"
#include "crypto/src/version.h"


#ifndef NO_FILESYSTEM
    #ifdef FREESCALE_MQX
        #include "crypto/src/fio.h"
    #else
        #include <stdio.h>   /* ERR_printf */
    #endif
#endif

#ifdef YASSL_PREFIX
    #include "prefix_ssl.h"
#endif

#ifdef LIBWOLFSSL_VERSION_STRING
    #define WOLFSSL_VERSION LIBWOLFSSL_VERSION_STRING
#endif

#ifdef _WIN32
    /* wincrypt.h clashes */
    #undef OCSP_REQUEST
    #undef OCSP_RESPONSE
#endif



#ifdef __cplusplus
    extern "C" {
#endif

typedef struct CYASSL          CYASSL;          
typedef struct WOLFSSL_SESSION  WOLFSSL_SESSION;
typedef struct WOLFSSL_METHOD   WOLFSSL_METHOD;
typedef struct WOLFSSL_CTX      WOLFSSL_CTX;

typedef struct WOLFSSL_X509       WOLFSSL_X509;
typedef struct WOLFSSL_X509_NAME  WOLFSSL_X509_NAME;
typedef struct WOLFSSL_X509_CHAIN WOLFSSL_X509_CHAIN;

typedef struct WOLFSSL_CERT_MANAGER WOLFSSL_CERT_MANAGER;
typedef struct WOLFSSL_SOCKADDR     WOLFSSL_SOCKADDR;

/* redeclare guard */
#define WOLFSSL_TYPES_DEFINED


typedef struct WOLFSSL_RSA            WOLFSSL_RSA;
typedef struct WOLFSSL_DSA            WOLFSSL_DSA;
typedef struct WOLFSSL_CIPHER         WOLFSSL_CIPHER;
typedef struct WOLFSSL_X509_LOOKUP    WOLFSSL_X509_LOOKUP;
typedef struct WOLFSSL_X509_LOOKUP_METHOD WOLFSSL_X509_LOOKUP_METHOD;
typedef struct WOLFSSL_X509_CRL       WOLFSSL_X509_CRL;
typedef struct WOLFSSL_BIO            WOLFSSL_BIO;
typedef struct WOLFSSL_BIO_METHOD     WOLFSSL_BIO_METHOD;
typedef struct WOLFSSL_X509_EXTENSION WOLFSSL_X509_EXTENSION;
typedef struct WOLFSSL_ASN1_TIME      WOLFSSL_ASN1_TIME;
typedef struct WOLFSSL_ASN1_INTEGER   WOLFSSL_ASN1_INTEGER;
typedef struct WOLFSSL_ASN1_OBJECT    WOLFSSL_ASN1_OBJECT;
typedef struct WOLFSSL_ASN1_STRING    WOLFSSL_ASN1_STRING;
typedef struct WOLFSSL_dynlock_value  WOLFSSL_dynlock_value;

#define WOLFSSL_ASN1_UTCTIME WOLFSSL_ASN1_TIME

typedef struct WOLFSSL_EVP_PKEY {
    int type;         /* openssh dereference */
    int save_type;    /* openssh dereference */
    int pkey_sz;
    union {
        char* ptr;
    } pkey;
    #ifdef HAVE_ECC
        int pkey_curve;
    #endif
} WOLFSSL_EVP_PKEY;

typedef struct WOLFSSL_MD4_CTX {
    int buffer[32];      /* big enough to hold, check size in Init */
} WOLFSSL_MD4_CTX;


typedef struct WOLFSSL_COMP_METHOD {
    int type;            /* stunnel dereference */
} WOLFSSL_COMP_METHOD;


typedef struct WOLFSSL_X509_STORE {
    int                  cache;          /* stunnel dereference */
    WOLFSSL_CERT_MANAGER* cm;
} WOLFSSL_X509_STORE;

typedef struct WOLFSSL_ALERT {
    int code;
    int level;
} WOLFSSL_ALERT;

typedef struct WOLFSSL_ALERT_HISTORY {
    WOLFSSL_ALERT last_rx;
    WOLFSSL_ALERT last_tx;
} WOLFSSL_ALERT_HISTORY;

typedef struct WOLFSSL_X509_REVOKED {
    WOLFSSL_ASN1_INTEGER* serialNumber;          /* stunnel dereference */
} WOLFSSL_X509_REVOKED;


typedef struct WOLFSSL_X509_OBJECT {
    union {
        char* ptr;
        WOLFSSL_X509_CRL* crl;           /* stunnel dereference */
    } data;
} WOLFSSL_X509_OBJECT;


typedef struct WOLFSSL_X509_STORE_CTX {
    WOLFSSL_X509_STORE* store;    /* Store full of a CA cert chain */
    WOLFSSL_X509* current_cert;   /* stunnel dereference */
    char* domain;                /* subject CN domain name */
    void* ex_data;               /* external data, for fortress build */
    void* userCtx;               /* user ctx */
    int   error;                 /* current error */
    int   error_depth;           /* cert depth for this error */
    int   discardSessionCerts;   /* so verify callback can flag for discard */ 
} WOLFSSL_X509_STORE_CTX;


/* Valid Alert types from page 16/17 */
enum AlertDescription {
    close_notify            = 0,
    unexpected_message      = 10,
    bad_record_mac          = 20,
    decompression_failure   = 30,
    handshake_failure       = 40,
    no_certificate          = 41,
    bad_certificate         = 42,
    unsupported_certificate = 43,
    certificate_revoked     = 44,
    certificate_expired     = 45,
    certificate_unknown     = 46,
    illegal_parameter       = 47,
    decrypt_error           = 51,
    protocol_version        = 70,
    no_renegotiation        = 100,
    unrecognized_name       = 112
};


enum AlertLevel {
    alert_warning = 1,
    alert_fatal = 2
};


WOLFSSL_API WOLFSSL_METHOD *CyaSSLv3_server_method(void);
WOLFSSL_API WOLFSSL_METHOD *CyaSSLv3_client_method(void);
WOLFSSL_API WOLFSSL_METHOD *CyaTLSv1_server_method(void);  
WOLFSSL_API WOLFSSL_METHOD *CyaTLSv1_client_method(void);
WOLFSSL_API WOLFSSL_METHOD *CyaTLSv1_1_server_method(void);  
WOLFSSL_API WOLFSSL_METHOD *CyaTLSv1_1_client_method(void);
WOLFSSL_API WOLFSSL_METHOD *CyaTLSv1_2_server_method(void);  
WOLFSSL_API WOLFSSL_METHOD *CyaTLSv1_2_client_method(void);

#ifdef WOLFSSL_DTLS
    WOLFSSL_API WOLFSSL_METHOD *CyaDTLSv1_client_method(void);
    WOLFSSL_API WOLFSSL_METHOD *CyaDTLSv1_server_method(void);
    WOLFSSL_API WOLFSSL_METHOD *CyaDTLSv1_2_client_method(void);
    WOLFSSL_API WOLFSSL_METHOD *CyaDTLSv1_2_server_method(void);
#endif

#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS)

WOLFSSL_API int CyaSSL_CTX_use_certificate_file(WOLFSSL_CTX*, const char*, int);
WOLFSSL_API int CyaSSL_CTX_use_PrivateKey_file(WOLFSSL_CTX*, const char*, int);
WOLFSSL_API int CyaSSL_CTX_load_verify_locations(WOLFSSL_CTX*, const char*,
                                                const char*);
WOLFSSL_API int CyaSSL_CTX_use_certificate_chain_file(WOLFSSL_CTX *,
                                                     const char *file);
WOLFSSL_API int CyaSSL_CTX_use_RSAPrivateKey_file(WOLFSSL_CTX*, const char*, int);

WOLFSSL_API int CyaSSL_use_certificate_file(CYASSL*, const char*, int);
WOLFSSL_API int CyaSSL_use_PrivateKey_file(CYASSL*, const char*, int);
WOLFSSL_API int CyaSSL_use_certificate_chain_file(CYASSL*, const char *file);
WOLFSSL_API int CyaSSL_use_RSAPrivateKey_file(CYASSL*, const char*, int);

#ifdef WOLFSSL_DER_LOAD
    WOLFSSL_API int CyaSSL_CTX_der_load_verify_locations(WOLFSSL_CTX*,
                                                    const char*, int);
#endif

#ifdef HAVE_NTRU
    WOLFSSL_API int CyaSSL_CTX_use_NTRUPrivateKey_file(WOLFSSL_CTX*, const char*);
    /* load NTRU private key blob */
#endif

WOLFSSL_API int CyaSSL_PemCertToDer(const char*, unsigned char*, int);

#endif /* !NO_FILESYSTEM && !NO_CERTS */

WOLFSSL_API WOLFSSL_CTX* CyaSSL_CTX_new(WOLFSSL_METHOD*);
WOLFSSL_API CYASSL* CyaSSL_new(WOLFSSL_CTX*);
WOLFSSL_API int  CyaSSL_set_fd (CYASSL*, int);
WOLFSSL_API int  CyaSSL_get_fd(const CYASSL*);
WOLFSSL_API void CyaSSL_set_using_nonblock(CYASSL*, int);
WOLFSSL_API int  CyaSSL_get_using_nonblock(CYASSL*);
WOLFSSL_API int  CyaSSL_connect(CYASSL*);     /* please see note at top of README
                                             if you get an error from connect */
WOLFSSL_API int  CyaSSL_write(CYASSL*, const void*, int);
WOLFSSL_API int  CyaSSL_read(CYASSL*, void*, int);
WOLFSSL_API int  CyaSSL_peek(CYASSL*, void*, int);
WOLFSSL_API int  CyaSSL_accept(CYASSL*);
WOLFSSL_API void CyaSSL_CTX_free(WOLFSSL_CTX*);
WOLFSSL_API void CyaSSL_free(CYASSL*);
WOLFSSL_API int  CyaSSL_shutdown(CYASSL*);
WOLFSSL_API int  CyaSSL_send(CYASSL*, const void*, int sz, int flags);
WOLFSSL_API int  CyaSSL_recv(CYASSL*, void*, int sz, int flags);

WOLFSSL_API void CyaSSL_CTX_set_quiet_shutdown(WOLFSSL_CTX*, int);
WOLFSSL_API void CyaSSL_set_quiet_shutdown(CYASSL*, int);

WOLFSSL_API int  CyaSSL_get_error(CYASSL*, int);
WOLFSSL_API int  CyaSSL_get_alert_history(CYASSL*, WOLFSSL_ALERT_HISTORY *);

WOLFSSL_API int        CyaSSL_set_session(CYASSL* ssl,WOLFSSL_SESSION* session);
WOLFSSL_API WOLFSSL_SESSION* CyaSSL_get_session(CYASSL* ssl);
WOLFSSL_API void       CyaSSL_flush_sessions(WOLFSSL_CTX *ctx, long tm);
WOLFSSL_API int        CyaSSL_SetServerID(CYASSL* ssl, const unsigned char*, 
                                         int, int);

#ifdef SESSION_INDEX
WOLFSSL_API int CyaSSL_GetSessionIndex(CYASSL* ssl);
WOLFSSL_API int CyaSSL_GetSessionAtIndex(int index, WOLFSSL_SESSION* session);
#endif /* SESSION_INDEX */

#if defined(SESSION_INDEX) && defined(SESSION_CERTS)
WOLFSSL_API 
    WOLFSSL_X509_CHAIN* CyaSSL_SESSION_get_peer_chain(WOLFSSL_SESSION* session);
#endif /* SESSION_INDEX && SESSION_CERTS */

typedef int (*VerifyCallback)(int, WOLFSSL_X509_STORE_CTX*);
typedef int (*pem_password_cb)(char*, int, int, void*);

WOLFSSL_API void CyaSSL_CTX_set_verify(WOLFSSL_CTX*, int, 
                                      VerifyCallback verify_callback);
WOLFSSL_API void CyaSSL_set_verify(CYASSL*, int, VerifyCallback verify_callback);
WOLFSSL_API void CyaSSL_SetCertCbCtx(CYASSL*, void*);

WOLFSSL_API int  CyaSSL_pending(CYASSL*);

WOLFSSL_API void CyaSSL_load_error_strings(void);
WOLFSSL_API int  CyaSSL_library_init(void);
WOLFSSL_API long CyaSSL_CTX_set_session_cache_mode(WOLFSSL_CTX*, long);

/* session cache persistence */
WOLFSSL_API int  CyaSSL_save_session_cache(const char*);
WOLFSSL_API int  CyaSSL_restore_session_cache(const char*);
WOLFSSL_API int  CyaSSL_memsave_session_cache(void*, int);
WOLFSSL_API int  CyaSSL_memrestore_session_cache(const void*, int);
WOLFSSL_API int  CyaSSL_get_session_cache_memsize(void);

/* certificate cache persistence, uses ctx since certs are per ctx */
WOLFSSL_API int  CyaSSL_CTX_save_cert_cache(WOLFSSL_CTX*, const char*);
WOLFSSL_API int  CyaSSL_CTX_restore_cert_cache(WOLFSSL_CTX*, const char*);
WOLFSSL_API int  CyaSSL_CTX_memsave_cert_cache(WOLFSSL_CTX*, void*, int, int*);
WOLFSSL_API int  CyaSSL_CTX_memrestore_cert_cache(WOLFSSL_CTX*, const void*, int);
WOLFSSL_API int  CyaSSL_CTX_get_cert_cache_memsize(WOLFSSL_CTX*);

/* only supports full name from cipher_name[] delimited by : */
WOLFSSL_API int  CyaSSL_CTX_set_cipher_list(WOLFSSL_CTX*, const char*);
WOLFSSL_API int  CyaSSL_set_cipher_list(CYASSL*, const char*);

/* Nonblocking DTLS helper functions */
WOLFSSL_API int  CyaSSL_dtls_get_current_timeout(CYASSL* ssl);
WOLFSSL_API int  CyaSSL_dtls_set_timeout_init(CYASSL* ssl, int);
WOLFSSL_API int  CyaSSL_dtls_set_timeout_max(CYASSL* ssl, int);
WOLFSSL_API int  CyaSSL_dtls_got_timeout(CYASSL* ssl);
WOLFSSL_API int  CyaSSL_dtls(CYASSL* ssl);

WOLFSSL_API int  CyaSSL_dtls_set_peer(CYASSL*, void*, unsigned int);
WOLFSSL_API int  CyaSSL_dtls_get_peer(CYASSL*, void*, unsigned int*);

WOLFSSL_API int   CyaSSL_ERR_GET_REASON(int err);
WOLFSSL_API char* CyaSSL_ERR_error_string(unsigned long,char*);
WOLFSSL_API void  CyaSSL_ERR_error_string_n(unsigned long e, char* buf,
                                           unsigned long sz);

/* extras */

#define STACK_OF(x) x

WOLFSSL_API int  CyaSSL_set_ex_data(CYASSL*, int, void*);
WOLFSSL_API int  CyaSSL_get_shutdown(const CYASSL*);
WOLFSSL_API int  CyaSSL_set_rfd(CYASSL*, int);
WOLFSSL_API int  CyaSSL_set_wfd(CYASSL*, int);
WOLFSSL_API void CyaSSL_set_shutdown(CYASSL*, int);
WOLFSSL_API int  CyaSSL_set_session_id_context(CYASSL*, const unsigned char*,
                                           unsigned int);
WOLFSSL_API void CyaSSL_set_connect_state(CYASSL*);
WOLFSSL_API void CyaSSL_set_accept_state(CYASSL*);
WOLFSSL_API int  CyaSSL_session_reused(CYASSL*);
WOLFSSL_API void CyaSSL_SESSION_free(WOLFSSL_SESSION* session);
WOLFSSL_API int  CyaSSL_is_init_finished(CYASSL*);

WOLFSSL_API const char*  CyaSSL_get_version(CYASSL*);
WOLFSSL_API int  CyaSSL_get_current_cipher_suite(CYASSL* ssl);
WOLFSSL_API WOLFSSL_CIPHER*  CyaSSL_get_current_cipher(CYASSL*);
WOLFSSL_API char*        CyaSSL_CIPHER_description(WOLFSSL_CIPHER*, char*, int);
WOLFSSL_API const char*  CyaSSL_CIPHER_get_name(const WOLFSSL_CIPHER* cipher);
WOLFSSL_API const char*  CyaSSL_get_cipher(CYASSL*);
WOLFSSL_API WOLFSSL_SESSION* CyaSSL_get1_session(CYASSL* ssl);
                           /* what's ref count */

WOLFSSL_API void CyaSSL_X509_free(WOLFSSL_X509*);
WOLFSSL_API void CyaSSL_OPENSSL_free(void*);

WOLFSSL_API int CyaSSL_OCSP_parse_url(char* url, char** host, char** port,
                                     char** path, int* ssl);

WOLFSSL_API WOLFSSL_METHOD* CyaSSLv23_client_method(void);
WOLFSSL_API WOLFSSL_METHOD* CyaSSLv2_client_method(void);
WOLFSSL_API WOLFSSL_METHOD* CyaSSLv2_server_method(void);

WOLFSSL_API void CyaSSL_MD4_Init(WOLFSSL_MD4_CTX*);
WOLFSSL_API void CyaSSL_MD4_Update(WOLFSSL_MD4_CTX*, const void*, unsigned long);
WOLFSSL_API void CyaSSL_MD4_Final(unsigned char*, WOLFSSL_MD4_CTX*);


WOLFSSL_API WOLFSSL_BIO* CyaSSL_BIO_new(WOLFSSL_BIO_METHOD*);
WOLFSSL_API int  CyaSSL_BIO_free(WOLFSSL_BIO*);
WOLFSSL_API int  CyaSSL_BIO_free_all(WOLFSSL_BIO*);
WOLFSSL_API int  CyaSSL_BIO_read(WOLFSSL_BIO*, void*, int);
WOLFSSL_API int  CyaSSL_BIO_write(WOLFSSL_BIO*, const void*, int);
WOLFSSL_API WOLFSSL_BIO* CyaSSL_BIO_push(WOLFSSL_BIO*, WOLFSSL_BIO* append);
WOLFSSL_API WOLFSSL_BIO* CyaSSL_BIO_pop(WOLFSSL_BIO*);
WOLFSSL_API int  CyaSSL_BIO_flush(WOLFSSL_BIO*);
WOLFSSL_API int  CyaSSL_BIO_pending(WOLFSSL_BIO*);

WOLFSSL_API WOLFSSL_BIO_METHOD* CyaSSL_BIO_f_buffer(void);
WOLFSSL_API long CyaSSL_BIO_set_write_buffer_size(WOLFSSL_BIO*, long size);
WOLFSSL_API WOLFSSL_BIO_METHOD* CyaSSL_BIO_f_ssl(void);
WOLFSSL_API WOLFSSL_BIO*        CyaSSL_BIO_new_socket(int sfd, int flag);
WOLFSSL_API int         CyaSSL_BIO_eof(WOLFSSL_BIO*);

WOLFSSL_API WOLFSSL_BIO_METHOD* CyaSSL_BIO_s_mem(void);
WOLFSSL_API WOLFSSL_BIO_METHOD* CyaSSL_BIO_f_base64(void);
WOLFSSL_API void CyaSSL_BIO_set_flags(WOLFSSL_BIO*, int);

WOLFSSL_API int CyaSSL_BIO_get_mem_data(WOLFSSL_BIO* bio,const unsigned char** p);
WOLFSSL_API WOLFSSL_BIO* CyaSSL_BIO_new_mem_buf(void* buf, int len);


WOLFSSL_API long        CyaSSL_BIO_set_ssl(WOLFSSL_BIO*, CYASSL*, int flag);
WOLFSSL_API void        CyaSSL_set_bio(CYASSL*, WOLFSSL_BIO* rd, WOLFSSL_BIO* wr);

WOLFSSL_API int  CyaSSL_add_all_algorithms(void);

WOLFSSL_API void        CyaSSL_RAND_screen(void);
WOLFSSL_API const char* CyaSSL_RAND_file_name(char*, unsigned long);
WOLFSSL_API int         CyaSSL_RAND_write_file(const char*);
WOLFSSL_API int         CyaSSL_RAND_load_file(const char*, long);
WOLFSSL_API int         CyaSSL_RAND_egd(const char*);
WOLFSSL_API int         CyaSSL_RAND_seed(const void*, int);
WOLFSSL_API void        CyaSSL_RAND_add(const void*, int, double);

WOLFSSL_API WOLFSSL_COMP_METHOD* CyaSSL_COMP_zlib(void);
WOLFSSL_API WOLFSSL_COMP_METHOD* CyaSSL_COMP_rle(void);
WOLFSSL_API int CyaSSL_COMP_add_compression_method(int, void*);

WOLFSSL_API int CyaSSL_get_ex_new_index(long, void*, void*, void*, void*);

WOLFSSL_API void CyaSSL_set_id_callback(unsigned long (*f)(void));
WOLFSSL_API void CyaSSL_set_locking_callback(void (*f)(int, int, const char*,
                                                      int));
WOLFSSL_API void CyaSSL_set_dynlock_create_callback(WOLFSSL_dynlock_value* (*f)
                                                   (const char*, int));
WOLFSSL_API void CyaSSL_set_dynlock_lock_callback(void (*f)(int,
                                      WOLFSSL_dynlock_value*, const char*, int));
WOLFSSL_API void CyaSSL_set_dynlock_destroy_callback(void (*f)
                                     (WOLFSSL_dynlock_value*, const char*, int));
WOLFSSL_API int  CyaSSL_num_locks(void);

WOLFSSL_API WOLFSSL_X509* CyaSSL_X509_STORE_CTX_get_current_cert(
                                                        WOLFSSL_X509_STORE_CTX*);
WOLFSSL_API int   CyaSSL_X509_STORE_CTX_get_error(WOLFSSL_X509_STORE_CTX*);
WOLFSSL_API int   CyaSSL_X509_STORE_CTX_get_error_depth(WOLFSSL_X509_STORE_CTX*);

WOLFSSL_API char*       CyaSSL_X509_NAME_oneline(WOLFSSL_X509_NAME*, char*, int);
WOLFSSL_API WOLFSSL_X509_NAME*  CyaSSL_X509_get_issuer_name(WOLFSSL_X509*);
WOLFSSL_API WOLFSSL_X509_NAME*  CyaSSL_X509_get_subject_name(WOLFSSL_X509*);
WOLFSSL_API int  CyaSSL_X509_ext_isSet_by_NID(WOLFSSL_X509*, int);
WOLFSSL_API int  CyaSSL_X509_ext_get_critical_by_NID(WOLFSSL_X509*, int);
WOLFSSL_API int  CyaSSL_X509_get_isCA(WOLFSSL_X509*);
WOLFSSL_API int  CyaSSL_X509_get_isSet_pathLength(WOLFSSL_X509*);
WOLFSSL_API unsigned int CyaSSL_X509_get_pathLength(WOLFSSL_X509*);
WOLFSSL_API unsigned int CyaSSL_X509_get_keyUsage(WOLFSSL_X509*);
WOLFSSL_API unsigned char* CyaSSL_X509_get_authorityKeyID(
                                            WOLFSSL_X509*, unsigned char*, int*);
WOLFSSL_API unsigned char* CyaSSL_X509_get_subjectKeyID(
                                            WOLFSSL_X509*, unsigned char*, int*);
WOLFSSL_API int CyaSSL_X509_NAME_entry_count(WOLFSSL_X509_NAME*);
WOLFSSL_API int CyaSSL_X509_NAME_get_text_by_NID(
                                            WOLFSSL_X509_NAME*, int, char*, int);
WOLFSSL_API int         CyaSSL_X509_verify_cert(WOLFSSL_X509_STORE_CTX*);
WOLFSSL_API const char* CyaSSL_X509_verify_cert_error_string(long);
WOLFSSL_API int CyaSSL_X509_get_signature_type(WOLFSSL_X509*);
WOLFSSL_API int CyaSSL_X509_get_signature(WOLFSSL_X509*, unsigned char*, int*);

WOLFSSL_API int CyaSSL_X509_LOOKUP_add_dir(WOLFSSL_X509_LOOKUP*,const char*,long);
WOLFSSL_API int CyaSSL_X509_LOOKUP_load_file(WOLFSSL_X509_LOOKUP*, const char*,
                                            long);
WOLFSSL_API WOLFSSL_X509_LOOKUP_METHOD* CyaSSL_X509_LOOKUP_hash_dir(void);
WOLFSSL_API WOLFSSL_X509_LOOKUP_METHOD* CyaSSL_X509_LOOKUP_file(void);

WOLFSSL_API WOLFSSL_X509_LOOKUP* CyaSSL_X509_STORE_add_lookup(WOLFSSL_X509_STORE*,
                                                    WOLFSSL_X509_LOOKUP_METHOD*);
WOLFSSL_API WOLFSSL_X509_STORE*  CyaSSL_X509_STORE_new(void);
WOLFSSL_API void         CyaSSL_X509_STORE_free(WOLFSSL_X509_STORE*);
WOLFSSL_API int          CyaSSL_X509_STORE_add_cert(
                                              WOLFSSL_X509_STORE*, WOLFSSL_X509*);
WOLFSSL_API int          CyaSSL_X509_STORE_set_default_paths(WOLFSSL_X509_STORE*);
WOLFSSL_API int          CyaSSL_X509_STORE_get_by_subject(WOLFSSL_X509_STORE_CTX*,
                                   int, WOLFSSL_X509_NAME*, WOLFSSL_X509_OBJECT*);
WOLFSSL_API WOLFSSL_X509_STORE_CTX* CyaSSL_X509_STORE_CTX_new(void);
WOLFSSL_API int  CyaSSL_X509_STORE_CTX_init(WOLFSSL_X509_STORE_CTX*,
                      WOLFSSL_X509_STORE*, WOLFSSL_X509*, STACK_OF(WOLFSSL_X509)*);
WOLFSSL_API void CyaSSL_X509_STORE_CTX_free(WOLFSSL_X509_STORE_CTX*);
WOLFSSL_API void CyaSSL_X509_STORE_CTX_cleanup(WOLFSSL_X509_STORE_CTX*);

WOLFSSL_API WOLFSSL_ASN1_TIME* CyaSSL_X509_CRL_get_lastUpdate(WOLFSSL_X509_CRL*);
WOLFSSL_API WOLFSSL_ASN1_TIME* CyaSSL_X509_CRL_get_nextUpdate(WOLFSSL_X509_CRL*);

WOLFSSL_API WOLFSSL_EVP_PKEY* CyaSSL_X509_get_pubkey(WOLFSSL_X509*);
WOLFSSL_API int       CyaSSL_X509_CRL_verify(WOLFSSL_X509_CRL*, WOLFSSL_EVP_PKEY*);
WOLFSSL_API void      CyaSSL_X509_STORE_CTX_set_error(WOLFSSL_X509_STORE_CTX*,
                                                     int);
WOLFSSL_API void      CyaSSL_X509_OBJECT_free_contents(WOLFSSL_X509_OBJECT*);
WOLFSSL_API void      CyaSSL_EVP_PKEY_free(WOLFSSL_EVP_PKEY*);
WOLFSSL_API int       CyaSSL_X509_cmp_current_time(const WOLFSSL_ASN1_TIME*);
WOLFSSL_API int       CyaSSL_sk_X509_REVOKED_num(WOLFSSL_X509_REVOKED*);

WOLFSSL_API WOLFSSL_X509_REVOKED* CyaSSL_X509_CRL_get_REVOKED(WOLFSSL_X509_CRL*);
WOLFSSL_API WOLFSSL_X509_REVOKED* CyaSSL_sk_X509_REVOKED_value(
                                                      WOLFSSL_X509_REVOKED*,int);
WOLFSSL_API WOLFSSL_ASN1_INTEGER* CyaSSL_X509_get_serialNumber(WOLFSSL_X509*);

WOLFSSL_API int CyaSSL_ASN1_TIME_print(WOLFSSL_BIO*, const WOLFSSL_ASN1_TIME*);

WOLFSSL_API int  CyaSSL_ASN1_INTEGER_cmp(const WOLFSSL_ASN1_INTEGER*,
                                       const WOLFSSL_ASN1_INTEGER*);
WOLFSSL_API long CyaSSL_ASN1_INTEGER_get(const WOLFSSL_ASN1_INTEGER*);

WOLFSSL_API STACK_OF(WOLFSSL_X509_NAME)* CyaSSL_load_client_CA_file(const char*);

WOLFSSL_API void  CyaSSL_CTX_set_client_CA_list(WOLFSSL_CTX*,
                                               STACK_OF(WOLFSSL_X509_NAME)*);
WOLFSSL_API void* CyaSSL_X509_STORE_CTX_get_ex_data(WOLFSSL_X509_STORE_CTX*, int);
WOLFSSL_API int   CyaSSL_get_ex_data_X509_STORE_CTX_idx(void);
WOLFSSL_API void* CyaSSL_get_ex_data(const CYASSL*, int);

WOLFSSL_API void CyaSSL_CTX_set_default_passwd_cb_userdata(WOLFSSL_CTX*,
                                                          void* userdata);
WOLFSSL_API void CyaSSL_CTX_set_default_passwd_cb(WOLFSSL_CTX*, pem_password_cb);


WOLFSSL_API void CyaSSL_CTX_set_info_callback(WOLFSSL_CTX*, void (*)(void));

WOLFSSL_API unsigned long CyaSSL_ERR_peek_error(void);
WOLFSSL_API int           CyaSSL_GET_REASON(int);

WOLFSSL_API char* CyaSSL_alert_type_string_long(int);
WOLFSSL_API char* CyaSSL_alert_desc_string_long(int);
WOLFSSL_API char* CyaSSL_state_string_long(CYASSL*);

WOLFSSL_API WOLFSSL_RSA* CyaSSL_RSA_generate_key(int, unsigned long,
                                               void(*)(int, int, void*), void*);
WOLFSSL_API void CyaSSL_CTX_set_tmp_rsa_callback(WOLFSSL_CTX*,
                                             WOLFSSL_RSA*(*)(CYASSL*, int, int));

WOLFSSL_API int CyaSSL_PEM_def_callback(char*, int num, int w, void* key);

WOLFSSL_API long CyaSSL_CTX_sess_accept(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_connect(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_accept_good(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_connect_good(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_accept_renegotiate(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_connect_renegotiate(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_hits(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_cb_hits(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_cache_full(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_misses(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_timeouts(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_number(WOLFSSL_CTX*);
WOLFSSL_API long CyaSSL_CTX_sess_get_cache_size(WOLFSSL_CTX*);

#define WOLFSSL_DEFAULT_CIPHER_LIST ""   /* default all */
#define WOLFSSL_RSA_F4 0x10001L

enum {
    OCSP_NOCERTS     = 1,
    OCSP_NOINTERN    = 2,
    OCSP_NOSIGS      = 4,
    OCSP_NOCHAIN     = 8,
    OCSP_NOVERIFY    = 16,
    OCSP_NOEXPLICIT  = 32,
    OCSP_NOCASIGN    = 64,
    OCSP_NODELEGATED = 128,
    OCSP_NOCHECKS    = 256,
    OCSP_TRUSTOTHER  = 512,
    OCSP_RESPID_KEY  = 1024,
    OCSP_NOTIME      = 2048,

    OCSP_CERTID   = 2,
    OCSP_REQUEST  = 4,
    OCSP_RESPONSE = 8,
    OCSP_BASICRESP = 16,

    WOLFSSL_OCSP_URL_OVERRIDE = 1,
    WOLFSSL_OCSP_NO_NONCE     = 2,

    WOLFSSL_CRL_CHECKALL = 1,

    ASN1_GENERALIZEDTIME = 4,

    SSL_OP_MICROSOFT_SESS_ID_BUG = 1,
    SSL_OP_NETSCAPE_CHALLENGE_BUG = 2,
    SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG = 3,
    SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG = 4,
    SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER = 5,
    SSL_OP_MSIE_SSLV2_RSA_PADDING = 6,
    SSL_OP_SSLEAY_080_CLIENT_DH_BUG = 7,
    SSL_OP_TLS_D5_BUG = 8,
    SSL_OP_TLS_BLOCK_PADDING_BUG = 9,
    SSL_OP_TLS_ROLLBACK_BUG = 10,
    SSL_OP_ALL = 11,
    SSL_OP_EPHEMERAL_RSA = 12,
    SSL_OP_NO_SSLv3 = 13,
    SSL_OP_NO_TLSv1 = 14,
    SSL_OP_PKCS1_CHECK_1 = 15,
    SSL_OP_PKCS1_CHECK_2 = 16,
    SSL_OP_NETSCAPE_CA_DN_BUG = 17,
    SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG = 18,
    SSL_OP_SINGLE_DH_USE = 19,
    SSL_OP_NO_TICKET = 20,
    SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS = 21,
    SSL_OP_NO_QUERY_MTU = 22,
    SSL_OP_COOKIE_EXCHANGE = 23,
    SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION = 24,
    SSL_OP_SINGLE_ECDH_USE = 25,
    SSL_OP_CIPHER_SERVER_PREFERENCE = 26,

    SSL_MAX_SSL_SESSION_ID_LENGTH = 32,

    EVP_R_BAD_DECRYPT = 2,

    SSL_CB_LOOP = 4,
    SSL_ST_CONNECT = 5,
    SSL_ST_ACCEPT  = 6,
    SSL_CB_ALERT   = 7,
    SSL_CB_READ    = 8,
    SSL_CB_HANDSHAKE_DONE = 9,

    SSL_MODE_ENABLE_PARTIAL_WRITE = 2,

    BIO_FLAGS_BASE64_NO_NL = 1,
    BIO_CLOSE   = 1,
    BIO_NOCLOSE = 0,

    NID_undef = 0,

    X509_FILETYPE_PEM = 8,
    X509_LU_X509      = 9,
    X509_LU_CRL       = 12,
    
    X509_V_ERR_CRL_SIGNATURE_FAILURE = 13,
    X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD = 14,
    X509_V_ERR_CRL_HAS_EXPIRED                = 15,
    X509_V_ERR_CERT_REVOKED                   = 16,
    X509_V_ERR_CERT_CHAIN_TOO_LONG            = 17,
    X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT      = 18,
    X509_V_ERR_CERT_NOT_YET_VALID             = 19,
    X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD = 20,
    X509_V_ERR_CERT_HAS_EXPIRED               = 21,
    X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD  = 22,

    X509_V_OK = 0,

    CRYPTO_LOCK = 1,
    CRYPTO_NUM_LOCKS = 10
};

/* extras end */

#ifndef NO_FILESYSTEM
/* CyaSSL extension, provide last error from SSL_get_error
   since not using thread storage error queue */
WOLFSSL_API void  CyaSSL_ERR_print_errors_fp(FILE*, int err);
#endif

enum { /* ssl Constants */
    SSL_ERROR_NONE      =  0,   /* for most functions */
    SSL_FAILURE         =  0,   /* for some functions */
    SSL_SUCCESS         =  1,

    SSL_BAD_CERTTYPE    = -8,
    SSL_BAD_STAT        = -7,
    SSL_BAD_PATH        = -6,
    SSL_BAD_FILETYPE    = -5,
    SSL_BAD_FILE        = -4,
    SSL_NOT_IMPLEMENTED = -3,
    SSL_UNKNOWN         = -2,
    SSL_FATAL_ERROR     = -1,

    SSL_FILETYPE_ASN1    = 2,
    SSL_FILETYPE_PEM     = 1,
    SSL_FILETYPE_DEFAULT = 2, /* ASN1 */
    SSL_FILETYPE_RAW     = 3, /* NTRU raw key blob */

    SSL_VERIFY_NONE                 = 0,
    SSL_VERIFY_PEER                 = 1,
    SSL_VERIFY_FAIL_IF_NO_PEER_CERT = 2,
    SSL_VERIFY_CLIENT_ONCE          = 4,

    SSL_SESS_CACHE_OFF                = 30,
    SSL_SESS_CACHE_CLIENT             = 31,
    SSL_SESS_CACHE_SERVER             = 32,
    SSL_SESS_CACHE_BOTH               = 33,
    SSL_SESS_CACHE_NO_AUTO_CLEAR      = 34,
    SSL_SESS_CACHE_NO_INTERNAL_LOOKUP = 35,

    SSL_ERROR_WANT_READ        =  2,
    SSL_ERROR_WANT_WRITE       =  3,
    SSL_ERROR_WANT_CONNECT     =  7,
    SSL_ERROR_WANT_ACCEPT      =  8,
    SSL_ERROR_SYSCALL          =  5,
    SSL_ERROR_WANT_X509_LOOKUP = 83,
    SSL_ERROR_ZERO_RETURN      =  6,
    SSL_ERROR_SSL              = 85,

    SSL_SENT_SHUTDOWN     = 1,
    SSL_RECEIVED_SHUTDOWN = 2,
    SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER = 4,
    SSL_OP_NO_SSLv2       = 8,

    SSL_R_SSL_HANDSHAKE_FAILURE           = 101,
    SSL_R_TLSV1_ALERT_UNKNOWN_CA          = 102,
    SSL_R_SSLV3_ALERT_CERTIFICATE_UNKNOWN = 103,
    SSL_R_SSLV3_ALERT_BAD_CERTIFICATE     = 104,

    PEM_BUFSIZE = 1024
};


#ifndef NO_PSK
    typedef unsigned int (*psk_client_callback)(CYASSL*, const char*, char*,
                                    unsigned int, unsigned char*, unsigned int);
    WOLFSSL_API void CyaSSL_CTX_set_psk_client_callback(WOLFSSL_CTX*,
                                                    psk_client_callback);
    WOLFSSL_API void CyaSSL_set_psk_client_callback(CYASSL*,psk_client_callback);

    WOLFSSL_API const char* CyaSSL_get_psk_identity_hint(const CYASSL*);
    WOLFSSL_API const char* CyaSSL_get_psk_identity(const CYASSL*);

    WOLFSSL_API int CyaSSL_CTX_use_psk_identity_hint(WOLFSSL_CTX*, const char*);
    WOLFSSL_API int CyaSSL_use_psk_identity_hint(CYASSL*, const char*);

    typedef unsigned int (*psk_server_callback)(CYASSL*, const char*,
                          unsigned char*, unsigned int);
    WOLFSSL_API void CyaSSL_CTX_set_psk_server_callback(WOLFSSL_CTX*,
                                                    psk_server_callback);
    WOLFSSL_API void CyaSSL_set_psk_server_callback(CYASSL*,psk_server_callback);

    #define PSK_TYPES_DEFINED
#endif /* NO_PSK */


/* extra begins */

enum {  /* ERR Constants */
    ERR_TXT_STRING = 1
};

WOLFSSL_API unsigned long CyaSSL_ERR_get_error_line_data(const char**, int*,
                                                 const char**, int *);

WOLFSSL_API unsigned long CyaSSL_ERR_get_error(void);
WOLFSSL_API void          CyaSSL_ERR_clear_error(void);


WOLFSSL_API int  CyaSSL_RAND_status(void);
WOLFSSL_API int  CyaSSL_RAND_bytes(unsigned char* buf, int num);
WOLFSSL_API WOLFSSL_METHOD *CyaSSLv23_server_method(void);
WOLFSSL_API long CyaSSL_CTX_set_options(WOLFSSL_CTX*, long);
#ifndef NO_CERTS
  WOLFSSL_API int  CyaSSL_CTX_check_private_key(WOLFSSL_CTX*);
#endif /* !NO_CERTS */

WOLFSSL_API void CyaSSL_ERR_free_strings(void);
WOLFSSL_API void CyaSSL_ERR_remove_state(unsigned long);
WOLFSSL_API void CyaSSL_EVP_cleanup(void);

WOLFSSL_API void CyaSSL_cleanup_all_ex_data(void);
WOLFSSL_API long CyaSSL_CTX_set_mode(WOLFSSL_CTX* ctx, long mode);
WOLFSSL_API long CyaSSL_CTX_get_mode(WOLFSSL_CTX* ctx);
WOLFSSL_API void CyaSSL_CTX_set_default_read_ahead(WOLFSSL_CTX* ctx, int m);

WOLFSSL_API long CyaSSL_CTX_sess_set_cache_size(WOLFSSL_CTX*, long);

WOLFSSL_API int  CyaSSL_CTX_set_default_verify_paths(WOLFSSL_CTX*);
WOLFSSL_API int  CyaSSL_CTX_set_session_id_context(WOLFSSL_CTX*,
                                            const unsigned char*, unsigned int);
WOLFSSL_API WOLFSSL_X509* CyaSSL_get_peer_certificate(CYASSL* ssl);

WOLFSSL_API int CyaSSL_want_read(CYASSL*);
WOLFSSL_API int CyaSSL_want_write(CYASSL*);

WOLFSSL_API int CyaSSL_BIO_printf(WOLFSSL_BIO*, const char*, ...);
WOLFSSL_API int CyaSSL_ASN1_UTCTIME_print(WOLFSSL_BIO*,
                                         const WOLFSSL_ASN1_UTCTIME*);
WOLFSSL_API int   CyaSSL_sk_num(WOLFSSL_X509_REVOKED*);
WOLFSSL_API void* CyaSSL_sk_value(WOLFSSL_X509_REVOKED*, int);

/* stunnel 4.28 needs */
WOLFSSL_API void* CyaSSL_CTX_get_ex_data(const WOLFSSL_CTX*, int);
WOLFSSL_API int   CyaSSL_CTX_set_ex_data(WOLFSSL_CTX*, int, void*);
WOLFSSL_API void  CyaSSL_CTX_sess_set_get_cb(WOLFSSL_CTX*,
                       WOLFSSL_SESSION*(*f)(CYASSL*, unsigned char*, int, int*));
WOLFSSL_API void  CyaSSL_CTX_sess_set_new_cb(WOLFSSL_CTX*,
                                            int (*f)(CYASSL*, WOLFSSL_SESSION*));
WOLFSSL_API void  CyaSSL_CTX_sess_set_remove_cb(WOLFSSL_CTX*,
                                       void (*f)(WOLFSSL_CTX*, WOLFSSL_SESSION*));

WOLFSSL_API int          CyaSSL_i2d_SSL_SESSION(WOLFSSL_SESSION*,unsigned char**);
WOLFSSL_API WOLFSSL_SESSION* CyaSSL_d2i_SSL_SESSION(WOLFSSL_SESSION**,
                                                   const unsigned char**, long);

WOLFSSL_API long CyaSSL_SESSION_get_timeout(const WOLFSSL_SESSION*);
WOLFSSL_API long CyaSSL_SESSION_get_time(const WOLFSSL_SESSION*);
WOLFSSL_API int  CyaSSL_CTX_get_ex_new_index(long, void*, void*, void*, void*);

/* extra ends */


/* CyaSSL extensions */

/* call before SSL_connect, if verifying will add name check to
   date check and signature check */
WOLFSSL_API int CyaSSL_check_domain_name(CYASSL* ssl, const char* dn);

/* need to call once to load library (session cache) */
WOLFSSL_API int CyaSSL_Init(void);
/* call when done to cleanup/free session cache mutex / resources  */
WOLFSSL_API int CyaSSL_Cleanup(void);

/* turn logging on, only if compiled in */
WOLFSSL_API int  CyaSSL_Debugging_ON(void);
/* turn logging off */
WOLFSSL_API void CyaSSL_Debugging_OFF(void);

/* do accept or connect depedning on side */
WOLFSSL_API int CyaSSL_negotiate(CYASSL* ssl);
/* turn on CyaSSL data compression */
WOLFSSL_API int CyaSSL_set_compression(CYASSL* ssl);

WOLFSSL_API int CyaSSL_set_timeout(CYASSL*, unsigned int);
WOLFSSL_API int CyaSSL_CTX_set_timeout(WOLFSSL_CTX*, unsigned int);

/* get CyaSSL peer X509_CHAIN */
WOLFSSL_API WOLFSSL_X509_CHAIN* CyaSSL_get_peer_chain(CYASSL* ssl);
/* peer chain count */
WOLFSSL_API int  CyaSSL_get_chain_count(WOLFSSL_X509_CHAIN* chain);
/* index cert length */
WOLFSSL_API int  CyaSSL_get_chain_length(WOLFSSL_X509_CHAIN*, int idx);
/* index cert */
WOLFSSL_API unsigned char* CyaSSL_get_chain_cert(WOLFSSL_X509_CHAIN*, int idx);
/* index cert in X509 */
WOLFSSL_API WOLFSSL_X509* CyaSSL_get_chain_X509(WOLFSSL_X509_CHAIN*, int idx);
/* free X509 */
WOLFSSL_API void CyaSSL_FreeX509(WOLFSSL_X509*);
/* get index cert in PEM */
WOLFSSL_API int  CyaSSL_get_chain_cert_pem(WOLFSSL_X509_CHAIN*, int idx,
                                unsigned char* buffer, int inLen, int* outLen);
WOLFSSL_API const unsigned char* CyaSSL_get_sessionID(const WOLFSSL_SESSION* s);
WOLFSSL_API int  CyaSSL_X509_get_serial_number(WOLFSSL_X509*,unsigned char*,int*);
WOLFSSL_API char*  CyaSSL_X509_get_subjectCN(WOLFSSL_X509*);
WOLFSSL_API const unsigned char* CyaSSL_X509_get_der(WOLFSSL_X509*, int*);
WOLFSSL_API const unsigned char* CyaSSL_X509_notBefore(WOLFSSL_X509*);
WOLFSSL_API const unsigned char* CyaSSL_X509_notAfter(WOLFSSL_X509*);
WOLFSSL_API int CyaSSL_X509_version(WOLFSSL_X509*);
WOLFSSL_API 

WOLFSSL_API int CyaSSL_cmp_peer_cert_to_file(CYASSL*, const char*);

WOLFSSL_API char* CyaSSL_X509_get_next_altname(WOLFSSL_X509*);

WOLFSSL_API WOLFSSL_X509*
    CyaSSL_X509_d2i(WOLFSSL_X509** x509, const unsigned char* in, int len);
#ifndef NO_FILESYSTEM
WOLFSSL_API WOLFSSL_X509*
    CyaSSL_X509_d2i_fp(WOLFSSL_X509** x509, FILE* file);
WOLFSSL_API WOLFSSL_X509*
    CyaSSL_X509_load_certificate_file(const char* fname, int format);
#endif

#ifdef WOLFSSL_SEP
    WOLFSSL_API unsigned char*
           CyaSSL_X509_get_device_type(WOLFSSL_X509*, unsigned char*, int*);
    WOLFSSL_API unsigned char*
           CyaSSL_X509_get_hw_type(WOLFSSL_X509*, unsigned char*, int*);
    WOLFSSL_API unsigned char*
           CyaSSL_X509_get_hw_serial_number(WOLFSSL_X509*, unsigned char*, int*);
#endif

/* connect enough to get peer cert */
WOLFSSL_API int  CyaSSL_connect_cert(CYASSL* ssl);

/* XXX This should be #ifndef NO_DH */
#ifndef NO_CERTS
/* server Diffie-Hellman parameters */
WOLFSSL_API int  CyaSSL_SetTmpDH(CYASSL*, const unsigned char* p, int pSz,
                                const unsigned char* g, int gSz);
WOLFSSL_API int  CyaSSL_SetTmpDH_buffer(CYASSL*, const unsigned char* b, long sz,
                                       int format);
WOLFSSL_API int  CyaSSL_SetTmpEC_DHE_Sz(CYASSL*, unsigned short);
#ifndef NO_FILESYSTEM
    WOLFSSL_API int  CyaSSL_SetTmpDH_file(CYASSL*, const char* f, int format);
#endif

/* server ctx Diffie-Hellman parameters */
WOLFSSL_API int  CyaSSL_CTX_SetTmpDH(WOLFSSL_CTX*, const unsigned char* p,
                                    int pSz, const unsigned char* g, int gSz);
WOLFSSL_API int  CyaSSL_CTX_SetTmpDH_buffer(WOLFSSL_CTX*, const unsigned char* b,
                                           long sz, int format);
WOLFSSL_API int  CyaSSL_CTX_SetTmpEC_DHE_Sz(WOLFSSL_CTX*, unsigned short);

#ifndef NO_FILESYSTEM
    WOLFSSL_API int  CyaSSL_CTX_SetTmpDH_file(WOLFSSL_CTX*, const char* f,
                                             int format);
#endif
#endif

/* keyblock size in bytes or -1 */
/* need to call CyaSSL_KeepArrays before handshake to save keys */
WOLFSSL_API int CyaSSL_get_keyblock_size(CYASSL*);
WOLFSSL_API int CyaSSL_get_keys(CYASSL*,unsigned char** ms, unsigned int* msLen,
                                       unsigned char** sr, unsigned int* srLen,
                                       unsigned char** cr, unsigned int* crLen);

/* Computes EAP-TLS and EAP-TTLS keying material from the master_secret. */
WOLFSSL_API int CyaSSL_make_eap_keys(CYASSL*, void* key, unsigned int len, 
                                                             const char* label);


#ifndef _WIN32
    #ifndef NO_WRITEV
        #ifdef __PPU
            #include "crypto/src/sys/types.h"
            #include "crypto/src/sys/socket.h"
        #elif !defined(WOLFSSL_MDK_ARM)
            #include "crypto/src/sys/uio.h"
        #endif
        /* allow writev style writing */
        WOLFSSL_API int CyaSSL_writev(CYASSL* ssl, const struct iovec* iov,
                                     int iovcnt);
    #endif
#endif


#ifndef NO_CERTS
    /* SSL_CTX versions */
    WOLFSSL_API int CyaSSL_CTX_UnloadCAs(WOLFSSL_CTX*);
    WOLFSSL_API int CyaSSL_CTX_load_verify_buffer(WOLFSSL_CTX*, 
                                               const unsigned char*, long, int);
    WOLFSSL_API int CyaSSL_CTX_use_certificate_buffer(WOLFSSL_CTX*,
                                               const unsigned char*, long, int);
    WOLFSSL_API int CyaSSL_CTX_use_PrivateKey_buffer(WOLFSSL_CTX*,
                                               const unsigned char*, long, int);
    WOLFSSL_API int CyaSSL_CTX_use_certificate_chain_buffer(WOLFSSL_CTX*, 
                                                    const unsigned char*, long);

    /* SSL versions */
    WOLFSSL_API int CyaSSL_use_certificate_buffer(CYASSL*, const unsigned char*,
                                               long, int);
    WOLFSSL_API int CyaSSL_use_PrivateKey_buffer(CYASSL*, const unsigned char*,
                                               long, int);
    WOLFSSL_API int CyaSSL_use_certificate_chain_buffer(CYASSL*, 
                                               const unsigned char*, long);
    WOLFSSL_API int CyaSSL_UnloadCertsKeys(CYASSL*);
#endif

WOLFSSL_API int CyaSSL_CTX_set_group_messages(WOLFSSL_CTX*);
WOLFSSL_API int CyaSSL_set_group_messages(CYASSL*);

/* I/O callbacks */
typedef int (*CallbackIORecv)(CYASSL *ssl, char *buf, int sz, void *ctx);
typedef int (*CallbackIOSend)(CYASSL *ssl, char *buf, int sz, void *ctx);

WOLFSSL_API void CyaSSL_SetIORecv(WOLFSSL_CTX*, CallbackIORecv);
WOLFSSL_API void CyaSSL_SetIOSend(WOLFSSL_CTX*, CallbackIOSend);

WOLFSSL_API void CyaSSL_SetIOReadCtx( CYASSL* ssl, void *ctx);
WOLFSSL_API void CyaSSL_SetIOWriteCtx(CYASSL* ssl, void *ctx);

WOLFSSL_API void* CyaSSL_GetIOReadCtx( CYASSL* ssl);
WOLFSSL_API void* CyaSSL_GetIOWriteCtx(CYASSL* ssl);

WOLFSSL_API void CyaSSL_SetIOReadFlags( CYASSL* ssl, int flags);
WOLFSSL_API void CyaSSL_SetIOWriteFlags(CYASSL* ssl, int flags);

#ifdef HAVE_NETX
    WOLFSSL_API void CyaSSL_SetIO_NetX(CYASSL* ssl, NX_TCP_SOCKET* nxsocket,
                                      ULONG waitoption);
#endif

typedef int (*CallbackGenCookie)(CYASSL* ssl, unsigned char* buf, int sz,
                                 void* ctx);
WOLFSSL_API void  CyaSSL_CTX_SetGenCookie(WOLFSSL_CTX*, CallbackGenCookie);
WOLFSSL_API void  CyaSSL_SetCookieCtx(CYASSL* ssl, void *ctx);
WOLFSSL_API void* CyaSSL_GetCookieCtx(CYASSL* ssl);


/* I/O Callback default errors */
enum IOerrors {
    WOLFSSL_CBIO_ERR_GENERAL    = -1,     /* general unexpected err */
    WOLFSSL_CBIO_ERR_WANT_READ  = -2,     /* need to call read  again */
    WOLFSSL_CBIO_ERR_WANT_WRITE = -2,     /* need to call write again */
    WOLFSSL_CBIO_ERR_CONN_RST   = -3,     /* connection reset */
    WOLFSSL_CBIO_ERR_ISR        = -4,     /* interrupt */
    WOLFSSL_CBIO_ERR_CONN_CLOSE = -5,     /* connection closed or epipe */
    WOLFSSL_CBIO_ERR_TIMEOUT    = -6      /* socket timeout */
};


/* CA cache callbacks */
enum {
    WOLFSSL_SSLV3    = 0,
    WOLFSSL_TLSV1    = 1,
    WOLFSSL_TLSV1_1  = 2,
    WOLFSSL_TLSV1_2  = 3,
    WOLFSSL_USER_CA  = 1,          /* user added as trusted */
    WOLFSSL_CHAIN_CA = 2           /* added to cache from trusted chain */
};

WOLFSSL_API int CyaSSL_GetObjectSize(void);  /* object size based on build */
WOLFSSL_API int CyaSSL_SetVersion(CYASSL* ssl, int version);
WOLFSSL_API int CyaSSL_KeyPemToDer(const unsigned char*, int sz, unsigned char*,
                                  int, const char*);
WOLFSSL_API int CyaSSL_CertPemToDer(const unsigned char*, int sz, unsigned char*,
                                   int, int);

typedef void (*CallbackCACache)(unsigned char* der, int sz, int type);
typedef void (*CbMissingCRL)(const char* url);
typedef int  (*CbOCSPIO)(void*, const char*, int,
                                         unsigned char*, int, unsigned char**);
typedef void (*CbOCSPRespFree)(void*,unsigned char*);

/* User Atomic Record Layer CallBacks */
typedef int (*CallbackMacEncrypt)(CYASSL* ssl, unsigned char* macOut, 
       const unsigned char* macIn, unsigned int macInSz, int macContent, 
       int macVerify, unsigned char* encOut, const unsigned char* encIn,
       unsigned int encSz, void* ctx);
WOLFSSL_API void  CyaSSL_CTX_SetMacEncryptCb(WOLFSSL_CTX*, CallbackMacEncrypt);
WOLFSSL_API void  CyaSSL_SetMacEncryptCtx(CYASSL* ssl, void *ctx);
WOLFSSL_API void* CyaSSL_GetMacEncryptCtx(CYASSL* ssl);

typedef int (*CallbackDecryptVerify)(CYASSL* ssl, 
       unsigned char* decOut, const unsigned char* decIn,
       unsigned int decSz, int content, int verify, unsigned int* padSz,
       void* ctx);
WOLFSSL_API void  CyaSSL_CTX_SetDecryptVerifyCb(WOLFSSL_CTX*,
                                               CallbackDecryptVerify);
WOLFSSL_API void  CyaSSL_SetDecryptVerifyCtx(CYASSL* ssl, void *ctx);
WOLFSSL_API void* CyaSSL_GetDecryptVerifyCtx(CYASSL* ssl);

WOLFSSL_API const unsigned char* CyaSSL_GetMacSecret(CYASSL*, int);
WOLFSSL_API const unsigned char* CyaSSL_GetClientWriteKey(CYASSL*);
WOLFSSL_API const unsigned char* CyaSSL_GetClientWriteIV(CYASSL*);
WOLFSSL_API const unsigned char* CyaSSL_GetServerWriteKey(CYASSL*);
WOLFSSL_API const unsigned char* CyaSSL_GetServerWriteIV(CYASSL*);
WOLFSSL_API int                  CyaSSL_GetKeySize(CYASSL*);
WOLFSSL_API int                  CyaSSL_GetIVSize(CYASSL*);
WOLFSSL_API int                  CyaSSL_GetSide(CYASSL*);
WOLFSSL_API int                  CyaSSL_IsTLSv1_1(CYASSL*);
WOLFSSL_API int                  CyaSSL_GetBulkCipher(CYASSL*);
WOLFSSL_API int                  CyaSSL_GetCipherBlockSize(CYASSL*);
WOLFSSL_API int                  CyaSSL_GetAeadMacSize(CYASSL*);
WOLFSSL_API int                  CyaSSL_GetHmacSize(CYASSL*);
WOLFSSL_API int                  CyaSSL_GetHmacType(CYASSL*);
WOLFSSL_API int                  CyaSSL_GetCipherType(CYASSL*);
WOLFSSL_API int                  CyaSSL_SetTlsHmacInner(CYASSL*, unsigned char*,
                                                       unsigned int, int, int);

/* Atomic User Needs */
enum {
    WOLFSSL_SERVER_END = 0,
    WOLFSSL_CLIENT_END = 1,
    WOLFSSL_BLOCK_TYPE = 2,
    WOLFSSL_STREAM_TYPE = 3,
    WOLFSSL_AEAD_TYPE = 4,
    WOLFSSL_TLS_HMAC_INNER_SZ = 13      /* SEQ_SZ + ENUM + VERSION_SZ + LEN_SZ */
};

/* for GetBulkCipher and internal use */
enum BulkCipherAlgorithm { 
    cyassl_cipher_null,
    cyassl_rc4,
    cyassl_rc2,
    cyassl_des,
    cyassl_triple_des,             /* leading 3 (3des) not valid identifier */
    cyassl_des40,
    cyassl_idea,
    cyassl_aes,
    cyassl_aes_gcm,
    cyassl_aes_ccm,
    cyassl_camellia,
    cyassl_hc128,                  /* CyaSSL extensions */
    cyassl_rabbit
};


/* Public Key Callback support */
typedef int (*CallbackEccSign)(CYASSL* ssl, 
       const unsigned char* in, unsigned int inSz,
       unsigned char* out, unsigned int* outSz,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  CyaSSL_CTX_SetEccSignCb(WOLFSSL_CTX*, CallbackEccSign);
WOLFSSL_API void  CyaSSL_SetEccSignCtx(CYASSL* ssl, void *ctx);
WOLFSSL_API void* CyaSSL_GetEccSignCtx(CYASSL* ssl);

typedef int (*CallbackEccVerify)(CYASSL* ssl, 
       const unsigned char* sig, unsigned int sigSz,
       const unsigned char* hash, unsigned int hashSz,
       const unsigned char* keyDer, unsigned int keySz,
       int* result, void* ctx);
WOLFSSL_API void  CyaSSL_CTX_SetEccVerifyCb(WOLFSSL_CTX*, CallbackEccVerify);
WOLFSSL_API void  CyaSSL_SetEccVerifyCtx(CYASSL* ssl, void *ctx);
WOLFSSL_API void* CyaSSL_GetEccVerifyCtx(CYASSL* ssl);

typedef int (*CallbackRsaSign)(CYASSL* ssl, 
       const unsigned char* in, unsigned int inSz,
       unsigned char* out, unsigned int* outSz,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  CyaSSL_CTX_SetRsaSignCb(WOLFSSL_CTX*, CallbackRsaSign);
WOLFSSL_API void  CyaSSL_SetRsaSignCtx(CYASSL* ssl, void *ctx);
WOLFSSL_API void* CyaSSL_GetRsaSignCtx(CYASSL* ssl);

typedef int (*CallbackRsaVerify)(CYASSL* ssl, 
       unsigned char* sig, unsigned int sigSz,
       unsigned char** out,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  CyaSSL_CTX_SetRsaVerifyCb(WOLFSSL_CTX*, CallbackRsaVerify);
WOLFSSL_API void  CyaSSL_SetRsaVerifyCtx(CYASSL* ssl, void *ctx);
WOLFSSL_API void* CyaSSL_GetRsaVerifyCtx(CYASSL* ssl);

/* RSA Public Encrypt cb */
typedef int (*CallbackRsaEnc)(CYASSL* ssl, 
       const unsigned char* in, unsigned int inSz,
       unsigned char* out, unsigned int* outSz,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  CyaSSL_CTX_SetRsaEncCb(WOLFSSL_CTX*, CallbackRsaEnc);
WOLFSSL_API void  CyaSSL_SetRsaEncCtx(CYASSL* ssl, void *ctx);
WOLFSSL_API void* CyaSSL_GetRsaEncCtx(CYASSL* ssl);

/* RSA Private Decrypt cb */
typedef int (*CallbackRsaDec)(CYASSL* ssl, 
       unsigned char* in, unsigned int inSz,
       unsigned char** out,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  CyaSSL_CTX_SetRsaDecCb(WOLFSSL_CTX*, CallbackRsaDec);
WOLFSSL_API void  CyaSSL_SetRsaDecCtx(CYASSL* ssl, void *ctx);
WOLFSSL_API void* CyaSSL_GetRsaDecCtx(CYASSL* ssl);


#ifndef NO_CERTS
	WOLFSSL_API void CyaSSL_CTX_SetCACb(WOLFSSL_CTX*, CallbackCACache);

    WOLFSSL_API WOLFSSL_CERT_MANAGER* CyaSSL_CertManagerNew(void);
    WOLFSSL_API void CyaSSL_CertManagerFree(WOLFSSL_CERT_MANAGER*);

    WOLFSSL_API int CyaSSL_CertManagerLoadCA(WOLFSSL_CERT_MANAGER*, const char* f,
                                                                 const char* d);
    WOLFSSL_API int CyaSSL_CertManagerUnloadCAs(WOLFSSL_CERT_MANAGER* cm);
    WOLFSSL_API int CyaSSL_CertManagerVerify(WOLFSSL_CERT_MANAGER*, const char* f,
                                                                    int format);
    WOLFSSL_API int CyaSSL_CertManagerVerifyBuffer(WOLFSSL_CERT_MANAGER* cm,
                                const unsigned char* buff, long sz, int format);
    WOLFSSL_API int CyaSSL_CertManagerCheckCRL(WOLFSSL_CERT_MANAGER*,
                                                        unsigned char*, int sz);
    WOLFSSL_API int CyaSSL_CertManagerEnableCRL(WOLFSSL_CERT_MANAGER*,
                                                                   int options);
    WOLFSSL_API int CyaSSL_CertManagerDisableCRL(WOLFSSL_CERT_MANAGER*);
    WOLFSSL_API int CyaSSL_CertManagerLoadCRL(WOLFSSL_CERT_MANAGER*, const char*,
                                                                      int, int);
    WOLFSSL_API int CyaSSL_CertManagerSetCRL_Cb(WOLFSSL_CERT_MANAGER*,
                                                                  CbMissingCRL);
    WOLFSSL_API int CyaSSL_CertManagerCheckOCSP(WOLFSSL_CERT_MANAGER*,
                                                        unsigned char*, int sz);
    WOLFSSL_API int CyaSSL_CertManagerEnableOCSP(WOLFSSL_CERT_MANAGER*,
                                                                   int options);
    WOLFSSL_API int CyaSSL_CertManagerDisableOCSP(WOLFSSL_CERT_MANAGER*);
    WOLFSSL_API int CyaSSL_CertManagerSetOCSPOverrideURL(WOLFSSL_CERT_MANAGER*,
                                                                   const char*);
    WOLFSSL_API int CyaSSL_CertManagerSetOCSP_Cb(WOLFSSL_CERT_MANAGER*,
                                               CbOCSPIO, CbOCSPRespFree, void*);

    WOLFSSL_API int CyaSSL_EnableCRL(CYASSL* ssl, int options);
    WOLFSSL_API int CyaSSL_DisableCRL(CYASSL* ssl);
    WOLFSSL_API int CyaSSL_LoadCRL(CYASSL*, const char*, int, int);
    WOLFSSL_API int CyaSSL_SetCRL_Cb(CYASSL*, CbMissingCRL);
    WOLFSSL_API int CyaSSL_EnableOCSP(CYASSL*, int options);
    WOLFSSL_API int CyaSSL_DisableOCSP(CYASSL*);
    WOLFSSL_API int CyaSSL_SetOCSP_OverrideURL(CYASSL*, const char*);
    WOLFSSL_API int CyaSSL_SetOCSP_Cb(CYASSL*, CbOCSPIO, CbOCSPRespFree, void*);

    WOLFSSL_API int CyaSSL_CTX_EnableCRL(WOLFSSL_CTX* ctx, int options);
    WOLFSSL_API int CyaSSL_CTX_DisableCRL(WOLFSSL_CTX* ctx);
    WOLFSSL_API int CyaSSL_CTX_LoadCRL(WOLFSSL_CTX*, const char*, int, int);
    WOLFSSL_API int CyaSSL_CTX_SetCRL_Cb(WOLFSSL_CTX*, CbMissingCRL);
    WOLFSSL_API int CyaSSL_CTX_EnableOCSP(WOLFSSL_CTX*, int options);
    WOLFSSL_API int CyaSSL_CTX_DisableOCSP(WOLFSSL_CTX*);
    WOLFSSL_API int CyaSSL_CTX_SetOCSP_OverrideURL(WOLFSSL_CTX*, const char*);
    WOLFSSL_API int CyaSSL_CTX_SetOCSP_Cb(WOLFSSL_CTX*,
                                               CbOCSPIO, CbOCSPRespFree, void*);
#endif /* !NO_CERTS */

/* end of handshake frees temporary arrays, if user needs for get_keys or
   psk hints, call KeepArrays before handshake and then FreeArrays when done
   if don't want to wait for object free */
WOLFSSL_API void CyaSSL_KeepArrays(CYASSL*);
WOLFSSL_API void CyaSSL_FreeArrays(CYASSL*);


/* cavium additions */
WOLFSSL_API int CyaSSL_UseCavium(CYASSL*, int devId);
WOLFSSL_API int CyaSSL_CTX_UseCavium(WOLFSSL_CTX*, int devId);

/* TLS Extensions */

/* Server Name Indication */
#ifdef HAVE_SNI
/* SNI types */
enum {
    WOLFSSL_SNI_HOST_NAME = 0
};

WOLFSSL_API int CyaSSL_UseSNI(CYASSL* ssl, unsigned char type, const void* data,
                                                           unsigned short size);
WOLFSSL_API int CyaSSL_CTX_UseSNI(WOLFSSL_CTX* ctx, unsigned char type,
                                         const void* data, unsigned short size);

#ifndef NO_WOLFSSL_SERVER
/* SNI options */
enum {
    WOLFSSL_SNI_CONTINUE_ON_MISMATCH = 0x01, /* do not abort on mismatch flag */
    WOLFSSL_SNI_ANSWER_ON_MISMATCH   = 0x02  /* fake match on mismatch flag */
};

WOLFSSL_API void CyaSSL_SNI_SetOptions(CYASSL* ssl, unsigned char type,
                                                         unsigned char options);
WOLFSSL_API void CyaSSL_CTX_SNI_SetOptions(WOLFSSL_CTX* ctx, unsigned char type,
                                                         unsigned char options);

/* SNI status */
enum {
    WOLFSSL_SNI_NO_MATCH   = 0,
    WOLFSSL_SNI_FAKE_MATCH = 1, /* if WOLFSSL_SNI_ANSWER_ON_MISMATCH is enabled */
    WOLFSSL_SNI_REAL_MATCH = 2
};

WOLFSSL_API unsigned char CyaSSL_SNI_Status(CYASSL* ssl, unsigned char type);

WOLFSSL_API unsigned short CyaSSL_SNI_GetRequest(CYASSL *ssl, unsigned char type,
                                                                   void** data);

WOLFSSL_API int CyaSSL_SNI_GetFromBuffer(
                 const unsigned char* clientHello, unsigned int helloSz,
                 unsigned char type, unsigned char* sni, unsigned int* inOutSz);

#endif /* NO_WOLFSSL_SERVER */
#endif /* HAVE_SNI */

/* Maximum Fragment Length */
#ifdef HAVE_MAX_FRAGMENT
/* Fragment lengths */
enum {
    WOLFSSL_MFL_2_9  = 1, /*  512 bytes */
    WOLFSSL_MFL_2_10 = 2, /* 1024 bytes */
    WOLFSSL_MFL_2_11 = 3, /* 2048 bytes */
    WOLFSSL_MFL_2_12 = 4, /* 4096 bytes */
    WOLFSSL_MFL_2_13 = 5  /* 8192 bytes *//* CyaSSL ONLY!!! */
};

#ifndef NO_WOLFSSL_CLIENT

WOLFSSL_API int CyaSSL_UseMaxFragment(CYASSL* ssl, unsigned char mfl);
WOLFSSL_API int CyaSSL_CTX_UseMaxFragment(WOLFSSL_CTX* ctx, unsigned char mfl);

#endif /* NO_WOLFSSL_CLIENT */
#endif /* HAVE_MAX_FRAGMENT */

/* Truncated HMAC */
#ifdef HAVE_TRUNCATED_HMAC
#ifndef NO_WOLFSSL_CLIENT

WOLFSSL_API int CyaSSL_UseTruncatedHMAC(CYASSL* ssl);
WOLFSSL_API int CyaSSL_CTX_UseTruncatedHMAC(WOLFSSL_CTX* ctx);

#endif /* NO_WOLFSSL_CLIENT */
#endif /* HAVE_TRUNCATED_HMAC */

/* Elliptic Curves */
#ifdef HAVE_SUPPORTED_CURVES

enum {
    WOLFSSL_ECC_SECP160R1 = 0x10,
    WOLFSSL_ECC_SECP192R1 = 0x13,
    WOLFSSL_ECC_SECP224R1 = 0x15,
    WOLFSSL_ECC_SECP256R1 = 0x17,
    WOLFSSL_ECC_SECP384R1 = 0x18,
    WOLFSSL_ECC_SECP521R1 = 0x19
};

#ifndef NO_WOLFSSL_CLIENT

WOLFSSL_API int CyaSSL_UseSupportedCurve(CYASSL* ssl, unsigned short name);
WOLFSSL_API int CyaSSL_CTX_UseSupportedCurve(WOLFSSL_CTX* ctx,
                                                          unsigned short name);

#endif /* NO_WOLFSSL_CLIENT */
#endif /* HAVE_SUPPORTED_CURVES */


#define WOLFSSL_CRL_MONITOR   0x01   /* monitor this dir flag */
#define WOLFSSL_CRL_START_MON 0x02   /* start monitoring flag */

#ifdef WOLFSSL_CALLBACKS

/* used internally by CyaSSL while OpenSSL types aren't */
#include "crypto/src/cyassl/callbacks.h"

typedef int (*HandShakeCallBack)(HandShakeInfo*);
typedef int (*TimeoutCallBack)(TimeoutInfo*);

/* CyaSSL connect extension allowing HandShakeCallBack and/or TimeoutCallBack
   for diagnostics */
WOLFSSL_API int CyaSSL_connect_ex(CYASSL*, HandShakeCallBack, TimeoutCallBack,
                                 Timeval);
WOLFSSL_API int CyaSSL_accept_ex(CYASSL*, HandShakeCallBack, TimeoutCallBack,
                                Timeval);

#endif /* WOLFSSL_CALLBACKS */


#ifdef WOLFSSL_HAVE_WOLFSCEP
WOLFSSL_API void CyaSSL_wolfSCEP(void);
#endif /* WOLFSSL_HAVE_WOLFSCEP */


#ifdef __cplusplus
    }  /* extern "C" */
#endif


#endif /* WOLFSSL_SSL_H */

