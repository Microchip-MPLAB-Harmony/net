/*******************************************************************************
  TFTP Server Module API Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    tftps.h

  Summary:
    The TFTP server module implements the server for trivial file transfer protocol. 

  Description:
    The TFTP module is the simple protocol which is used to transfer files.It 
    has been implemented on top of Internet User Datagram Protocol.
    If the server grants the request, the connection is opened and the file is 
    sent in fixed length blocks of 512 bytes. 
    Each data packet contains one block of data, and must be acknowledged by an
    acknowledgment packet before the next packet can be sent.
********************************************************************************/
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

#ifndef __TFTPS_H
#define __TFTPS_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// Placeholder for TFTPS Module Configuration
typedef struct
{
    // The TFTP server will access files located under this top directory.
    // No access is possible above this directory.
    // As a good practice to follow, use something like: "/mnt_point/tftps/"
    // to limit an external user access to this only directory in the file system
    // when accessing files.
    // NOTE: to allow access to the whole file system use the root directory:
    // "/mnt_point/" or "/" depending on your SYS_FS settings/mounting
    // But this usage should be discouraged because it gives TFTP Server access
    // to all system files.
    const char* mountPath;
} TCPIP_TFTPS_MODULE_CONFIG;


// *****************************************************************************
/* TCPIP_TFTPS_RESULT Enumeration

  Summary:
    Provides a list of possible results for the TFTPS module.

  Description:
    Provides a list of possible TFTPS results.

  Remarks:
    None
 */
typedef enum
{
    // success codes
    TFTPS_RES_OK,                // the operation was successful
    TFTPS_RES_PROGRESS,          // an TFTPS operation is in progress

    // error codes
    TFTPS_RES_IS_NOT_READY       = -1,   // If the TFTP Server is not ready 
    TFTPS_RES_BUSY               = -2,   // module is busy
    TFTPS_RES_TSTAMP_ERROR       = -3,   // no timestamp is available, server not contacted
    TFTPS_RES_TSTAMP_STALE       = -4,   //timestamp is stale, there's no recent timestamp
    TFTPS_RES_SKT_ERR            = -5,   // TFTP server socket could not be opened
    TFTPS_RES_SKT_BIND_ERR       = -6,   // TFTP server socket bind failed
    TFTPS_RES_CONN_ERR           = -7,   // Illegal TFTP Operation and TFTP connection error    
    TFTPS_RES_MEMORY_ALLOC_ERR   = -8,   // TFTP Client socket UDP Memory error
    TFTPS_RES_ILLIGAL_OPERN      = -9,   // TFTP server illegal operation
    TFTPS_RES_CLIENT_ERROR       = -10,   // TFTP server receives Client error code data
    TFTPS_RES_RECEIVE_ERROR      = -11,  // TFTP Server has not received any data
    TFTPS_RES_NO_CLIENT_CB       = -12, // No client control block is available
    TFTPS_RES_RCVD_FROM_SAME_CLIENT = -13, // When receive a request from same client with same port which is process
    TFTPS_RES_FILE_LENGTH_ERROR = -14,   // Invalid file name length error
    TFTPS_RES_FILE_NOT_FOUND_ERROR = -15, // File not found in the File system
    TFTPS_RES_TRANSFERMODE_NOT_SUPPORTED = -16, // Transfer mode not supported
} TCPIP_TFTPS_RESULT;


// *****************************************************************************
/* TCPIP_TFTPS_EVENT_TYPE Enumeration

  Summary:
    List of events for the TFTPS module.

  Description:
    Provides a list of run time TFTPS events .

  Remarks:
    None
 */
typedef enum
{
    TCPIP_TFTPS_EVENT_NONE           = 0,    // invalid event
    TCPIP_TFTPS_EVENT_READ_COMPLETED = 1, // send notification after file READ communication completion
    TCPIP_TFTPS_EVENT_WRITE_COMPLETED = 2, // send notification after file WRITE communication completion
    // error events
    TCPIP_TFTPS_EVENT_IF_ERROR       = -1,   // could not select a valid interface
    TCPIP_TFTPS_EVENT_SKT_ERROR      = -2,   // could not bind the socket to the interface
                                            // or timeout on the socket TX
    TCPIP_TFTPS_EVENT_CONNECTION_FAILURE = -3, // connection failure
    TCPIP_TFTPS_EVENT_SAME_REQUEST_RCVD = -4, // EVENT when same client request 
                                              //received from same port and same IP address
}TCPIP_TFTPS_EVENT_TYPE;


// *****************************************************************************
/* Type:
        TCPIP_TFTPS_EVENT_HANDLER

  Summary:
    Notification handler that can be called when there is a TFTP process for a client.

  Description:
    The format of a notification handler registered with the TFTP server module.
    Once an TFTP server event occurs the TFTP server service will be called 
    for the registered handler.
    
  Parameters:
    hNet    - the interface on which the DNS event occurred
    evType  - the TFTP server reported event
    name    - the host name associated with the event
    param   - additional user parameter - see TCPIP_TFTPS_HandlerRegister 

  Remarks:
    If pNetIf == 0, the notification is called for events on any interface.
*/
typedef void    (*TCPIP_TFTPS_EVENT_HANDLER)(TCPIP_NET_HANDLE hNet, TCPIP_TFTPS_EVENT_TYPE evType, const char* name, const void* param);

