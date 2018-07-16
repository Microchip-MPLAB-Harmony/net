/*******************************************************************************
  MPU PLIB Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    plib_mpu.h

  Summary:
    MPU PLIB Source File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/

#include "plib_mpu.h"
#include "plib_mpu_local.h"


// *****************************************************************************
// *****************************************************************************
// Section: MPU Implementation
// *****************************************************************************
// *****************************************************************************

void MPU_Initialize(void)
{
    /*** Disable MPU            ***/
    MPU->CTRL = 0;

    /*** Configure MPU Regions  ***/

    /* Region 0 Name: ITCM, Base Address: 0x0, Size:   */
    MPU->RBAR = MPU_REGION(0, 0x0);
    MPU->RASR = MPU_REGION_SIZE(4) | MPU_RASR_AP(MPU_RASR_AP_NOACCESS_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;

    /* Region 1 Name: FLASH, Base Address: 0x0, Size:   */
    MPU->RBAR = MPU_REGION(1, 0x0);
    MPU->RASR = MPU_REGION_SIZE(4) | MPU_RASR_AP(MPU_RASR_AP_NOACCESS_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;

    /* Region 2 Name: DTCM, Base Address: 0x0, Size:   */
    MPU->RBAR = MPU_REGION(2, 0x0);
    MPU->RASR = MPU_REGION_SIZE(4) | MPU_RASR_AP(MPU_RASR_AP_NOACCESS_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;

    /* Region 3 Name: SRAM, Base Address: 0x0, Size:   */
    MPU->RBAR = MPU_REGION(3, 0x0);
    MPU->RASR = MPU_REGION_SIZE(4) | MPU_RASR_AP(MPU_RASR_AP_NOACCESS_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;

    /* Region 4 Name: PERIPHERALS, Base Address: 0x0, Size:   */
    MPU->RBAR = MPU_REGION(4, 0x0);
    MPU->RASR = MPU_REGION_SIZE(4) | MPU_RASR_AP(MPU_RASR_AP_NOACCESS_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;

    /* Region 5 Name: EBI_SMC, Base Address: 0x0, Size:   */
    MPU->RBAR = MPU_REGION(5, 0x0);
    MPU->RASR = MPU_REGION_SIZE(4) | MPU_RASR_AP(MPU_RASR_AP_NOACCESS_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;

    /* Region 6 Name: EBI_SDRAM, Base Address: 0x0, Size:   */
    MPU->RBAR = MPU_REGION(6, 0x0);
    MPU->RASR = MPU_REGION_SIZE(4) | MPU_RASR_AP(MPU_RASR_AP_NOACCESS_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;

    /* Region 7 Name: QSPI, Base Address: 0x0, Size:   */
    MPU->RBAR = MPU_REGION(7, 0x0);
    MPU->RASR = MPU_REGION_SIZE(4) | MPU_RASR_AP(MPU_RASR_AP_NOACCESS_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;

    /* Region 8 Name: USBHS_RAM, Base Address: 0x0, Size:   */
    MPU->RBAR = MPU_REGION(8, 0x0);
    MPU->RASR = MPU_REGION_SIZE(4) | MPU_RASR_AP(MPU_RASR_AP_NOACCESS_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;

    /* Region 9 Name: SYSTEM, Base Address: 0x0, Size:   */
    MPU->RBAR = MPU_REGION(9, 0x0);
    MPU->RASR = MPU_REGION_SIZE(4) | MPU_RASR_AP(MPU_RASR_AP_NOACCESS_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;

    /* Region 10 Name: , Base Address: 0x2045f000, Size: 4KB  */
    MPU->RBAR = MPU_REGION(10, 0x2045f000);
    MPU->RASR = MPU_REGION_SIZE(11) | MPU_RASR_AP(MPU_RASR_AP_READWRITE_Val) | MPU_ATTR_NORMAL \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;






    /* Enable Memory Management Fault */
    SCB->SHCSR |= (SCB_SHCSR_MEMFAULTENA_Msk);

    /* Enable MPU */
    MPU->CTRL = MPU_CTRL_ENABLE_Msk  ;

    __DSB();
    __ISB();
}

