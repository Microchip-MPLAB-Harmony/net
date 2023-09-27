/*******************************************************************************
  PPP Driver Library Public API

  Company:
    Microchip Technology Inc.
    
  FIle Name:
    drv_ppp.h

  Summary:
    This file contains the public API definitions for the PPP Driver library.

  Description:
    Public API definitions for the PPP Driver library.
    These definitions are separate from the 'standard' virtual MAC API.

*******************************************************************************/

//DOM-IGNORE-BEGIN
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

//DOM-IGNORE-END


#ifndef _DRV_PPP_H_
#define _DRV_PPP_H_

#include <stdint.h>
#include <stdbool.h>
#include "driver/driver_common.h"

// PPP Transition States
typedef enum
{
    PPP_STATE_INIT      = 0,    // Initial state, the lower layer is unavailable (Down), and no Open has occurred.
    PPP_STATE_STARTING,         // An administrative Open has been initiated, but the lower layer is still unavailable (Down)
    PPP_STATE_CLOSED,           // In the Closed state, the link is available (Up), but no Open has occurred.
    PPP_STATE_STOPPED,          // Stopped state is entered when the automaton is waiting for a Down event after
                                    // the This-Layer-Finished action, or after sending a Terminate-Ack.
    PPP_STATE_CLOSING,          // In the Closing state, an attempt is made to terminate the connection.  
    PPP_STATE_STOPPING,         // In the Stopping state A Terminate-Request has been sent and the Restart timer is
                                    // running, but a Terminate-Ack has not yet been received.
    PPP_STATE_REQ_SENT,         // In the Request-Sent state an attempt is made to configure the connection.
    PPP_STATE_ACK_RCVD,         // In the Ack-Received state, a Configure-Request has been sent and a Configure-Ack has been received.
    PPP_STATE_ACK_SENT,         // In the Ack-Sent state, a Configure-Request and a Configure-Ack have both been sent,
                                    // but a Configure-Ack has not yet been received.
    PPP_STATE_OPENED,           // In the Opened state, a Configure-Ack has been both sent and received. 
}PPP_STATE;

// PPP control protocol identifier
// Basic control protocols supported 
typedef enum
{
    PPP_CTRL_PROTO_NONE    = 0, // invalid/unknown
    // basic PPP level
    PPP_CTRL_PROTO_LCP,         // LCP control protocol
    PPP_CTRL_PROTO_IPCP,        // IPCP control protocol

}PPP_CTRL_PROTO;


// result of an PPP Echo Request call
typedef enum
{
    PPP_ECHO_OK                = 0,    // operation successful or a reply has been successfully received
    PPP_ECHO_TRUNCATED,                // warning: data length too big, had to be truncated
    // error codes, < 0
    PPP_ECHO_BAD_MAC_HANDLE    = -1,   // invalid MAC handle
    PPP_ECHO_NOT_READY         = -2,   // state error, module is not  ready (opened)
    PPP_ECHO_ALLOC_ERROR       = -3,   // could not allocate memory
    PPP_ECHO_TRANSMIT_ERROR    = -4,   // could not transmit (dead interface, etc.)
    PPP_ECHO_PARAMETER_ERROR   = -5,   // null pointer, etc.
    PPP_ECHO_BUSY              = -6,   // module currently busy
    PPP_ECHO_BAD_REQ_HANDLE    = -7,   // invalid request handle, no such request exist
    PPP_ECHO_TMO               = -8,   // no reply received; the current request timed out and will be deleted
}PPP_ECHO_RESULT;

// a handle that a client can use after the event handler has been registered
typedef const void* PPP_REQUEST_HANDLE;


// Forward definition for a request structure
struct _tag_PPPP_ECHO_REQUEST;

