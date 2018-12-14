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
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/








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

