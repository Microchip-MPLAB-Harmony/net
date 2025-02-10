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
/*
Copyright (C) 2015-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef H_DRV_ENC28J60_DS_DEF_H_
#define H_DRV_ENC28J60_DS_DEF_H_

#include "system_config.h"

#define DRV_ENC28J60_MEM_SIZE 8192U

typedef struct __attribute__((packed))
{
    unsigned BSEL:2;
    unsigned RXEN:1;
    unsigned TXRTS:1;
    unsigned CSUMEN:1;
    unsigned DMAST:1;
    unsigned RXRST:1;
    unsigned TXRST:1;
}  DRV_ENC28J60_RD_ECON1;


typedef struct __attribute__((packed))
{
    unsigned :3;
    unsigned VRPS:1;
    unsigned :1;
    unsigned PWRSV:1;
    unsigned PKTDEC:1;
    unsigned AUTOINC:1;
}  DRV_ENC28J60_RD_ECON2;

typedef struct 
{
    uint16_t EDMACS;
}  DRV_ENC28J60_RD_EDMACS;

typedef struct __attribute__((packed))
{
    unsigned ERDPT:13;
    unsigned :3;
}  DRV_ENC28J60_RD_ERDPT;

typedef struct __attribute__((packed))
{
    unsigned EWRPT:13;
    unsigned :3;
}  DRV_ENC28J60_RD_EWRPT;

typedef struct __attribute__((packed))
{
    unsigned ETXND:13;
    unsigned :3;
}  DRV_ENC28J60_RD_ETXND;

typedef struct __attribute__((packed))
{
    unsigned ERXND:13;
    unsigned :3;
}  DRV_ENC28J60_RD_ERXND;


typedef struct __attribute__((packed))
{
    unsigned EDMAND:13;
    unsigned :3;
}  DRV_ENC28J60_RD_EDMAND;

typedef struct __attribute__((packed))
{
    unsigned EDMADST:13;
    unsigned :3;
}  DRV_ENC28J60_RD_EDMADST;


typedef struct __attribute__((packed))
{
    unsigned EDMAST:13;
    unsigned :3;
}  DRV_ENC28J60_RD_EDMAST;



typedef struct 
{
    uint8_t EHT0;
}  DRV_ENC28J60_RD_EHT0;


typedef struct 
{
    uint8_t EHT1;
}  DRV_ENC28J60_RD_EHT1;


typedef struct 
{
    uint8_t EHT2;
}  DRV_ENC28J60_RD_EHT2;


typedef struct 
{
    uint8_t EHT3;
}  DRV_ENC28J60_RD_EHT3;


typedef struct 
{
    uint8_t EHT4;
}  DRV_ENC28J60_RD_EHT4;


typedef struct 
{
    uint8_t EHT5;
}  DRV_ENC28J60_RD_EHT5;


typedef struct 
{
    uint8_t EHT6;
}  DRV_ENC28J60_RD_EHT6;


typedef struct 
{
    uint8_t EHT7;
}  DRV_ENC28J60_RD_EHT7;

typedef struct __attribute__((packed))
{
    /**
     * <b>RXERIE: </b>Receive Error Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned RXERIE:1;
    /**
     * <b>TXERIE: </b>Transmit Error Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned TXERIE:1;
    /**
     * <b>Reserved: </b>Ignore on read, don't care on write<b>(1)</b>
     */
    unsigned :1;
    /**
     * <b>TXIE: </b>Transmit Done Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned TXIE:1;
    /**
     * <b>LINKIE: </b>PHY Link Status Change Interrupt Enable bit
     * 1 = Enabled
     * 0 = Disabled
     */
    unsigned LINKIE:1;
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
     * <b>INTIE: </b>INT Global Interrupt Enable bit
     * 1 = INT pin is controlled by the INT status bit (ESTAT<15>)
     * 0 = INT pin is driven high
     */
    unsigned INTIE:1;
}  DRV_ENC28J60_RD_EIE;


