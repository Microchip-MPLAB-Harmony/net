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

    /* Region 0 Name: ITCM, Base Address: 0x0, Size: 4MB  */
    MPU->RBAR = MPU_REGION(0, 0x0);
    MPU->RASR = MPU_REGION_SIZE(21) | MPU_RASR_AP(MPU_RASR_AP_READWRITE_Val) | MPU_ATTR_NORMAL \
                | MPU_ATTR_ENABLE  ;

    /* Region 1 Name: DTCM, Base Address: 0x20000000, Size: 4MB  */
    MPU->RBAR = MPU_REGION(1, 0x20000000);
    MPU->RASR = MPU_REGION_SIZE(21) | MPU_RASR_AP(MPU_RASR_AP_READWRITE_Val) | MPU_ATTR_NORMAL \
                | MPU_ATTR_ENABLE  ;

    /* Region 2 Name: SRAM, Base Address: 0x20400000, Size: 8MB  */
    MPU->RBAR = MPU_REGION(2, 0x20400000);
    MPU->RASR = MPU_REGION_SIZE(22) | MPU_RASR_AP(MPU_RASR_AP_READWRITE_Val) | MPU_ATTR_NORMAL_WB_WA \
                | MPU_ATTR_ENABLE  ;

    /* Region 3 Name: EBI_SMC, Base Address: 0x60000000, Size: 256MB  */
    MPU->RBAR = MPU_REGION(3, 0x60000000);
    MPU->RASR = MPU_REGION_SIZE(27) | MPU_RASR_AP(MPU_RASR_AP_READWRITE_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE  ;

    /* Region 4 Name: EBI_SDRAM, Base Address: 0x70000000, Size: 256MB  */
    MPU->RBAR = MPU_REGION(4, 0x70000000);
    MPU->RASR = MPU_REGION_SIZE(27) | MPU_RASR_AP(MPU_RASR_AP_READWRITE_Val) | MPU_ATTR_DEVICE \
                | MPU_ATTR_ENABLE  ;

    /* Region 5 Name: QSPI, Base Address: 0x80000000, Size: 256MB  */
    MPU->RBAR = MPU_REGION(5, 0x80000000);
    MPU->RASR = MPU_REGION_SIZE(27) | MPU_RASR_AP(MPU_RASR_AP_READWRITE_Val) | MPU_ATTR_STRONGLY_ORDERED \
                | MPU_ATTR_ENABLE  ;





    /* Region 10 Name: , Base Address: 0x2045f000, Size: 4KB  */
    MPU->RBAR = MPU_REGION(10, 0x2045f000);
    MPU->RASR = MPU_REGION_SIZE(11) | MPU_RASR_AP(MPU_RASR_AP_READWRITE_Val) | MPU_ATTR_NORMAL \
                | MPU_ATTR_ENABLE  ;






    /* Enable Memory Management Fault */
    SCB->SHCSR |= (SCB_SHCSR_MEMFAULTENA_Msk);

    /* Enable MPU */
    MPU->CTRL = MPU_CTRL_ENABLE_Msk  | MPU_CTRL_PRIVDEFENA_Msk;

    __DSB();
    __ISB();
}

