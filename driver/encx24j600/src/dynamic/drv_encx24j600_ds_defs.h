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
/*
Copyright (C) 2014-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#if !defined(MCHP_9AF9E982_4FC2_44a4_B2D8_34A7D80A51EA__INCLUDED_)
#define MCHP_9AF9E982_4FC2_44a4_B2D8_34A7D80A51EA__INCLUDED_


#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"

#define DRV_ENCX24J600_MEM_SIZE 0x6000U

typedef struct __attribute__((packed))
{
    unsigned RXEN:1;
    unsigned TXRTS:1;
    unsigned DMANOCS:1;
    unsigned DMACSSD:1;
    unsigned DMACPY:1;
    unsigned DMAST:1;
    unsigned FCOP:2;
    unsigned PKTDEC:1;
    unsigned AESOP:2;
    unsigned AESST:1;
    unsigned HASHLST:1;
    unsigned HASHOP:1;
    unsigned HASHEN:1;
    unsigned MODEXST:1;
}  DRV_ENCX24J600_RD_ECON1;


typedef struct  __attribute__((packed))
{
    unsigned AESLEN:2;
    unsigned MODLEN:2;
    unsigned ETHRST:1;
    unsigned RXRST:1;
    unsigned TXRST:1;
    unsigned AUTOFC:1;
    unsigned COCON:4;
    unsigned SHA1MD5:1;
    unsigned TXMAC:1;
    unsigned STRCH:1;
    unsigned ETHEN:1;
}  DRV_ENCX24J600_RD_ECON2;


typedef struct 
{
    uint16_t EDMACS;
}  DRV_ENCX24J600_RD_EDMACS;


typedef struct  __attribute__((packed))
{
    unsigned EDMALEN:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_EDMALEN;


typedef struct  __attribute__((packed))
{
    unsigned EDMAST:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_EDMAST;


typedef struct  __attribute__((packed))
{
    uint8_t EGPDATA;
    unsigned :8;
}  DRV_ENCX24J600_RD_EGPDATA;


typedef struct  __attribute__((packed))
{
    unsigned EGPRDPT:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_EGPRDPT;


typedef struct  __attribute__((packed))
{
    unsigned EGPWRPT:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_EGPWRPT;


typedef struct 
{
    uint16_t EHT1;
}  DRV_ENCX24J600_RD_EHT1;


typedef struct 
{
    uint16_t EHT2;
}  DRV_ENCX24J600_RD_EHT2;


typedef struct 
{
    uint16_t EHT3;
}  DRV_ENCX24J600_RD_EHT3;


typedef struct 
{
    uint16_t EHT4;
}  DRV_ENCX24J600_RD_EHT4;


typedef struct  __attribute__((packed))
{
    unsigned REVID:5;
    unsigned DEVID:3;
    unsigned LBCFG:4;
    unsigned LACFG:4;
}  DRV_ENCX24J600_RD_EIDLED;


typedef struct  __attribute__((packed))
{
    /**
     * <b>PCFULIE: </b>Packet Counter Full Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned PCFULIE:1;
    /**
     * <b>RXABTIE: </b>Receive Abort Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned RXABTIE:1;
    /**
     * <b>TXABTIE: </b>Transmit Abort Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned TXABTIE:1;
    /**
     * <b>TXIE: </b>Transmit Done Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned TXIE:1;
    /**
     * <b>Reserved: </b>Ignore on read, don't care on write<b>(1)</b>
     */
    unsigned :1;
    /**
     * <b>DMAIE: </b>DMA Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned DMAIE:1;
    /**
     * <b>PKTIE: </b>RX Packet Pending Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned PKTIE:1;
    /**
     * <b>Reserved: </b>Write as '0'
     */
    unsigned :1;
    unsigned :3;
    /**
     * <b>LINKIE: </b>PHY Link Status Change Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned LINKIE:1;
    /**
     * <b>AESIE: </b>AES Encrypt/Decrypt Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned AESIE:1;
    /**
     * <b>HASHIE: </b>MD5/SHA-1 Hash Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned HASHIE:1;
    /**
     * <b>MODEXIE: </b>Modular Exponentiation Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned MODEXIE:1;
    /**
     * <b>INTIE: </b>INT Global Interrupt Enable bit
     * 1 = INT pin is controlled by the INT status bit (ESTAT<15>)
     * 0 = INT pin is driven high
     */
    unsigned INTIE:1;
}  DRV_ENCX24J600_RD_EIE;


