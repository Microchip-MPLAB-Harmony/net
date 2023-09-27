/*******************************************************************************
  KSZ8041 definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_KSZ8041.h

  Summary:
    KSZ8041 definitions

  Description:
    This file provides the KSZ8041 definitions.

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

#ifndef _KSZ8041_H_

#define _KSZ8041_H_

typedef enum
{
    /*
    // basic registers, across all registers: 0-8
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
    // specific vendor registers: 16-31
    PHY_REG_MII_CONTROL = 0x14,
    PHY_REG_RXER_COUNTER = 0x15,
    PHY_REG_INTERUPT = 0x1b,
    PHY_REG_PHY_CONTROL_1 = 0x1e,
    PHY_REG_PHY_CONTROL_2 = 0x1f,
    
    
    //
    //PHY_REGISTERS     = 32    // total number of registers
}KSZ8041_VENDOR_REG;


// vendor registers
//
typedef union {
  struct {    
    unsigned :6;
    unsigned PREAMBLE_10BASE_T: 1;
    unsigned PREAMBLE_100BASE_T: 1;
    unsigned :8;
  };
  struct {
    unsigned short w:16;
  };
} __KSZ8041_MiiControlBits_t;   



typedef union {
  struct {
    unsigned short rxerCount:16;
  };
} __KSZ8041_RXERCounterBits_t;  


typedef union {
  struct {    
    unsigned LINK_UP_INTERRUPT:1;
    unsigned REMOTE_FAULT_INTERRUPT:1;
    unsigned LINK_DOWN_INTERRUPT:1;
    unsigned LINK_PARTNER_ACK_INTERRUPT:1;
    unsigned PARALLEL_DETECT_FAULT_INTERRUPT:1;
    unsigned PAGE_RECEIVE_INTERRUPT:1;
    unsigned RECEIVE_ERROR_INTERRUPT:1;
    unsigned JABBER_INTERRUPT:1;
    unsigned LINK_UP_ENABLE:1;
    unsigned REMOTE_FAULT_ENABLE:1;
    unsigned LINK_DOWN_ENABLE:1;
    unsigned LINK_PARTNER_ACK_ENABLE:1;
    unsigned PARALLEL_DETECT_FAULT_ENABLE:1;
    unsigned PAGE_RECEIVE_ENABLE:1;
    unsigned RECEIVE_ERROR_ENABLE:1;
    unsigned JABBER_ENABLE:1;  };
  struct {
    unsigned short w:16;
  };
} __KSZ8041_InterruptBits_t;    

typedef union {
    struct {
        unsigned :7;
        unsigned REMOTE_LOOPBACK:1;
        unsigned :3;
        unsigned MDI_STATE:1;
        unsigned :1;
        unsigned POLARITY:1;
        unsigned LED_MODE:2;
    };
    struct {
        unsigned short w:16;
    };
} __KSZ8041_PhyControl1Bits_t;

typedef union {
    struct {
        unsigned DATA_SCRAMBLER : 1;
        unsigned SQE_TEST : 1;
        unsigned OPERATION_MODE: 3;
        unsigned ISOLATE : 1;
        unsigned PAUSE : 1;
        unsigned AUTO_NEG_COMPLETE : 1;
        unsigned JABBER : 1;
        unsigned INTERUPT_LEVEL : 1;
        unsigned POWER_SAVING: 1;
        unsigned FORCE_LINK : 1;
        unsigned ENERGY_DETEC : 1;
        unsigned PAI_SWAP : 1;
        unsigned MDI_SELECT :1;
        unsigned HP_MDIX : 1;
    };
    struct {
        unsigned short w: 16;
    };
} __KSZ8041_PhyControl2Bits_t;



#endif  // _KSZ8041_H_