// *****************************************************************************
/* PPP Request Callback function

  Summary:
    Callback function notifying of an PPP request operation result

  Description:
    This callback function will be called when the PPP echo operation is completed


  Parameters:
    - pEchoReq  - pointer to a PPP_ECHO_REQUEST data structure describing the request:
                        - see the PPP_ECHO_REQUEST data structure description
    - pppHandle - the handle returned by PPP_EchoRequest
    - result    - the echo request result
                  when result == PPP_ECHO_REQUEST_RES_OK, the pEchoReq->pData points to the received data 
    - param     - user defined parameter to be called in the callback                 


  Remarks:
    None

*/
typedef void (*PPP_ECHO_CALLBACK)(const struct _tag_PPPP_ECHO_REQUEST* pEchoReq, PPP_REQUEST_HANDLE pppHandle, PPP_ECHO_RESULT result, const void* param);

// *****************************************************************************
/* PPP Request Data Structure

  Summary:
    Contains the data needed for an PPP request operation

  Description:
    This structure defines the information required for sending 
    an PPP request that contains user data.


  Remarks:
    When the callback is made with an error result, the PPP_ECHO_REQUEST
    structure may identify the request, if no reply was received (for example timeout)

    This is a PPP ECHO request
    Being a network interface a regular ICMP ping could be used instead

*/
typedef struct _tag_PPPP_ECHO_REQUEST
{
    uint8_t*            pData;      // input: data buffer to be sent as part of the request
                                    // callback : pointer to the received data buffer
    uint16_t            dataSize;   // input: number of bytes in the input data buffer
                                    // callback: number of bytes in the received data buffer
    uint16_t            seqNumber;  // monotonic number matching requests and replies
    PPP_ECHO_CALLBACK   callback;   // callback function to be called when a reply is received or an error occurred
    const void*         param;      // parameter to be used in the callback; it is user defined, PPP does not use it in any way
} PPP_ECHO_REQUEST;


// *****************************************************************************
/* Function:
    PPP_ECHO_RESULT PPP_EchoRequest (DRV_HANDLE hMac, PPP_ECHO_REQUEST* pEchoRequest, PPP_REQUEST_HANDLE* pHandle);

  Summary:
    Sends an PPP echo request containing user supplied data


  Description:
    This function allows a PPP client to send an PPP query message to a peer.
    The request will also contain user supplied data.
    The request is scheduled and the user will be notified of the outcome of the query
    using the callback function that's specified in the call.

  Precondition:
    The IP PPP module initialized and up and running.


  Parameters:
    - hMac          - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)
    - pEchoRequest  - pointer to a PPP_ECHO_REQUEST data structure describing the request:
                        - pData          - data buffer in the request
                        - dataSize       - number of bytes in the data buffer
                        - callback       - callback function to be called when a reply is received
    - pHandle       - address to store a handle to this PPP request.
                      It will contain a valid handle/pointer if the call succeeded,
                      0 otherwise.
                      It can be used to cancel the request (if the user timeout
                      is < than the PPP timeout: PPP_ECHO_REQUEST_TIMEOUT)
                      Could be NULL if not needed.    

  Returns:
    - PPP_ECHO_OK     - Indicates the query request was successfully sent
    - PPP_ECHO_RESULT - The query request was unsuccessfully sent, which results in an error code
                         (interface not ready for transmission, allocation error, etc.)

  Example:
  <code>
    uint8_t  myDataBuffer[200];     // buffer for the echo request data
    void EchoCallback(PPP_ECHO_REQUEST* pReqData, PPP_REQUEST_HANDLE pppHandle, PPP_ECHO_RESULT result);    // callback function to be called

    PPP_ECHO_REQUEST myEchoRequest;
    myEchoRequest.pData          = myDataBuffer;
    myEchoRequest.dataSize       = sizeof(myDataBuffer);
    myEchoRequest.callback       = EchoCallback;


    if(PPP_EchoRequest(hMac, &myEchoRequest, 0) == PPP_ECHO_OK )
    {
        // successfully sent the PPP request
        //
        // EchoCallback() will be called and data can be examined
    }
    else
    {
        // process the error
    }
  </code>

  Remarks:
    The data buffer that's passed as part of the callback routine is no longer available 
    after the callback routine returns control.

    The number of active echo requests at a time is controlled by PPP_MAX_ECHO_REQUESTS.
    If more requests are active, a PPP_ECHO_BUSY code will be returned. 


    Once the callback notification occurs, the echo request is completed
    and the pppHandle is invalidated.

*/
PPP_ECHO_RESULT PPP_EchoRequest (DRV_HANDLE hMac, PPP_ECHO_REQUEST* pEchoRequest, PPP_REQUEST_HANDLE* pHandle);

