/*******************************************************************************
  ARP module manager header

  Company:
    Microchip Technology Inc.
    
  File Name:
    arp_manager.h

  Summary:
    Stack internal definitions for ARP module

  Description:
    This file contains the stack internal API for the ARP module
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

#ifndef _ARP_MANAGER_H_
#define _ARP_MANAGER_H_


// stack private API functions

/*****************************************************************************
  Function:
    void TCPIP_ARP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_ARP_MODULE_CONFIG* arpData)

  Summary:
    Initializes the ARP module.

  Description:
    Initializes the ARP module.
    Calls can be done with the request of not tearing down the ARP cache
    This helps for ifup/ifdown sequences.
    Of course, if this is the case the memory allocated for the ARP cache
    has to be from a persistent heap.

  Precondition:
    None

  Parameters:
    stackCtrl  - stack initialization parameters
    arpData    - ARP specific initialization parameters

  Returns:
    true if initialization succeeded,
    false otherwise

  Remarks:
    The request to maintain old ARP cache info (deleteOld field from the
    TCPIP_ARP_MODULE_CONFIG initialization data) is not implemented for stack
    nit/deinit sequences.  To maintain the data after the stack is completely
    deinitialized would need a persistent heap that's not yet implemented.
    The selection cannot be changed by ifup since this operation does not
    carry ARP configuration parameters (arpDate == 0).
 */
bool  TCPIP_ARP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData ,const TCPIP_ARP_MODULE_CONFIG* arpData);


/*****************************************************************************
  Function:
    void  TCPIP_ARP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)

  Summary:
    Deinitializes the ARP module.

  Description:
    Deinitializes the ARP module.

  Precondition:
    None.

  Parameters:
    stackCtrl  - stack initialization parameters, used by TCPIP_ARP_Initialize
                 to initialize the ARP module.

  Returns:
    None.

  Remarks:
    None.
 */
void  TCPIP_ARP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);


// TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL API specific Definitions
struct arp_app_callbacks {
    bool used;
    void (*TCPIP_ARP_PacketNotify)( TCPIP_NET_IF*  pNetIf
                           ,uint32_t SenderIPAddr
                           ,uint32_t TargetIPAddr
                           ,TCPIP_MAC_ADDR* SenderMACAddr
                           ,TCPIP_MAC_ADDR* TargetMACAddr
                           ,uint8_t op_req);
};

/*****************************************************************************
  Function:
    int8_t TCPIP_ARP_CallbacksRegister(struct arp_app_callbacks *app)

  Summary:
    Registering callback with ARP module to get notified about certain events.

  Description:
    This function allows end user application to register with callbacks, which
    will be called by ARP module to give notification to user-application about
    events occurred at ARP layer. For ex: when a ARP-packet is received, which is
    conflicting with our own pair of addresses (MAC-Address and IP-address).
    This is an extension for Zeroconf protocol implementation (ZeroconfLL.c)

  Precondition:
    None

  Parameters:
    app - ARP-Application callbacks structure supplied by user-application

  Returns:
    id > 0 - Returns non-negative value that represents the id of registration
             The same id needs to be used in deregistration
    -1     - When registered applications exceed MAX_REG_APPS and there is no
             free slot for registration

 */
int8_t TCPIP_ARP_CallbacksRegister(struct arp_app_callbacks *app);


/*****************************************************************************
  Function:
    bool TCPIP_ARP_CallbacksDeregister(int8_t reg_id)

  Summary:
    De-Registering callbacks with ARP module that are registered previously.

  Description:
    This function allows end user-application to deregister with callbacks,
    which were registered previously.
    This is called by user-application, when its no longer interested in
    notifications from ARP-Module. This allows the other application to get
    registered with ARP-module.

  Precondition:
    None

  Parameters:
    reg_id - Registration-id returned in TCPIP_ARP_CallbacksRegister call

  Returns:
    true  - On success
    false - Failure to indicate invalid reg_id
 */
bool TCPIP_ARP_CallbacksDeregister(int8_t id);


#endif  // _ARP_MANAGER_H_