typedef struct __attribute__((packed))
{

    
    /**
     * <b>RXERIF: </b>Receive Error Interrupt Flag bit
     * 1 = A packet was aborted because there is insufficient buffer space 
     * or the packet count is 255
     * 0 = No receive error interrupt is pending
     */
    unsigned RXERIF:1;
    /**
     * <b>TXERIF: </b>Transmit Error Interrupt Flag bit
     * 1 = A transmit error has occurred 
     * 0 = No transmit error has occurred
     */
    unsigned TXERIF:1;
    /**
    * <b>Reserved: </b>Ignore on read, don't care on write<b>(1)</b>
    */
    unsigned :1;
    /**
     * <b>TXIF:: </b>Transmit Interrupt Flag bit
     * 1 = Transmit request has ended
     * 0 = No transmit interrupt is pending
     */
    unsigned TXIF:1;
    /**
     * <b>LINKIF:: </b>Link Change Interrupt Flag bit
     * 1 = PHY reports that the link status has changed; read PHIR register to clear
     * 0 = Link status has not changed
     */
    unsigned LINKIF:1;
    /**
     * <b>DMAIF:: </b>DMA Interrupt Flag bit
     * 1 = DMA copy or checksum calculation has completed
     * 0 = No DMA interrupt is pending
     */
    unsigned DMAIF:1;
    /**
     * <b>PKTIF:: </b>Receive Packet Pending Interrupt Flag bit
     * 1 = Receive buffer contains one or more unprocessed packets; cleared when PKTDEC is set
     * 0 = Receive buffer is empty
     */
    unsigned PKTIF:1;
    
    /**
    * <b>Unimplemented: </b>Unimplemented bit. Read as '0')</b>
    */
    unsigned :1;
}  DRV_ENC28J60_RD_EIR;


typedef struct 
{
    uint16_t EPAUS;
}  DRV_ENC28J60_RD_EPAUS;


typedef struct 
{
    uint16_t EPMCS;
}  DRV_ENC28J60_RD_EPMCS;


typedef struct 
{
    uint8_t EPMM0;
}  DRV_ENC28J60_RD_EPMM0;

typedef struct 
{
    uint8_t EPMM1;
}  DRV_ENC28J60_RD_EPMM1;

typedef struct 
{
    uint8_t EPMM2;
}  DRV_ENC28J60_RD_EPMM2;

typedef struct 
{
    uint8_t EPMM3;
}  DRV_ENC28J60_RD_EPMM3;

typedef struct 
{
    uint8_t EPMM4;
}  DRV_ENC28J60_RD_EPMM4;

typedef struct 
{
    uint8_t EPMM5;
}  DRV_ENC28J60_RD_EPMM5;

typedef struct 
{
    uint8_t EPMM6;
}  DRV_ENC28J60_RD_EPMM6;

typedef struct 
{
    uint8_t EPMM7;
}  DRV_ENC28J60_RD_EPMM7;


typedef struct __attribute__((packed))
{
    unsigned EPMO:13;
    unsigned :3;
}  DRV_ENC28J60_RD_EPMO;

typedef struct __attribute__((packed))
{
    unsigned BCEN:1;
    unsigned MCEN:1;
    unsigned HTEN:1;
    unsigned MPEN:1;
    unsigned PMEN:1;
    unsigned CRCEN:1;
    unsigned ANDOR:1;
    unsigned UCEN:1;
}  DRV_ENC28J60_RD_ERXFCON;


typedef struct __attribute__((packed))
{
    unsigned ERXRDPT:13;
    unsigned :3;
}  DRV_ENC28J60_RD_ERXRDPT;


typedef struct __attribute__((packed))
{
    unsigned ERXST:13;
    unsigned :3;
}  DRV_ENC28J60_RD_ERXST;

typedef struct __attribute__((packed))
{
    unsigned ERXWRPT:13;
    unsigned :3;
}  DRV_ENC28J60_RD_ERXWRPT;


typedef struct __attribute__((packed))
{
    unsigned CLKRDY:1;
    unsigned TXABRT:1;
    unsigned RXBUSY:1;
    unsigned :1;
    unsigned LATECOL:1;
    unsigned :1; 
    unsigned BUFER:1;
    unsigned INT:1;
}  DRV_ENC28J60_RD_ESTAT;



