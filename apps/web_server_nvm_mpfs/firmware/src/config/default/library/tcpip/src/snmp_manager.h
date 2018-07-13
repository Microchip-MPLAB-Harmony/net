/*******************************************************************************
  SNMP Server internal stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    snmp_manager.h

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

#ifndef __SNMP_MANAGER_H_
#define __SNMP_MANAGER_H_

/****************************************************************************
  Function:
    bool TCPIP_SNMP_Initialize(void)

  Summary:
    Initialize SNMP module internals.

  Description:
    This function initializes the SNMP agent. One UDP socket is initialized
    and opened at port 161. Agent will receive and transmit all the SNMP
    PDUs on this UDP socket.

  Precondition:
    At least one UDP socket must be available. TCPIP_UDP_Initialize() is already called.

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is called only once during lifetime of the application.
    One UDP socket will be used.
 */
bool TCPIP_SNMP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const TCPIP_SNMP_MODULE_CONFIG* snmpData);


/****************************************************************************
  Function:
    void TCPIP_SNMP_Deinitialize(void)

  Summary:
    DeInitialize SNMP module internals.

  Description:
    This function deinitializes the SNMP agent. Closes the UDP socket if all
    interfaces are down.

  Precondition:
    None.

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function may be called many times during lifetime of the application.
 */
void TCPIP_SNMP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);



#endif  // __SNMP_MANAGER_H_

