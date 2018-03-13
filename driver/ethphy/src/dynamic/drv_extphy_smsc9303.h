/*******************************************************************************
  SMSC LAN9303 definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    eth_pic32_ext_phy_smsc9303.h

  Summary:
    SMSC LAN9303 definitions

  Description:
    This file provides the SMSC LAN9303 definitions.
 *******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END

#ifndef _SMSC_9303_H_

#define _SMSC_9303_H_
#include "system_config.h"

#include "driver/driver_common.h"
#include "system/debug/sys_debug.h"
#include "system/command/sys_command.h"

typedef enum {
    /*
    // basic registers, across all registers: 0-1
    PHY_REG_BMCON		= 0,
    PHY_REG_BMSTAT		= 1,
    // extended registers: 2-15
    PHY_REG_PHYID1		= 2,
    PHY_REG_PHYID2		= 3,
    PHY_REG_ANAD		= 4,
    PHY_REG_ANLPAD		= 5,
    PHY_REG_ANLPADNP	= 5,
    PHY_REG_ANEXP		= 6,
    PHY_REG_ANNPTR		= 7,
    PHY_REG_ANLPRNP		= 8,
     */

    // Vendor Specific Registers
    //System Control and Status Registers
    PHY_REG_ID_REV = 0x050, // Chip ID and Revision Register
    PHY_REG_IRQ_CFG = 0x054, // Interrupt Configuration Register
    PHY_REG_INT_STS = 0x058, // Interrupt Status Register
    PHY_REG_INT_EN = 0x05C, // Interrupt Enable Register
    PHY_REG_BYTE_TEST = 0x064, // Byte Order Test Register
    PHY_REG_HW_CFG = 0x074, // Hardware Configuration Register
    PHY_REG_GPT_CFG = 0x08C, // General Purpose Timer Configuration Register
    PHY_REG_GPT_CNG = 0x090, // General Purpose Timer Count Register
    PHY_REG_FREE_RUN = 0x09C, // Free Running Counter Register
    PHY_REG_PMI_DATA = 0x0A4, // PHY Management Interface Data Register
    PHY_REG_PMI_ACCESS = 0x0A8, // PHY Management Interface Access Register
    PHY_REG_MANUAL_FC_1 = 0x1A0, // Port 1 Manual Flow Control Register
    PHY_REG_MANUAL_FC_2 = 0x1A4, // Port 2 Manual Flow Control Register
    PHY_REG_MANUAL_FC_0 = 0x1A8, // Port 0 Manual Flow Control Register
    PHY_REG_SWITCH_CSR_DATA = 0x1AC, // Switch Fabric CSR Interface Data Register
    PHY_REG_SWITCH_CSR_CMD = 0x1B0, // Switch Fabric CSR Interface Command Register
    PHY_REG_E2P_CMD = 0x1B4, // EEPROM Command Register
    PHY_REG_E2P_DATA = 0x1B8, // EEPROM Data Register
    PHY_REG_LED_CFG = 0x1BC, // LED Configuration Register
    PHY_REG_VPHY_BASIC_CTRL = 0x1C0, // Virtual PHY Basic Control Register
    PHY_REG_VPHY_BASIC_STATUS = 0x1C4, // Virtual PHY Basic Status Register
    PHY_REG_VPHY_ID_MSB = 0x1C8, // Virtual PHY Identification MSB Register
    PHY_REG_VPHY_ID_LSB = 0x1CC, // Virtual PHY Identification LSB Register
    PHY_REG_VPHY_AN_ADV = 0x1D0, // Virtual PHY Auto-Negotiation Advertisement Register
    PHY_REG_VPHY_AN_LP_BASE_ABILITY = 0x1D4, // Virtual PHY Auto-Negotiation Link Partner Base Page Ability Register
    PHY_REG_VPHY_AN_EXP = 0x1D8, // Virtual PHY Auto-Negotiation Expansion Register
    PHY_REG_VPHY_SPECIAL_CONTROL_STATUS = 0x1DC, // Virtual PHY Special Control/Status Register
    PHY_REG_GPIO_CFG = 0x1E0, // General Purpose I/O Configuration Register
    PHY_REG_GPIO_DATA_DIR = 0x1E4, // General Purpose I/O Data & Direction Register
    PHY_REG_GPIP_INT_STS_EN = 0x1E8, // General Purpose I/O Interupt Status and Enable Register
    PHY_REG_SWITCH_MAC_ADDRH = 0x1F0, // Switch MAC Address High Register
    PHY_REG_SWITCH_MAC_ADDRL = 0x1F4, // Switch MAC Address Low Register
    PHY_REG_RESET_CTL = 0x1F8, // Reset Control Register

    PHY_REG_SW_RESET = 0x200, // Switch Reset Register
    PHY_REG_SW_IMR = 0x204, // Switch GlobalInterrupt Mask Register
    PHY_REG_SF_MAC_RX_CFG_0 = 0x208, // Port 0 MAC Receive Configuration Register
    PHY_REG_SF_MAC_TX_CFG_0 = 0x20C, // Port 0 MAC Transmit Configuration Register
    PHY_REG_SF_MAC_TX_FC_SETTINGS_0 = 0x210, // Port 0 MAC Transmit Flow Control Settings Register
    PHY_REG_SF_MAC_IMR_0 = 0x214, // Port 0 MAC Interrupt Mask Register
    PHY_REG_SF_MAC_RX_CFG_1 = 0x218, // Port 1 MAC Receive Configuration Register
    PHY_REG_SF_MAC_TX_CFG_1 = 0x21C, // Port 1 MAC Transmit Configuration Register
    PHY_REG_SF_MAC_TX_FC_SETTINGS_1 = 0x220, // Port 1 MAC Transmit Flow Control Settings Register
    PHY_REG_SF_MAC_IMR_1 = 0x224, // Port 1 MAC Interrupt Mask Register
    PHY_REG_SF_MAC_RX_CFG_2 = 0x228, // Port 2 MAC Receive Configuration Register
    PHY_REG_SF_MAC_TX_CFG_2 = 0x22C, // Port 2 MAC Transmit Configuration Register
    PHY_REG_SF_MAC_TX_FC_SETTINGS_2 = 0x230, // Port 2 MAC Transmit Flow Control Settings Register
    PHY_REG_SF_MAC_IMR_2 = 0x234, // Port 2 MAC Interrupt Mask Register
    PHY_REG_SF_SWE_ALR_CMD = 0x238, // Switch Engine ALR Command REgister
    PHY_REG_SF_SWE_ALR_WR_DAT_0 = 0x23C, // Switch Engine ALR Write Data 0 Register
    PHY_REG_SF_SWE_ALR_WR_DAT_1 = 0x240, // Switch Engine ALR Write Data 1 Register
    PHY_REG_SF_SWE_ALR_CFG = 0x244, // Switch Engine ALR Configuration Register
    PHY_REG_SF_SWE_VLAN_CMD = 0x248, // Switch Engine VLAN Command Register
    PHY_REG_SF_SWE_VLAN_WR_DATA = 0x24C, // Switch Engine VLAN Write Register
    PHY_REG_SF_SWE_DIFFSERV_TBL_CMD = 0x250, // Switch Engine DIFSERV Table Command Register
    PHY_REG_SF_SWE_DIFFSERV_TBL_WR_DATA = 0x254, // Switch Engine DIFSERV Table Write Data
    PHY_REG_SF_SWE_GLB_INGRESS_CFG = 0x258, // Switch Engine Global Ingress Configuration Register
    PHY_REG_SF_SWE_PORT_INGRESS_CFG = 0x25C, // Switch Engine Port Ingress Configuration Register
    PHY_REG_SF_SWE_ADMT_ONLY_VLAN = 0x260, // Switch Engine Admit Only VLAN Register
    PHY_REG_SF_SWE_PORT_STATE = 0x264, // Switch Engine Port State Register
    PHY_REG_SF_SWE_PRI_TO_QUE = 0x268, // Switch Engine PRiority to Queue Register
    PHY_REG_SF_SWE_PORT_MIRROR = 0x26C, // Switch Engine Port Mirroring Register
    PHY_REG_SF_SWE_INGRESS_PORT_TYPE = 0x270, // Switch Engine Ingress Port Type Register
    PHY_REG_SF_SWE_BCST_THROT = 0x274, // Switch Engine Broadcast Throttling Register
    PHY_REG_SF_SWE_ADMT_N_MEMBER = 0x278, // Swtich Engine Admit Non Member Register
    PHY_REG_SF_SWE_INGRESS_RATE_CFG = 0x27C, // Switch Engine Ingress Rate Configuration Register
    PHY_REG_SF_SWE_INGRESS_RATE_CMD = 0x280, // Switch Engine Ingress Rate Command Register
    PHY_REG_SF_SWE_INGRESS_RATE_WR_DATA = 0x284, // Switch Engine Ingress Rate Write Data Register
    PHY_REG_SF_SWE_INGRESS_REGEN_TBL_0 = 0x288, // Switch Engine Port 0 Learn Discard Count Register
    PHY_REG_SF_SWE_INGRESS_REGEN_TBL_1 = 0x28C, // Switch Engine Port 1 Learn Discard Count Register
    PHY_REG_SF_SWE_INGRESS_REGEN_TBL_2 = 0x290, // Switch Engine Port 2 Learn Discard Count Register
    PHY_REG_SF_SWE_IMR = 0x294, // Switch Engine Interrupt Mask Register
    PHY_REG_SF_BM_CFG = 0x298, // Buffer Manager Configuration Register
    PHY_REG_SF_BM_DROP_LVL = 0x29C, // Buffer Manager Drop Level Register
    PHY_REG_SF_BM_FC_PAUSE_LVL = 0x2A0, // Buffer Manager Flow Control Pause Level Register
    PHY_REG_SF_BM_FC_RESUME_LVL = 0x2A4, // Buffer Manager Flow Control Resume Level Register
    PHY_REG_SF_BM_BCST_LVL = 0x2A8, // Buffer Manager Broadcast Buffer Level Register
    PHY_REG_SF_BM_RNDM_DSCRD_TBL_CMD = 0x2AC, // Buffer Manager Random Discard Table Command Register
    PHY_REG_SF_BM_RNDM_DSCRD_TBL_WDATA = 0x2B0, // Buffer Manager Random Discard Table Write Data Register
    PHY_REG_SF_BM_EGRSS_PORT_TYPE = 0x2B4, // Buffer Manager Egress Port Type Register
    PHY_REG_SF_BM_EGRSS_RATE_00_01 = 0x2B8, // Buffer Manager Port 0 Egress Rate PRiotity Queue 0/1 Register
    PHY_REG_SF_BM_EGRSS_RATE_02_03 = 0x2BC, // Buffer Manager Port 0 Egress Rate PRiotity Queue 2/3 Register
    PHY_REG_SF_BM_EGRSS_RATE_10_11 = 0x2C0, // Buffer Manager Port 1 Egress Rate PRiotity Queue 0/1 Register
    PHY_REG_SF_BM_EGRSS_RATE_12_13 = 0x2C4, // Buffer Manager Port 1 Egress Rate PRiotity Queue 2/3 Register
    PHY_REG_SF_BM_EGRSS_RATE_20_21 = 0x2C8, // Buffer Manager Port 2 Egress Rate PRiotity Queue 0/1 Register
    PHY_REG_SF_BM_EGRSS_RATE_22_23 = 0x2CC, // Buffer Manager Port 2 Egress Rate PRiotity Queue 2/3 Register
    PHY_REG_SF_BM_VLAN_0 = 0x2D0, // Buffer Manager Port 0 Default VLAN ID and PRiority Register
    PHY_REG_SF_BM_VLAN_1 = 0x2D4, // Buffer Manager Port 1 Default VLAN ID and PRiority Register
    PHY_REG_SF_BM_VLAN_2 = 0x2D8, // Buffer Manager Port 2 Default VLAN ID and PRiority Register
    PHY_REG_SF_BM_IMR = 0x2DC, // Buffer Manager Interrupt Mask Register

    PHY_REG_LAST_DIRECT_ACCESS, // Direct Access registers are above this, indirect access registers are below.

    PHY_REG_SF_IA_SW_DEV_ID = 0x0000, // Switch Device ID Register
    PHY_REG_SF_IA_SW_RESET = 0x0001, // Switch Reset Register
    PHY_REG_SF_IA_SW_IMR = 0x0004, // Switch Global Interrupt Mask Register
    PHY_REG_SF_IA_SW_IPR = 0x0005, // Switch Global Interrupt Pending Register
    PHY_REG_SF_IA_MAC_VER_ID_0 = 0x0400, //Port 0 MAC Version ID Register
    PHY_REG_SF_IA_MAC_RX_CFG_0 = 0x0401, // Port 0 MAC Receive Configuration Register
    PHY_REG_SF_IA_MAC_RX_UNDSZE_CNT_0 = 0x0410, //Port 0 MAC Receive Undersize Count Register
    PHY_REG_SF_IA_MAC_RX_64_CNT_0 = 0x0411, //Port 0 MAC REceive 64 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_65_TO_127_CNT_0 = 0x0412, //Port 0 Mac Receive 65 to 127 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_128_TO_255_CNT_0 = 0x0413, //Port 0 Mac Receive 128 to 255 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_256_TO_511_CNT_0 = 0x0414, //Port 0 Mac Receive 255 to 511 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_512_TO_1023_CNT_0 = 0x0415, //Port 0 Mac Receive 512 to 1023 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_1024_TO_MAX_CNT_0 = 0x0416, //Port 0 Mac Receive 1024 to Max Byte Count Register
    PHY_REG_SF_IA_MAC_RX_OVRSZE_CNT_0 = 0x0417, //Port 0 MAC Receive Oversize Count Register
    PHY_REG_SF_IA_MAC_RX_PKTOK_CNT_0 = 0x0418, //Port 0 MAC Receive OK Count Register
    PHY_REG_SF_IA_MAC_RX_CRCERR_CNT_0 = 0x0419, //Port 0 MAC Receive CRC Error Count Register
    PHY_REG_SF_IA_MAC_RX_MULCST_CNT_0 = 0x041A, //Port 0 MAC Receive Multicast Count Register
    PHY_REG_SF_IA_MAC_RX_BRDCST_CNT_0 = 0x041B, //Port 0 MAC Receive Broadcast Count Register
    PHY_REG_SF_IA_MAC_RX_PAUSE_CNT_0 = 0x041C, //Port 0 MAC Receive Pause Count Register
    PHY_REG_SF_IA_MAC_RX_FRAG_CNT_0 = 0x041D, //Port 0 MAC Receive Fragment Error Count Register
    PHY_REG_SF_IA_MAC_RX_JABB_CNT_0 = 0x041E, //Port 0 MAC Receive Jabber Error Count Register
    PHY_REG_SF_IA_MAC_RX_ALIGN_CNT_0 = 0x041F, //Port 0 MAC Receive Alignment Error Count Register
    PHY_REG_SF_IA_MAC_RX_PKTLEN_CNT_0 = 0x0420, //Port 0 MAC Receive Packet Length Count Register
    PHY_REG_SF_IA_MAC_RX_GOODPKTLEN_CNT_0 = 0x0421, //Port 0 MAC Receive Good Packet Length Count Register
    PHY_REG_SF_IA_MAC_RX_SYMBL_CNT_0 = 0x0422, //Port 0 MAC Receive Symbol Error Count Register
    PHY_REG_SF_IA_MAC_RX_CTLFRM_CNT_0 = 0x0423, //Port 0 MAC Receive Control Frame Count Register
    PHY_REG_SF_IA_MAC_TX_CFG_0 = 0x0440, // Port 0 MAC Transmit Configuration Register
    PHY_REG_SF_IA_MAC_TX_FC_SETTINGS_0 = 0x0441, // Port 0 MAC Transmit Flow Control Settings Register
    PHY_REG_SF_IA_MAC_TX_DEFER_CNT_0 = 0x0451, //Port 0 MAC Transmit Deferred Count Register
    PHY_REG_SF_IA_MAC_TX_PAUSE_CNT_0 = 0x0452, //Port 0 MAC Transmit Pause Count Register
    PHY_REG_SF_IA_MAC_TX_PKTOK_CNT_0 = 0x0453, //Port 0 MAC Transmit OK Count Register
    PHY_REG_SF_IA_MAC_TX_64_CNT_0 = 0x0454, //Port 0 MAC Transmit 64 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_65_TO_127_CNT_0 = 0x0455, //Port 0 Mac Transmit 65 to 127 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_128_TO_255_CNT_0 = 0x0456, //Port 0 Mac Transmit 128 to 255 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_256_TO_511_CNT_0 = 0x0457, //Port 0 Mac Transmit 255 to 511 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_512_TO_1023_CNT_0 = 0x0458, //Port 0 Mac Transmit 512 to 1023 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_1024_TO_MAX_CNT_0 = 0x0459, //Port 0 Mac Transmit 1024 to Max Byte Count Register
    PHY_REG_SF_IA_MAC_TX_UNDSIZE_CNT_0 = 0x045A, //Port 0 MAC Transmit Undersize Count Register
    PHY_REG_SF_IA_MAC_TX_PKTLEN_CNT_0 = 0x045C, //Port 0 MAC Transmit Packet Length Count Register
    PHY_REG_SF_IA_MAC_TX_BRDCST_CNT_0 = 0x045D, //Port 0 MAC Transmit Broadcast Count Register
    PHY_REG_SF_IA_MAC_TX_MULCST_CNT_0 = 0x045E, //Port 0 MAC Transmit Multicast Count Register
    PHY_REG_SF_IA_MAC_TX_LATECOL_CNT_0 = 0x045F, //Port 0 MAC Transmit Late Collision Count Register
    PHY_REG_SF_IA_MAC_TX_EXCOL_CNT_0 = 0x0460, //Port 0 MAC Transmit Excessive Collision Count Register
    PHY_REG_SF_IA_MAC_TX_SINGLECOL_CNT_0 = 0x0461, //Port 0 MAC Transmit Single Collision Count Register
    PHY_REG_SF_IA_MAC_TX_MULTICOL_CNT_0 = 0x0462, //Port 0 MAC Transmit Multiple Collision Count Register
    PHY_REG_SF_IA_MAC_TX_TOTALCOL_CNT_0 = 0x0463, //Port 0 MAC Transmit Total Collision Count Register
    PHY_REG_SF_IA_MAC_IMR_0 = 0x0480, // Port 0 MAC Interrupt Mask Register
    PHY_REG_SF_IA_MAC_IPR_0 = 0x0481, //Port 0 MAC Interrupt Pending REgister

    PHY_REG_SF_IA_MAC_VER_ID_1 = 0x0800, //Port 1 MAC Version ID Register
    PHY_REG_SF_IA_MAC_RX_CFG_1 = 0x0801, // Port 1 MAC Receive Configuration Register
    PHY_REG_SF_IA_MAC_RX_UNDSZE_CNT_1 = 0x0810, //Port 1 MAC Receive Undersize Count Register
    PHY_REG_SF_IA_MAC_RX_64_CNT_1 = 0x0811, //Port 1 MAC REceive 64 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_65_TO_127_CNT_1 = 0x0812, //Port 1 Mac Receive 65 to 127 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_128_TO_255_CNT_1 = 0x0813, //Port 1 Mac Receive 128 to 255 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_256_TO_511_CNT_1 = 0x0814, //Port 1 Mac Receive 255 to 511 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_512_TO_1023_CNT_1 = 0x0815, //Port 1 Mac Receive 512 to 1023 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_1024_TO_MAX_CNT_1 = 0x0816, //Port 1 Mac Receive 1024 to Max Byte Count Register
    PHY_REG_SF_IA_MAC_RX_OVRSZE_CNT_1 = 0x0817, //Port 1 MAC Receive Oversize Count Register
    PHY_REG_SF_IA_MAC_RX_PKTOK_CNT_1 = 0x0818, //Port 1 MAC Receive OK Count Register
    PHY_REG_SF_IA_MAC_RX_CRCERR_CNT_1 = 0x0819, //Port 1 MAC Receive CRC Error Count Register
    PHY_REG_SF_IA_MAC_RX_MULCST_CNT_1 = 0x081A, //Port 1 MAC Receive Multicast Count Register
    PHY_REG_SF_IA_MAC_RX_BRDCST_CNT_1 = 0x081B, //Port 1 MAC Receive Broadcast Count Register
    PHY_REG_SF_IA_MAC_RX_PAUSE_CNT_1 = 0x081C, //Port 1 MAC Receive Pause Count Register
    PHY_REG_SF_IA_MAC_RX_FRAG_CNT_1 = 0x081D, //Port 1 MAC Receive Fragment Error Count Register
    PHY_REG_SF_IA_MAC_RX_JABB_CNT_1 = 0x081E, //Port 1 MAC Receive Jabber Error Count Register
    PHY_REG_SF_IA_MAC_RX_ALIGN_CNT_1 = 0x081F, //Port 1 MAC Receive Alignment Error Count Register
    PHY_REG_SF_IA_MAC_RX_PKTLEN_CNT_1 = 0x0820, //Port 1 MAC Receive Packet Length Count Register
    PHY_REG_SF_IA_MAC_RX_GOODPKTLEN_CNT_1 = 0x0821, //Port 1 MAC Receive Good Packet Length Count Register
    PHY_REG_SF_IA_MAC_RX_SYMBL_CNT_1 = 0x0822, //Port 1 MAC Receive Symbol Error Count Register
    PHY_REG_SF_IA_MAC_RX_CTLFRM_CNT_1 = 0x0823, //Port 1 MAC Receive Control Frame Count Register
    PHY_REG_SF_IA_MAC_TX_CFG_1 = 0x0840, // Port 1 MAC Transmit Configuration Register
    PHY_REG_SF_IA_MAC_TX_FC_SETTINGS_1 = 0x0841, // Port 1 MAC Transmit Flow Control Settings Register
    PHY_REG_SF_IA_MAC_TX_DEFER_CNT_1 = 0x0851, //Port 1 MAC Transmit Deferred Count Register
    PHY_REG_SF_IA_MAC_TX_PAUSE_CNT_1 = 0x0852, //Port 1 MAC Transmit Pause Count Register
    PHY_REG_SF_IA_MAC_TX_PKTOK_CNT_1 = 0x0853, //Port 1 MAC Transmit OK Count Register
    PHY_REG_SF_IA_MAC_TX_64_CNT_1 = 0x0854, //Port 1 MAC Transmit 64 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_65_TO_127_CNT_1 = 0x0855, //Port 1 Mac Transmit 65 to 127 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_128_TO_255_CNT_1 = 0x0856, //Port 1 Mac Transmit 128 to 255 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_256_TO_511_CNT_1 = 0x0857, //Port 1 Mac Transmit 255 to 511 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_512_TO_1023_CNT_1 = 0x0858, //Port 1 Mac Transmit 512 to 1023 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_1024_TO_MAX_CNT_1 = 0x0859, //Port 1 Mac Transmit 1024 to Max Byte Count Register
    PHY_REG_SF_IA_MAC_TX_UNDSIZE_CNT_1 = 0x085A, //Port 1 MAC Transmit Undersize Count Register
    PHY_REG_SF_IA_MAC_TX_PKTLEN_CNT_1 = 0x085C, //Port 1 MAC Transmit Packet Length Count Register
    PHY_REG_SF_IA_MAC_TX_BRDCST_CNT_1 = 0x085D, //Port 1 MAC Transmit Broadcast Count Register
    PHY_REG_SF_IA_MAC_TX_MULCST_CNT_1 = 0x085E, //Port 1 MAC Transmit Multicast Count Register
    PHY_REG_SF_IA_MAC_TX_LATECOL_CNT_1 = 0x085F, //Port 1 MAC Transmit Late Collision Count Register
    PHY_REG_SF_IA_MAC_TX_EXCOL_CNT_1 = 0x0860, //Port 1 MAC Transmit Excessive Collision Count Register
    PHY_REG_SF_IA_MAC_TX_SINGLECOL_CNT_1 = 0x0861, //Port 1 MAC Transmit Single Collision Count Register
    PHY_REG_SF_IA_MAC_TX_MULTICOL_CNT_1 = 0x0862, //Port 1 MAC Transmit Multiple Collision Count Register
    PHY_REG_SF_IA_MAC_TX_TOTALCOL_CNT_1 = 0x0863, //Port 1 MAC Transmit Total Collision Count Register
    PHY_REG_SF_IA_MAC_IMR_1 = 0x0880, // Port 1 MAC Interrupt Mask Register
    PHY_REG_SF_IA_MAC_IPR_1 = 0x0881, //Port 1 MAC Interrupt Pending REgister

    PHY_REG_SF_IA_MAC_VER_ID_2 = 0x0C00, //Port 2 MAC Version ID Register
    PHY_REG_SF_IA_MAC_RX_CFG_2 = 0x0c01, // Port 2 MAC Receive Configuration Register
    PHY_REG_SF_IA_MAC_RX_UNDSZE_CNT_2 = 0x0C10, //Port 2 MAC Receive Undersize Count Register
    PHY_REG_SF_IA_MAC_RX_64_CNT_2 = 0x0C11, //Port 2 MAC REceive 64 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_65_TO_227_CNT_2 = 0x0C12, //Port 2 Mac Receive 65 to 127 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_228_TO_255_CNT_2 = 0x0C13, //Port 2 Mac Receive 128 to 255 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_256_TO_511_CNT_2 = 0x0C14, //Port 2 Mac Receive 255 to 511 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_512_TO_2023_CNT_2 = 0x0C15, //Port 2 Mac Receive 512 to 1023 Byte Count Register
    PHY_REG_SF_IA_MAC_RX_2024_TO_MAX_CNT_2 = 0x0C16, //Port 2 Mac Receive 1024 to Max Byte Count Register
    PHY_REG_SF_IA_MAC_RX_OVRSZE_CNT_2 = 0x0C17, //Port 2 MAC Receive Oversize Count Register
    PHY_REG_SF_IA_MAC_RX_PKTOK_CNT_2 = 0x0C18, //Port 2 MAC Receive OK Count Register
    PHY_REG_SF_IA_MAC_RX_CRCERR_CNT_2 = 0x0C19, //Port 2 MAC Receive CRC Error Count Register
    PHY_REG_SF_IA_MAC_RX_MULCST_CNT_2 = 0x0C1A, //Port 2 MAC Receive Multicast Count Register
    PHY_REG_SF_IA_MAC_RX_BRDCST_CNT_2 = 0x0C1B, //Port 2 MAC Receive Broadcast Count Register
    PHY_REG_SF_IA_MAC_RX_PAUSE_CNT_2 = 0x0C1C, //Port 2 MAC Receive Pause Count Register
    PHY_REG_SF_IA_MAC_RX_FRAG_CNT_2 = 0x0C1D, //Port 2 MAC Receive Fragment Error Count Register
    PHY_REG_SF_IA_MAC_RX_JABB_CNT_2 = 0x0C1E, //Port 2 MAC Receive Jabber Error Count Register
    PHY_REG_SF_IA_MAC_RX_ALIGN_CNT_2 = 0x0C1F, //Port 2 MAC Receive Alignment Error Count Register
    PHY_REG_SF_IA_MAC_RX_PKTLEN_CNT_2 = 0x0C20, //Port 2 MAC Receive Packet Length Count Register
    PHY_REG_SF_IA_MAC_RX_GOODPKTLEN_CNT_2 = 0x0C21, //Port 2 MAC Receive Good Packet Length Count Register
    PHY_REG_SF_IA_MAC_RX_SYMBL_CNT_2 = 0x0C22, //Port 2 MAC Receive Symbol Error Count Register
    PHY_REG_SF_IA_MAC_RX_CTLFRM_CNT_2 = 0x0C23, //Port 2 MAC Receive Control Frame Count Register
    PHY_REG_SF_IA_MAC_TX_CFG_2 = 0x0C40, // Port 2 MAC Transmit Configuration Register
    PHY_REG_SF_IA_MAC_TX_FC_SETTINGS_2 = 0x0C41, // Port 2 MAC Transmit Flow Control Settings Register
    PHY_REG_SF_IA_MAC_TX_DEFER_CNT_2 = 0x0C51, //Port 2 MAC Transmit Deferred Count Register
    PHY_REG_SF_IA_MAC_TX_PAUSE_CNT_2 = 0x0C52, //Port 2 MAC Transmit Pause Count Register
    PHY_REG_SF_IA_MAC_TX_PKTOK_CNT_2 = 0x0C53, //Port 2 MAC Transmit OK Count Register
    PHY_REG_SF_IA_MAC_TX_64_CNT_2 = 0x0C54, //Port 2 MAC Transmit 64 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_65_TO_227_CNT_2 = 0x0C55, //Port 2 Mac Transmit 65 to 127 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_228_TO_255_CNT_2 = 0x0C56, //Port 2 Mac Transmit 128 to 255 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_256_TO_511_CNT_2 = 0x0C57, //Port 2 Mac Transmit 255 to 511 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_512_TO_2023_CNT_2 = 0x0C58, //Port 2 Mac Transmit 512 to 1023 Byte Count Register
    PHY_REG_SF_IA_MAC_TX_2024_TO_MAX_CNT_2 = 0x0C59, //Port 2 Mac Transmit 1024 to Max Byte Count Register
    PHY_REG_SF_IA_MAC_TX_UNDSIZE_CNT_2 = 0x0C5A, //Port 2 MAC Transmit Undersize Count Register
    PHY_REG_SF_IA_MAC_TX_PKTLEN_CNT_2 = 0x0C5C, //Port 2 MAC Transmit Packet Length Count Register
    PHY_REG_SF_IA_MAC_TX_BRDCST_CNT_2 = 0x0C5D, //Port 2 MAC Transmit Broadcast Count Register
    PHY_REG_SF_IA_MAC_TX_MULCST_CNT_2 = 0x0C5E, //Port 2 MAC Transmit Multicast Count Register
    PHY_REG_SF_IA_MAC_TX_LATECOL_CNT_2 = 0x0C5F, //Port 2 MAC Transmit Late Collision Count Register
    PHY_REG_SF_IA_MAC_TX_EXCOL_CNT_2 = 0x0C60, //Port 2 MAC Transmit Excessive Collision Count Register
    PHY_REG_SF_IA_MAC_TX_SINGLECOL_CNT_2 = 0x0C61, //Port 2 MAC Transmit Single Collision Count Register
    PHY_REG_SF_IA_MAC_TX_MULTICOL_CNT_2 = 0x0C62, //Port 2 MAC Transmit Multiple Collision Count Register
    PHY_REG_SF_IA_MAC_TX_TOTALCOL_CNT_2 = 0x0C63, //Port 2 MAC Transmit Total Collision Count Register
    PHY_REG_SF_IA_MAC_IMR_2 = 0x0C80, // Port 2 MAC Interrupt Mask Register
    PHY_REG_SF_IA_MAC_IPR_2 = 0x0C81, //Port 2 MAC Interrupt Pending Register

    PHY_REG_SF_IA_SWE_ALR_CMD = 0x1800, // Switch Engine ALR Command REgister
    PHY_REG_SF_IA_SWE_ALR_WR_DAT_0 = 0x1801, // Switch Engine ALR Write Data 0 Register
    PHY_REG_SF_IASWE_ALR_WR_DAT_1 = 0x1802, // Switch Engine ALR Write Data 1 Register
    PHY_REG_SF_IA_SWE_ALR_RD_DAT_0 = 0x1805, //Switch Engine ALR Read Data 0 Register
    PHY_REG_SF_IA_SWE_ALR_RD_DAT_1 = 0x1806, //Switch Engine ALR Read Data 1 Register
    PHY_REG_SF_IA_SWE_ALR_CMD_STS = 0x1808, //Switch Engine ALR Command Status Register
    PHY_REG_SF_IA_SWE_ALR_CFG = 0x1809, // Switch Engine ALR Configuration Register
    PHY_REG_SF_IA_SWE_VLAN_CMD = 0x180B, // Switch Engine VLAN Command Register
    PHY_REG_SF_IA_SWE_VLAN_WR_DATA = 0x180C, // Switch Engine VLAN Write Register
    PHY_REG_SF_IA_SWE_VLAN_RO_DATA = 0x180E, //Switch Engine VLAN Read Data Register
    PHY_REG_SF_IA_SWE_VLAN_CMD_STS = 0x1810, //Switch Engine VLAN Command Status Register
    PHY_REG_SF_IA_SWE_DIFFSERV_TBL_CMD = 0x1811, // Switch Engine DIFSERV Table Command Register
    PHY_REG_SF_IA_SWE_DIFFSERV_TBL_WR_DATA = 0x1812, // Switch Engine DIFSERV Table Write Data
    PHY_REG_SF_IA_SWE_DIFFSERV_TBL_RO_DATA = 0x1813, //Switch Engine DIFFSERV Table Read Data Register
    PHY_REG_SF_IA_SWE_DIFFSERV_TBL_CMD_STS = 0x1814, //Switch Engine DIFFSERV Table Command Status Register
    PHY_REG_SF_IA_SWE_GLB_INGRESS_CFG = 0x1840, // Switch Engine Global Ingress Configuration Register
    PHY_REG_SF_IA_SWE_PORT_INGRESS_CFG = 0x1841, // Switch Engine Port Ingress Configuration Register
    PHY_REG_SF_IA_SWE_ADMT_ONLY_VLAN = 0x1842, // Switch Engine Admit Only VLAN Register
    PHY_REG_SF_IA_SWE_PORT_STATE = 0x1843, // Switch Engine Port State Register
    PHY_REG_SF_IA_SWE_PRI_TO_QUE = 0x1845, // Switch Engine PRiority to Queue Register
    PHY_REG_SF_IA_SWE_PORT_MIRROR = 0x1846, // Switch Engine Port Mirroring Register
    PHY_REG_SF_IA_SWE_INGRESS_PORT_TYPE = 0x1847, // Switch Engine Ingress Port Type Register
    PHY_REG_SF_IA_SWE_BCST_THROT = 0x1848, // Switch Engine Broadcast Throttling Register
    PHY_REG_SF_IA_SWE_ADMT_N_MEMBER = 0x1849, // Swtich Engine Admit Non Member Register
    PHY_REG_SF_IA_SWE_INGRESS_RATE_CFG = 0x184A, // Switch Engine Ingress Rate Configuration Register
    PHY_REG_SF_IA_SWE_INGRESS_RATE_CMD = 0x184B, // Switch Engine Ingress Rate Command Register
    PHY_REG_SF_IA_SWE_INGRESS_RATE_CMD_STS = 0x184C, //Switch Engine Ingress Rate Command Status Register
    PHY_REG_SF_IA_SWE_INGRESS_RATE_WR_DATA = 0x184D, // Switch Engine Ingress Rate Write Data Register
    PHY_REG_SF_IA_SWE_INGRESS_RATE_RD_DATA = 0x184E, //Switch Engine Ingress Rate Read Data Register
    PHY_REG_SF_IA_SWE_FILTERED_CNT_0 = 0x1850, //Switch Engine Port 0 Ingress Filtered Count Register
    PHY_REG_SF_IA_SWE_FILTERED_CNT_1 = 0x1851, //Switch Engine Port 1 Ingress Filtered Count Register
    PHY_REG_SF_IA_SWE_FILTERED_CNT_2 = 0x1852, //Switch Engine Port 2 Ingress Filtered Count Register
    PHY_REG_SF_IA_SWE_INGRESS_REGEN_TBL_0 = 0x1855, // Switch Engine Port 0 Learn Discard Count Register
    PHY_REG_SF_IA_SWE_INGRESS_REGEN_TBL_1 = 0x1856, // Switch Engine Port 1 Learn Discard Count Register
    PHY_REG_SF_IA_SWE_INGRESS_REGEN_TBL_2 = 0x1857, // Switch Engine Port 2 Learn Discard Count Register
    PHY_REG_SF_IA_SWE_LRN_DISCRD_CNT_0 = 0x1858, //Switch Engine Port 0 Learn Discard Count Register
    PHY_REG_SF_IA_SWE_LRN_DISCRD_CNT_1 = 0x1859, //Switch Engine Port 1 Learn Discard Count Register
    PHY_REG_SF_IA_SWE_LRN_DISCRD_CNT_2 = 0x185A, //Switch Engine Port 2 Learn Discard Count Register
    PHY_REG_SF_IA_SWE_IMR = 0x1880, // Switch Engine Interrupt Mask Register
    PHY_REG_SF_IA_SWE_IPR = 0x1881, //Switch Engine Interrupt PEnding Register

    PHY_REG_SF_IA_BM_CFG = 0x1C00, // Buffer Manager Configuration Register
    PHY_REG_SF_IA_BM_DROP_LVL = 0x1C01, // Buffer Manager Drop Level Register
    PHY_REG_SF_IA_BM_FC_PAUSE_LVL = 0x1C02, // Buffer Manager Flow Control Pause Level Register
    PHY_REG_SF_IA_BM_FC_RESUME_LVL = 0x1C03, // Buffer Manager Flow Control Resume Level Register
    PHY_REG_SF_IA_BM_BCST_LVL = 0x1C04, // Buffer Manager Broadcast Buffer Level Register
    PHY_REG_SF_IA_BM_DRP_CNT_SRC_0 = 0x1C05, //Buffer ManagerPort 0 Drop Count Register
    PHY_REG_SF_IA_BM_DRP_CNT_SRC_1 = 0x1C06, //Buffer ManagerPort 1 Drop Count Register
    PHY_REG_SF_IA_BM_DRP_CNT_SRC_2 = 0x1C07, //Buffer ManagerPort 2 Drop Count Register
    PHY_REG_SF_IA_BM_RST_STS = 0x1C08, //Buffer Manager Reset Status Register
    PHY_REG_SF_IA_BM_RNDM_DSCRD_TBL_CMD = 0x1C09, // Buffer Manager Random Discard Table Command Register
    PHY_REG_SF_IA_BM_RNDM_DSCRD_TBL_WDATA = 0x1C0A, // Buffer Manager Random Discard Table Write Data Register
    PHY_REG_SF_IA_BM_RNDM_DSCRD_TABLE_RDATA = 0x1C0B, //Buffer Manager Random Discard Table Read Data Register
    PHY_REG_SF_IA_BM_EGRSS_PORT_TYPE = 0x1C0C, // Buffer Manager Egress Port Type Register
    PHY_REG_SF_IA_BM_EGRSS_RATE_00_01 = 0x1C0D, // Buffer Manager Port 0 Egress Rate PRiotity Queue 0/1 Register
    PHY_REG_SF_IA_BM_EGRSS_RATE_02_03 = 0x1C0E, // Buffer Manager Port 0 Egress Rate PRiotity Queue 2/3 Register
    PHY_REG_SF_IA_BM_EGRSS_RATE_10_11 = 0x1C0F, // Buffer Manager Port 1 Egress Rate PRiotity Queue 0/1 Register
    PHY_REG_SF_IA_BM_EGRSS_RATE_12_13 = 0x1C10, // Buffer Manager Port 1 Egress Rate PRiotity Queue 2/3 Register
    PHY_REG_SF_IA_BM_EGRSS_RATE_20_21 = 0x1C11, // Buffer Manager Port 2 Egress Rate PRiotity Queue 0/1 Register
    PHY_REG_SF_IA_BM_EGRSS_RATE_22_23 = 0x1C12, // Buffer Manager Port 2 Egress Rate PRiotity Queue 2/3 Register
    PHY_REG_SF_IA_BM_VLAN_0 = 0x1C13, // Buffer Manager Port 0 Default VLAN ID and PRiority Register
    PHY_REG_SF_IA_BM_VLAN_1 = 0x1C14, // Buffer Manager Port 1 Default VLAN ID and PRiority Register
    PHY_REG_SF_IA_BM_VLAN_2 = 0x1C15, // Buffer Manager Port 2 Default VLAN ID and PRiority Register
    PHY_REG_SF_IA_BM_RATE_DRP_CNT_SRC_0 = 0x1C16, //Buffer Manager Port 0 Ingress Rate Drop Count Register
    PHY_REG_SF_IA_BM_RATE_DRP_CNT_SRC_1 = 0x1C17, //Buffer Manager Port 0 Ingress Rate Drop Count Register
    PHY_REG_SF_IA_BM_RATE_DRP_CNT_SRC_2 = 0x1C18, //Buffer Manager Port 0 Ingress Rate Drop Count Register
    PHY_REG_SF_IA_BM_IMR = 0x1C20, // Buffer Manager Interrupt Mask Register
    PHY_REG_SF_IA_BM_IPR = 0x1C21, // Buffer Manger Interrupt Pending Register
} ePHY_VENDOR_REG;
// updated version of ePHY_REG




