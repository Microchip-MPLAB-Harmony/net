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
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _DRV_EXTPHY_REGS_H_
#define _DRV_EXTPHY_REGS_H_

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
    struct
    {
        uint16_t             :6;
        uint16_t SPEED1000   :1;
        uint16_t COLTEST     :1;
        uint16_t DUPLEX      :1;
        uint16_t AN_RESTART  :1;
        uint16_t ISOLATE     :1;
        uint16_t PDWN        :1;
        uint16_t AN_ENABLE   :1;
        uint16_t SPEED       :1;
        uint16_t LOOPBACK    :1;
        uint16_t RESET       :1;
    };
} __BMCONbits_t;    // reg 0: PHY_REG_BMCON


#define _BMCON_SPEED1000_MASK   0x0040
#define _BMCON_COLTEST_MASK     0x0080
#define _BMCON_DUPLEX_MASK      0x0100
#define _BMCON_AN_RESTART_MASK  0x0200
#define _BMCON_ISOLATE_MASK     0x0400
#define _BMCON_PDWN_MASK        0x0800
#define _BMCON_AN_ENABLE_MASK   0x1000
#define _BMCON_SPEED100_MASK    0x2000
#define _BMCON_LOOPBACK_MASK    0x4000
#define _BMCON_RESET_MASK       0x8000



typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t EXTEND_ABLE        :1;
        uint16_t JABBER_DET         :1;
        uint16_t LINK_STAT          :1;
        uint16_t AN_ABLE            :1;
        uint16_t REM_FAULT          :1;
        uint16_t AN_COMPLETE        :1;
        uint16_t PREAMBLE_SUPPRESS  :1;
        uint16_t                    :1;
        uint16_t EXTENDED_STAT      :1;
        uint16_t                    :2;
        uint16_t BASE10T_HDX        :1;
        uint16_t BASE10T_FDX        :1;
        uint16_t BASE100TX_HDX      :1;
        uint16_t BASE100TX_FDX      :1;
        uint16_t BASE100T4          :1;
    };
} __BMSTATbits_t;   // reg 1: PHY_REG_BMSTAT

#define _BMSTAT_EXTEND_ABLE_MASK        0x0001
#define _BMSTAT_JABBER_DET_MASK         0x0002
#define _BMSTAT_LINK_STAT_MASK          0x0004
#define _BMSTAT_AN_ABLE_MASK            0x0008
#define _BMSTAT_REM_FAULT_MASK          0x0010
#define _BMSTAT_AN_COMPLETE_MASK        0x0020
#define _BMSTAT_PREAMBLE_SUPPRESS_MASK  0x0040
#define _BMSTAT_EXTSTAT_MASK            0x0100
#define _BMSTAT_BASE10T_HDX_MASK        0x0800
#define _BMSTAT_BASE10T_FDX_MASK        0x1000
#define _BMSTAT_BASE100TX_HDX_MASK      0x2000
#define _BMSTAT_BASE100TX_FDX_MASK      0x4000
#define _BMSTAT_BASE100T4_MASK          0x8000

#define _EXTSTAT_1000BASEX_FDX_MASK     0x8000
#define _EXTSTAT_1000BASEX_HDX_MASK     0x4000
#define _EXTSTAT_1000BASET_FDX_MASK     0x2000
#define _EXTSTAT_1000BASET_HDX_MASK     0x1000


#define _BASE1000X_FDX_MASK 0x0400
#define _BASE1000X_HDX_MASK 0x0200
#define _BASE1000T_FDX_MASK 0x0100
#define _BASE1000T_HDX_MASK 0x0080


#define _BMSTAT_NEGOTIATION_MASK    (_BMSTAT_BASE10T_HDX_MASK | _BMSTAT_BASE10T_FDX_MASK| \
                                    _BMSTAT_BASE100TX_HDX_MASK | _BMSTAT_BASE100TX_FDX_MASK | \
                                    _BMSTAT_BASE100T4_MASK)       // negotiation field mask
