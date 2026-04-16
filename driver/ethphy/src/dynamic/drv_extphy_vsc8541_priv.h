/*******************************************************************************
  VSC8541 private definitions

  Company :
    Microchip Technology Inc.
    
  File Name :
    drv_extphy_vsc8541_priv.h

  Summary :
    VSC8541 private definitions

  Description :
    This file provides the VSC8541 private definitions.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2026, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef H_VSC8541_PRIV_H_

#define H_VSC8541_PRIV_H_




typedef enum
{
    /*
    - basic registers, accross all registers : 0-1
    PHY_REG_BMCON  = 0,
    PHY_REG_BMSTAT  = 1,
    - extended registers : 2-15
    PHY_REG_PHYID1  = 2,
    PHY_REG_PHYID2  = 3,
    PHY_REG_ANAD  = 4,
    PHY_REG_ANLPAD  = 5,
    PHY_REG_ANLPADNP = 5,
    PHY_REG_ANEXP  = 6,
    PHY_REG_ANNPTR  = 7,
    PHY_REG_ANLPRNP  = 8,
    */
    // specific vendor registers : 16-31
    VSC8541_REG_1000BASE_T_CTRL = 9,
    VSC8541_REG_1000BASE_T_STAT = 10,
    VSC8541_REG_MMD_ACCESS_CTRL = 13,
    VSC8541_REG_MMD_ACCESS_DATA_ADDR = 14,
    VSC8541_REG_1000BASE_T_STAT_EXENTSION_1 = 15,
    VSC8541_REG_100BASE_TX_STAT_EXENTSION = 16,
    VSC8541_REG_1000BASE_T_STAT_EXENTSION_2 = 17,
    VSC8541_REG_BYPASS_CTRL = 18,
    VSC8541_REG_ERR_COUNTER_1 = 19,
    VSC8541_REG_ERR_COUNTER_2 = 20,
    VSC8541_REG_ERR_COUNTER_3 = 21,
    VSC8541_REG_EXTEND_CTRL_STAT = 22,
    VSC8541_REG_EXTEND_PHY_CTRL_1 = 23,
    VSC8541_REG_EXTEND_PHY_CTRL_2 = 24,
    VSC8541_REG_INT_MASK = 25,
    VSC8541_REG_INT_STAT = 26,
    VSC8541_REG_AUXILIARY_CTRL_STAT = 28,
    VSC8541_REG_LED_MODE_SELECT = 29,
    VSC8541_REG_LED_BEHAVIOR = 30,
    VSC8541_REG_EXTENDED_PAGE_ACCESS = 31,
    //
    // PHY_REGISTERS  = 32 // total number of registers

    // Extend Page 1 Registers
    VSC8541_REG_EXT1_CU_MEDIA_CRC_GOOD_COUNTER = 18,
    VSC8541_REG_EXT1_EXTENDED_MODE_CTRL = 19,
    VSC8541_REG_EXT1_EXTENDED_PHY_CTRL_3 = 20,
    VSC8541_REG_EXT1_EXTENDED_PHY_CTRL_4 = 23,
    VSC8541_REG_EXT1_ETH_PACKET_GEN_1 = 29,
    VSC8541_REG_EXT1_ETH_PACKET_GEN_2 = 30,

    // Extend Page 2 Registers
    VSC8541_REG_EXT2_CU_PMD_TRANSMIT_CTRL = 16,
    VSC8541_REG_EXT2_EEE_CTRL = 17,
    VSC8541_REG_EXT2_RGMII_CTRL = 20,
    VSC8541_REG_EXT2_WOL_MAC_ADD_15_0 = 21,
    VSC8541_REG_EXT2_WOL_MAC_ADD_31_16 = 22,
    VSC8541_REG_EXT2_WOL_MAC_ADD_47_32 = 23,
    VSC8541_REG_EXT2_SECURE_ON_PASSWORD_15_0 = 24,
    VSC8541_REG_EXT2_SECURE_ON_PASSWORD_31_16 = 25,
    VSC8541_REG_EXT2_SECURE_ON_PASSWORD_47_32 = 26,
    VSC8541_REG_EXT2_WOL_MAC_INTERFACE_CTRL = 27,
    VSC8541_REG_EXT2_EXTENDED_INT_MASK = 28,
    VSC8541_REG_EXT2_EXTENDED_INT_STAT = 29,
    VSC8541_REG_EXT2_RING_RESILIENCY_CTRL = 30,

    // Extend General Purpose Registers
    VSC8541_REG_EXTG_CLKOUT_CTRL = 13,
    VSC8541_REG_EXTG_GPIO_CTRL_2 = 14,
    VSC8541_REG_EXTG_FAST_LINK_REG = 19,
    VSC8541_REG_EXTG_RECOVERED_CLK_CTRL = 23,
    VSC8541_REG_EXTG_ENHANCED_LED_CTRL = 25,

}VSC8541_VENDOR_REG;

/*----------------------------------------------------------------*/

// vendor registers: Page 0x00
//
typedef union
{
    struct __attribute__((packed)) 
    {    
        unsigned : 8;        
        unsigned F_1000BASE_T_HDX_CAPBL  : 1;
        unsigned F_1000BASE_T_FDX_CAPBL  : 1;
        unsigned PORT_TYPE  : 1;
        unsigned MASTER_SLAVE_VAL   : 1;
        unsigned MASTER_SLAVE_MAN_CONFIG    : 1;
        unsigned TX_TEST_MODE : 3;
    };
    unsigned short w;
}VSC8541_REG_1000BASE_T_CTRL_bits_t;    // reg 9