// *****************************************************************************
/* Function:
    PPP_ECHO_RESULT PPP_EchoRequestCancel (PPP_REQUEST_HANDLE pppHandle);

  Summary:
    Cancels a previously sent PPP echo request


  Description:
    This function allows a PPP client to cancel a pending PPP echo request.
    The request should have been previously scheduled with PPP_EchoRequest.

  Precondition:
    The PPP module must be initialized and up and running.
    Valid handle obtained using PPP_EchoRequest.


  Parameters:
    - pppHandle    - valid PPP handle

  Returns:
    - PPP_ECHO_OK          - Indicates the cancel request was successful

    - PPP_ECHO_BAD_REQ_HANDLE  - No such request is currently scheduled
                              Invalid handle 

  Example:
  <code>
  </code>

  Remarks:
    None
*/
PPP_ECHO_RESULT PPP_EchoRequestCancel (PPP_REQUEST_HANDLE pppHandle);

// *****************************************************************************
/*
  Enumeration:
    PPP_EVENT

  Summary:
    PPP event.

  Description:
    PPP event occurring during the driver operation

  Remarks:
    None

*/
typedef enum
{
    PPP_EVENT_NONE      = 0,    // no event/invalid
    PPP_EVENT_UP,               // lower layer is Up
    PPP_EVENT_DOWN,             // lower layer is down
    PPP_EVENT_OPEN,             // administrative Open
    PPP_EVENT_CLOSE,            // administrative Close
    PPP_EVENT_TMO_P,            // Timeout with counter > 0
    PPP_EVENT_TMO_N,            // Timeout with counter expired
    PPP_EVENT_RCR_P,            // Receive-Configure-Request (Good)
    PPP_EVENT_RCR_N_NAK,        // Receive-Configure-Request (Bad) that needs to be NAK-ed
    PPP_EVENT_RCR_N_REJ,        // Receive-Configure-Request (Bad) that needs to be rejected
    PPP_EVENT_RCA,              // Receive-Configure-Ack
    PPP_EVENT_RCN,              // Receive-Configure-Nak/Rej
    PPP_EVENT_RTR,              // Receive-Terminate-Request
    PPP_EVENT_RTA,              // Receive-Terminate-Ack 
    PPP_EVENT_RUC,              // Receive-Unknown-Code
    PPP_EVENT_RXJ_P,            // Receive-Code-Reject (permitted) or Receive-Protocol-Reject
    PPP_EVENT_RXJ_N,            // Receive-Code-Reject (catastrophic) or Receive-Protocol-Reject
    PPP_EVENT_RXR,              // Receive-Echo-Request or Receive-Echo-Reply or Receive-Discard-Request

    // 
    PPP_EVENT_RUP,              // Receive-Unknown-Protocol
    PPP_EVENT_IP,               // Receive IP Protocol frame
}PPP_EVENT;

// *****************************************************************************
/*
  Type:
    PPP_EVENT_HANDLE

  Summary:
    PPP event handle.

  Description:
    A handle that a client can use after the PPP event handler has been registered.
 */

typedef const void* PPP_EVENT_HANDLE;


// *****************************************************************************
/*
  Type:
    PPP_EVENT_HANDLER

  Summary:
    PPP event handler prototype.

  Description:
    Prototype of a PPP event handler. Clients can register a handler with the PPP driver.
    Once an PPP event occurs the PPP driver will call the registered handler.

  Parameters:
    hMac    - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)
    event   - the occurring event
    pppFrame - pointer to the frame that caused the event and it's logged
    param   - user supplied handle parameter
              Not used by the PPP module.

  Remarks:
    Some events (PPP_EVENT_UP, PPP_EVENT_TMO_P, etc.) will have pppFrame == 0

    The handler has to be short and fast. It is meant for
    setting an event flag, <i>not</i> for lengthy processing!

 */

