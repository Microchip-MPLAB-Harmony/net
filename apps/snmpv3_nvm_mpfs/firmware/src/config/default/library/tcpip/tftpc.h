/*******************************************************************************
  TFTP Client Module API Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    tftpc.h

  Summary:
    The TFTP Client module implements the Trivial File Transfer Protocol (TFTP). 
    
  Description:
    The TFTP Client module implements the Trivial File Transfer Protocol (TFTP). 
	By default, the module opens a client socket for the default interface 
	configuration.
    From the command prompt, the TFTP client module mode will be selected. At 
	present only two modes are supported: Read and Write.
    * For Read mode - File will be fetched from the Server using the GET command.
    * For Write mode - Server will be able to fetch the file from the client using 
	  the PUT command.
    The TFTP module needs the file system for GET and PUT command operation.
    When one mode is in operation, access to the other mode or another server 
	is not allowed.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef __TFTPC_H
#define __TFTPC_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// *****************************************************************************
/*
  Enumeration:
    TCPIP_TFTP_CMD_TYPE

  Summary:
    File command mode used for TFTP PUT and GET commands.

  Description:
    These enum values are issued from the command line.
*/
typedef enum _TFTP_CMD_TYPE
{
    TFTP_CMD_PUT_TYPE=0,   // TFTP client issues a PUT command to write a file 
	                       // to the server
    TFTP_CMD_GET_TYPE,     // TFTP client issues a GET command to read the file 
	                       // from the server
    TFTP_CMD_NONE,
}TCPIP_TFTP_CMD_TYPE;



// *****************************************************************************
/*
  Enumeration:
    TCPIP_TFTPC_OPERATION_RESULT

  Summary:
    Standard error codes for TFTP PUT and GET command operation.

  Description:
    This enumeration defines the standard error codes for TFTP PUT and GET 
	command operation.
*/

typedef enum 
{
    TFTPC_ERROR_NONE = 0,
    TFTPC_ERROR_FILE_NOT_FOUND = -1,     // TFTP client file not found
    TFTPC_ERROR_BUSY = -2,               // TFTP client is busy when one file 
                                         // put or get transfer is going on.
    TFTPC_ERROR_DISK_FULL = -3,          // TFTP client buffer full
    TFTPC_ERROR_INVALID_OPERATION = -4,  // TFTP client invalid command operation
    TFTPC_ERROR_UNKNOWN_TID = -5,        // TFTP ID error
    TFTPC_ERROR_FILE_EXISTS = -6,        // TFTP client file already exists
    TFTPC_ERROR_NO_SUCH_USE = -7,        // TFTP client not in use
    TFTPC_ERROR_DNS_RESOLVE_ERR = -8,    // TFTP client DNS resolve error
    TFTPC_ERROR_INVALID_INTERFACE = -9,    // TFTP client interface error
    TFTPC_ERROR_INVALID_FILE_LENGTH = -10, // TFTP client file length is more than 
	                                     // the expected size, which should be
                                         // the size of SYS_FS_MAX_PATH
    TFTPC_ERROR_INVALID_SERVER_ADDR=-11,   // Invalid Server Address

} TCPIP_TFTPC_OPERATION_RESULT;

// *****************************************************************************
/*
  Type:
    TCPIP_TFTPC_HANDLE

  Summary:
    TFTPC handle.

  Description:
    A handle that a client can use after the event handler has been registered.
 */
typedef const void* TCPIP_TFTPC_HANDLE;

// *****************************************************************************
/* Enumeration: TCPIP_TFTPC_EVENT_TYPE

  Summary:
    TFTP client Event Type

  Description:
    None.
 */
typedef enum
{
    TFTPC_EVENT_NONE            = 0,     // TFTP no event
    TFTPC_EVENT_PUT_REQUEST     = 0x0001,// TFTP PUT request sent
    TFTPC_EVENT_GET_REQUEST     = 0x0002,// TFTP GET Request sent
    TFTPC_EVENT_ACKED           = 0x0004,// TFTP request acknowledge was received
    TFTP_EVENT_DATA_RECEIVED    = 0x0008,// TFTP Client received Data for GET request
    TFTPC_EVENT_DECLINE         = 0x0010,// TFTP File Put or Get communication declined due to Bad PDU
    TFTPC_EVENT_TIMEOUT         = 0x0020,// TFTP server timeout
    TFTPC_EVENT_COMPLETED       = 0x0040,// TFTP File Put or Get completed    
    TFTPC_EVENT_CONN_LOST       = 0x0080,// connection to the TFTP server lost
    TFTPC_EVENT_CONN_ESTABLISHED= 0x0100,// connection re-established
    TFTPC_EVENT_SERVICE_DISABLED= 0x0200,// TFTP service disabled
    TFTPC_EVENT_BUSY            = 0x0400,// TFTP Client communication is going on
} TCPIP_TFTPC_EVENT_TYPE;

// *****************************************************************************
/*
  Structure:
    TCPIP_TFTPC_MODULE_CONFIG

  Summary:
    Placeholder for TFTP Client module configuration.

  Description:
    This structure is a placeholder for TFTP Client module configuration.
*/
typedef struct
{
    const char*     tftpc_interface;
    uint32_t        tftpc_reply_timeout;      // time-out for the server reply in seconds
} TCPIP_TFTPC_MODULE_CONFIG;

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