typedef union
{
    struct __attribute__((packed)) 
    {    
        unsigned IDLE_ERROR_COUNT   : 8;
        unsigned : 2;
        unsigned LP_1000BASE_T_HDX_CAPBL  : 1;
        unsigned LP_1000BASE_T_FDX_CAPBL  : 1;
        unsigned REM_RECEIVE_STAT  : 1;
        unsigned LCL_RECEIVE_STAT  : 1;
        unsigned MASTER_SLAVE_CONFIG_RES    : 1;
        unsigned MASTER_SLAVE_CONFIG_FAULT    : 1;
    };
    unsigned short w;
}VSC8541_REG_1000BASE_T_STAT_bits_t;    // reg 10

typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned DVAD : 5;
        unsigned : 9;
        unsigned FUNCTION : 2;
    };
    unsigned short w;
} VSC8541_REG_MMD_ACCESS_CTRL_bits_t; // reg 13
#define M_MMD_ACCESS_CTRL_FUNCTION_MASK  0xC000U
#define M_MMD_ACCESS_CTRL_DVAD_MASK      0x001FU


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned REGISTER_ADD_DATA : 16;
    };
    unsigned short w;
} VSC8541_REG_MMD_ACCESS_DATA_ADDR_bits_t; // reg 14:
#define M_MMD_REGISTER_ADD_DATA_MASK     0x0002U


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned    : 12;
        unsigned F_1000BASE_X_HDX_CAPBL  : 1;
        unsigned F_1000BASE_T_FDX_CAPBL  : 1;
        unsigned F_1000BASE_T_HDX_CAPBL : 1;
        unsigned    : 1;
    };
    unsigned short w;
} VSC8541_REG_1000BASE_T_STAT_EXENTSION_1_bits_t; // reg 15: 

typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned : 8;
        unsigned ESD_ERR : 1;
        unsigned SSD_ERR : 1;
        unsigned TRANSMIT_ERR : 1;
        unsigned RECEIVE_ERR : 1;
        unsigned CURRENT_LINK_STAT : 1;
        unsigned DISCONNECT_STATE : 1;
        unsigned LOCK_ERR : 1;
        unsigned DESCRAMBLER_LOCK : 1;
    };
    unsigned short w;
} VSC8541_REG_100BASE_TX_STAT_EXENTSION_bits_t; // reg 16: 
#define M_100BASE_TX_DESCRAMBLER_LOCK_MASK       0x8000U
#define M_100BASE_TX_LOCK_ERR_MASK               0x4000U
#define M_100BASE_TX_DISCONNECT_STATE_MASK       0x2000U
#define M_100BASE_TX_CURRENT_LINK_STAT_MASK      0x1000U
#define M_100BASE_TX_RECEIVE_ERR_MASK            0x0800U
#define M_100BASE_TX_TRANSMIT_ERR_MASK           0x0400U
#define M_100BASE_TX_SSD_ERR_MASK                0x0200U
#define M_100BASE_TX_ESD_ERR_MASK                0x0100U

typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned    : 5;
        unsigned MDI_XOVR_ERR    : 1;
        unsigned NON_COMPL_BCM5400_DETECT   : 1;
        unsigned CARRIER_EXT_ERR    : 1;
        unsigned ESD_ERR    : 1;
        unsigned SSD_ERR    : 1;
        unsigned TRANSMIT_ERR   : 1;
        unsigned RECEIVE_ERR   : 1;
        unsigned CURRENT_LINK_STAT  : 1;
        unsigned DISCONNECT_STATE   : 1;
        unsigned LOCK_ERR   : 1;
        unsigned DESCRAMBLER_LOCK    : 1;
    };
    unsigned short w;
} VSC8541_REG_1000BASE_T_STAT_EXENTSION_2_bits_t; // reg 17: 
#define M_1000BASE_T_DESCRAMBLER_LOCK_MASK       0x8000U
#define M_1000BASE_T_LOCK_ERR_MASK               0x4000U
#define M_1000BASE_T_DISCONNECT_STATE_MASK       0x2000U
#define M_1000BASE_T_CURRENT_LINK_STAT_MASK      0x1000U
#define M_1000BASE_T_RECEIVE_ERR_MASK            0x0800U
#define M_1000BASE_T_TRANSMIT_ERR_MASK           0x0400U
#define M_1000BASE_T_SSD_ERR_MASK                0x0200U
#define M_1000BASE_T_ESD_ERR_MASK                0x0100U
#define M_1000BASE_T_CARRIER_EXT_ERR_MASK        0x0080U
#define M_1000BASE_T_NON_COMPL_BCM5400_MASK      0x0040U
#define M_1000BASE_T_MDI_XOVR_ERR_MASK           0x0020U


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned : 1;
        unsigned DISABLE_AUTO_1000BASE_T_NEXT_PAGE_XCHG : 1;
        unsigned DISABLE_PULSE_SHAPE_FILTER : 1;
        unsigned PARALLEL_DETECT_CTRL : 1;
        unsigned DISABLE_POLARITY_CORRECTION : 1;
        unsigned DISABLE_PAIR_SWAP_CORRECTION : 1;
        unsigned DISABLE_NON_COMPL_BCM5400_DETECT : 1;
        unsigned DISABLE_HP_AUTO_MDIX : 1;
        unsigned : 1;
        unsigned BYPASS_LFI_TIMER : 1;
        unsigned BYPASS_PCS_TRANSMIT : 1;
        unsigned BYPAS_PCS_RECEIVER : 1;
        unsigned BYPASS_DESCRAMBLER : 1;
        unsigned BYPASS_SCRAMBLER : 1;
        unsigned BYPASS_4B5B_EN_DE_CODER : 1;
        unsigned TRANSMIT_DISABLE : 1;
    };
    unsigned short w;
} VVSC8541_REG_BYPASS_CTRL_bits_t; // reg 18: 
#define M_BYPASS_TRANSMIT_DISABLE_MASK                 0x8000U
#define M_BYPASS_4B5B_EN_DE_CODER_MASK                 0x4000U
#define M_BYPASS_BYPASS_SCRAMBLER_MASK                 0x2000U
#define M_BYPASS_BYPASS_DESCRAMBLER_MASK               0x1000U
#define M_BYPASS_PCS_RECEIVER_MASK                     0x0800U
#define M_BYPASS_PCS_TRANSMIT_MASK                     0x0400U
#define M_BYPASS_LFI_TIMER_MASK                        0x0200U
#define M_BYPASS_HP_AUTO_MDIX_MASK                     0x0080U
#define M_BYPASS_NON_COMPL_BCM5400_DETECT_MASK         0x0040U
#define M_BYPASS_DISABLE_PAIR_SWAP_CORRECTION_MASK     0x0020U
#define M_BYPASS_DISABLE_POLARITY_CORRECTION_MASK      0x0010U
#define M_BYPASS_PARALLEL_DETECT_CTRL_MASK             0x0008U
#define M_BYPASS_PULSE_SHAPE_FILTER_MASK               0x0004U
#define M_BYPASS_AUTO_1000BASE_T_NEXT_PAGE_XCHG_MASK   0x0002U


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned RECEIVE_ERR_COUNTER : 8;
        unsigned : 8;
    };
    unsigned short w;
} VSC8541_REG_ERR_COUNTER_1_bits_t; // reg 19: 
#define M_ERR_COUNTER_1_COUNTER_ERR_MASK                     0x00FFU


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned FALSE_CARRIER_COUNTER : 8;
        unsigned : 8;
    };
    unsigned short w;
} VSC8541_REG_ERR_COUNTER_2_bits_t; // reg 20: 
#define M_ERR_COUNTER_2_FALSE_CARRIER_COUNTER_MASK         0x00FFU


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned COPPER_LINK_DISCONNECT_COUNTER : 8;
        unsigned : 8;
    };
    unsigned short w;
} VSC8541_REG_ERR_COUNTER_3_bits_t; // reg 21: 
#define M_ERR_COUNTER_3_COPPER_LINK_COUNTER_MASK         0x00FFU


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned SMI_BROADCAST_WR : 1;
        unsigned : 5;
        unsigned F_10BASE_T_LINK_STAT : 1;
        unsigned F_10BASE_T_DISCONNECT_STATE : 1;
        unsigned EOF_ERR : 1;
        unsigned STICKY_RST_ENABLE : 1;
        unsigned F_10BASE_T_SQUELCH_CTRL : 2;
        unsigned SQE_DISABLE : 1;
        unsigned F_10BASE_T_ECHO_DISABLE : 1;
        unsigned JABBER_DETECT_DISABLE : 1;
        unsigned F_10BASE_T_LINK_H : 1;
    };
    unsigned short w;
} VSC8541_REG_EXTEND_CTRL_STAT_bits_t; // reg 22: 
#define M_EXT_CTRL_STAT_10BASE_T_LINK_H_MASK             0x8000U
#define M_EXT_CTRL_STAT_JABBER_DETECT_DISABLE_MASK       0x4000U
#define M_EXT_CTRL_STAT_10BASE_T_ECHO_DISABLE_MASK       0x2000U
#define M_EXT_CTRL_STAT_SQE_DISABLE_MASK                 0x1000U
#define M_EXT_CTRL_STAT_10BASE_T_SQUELCH_CTRL_MASK       0x0C00U
#define M_EXT_CTRL_STAT_STICKY_RST_ENABLE_MASK           0x0200U
#define M_EXT_CTRL_STAT_EOF_ERR_MASK                     0x0100U
#define M_EXT_CTRL_STAT_10BASE_T_DISCONNECT_STATE_MASK   0x0080U
#define M_EXT_CTRL_STAT_10BASE_T_LINK_STAT_MASK          0x0040U
#define M_EXT_CTRL_STAT_SMI_BROADCAST_WR_MASK            0x0000U


typedef union 
{
    struct __attribute__((packed)) 
    {
        unsigned : 3;
        unsigned FAREND_LOOPBACK_MODE : 1;
        unsigned : 7;
        unsigned MAC_INTERFACE_SEL : 2;
        unsigned MAC_CLK_ENABLE : 1;    
        unsigned : 2;
    };
    unsigned short w;
} VSC8541_REG_EXTEND_PHY_CTRL_1_bits_t; // reg 23: 
#define M_EXT_PHY_CTRL_MAC_CLK_ENABLE_MASK              0x2000U
#define M_EXT_PHY_CTRL_MAC_INTERFACE_SEL_MASK           0x1800U
// mask values for the MAC_INTERFACE_SEL field 
#define M_MAC_INTERFACE_SEL_GMII_MASK           0x0000U
#define M_MAC_INTERFACE_SEL_MII_MASK            0x0000U
#define M_MAC_INTERFACE_SEL_RMII_MASK           0x0800U
#define M_MAC_INTERFACE_SEL_RGMII_MASK          0x1000U