typedef void    (*PPP_EVENT_HANDLER)(DRV_HANDLE hMac, PPP_EVENT event, const uint8_t* pppFrame, const void* param);



// *****************************************************************************
/* Function:
    PPP_EVENT_HANDLE PPP_EventHandlerRegister(DRV_HANDLE hMac, PPP_EVENT_HANDLER handler, 
                               const void* hParam)

  Summary:
    Registers a PPP Handler.

  Description:
    This function registers a PPP event handler.
    The PPP module will call the registered handler when a
    PPP event occurs.

  Precondition:
    The PPP module must be initialized.

  Parameters:
    hMac    - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)
    handler - Handler to be called when a PPP event occurs.
    hParam  - Parameter to be used in the handler call.
              This is user supplied and is not used by the PPP module.


  Returns:
    Returns a valid handle if the call succeeds, or a null handle if
    the call failed (out of memory, for example).

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

    The hParam is passed by the client and will be used by the PPP when the
    notification is made. It is used for per-thread content or if more modules,
    for example, share the same handler and need a way to differentiate the
    callback.

    This API exists only when the build symbol PPP_NOTIFICATIONS_ENABLE != 0 
 */

PPP_EVENT_HANDLE PPP_EventHandlerRegister(DRV_HANDLE hMac, PPP_EVENT_HANDLER handler, const void* hParam);


// *****************************************************************************
/* Function:
    bool PPP_EventHandlerDeregister(DRV_HANDLE hMac, PPP_EVENT_HANDLE hPPP)

  Summary:
    Deregisters a previously registered PPP handler.
    
  Description:
    This function deregisters the PPP event handler.

  Precondition:
    The PPP module must be initialized.

  Parameters:
    hMac    - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)
    hPPP   - A handle returned by a previous call to PPP_EventHandlerRegister.

  Returns:
    - true  - if the call succeeds
    - false - if no such handler is registered


  Remarks:
    This API exists only when the build symbol PPP_NOTIFICATIONS_ENABLE != 0 
 */

bool             PPP_EventHandlerDeregister(DRV_HANDLE hMac, PPP_EVENT_HANDLE hPPP);

// *****************************************************************************
/* PPP statistics Data Structure

  Summary:
    Contains the statistics data maintained by the PPP module

  Description:
    This structure defines the statistic counters maintained by the PPP module 


  Remarks:
    None

*/
typedef struct
{
    uint32_t    lcpPkts;            // number of LCP packets received
    uint32_t    ipcpPkts;           // number of IPCP packets received
    uint32_t    ipPkts;             // number of IP packets received
    uint32_t    tcpPkts;            // number of TCP packets received

    uint32_t    pppQueued;          // number of queued LCP/IPCP packets for processing
    uint32_t    netQueued;          // number of queued IP/TCP packets for processing
    uint32_t    echoReqFree;        // number of available requests to be served
    uint32_t    echoReqQueued;      // number of queued echo request packets for processing


    uint32_t    echoDiscardPkts;    // number of discard request packets received
    uint32_t    echoReqPkts;        // number of ECHO request packets received
    uint32_t    echoReplyPkts;      // number of ECHO reply packets received

    uint32_t    discardPkts;        // number of silently discarded packets
    uint32_t    protoErr;           // number of frames with protocol error
    uint32_t    lengthErr;          // number of frames with length error
    uint32_t    mruErr;             // number of frames with length > MRU error
    uint32_t    codeErr;            // number of frames with unknwon code (RUC) error
    uint32_t    formatErr;          // number wrong formatted frames
    uint32_t    rcaMatchErr;        // number of ACK frames not matching the REQ ones
    uint32_t    rcrIdentErr;        // Configure response frames not matching the REQ identifier
    uint32_t    rucErr;             // number of received RUC frames
    uint32_t    rxrErr;             // number of wrong RXR received frames
    uint32_t    rxjErr;             // number of wrong RXJ received frames
    uint32_t    rxjProtoErr;        // number of RXJ frames rejecting LCP, IPCP
    uint32_t    rxjCodeErr;         // number of RXJ frames rejecting LCP, IPCP
    uint32_t    crossedErr;         // number of received crossed RCA frames
    uint32_t    peerMagicErr;       // number of received RXR frames with wrong peer magic number
    uint32_t    loopbackErr;        // number of loopbacked RXR frames (our magic number instead of peer's)
    uint32_t    lcpCodeErr;         // number of frames with the wrong LCP code
    uint32_t    optionErr;          // number of frames with unknown/unsupported option


    uint32_t    hdlcWriteErr;       // HDLC write errors
    uint32_t    illegalEvents;      // illegal events count; debug only; should not happen
    uint32_t    buffFail;           // internal buffer failures; debug only; should not happen

}PPP_STATISTICS;

