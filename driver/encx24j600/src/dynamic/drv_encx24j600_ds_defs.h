/*******************************************************************************
 ENCX24J600 Data Sheet Definitions.
  Company:
    Microchip Technology Inc.
    
  File Name:
    DRV_ENCX24J600_DS_DEFS.h
  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

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
#if !defined(MCHP_9AF9E982_4FC2_44a4_B2D8_34A7D80A51EA__INCLUDED_)
#define MCHP_9AF9E982_4FC2_44a4_B2D8_34A7D80A51EA__INCLUDED_


#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"

#define DRV_ENCX24J600_MEM_SIZE 0x6000

typedef struct _DRV_ENCX24J600_RD_ECON1
{
	uint8_t RXEN:1;
	uint8_t TXRTS:1;
	uint8_t DMANOCS:1;
	uint8_t DMACSSD:1;
	uint8_t DMACPY:1;
	uint8_t DMAST:1;
	uint8_t FCOP:2;
	uint8_t PKTDEC:1;
	uint8_t AESOP:2;
	uint8_t AESST:1;
	uint8_t HASHLST:1;
	uint8_t HASHOP:1;
	uint8_t HASHEN:1;
	uint8_t MODEXST:1;
}  DRV_ENCX24J600_RD_ECON1;


typedef struct _DRV_ENCX24J600_RD_ECON2
{
	uint8_t AESLEN:2;
	uint8_t MODLEN:2;
	uint8_t ETHRST:1;
	uint8_t RXRST:1;
	uint8_t TXRST:1;
	uint8_t AUTOFC:1;
	uint8_t COCON:4;
	uint8_t SHA1MD5:1;
	uint8_t TXMAC:1;
	uint8_t STRCH:1;
	uint8_t ETHEN:1;
}  DRV_ENCX24J600_RD_ECON2;


typedef struct _DRV_ENCX24J600_RD_EDMACS
{
	uint16_t EDMACS;
}  DRV_ENCX24J600_RD_EDMACS;


typedef struct _DRV_ENCX24J600_RD_EDMALEN
{
	uint16_t EDMALEN:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_EDMALEN;


typedef struct _DRV_ENCX24J600_RD_EDMAST
{
	uint16_t EDMAST:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_EDMAST;


typedef struct _DRV_ENCX24J600_RD_EGPDATA
{
	uint8_t EGPDATA;
	uint8_t :8;
}  DRV_ENCX24J600_RD_EGPDATA;


typedef struct _DRV_ENCX24J600_RD_EGPRDPT
{
	uint16_t EGPRDPT:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_EGPRDPT;


typedef struct _DRV_ENCX24J600_RD_EGPWRPT
{
	uint16_t EGPWRPT:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_EGPWRPT;


typedef struct _DRV_ENCX24J600_RD_EHT1
{
	uint16_t EHT1;
}  DRV_ENCX24J600_RD_EHT1;


typedef struct _DRV_ENCX24J600_RD_EHT2
{
	uint16_t EHT2;
}  DRV_ENCX24J600_RD_EHT2;


typedef struct _DRV_ENCX24J600_RD_EHT3
{
	uint16_t EHT3;
}  DRV_ENCX24J600_RD_EHT3;


typedef struct _DRV_ENCX24J600_RD_EHT4
{
	uint16_t EHT4;
}  DRV_ENCX24J600_RD_EHT4;


typedef struct _DRV_ENCX24J600_RD_EIDLED
{
	uint8_t REVID:5;
	uint8_t DEVID:3;
	uint8_t LBCFG:4;
	uint8_t LACFG:4;
}  DRV_ENCX24J600_RD_EIDLED;


typedef struct _DRV_ENCX24J600_RD_EIE
{
	/**
	 * <b>PCFULIE: </b>Packet Counter Full Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t PCFULIE:1;
	/**
	 * <b>RXABTIE: </b>Receive Abort Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t RXABTIE:1;
	/**
	 * <b>TXABTIE: </b>Transmit Abort Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t TXABTIE:1;
	/**
	 * <b>TXIE: </b>Transmit Done Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t TXIE:1;
	/**
	 * <b>Reserved: </b>Ignore on read, don't care on write<b>(1)</b>
	 */
	uint8_t :1;
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
	 * <b>Reserved: </b>Write as '0'
	 */
	uint8_t :1;
	uint8_t :3;
	/**
	 * <b>LINKIE: </b>PHY Link Status Change Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t LINKIE:1;
	/**
	 * <b>AESIE: </b>AES Encrypt/Decrypt Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t AESIE:1;
	/**
	 * <b>HASHIE: </b>MD5/SHA-1 Hash Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t HASHIE:1;
	/**
	 * <b>MODEXIE: </b>Modular Exponentiation Interrupt Enable bit
	 * 1 = Enabled
	 * 0 = Disabled
	 */
	uint8_t MODEXIE:1;
	/**
	 * <b>INTIE: </b>INT Global Interrupt Enable bit
	 * 1 = INT pin is controlled by the INT status bit (ESTAT<15>)
	 * 0 = INT pin is driven high
	 */
	uint8_t INTIE:1;
}  DRV_ENCX24J600_RD_EIE;


