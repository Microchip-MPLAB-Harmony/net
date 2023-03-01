/*******************************************************************************
  MCHP LAN8742A definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_lan8742a.h

  Summary:
    MCHP LAN8742A definitions

  Description:
    This file provides the MCHP LAN8742A definitions.
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2022-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _LAN_8742A_H_

#define _LAN_8742A_H_

#include <stdint.h>

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
    PHY_MMD_ACCESS_CONTROL  = 13,
    PHY_MMD_ACCESS_DATA_ADDR= 14,
    // specific vendor registers: 13-31
    PHY_REG_EDPD_NLP        = 16,
    PHY_REG_MODE_CTRL       = 17,
    PHY_REG_SPECIAL_MODE    = 18,
    PHY_REG_TDR_PATTERN     = 24,
    PHY_REG_TDR_CTRL        = 25,

    PHY_REG_SYMBOL_ERR_CNT  = 26,
    PHY_REG_SPECIAL_CTRL    = 27,
    PHY_REG_CABLE_LEN       = 28,

    PHY_REG_INT_SOURCE      = 29,
    PHY_REG_INT_MASK        = 30,
    PHY_REG_PHY_CTRL        = 31,
    //
    //PHY_REGISTERS     = 32    // total number of registers
}ePHY_VENDOR_REG;
// updated version of ePHY_REG


// vendor registers
//
// PHY_MMD_ACCESS_CONTROL: MMD ACCESS CONTROL REGISTER
typedef union
{
    uint16_t w;
    struct
    {        
        uint16_t MMD_DEV_ADDR:  5;
        uint16_t :  9;
        uint16_t MMD_FUNCTION:    2;
    };
} __MMD_ACCESS_CONTROL_bits_t;  // reg 13: PHY_MMD_ACCESS_CONTROL


// PHY_MMD_ACCESS_DATA_ADDR: MMD ACCESS ADDRESS/DATA REGISTER
typedef union
{
    uint16_t w;
    struct
    {        
        uint16_t MMD_REG_ADDR_DATA:16;
    };
} __MMDACCESS_ADDRESS_DATA_bits_t;  // reg 14: PHY_MMD_ACCESS_DATA_ADDR

// PHY_REG_EDPD_NLP
typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t EXT_MANUAL:    1;
        uint16_t EDPD_EXTEND_CROSSOVER: 1;
        uint16_t :      8;
        uint16_t EDPD_RX_NLP_MAX_INTERVAL: 2;
        uint16_t EDPD_RX_SINGLE_NLP_WAKE: 1;
        uint16_t EDPD_TX_NLP_INTERVAL : 2;
        uint16_t EDPD_TX_NLP_ENABLE : 1;
    };
}__EDPD_NLP_CROSSOVER_TIME_bits_t;  // reg 16:PHY_REG_EDPD_NLP


// PHY_REG_MODE_CTRL
typedef union
{
    uint16_t w;
    struct
    {    
        uint16_t :1;
        uint16_t ENERGYON:1;
        uint16_t :4;
        uint16_t ALTINT:1;
        uint16_t :2;
        uint16_t FAR_LOOPBACK:1;
        uint16_t :3;
        uint16_t EDPWRDOWN:1;
        uint16_t :2;
    };
} __MODECTRLbits_t; // reg 17: PHY_REG_MODE_CTRL

// PHY_REG_SPECIAL_MODE
typedef union
{
    uint16_t w;
    struct
    {    
        uint16_t PHYAD:5;
        uint16_t MODE:3;
        uint16_t :8;
    };
} __SPECIALMODEbits_t;  // reg 18: PHY_REG_SPECIAL_MODE


// PHY_REG_TDR_PATTERN
typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t    PATTERN_LOW: 6;
        uint16_t    PATTERN_HIGH: 6;
        uint16_t    LINE_BREAK_CNT: 3;
        uint16_t    DELAY_IN: 1;
    };
}__TDR_PATTERN_bits_t;   // reg 24: PHY_REG_TDR_PATTERN

// PHY_REG_TDR_CTRL
typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t    CHANNEL_LENGTH: 8;
        uint16_t    CHANNEL_STATUS: 1;
        uint16_t    CHANNEL_CABLE_TYPE: 2;
        uint16_t : 3;
        uint16_t AtoD_FILTER_ENABLE: 1;
        uint16_t    ENABLE: 1;
    };
}__TDR_CONTROL_bits_t;   // reg 25: PHY_REG_TDR_CTRL


// PHY_REG_SYMBOL_ERR_CNT
typedef union
{
    uint16_t w;
    struct
    {
        uint16_t SYM_ERR_CNT:16;
    };
} __SYMBOL_ERR_CNT_bits_t; // reg 26: PHY_REG_SYMBOL_ERR_CNT


// PHY_REG_SPECIAL_CTRL
typedef union
{
    uint16_t w;
    struct
    {    
        uint16_t :4;
        uint16_t XPOL:1;
        uint16_t :6;
        uint16_t SQEOFF:1;
        uint16_t :1;
        uint16_t CH_SELECT:1;
        uint16_t :1;
        uint16_t AMDIXCTRL:1;
    };
} __SPECIAL_CTRL_bits_t;  // reg 27: PHY_REG_SPECIAL_CTRL

#define _SPECIALCTRL_XPOL_MASK      0x0010
#define _SPECIALCTRL_SQEOFF_MASK    0x0800
#define _SPECIALCTRL_CH_SELECT_MASK 0x2000    // Manual MDIX
#define _SPECIALCTRL_AMDIXCTRL_MASK 0x8000

// PHY_REG_CABLE_LEN
typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t :12;
        uint16_t CABLE_LEN: 4;
    };
}__PHY_REG_CABLE_LEN_bits_t;   // reg 28: PHY_REG_CABLE_LEN

// PHY_REG_INT_SOURCE
typedef union
{
    uint16_t w;
    struct
    {    
        uint16_t :1;
        uint16_t INT1:1;
        uint16_t INT2:1;
        uint16_t INT3:1;
        uint16_t INT4:1;
        uint16_t INT5:1;
        uint16_t INT6:1;
        uint16_t INT7:1;
        uint16_t INT8:1;
        uint16_t :7;
    };
} __INT_SOURCE_bits_t;    // reg 29: PHY_REG_INT_SOURCE

// PHY_REG_INT_MASK
typedef union
{
    uint16_t w;
    struct
    {
        uint16_t :1;
        uint16_t INT1:1;
        uint16_t INT2:1;
        uint16_t INT3:1;
        uint16_t INT4:1;
        uint16_t INT5:1;
        uint16_t INT6:1;
        uint16_t INT7:1;
        uint16_t INT8:1;
        uint16_t :7;
    };
} __INT_MASK_bits_t;  // reg 30: PHY_REG_INT_MASK

#define WOL_INT8_EN 0x0100

// PHY_REG_PHY_CTRL
typedef union
{
    uint16_t w;
    struct
    {        
        uint16_t :2;
        uint16_t SPEED:3;   // 1: 10MbpsHD; 5:10MbpsFD; 2: 100MbpsHD; 6: 100MbpsFD; 
        uint16_t :7;
        uint16_t AUTODONE:1;
        uint16_t :3;
    };
} __PHY_CTRL_bits_t;  // reg 31: PHY_REG_PHY_CTRL

#define _PHYCTRL_SPEED_MASK     0x001c
#define _PHYCTRL_SPEED_FDUPLX_MASK  0x0010
#define _PHYCTRL_SPEED_100_10_MASK  0x000c
#define _PHYCTRL_SPEED_100_MASK     0x0008
#define _PHYCTRL_AUTODONE_MASK      0x1000


// WUCSR: Wake up control and Status Register
typedef union
{
    uint16_t w;
    struct
    {
        uint16_t BCST_EN:1;
        uint16_t MPEN:1;
        uint16_t WUEN:1;
        uint16_t PFDA_EN:1;
        uint16_t BCAST_FR:1;
        uint16_t MPR:1;
        uint16_t WUFR:1;
        uint16_t PFDA_FR:1;
        uint16_t WOL_CONFIGURED:1;
        uint16_t NPME_SELF_CLEAR:1;
        uint16_t :1;
        uint16_t LED2_SELECT:2;
        uint16_t LED1_SELECT:2;
        uint16_t INTERFACE_DISABLE:1;
    };
} __WUCSR_bits_t; // reg 3.32784: WUCSR 


// Wake up Filter Configuration Register WUF_CFGA
typedef union
{
    uint16_t w;
    struct
    {
        uint16_t FILTER_OFFSET:8;
        uint16_t FILTER_BROADCAST_EN:1;
        uint16_t FILTER_MULTICAST_EN:1;
        uint16_t FILTER_MATCH_EN:1;
        uint16_t :3;
        uint16_t FILTER_TRIGGERED:1;
        uint16_t FILTER_EN:1;
    };
} __WUF_CFGA_bits_t;   // Index 3.32785: WUF_CFGA 

#define _WOL_WAKEUP_FILTER_ENABLE           0x8000
#define _WOL_WAKEUP_FILTER_TRIGGERED        0x4000
#define _WOL_WAKEUP_ADDRESS_MATCH_ENA       0x0400
#define _WOL_WAKEUP_MULTICAST_MATCH_ENA     0x0200
#define _WOL_WAKEUP_BROADCAST_ENA           0x0100
#define _WOL_WAKEUP_PATTERN_OFFSET_MASK     0x00FF

typedef enum
{
    UNICAST_ADDRESS_MATCH=_WOL_WAKEUP_ADDRESS_MATCH_ENA,
    MULTICAST_ADDRESS_MATCH=_WOL_WAKEUP_MULTICAST_MATCH_ENA,
    BROADCAST_ADDRESS_MATCH=_WOL_WAKEUP_BROADCAST_ENA,
    WAKEUP_FILTER_NONE=0,
}eWOL_WAKEUP_FILTER;

typedef enum
{
    WOL_WAKEUP_CONTROL_STATUS=32784,
    WOL_WAKEUP_FILTER_REGISTER_A=32785,
    WOL_WAKEUP_FILTER_CRC_REGISTER_B=32786,
    WOL_WAKEUP_FILTER_BYTEMASK_8=32801,
    WOL_WAKEUP_FILTER_BYTEMASK_7=32802,
    WOL_WAKEUP_FILTER_BYTEMASK_6=32803,
    WOL_WAKEUP_FILTER_BYTEMASK_5=32804,
    WOL_WAKEUP_FILTER_BYTEMASK_4=32805,
    WOL_WAKEUP_FILTER_BYTEMASK_3=32806,
    WOL_WAKEUP_FILTER_BYTEMASK_2=32807,
    WOL_WAKEUP_FILTER_BYTEMASK_1=32808,
    WOL_MAC_ADDRESS_A=32865,
    WOL_MAC_ADDRESS_B=32866,
    WOL_MAC_ADDRESS_C=32867,

}eWOL_MMD_INDEXNO;

typedef enum
{
    PERFECT_DA_FRAME=0,
    REMOTE_WAKEUP_FRAME,
    MAGIC_PKT_RECEIVED,
    BROADCAST_FRAME_RCVD,
}eWOL_WAKEUP_FRAME;

typedef enum
{
    WOL_INT_ENABLED=1,
    WOL_INT_DISABLED,
}eWOLIntSatus;

typedef enum
{
    WOL_WAKEUP_FILTER_ENA=1,
    WOL_WAKEUP_FILTER_DIS,
}eWOLWakeupFilterSatus;


#define WOl_CRC_POLY 0x8005 /* s/b international standard for CRC-16 */

