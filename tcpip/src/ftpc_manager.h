/*******************************************************************************
  FTPC Manager Header file

  Company:
    Microchip Technology Inc.
    
  File Name:
    ftpc_manager.h

  Summary:
    FTPC manager interface file

  Description:
    This source file contains the stack internal FTPC manager API
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _FTPC_MANAGER_H_
#define _FTPC_MANAGER_H_


// stack private module API
//FTPC module initialization
bool        TCPIP_FTPC_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_FTPC_MODULE_CONFIG_TYPE* pftpcConfig);
//FTPC de-initailize function
void        TCPIP_FTPC_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);

#endif  // _FTPC_MANAGER_H_