typedef struct _DRV_ENCX24J600_RD_EIR
{
	/**
	 * <b>PCFULIF: </b>Packet Counter Full Interrupt Flag bit
	 * 1 = PKTCNT field has reached FFh. Software must decrement the packet counter to
	 * prevent the next RX packet from being dropped.
	 * 0 = No interrupt pending
	 */
	uint8_t PCFULIF:1;
	/**
	 * <b>RXABTIF: </b>Receive Abort Interrupt Flag bit
	 * 1 = An RX packet was dropped because there is insufficient space in the RX
	 * buffer to store the complete packet or the PKTCNT field is saturated at FFh
	 * 0 = No interrupt pending
	 */
	uint8_t RXABTIF:1;
	/**
	 * <b>TXABTIF: </b>Transmit Abort Interrupt Flag bit
	 * 1 = Packet transmission has been aborted due to an error. Read the ETXSTAT
	 * register to determine the cause. TXRTS (ECON1<1>) has been cleared by hardware.
	 * 
	 * 0 = No interrupt pending
	 */
	uint8_t TXABTIF:1;
	/**
	 * <b>TXIF: </b>Transmit Done Interrupt Flag bit
	 * 1 = Packet transmission has completed. TXRTS (ECON1<1>) has been cleared by
	 * hardware.
	 * 0 = No interrupt pending
	 */
	uint8_t TXIF:1;
	/**
	 * <b>Reserved: </b>Ignore on read, don't care on write
	 */
	uint8_t :1;
	/**
	 * <b>DMAIF: </b>DMA Interrupt Flag bit
	 * 1 = DMA copy or checksum operation is complete
	 * 0 = No interrupt pending
	 */
	uint8_t DMAIF:1;
	/**
	 * <b>PKTIF: </b>RX Packet Pending Interrupt Flag bit
	 * 1 = One or more RX packets have been saved and are ready for software
	 * processing. The
	 * PKTCNT<7:0> (ESTAT<7:0>) bits are non-zero. To clear this flag, decrement the
	 * PKTCNT bits to zero by setting PKTDEC (ECON1<8>).
	 * 0 = No RX packets are pending
	 */
	uint8_t PKTIF:1;
	/**
	 * <b>Reserved: </b>Ignore on read, don't care on write
	 */
	uint8_t :1;
	uint8_t :3;
	/**
	 * <b>LINKIF: </b>PHY Link Status Change Interrupt Flag bit
	 * 1 = PHY Ethernet link status has changed. Read PHYLNK (ESTAT<8>) to determine
	 * the current state.
	 * 0 = No interrupt pending
	 */
	uint8_t LINKIF:1;
	/**
	 * <b>AESIF: </b>AES Encrypt/Decrypt Interrupt Flag bit
	 * 1 = AES encrypt/decrypt operation is complete
	 * 0 = No interrupt pending
	 */
	uint8_t AESIF:1;
	/**
	 * <b>HASHIF: </b>MD5/SHA-1 Hash Interrupt Flag bit
	 * 1 = MD5/SHA-1 hash operation is complete
	 * 0 = No interrupt pending
	 */
	uint8_t HASHIF:1;
	/**
	 * <b>MODEXIF: </b>Modular Exponentiation Interrupt Flag bit
	 * 1 = Modular exponentiation calculation is complete
	 * 0 = No interrupt pending
	 */
	uint8_t MODEXIF:1;
	/**
	 * <b>CRYPTEN</b>: Modular Exponentiation and AES Cryptographic Modules Enable
	 * bit
	 * 1 = All cryptographic engine modules are enabled
	 * 0 = Modular exponentiation and AES modules are disabled and powered down;
	 * MD5/SHA-1 hashing is still available
	 */
	uint8_t CRYPTEN:1;
}  DRV_ENCX24J600_RD_EIR;


typedef struct _DRV_ENCX24J600_RD_EPAUS
{
	uint16_t EPAUS;
}  DRV_ENCX24J600_RD_EPAUS;


typedef struct _DRV_ENCX24J600_RD_EPMCS
{
	uint16_t EPMCS;
}  DRV_ENCX24J600_RD_EPMCS;


typedef struct _DRV_ENCX24J600_RD_EPMM1
{
	uint16_t EPMM1;
}  DRV_ENCX24J600_RD_EPMM1;


typedef struct _DRV_ENCX24J600_RD_EPMM2
{
	uint16_t EPMM2;
}  DRV_ENCX24J600_RD_EPMM2;


typedef struct _DRV_ENCX24J600_RD_EPMM3
{
	uint16_t EPMM3;
}  DRV_ENCX24J600_RD_EPMM3;


typedef struct _DRV_ENCX24J600_RD_EPMM4
{
	uint16_t EPMM4;
}  DRV_ENCX24J600_RD_EPMM4;


typedef struct _DRV_ENCX24J600_RD_EPMO
{
	uint16_t EPMO;
}  DRV_ENCX24J600_RD_EPMO;


typedef struct _DRV_ENCX24J600_RD_ERXDATA
{
	uint8_t ERXDATA;
	uint8_t :8;
}  DRV_ENCX24J600_RD_ERXDATA;


