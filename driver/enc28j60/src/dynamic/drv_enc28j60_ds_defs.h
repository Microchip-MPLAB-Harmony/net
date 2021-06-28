/*******************************************************************************
 ENC28J60 Data Sheet Definitions.
  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_enc28j60_ds_defs.h
  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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

#ifndef _DRV_ENC28J60_DS_DEF_H_
#define _DRV_ENC28J60_DS_DEF_H_

#include "system_config.h"

#define DRV_ENC28J60_MEM_SIZE 8192

typedef struct _DRV_ENC28J60_RD_ECON1
{
	uint8_t BSEL:2;
	uint8_t RXEN:1;
	uint8_t TXRTS:1;
	uint8_t CSUMEN:1;
	uint8_t DMAST:1;
	uint8_t RXRST:1;
	uint8_t TXRST:1;
}  DRV_ENC28J60_RD_ECON1;


typedef struct _DRV_ENC28J60_RD_ECON2
{
	uint8_t :3;
	uint8_t VRPS:1;
	uint8_t :1;
	uint8_t PWRSV:1;
	uint8_t PKTDEC:1;
	uint8_t AUTOINC:1;
}  DRV_ENC28J60_RD_ECON2;

typedef struct _DRV_ENC28J60_RD_EDMACS
{
	uint16_t EDMACS;
}  DRV_ENC28J60_RD_EDMACS;

typedef struct _DRV_ENC28J60_RD_ERDPT
{
	uint16_t ERDPT:13;
	uint16_t :3;
}  DRV_ENC28J60_RD_ERDPT;

typedef struct _DRV_ENC28J60_RD_EWRPT
{
	uint16_t EWRPT:13;
	uint16_t :3;
}  DRV_ENC28J60_RD_EWRPT;

typedef struct _DRV_ENC28J60_RD_ETXND
{
	uint16_t ETXND:13;
	uint16_t :3;
}  DRV_ENC28J60_RD_ETXND;

typedef struct _DRV_ENC28J60_RD_ERXND
{
	uint16_t ERXND:13;
	uint16_t :3;
}  DRV_ENC28J60_RD_ERXND;


typedef struct _DRV_ENC28J60_RD_EDMAND
{
	uint16_t EDMAND:13;
	uint16_t :3;
}  DRV_ENC28J60_RD_EDMAND;

typedef struct _DRV_ENC28J60_RD_EDMADST
{
	uint16_t EDMADST:13;
	uint16_t :3;
}  DRV_ENC28J60_RD_EDMADST;


typedef struct _DRV_ENC28J60_RD_EDMAST
{
	uint16_t EDMAST:13;
	uint16_t :3;
}  DRV_ENC28J60_RD_EDMAST;



typedef struct _DRV_ENC28J60_RD_EHT0
{
	uint8_t EHT0;
}  DRV_ENC28J60_RD_EHT0;


typedef struct _DRV_ENC28J60_RD_EHT1
{
	uint8_t EHT1;
}  DRV_ENC28J60_RD_EHT1;


typedef struct _DRV_ENC28J60_RD_EHT2
{
	uint8_t EHT2;
}  DRV_ENC28J60_RD_EHT2;


typedef struct _DRV_ENC28J60_RD_EHT3
{
	uint8_t EHT3;
}  DRV_ENC28J60_RD_EHT3;


typedef struct _DRV_ENC28J60_RD_EHT4
{
	uint8_t EHT4;
}  DRV_ENC28J60_RD_EHT4;


typedef struct _DRV_ENC28J60_RD_EHT5
{
	uint8_t EHT5;
}  DRV_ENC28J60_RD_EHT5;


typedef struct _DRV_ENC28J60_RD_EHT6
{
	uint8_t EHT6;
}  DRV_ENC28J60_RD_EHT6;


typedef struct _DRV_ENC28J60_RD_EHT7
{
	uint8_t EHT7;
}  DRV_ENC28J60_RD_EHT7;

typedef struct _DRV_ENC28J60_RD_EIE
{
	/**
	 * <b>RXERIE: </b>Receive Error Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t RXERIE:1;
	/**
	 * <b>TXERIE: </b>Transmit Error Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t TXERIE:1;
    /**
	 * <b>Reserved: </b>Ignore on read, don't care on write<b>(1)</b>
	 */
	uint8_t :1;
	/**
	 * <b>TXIE: </b>Transmit Done Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t TXIE:1;
	/**
	 * <b>LINKIE: </b>PHY Link Status Change Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t LINKIE:1;
	/**
	 * <b>DMAIE: </b>DMA Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t DMAIE:1;
	/**
	 * <b>PKTIE: </b>RX Packet Pending Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t PKTIE:1;
	/**
	 * <b>INTIE: </b>INT Global Interrupt Enable bit
	 * 1 = INT pin is controlled by the INT status bit (ESTAT<15>)
	 * 0 = INT pin is driven high
	 */
	uint8_t INTIE:1;
}  DRV_ENC28J60_RD_EIE;