#define WOL_WAKEUP_FRAME_BYTEMASK_SIZE 8
// User configuration for WOL Wake up frame Part
#define WOL_WAKEUP_FRAME_START_OFFSET   0x40

// WOL_WAKEUP_FRAME_PATTERN_LEN is the size of  WOL_WAKEUP_FRAME_PATTERN . 
// So both WOL_WAKEUP_FRAME_PATTERN_LEN and WOL_WAKEUP_FRAME_PATTERN are 
// related to each other
#define WOL_WAKEUP_FRAME_PATTERN_LEN 4
#define WOL_WAKEUP_FRAME_PATTERN  {0x01,0x01,0x01,0x01}

// WOL wake up frame byte mask values need to be provided here
//The byte mask is a 128-bit field that specifies whether or not each of the 128
//contiguous bytes within the frame, beginning with the pattern offset, should be checked.
//If bit j in the byte mask is set, the detection logic checks the byte (pattern offset + j) in the
//frame, otherwise byte (pattern offset + j) is ignored.

#define WOL_WAKEUP_BYTEMASK_REG0 0x000F
#define WOL_WAKEUP_BYTEMASK_REG1 0x0000
#define WOL_WAKEUP_BYTEMASK_REG2 0x0000
#define WOL_WAKEUP_BYTEMASK_REG3 0x0000
#define WOL_WAKEUP_BYTEMASK_REG4 0x0000
#define WOL_WAKEUP_BYTEMASK_REG5 0x0000
#define WOL_WAKEUP_BYTEMASK_REG6 0x0000
#define WOL_WAKEUP_BYTEMASK_REG7 0x0000

#endif  // _LAN_8742A_H_

