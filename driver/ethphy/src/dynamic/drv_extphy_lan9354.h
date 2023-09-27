/*******************************************************************************
  LAN9354 definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_lan9354.h

  Summary:
    LAN9354 definitions

  Description:
    This file provides the LAN9354 definitions.
 *******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _LAN_9354_H_

#define _LAN_9354_H_

#include <stdint.h>

// VIRTUAL PHY MII SERIALLY ADDRESSABLE REGISTER INDEX
// std IEEE 802.3

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
    PHY_REG_ANEXP       = 6,
     */
    // specific vendor registers: 16-31
    // Note: reg 27 exists for the Port A, B only!
    LAN9354_REG_SPECIAL_CTRL_STAT_IND = 27,
    LAN9354_REG_CTRL_STAT   = 31,
}LAN9354_VENDOR_REGS;

// register 31 layout
typedef enum
{
    HD_10MBPS       = 1,    // half duplex, 10 Mbps
    HD_100MBPS      = 2,    // half duplex, 100 Mbps
    FD_10MBPS       = 5,    // full duplex, 10 Mbps
    FD_100MBPS      = 6,    // full duplex, 100 Mbps
                            //
    // any other value is reserved
}LAN9354_SPEED_DUPLEX;

typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t                        : 2;
        uint16_t SPEED_DUPLEX           : 3;    // a LAN9354_SPEED_DUPLEX value
        uint16_t RMII_CLOCK_STRENGTH    : 1;
        uint16_t RMII_CLOCK_DIR         : 1;
        uint16_t SWITCH_COLL_TEST       : 1;
        uint16_t MODE_10                : 2;
        uint16_t                        : 4;
        uint16_t SWITCH_LOOPBACK        : 1;
        uint16_t MODE_2                 : 1;
    };
}LAN9354_REG_CTRL_STAT_bits_t;

// register 27 layout
typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t            : 4;
        uint16_t XPOL       : 1;
        uint16_t FEFI_EN    : 1;
        uint16_t            : 5;
        uint16_t SQEOFF     : 1;
        uint16_t            : 1;
        uint16_t AMDIXSTATE : 1;
        uint16_t AMDIXEN    : 1;
        uint16_t AMDIXCTRL  : 1;
    };
}LAN9354_REG_SPECIAL_CTRL_STAT_IND_bits_t;

#define LAN9354_AMDIXCTRL_MASK  0x8000
#define LAN9354_AMDIXEN_MASK    0x4000
#define LAN9354_AMDIXSTATE_MASK 0x2000

#define LAN9354_AMDIX_BITS_MASK 0xe000

// register 0x74 HW_CFG layout
typedef union
{
    uint32_t    w;
    struct
    {
        uint32_t                    : 25;
        uint32_t AMDIX_EN_STRAP_A   : 1;
        uint32_t AMDIX_EN_STRAP_B   : 1;
        uint32_t READY              : 1;
        uint32_t                    : 4;
    };
}LAN9354_REG_HW_CFG_bits_t;

