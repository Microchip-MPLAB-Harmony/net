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

