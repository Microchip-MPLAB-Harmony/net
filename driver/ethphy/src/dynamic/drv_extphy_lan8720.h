/*******************************************************************************
  LAN8720 definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_lan8720.h

  Summary:
    LAN8720 definitions

  Description:
    This file provides the LAN8720 definitions.

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

#ifndef _LAN_8720_H_

#define _LAN_8720_H_

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
    // PHY_REG_ANNPTR       = 7,    not defined for LAN8720
    // PHY_REG_ANLPRNP      = 8,    not defined for LAN8720
    */
    // specific vendor registers: 16-31
    PHY_REG_MODE_CTRL       = 17,
    PHY_REG_SPECIAL_MODE    = 18,
    PHY_REG_SYMBOL_ERR_CNT  = 26,
    PHY_REG_SPECIAL_CTRL    = 27,
    PHY_REG_INT_SOURCE      = 29,
    PHY_REG_INT_MASK        = 30,
    PHY_REG_PHY_CTRL        = 31,
    //
    //PHY_REGISTERS     = 32    // total number of registers
}LAN8720_VENDOR_REG;


// vendor registers
//

typedef union {
  struct {
    unsigned               :1;
    unsigned ENERGYON      :1;
    unsigned               :4;
    unsigned ALTINT        :1;
    unsigned               :2;
    unsigned FAR_LOOPBACK  :1;
    unsigned               :3;
    unsigned EDPWRDOWN     :1;
    unsigned               :2;
  };
  struct {
    unsigned short w       :16;
  };
} __MODECTRLbits_t; // reg 17: PHY_REG_MODE_CTRL
#define _MODECTRL_ENERGYON_MASK     0x0002
#define _MODECTRL_ALTINT_MASK       0x0040
#define _MODECTRL_FAR_LOOPBACK_MASK 0x0200
#define _MODECTRL_EDPWRDOWN_MASK    0x2000


typedef union {
  struct {
    unsigned PHYAD    :5;
    unsigned MODE     :3;
    unsigned          :6;
    unsigned          :1;
    unsigned          :1;
  };
  struct {
    unsigned short w  :16;
  };
} __SPECIALMODEbits_t;  // reg 18: PHY_REG_SPECIAL_MODE
#define _SPECIALMODE_PHYAD_MASK     0x001f
#define _SPECIALMODE_MODE_MASK      0x00e0




typedef union {
  struct {
    unsigned Sym_Err_Cnt:16;
  };
  struct {
    unsigned short w:16;
  };
} __SYMBOLERRCNTbits_t; // reg 26: PHY_REG_SYMBOL_ERR_CNT


typedef union {
  struct {
    unsigned            :4;
    unsigned XPOL       :1;
    unsigned            :6;
    unsigned            :2;
    unsigned CH_SELECT  :1;
    unsigned            :1;
    unsigned AMDIXCTRL  :1;
  };
  struct {
    unsigned short w    :16;
  };
} __SPECIALCTRLbits_t;  // reg 27: PHY_REG_SPECIAL_CTRL
#define _SPECIALCTRL_XPOL_MASK      0x0010
#define _SPECIALCTRL_CH_SELECT_MASK 0x2000
#define _SPECIALCTRL_AMDIXCTRL_MASK 0x8000




typedef union {
  struct {
    unsigned          :1;
    unsigned INT1     :1;
    unsigned INT2     :1;
    unsigned INT3     :1;
    unsigned INT4     :1;
    unsigned INT5     :1;
    unsigned INT6     :1;
    unsigned INT7     :1;
    unsigned          :8;
  };
  struct {
    unsigned short w  :16;
  };
} __INTSOURCEbits_t;    // reg 29: PHY_REG_INT_SOURCE

typedef union {
  struct {
    unsigned          :1;
    unsigned INT1     :1;
    unsigned INT2     :1;
    unsigned INT3     :1;
    unsigned INT4     :1;
    unsigned INT5     :1;
    unsigned INT6     :1;
    unsigned INT7     :1;
    unsigned          :8;
  };
  struct {
    unsigned short w  :16;
  };
} __INTMASKbits_t;  // reg 30: PHY_REG_INT_MASK


typedef union {
  struct {
    unsigned           :2;
    unsigned SPEED     :3;   // 1: 10MbpsHD; 5:10MbpsFD; 2: 100MbpsHD; 6: 100MbpsFD;
    unsigned           :1;
    unsigned           :1;  // ENABLE_4B5B: should be always set
    unsigned           :3;        // GPO: N/A
    unsigned           :2;
    unsigned AUTODONE  :1;
    unsigned           :3;
  };
  struct {
    unsigned short w   :16;
  };
} __PHYCTRLbits_t;  // reg 31: PHY_REG_PHY_CTRL
#define _PHYCTRL_SPEED_MASK         0x001c
#define _PHYCTRL_SPEED_FDUPLX_MASK  0x0010
#define _PHYCTRL_SPEED_100_10_MASK  0x000c
#define _PHYCTRL_SPEED_100_MASK     0x0008
#define _PHYCTRL_AUTODONE_MASK      0x1000



#endif  // _LAN_8720_H_