// vendor registers
//

// Interrupt Configuration Register (IRQ_CFG) 13.2.1.1

typedef union {

    struct {
        unsigned IRQ_TYPE : 1; // IRQ Buffer Type
        unsigned : 3;
        unsigned IRQ_POL : 1; // IRQ Polarity
        unsigned : 3;
        unsigned IRQ_EN : 1; // IRQ Enable
        unsigned : 3;
        unsigned IRQ_INT : 1; //Master Interrupt
        unsigned INT_DEAS_STS : 1; // Interrupt De-assertion Status
        unsigned INT_DEAS_CLR : 1; // Interrupt De-assertion Interval Clear
        unsigned : 9;
        unsigned INT_DEAS : 8; // Interrupt De-assertion Interval
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_IRQ_CRG_t;

// Interrupt Status Register (INT_STS) 13.2.1.2

typedef union {

    struct {
        unsigned : 12;
        unsigned GPIO : 1; // GPIO Interrupt Event
        unsigned : 6;
        unsigned GPT_INT : 1; //GP Timer
        unsigned : 6;
        unsigned PHY_INT1 : 1; //Port 1 PHY Interrupt Event
        unsigned PHY_INT2 : 1; //Port 2 PHY Interrupt Event
        unsigned SWITCH_INT : 1; // Switch Fabric Interrupt Event
        unsigned : 1;
        unsigned READY : 1; // Device Ready
        unsigned SW_INT : 1; // Software Interrupt
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_IRQ_STS_t;

// Interrupt Enable Register (INT_EN) 13.2.1.3

typedef union {

    struct {
        unsigned : 12;
        unsigned GPIO_EN : 1; //GPIO Interrupt Event Enable
        unsigned : 6;
        unsigned GPT_INT_EN : 1; // GP Timer Interrupt Enable
        unsigned : 6;
        unsigned PHY_INT1_EN : 1; //Port 1 PHY Interrupt Event Enable
        unsigned PHY_INT2_EN : 2; //Port 2 PHY Interrupt Event Enable
        unsigned SWITCH_INT_EN : 1; //Switch Fabric Interrupt Event Enable
        unsigned : 1;
        unsigned READY_EN : 1; //Device Ready Enable
        unsigned SW_INT_EN : 1; //Software Interrupt Enable
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_IRQ_EN_t;


//General Purpose I/O Configuration Register 13.2.2.1

typedef union {

    struct {
        unsigned GPIOBUF0 : 1; //GPIO Buffer Type 0
        unsigned GPIOBUF1 : 1; //GPIO Buffer Type 1
        unsigned GPIOBUF2 : 1; //GPIO Buffer Type 2
        unsigned GPIOBUF3 : 1; //GPIO Buffer Type 3
        unsigned GPIOBUF4 : 1; //GPIO Buffer Type 4
        unsigned GPIOBUF5 : 1; //GPIO Buffer Type 5
        unsigned : 10;
        unsigned GPIO_INT_POL0 : 1; //GPIO Interrupt Polarity 0
        unsigned GPIO_INT_POL1 : 1; //GPIO Interrupt Polarity 1
        unsigned GPIO_INT_POL2 : 1; //GPIO Interrupt Polarity 2
        unsigned GPIO_INT_POL3 : 1; //GPIO Interrupt Polarity 3
        unsigned GPIO_INT_POL4 : 1; //GPIO Interrupt Polarity 4
        unsigned GPIO_INT_POL5 : 1; //GPIO Interrupt Polarity 5
        unsigned : 10;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_GPIO_CFG_t;

//General Purpose I/O DATA & Direction Register 13.2.2.2

typedef union {

    struct {
        unsigned GPIOD0 : 1; //GPIO Data 0
        unsigned GPIOD1 : 1; //GPIO Data 1
        unsigned GPIOD2 : 1; //GPIO Data 2
        unsigned GPIOD3 : 1; //GPIO Data 3
        unsigned GPIOD4 : 1; //GPIO Data 4
        unsigned GPIOD5 : 1; //GPIO Data 5
        unsigned : 10;
        unsigned GPDIR0 : 1; //GPIO Direction 0
        unsigned GPDIR1 : 1; //GPIO Direction 1
        unsigned GPDIR2 : 1; //GPIO Direction 2
        unsigned GPDIR3 : 1; //GPIO Direction 3
        unsigned GPDIR4 : 1; //GPIO Direction 4
        unsigned GPDIR5 : 1; //GPIO Direction 5
        unsigned : 10;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_GPIO_DATA_DIR_t; // 

//General Purpose I/O Interrupt Status and Enable Register 13.2.2.3

typedef union {

    struct {
        unsigned GPIO0_INT : 1; //GPIO Interrupt 0
        unsigned GPIO1_INT : 1; //GPIO Interrupt 1
        unsigned GPIO2_INT : 1; //GPIO Interrupt 2
        unsigned GPIO3_INT : 1; //GPIO Interrupt 3
        unsigned GPIO4_INT : 1; //GPIO Interrupt 4
        unsigned GPIO5_INT : 1; //GPIO Interrupt 5
        unsigned : 10;
        unsigned GPIO0_INT_EN : 1; //GPIO Interrupt Enable 0
        unsigned GPIO1_INT_EN : 1; //GPIO Interrupt Enable 1
        unsigned GPIO2_INT_EN : 1; //GPIO Interrupt Enable 2
        unsigned GPIO3_INT_EN : 1; //GPIO Interrupt Enable 3
        unsigned GPIO4_INT_EN : 1; //GPIO Interrupt Enable 4
        unsigned GPIO5_INT_EN : 1; //GPIO Interrupt Enable 5
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_GPIO_INT_STS_EN_t; //


//LED Configuration Register 13.2.2.4

typedef union {

    struct {
        unsigned LED_EN0 : 1; //LED Enable 0
        unsigned LED_EN1 : 1; //LED Enable 1
        unsigned LED_EN2 : 1; //LED Enable 2
        unsigned LED_EN3 : 1; //LED Enable 3
        unsigned LED_EN4 : 1; //LED Enable 4
        unsigned LED_EN5 : 1; //LED Enable 5
        unsigned : 2;
        unsigned LED_FUN : 2; //LED Function
        unsigned : 22;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_LED_CFG_t;

// EEPROM Command Register 13.2.3.1

typedef union {

    struct {
        unsigned EPC_ADDRESS : 16; // EEPROM Controller Address
        unsigned CFG_LOADED : 1; //Configuration Loaded
        unsigned EPC_TIMEOUT : 1; //EEPROM Controller Timeout
        unsigned LOADER_OVERFLOW : 1; //EEPROM Loader Address Overflow
        unsigned : 9;
        unsigned EPC_COMMAND : 3; // EEPROM Controller Command
        unsigned EPC_BUSY : 1; //EEPROM Controller Busy
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_E2P_CMD_t;

//EEPROM Data Register 13.2.3.2

typedef union {

    struct {
        unsigned EEPROM_DATA : 8; //EEPROM Data
        unsigned : 24;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_E2P_DATA_t;

//Port 1 Manual Flow Control Register 13.2.4.1

typedef union {

    struct {
        unsigned MANUAL_FC_1 : 1; // Port 1 Full-Duplex Manual Flow Control Select
        unsigned TX_FC_1 : 1; //Port 1 Full-Duplex Transmit Flow Control Enable
        unsigned RX_FC_1 : 1; //Port 1 Full-Duplex Receive Flow Control Enable
        unsigned CUR_TX_FC_1 : 1; //Port 1 Current Transmit Flow Control Enable
        unsigned CUR_RX_FC_1 : 1; //Port 1 Current Receive Flow Control Enable
        unsigned CUR_DUP_1 : 1; //Port 1 Current Duplex
        unsigned BP_EN_1 : 1; //Port 1 Backpressure Enable
        unsigned : 25;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_MANUAL_FC_1_t;

//Port 2 Manual Flow Control Register 13.2.4.2

typedef union {

    struct {
        unsigned MANUAL_FC_2 : 1; // Port 2 Full-Duplex Manual Flow Control Select
        unsigned TX_FC_2 : 1; //Port 2 Full-Duplex Transmit Flow Control Enable
        unsigned RX_FC_2 : 1; //Port 2 Full-Duplex Receive Flow Control Enable
        unsigned CUR_TX_FC_2 : 1; //Port 2 Current Transmit Flow Control Enable
        unsigned CUR_RX_FC_2 : 1; //Port 2 Current Receive Flow Control Enable
        unsigned CUR_DUP_2 : 1; //Port 2 Current Duplex
        unsigned BP_EN_2 : 1; //Port 2 Backpressure Enable
        unsigned : 25;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_MANUAL_FC_2_t;

//Port 0 Manual Flow Control Register 13.2.4.3

typedef union {

    struct {
        unsigned MANUAL_FC_0 : 1; // Port 0 Full-Duplex Manual Flow Control Select
        unsigned TX_FC_0 : 1; //Port 0 Full-Duplex Transmit Flow Control Enable
        unsigned RX_FC_0 : 1; //Port 0 Full-Duplex Receive Flow Control Enable
        unsigned CUR_TX_FC_0 : 1; //Port 0 Current Transmit Flow Control Enable
        unsigned CUR_RX_FC_0 : 1; //Port 0 Current Receive Flow Control Enable
        unsigned CUR_DUP_0 : 1; //Port 0 Current Duplex
        unsigned BP_EN_0 : 1; //Port 0 Backpressure Enable
        unsigned : 25;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_MANUAL_FC_0_t;

//Switch Fabric CSR Interface Data Register 13.2.4.4

typedef union {

    struct {
        unsigned CSR_DATA : 32; //Switch CSR Data
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWITCH_CSR_DATA_t;

//Switch Fabric CSR Interface Command Register 13.2.4.5

typedef union {

    struct {
        unsigned CSR_ADDR : 16; // CSR Address
        unsigned CSR_BE : 4; // CSR Byte Enabled
        unsigned : 8;
        unsigned AUTO_DEC : 1; // Auto Decrement
        unsigned AUTO_INC : 1; // Auto Increment
        unsigned R_nW : 1; // Read/Write
        unsigned CSR_BUSY : 1; // CSR Busy
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWITCH_CSR_CMD_t;

//Switch Fabric MAC Address High Register  13.2.4.6

typedef union {

    struct {
        unsigned PHY_ADDR : 16; // Physical Address [47:32]
        unsigned PORT0_PHY_ADDR : 2; //Port 0 Physical Address
        unsigned PORT1_PHY_ADDR : 2; //Port 1 Physical Address
        unsigned PORT2_PHY_ADDR : 2; //Port 2 Physical Address
        unsigned DiffPauseAddr : 1; //DiffPauseAddr
        unsigned : 9;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWITCH_MAC_ADDRH_t;

//Switch Fabric MAC Address Low Regiser 13.2.4.7

typedef union {

    struct {
        unsigned PHY_ADDR : 32; //Phyusical Address
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWITCH_MAC_ADDRL_t;

//Switch Fabric CSR Interface Direct Data Registers 13.2.4.7

typedef union {

    struct {
        unsigned CSR_DATA : 32; //Phyusical Address
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWITCH_CSR_DIRECT_DATA_t;

//PHY Management Interface 13.2.5.1

typedef union {

    struct {
        unsigned MIIData : 16; //MII Data
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_PMI_t;

//PHY Management Interface Access Register 13.2.5.2

typedef union {

    struct {
        unsigned MIIBZY : 1; //MII Busy
        unsigned MIIWnR : 1; //MII Write
        unsigned : 4;
        unsigned MIIRINDA : 5; //MII Register Index
        unsigned PHY_ADDR : 5; //PHY Addr
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_PMI_ACCESS_t;

//Virtual PHY Basic Control Register 13.2.6.1

typedef union {

    struct {
        unsigned : 6;
        unsigned VPHY_SPEED_SEL_MSB : 1; // Speed Select MSB
        unsigned VPHY_COL_TEST : 1; //Collision Test
        unsigned VPHY_DUPLEX : 1; //Duplex Mode
        unsigned VPHY_RST_AN : 1; //Restart Auto-Negotiation
        unsigned VPHY_ISO : 1; //Isolate
        unsigned VPHY_PWR_DWN : 1; //Power Down
        unsigned VPHY_AN : 1; //Auto-Negotiation
        unsigned VPHY_SPEED_SEL_LSB : 1; //Speed Select LSB
        unsigned VPHY_LOOPBACK : 1; //Loopback
        unsigned VPHY_RST : 1; //Reset
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_VPHY_BASIC_CTRL_t;

//virtual PHY Basic Status Register 13.2.6.2

typedef union {

    struct {
        unsigned EXT_CAP : 1; //Extended Capability
        unsigned JAB_DECT : 1; //Jabber Detect
        unsigned LINK_STATUS : 1; //Link STatus
        unsigned AN_ABILITY : 1; //Auto-Negotiation Ability
        unsigned RMT_FAULT : 1; //Remote Fault
        unsigned AN_CMPT : 1; //Auto-Negotation Complete
        unsigned MF_PRE_SUP : 1; //MF Preable Suppression
        unsigned : 1;
        unsigned EXT_STATUS : 1; //Extended Status
        unsigned SPD_100_T2_HALF : 1; //100 Base-T2 Half Duplex
        unsigned SPD_100_T2_FULL : 1; //100 Base-T2 Full Duplex
        unsigned SPD_10_T2_HALF : 1; //10 Base-T Half Duplex
        unsigned SPD_10_T2_FULL : 1; //10 Base-T Full Duplex
        unsigned SPD_100_X_HALF : 1; //100 Base-X Half Duplex
        unsigned SPD_100_X_FULL : 1; //100 Base-X Full Duplex
        unsigned SPD_100_T4_FULL : 1; //100 Base-T4
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };

} __SMSC9303_VPHY_BASIC_STATUS_t;


//virtual PHY Identification MSB Register 13.2.6.3

typedef union {

    struct {
        unsigned PHY_ID_MSB : 16; //PHY_ID
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_VPHY_ID_MSB_t;

//virtual PHY Identification LSB Register 13.2.6.4

typedef union {

    struct {
        unsigned REV_NUMBER : 4; //Revision Number
        unsigned MOD_NUMBER : 6; //Model Number
        unsigned PHY_ID_LSB : 6; //PHY_ID
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_VPHY_ID_LSB_t;

//Virtual PHY Auto-Negotiation Advertisement Register 13.2.8.5

typedef union {

    struct {
        unsigned SELECT_FIELD : 5; //Selector Field
        unsigned SPD_10_T2_HALF : 1; //10 Base-T Half Duplex
        unsigned SPD_10_T2_FULL : 1; //10 Base-T Full Duplex
        unsigned SPD_100_X_HALF : 1; //100 Base-X Half Duplex
        unsigned SPD_100_X_FULL : 1; //100 Base-X Full Duplex
        unsigned SPD_100_T4_FULL : 1; //100 Base-T4
        unsigned SYM_PAUSE : 1; //Symmetric PAuse
        unsigned ASYM_PAUSE : 1; //Asymmetric Pause
        unsigned : 1;
        unsigned REMOTE_FAULT : 1; //Remote Fault
        unsigned : 1;
        unsigned NEXT_PAGE : 1; //Next Page
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_VPHY_AN_ADV_t;

//Virtual PHY Auto-Negotiation Link Partner Base Page Ability Register 13.2.6.6

typedef union {

    struct {
        unsigned SELECT_FIELD : 5; //Selector Field
        unsigned SPD_10_T2_HALF : 1; //10 Base-T Half Duplex
        unsigned SPD_10_T2_FULL : 1; //10 Base-T Full Duplex
        unsigned SPD_100_X_HALF : 1; //100 Base-X Half Duplex
        unsigned SPD_100_X_FULL : 1; //100 Base-X Full Duplex
        unsigned SPD_100_T4_FULL : 1; //100 Base-T4
        unsigned SYM_PAUSE : 1; //Symmetric PAuse
        unsigned ASYM_PAUSE : 1; //Asymmetric Pause
        unsigned : 1;
        unsigned REMOTE_FAULT : 1; //Remote Fault
        unsigned ACK : 1; //Acknowledge
        unsigned NEXT_PAGE : 1; //Next Page
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_VPHY_AN_LP_BASE_ABILITY_t;


//Virtual PHY Auto-Negotation Expansion Register 13.2.6.7

typedef union {

    struct {
        unsigned LINK_PTRN_AN_ABLE : 1; //Link PArtner Auto-Negotation Able
        unsigned PAGE_RXED : 1; //Page Received
        unsigned LOC_DEV_NEXT_PAGE_ABLE : 1; //Local Device Next Page Able
        unsigned LINK_PTRN_NEXT_PAGE_ABLE : 1; //Link PArtner Next Page Able
        unsigned PARALLEL_DET_FAULT : 1; //Parallel Detection Fault
        unsigned : 27;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_VPHY_AN_EXP_t;

//Virtual PHY Special Control/Status REgister 13.2.6.8

typedef union {

    struct {
        unsigned SQEOFF : 1; //SQEOFF
        unsigned : 1;
        unsigned CUR_SPD_DUP_IND : 3; // Current Speed / Duplex Indication
        unsigned RMII_TURBO_MII_CLK_STR : 1; //RMII/Turbo MII Clock Strength
        unsigned RMII_CLK_DIR : 1; //RMII Clock Direction
        unsigned SWITCH_COLL_TEST_PORT_0 : 1; //Switch Collision Test Port 0
        unsigned MODE : 2;
        unsigned TURBO_MII_EN : 1; //Turbo MII Enable
        unsigned : 3;
        unsigned SWITCH_LOOPBACK_PORT_0 : 1; //Switch Loopback Port 0
        unsigned : 17;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_VPHY_SPECIAL_CONTROL_STATUS_t;

// Chip ID and Revision 13.2.7.1

typedef union {

    struct {
        unsigned CHIP_REV : 16; //Chip REvision
        unsigned CHIP_ID : 16; //Chip ID
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_ID_REV_t;

//Byte Order Test Register 13.2.7.2

typedef union {

    struct {
        unsigned BYTE_TEST : 32;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_BYTE_TEST_t;

//Hardware Configuration Register 13.2.7.3

typedef union {

    struct {
        unsigned : 25;
        unsigned AMDIX_EN1 : 1; //AMDIX_EN Strap State Port 1
        unsigned AMDIX_EN2 : 1; //AMDIX_EN Strap State Port 2
        unsigned READY : 1; //Device Ready
        unsigned : 4;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_HW_CFG_t;

//General Purpose Time Configuration Register 13.2.7.4

typedef union {

    struct {
        unsigned GPT_LOAD : 16; // General Pupose Timer Pre-Load
        unsigned : 13;
        unsigned TIMER_EN : 1; // General Purpose Timer Enable
        unsigned : 2;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_GPT_CFG_t;

//General Purpose Timer Count Register 13.2.7.5

typedef union {

    struct {
        unsigned GPT_CNT : 16; //General Purpose Timer Current Count
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_GPT_CNT_t;

//Free Running 25MHz Counter Register 13.2.7.6

typedef union {

    struct {
        unsigned FR_CNT : 32; //Free Running Count
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_FREE_RUN_t;


//Reset Control Register 13.2.7.7

typedef union {

    struct {
        unsigned DIGITAL_RST : 1; // Digital Reset
        unsigned PHY1_RST : 1; //Port 1 PHY Reset
        unsigned PHY2_RST : 1; //Port 2 PHY Reset
        unsigned VPHY_RST : 1; //Virtual PHY Reset
        unsigned : 28;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_RESET_CTL_t;

//Switch Device ID Register 13.4.1.1

typedef union {

    struct {
        unsigned : 8;
        unsigned CHIP_VERSION : 8; // Chip Version Code
        unsigned DEVICE_TYPE : 8; //Device Type Code
        unsigned : 8;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SW_DEV_ID_t;

//Switch Reset Register 13.4.1.2

typedef union {

    struct {
        unsigned SW_RESET : 1;
        unsigned : 31;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SW_RESET_t;


//Switch Global Interrupt Mask Register  13.4.1.3

typedef union {

    struct {
        unsigned MAC_0 : 1; // Port 0 MAC Interrupt Mask
        unsigned MAC_1 : 1; // Port 1 MAC Interrupt Mask
        unsigned MAC_2 : 1; // Port 2 MAC Interrupt Mask
        unsigned RESERVED_3_4 : 2; // These bits must be writted as 11b
        unsigned SWE : 1; //Switch Engine Interrupt
        unsigned BM : 1; //Buffer Manager Interrupt
        unsigned RESERVED_7_8 : 2; // These bits must be written as 11b
        unsigned : 23;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SW_IMR_t;

// Switch Global Interrup Pending Register 13.4.1.4

typedef union {

    struct {
        unsigned MAC_0 : 1; // Port 0 MAC Interrupt
        unsigned MAC_1 : 1; // Port 1 MAC Interrupt
        unsigned MAC_2 : 1; // Port 2 MAC Interrupt
        unsigned : 2;
        unsigned SWE : 1; // Switch Engine Interrupt
        unsigned BM : 1; //Buffer Manager Interrupt
        unsigned : 25;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SW_IPR_t;

//Port x MAC Version ID register 13.4.2.1

typedef union {

    struct {
        unsigned REVISION : 4; //Revision Code
        unsigned CHIP_VERSION : 4; //Chip Version Code
        unsigned DEVICE_TYPE : 4; //Device Type
        unsigned : 20;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_MAC_VERSION_ID_x_t;


//Poirt x MAC REceive Configuration REgister

typedef union {

    struct {
        unsigned RX_ENABLE : 1; // RX Enable
        unsigned REJECT_MAC_TYPES : 1; // Reject MAck Types
        unsigned : 1;
        unsigned JUMBO_2K : 1; //Jumbo 2K
        unsigned ENABLE_RX_OWN_TX : 1; // Enable Receive Own Transmit
        unsigned : 1;
        unsigned RESERVED_7 : 1; // Must always be written as 0
        unsigned : 24;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_MAC_RX_CFG_x_t;


// Port x MAC Receive Undersized Count Register 13.4.2.3
// Port x MAC Receive 64 Byte Count Register 13.4.2.4
// Port x MAC Receive 65 to 127 Byte Count Register 13.4.2.5
// Port x MAC Receive 128 to 255 Byte Count Register 13.4.2.6
// Port x MAC Receive 256 to 511 Byte Count Register 13.4.2.7
// Port x MAC Receive 512 to 1023 Byte Count Register 13.4.2.8
// Port x MAC Receive 1024 to MAX Byte Count Register 13.4.2.9
// Port x MAC Receive Oversize Count Register 13.4.2.10
// Port x MAC Receive OK Count Register 13.4.2.11
// Port x MAC Receive CRC Error Count Register 13.4.2.12
// Port x MAC Receive Multicast Count Register 13.4.2.13
// Port x MAC Receive Broadcast Count Register 13.4.2.14
// Port x MAC Receive Pause Frame Count Register 13.4.2.15
// Port x MAC Receive Fragment Error Count Register 13.4.2.16
// Port x MAC Receive Jabber Error Count Register 13.4.2.17
// Port x MAC Receive Alignment Error Count Register 13.4.2.18
// Port x MAC Receive Packet Length Count Register 13.4.2.19
// Port X MAC Receive Good Packet LEngth Count Register 13.4.2.20
// Port X MAC Receive Symbol Error Count Register 13.4.2.21
// Port X MAC Receive Control Fram Count Register 13.4.2.22
// Port X MAC Transmit Deferred Count Register 13.4.2.25
// Port X MAC Transmit Pause Count Register 13.4.2.26
// Port X MAC Transmit OK Count Register 13.4.2.27
// Port x MAC Transmit 64 Byte Count Register 13.4.2.28
// Port x MAC Transmit 65 to 127 Byte Count Register 13.4.2.29
// Port x MAC Transmit 128 to 255 Byte Count Register 13.4.2.30
// Port x MAC Transmit 256 to 511 Byte Count Register 13.4.2.31
// Port x MAC Transmit 512 to 1023 Byte Count Register 13.4.2.32
// Port x MAC Transmit 1024 to MAX Byte Count Register 13.4.2.33
// Port x MAC Transmit Undersized Count Register 13.4.2.34
// Port x MAC Transmit Packet Length Count Register 13.4.2.35
// Port x MAC Transmit Broadcast Count Register 13.4.2.36
// Port x MAC Transmit Multicast Count Register 13.4.2.37
// Port x MAC Transmit Late Collision Count Register 13.4.2.38
// Port x MAC Transmit Excessive Collision Count Register 13.4.2.39
// Port x MAC Transmit Single Collision Count Register 13.4.2.40
// Port x MAC Transmit Multiple Collision Count Register 13.4.2.41
// Port x MAC Transmit Total Collision Count Register 13.4.2.42
// Switch Engine Port 0 Ingress Filtered Count Register 13.4.3.30
// Switch Engine Port 1 Ingress Filtered Count Register 13.4.3.31
// Switch Engine Port 2 Ingress Filtered Count Register 13.4.3.32
// Switch Engine Port 0 Learn Discard Count Register 13.4.3.36
// Switch Engine Port 1 Learn Discard Count Register 13.4.3.37
// Switch Engine Port 2 Learn Discard Count Register 13.4.3.38
// Buffer Manager Port 0 Drop Count Register 13.4.4.6
// Buffer Manager Port 1 Drop Count Register 13.4.4.7
// Buffer Manager Port 2 Drop Count Register 13.4.4.8
// Buffer Manager Port 0 Ingress Rate Drop Register 13.4.4.23
// Buffer Manager Port 1 Ingress Rate Drop Register 13.4.4.24
// Buffer Manager Port 2 Ingress Rate Drop Register 13.4.4.25

typedef union {

    struct {
        unsigned COUNT : 32; //Phyusical Address
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_Count_t,
__SMSC9303_MAC_RX_UNDSZE_CNT_x_t,
__SMSC9303_MAC_RX_64_CNT_x_t,
__SMSC9303_MAC_RX_65_TO_127_CNT_x_t,
__SMSC9303_MAC_RX_128_TO_255_CNT_x_t,
__SMSC9303_MAC_RX_256_TO_511_CNT_x_t,
__SMSC9303_MAC_RX_512_TO_1023_CNT_x_t,
__SMSC9303_MAC_RX_1024_TO_MAX_CNT_x_t,
__SMSC9303_MAC_RX_OVRSZE_CNT_x_t,
__SMSC9303_MAC_RX_PKTOK_CNT_x_t,
__SMSC9303_MAC_RX_CRCERR_CNT_x_t,
__SMSC9303_MAC_RX_MULCST_CNT_x_t,
__SMSC9303_MAC_RX_BRDCST_CNT_x_t,
__SMSC9303_MAC_RX_PAUSE_CNT_x_t,
__SMSC9303_MAC_RX_FRAG_CNT_x_t,
__SMSC9303_MAC_RX_JABB_CNT_x_t,
__SMSC9303_MAC_RX_ALIGN_CNT_x_t,
__SMSC9303_MAC_RX_PKTLEN_CNT_x_t,
__SMSC9303_MAC_RX_GOODPKTLEN_CNT_x_t,
__SMSC9303_MAC_RX_SYMBOL_CNT_x_t,
__SMSC9303_MAC_RX_CTLFRM_CNT_x_t,
__SMSC9303_MAC_TX_DEFER_CNT_x_t,
__SMSC9303_MAC_TX_PAUSE_CNT_x_t,
__SMSC9303_MAC_TX_PKTOK_CNT_x_t,
__SMSC9303_MAC_TX_64_CNT_x_t,
__SMSC9303_MAC_TX_65_TO_127_CNT_x_t,
__SMSC9303_MAC_TX_128_TO_255_CNT_x_t,
__SMSC9303_MAC_TX_256_TO_511_CNT_x_t,
__SMSC9303_MAC_TX_512_TO_1023_CNT_x_t,
__SMSC9303_MAC_TX_1024_TO_MAX_CNT_x_t,
__SMSC9303_MAC_TX_UNDSZE_CNT_x_t,
__SMSC9303_MAC_TX_PKTLEN_CNT_x_t,
__SMSC9303_MAC_TX_BRDCST_CNT_x_t,
__SMSC9303_MAC_TX_MULCST_CNT_x_t,
__SMSC9303_MAC_TX_LATECOL_CNT_x_t,
__SMSC9303_MAC_TX_EXCCOL_CNT_x_t,
__SMSC9303_MAC_TX_SNGLECOL_CNT_x_t,
__SMSC9303_MAC_TX_MULTICOL_CNT_x_t,
__SMSC9303_MAC_TX_TOTALCOL_CNT_x_t,
__SMSC9303_FILTERED_CNT_0_t,
__SMSC9303_FILTERED_CNT_1_t,
__SMSC9303_FILTERED_CNT_2_t,
__SMSC9303_LRN_DISCRD_CNT_0_t,
__SMSC9303_LRN_DISCRD_CNT_1_t,
__SMSC9303_LRN_DISCRD_CNT_2_t,
__SMSC9303_BM_DRP_CNT_SRC_0_t,
__SMSC9303_BM_DRP_CNT_SRC_1_t,
__SMSC9303_BM_DRP_CNT_SRC_2_t,
__SMSC9303_BM_RATE_DRP_CNT_SRC_0_t,
__SMSC9303_BM_RATE_DRP_CNT_SRC_1_t,
__SMSC9303_BM_RATE_DRP_CNT_SRC_2_t;


// Port x MAC Transmit Configuration Register 13.4.2.23

typedef union {

    struct {
        unsigned TX_ENABLE : 1; // TX Enable
        unsigned TX_PAD_ENABLE : 1; // TX Pad Enable
        unsigned IFG_CFG : 5; // IFG Config
        unsigned MAC_COUNTER_TEST : 1; //MAC Counter Test
        unsigned : 24;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_MAX_TX_CFG_x_t;

// Port x MAC Transmit Flow Control Settigns Register 13.4.2.24

typedef union {

    struct {
        unsigned PAUSE_TIME : 16; //  Pause Time Value
        unsigned BACK_FF_RESET : 2; // Backoff Reset RX/TX
        unsigned : 14;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_MAC_TX_FC_SETTINGS_x_t;

// Switch Engine ALR Command Register 13.4.3.1

typedef union {

    struct {
        unsigned GET_NEXT_ENTRY : 1; // Get Next Entry
        unsigned GET_FIRST_ENTRY : 1; // Get First Entry
        unsigned MAKE_ENTRY : 1; //Make Entry
        unsigned : 29;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_ALR_CMD_t;

//Switch Engine ALR Write Data 0 Register 13.4.3.2

typedef union {

    struct {
        unsigned MAC_ADDR : 32; // MAC Address
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_ALR_WR_DAT_0_t;

//Switch Engine ALR Write Data 1 Register 13.4.3.3

typedef union {

    struct {
        unsigned MAC_ADDR : 16; // MAC Address
        unsigned PORT : 3; // Port
        unsigned PRIORITY : 3; // Priority
        unsigned PRIORITY_ENABLE : 1; // Priority Enable
        unsigned FILTER : 1; //Filter
        unsigned STATIC : 1; // Static
        unsigned AGE_OVERRIDE : 1; //Age/Override
        unsigned VALID : 1; // Valid
        unsigned : 5;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_ALR_WR_DAT_1_t;

//Switch Engine ALR Read Data 0 Register 13.4.3.4

typedef union {

    struct {
        unsigned MAC_ADDR : 32; // MAC Address
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_ALR_RD_DAT_0_t;

//Switch Engine ALR Read Data 1 Register 13.4.3.3

typedef union {

    struct {
        unsigned MAC_ADDR : 16; // MAC Address
        unsigned PORT : 3; // Port
        unsigned PRIORITY : 3; // Priority
        unsigned PRIORITY_ENABLE : 1; // Priority Enable
        unsigned FILTER : 1; //Filter
        unsigned STATIC : 1; // Static
        unsigned END_OF_TABLE : 1; //End of Table
        unsigned VALID : 1; // Valid
        unsigned : 5;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_ALR_RD_DAT_1_t;


//Switch Engine ALR Command Status Register  13.4.3.6

typedef union {

    struct {
        unsigned MAKE_PENDING : 1; // Make Pending
        unsigned ALR_INIT_DONE : 1; // ALR Init Done
        unsigned : 30;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_ALR_CMD_STS_t;

//swtich Engine ALR Configuration Register 13.4.3.7

typedef union {

    struct {
        unsigned ALR_AGE_TEST : 1; // ALR Age Test
        unsigned : 31;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC_SWE_ALR_CFG_t;

//Switch Engine VLAN Command Register 13.4.3.8

typedef union {

    struct {
        unsigned VLAN_PORT : 4; // VLAN/Port
        unsigned PVIDN_VLAN : 1; // PVIDnVLAN
        unsigned VLAN_RnW : 1; // VLAN RnW
        unsigned : 26;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_VLAN_CMD_t;

//Switch Engine VLAN Write Data Register 13.4.3.9
//Switch Engine VLAN Read Data Register 13.4.3.10

typedef union {

    struct {
        unsigned VID : 12;

        union {

            struct {
                unsigned PRIORITY : 3;
                unsigned : 3;
            };

            struct {
                unsigned UN_TAG_PORT_0 : 1;
                unsigned MEMBER_PORT_0 : 1;
                unsigned UN_TAG_PORT_1 : 1;
                unsigned MEMBER_PORT_1 : 1;
                unsigned UN_TAG_PORT_2 : 1;
                unsigned MEMBER_PORT_2 : 1;
            };
        };
        unsigned : 15;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_VLAN_WR_DATA_t,
__SMSC9303_SWE_VLAN_RD_DATA_t;

//Switch Engine VLAN Command Status Register 13.4.3.11
//Switch Engine DIFFSERV Command Status Register 13.4.3.15
// Switch Engine Ingress Rate Status Register 13.4.3.27

typedef union {

    struct {
        unsigned OP_PENDING : 1; // Operation Pending
        unsigned : 31;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_VLAN_CMD_STS_t,
__SMSC9303_SWE_DIFFSERV_TBL_CMD_STS_t,
__SMSC9303_SWE_INGRSS_RATE_CMD_STS_t;

//Switch Engine DIFFSERV Table Command Register 13.4.3.12

typedef union {

    struct {
        unsigned DIFFSERV_TBL_IDX : 6; // DIFFSERV Table Index
        unsigned : 1;
        unsigned DIFSERV_TBL_RnW : 1; // DIFFSERV table Read or Write
        unsigned : 24;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_DIFFSERV_TBL_CFG_t;

//Switch Engine DIFFSERV Table Write Data Register 13.4.3.13
//Switch Engine DIFFSERV Table Read Data Register 13.4.3.14

typedef union {

    struct {
        unsigned DIFFSERV_PRIORITY : 3; // DIFFSERV PRiority
        unsigned : 29;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_DIFFSERV_TBL_WR_DATA_t,
__SMSC9303_SWE_DIFFSERV_TBL_RD_DATA_t;


//Switch Engine Global Ingress Coniguration Register 13.4.3.16

typedef union {

    struct {
        unsigned VLAN_ENABLE : 1; // VLAN Enable
        unsigned VL_HGHR_PRI : 1; // VL Higher Priority
        unsigned USE_PREC : 1; // Use Precedence
        unsigned DRP_UNKN : 1; // Drop Unknown
        unsigned FLTR_MLTCST : 1; //Filter Multicast
        unsigned DA_HGHST_PRI : 1; // DA "Highest Priority
        unsigned SWE_CNT_TST : 1; //SWE Counter Test
        unsigned ENABLE_IGMP_MONITORING : 1; // Enable IGMP Monitoring
        unsigned : 1;
        unsigned USE_IP : 1; // Use IP
        unsigned IGMP_MONITOR_PORT : 3; // IGMP Monitor Port
        unsigned MONITOR_ECHO : 1; // Allow Monitor Echo
        unsigned USE_TAG : 1; // Use Tag
        unsigned VLAN_802_1Q_DISABLE; // 802.1Q VLAN Disable
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_GLOBAL_INGRESS_CFG_t;

//Switch Engine Port Ingress Configuiration Register 13.4.3.17

typedef union {

    struct {
        unsigned ENABLE_MEM_CHK : 3; // Enable Membership Checking
        unsigned ENABLE_LERN_ON_ING : 3; // Enable Learning on Ingress
        unsigned : 26;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_PORT_INGRESS_CFG_t;

//Switch Engine Admit Only VLAN Register  13.4.3.18

typedef union {

    struct {
        unsigned ADMIT_ONLY_VLAN : 3; // Admit Only VLAN
        unsigned : 29;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_ADMT_ONLY_VLAN_t;

//Switch Engine Port State Register 13.4.3.19

typedef union {

    struct {
        unsigned PORT_0 : 2; // Port State Port 0
        unsigned PORT_1 : 2; // Port State Port 1
        unsigned PORT_2 : 2; // Port State Port 2
        unsigned : 26;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_PORT_STATE_t;

//Switch Engine Priority to Queue Register 13.4.3.20

typedef union {

    struct {
        unsigned PRI_0_TFC_CLS : 2; // Priority 0 traffic class
        unsigned PRI_1_TFC_CLS : 2; // Priority 0 traffic class
        unsigned PRI_2_TFC_CLS : 2; // Priority 0 traffic class
        unsigned PRI_3_TFC_CLS : 2; // Priority 0 traffic class
        unsigned PRI_4_TFC_CLS : 2; // Priority 0 traffic class
        unsigned PRI_5_TFC_CLS : 2; // Priority 0 traffic class
        unsigned PRI_6_TFC_CLS : 2; // Priority 0 traffic class
        unsigned PRI_7_TFC_CLS : 2; // Priority 0 traffic class
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_PRI_TO_QUE_t;

//Switch Engine Port Mirroring Register 13.4.3.21

typedef union {

    struct {
        unsigned ENABLE_TX_MIRRORING : 1; // Enable TX Mirroring
        unsigned ENABLE_RX_MIRRORING : 1; // Enable RX Mirroring
        unsigned MIRROR : 3; // Mirrored Port
        unsigned SNIFFER : 3; // Sniffer Port
        unsigned RX_MIRROR_FILTER : 1; // Enable RX Mirroring Filtered
        unsigned : 23;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_PORT_MIRROR_t;

// Switch Engine Ingress Port Type Register 13.4.3.22

typedef union {

    struct {
        unsigned PORT_0_TYPE : 2; // Ingress Port Type Port 0
        unsigned PORT_1_TYPE : 2; // Ingress Port Type Port 1
        unsigned PORT_2_TYPE : 2; // Ingress Port Type Port 2
        unsigned : 26;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_INGRESS_PORT_TYP_t;

//Switch Engine Broadcast Throttling Register 13.4.3.23

typedef union {

    struct {
        unsigned PORT_0_LEVEL : 8; // Broadcast Throttle Level Port 0
        unsigned PORT_0_ENABLE : 1; // Brodcast Throttle Enable Port 0
        unsigned PORT_1_LEVEL : 8; // Broadcast Throttle Level Port 1
        unsigned PORT_1_ENABLE : 1; // Brodcast Throttle Enable Port 1
        unsigned PORT_2_LEVEL : 8; // Broadcast Throttle Level Port 2
        unsigned PORT_2_ENABLE : 1; // Brodcast Throttle Enable Port 2
        unsigned : 5;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_BCST_THROT_t;

// Switch Engine Admit Non Member Register 13.4.3.24

typedef union {

    struct {
        unsigned Admin_NON_MEMBER : 3; // Admit Non Member
        unsigned : 29;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_ADMT_N_MEMBER_t;

// Switch Engine Ingress Rate Configuration Register 13.4.3.25

typedef union {

    struct {
        unsigned RATE_ENABLE : 1; // Ingress Rate Enable
        unsigned RATE_MODE : 2; // Rate Mode
        unsigned : 29;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_INGRSS_RATE_CFG_t;

// Switch Engine Ingress Rate Command Register 13.4.3.26

typedef union {

    struct {
        unsigned CIR_ADDR : 5; // CIR Address
        unsigned TYPE : 2; // Type
        unsigned RATE_RnW : 1; // Ingress Rate RnW
        unsigned : 24;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_INGRSS_RATE_CMD_t;


// Switch Engine Ingress Rate Write Data Register 13.4.3.28
// Switch Engine Ingress Rate Read Data Register 13.4.3.29

typedef union {

    struct {
        unsigned DATA : 16;
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_INGRSS_RATE_WR_DATA_t,
__SMSC9303_SWE_INGRSS_RATE_RD_DATA_t;

typedef union {

    struct {
        unsigned FILTERED : 32; // Filtered
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_FILTERED_CNT_t;

//Switch Engine Port 0 Ingress VLAN Priority Regeneration Table Register  13.4.3.33
//Switch Engine Port 1 Ingress VLAN Priority Regeneration Table Register  13.4.3.34
//Switch Engine Port 2 Ingress VLAN Priority Regeneration Table Register  13.4.3.35

typedef union {

    struct {
        unsigned REGEN_0 : 2; // Regen0
        unsigned REGEN_1 : 2; // Regen1
        unsigned REGEN_2 : 2; // Regen2
        unsigned REGEN_3 : 2; // Regen3
        unsigned REGEN_4 : 2; // Regen4
        unsigned REGEN_5 : 2; // Regen5
        unsigned REGEN_6 : 2; // Regen6
        unsigned REGEN_7 : 2; // Regen7
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_INGRSS_REGEN_TBL_0_t,
__SMSC9303_SWE_INGRSS_REGEN_TBL_1_t,
__SMSC9303_SWE_INGRSS_REGEN_TBL_2_t;

// Switch Engine Interrupt Mask Register 13.4.3.39
// Buffer Manager Interrupt Mask Register 13.4.4.26

typedef union {

    struct {
        unsigned IMR : 1; // Interrupt Mask
        unsigned : 31;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_IMR_t,
__SMSC9303_BM_IMR_t;

//Switch Engine Interrupt Pending Register 13.4.3.40

typedef union {

    struct {
        unsigned IP : 1; // Interrupt Pending
        unsigned SET_A_VALID : 1; // Set A Valid
        unsigned SOURCE_PORT_A : 2; // Source Port A
        unsigned DROP_REASON_A : 4; // Drop Reason A
        unsigned SET_B_VALID : 1; // Set B Valid
        unsigned SOURCE_PORT_B : 2; // Source Port B
        unsigned DROP_REASON_B : 4; // Drop Reason B
        unsigned : 17;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_SWE_IPR_t;

//Buffer Manager Congifuration Register  13.4.4.1

typedef union {

    struct {
        unsigned DROP_ON_RED : 1; // Drop on Red
        unsigned DROP_ON_YELLOW : 1; // Drop on Yellow
        unsigned EGRESS_RATE_ENABLE : 3; // Egress Rate Enable
        unsigned FPQS : 1; // Fixed PRiority Queue Servicing
        unsigned BM_CNTR_TEST : 1; // BM Counter Test
        unsigned : 25;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_BM_CFG_t;

//Buffer Manager Drop Level Register 13.4.4.2

typedef union {

    struct {
        unsigned DROP_LEVEL_HIGH : 8; // Drop Level High
        unsigned DROP_LEVEL_LOW : 8; // Drop Level Low
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_BM_DROP_LVL_t;

// Buffer Manager Flow Control Pause Level Register 13.4.4.3

typedef union {

    struct {
        unsigned PAUSE_LEVEL_HIGH : 8; // Pause Level High
        unsigned PAUSE_LEVEL_LOW : 8; // Pause Level Low
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_FC_PAUSE_LVL_t;

// Buffer Manager Flow Control Resume Level Register 13.4.4.4

typedef union {

    struct {
        unsigned RESUME_LEVEL_HIGH : 8; // Resume Level High
        unsigned RESUME_LEVEL_LOW : 8; // Resume Level Low
        unsigned : 16;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC_FC_PAUSE_LVL_t;


//Buffer Manager Broadcast Buffer Level 13.4.4.5

typedef union {

    struct {
        unsigned BCAST_DROP_LEVEL : 8; // Broadcast Drop Level
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_BM_BCST_LEVEL_t;

//Buffer Manager Random Discard Table Write Data Register 13.4.4.11
//Buffer Manager Random Discard Table Read Data Register 13.4.4.12

typedef union {

    struct {
        unsigned DRP_PROB : 10; // Drop Probablity
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_BM_RNDM_DSCRD_TBL_WDATA_t,
__SMSC9303_BM_RNDM_DSCRD_TBL_RDATA_t;

//Buffer Manager Egress Port Type Register 13.4.4.13

typedef union {

    struct {
        unsigned PORT_0_TYPE : 2; // Egress Port Type Port 0
        unsigned PORR_0_TAG : 1; // Change Tag Port 0
        unsigned PORT_0_PRIORITY : 1; // Change Priority Port 0
        unsigned PORT_0_VLAN : 1; // Change VLAN ID Port 0
        unsigned PORT_0_INSERT_TAG : 1; // Inster Tag Port 0
        unsigned PORT_0_VID_PRIORITY : 1; // VID/PRiority Select Port 0
        unsigned : 1;
        unsigned PORT_1_TYPE : 2; // Egress Port Type Port 1
        unsigned PORR_1_TAG : 1; // Change Tag Port 1
        unsigned PORT_1_PRIORITY : 1; // Change Priority Port 1
        unsigned PORT_1_VLAN : 1; // Change VLAN ID Port 1
        unsigned PORT_1_INSERT_TAG : 1; // Inster Tag Port 1
        unsigned PORT_1_VID_PRIORITY : 1; // VID/PRiority Select Port 1
        unsigned : 1;
        unsigned PORT_2_TYPE : 2; // Egress Port Type Port 2
        unsigned PORR_2_TAG : 1; // Change Tag Port 2
        unsigned PORT_2_PRIORITY : 1; // Change Priority POort 2
        unsigned PORT_2_VLAN : 1; // Change VLAN ID Port 2
        unsigned PORT_2_INSERT_TAG : 1; // Inster Tag Port 2
        unsigned PORT_2_VID_PRIORITY : 1; // VID/PRiority Select Port 2
        unsigned : 9;

    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_BM_EGRSS_PORT_TYPE_t;


// Buffer Manager Port 0 Egress Rate Priorituy Queue 0/1 Register  13.4.4.14
// Buffer Manager Port 1 Egress Rate Priorituy Queue 0/1 Register  13.4.4.16
// Buffer Manager Port 2 Egress Rate Priorituy Queue 0/1 Register  13.4.4.18

typedef union {

    struct {
        unsigned QUEUE_0 : 13;
        unsigned QUEUE_1 : 13;
        unsigned : 6;
    };

    struct {
        uint32_t d : 32;
    };
} _SMSC9303_BM_EGRSS_RATE_00_01_t,
_SMSC9303_BM_EGRSS_RATE_10_11_t,
_SMSC9303_BM_EGRSS_RATE_20_21_t;

// Buffer Manager Port 0 Egress Rate Priorituy Queue 2/3 Register  13.4.4.16
// Buffer Manager Port 1 Egress Rate Priorituy Queue 2/3 Register  13.4.4.17
// Buffer Manager Port 2 Egress Rate Priorituy Queue 2/3 Register  13.4.4.19

typedef union {

    struct {
        unsigned QUEUE_2 : 13;
        unsigned QUEUE_3 : 13;
        unsigned : 6;
    };

    struct {
        uint32_t d : 32;
    };
} _SMSC9303_BM_EGRSS_RATE_02_03_t,
_SMSC9303_BM_EGRSS_RATE_12_13_t,
_SMSC9303_BM_EGRSS_RATE_22_23_t;

// Buffer Manager Port 0 Devault VLAN ID and PRiority Register 13.4.4.20
// Buffer Manager Port 1 Devault VLAN ID and PRiority Register 13.4.4.21
// Buffer Manager Port 2 Devault VLAN ID and PRiority Register 13.4.4.22

typedef union {

    struct {
        unsigned VLAN_ID : 12; // Default VLAN ID
        unsigned PRIORITY : 3; // Default Priority
        unsigned : 17;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_BM_VLAN_0_t,
__SMSC9303_BM_VLAN_1_t,
__SMSC9303_BM_VLAN_2_t;

// Buffer Manager Interrupt Pending Register 13.4.4.27

typedef union {

    struct {
        unsigned SET_A_VALID : 1; // Set A Valid
        unsigned SOURCE_PORT_A : 2; // Source Port A
        unsigned DROP_REASON_A : 4; // Drop Reason A
        unsigned SET_B_VALID : 1; // Set B Valid
        unsigned SOURCE_PORT_B : 2; // Source Port B
        unsigned DROP_REASON_B : 4; // Drop Reason B
        unsigned : 18;
    };

    struct {
        uint32_t d : 32;
    };
} __SMSC9303_BM_IPR;

#endif	// _SMSC_9303_H_


