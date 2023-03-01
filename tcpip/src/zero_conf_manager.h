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
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
