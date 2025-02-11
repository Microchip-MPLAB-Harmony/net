/*******************************************************************************
  External Phy register definition file

  Company:
    Microchip Technology Inc.
    
  FIle Name:
    drv_extphy_regs.h

  Summary:
    External Phy register definitions

  Description:
    This file provides the External Phy register definition.
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

#ifndef H_DRV_EXTPHY_REGS_H
#define H_DRV_EXTPHY_REGS_H

// This file contains common definitions (accross all PHY's)
//


// MIIM registers access
//

typedef enum
{
    // basic registers, accross all registers: 0-1
    PHY_REG_BMCON           = 0,  // Basic:    Control Register
    PHY_REG_BMSTAT          = 1,  // Basic:    Status Register
    PHY_REG_PHYID1          = 2,  // Extended: PHY Identifier 1
    PHY_REG_PHYID2          = 3,  // Extended: PHY Identifier 2
    PHY_REG_ANAD            = 4,  // Extended: Auto-Negotiation Advertisement Register
    PHY_REG_ANLPAD          = 5,  // Extended: Auto-Negotiation Link Partner Advertisement
    PHY_REG_ANLPADNP        = 5,  // Extended: Auto-Negotiation Link Partner Advertisement, Next Page (?)
    PHY_REG_ANEXP           = 6,  // Extended: Auto-Negotiation Expansion
    PHY_REG_ANNPTR          = 7,  // Extended: Auto-Negotiation Next Page Transmit
    PHY_REG_ANLPRNP         = 8,  // Extended: Auto-Negotiation Link Partner Received Next Page
    PHY_REG_1000BASECON     = 9,
    PHY_REG_1000BASESTAT    = 10,
    PHY_REG_MMD_CONTROL     = 13,
    PHY_REG_MMD_DATA        = 14,
    PHY_REG_EXTSTAT         = 15,
    // specific vendor registers: 16-31
    PHY_REG_VENDOR  = 16,   // this is updated by each specific PHY
    //
    //
    //
    PHY_REGISTERS       = 32    // total number of registers
} PHY_BASIC_REG;

// MIIM registers definitions
//

// basic registers
//

typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned             :6;
        unsigned SPEED1000   :1;
        unsigned COLTEST     :1;
        unsigned DUPLEX      :1;
        unsigned AN_RESTART  :1;
        unsigned ISOLATE     :1;
        unsigned PDWN        :1;
        unsigned AN_ENABLE   :1;
        unsigned SPEED       :1;
        unsigned LOOPBACK    :1;
        unsigned RESET       :1;
    };
} BMCONbits_t;    // reg 0: PHY_REG_BMCON


#define BMCON_SPEED1000_MASK   0x0040U
#define BMCON_COLTEST_MASK     0x0080U
#define BMCON_DUPLEX_MASK      0x0100U
#define BMCON_AN_RESTART_MASK  0x0200U
#define BMCON_ISOLATE_MASK     0x0400U
#define BMCON_PDWN_MASK        0x0800U
#define BMCON_AN_ENABLE_MASK   0x1000U
#define BMCON_SPEED100_MASK    0x2000U
#define BMCON_LOOPBACK_MASK    0x4000U
#define BMCON_RESET_MASK       0x8000U



typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned EXTEND_ABLE        :1;
        unsigned JABBER_DET         :1;
        unsigned LINK_STAT          :1;
        unsigned AN_ABLE            :1;
        unsigned REM_FAULT          :1;
        unsigned AN_COMPLETE        :1;
        unsigned PREAMBLE_SUPPRESS  :1;
        unsigned                    :1;
        unsigned EXTENDED_STAT      :1;
        unsigned                    :2;
        unsigned BASE10T_HDX        :1;
        unsigned BASE10T_FDX        :1;
        unsigned BASE100TX_HDX      :1;
        unsigned BASE100TX_FDX      :1;
        unsigned BASE100T4          :1;
    };
} BMSTATbits_t;   // reg 1: PHY_REG_BMSTAT

#define BMSTAT_EXTEND_ABLE_MASK        0x0001
#define BMSTAT_JABBER_DET_MASK         0x0002U
#define BMSTAT_LINK_STAT_MASK          0x0004U
#define BMSTAT_AN_ABLE_MASK            0x0008U
#define BMSTAT_REM_FAULT_MASK          0x0010U
#define BMSTAT_AN_COMPLETE_MASK        0x0020U
#define BMSTAT_PREAMBLE_SUPPRESS_MASK  0x0040U
#define BMSTAT_EXTSTAT_MASK            0x0100U
#define BMSTAT_BASE10T_HDX_MASK        0x0800U
#define BMSTAT_BASE10T_FDX_MASK        0x1000U
#define BMSTAT_BASE100TX_HDX_MASK      0x2000U
#define BMSTAT_BASE100TX_FDX_MASK      0x4000U
#define BMSTAT_BASE100T4_MASK          0x8000U

#define EXTSTAT_1000BASEX_FDX_MASK     0x8000U
#define EXTSTAT_1000BASEX_HDX_MASK     0x4000U
#define EXTSTAT_1000BASET_FDX_MASK     0x2000U
#define EXTSTAT_1000BASET_HDX_MASK     0x1000U


#define BASE1000X_FDX_MASK 0x0400U
#define BASE1000X_HDX_MASK 0x0200U
#define BASE1000T_FDX_MASK 0x0100U
#define BASE1000T_HDX_MASK 0x0080U


#define BMSTAT_NEGOTIATION_MASK    (BMSTAT_BASE10T_HDX_MASK | BMSTAT_BASE10T_FDX_MASK| \
                                    BMSTAT_BASE100TX_HDX_MASK | BMSTAT_BASE100TX_FDX_MASK | \
                                    BMSTAT_BASE100T4_MASK)       // negotiation field mask
#define BMSTAT_CAPABILITY_POS      11U  // capability field position in basic status register
#define BMSTAT_NEGOTIATION_LENGTH  5U   // negotiation field length


// extended registers
//

typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned OUI_MSB  :16;
    };
} PHYID1bits_t;   // reg 2: PHY_REG_PHYID1


typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned MNF_REV    :4;
        unsigned MNF_MODEL  :6;
        unsigned OUI_LSB    :6;
    };
} PHYID2bits_t;   // reg 3: PHY_REG_PHYID2

#define PHYID2_MNF_REV_MASK    0x000FU
#define PHYID2_MNF_MODEL_MASK  0x03F0U
#define PHYID2_OUI_LSB_MASK    0xFC00U


typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned PROT_SEL       :5;
        unsigned BASE10T        :1;
        unsigned BASE10T_FDX    :1;
        unsigned BASE100TX      :1;
        unsigned BASE100TX_FDX  :1;
        unsigned BASE100T4      :1;
        unsigned PAUSE          :1; // NOTE: the PAUSE fields coding LAN8xxx PHYs may be reversed!
        unsigned ASM_DIR        :1; // typo in the data sheet?
        unsigned                :1;
        unsigned REM_FAULT      :1;
        unsigned                :1;
        unsigned NP_ABLE        :1;
    };
} ANADbits_t;     // reg 4: PHY_REG_ANAD

#define ANAD_PROT_SEL_MASK       0x001FU
#define ANAD_BASE10T_MASK        0x0020U
#define ANAD_BASE10T_FDX_MASK    0x0040U
#define ANAD_BASE100TX_MASK      0x0080U
#define ANAD_BASE100TX_FDX_MASK  0x0100U
#define ANAD_BASE100T4_MASK      0x0200U
#define ANAD_PAUSE_MASK          0x0400U
#define ANAD_ASM_DIR_MASK        0x0800U
#define ANAD_REM_FAULT_MASK      0x2000U
#define ANAD_NP_ABLE_MASK        0x8000U

#define ANAD_NEGOTIATION_MASK      ((uint16_t)ANAD_BASE10T_MASK | (uint16_t)ANAD_BASE10T_FDX_MASK | \
                                    (uint16_t)ANAD_BASE100TX_MASK|(uint16_t)ANAD_BASE100TX_FDX_MASK| \
                                    (uint16_t)ANAD_BASE100T4_MASK)   // negotiation field mask
#define ANAD_CAPABLITY_POS         5U // capability field position in Auto-Neg Adv register
#define ANAD_NEGOTIATION_LENGTH    5U // negotiation field length


typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned PROT_SEL       :5;
        unsigned BASE10T        :1;
        unsigned BASE10T_FDX    :1;
        unsigned BASE100TX      :1;
        unsigned BASE100TX_FDX  :1;
        unsigned BASE100T4      :1;
        unsigned PAUSE          :1;
        unsigned ASM_DIR        :1;
        unsigned                :1;
        unsigned REM_FAULT      :1;
        unsigned ACK            :1;
        unsigned NP_ABLE        :1;
    };
} ANLPADbits_t;   // reg 5: PHY_REG_ANLPAD

#define ANLPAD_PROT_SEL_MASK       0x001FU
#define ANLPAD_BASE10T_MASK        0x0020U
#define ANLPAD_BASE10T_FDX_MASK    0x0040U
#define ANLPAD_BASE100TX_MASK      0x0080U
#define ANLPAD_BASE100TX_FDX_MASK  0x0100U
#define ANLPAD_BASE100T4_MASK      0x0200U
#define ANLPAD_PAUSE_MASK          0x0400U
#define ANLPAD_ASM_DIR_MASK        0x0800U
#define ANLPAD_REM_FAULT_MASK      0x2000U
#define ANLPAD_ACK_MASK            0x4000U
#define ANLPAD_NP_ABLE_MASK        0x8000U

typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {    
        unsigned MESSAGE:11;
        unsigned TOGGLE:1;      
        unsigned ACK2:1;
        unsigned MSGP:1;
        unsigned ACK:1;
        unsigned NP:1;
    };
} ANLPADNPbits_t; // reg 5: PHY_REG_ANLPADNP: next page

#define ANLPADNP_MESSAGE_MASK  0x07FFU
#define ANLPADNP_TOGGLE_MASK   0x0800U
#define ANLPADNP_ACK2_MASK     0x1000U
#define ANLPADNP_MSGP_MASK     0x2000U
#define ANLPADNP_ACK_MASK      0x4000U
#define ANLPADNP_NP_MASK       0x8000U


typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned LP_AN_ABLE  :1;
        unsigned PAGE_RX     :1;
        unsigned NP_ABLE     :1;
        unsigned LP_NP_ABLE  :1;
        unsigned PDF         :1;
        unsigned             :11;
    };
} ANEXPbits_t;    // reg 6: PHY_REG_ANEXP

#define ANEXP_LP_AN_ABLE_MASK  0x0001U
#define ANEXP_PAGE_RX_MASK     0x0002U
#define ANEXP_NP_ABLE_MASK     0x0004U
#define ANEXP_LP_NP_ABLE_MASK  0x0008U
#define ANEXP_PDF_MASK         0x0010U

typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned MESSAGE  :11;
        unsigned TOGGLE   :1;
        unsigned ACK2     :1;
        unsigned MSGP     :1;
        unsigned          :1;
        unsigned NP       :1;
    };
} ANNPTRbits_t;   // reg 7: PHY_REG_ANNPTR

#define ANNPTR_MESSAGE_MASK  0x07FFU
#define ANNPTR_TOGGLE_MASK   0x0800U
#define ANNPTR_ACK2_MASK     0x1000U
#define ANNPTR_MSGP_MASK     0x2000U
#define ANNPTR_NP_MASK       0x8000U

typedef union
{
    uint16_t    w;
    struct __attribute__((packed))
    {
        unsigned MESSAGE  :11;
        unsigned TOGGLE   :1;
        unsigned ACK2     :1;
        unsigned MSGP     :1;
        unsigned ACK      :1;
        unsigned NP       :1;
    };
} ANLPRNPbits_t;  // reg 8: PHY_REG_ANLPRNP

#define ANLPRNP_MESSAGE_MASK  0x07FFU
#define ANLPRNP_TOGGLE_MASK   0x0800U
#define ANLPRNP_ACK2_MASK     0x1000U
#define ANLPRNP_MSGP_MASK     0x2000U
#define ANLPRNP_ACK_MASK      0x4000U
#define ANLPRNP_NP_MASK       0x8000U

#define EXTSTAT_BASE1000X_FDX_MASK 0x8000U
#define EXTSTAT_BASE1000X_HDX_MASK 0x4000U
#define EXTSTAT_BASE1000T_FDX_MASK 0x2000U
#define EXTSTAT_BASE1000T_HDX_MASK 0x1000U

#define TCTRL_1000BASE_TFDX_MASK 0x0200U
#define TCTRL_1000BASE_THDX_MASK 0x0100U

#define STAT1000B_1000T_FDX_MASK      0x0800U
#define STAT1000B_1000T_HDX_MASK      0x0400U


#define KSZ9031_PHYID1          (0x0022U)
#define KSZ9031_PHYID2_ID_NUM   (0x1400U)
#define KSZ9031_PHYID2_MOD_NUM  (0x0220U)
#define KSZ9031_PHYID2          ((uint16_t)KSZ9031_PHYID2_ID_NUM | (uint16_t)KSZ9031_PHYID2_MOD_NUM)

#endif  // H_DRV_EXTPHY_REGS_H
