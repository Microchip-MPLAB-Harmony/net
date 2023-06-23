/*******************************************************************************
  HTTP internal API Headers for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    http_net_manager.h

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

#ifndef __HTTP_NET_MANAGER_H_
#define __HTTP_NET_MANAGER_H_


/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/

/*****************************************************************************
  Function:
    bool TCPIP_HTTP_NET_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl,
        const TCPIP_HTTP_NET_MODULE_CONFIG* httpInitData)

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
bool TCPIP_HTTP_NET_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const TCPIP_HTTP_NET_MODULE_CONFIG* httpData);


/*****************************************************************************
  Function:
    bool TCPIP_HTTP_NET_Deinitialize(void)

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
    This function is called only once during lifetime of the application.
  ***************************************************************************/
void TCPIP_HTTP_NET_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);


// debug and run time info
//

// information for a HTTP connection associated chunk
typedef struct
{
    uint16_t        flags;      // chunk flags:TCPIP_HTTP_CHUNK_FLAGS
    uint16_t        status;     // chunk status: TCPIP_HTTP_CHUNK_STATE 
    char            chunkFName[20]; // chunk file name
    char            dynVarName[20]; // dynamic variable name, if the chunk is a dyn var chunk
    uint16_t        nDynBuffers;    // waiting dyn buffers
    uint16_t        padding;        // future use
}TCPIP_HTTP_NET_CHUNK_INFO;

// information for a HTTP connection
typedef struct
{
    uint16_t    httpStatus;     // TCPIP_HTTP_NET_STATUS value 
    uint16_t    connStatus;     // TCPIP_HTTP_NET_CONN_STATE value
    uint16_t    nChunks;        // currently active chunks
    uint16_t    chunkPoolEmpty; // counter for the dynamic chunk pool empty condition
    uint16_t    fileBufferPoolEmpty; // counter for file buffer empty condition
    uint16_t    listenPort;
}TCPIP_HTTP_NET_CONN_INFO;

// HTTP statistics info
typedef struct
{
    int         nConns;             // number of HTTP connections
    int         nOpenConns;         // number of opened HTTP connections
    int         nActiveConns;       // number of active HTTP connections
    uint32_t    dynPoolEmpty;       // dynamic variables buffer pool empty condition counter
    uint32_t    maxRecurseDepth;    // maximum chunk depth counter
    uint32_t    dynParseRetry;      // dynamic variables parsing retries because the parsed line
                                    // didn't fit in the socket buffer
}TCPIP_HTTP_NET_STAT_INFO;



// return info about a specific connection
// returns true if conenction ix found and info updated, false if failed
bool TCPIP_HTTP_NET_InfoGet(int connIx, TCPIP_HTTP_NET_CONN_INFO* pHttpInfo);


// return HTTP statistics
bool TCPIP_HTTP_NET_StatGet(TCPIP_HTTP_NET_STAT_INFO* pStatInfo);

// return advanced chunk info about a specific connection
// pChunkInfo points to an array of TCPIP_HTTP_NET_CHUNK_INFO, nInfos in size;
// returns true if conenction ix found and info updated, false if failed
// debug purpose only
bool TCPIP_HTTP_NET_ChunkInfoGet(int connIx, TCPIP_HTTP_NET_CHUNK_INFO* pChunkInfo, int nInfos);


#endif // __HTTP_NET_MANAGER_H_