// *****************************************************************************
/* Function:
    bool PPP_StatisticsGet(DRV_HANDLE hMac, PPP_STATISTICS* pStat, bool clear);

  Summary:
    Returns a copy of the current PPP statistics
    
  Description:
    This function will copy the current value of the PPP statistics to the user supplied data structure

  Precondition:
    The PPP module must be initialized.

  Parameters:
    hMac    - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)
    pStat   - address where to store the statistics
    clear   - if true, the statistics data will be cleared

  Returns:
    - true  - if the call succeeds
    - false - if no such hMac exists


  Remarks:
    This API exists only when the build symbol PPP_ENABLE_STATISTICS != 0 
 */

bool             PPP_StatisticsGet(DRV_HANDLE hMac, PPP_STATISTICS* pStat, bool clear);

// *****************************************************************************
/*
  Type:
    PPP_STAT_HANDLE

  Summary:
    PPP statistics handle.

  Description:
    A handle that a client can use after the PPP statistics handler has been registered.
 */

typedef const void* PPP_STAT_HANDLE;


// *****************************************************************************
/*
  Type:
    PPP_STAT_HANDLER

  Summary:
    PPP statistics handler prototype.

  Description:
    Prototype of a PPP statistics handler. Clients can register a handler with the PPP driver.
    Once an PPP statistics event occurs the PPP driver will call the registered handler.

  Parameters:
    hMac    - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)
    offset  - offset in the PPP_STATISTICS structure indicating the exact member that's incremented
    pppFrame - pointer to the frame that caused the statistics increment
    newValue - Updated (incremented) value of the statistics counter

  Remarks:
    Some statistics increment event could have pppFrame == 0

    The handler has to be short and fast. It is meant for
    setting an event flag, <i>not</i> for lengthy processing!

 */

typedef void    (*PPP_STAT_HANDLER)(DRV_HANDLE hMac, int offset, const uint8_t* pppFrame, uint32_t newValue);



// *****************************************************************************
/* Function:
    PPP_STAT_HANDLE PPP_StatHandlerRegister(DRV_HANDLE hMac, PPP_STAT_HANDLER handler)

  Summary:
    Registers a PPP Handler.

  Description:
    This function registers a PPP statistics handler.
    The PPP module will call the registered handler when a
    PPP statistics increment occurs.

  Precondition:
    The PPP module must be initialized.

  Parameters:
    hMac    - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)
    handler - Handler to be called when a PPP statistics occurs.


  Returns:
    Returns a valid handle if the call succeeds, or a null handle if
    the call failed (out of memory, for example).

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

    This API exists only when the build symbol PPP_ENABLE_STATISTICS != 0 
 */

PPP_STAT_HANDLE PPP_StatHandlerRegister(DRV_HANDLE hMac, PPP_STAT_HANDLER handler);