// *****************************************************************************
/*
  Type:
    TCPIP_TFTPS_HANDLE

  Summary:
    TFTP server handle.

  Description:
    A handle that a application needs to use when de-registering a notification handler. 

  Remarks:
    This handle can be used by the application after the event handler has been registered.
*/
typedef const void* TCPIP_TFTPS_HANDLE;

//*****************************************************************************
//*****************************************************************************
// Section: Functions
//*****************************************************************************
//*****************************************************************************
/*
  Function:
    void  TCPIP_TFTPS_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs TFTPS module tasks in the TCP/IP stack.

  Precondition:
    The TFTPS module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_TFTPS_Task(void);

//*****************************************************************************
/*
  Function:
    bool TCPIP_TFTPS_IsEnabled(void)

  Summary:
    Determines if a TFTP server is active or not.

  Description:
    This function determines if a  TFTP server socket connection is established 
    and ready to receive packet from the different client.

  Precondition:
    The TFTPS module should have been initialized.

  Parameters:
    None.

  Returns:
    true : if connection is established
    false: if server open fails. 

  Remarks:
    None.
*/
bool TCPIP_TFTPS_IsEnabled(void);

//*****************************************************************************
/*
  Function:
    uint8_t TCPIP_TFTPS_ClientsNumber(void)

  Summary:
    Determines the number of clients running for this server.

  Description:
    This function determines the number of active clients running for this TFTP server.
    The maximum possible clients will be as per the value of TCPIP_TFTPS_CLIENT_NUMBER

  Precondition:
    The TFTPS module should have been initialized.

  Parameters:
    None.

  Returns:
    number of active clients

  Remarks:
    None.
*/
uint8_t TCPIP_TFTPS_ClientsNumber(void);

//*****************************************************************************
/*
  Function:
    bool TCPIP_TFTPS_Disable(TCPIP_NET_HANDLE hNet);

  Summary:
    Disables the TFTP server for the specified interface.

  Description:
    This function disables the TFTP server for the specified interface.    

  Precondition:
    The TFTPS module should have been initialized.

  Parameters:
    hNet   - Interface for which to disable.

  Returns:
    - true  - if successful
    - false - if unsuccessful

  Remarks:
    None.
*/
bool TCPIP_TFTPS_Disable(TCPIP_NET_HANDLE hNet);

//*****************************************************************************
/*
  Function:
    bool TCPIP_TFTPS_Enable(TCPIP_NET_HANDLE hNet,IP_ADDRESS_TYPE ipType)

  Summary:
    Enables the TFTP server for the specified interface.

  Description:
    This function enables the TFTP server for the specified interface  
    and it opens the TFTP server with specific IP address TYPE.

  Precondition:
    The TFTPS module should have been initialized.

  Parameters:
    hNet    - Interface to enable
    ipType  - Interface address type. Depends on the enum type of IP_ADDRESS_TYPE

  Returns:
    - true  - if successful
    - false - if unsuccessful, if server open fails for this interface.

  Remarks:
    None.
*/
bool TCPIP_TFTPS_Enable(TCPIP_NET_HANDLE hNet,IP_ADDRESS_TYPE ipType);

// *****************************************************************************
/* 
  Function:
    TCPIP_TFTPS_HANDLE 
    TCPIP_TFTPS_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_TFTPS_EVENT_HANDLER handler, const void* hParam);

  Summary:
    Registers a TFTP Server Handler.

  Description:
    This function registers a TFTP Server event handler.
    The TFTP Server module will call the registered handler when a
    TFTP Server event (TCPIP_TFTPS_EVENT_TYPE) occurs during the File transfer 
    process.

  Precondition:
    The TFTP Server module must be initialized.

  Parameters:
    hNet    - Interface handle.
              Use hNet == 0 to register on all interfaces available.
    handler - Handler to be called when a DNS client event occurs.
    hParam  - Pointer to non-volatile ASCIIZ string to be used in the handler call.
              It is used as a domain/host name.
              If not NULL, a TFTP Server module notification will be delivered
              only for a file name resolution that matches the hParam.
              If the hParam == 0, then the notification is triggered
              for any host name resolution. 

  Returns:
    - Returns a valid handle if the call succeeds 
    - Returns null handle if the call failed (out of memory, for example)

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, <i>not</i> for lengthy processing!   
 */
TCPIP_TFTPS_HANDLE TCPIP_TFTPS_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_TFTPS_EVENT_HANDLER handler, const void* hParam);

// *****************************************************************************
/* Function:
    bool TCPIP_TFTPS_HandlerDeRegister(TCPIP_TFTPS_HANDLE hTftps);

  Summary:
    De-registers a previously registered TFTP Server handler.
    
  Description:
    This function de-registers the TFTP server module event handler.

  Precondition:
    The TFTP Server module must be initialized.

  Parameters:
    hTftps   - A handle returned by a previous call to TCPIP_TFTPS_HandlerRegister.

  Returns:
   - true  - if the call succeeds
   - false - if no such handler is registered
 */
bool TCPIP_TFTPS_HandlerDeRegister(TCPIP_TFTPS_HANDLE hTftps);
//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __TFTPS_H
