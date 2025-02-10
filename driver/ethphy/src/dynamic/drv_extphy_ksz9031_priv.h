/*******************************************************************************
  KSZ9031 private definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_ksz9031_priv.h

  Summary:
    KSZ9031 private definitions

  Description:
    This file provides the KSZ9031 private definitions.

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

#ifndef H_KSZ9031_PRIV_H_

#define H_KSZ9031_PRIV_H_

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
    PHY_REG_SILICON_REV = 16,
    PHY_REG_MODE_CTRL   = 17,
    PHY_REG_SPECIAL_MODE    = 18,
    PHY_REG_SYMBOL_ERR_CNT  = 26,
    PHY_REG_SPECIAL_CTRL    = 27,
    PHY_REG_INT_SOURCE  = 29,
    PHY_REG_INT_MASK    = 30,
    PHY_REG_PHY_CTRL    = 31,   
    //
    //PHY_REGISTERS     = 32    // total number of registers
}KSZ9031_VENDOR_REG;


// vendor registers
//
typedef union 
{
    struct __attribute__((packed))
    {    
        unsigned :6;
        unsigned SILICON_REV:4;
        unsigned :6;
    };
    unsigned short w;
} KSZ9031_SILICONREVbits_t;   // reg 16: PHY_REG_SILICON_REV 
#define M_SILICONREV_SILICON_REV_MASK        0x03c0U


typedef union 
{
    struct __attribute__((packed))
    {    
        unsigned :1;
        unsigned ENERGYON:1;
        unsigned FORCE_GOOD_LINK:1;
        unsigned PHYADPB:1;
        unsigned :2;
        unsigned ALTINT:1;
        unsigned :2;
        unsigned FAR_LOOPBACK:1;
        unsigned MDPREPB:1;
        unsigned LOWSQEN:1;
        unsigned :1;
        unsigned EDPWRDOWN:1;
        unsigned :2;
    };
    unsigned short w;
} KSZ9031_MODECTRLbits_t; // reg 17: PHY_REG_MODE_CTRL
#define M_MODECTRL_ENERGYON_MASK     0x0002U
#define M_MODECTRL_FORCE_GOOD_LINK_MASK  0x0004U
#define M_MODECTRL_PHYADPB_MASK      0x0008U
#define M_MODECTRL_ALTINT_MASK       0x0040U
#define M_MODECTRL_FAR_LOOPBACK_MASK 0x0200U
#define M_MODECTRL_MDPREPB_MASK      0x0400U
#define M_MODECTRL_LOWSQEN_MASK      0x0800U
#define M_MODECTRL_EDPWRDOWN_MASK    0x2000U


typedef union 
{
    struct __attribute__((packed))
    {    
        unsigned PHYAD:5;
        unsigned MODE:3;
        unsigned :6;
        unsigned MDIOODE:1;
        unsigned :1;
    };
    unsigned short w;
} KSZ9031_SPECIALMODEbits_t;  // reg 18: PHY_REG_SPECIAL_MODE
#define M_SPECIALMODE_PHYAD_MASK     0x001fU
#define M_SPECIALMODE_MODE_MASK      0x00e0U
#define M_SPECIALMODE_MDIOODE_MASK   0x4000U




typedef union 
{
    struct __attribute__((packed))
    {
        unsigned Sym_Err_Cnt:16;
    };
    unsigned short w;
} KSZ9031_SYMBOLERRCNTbits_t; // reg 26: PHY_REG_SYMBOL_ERR_CNT


typedef union 
{
    struct __attribute__((packed))
    {    
        unsigned :4;
        unsigned XPOL:1;
        unsigned :6;
        unsigned SQEOFF:1;
        unsigned :1;
        unsigned CH_SELECT:1;
        unsigned :1;
        unsigned AMDIXCTRL:1;
    };
    unsigned short w;
} KSZ9031_SPECIALCTRLbits_t;  // reg 27: PHY_REG_SPECIAL_CTRL
#define M_SPECIALCTRL_XPOL_MASK      0x0010U
#define M_SPECIALCTRL_SQEOFF_MASK    0x0800U
#define M_SPECIALCTRL_CH_SELECT_MASK 0x2000U   // Manual MDIX
#define M_SPECIALCTRL_AMDIXCTRL_MASK 0x8000U




typedef union 
{
    struct __attribute__((packed))
    {    
        unsigned :1;
        unsigned INT1:1;
        unsigned INT2:1;
        unsigned INT3:1;
        unsigned INT4:1;
        unsigned INT5:1;
        unsigned INT6:1;
        unsigned INT7:1;
        unsigned INT8:1;
        unsigned :7;
    };
    unsigned short w;
} KSZ9031_INTSOURCEbits_t;    // reg 29: PHY_REG_INT_SOURCE

typedef union 
{
    struct __attribute__((packed))
    {
        unsigned :1;
        unsigned INT1:1;
        unsigned INT2:1;
        unsigned INT3:1;
        unsigned INT4:1;
        unsigned INT5:1;
        unsigned INT6:1;
        unsigned INT7:1;
        unsigned INT8:1;
        unsigned :7;
    };
    unsigned short w;
} KSZ9031_INTMASKbits_t;  // reg 30: PHY_REG_INT_MASK

#define WOL_INT8_EN 0x0100U

typedef union 
{
    struct __attribute__((packed))
    {        
        unsigned SCRMBL_DISBL:1;
        unsigned :1;
        unsigned SPEED:3;   // 1: 10MbpsHD; 5:10MbpsFD; 2: 100MbpsHD; 6: 100MbpsFD; 
        unsigned :1;
        unsigned ENABLE_4B5B:1;
        unsigned GPO:3;
        unsigned :2;
        unsigned AUTODONE:1;
        unsigned :3;
    };
    unsigned short w;
} KSZ9031_PHYCTRLbits_t;  // reg 31: PHY_REG_PHY_CTRL
#define M_PHYCTRL_SCRMBL_DISBL_MASK 0x0001U
#define M_PHYCTRL_SPEED_MASK        0x001cU
#define M_PHYCTRL_SPEED_FDUPLX_MASK 0x0010U
#define M_PHYCTRL_SPEED_100_10_MASK 0x000cU
#define M_PHYCTRL_SPEED_100_MASK    0x0008U
#define M_PHYCTRL_ENABLE_4B5B_MASK  0x0040U
#define M_PHYCTRL_GPO_MASK          0x0380U
#define M_PHYCTRL_AUTODONE_MASK     0x1000U

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
} KSZ9031_MMDACCESSCONTROLbits_t; // Index 13: 

typedef enum
{   // MMD Register under Device Address 0
    PHY_MMD_DEV0_AN_FLP_BURST_TX_LO_REG = 3,
    PHY_MMD_DEV0_AN_FLP_BURST_TX_HI_REG = 4,
}KSZ9031_PHY_MMD_DEV0_REG;

#define M_PHY_MMD_CNTL_ACCESS_ADDRESS_MASK  0x0000U     // 15 and 14th is 00
#define M_PHY_MMD_CNTL_ACCESS_DATA_MASK     0x4000U     // 15 and 14 th is 01
#define M_PHY_MMD_RESERVED_MASK             0x3FE0U

typedef enum 
{
    MMD_PCS =3,
    MMD_AUTONEG=7,
    MMD_VENDOR_SPECIFIC=30,
}KSZ9031_MMDDEVADDRES;

// Wake up control and Status Register
typedef union 
{
    struct __attribute__((packed))
    {
        unsigned BROADCAST_WKAEUP_ENBL:1;
        unsigned MAGIC_PKT_ENBL:1;
        unsigned WAKEUP_FRAME_ENBL:1;
        unsigned PERFECT_DA_WAKEUP_ENBL:1;
        unsigned BROADCAST_PKT_RCVD:1;
        unsigned MAGIC_PKT_RCVD:1;
        unsigned RMT_WAKEUP_RCVD:1;
        unsigned WOL_DA_FRAME_RCVD:1;
        unsigned WOL_CONFIG:1;
        unsigned NPME_SLEF_CLEAR:1;
        unsigned RMII_SELECT:1;
        unsigned LED2_SELECT:1;
        unsigned LED1_SELECT:1;
        unsigned INTF_DISABLE:1;
    };
    unsigned short w;
} KSZ9031_WOL_CNTL_STATUS_bits_t; // Index 3.32784: 

#define M_MII_RMII_INTF_MASK                     0x8000U
#define M_LED1_FUNCTION_LINKACTIVITY_MASK        0x0000U
#define M_LED1_FUNCTION_NINT_MASK                0x2000U
#define M_LED1_FUNCTION_NPME_MASK                0x4000U
#define M_LED1_FUNCTION_LINK_SPEED_MASK          0x6000U
#define M_LED2_FUNCTION_LINKSPEED_MASK           0x0000U
#define M_LED2_FUNCTION_NINT_MASK                0x0800U
#define M_LED2_FUNCTION_NPME_MASK                0x1000U
#define M_LED2_FUNCTION_LINKACTIVITY_MASK        0x1800U

#define M_RXD2_FUNCTION_SELC_MASK                0x0400U
#define M_NPME_FUNCTION_SELCTION_MASK            0x0200U
#define M_WOL_CONFIGURED_MASK                    0x0100U
#define M_WOL_DA_FRAME_RECEIVED_MASK             0x0080U
#define M_WOL_REMOTE_WAKEUP_FRAME_RECEIVED_MASK  0x0040U
#define M_WOL_MAGIC_PACKET_RECEIVED_MASK         0x0020U
#define M_WOL_BROADCAST_FRAME_RECEIVED_MASK      0x0010U
#define M_WOL_DA_WAKEUP_FRAME_ENABLE_MASK        0x0008U
#define M_WOL_WAKEUP_FRAME_ENABLE_MASK           0x0004U
#define M_WOL_MAGIC_PKT_ENABLE_MASK              0x0002U
#define M_WOL_BROADCAST_WAKEU_ENABLE_MASK        0x0001U


// Wake up Filter Configuration Register WUF_CFGA
typedef union 
{
    struct __attribute__((packed))
    {
        unsigned filter_offset:8;
        unsigned filter_broadcast_en:1;
        unsigned filter_multicast_en:1;
        unsigned filter_match_en:1;
        unsigned :3;
        unsigned filter_triggered:1;
        unsigned filter_filter_en:1;
    };
    unsigned short w;
} KSZ9031_WOL_FILTER_CONFIG_bits_t;   // Index 3.32785: 

#define M_WOL_WAKEUP_FILTER_ENABLE           0x8000U
#define M_WOL_WAKEUP_FILTER_TRIGGERED        0x4000U
#define M_WOL_WAKEUP_ADDRESS_MATCH_ENA       0x0400U
#define M_WOL_WAKEUP_MULTICAST_MATCH_ENA     0x0200U
#define M_WOL_WAKEUP_BROADCAST_ENA           0x0100U
#define M_WOL_WAKEUP_PATTERN_OFFSET_MASK     0x00FFU

typedef enum
{
    UNICAST_ADDRESS_MATCH=M_WOL_WAKEUP_ADDRESS_MATCH_ENA,
    MULTICAST_ADDRESS_MATCH=M_WOL_WAKEUP_MULTICAST_MATCH_ENA,
    BROADCAST_ADDRESS_MATCH=M_WOL_WAKEUP_BROADCAST_ENA,
    WAKEUP_FILTER_NONE=0,
}KSZ9031_WOL_WAKEUP_FILTER;

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

}KSZ9031_WOL_MMD_INDEXNO;

typedef enum
{
    PERFECT_DA_FRAME=0,
    REMOTE_WAKEUP_FRAME,
    MAGIC_PKT_RECEIVED,
    BROADCAST_FRAME_RCVD,
}KSZ9031_WOL_WAKEUP_FRAME;

typedef enum
{
    WOL_INT_ENABLED=1,
    WOL_INT_DISABLED,
}KSZ9031_WOLIntSatus;

typedef enum
{
    WOL_WAKEUP_FILTER_ENA=1,
    WOL_WAKEUP_FILTER_DIS,
}KSZ9031_WOLWakeupFilterSatus;


#define WOl_CRC_POLY 0x8005U    /* s/b international standard for CRC-16 */

