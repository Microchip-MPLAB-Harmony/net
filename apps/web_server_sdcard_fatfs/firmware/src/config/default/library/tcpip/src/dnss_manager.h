/*******************************************************************************
Domain Name System (DNS) server Header file
  
  Company:
    Microchip Technology Inc.
	
  File Name:
    dnss_manager.h

  Summary:
    DNS Server manager interface file
    
  Description:
    This source file contains the DNS manager API
*******************************************************************************/

/*******************************************************************************
File Name:  dnss_manager.h 
Copyright © 2012 released Microchip Technology Inc.  All rights
reserved.

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

#ifndef _DNSS_MANAGER_H_
#define _DNSS_MANAGER_H_




// DNSs.c function prototypes
bool        TCPIP_DNSS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_DNSS_MODULE_CONFIG* pDnsConfig);

void        TCPIP_DNSS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);

#endif  // _DNSS_MANAGER_H_

