/*******************************************************************************
  IP101GR definitions API header file

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_ip101gr.h

  Summary:
    IP101GR definitions

  Description:
    This file provides the IP101GR definitions.
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _IP_101GR_H_

#define _IP_101GR_H_

typedef enum
{
	/*
	// basic registers, accross all registers: 0-1
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
    
	
}ePHY_VENDOR_REG;
// updated version of ePHY_REG


// vendor registers
//

typedef union {
  struct {    
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
  struct {
    unsigned short w:16;
  };
} __MODECTRL_UTP_bits_t;	// page 16, reg 16: PHY_REG_UTP_CONTROL_MODE
#define	_UTP_MODECTRL_ANALOG_OFF	            0x0001
#define	_UTP_MODECTRL_LDPS_MASK		            0x0002
#define	_UTP_MODECTRL_REPEATER_MODE_MASK		0x0004
#define	_UTP_MODECTRL_BYPASS_DSP_RESET_MASK	    0x0020
#define	_UTP_MODECTRL_NWAY_PWSAVE_MASK	        0x0080
#define	_UTP_MODECTRL_FEF_DIS_MASK	            0x0100
#define	_UTP_MODECTRL_JABBER_EN_MASK	        0x0200
#define	_UTP_MODECTRL_AUTO_MDIX_MASK	        0x0800
#define	_UTP_MODECTRL_RMII_V12_MASK	            0x1000
#define	_UTP_MODECTRL_RMII_V10_MASK	            0x2000

typedef union {
  struct {   
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
  struct {
    unsigned short w:16;
  };
} __INTERRUPTSTATUSbits_t;	// page 16 reg 17: PHY_REG_INTR_STATUS
#define	_INTERRUPTSTATUS_INTR_MASK		        0x8000
#define	_INTERRUPTSTATUS_INTR_STATUS_MASK		0x0008


typedef union {
  struct {    
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
  struct {
    unsigned short w:16;
  };
} __UTP_INTERRUPT_CONTR_STATUSbits_t;	// page 16 reg 18: PHY_REG_UTP_INT_CONTROL_STATUS
#define	_UTP_INTERRUPT_CONTR_STATUS_MDIX_MASK		        0x0020
#define	_UTP_INTERRUPT_CONTR_STATUS_LINKUP_MASK		        0x0040


typedef union {
  struct {    
    unsigned OP_MODE:3;
    unsigned FORCE_MDIX:1;
    unsigned :4;
    unsigned LINK_UP:1;
    unsigned :7;
  };
  struct {
    unsigned short w:16;
  };
} __MDIX_CONTR_STATUSbits_t;	// page 16 reg 30: PHY_REG_MDIX_CNTL_SPFC_STATUS
#define	_MDIX_CONTR_STATUS_FORCEMDIX_MASK		        0x0008

typedef union {
  struct {    
    unsigned :7;
    unsigned FORCE_LINK_100:1;
    unsigned FORCE_LINK_10:1;
    unsigned :7;
  };
  struct {
    unsigned short w:16;
  };
} __FORCE_LINK_STATUSbits_t;	// page 1 reg 17: PHY_REG_FORCE_LINK_CONTROL
#define	_FORCE_LINK_STATUS_LINK100_MASK		        0x0008
#define	_FORCE_LINK_STATUS_LINK10_MASK		        0x0010



typedef union {
  struct {   
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
  struct {
    unsigned short w:16;
  };
} __WOL_CNTRLbits_t;	// page 4 reg 16: PHY_REG_WOL_CNTRL
#define	_WOL_EN_MASK		                    0x8000
#define	_WOL_PLUS_MASTER_SLAVE_MASK		        0x4000
#define _WOL_INTR_ACTIVE_HIGH_MASK              0x2000
#define _WOL_SENSE_MAGIC_PKT_MASK               0x0800
#define _WOL_SENSE_ANY_PKT_MASK                 0x0400
#define _WOL_PLUS_SENSE_DUT_MASK                     0x0200
#define _WOL_PLUS_DOWN_SPEED_MASK                    0x0100
#define _WOL_PLUS_TIMER_10MIN_SEL_MASK          0x00C0
#define _WOL_PLUS_TIMER_3MIN_SEL_MASK           0x0060
#define _WOL_PLUS_TIMER_30SEC_SEL_MASK          0x0000
#define _WOL_PLUS_MANUAL_SET_MASK               0x0020



typedef union {
  struct {   
    unsigned WOL_PLUS_WAKE_STATUS:1;
    unsigned WOL_PLUS_SLEEP:1;
    unsigned WOL_PLUS_SLEEPING_STATUS:1;
    unsigned WOL_PLUS_INTR_STATUS:1;
    unsigned :11;
    unsigned WOL_PLUS_INTR_DIS:1;
  };
  struct {
    unsigned short w:16;
  };
} __WOL_STATUSbits_t;	// page 17 reg 17: PHY_REG_WOL_STATUS
#define	_WOL_PLUS_WAKE_STATUS_MASK              0x0001
#define	_WOL_PLUS_SLEEP_MASK		            0x0002
#define _WOL_PLUS_SLEEPING_STATUS_MASK             0x0004
#define _WOL_PLUS_INTR_STATUS_MASK              0x0008
#define _WOL_PLUS_INTR_DIS_MASK                 0x8000


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
}ePAGENUMBERSEL;

typedef enum 
{   
    WOL_NORMAL=1,  
    WOL_SLEEPING,
    WOL_WAKEUP,
    WOL_RDY4SLP,
    WOL_RDY4WAKE,
}eWOL_STATE;

typedef enum
{
    WOL_MODE_MASTER	=1,
    WOL_MODE_SLAVE = 0,
}WOL_OPERATION_MODE;

typedef enum
{
    IP101GR_WOL_DISABLE=0,
    IP101GR_WOL_ENBALE=1,
}WOL_FUNCTION;

typedef enum
{
    WOL_TIMER_30SEC=0,
    WOL_TIMER_10MIN,
    WOL_TIMER_3MIN,
}IP101GR_WOL_TIMER;

#define IP101GR_INTR_PIN_32_ENABLE      1
#define IP101GR_INTR_PIN_32_DISABLE     0


#endif	// _IP_101GR_H_

