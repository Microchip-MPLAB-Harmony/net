/*******************************************************************************
  SNTP Client manager private stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    sntp_manager.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#ifndef __SNTP_MANAGER_H_
#define __SNTP_MANAGER_H_

/*****************************************************************************
  Function:
    bool TCPIP_SNTP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_SNTP_MODULE_CONFIG* pSNTPConfig);

  Summary:
    Resets the SNTP client module.

  Description:
    Initialization of the SNTP module

  Precondition:
    None

  Parameters:
    stackCtrl - pointer to stack structure specifying the interface to initialize
    pSNTPConfig - pointer to SNTP configuration data

  Returns:
    None

  Remarks:
    None
 */
bool        TCPIP_SNTP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_SNTP_MODULE_CONFIG* pSNTPConfig);


/*****************************************************************************
  Function:
    bool TCPIP_SNTP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit);

  Summary:
    Turns off the SNTP module for the specified interface.

  Description:
    Deinitialization of the SNTP module.

  Precondition:
    None

  Parameters:
    stackData - pointer to stack structure specifying the interface to deinitialize

  Returns:
    None

  Remarks:
 */
void        TCPIP_SNTP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);



#endif  // __SNTP_MANAGER_H_
