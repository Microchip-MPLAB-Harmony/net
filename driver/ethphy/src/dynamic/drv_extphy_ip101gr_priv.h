/*******************************************************************************
  IP101GR private definitions API header file

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_ip101gr_priv.h

  Summary:
    IP101GR private definitions

  Description:
    This file provides the IP101GR private definitions.
    
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

#ifndef H_IP_101GR_PRIV_H_

#define H_IP_101GR_PRIV_H_

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
    // specific vendor registers: 16-31
    PHY_REG_PAGE_SEL = 20,
    PHY_REG_UTP_CONTROL_MODE = 16,
    PHY_REG_INTR_STATUS = 17,
    PHY_REG_UTP_INT_CONTROL_STATUS = 18,
    PHY_REG_DIGITAL_IO_SPCF_CNTL = 29,
    PHY_REG_MDIX_CNTL_SPFC_STATUS = 30,
    PHY_REG_FORCE_LINK_CONTROL = 17, // page 1
    PHY_REG_LED_CNTRL = 16,  // page 3
    PHY_REG_WOL_CNTRL = 16,  // page 4
    PHY_REG_WOL_MAC_ADDR = 16, // page 5
    PHY_REG_WOL_STATUS = 17, // page 17
    
    
}IP101GR_VENDOR_REG;


// vendor registers
//

typedef union 
{
    struct __attribute__((packed))
    {    
        unsigned ANALOG_OFF:1;
        unsigned LDPS_EN:1;
        unsigned REPEATER_MODE:1;
        unsigned :2;
        unsigned BYPASS_DSP_RESET:1;
        unsigned :1;
        unsigned NWAY_PSAVE_DIS:1;  
        unsigned JABBER_EN:1;
        unsigned FEF_DIS:1;
        unsigned :1;
        unsigned AUTO_MDIX:1;
        unsigned RMII_V12:1;
        unsigned RMII_V10:1;
        unsigned :2;
    };
    unsigned short w;
} IP101GR_MODECTRL_UTP_bits_t;    // page 16, reg 16: PHY_REG_UTP_CONTROL_MODE
#define M_UTP_MODECTRL_ANALOG_OFF                0x0001U
#define M_UTP_MODECTRL_LDPS_MASK                 0x0002U
#define M_UTP_MODECTRL_REPEATER_MODE_MASK        0x0004U
#define M_UTP_MODECTRL_BYPASS_DSP_RESET_MASK     0x0020U
#define M_UTP_MODECTRL_NWAY_PWSAVE_MASK          0x0080U
#define M_UTP_MODECTRL_FEF_DIS_MASK              0x0100U
#define M_UTP_MODECTRL_JABBER_EN_MASK            0x0200U
#define M_UTP_MODECTRL_AUTO_MDIX_MASK            0x0800U
#define M_UTP_MODECTRL_RMII_V12_MASK             0x1000U
#define M_UTP_MODECTRL_RMII_V10_MASK             0x2000U

typedef union 
{
    struct __attribute__((packed))
    {   
        unsigned LINK_CHANGE:1;
        unsigned DUPLEX_CHANGE:1;
        unsigned SPEED_CHANGE:1;
        unsigned INTR_STATUS:1;
        unsigned :4;
        unsigned LINK_MASK:1;
        unsigned DUPLEX_MASK:1;
        unsigned SPEED_MASK:1;
        unsigned ALL_MASK:1;
        unsigned :3;
        unsigned INTR:1;
    };
    unsigned short w;
} IP101GR_INTERRUPTSTATUSbits_t;  // page 16 reg 17: PHY_REG_INTR_STATUS
#define M_INTERRUPTSTATUS_INTR_MASK              0x8000U
#define M_INTERRUPTSTATUS_INTR_STATUS_MASK       0x0008U


typedef union 
{
    struct __attribute__((packed))
    {    
        unsigned ARBIT_STATE:4;
        unsigned :3;
        unsigned JABBER:1;
        unsigned POLARITY:1;
        unsigned MDIX:1;
        unsigned LINK_UP:1;
        unsigned REOSOLVED_AUTONEG:1;
        unsigned :1;
        unsigned RESOLVED_DUPLEX:1;
        unsigned RESOLVED_SPEED:1;
        unsigned :1;
    };
    unsigned short w;
} IP101GR_UTP_INTERRUPT_CONTR_STATUSbits_t;   // page 16 reg 18: PHY_REG_UTP_INT_CONTROL_STATUS
#define M_UTP_INTERRUPT_CONTR_STATUS_MDIX_MASK      0x0020U
#define M_UTP_INTERRUPT_CONTR_STATUS_LINKUP_MASK    0x0040U


typedef union 
{
    struct __attribute__((packed))
    {    
        unsigned OP_MODE:3;
        unsigned FORCE_MDIX:1;
        unsigned :4;
        unsigned LINK_UP:1;
        unsigned :7;
    };
    unsigned short w;
} IP101GR_MDIX_CONTR_STATUSbits_t;    // page 16 reg 30: PHY_REG_MDIX_CNTL_SPFC_STATUS
#define M_MDIX_CONTR_STATUS_FORCEMDIX_MASK      0x0008U

typedef union 
{
    struct __attribute__((packed))
    {    
        unsigned :7;
        unsigned FORCE_LINK_100:1;
        unsigned FORCE_LINK_10:1;
        unsigned :7;
    };
    unsigned short w;
} IP101GR_FORCE_LINK_STATUSbits_t;    // page 1 reg 17: PHY_REG_FORCE_LINK_CONTROL
#define M_FORCE_LINK_STATUS_LINK100_MASK        0x0008U
#define M_FORCE_LINK_STATUS_LINK10_MASK         0x0010U



typedef union 
{
    struct __attribute__((packed))
    {   
        unsigned :5;
        unsigned WOL_PLUS_MAN_SET:1;
        unsigned WOL_PLUS_TIMER_SEL:2;
        unsigned WOL_DOWN_SPEED_EN:1;
        unsigned SENSE_DUT:1;
        unsigned SENSE_ANY_PKT:1;
        unsigned SENSE_MAGIC_PKT:1;
        unsigned :1;
        unsigned INTR_STATUS:1;
        unsigned WOL_MASTER_SLAVE:1;
        unsigned WOL_EN:1;
    };
    unsigned short w;
} IP101GR_WOL_CNTRLbits_t;    // page 4 reg 16: PHY_REG_WOL_CNTRL
#define M_WOL_EN_MASK                            0x8000U
#define M_WOL_PLUS_MASTER_SLAVE_MASK             0x4000U
#define M_WOL_INTR_ACTIVE_HIGH_MASK              0x2000U
#define M_WOL_SENSE_MAGIC_PKT_MASK               0x0800U
#define M_WOL_SENSE_ANY_PKT_MASK                 0x0400U
#define M_WOL_PLUS_SENSE_DUT_MASK                0x0200U
#define M_WOL_PLUS_DOWN_SPEED_MASK               0x0100U
#define M_WOL_PLUS_TIMER_10MIN_SEL_MASK          0x00C0U
#define M_WOL_PLUS_TIMER_3MIN_SEL_MASK           0x0060U
#define M_WOL_PLUS_TIMER_30SEC_SEL_MASK          0x0000U
#define M_WOL_PLUS_MANUAL_SET_MASK               0x0020U



typedef union 
{
    struct __attribute__((packed))
    {   
        unsigned WOL_PLUS_WAKE_STATUS:1;
        unsigned WOL_PLUS_SLEEP:1;
        unsigned WOL_PLUS_SLEEPING_STATUS:1;
        unsigned WOL_PLUS_INTR_STATUS:1;
        unsigned :11;
        unsigned WOL_PLUS_INTR_DIS:1;
    };
    unsigned short w;
} IP101GR_WOL_STATUSbits_t;   // page 17 reg 17: PHY_REG_WOL_STATUS
#define M_WOL_PLUS_WAKE_STATUS_MASK              0x0001U
#define M_WOL_PLUS_SLEEP_MASK                    0x0002U
#define M_WOL_PLUS_SLEEPING_STATUS_MASK          0x0004U
#define M_WOL_PLUS_INTR_STATUS_MASK              0x0008U
#define M_WOL_PLUS_INTR_DIS_MASK                 0x8000U


typedef enum
{
    PAGENUM_0=0,
    PAGENUM_1=1,
    PAGENUM_2=2,
    PAGENUM_3=3,
    PAGENUM_4=4,
    PAGENUM_5=5,
    PAGENUM_16=16,
    PAGENUM_17=17,    
}IP101GR_PAGENUMBERSEL;

typedef enum 
{   
    WOL_NORMAL=1,  
    WOL_SLEEPING,
    WOL_WAKEUP,
    WOL_RDY4SLP,
    WOL_RDY4WAKE,
}IP101GR_WOL_STATE;

typedef enum
{
    WOL_MODE_MASTER =1,
    WOL_MODE_SLAVE = 0,
}IP101GR_WOL_OPERATION_MODE;

typedef enum
{
    IP101GR_WOL_DISABLE=0,
    IP101GR_WOL_ENBALE=1,
}IP101GR_WOL_FUNCTION;

typedef enum
{
    WOL_TIMER_30SEC=0,
    WOL_TIMER_10MIN,
    WOL_TIMER_3MIN,
}IP101GR_WOL_TIMER;

#define IP101GR_INTR_PIN_32_ENABLE      1U
#define IP101GR_INTR_PIN_32_DISABLE     0U


#endif  // H_IP_101GR_PRIV_H_