typedef struct _DRV_ENC28J60_RD_EIR
{

	
	/**
	 * <b>RXERIF: </b>Receive Error Interrupt Flag bit
	 * 1 = A packet was aborted because there is insufficient buffer space 
	 * or the packet count is 255
	 * 0 = No receive error interrupt is pending
	 */
	uint8_t RXERIF:1;
	/**
	 * <b>TXERIF: </b>Transmit Error Interrupt Flag bit
	 * 1 = A transmit error has occurred 
	 * 0 = No transmit error has occurred
	 */
	uint8_t TXERIF:1;
	/**
	* <b>Reserved: </b>Ignore on read, don't care on write<b>(1)</b>
	*/
   	uint8_t :1;
	/**
	 * <b>TXIF:: </b>Transmit Interrupt Flag bit
	 * 1 = Transmit request has ended
	 * 0 = No transmit interrupt is pending
	 */
	uint8_t TXIF:1;
	/**
	 * <b>LINKIF:: </b>Link Change Interrupt Flag bit
	 * 1 = PHY reports that the link status has changed; read PHIR register to clear
	 * 0 = Link status has not changed
	 */
	uint8_t LINKIF:1;
	/**
	 * <b>DMAIF:: </b>DMA Interrupt Flag bit
	 * 1 = DMA copy or checksum calculation has completed
	 * 0 = No DMA interrupt is pending
	 */
	uint8_t DMAIF:1;
	/**
	 * <b>PKTIF:: </b>Receive Packet Pending Interrupt Flag bit
	 * 1 = Receive buffer contains one or more unprocessed packets; cleared when PKTDEC is set
	 * 0 = Receive buffer is empty
	 */
	uint8_t PKTIF:1;
	
	/**
	* <b>Unimplemented: </b>Unimplemented bit. Read as '0')</b>
	*/
	uint8_t :1;
}  DRV_ENC28J60_RD_EIR;


typedef struct _DRV_ENC28J60_RD_EPAUS
{
	uint16_t EPAUS;
}  DRV_ENC28J60_RD_EPAUS;


typedef struct _DRV_ENC28J60_RD_EPMCS
{
	uint16_t EPMCS;
}  DRV_ENC28J60_RD_EPMCS;


typedef struct _DRV_ENC28J60_RD_EPMM0
{
	uint8_t EPMM0;
}  DRV_ENC28J60_RD_EPMM0;

typedef struct _DRV_ENC28J60_RD_EPMM1
{
	uint8_t EPMM1;
}  DRV_ENC28J60_RD_EPMM1;

typedef struct _DRV_ENC28J60_RD_EPMM2
{
	uint8_t EPMM2;
}  DRV_ENC28J60_RD_EPMM2;

typedef struct _DRV_ENC28J60_RD_EPMM3
{
	uint8_t EPMM3;
}  DRV_ENC28J60_RD_EPMM3;

typedef struct _DRV_ENC28J60_RD_EPMM4
{
	uint8_t EPMM4;
}  DRV_ENC28J60_RD_EPMM4;

typedef struct _DRV_ENC28J60_RD_EPMM5
{
	uint8_t EPMM5;
}  DRV_ENC28J60_RD_EPMM5;

