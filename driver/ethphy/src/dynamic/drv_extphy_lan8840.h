/*******************************************************************************
  LAN8840 definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_LAN8840.h

  Summary:
    LAN8840 definitions

  Description:
    This file provides the LAN8840 definitions.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _LAN8840_H_

#define _LAN8840_H_

typedef enum
{
    /*
    // basic registers, accross all registers: 0-1
    PHY_REG_BMCON       = 0,
    PHY_REG_BMSTAT      = 1,
    // extended registers: 2-15
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
    PHY_REG_LAN8840_LED_MODE    = 26,
    PHY_REG_INT_CTRL    = 27,
    PHY_REG_AUTO_MDIX   = 28,
    PHY_REG_SOFT_POWER_DOWN = 29,
    PHY_REG_EXT_LPBK    = 30,
    PHY_REG_PHY_CTRL    = 31,
    //
    //PHY_REGISTERS     = 32    // total number of registers
}LAN8840_VENDOR_REG;



// MMD ( MDIO Managable Devices )
typedef union {
  struct {        
    unsigned MMD_DEV_ADDR:5;
    unsigned :9;
    unsigned MMD_FUNCTION:2;
  };
  struct {
    unsigned short w:16;
  };
} __MMDACCESSCONTROLbits_t; // Index 13: 

typedef enum
{   // MMD Register under Device Address 2
    PHY_MMD_CTL_SKEW_REG = 4,
    PHY_MMD_RGMII_RX_DATA_SKEW_REG = 5,
    PHY_MMD_RGMII_TX_DATA_SKEW_REG = 6,            
    PHY_MMD_CLK_SKEW_REG = 8,
    PHY_MMD_TX_DLL_REG   = 77,
}PHY_MMD_DEV2_REG;

#define M_PHY_MMD_CNTL_ACCESS_ADDRESS_MASK  0x0000 // 15 and 14th is 00
#define M_PHY_MMD_CNTL_ACCESS_DATA_MASK  0x4000 // 15 and 14 th is 01
#define M_PHY_MMD_RESERVED_MASK 0x3FE0

#define PHY_MMD_DEVICE_ADDRESS 0x02

#define PHY_MMD_TX_CLK_SKEW_POS 5U
#define PHY_MMD_RX_CTL_SKEW_POS 4U
#define PHY_MMD_RXD3_PAD_SKEW_POS 12U
#define PHY_MMD_RXD2_PAD_SKEW_POS 8U
#define PHY_MMD_RXD1_PAD_SKEW_POS 4U
#define PHY_MMD_TXD3_PAD_SKEW_POS 12U
#define PHY_MMD_TXD2_PAD_SKEW_POS 8U
#define PHY_MMD_TXD1_PAD_SKEW_POS 4U

#define PHY_MMD_TX_CLK_SKEW_MASK 0x03E0
#define PHY_MMD_RX_CLK_SKEW_MASK 0x001F
#define PHY_MMD_RX_CTL_SKEW_MASK 0x00F0
#define PHY_MMD_TX_CTL_SKEW_MASK 0x000F
#define PHY_MMD_RXD3_PAD_SKEW_MASK 0xF000
#define PHY_MMD_RXD2_PAD_SKEW_MASK 0x0F00
#define PHY_MMD_RXD1_PAD_SKEW_MASK 0x00F0
#define PHY_MMD_RXD0_PAD_SKEW_MASK 0x000F
#define PHY_MMD_TXD3_PAD_SKEW_MASK 0xF000
#define PHY_MMD_TXD2_PAD_SKEW_MASK 0x0F00
#define PHY_MMD_TXD1_PAD_SKEW_MASK 0x00F0
#define PHY_MMD_TXD0_PAD_SKEW_MASK 0x000F


typedef enum
{
    TX_CLK_SKEW_READ = 0,
            
    TX_CLK_SKEW_WRITE,

    RX_CLK_SKEW_READ,
            
    RX_CLK_SKEW_WRITE,
            
    RX_CTL_SKEW_READ,
            
    RX_CTL_SKEW_WRITE,
       
    TX_CTL_SKEW_READ,
            
    TX_CTL_SKEW_WRITE,            
            
    RXD3_SKEW_READ,
            
    RXD3_SKEW_WRITE,
            
    RXD2_SKEW_READ,
            
    RXD2_SKEW_WRITE,
            
    RXD1_SKEW_READ,
            
    RXD1_SKEW_WRITE,
            
    RXD0_SKEW_READ,
            
    RXD0_SKEW_WRITE,
            
    TXD3_SKEW_READ,
            
    TXD3_SKEW_WRITE,
            
    TXD2_SKEW_READ,
            
    TXD2_SKEW_WRITE,
            
    TXD1_SKEW_READ,
            
    TXD1_SKEW_WRITE,
            
    TXD0_SKEW_READ,
            
    TXD0_SKEW_WRITE,          
                   
} SKEW_SET_STATES;

#endif  // _LAN8840_H_

