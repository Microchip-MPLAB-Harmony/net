/*******************************************************************************
  LLDP Module API Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    lldp.h

  Summary:
    LLDP Module API definitions.

  Description:
    The LLDP module implements the Link Local Discovery Protocol.
********************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2014-2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef __LLDP_H
#define __LLDP_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// LLDP Module Configuration
typedef struct
{
} TCPIP_LLDP_MODULE_CONFIG;

void TCPIP_LLDP_RxEnable(void);
void TCPIP_LLDP_TxEnable(void);
void TCPIP_LLDP_RxTxEnable(void);
void TCPIP_LLDP_PortDisable(void);

void TCPIP_LLDP_FastTxCounterSet(uint8_t n);

void TCPIP_LLDP_MacDestAddressSet(TCPIP_MAC_ADDR* pAddr);

// returns the current allocated power
uint16_t TCPIP_LLDP_AllocatedPowerGet(void);

void     TCPIP_LLDP_DesiredPowerSet(uint16_t desiredPower);

bool TCPIP_LLDP_UPoePowerIsEnabled(void);

bool TCPIP_LLDP_PoePlusPowerIsEnabled(void);
bool TCPIP_LLDP_PoeMinPowerIsEnabled(void);

// *****************************************************************************
// *****************************************************************************
// Section: Task Function
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    void  TCPIP_LLDP_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs LLDP module tasks in the TCP/IP stack.

  Precondition:
    The LLDP module should have been initialized

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_LLDP_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // __LLDP_H

