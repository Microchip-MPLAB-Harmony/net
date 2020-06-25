/*******************************************************************************
  PIC32MZW1 PMU MLDO TRIMMING

  File Name:
    sys_pmu_mldo_trim.c

  Summary:
    PIC32MZW1 boot time PMU MLDO mode Configuration.

  Description:
    This interface helps configure the PMU in MLDO only mode and also trim
    the voltages in this mode to the operating range.
    
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2019 released Microchip Technology Inc.  All rights reserved.

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
#include "definitions.h"                // SYS function prototypes


/*Bits [15-6] in the SPI Status register have Read data */
#define PMU_SPI_READ_MASK 0xFFFF0000

// PMU related defines
#define BUCKCFG1_ADDR 0x14
#define BUCKCFG2_ADDR 0x15
#define MLDOCFG1_ADDR 0x17
#define MLDOCFG2_ADDR 0x18
#define SPI_CMD_OFFSET 24
#define SPI_ADDR_OFFSET 16
#define DC_DC_MASK 0xFFFFC3FF
#define BUCK_SLOPE_MASK 0xFFFFFF8F
#define MLDO_VTUNE_MASK 0xFFFFFC3F
#define PLDO1_MASK 0xFFFFFFE0
#define PLDO2_MASK 0xFFFFE0FF
#define PLDO3_MASK 0xFFE0FFFF
#define VREG_MASK 0xE0FFFFFF
//Overctrl Register
#define OVERRIDE_EN 0x800000
#define OMLDO_EN 0x40000000
#define OBUCKEN_EN 0x80000000

#define PHWC_MASK 0xFFBFFFFF
#define PMU_MLDO_ILEAK_CFG  0x00000038
#define PMU_MLDO_ISENS_CFG  0x00000007
#define MLDO_EN 0x00000800
#define MLDO_HW_BP_EN 0x4

//Flash Area housing the PMU calibration values 
unsigned int *pmu_buckcfg1_data = (unsigned int *)0xBFC56FE8;
unsigned int *pmu_buckcfg2_data = (unsigned int *) 0xBFC56FEC;
unsigned int *pmu_mldocfg1_data = (unsigned int *) 0xBFC56FF4;
unsigned int *pmu_mldocfg2_data = (unsigned int *) 0xBFC56FF8;
unsigned int *pmu_treg3_data = (unsigned int *)0xBFC56FFC;

unsigned int *spi_status_reg = (unsigned int *)0xBF813E04;
unsigned int *spi_cntrl_reg = (unsigned int *)0xBF813E00;
unsigned int *pmu_cmode_reg = (unsigned int *)0xBF813E20;


#define SPI_WR_DELAY() { int delay = 0x10000; while (delay--) ;}

static unsigned int SYS_PMU_SPI_READ(unsigned int spi_addr)
{
    unsigned int spi_val , reg_val;
    int status = 0;
    reg_val = (1 << SPI_CMD_OFFSET) | (spi_addr << SPI_ADDR_OFFSET) ;
    *spi_cntrl_reg = reg_val;
    SPI_WR_DELAY();
   
    while (1)
    {
        status = *spi_status_reg;
        if (status & 0x80)
            break;
    }
    spi_val = ((status & PMU_SPI_READ_MASK) >> SPI_ADDR_OFFSET);
    SPI_WR_DELAY();
    return spi_val;
}

static void SYS_PMU_SPI_WRITE(unsigned int spi_addr, unsigned int reg_val)
{
    unsigned int spi_val ;
    int status = 0;
    reg_val &= 0xFFFF;
    spi_val = (spi_addr << SPI_ADDR_OFFSET) | reg_val;
    *spi_cntrl_reg = spi_val;
    SPI_WR_DELAY();
    while (1)
    {
        status = *spi_status_reg;
        if (status & 0x80)
            break;
    }  
    SPI_WR_DELAY();
}

/*This function will configure the PMU with
 *the tune bits from Flash. 
 * 
 * Flash area to read from
 * 0xBFC56FE0	BLANK 
 * 0xBFC56FE4	BLANK
 * 0xBFC56FE8	BUCKCFG1 (vo_tune in bits [13:10] other bits written low
 * 0xBFC56FEC	BUCKCFG2 (default buk_curve value written to bits [6:4], other bits written low.
 * 0xBFC56FF0	BUCKCFG3 (currently BLANK, set aside for possible future use)
 * 0xBFC56FF4	MLDOCFG1 (currently BLANK until we receive MLDO trim pattern and implement calibration on ATE)
 * 0xBFC56FF8	MLDOCFG2 (currently BLANK until we receive MLDO trim pattern and implement calibration on ATE)
 * 0xBFC56FFC	TREG3 (VREG4,3,2,1) values
 *  
 * Below are the configurations done here
 * Configure buk_Vo_tune<13:10> in BUCKCFG1, ADDR=0x14 register
 * Configure mldo_vtun<9:6> in the MLDOCFG1, ADDR=0x17 register
 * Configure PLDO1[4:0] in the TREG3 register
 * Configure PLDO2[12:8] in the TREG3 register
 * Configure PLDO3[20:16] in the TREG3 register
 * Configure VREGPLL1[28:24] in the TREG3 register
 */

