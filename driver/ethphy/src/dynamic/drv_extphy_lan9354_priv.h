/*******************************************************************************
  LAN9354 private definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_lan9354_priv.h

  Summary:
    LAN9354 private definitions

  Description:
    This file provides the LAN9354 private definitions.
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

#ifndef H_LAN_9354_PRIV_H_

#define H_LAN_9354_PRIV_H_

#include <stdint.h>

// VIRTUAL PHY MII SERIALLY ADDRESSABLE REGISTER INDEX
// std IEEE 802.3

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
    PHY_REG_ANEXP       = 6,
     */
    // specific vendor registers: 16-31
    // Note: reg 27 exists for the Port A, B only!
    LAN9354_REG_CTRL_STAT_IND = 27,
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
    struct __attribute__((packed))
    {
        unsigned                        : 2;
        unsigned SPEED_DUPLEX           : 3;    // a LAN9354_SPEED_DUPLEX value
        unsigned RMII_CLK_STRENGTH      : 1;
        unsigned RMII_CLK_DIR           : 1;
        unsigned SWITCH_COLL_TEST       : 1;
        unsigned MODE_10                : 2;
        unsigned                        : 4;
        unsigned SWITCH_LOOPBACK        : 1;
        unsigned MODE_2                 : 1;
    };
}LAN9354_REG_CTRL_STAT_bits_t;

// register 27 layout
typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned            : 4;
        unsigned XPOL       : 1;
        unsigned FEFI_EN    : 1;
        unsigned            : 5;
        unsigned SQEOFF     : 1;
        unsigned            : 1;
        unsigned AMDIXSTATE : 1;
        unsigned AMDIXEN    : 1;
        unsigned AMDIXCTRL  : 1;
    };
}LAN9354_REG_CTRL_STAT_IND_bits_t;

#define LAN9354_AMDIXCTRL_MASK  0x8000U
#define LAN9354_AMDIXEN_MASK    0x4000U
#define LAN9354_AMDIXSTATE_MASK 0x2000U

#define LAN9354_AMDIX_BITS_MASK 0xe000U

// register 0x74 HW_CFG layout
typedef union
{
    uint32_t    w;
    struct __attribute__((packed))
    {
        unsigned                    : 25;
        unsigned AMDIX_EN_STRAP_A   : 1;
        unsigned AMDIX_EN_STRAP_B   : 1;
        unsigned READY              : 1;
        unsigned                    : 4;
    };
}LAN9354_REG_HW_CFG_bits_t;

