/*******************************************************************************
  NetBIOS Name Service (NBNS) Server internal API

  Company:
    Microchip Technology Inc.
    
  File Name:
    nbns_manager.h

  Summary:

  Description:
    Responds to NBNS name requests to allow human name assignment
    to the board.
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

#ifndef __NBNS_MANAGER_H_
#define __NBNS_MANAGER_H_


/*********************************************************************
  Function:        void TCPIP_NBNS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_NBNS_MODULE_CONFIG* pNbnsInit)

  PreCondition:    None

  Input:           stackCtrl - Interface and stack module data.
                   pNbnsInit - Module-specific information for NBNS.

  Output:          None

  Side Effects:    None

  Overview:        Initializes state machine

  Note:            None
 */
bool TCPIP_NBNS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, const TCPIP_NBNS_MODULE_CONFIG* pNbnsInit);


/*********************************************************************
  Function:        void TCPIP_NBNS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)

  PreCondition:    None

  Input:           stackCtrl - Interface and stack module data.

  Output:          None

  Side Effects:    None

  Overview:        DeInitializes state machine

  Note:            None
 */
void TCPIP_NBNS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);



#endif  // __NBNS_MANAGER_H_