typedef struct  __attribute__((packed))
{
    /**
     * <b>PCFULIF: </b>Packet Counter Full Interrupt Flag bit
     * 1 = PKTCNT field has reached FFh. Software must decrement the packet counter to
     * prevent the next RX packet from being dropped.
     * 0 = No interrupt pending
     */
    unsigned PCFULIF:1;
    /**
     * <b>RXABTIF: </b>Receive Abort Interrupt Flag bit
     * 1 = An RX packet was dropped because there is insufficient space in the RX
     * buffer to store the complete packet or the PKTCNT field is saturated at FFh
     * 0 = No interrupt pending
     */
    unsigned RXABTIF:1;
    /**
     * <b>TXABTIF: </b>Transmit Abort Interrupt Flag bit
     * 1 = Packet transmission has been aborted due to an error. Read the ETXSTAT
     * register to determine the cause. TXRTS (ECON1<1>) has been cleared by hardware.
     * 
     * 0 = No interrupt pending
     */
    unsigned TXABTIF:1;
    /**
     * <b>TXIF: </b>Transmit Done Interrupt Flag bit
     * 1 = Packet transmission has completed. TXRTS (ECON1<1>) has been cleared by
     * hardware.
     * 0 = No interrupt pending
     */
    unsigned TXIF:1;
    /**
     * <b>Reserved: </b>Ignore on read, don't care on write
     */
    unsigned :1;
    /**
     * <b>DMAIF: </b>DMA Interrupt Flag bit
     * 1 = DMA copy or checksum operation is complete
     * 0 = No interrupt pending
     */
    unsigned DMAIF:1;
    /**
     * <b>PKTIF: </b>RX Packet Pending Interrupt Flag bit
     * 1 = One or more RX packets have been saved and are ready for software
     * processing. The
     * PKTCNT<7:0> (ESTAT<7:0>) bits are non-zero. To clear this flag, decrement the
     * PKTCNT bits to zero by setting PKTDEC (ECON1<8>).
     * 0 = No RX packets are pending
     */
    unsigned PKTIF:1;
    /**
     * <b>Reserved: </b>Ignore on read, don't care on write
     */
    unsigned :1;
    unsigned :3;
    /**
     * <b>LINKIF: </b>PHY Link Status Change Interrupt Flag bit
     * 1 = PHY Ethernet link status has changed. Read PHYLNK (ESTAT<8>) to determine
     * the current state.
     * 0 = No interrupt pending
     */
    unsigned LINKIF:1;
    /**
     * <b>AESIF: </b>AES Encrypt/Decrypt Interrupt Flag bit
     * 1 = AES encrypt/decrypt operation is complete
     * 0 = No interrupt pending
     */
    unsigned AESIF:1;
    /**
     * <b>HASHIF: </b>MD5/SHA-1 Hash Interrupt Flag bit
     * 1 = MD5/SHA-1 hash operation is complete
     * 0 = No interrupt pending
     */
    unsigned HASHIF:1;
    /**
     * <b>MODEXIF: </b>Modular Exponentiation Interrupt Flag bit
     * 1 = Modular exponentiation calculation is complete
     * 0 = No interrupt pending
     */
    unsigned MODEXIF:1;
    /**
     * <b>CRYPTEN</b>: Modular Exponentiation and AES Cryptographic Modules Enable
     * bit
     * 1 = All cryptographic engine modules are enabled
     * 0 = Modular exponentiation and AES modules are disabled and powered down;
     * MD5/SHA-1 hashing is still available
     */
    unsigned CRYPTEN:1;
}  DRV_ENCX24J600_RD_EIR;


typedef struct 
{
    uint16_t EPAUS;
}  DRV_ENCX24J600_RD_EPAUS;


