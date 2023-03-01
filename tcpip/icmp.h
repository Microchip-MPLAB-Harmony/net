/*******************************************************************************
  ICMP Module Definitions for the Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    icmp.h

  Summary:
    The Internet Control Message Protocol is used to send error and status messages
    and requests.

  Description:
    The Internet Control Message Protocol is used to send error and status messages 
    and requests. The ICMP module implements the Echo Reply message type (commonly 
    referred to as a ping) which can be used to determine if a specified host is 
    reachable across an IP network from a device running the TCP/IP stack. An ICMP 
    server is also supported to respond to pings from other devices.
*******************************************************************************/
//DOM-IGNORE-BEGIN
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








//DOM-IGNORE-END

#ifndef __ICMP_H
#define __ICMP_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// result of an ICMP Echo Request call
typedef enum
{
    ICMP_ECHO_OK                = 0,    // operation successful
    // error codes, < 0
    ICMP_ECHO_ALLOC_ERROR       = -1,   // could not allocate memory
    ICMP_ECHO_ROUTE_ERROR       = -2,   // could not find a route to destination
    ICMP_ECHO_TRANSMIT_ERROR    = -3,   // could not transmit (dead interface, etc.)
    ICMP_ECHO_PARAMETER_ERROR   = -4,   // null pointer, etc.
    ICMP_ECHO_BUSY              = -5,   // module currently busy
    ICMP_ECHO_BAD_HANDLE        = -6,   // invalid handle, no such request exist
}ICMP_ECHO_RESULT;

// a handle that a client can use after the event handler has been registered
typedef const void* ICMP_HANDLE;
typedef const void* TCPIP_ICMP_REQUEST_HANDLE;

// A result code for an Echo Request callback (as a result of an ICMP TCPIP_ICMP_EchoRequest call)
typedef enum
{
    TCPIP_ICMP_ECHO_REQUEST_RES_OK    = 0,    // a reply has been successfully received
    // error codes, < 0
    TCPIP_ICMP_ECHO_REQUEST_RES_TMO   = -1,   // no reply received; the current request timed out and will be deleted
}TCPIP_ICMP_ECHO_REQUEST_RESULT;


// *****************************************************************************
/* ICMP Request Data Structure

  Summary:
    Contains the data needed for an ICMP request operation

  Description:
    This structure defines the information required for sending 
    an ICMP request that contains user data.


  Remarks:
    When the callback is made with an error result, the TCPIP_ICMP_ECHO_REQUEST
    structure may identify the request, if no reply was received (for example timeout)

*/
typedef struct _tag_TCPIP_ICMP_ECHO_REQUEST
{
    TCPIP_NET_HANDLE    netH;       // input: The handle of the network interface to use for the request.
                                    //        Can be 0 if a default interface is to be selected 
                                    // callback: the handle of the network interface on which the request was received
    IPV4_ADDR           targetAddr; // input: The IP address where to send the request
                                    // callback: the IP address of the host that replied
    uint16_t            sequenceNumber; // input: A sequence number to be used in the request
                                        //        Sequential echo requests with the same identifier
                                        //        should have different sequence numbers (counter usually).
                                        // callback: sequence number of the received reply
    uint16_t            identifier; // input: An identifier to be used in the request
                                    // callback: identifier of the received reply
    uint8_t*            pData;      // input: data buffer to be sent as part of the request
                                    // callback : pointer to the received data buffer
    uint16_t            dataSize;   // input: number of bytes in the input data buffer
                                    // callback: number of bytes in the received data buffer
    void (*callback)(const struct _tag_TCPIP_ICMP_ECHO_REQUEST* pReqData, TCPIP_ICMP_REQUEST_HANDLE icmpHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result, const void* param);
                                    // callback function to be called
                                    // when a reply/error is received
                                    // result indicates the reason for the call
                                    // when result == TCPIP_ICMP_ECHO_REQUEST_RES_OK, the pData points to the received data 
                                    // icmpHandle is the handle returned by TCPIP_ICMP_EchoRequest
    const void*         param;      // parameter to be called in the callback; it is user defined, ICMP does not use it in any way
} TCPIP_ICMP_ECHO_REQUEST;





// *****************************************************************************
/* ICMP Module Configuration Structure Typedef

  Summary:
    Placeholder for ICMP module configuration.

  Description:
    Provides a placeholder for ICMP module configuration.

  Remarks:
    None.
*/
typedef struct
{
    void*   reserved;
} TCPIP_ICMP_MODULE_CONFIG;