// LAN9354 System Control and Status Registers
// The structure and definition of each registers are in the LAN9354 DS
typedef enum
{
    // General registers
    LAN9354_ID_REV                              = 0x050, // Chip ID and Revision Register
    LAN9354_IRQ_CFG                             = 0x054, // Interrupt Configuration Register
    LAN9354_INT_STS                             = 0x058, // Interrupt Status Register
    LAN9354_INT_EN                              = 0x05C, // Interrupt Enable Register
    LAN9354_BYTE_TEST                           = 0x064, // Byte Order Test Register
    LAN9354_HW_CFG                              = 0x074, // Hardware Configuration Register
    LAN9354_GPT_CFG                             = 0x08C, // General Purpose Timer Configuration Register
    LAN9354_GPT_CNG                             = 0x090, // General Purpose Timer Count Register
    LAN9354_FREE_RUN                            = 0x09C, // Free Running Counter Register
    LAN9354_PMI_DATA                            = 0x0A4, // PHY Management Interface Data Register
    LAN9354_PMI_ACCESS                          = 0x0A8, // PHY Management Interface Access Register
                                //
    // 1588 registers
    LAN9354_1588_CMD_CTL                        = 0x100,
    LAN9354_1588_GENERAL_CONFIG                 = 0x104,
    LAN9354_1588_INT_STS                        = 0x108,
    LAN9354_1588_INT_EN                         = 0x10C,
    LAN9354_1588_CLK_SEC                        = 0x110,
    LAN9354_1588_CLK_NS                         = 0x114,
    LAN9354_1588_CLK_SUBNS                      = 0x118,
    LAN9354_1588_CLK_RATE_ADJ                   = 0x11C,
    LAN9354_1588_CLK_T_RATE_ADJ                 = 0x120,
    LAN9354_1588_CLK_T_RATE_DUR                 = 0x124,
    LAN9354_1588_CLK_STEP_ADJ                   = 0x128,
    LAN9354_1588_CLK_TGT_SEC_A                  = 0x12C,
    LAN9354_1588_CLK_TGT_NS_A                   = 0x130,
    LAN9354_1588_CLK_TGT_RLD_SEC_A              = 0x134, 
    LAN9354_1588_CLK_TGT_RLD_NS_A               = 0x139,
    LAN9354_1588_CLK_TGT_SEC_B                  = 0x13C,
    LAN9354_1588_CLK_TGT_NS_B                   = 0x140,
    LAN9354_1588_CLK_TGT_RLD_SEC_B              = 0x144, 
    LAN9354_1588_CLK_TGT_RLD_NS_B               = 0x148,
    LAN9354_1588_USER_MAC_HI                    = 0x14C, 
    LAN9354_1588_USER_MAC_LO                    = 0x150, 
    LAN9354_1588_BANK_PORT_GPIO_SEL             = 0x154,
    LAN9354_1588_LATENCY_x                      = 0x158, 
    LAN9354_1588_RX_PARSE_CONFIG_x              = 0x158,
    LAN9354_1588_TX_PARSE_CONFIG_x              = 0x158,
    LAN9354_1588_ASYM_PEERDLY_x                 = 0x15C,
    LAN9354_1588_RX_TIMESTAMP_CONFIG_x          = 0x15C,
    LAN9354_1588_TX_TIMESTAMP_CONFIG_x          = 0x15C,
    LAN9354_1588_GPIO_CAP_CONFIG                = 0x15C,
    LAN9354_1588_CAP_INFO_x                     = 0x160, 
    LAN9354_1588_RX_TS_INSERT_CONFIG_x          = 0x160,
    LAN9354_1588_RX_CF_MOD_x                    = 0x164, 
    LAN9354_1588_TX_MOD_x                       = 0x164, 
    LAN9354_1588_RX_FILTER_CONFIG_x             = 0x168,
    LAN9354_1588_TX_MOD2_x                      = 0x168, 
    LAN9354_1588_RX_ING_SEC_x                   = 0x16C,
    LAN9354_1588_TX_EGRESS_SEC_x                = 0x16C,
    LAN9354_1588_GPIO_RE_CLK_SEC_CAP_x          = 0x16C,
    LAN9354_1588_RX_ING_NS_x                    = 0x170,
    LAN9354_1588_TX_EGRESS_NS_x                 = 0x170,
    LAN9354_1588_GPIO_RE_CLK_NS_CAP_x           = 0x170,
    LAN9354_1588_RX_MSG_HEADER_x                = 0x174,
    LAN9354_1588_TX_MSG_HEADER_x                = 0x174,
    LAN9354_1588_RX_PDREQ_SEC_x                 = 0x178,
    LAN9354_1588_TX_DREQ_SEC_x                  = 0x178, 
    LAN9354_1588_GPIO_FE_CLK_SEC_CAP_x          = 0x178,
    LAN9354_1588_RX_PDREQ_NS_x                  = 0x17C, 
    LAN9354_1588_TX_DREQ_NS_x                   = 0x17C, 
    LAN9354_1588_GPIO_FE_CLK_NS_CAP_x           = 0x17C,
    LAN9354_1588_RX_PDREQ_CF_HI_x               = 0x180,
    LAN9354_1588_TX_ONE_STEP_SYNC_SEC           = 0x180,
    LAN9354_1588_RX_PDREQ_CF_LOW_x              = 0x184,
    LAN9354_1588_RX_CHKSUM_DROPPED_Cx           = 0x188,
    LAN9354_1588_RX_FILTC_x                     = 0x18C,

    // Switch Registers
    LAN9354_MANUAL_FC_1                         = 0x1A0, // Port 1 Manual Flow Control Register
    LAN9354_MANUAL_FC_2                         = 0x1A4, // Port 2 Manual Flow Control Register
    LAN9354_MANUAL_FC_0                         = 0x1A8, // Port 0 Manual Flow Control Register
    LAN9354_SWITCH_CSR_DATA                     = 0x1AC, // Switch Fabric CSR Interface Data Register
    LAN9354_SWITCH_CSR_CMD                      = 0x1B0, // Switch Fabric CSR Interface Command Register

    // EEPROM/LED Registers
    LAN9354_E2P_CMD                             = 0x1B4, // EEPROM Command Register
    LAN9354_E2P_DATA                            = 0x1B8, // EEPROM Data Register
    LAN9354_LED_CFG                             = 0x1BC, // LED Configuration Register

    // Virtual PHY Registers
    LAN9354_VPHY_BASIC_CTRL                     = 0x1C0, // Virtual PHY Basic Control Register
    LAN9354_VPHY_BASIC_STATUS                   = 0x1C4, // Virtual PHY Basic Status Register
    LAN9354_VPHY_ID_MSB                         = 0x1C8, // Virtual PHY Identification MSB Register
    LAN9354_VPHY_ID_LSB                         = 0x1CC, // Virtual PHY Identification LSB Register
    LAN9354_VPHY_AN_ADV                         = 0x1D0, // Virtual PHY Auto-Negotiation Advertisement Register
    LAN9354_VPHY_AN_LP_BASE_ABILITY             = 0x1D4, // Virtual PHY Auto-Negotiation Link Partner Base Page Ability Register
    LAN9354_VPHY_AN_EXP                         = 0x1D8, // Virtual PHY Auto-Negotiation Expansion Register
    LAN9354_VPHY_SPECIAL_CONTROL_STATUS         = 0x1DC, // Virtual PHY Special Control/Status Register

    // GPIO Registers
    LAN9354_GPIO_CFG                            = 0x1E0, // General Purpose I/O Configuration Register
    LAN9354_GPIO_DATA_DIR                       = 0x1E4, // General Purpose I/O Data & Direction Register
    LAN9354_GPIP_INT_STS_EN                     = 0x1E8, // General Purpose I/O Interupt Status and Enable Register
                                     
    // Switch Fabric MAC Address Registers
    LAN9354_SWITCH_MAC_ADDRH                    = 0x1F0, // Switch MAC Address High Register
    LAN9354_SWITCH_MAC_ADDRL                    = 0x1F4, // Switch MAC Address Low Register

    // Reset Register
    LAN9354_RESET_CTL                           = 0x1F8, // Reset Control Register

    // Switch Fabric CSR Interface Direct Data Registers
    LAN9354_SW_RESET                            = 0x200, // Switch Reset Register
    LAN9354_SW_IMR                              = 0x204, // Switch GlobalInterrupt Mask Register
    LAN9354_SF_MAC_RX_CFG_0                     = 0x208, // Port 0 MAC Receive Configuration Register
    LAN9354_SF_MAC_TX_CFG_0                     = 0x20C, // Port 0 MAC Transmit Configuration Register
    LAN9354_SF_MAC_TX_FC_SET0                   = 0x210, // Port 0 MAC Transmit Flow Control Settings Register
    LAN9354_SF_MAC_IMR_0                        = 0x214, // Port 0 MAC Interrupt Mask Register
    LAN9354_SF_MAC_RX_CFG_1                     = 0x218, // Port 1 MAC Receive Configuration Register
    LAN9354_SF_MAC_TX_CFG_1                     = 0x21C, // Port 1 MAC Transmit Configuration Register
    LAN9354_SF_MAC_TX_FC_SET1                   = 0x220, // Port 1 MAC Transmit Flow Control Settings Register
    LAN9354_SF_MAC_IMR_1                        = 0x224, // Port 1 MAC Interrupt Mask Register
    LAN9354_SF_MAC_RX_CFG_2                     = 0x228, // Port 2 MAC Receive Configuration Register
    LAN9354_SF_MAC_TX_CFG_2                     = 0x22C, // Port 2 MAC Transmit Configuration Register
    LAN9354_SF_MAC_TX_FC_SET2                   = 0x230, // Port 2 MAC Transmit Flow Control Settings Register
    LAN9354_SF_MAC_IMR_2                        = 0x234, // Port 2 MAC Interrupt Mask Register
    LAN9354_SF_SWE_ALR_CMD                      = 0x238, // Switch Engine ALR Command REgister
    LAN9354_SF_SWE_ALR_WR_D0                    = 0x23C, // Switch Engine ALR Write Data 0 Register
    LAN9354_SF_SWE_ALR_WR_D1                    = 0x240, // Switch Engine ALR Write Data 1 Register
    LAN9354_SF_SWE_ALR_CFG                      = 0x244, // Switch Engine ALR Configuration Register
    LAN9354_SF_SWE_VLAN_CMD                     = 0x248, // Switch Engine VLAN Command Register
    LAN9354_SF_SWE_VLAN_WR_DATA                 = 0x24C, // Switch Engine VLAN Write Register
    LAN9354_SF_SWE_DSRV_TCMD                    = 0x250, // Switch Engine DIFSERV Table Command Register
    LAN9354_SF_SWE_DSRV_TWRDATA                 = 0x254, // Switch Engine DIFSERV Table Write Data
    LAN9354_SF_SWE_GLB_ING_CFG                  = 0x258, // Switch Engine Global Ingress Configuration Register
    LAN9354_SF_SWE_PORT_ING_CFG                 = 0x25C, // Switch Engine Port Ingress Configuration Register
    LAN9354_SF_SWE_ADMT_ONLY_VLAN               = 0x260, // Switch Engine Admit Only VLAN Register
    LAN9354_SF_SWE_PORT_STATE                   = 0x264, // Switch Engine Port State Register
    LAN9354_SF_SWE_PRI_QUE                      = 0x268, // Switch Engine PRiority to Queue Register
    LAN9354_SF_SWE_PORT_MIRROR                  = 0x26C, // Switch Engine Port Mirroring Register
    LAN9354_SF_SWE_ING_PORT_TYPE                = 0x270, // Switch Engine Ingress Port Type Register
    LAN9354_SF_SWE_BCST_THROT                   = 0x274, // Switch Engine Broadcast Throttling Register
    LAN9354_SF_SWE_ADMT_N_MEMBER                = 0x278, // Swtich Engine Admit Non Member Register
    LAN9354_SF_SWE_ING_RCFG                     = 0x27C, // Switch Engine Ingress Rate Configuration Register
    LAN9354_SF_SWE_ING_RCMD                     = 0x280, // Switch Engine Ingress Rate Command Register
    LAN9354_SF_SWE_ING_RWR_DATA                 = 0x284, // Switch Engine Ingress Rate Write Data Register
    LAN9354_SF_SWE_ING_RGN_T0                   = 0x288, // Switch Engine Port 0 Learn Discard Count Register
    LAN9354_SF_SWE_ING_RGN_T1                   = 0x28C, // Switch Engine Port 1 Learn Discard Count Register
    LAN9354_SF_SWE_ING_RGN_T2                   = 0x290, // Switch Engine Port 2 Learn Discard Count Register
    LAN9354_SF_SWE_IMR                          = 0x294, // Switch Engine Interrupt Mask Register
    LAN9354_SF_BM_CFG                           = 0x298, // Buffer Manager Configuration Register
    LAN9354_SF_BM_DROP_LVL                      = 0x29C, // Buffer Manager Drop Level Register
    LAN9354_SF_BM_FC_PAUSE_LVL                  = 0x2A0, // Buffer Manager Flow Control Pause Level Register
    LAN9354_SF_BM_FC_RESUME_LVL                 = 0x2A4, // Buffer Manager Flow Control Resume Level Register
    LAN9354_SF_BM_BCST_LVL                      = 0x2A8, // Buffer Manager Broadcast Buffer Level Register
    LAN9354_SF_BM_RDCRD_TCMD                    = 0x2AC, // Buffer Manager Random Discard Table Command Register
    LAN9354_SF_BM_RDCRD_TWDATA                  = 0x2B0, // Buffer Manager Random Discard Table Write Data Register
    LAN9354_SF_BM_EGRSS_PORT_TYPE               = 0x2B4, // Buffer Manager Egress Port Type Register
    LAN9354_SF_BM_ERATE_00_01                   = 0x2B8, // Buffer Manager Port 0 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_SF_BM_ERATE_02_03                   = 0x2BC, // Buffer Manager Port 0 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_SF_BM_ERATE_10_11                   = 0x2C0, // Buffer Manager Port 1 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_SF_BM_ERATE_12_13                   = 0x2C4, // Buffer Manager Port 1 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_SF_BM_ERATE_20_21                   = 0x2C8, // Buffer Manager Port 2 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_SF_BM_ERATE_22_23                   = 0x2CC, // Buffer Manager Port 2 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_SF_BM_VLAN_0                        = 0x2D0, // Buffer Manager Port 0 Default VLAN ID and PRiority Register
    LAN9354_SF_BM_VLAN_1                        = 0x2D4, // Buffer Manager Port 1 Default VLAN ID and PRiority Register
    LAN9354_SF_BM_VLAN_2                        = 0x2D8, // Buffer Manager Port 2 Default VLAN ID and PRiority Register
    LAN9354_SF_BM_IMR                           = 0x2DC, // Buffer Manager Interrupt Mask Register
}LAN9354_CSR_REGISTERS;



