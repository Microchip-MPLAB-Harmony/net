/*******************************************************************************
  PTP Manager internal stack API for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ptp_manager.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2017 released Microchip Technology Inc.  All rights reserved.

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

#ifndef  _PTP_MANAGER_H_
#define  _PTP_MANAGER_H_

/*****************************************************************************
  Function:
    bool TCPIP_PTP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, TCPIP_PTP_MODULE_CONFIG* pPtpInit)

  Summary:
    Initializes the PTP module.

  Description:
	Initializes the PTP module.
    This function will register the module with the stack manager.
    If the initialization failed it will return false.

  Precondition:
	None

  Parameters:
    stackInit   - pointer to stack initialization data; contains heap, interfaces, etc

    pPtpInit    - pointer to a PTP initialization structure containing the PTP configuration parameters

  Returns:
  	true if initialization succeeded
    false otherwise
  	
  Remarks:
   None
 */
bool TCPIP_PTP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, const TCPIP_PTP_MODULE_CONFIG* pPtpInit);


/*****************************************************************************
  Function:
    void TCPIP_PTP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit)

  Summary:
    De-Initializes the PTP module.

  Description:
    De-Initializes the PTP module.

  Precondition:
    TCPIP_PTP_Initialize() should have been called

  Parameters:
    stackInit   - pointer to stack initialization data; contains heap, interfaces, etc
                  and interface that's going down

  Returns:
    None

  Remarks:
    None
 */
void TCPIP_PTP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit);



#endif  // _PTP_MANAGER_H_