typedef struct _DRV_ENC28J60_RD_EPMM6
{
	uint8_t EPMM6;
}  DRV_ENC28J60_RD_EPMM6;

typedef struct _DRV_ENC28J60_RD_EPMM7
{
	uint8_t EPMM7;
}  DRV_ENC28J60_RD_EPMM7;


typedef struct _DRV_ENC28J60_RD_EPMO
{
	uint16_t EPMO:13;
	uint16_t :3;
}  DRV_ENC28J60_RD_EPMO;

typedef struct _DRV_ENC28J60_RD_ERXFCON
{
	uint8_t BCEN:1;
	uint8_t MCEN:1;
	uint8_t HTEN:1;
	uint8_t MPEN:1;
	uint8_t PMEN:1;
	uint8_t CRCEN:1;
	uint8_t ANDOR:1;
	uint8_t UCEN:1;
}  DRV_ENC28J60_RD_ERXFCON;


typedef struct _DRV_ENC28J60_RD_ERXRDPT
{
	uint16_t ERXRDPT:13;
	uint8_t :3;
}  DRV_ENC28J60_RD_ERXRDPT;


typedef struct _DRV_ENC28J60_RD_ERXST
{
	uint16_t ERXST:13;
	uint8_t :3;
}  DRV_ENC28J60_RD_ERXST;

typedef struct _DRV_ENC28J60_RD_ERXWRPT
{
	uint16_t ERXWRPT:13;
	uint8_t :3;
}  DRV_ENC28J60_RD_ERXWRPT;


typedef struct _DRV_ENC28J60_RD_ESTAT
{
	uint8_t CLKRDY:1;
	uint8_t TXABRT:1;
	uint8_t RXBUSY:1;
	uint8_t :1;
	uint8_t LATECOL:1;
	uint8_t :1;	
	uint8_t BUFER:1;
	uint8_t INT:1;
}  DRV_ENC28J60_RD_ESTAT;



typedef struct _DRV_ENC28J60_RD_ETXST
{
	uint16_t ETXST:13;
	uint8_t :3;
}  DRV_ENC28J60_RD_ETXST;

typedef struct _DRV_ENC28J60_RD_MAADR1
{
	uint8_t MAADR1;
}  DRV_ENC28J60_RD_MAADR1;

typedef struct _DRV_ENC28J60_RD_MAADR2
{
	uint8_t MAADR2;
}  DRV_ENC28J60_RD_MAADR2;

typedef struct _DRV_ENC28J60_RD_MAADR3
{
	uint8_t MAADR3;
}  DRV_ENC28J60_RD_MAADR3;

typedef struct _DRV_ENC28J60_RD_MAADR4
{
	uint8_t MAADR4;
} DRV_ENC28J60_RD_MAADR4;

typedef struct _DRV_ENC28J60_RD_MAADR5
{
	uint8_t MAADR5;
} DRV_ENC28J60_RD_MAADR5;

typedef struct _DRV_ENC28J60_RD_MAADR6
{
	uint8_t MAADR6;
} DRV_ENC28J60_RD_MAADR6;


typedef struct _DRV_ENC28J60_RD_MABBIPG
{
	uint8_t BBIPG:7;
	uint8_t :1;
}  DRV_ENC28J60_RD_MABBIPG;


typedef struct _DRV_ENC28J60_RD_MACLCON
{
	uint8_t RETMAX:4;
	uint8_t COLWIN:6;
}  DRV_ENC28J60_RD_MACLCON;


typedef struct _DRV_ENC28J60_RD_MACON1
{
	uint8_t MARXEN:1;
	uint8_t PASSALL:1;
	uint8_t RXPAUS:1;
	uint8_t TXPAUS:1;
	uint8_t :4;
}  DRV_ENC28J60_RD_MACON1;


typedef struct _DRV_ENC28J60_RD_MACON3
{
	uint8_t FULDPX:1;
	uint8_t FRMLNEN:1;
	uint8_t HFRMEN:1;
	uint8_t PHDREN:1;
	uint8_t TXCRCEN:1;
	uint8_t PADCFG:3;
}  DRV_ENC28J60_RD_MACON3;


