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
    void*   reserved;
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

