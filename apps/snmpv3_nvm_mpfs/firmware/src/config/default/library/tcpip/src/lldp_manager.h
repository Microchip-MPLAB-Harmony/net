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
/*******************************************************************************
Copyright © 2014 released Microchip Technology Inc.  All rights reserved.

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