//*********************************************************************
/*
 Function:
    TCPIP_TFTPC_OPERATION_RESULT TCPIP_TFTPC_SetCommand(IP_MULTI_ADDRESS* mAddr,
 *          IP_ADDRESS_TYPE ipType,TCPIP_TFTP_CMD_TYPE cmdType,
 *          const char * fileName )

  Summary:
    TFTP client command operation configuration.

  Description:
    This function is used to set the client mode, server, and file name.
    The file name is accessed as per the TFTP command mode.

  Precondition:
    The TCP/IP Stack should have been initialized.

  Parameters:
    mAddr - Server address
    ipType - IP address type either IPv4 or IPv6 type
    cmdType -  GET or PUT command
    fileName - File to be processed

  Returns:
    TFTPC_ERROR_BUSY - TFTP client is busy for one file processing and please retry later.
    when there is a TFTP operation going on, the another operation has to wait 
    till the TFTP operation is completed.
    TFTPC_ERROR_INVALID_FILE_LENGTH -  File Length should not be more than TCPIP_TFTPC_FILENAME_LEN.
    TFTPC_ERROR_NONE -  Successful command operation.
  Remarks:
    None.
 */
TCPIP_TFTPC_OPERATION_RESULT TCPIP_TFTPC_SetCommand(IP_MULTI_ADDRESS* mAddr,
          IP_ADDRESS_TYPE ipType,TCPIP_TFTP_CMD_TYPE cmdType,
         const char * fileName );

// *****************************************************************************
/*
  Function:
    void  TCPIP_TFTPC_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs TFTP module tasks in the TCP/IP stack.

  Precondition:
    The TFTP module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_TFTPC_Task(void);

// *****************************************************************************
/*
  Type:
    TCPIP_TFTPC_EVENT_HANDLER

  Summary:
    TFTPC event handler prototype.

  Description:
    Prototype of a TFTPC event handler. Clients can register a handler with the
    TFTP service. Once an TFTP event occurs the TFTP Client service will be called the
    registered handler.
    The handler has to be short and fast. It is meant for
    setting an event flag, <i>not</i> for lengthy processing!
    buf - Buffer is used to provide the memory Pointer .
          buf type need to be typecasted to char* while processing
    bufLen - The number of bytes present in the buffer.    
 */

typedef void    (*TCPIP_TFTPC_EVENT_HANDLER)(TCPIP_NET_HANDLE hNet, TCPIP_TFTPC_EVENT_TYPE evType,void *buf,uint32_t bufLen, const void* param);


// *****************************************************************************
/* Function:
    TCPIP_TFTPC_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_TFTPC_EVENT_HANDLER handler,
	                           const void* hParam)

  Summary:
    Registers a TFTPC Handler.

  Description:
    This function registers a TFTPC event handler.
    The TFTP Client module will call the registered handler when a
    TFTP Client event (TCPIP_TFTPC_EVENT_TYPE) occurs.

  Precondition:
    The TFTP Client module must be initialized.

  Parameters:
    hNet    - Interface handle.
              Use hNet == 0 to register on all interfaces available.
    handler - Handler to be called when a TFTP Client event occurs.
    hParam  - Parameter to be used in the handler call.
              This is user supplied and is not used by the DHCP module.


  Returns:
    Returns a valid handle if the call succeeds, or a null handle if
    the call failed (out of memory, for example).

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

    The hParam is passed by the client and will be used by the DHCP when the
    notification is made. It is used for per-thread content or if more modules,
    for example, share the same handler and need a way to differentiate the
    callback.
 */

TCPIP_TFTPC_HANDLE  TCPIP_TFTPC_HandlerRegister(TCPIP_NET_HANDLE hNet, 
                          TCPIP_TFTPC_EVENT_HANDLER handler, const void* hParam);

// *****************************************************************************
/* Function:
    bool TCPIP_TFTPC_HandlerDeRegister(TCPIP_TFTPC_HANDLE htftpc)

  Summary:
    Deregisters a previously registered TFTP Client handler.
    
  Description:
    This function deregisters the TFTP Client event handler.

  Precondition:
    The TFTP Client module must be initialized.

  Parameters:
    htftpc   - A handle returned by a previous call to TCPIP_TFTPC_HandlerRegister.

  Returns:
    - true	- if the call succeeds
    - false - if no such handler is registered
 */
bool  TCPIP_TFTPC_HandlerDeRegister(TCPIP_TFTPC_HANDLE hDhcp);

// *****************************************************************************
/* Function:
    TCPIP_TFTPC_EVENT_TYPE TCPIP_TFTPC_GetEventNotification(void)

  Summary:
    Get the details of the TFTP client file mode communication event details.
    
  Description:
    This function returns the event type TCPIP_TFTPC_EVENT_TYPE for different 
    modes of TFTP file communication.

  Precondition:
    The TFTP Client module must be initialized.

  Parameters:
    None

  Returns:
    TCPIP_TFTPC_EVENT_TYPE : It will be OR of different events.
 */
TCPIP_TFTPC_EVENT_TYPE TCPIP_TFTPC_GetEventNotification(void);

// *****************************************************************************
/* Function:
    void TCPIP_TFTPC_SetServerAddress(IP_MULTI_ADDRESS* ipAddr,IP_ADDRESS_TYPE ipType)

  Summary:
    Set the Server IP address for TFTP Client .
    
  Description:
    This function is used to set the TFTP server address( either it will be IPv4 address
    or IPv6 address). This address will be used for either Get or Put mode of TFTP 
    Client operation.

  Precondition:
    The TFTP Client module must be initialized.

  Parameters:
    ipAddr  - pointer to the server address
    ipType  - type of address: IPv4/IPv6

  Returns:
    None
 */
void TCPIP_TFTPC_SetServerAddress(IP_MULTI_ADDRESS* ipAddr,IP_ADDRESS_TYPE ipType);


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __TFTPC_H