typedef struct _DRV_ENC28J60_RD_MACON4
{
	uint8_t :4;
	uint8_t NOBKOFF:1;
	uint8_t BPEN:1;
	uint8_t DEFER:1;
	uint8_t :1;
}  DRV_ENC28J60_RD_MACON4;


typedef struct _DRV_ENC28J60_RD_MAIPG
{
	uint8_t IPGL:7;
	uint8_t IPGH:7;
}  DRV_ENC28J60_RD_MAIPG;


typedef struct _DRV_ENC28J60_RD_MAMXFL
{
	uint16_t MAMXFL;
}  DRV_ENC28J60_RD_MAMXFL;


typedef struct _DRV_ENC28J60_RD_MICMD
{
	uint8_t MIIRD:1;
	uint8_t MIISCAN:1;
	uint8_t :6;
}  DRV_ENC28J60_RD_MICMD;


typedef struct _DRV_ENC28J60_RD_MIRD
{
	uint16_t MIRD;
}  DRV_ENC28J60_RD_MIRD;


typedef struct _DRV_ENC28J60_RD_MIREGADR
{
	uint8_t MIREGADR:5;
}  DRV_ENC28J60_RD_MIREGADR;


typedef struct _DRV_ENC28J60_RD_MISTAT
{
	uint8_t BUSY:1;
	uint8_t SCAN:1;
	uint8_t NVALID:1;
	uint8_t :5;	
}  DRV_ENC28J60_RD_MISTAT;


typedef struct _DRV_ENC28J60_RD_MIWR
{
	uint16_t MIWR;
}  DRV_ENC28J60_RD_MIWR;

typedef struct _DRV_ENC28J60_RD_PHCON1
{
	uint16_t :8;
	uint16_t PDPXMD:1;
	uint16_t :2;
	uint16_t PPWRSV:1;
	uint16_t :2;
	uint16_t PLOOPBK:1;
	uint16_t PRST:1;
}  DRV_ENC28J60_RD_PHCON1;


typedef struct _DRV_ENC28J60_RD_PHCON2
{
	uint16_t :8;
	uint16_t HDLDIS:1;
	uint16_t :1;
	uint16_t JABBER:1;
	uint16_t :2;
	uint16_t TXDIS:1;
	uint16_t FRCLNK:1;
	uint16_t :1;
}  DRV_ENC28J60_RD_PHCON2;


typedef struct _DRV_ENC28J60_RD_PHSTAT1
{
	uint16_t :1;
	uint16_t JBSTAT:1;
	uint16_t LLSTAT:1;
	uint16_t :8;

	uint16_t PHDPX:1;
	uint16_t PFDPX:1;
	uint16_t :3;
	
}  DRV_ENC28J60_RD_PHSTAT1;


typedef struct _DRV_ENC28J60_RD_PHSTAT2
{
	uint16_t :5;
	uint16_t PLRITY:1;
	uint16_t :3;
	uint16_t DPXSTAT:1;
	uint16_t LSTAT:1;
	uint16_t COLSTAT:1;
	uint16_t RXSTAT:1;
	uint16_t TXSTAT:1;
	uint16_t :2;
}  DRV_ENC28J60_RD_PHSTAT2;

typedef struct _DRV_ENC28J60_RD_PHID1
{
	uint16_t PHID1;
}  DRV_ENC28J60_RD_PHID1;

typedef struct _DRV_ENC28J60_RD_PHID2
{
	uint16_t PHID2;
}  DRV_ENC28J60_RD_PHID2;

typedef struct _DRV_ENC28J60_RD_PHIE
{
	uint16_t :1;
	uint16_t PGEIE:1;
	uint16_t :2;
	uint16_t PLNKIE:1;
	uint16_t :11;
}  DRV_ENC28J60_RD_PHIE;


typedef struct _DRV_ENC28J60_RD_PHIR
{
	uint16_t :2;
	uint16_t PGIF:1;
	uint16_t :1;
	uint16_t PLNKIF:1;
	uint16_t :11;
}  DRV_ENC28J60_RD_PHIR;

