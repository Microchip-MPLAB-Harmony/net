/*******************************************************************************
  HTTP internal API Headers for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    http_manager.h

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

#ifndef __HTTP_MANAGER_H_
#define __HTTP_MANAGER_H_


/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/

/*****************************************************************************
  Function:
    bool TCPIP_HTTP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl,
        const TCPIP_HTTP_MODULE_CONFIG* httpInitData)

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
bool TCPIP_HTTP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const TCPIP_HTTP_MODULE_CONFIG* httpData);


/*****************************************************************************
  Function:
    bool TCPIP_HTTP_Deinitialize(void)

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
void TCPIP_HTTP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);



#endif // __HTTP_MANAGER_H_