// LAN9354 System Control and Status Registers
// The structure and definition of each registers are in the LAN9354 DS
typedef enum
{
    // General registers
    LAN9354_REG_ID_REV                              = 0x050, // Chip ID and Revision Register
    LAN9354_REG_IRQ_CFG                             = 0x054, // Interrupt Configuration Register
    LAN9354_REG_INT_STS                             = 0x058, // Interrupt Status Register
    LAN9354_REG_INT_EN                              = 0x05C, // Interrupt Enable Register
    LAN9354_REG_BYTE_TEST                           = 0x064, // Byte Order Test Register
    LAN9354_REG_HW_CFG                              = 0x074, // Hardware Configuration Register
    LAN9354_REG_GPT_CFG                             = 0x08C, // General Purpose Timer Configuration Register
    LAN9354_REG_GPT_CNG                             = 0x090, // General Purpose Timer Count Register
    LAN9354_REG_FREE_RUN                            = 0x09C, // Free Running Counter Register
    LAN9354_REG_PMI_DATA                            = 0x0A4, // PHY Management Interface Data Register
    LAN9354_REG_PMI_ACCESS                          = 0x0A8, // PHY Management Interface Access Register
                                //
    // 1588 registers
    LAN9354_REG_1588_CMD_CTL                        = 0x100,
    LAN9354_REG_1588_GENERAL_CONFIG                 = 0x104,
    LAN9354_REG_1588_INT_STS                        = 0x108,
    LAN9354_REG_1588_INT_EN                         = 0x10C,
    LAN9354_REG_1588_CLOCK_SEC                      = 0x110,
    LAN9354_REG_1588_CLOCK_NS                       = 0x114,
    LAN9354_REG_1588_CLOCK_SUBNS                    = 0x118,
    LAN9354_REG_1588_CLOCK_RATE_ADJ                 = 0x11C,
    LAN9354_REG_1588_CLOCK_TEMP_RATE_ADJ            = 0x120,
    LAN9354_REG_1588_CLOCK_TEMP_RATE_DURATION       = 0x124,
    LAN9354_REG_1588_CLOCK_STEP_ADJ                 = 0x128,
    LAN9354_REG_1588_CLOCK_TARGET_SEC_A             = 0x12C,
    LAN9354_REG_1588_CLOCK_TARGET_NS_A              = 0x130,
    LAN9354_REG_1588_CLOCK_TARGET_RELOAD_SEC_A      = 0x134, 
    LAN9354_REG_1588_CLOCK_TARGET_RE_LOAD_NS_A      = 0x138,
    LAN9354_REG_1588_CLOCK_TARGET_SEC_B             = 0x13C,
    LAN9354_REG_1588_CLOCK_TARGET_NS_B              = 0x140,
    LAN9354_REG_1588_CLOCK_TARGET_RELOAD_SEC_B      = 0x144, 
    LAN9354_REG_1588_CLOCK_TARGET_RE_LOAD_NS_B      = 0x148,
    LAN9354_REG_1588_USER_MAC_HI                    = 0x14C, 
    LAN9354_REG_1588_USER_MAC_LO                    = 0x150, 
    LAN9354_REG_1588_BANK_PORT_GPIO_SEL             = 0x154,
    LAN9354_REG_1588_LATENCY_x                      = 0x158, 
    LAN9354_REG_1588_RX_PARSE_CONFIG_x              = 0x158,
    LAN9354_REG_1588_TX_PARSE_CONFIG_x              = 0x158,
    LAN9354_REG_1588_ASYM_PEERDLY_x                 = 0x15C,
    LAN9354_REG_1588_RX_TIMESTAMP_CONFIG_x          = 0x15C,
    LAN9354_REG_1588_TX_TIMESTAMP_CONFIG_x          = 0x15C,
    LAN9354_REG_1588_GPIO_CAP_CONFIG                = 0x15C,
    LAN9354_REG_1588_CAP_INFO_x                     = 0x160, 
    LAN9354_REG_1588_RX_TS_INSERT_CONFIG_x          = 0x160,
    LAN9354_REG_1588_RX_CF_MOD_x                    = 0x164, 
    LAN9354_REG_1588_TX_MOD_x                       = 0x164, 
    LAN9354_REG_1588_RX_FILTER_CONFIG_x             = 0x168,
    LAN9354_REG_1588_TX_MOD2_x                      = 0x168, 
    LAN9354_REG_1588_RX_INGRESS_SEC_x               = 0x16C,
    LAN9354_REG_1588_TX_EGRESS_SEC_x                = 0x16C,
    LAN9354_REG_1588_GPIO_RE_CLOCK_SEC_CAP_x        = 0x16C,
    LAN9354_REG_1588_RX_INGRESS_NS_x                = 0x170,
    LAN9354_REG_1588_TX_EGRESS_NS_x                 = 0x170,
    LAN9354_REG_1588_GPIO_RE_CLOCK_NS_CAP_x         = 0x170,
    LAN9354_REG_1588_RX_MSG_HEADER_x                = 0x174,
    LAN9354_REG_1588_TX_MSG_HEADER_x                = 0x174,
    LAN9354_REG_1588_RX_PDREQ_SEC_x                 = 0x178,
    LAN9354_REG_1588_TX_DREQ_SEC_x                  = 0x178, 
    LAN9354_REG_1588_GPIO_FE_CLOCK_SEC_CAP_x        = 0x178,
    LAN9354_REG_1588_RX_PDREQ_NS_x                  = 0x17C, 
    LAN9354_REG_1588_TX_DREQ_NS_x                   = 0x17C, 
    LAN9354_REG_1588_GPIO_FE_CLOCK_NS_CAP_x         = 0x17C,
    LAN9354_REG_1588_RX_PDREQ_CF_HI_x               = 0x180,
    LAN9354_REG_1588_TX_ONE_STEP_SYNC_SEC           = 0x180,
    LAN9354_REG_1588_RX_PDREQ_CF_LOW_x              = 0x184,
    LAN9354_REG_1588_RX_CHKSUM_DROPPED_CNT_x        = 0x188,
    LAN9354_REG_1588_RX_FILTERED_CNT_x              = 0x18C,

    // Switch Registers
    LAN9354_REG_MANUAL_FC_1                         = 0x1A0, // Port 1 Manual Flow Control Register
    LAN9354_REG_MANUAL_FC_2                         = 0x1A4, // Port 2 Manual Flow Control Register
    LAN9354_REG_MANUAL_FC_0                         = 0x1A8, // Port 0 Manual Flow Control Register
    LAN9354_REG_SWITCH_CSR_DATA                     = 0x1AC, // Switch Fabric CSR Interface Data Register
    LAN9354_REG_SWITCH_CSR_CMD                      = 0x1B0, // Switch Fabric CSR Interface Command Register

    // EEPROM/LED Registers
    LAN9354_REG_E2P_CMD                             = 0x1B4, // EEPROM Command Register
    LAN9354_REG_E2P_DATA                            = 0x1B8, // EEPROM Data Register
    LAN9354_REG_LED_CFG                             = 0x1BC, // LED Configuration Register

    // Virtual PHY Registers
    LAN9354_REG_VPHY_BASIC_CTRL                     = 0x1C0, // Virtual PHY Basic Control Register
    LAN9354_REG_VPHY_BASIC_STATUS                   = 0x1C4, // Virtual PHY Basic Status Register
    LAN9354_REG_VPHY_ID_MSB                         = 0x1C8, // Virtual PHY Identification MSB Register
    LAN9354_REG_VPHY_ID_LSB                         = 0x1CC, // Virtual PHY Identification LSB Register
    LAN9354_REG_VPHY_AN_ADV                         = 0x1D0, // Virtual PHY Auto-Negotiation Advertisement Register
    LAN9354_REG_VPHY_AN_LP_BASE_ABILITY             = 0x1D4, // Virtual PHY Auto-Negotiation Link Partner Base Page Ability Register
    LAN9354_REG_VPHY_AN_EXP                         = 0x1D8, // Virtual PHY Auto-Negotiation Expansion Register
    LAN9354_REG_VPHY_SPECIAL_CONTROL_STATUS         = 0x1DC, // Virtual PHY Special Control/Status Register

    // GPIO Registers
    LAN9354_REG_GPIO_CFG                            = 0x1E0, // General Purpose I/O Configuration Register
    LAN9354_REG_GPIO_DATA_DIR                       = 0x1E4, // General Purpose I/O Data & Direction Register
    LAN9354_REG_GPIP_INT_STS_EN                     = 0x1E8, // General Purpose I/O Interupt Status and Enable Register
                                     
    // Switch Fabric MAC Address Registers
    LAN9354_REG_SWITCH_MAC_ADDRH                    = 0x1F0, // Switch MAC Address High Register
    LAN9354_REG_SWITCH_MAC_ADDRL                    = 0x1F4, // Switch MAC Address Low Register

    // Reset Register
    LAN9354_REG_RESET_CTL                           = 0x1F8, // Reset Control Register

    // Switch Fabric CSR Interface Direct Data Registers
    LAN9354_REG_SW_RESET                            = 0x200, // Switch Reset Register
    LAN9354_REG_SW_IMR                              = 0x204, // Switch GlobalInterrupt Mask Register
    LAN9354_REG_SF_MAC_RX_CFG_0                     = 0x208, // Port 0 MAC Receive Configuration Register
    LAN9354_REG_SF_MAC_TX_CFG_0                     = 0x20C, // Port 0 MAC Transmit Configuration Register
    LAN9354_REG_SF_MAC_TX_FC_SETTINGS_0             = 0x210, // Port 0 MAC Transmit Flow Control Settings Register
    LAN9354_REG_SF_MAC_IMR_0                        = 0x214, // Port 0 MAC Interrupt Mask Register
    LAN9354_REG_SF_MAC_RX_CFG_1                     = 0x218, // Port 1 MAC Receive Configuration Register
    LAN9354_REG_SF_MAC_TX_CFG_1                     = 0x21C, // Port 1 MAC Transmit Configuration Register
    LAN9354_REG_SF_MAC_TX_FC_SETTINGS_1             = 0x220, // Port 1 MAC Transmit Flow Control Settings Register
    LAN9354_REG_SF_MAC_IMR_1                        = 0x224, // Port 1 MAC Interrupt Mask Register
    LAN9354_REG_SF_MAC_RX_CFG_2                     = 0x228, // Port 2 MAC Receive Configuration Register
    LAN9354_REG_SF_MAC_TX_CFG_2                     = 0x22C, // Port 2 MAC Transmit Configuration Register
    LAN9354_REG_SF_MAC_TX_FC_SETTINGS_2             = 0x230, // Port 2 MAC Transmit Flow Control Settings Register
    LAN9354_REG_SF_MAC_IMR_2                        = 0x234, // Port 2 MAC Interrupt Mask Register
    LAN9354_REG_SF_SWE_ALR_CMD                      = 0x238, // Switch Engine ALR Command REgister
    LAN9354_REG_SF_SWE_ALR_WR_DAT_0                 = 0x23C, // Switch Engine ALR Write Data 0 Register
    LAN9354_REG_SF_SWE_ALR_WR_DAT_1                 = 0x240, // Switch Engine ALR Write Data 1 Register
    LAN9354_REG_SF_SWE_ALR_CFG                      = 0x244, // Switch Engine ALR Configuration Register
    LAN9354_REG_SF_SWE_VLAN_CMD                     = 0x248, // Switch Engine VLAN Command Register
    LAN9354_REG_SF_SWE_VLAN_WR_DATA                 = 0x24C, // Switch Engine VLAN Write Register
    LAN9354_REG_SF_SWE_DIFFSERV_TBL_CMD             = 0x250, // Switch Engine DIFSERV Table Command Register
    LAN9354_REG_SF_SWE_DIFFSERV_TBL_WR_DATA         = 0x254, // Switch Engine DIFSERV Table Write Data
    LAN9354_REG_SF_SWE_GLB_INGRESS_CFG              = 0x258, // Switch Engine Global Ingress Configuration Register
    LAN9354_REG_SF_SWE_PORT_INGRESS_CFG             = 0x25C, // Switch Engine Port Ingress Configuration Register
    LAN9354_REG_SF_SWE_ADMT_ONLY_VLAN               = 0x260, // Switch Engine Admit Only VLAN Register
    LAN9354_REG_SF_SWE_PORT_STATE                   = 0x264, // Switch Engine Port State Register
    LAN9354_REG_SF_SWE_PRI_TO_QUE                   = 0x268, // Switch Engine PRiority to Queue Register
    LAN9354_REG_SF_SWE_PORT_MIRROR                  = 0x26C, // Switch Engine Port Mirroring Register
    LAN9354_REG_SF_SWE_INGRESS_PORT_TYPE            = 0x270, // Switch Engine Ingress Port Type Register
    LAN9354_REG_SF_SWE_BCST_THROT                   = 0x274, // Switch Engine Broadcast Throttling Register
    LAN9354_REG_SF_SWE_ADMT_N_MEMBER                = 0x278, // Swtich Engine Admit Non Member Register
    LAN9354_REG_SF_SWE_INGRESS_RATE_CFG             = 0x27C, // Switch Engine Ingress Rate Configuration Register
    LAN9354_REG_SF_SWE_INGRESS_RATE_CMD             = 0x280, // Switch Engine Ingress Rate Command Register
    LAN9354_REG_SF_SWE_INGRESS_RATE_WR_DATA         = 0x284, // Switch Engine Ingress Rate Write Data Register
    LAN9354_REG_SF_SWE_INGRESS_REGEN_TBL_0          = 0x288, // Switch Engine Port 0 Learn Discard Count Register
    LAN9354_REG_SF_SWE_INGRESS_REGEN_TBL_1          = 0x28C, // Switch Engine Port 1 Learn Discard Count Register
    LAN9354_REG_SF_SWE_INGRESS_REGEN_TBL_2          = 0x290, // Switch Engine Port 2 Learn Discard Count Register
    LAN9354_REG_SF_SWE_IMR                          = 0x294, // Switch Engine Interrupt Mask Register
    LAN9354_REG_SF_BM_CFG                           = 0x298, // Buffer Manager Configuration Register
    LAN9354_REG_SF_BM_DROP_LVL                      = 0x29C, // Buffer Manager Drop Level Register
    LAN9354_REG_SF_BM_FC_PAUSE_LVL                  = 0x2A0, // Buffer Manager Flow Control Pause Level Register
    LAN9354_REG_SF_BM_FC_RESUME_LVL                 = 0x2A4, // Buffer Manager Flow Control Resume Level Register
    LAN9354_REG_SF_BM_BCST_LVL                      = 0x2A8, // Buffer Manager Broadcast Buffer Level Register
    LAN9354_REG_SF_BM_RNDM_DSCRD_TBL_CMD            = 0x2AC, // Buffer Manager Random Discard Table Command Register
    LAN9354_REG_SF_BM_RNDM_DSCRD_TBL_WDATA          = 0x2B0, // Buffer Manager Random Discard Table Write Data Register
    LAN9354_REG_SF_BM_EGRSS_PORT_TYPE               = 0x2B4, // Buffer Manager Egress Port Type Register
    LAN9354_REG_SF_BM_EGRSS_RATE_00_01              = 0x2B8, // Buffer Manager Port 0 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_REG_SF_BM_EGRSS_RATE_02_03              = 0x2BC, // Buffer Manager Port 0 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_REG_SF_BM_EGRSS_RATE_10_11              = 0x2C0, // Buffer Manager Port 1 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_REG_SF_BM_EGRSS_RATE_12_13              = 0x2C4, // Buffer Manager Port 1 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_REG_SF_BM_EGRSS_RATE_20_21              = 0x2C8, // Buffer Manager Port 2 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_REG_SF_BM_EGRSS_RATE_22_23              = 0x2CC, // Buffer Manager Port 2 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_REG_SF_BM_VLAN_0                        = 0x2D0, // Buffer Manager Port 0 Default VLAN ID and PRiority Register
    LAN9354_REG_SF_BM_VLAN_1                        = 0x2D4, // Buffer Manager Port 1 Default VLAN ID and PRiority Register
    LAN9354_REG_SF_BM_VLAN_2                        = 0x2D8, // Buffer Manager Port 2 Default VLAN ID and PRiority Register
    LAN9354_REG_SF_BM_IMR                           = 0x2DC, // Buffer Manager Interrupt Mask Register
}LAN9354_CSR_REGISTERS;