// *****************************************************************************
/* Function:
    ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequest (TCPIP_ICMP_ECHO_REQUEST* pEchoRequest, TCPIP_ICMP_REQUEST_HANDLE* pHandle);

  Summary:
    Sends an ICMP echo request containing user supplied data


  Description:
    This function allows a stack client to send an ICMP query message to a remote host.
    The supplied sequence number and identifier will be used in the query message.
    The request will also contain user supplied data.
    The request is scheduled and the user will be notified of the outcome of the query
    using the callback function that's specified in the call.

  Precondition:
    The TCP/IP Stack must be initialized and up and running.


  Parameters:
    - pEchoRequest  - pointer to a TCPIP_ICMP_ECHO_REQUEST data structure describing the request:
                        - netH           - The handle of the network interface to use for the request. Can be 0 if 
                                           a default interface is to be selected 
                        - targetAddr     - The IP address of the remote Host
                        - sequenceNumber - A sequence number to be used in the request
                        - identifier     - An identifier to be used in the request
                        - pData          - data buffer in the request
                        - dataSize       - number of bytes in the data buffer
                        - callback       - callback function to be called when a reply is received
    - pHandle       - address to store a handle to this ICMP request.
                      It will contain a valid handle/pointer if the call succeeded,
                      0 otherwise.
                      It can be used to cancel the request (if the user timeout
                      is < than the ICMP timeout: TCPIP_ICMP_ECHO_REQUEST_TIMEOUT)
                      Could be NULL if not needed.    

  Returns:
    - ICMP_ECHO_OK     - Indicates the query request was successfully sent
    - ICMP_ECHO_RESULT - The query request was unsuccessfully sent, which results in an error code
                         (interface not ready for transmission, allocation error, etc.)

  Example:
  <code>
    uint8_t  myDataBuffer[200];     // buffer for the ping data
    void EchoCallback(TCPIP_ICMP_ECHO_REQUEST* pReqData, TCPIP_ICMP_REQUEST_HANDLE icmpHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result);    // callback function to be called

    TCPIP_ICMP_ECHO_REQUEST myEchoRequest;
    myEchoRequest.netH  = 0;
    myEchoRequest.targetAddr  = 0xc0a00101;
    myEchoRequest.sequenceNumber = 1;
    myEchoRequest.identifier     = 0x1234;
    myEchoRequest.pData          = myDataBuffer;
    myEchoRequest.dataSize       = sizeof(myDataBuffer);
    myEchoRequest.callback       = EchoCallback;


    if(TCPIP_ICMP_EchoRequest(&myEchoRequest, 0) == ICMP_ECHO_OK )
    {
        // successfully sent the ICMP request
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

    The user has to enforce the fact that the "identifier" field has 
    to be unique per (destination address, source address) pair.

    The number of active ping/echo requests at a time is controlled by TCPIP_ICMP_MAX_ECHO_REQUESTS.
    If more requests are active, a ICMP_ECHO_BUSY code will be returned. 


    Once the callback notification occurs, the echo request is completed
    and the icmpHandle is invalidated.

*/
ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequest (TCPIP_ICMP_ECHO_REQUEST* pEchoRequest, TCPIP_ICMP_REQUEST_HANDLE* pHandle);

// *****************************************************************************
/* Function:
    ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequestCancel (TCPIP_ICMP_REQUEST_HANDLE icmpHandle);

  Summary:
    Cancels a previously sent ICMP echo request


  Description:
    This function allows a stack client to cancel a pending ICMP echo request.
    The request should have been previously scheduled with TCPIP_ICMP_EchoRequest.

  Precondition:
    The TCP/IP Stack must be initialized and up and running.
    Valid handle obtained using TCPIP_ICMP_EchoRequest.


  Parameters:
    - icmpHandle    - valid ICMP handle

  Returns:
    - ICMP_ECHO_OK          - Indicates the cancel request was successful

    - ICMP_ECHO_BAD_HANDLE  - No such request is currently scheduled
                              Invalid handle 

  Example:
  <code>
  </code>

  Remarks:
    None
*/
ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequestCancel (TCPIP_ICMP_REQUEST_HANDLE icmpHandle);


// *****************************************************************************
/*
  Function:
    void  TCPIP_ICMP_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs ICMPv4 module tasks in the TCP/IP stack.

  Precondition:
    The ICMP module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_ICMP_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __ICMP_H
