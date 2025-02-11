/*******************************************************************************
  National DP83848 PHY private definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_dp83848_priv.h

  Summary:
    National DP83848 PHY private definitions

  Description:
    This file provides the National DP83848 PHY private definitions.

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

#ifndef H_NAT_DP83848C_PRIV_H_

#define H_NAT_DP83848C_PRIV_H_

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
    // specific vendor registers: 16-31
    PHY_REG_STAT            = 0x10,
    PHY_REG_MII_INT_CTRL    = 0x11,
    PHY_REG_MII_INT_STAT    = 0x12,
    PHY_REG_FALSE_CS_COUNT  = 0x14,
    PHY_REG_RXERR_COUNT     = 0x15,
    PHY_REG_PCS_CONFIG      = 0x16,
    PHY_REG_RMII_BYPASS     = 0x17,
    PHY_REG_LED_CTRL        = 0x18,
    PHY_REG_PHY_CTRL        = 0x19,
    PHY_REG_10BT_CTRL       = 0x1a,
    PHY_REG_TEST_CTRL       = 0x1b,
    PHY_REG_ENERGY_CTRL     = 0x1d,

    //
    //PHY_REGISTERS     = 32    // total number of registers
}DP83848_VENDOR_REG;


// vendor registers
//
typedef union
{
    struct __attribute__((packed))
    {
        unsigned ELAST_BUF    :2;
        unsigned RX_UNF_STS   :1;
        unsigned RX_OVF_STS   :1;
        unsigned RMII_REV1_0  :1;
        unsigned RMII_MODE    :1;
        unsigned              :10;
    };
    unsigned short w;
} DP83848_RMIIBYPASSbits_t;   // reg 0x17: PHY_REG_RMII_BYPASS
#define M_RMIIBYPASS_RMII_MODE_MASK    0x0020U
#define M_RMIIBYPASS_RMII_REV1_0_MASK  0x0010U
#define M_RMIIBYPASS_RX_OVF_STS_MASK   0x0008U
#define M_RMIIBYPASS_RX_UNF_STS_MASK   0x0004U
#define M_RMIIBYPASS_ELAST_BUF_MASK    0x0003U




typedef union
{
    struct __attribute__((packed))
    {
        unsigned PHYADDR      :5;
        unsigned LED_CFG      :2;
        unsigned BP_STRETCH   :1;
        unsigned BIST_START   :1;
        unsigned BIST_STATUS  :1;
        unsigned PSR_15       :1;
        unsigned BIST_FE      :1;
        unsigned PAUSE_TX     :1;
        unsigned PAUSE_RX     :1;
        unsigned FORCE_MDIX   :1;
        unsigned MDIX_EN      :1;
    };
    unsigned short w;
} DP83848_PHYCTRLbits_t;  // reg 0x19: PHY_REG_PHY_CTRL
#define M_PHYCTRL_PHYADDR_MASK      0x001fU
#define M_PHYCTRL_LED_CFG_MASK      0x0060U
#define M_PHYCTRL_BP_STRETCH_MASK   0x0080U
#define M_PHYCTRL_BIST_START_MASK   0x0100U
#define M_PHYCTRL_BIST_STATUS_MASK  0x0200U
#define M_PHYCTRL_PSR_15_MASK       0x0400U
#define M_PHYCTRL_BIST_FE_MASK      0x0800U
#define M_PHYCTRL_PAUSE_TX_MASK     0x1000U
#define M_PHYCTRL_PAUSE_RX_MASK     0x2000U
#define M_PHYCTRL_FORCE_MDIX_MASK   0x4000U
#define M_PHYCTRL_MDIX_EN_MASK      0x8000U






#endif  // H_NAT_DP83848C_PRIV_H_

