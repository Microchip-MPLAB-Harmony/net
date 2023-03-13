/*******************************************************************************
  VSC8540 definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_vsc8540.h

  Summary:
    VSC8540 definitions

  Description:
    This file provides the VSC8540 definitions.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2017-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _VSC8540_H_

#define _VSC8540_H_

typedef enum
{
    /*
    // basic registers, accross all registers: 0-1
    PHY_REG_BMCON  = 0,
    PHY_REG_BMSTAT  = 1,
    // extended registers: 2-15
    PHY_REG_PHYID1  = 2,
    PHY_REG_PHYID2  = 3,
    PHY_REG_ANAD  = 4,
    PHY_REG_ANLPAD  = 5,
    PHY_REG_ANLPADNP = 5,
    PHY_REG_ANEXP  = 6,
    PHY_REG_ANNPTR  = 7,
    PHY_REG_ANLPRNP  = 8,
    */
    // specific vendor registers: 16-31

    PHY_MMD_ACCESS_CONTROL = 13,
    PHY_MMD_ACCESS_DATA_ADDR = 14,
    PHY_1000BASET_STATUS_EXENTSION_1 = 15,
    PHY_100BASE_TX_STATUS_EXENTSION = 16,
    PHY_1000BASE_T_STATUS_EXENTSION_2 = 17,
    PHY_REG_BYPASS_RMII = 18,
    PHY_REG_ERR_COUNTER_1 = 19,
    PHY_REG_ERR_COUNTER_2 = 20,
    PHY_REG_ERR_COUNTER_3 = 21,
    PHY_REG_EXTEND_CONTROL_STATUS = 22,
    PHY_REG_EXTEND_PHY_CONTROL_1 = 23,
    PHY_REG_EXTEND_PHY_CONTROL_2 = 24,
    PHY_REG_INTERRUPT_MASK = 25,
    PHY_REG_INTERRUPT_STATUS = 26,
    PHY_REG_AUXILIARY_CONTROL_STATUS = 28,
    PHY_REG_LED_MODE_SELECT = 29,
    PHY_REG_lED_BEHAVIOR = 30,
    PHY_REG_EXTENDED_PAGE_ACCESS = 31,
    //
    // PHY_REGISTERS  = 32 // total number of registers

    // Extend Page 1 Registers
    PHY_EXT1_CU_MEDIA_CRC_GOOD_COUNTER = 18,
    PHY_EXT1_EXTENDED_MODE_CTRL = 19,
    PHY_EXT1_PHY_CTRL_3 = 20,
    PHY_EXT1_PHY_CTRL_4 = 23,
    PHY_EXT1_ETH_PACKET_GEN_1 = 29,
    PHY_EXT1_ETH_PACKET_GEN_2 = 30,

    // Extend Page 2 Registers
    PHY_EXT2_CU_PMD_TRANSMIT_CTRL = 16,
    PHY_EXT2_EEE_CTRL = 17,
    PHY_EXT2_RGMII_CTRL = 20,
    PHY_EXT2_WoL_MAC_ADD_15_0 = 21,
    PHY_EXT2_WoL_MAC_ADD_31_16 = 22,
    PHY_EXT2_WoL_MAC_ADD_47_32 = 23,
    PHY_EXT2_SECURE_ON_PASSWORD_15_0 = 24,
    PHY_EXT2_SECURE_ON_PASSWORD_31_16 = 25,
    PHY_EXT2_SECURE_ON_PASSWORD_47_32 = 26,
    PHY_EXT2_WoL_MAC_INTERFACE_CTRL = 27,
    PHY_EXT2_EXTENDED_INTRPT_MASK = 28,
    PHY_EXT2_EXTENDED_INTRPT_STATUS = 29,

    // Extend General Purpose Registers
    PHY_EXT_GP_CLKOUT_CTRL = 13,
    PHY_EXT_GP_GPIO_CTRL_2 = 14,
    PHY_EXT_GP_FAST_LINK_REG = 19,
    PHY_EXT_GP_RECOVERED_CLK_CTRL = 23,
    PHY_EXT_GP_ENHANCED_LED_CTRL = 25,


}VSC8540_VENDOR_REG;

/*----------------------------------------------------------------*/

