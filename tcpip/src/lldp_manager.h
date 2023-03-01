/*******************************************************************************
  LLDP manager private stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    lldp_manager.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2014-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef __LLDP_MANAGER_H_
#define __LLDP_MANAGER_H_

/*****************************************************************************
  Function:
    bool TCPIP_LLDP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_LLDP_MODULE_CONFIG* pLLDPConfig);

  Summary:
    Resets the LLDP client module.

  Description:
    Initialization of the LLDP module

  Precondition:
    None

  Parameters:
    stackCtrl   - pointer to stack structure specifying the interface to initialize
    pLLDPConfig - pointer to LLDP configuration data

  Returns:
    None

  Remarks:
    None
 */
bool        TCPIP_LLDP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_LLDP_MODULE_CONFIG* pLLDPConfig);


/*****************************************************************************
  Function:
    bool TCPIP_LLDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit);

  Summary:
    Turns off the LLDP module for the specified interface.

  Description:
    Deinitialization of the LLDP module.

  Precondition:
    None

  Parameters:
    stackData - pointer to stack structure specifying the interface to deinitialize

  Returns:
    None

  Remarks:
 */
void        TCPIP_LLDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);



// current LLDPDU interface MAC address
const uint8_t* TCPIP_LLDP_LocalIfAddressGet(void);

// current destination MAC address
const TCPIP_MAC_ADDR* TCPIP_LLDP_MacDestAddressGet(void);

// forward declaration
struct lldp_per_port_t;
const struct lldp_per_port_t*      TCPIP_LLDP_PortGet(void);

// set the allocated power
void    TCPIP_LLDP_AllocatedPowerSet(uint16_t allocatedPower);

typedef union
{
    uint8_t val;
    struct
    {
        unsigned fixTlvSize             :1;
        unsigned poeEnabledPair         :1;
        unsigned powerAllocated         :1;
        unsigned uPoeEnabledPower       :1;
        unsigned poePlusEnabledPower    :1;
        unsigned poeEnabledMinPower     :1;
        unsigned reserved               :2;
    };
}TCPIP_LLDP_ORG_FLAGS;

bool    TCPIP_LLDP_OrgProcessFlagsGet(TCPIP_LLDP_ORG_FLAGS* pFlags);


#endif  // __LLDP_MANAGER_H_
