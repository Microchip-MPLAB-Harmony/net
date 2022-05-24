/*******************************************************************************
  KSZ9031 definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_KSZ9131.h

  Summary:
    KSZ9131 definitions

  Description:
    This file provides the KSZ9131 definitions.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

// DOM-IGNORE-END

#ifndef _KSZ9131_H_

#define _KSZ9131_H_

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
	PHY_MMD_ACCESS_CONTROL = 13,
	PHY_MMD_ACCESS_DATA_ADDR = 14,
	// specific vendor registers: 16-31
	PHY_REG_RESERVED	= 16,
	PHY_REG_REMOTE_LPBK	= 17,
	PHY_REG_LINKMD_DIAG	= 18,
    PHY_REG_DIG_PMA	= 19,
	PHY_REG_RX_ERR_CNT	= 21,
    PHY_REG_LED_MODE	= 22,
    PHY_REG_LED_BEHV	= 23,
	PHY_REG_MDIO_DRV	= 25,
    PHY_REG_KSZ9031_LED_MODE	= 26,
    PHY_REG_INT_CTRL	= 27,
    PHY_REG_AUTO_MDIX	= 28,
	PHY_REG_SOFT_POWER_DOWN	= 29,
	PHY_REG_EXT_LPBK	= 30,
	PHY_REG_PHY_CTRL	= 31,
	//
	//PHY_REGISTERS		= 32	// total number of registers
}ePHY_VENDOR_REG;
// updated version of ePHY_REG



// MMD ( MDIO Managable Devices )
typedef union {
  struct {	      
    unsigned MMD_DEV_ADDR:5;
    unsigned :9;
    unsigned MMD_FUNCTION:2;
  };
  struct {
    unsigned short w:16;
  };
} __MMDACCESSCONTROLbits_t;	// Index 13: 

typedef enum
{	// MMD Register under Device Address 2
    PHY_MMD_CLK_SKEW_REG = 8,
	PHY_MMD_TX_DLL_REG	 = 77,
}PHY_MMD_DEV2_REG;

#define _PHY_MMD_CNTL_ACCESS_ADDRESS_MASK  0x0000 // 15 and 14th is 00
#define _PHY_MMD_CNTL_ACCESS_DATA_MASK  0x4000 // 15 and 14 th is 01
#define _PHY_MMD_RESERVED_MASK 0x3FE0


#endif  // _KSZ9131_H_