typedef struct _DRV_ENCX24J600_RD_ERXFCON
{
	uint8_t BCEN:1;
	uint8_t MCEN:1;
	uint8_t NOTMEEN:1;
	uint8_t UCEN:1;
	uint8_t RUNTEN:1;
	uint8_t RUNTEEN:1;
	uint8_t CRCEN:1;
	uint8_t CRCEEN:1;
	uint8_t PMEN:4;
	uint8_t NOTPM:1;
	uint8_t :1;
	uint8_t MPEN:1;
	uint8_t HTEN:1;
}  DRV_ENCX24J600_RD_ERXFCON;


typedef struct _DRV_ENCX24J600_RD_ERXHEAD
{
	uint16_t ERXTAIL:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_ERXHEAD;


typedef struct _DRV_ENCX24J600_RD_ERXRDPT
{
	uint16_t ERXRDPT:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_ERXRDPT;


typedef struct _DRV_ENCX24J600_RD_ERXST
{
	uint16_t ERXST:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_ERXST;


typedef struct _DRV_ENCX24J600_RD_ERXTAIL
{
	uint16_t ERXTAIL:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_ERXTAIL;


typedef struct _DRV_ENCX24J600_RD_ERXWM
{
	uint8_t RXEWM;
	uint8_t RXFWM;
}  DRV_ENCX24J600_RD_ERXWM;


typedef struct _DRV_ENCX24J600_RD_ERXWRPT
{
	uint16_t ERXWRPT:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_ERXWRPT;


typedef struct _DRV_ENCX24J600_RD_ESTAT
{
	uint8_t PKTCNT:8;
	uint8_t PHYLNK:1;
	uint8_t :1;
	uint8_t PHYDPX:1;
	uint8_t :1;
	uint8_t CLKRDY:1;
	uint8_t RXBUSY:1;
	uint8_t FCIDLE:1;
	uint8_t INT:1;
}  DRV_ENCX24J600_RD_ESTAT;


typedef struct _DRV_ENCX24J600_RD_ETXLEN
{
	uint16_t ETXLEN:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_ETXLEN;


typedef struct _DRV_ENCX24J600_RD_ETXST
{
	uint16_t ETXST:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_ETXST;


typedef struct _DRV_ENCX24J600_RD_ETXSTAT
{
	uint8_t COLCNT:4;
	uint8_t CRCBAD:1;
	uint8_t :2;
	uint8_t DEFER:1;
	uint8_t EXDEFER:1;
	uint8_t MAXCOL:1;
	uint8_t LATECOL:1;
	uint8_t :5;
}  DRV_ENCX24J600_RD_ETXSTAT;


typedef struct _DRV_ENCX24J600_RD_ETXWIRE
{
	uint16_t ETXWIRE;
}  DRV_ENCX24J600_RD_ETXWIRE;


typedef struct _DRV_ENCX24J600_RD_EUDADATA
{
	uint8_t EUDADATA;
	uint8_t :8;
}  DRV_ENCX24J600_RD_EUDADATA;


typedef struct _DRV_ENCX24J600_RD_EUDAND
{
	uint16_t EUDAND:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_EUDAND;


typedef struct _DRV_ENCX24J600_RD_EUDARDPT
{
	uint16_t EUDARDPT:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_EUDARDPT;


typedef struct _DRV_ENCX24J600_RD_EUDAST
{
	uint16_t EUDAST:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_EUDAST;


typedef struct _DRV_ENCX24J600_RD_EUDAWRPT
{
	uint16_t EUDAWRPT:15;
	uint8_t :1;
}  DRV_ENCX24J600_RD_EUDAWRPT;


typedef struct _DRV_ENCX24J600_RD_MAADR1
{
	uint16_t MAADR1;
}  DRV_ENCX24J600_RD_MAADR1;


typedef struct _DRV_ENCX24J600_RD_MAADR2
{
	uint16_t MAADR2;
}  DRV_ENCX24J600_RD_MAADR2;


typedef struct _DRV_ENCX24J600_RD_MAADR3
{
	uint16_t MAADR3;
}  DRV_ENCX24J600_RD_MAADR3;


typedef struct _DRV_ENCX24J600_RD_MABBIPG
{
	uint8_t BBIPG:7;
	uint16_t :9;
}  DRV_ENCX24J600_RD_MABBIPG;


typedef struct _DRV_ENCX24J600_RD_MACLCON
{
	uint8_t MAXRET:4;
	uint16_t :12;
}  DRV_ENCX24J600_RD_MACLCON;


typedef struct _DRV_ENCX24J600_RD_MACON1
{
	uint8_t :1;
	uint8_t PASSALL:1;
	uint8_t RXPAUS:1;
	uint8_t :1;
	uint8_t LOOPBK:1;
	uint16_t :11;
}  DRV_ENCX24J600_RD_MACON1;


typedef struct _DRV_ENCX24J600_RD_MACON2
{
	uint8_t FULDPX:1;
	uint8_t :1;
	uint8_t HFRMEN:1;
	uint8_t PHDREN:1;
	uint8_t TXCRCEN:1;
	uint8_t PADCFG:3;
	uint8_t :4;
	uint8_t NOBKOFF:1;
	uint8_t BPEN:1;
	uint8_t DEFER:1;
	uint8_t :1;
}  DRV_ENCX24J600_RD_MACON2;


typedef struct _DRV_ENCX24J600_RD_MAIPG
{
	uint8_t IPG:7;
	uint16_t :9;
}  DRV_ENCX24J600_RD_MAIPG;


typedef struct _DRV_ENCX24J600_RD_MAMXFL
{
	uint16_t MAMXFL;
}  DRV_ENCX24J600_RD_MAMXFL;


typedef struct _DRV_ENCX24J600_RD_MICMD
{
	uint8_t MIIRD:1;
	uint8_t MIISCAN:1;
	uint16_t :14;
}  DRV_ENCX24J600_RD_MICMD;


typedef struct _DRV_ENCX24J600_RD_MIRD
{
	uint16_t MIRD;
}  DRV_ENCX24J600_RD_MIRD;


typedef struct _DRV_ENCX24J600_RD_MIREGADR
{
	uint8_t PHREG:5;
	uint8_t :3;
	uint8_t writeAsOne:1;
	uint8_t :7;
}  DRV_ENCX24J600_RD_MIREGADR;


typedef struct _DRV_ENCX24J600_RD_MISTAT
{
	uint8_t BUSY:1;
	uint8_t SCAN:1;
	uint8_t NVALID:1;
	uint16_t :13;
}  DRV_ENCX24J600_RD_MISTAT;


typedef struct _DRV_ENCX24J600_RD_MIWR
{
	uint16_t MIWR;
}  DRV_ENCX24J600_RD_MIWR;


typedef struct _DRV_ENCX24J600_RD_PHANA
{
	uint8_t ADIEEE:5;
	uint8_t AD10:1;
	uint8_t AD10FD:1;
	uint8_t AD100:1;
	uint8_t AD100FD:1;
	uint8_t __unnamed_9:1;
	uint8_t ADPAUS:2;
	uint8_t __unnamed_12:1;
	uint8_t ADFAULT:1;
	uint8_t __unnamed_14:1;
	uint8_t ADNP:1;
}  DRV_ENCX24J600_RD_PHANA;


typedef struct _DRV_ENCX24J600_RD_PHANE
{
	uint8_t LPANABL:1;
	uint8_t LPAPCD:1;
	uint8_t :2;
	uint8_t PDFLT:1;
	uint16_t :11;
}  DRV_ENCX24J600_RD_PHANE;


typedef struct _DRV_ENCX24J600_RD_PHANLPA
{
	uint8_t LPIEEE:5;
	uint8_t LP10:1;
	uint8_t LP10FD:1;
	uint8_t LP100:1;
	uint8_t LP100FD:1;
	uint8_t LP100T4:1;
	uint8_t LPPAUS:2;
	uint8_t :1;
	uint8_t LPFAULT:1;
	uint8_t LPACK:1;
	uint8_t LPNP:1;
}  DRV_ENCX24J600_RD_PHANLPA;


typedef struct _DRV_ENCX24J600_RD_PHCON1
{
	uint8_t :8;
	uint8_t PFULDPX:1;
	uint8_t REGNEG:1;
	uint8_t :1;
	uint8_t PSLEEP:1;
	uint8_t ANEN:1;
	uint8_t SPD100:1;
	uint8_t PLOOPBK:1;
	uint8_t PRST:1;
}  DRV_ENCX24J600_RD_PHCON1;


typedef struct _DRV_ENCX24J600_RD_PHCON2
{
	uint8_t :1;
	uint8_t EDSTAT:1;
	uint8_t FRCLNK:1;
	uint8_t :8;
	uint8_t EDTHRES:1;
	uint8_t :1;
	uint8_t EDPWRDN:1;
	uint8_t :2;
}  DRV_ENCX24J600_RD_PHCON2;


typedef struct _DRV_ENCX24J600_RD_PHSTAT1
{
	uint8_t EXTREGS:1;
	uint8_t :1;
	uint8_t LLSTAT:1;
	uint8_t ANABLE:1;
	uint8_t LRFAULT:1;
	uint8_t ANDONE:1;
	uint8_t :5;
	uint8_t HALF10:1;
	uint8_t FULL10:1;
	uint8_t HALF100:1;
	uint8_t FULL100:1;
	uint8_t :1;
}  DRV_ENCX24J600_RD_PHSTAT1;


typedef struct _DRV_ENCX24J600_RD_PHSTAT2
{
	uint8_t :4;
	uint8_t PLRITY:1;
	uint16_t :11;
}  DRV_ENCX24J600_RD_PHSTAT2;


typedef struct _DRV_ENCX24J600_RD_PHSTAT3
{
	uint8_t :2;
	uint8_t SPDDPX:3;
	uint16_t :11;
}  DRV_ENCX24J600_RD_PHSTAT3;


typedef union _DRV_ENCX24J600_RegUnion
{
	uint16_t value;
	DRV_ENCX24J600_RD_ECON1 econ1;
	DRV_ENCX24J600_RD_ECON2 econ2;
	DRV_ENCX24J600_RD_EDMALEN edmalen;
	DRV_ENCX24J600_RD_EDMACS edmacs;
	DRV_ENCX24J600_RD_EDMAST edmast;
	DRV_ENCX24J600_RD_EGPDATA egpdata;
	DRV_ENCX24J600_RD_EGPRDPT egprdpt;
	DRV_ENCX24J600_RD_EGPWRPT egpwrpt;
	DRV_ENCX24J600_RD_EHT1 eht1;
	DRV_ENCX24J600_RD_EIR eir;
	DRV_ENCX24J600_RD_EPAUS epaus;
	DRV_ENCX24J600_RD_EPMCS epmcs;
	DRV_ENCX24J600_RD_EPMM1 epmm1;
	DRV_ENCX24J600_RD_EPMM2 epmm2;
	DRV_ENCX24J600_RD_EPMM3 epmm3;
	DRV_ENCX24J600_RD_EHT2 eht2;
	DRV_ENCX24J600_RD_EHT3 eht3;
	DRV_ENCX24J600_RD_EHT4 eht4;
	DRV_ENCX24J600_RD_EIDLED eidled;
	DRV_ENCX24J600_RD_EIE eie;
	DRV_ENCX24J600_RD_EPMM4 epmm4;
	DRV_ENCX24J600_RD_EPMO epmo;
	DRV_ENCX24J600_RD_ERXDATA erxdata;
	DRV_ENCX24J600_RD_ERXFCON erxfcon;
	DRV_ENCX24J600_RD_ERXHEAD erxhead;
	DRV_ENCX24J600_RD_ERXRDPT erxrdpt;
	DRV_ENCX24J600_RD_ERXST erxst;
	DRV_ENCX24J600_RD_ERXTAIL erxtail;
	DRV_ENCX24J600_RD_ERXWM erxwm;
	DRV_ENCX24J600_RD_ERXWRPT erxwrpt;
	DRV_ENCX24J600_RD_EUDADATA eudadata;
	DRV_ENCX24J600_RD_EUDAND eudand;
	DRV_ENCX24J600_RD_EUDARDPT eudardpt;
	DRV_ENCX24J600_RD_EUDAST eudast;
	DRV_ENCX24J600_RD_EUDAWRPT eudawrpt;
	DRV_ENCX24J600_RD_MAADR1 maadr1;
	DRV_ENCX24J600_RD_MAADR2 maadr2;
	DRV_ENCX24J600_RD_MAADR3 maadr3;
	DRV_ENCX24J600_RD_MABBIPG mabbipg;
	DRV_ENCX24J600_RD_MACLCON maclcon;
	DRV_ENCX24J600_RD_MIRD mird;
	DRV_ENCX24J600_RD_MIREGADR miregadr;
	DRV_ENCX24J600_RD_MISTAT mistat;
	DRV_ENCX24J600_RD_MIWR miwr;
	DRV_ENCX24J600_RD_PHANA phana;
	DRV_ENCX24J600_RD_PHSTAT1 phstat1;
	DRV_ENCX24J600_RD_PHSTAT2 phstat2;
	DRV_ENCX24J600_RD_PHSTAT3 phstat3;
	DRV_ENCX24J600_RD_ESTAT estat;
	DRV_ENCX24J600_RD_ETXLEN etxlen;
	DRV_ENCX24J600_RD_ETXST etxst;
	DRV_ENCX24J600_RD_ETXSTAT etxstat;
	DRV_ENCX24J600_RD_ETXWIRE etxwire;
	DRV_ENCX24J600_RD_MACON1 macon1;
	DRV_ENCX24J600_RD_MACON2 macon2;
	DRV_ENCX24J600_RD_MAIPG maipg;
	DRV_ENCX24J600_RD_MAMXFL mamxfl;
	DRV_ENCX24J600_RD_MICMD micmd;
	DRV_ENCX24J600_RD_PHANE phane;
	DRV_ENCX24J600_RD_PHANLPA phanlpa;
	DRV_ENCX24J600_RD_PHCON1 phcon1;
	DRV_ENCX24J600_RD_PHCON2 phcon2;
}  DRV_ENCX24J600_RegUnion;


typedef struct _DRV_ENCX24J600_RSV
{
	uint16_t pNextPacket:16;
	uint16_t rxByteCount:16;
	uint8_t prevIgnored:1;
	uint8_t :1;
	uint8_t carrierEventPrevSeen:1;
	uint8_t :1;
	uint8_t crcError:1;
	uint8_t lenChkError:1;
	uint8_t lenOutOfRange:1;
	uint8_t rxOk:1;
	uint8_t rxMultcast:1;
	uint8_t rxBcast:1;
	uint8_t dribbleNibble:1;
	uint8_t rxCtrlFrm:1;
	uint8_t rxPauseCtrlFrm:1;
	uint8_t rxUkwnOpCode:1;
	uint8_t rxVLAN:1;
	uint8_t runtFltrMtch:1;
	uint8_t notMeFltrMtch:1;
	uint8_t hashFltrMtch:1;
	uint8_t magicPktFltrMtch:1;
	uint8_t patternFltrMtch:1;
	uint8_t unicastFltrMtch:1;
	uint16_t :11;
}  DRV_ENCx24J600_RSV;

typedef struct
{
    uint8_t            filler;
    DRV_ENCx24J600_RSV rsv;
}DRV_ENCx24J600_RSV_EXT;

typedef enum
{
	DRV_ENCX24J600_PHY_SFR_PHCON1 = 0x00,
	DRV_ENCX24J600_PHY_SFR_PHSTAT1 = 0x01,
	DRV_ENCX24J600_PHY_SFR_PHANA = 0x04,
	DRV_ENCX24J600_PHY_SFR_PHANLPA = 0x05,
	DRV_ENCX24J600_PHY_SFR_PHANE = 0x06,
	DRV_ENCX24J600_PHY_SFR_PHCON2 = 0x11,
	DRV_ENCX24J600_PHY_SFR_PHSTAT2 = 0x1B,
	DRV_ENCX24J600_PHY_SFR_PHSTAT3 = 0x1F
} DRV_ENCX24J600_PHY_SFR_MAP;

typedef enum
{
	DRV_ENCX24J600_SFR_ETXS,
	DRV_ENCX24J600_SFR_ETXLEN,
	DRV_ENCX24J600_SFR_ERXST,
	DRV_ENCX24J600_SFR_ERXTAIL,
	DRV_ENCX24J600_SFR_ERXHEAD,
	DRV_ENCX24J600_SFR_EDMAST,
	DRV_ENCX24J600_SFR_EDMALEN,
	DRV_ENCX24J600_SFR_EDMADST,
	DRV_ENCX24J600_SFR_EDMACS,
	DRV_ENCX24J600_SFR_ETXSTAT,
	DRV_ENCX24J600_SFR_ETXWIRE,
	DRV_ENCX24J600_SFR_EUDAST,
	DRV_ENCX24J600_SFR_EUDAND,
	DRV_ENCX24J600_SFR_ESTAT,
	DRV_ENCX24J600_SFR_EIR,
	DRV_ENCX24J600_SFR_ECON1,
	DRV_ENCX24J600_SFR_EHT1,
	DRV_ENCX24J600_SFR_EHT2,
	DRV_ENCX24J600_SFR_EHT3,
	DRV_ENCX24J600_SFR_EHT4,
	DRV_ENCX24J600_SFR_EPMM1,
	DRV_ENCX24J600_SFR_EPMM2,
	DRV_ENCX24J600_SFR_EPMM3,
	DRV_ENCX24J600_SFR_EPMM4,
	DRV_ENCX24J600_SFR_EPMCS,
	DRV_ENCX24J600_SFR_EPMO,
	DRV_ENCX24J600_SFR_ERXFCON,
	DRV_ENCX24J600_SFR_MACON1,
	DRV_ENCX24J600_SFR_MACON2,
	DRV_ENCX24J600_SFR_MABBIPG,
	DRV_ENCX24J600_SFR_MAIPG,
	DRV_ENCX24J600_SFR_MACLCON,
	DRV_ENCX24J600_SFR_MAMXFL,
	DRV_ENCX24J600_SFR_MICMD,
	DRV_ENCX24J600_SFR_MIREGADR,
	DRV_ENCX24J600_SFR_MAADR3,
	DRV_ENCX24J600_SFR_MAADR2,
	DRV_ENCX24J600_SFR_MAADR1,
	DRV_ENCX24J600_SFR_MIWR,
	DRV_ENCX24J600_SFR_MIRD,
	DRV_ENCX24J600_SFR_MISTAT,
	DRV_ENCX24J600_SFR_EPAUS,
	DRV_ENCX24J600_SFR_ECON2,
	DRV_ENCX24J600_SFR_ERXWM,
	DRV_ENCX24J600_SFR_EIE,
	DRV_ENCX24J600_SFR_EIDLED,
	DRV_ENCX24J600_SFR_EGPDATA,
	DRV_ENCX24J600_SFR_ERXDATA,
	DRV_ENCX24J600_SFR_EUDADTA,
	DRV_ENCX24J600_SFR_EGPRDPT,
	DRV_ENCX24J600_SFR_EGPWRPT,
	DRV_ENCX24J600_SFR_ERXRDPT,
	DRV_ENCX24J600_SFR_ERXWRPT,
	DRV_ENCX24J600_SFR_EUDARDPT,
	DRV_ENCX24J600_SFR_EUDAWRPT
} DRV_ENCX24J600_SFR_MAP;

typedef enum
{
	/**
	 * Bank 0 Select
	 */
	DRV_ENCX24J600_SPI_INST_B0SEL = 0xC0,
	/**
	 * Bank 1 Select
	 */
	DRV_ENCX24J600_SPI_INST_B1SEL = 0xC2,
	/**
	 * Bank 2 Select
	 */
	DRV_ENCX24J600_SPI_INST_B2SEL = 0xC4,
	/**
	 * Bank 3 Select
	 */
	DRV_ENCX24J600_SPI_INST_B3SEL = 0xC6,
	/**
	 * System Reset
	 */
	DRV_ENCX24J600_SPI_INST_SETETHRST = 0xCA,
	/**
	 * Flow Control Disable
	 */
	DRV_ENCX24J600_SPI_INST_FCDISABLE = 0xE0,
	/**
	 * Flow Control Single
	 */
	DRV_ENCX24J600_SPI_INST_FCSINGLE = 0xE2,
	/**
	 * Flow Control Multiple
	 */
	DRV_ENCX24J600_SPI_INST_FCMULTIPLE = 0xE4,
	/**
	 * Flow Control Clear
	 */
	DRV_ENCX24J600_SPI_INST_FCCLEAR = 0xE6,
	/**
	 * Decrement Packet Counter
	 */
	DRV_ENCX24J600_SPI_INST_SETPKTDEC = 0xCC,
	/**
	 * DMA Stop
	 */
	DRV_ENCX24J600_SPI_INST_DMASTOP = 0xD2,
	/**
	 * DMA Start Checksum
	 */
	DRV_ENCX24J600_SPI_INST_DMACKSUM = 0xD8,
	/**
	 * DMA Start Checksum with Seed
	 */
	DRV_ENCX24J600_SPI_INST_DMACKSUMS = 0xDA,
	/**
	 * DMA Start Copy
	 */
	DRV_ENCX24J600_SPI_INST_DMACOPY = 0xDC,
	/**
	 * DMA Start Copy and Checksum with Seed
	 */
	DRV_ENCX24J600_SPI_INST_DMACOPYS = 0xDE,
	/**
	 * Request Packet Transmission
	 */
	DRV_ENCX24J600_SPI_INST_SETTXRTS = 0xD4,
	/**
	 * Enable RX
	 */
	DRV_ENCX24J600_SPI_INST_ENABLERX = 0xE8,
	/**
	 * Disable RX
	 */
	DRV_ENCX24J600_SPI_INST_DISABLERX = 0xEA,
	/**
	 * Enable Interrupts
	 */
	DRV_ENCX24J600_SPI_INST_SETEIE = 0xEC,
	/**
	 * Disable Interrupts
	 */
	DRV_ENCX24J600_SPI_INST_CLREIE = 0xEE,
	/**
	 * Read Bank Select
	 */
	DRV_ENCX24J600_SPI_INST_RBSEL = 0xC8,
	/**
	 * Write EGPRDPT
	 */
	DRV_ENCX24J600_SPI_INST_WGPRDPT = 0x60,
	/**
	 * Read EGPRDPT
	 */
	DRV_ENCX24J600_SPI_INST_RGPRDPT = 0x62,
	/**
	 * Write ERXRDPT
	 */
	DRV_ENCX24J600_SPI_INST_WRXRDPT = 0x64,
	/**
	 * Read ERXRDPT
	 */
	DRV_ENCX24J600_SPI_INST_RRXRDPT = 0x66,
	/**
	 * Write EUDARDPT
	 */
	DRV_ENCX24J600_SPI_INST_WUDARDPT = 0x68,
	/**
	 * Read EUDARDPT
	 */
	DRV_ENCX24J600_SPI_INST_RUDARDPT = 0x6A,
	/**
	 * Write EGPWRPT
	 */
	DRV_ENCX24J600_SPI_INST_WGPWRPT = 0x6C,
	/**
	 * Read EGPWRPT
	 */
	DRV_ENCX24J600_SPI_INST_RGPWRPT = 0x6E,
	/**
	 * Write ERXWRPT
	 */
	DRV_ENCX24J600_SPI_INST_WRXWRPT = 0x70,
	/**
	 * Read ERXWRPT
	 */
	DRV_ENCX24J600_SPI_INST_RRXWRPT = 0x72,
	/**
	 * Write EUDAWRPT
	 */
	DRV_ENCX24J600_SPI_INST_WUDAWRPT = 0x74,
	/**
	 * Read EUDAWRPT
	 */
	DRV_ENCX24J600_SPI_INST_RUDAWRPT = 0x76,
	/**
	 * Read Control Register
	 */
	DRV_ENCX24J600_SPI_INST_RCR = 0x00,
	/**
	 * Write Control Register
	 */
	DRV_ENCX24J600_SPI_INST_WCR = 0x40,
	/**
	 * Read Control Register Unbanked
	 */
	DRV_ENCX24J600_SPI_INST_RCRU = 0x20,
	/**
	 * Write Control Register Unbanked
	 */
	DRV_ENCX24J600_SPI_INST_WCRU = 0x22,
	/**
	 * Bit Field Set
	 */
	DRV_ENCX24J600_SPI_INST_BFS = 0x80,
	/**
	 * Bit Field Clear
	 */
	DRV_ENCX24J600_SPI_INST_BFC = 0xA0,
	/**
	 * Bit Field Set Unbanked
	 */
	DRV_ENCX24J600_SPI_INST_BFSU = 0x24,
	/**
	 * Bit Field Clear Unbanked
	 */
	DRV_ENCX24J600_SPI_INST_BFCU = 0x26,
	/**
	 * Read EGPDATA
	 */
	DRV_ENCX24J600_SPI_INST_RGPDATA = 0x28,
	/**
	 * Write EGPDATA
	 */
	DRV_ENCX24J600_SPI_INST_WGPDATA = 0x2A,
	/**
	 * Read ERXDATA
	 */
	DRV_ENCX24J600_SPI_INST_RRXDATA = 0x2C,
	/**
	 * Write ERXDATA
	 */
	DRV_ENCX24J600_SPI_INST_WRXDATA = 0x2E,
	/**
	 * Read EUDADATA
	 */
	DRV_ENCX24J600_SPI_INST_RUDADATA = 0x30,
	/**
	 * Write EUDADATA
	 */
	DRV_ENCX24J600_SPI_INST_WUDADATA = 0x32
} DRV_ENCX24J600_SPI_INST_SET;

typedef enum
{
	DRV_ENCX24J600_SPI_SFR_ETXST = 0x00,
	DRV_ENCX24J600_SPI_SFR_ETXLEN = 0x02,
	DRV_ENCX24J600_SPI_SFR_ERXST = 0x04,
	DRV_ENCX24J600_SPI_SFR_ERXTAIL = 0x06,
	DRV_ENCX24J600_SPI_SFR_ERXHEAD = 0x08,
	DRV_ENCX24J600_SPI_SFR_EDMAST = 0x0A,
	DRV_ENCX24J600_SPI_SFR_EDMALEN = 0x0C,
	DRV_ENCX24J600_SPI_SFR_EDMADST = 0x0E,
	DRV_ENCX24J600_SPI_SFR_EDMACS = 0x10,
	DRV_ENCX24J600_SPI_SFR_ETXSTAT = 0x12,
	DRV_ENCX24J600_SPI_SFR_ETXWIRE = 0x14,
	DRV_ENCX24J600_SPI_SFR_EUDAST = 0x16,
	DRV_ENCX24J600_SPI_SFR_EUDAND = 0x18,
	DRV_ENCX24J600_SPI_SFR_ESTAT = 0x1A,
	DRV_ENCX24J600_SPI_SFR_EIR = 0x1C,
	DRV_ENCX24J600_SPI_SFR_ECON1 = 0x1E,
	DRV_ENCX24J600_SPI_SFR_EHT1 = 0x20,
	DRV_ENCX24J600_SPI_SFR_EHT2 = 0x22,
	DRV_ENCX24J600_SPI_SFR_EHT3 = 0x24,
	DRV_ENCX24J600_SPI_SFR_EHT4 = 0x26,
	DRV_ENCX24J600_SPI_SFR_EPMM1 = 0x28,
	DRV_ENCX24J600_SPI_SFR_EPMM2 = 0x2A,
	DRV_ENCX24J600_SPI_SFR_EPMM3 = 0x2C,
	DRV_ENCX24J600_SPI_SFR_EPMM4 = 0x2E,
	DRV_ENCX24J600_SPI_SFR_EPMCS = 0x30,
	DRV_ENCX24J600_SPI_SFR_EPMO = 0x32,
	DRV_ENCX24J600_SPI_SFR_ERXFCON = 0x34,
	DRV_ENCX24J600_SPI_SFR_MACON1 = 0x40,
	DRV_ENCX24J600_SPI_SFR_MACON2 = 0x42,
	DRV_ENCX24J600_SPI_SFR_MABBIPG = 0x44,
	DRV_ENCX24J600_SPI_SFR_MAIPG = 0x46,
	DRV_ENCX24J600_SPI_SFR_MACLCON = 0x48,
	DRV_ENCX24J600_SPI_SFR_MAMXFL = 0x4A,
	DRV_ENCX24J600_SPI_SFR_MICMD = 0x52,
	DRV_ENCX24J600_SPI_SFR_MIREGADR = 0x54,
	DRV_ENCX24J600_SPI_SFR_MAADR3 = 0x60,
	DRV_ENCX24J600_SPI_SFR_MAADR2 = 0x62,
	DRV_ENCX24J600_SPI_SFR_MAADR1 = 0x64,
	DRV_ENCX24J600_SPI_SFR_MIWR = 0x66,
	DRV_ENCX24J600_SPI_SFR_MIRD = 0x68,
	DRV_ENCX24J600_SPI_SFR_MISTAT = 0x6A,
	DRV_ENCX24J600_SPI_SFR_EPAUS = 0x6C,
	DRV_ENCX24J600_SPI_SFR_ECON2 = 0x6E,
	DRV_ENCX24J600_SPI_SFR_ERXWM = 0x70,
	DRV_ENCX24J600_SPI_SFR_EIE = 0x72,
	DRV_ENCX24J600_SPI_SFR_EIDLED = 0x74,
	DRV_ENCX24J600_SPI_SFR_EGPDATA = 0x80,
	DRV_ENCX24J600_SPI_SFR_ERXDATA = 0x82,
	DRV_ENCX24J600_SPI_SFR_EUDADTA = 0x84,
	DRV_ENCX24J600_SPI_SFR_EGPRDPT = 0x86,
	DRV_ENCX24J600_SPI_SFR_EGPWRPT = 0x88,
	DRV_ENCX24J600_SPI_SFR_ERXRDPT = 0x8A,
	DRV_ENCX24J600_SPI_SFR_ERXWRPT = 0x8C,
	DRV_ENCX24J600_SPI_SFR_EUDARDPT = 0x8E,
	DRV_ENCX24J600_SPI_SFR_EUDAWRPT = 0x90
} DRV_ENCX24J600_SPI_SFR_MAP;


#ifdef __cplusplus
}
#endif


#endif /*!defined(MCHP_9AF9E982_4FC2_44a4_B2D8_34A7D80A51EA__INCLUDED_)*/
 