typedef struct 
{
    uint16_t EPMCS;
}  DRV_ENCX24J600_RD_EPMCS;


typedef struct 
{
    uint16_t EPMM1;
}  DRV_ENCX24J600_RD_EPMM1;


typedef struct 
{
    uint16_t EPMM2;
}  DRV_ENCX24J600_RD_EPMM2;


typedef struct 
{
    uint16_t EPMM3;
}  DRV_ENCX24J600_RD_EPMM3;


typedef struct 
{
    uint16_t EPMM4;
}  DRV_ENCX24J600_RD_EPMM4;


typedef struct 
{
    uint16_t EPMO;
}  DRV_ENCX24J600_RD_EPMO;


typedef struct  __attribute__((packed))
{
    uint8_t ERXDATA;
    unsigned :8;
}  DRV_ENCX24J600_RD_ERXDATA;


typedef struct  __attribute__((packed))
{
    unsigned BCEN:1;
    unsigned MCEN:1;
    unsigned NOTMEEN:1;
    unsigned UCEN:1;
    unsigned RUNTEN:1;
    unsigned RUNTEEN:1;
    unsigned CRCEN:1;
    unsigned CRCEEN:1;
    unsigned PMEN:4;
    unsigned NOTPM:1;
    unsigned :1;
    unsigned MPEN:1;
    unsigned HTEN:1;
}  DRV_ENCX24J600_RD_ERXFCON;


typedef struct  __attribute__((packed))
{
    uint16_t ERXTAIL;   // 15 bits only
}  DRV_ENCX24J600_RD_ERXHEAD;


