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
/*******************************************************************************
Copyright 2012-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
                                    // callback: th eIP address of the host that replied
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
    void (*callback)(const struct _tag_TCPIP_ICMP_ECHO_REQUEST* pReqData, TCPIP_ICMP_REQUEST_HANDLE icmpHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result);
                                    // callback function to be called
                                    // when a reply/error is received
                                    // result indicates the reason for the call
                                    // icmpHandle is the handle returned by TCPIP_ICMP_EchoRequest
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

    This routine is more generic than TCPIP_ICMP_EchoRequestSend and is preferred. 

    The user has to enforce the fact that the "identifier" field has 
    to be unique per (destination address, source address) pair.

    Currently there could be only one ping/echo request active at a time.
    If another echo request is active, a ICMP_ECHO_BUSY code will be returned. 


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
/* Function:
    ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequestSend (TCPIP_NET_HANDLE netH, IPV4_ADDR * targetAddr, 
	                                             uint16_t sequenceNumber, uint16_t identifier)

  Summary:
    Sends an ICMP echo request to a remote node.

  Description:
    This function allows a stack client to send an ICMP query message to a remote host.
    The supplied sequence number and identifier will be used in the query message.
    The user will be notified by the result of the query using a notification handle
    registered by using the TCPIP_ICMP_CallbackRegister function.

  Precondition:
    The TCP/IP Stack must be initialized and up and running.


  Parameters:
    - netH           - The handle of the network interface to use for the request. Can be 0 if 
	                   a default interface is to be selected 
    - targetAddr     - The IP address of the remote Host
    - sequenceNumber - A sequence number to be used in the request
    - identifier     - An identifier to be used in the request

  Returns:
    - ICMP_ECHO_OK     - Indicates the query request was successfully sent
    - ICMP_ECHO_RESULT - The query request was unsuccessfully sent, which results in an error code
                         (interface not ready for transmission, allocation error, etc.)

  Example:
  <code>
    IPV4_ADDR remoteAddress = 0xc0a00101;
    uint16_t mySequenceNumber = 1;
    uint16_t myId = 0x1234;

    if(TCPIP_ICMP_EchoRequestSend(0, &remoteAddress, mySequenceNumber, myId) == ICMP_ECHO_OK )
    {
        // successfully sent the ICMP request
        //
    }
    else
    {
        // process the error
    }
  </code>

  Remarks:
    This function does not allow the user to specify a data buffer to be sent as part of the request.
    The preferred function to use is TCPIP_ICMP_EchoRequest.

    This function will eventually be obsoleted.

*/
ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequestSend (TCPIP_NET_HANDLE netH, IPV4_ADDR * targetAddr, 
                                             uint16_t sequenceNumber, uint16_t identifier);


// *****************************************************************************
/* Function:
    ICMP_HANDLE TCPIP_ICMP_CallbackRegister (void (*callback)(TCPIP_NET_HANDLE hNetIf, 
	                                         IPV4_ADDR * remoteIP, void * data))

  Summary:
    Registers a callback to allow the application layer to process incoming ICMPv4 packets

  Description:
    Allows a stack client to be notified of the receiving of a response from an ICMP query.
    Once an Echo request reply is received, the notification handler callback will be called,
    letting the client know of the result of the query.
    The callback will contain as parameters:
        - the network interface handle on which the query reply was received
        - the remote host IP address
        - a 32-bit value containing the sequence number in the low 16-bit part
          and the identifier value in the high 16-bit part.

  Precondition:
    The TCP/IP Stack must be initialized and up and running.

  Parameters:
    callback    - notification function to be registered.
                  This function will be called when an echo request reply is received.

  Returns:
    - A non-null handle if the registration succeeded
    - 0 if the registration operation failed (for example, out of memory)

  Example:
  <code>
    // Callback function prototype
    void MyICMPCallbackFunction(TCPIP_NET_HANDLE hNetIf, IPV4_ADDR * remoteIP, void * data);

    // *****************************************************************************
    // Register callback function
    ICMP_HANDLE hIcmp = TCPIP_ICMP_CallbackRegister(&MyICMPCallbackFunction);
    if(hIcmp == 0)
    {
        // process error; couldn't register a handler
    }

    // success; I can send an Echo request and receive notification


    // *****************************************************************************
    IPV4_ADDR remoteIP = 0xc0a00101;
    uint16_t mySequenceNumber = 1;
    uint16_t myId = 0x1234;
    // send an ICMP query request
    TCPIP_ICMP_EchoRequestSend(&remoteIP, mySequenceNumber, myId);


    // *****************************************************************************
    // process the ICMP reply in the callback function
    void MyICMPCallbackFunction(TCPIP_NET_HANDLE hNetIf, IPV4_ADDR * remoteIP, void * data)
    {
        // process request from interface hNetIf and remoteIP address
        uint16_t* pReply = (uint16_t*)data;
        uint16_t myRecvId = *pReply;
        uint16_t myRecvSequenceNumber = *(pReply + 1);

        // check that the sequence number, ID and IP address match, etc.
    }
  </code>

  Remarks:
    None.
*/
ICMP_HANDLE TCPIP_ICMP_CallbackRegister (void (*callback)(TCPIP_NET_HANDLE hNetIf, 
                                         IPV4_ADDR * remoteIP, void * data));


// *****************************************************************************
/* Function:
    bool  TCPIP_ICMP_CallbackDeregister(ICMP_HANDLE hIcmp)

  Summary:
    Deregisters the ICMP callback function.

  Description:
    This function notifies a stack client to remove its former registered notification handler.
    After this operation the client will no longer be notified about the receiving
    of replies to the ICMP requests.

  Precondition:
    The TCP/IP Stack must be initialized and up and running.
    A previous successful call to TCPIP_ICMP_CallbackRegister has been done.

  Parameters:
    hIcmp   - an ICMP handle obtained by TCPIP_ICMP_CallbackRegister
	
  Returns:
    - true  - if the notification handler has been successfully removed
    - false - if such a notification handler could not be found

  Example:
  <code>
    void MyICMPCallbackFunction(TCPIP_NET_HANDLE hNetIf, IPV4_ADDR * remoteIP, void * data);
    ICMP_HANDLE hIcmp = TCPIP_ICMP_CallbackRegister(&MyICMPCallbackFunction);
    if(hIcmp != 0)
    {
        // successfully registered my handler
        // send requests and process the incoming results
        // ...
        // later on, once we're done, remove the notification handler
        TCPIP_ICMP_CallbackDeregister(hIcmp);
    }
  </code>

  Remarks:
    None.
*/
bool  TCPIP_ICMP_CallbackDeregister(ICMP_HANDLE hIcmp);


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
