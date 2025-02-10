/*******************************************************************************
  KSZ9031 private definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_ksz9131_priv.h

  Summary:
    KSZ9131 private definitions

  Description:
    This file provides the KSZ9131 private definitions.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2022-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef H_KSZ9131_PRIV_H_

#define H_KSZ9131_PRIV_H_

typedef enum
{
    /*
    - basic registers, accross all registers: 0-1
    PHY_REG_BMCON       = 0,
    PHY_REG_BMSTAT      = 1,
    - extended registers: 2-15
    PHY_REG_PHYID1      = 2,
    PHY_REG_PHYID2      = 3,
    PHY_REG_ANAD        = 4,
    PHY_REG_ANLPAD      = 5,
    PHY_REG_ANLPADNP    = 5,
    PHY_REG_ANEXP       = 6,
    PHY_REG_ANNPTR      = 7,
    PHY_REG_ANLPRNP     = 8,
    */    
    PHY_MMD_ACCESS_CONTROL = 13,
    PHY_MMD_ACCESS_DATA_ADDR = 14,
    // specific vendor registers: 16-31
    PHY_REG_RESERVED    = 16,
    PHY_REG_REMOTE_LPBK = 17,
    PHY_REG_LINKMD_DIAG = 18,
    PHY_REG_DIG_PMA = 19,
    PHY_REG_RX_ERR_CNT  = 21,
    PHY_REG_LED_MODE    = 22,
    PHY_REG_LED_BEHV    = 23,
    PHY_REG_MDIO_DRV    = 25,
    PHY_REG_KSZ9031_LED_MODE    = 26,
    PHY_REG_INT_CTRL    = 27,
    PHY_REG_AUTO_MDIX   = 28,
    PHY_REG_SOFT_POWER_DOWN = 29,
    PHY_REG_EXT_LPBK    = 30,
    PHY_REG_PHY_CTRL    = 31,
    //
    //PHY_REGISTERS     = 32    // total number of registers
}KSZ9131_VENDOR_REG;



// MMD ( MDIO Managable Devices )
typedef union 
{
    struct __attribute__((packed))
    {        
        unsigned MMD_DEV_ADDR:5;
        unsigned :9;
        unsigned MMD_FUNCTION:2;
    };
    unsigned short w;
} KSZ9131_MMDACCESSCONTROLbits_t; // Index 13: 

typedef enum
{   // MMD Register under Device Address 2
    PHY_MMD_CLK_SKEW_REG = 8,
    PHY_MMD_TX_DLL_REG   = 77,
}KSZ9131_PHY_MMD_DEV2_REG;

#define M_PHY_MMD_CNTL_ACCESS_ADDRESS_MASK  0x0000U // 15 and 14th is 00
#define M_PHY_MMD_CNTL_ACCESS_DATA_MASK     0x4000U // 15 and 14 th is 01
#define M_PHY_MMD_RESERVED_MASK             0x3FE0U


#endif  // H_KSZ9131_PRIV_H_

