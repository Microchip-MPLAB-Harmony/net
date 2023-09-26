/*******************************************************************************
  Ethernet Driver Library Definition

  Company:
    Microchip Technology Inc.
    
  FIle Name:
    drv_gmac_lib.h

  Summary:
    This file contains the Application Program Interface (API) definition  for
    the Ethernet Driver library.

  Description:
    This library provides a low-level abstraction of the Ethernet module
    on Microchip PIC32C family microcontrollers with a convenient C language
    interface.  It can be used to simplify low-level access to the module
    without the necessity of interacting directly with the module's registers,
    thus hiding differences from one microcontroller variant to another.
*******************************************************************************/
//DOM-IGNORE-BEGIN
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

//DOM-IGNORE-END


#ifndef _DRV_PIC32CGMAC_LIB_SAM_H_
#define _DRV_PIC32CGMAC_LIB_SAM_H_

#include <stdlib.h>

/******************************************************************************/

/* Ethernet Event Flags

  Summary:
    Ethernet event flags

  Description:
    This enumeration defines flags for the possible Ethernet events that can
    cause interrupts.
*/
typedef enum{
    // All events
    GMAC_EV_NONE
    /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,
    // Management Frame Sent
    GMAC_EV_MGMTFRAMESENT
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_MFS_Msk /*DOM-IGNORE-END*/,
    
    // Receive Complete
    GMAC_EV_RXCOMPLETE
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_RCOMP_Msk /*DOM-IGNORE-END*/,
    
    // RX Used Bit Read
    GMAC_EV_RXUSEDBITREAD
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_RXUBR_Msk /*DOM-IGNORE-END*/,
    
    // TX Used Bit Read
    GMAC_EV_TXUSEDBITREAD
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_TXUBR_Msk /*DOM-IGNORE-END*/,
    
    // Transmit Underrun
    GMAC_EV_TXUNDERRUN
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_TUR_Msk /*DOM-IGNORE-END*/,
    
    // Retry Limit Exceeded
    GMAC_EV_RETRYLIMITEXCEED
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_RLEX_Msk /*DOM-IGNORE-END*/,
    
    // Transmit Frame Corruption
    GMAC_EV_TXFRAMECORRUPT
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_TFC_Msk /*DOM-IGNORE-END*/,
    
    // Transmit Complete
    GMAC_EV_TXCOMPLETE
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_TCOMP_Msk /*DOM-IGNORE-END*/,

    // Receive Overrun
    GMAC_EV_RXOVERRUN
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_ROVR_Msk /*DOM-IGNORE-END*/,

    // HRESP Not OK
    GMAC_EV_HRESPNOTOK
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_HRESP_Msk /*DOM-IGNORE-END*/,

    // Pause Frame with Non-zero Pause Quantum Received
    GMAC_EV_PAUSEFRAMENZRECV
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_PFNZ_Msk /*DOM-IGNORE-END*/,

    // Pause Time Zero
    GMAC_EV_PAUSETIMEZERO
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_PTZ_Msk /*DOM-IGNORE-END*/,

    // Pause Frame Transmitted
    GMAC_EV_PAUSEFRAMETX
    /*DOM-IGNORE-BEGIN*/ = GMAC_ISR_PFTR_Msk /*DOM-IGNORE-END*/,
    // All events
    GMAC_EV_ALL
    /*DOM-IGNORE-BEGIN*/= ( GMAC_EV_MGMTFRAMESENT | GMAC_EV_RXCOMPLETE |  GMAC_EV_RXUSEDBITREAD |
    GMAC_EV_TXUNDERRUN | GMAC_EV_RETRYLIMITEXCEED | GMAC_EV_TXFRAMECORRUPT |
    GMAC_EV_TXCOMPLETE | GMAC_EV_RXOVERRUN | GMAC_EV_HRESPNOTOK |
    GMAC_EV_PAUSEFRAMENZRECV | GMAC_EV_PAUSETIMEZERO | GMAC_EV_PAUSEFRAMETX ) /*DOM-IGNORE-END*/,
    // RX events
    GMAC_EV_RX_ALL
    /*DOM-IGNORE-BEGIN*/= (GMAC_EV_RXCOMPLETE | GMAC_EV_RXUSEDBITREAD | GMAC_EV_RXOVERRUN) /*DOM-IGNORE-END*/, 
    // TX events
    GMAC_EV_TX_ALL
    /*DOM-IGNORE-BEGIN*/= (GMAC_EV_TXCOMPLETE | GMAC_EV_TXFRAMECORRUPT | GMAC_EV_TXUNDERRUN) /*DOM-IGNORE-END*/ 
} GMAC_EVENTS;

// *****************************************************************************

/* TCPIP Stack Event Descriptor

  Summary:

  Description:

  Remarks:
    None
*/
typedef struct
{
    TCPIP_MAC_EVENT             _TcpEnabledEvents;          // group enabled notification events
    volatile TCPIP_MAC_EVENT    _TcpPendingEvents;          // group notification events that are set, waiting to be re-acknowledged
    GMAC_EVENTS                 _EthEnabledEvents;          // copy in GMAC_EVENTS space
    volatile GMAC_EVENTS        _EthPendingEvents;          // copy in GMAC_EVENTS space
    TCPIP_MAC_EventF            _TcpNotifyFnc;              // group notification handler
    const void*                 _TcpNotifyParam;            // notification parameter
}DRV_GMAC_EVENT_DCPT;   // event descriptor per group