#define WOL_WAKEUP_FRAME_BYTEMASK_SIZE 8U
// User configuration for WOL Wake up frame Part
#define WOL_WAKEUP_FRAME_START_OFFSET   0x40U

// WOL_WAKEUP_FRAME_PATTERN_LEN is the size of  WOL_WAKEUP_FRAME_PATTERN . 
// So both WOL_WAKEUP_FRAME_PATTERN_LEN and WOL_WAKEUP_FRAME_PATTERN are 
// related to each other
#define WOL_WAKEUP_FRAME_PATTERN_LEN 4U
#define WOL_WAKEUP_FRAME_PATTERN  {0x01U, 0x01U, 0x01U, 0x01U}

// WOL wake up frame byte mask values need to be provided here
//The byte mask is a 128-bit field that specifies whether or not each of the 128
//contiguous bytes within the frame, beginning with the pattern offset, should be checked.
//If bit j in the byte mask is set, the detection logic checks the byte (pattern offset + j) in the
//frame, otherwise byte (pattern offset + j) is ignored.

#define WOL_WAKEUP_BYTEMASK_REG0 0x000FU
#define WOL_WAKEUP_BYTEMASK_REG1 0x0000U
#define WOL_WAKEUP_BYTEMASK_REG2 0x0000U
#define WOL_WAKEUP_BYTEMASK_REG3 0x0000U
#define WOL_WAKEUP_BYTEMASK_REG4 0x0000U
#define WOL_WAKEUP_BYTEMASK_REG5 0x0000U
#define WOL_WAKEUP_BYTEMASK_REG6 0x0000U
#define WOL_WAKEUP_BYTEMASK_REG7 0x0000U



#endif  // H_KSZ9031_PRIV_H_