typedef struct __attribute__((packed))
{
    unsigned ETXST:13;
    unsigned :3;
}  DRV_ENC28J60_RD_ETXST;

typedef struct 
{
    uint8_t MAADR1;
}  DRV_ENC28J60_RD_MAADR1;

typedef struct 
{
    uint8_t MAADR2;
}  DRV_ENC28J60_RD_MAADR2;

typedef struct 
{
    uint8_t MAADR3;
}  DRV_ENC28J60_RD_MAADR3;

typedef struct 
{
    uint8_t MAADR4;
} DRV_ENC28J60_RD_MAADR4;

typedef struct 
{
    uint8_t MAADR5;
} DRV_ENC28J60_RD_MAADR5;

typedef struct 
{
    uint8_t MAADR6;
} DRV_ENC28J60_RD_MAADR6;


typedef struct __attribute__((packed))
{
    unsigned BBIPG:7;
    unsigned :1;
}  DRV_ENC28J60_RD_MABBIPG;


typedef struct __attribute__((packed))
{
    unsigned RETMAX:4;
    unsigned COLWIN:6;
}  DRV_ENC28J60_RD_MACLCON;


typedef struct __attribute__((packed))
{
    unsigned MARXEN:1;
    unsigned PASSALL:1;
    unsigned RXPAUS:1;
    unsigned TXPAUS:1;
    unsigned :4;
}  DRV_ENC28J60_RD_MACON1;


typedef struct __attribute__((packed))
{
    unsigned FULDPX:1;
    unsigned FRMLNEN:1;
    unsigned HFRMEN:1;
    unsigned PHDREN:1;
    unsigned TXCRCEN:1;
    unsigned PADCFG:3;
}  DRV_ENC28J60_RD_MACON3;


typedef struct __attribute__((packed))
{
    unsigned :4;
    unsigned NOBKOFF:1;
    unsigned BPEN:1;
    unsigned DEFER:1;
    unsigned :1;
}  DRV_ENC28J60_RD_MACON4;


typedef struct __attribute__((packed))
{
    unsigned IPGL:7;
    unsigned IPGH:7;
}  DRV_ENC28J60_RD_MAIPG;


typedef struct 
{
    uint16_t MAMXFL;
}  DRV_ENC28J60_RD_MAMXFL;


typedef struct __attribute__((packed))
{
    unsigned MIIRD:1;
    unsigned MIISCAN:1;
    unsigned :6;
}  DRV_ENC28J60_RD_MICMD;


typedef struct 
{
    uint16_t MIRD;
}  DRV_ENC28J60_RD_MIRD;


typedef struct __attribute__((packed))
{
    unsigned MIREGADR:5;
}  DRV_ENC28J60_RD_MIREGADR;


typedef struct __attribute__((packed))
{
    unsigned BUSY:1;
    unsigned SCAN:1;
    unsigned NVALID:1;
    unsigned :5; 
}  DRV_ENC28J60_RD_MISTAT;


typedef struct 
{
    uint16_t MIWR;
}  DRV_ENC28J60_RD_MIWR;

typedef struct __attribute__((packed))
{
    unsigned :8;
    unsigned PDPXMD:1;
    unsigned :2;
    unsigned PPWRSV:1;
    unsigned :2;
    unsigned PLOOPBK:1;
    unsigned PRST:1;
}  DRV_ENC28J60_RD_PHCON1;


typedef struct __attribute__((packed))
{
    unsigned :8;
    unsigned HDLDIS:1;
    unsigned :1;
    unsigned JABBER:1;
    unsigned :2;
    unsigned TXDIS:1;
    unsigned FRCLNK:1;
    unsigned :1;
}  DRV_ENC28J60_RD_PHCON2;


typedef struct __attribute__((packed))
{
    unsigned :1;
    unsigned JBSTAT:1;
    unsigned LLSTAT:1;
    unsigned :8;

    unsigned PHDPX:1;
    unsigned PFDPX:1;
    unsigned :3;
    
}  DRV_ENC28J60_RD_PHSTAT1;