typedef struct _DRV_ENC28J60_RD_PHLCON
{
	uint16_t :1;
	uint16_t STRCH:1;
	uint16_t LFRQ0:1;
	uint16_t LFRQ1:1;
	uint16_t LBCFG0:1;
	uint16_t LBCFG1:1;
	uint16_t LBCFG2:1;
	uint16_t LBCFG3:1;
	uint16_t LACFG0:1;
	uint16_t LACFG1:1;
	uint16_t LACFG2:1;
	uint16_t LACFG3:1;
	uint16_t :4;
}  DRV_ENC28J60_RD_PHLCON;

typedef struct _DRV_ENC28J60_RD_EREVID
{
	uint8_t EREVID:5;
	uint8_t :3;
}  DRV_ENC28J60_RD_EREVID;

typedef struct _DRV_ENC28J60_RD_ECOCON
{
	uint8_t COCON:3;
	uint8_t :5;
}  DRV_ENC28J60_RD_ECOCON;

typedef struct _DRV_ENC28J60_RD_EFLOCON
{
	uint8_t FCEN:2;
	uint8_t FULDPXS:1;
	uint8_t :5;
}  DRV_ENC28J60_RD_EFLOCON;

typedef struct _DRV_ENC28J60_RD_EBSTSD
{
	uint8_t EBSTSD;

}  DRV_ENC28J60_RD_EBSTSD;

typedef struct _DRV_ENC28J60_RD_EBSTCON
{
	uint8_t BISTST:1;
	uint8_t TME:1;
	uint8_t TMSEL:2;
	uint8_t PSEL:1;
	uint8_t PSV:3;

}  DRV_ENC28J60_RD_EBSTCON;

typedef struct _DRV_ENC28J60_RD_EBSTCS
{
	uint16_t EBSTCS;

}  DRV_ENC28J60_RD_EBSTCS;


typedef struct _DRV_ENC28J60_RD_EPKTCNT
{
	uint8_t EPKTCNT;

} DRV_ENC28J60_RD_EPKTCNT;

