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
    void* reserved;
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
