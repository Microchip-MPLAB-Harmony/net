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