#define _BMSTAT_CAPABILITY_POS      11  // capability field position in basic status register
#define _BMSTAT_NEGOTIATION_LENGTH  5   // negotiation field length


// extended registers
//

typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t OUI_MSB  :16;
    };
} __PHYID1bits_t;   // reg 2: PHY_REG_PHYID1


typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t MNF_REV    :4;
        uint16_t MNF_MODEL  :6;
        uint16_t OUI_LSB    :6;
    };
} __PHYID2bits_t;   // reg 3: PHY_REG_PHYID2

#define _PHYID2_MNF_REV_MASK    0x000F
#define _PHYID2_MNF_MODEL_MASK  0x03F0
#define _PHYID2_OUI_LSB_MASK    0xFC00


typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t PROT_SEL       :5;
        uint16_t BASE10T        :1;
        uint16_t BASE10T_FDX    :1;
        uint16_t BASE100TX      :1;
        uint16_t BASE100TX_FDX  :1;
        uint16_t BASE100T4      :1;
        uint16_t PAUSE          :1; // NOTE: the PAUSE fields coding LAN8xxx PHYs may be reversed!
        uint16_t ASM_DIR        :1; // typo in the data sheet?
        uint16_t                :1;
        uint16_t REM_FAULT      :1;
        uint16_t                :1;
        uint16_t NP_ABLE        :1;
    };
} __ANADbits_t;     // reg 4: PHY_REG_ANAD

#define _ANAD_PROT_SEL_MASK       0x001F
#define _ANAD_BASE10T_MASK        0x0020
#define _ANAD_BASE10T_FDX_MASK    0x0040
#define _ANAD_BASE100TX_MASK      0x0080
#define _ANAD_BASE100TX_FDX_MASK  0x0100
#define _ANAD_BASE100T4_MASK      0x0200
#define _ANAD_PAUSE_MASK          0x0400
#define _ANAD_ASM_DIR_MASK        0x0800
#define _ANAD_REM_FAULT_MASK      0x2000
#define _ANAD_NP_ABLE_MASK        0x8000

#define _ANAD_NEGOTIATION_MASK      (_ANAD_BASE10T_MASK | _ANAD_BASE10T_FDX_MASK | \
                                    _ANAD_BASE100TX_MASK|_ANAD_BASE100TX_FDX_MASK| \
                                    _ANAD_BASE100T4_MASK)   // negotiation field mask
#define _ANAD_CAPABLITY_POS         5 // capability field position in Auto-Neg Adv register
#define _ANAD_NEGOTIATION_LENGTH    5 // negotiation field length


typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t PROT_SEL       :5;
        uint16_t BASE10T        :1;
        uint16_t BASE10T_FDX    :1;
        uint16_t BASE100TX      :1;
        uint16_t BASE100TX_FDX  :1;
        uint16_t BASE100T4      :1;
        uint16_t PAUSE          :1;
        uint16_t ASM_DIR        :1;
        uint16_t                :1;
        uint16_t REM_FAULT      :1;
        uint16_t ACK            :1;
        uint16_t NP_ABLE        :1;
    };
} __ANLPADbits_t;   // reg 5: PHY_REG_ANLPAD

#define _ANLPAD_PROT_SEL_MASK       0x001F
#define _ANLPAD_BASE10T_MASK        0x0020
#define _ANLPAD_BASE10T_FDX_MASK    0x0040
#define _ANLPAD_BASE100TX_MASK      0x0080
#define _ANLPAD_BASE100TX_FDX_MASK  0x0100
#define _ANLPAD_BASE100T4_MASK      0x0200
#define _ANLPAD_PAUSE_MASK          0x0400
#define _ANLPAD_ASM_DIR_MASK        0x0800
#define _ANLPAD_REM_FAULT_MASK      0x2000
#define _ANLPAD_ACK_MASK            0x4000
#define _ANLPAD_NP_ABLE_MASK        0x8000

