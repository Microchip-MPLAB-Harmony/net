/*******************************************************************************
  NetBIOS Name Service (NBNS) Server public API

  Company:
    Microchip Technology Inc.
    
  File Name:
    nbns.h

  Summary:
    The NetBIOS Name Service protocol associates host names with IP addresses,
    similarly to DNS, but on the same IP subnet.

  Description:
    The NetBIOS Name Service protocol associates host names with IP addresses, 
    similarly to DNS, but on the same IP subnet. Practically, this allows the 
    assignment of human-name hostnames to access boards on the same subnet. For 
    example. in the "TCP/IP Demo App" demonstration project, the demo board is 
    programmed with the human name 'mchpboard' so it can be accessed directly 
    instead of with its IP address.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2012-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
//DOM-IGNORE-END

#ifndef __NBNS_H_
#define __NBNS_H_


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

//******************************************************************************
/*
  TCPIP_NBNS_MODULE_CONFIG Structure Typedef

  Summary:
    Placeholder for NBNS configuration upgrades.

  Description:
    This type definition provides a placeholder for NBNS configuration upgrades.

  Remarks:
    None.
*/
typedef struct
{
} TCPIP_NBNS_MODULE_CONFIG;

// *****************************************************************************
// *****************************************************************************
// Section: Task Function
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    void  TCPIP_NBNS_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs NBNS module tasks in the TCP/IP stack.

  Precondition:
    The NBNS module should have been initialized.

  Parameters:
    None.

  Returns:
    None.
	
  Remarks:
    None.
*/
void  TCPIP_NBNS_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __NBNS_H_