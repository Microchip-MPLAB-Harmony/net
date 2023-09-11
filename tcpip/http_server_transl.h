/*******************************************************************************
  HTTP internal API Headers for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    http_server_transl.h

  Summary:

  Description:
    Provides backward compatibility between the http_server v2 and the HTTP_NET definitions
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END

#ifndef __HTTP_SERVER_TRANSL_H_
#define __HTTP_SERVER_TRANSL_H_

// translation definitions

#define TCPIP_HTTP_NET_IO_RESULT        TCPIP_HTTP_IO_RESULT
#define TCPIP_HTTP_NET_IO_RES_DONE      TCPIP_HTTP_IO_RES_DONE
#define TCPIP_HTTP_NET_IO_RES_NEED_DATA TCPIP_HTTP_IO_RES_NEED_DATA
#define TCPIP_HTTP_NET_IO_RES_WAITING   TCPIP_HTTP_IO_RES_WAITING
#define TCPIP_HTTP_NET_IO_RES_ERROR     TCPIP_HTTP_IO_RES_ERROR


#define TCPIP_HTTP_NET_STATUS                       TCPIP_HTTP_STATUS 
#define TCPIP_HTTP_NET_STAT_GET                     TCPIP_HTTP_STAT_GET
#define TCPIP_HTTP_NET_STAT_POST                    TCPIP_HTTP_STAT_POST
#define TCPIP_HTTP_NET_STAT_BAD_REQUEST             TCPIP_HTTP_STAT_BAD_REQUEST
#define TCPIP_HTTP_NET_STAT_UNAUTHORIZED            TCPIP_HTTP_STAT_UNAUTHORIZED
#define TCPIP_HTTP_NET_STAT_NOT_FOUND               TCPIP_HTTP_STAT_NOT_FOUND
#define TCPIP_HTTP_NET_STAT_OVERFLOW                TCPIP_HTTP_STAT_OVERFLOW
#define TCPIP_HTTP_NET_STAT_INTERNAL_SERVER_ERROR   TCPIP_HTTP_STAT_INTERNAL_SERVER_ERROR
#define TCPIP_HTTP_NET_STAT_NOT_IMPLEMENTED         TCPIP_HTTP_STAT_NOT_IMPLEMENTED
#define TCPIP_HTTP_NET_STAT_REDIRECT                TCPIP_HTTP_STAT_REDIRECT
#define TCPIP_HTTP_NET_STAT_TLS_REQUIRED            TCPIP_HTTP_STAT_TLS_REQUIRED
#define TCPIP_HTTP_NET_STAT_UPLOAD_FORM             TCPIP_HTTP_STAT_UPLOAD_FORM
#define TCPIP_HTTP_NET_STAT_UPLOAD_STARTED          TCPIP_HTTP_STAT_UPLOAD_STARTED
#define TCPIP_HTTP_NET_STAT_UPLOAD_WRITE            TCPIP_HTTP_STAT_UPLOAD_WRITE
#define TCPIP_HTTP_NET_STAT_UPLOAD_WRITE_WAIT       TCPIP_HTTP_STAT_UPLOAD_WRITE_WAIT
#define TCPIP_HTTP_NET_STAT_UPLOAD_OK               TCPIP_HTTP_STAT_UPLOAD_OK
#define TCPIP_HTTP_NET_STAT_UPLOAD_ERROR            TCPIP_HTTP_STAT_UPLOAD_ERROR


#define TCPIP_HTTP_NET_READ_STATUS      TCPIP_HTTP_READ_STATUS
#define TCPIP_HTTP_NET_READ_OK          TCPIP_HTTP_READ_OK
#define TCPIP_HTTP_NET_READ_TRUNCATED   TCPIP_HTTP_READ_TRUNCATED
#define TCPIP_HTTP_NET_READ_INCOMPLETE  TCPIP_HTTP_READ_INCOMPLETE

#define TCPIP_HTTP_NET_EVENT_TYPE                           TCPIP_HTTP_EVENT_TYPE
#define TCPIP_HTTP_NET_EVENT_NONE                           TCPIP_HTTP_EVENT_NONE                         
#define TCPIP_HTTP_NET_EVENT_FS_UPLOAD_COMPLETE             TCPIP_HTTP_EVENT_FS_UPLOAD_COMPLETE 
#define TCPIP_HTTP_NET_EVENT_OPEN                           TCPIP_HTTP_EVENT_OPEN     
#define TCPIP_HTTP_NET_EVENT_CLOSE_DONE                     TCPIP_HTTP_EVENT_CLOSE_DONE     
#define TCPIP_HTTP_NET_EVENT_CLOSE_TIMEOUT                  TCPIP_HTTP_EVENT_CLOSE_TIMEOUT     
#define TCPIP_HTTP_NET_EVENT_CLOSE_POST_ERROR               TCPIP_HTTP_EVENT_CLOSE_POST_ERROR     
#define TCPIP_HTTP_NET_EVENT_CLOSE_REMOTE                   TCPIP_HTTP_EVENT_CLOSE_REMOTE     
#define TCPIP_HTTP_NET_EVENT_FILE_OPEN                      TCPIP_HTTP_EVENT_FILE_OPEN 
#define TCPIP_HTTP_NET_EVENT_FILE_CLOSE                     TCPIP_HTTP_EVENT_FILE_CLOSE 
#define TCPIP_HTTP_NET_EVENT_FILE_OPEN_ERROR                TCPIP_HTTP_EVENT_FILE_OPEN_ERROR              
#define TCPIP_HTTP_NET_EVENT_FILE_NAME_ERROR                TCPIP_HTTP_EVENT_FILE_NAME_ERROR              
#define TCPIP_HTTP_NET_EVENT_FILE_NAME_SIZE_ERROR           TCPIP_HTTP_EVENT_FILE_NAME_SIZE_ERROR         
#define TCPIP_HTTP_NET_EVENT_FILE_SIZE_ERROR                TCPIP_HTTP_EVENT_FILE_SIZE_ERROR              
#define TCPIP_HTTP_NET_EVENT_FILE_READ_ERROR                TCPIP_HTTP_EVENT_FILE_READ_ERROR              
#define TCPIP_HTTP_NET_EVENT_FILE_PARSE_ERROR               TCPIP_HTTP_EVENT_FILE_PARSE_ERROR             
#define TCPIP_HTTP_NET_EVENT_DEPTH_ERROR                    TCPIP_HTTP_EVENT_DEPTH_ERROR                  
#define TCPIP_HTTP_NET_EVENT_DYNVAR_PARSE_ERROR             TCPIP_HTTP_EVENT_DYNVAR_PARSE_ERROR           
#define TCPIP_HTTP_NET_EVENT_FS_WRITE_ERROR                 TCPIP_HTTP_EVENT_FS_WRITE_ERROR               
#define TCPIP_HTTP_NET_EVENT_FS_MOUNT_ERROR                 TCPIP_HTTP_EVENT_FS_MOUNT_ERROR               
#define TCPIP_HTTP_NET_EVENT_CHUNK_POOL_ERROR               TCPIP_HTTP_EVENT_CHUNK_POOL_ERROR             
#define TCPIP_HTTP_NET_EVENT_FILE_BUFFER_POOL_ERROR         TCPIP_HTTP_EVENT_FILE_BUFFER_POOL_ERROR       
#define TCPIP_HTTP_NET_EVENT_DYNVAR_ALLOC_ERROR             TCPIP_HTTP_EVENT_DYNVAR_ALLOC_ERROR           
#define TCPIP_HTTP_NET_EVENT_UPLOAD_ALLOC_ERROR             TCPIP_HTTP_EVENT_UPLOAD_ALLOC_ERROR           
#define TCPIP_HTTP_NET_EVENT_SSI_PARSE_ERROR                TCPIP_HTTP_EVENT_SSI_PARSE_ERROR              
#define TCPIP_HTTP_NET_EVENT_SSI_COMMAND_ERROR              TCPIP_HTTP_EVENT_SSI_COMMAND_ERROR            
#define TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_ERROR               TCPIP_HTTP_EVENT_SSI_ATTRIB_ERROR             
#define TCPIP_HTTP_NET_EVENT_SSI_ALLOC_DESCRIPTOR_ERROR     TCPIP_HTTP_EVENT_SSI_ALLOC_DESCRIPTOR_ERROR   
#define TCPIP_HTTP_NET_EVENT_PEEK_ALLOC_BUFFER_ERROR        TCPIP_HTTP_EVENT_PEEK_ALLOC_BUFFER_ERROR      
#define TCPIP_HTTP_NET_EVENT_SSI_ALLOC_ECHO_ERROR           TCPIP_HTTP_EVENT_SSI_ALLOC_ECHO_ERROR         
#define TCPIP_HTTP_NET_EVENT_DYNVAR_ARG_NAME_TRUNCATED      TCPIP_HTTP_EVENT_DYNVAR_ARG_NAME_TRUNCATED    
#define TCPIP_HTTP_NET_EVENT_DYNVAR_ARG_NUMBER_TRUNCATED    TCPIP_HTTP_EVENT_DYNVAR_ARG_NUMBER_TRUNCATED  
#define TCPIP_HTTP_NET_EVENT_DYNVAR_RETRIES_EXCEEDED        TCPIP_HTTP_EVENT_DYNVAR_RETRIES_EXCEEDED      
#define TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_NUMBER_TRUNCATED    TCPIP_HTTP_EVENT_SSI_ATTRIB_NUMBER_TRUNCATED  
#define TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_UNKNOWN             TCPIP_HTTP_EVENT_SSI_ATTRIB_UNKNOWN           
#define TCPIP_HTTP_NET_EVENT_SSI_ATTRIB_NUMBER_MISMATCH    =TCPIP_HTTP_EVENT_SSI_ATTRIB_NUMBER_MISMATCH   
#define TCPIP_HTTP_NET_EVENT_SSI_VAR_NUMBER_EXCEEDED       =TCPIP_HTTP_EVENT_SSI_VAR_NUMBER_EXCEEDED      
#define TCPIP_HTTP_NET_EVENT_SSI_VAR_UNKNOWN               =TCPIP_HTTP_EVENT_SSI_VAR_UNKNOWN              
#define TCPIP_HTTP_NET_EVENT_SSI_VAR_VOID                  =TCPIP_HTTP_EVENT_SSI_VAR_VOID                 
#define TCPIP_HTTP_NET_EVENT_SSI_HASH_CREATE_FAILED       = TCPIP_HTTP_EVENT_SSI_HASH_CREATE_FAILED       
#define TCPIP_HTTP_NET_EVENT_SSI_VAR_DELETED               =TCPIP_HTTP_EVENT_SSI_VAR_DELETED              
#define TCPIP_HTTP_NET_EVENT_CHUNK_POOL_EMPTY               TCPIP_HTTP_EVENT_CHUNK_POOL_EMPTY             
#define TCPIP_HTTP_NET_EVENT_FILE_BUFFER_POOL_EMPTY         TCPIP_HTTP_EVENT_FILE_BUFFER_POOL_EMPTY       
#define TCPIP_HTTP_NET_EVENT_PEEK_BUFFER_SIZE_EXCEEDED      TCPIP_HTTP_EVENT_PEEK_BUFFER_SIZE_EXCEEDED    

#define TCPIP_HTTP_NET_CONN_HANDLE  TCPIP_HTTP_CONN_HANDLE 


#define TCPIP_HTTP_NET_MODULE_FLAGS                     TCPIP_HTTP_MODULE_FLAGS                     
#define TCPIP_HTTP_NET_MODULE_FLAG_DEFAULT              TCPIP_HTTP_MODULE_FLAG_DEFAULT              
#define TCPIP_HTTP_NET_MODULE_FLAG_NON_PERSISTENT       TCPIP_HTTP_MODULE_FLAG_NON_PERSISTENT       
#define TCPIP_HTTP_NET_MODULE_FLAG_NO_DELAY             TCPIP_HTTP_MODULE_FLAG_NO_DELAY             
#define TCPIP_HTTP_NET_MODULE_FLAG_SECURE_ON            TCPIP_HTTP_MODULE_FLAG_SECURE_ON            
#define TCPIP_HTTP_NET_MODULE_FLAG_SECURE_OFF           TCPIP_HTTP_MODULE_FLAG_SECURE_OFF           
#define TCPIP_HTTP_NET_MODULE_FLAG_SECURE_DEFAULT       TCPIP_HTTP_MODULE_FLAG_SECURE_DEFAULT       



#define TCPIP_HTTP_NET_URLDecode(cData) TCPIP_HTTP_URLDecode(cData)

#define TCPIP_HTTP_NET_ArgGet(cData, cArg) TCPIP_HTTP_ArgGet(cData, cArg)


#define TCPIP_HTTP_NET_ConnectionDynamicDescriptors() (int)TCPIP_HTTP_ConnectionDynamicDescriptors(0)

#define TCPIP_HTTP_NET_ConnectionPostNameRead(connHandle, cData, wLen) TCPIP_HTTP_ConnectionPostNameRead(connHandle, cData, wLen)

#define TCPIP_HTTP_NET_ConnectionPostValueRead(connHandle, cData, wLen) TCPIP_HTTP_ConnectionPostValueRead(connHandle, cData, wLen)

#define TCPIP_HTTP_NET_ConnectionFileGet(connHandle)    TCPIP_HTTP_ConnectionFileGet(connHandle)

#define TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle) TCPIP_HTTP_ConnectionPostSmGet(connHandle) 

#define TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, stat) TCPIP_HTTP_ConnectionPostSmSet(connHandle, stat)

#define TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle) TCPIP_HTTP_ConnectionDataBufferGet(connHandle)

#define TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(connHandle) TCPIP_HTTP_ConnectionDataBufferSizeGet(connHandle)

#define TCPIP_HTTP_NET_ConnectionCallbackPosGet(connHandle) TCPIP_HTTP_ConnectionCallbackPosGet(connHandle)

#define TCPIP_HTTP_NET_ConnectionCallbackPosSet(connHandle, pos) TCPIP_HTTP_ConnectionCallbackPosSet(connHandle, pos)

#define TCPIP_HTTP_NET_ConnectionStatusGet(connHandle) TCPIP_HTTP_ConnectionStatusGet(connHandle)

#define TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, stat) TCPIP_HTTP_ConnectionStatusSet(connHandle, stat)

#define TCPIP_HTTP_NET_ConnectionHasArgsGet(connHandle) TCPIP_HTTP_ConnectionHasArgsGet(connHandle)

#define TCPIP_HTTP_NET_ConnectionHasArgsSet(connHandle, args) TCPIP_HTTP_ConnectionHasArgsSet(connHandle, args);

#define TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle) TCPIP_HTTP_ConnectionByteCountGet(connHandle)

#define TCPIP_HTTP_NET_ConnectionByteCountSet(connHandle, byteCount) TCPIP_HTTP_ConnectionByteCountSet(connHandle, byteCount)

#define TCPIP_HTTP_NET_ConnectionByteCountDec(connHandle, byteCount) TCPIP_HTTP_ConnectionByteCountDec(connHandle, byteCount)

#define TCPIP_HTTP_NET_ConnectionIsAuthorizedGet(connHandle) TCPIP_HTTP_ConnectionIsAuthorizedGet(connHandle)

#define TCPIP_HTTP_NET_ConnectionIsAuthorizedSet(connHandle, auth) TCPIP_HTTP_ConnectionIsAuthorizedSet(connHandle, auth)

#define TCPIP_HTTP_NET_ConnectionUserDataSet(connHandle, uData) TCPIP_HTTP_ConnectionUserDataSet(connHandle, uData)

#define TCPIP_HTTP_NET_ConnectionUserDataGet(connHandle) TCPIP_HTTP_ConnectionUserDataGet(connHandle)

#define TCPIP_HTTP_NET_ConnectionIndexGet(connHandle) (int)TCPIP_HTTP_ConnectionIndexGet(connHandle)

#define TCPIP_HTTP_NET_ConnectionHandleGet(connIx)  TCPIP_HTTP_ConnectionHandleGet(0, 0, connIx)

#define TCPIP_HTTP_NET_ActiveConnectionCountGet(pOpenCount) (int)TCPIP_HTTP_ActiveConnectionCountGet(0, 0, (size_t*)pOpenCount)

#define TCPIP_HTTP_NET_USER_CALLBACK    TCPIP_HTTP_USER_CALLBACK
#define _tag_TCPIP_HTTP_NET_USER_CALLBACK    _tag_TCPIP_HTTP_USER_CALLBACK

#define TCPIP_HTTP_NET_USER_HANDLE    TCPIP_HTTP_USER_HANDLE

#define TCPIP_HTTP_NET_UserHandlerRegister(cback) TCPIP_HTTP_UserHandlerRegister(0, cback)

#define TCPIP_HTTP_NET_UserHandlerDeregister(hHttp) TCPIP_HTTP_UserHandlerDeregister(0, hHttp)

#define TCPIP_HTTP_NET_DynamicFileInclude(connHandle, varDcpt, fileName) TCPIP_HTTP_DynamicFileInclude(connHandle, varDcpt, fileName)

#define TCPIP_HTTP_NET_DynamicWrite(vDcpt, buffer, size, needAck) TCPIP_HTTP_DynamicWrite(vDcpt, buffer, size, needAck)

#define TCPIP_HTTP_NET_DynamicWriteString(varDcpt, str, needAck) TCPIP_HTTP_DynamicWriteString(varDcpt, str, needAck)

#define TCPIP_HTTP_NET_ConnectionFlush(connHandle) TCPIP_HTTP_ConnectionFlush(connHandle)

#define TCPIP_HTTP_NET_ConnectionReadIsReady(connHandle) TCPIP_HTTP_ConnectionReadIsReady(connHandle)

#define TCPIP_HTTP_NET_ConnectionRead(connHandle, buffer, size) TCPIP_HTTP_ConnectionRead(connHandle, buffer, size)

#define TCPIP_HTTP_NET_ConnectionPeek(connHandle,  buffer, size) TCPIP_HTTP_ConnectionPeek(connHandle,  buffer, size)

#define TCPIP_HTTP_NET_ConnectionDiscard(connHandle) TCPIP_HTTP_ConnectionDiscard(connHandle)

#define TCPIP_HTTP_NET_ConnectionReadBufferSize(connHandle) TCPIP_HTTP_ConnectionReadBufferSize(connHandle)

#define TCPIP_HTTP_NET_ConnectionNetHandle(connHandle) TCPIP_HTTP_ConnectionNetHandle(connHandle)

#define TCPIP_HTTP_NET_ConnectionSocketGet(connHandle)  TCPIP_HTTP_ConnectionSocketGet(connHandle)

#define TCPIP_HTTP_NET_ConnectionStringFind(connHandle, str, startOffs, searchLen) TCPIP_HTTP_ConnectionStringFind(connHandle, str, startOffs, searchLen)

#define TCPIP_HTTP_NET_SSIVariableDelete(varName) TCPIP_HTTP_SSIVariableDelete(0, varName)

#define TCPIP_HTTP_NET_SSIVariablesNumberGet(pMaxNo) (int)TCPIP_HTTP_SSIVariablesNumberGet(0, (size_t*)pMaxNo)







// from configuration.h
//
#define TCPIP_HTTP_NET_USE_AUTHENTICATION       TCPIP_HTTP_USE_AUTHENTICATION
#define TCPIP_HTTP_NET_USE_POST                 TCPIP_HTTP_USE_POST
#define TCPIP_HTTP_NET_SSI_PROCESS              TCPIP_HTTP_SSI_PROCESS



// from http_net_manager.h/http_server.h
#define TCPIP_HTTP_NET_CONN_INFO    TCPIP_HTTP_CONN_INFO
#define TCPIP_HTTP_NET_CHUNK_INFO   TCPIP_HTTP_CHUNK_INFO
#define TCPIP_HTTP_NET_STAT_INFO    TCPIP_HTTP_STATISTICS

#define TCPIP_HTTP_NET_StatGet(pStatInfo) TCPIP_HTTP_StatsticsGet(0, 0, pStatInfo)

#define TCPIP_HTTP_NET_InfoGet(connIx, pHttpInfo) TCPIP_HTTP_InfoGet(0, 0, connIx, pHttpInfo)

#define TCPIP_HTTP_NET_ChunkInfoGet(connIx, pChunkInfo, nInfos) TCPIP_HTTP_ChunkInfoGet(0, 0, connIx, pChunkInfo, nInfos)



// special SSI functions that changed signature
// these need to be true functions
const char*   TCPIP_HTTP_NET_SSIVariableGet(const char* varName, TCPIP_HTTP_DYN_ARG_TYPE* pVarType, int32_t* pVarInt);

bool TCPIP_HTTP_NET_SSIVariableSet(const char* varName, TCPIP_HTTP_DYN_ARG_TYPE varType, const char* strValue, int32_t intValue);

const char*   TCPIP_HTTP_NET_SSIVariableGetByIndex(int varIndex, const char** pVarName, TCPIP_HTTP_DYN_ARG_TYPE* pVarType, int32_t* pVarInt);


#endif // __HTTP_SERVER_TRANSL_H_