// INDIRECTLY ACCESSIBLE SWITCH CONTROL AND STATUS REGISTERS
// The structure and definition of each registers are in the LAN9354 DS
typedef enum
{
    // General Switch CSRs
    LAN9354_REG_SF_IA_SW_DEV_ID                         = 0x0000, // Switch Device ID Register
    LAN9354_REG_SF_IA_SW_RESET                          = 0x0001, // Switch Reset Register
    LAN9354_REG_SF_IA_SW_IMR                            = 0x0004, // Switch Global Interrupt Mask Register
    LAN9354_REG_SF_IA_SW_IPR                            = 0x0005, // Switch Global Interrupt Pending Register
                                   
    // Switch Port 0 CSRs (x=0)
    LAN9354_REG_SF_IA_MAC_VER_ID_0                      = 0x0400, // Port 0 MAC Version ID Register
    LAN9354_REG_SF_IA_MAC_RX_CFG_0                      = 0x0401, // Port 0 MAC Receive Configuration Register
    LAN9354_REG_SF_IA_MAC_RX_UNDSZE_CNT_0               = 0x0410, // Port 0 MAC Receive Undersize Count Register
    LAN9354_REG_SF_IA_MAC_RX_64_CNT_0                   = 0x0411, // Port 0 MAC REceive 64 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_65_TO_127_CNT_0            = 0x0412, // Port 0 Mac Receive 65 to 127 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_128_TO_255_CNT_0           = 0x0413, // Port 0 Mac Receive 128 to 255 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_256_TO_511_CNT_0           = 0x0414, // Port 0 Mac Receive 255 to 511 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_512_TO_1023_CNT_0          = 0x0415, // Port 0 Mac Receive 512 to 1023 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_1024_TO_MAX_CNT_0          = 0x0416, // Port 0 Mac Receive 1024 to Max Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_OVRSZE_CNT_0               = 0x0417, // Port 0 MAC Receive Oversize Count Register
    LAN9354_REG_SF_IA_MAC_RX_PKTOK_CNT_0                = 0x0418, // Port 0 MAC Receive OK Count Register
    LAN9354_REG_SF_IA_MAC_RX_CRCERR_CNT_0               = 0x0419, // Port 0 MAC Receive CRC Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_MULCST_CNT_0               = 0x041A, // Port 0 MAC Receive Multicast Count Register
    LAN9354_REG_SF_IA_MAC_RX_BRDCST_CNT_0               = 0x041B, // Port 0 MAC Receive Broadcast Count Register
    LAN9354_REG_SF_IA_MAC_RX_PAUSE_CNT_0                = 0x041C, // Port 0 MAC Receive Pause Count Register
    LAN9354_REG_SF_IA_MAC_RX_FRAG_CNT_0                 = 0x041D, // Port 0 MAC Receive Fragment Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_JABB_CNT_0                 = 0x041E, // Port 0 MAC Receive Jabber Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_ALIGN_CNT_0                = 0x041F, // Port 0 MAC Receive Alignment Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_PKTLEN_CNT_0               = 0x0420, // Port 0 MAC Receive Packet Length Count Register
    LAN9354_REG_SF_IA_MAC_RX_GOODPKTLEN_CNT_0           = 0x0421, // Port 0 MAC Receive Good Packet Length Count Register
    LAN9354_REG_SF_IA_MAC_RX_SYMBL_CNT_0                = 0x0422, // Port 0 MAC Receive Symbol Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_CTLFRM_CNT_0               = 0x0423, // Port 0 MAC Receive Control Frame Count Register
    LAN9354_REG_SF_IA_MAC_TX_CFG_0                      = 0x0440, // Port 0 MAC Transmit Configuration Register
    LAN9354_REG_SF_IA_MAC_TX_FC_SETTINGS_0              = 0x0441, // Port 0 MAC Transmit Flow Control Settings Register
    LAN9354_REG_SF_IA_MAC_TX_DEFER_CNT_0                = 0x0451, // Port 0 MAC Transmit Deferred Count Register
    LAN9354_REG_SF_IA_MAC_TX_PAUSE_CNT_0                = 0x0452, // Port 0 MAC Transmit Pause Count Register
    LAN9354_REG_SF_IA_MAC_TX_PKTOK_CNT_0                = 0x0453, // Port 0 MAC Transmit OK Count Register
    LAN9354_REG_SF_IA_MAC_TX_64_CNT_0                   = 0x0454, // Port 0 MAC Transmit 64 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_65_TO_127_CNT_0            = 0x0455, // Port 0 Mac Transmit 65 to 127 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_128_TO_255_CNT_0           = 0x0456, // Port 0 Mac Transmit 128 to 255 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_256_TO_511_CNT_0           = 0x0457, // Port 0 Mac Transmit 255 to 511 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_512_TO_1023_CNT_0          = 0x0458, // Port 0 Mac Transmit 512 to 1023 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_1024_TO_MAX_CNT_0          = 0x0459, // Port 0 Mac Transmit 1024 to Max Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_UNDSIZE_CNT_0              = 0x045A, // Port 0 MAC Transmit Undersize Count Register
    LAN9354_REG_SF_IA_MAC_TX_PKTLEN_CNT_0               = 0x045C, // Port 0 MAC Transmit Packet Length Count Register
    LAN9354_REG_SF_IA_MAC_TX_BRDCST_CNT_0               = 0x045D, // Port 0 MAC Transmit Broadcast Count Register
    LAN9354_REG_SF_IA_MAC_TX_MULCST_CNT_0               = 0x045E, // Port 0 MAC Transmit Multicast Count Register
    LAN9354_REG_SF_IA_MAC_TX_LATECOL_CNT_0              = 0x045F, // Port 0 MAC Transmit Late Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_EXCOL_CNT_0                = 0x0460, // Port 0 MAC Transmit Excessive Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_SINGLECOL_CNT_0            = 0x0461, // Port 0 MAC Transmit Single Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_MULTICOL_CNT_0             = 0x0462, // Port 0 MAC Transmit Multiple Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_TOTALCOL_CNT_0             = 0x0463, // Port 0 MAC Transmit Total Collision Count Register
    LAN9354_REG_SF_IA_MAC_IMR_0                         = 0x0480, // Port 0 MAC Interrupt Mask Register
    LAN9354_REG_SF_IA_MAC_IPR_0                         = 0x0481, // Port 0 MAC Interrupt Pending REgister

    // Switch Port 1 CSRs (x=1)
    LAN9354_REG_SF_IA_MAC_VER_ID_1                      = 0x0800, // Port 1 MAC Version ID Register
    LAN9354_REG_SF_IA_MAC_RX_CFG_1                      = 0x0801, // Port 1 MAC Receive Configuration Register
    LAN9354_REG_SF_IA_MAC_RX_UNDSZE_CNT_1               = 0x0810, // Port 1 MAC Receive Undersize Count Register
    LAN9354_REG_SF_IA_MAC_RX_64_CNT_1                   = 0x0811, // Port 1 MAC REceive 64 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_65_TO_127_CNT_1            = 0x0812, // Port 1 Mac Receive 65 to 127 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_128_TO_255_CNT_1           = 0x0813, // Port 1 Mac Receive 128 to 255 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_256_TO_511_CNT_1           = 0x0814, // Port 1 Mac Receive 255 to 511 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_512_TO_1023_CNT_1          = 0x0815, // Port 1 Mac Receive 512 to 1023 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_1024_TO_MAX_CNT_1          = 0x0816, // Port 1 Mac Receive 1024 to Max Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_OVRSZE_CNT_1               = 0x0817, // Port 1 MAC Receive Oversize Count Register
    LAN9354_REG_SF_IA_MAC_RX_PKTOK_CNT_1                = 0x0818, // Port 1 MAC Receive OK Count Register
    LAN9354_REG_SF_IA_MAC_RX_CRCERR_CNT_1               = 0x0819, // Port 1 MAC Receive CRC Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_MULCST_CNT_1               = 0x081A, // Port 1 MAC Receive Multicast Count Register
    LAN9354_REG_SF_IA_MAC_RX_BRDCST_CNT_1               = 0x081B, // Port 1 MAC Receive Broadcast Count Register
    LAN9354_REG_SF_IA_MAC_RX_PAUSE_CNT_1                = 0x081C, // Port 1 MAC Receive Pause Count Register
    LAN9354_REG_SF_IA_MAC_RX_FRAG_CNT_1                 = 0x081D, // Port 1 MAC Receive Fragment Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_JABB_CNT_1                 = 0x081E, // Port 1 MAC Receive Jabber Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_ALIGN_CNT_1                = 0x081F, // Port 1 MAC Receive Alignment Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_PKTLEN_CNT_1               = 0x0820, // Port 1 MAC Receive Packet Length Count Register
    LAN9354_REG_SF_IA_MAC_RX_GOODPKTLEN_CNT_1           = 0x0821, // Port 1 MAC Receive Good Packet Length Count Register
    LAN9354_REG_SF_IA_MAC_RX_SYMBL_CNT_1                = 0x0822, // Port 1 MAC Receive Symbol Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_CTLFRM_CNT_1               = 0x0823, // Port 1 MAC Receive Control Frame Count Register
    LAN9354_REG_SF_IA_MAC_TX_CFG_1                      = 0x0840, // Port 1 MAC Transmit Configuration Register
    LAN9354_REG_SF_IA_MAC_TX_FC_SETTINGS_1              = 0x0841, // Port 1 MAC Transmit Flow Control Settings Register
    LAN9354_REG_SF_IA_MAC_TX_DEFER_CNT_1                = 0x0851, // Port 1 MAC Transmit Deferred Count Register
    LAN9354_REG_SF_IA_MAC_TX_PAUSE_CNT_1                = 0x0852, // Port 1 MAC Transmit Pause Count Register
    LAN9354_REG_SF_IA_MAC_TX_PKTOK_CNT_1                = 0x0853, // Port 1 MAC Transmit OK Count Register
    LAN9354_REG_SF_IA_MAC_TX_64_CNT_1                   = 0x0854, // Port 1 MAC Transmit 64 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_65_TO_127_CNT_1            = 0x0855, // Port 1 Mac Transmit 65 to 127 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_128_TO_255_CNT_1           = 0x0856, // Port 1 Mac Transmit 128 to 255 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_256_TO_511_CNT_1           = 0x0857, // Port 1 Mac Transmit 255 to 511 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_512_TO_1023_CNT_1          = 0x0858, // Port 1 Mac Transmit 512 to 1023 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_1024_TO_MAX_CNT_1          = 0x0859, // Port 1 Mac Transmit 1024 to Max Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_UNDSIZE_CNT_1              = 0x085A, // Port 1 MAC Transmit Undersize Count Register
    LAN9354_REG_SF_IA_MAC_TX_PKTLEN_CNT_1               = 0x085C, // Port 1 MAC Transmit Packet Length Count Register
    LAN9354_REG_SF_IA_MAC_TX_BRDCST_CNT_1               = 0x085D, // Port 1 MAC Transmit Broadcast Count Register
    LAN9354_REG_SF_IA_MAC_TX_MULCST_CNT_1               = 0x085E, // Port 1 MAC Transmit Multicast Count Register
    LAN9354_REG_SF_IA_MAC_TX_LATECOL_CNT_1              = 0x085F, // Port 1 MAC Transmit Late Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_EXCOL_CNT_1                = 0x0860, // Port 1 MAC Transmit Excessive Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_SINGLECOL_CNT_1            = 0x0861, // Port 1 MAC Transmit Single Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_MULTICOL_CNT_1             = 0x0862, // Port 1 MAC Transmit Multiple Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_TOTALCOL_CNT_1             = 0x0863, // Port 1 MAC Transmit Total Collision Count Register
    LAN9354_REG_SF_IA_MAC_IMR_1                         = 0x0880, // Port 1 MAC Interrupt Mask Register
    LAN9354_REG_SF_IA_MAC_IPR_1                         = 0x0881, // Port 1 MAC Interrupt Pending REgister

    // Switch Port 2 CSRs (x=2)
    LAN9354_REG_SF_IA_MAC_VER_ID_2                      = 0x0C00, // Port 2 MAC Version ID Register
    LAN9354_REG_SF_IA_MAC_RX_CFG_2                      = 0x0c01, // Port 2 MAC Receive Configuration Register
    LAN9354_REG_SF_IA_MAC_RX_UNDSZE_CNT_2               = 0x0C10, // Port 2 MAC Receive Undersize Count Register
    LAN9354_REG_SF_IA_MAC_RX_64_CNT_2                   = 0x0C11, // Port 2 MAC REceive 64 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_65_TO_227_CNT_2            = 0x0C12, // Port 2 Mac Receive 65 to 127 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_228_TO_255_CNT_2           = 0x0C13, // Port 2 Mac Receive 128 to 255 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_256_TO_511_CNT_2           = 0x0C14, // Port 2 Mac Receive 255 to 511 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_512_TO_2023_CNT_2          = 0x0C15, // Port 2 Mac Receive 512 to 1023 Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_2024_TO_MAX_CNT_2          = 0x0C16, // Port 2 Mac Receive 1024 to Max Byte Count Register
    LAN9354_REG_SF_IA_MAC_RX_OVRSZE_CNT_2               = 0x0C17, // Port 2 MAC Receive Oversize Count Register
    LAN9354_REG_SF_IA_MAC_RX_PKTOK_CNT_2                = 0x0C18, // Port 2 MAC Receive OK Count Register
    LAN9354_REG_SF_IA_MAC_RX_CRCERR_CNT_2               = 0x0C19, // Port 2 MAC Receive CRC Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_MULCST_CNT_2               = 0x0C1A, // Port 2 MAC Receive Multicast Count Register
    LAN9354_REG_SF_IA_MAC_RX_BRDCST_CNT_2               = 0x0C1B, // Port 2 MAC Receive Broadcast Count Register
    LAN9354_REG_SF_IA_MAC_RX_PAUSE_CNT_2                = 0x0C1C, // Port 2 MAC Receive Pause Count Register
    LAN9354_REG_SF_IA_MAC_RX_FRAG_CNT_2                 = 0x0C1D, // Port 2 MAC Receive Fragment Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_JABB_CNT_2                 = 0x0C1E, // Port 2 MAC Receive Jabber Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_ALIGN_CNT_2                = 0x0C1F, // Port 2 MAC Receive Alignment Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_PKTLEN_CNT_2               = 0x0C20, // Port 2 MAC Receive Packet Length Count Register
    LAN9354_REG_SF_IA_MAC_RX_GOODPKTLEN_CNT_2           = 0x0C21, // Port 2 MAC Receive Good Packet Length Count Register
    LAN9354_REG_SF_IA_MAC_RX_SYMBL_CNT_2                = 0x0C22, // Port 2 MAC Receive Symbol Error Count Register
    LAN9354_REG_SF_IA_MAC_RX_CTLFRM_CNT_2               = 0x0C23, // Port 2 MAC Receive Control Frame Count Register
    LAN9354_REG_SF_IA_MAC_TX_CFG_2                      = 0x0C40, // Port 2 MAC Transmit Configuration Register
    LAN9354_REG_SF_IA_MAC_TX_FC_SETTINGS_2              = 0x0C41, // Port 2 MAC Transmit Flow Control Settings Register
    LAN9354_REG_SF_IA_MAC_TX_DEFER_CNT_2                = 0x0C51, // Port 2 MAC Transmit Deferred Count Register
    LAN9354_REG_SF_IA_MAC_TX_PAUSE_CNT_2                = 0x0C52, // Port 2 MAC Transmit Pause Count Register
    LAN9354_REG_SF_IA_MAC_TX_PKTOK_CNT_2                = 0x0C53, // Port 2 MAC Transmit OK Count Register
    LAN9354_REG_SF_IA_MAC_TX_64_CNT_2                   = 0x0C54, // Port 2 MAC Transmit 64 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_65_TO_227_CNT_2            = 0x0C55, // Port 2 Mac Transmit 65 to 127 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_228_TO_255_CNT_2           = 0x0C56, // Port 2 Mac Transmit 128 to 255 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_256_TO_511_CNT_2           = 0x0C57, // Port 2 Mac Transmit 255 to 511 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_512_TO_2023_CNT_2          = 0x0C58, // Port 2 Mac Transmit 512 to 1023 Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_2024_TO_MAX_CNT_2          = 0x0C59, // Port 2 Mac Transmit 1024 to Max Byte Count Register
    LAN9354_REG_SF_IA_MAC_TX_UNDSIZE_CNT_2              = 0x0C5A, // Port 2 MAC Transmit Undersize Count Register
    LAN9354_REG_SF_IA_MAC_TX_PKTLEN_CNT_2               = 0x0C5C, // Port 2 MAC Transmit Packet Length Count Register
    LAN9354_REG_SF_IA_MAC_TX_BRDCST_CNT_2               = 0x0C5D, // Port 2 MAC Transmit Broadcast Count Register
    LAN9354_REG_SF_IA_MAC_TX_MULCST_CNT_2               = 0x0C5E, // Port 2 MAC Transmit Multicast Count Register
    LAN9354_REG_SF_IA_MAC_TX_LATECOL_CNT_2              = 0x0C5F, // Port 2 MAC Transmit Late Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_EXCOL_CNT_2                = 0x0C60, // Port 2 MAC Transmit Excessive Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_SINGLECOL_CNT_2            = 0x0C61, // Port 2 MAC Transmit Single Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_MULTICOL_CNT_2             = 0x0C62, // Port 2 MAC Transmit Multiple Collision Count Register
    LAN9354_REG_SF_IA_MAC_TX_TOTALCOL_CNT_2             = 0x0C63, // Port 2 MAC Transmit Total Collision Count Register
    LAN9354_REG_SF_IA_MAC_IMR_2                         = 0x0C80, // Port 2 MAC Interrupt Mask Register
    LAN9354_REG_SF_IA_MAC_IPR_2                         = 0x0C81, // Port 2 MAC Interrupt Pending Register

    // Switch Engine CSRs
    LAN9354_REG_SF_IA_SWE_ALR_CMD                       = 0x1800, // Switch Engine ALR Command REgister
    LAN9354_REG_SF_IA_SWE_ALR_WR_DAT_0                  = 0x1801, // Switch Engine ALR Write Data 0 Register
    LAN9354_REG_SF_IASWE_ALR_WR_DAT_1                   = 0x1802, // Switch Engine ALR Write Data 1 Register
    LAN9354_REG_SF_IA_SWE_ALR_RD_DAT_0                  = 0x1805, // Switch Engine ALR Read Data 0 Register
    LAN9354_REG_SF_IA_SWE_ALR_RD_DAT_1                  = 0x1806, // Switch Engine ALR Read Data 1 Register
    LAN9354_REG_SF_IA_SWE_ALR_CMD_STS                   = 0x1808, // Switch Engine ALR Command Status Register
    LAN9354_REG_SF_IA_SWE_ALR_CFG                       = 0x1809, // Switch Engine ALR Configuration Register
    LAN9354_REG_SF_IA_SWE_VLAN_CMD                      = 0x180B, // Switch Engine VLAN Command Register
    LAN9354_REG_SF_IA_SWE_VLAN_WR_DATA                  = 0x180C, // Switch Engine VLAN Write Register
    LAN9354_REG_SF_IA_SWE_VLAN_RO_DATA                  = 0x180E, // Switch Engine VLAN Read Data Register
    LAN9354_REG_SF_IA_SWE_VLAN_CMD_STS                  = 0x1810, // Switch Engine VLAN Command Status Register
    LAN9354_REG_SF_IA_SWE_DIFFSERV_TBL_CMD              = 0x1811, // Switch Engine DIFSERV Table Command Register
    LAN9354_REG_SF_IA_SWE_DIFFSERV_TBL_WR_DATA          = 0x1812, // Switch Engine DIFSERV Table Write Data
    LAN9354_REG_SF_IA_SWE_DIFFSERV_TBL_RO_DATA          = 0x1813, // Switch Engine DIFFSERV Table Read Data Register
    LAN9354_REG_SF_IA_SWE_DIFFSERV_TBL_CMD_STS          = 0x1814, // Switch Engine DIFFSERV Table Command Status Register
    LAN9354_REG_SF_IA_SWE_GLB_INGRESS_CFG               = 0x1840, // Switch Engine Global Ingress Configuration Register
    LAN9354_REG_SF_IA_SWE_PORT_INGRESS_CFG              = 0x1841, // Switch Engine Port Ingress Configuration Register
    LAN9354_REG_SF_IA_SWE_ADMT_ONLY_VLAN                = 0x1842, // Switch Engine Admit Only VLAN Register
    LAN9354_REG_SF_IA_SWE_PORT_STATE                    = 0x1843, // Switch Engine Port State Register
    LAN9354_REG_SF_IA_SWE_PRI_TO_QUE                    = 0x1845, // Switch Engine PRiority to Queue Register
    LAN9354_REG_SF_IA_SWE_PORT_MIRROR                   = 0x1846, // Switch Engine Port Mirroring Register
    LAN9354_REG_SF_IA_SWE_INGRESS_PORT_TYPE             = 0x1847, // Switch Engine Ingress Port Type Register
    LAN9354_REG_SF_IA_SWE_BCST_THROT                    = 0x1848, // Switch Engine Broadcast Throttling Register
    LAN9354_REG_SF_IA_SWE_ADMT_N_MEMBER                 = 0x1849, // Swtich Engine Admit Non Member Register
    LAN9354_REG_SF_IA_SWE_INGRESS_RATE_CFG              = 0x184A, // Switch Engine Ingress Rate Configuration Register
    LAN9354_REG_SF_IA_SWE_INGRESS_RATE_CMD              = 0x184B, // Switch Engine Ingress Rate Command Register
    LAN9354_REG_SF_IA_SWE_INGRESS_RATE_CMD_STS          = 0x184C, // Switch Engine Ingress Rate Command Status Register
    LAN9354_REG_SF_IA_SWE_INGRESS_RATE_WR_DATA          = 0x184D, // Switch Engine Ingress Rate Write Data Register
    LAN9354_REG_SF_IA_SWE_INGRESS_RATE_RD_DATA          = 0x184E, // Switch Engine Ingress Rate Read Data Register
    LAN9354_REG_SF_IA_SWE_FILTERED_CNT_0                = 0x1850, // Switch Engine Port 0 Ingress Filtered Count Register
    LAN9354_REG_SF_IA_SWE_FILTERED_CNT_1                = 0x1851, // Switch Engine Port 1 Ingress Filtered Count Register
    LAN9354_REG_SF_IA_SWE_FILTERED_CNT_2                = 0x1852, // Switch Engine Port 2 Ingress Filtered Count Register
    LAN9354_REG_SF_IA_SWE_INGRESS_REGEN_TBL_0           = 0x1855, // Switch Engine Port 0 Learn Discard Count Register
    LAN9354_REG_SF_IA_SWE_INGRESS_REGEN_TBL_1           = 0x1856, // Switch Engine Port 1 Learn Discard Count Register
    LAN9354_REG_SF_IA_SWE_INGRESS_REGEN_TBL_2           = 0x1857, // Switch Engine Port 2 Learn Discard Count Register
    LAN9354_REG_SF_IA_SWE_LRN_DISCRD_CNT_0              = 0x1858, // Switch Engine Port 0 Learn Discard Count Register
    LAN9354_REG_SF_IA_SWE_LRN_DISCRD_CNT_1              = 0x1859, // Switch Engine Port 1 Learn Discard Count Register
    LAN9354_REG_SF_IA_SWE_LRN_DISCRD_CNT_2              = 0x185A, // Switch Engine Port 2 Learn Discard Count Register
    LAN9354_REG_SF_IA_SWE_IMR                           = 0x1880, // Switch Engine Interrupt Mask Register
    LAN9354_REG_SF_IA_SWE_IPR                           = 0x1881, // Switch Engine Interrupt PEnding Register

    // Buffer Manager (BM) CSRs
    LAN9354_REG_SF_IA_BM_CFG                            = 0x1C00, // Buffer Manager Configuration Register
    LAN9354_REG_SF_IA_BM_DROP_LVL                       = 0x1C01, // Buffer Manager Drop Level Register
    LAN9354_REG_SF_IA_BM_FC_PAUSE_LVL                   = 0x1C02, // Buffer Manager Flow Control Pause Level Register
    LAN9354_REG_SF_IA_BM_FC_RESUME_LVL                  = 0x1C03, // Buffer Manager Flow Control Resume Level Register
    LAN9354_REG_SF_IA_BM_BCST_LVL                       = 0x1C04, // Buffer Manager Broadcast Buffer Level Register
    LAN9354_REG_SF_IA_BM_DRP_CNT_SRC_0                  = 0x1C05, // Buffer ManagerPort 0 Drop Count Register
    LAN9354_REG_SF_IA_BM_DRP_CNT_SRC_1                  = 0x1C06, // Buffer ManagerPort 1 Drop Count Register
    LAN9354_REG_SF_IA_BM_DRP_CNT_SRC_2                  = 0x1C07, // Buffer ManagerPort 2 Drop Count Register
    LAN9354_REG_SF_IA_BM_RST_STS                        = 0x1C08, // Buffer Manager Reset Status Register
    LAN9354_REG_SF_IA_BM_RNDM_DSCRD_TBL_CMD             = 0x1C09, // Buffer Manager Random Discard Table Command Register
    LAN9354_REG_SF_IA_BM_RNDM_DSCRD_TBL_WDATA           = 0x1C0A, // Buffer Manager Random Discard Table Write Data Register
    LAN9354_REG_SF_IA_BM_RNDM_DSCRD_TABLE_RDATA         = 0x1C0B, // Buffer Manager Random Discard Table Read Data Register
    LAN9354_REG_SF_IA_BM_EGRSS_PORT_TYPE                = 0x1C0C, // Buffer Manager Egress Port Type Register
    LAN9354_REG_SF_IA_BM_EGRSS_RATE_00_01               = 0x1C0D, // Buffer Manager Port 0 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_REG_SF_IA_BM_EGRSS_RATE_02_03               = 0x1C0E, // Buffer Manager Port 0 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_REG_SF_IA_BM_EGRSS_RATE_10_11               = 0x1C0F, // Buffer Manager Port 1 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_REG_SF_IA_BM_EGRSS_RATE_12_13               = 0x1C10, // Buffer Manager Port 1 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_REG_SF_IA_BM_EGRSS_RATE_20_21               = 0x1C11, // Buffer Manager Port 2 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_REG_SF_IA_BM_EGRSS_RATE_22_23               = 0x1C12, // Buffer Manager Port 2 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_REG_SF_IA_BM_VLAN_0                         = 0x1C13, // Buffer Manager Port 0 Default VLAN ID and PRiority Register
    LAN9354_REG_SF_IA_BM_VLAN_1                         = 0x1C14, // Buffer Manager Port 1 Default VLAN ID and PRiority Register
    LAN9354_REG_SF_IA_BM_VLAN_2                         = 0x1C15, // Buffer Manager Port 2 Default VLAN ID and PRiority Register
    LAN9354_REG_SF_IA_BM_RATE_DRP_CNT_SRC_0             = 0x1C16, // Buffer Manager Port 0 Ingress Rate Drop Count Register
    LAN9354_REG_SF_IA_BM_RATE_DRP_CNT_SRC_1             = 0x1C17, // Buffer Manager Port 0 Ingress Rate Drop Count Register
    LAN9354_REG_SF_IA_BM_RATE_DRP_CNT_SRC_2             = 0x1C18, // Buffer Manager Port 0 Ingress Rate Drop Count Register
    LAN9354_REG_SF_IA_BM_IMR                            = 0x1C20, // Buffer Manager Interrupt Mask Register
    LAN9354_REG_SF_IA_BM_IPR                            = 0x1C21, // Buffer Manger Interrupt Pending Register
}LAN9354_INDIRECT_SW_CTL_STAT_REG;


#endif  // _LAN_9354_H_


