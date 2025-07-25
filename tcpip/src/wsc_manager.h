/*******************************************************************************
  WSC module manager header

  Company:
    Microchip Technology Inc.
    
  File Name:
    wsc_manager.h

  Summary:
    Stack internal definitions for WSC module

  Description:
    This file contains the stack internal API for the WSC module
*******************************************************************************/
/*
Copyright (C) 2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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









#ifndef H_WSC_MANAGER_H_
#define H_WSC_MANAGER_H_


// stack private API functions

/*****************************************************************************
  Function:
    void TCPIP_WSC_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_WSC_MODULE_CONFIG* wscData)

  Summary:
    Initializes the WSC module.

  Description:
    Initializes the WSC module.

  Precondition:
    None

  Parameters:
    stackCtrl  - stack initialization parameters
    wscData    - WSC specific initialization parameters

  Returns:
    true if initialization succeeded,
    false otherwise

  Remarks:
    None
 */
bool  TCPIP_WSC_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData);


/*****************************************************************************
  Function:
    void  TCPIP_WSC_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)

  Summary:
    Deinitializes the WSC module.

  Description:
    Deinitializes the WSC module.

  Precondition:
    None.

  Parameters:
    stackCtrl  - stack initialization parameters, used by TCPIP_WSC_Initialize
                 to initialize the WSC module.

  Returns:
    None.

  Remarks:
    None.
 */
void  TCPIP_WSC_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);

#endif  // H_WSC_MANAGER_H_

