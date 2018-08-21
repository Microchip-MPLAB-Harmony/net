/*******************************************************************************
  ZeroConf Module manager - private stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    zero_conf_manager.h

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

#ifndef __ZERO_CONF_MANAGER_H_
#define __ZERO_CONF_MANAGER_H_

bool TCPIP_MDNS_Initialize( const TCPIP_STACK_MODULE_CTRL* const stackData ,const TCPIP_DNS_CLIENT_MODULE_CONFIG* dnsData);
void TCPIP_MDNS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);


/***************************************************************
  Function:
   void TCPIP_ZCLL_Initialize(void)

  Summary:
   Initialization routine for Zeroconf Link-Local state-machine.

  Description:
    This is initialization function for Zeroconf Link-Local and
    invoked from initialization portion of Main-function.

    This function registers with ARP-module to get notifications
    about the incoming packets. Checks whether the Wi-Fi MAC is
    connected to an Access-Point or not.

  Parameters:
   None

  Returns:
     None
 */
bool TCPIP_ZCLL_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData ,const ZCLL_MODULE_CONFIG* zeroData);


/***************************************************************
  Function:
   void TCPIP_ZCLL_Deinitialize(void)

  Summary:
   Deinitialization routine for Zeroconf Link-Local state-machine.

  Description:
    This is deinitialization function for Zeroconf Link-Local and
    invoked from deinitialization portion of Main-function.

  Parameters:
   None

  Returns:
     None
 */
void TCPIP_ZCLL_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);


/***************************************************************
  Function:
   bool TCPIP_ZCLL_ServiceEnable(TCPIP_NET_IF* pNetIf, bool enable);

  Summary:
   Enables/disables the ZCLL service as requested by the stack manager

  Description:
    This function is called by the stack manager when the ZCLL service
    needs to be enabled or disabled.
    No callback into the stack manager is done.

  Parameters:
    pNetIf  - interface 
    enable  - if true, the ZCLL service will be enabled
              else the ZCLL service will be disabled      

  Returns:
     true if the call succeeds,
     false otherwise.

 */
bool TCPIP_ZCLL_ServiceEnable(TCPIP_NET_IF* pNetIf, bool enable);



#endif // __ZERO_CONF_MANAGER_H_