typedef struct  __attribute__((packed))
{
    unsigned ERXRDPT:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_ERXRDPT;


typedef struct  __attribute__((packed))
{
    unsigned ERXST:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_ERXST;


typedef struct  __attribute__((packed))
{
    uint16_t ERXTAIL;   // 15 bits only
}  DRV_ENCX24J600_RD_ERXTAIL;


typedef struct 
{
    uint8_t RXEWM;
    uint8_t RXFWM;
}  DRV_ENCX24J600_RD_ERXWM;


typedef struct  __attribute__((packed))
{
    unsigned ERXWRPT:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_ERXWRPT;


typedef struct  __attribute__((packed))
{
    unsigned PKTCNT:8;
    unsigned PHYLNK:1;
    unsigned :1;
    unsigned PHYDPX:1;
    unsigned :1;
    unsigned CLKRDY:1;
    unsigned RXBUSY:1;
    unsigned FCIDLE:1;
    unsigned INT:1;
}  DRV_ENCX24J600_RD_ESTAT;


typedef struct  __attribute__((packed))
{
    unsigned ETXLEN:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_ETXLEN;


typedef struct  __attribute__((packed))
{
    unsigned ETXST:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_ETXST;


typedef struct  __attribute__((packed))
{
    unsigned COLCNT:4;
    unsigned CRCBAD:1;
    unsigned :2;
    unsigned DEFER:1;
    unsigned EXDEFER:1;
    unsigned MAXCOL:1;
    unsigned LATECOL:1;
    unsigned :5;
}  DRV_ENCX24J600_RD_ETXSTAT;


typedef struct 
{
    uint16_t ETXWIRE;
}  DRV_ENCX24J600_RD_ETXWIRE;


typedef struct  __attribute__((packed))
{
    unsigned EUDADATA;
    unsigned :8;
}  DRV_ENCX24J600_RD_EUDADATA;


typedef struct  __attribute__((packed))
{
    unsigned EUDAND:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_EUDAND;


typedef struct  __attribute__((packed))
{
    unsigned EUDARDPT:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_EUDARDPT;


typedef struct  __attribute__((packed))
{
    unsigned EUDAST:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_EUDAST;


typedef struct  __attribute__((packed))
{
    unsigned EUDAWRPT:15;
    unsigned :1;
}  DRV_ENCX24J600_RD_EUDAWRPT;


typedef struct 
{
    uint16_t MAADR1;
}  DRV_ENCX24J600_RD_MAADR1;


typedef struct 
{
    uint16_t MAADR2;
}  DRV_ENCX24J600_RD_MAADR2;


typedef struct 
{
    uint16_t MAADR3;
}  DRV_ENCX24J600_RD_MAADR3;


typedef struct  __attribute__((packed))
{
    unsigned BBIPG:7;
    unsigned :9;
}  DRV_ENCX24J600_RD_MABBIPG;


typedef struct  __attribute__((packed))
{
    unsigned MAXRET:4;
    unsigned :12;
}  DRV_ENCX24J600_RD_MACLCON;


typedef struct  __attribute__((packed))
{
    unsigned :1;
    unsigned PASSALL:1;
    unsigned RXPAUS:1;
    unsigned :1;
    unsigned LOOPBK:1;
    unsigned :11;
}  DRV_ENCX24J600_RD_MACON1;


typedef struct  __attribute__((packed))
{
    unsigned FULDPX:1;
    unsigned :1;
    unsigned HFRMEN:1;
    unsigned PHDREN:1;
    unsigned TXCRCEN:1;
    unsigned PADCFG:3;
    unsigned :4;
    unsigned NOBKOFF:1;
    unsigned BPEN:1;
    unsigned DEFER:1;
    unsigned :1;
}  DRV_ENCX24J600_RD_MACON2;


typedef struct  __attribute__((packed))
{
    unsigned IPG:7;
    unsigned :9;
}  DRV_ENCX24J600_RD_MAIPG;


typedef struct 
{
    uint16_t MAMXFL;
}  DRV_ENCX24J600_RD_MAMXFL;


typedef struct  __attribute__((packed))
{
    unsigned MIIRD:1;
    unsigned MIISCAN:1;
    unsigned :14;
}  DRV_ENCX24J600_RD_MICMD;


typedef struct 
{
    uint16_t MIRD;
}  DRV_ENCX24J600_RD_MIRD;


typedef struct  __attribute__((packed))
{
    unsigned PHREG:5;
    unsigned :3;
    unsigned writeAsOne:1;
    unsigned :7;
}  DRV_ENCX24J600_RD_MIREGADR;


typedef struct  __attribute__((packed))
{
    unsigned BUSY:1;
    unsigned SCAN:1;
    unsigned NVALID:1;
    unsigned :13;
}  DRV_ENCX24J600_RD_MISTAT;


typedef struct 
{
    uint16_t MIWR;
}  DRV_ENCX24J600_RD_MIWR;


typedef struct  __attribute__((packed))
{
    unsigned ADIEEE:5;
    unsigned AD10:1;
    unsigned AD10FD:1;
    unsigned AD100:1;
    unsigned AD100FD:1;
    unsigned unnamed_9:1;
    unsigned ADPAUS:2;
    unsigned unnamed_12:1;
    unsigned ADFAULT:1;
    unsigned unnamed_14:1;
    unsigned ADNP:1;
}  DRV_ENCX24J600_RD_PHANA;


typedef struct  __attribute__((packed))
{
    unsigned LPANABL:1;
    unsigned LPAPCD:1;
    unsigned :2;
    unsigned PDFLT:1;
    unsigned :11;
}  DRV_ENCX24J600_RD_PHANE;


typedef struct  __attribute__((packed))
{
    unsigned LPIEEE:5;
    unsigned LP10:1;
    unsigned LP10FD:1;
    unsigned LP100:1;
    unsigned LP100FD:1;
    unsigned LP100T4:1;
    unsigned LPPAUS:2;
    unsigned :1;
    unsigned LPFAULT:1;
    unsigned LPACK:1;
    unsigned LPNP:1;
}  DRV_ENCX24J600_RD_PHANLPA;


typedef struct  __attribute__((packed))
{
    unsigned :8;
    unsigned PFULDPX:1;
    unsigned REGNEG:1;
    unsigned :1;
    unsigned PSLEEP:1;
    unsigned ANEN:1;
    unsigned SPD100:1;
    unsigned PLOOPBK:1;
    unsigned PRST:1;
}  DRV_ENCX24J600_RD_PHCON1;


typedef struct  __attribute__((packed))
{
    unsigned :1;
    unsigned EDSTAT:1;
    unsigned FRCLNK:1;
    unsigned :8;
    unsigned EDTHRES:1;
    unsigned :1;
    unsigned EDPWRDN:1;
    unsigned :2;
}  DRV_ENCX24J600_RD_PHCON2;


typedef struct  __attribute__((packed))
{
    unsigned EXTREGS:1;
    unsigned :1;
    unsigned LLSTAT:1;
    unsigned ANABLE:1;
    unsigned LRFAULT:1;
    unsigned ANDONE:1;
    unsigned :5;
    unsigned HALF10:1;
    unsigned FULL10:1;
    unsigned HALF100:1;
    unsigned FULL100:1;
    unsigned :1;
}  DRV_ENCX24J600_RD_PHSTAT1;


typedef struct  __attribute__((packed))
{
    unsigned :4;
    unsigned PLRITY:1;
    unsigned :11;
}  DRV_ENCX24J600_RD_PHSTAT2;


typedef struct  __attribute__((packed))
{
    unsigned :2;
    unsigned SPDDPX:3;
    unsigned :11;
}  DRV_ENCX24J600_RD_PHSTAT3;


typedef union 
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


typedef union
{
    uint8_t val[8];
    struct  __attribute__((packed))
    {
        unsigned pNextPacket:16;
        unsigned rxByteCount:16;
        unsigned prevIgnored:1;
        unsigned :1;
        unsigned carrierEventPrevSeen:1;
        unsigned :1;
        unsigned crcError:1;
        unsigned lenChkError:1;
        unsigned lenOutOfRange:1;
        unsigned rxOk:1;
        unsigned rxMultcast:1;
        unsigned rxBcast:1;
        unsigned dribbleNibble:1;
        unsigned rxCtrlFrm:1;
        unsigned rxPauseCtrlFrm:1;
        unsigned rxUkwnOpCode:1;
        unsigned rxVLAN:1;
        unsigned runtFltrMtch:1;
        unsigned notMeFltrMtch:1;
        unsigned hashFltrMtch:1;
        unsigned magicPktFltrMtch:1;
        unsigned patternFltrMtch:1;
        unsigned unicastFltrMtch:1;
        unsigned :11;
    };
}DRV_ENCx24J600_RSV;

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
    DRV_ENCX24J600_SPI_B0SEL = 0xC0,
    /**
     * Bank 1 Select
     */
    DRV_ENCX24J600_SPI_B1SEL = 0xC2,
    /**
     * Bank 2 Select
     */
    DRV_ENCX24J600_SPI_B2SEL = 0xC4,
    /**
     * Bank 3 Select
     */
    DRV_ENCX24J600_SPI_B3SEL = 0xC6,
    /**
     * System Reset
     */
    DRV_ENCX24J600_SPI_SETETHRST = 0xCA,
    /**
     * Flow Control Disable
     */
    DRV_ENCX24J600_SPI_FCDISABLE = 0xE0,
    /**
     * Flow Control Single
     */
    DRV_ENCX24J600_SPI_FCSINGLE = 0xE2,
    /**
     * Flow Control Multiple
     */
    DRV_ENCX24J600_SPI_FCMULTIPLE = 0xE4,
    /**
     * Flow Control Clear
     */
    DRV_ENCX24J600_SPI_FCCLEAR = 0xE6,
    /**
     * Decrement Packet Counter
     */
    DRV_ENCX24J600_SPI_SETPKTDEC = 0xCC,
    /**
     * DMA Stop
     */
    DRV_ENCX24J600_SPI_DMASTOP = 0xD2,
    /**
     * DMA Start Checksum
     */
    DRV_ENCX24J600_SPI_DMACKSUM = 0xD8,
    /**
     * DMA Start Checksum with Seed
     */
DRV_ENCX24J600_SPI_DMACKSUMS = 0xDA,
    /**
     * DMA Start Copy
     */
    DRV_ENCX24J600_SPI_DMACOPY = 0xDC,
    /**
     * DMA Start Copy and Checksum with Seed
     */
    DRV_ENCX24J600_SPI_DMACOPYS = 0xDE,
    /**
     * Request Packet Transmission
     */
    DRV_ENCX24J600_SPI_SETTXRTS = 0xD4,
    /**
     * Enable RX
     */
    DRV_ENCX24J600_SPI_ENABLERX = 0xE8,
    /**
     * Disable RX
     */
    DRV_ENCX24J600_SPI_DISABLERX = 0xEA,
    /**
     * Enable Interrupts
     */
    DRV_ENCX24J600_SPI_SETEIE = 0xEC,
    /**
     * Disable Interrupts
     */
    DRV_ENCX24J600_SPI_CLREIE = 0xEE,
    /**
     * Read Bank Select
     */
    DRV_ENCX24J600_SPI_RBSEL = 0xC8,
    /**
     * Write EGPRDPT
     */
    DRV_ENCX24J600_SPI_WGPRDPT = 0x60,
    /**
     * Read EGPRDPT
     */
    DRV_ENCX24J600_SPI_RGPRDPT = 0x62,
    /**
     * Write ERXRDPT
     */
    DRV_ENCX24J600_SPI_WRXRDPT = 0x64,
    /**
     * Read ERXRDPT
     */
    DRV_ENCX24J600_SPI_RRXRDPT = 0x66,
    /**
     * Write EUDARDPT
     */
    DRV_ENCX24J600_SPI_WUDARDPT = 0x68,
    /**
     * Read EUDARDPT
     */
    DRV_ENCX24J600_SPI_RUDARDPT = 0x6A,
    /**
     * Write EGPWRPT
     */
    DRV_ENCX24J600_SPI_WGPWRPT = 0x6C,
    /**
     * Read EGPWRPT
     */
    DRV_ENCX24J600_SPI_RGPWRPT = 0x6E,
    /**
     * Write ERXWRPT
     */
    DRV_ENCX24J600_SPI_WRXWRPT = 0x70,
    /**
     * Read ERXWRPT
     */
    DRV_ENCX24J600_SPI_RRXWRPT = 0x72,
    /**
     * Write EUDAWRPT
     */
    DRV_ENCX24J600_SPI_WUDAWRPT = 0x74,
    /**
     * Read EUDAWRPT
     */
    DRV_ENCX24J600_SPI_RUDAWRPT = 0x76,
    /**
     * Read Control Register
     */
    DRV_ENCX24J600_SPI_RCR = 0x00,
    /**
     * Write Control Register
     */
    DRV_ENCX24J600_SPI_WCR = 0x40,
    /**
     * Read Control Register Unbanked
     */
    DRV_ENCX24J600_SPI_RCRU = 0x20,
    /**
     * Write Control Register Unbanked
     */
    DRV_ENCX24J600_SPI_WCRU = 0x22,
    /**
     * Bit Field Set
     */
    DRV_ENCX24J600_SPI_BFS = 0x80,
    /**
     * Bit Field Clear
     */
    DRV_ENCX24J600_SPI_BFC = 0xA0,
    /**
     * Bit Field Set Unbanked
     */
    DRV_ENCX24J600_SPI_BFSU = 0x24,
    /**
     * Bit Field Clear Unbanked
     */
    DRV_ENCX24J600_SPI_BFCU = 0x26,
    /**
     * Read EGPDATA
     */
    DRV_ENCX24J600_SPI_RGPDATA = 0x28,
    /**
     * Write EGPDATA
     */
    DRV_ENCX24J600_SPI_WGPDATA = 0x2A,
    /**
     * Read ERXDATA
     */
    DRV_ENCX24J600_SPI_RRXDATA = 0x2C,
    /**
     * Write ERXDATA
     */
    DRV_ENCX24J600_SPI_WRXDATA = 0x2E,
    /**
     * Read EUDADATA
     */
    DRV_ENCX24J600_SPI_RUDADATA = 0x30,
    /**
     * Write EUDADATA
     */
    DRV_ENCX24J600_SPI_WUDADATA = 0x32
} DRV_ENCX24J600_SPI_SET;

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
 
