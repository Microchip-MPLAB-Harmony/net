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