typedef struct __attribute__((packed))
{
    unsigned :5;
    unsigned PLRITY:1;
    unsigned :3;
    unsigned DPXSTAT:1;
    unsigned LSTAT:1;
    unsigned COLSTAT:1;
    unsigned RXSTAT:1;
    unsigned TXSTAT:1;
    unsigned :2;
}  DRV_ENC28J60_RD_PHSTAT2;

typedef struct 
{
    uint16_t PHID1;
}  DRV_ENC28J60_RD_PHID1;

typedef struct 
{
    uint16_t PHID2;
}  DRV_ENC28J60_RD_PHID2;

typedef struct __attribute__((packed))
{
    unsigned :1;
    unsigned PGEIE:1;
    unsigned :2;
    unsigned PLNKIE:1;
    unsigned :11;
}  DRV_ENC28J60_RD_PHIE;


typedef struct __attribute__((packed))
{
    unsigned :2;
    unsigned PGIF:1;
    unsigned :1;
    unsigned PLNKIF:1;
    unsigned :11;
}  DRV_ENC28J60_RD_PHIR;

typedef struct __attribute__((packed))
{
    unsigned :1;
    unsigned STRCH:1;
    unsigned LFRQ0:1;
    unsigned LFRQ1:1;
    unsigned LBCFG0:1;
    unsigned LBCFG1:1;
    unsigned LBCFG2:1;
    unsigned LBCFG3:1;
    unsigned LACFG0:1;
    unsigned LACFG1:1;
    unsigned LACFG2:1;
    unsigned LACFG3:1;
    unsigned :4;
}  DRV_ENC28J60_RD_PHLCON;

typedef struct __attribute__((packed))
{
    unsigned EREVID:5;
    unsigned :3;
}  DRV_ENC28J60_RD_EREVID;

typedef struct __attribute__((packed))
{
    unsigned COCON:3;
    unsigned :5;
}  DRV_ENC28J60_RD_ECOCON;

typedef struct __attribute__((packed))
{
    unsigned FCEN:2;
    unsigned FULDPXS:1;
    unsigned :5;
}  DRV_ENC28J60_RD_EFLOCON;

typedef struct 
{
    uint8_t EBSTSD;

}  DRV_ENC28J60_RD_EBSTSD;

typedef struct __attribute__((packed))
{
    unsigned BISTST:1;
    unsigned TME:1;
    unsigned TMSEL:2;
    unsigned PSEL:1;
    unsigned PSV:3;

}  DRV_ENC28J60_RD_EBSTCON;

typedef struct 
{
    uint16_t EBSTCS;

}  DRV_ENC28J60_RD_EBSTCS;


typedef struct 
{
    uint8_t EPKTCNT;

} DRV_ENC28J60_RD_EPKTCNT;

typedef union
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

typedef union
{
    uint8_t v[7];
    struct __attribute__((packed))
    {
        uint16_t ByteCount;
        unsigned CollisionCount:4;
        unsigned CRCError:1;
        unsigned LengthCheckError:1;
        unsigned LengthOutOfRange:1;
        unsigned Done:1;
        unsigned Multicast:1;
        unsigned Broadcast:1;
        unsigned PacketDefer:1;
        unsigned ExcessiveDefer:1;
        unsigned MaximumCollisions:1;
        unsigned LateCollision:1;
        unsigned Giant:1;
        unsigned Underrun:1;
        uint16_t BytesTransmittedOnWire;
        unsigned ControlFrame:1;
        unsigned PAUSEControlFrame:1;
        unsigned BackpressureApplied:1;
        unsigned VLANTaggedFrame:1;
        unsigned Zeros:4;
    } bits;
} TXSTATUS;


typedef union
{
    uint8_t v[6];
    struct __attribute__((packed))
    {
        unsigned pNextPacket:16;
        unsigned rxByteCount:16;
        unsigned lonDropEvent:1;
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
        unsigned rxUnknOpcode:1;
        unsigned rxVlanTypeDetected:1;
        unsigned zero:1;
    };
}DRV_ENC28J60_RSV;

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


#endif  // H_DRV_ENC28J60_DS_DEF_H_

