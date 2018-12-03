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
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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

    /* Region 6 Name: GMAC Descriptor, Base Address: 0x2045f000, Size: 4KB  */
    MPU->RBAR = MPU_REGION(6, 0x2045f000);
    MPU->RASR = MPU_REGION_SIZE(11) | MPU_RASR_AP(MPU_RASR_AP_READWRITE_Val) | MPU_ATTR_NORMAL \
                | MPU_ATTR_ENABLE | MPU_ATTR_EXECUTE_NEVER ;










    /* Enable Memory Management Fault */
    SCB->SHCSR |= (SCB_SHCSR_MEMFAULTENA_Msk);

    /* Enable MPU */
    MPU->CTRL = MPU_CTRL_ENABLE_Msk  | MPU_CTRL_PRIVDEFENA_Msk;

    __DSB();
    __ISB();
}