typedef union
{
    uint16_t    w;
    struct
    {    
        uint16_t MESSAGE:11;
        uint16_t TOGGLE:1;      
        uint16_t ACK2:1;
        uint16_t MSGP:1;
        uint16_t ACK:1;
        uint16_t NP:1;
    };
} __ANLPADNPbits_t; // reg 5: PHY_REG_ANLPADNP: next page

#define _ANLPADNP_MESSAGE_MASK  0x07FF
#define _ANLPADNP_TOGGLE_MASK   0x0800
#define _ANLPADNP_ACK2_MASK     0x1000
#define _ANLPADNP_MSGP_MASK     0x2000
#define _ANLPADNP_ACK_MASK      0x4000
#define _ANLPADNP_NP_MASK       0x8000


typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t LP_AN_ABLE  :1;
        uint16_t PAGE_RX     :1;
        uint16_t NP_ABLE     :1;
        uint16_t LP_NP_ABLE  :1;
        uint16_t PDF         :1;
        uint16_t             :11;
    };
} __ANEXPbits_t;    // reg 6: PHY_REG_ANEXP

#define _ANEXP_LP_AN_ABLE_MASK  0x0001
#define _ANEXP_PAGE_RX_MASK     0x0002
#define _ANEXP_NP_ABLE_MASK     0x0004
#define _ANEXP_LP_NP_ABLE_MASK  0x0008
#define _ANEXP_PDF_MASK         0x0010

typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t MESSAGE  :11;
        uint16_t TOGGLE   :1;
        uint16_t ACK2     :1;
        uint16_t MSGP     :1;
        uint16_t          :1;
        uint16_t NP       :1;
    };
} __ANNPTRbits_t;   // reg 7: PHY_REG_ANNPTR

#define _ANNPTR_MESSAGE_MASK  0x07FF
#define _ANNPTR_TOGGLE_MASK   0x0800
#define _ANNPTR_ACK2_MASK     0x1000
#define _ANNPTR_MSGP_MASK     0x2000
#define _ANNPTR_NP_MASK       0x8000

typedef union
{
    uint16_t    w;
    struct
    {
        uint16_t MESSAGE  :11;
        uint16_t TOGGLE   :1;
        uint16_t ACK2     :1;
        uint16_t MSGP     :1;
        uint16_t ACK      :1;
        uint16_t NP       :1;
    };
} __ANLPRNPbits_t;  // reg 8: PHY_REG_ANLPRNP

#define _ANLPRNP_MESSAGE_MASK  0x07FF
#define _ANLPRNP_TOGGLE_MASK   0x0800
#define _ANLPRNP_ACK2_MASK     0x1000
#define _ANLPRNP_MSGP_MASK     0x2000
#define _ANLPRNP_ACK_MASK      0x4000
#define _ANLPRNP_NP_MASK       0x8000

#define _EXTSTAT_BASE1000X_FDX_MASK 0x8000
#define _EXTSTAT_BASE1000X_HDX_MASK 0x4000
#define _EXTSTAT_BASE1000T_FDX_MASK 0x2000
#define _EXTSTAT_BASE1000T_HDX_MASK 0x1000

#define _1000BASE_TCTRL_TFDX_MASK 0x0200
#define _1000BASE_TCTRL_THDX_MASK 0x0100

#define _1000BSTAT_1000T_FDX_MASK      0x0800
#define _1000BSTAT_1000T_HDX_MASK      0x0400


#define KSZ9031_PHYID1          (0x0022)
#define KSZ9031_PHYID2_ID_NUM   (0x1400)
#define KSZ9031_PHYID2_MOD_NUM  (0x0220)
#define KSZ9031_PHYID2          (KSZ9031_PHYID2_ID_NUM | KSZ9031_PHYID2_MOD_NUM)

#endif  // _DRV_EXTPHY_REGS_H_
