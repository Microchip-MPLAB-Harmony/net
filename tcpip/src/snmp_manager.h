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