// INDIRECTLY ACCESSIBLE SWITCH CONTROL AND STATUS REGISTERS
// The structure and definition of each registers are in the LAN9354 DS
typedef enum
{
    // General Switch CSRs
    LAN9354_SF_IA_SW_DEV_ID                         = 0x0000, // Switch Device ID Register
    LAN9354_SF_IA_SW_RESET                          = 0x0001, // Switch Reset Register
    LAN9354_SF_IA_SW_IMR                            = 0x0004, // Switch Global Interrupt Mask Register
    LAN9354_SF_IA_SW_IPR                            = 0x0005, // Switch Global Interrupt Pending Register
                                   
    // Switch Port 0 CSRs (x=0)
    LAN9354_SF_IA_MAC_VER_ID_0                      = 0x0400, // Port 0 MAC Version ID Register
    LAN9354_SF_IA_MAC_RX_CFG_0                      = 0x0401, // Port 0 MAC Receive Configuration Register
    LAN9354_SF_IA_MAC_RX_USZ_C0                     = 0x0410, // Port 0 MAC Receive Undersize Count Register
    LAN9354_SF_IA_MAC_RX_64_C0                      = 0x0411, // Port 0 MAC REceive 64 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S0_C0                      = 0x0412, // Port 0 Mac Receive 65 to 127 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S1_C0                      = 0x0413, // Port 0 Mac Receive 128 to 255 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S2_C0                      = 0x0414, // Port 0 Mac Receive 255 to 511 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S3_C0                      = 0x0415, // Port 0 Mac Receive 512 to 1023 Byte Count Register
    LAN9354_SF_IA_MAC_RX_1K_MAX_C0                  = 0x0416, // Port 0 Mac Receive 1024 to Max Byte Count Register
    LAN9354_SF_IA_MAC_RX_OVRSZE_C0                  = 0x0417, // Port 0 MAC Receive Oversize Count Register
    LAN9354_SF_IA_MAC_RX_PKTOK_C0                   = 0x0418, // Port 0 MAC Receive OK Count Register
    LAN9354_SF_IA_MAC_RX_CRCERR_C0                  = 0x0419, // Port 0 MAC Receive CRC Error Count Register
    LAN9354_SF_IA_MAC_RX_MULCST_C0                  = 0x041A, // Port 0 MAC Receive Multicast Count Register
    LAN9354_SF_IA_MAC_RX_BRDCST_C0                  = 0x041B, // Port 0 MAC Receive Broadcast Count Register
    LAN9354_SF_IA_MAC_RX_PAUSE_C0                   = 0x041C, // Port 0 MAC Receive Pause Count Register
    LAN9354_SF_IA_MAC_RX_FRAG_C0                    = 0x041D, // Port 0 MAC Receive Fragment Error Count Register
    LAN9354_SF_IA_MAC_RX_JABB_C0                    = 0x041E, // Port 0 MAC Receive Jabber Error Count Register
    LAN9354_SF_IA_MAC_RX_ALIGN_C0                   = 0x041F, // Port 0 MAC Receive Alignment Error Count Register
    LAN9354_SF_IA_MAC_RX_PLEN_C0                    = 0x0420, // Port 0 MAC Receive Packet Length Count Register
LAN9354_SF_IA_MAC_RX_OKPLEN_C0                      = 0x0421, // Port 0 MAC Receive Good Packet Length Count Register
    LAN9354_SF_IA_MAC_RX_SYMBL_C0                   = 0x0422, // Port 0 MAC Receive Symbol Error Count Register
    LAN9354_SF_IA_MAC_RX_CTLFRM_C0                  = 0x0423, // Port 0 MAC Receive Control Frame Count Register
    LAN9354_SF_IA_MAC_TX_CFG_0                      = 0x0440, // Port 0 MAC Transmit Configuration Register
    LAN9354_SF_IA_MAC_TX_FC_SET0                    = 0x0441, // Port 0 MAC Transmit Flow Control Settings Register
    LAN9354_SF_IA_MAC_TX_DEFER_C0                   = 0x0451, // Port 0 MAC Transmit Deferred Count Register
    LAN9354_SF_IA_MAC_TX_PAUSE_C0                   = 0x0452, // Port 0 MAC Transmit Pause Count Register
    LAN9354_SF_IA_MAC_TX_PKTOK_C0                   = 0x0453, // Port 0 MAC Transmit OK Count Register
    LAN9354_SF_IA_MAC_TX_64_C0                      = 0x0454, // Port 0 MAC Transmit 64 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S0_C0                      = 0x0455, // Port 0 Mac Transmit 65 to 127 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S1_C0                      = 0x0456, // Port 0 Mac Transmit 128 to 255 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S2_C0                      = 0x0457, // Port 0 Mac Transmit 255 to 511 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S3_C0                      = 0x0458, // Port 0 Mac Transmit 512 to 1023 Byte Count Register
    LAN9354_SF_IA_MAC_TX_1K_MAX_C0                  = 0x0459, // Port 0 Mac Transmit 1024 to Max Byte Count Register
    LAN9354_SF_IA_MAC_TX_UNDSIZE_C0                 = 0x045A, // Port 0 MAC Transmit Undersize Count Register
    LAN9354_SF_IA_MAC_TX_PLEN_C0                    = 0x045C, // Port 0 MAC Transmit Packet Length Count Register
    LAN9354_SF_IA_MAC_TX_BRDCST_C0                  = 0x045D, // Port 0 MAC Transmit Broadcast Count Register
    LAN9354_SF_IA_MAC_TX_MULCST_C0                  = 0x045E, // Port 0 MAC Transmit Multicast Count Register
    LAN9354_SF_IA_MAC_TX_LATECOL_C0                 = 0x045F, // Port 0 MAC Transmit Late Collision Count Register
    LAN9354_SF_IA_MAC_TX_EXCOL_C0                   = 0x0460, // Port 0 MAC Transmit Excessive Collision Count Register
    LAN9354_SF_IA_MAC_TX_SGLCOL_C0                  = 0x0461, // Port 0 MAC Transmit Single Collision Count Register
    LAN9354_SF_IA_MAC_TX_MCOL_C0                    = 0x0462, // Port 0 MAC Transmit Multiple Collision Count Register
    LAN9354_SF_IA_MAC_TX_TCOL_C0                    = 0x0463, // Port 0 MAC Transmit Total Collision Count Register
    LAN9354_SF_IA_MAC_IMR_0                         = 0x0480, // Port 0 MAC Interrupt Mask Register
    LAN9354_SF_IA_MAC_IPR_0                         = 0x0481, // Port 0 MAC Interrupt Pending REgister

    // Switch Port 1 CSRs (x=1)
    LAN9354_SF_IA_MAC_VER_ID_1                      = 0x0800, // Port 1 MAC Version ID Register
    LAN9354_SF_IA_MAC_RX_CFG_1                      = 0x0801, // Port 1 MAC Receive Configuration Register
    LAN9354_SF_IA_MAC_RX_USZ_C1                     = 0x0810, // Port 1 MAC Receive Undersize Count Register
    LAN9354_SF_IA_MAC_RX_64_C1                      = 0x0811, // Port 1 MAC REceive 64 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S0_C1                      = 0x0812, // Port 1 Mac Receive 65 to 127 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S1_C1                      = 0x0813, // Port 1 Mac Receive 128 to 255 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S2_C1                      = 0x0814, // Port 1 Mac Receive 255 to 511 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S3_C1                      = 0x0815, // Port 1 Mac Receive 512 to 1023 Byte Count Register
    LAN9354_SF_IA_MAC_RX_1K_MAX_C1                  = 0x0816, // Port 1 Mac Receive 1024 to Max Byte Count Register
    LAN9354_SF_IA_MAC_RX_OVRSZE_C1                  = 0x0817, // Port 1 MAC Receive Oversize Count Register
    LAN9354_SF_IA_MAC_RX_PKTOK_C1                   = 0x0818, // Port 1 MAC Receive OK Count Register
    LAN9354_SF_IA_MAC_RX_CRCERR_C1                  = 0x0819, // Port 1 MAC Receive CRC Error Count Register
    LAN9354_SF_IA_MAC_RX_MULCST_C1                  = 0x081A, // Port 1 MAC Receive Multicast Count Register
    LAN9354_SF_IA_MAC_RX_BRDCST_C1                  = 0x081B, // Port 1 MAC Receive Broadcast Count Register
    LAN9354_SF_IA_MAC_RX_PAUSE_C1                   = 0x081C, // Port 1 MAC Receive Pause Count Register
    LAN9354_SF_IA_MAC_RX_FRAG_C1                    = 0x081D, // Port 1 MAC Receive Fragment Error Count Register
    LAN9354_SF_IA_MAC_RX_JABB_C1                    = 0x081E, // Port 1 MAC Receive Jabber Error Count Register
    LAN9354_SF_IA_MAC_RX_ALIGN_C1                   = 0x081F, // Port 1 MAC Receive Alignment Error Count Register
    LAN9354_SF_IA_MAC_RX_PLEN_C1                    = 0x0820, // Port 1 MAC Receive Packet Length Count Register
    LAN9354_SF_IA_MAC_RX_OKPLEN_C1                  = 0x0821, // Port 1 MAC Receive Good Packet Length Count Register
    LAN9354_SF_IA_MAC_RX_SYMBL_C1                   = 0x0822, // Port 1 MAC Receive Symbol Error Count Register
    LAN9354_SF_IA_MAC_RX_CTLFRM_C1                  = 0x0823, // Port 1 MAC Receive Control Frame Count Register
    LAN9354_SF_IA_MAC_TX_CFG_1                      = 0x0840, // Port 1 MAC Transmit Configuration Register
    LAN9354_SF_IA_MAC_TX_FC_SET1                    = 0x0841, // Port 1 MAC Transmit Flow Control Settings Register
    LAN9354_SF_IA_MAC_TX_DEFER_C1                   = 0x0851, // Port 1 MAC Transmit Deferred Count Register
    LAN9354_SF_IA_MAC_TX_PAUSE_C1                   = 0x0852, // Port 1 MAC Transmit Pause Count Register
    LAN9354_SF_IA_MAC_TX_PKTOK_C1                   = 0x0853, // Port 1 MAC Transmit OK Count Register
    LAN9354_SF_IA_MAC_TX_64_C1                      = 0x0854, // Port 1 MAC Transmit 64 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S0_C1                      = 0x0855, // Port 1 Mac Transmit 65 to 127 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S1_C1                      = 0x0856, // Port 1 Mac Transmit 128 to 255 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S2_C1                      = 0x0857, // Port 1 Mac Transmit 255 to 511 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S3_C1                      = 0x0858, // Port 1 Mac Transmit 512 to 1023 Byte Count Register
    LAN9354_SF_IA_MAC_TX_1K_MAX_C1                  = 0x0859, // Port 1 Mac Transmit 1024 to Max Byte Count Register
    LAN9354_SF_IA_MAC_TX_UNDSIZE_C1                 = 0x085A, // Port 1 MAC Transmit Undersize Count Register
    LAN9354_SF_IA_MAC_TX_PLEN_C1                    = 0x085C, // Port 1 MAC Transmit Packet Length Count Register
    LAN9354_SF_IA_MAC_TX_BRDCST_C1                  = 0x085D, // Port 1 MAC Transmit Broadcast Count Register
    LAN9354_SF_IA_MAC_TX_MULCST_C1                  = 0x085E, // Port 1 MAC Transmit Multicast Count Register
    LAN9354_SF_IA_MAC_TX_LATECOL_C1                 = 0x085F, // Port 1 MAC Transmit Late Collision Count Register
    LAN9354_SF_IA_MAC_TX_EXCOL_C1                   = 0x0860, // Port 1 MAC Transmit Excessive Collision Count Register
    LAN9354_SF_IA_MAC_TX_SGLCOL_C1                  = 0x0861, // Port 1 MAC Transmit Single Collision Count Register
    LAN9354_SF_IA_MAC_TX_MCOL_C1                    = 0x0862, // Port 1 MAC Transmit Multiple Collision Count Register
    LAN9354_SF_IA_MAC_TX_TCOL_C1                    = 0x0863, // Port 1 MAC Transmit Total Collision Count Register
    LAN9354_SF_IA_MAC_IMR_1                         = 0x0880, // Port 1 MAC Interrupt Mask Register
    LAN9354_SF_IA_MAC_IPR_1                         = 0x0881, // Port 1 MAC Interrupt Pending REgister

    // Switch Port 2 CSRs (x=2)
    LAN9354_SF_IA_MAC_VER_ID_2                      = 0x0C00, // Port 2 MAC Version ID Register
    LAN9354_SF_IA_MAC_RX_CFG_2                      = 0x0c01, // Port 2 MAC Receive Configuration Register
    LAN9354_SF_IA_MAC_RX_USZ_C2                     = 0x0C10, // Port 2 MAC Receive Undersize Count Register
    LAN9354_SF_IA_MAC_RX_64_C2                      = 0x0C11, // Port 2 MAC REceive 64 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S1_C2                      = 0x0C12, // Port 2 Mac Receive 65 to 127 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S11_C2                     = 0x0C13, // Port 2 Mac Receive 128 to 255 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S2_C2                      = 0x0C14, // Port 2 Mac Receive 255 to 511 Byte Count Register
    LAN9354_SF_IA_MAC_RX_S3_C2                      = 0x0C15, // Port 2 Mac Receive 512 to 1023 Byte Count Register
    LAN9354_SF_IA_MAC_RX_2K_MAX_C2                  = 0x0C16, // Port 2 Mac Receive 1024 to Max Byte Count Register
    LAN9354_SF_IA_MAC_RX_OVRSZE_C2                  = 0x0C17, // Port 2 MAC Receive Oversize Count Register
    LAN9354_SF_IA_MAC_RX_PKTOK_C2                   = 0x0C18, // Port 2 MAC Receive OK Count Register
    LAN9354_SF_IA_MAC_RX_CRCERR_C2                  = 0x0C19, // Port 2 MAC Receive CRC Error Count Register
    LAN9354_SF_IA_MAC_RX_MULCST_C2                  = 0x0C1A, // Port 2 MAC Receive Multicast Count Register
    LAN9354_SF_IA_MAC_RX_BRDCST_C2                  = 0x0C1B, // Port 2 MAC Receive Broadcast Count Register
    LAN9354_SF_IA_MAC_RX_PAUSE_C2                   = 0x0C1C, // Port 2 MAC Receive Pause Count Register
    LAN9354_SF_IA_MAC_RX_FRAG_C2                    = 0x0C1D, // Port 2 MAC Receive Fragment Error Count Register
    LAN9354_SF_IA_MAC_RX_JABB_C2                    = 0x0C1E, // Port 2 MAC Receive Jabber Error Count Register
    LAN9354_SF_IA_MAC_RX_ALIGN_C2                   = 0x0C1F, // Port 2 MAC Receive Alignment Error Count Register
    LAN9354_SF_IA_MAC_RX_PLEN_C2                    = 0x0C20, // Port 2 MAC Receive Packet Length Count Register
    LAN9354_SF_IA_MAC_RX_OKPLEN_C2                  = 0x0C21, // Port 2 MAC Receive Good Packet Length Count Register
    LAN9354_SF_IA_MAC_RX_SYMBL_C2                   = 0x0C22, // Port 2 MAC Receive Symbol Error Count Register
    LAN9354_SF_IA_MAC_RX_CTLFRM_C2                  = 0x0C23, // Port 2 MAC Receive Control Frame Count Register
    LAN9354_SF_IA_MAC_TX_CFG_2                      = 0x0C40, // Port 2 MAC Transmit Configuration Register
    LAN9354_SF_IA_MAC_TX_FC_SET2                    = 0x0C41, // Port 2 MAC Transmit Flow Control Settings Register
    LAN9354_SF_IA_MAC_TX_DEFER_C2                   = 0x0C51, // Port 2 MAC Transmit Deferred Count Register
    LAN9354_SF_IA_MAC_TX_PAUSE_C2                   = 0x0C52, // Port 2 MAC Transmit Pause Count Register
    LAN9354_SF_IA_MAC_TX_PKTOK_C2                   = 0x0C53, // Port 2 MAC Transmit OK Count Register
    LAN9354_SF_IA_MAC_TX_64_C2                      = 0x0C54, // Port 2 MAC Transmit 64 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S1_C2                      = 0x0C55, // Port 2 Mac Transmit 65 to 127 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S11_C2                     = 0x0C56, // Port 2 Mac Transmit 128 to 255 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S2_C2                      = 0x0C57, // Port 2 Mac Transmit 255 to 511 Byte Count Register
    LAN9354_SF_IA_MAC_TX_S3_C2                      = 0x0C58, // Port 2 Mac Transmit 512 to 1023 Byte Count Register
    LAN9354_SF_IA_MAC_TX_2K_MAX_C2                  = 0x0C59, // Port 2 Mac Transmit 1024 to Max Byte Count Register
    LAN9354_SF_IA_MAC_TX_UNDSIZE_C2                 = 0x0C5A, // Port 2 MAC Transmit Undersize Count Register
    LAN9354_SF_IA_MAC_TX_PLEN_C2                    = 0x0C5C, // Port 2 MAC Transmit Packet Length Count Register
    LAN9354_SF_IA_MAC_TX_BRDCST_C2                  = 0x0C5D, // Port 2 MAC Transmit Broadcast Count Register
    LAN9354_SF_IA_MAC_TX_MULCST_C2                  = 0x0C5E, // Port 2 MAC Transmit Multicast Count Register
    LAN9354_SF_IA_MAC_TX_LATECOL_C2                 = 0x0C5F, // Port 2 MAC Transmit Late Collision Count Register
    LAN9354_SF_IA_MAC_TX_EXCOL_C2                   = 0x0C60, // Port 2 MAC Transmit Excessive Collision Count Register
    LAN9354_SF_IA_MAC_TX_SGLCOL_C2                  = 0x0C61, // Port 2 MAC Transmit Single Collision Count Register
    LAN9354_SF_IA_MAC_TX_MCOL_C2                    = 0x0C62, // Port 2 MAC Transmit Multiple Collision Count Register
    LAN9354_SF_IA_MAC_TX_TCOL_C2                    = 0x0C63, // Port 2 MAC Transmit Total Collision Count Register
    LAN9354_SF_IA_MAC_IMR_2                         = 0x0C80, // Port 2 MAC Interrupt Mask Register
    LAN9354_SF_IA_MAC_IPR_2                         = 0x0C81, // Port 2 MAC Interrupt Pending Register

    // Switch Engine CSRs
    LAN9354_SF_IA_SWE_ALR_CMD                       = 0x1800, // Switch Engine ALR Command REgister
    LAN9354_SF_IA_SWE_ALR_WR_D0                     = 0x1801, // Switch Engine ALR Write Data 0 Register
    LAN9354_SF_IASWE_ALR_WR_D1                      = 0x1802, // Switch Engine ALR Write Data 1 Register
    LAN9354_SF_IA_SWE_ALR_RD_D0                     = 0x1805, // Switch Engine ALR Read Data 0 Register
    LAN9354_SF_IA_SWE_ALR_RD_D1                     = 0x1806, // Switch Engine ALR Read Data 1 Register
    LAN9354_SF_IA_SWE_ALR_CMD_STS                   = 0x1808, // Switch Engine ALR Command Status Register
    LAN9354_SF_IA_SWE_ALR_CFG                       = 0x1809, // Switch Engine ALR Configuration Register
    LAN9354_SF_IA_SWE_VLAN_CMD                      = 0x180B, // Switch Engine VLAN Command Register
    LAN9354_SF_IA_SWE_VLAN_WR_DATA                  = 0x180C, // Switch Engine VLAN Write Register
    LAN9354_SF_IA_SWE_VLAN_RO_DATA                  = 0x180E, // Switch Engine VLAN Read Data Register
    LAN9354_SF_IA_SWE_VLAN_CMD_STS                  = 0x1810, // Switch Engine VLAN Command Status Register
    LAN9354_SF_IA_SWE_DSRV_TCMD                     = 0x1811, // Switch Engine DIFSERV Table Command Register
    LAN9354_SF_IA_SWE_DSRV_TWRDATA                  = 0x1812, // Switch Engine DIFSERV Table Write Data
    LAN9354_SF_IA_SWE_DSRV_TRODATA                  = 0x1813, // Switch Engine DIFFSERV Table Read Data Register
    LAN9354_SF_IA_SWE_DSRV_TSTS                     = 0x1814, // Switch Engine DIFFSERV Table Command Status Register
    LAN9354_SF_IA_SWE_GLB_ING_CFG                   = 0x1840, // Switch Engine Global Ingress Configuration Register
    LAN9354_SF_IA_SWE_PORT_ING_CFG                  = 0x1841, // Switch Engine Port Ingress Configuration Register
    LAN9354_SF_IA_SWE_ADMT_ONLY_VLAN                = 0x1842, // Switch Engine Admit Only VLAN Register
    LAN9354_SF_IA_SWE_PORT_STATE                    = 0x1843, // Switch Engine Port State Register
    LAN9354_SF_IA_SWE_PRI_QUE                       = 0x1845, // Switch Engine PRiority to Queue Register
    LAN9354_SF_IA_SWE_PORT_MIRROR                   = 0x1846, // Switch Engine Port Mirroring Register
    LAN9354_SF_IA_SWE_ING_PORT_TYPE                 = 0x1847, // Switch Engine Ingress Port Type Register
    LAN9354_SF_IA_SWE_BCST_THROT                    = 0x1848, // Switch Engine Broadcast Throttling Register
    LAN9354_SF_IA_SWE_ADMT_N_MEMBER                 = 0x1849, // Swtich Engine Admit Non Member Register
    LAN9354_SF_IA_SWE_ING_RCFG                      = 0x184A, // Switch Engine Ingress Rate Configuration Register
    LAN9354_SF_IA_SWE_ING_RCMD                      = 0x184B, // Switch Engine Ingress Rate Command Register
    LAN9354_SF_IA_SWE_ING_RCMD_STS                  = 0x184C, // Switch Engine Ingress Rate Command Status Register
    LAN9354_SF_IA_SWE_ING_RWR_DATA                  = 0x184D, // Switch Engine Ingress Rate Write Data Register
    LAN9354_SF_IA_SWE_ING_RATE_RD_DATA              = 0x184E, // Switch Engine Ingress Rate Read Data Register
    LAN9354_SF_IA_SWE_FILTC_0                       = 0x1850, // Switch Engine Port 0 Ingress Filtered Count Register
    LAN9354_SF_IA_SWE_FILTC_1                       = 0x1851, // Switch Engine Port 1 Ingress Filtered Count Register
    LAN9354_SF_IA_SWE_FILTC_2                       = 0x1852, // Switch Engine Port 2 Ingress Filtered Count Register
    LAN9354_SF_IA_SWE_ING_RGNT0                     = 0x1855, // Switch Engine Port 0 Learn Discard Count Register
    LAN9354_SF_IA_SWE_ING_RGNT1                     = 0x1856, // Switch Engine Port 1 Learn Discard Count Register
    LAN9354_SF_IA_SWE_ING_RGNT2                     = 0x1857, // Switch Engine Port 2 Learn Discard Count Register
    LAN9354_SF_IA_SWE_LNDCRD_C0                     = 0x1858, // Switch Engine Port 0 Learn Discard Count Register
    LAN9354_SF_IA_SWE_LNDCRD_C1                     = 0x1859, // Switch Engine Port 1 Learn Discard Count Register
    LAN9354_SF_IA_SWE_LNDCRD_C2                     = 0x185A, // Switch Engine Port 2 Learn Discard Count Register
    LAN9354_SF_IA_SWE_IMR                           = 0x1880, // Switch Engine Interrupt Mask Register
    LAN9354_SF_IA_SWE_IPR                           = 0x1881, // Switch Engine Interrupt PEnding Register

    // Buffer Manager (BM) CSRs
    LAN9354_SF_IA_BM_CFG                            = 0x1C00, // Buffer Manager Configuration Register
    LAN9354_SF_IA_BM_DROP_LVL                       = 0x1C01, // Buffer Manager Drop Level Register
    LAN9354_SF_IA_BM_FC_PAUSE_LVL                   = 0x1C02, // Buffer Manager Flow Control Pause Level Register
    LAN9354_SF_IA_BM_FC_RESUME_LVL                  = 0x1C03, // Buffer Manager Flow Control Resume Level Register
    LAN9354_SF_IA_BM_BCST_LVL                       = 0x1C04, // Buffer Manager Broadcast Buffer Level Register
    LAN9354_SF_IA_BM_DRP_CSRC_0                     = 0x1C05, // Buffer ManagerPort 0 Drop Count Register
    LAN9354_SF_IA_BM_DRP_CSRC_1                     = 0x1C06, // Buffer ManagerPort 1 Drop Count Register
    LAN9354_SF_IA_BM_DRP_CSRC_2                     = 0x1C07, // Buffer ManagerPort 2 Drop Count Register
    LAN9354_SF_IA_BM_RST_STS                        = 0x1C08, // Buffer Manager Reset Status Register
    LAN9354_SF_IA_BM_RDCRD_TCMD                     = 0x1C09, // Buffer Manager Random Discard Table Command Register
    LAN9354_SF_IA_BM_RDCRD_TWDATA                   = 0x1C0A, // Buffer Manager Random Discard Table Write Data Register
    LAN9354_SF_IA_BM_RDCRD_TRDATA                   = 0x1C0B, // Buffer Manager Random Discard Table Read Data Register
    LAN9354_SF_IA_BM_EGRSS_PORT_TYPE                = 0x1C0C, // Buffer Manager Egress Port Type Register
    LAN9354_SF_IA_BM_ERATE_00_01                    = 0x1C0D, // Buffer Manager Port 0 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_SF_IA_BM_ERATE_02_03                    = 0x1C0E, // Buffer Manager Port 0 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_SF_IA_BM_ERATE_10_11                    = 0x1C0F, // Buffer Manager Port 1 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_SF_IA_BM_ERATE_12_13                    = 0x1C10, // Buffer Manager Port 1 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_SF_IA_BM_ERATE_20_21                    = 0x1C11, // Buffer Manager Port 2 Egress Rate PRiotity Queue 0/1 Register
    LAN9354_SF_IA_BM_ERATE_22_23                    = 0x1C12, // Buffer Manager Port 2 Egress Rate PRiotity Queue 2/3 Register
    LAN9354_SF_IA_BM_VLAN_0                         = 0x1C13, // Buffer Manager Port 0 Default VLAN ID and PRiority Register
    LAN9354_SF_IA_BM_VLAN_1                         = 0x1C14, // Buffer Manager Port 1 Default VLAN ID and PRiority Register
    LAN9354_SF_IA_BM_VLAN_2                         = 0x1C15, // Buffer Manager Port 2 Default VLAN ID and PRiority Register
    LAN9354_SF_IA_BM_RATE_DRP0                      = 0x1C16, // Buffer Manager Port 0 Ingress Rate Drop Count Register
    LAN9354_SF_IA_BM_RATE_DRP1                      = 0x1C17, // Buffer Manager Port 0 Ingress Rate Drop Count Register
    LAN9354_SF_IA_BM_RATE_DRP2                      = 0x1C18, // Buffer Manager Port 0 Ingress Rate Drop Count Register
    LAN9354_SF_IA_BM_IMR                            = 0x1C20, // Buffer Manager Interrupt Mask Register
    LAN9354_SF_IA_BM_IPR                            = 0x1C21, // Buffer Manger Interrupt Pending Register
}LAN9354_INDIRECT_SW_CTL_STAT_REG;



#endif  // H_LAN_9354_PRIV_H_