#define M_EXT_PHY_CTRL_FAREND_LOOPBACK_MODE_MASK        0x0008U


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned CONNECTOR_LOOPBACK : 1;
        unsigned : 3;
        unsigned JUMBO_PKT_MODE : 2;
        unsigned : 6;
        unsigned PICMG_REDUCE_PWR_MODE : 1;
        unsigned F_100BASE_T_EDGE_RATE_CTRL : 3;
    };
    unsigned short w;
} VSC8541_REG_EXTEND_PHY_CTRL_2_bits_t; // reg 24: 
#define M_EXT_PHY_CTRL_STAT_100BASE_T_EDGE_RATE_CTRL_MASK   0xE000U
#define M_EXT_PHY_CTRL_STAT_PICMG_REDUCE_PWR_MODE_MASK      0x1000U
#define M_EXT_PHY_CTRL_STAT_JUMBO_PKT_MODE_MASK             0x0030U
#define M_EXT_PHY_CTRL_STAT_CONNECTOR_LOOPBACK_MASK         0x0000U


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned RX_ER_INT_MASK : 1;
        unsigned MASTER_SLAVE_RES_INT_MASK: 1;
        unsigned LINK_SPEED_DOWNSHIFT_INT_MASK: 1;
        unsigned FALSE_CARRRIER_INT_MASK : 1;
        unsigned : 1;
        unsigned EXTEND_INT_MASK : 1;
        unsigned WOL_EVENT_MASK : 1;
        unsigned FAST_LINK_FAILURE_INT_MASK : 1;
        unsigned SYMBOL_ERROR_INT_MASK : 1;
        unsigned POE_DETECT_MASK : 1;
        unsigned AUTONEG_COMPLETE_MASK : 1;
        unsigned AUTONEG_ERROR_MASK : 1;
        unsigned FDX_STATE_CHANGE_MASK : 1;
        unsigned LINK_STATE_CHANGE_MASK : 1;
        unsigned SPEED_STATE_CHANGE_MASK : 1;
        unsigned MDINT_INT_STAT_EN : 1;
    };
    unsigned short w;
} VSC8541_REG_INT_MASK_bits_t; // reg 25: 
#define M_MDINT_INT_STAT_EN_MASK            0x8000U
#define M_SPEED_STATE_CHANGE_MASK           0x4000U
#define M_LINK_STATE_CHANGE_MASK            0x2000U
#define M_FDX_STATE_CHANGE_MASK             0x1000U
#define M_AUTONEG_ERROR_MASK                0x0800U
#define M_AUTONEG_COMPLETE_MASK             0x0400U
#define M_POE_DETECT_MASK                   0x0200U
#define M_SYMBOL_ERROR_INT_MASK             0x0100U
#define M_FAST_LINK_FAILURE_INT_MASK        0x0080U
#define M_WOL_EVENT_MASK                    0x0040U
#define M_EXTEND_INT_MASK                   0x0020U
#define M_FALSE_CARRRIER_INT_MASK           0x0008U
#define M_LINK_SPEED_DOWNSHIFT_INT_MASK     0x0004U
#define M_MASTER_SLAVE_RES_INT_MASK         0x0002U
#define M_RX_ER_INT_MASK                    0x0001U


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned RX_ER_INT_STAT : 1;
        unsigned MASTER_SLAVE_RES_INT_STAT : 1;
        unsigned LINK_SPEED_DOWNSHIFT_INT_STAT : 1;
        unsigned FALSE_CARRRIER_INT_STAT : 1;
        unsigned : 1;
        unsigned EXTEND_INT_STAT : 1;
        unsigned WOL_EVENT_STAT : 1;
        unsigned FAST_LINK_FAILURE_INT_STAT : 1;
        unsigned SYMBOL_ERROR_INT_STAT : 1;
        unsigned POE_DETECT_STAT : 1;
        unsigned AUTONEG_COMPLETE_STAT : 1;
        unsigned AUTONEG_ERROR_STAT : 1;
        unsigned FDX_STATE_CHANGE_STAT : 1;
        unsigned LINK_STATE_CHANGE_STAT : 1;
        unsigned SPEED_STATE_CHANGE_STAT : 1;
        unsigned INT_STAT : 1;
    };
    unsigned short w;
} VSC8541_REG_INT_STAT_bits_t; // reg 26: 
#define M_MDINT_INT_STAT_MASK                   0x8000U
#define M_SPEED_STATE_CHANGE_STAT_MASK          0x4000U
#define M_LINK_STATE_CHANGE_STAT_MASK           0x2000U
#define M_FDX_STATE_CHANGE_STAT_MASK            0x1000U
#define M_AUTONEG_ERROR_STAT_MASK               0x0800U
#define M_AUTONEG_COMPLETE_STAT_MASK            0x0400U
#define M_POE_DETECT_STAT_MASK                  0x0200U
#define M_SYMBOL_ERROR_INT_STAT_MASK            0x0100U
#define M_FAST_LINK_FAILURE_INT_STAT_MASK       0x0080U
#define M_WOL_EVENT_STAT_MASK                   0x0040U
#define M_EXTEND_INT_STAT_MASK                  0x0020U
#define M_FALSE_CARRRIER_INT_STAT_MASK          0x0008U
#define M_LINK_SPEED_DOWNSHIFT_INT_STAT_MASK    0x0004U
#define M_MASTER_SLAVE_RES_INT_STAT_MASK        0x0002U
#define M_RX_ER_INT_STAT_MASK                   0x0001U


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned MEDIA_MODE_STAT : 2;
        unsigned ACTIPHY_LINK_STAT_TMO_CTRL_0: 1;
        unsigned SPEED_STAT : 2;
        unsigned FDX_STAT : 1;
        unsigned ACTIPHY_ENABLE : 1;
        unsigned ACTIPHY_LINK_STAT_TMO_CTRL_1: 1;
        unsigned D_POLARITY_INVERSION : 1;
        unsigned C_POLARITY_INVERSION : 1;
        unsigned B_POLARITY_INVERSION : 1;
        unsigned A_POLARITY_INVERSION : 1;
        unsigned CD_PAIR_SWAP : 1;
        unsigned HP_AUTO_MDIX_XOVR_INDICATION : 1;
        unsigned AUTONEG_DISABLED : 1;
        unsigned AUTONEG_COMPLETED : 1;
    };
    unsigned short w;
} VSC8541_REG_AUXILIARY_CTRL_STAT_bits_t; // reg 28: 
#define M_AUTONEG_COMPLETED_MASK                    0x8000U
#define M_AUTONEG_DISABLED_MASK                     0x4000U
#define M_HP_AUTO_MDIX_XOVR_INDICATION_MASK         0x2000U
#define M_CD_PAIR_SWAP_MASK                         0x1000U
#define M_A_POLARITY_INVERSION_MASK                 0x0800U
#define M_B_POLARITY_INVERSION_MASK                 0x0400U
#define M_C_POLARITY_INVERSION_MASK                 0x0200U
#define M_D_POLARITY_INVERSION_MASK                 0x0100U
#define M_ACTIPHY_LINK_STAT_TMO_CTRL_1_MASK         0x0080U
#define M_ACTIPHY_ENABLE_MASK                       0x0040U
#define M_FDX_STAT_MASK                             0x0020U
#define M_SPEED_STAT_MASK                           0x0018U
#define M_ACTIPHY_LINK_STAT_TMO_CTRL_0_MASK         0x0004U
#define M_MEDIA_MODE_STAT_MASK                      0x0003U


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned LED2_MODE_SEL : 4;
        unsigned LED1_MODE_SEL : 4;
        unsigned : 8;
    };
    unsigned short w;
} VSC8541_REG_LED_MODE_SELECT_bits_t; // reg 29
#define M_LED1_MODE_SEL_MASK  0x00F0U
#define M_LED2_MODE_SEL_MASK  0x000FU


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned LED0_COMBINE_FEATURES_DISABLE : 1;
        unsigned LED1_COMBINE_FEATURES_DISABLE : 1;
        unsigned : 3;
        unsigned LED0_PULSE_BLINK_SEL : 1;
        unsigned LED1_PULSE_BLINK_SEL : 1;
        unsigned : 3;
        unsigned LED_PULSE_BLINK_RATE : 2;
        unsigned LED_PULSING_EN : 1;
        unsigned : 3;
    };
    unsigned short w;
} VSC8541_REG_LED_BEHAVIOR_bits_t; // reg 30: 
#define M_LED_PULSING_EN_MASK                     0x1000U
#define M_LED_PULSE_BLINK_RATE_MASK               0x0C00U
#define M_LED1_PULSE_BLINK_SEL_MASK               0x0040U
#define M_LED0_PULSE_BLINK_SEL_MASK               0x0020U
#define M_LED1_COMBINE_FEATURES_DISABLE_MASK      0x0002U
#define M_LED0_COMBINE_FEATURES_DISABLE_1_MASK    0x0001U


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned EXT_PAGE_REGISTER_ACCESS : 16;
    };
    unsigned short w;
} VSC8541_REG_EXTENDED_PAGE_ACCESS_bits_t; // reg 31: 



