/**************************************************************************
iPerf Header File 

 Company:
    Microchip Technology Inc.
	
  File Name:
    iperf.h
	
  Summary:
    iPerf provides standard network benchmarking
    at run time for both TCP and UDP.
	
  Description:
    iPerf benchmarking tool.

  **************************************************************************/
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

#ifndef __IPERF_H
#define __IPERF_H

#include <stdint.h>
#include <stdbool.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
/*
 iPerf Configuration structure placeholder
*/
typedef struct
{
}TCPIP_IPERF_MODULE_CONFIG;


// *****************************************************************************
/*
  Function:
    void  TCPIP_IPERF_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs iPerf module tasks in the TCP/IP stack.

  Precondition:
    The iPerf module should have been initialized

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_IPERF_Task(void);
    
//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif	//#ifndef __IPERF_H