void SYS_PMU_MLDO_TRIM(void)
{
    unsigned int nvm_flash_data, pmu_register_data;
    unsigned int mldocfg1, mldocfg2, buckcfg0;

    unsigned int *override_ctrl = (unsigned int *) 0xBF813E1C;
    
        PMUCLKCTRL = 0x1508A;
        
        //Read MLDO Value
        mldocfg1 = SYS_PMU_SPI_READ(MLDOCFG1_ADDR);
    
        //Modify Value
        nvm_flash_data = (((*pmu_mldocfg1_data) & ~MLDO_VTUNE_MASK));

        //  printf("MLDO vtune value read from Flash is 0x%x\n",nvm_flash_data >> 6);
        //  printf("MLDOCFG1  value is 0x%x\n", mldocfg1);
        if((nvm_flash_data == 0x00000000) || (nvm_flash_data == 0xFFFFFFFF))
            nvm_flash_data = 0x5 << 6;
        else
            nvm_flash_data |= (PMU_MLDO_ILEAK_CFG | PMU_MLDO_ISENS_CFG);

        mldocfg1 = nvm_flash_data;
        mldocfg2 = SYS_PMU_SPI_READ(MLDOCFG2_ADDR);
        //  printf("MLDOCFG2  value is 0x%x\n", mldocfg2);
        
        nvm_flash_data = (*pmu_mldocfg2_data);
        mldocfg2 = nvm_flash_data;
        
        buckcfg0 = SYS_PMU_SPI_READ(BUCKCFG1_ADDR);
        
        //Writeback
        SYS_PMU_SPI_WRITE(MLDOCFG1_ADDR, mldocfg1);

        mldocfg2 |= MLDO_EN;
        SYS_PMU_SPI_WRITE(MLDOCFG2_ADDR, mldocfg2);
        
        buckcfg0 |= MLDO_HW_BP_EN;
        SYS_PMU_SPI_WRITE(BUCKCFG1_ADDR, buckcfg0);                

        //Configure PLDO1
        pmu_register_data = *override_ctrl & PLDO1_MASK;
        //Modify Value
        nvm_flash_data = *pmu_treg3_data & ~PLDO1_MASK;
        //  printf("VREG4OCTRL value read from Flash is 0x%x\n",nvm_flash_data);

        if((nvm_flash_data > 0x17) || (nvm_flash_data < 0xA))
            nvm_flash_data = 0x16;

        pmu_register_data = (pmu_register_data | nvm_flash_data);
        //Write back
        *override_ctrl = pmu_register_data;

         //Configure PLDO2
        pmu_register_data = *override_ctrl & PLDO2_MASK;
        //Modify Value
        nvm_flash_data = *pmu_treg3_data & ~PLDO2_MASK;

        //  printf("VREG3OCTRL value read from Flash is 0x%x\n",nvm_flash_data >> 8);

        if(((nvm_flash_data >> 8) > 0x18) || ((nvm_flash_data >> 8)< 0xA))
            nvm_flash_data = 0x16 << 8;

        //   nvm_flash_data = nvm_flash_data << 8;
        pmu_register_data = (pmu_register_data | nvm_flash_data);
        //Write back
        *override_ctrl = pmu_register_data;

         //Configure PLDO3
        pmu_register_data = *override_ctrl & PLDO3_MASK;
        //Modify Value
        nvm_flash_data = *pmu_treg3_data & ~PLDO3_MASK;
        //   nvm_flash_data = nvm_flash_data >> 16;

     //   printf("VREG2OCTRL value read from Flash is 0x%x\n",nvm_flash_data >> 16);
        if(((nvm_flash_data >> 16) > 0x18) || ((nvm_flash_data >> 16)< 0xA))
            nvm_flash_data = 0x16 << 16;

        //    nvm_flash_data = nvm_flash_data << 16;
        pmu_register_data = (pmu_register_data | nvm_flash_data);
        //Write back
        *override_ctrl = pmu_register_data;

         //Configure VREGPLL1
        pmu_register_data = *override_ctrl & VREG_MASK;
        //Modify Value
        nvm_flash_data = *pmu_treg3_data & ~VREG_MASK;
        //   nvm_flash_data = nvm_flash_data >> 24;

      //  printf("VREG1OCTRL value read from Flash is 0x%x\n",nvm_flash_data >> 24);

        if(((nvm_flash_data >> 24) > 0x18) || ((nvm_flash_data >> 24)< 0xA))
            nvm_flash_data = 0x16 << 24;
        //   nvm_flash_data = nvm_flash_data << 24;
        pmu_register_data = (pmu_register_data | nvm_flash_data);
        //Write back
        *override_ctrl = pmu_register_data;
     
        pmu_register_data = *override_ctrl & ~OBUCKEN_EN; 
        *override_ctrl = pmu_register_data;
        
        pmu_register_data = *override_ctrl; 
        pmu_register_data |= OMLDO_EN;

        pmu_register_data = *override_ctrl | OVERRIDE_EN;
        pmu_register_data &= PHWC_MASK;
        *override_ctrl = pmu_register_data;
      //  printf("Reading back programmed values from PMU OVERCTRL register\n");
      //  printf("PMU OVERCTRL value is 0x%x\n", *override_ctrl);
      //  printf("MLDOCFG1  value is 0x%x\n", pmu_spi_read(MLDOCFG1_ADDR));
      //  printf("MLDOCFG2  value is 0x%x\n", pmu_spi_read(MLDOCFG2_ADDR));
      //  printf("BUCKCFG1  value is 0x%x\n", pmu_spi_read(BUCKCFG1_ADDR));
    
}