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