typedef union _DRV_ENC28J60_RegUnion
{
	uint16_t value;

	DRV_ENC28J60_RD_ERDPT erdpt;
	DRV_ENC28J60_RD_EWRPT ewrpt;
	DRV_ENC28J60_RD_ETXST etxst;
	DRV_ENC28J60_RD_ETXND etxnd;
	DRV_ENC28J60_RD_ERXST erxst;
	DRV_ENC28J60_RD_ERXND erxnd;
	DRV_ENC28J60_RD_ERXRDPT erxrdpt;
	DRV_ENC28J60_RD_ERXWRPT erxwrpt;
	DRV_ENC28J60_RD_EDMAST edmast;
	DRV_ENC28J60_RD_EDMAND edmand;
	DRV_ENC28J60_RD_EDMADST edmadst;
	DRV_ENC28J60_RD_EDMACS edmacs;
	DRV_ENC28J60_RD_ECON1 econ1;
	DRV_ENC28J60_RD_ECON2 econ2;
	DRV_ENC28J60_RD_EIR eir;
	DRV_ENC28J60_RD_EIE eie;
	DRV_ENC28J60_RD_ESTAT estat;
	DRV_ENC28J60_RD_EHT0 eht0;
	DRV_ENC28J60_RD_EHT1 eht1;
	DRV_ENC28J60_RD_EHT2 eht2;
	DRV_ENC28J60_RD_EHT3 eht3;
	DRV_ENC28J60_RD_EHT4 eht4;
	DRV_ENC28J60_RD_EHT5 eht5;
	DRV_ENC28J60_RD_EHT6 eht6;
	DRV_ENC28J60_RD_EHT7 eht7;
	DRV_ENC28J60_RD_EPMM0 epmm0;
	DRV_ENC28J60_RD_EPMM1 epmm1;
	DRV_ENC28J60_RD_EPMM2 epmm2;
	DRV_ENC28J60_RD_EPMM3 epmm3;
	DRV_ENC28J60_RD_EPMM4 epmm4;
	DRV_ENC28J60_RD_EPMM5 epmm5;
	DRV_ENC28J60_RD_EPMM6 epmm6;
	DRV_ENC28J60_RD_EPMM7 epmm7;
	DRV_ENC28J60_RD_EPMCS epmcs;
	DRV_ENC28J60_RD_EPMO epmo;
	DRV_ENC28J60_RD_ERXFCON erxfcon;
	DRV_ENC28J60_RD_EPKTCNT epktcnt;
	DRV_ENC28J60_RD_MACON1 macon1;
	DRV_ENC28J60_RD_MACON3 macon3;
	DRV_ENC28J60_RD_MACON4 macon4;
	DRV_ENC28J60_RD_MABBIPG mabbipg;
	DRV_ENC28J60_RD_MAIPG maipg;
	DRV_ENC28J60_RD_MACLCON maclcon;
	DRV_ENC28J60_RD_MAMXFL mamxfl;
	DRV_ENC28J60_RD_MICMD micmd;
	DRV_ENC28J60_RD_MIREGADR miregadr;
	DRV_ENC28J60_RD_MIWR miwr;
	DRV_ENC28J60_RD_MIRD mird;
	DRV_ENC28J60_RD_MAADR1 maadr1;
	DRV_ENC28J60_RD_MAADR2 maadr2;
	DRV_ENC28J60_RD_MAADR3 maadr3;
	DRV_ENC28J60_RD_MAADR4 maadr4;
	DRV_ENC28J60_RD_MAADR5 maadr5;
	DRV_ENC28J60_RD_MAADR6 maadr6;
	DRV_ENC28J60_RD_EBSTSD ebstsd;
	DRV_ENC28J60_RD_EBSTCON ebstcon;
	DRV_ENC28J60_RD_EBSTCS ebstcs;
	DRV_ENC28J60_RD_MISTAT mistat;
	DRV_ENC28J60_RD_EREVID erevid;
	DRV_ENC28J60_RD_ECOCON ecocon;
	DRV_ENC28J60_RD_EFLOCON eflocon;
	DRV_ENC28J60_RD_EPAUS epaus;
	DRV_ENC28J60_RD_PHCON1 phcon1;
	DRV_ENC28J60_RD_PHCON2 phcon2;
	DRV_ENC28J60_RD_PHSTAT1 phstat1;
	DRV_ENC28J60_RD_PHSTAT2 phstat2;
	DRV_ENC28J60_RD_PHIE phie;
	DRV_ENC28J60_RD_PHIR phir;
	DRV_ENC28J60_RD_PHLCON phlcon;
	DRV_ENC28J60_RD_PHID1 phid1;
	DRV_ENC28J60_RD_PHID2 phid2;
	
}  DRV_ENC28J60_RegUnion;

typedef union {
	uint8_t v[7];
	struct {
		uint16_t	 		ByteCount;
		uint8_t	CollisionCount:4;
		uint8_t	CRCError:1;
		uint8_t	LengthCheckError:1;
		uint8_t	LengthOutOfRange:1;
		uint8_t	Done:1;
		uint8_t	Multicast:1;
		uint8_t	Broadcast:1;
		uint8_t	PacketDefer:1;
		uint8_t	ExcessiveDefer:1;
		uint8_t	MaximumCollisions:1;
		uint8_t	LateCollision:1;
		uint8_t	Giant:1;
		uint8_t	Underrun:1;
		uint16_t	BytesTransmittedOnWire;
		uint8_t	ControlFrame:1;
		uint8_t	PAUSEControlFrame:1;
		uint8_t	BackpressureApplied:1;
		uint8_t	VLANTaggedFrame:1;
		uint8_t	Zeros:4;
	} bits;
} TXSTATUS;


typedef struct _DRV_ENC28J60_RSV
{
	uint16_t pNextPacket:16;
	uint16_t rxByteCount:16;
    uint16_t lonDropEvent:1;
	uint16_t :1;
	uint16_t carrierEventPrevSeen:1;
	uint16_t :1;
	uint16_t crcError:1;
	uint16_t lenChkError:1;
	uint16_t lenOutOfRange:1;
	uint16_t rxOk:1;
	uint16_t rxMultcast:1;
	uint16_t rxBcast:1;
	uint16_t dribbleNibble:1;
	uint16_t rxCtrlFrm:1;
	uint16_t rxPauseCtrlFrm:1;
	uint16_t rxUnknOpcode:1;
	uint16_t rxVlanTypeDetected:1;
	uint16_t zero:1;
}  DRV_ENC28J60_RSV;

