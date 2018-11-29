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
    assignment of human-name hostnames to access boards on the same subnet.
*******************************************************************************/
//DOM-IGNORE-BEGIN
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
    void* reserved;
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
