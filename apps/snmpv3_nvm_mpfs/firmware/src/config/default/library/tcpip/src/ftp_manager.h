/*******************************************************************************
  FTP Server Internal Stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    ftp_manager.h

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

#ifndef __FTP_MANAGER_H_
#define __FTP_MANAGER_H_

/*********************************************************************
  Function:  bool TCPIP_FTP_ServerInitialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const TCPIP_FTP_MODULE_CONFIG* ftpData)

  PreCondition:    TCP module is already initialized.

  Input:           None

  Output:          true is success
                   false otherwise

  Side Effects:    None

  Overview:        Initializes internal variables of FTP

  Note:
 */
bool TCPIP_FTP_ServerInitialize(const TCPIP_STACK_MODULE_CTRL* const stackData,
                          const TCPIP_FTP_MODULE_CONFIG* ftpData);


/*********************************************************************
  Function:        void TCPIP_FTP_ServerDeinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)

  PreCondition:    None

  Input:           None

  Output:          None

  Side Effects:    None

  Overview:        DeInitializes internal variables of FTP

  Note:
 */
void TCPIP_FTP_ServerDeinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);



#endif  // __FTP_MANAGER_H_