// *****************************************************************************
/* Function:
    bool PPP_StatHandlerDeregister(DRV_HANDLE hMac, PPP_STAT_HANDLE hPPP)

  Summary:
    Deregisters a previously registered PPP handler.
    
  Description:
    This function deregisters the PPP statistics handler.

  Precondition:
    The PPP module must be initialized.

  Parameters:
    hMac    - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)
    hPPP   - A handle returned by a previous call to PPP_StatHandlerRegister.

  Returns:
    - true  - if the call succeeds
    - false - if no such handler is registered


  Remarks:
    This API exists only when the build symbol PPP_ENABLE_STATISTICS != 0 
 */

bool             PPP_StatHandlerDeregister(DRV_HANDLE hMac, PPP_STAT_HANDLE hPPP);

// *****************************************************************************
/* Function:
    bool PPP_SendAdminEvent(DRV_HANDLE hMac, PPP_EVENT event, PPP_CTRL_PROTO ctlProt);

  Summary:
    Sends an administrative event
    
  Description:
    This function sends an administrative event to the PPP driver

  Precondition:
    The PPP module must be initialized.

  Parameters:
    hMac    - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)
    event   - event to be sent
    ctlProt - the control protocol type: LCP or IPCP

  Returns:
    - true  - if the call succeeds
    - false - if the call failed (invalid handle)

  Remarks:
    In normal operation just administrative PPP_EVENT_OPEN/PPP_EVENT_CLOSE events should be used

    The event should be sent to LCP. 
    An LCP PPP_EVENT_OPEN will eventually open IPCP too.
    An LCP PPP_EVENT_CLOSE will close IPCP too.

    Sending events to IPCP is for testing purposes only
    and should not be normall used.
 */

bool            PPP_SendAdminEvent(DRV_HANDLE hMac, PPP_EVENT event, PPP_CTRL_PROTO ctlProt);


// *****************************************************************************
/* Function:
    uint32_t        PPP_GetLocalIpv4Addr(DRV_HANDLE hMac);

  Summary:
    Gets the PPP local IPv4 address
    
  Description:
    This function gets the PPP local IPv4 address currently assigned
    for the PPP driver

  Precondition:
    The PPP module must be initialized.

  Parameters:
    hMac    - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)

  Returns:
    current IPv4 local address
    Or 0 if an error

  Remarks:
    This should be the IPv4 address passed at the PPP initialization
    It could be a peer suggested address if the configuration address was 0
    or the peer suggested a different address.

 */

uint32_t        PPP_GetLocalIpv4Addr(DRV_HANDLE hMac);

// *****************************************************************************
/* Function:
    uint32_t        PPP_GetRemoteIpv4Addr(DRV_HANDLE hMac);

  Summary:
    Gets the PPP remote IPv4 address
    
  Description:
    This function gets the PPP remote IPv4 address currently assigned
    for the PPP driver

  Precondition:
    The PPP module must be initialized.

  Parameters:
    hMac    - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)

  Returns:
    current IPv4 remote address
    Or 0 if an error

  Remarks:
    This is the IPv4 address of the remote node
    It is set during PPP negotiation

    It could be a suggested address by this node (not supported yet)
 */

uint32_t        PPP_GetRemoteIpv4Addr(DRV_HANDLE hMac);

// *****************************************************************************
/* Function:
    bool        PPP_GetState(DRV_HANDLE hMac, PPP_STATE state[2]);

  Summary:
    Gets the PPP current state
    
  Description:
    This function gets the PPP current state for the
    LCP and IPCP

  Precondition:
    The PPP module must be initialized.

  Parameters:
    hMac    - driver handle (obtained by calling TCPIP_MAC_Open/DRV_PPP_MAC_Open)
    state   - address to receive the current LCP and IPCP states.
              Must be able to hold both instances.

  Returns:
    true    - if the call succeeded
    false   - if there was an error

  Remarks:
    None

 */

bool        PPP_GetState(DRV_HANDLE hMac, PPP_STATE state[2]);
//
// 
#endif  // _DRV_PPP_H_

