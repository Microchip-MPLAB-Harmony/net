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
// pChunkInfo points to an array of TCPIP_HTTP_NET_CHUNK_INFO, nInfos in size; could be 0 if not needed
// returns true if conenction ix found and info updated, false if failed
bool TCPIP_HTTP_NET_InfoGet(int connIx, TCPIP_HTTP_NET_CONN_INFO* pHttpInfo, TCPIP_HTTP_NET_CHUNK_INFO* pChunkInfo, int nInfos);


// return HTTP statistics
void TCPIP_HTTP_NET_StatGet(TCPIP_HTTP_NET_STAT_INFO* pStatInfo);


#endif // __HTTP_NET_MANAGER_H_

