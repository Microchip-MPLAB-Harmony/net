/*******************************************************************************
  Zero Configuration (Zeroconf) IPV4 Link Local Addressing Module for the Microchip 
  TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
   zero_conf_link_local.h

  Summary:
    Zero configuration (Zeroconf), provides a mechanism to ease the configuration 
    of a device on a network.
    
  Description:
    Zero configuration (Zeroconf), provides a mechanism to ease the configuration
    of a device on a network. It also provides for a more human-like naming 
    convention, instead of relying on IP addresses alone. Zeroconf also goes 
    by the names Bonjour (Apple) and Avahi (Linux), and is an IETF standard.
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
#ifndef __ZEROCONF_LINK_LOCAL_H
#define __ZEROCONF_LINK_LOCAL_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// Placeholder for Zero Configuration Link Layer module configuration.
typedef struct
{
}ZCLL_MODULE_CONFIG;

// ZCLL API

//*****************************************************************************
//*****************************************************************************
// Section: Functions
//*****************************************************************************
//*****************************************************************************

//******************************************************************************
/*
  Function:
    bool TCPIP_ZCLL_Enable(TCPIP_NET_HANDLE hNet)

  Summary:
   Enables Zero Configuration on the specified interface.

  Description:
   This API is used by end-user application to enable Zero Configuration on
   a specific interface.
   
  Precondition:
   TCP/IP stack must be initialized before calling this function.

  Parameters:
    hNet - handle of the network to be enabled

  Returns:
   - true  - Indicates success
   - false - Indicates failure
   
  Remarks:
    None.
*/
bool    TCPIP_ZCLL_Enable(TCPIP_NET_HANDLE hNet);

//***************************************************************
/*
  Function:
    bool TCPIP_ZCLL_Disable(TCPIP_NET_HANDLE hNet)

  Summary:
    Disables Zero Configuration on the specified interface.

  Description:
   This API is used by end-user application to disable Zero Configuration on
   a specific interface.
   
  Precondition:
    The TCP/IP stack must be initialized before calling this function.

  Parameters:
    hNet - handle of the network to be disabled

  Returns:
   - true  - Indicates success
   - false - Indicates failure
   
  Remarks:
    None.
*/
bool    TCPIP_ZCLL_Disable(TCPIP_NET_HANDLE hNet);

//***************************************************************
/*
  Function:
    bool TCPIP_ZCLL_IsEnabled(TCPIP_NET_HANDLE hNet)

  Summary:
    Returns whether or not an interface is enabled for zero configuration

  Description:
   This API is used by end-user application check whether or not that an
   interface is enabled for zero configuration.
   
  Precondition:
   The TCP/IP stack must be initialized before calling this function.

  Parameters:
    hNet - handle of the network to be examined

  Returns:
    - true  - interface is enabled for zero configuration
    - false - interface is not enabled for zero configuration
  **************************************************************/
bool    TCPIP_ZCLL_IsEnabled(TCPIP_NET_HANDLE hNet);


// *****************************************************************************
/*
  Function:
    void  TCPIP_ZCLL_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs ZCLL module tasks in the TCP/IP stack.

  Precondition:
    The ZCLL module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_ZCLL_Task(void);


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif //#ifndef __ZEROCONF_LINK_LOCAL_H