/*----------------------------------------------------------------*/
// Extend Page 1 Registers: Page 0x01


typedef union 
{
    struct __attribute__((packed)) 
    {    
        unsigned CRC_GOOD_COUNTER_CONTENTS : 14;
        unsigned : 1;
        unsigned PKT_SINCE_LAST_READ : 1;
    };
    unsigned short w;
} VSC8541_REG_EXT1_CU_MEDIA_CRC_GOOD_COUNTER_bits_t; // reg 18E1: 
#define M_EXT1_PKT_SINCE_LAST_READ_MASK              0x8000U
#define M_EXT1_CRC_GOOD_COUNTER_CONTENTS_MASK        0x3FFFU


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned : 1;
        unsigned FORCE_MDI_XOVER : 2;
        unsigned FAST_LINK_FAILURE : 1;
        unsigned : 6;
        unsigned LED_RST_BLINK_SUPPRESS : 3;
        unsigned LED0_EXT_MODE : 1;
        unsigned LED1_EXT_MODE : 1;
        unsigned : 2;
    };
    unsigned short w;
} VSC8541_REG_EXT1_EXTENDED_MODE_CTRL_bits_t; // reg 19E1: 
#define M_EXT1_LED1_EXT_MODE_MASK                0x2000U
#define M_EXT1_LED0_EXT_MODE_MASK                0x1000U
#define M_EXT1_LED_RST_BLINK_SUPPRESS_MASK       0x0800U
#define M_EXT1_FAST_LINK_FAILURE_MASK            0x0010U
#define M_EXT1_FORCE_MDI_XOVER_MASK              0x000CU


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned : 1;
        unsigned LINK_SPEED_AUTO_DOWN_SHIFT_STAT : 1;
        unsigned LINK_SPEED_AUTO_DOWN_SHIFT_CTRL : 2;
        unsigned LINK_SPEED_AUTO_DOWN_SHIFT_EN : 1;
        unsigned EN_10BASE_T_NO_PREAMBLE_MODE : 1;
        unsigned MEDIA_MODE_STAT : 2;
        unsigned : 2;
        unsigned SLOW_MDC : 1;
        unsigned ACTIPHY_WAKE_UP_TIMER : 2;
        unsigned ACTIPHY_SLEEP_TIMER : 2;
        unsigned DISABLE_CARRIER_EXT : 1;
    };
    unsigned short w;
} VSC8541_REG_EXT1_EXTENDED_PHY_CTRL_3_bits_t; // reg 20E1: 
#define M_EXT1_DISABLE_CARRIER_EXT_MASK                  0x8000U
#define M_EXT1_ACTIPHY_SLEEP_TIMER_MASK                  0x6000U
#define M_EXT1_ACTIPHY_WAKE_UP_TIMER_MASK                0x1800U
#define M_EXT1_SLOW_MDC_MASK                             0x0400U
#define M_EXT1_MEDIA_MODE_STAT_MASK                      0x00C0U
#define M_EXT1_EN_10BASE_T_NO_PREAMBLE_MODE_MASK         0x0020U

#define M_EXT1_LINK_SPEED_AUTO_DOWN_SHIFT_EN_MASK        0x0010U
#define M_EXT1_LINK_SPEED_AUTO_DOWN_SHIFT_CTRL_MASK      0x000CU
#define M_EXT1_LINK_SPEED_AUTO_DOWN_SHIFT_STAT_MASK      0x0002U

typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned CU_MEDIA_CRC_ERROR_COUNTER : 8;
        unsigned INLINE_PWRD_DEVICE_DETECT_STAT : 2;
        unsigned INLINE_PWRD_DEVICE_DETECT : 1;
        unsigned PHY_ADDR : 5;
    };
    unsigned short w;
} VSC8541_REG_EXT1_EXTENDED_PHY_CTRL_4_bits_t; // reg 23E1: 
#define M_EXT1_PHY_ADDR_MASK                          0xF800U
#define M_EXT1_INLINE_PWRD_DEVICE_DETECT_MASK         0x0400U
#define M_EXT1_INLINE_PWRD_DEVICE_DETECT_STAT_MASK    0x0300U
#define M_EXT1_CU_MEDIA_CRC_ERROR_COUNTER_MASK        0x00FFU


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned BAD_FCS_GEN : 1;
        unsigned PAYLOAD_TYPE : 1;
        unsigned SOURCE_ADDR : 4;
        unsigned DEST_ADDR : 4;
        unsigned INTER_PKT_GAP : 1;
        unsigned PKT_LENGTH : 2;
        unsigned TRANSMISSION_DURATION : 1;
        unsigned EPG_RUN_STOP : 1;
        unsigned EPG_ENABLE : 1;
    };
    unsigned short w;
} VSC8541_REG_EXT1_ETH_PACKET_GEN_1_bits_t; // reg 29E1: 
#define M_EXT1_EPG_ENABLE_MASK                   0x8000U
#define M_EXT1_EPG_RUN_STOP_MASK                 0x4000U
#define M_EXT1_TRANSMISSION_DURATION_MASK        0x2000U
#define M_EXT1_PKT_LENGTH_MASK                   0x1800U
#define M_EXT1_INTER_PKT_GAP_MASK                0x0400U
#define M_EXT1_DEST_ADDR_MASK                    0x03C0U
#define M_EXT1_SOURCE_ADDR_MASK                  0x003CU
#define M_EXT1_PAYLOAD_TYPE_MASK                 0x0002U
#define M_EXT1_BAD_FCS_GEN_MASK                  0x0001U

typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned EPG_PKT_PAYLOAD : 16;
    };
    unsigned short w;
} VSC8541_REG_EXT1_ETH_PACKET_GEN_2_bits_t; // reg 30E1: 
#define M_EXT1_EPG_PKT_PAYLOAD_MASK                         0xFFFFU


/*----------------------------------------------------------------*/
// Extend Page 2 Registers: Page 0x02


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned F_10BASE_Te_SIGNAL_AMP_TRIM : 4;
        unsigned F_10BASE_T_SIGNAL_AMP_TRIM : 4;
        unsigned F_100BASE_TX_SIGNAL_AMP_TRIM : 4;
        unsigned F_1000BASE_T_SIG_AMP_TRIM : 4;
    };
    unsigned short w;
} VSC8541_REG_EXT2_CU_PMD_TRANSMIT_CTRL_bits_t; // reg 16E2: 
#define M_EXT2_1000BASE_T_SIGNAL_AMP_TRIM_MASK          0xF000U
#define M_EXT2_100BASE_TX_SIGNAL_AMP_TRIM_MASK          0x0F00U
#define M_EXT2_10BASE_T_SIGNAL_AMP_TRIM_MASK            0x00F0U
#define M_EXT2_10BASE_Te_SIGNAL_AMP_TRIM_MASK           0x000FU


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned INH_1000BASE_T_RECEIVE_EEE_LPI : 1;
        unsigned INH_1000BASE_T_TRANSMIT_EEE_LPI : 1;
        unsigned INH_100BASE_TX_RECEIVE_EEE_LPI : 1;
        unsigned INH_100BASE_TX_TRANSMIT_EEE_LPI : 1;
        unsigned FORCE_TRANSMIT_LPI : 1;
        unsigned EN_1000BASE_T_FORCE : 1;
        unsigned F_100BASE_TX_EEE_EN : 1;
        unsigned F_1000BASE_T_EEE_EN : 1;
        unsigned LINK_STAT : 1;
        unsigned : 1;
        unsigned INVERT_LED_POLARITY : 2;
        unsigned : 3;
        unsigned EN_10BASE_Te : 1;
    };
    unsigned short w;
} VSC8541_REG_EXT2_EEE_CTRL_bits_t; // reg 17E2: 
#define M_EXT2_EN_10BASE_Te_MASK                        0x8000U
#define M_EXT2_INVERT_LED_POLARITY_MASK                 0x0C00U
#define M_EXT2_LINK_STAT_MASK                           0x0100U
#define M_EXT2_1000BASE_T_EEE_EN_MASK                   0x0080U
#define M_EXT2_100BASE_TX_EEE_EN_MASK                   0x0040U
#define M_EXT2_EN_1000BASE_T_FORCE_MASK                 0x0020U
#define M_EXT2_FORCE_TRANSMIT_LPI_MASK                  0x0010U
#define M_EXT2_INH_100BASE_TX_TRANSMIT_EEE_LPI_MASK     0x0008U
#define M_EXT2_INH_100BASE_TX_RECEIVE_EEE_LPI_MASK      0x0004U
#define M_EXT2_INH_1000BASE_T_TRANSMIT_EEE_LPI_MASK     0x0002U
#define M_EXT2_INH_1000BASE_T_RECEIVE_EEE_LPI_MASK      0x0001U



typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned TX_CLK_DELAY : 3;
        unsigned RGMII_GMII_TXD_BIT_REVERSAL : 1;
        unsigned RX_CLK_DELAY : 3;
        unsigned RGMII_GMII_RXD_BIT_REVERSAL : 1;
        unsigned : 4;
        unsigned SOF_EN : 1;
        unsigned : 2;
        unsigned FLF2_EN : 1;
    };
    unsigned short w;
} VSC8541_REG_EXT2_RGMII_CTRL_bits_t; // reg 20E2: 
#define M_EXT2_FLF2_EN_MASK                         0x8000U
#define M_EXT2_SOF_EN_MASK                          0x1000U
#define M_EXT2_RGMII_GMII_RXD_BIT_REVERSAL_MASK     0x0080U
#define M_EXT2_RX_CLK_DELAY_MASK                    0x0070U
#define M_EXT2_RGMII_GMII_TXD_BIT_REVERSAL_MASK     0x0008U
#define M_EXT2_TX_CLK_DELAY_MASK                    0x0007U


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned WOL_MAC_ADD_15_0: 16;
    };
    unsigned short w;
} VSC8541_REG_EXT2_WOL_MAC_ADD_15_0_bits_t; // reg 21E2: 
#define M_EXT2_WOL_MAC_ADD_15_0_MASK                         0xFFFFU


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned WOL_MAC_ADD_31_16: 16;
    };
    unsigned short w;
} VSC8541_REG_EXT2_WOL_MAC_ADD_31_16_bits_t; // reg 22E2: 
#define M_EXT2_WOL_MAC_ADD_31_16_MASK                         0xFFFFU


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned WOL_MAC_ADD_47_32: 16;
    };
    unsigned short w;
} VSC8541_REG_EXT2_WOL_MAC_ADD_47_32_bits_t; // reg 23E2: 
#define M_EXT2_WOL_MAC_ADD_47_32_MASK                         0xFFFFU


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned SECURE_ON_PASSWORD_15_0: 16;
    };
    unsigned short w;
}VSC8541_REG_EXT2_SECURE_ON_PASSWORD_15_0_bits_t; // reg 24E2: 
#define M_EXT2_SECURE_ON_PASSWORD_15_0_MASK                         0xFFFFU

typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned SECURE_ON_PASSWORD_31_16: 16;
    };
    unsigned short w;
}VSC8541_REG_EXT2_SECURE_ON_PASSWORD_31_16_bits_t; // reg 25E2: 
#define M_EXT2_SECURE_ON_PASSWORD_31_16_MASK                         0xFFFFU


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned SECURE_ON_PASSWORD_47_32: 16;
    };
    unsigned short w;
}VSC8541_REG_EXT2_SECURE_ON_PASSWORD_47_32_bits_t; // reg 26E2: 
#define M_EXT2_SECURE_ON_PASSWORD_47_32_MASK                         0xFFFFU


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned MDINT_CMOS_DRIVE : 1;
        unsigned : 3;
        unsigned RMII_CLKOUT_EN : 1;
        unsigned PAD_EDGE_RATE_CTRL : 3;
        unsigned ADD_REP_COUNT_MAGIC_PKT : 4;
        unsigned : 2;
        unsigned SECURE_ON_PASSWORD_LENGTH : 1;
        unsigned SECURE_ON_EN : 1;
    };
    unsigned short w;
} VSC8541_REG_EXT2_WOL_MAC_INTERFACE_CTRL_bits_t; // reg 27E2: 
#define M_EXT2_SECURE_ON_EN_MASK                    0x8000U
#define M_EXT2_SECURE_ON_PASSWORD_LENGTH_MASK       0x4000U
#define M_EXT2_ADD_REP_COUNT_MAGIC_PKT_MASK         0x00F0U
#define M_EXT2_PAD_EDGE_RATE_CTRL_MASK              0x00E0U
#define M_EXT2_RMII_CLKOUT_EN_MASK                  0x0010U
#define M_EXT2_MDINT_CMOS_DRIVE_MASK                0x0001U


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned EEE_WAKE_ERR_INT_MASK : 1;
        unsigned EEE_WAIT_QUIET_TS_TIMER_INT_MASK : 1;
        unsigned EEE_RX_TQ_TIMER_INT_MASK : 1;
        unsigned EEE_LINK_FAIL_INT_MASK : 1;
        unsigned RR_SWITCH_COMPLETE_INT_MASK : 1;
        unsigned : 7;
        unsigned TX_FIFO_FLOW_INT_MASK : 1;
        unsigned RX_FIFO_FLOW_INT_MASK : 1;
        unsigned : 2;
    };
    unsigned short w;
} VSC8541_REG_EXT2_EXTENDED_INT_MASK_bits_t; // reg 28E2: 
#define M_EXT2_RX_FIFO_FLOW_INT_MASK                0x2000U
#define M_EXT2_TX_FIFO_FLOW_INT_MASK                0x1000U
#define M_EXT2_RR_SWITCH_COMPLETE_INT_MASK          0x0010U
#define M_EXT2_EEE_LINK_FAIL_INT_MASK               0x0008U
#define M_EXT2_EEE_RX_TQ_TIMER_INT_MASK             0x0004U
#define M_EXT2_EEE_WAIT_QUIET_TMR_INT_MASK          0x0002U
#define M_EXT2_EEE_WAKE_ERR_INT_MASK                0x0001U

typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned EEE_WAKE_ERR_INT_STAT : 1;
        unsigned EEE_WAIT_QUIET_TS_TIMER_INT_STAT : 1;
        unsigned EEE_RX_TQ_TIMER_INT_STAT : 1;
        unsigned EEE_LINK_FAIL_INT_STAT : 1;
        unsigned RR_SWITCH_COMPLETE_INT_STAT : 1;
        unsigned : 7;
        unsigned TX_FIFO_FLOW_INT_STAT : 1;
        unsigned RX_FIFO_FLOW_INT_STAT : 1;
        unsigned : 2;
    };
    unsigned short w;
} VSC8541_REG_EXT2_EXTENDED_INT_STAT_bits_t; // reg 29E2: 
#define M_EXT2_RX_FIFO_FLOW_INT_STAT_MASK               0x2000U
#define M_EXT2_TX_FIFO_FLOW_INT_STAT_MASK               0x1000U
#define M_EXT2_RR_SWITCH_COMPLETE_INT_STAT_MASK         0x0010U
#define M_EXT2_EEE_LINK_FAIL_INT_STAT_MASK              0x0008U
#define M_EXT2_EEE_RX_TQ_TIMER_INT_STAT_MASK            0x0004U
#define M_EXT2_EEE_WAIT_QUIET_TMR_INT_STAT_MASK         0x0002U
#define M_EXT2_EEE_WAKE_ERR_INT_STAT_MASK               0x0001U

typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned START_SWITCHOVER : 1;
        unsigned : 3;
        unsigned RING_RESILIENCY_STAT : 2;
        unsigned : 6;
        unsigned FORCE_RING_RESILIENCY_EN : 1;
        unsigned LP_ADV_RING_RESILIENCY : 1;
        unsigned ADV_RING_RESILIENCY : 1;
        unsigned RING_RESILIENCY_START_EN : 1;
    };
    unsigned short w;
} VSC8541_REG_EXT2_RING_RESILIENCY_CTRL_bits_t; // reg 30E2: 
#define M_EXT2_RING_RESILIENCY_START_EN_MASK            0x8000U
#define M_EXT2_ADV_RING_RESILIENCY_MASK                 0x4000U
#define M_EXT2_LP_ADV_RING_RESILIENCY_MASK              0x2000U
#define M_EXT2_FORCE_RING_RESILIENCY_EN_MASK            0x1000U
#define M_EXT2_RING_RESILIENCY_STAT_MASK                0x00C0U
#define M_EXT2_START_SWITCHOVER_MASK                    0x0001U

/*----------------------------------------------------------------*/
// General Purpose Registers: Page 0x10


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned : 13;
        unsigned CLKOUT_FREQ_SELECT : 2;
        unsigned CLKOUT_EN : 1;
    };
    unsigned short w;
}VSC8541_REG_EXTG_CLKOUT_CTRL_bits_t; // reg 13G: 
#define M_EXT_GP_CLKOUT_EN_MASK             0x8000U
#define M_EXT_GP_CLKOUT_FREQ_SELECT_MASK    0x6000U


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned : 9;
        unsigned TRISTATE_EN_LED : 1;
        unsigned : 1;
        unsigned COMA_MODE_INPUT_DATA : 1;
        unsigned COMA_MODE_OUTPUT_DATA : 1;
        unsigned COMA_MODE_OUTPUT_EN : 1;
        unsigned : 2;
    };
    unsigned short w;
} VSC8541_REG_EXTG_GPIO_CTRL_2_bits_t; // reg 14G: 
#define M_EXT_GP_COMA_MODE_OUTPUT_EN_MASK       0x2000U
#define M_EXT_GP_COMA_MODE_OUTPUT_DATA_MASK     0x1000U
#define M_EXT_GP_COMA_MODE_INPUT_DATA_MASK      0x0800U
#define M_EXT_GP_TRISTATE_EN_LED_MASK           0x0200U


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned FAST_LINK_FAILURE_SETTING : 4;
        unsigned : 12;
    };
    unsigned short w;
} VSC8541_REG_EXTG_FAST_LINK_REG_bits_t; // reg 19G: 
#define M_EXT_GP_FAST_LINK_FAILURE_SETTING_MASK             0x000FU


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned CLK_SEL : 3;
        unsigned : 1;
        unsigned CLK_SQUELCH_LVL : 2;
        unsigned : 2;
        unsigned CLK_FREQ_SELECT : 3;
        unsigned : 4;
        unsigned EN_RCVRD_CLK : 1;
    };
    unsigned short w;
} VSC8541_REG_EXTG_RECOVERED_CLK_CTRL_bits_t; // reg 23G: 
#define M_EXT_GP_EN_RCVRD_CLK_MASK        0x8000U
#define M_EXT_GP_CLK_FREQ_SELECT_MASK     0x0700U
#define M_EXT_GP_CLK_SQUELCH_LVL_MASK     0x0030U
#define M_EXT_GP_CLK_SEL_MASK             0x0004U


typedef union 
{
    struct __attribute__((packed)) 
    {  
        unsigned : 8;
        unsigned LED_PULSE_DUTY_CYCLE_CTRL : 8;
    };
    unsigned short w;
} VSC8541_REG_EXTG_ENHANCED_LED_CTRL_bits_t; // reg 25G: 
#define M_EXT_GP_LED_PULSE_DUTY_CYCLE_CTRL_MASK            0xFF000U



/*******************************************************************************
  Summary :
    Defines the phases of the VSC8541 Phy configuration

  Description :
    This enumeration defines all the configuration phase of the VSC8541 when put in Managed mode.
 ******************************************************************************/
typedef enum
{

  /*Select the Main register page to access to the Phy Control 1 and base Mode Control registers*/
  MAIN_REG_SEL1 = 0,
  /*Read the Phy Control 1 register to initiate the configuration of the RMII or the MII mode*/
  PHYCTRL1_READ,
  /* Modify the Phy Control 1 register to change the VSC8541 Mii or RMII mode*/
  PHYCTRL1_MODIFY,
  /*Write the Phy Control 1 register*/
  PHYCTRL1_WRITE,
  /*Read the Base Mode Control register to initiate the soft reset of the VSC8541*/
  BMCON_RST_READ,
  /*Modify the Base Mode Control register to Soft reset the VSC8541*/
  BMCON_RST_MODIFY,
  /*Write the Base Mode Control register*/
  BMCON_RST_WRITE,
  /*Select the Main register page to end the configuration*/
  MAIN_REG_SEL2,
}VSC8541_CONF_PHASE;



#endif  // H_VSC8541_PRIV_H_



    
