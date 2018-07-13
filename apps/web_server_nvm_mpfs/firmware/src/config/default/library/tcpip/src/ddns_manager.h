/*******************************************************************************
  Dynamic DNS Headers for Internal Stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    ddns_manager.h

  Summary:
    Dynamic DNS Headers for Internal Stack API

  Description:
    This file provides the DDNS API definitions.

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

#ifndef __DYN_DNS_MANAGER_H
#define __DYN_DNS_MANAGER_H



/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/

/****************************************************************************
  Function:
    void TCPIP_DDNS_Initialize(void)

  Summary:
    Initializes the Dynamic DNS module.

  Description:
    This function initializes the Dynamic DNS client.  It clears the
    DDNSClient pointers structure, and tells the module to attempt the
    first update after 15 seconds have elapsed (so as to allow the DHCP
    configuration to stabilize).

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is called only one during lifetime of the application.
  ***************************************************************************/
bool TCPIP_DDNS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const DDNS_MODULE_CONFIG* ddnsData);


/****************************************************************************
  Function:
    void TCPIP_DDNS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)

  Summary:
    Deinitializes the Dynamic DNS module.

  Description:
    This function deinitializes the Dynamic DNS client.  Clears the
    DDNSClient pointers structure when bringing an interface down.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is called only once during lifetime of the application.
  ***************************************************************************/
void TCPIP_DDNS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);


#endif  // __DYN_DNS_MANAGER_H