/// The buffer addresses written into the descriptors must be aligned so the
/// last few bits are zero.  These bits have special meaning for the GMAC
/// peripheral and cannot be used as part of the address.
#define GMAC_RX_ADDRESS_MASK    ((unsigned int)0xFFFFFFFC)
#define GMAC_LENGTH_FRAME       ((unsigned int)0x3FFF)    /// Length of frame mask
#define GMAC_RX_CHECKSUM_OFFLOAD_STATUS     ((unsigned int)0x00C00000)
#define GMAC_RX_CHECKSUM_IP_OFFLOAD         ((unsigned int)0x00400000)
#define GMAC_RX_CHECKSUM_IP_TCP_OFFLOAD     ((unsigned int)0x00800000)
#define GMAC_RX_CHECKSUM_IP_UDP_OFFLOAD     ((unsigned int)0x00C00000)

// receive buffer descriptor bits
#define GMAC_RX_OWNERSHIP_BIT   (1u <<  0)
#define GMAC_RX_WRAP_BIT        (1u <<  1)
#define GMAC_RX_SOF_BIT         (1u << 14)
#define GMAC_RX_EOF_BIT         (1u << 15)

// Transmit buffer descriptor bits
#define GMAC_TX_LAST_BUFFER_BIT (1u << 15)
#define GMAC_TX_WRAP_BIT        (1u << 30)
#define GMAC_TX_USED_BIT        (1u << 31)
#define GMAC_TX_RLE_BIT         (1u << 29) /// Retry Limit Exceeded
#define GMAC_TX_LCOL_BIT        (1u << 28) /// Tx error due to late collision
#define GMAC_TX_AHB_ERR_BIT     (1u << 27) /// Frame corruption due to AHB Error
#define GMAC_TX_ERR_BITS        (GMAC_TX_RLE_BIT | GMAC_TX_LCOL_BIT | GMAC_TX_AHB_ERR_BIT)

/* Ethernet Receive Filter Flags

  Summary:
    Defines the receive filter flags

  Description:
    This enumeration defines the receive filter flags.

  Remarks:
    Multiple values can be OR-ed together.

    The values in this enumeration are displayed in the order of priority
    that the receive filter state machine works, with the highest priority first.
    Once a filter accepts or rejects a packet, further filters are not tried.
    If a packet isn't rejected/accepted after all filters are tried, it will be rejected by
    default!
*/
typedef enum{
    // Frames with wrong Preamble are accepted
    GMAC_FILT_PREAMBLE_ERR_ACCEPT
    /*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_RXBP_Msk /*DOM-IGNORE-END*/,

    // Frames with wrong CRC are accepted
    GMAC_FILT_CRC_ERR_ACCEPT
    /*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_IRXFCS_Msk /*DOM-IGNORE-END*/,

    // Frames with Maximum frame size accepted
    GMAC_FILT_MAXFRAME_ACCEPT
    /*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_MAXFS_Msk /*DOM-IGNORE-END*/,

    // Frames with Unicast Hash match accepted
    GMAC_FILT_UCASTHASH_ACCEPT
    /*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_UNIHEN_Msk /*DOM-IGNORE-END*/,
    
    // Frames with Multicast Hash match accepted
    GMAC_FILT_MCASTHASH_ACCEPT
    /*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_MTIHEN_Msk /*DOM-IGNORE-END*/,
    
    // All Broadcast rejected
    GMAC_FILT_BCAST_REJECT
    /*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_NBC_Msk /*DOM-IGNORE-END*/,
    
    // All valid frames accepted
    GMAC_FILT_ALLFRAME_ACCEPT
    /*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_CAF_Msk /*DOM-IGNORE-END*/,
    
    // Jumbo frames accepted
    GMAC_FILT_JUMBOFRAME_ACCEPT
    /*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_JFRAME_Msk /*DOM-IGNORE-END*/,    
    
    GMAC_FILT_ALL_FILTERS
    /*DOM-IGNORE-BEGIN*/    =   GMAC_FILT_PREAMBLE_ERR_ACCEPT   |   GMAC_FILT_CRC_ERR_ACCEPT    |
                                GMAC_FILT_MAXFRAME_ACCEPT       |   GMAC_FILT_UCASTHASH_ACCEPT  |
                                GMAC_FILT_MCASTHASH_ACCEPT      |   GMAC_FILT_BCAST_REJECT      |
                                GMAC_FILT_ALLFRAME_ACCEPT       |   GMAC_FILT_ALLFRAME_ACCEPT   |
                                GMAC_FILT_JUMBOFRAME_ACCEPT /*DOM-IGNORE-END*/
} GMAC_RX_FILTERS;


// Interrupt bits
// All interrupts
#define GMAC_INT_ALL 0xFFFFFFFF
// RX Interrupts
#define GMAC_INT_RX_BITS (GMAC_IER_RCOMP_Msk  | GMAC_IER_RXUBR_Msk  | GMAC_IER_ROVR_Msk )
// TX err interrupts
#define GMAC_INT_TX_ERR_BITS (GMAC_IER_TUR_Msk  | GMAC_IER_RLEX_Msk  | GMAC_IER_TFC_Msk  | GMAC_IER_HRESP_Msk)
// TX interrupts
#define GMAC_INT_TX_BITS  (GMAC_INT_TX_ERR_BITS)
// Interrupt Status bits
#define GMAC_INT_RX_STATUS_BITS  (GMAC_ISR_RCOMP_Msk  | GMAC_ISR_RXUBR_Msk  | GMAC_ISR_ROVR_Msk )
#define GMAC_INT_TX_STATUS_ERR_BITS  (GMAC_ISR_TUR_Msk  | GMAC_ISR_RLEX_Msk  | GMAC_ISR_TFC_Msk  | GMAC_ISR_HRESP_Msk )

#endif  // _DRV_PIC32CGMAC_LIB_H_

