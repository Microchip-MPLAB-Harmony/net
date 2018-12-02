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


