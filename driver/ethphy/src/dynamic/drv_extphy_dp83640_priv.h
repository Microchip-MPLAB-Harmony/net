/*******************************************************************************
  National 83640 private definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_dp83640_priv.h

  Summary:
    National 83640 private definitions

  Description:
    This file provides the National 83640 private definitions.
    
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

#ifndef H_NAT_DP83640_PRIV_H_

#define H_NAT_DP83640_PRIV_H_

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
    - PHY_REG_ANLPRNP      = 8, not on 83640
    */

    // specific vendor registers: 16-31
    PHY_REG_STS             = 0x10,
    PHY_REG_MII_INT_CTRL    = 0x11,
    PHY_REG_MII_INT_STAT    = 0x12,
    PHY_REG_PAGESEL         = 0x13,     // extended register accessed by page selection register

    // extended registers - page 0
    PHY_REG_FALSE_CS_COUNT  = 0x14,
    PHY_REG_RXERR_COUNT     = 0x15,
    PHY_REG_PCS_CONFIG      = 0x16,
    PHY_REG_RMII_BYPASS     = 0x17,
    PHY_REG_LED_CTRL        = 0x18,
    PHY_REG_PHY_CTRL        = 0x19,
    PHY_REG_10BT_CTRL       = 0x1a,
    PHY_REG_TEST_CTRL       = 0x1b,
    PHY_REG_ENERGY_CTRL     = 0x1d,
    PHY_REG_PCFCR           = 0x1F,

    // test registers - page 1
    PHY_REG_TEST_SD_CNFG    = 0x1E,

    // link diagnostics registers - page 2
    PHY_REG_LEN100_DET  = 0x14,
    PHY_REG_FREQ100     = 0x15,
    PHY_REG_TDR_CTRL    = 0x16,
    PHY_REG_TDR_WIN     = 0x17,
    PHY_REG_TDR_PEAK    = 0x18,
    PHY_REG_TDR_THR     = 0x19,
    PHY_REG_VAR_CTRL    = 0x1A,
    PHY_REG_VAR_DAT     = 0x1B,
    PHY_REG_LQMR        = 0x1D,
    PHY_REG_LQDR        = 0x1E,
    PHY_REG_LQMR2       = 0x1F,

    // PTP 1588 base registers - page 4
    PHY_REG_PTP_DTL     = 0x14,
    PHY_REG_PTP_TDR     = 0x15,
    PHY_REG_PTP_STS     = 0x16,
    PHY_REG_PTP_TSTS    = 0x17,
    PHY_REG_PTP_RATEL   = 0x18,
    PHY_REG_PTP_RATEH   = 0x19,
    PHY_REG_PTP_RDCKSUM = 0x1A,
    PHY_REG_PTP_WRCKSUM = 0x1B,
    PHY_REG_PTP_TXTS    = 0x1C,
    PHY_REG_PTP_RXTS    = 0x1D,
    PHY_REG_PTP_ESTS    = 0x1E,
    PHY_REG_PTP_EDATA   = 0x1F,

    // PTP 1588 Configuration registers - Page 5
    PHY_REG_PTP_TRIG    = 0x14,
    PHY_REG_PTP_EVNT    = 0x15,
    PHY_REG_PTP_TXCFG0  = 0x16,
    PHY_REG_PTP_TXCFG1  = 0x17,
    PHY_REG_PSF_CFG0    = 0x18,
    PHY_REG_PTP_RXCFG0  = 0x19,
    PHY_REG_PTP_RXCFG1  = 0x1A,
    PHY_REG_PTP_RXCFG2  = 0x1B,
    PHY_REG_PTP_RXCFG3  = 0x1C,
    PHY_REG_PTP_RXCFG4  = 0x1D,
    PHY_REG_PTP_TRDL    = 0x1E,
    PHY_REG_PTP_TRDH    = 0x1F,

    // PTP 1588 Configuration Registers - Page 6
    PHY_REG_PTP_COC     = 0x14,
    PHY_REG_PSF_CFG1    = 0x15,
    PHY_REG_PSF_CFG2    = 0x16,
    PHY_REG_PSF_CFG3    = 0x17,
    PHY_REG_PSF_CFG4    = 0x18,
    PHY_REG_PTP_SFDCFG  = 0x19,
    PHY_REG_PTP_INTCTL  = 0x1A,
    PHY_REG_PTP_CLKSRC  = 0x1B,
    PHY_REG_PTP_ETR     = 0x1C,
    PHY_REG_PTP_OFF     = 0x1D,
    PHY_REG_PTP_GPIOMON = 0x1E,
    PHY_REG_PTP_RXHASH  = 0x1F,

    //
    //PHY_REGISTERS     = 32    // total number of registers
}DP83640_VENDOR_REG;


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
        unsigned SCMII_TX     :1;
        unsigned SCMII_RX     :1;
        unsigned PMD_LOOP     :1;
        unsigned              :4;
        unsigned DIS_TX_OPT   :1;
        unsigned RMII_MASTER  :1;
        unsigned              :1;
    };
    unsigned short w;
} DP83640_RMIIBYPASSbits_t;   // reg 0x17: PHY_REG_RMII_BYPASS
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
} DP83640_PHYCTRLbits_t;  // reg 0x19: PHY_REG_PHY_CTRL
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


#endif  // H_NAT_DP83640_PRIV_H_

