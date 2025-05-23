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
/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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








// DOM-IGNORE-END

#ifndef H_ARP_MANAGER_H_
#define H_ARP_MANAGER_H_


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
// bool  TCPIP_ARP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_ARP_MODULE_CONFIG* arpData);
bool  TCPIP_ARP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData);


/*****************************************************************************
  Function:
    void  TCPIP_ARP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)

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
void  TCPIP_ARP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);


// TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL API specific Definitions
struct arp_app_callbacks {
    bool used;
    void (*TCPIP_ARP_PacketNotify)( const TCPIP_NET_IF*  pNetIf
                           ,uint32_t SenderIPAddr
                           ,uint32_t TargetIPAddr
                           ,TCPIP_MAC_ADDR* SenderMACAddr
                           ,TCPIP_MAC_ADDR* TargetMACAddr
                           ,uint8_t op_req);
};

/*****************************************************************************
  Function:
    ssize_t TCPIP_ARP_CallbacksRegister(struct arp_app_callbacks *app)

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
ssize_t TCPIP_ARP_CallbacksRegister(struct arp_app_callbacks *app);


/*****************************************************************************
  Function:
    bool TCPIP_ARP_CallbacksDeregister(size_t regId)

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
    regId - Registration-id returned in TCPIP_ARP_CallbacksRegister call

  Returns:
    true  - On success
    false - Failure to indicate invalid regId
 */
bool TCPIP_ARP_CallbacksDeregister(size_t regId);


#endif  // H_ARP_MANAGER_H_