// Extended Receive Status Vector
typedef struct 
{
    uint8_t filler;
    DRV_ENC28J60_RSV rsv;
} DRV_ENC28J60_RSV_EXT;


// the PHY ID on the ENC28J60
#define DRV_ENC28J60_PHY_ID1_VALUE  0x0083 

typedef enum
{
	DRV_ENC28J60_PHY_SFR_PHCON1 = 0x00,
	DRV_ENC28J60_PHY_SFR_PHSTAT1 = 0x01,
	DRV_ENC28J60_PHY_SFR_PHID1 = 0x02,
	DRV_ENC28J60_PHY_SFR_PHID2 = 0x03,
	DRV_ENC28J60_PHY_SFR_PHCON2 = 0x10,
	DRV_ENC28J60_PHY_SFR_PHSTAT2 = 0x11,
	DRV_ENC28J60_PHY_SFR_PHIE = 0x12,
	DRV_ENC28J60_PHY_SFR_PHIR = 0x13,
	DRV_ENC28J60_PHY_SFR_PHLCON = 0x14	
	
} DRV_ENC28J60_PHY_SFR_MAP;



typedef enum
{
    // bank 0
	DRV_ENC28J60_SFR_ERDPTL=0x00,
	DRV_ENC28J60_SFR_ERDPTH=0x01,
	DRV_ENC28J60_SFR_EWRPTL=0x02,
	DRV_ENC28J60_SFR_EWRPTH=0x03,
	DRV_ENC28J60_SFR_ETXSTL=0x04,
	DRV_ENC28J60_SFR_ETXSTH=0x05,
	DRV_ENC28J60_SFR_ETXNDL=0x06,
	DRV_ENC28J60_SFR_ETXNDH=0x07,
	DRV_ENC28J60_SFR_ERXSTL=0x08,
	DRV_ENC28J60_SFR_ERXSTH=0x09,
	DRV_ENC28J60_SFR_ERXNDL=0x0A,
	DRV_ENC28J60_SFR_ERXNDH=0x0B,
	DRV_ENC28J60_SFR_ERXRDPTL=0x0C,
	DRV_ENC28J60_SFR_ERXRDPTH=0x0D,
	DRV_ENC28J60_SFR_ERXWRPTL=0x0E,
	DRV_ENC28J60_SFR_ERXWRPTH=0x0F,
	DRV_ENC28J60_SFR_EDMASTL=0x10,
	DRV_ENC28J60_SFR_EDMASTH=0x11,
	DRV_ENC28J60_SFR_EDMANDL=0x12,
	DRV_ENC28J60_SFR_EDMANDH=0x13,
	DRV_ENC28J60_SFR_EDMADSTL=0x14,
	DRV_ENC28J60_SFR_EDMADSTH=0x15,
	DRV_ENC28J60_SFR_EDMACSL=0x16,
	DRV_ENC28J60_SFR_EDMACSH=0x17,
	DRV_ENC28J60_SFR_EIE=0x1B,
	DRV_ENC28J60_SFR_EIR=0x1C,
	DRV_ENC28J60_SFR_ESTAT=0x1D,
	DRV_ENC28J60_SFR_ECON2=0x1E,
	DRV_ENC28J60_SFR_ECON1=0x1F,

    // bank 1
	DRV_ENC28J60_SFR_EHT0=0x20,
	DRV_ENC28J60_SFR_EHT1=0x21,
	DRV_ENC28J60_SFR_EHT2=0x22,
	DRV_ENC28J60_SFR_EHT3=0x23,
	DRV_ENC28J60_SFR_EHT4=0x24,
	DRV_ENC28J60_SFR_EHT5=0x25,
	DRV_ENC28J60_SFR_EHT6=0x26,
	DRV_ENC28J60_SFR_EHT7=0x27,
	DRV_ENC28J60_SFR_EPMM0=0x28,
	DRV_ENC28J60_SFR_EPMM1=0x29,
	DRV_ENC28J60_SFR_EPMM2=0x2A,
	DRV_ENC28J60_SFR_EPMM3=0x2B,
	DRV_ENC28J60_SFR_EPMM4=0x2C,
	DRV_ENC28J60_SFR_EPMM5=0x2D,
	DRV_ENC28J60_SFR_EPMM6=0x2E,
	DRV_ENC28J60_SFR_EPMM7=0x2F,
	DRV_ENC28J60_SFR_EPMCSL=0x30,
	DRV_ENC28J60_SFR_EPMCSH=0x31,
	DRV_ENC28J60_SFR_EPMOL=0x34,
	DRV_ENC28J60_SFR_EPMOH=0x35,
	DRV_ENC28J60_SFR_ERXFCON=0x38,
	DRV_ENC28J60_SFR_EPKTCNT=0x39,

    // bank 2
	DRV_ENC28J60_SFR_MACON1=0x40,
	DRV_ENC28J60_SFR_MACON3=0x42,
	DRV_ENC28J60_SFR_MACON4=0x43,
	DRV_ENC28J60_SFR_MABBIPG=0x44,
	DRV_ENC28J60_SFR_MAIPGL=0x46,
	DRV_ENC28J60_SFR_MAIPGH=0x47,
	DRV_ENC28J60_SFR_MACLCON1=0x48,
	DRV_ENC28J60_SFR_MACLCON2=0x49,
	DRV_ENC28J60_SFR_MAMXFLL=0x4A,
	DRV_ENC28J60_SFR_MAMXFLH=0x4B,
	DRV_ENC28J60_SFR_MICMD=0x52,
	DRV_ENC28J60_SFR_MIREGADR=0x54,
	DRV_ENC28J60_SFR_MIWRL=0x56,
	DRV_ENC28J60_SFR_MIWRH=0x57,
	DRV_ENC28J60_SFR_MIRDL=0x58,
	DRV_ENC28J60_SFR_MIRDH=0x59,

    // bank 3
	DRV_ENC28J60_SFR_MAADR5=0x60,
	DRV_ENC28J60_SFR_MAADR6=0x61,
	DRV_ENC28J60_SFR_MAADR3=0x62,
	DRV_ENC28J60_SFR_MAADR4=0x63,
	DRV_ENC28J60_SFR_MAADR1=0x64,
	DRV_ENC28J60_SFR_MAADR2=0x65,
	DRV_ENC28J60_SFR_EBSTSD=0x66,
	DRV_ENC28J60_SFR_EBSTCON=0x67,
	DRV_ENC28J60_SFR_EBSTCSL=0x68,
	DRV_ENC28J60_SFR_EBSTCSH=0x69,
	DRV_ENC28J60_SFR_MISTAT=0x6A,
	DRV_ENC28J60_SFR_EREVID=0x72,
	DRV_ENC28J60_SFR_ECOCON=0x75,
	DRV_ENC28J60_SFR_EFLOCON=0x77,
	DRV_ENC28J60_SFR_EPAUSL=0x78,
	DRV_ENC28J60_SFR_EPAUSH=0x79,
	
} DRV_ENC28J60_SFR_MAP;

typedef enum
{

	/**
	 * Read Control Register
	 */
	DRV_ENC28J60_SPI_INST_RCR = 0x00,
	/**
	 * Read Buffer Memory
	 */
	DRV_ENC28J60_SPI_INST_RBM = 0x3A,
	/**
	 * Write Control Register
	 */
	DRV_ENC28J60_SPI_INST_WCR = 0x40,
	/**
	 * Write Buffer Memory
	 */
	DRV_ENC28J60_SPI_INST_WBM = 0x7A,
	/**
	 * Bit Field Set
	 */
	DRV_ENC28J60_SPI_INST_BFS = 0x80,
	/**
	 * Bit Field Clear
	 */
	DRV_ENC28J60_SPI_INST_BFC = 0xA0,
	/**
	 * System Reset Command
	 */
	DRV_ENC28J60_SPI_INST_SRC = 0xFF


} DRV_ENC28J60_SPI_INST_SET;


#endif  
