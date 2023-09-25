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
