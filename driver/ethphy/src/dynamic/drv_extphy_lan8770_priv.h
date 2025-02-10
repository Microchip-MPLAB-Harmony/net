/*******************************************************************************
  LAN8770 private definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_lan8770_priv.h

  Summary:
    LAN8770 private definitions

  Description:
    This file provides the LAN8770 private definitions.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2024-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef H_LAN_8770_PRIV_H_

#define H_LAN_8770_PRIV_H_

typedef enum
{
    /*
    - basic registers, accross all registers: 0-1
    PHY_REG_BMCON       = 0,
    PHY_REG_BMSTAT      = 1,

    - extended registers: 2-15
    PHY_REG_PHYID1      = 2,
    PHY_REG_PHYID2      = 3,
    */
    LAN8770_REG_MS_CTRL             = 9,
    LAN8770_REG_MS_STAT             = 10,

    // specific vendor registers:   16-31
    LAN8770_REG_MDIO_CTRL2          = 16,
    LAN8770_REG_MODE_STAT           = 17,
    LAN8770_REG_EXT_REG_CTRL        = 20,
    LAN8770_REG_EXT_REG_RD_DATA     = 21,
    LAN8770_REG_EXT_REG_WR_DATA     = 22,
    LAN8770_REG_PCS_CTRL            = 23,
    LAN8770_REG_INT_SOURCE          = 24,
    LAN8770_REG_INT_MASK            = 25,
    LAN8770_REG_PWR_DOWN_CTRL       = 26,
    LAN8770_REG_PCS_RX_ERR_CNT_STS  = 30,

}LAN8770_VENDOR_REG;


// extended registers
//

typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned                    : 8;
        unsigned ADV_1000_BASE_T_HD : 1;
        unsigned ADV_1000_BASE_T_FD : 1;
        unsigned PORT_TYPE          : 1;
        unsigned MS_CONFIG_VALUE    : 1;
        unsigned MS_CONFIG_ENABLE   : 1;
        unsigned TEST_MODE          : 3;
    };
}LAN8770_MS_CTRL_bits_t; // reg 9: LAN8770_REG_MS_CTRL


typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned IDLE_ERR_CNT       : 8;
        unsigned                    : 2;
        unsigned LP_1000BASE_T_HD   : 1;
        unsigned LP_1000BASE_T_FD   : 1;
        unsigned REM_RECV_STATUS    : 1;
        unsigned LCL_RECV_STATUS    : 1;
        unsigned MS_CONFIG_RES      : 1;
        unsigned MS_CONFIG_FAULT    : 1;
    };
}LAN8770_REG_MS_STAT_bits_t; // reg 9: LAN8770_REG_MS_STAT

// vendor registers
//


typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned            : 12;
        unsigned SLEEP_REQ  : 1;
        unsigned WAKE_REQ   : 1;
        unsigned            : 2;
    };
}LAN8770_REG_MDIO_CTRL2_bits_t; // reg 16: LAN8770_REG_MDIO_CTRL2


typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned LINK_UP        : 1;
        unsigned                : 2;
        unsigned DSCR_LOCK_STAT : 1;
        unsigned                : 2;
        unsigned ENERGY_STAT    : 1;
        unsigned                : 9;
    };
}LAN8770_REG_MODE_STAT_bits_t; // reg 17: LAN8770_REG_MODE_STAT



typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned REG_ADDRESS    : 8;
        unsigned REG_BANK       : 3;
        unsigned WRITE_CTRL     : 1;
        unsigned READ_CTRL      : 1;
        unsigned                : 3;
    };
}LAN8770_REG_EXT_REG_CTRL_bits_t; // reg 20: LAN8770_REG_EXT_REG_CTRL


typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned READ_DATA  : 16;
    };
}LAN8770_REG_EXT_REG_RD_DATA_bits_t; // reg 21: LAN8770_REG_EXT_REG_RD_DATA

typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned WRITE_DATA  : 16;
    };
}LAN8770_REG_EXT_REG_WR_DATA_bits_t; // reg 22: LAN8770_REG_EXT_REG_WR_DATA

typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned                    : 7;
        unsigned POL_FLIP_MAN_CTRL  : 1;
        unsigned POL_FLIP_TX_CTRL   : 1;
        unsigned POL_FLIP_RX_CTRL   : 1;
        unsigned                    : 6;
    };
}LAN8770_REG_PCS_CTRL_bits_t; // reg 23: LAN8770_REG_PCS_CTRL


typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned ENERGY_ON_INT          : 1;
        unsigned LINK_DOWN_INT          : 1;
        unsigned LINK_UP_INT            : 1;
        unsigned JABBER_DETECT_INT      : 1;
        unsigned NEW_LINK_DETECT_INT    : 1;
        unsigned RECV_LPS_INT           : 1;
        unsigned ENERGY_OFF_INT         : 1;
        unsigned                        : 3;
        unsigned RECV_WAKE_INT          : 1;
        unsigned OVR_TEMP_INT           : 1;
        unsigned                        : 1;
        unsigned BER_TOGGLE             : 1;
        unsigned WAKE_IN_INT            : 1;
        unsigned SW_INT                 : 1;
    };
}LAN8770_REG_INT_SOURCE_bits_t; // reg 24: LAN8770_REG_INT_SOURCE

typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned ENERGY_ON_INT_MASK         : 1;
        unsigned LINK_DOWN_INT_MASK         : 1;
        unsigned LINK_UP_INT_MASK           : 1;
        unsigned JABBER_DETECT_INT_MASK     : 1;
        unsigned NEW_LINK_DETECT_INT_MASK   : 1;
        unsigned RECV_LPS_INT_MASK          : 1;
        unsigned ENERGY_OFF_INT_MASK        : 1;
        unsigned                            : 3;
        unsigned RECV_WAKE_INT_MASK         : 1;
        unsigned OVR_TEMP_INT_MASK          : 1;
        unsigned                            : 1;
        unsigned BER_TOGGLE_MASK            : 1;
        unsigned WAKE_IN_INT_MASK           : 1;
        unsigned SW_INT_MASK                : 1;
    };
}LAN8770_REG_INT_MASK_bits_t; // reg 25: LAN8770_REG_INT_MASK


typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned                    : 8;
        unsigned HW_INIT_SEQ_ENABLE : 1;
        unsigned                    : 7;
    };
}LAN8770_REG_PWR_DOWN_CTRL_bits_t; // reg 26: LAN8770_REG_PWR_DOWN_CTRL


typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned PCS_RX_ERR_CNT : 16;
    };
}LAN8770_REG_PCS_RX_ERR_CNT_STS_bits_t; // reg 30: LAN8770_REG_PCS_RX_ERR_CNT_STS




#endif  // H_LAN_8770_PRIV_H_