// vendor registers
//
typedef union {
  struct {    
    unsigned DVAD:5;
    unsigned :9;
    unsigned FUNCTION :2;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_MMD_ACCESS_CONTROL_bits_t; // reg 13
#define _MMD_ACCESS_CTRL_FUNCTION_MASK  0xc000
#define _MMD_ACCESS_CTRL_DVAD_MASK      0x001f


typedef union {
  struct {    
    unsigned REGISTER_ADD_DATA:16;
    
  };
  struct {
    unsigned short w:16;
  };
} __PHY_MMD_ACCESS_DATA_ADDR_bits_t; // reg 14: PHY_MMD_ACCESS_DATA_ADDR
#define _MMD_REGISTER_ADD_DATA_MASK     0x0002


typedef union {
  struct {    
    unsigned :8;
    unsigned ESD_ERR:1;
    unsigned SSD_ERR:1;
    unsigned TRANSMIT_ERR:1;
    unsigned RCVD_ERR:1;
    unsigned CURRENT_LINK_STATUS:1;
    unsigned DISCONNECT_STATE:1;
    unsigned LOCK_ERR:1;
    unsigned DESCRAMBLER:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_100BASE_TX_STATUS_EXENTSION_bits_t; // reg 16: 
#define _100BASE_TX_DESCRAMBLER_MASK            0x8000
#define _100BASE_TX_LOCK_ERR_MASK               0x4000
#define _100BASE_TX_DISCONNECT_STATE_MASK       0x2000
#define _100BASE_TX_CURRENT_LINK_STATUS_MASK    0x1000
#define _100BASE_TX_RCVD_ERR_MASK               0x0800
#define _100BASE_TX_TRANSMIT_ERR_MASK           0x0400
#define _100BASE_TX_SSD_ERR_MASK                0x0200
#define _100BASE_TX_ESD_ERR_MASK                0x0100


typedef union {
  struct {    
    unsigned :3;
    unsigned PARALLEL_DETECT_CTRL:1;
    unsigned DISABLE_POLARITY_CORRECTION:1;
    unsigned DISABLE_PAIR_SWAP_CORRECTION:1;
    unsigned :1;
    unsigned HP_AUTO_MDIX:1;
    unsigned :1;
    unsigned LFI_TIMER:1;
    unsigned PCS_TRANSMIT:1;
    unsigned PCS_RECEIVED:1;
    unsigned BYPASS_DESCRAMBLER:1;
    unsigned BYPASS_SCRAMBLER:1;
    unsigned _4B5B_EN_DE_CODER:1;
    unsigned TRANSMIT_DISABLE:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_BYPASS_RMII_bits_t; // reg 18: 
#define _BYPASS_RMII_TRANSMIT_DISABLE_MASK                 0x8000
#define _BYPASS_RMII_4B5B_EN_DE_CODER_MASK                 0x4000
#define _BYPASS_RMII_BYPASS_SCRAMBLER_MASK                 0x2000
#define _BYPASS_RMII_BYPASS_DESCRAMBLER_MASK               0x1000
#define _BYPASS_RMII_PCS_RECEIVED_MASK                     0x0800
#define _BYPASS_RMII_PCS_TRANSMIT_MASK                     0x0400
#define _BYPASS_RMII_LFI_TIMER_MASK                        0x0100
#define _BYPASS_RMII_HP_AUTO_MDIX_MASK                     0x0040
#define _BYPASS_RMII_DISABLE_PAIR_SWAP_CORRECTION_MASK     0x0020
#define _BYPASS_RMII_DISABLE_POLARITY_CORRECTION_MASK      0x0010
#define _BYPASS_RMII_PARALLEL_DETECT_CTRL_MASK             0x0008


typedef union {
  struct {    
    unsigned RECEIVED_ERR_COUNTER:8;
    unsigned :8;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_ERR_COUNTER_1_bits_t; // reg 19: 
#define _ERR_COUNTER_1_COUNTER_ERR_MASK                     0x00FF


typedef union {
  struct {    
    unsigned FALSE_CARRIER_COUNTER:8;
    unsigned :8;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_ERR_COUNTER_2_bits_t; // reg 20: 
#define _ERR_COUNTER_2_FALSE_CARRIER_COUNTER_MASK         0x00FF


typedef union {
  struct {    
    unsigned COPPER_LINK_DISCONNECT_COUNTER:8;
    unsigned :8;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_ERR_COUNTER_3_bits_t; // reg 21: 
#define _ERR_COUNTER_3_COPPER_LINK_COUNTER_MASK         0x00FF


typedef union {
  struct {    
    unsigned SMI_BROADCAST_WR:1;
    unsigned :5;
    unsigned _10BASE_T_LINK_STATUS:1;
    unsigned _10BASE_T_DISCONNECT_STATE:1;
    unsigned EOF_ERR:1;
    unsigned STICKY_RST_ENABLE:1;
    unsigned _10BASE_T_SQUELCH_CTRL:2;
    unsigned SQE_DISABLE:1;
    unsigned _10BASE_T_ECHO_DISABLE:1;
    unsigned JABBER_DETECT_DISABLE:1;
    unsigned _10BASE_T_LINK_H:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_EXTEND_CONTROL_STATUS_bits_t; // reg 22: 
#define _EXT_CTRL_STATUS_10BASE_T_LINK_H_MASK             0x8000
#define _EXT_CTRL_STATUS_JABBER_DETECT_DISABLE_MASK       0x4000
#define _EXT_CTRL_STATUS_10BASE_T_ECHO_DISABLE_MASK       0x2000
#define _EXT_CTRL_STATUS_SQE_DISABLE_MASK                 0x1000
#define _EXT_CTRL_STATUS_10BASE_T_SQUELCH_CTRL_MASK       0x0C00
#define _EXT_CTRL_STATUS_STICKY_RST_ENABLE_MASK           0x0200
#define _EXT_CTRL_STATUS_EOF_ERR_MASK                     0x0100
#define _EXT_CTRL_STATUS_10BASE_T_DISCONNECT_STATE_MASK   0x0080
#define _EXT_CTRL_STATUS_10BASE_T_LINK_STATUS_MASK        0x0040
#define _EXT_CTRL_STATUS_SMI_BROADCAST_WR_MASK            0x0000


typedef union {
  struct {
    unsigned :3;
    unsigned FAREND_LOOPBACK_MODE:1;
    unsigned :7;
    unsigned MAC_INTERFACE_SEL:2;
    unsigned MAC_CLK_ENABLE:1;    
    unsigned :2;
    };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_EXTEND_PHY_CONTROL_1_bits_t; // reg 23: 
#define _EXT_PHY_CTRL_MAC_CLK_ENABLE_MASK            0x2000
#define _EXT_PHY_CTRL_MAC_INTERFACE_SEL_MASK         0x1800
#define _EXT_PHY_CTRL_FAREND_LOOPBACK_MODE_MASK      0x0008


typedef union {
  struct {    
    unsigned CONNECTOR_LOOPBACK:1;
    unsigned :4;
    unsigned JUMBO_PCKT_MODE:2;
    unsigned :6;
    unsigned PICMG_REDUCE_PWR_MODE:1;
    unsigned _100BASE_T_EDGE_RATE_CTRL:3;
    };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_EXTEND_PHY_CONTROL_2_bits_t; // reg 24: 
#define _EXT_PHY_CTRL_STATUS_100BASE_T_EDGE_RATE_CTRL_MASK    0xe000
#define _EXT_PHY_CTRL_STATUS_PICMG_REDUCE_PWR_MODE_MASK       0x1000
#define _EXT_PHY_CTRL_STATUS_JUMBO_PCKT_MODE_MASK             0x0030
#define _EXT_PHY_CTRL_STATUS_CONNECTOR_LOOPBACK_MASK          0x0000


typedef union {
  struct {    
    unsigned RX_ER_INTRPT_MASk:1;
    unsigned :1;
    unsigned :1;
    unsigned FALSE_CARRRIER_INTRPT_MASK:1;
    unsigned :1;
    unsigned EXTEND_INTRPT_MASK:1;
    unsigned WAKEonLAN_EVENT_INTRPUT_MASK:1;
    unsigned FAST_LINK_FAILURE_INTRPT_MASK:1;
    unsigned SYMBOL_ERROR_INTRPT_MASK:1;
    unsigned PoE_DETECT_MASK:1;
    unsigned AUTONEG_COMPLETE_MASK:1;
    unsigned AUTONEG_ERROR_MASK:1;
    unsigned FXD_STATE_CHANGE_MASK:1;
    unsigned LINK_STATE_CHANGE_MASK:1;
    unsigned SPEED_STATE_CHANGE_MASK:1;
    unsigned MDINT_INTRUPT_STATUS_EN:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_PHY_REG_INTERRUPT_MASK_bits_t; // reg 25: 
#define _MDINT_INTRUPT_STATUS_EN_MASK              0x8000
#define _SPEED_STATE_CHANGE_MASK_MASK              0x4000
#define _LINK_STATE_CHANGE_MASK_MASK               0x2000
#define _FXD_STATE_CHANGE_MASK_MASK                0x1000
#define _AUTONEG_ERROR_MASK_MASK                   0x0800
#define _AUTONEG_COMPLETE_MASK_MASK                0x0400
#define _PoE_DETECT_MASK_MASK                      0x0200
#define _SYMBOL_ERROR_INTRPT_MASK_MASK             0x0100
#define _FAST_LINK_FAILURE_INTRPT_MASK_MASK        0x0080
#define _WAKEonLAN_EVENT_INTRPUT_MASK_MASK         0x0040
#define _EXTEND_INTRPT_MASK_MASK                   0x0020
#define _FALSE_CARRRIER_INTRPT_MASK_MASK           0x0008
#define _RX_ER_INTRPT_MASk _MASK                   0x0001


typedef union {
  struct {    
    unsigned RX_ER_INTRPT_MASk:1;
    unsigned :1;
    unsigned :1;
    unsigned FALSE_CARRRIER_INTRPT_STATUS:1;
    unsigned :1;
    unsigned EXTEND_INTRPT_STATUS:1;
    unsigned WAKEonLAN_EVENT_INTRPUT_STATUS:1;
    unsigned FAST_LINK_FAILURE_INTRPT_STATUS:1;
    unsigned SYMBOL_ERROR_INTRPT_STATUS:1;
    unsigned PoE_DETECT_STATUS:1;
    unsigned AUTONEG_COMPLETE_STATUS:1;
    unsigned AUTONEG_ERROR_STATUS:1;
    unsigned FXD_STATE_CHANGE_STATUS:1;
    unsigned LINK_STATE_CHANGE_STATUS:1;
    unsigned SPEED_STATE_CHANGE_STATUS:1;
    unsigned INTRUPT_STATUS:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_INTERRUPT_STATUS_bits_t; // reg 26: 
#define _MDINT_INTRUPT_STATUS_MASK                   0x8000
#define _SPEED_STATE_CHANGE_STATUS_MASK              0x4000
#define _LINK_STATE_CHANGE_STATUS_MASK               0x2000
#define _FXD_STATE_CHANGE_STATUS_MASK                0x1000
#define _AUTONEG_ERROR_STATUS_MASK                   0x0800
#define _AUTONEG_COMPLETE_STATUS_MASK                0x0400
#define _PoE_DETECT_STATUS_MASK                      0x0200
#define _SYMBOL_ERROR_INTRPT_STATUS_MASK             0x0100
#define _FAST_LINK_FAILURE_INTRPT_STATUS_MASK        0x0080
#define _WAKEonLAN_EVENT_INTRPUT_STATUS_MASK         0x0040
#define _EXTEND_INTRPT_STATUS_MASK                   0x0020
#define _FALSE_CARRRIER_INTRPT_STATUS_MASK           0x0008
#define _RX_ER_INTRPT_STATUS _MASK                   0x0001


typedef union {
  struct {    
    unsigned MEDIA_MODE_STATUS:2;
    unsigned ACTIPHY_LINK_STATUS_TIMEOUT_CTRL_0:1;
    unsigned SPEED_STATUS:2;
    unsigned FDX_STATUS:1;
    unsigned ACTIPHY_ENABLE:1;
    unsigned ACTIPHY_LINK_STATUS_TIMEOUT_CTRL_1:1;
    unsigned :2;
    unsigned B_POLARITY_INVERSION:1;
    unsigned A_POLARITY_INVERSION:1;
    unsigned :1;
    unsigned HP_AUTO_MDIX_CROSS_INDICATION:1;
    unsigned AUTONEG_DISABLE:1;
    unsigned AUTONEG_COMPLETE:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_AUXILIARY_CONTROL_STATUS_bits_t; // reg 28: 
#define _AUTONEG_COMPLETE_MASK                      0x8000
#define _AUTONEG_DISABLE_MASK                       0x4000
#define _HP_AUTO_MDIX_CROSS_INDICATION_MASK         0x2000
#define _A_POLARITY_INVERSION_MASK                  0x0800
#define _B_POLARITY_INVERSION_MASK                  0x0400
#define _ACTIPHY_LINK_STATUS_TIMEOUT_CTRL_1_MASK    0x0080
#define _ACTIPHY_ENABLE_MASK                        0x0040
#define _FDX_STATUS_MASK                            0x0020
#define _SPEED_STATUS_MASK                          0x0018
#define _ACTIPHY_LINK_STATUS_TIMEOUT_CTRL_0_MASK    0x0004
#define _MEDIA_MODE_STATUS_MASK                     0x0003


typedef union {
  struct {    
    unsigned LED2_MODE_SEL:4;
    unsigned LED1_MODE_SEL:4;
    unsigned :7;
    unsigned :1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_LED_MODE_SELECT_bits_t; // reg 29
#define _LED1_MODE_SEL_MASK  0x00F0
#define _LED2_MODE_SEL_MASK  0x000F


typedef union {
  struct {    
    unsigned LED0_COMBINE_FEATURES_DISABLE:1;
    unsigned LED1_COMBINE_FEATURES_DISABLE:1;
    unsigned :3;
    unsigned LED0_PULSE_BLINK_SEL:1;
    unsigned LED1_PULSE_BLINK_SEL:1;
    unsigned :3;
    unsigned LED_PULSE_BLINK_RATE:2;
    unsigned LED_PULSING_EN:1;
    unsigned :3;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_lED_BEHAVIOR_bits_t; // reg 30: 
#define _LED_PULSING_EN_MASK                     0x1000
#define _LED_PULSE_BLINK_RATE_MASK               0x0c00
#define _LED1_PULSE_BLINK_SEL_MASK               0x0040
#define _LED0_PULSE_BLINK_SEL_MASK               0x0020
#define _LED1_COMBINE_FEATURES_DISABLE_MASK      0x0002
#define _LED0_COMBINE_FEATURES_DISABLE_1_MASK    0x0001


typedef union {
  struct {    
    unsigned EXT_PAGE_REGISTER_ACCES:16;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_REG_EXTENDED_PAGE_ACCESS_bits_t; // reg 31: 
#define _LED_PULSING_EN_VSC8540_MASK               0xFFFF



/*----------------------------------------------------------------*/
//Extend Page 1 Registers


typedef union {
  struct {    
    unsigned CRC_GOOD_COUNTER_CONTENTS:14;
    unsigned :1;
    unsigned PCKT_SINCE_LAST_READ:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT1_CU_MEDIA_CRC_GOOD_COUNTER_bits_t; // reg 18: 
#define _PCKT_SINCE_LAST_READ_MASK               0x8000
#define _CRC_GOOD_COUNTER_CONTENTS_MASK          0x3fff


typedef union {
  struct {  
    unsigned :1;
    unsigned FORCE_MDI_CROSSOVER:2;
    unsigned FAST_LINK_FAILURE:1;
    unsigned :6;
    unsigned LED_RST_BLINK_SUPPRESS:3;
    unsigned LED0_EXT_MODE:1;
    unsigned LED1_EXT_MODE:1;
    unsigned :2;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT1_EXTENDED_MODE_CTRL_bits_t; // reg 19: 
#define _LED1_EXT_MODE_MASK               0x2000
#define _LED0_EXT_MODE_MASK               0x1000
#define _LED_RST_BLINK_SUPPRESS_MASK      0x0800
#define _FAST_LINK_FAILURE_MASK           0x0010
#define _FORCE_MDI_CROSSOVER_MASK         0x000c


typedef union {
  struct {  
    unsigned :5;
    unsigned EN_10BASET_NO_PREAMBLE_MODE:1;
    unsigned MEDIA_MODE_STATUS_B:2;
    unsigned :2;
    unsigned SLOW_MDC:1;
    unsigned ACTIPHY_WAKE_UP_TIMER:2;
    unsigned ACTIPHY_SLEEP_TIMER:2;
    unsigned :1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT1_PHY_CTRL_3_bits_t; // reg 20: 
#define _ACTIPHY_SLEEP_TIMER_MASK                 0x6000
#define _ACTIPHY_WAKE_UP_TIMER_MASK               0x1800
#define _SLOW_MDC_MASK                            0x0400
#define _uMEDIA_MODE_STATUS_B_MASK                0x00c0
#define _EN_10BASET_NO_PREAMBLE_MODE_MASK         0x0020


typedef union {
  struct {  
    unsigned CU_MEDIA_CRC_ERROR_COUNTER:8;
    unsigned INLINE_PWRD_DEVICE_DETECT_STATUS:2;
    unsigned INLINE_PWRD_DEVICE_DETECT:1;
    unsigned PHY_ADDR:5;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT1_PHY_CTRL_4_bits_t; // reg 23: 
#define _PHY_ADDR_MASK                          0xf800
#define _INLINE_PWRD_DEVICE_DETECT_MASK         0x0400
#define _INLINE_PWRD_DEVICE_DETECT_STATUS_MASK  0x0300
#define _CU_MEDIA_CRC_ERROR_COUNTER_MASK        0x00FF


typedef union {
  struct {  
    unsigned BAD_FCS_GEN:1;
    unsigned PAYLOAD_TYPE:1;
    unsigned SOURCE_ADDR:4;
    unsigned DEST_ADDR:4;
    unsigned INTERPACKET_GAP:1;
    unsigned PCKT_LENGTH:2;
    unsigned TRANSMISSION_DURATION:2;
    unsigned EPG_RUN_STOP:1;
    unsigned EPG_ENABLE:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT1_ETH_PACKET_GEN_1_bits_t; // reg 20: 
#define _EPG_ENABLE_MASK                  0x8000
#define _EPG_RUN_STOP_MASK                0x4800
#define _TRANSMISSION_DURATION_MASK       0x2000
#define _PCKT_LENGTH_MASK                 0x1800
#define _INTERPACKET_GAP_MASK             0x0420
#define _DEST_ADDR_MASK                   0x03c0
#define _SOURCE_ADDR_MASK                 0x003c
#define _PAYLOAD_TYPE_MASK                0x0002
#define _BAD_FCS_GEN_MASK                 0x0001

typedef union {
  struct {  
    unsigned EPG_PCKT_PAYLOAD:16;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT1_ETH_PACKET_GEN_2_bits_t; // reg 23: 
#define _EPG_PCKT_PAYLOAD_MASK                         0xffff


/*----------------------------------------------------------------*/
//Extend Page 2 Registers


typedef union {
  struct {  
    unsigned _100BASETX_SIGNAL_AMPLI_TRIM :4;
    unsigned _10BASET_SIGNAL_AMPLI_TRIM :4;
    unsigned _10BASETe_SIGNAL_AMPLI_TRIM :4;
    unsigned :4;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_CU_PMD_TRANSMIT_CTRL_bits_t; // reg 16: 
#define _100BASETX_SIGNAL_AMPLI_TRIM_MASK          0x0f00
#define _10BASET_SIGNAL_AMPLI_TRIM_MASK            0x00f0
#define _10BASETe_SIGNAL_AMPLI_TRIM_MASK           0x000f


typedef union {
  struct {  
    unsigned :2;
    unsigned INHIBIT_100BTX_RECEIVED_EEE_LPI:1;
    unsigned INHIBIT_100BTX_TRANSMIT_EEE_LPI:1;
    unsigned FORCE_TRANSMIT_LPI:1;
    unsigned :1;
    unsigned _100BASETX_EEE_EN:1;
    unsigned :1;
    unsigned LINK_STATUS:1;
    unsigned :1;
    unsigned INVERT_LED_POLARITY:2;
    unsigned :3;
    unsigned EN_10BASETe:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_EEE_CTRL_bits_t; // reg 17: 
#define _EN_10BASETe_MASK                         0x8000
#define _INVERT_LED_POLARITY_MASK                 0x0c00
#define _LINK_STATUS_MASK                         0x0100
#define _100BASETX_EEE_EN_MASK                    0x0040
#define _FORCE_TRANSMIT_LPI_MASK                  0x0010
#define _INHIBIT_100BTX_TRANSMIT_EEE_LPI_MASK     0x0008
#define _INHIBIT_100BTX_RECEIVED_EEE_LPI_MASK     0x0004


typedef union {
  struct {  
    unsigned TX_CLK_DELAY:3;
    unsigned RGMII_MII_TXD_BIT_REVERSAL:1;
    unsigned RX_CLK_DELAY:3;
    unsigned RGMII_MII_RXD_BIT_REVERSAL:1;
    unsigned :4;
    unsigned SOF_EN:1;
    unsigned :2;
    unsigned FLF2_EN:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_RGMII_CTRL_bits_t; // reg 20: 
#define _FLF2_EN_MASK                       0x8000
#define _SOF_EN_MASK                        0x1000
#define _RGMII_MII_RXD_BIT_REVERSAL_MASK    0x0080
#define _RX_CLK_DELAY_MASK                  0x0070
#define _RGMII_MII_TXD_BIT_REVERSAL_MASK    0x0008
#define _TX_CLK_DELAY_MASK                  0x0007


typedef union {
  struct {  
    unsigned WoL_MAC_ADD_15_0:16;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_WoL_MAC_ADD_15_0_bits_t; // reg 21: 
#define _WoL_MAC_ADD_15_0_MASK                         0xffff


typedef union {
  struct {  
    unsigned WoL_MAC_ADD_31_16:16;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_WoL_MAC_ADD_31_16_bits_t; // reg 22: 
#define _WoL_MAC_ADD_31_16_MASK                         0xffff


typedef union {
  struct {  
    unsigned WoL_MAC_ADD_47_32:16;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_WoL_MAC_ADD_47_32_bits_t; // reg 23: 
#define _WoL_MAC_ADD_47_32_MASK                         0xffff


typedef union {
  struct {  
    unsigned SECURE_ON_PASSWORD_15_0:16;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_SECURE_ON_PASSWORD_15_0_bits_t; // reg 24: 
#define _SECURE_ON_PASSWORD_15_0_MASK                         0xffff

typedef union {
  struct {  
    unsigned SECURE_ON_PASSWORD_31_16:16;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_SECURE_ON_PASSWORD_31_16_bits_t; // reg 25: 
#define _SECURE_ON_PASSWORD_31_16_MASK                         0xffff


typedef union {
  struct {  
    unsigned SECURE_ON_PASSWORD_47_32:16;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_SECURE_ON_PASSWORD_47_32_bits_t; // reg 26: 
#define _SECURE_ON_PASSWORD_47_32_MASK                         0xffff


typedef union {
  struct {  
    unsigned MDINT_CMOS_drive:1;
    unsigned :3;
    unsigned RMII_CLKOUT_EN:1;
    unsigned PAD_EDGE_RATE_CTRL:3;
    unsigned ADD_REP_COUNT_MAGIC_PCKT:4;
    unsigned :2;
    unsigned SECURE_ON_PASSWORD_LENGTH:1;
    unsigned SECURE_ON_EN:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_WoL_MAC_INTERFACE_CTRL_bits_t; // reg 27: 
#define _SECURE_ON_EN_MASK                  0x8000
#define _SECURE_ON_PASSWORD_LENGTH_MASK     0x4000
#define _ADD_REP_COUNT_MAGIC_PCKT_MASK      0x00f0
#define _PAD_EDGE_RATE_CTRL_MASK            0x00e0
#define _RMII_CLKOUT_EN_MASK                0x0010
#define _MDINT_CMOS_drive_MASK              0x0001


typedef union {
  struct {  
    unsigned EEE_WAKE_ERR_INTRPT_MASK:1;
    unsigned EEE_WAIT_QUIET_TS_TIMER_INTRPT_MASK:1;
    unsigned EEE_RX_TQ_TIMER_INTRPT_MASK:1;
    unsigned EEE_LINK_FAIL_INTRPT_MASK:1;
    unsigned :8;
    unsigned TX_FIFO_FLOW_INTRPT_MASK:1;
    unsigned RX_FIFO_FLOW_INTRPT_MASK:1;
    unsigned :2;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_EXTENDED_INTRPT_MASK_bits_t; // reg 28: 
#define _RX_FIFO_FLOW_INTRPT_MASK_MASK              0x2000
#define _TX_FIFO_FLOW_INTRPT_MASK_MASK              0x1000
#define _EEE_LINK_FAIL_INTRPT_MASK_MASK             0x0004
#define _EEE_RX_TQ_TIMER_INTRPT_MASK_MASK           0x0003
#define _EEE_WAIT_QUIET_TS_TIMER_INTRPT_MASK_MASK   0x0002
#define _EEE_WAKE_ERR_INTRPT_MASK_MASK              0x0001


typedef union {
  struct {  
    unsigned EEE_WAKE_ERR_INTRPT_STATUS:1;
    unsigned EEE_WAIT_QUIET_TS_TIMER_INTRPT_STATUS:1;
    unsigned EEE_RX_TQ_TIMER_INTRPT_STATUS:1;
    unsigned EEE_LINK_FAIL_INTRPT_STATUS:1;
    unsigned :8;
    unsigned TX_FIFO_FLOW_INTRPT_STATUS:1;
    unsigned RX_FIFO_FLOW_INTRPT_STATUS:1;
    unsigned :2;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT2_EXTENDED_INTRPT_STATUS_bits_t; // reg 29: 
#define _RX_FIFO_FLOW_INTRPT_STATUS_MASK              0x2000
#define _TX_FIFO_FLOW_INTRPT_STATUS_MASK              0x1000
#define _EEE_LINK_FAIL_INTRPT_STATUS_MASK             0x0004
#define _EEE_RX_TQ_TIMER_INTRPT_STATUS_MASK           0x0003
#define _EEE_WAIT_QUIET_TS_TIMER_INTRPT_STATUS_MASK   0x0002
#define _EEE_WAKE_ERR_INTRPT_STATUS_MASK              0x0001


/*----------------------------------------------------------------*/
//General Purpose Registers


typedef union {
  struct {  
    unsigned :13;
    unsigned CLKOUT_FREQ_SELECT:2;
    unsigned CLKOUT_EN:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT_GP_CLKOUT_CTRL_bits_t; // reg 13: 
#define _CLKOUT_EN_MASK             0x8000
#define _CLKOUT_FREQ_SELECT_MASK    0x6000


typedef union {
  struct {  
    unsigned :9;
    unsigned TRISTATE_EN_LED:1;
    unsigned :1;
    unsigned COMA_MODE_INPUT_DATA:1;
    unsigned COMA_MODE_OUTPUT_DATA:1;
    unsigned COMA_MODE_OUTPUT_EN:1;
    unsigned :2;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT_GP_GPIO_CTRL_2_bits_t; // reg 14: 
#define _COMA_MODE_OUTPUT_EN_MASK       0x2000
#define _COMA_MODE_OUTPUT_DATA_MASK     0x1000
#define _COMA_MODE_INPUT_DATA_MASK      0x0800
#define _TRISTATE_EN_LED_MASK           0x0200


typedef union {
  struct {  
    unsigned FAST_LINK_FAILURE_SETTING:4;
    unsigned :12;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT_GP_FAST_LINK_REG_bits_t; // reg 19: 
#define _FAST_LINK_FAILURE_SETTING_MASK             0x000f


typedef union {
  struct {  
    unsigned CLK_SEL:3;
    unsigned :1;
    unsigned CLK_SQUELCH_LVL:2;
    unsigned :2;
    unsigned CLK_FREQ_SELECT:3;
    unsigned :4;
    unsigned EN_RCVRD_CLK:1;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT_GP_RECOVERED_CLK_CTRL_bits_t; // reg 23: 
#define _EN_RCVRD_CLK_MASK        0x8000
#define _CLK_FREQ_SELECT_MASK     0x0700
#define _CLK_SQUELCH_LVL_MASK     0x0030
#define _CLK_SEL_MASK             0x0004


typedef union {
  struct {  
    unsigned :8;
    unsigned LED_PULSE_DUTY_CYCLE_CTRL:8;
  };
  struct {
    unsigned short w:16;
  };
} __PHY_EXT_GP_ENHANCED_LED_CTRL_bits_t; // reg 25: 
#define _LED_PULSE_DUTY_CYCLE_CTRL_MASK            0xff000


/*******************************************************************************
  Summary:
    Defines the phases of the VSC8540 Phy configuration

  Description:
    This enumeration defines all the configuration phase of the VSC8540 when put in Managed mode.
 ******************************************************************************/
typedef enum {

  /*Select the Main register page to access to the Phy Control 1 and base Mode Control registers*/
  MAIN_REG_SEL1 = 0,
  /*Read the Phy Control 1 register to initiate the configuration of the RMII or the MII mode*/
  PHYCTRL1_READ = 1,
  /* Modify the Phy Control 1 register to change the VSC8540 Mii or RMII mode*/
  PHYCTRL1_MODIFY = 2,
  /*Write the Phy Control 1 register*/
  PHYCTRL1_WRITE = 3,
  /*Read the Base Mode Control register to initiate the soft reset of the VSC8540*/
  BMCON_RST_READ = 4,
  /*Modify the Base Mode Control register to Soft reset the VSC8540*/
  BMCON_RST_MODIFY = 5,
  /*Write the Base Mode Control register*/
  BMCON_RST_WRITE = 6,
  /*Read the Base Mode Control register to initiate the configuration of the duplex mode, the autonegotiation and the speed of the VSC8540*/
  BMCON_CONFIG_READ = 7,
  /*Modify the Base Mode Control register to change the duplex mode, the autonegotiation and the speed of the VSC8540*/
  BMCON_CONFIG_MODIFY = 8,
  /*Write the Base Mode Control register*/
  BMCON_CONFIG_WRITE = 9,
  /*Select the Extended Page 2 to access the RGMII Control register*/
  EXT_PAGE_2_SEL = 10,
  /*Read the RGMII Control register to initiate the configuration of the Rx and Tx delay*/
  RGMII_CTRL_READ = 11,
  /*Modify the RGMII Control register to configure of the Rx and Tx delay*/
  RGMII_CTRL_MODIFY = 12,
  /*Write the RGMII Control register*/
  RGMII_CTRL_WRITE = 13,
  /*Select the Extended Page 2 to access the CLKOUT Control register*/
  GP_REG_SEL = 14,
  /*Read the CLKOUT Control register to initiate the configuration of the CLKOUT*/
  CLKOUT_CTRL_READ = 15,
  /*Modify the CLKOUT Control register to enbale CLKOUT and select the frequency*/
  CLKOUT_CTRL_MODIFY = 16,
  /*Write the CLKOUT Control register*/
  CLKOUT_CTRL_WRITE = 17,
  /*Select the Main register page to end the configuration*/
  MAIN_REG_SEL2 = 18

}VSC8540_CONF_PHASE;

#endif  // _VSC8540_H_



    
