/*******************************************************************************
  Ethernet Driver Library Definition

  Company:
    Microchip Technology Inc.

  FIle Name:
    drv_emac_lib.h

  Summary:
    This file contains the Application Program Interface (API) definition  for
    the Ethernet Driver library.

  Description:
    This library provides a low-level abstraction of the Ethernet module
    on Microchip microcontrollers with a convenient C language
    interface.  It can be used to simplify low-level access to the module
    without the necessity of interacting directly with the module's registers,
    thus hiding differences from one microcontroller variant to another.
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2020-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#ifndef H_DRV_EMAC_LIB_H_
#define H_DRV_EMAC_LIB_H_

#include <stdlib.h>
#include "system_config.h"
#include "system/debug/sys_debug.h"
#include "system/time/sys_time.h"
#include "system/sys_time_h2_adapter.h"
#include "tcpip/tcpip_ethernet.h"
#include "driver/emac/src/dynamic/drv_emac_dcpt_lists.h"
#include "driver/emac/src/drv_emac_local.h"
#include "device.h"

// EMAC debugging levels/masks
// basic debugging
#define EMAC_DRV_DEBUG_MASK_BASIC           (0x0001)

#define EMAC_DRV_DEBUG_LEVEL                EMAC_DRV_DEBUG_MASK_BASIC

// *****************************************************************************
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
    If a packet isn't rejected/accepted after all filters are tried, it will be
    rejected by default!
*/
typedef enum
{
    MAC_FILT_NONE       = 0,        // no filter set
    // Frames with wrong Preamble are accepted
    MAC_FILT_PREAMBLE_ERR_ACCEPT    /*DOM-IGNORE-BEGIN*/ = EMAC_NCFGR_IRXFCS_Msk /*DOM-IGNORE-END*/,
    // Frames with wrong CRC are accepted
    MAC_FILT_CRC_ERR_ACCEPT         /*DOM-IGNORE-BEGIN*/ = EMAC_NCFGR_IRXFCS_Msk /*DOM-IGNORE-END*/,
    // Frames with Maximum frame size accepted
    MAC_FILT_MAXFRAME_ACCEPT        /*DOM-IGNORE-BEGIN*/ = EMAC_NCFGR_BIG_Msk /*DOM-IGNORE-END*/,
    // Frames with Unicast Hash match accepted
    MAC_FILT_UCASTHASH_ACCEPT       /*DOM-IGNORE-BEGIN*/ = EMAC_NCFGR_UNI_Msk /*DOM-IGNORE-END*/,
    // Frames with Multicast Hash match accepted
    MAC_FILT_MCASTHASH_ACCEPT       /*DOM-IGNORE-BEGIN*/ = EMAC_NCFGR_MTI_Msk /*DOM-IGNORE-END*/,
    // All Broadcast rejected
    MAC_FILT_BCAST_REJECT           /*DOM-IGNORE-BEGIN*/ = EMAC_NCFGR_NBC_Msk /*DOM-IGNORE-END*/,
    // All valid frames accepted
    MAC_FILT_ALLFRAME_ACCEPT        /*DOM-IGNORE-BEGIN*/ = EMAC_NCFGR_CAF_Msk /*DOM-IGNORE-END*/,
    // Jumbo frames accepted
    MAC_FILT_JUMBOFRAME_ACCEPT      /*DOM-IGNORE-BEGIN*/ = EMAC_NCFGR_JFRAME_Msk /*DOM-IGNORE-END*/,
    MAC_FILT_ALL_FILTERS            /*DOM-IGNORE-BEGIN*/ =  ( 0
                                                            | MAC_FILT_PREAMBLE_ERR_ACCEPT
                                                            | MAC_FILT_CRC_ERR_ACCEPT
                                                            | MAC_FILT_MAXFRAME_ACCEPT
                                                            | MAC_FILT_UCASTHASH_ACCEPT
                                                            | MAC_FILT_MCASTHASH_ACCEPT
                                                            | MAC_FILT_BCAST_REJECT
                                                            | MAC_FILT_ALLFRAME_ACCEPT
                                                            | MAC_FILT_JUMBOFRAME_ACCEPT
                                                            )
                                                            /*DOM-IGNORE-END*/
} MAC_RX_FILTERS;

/*******************************************************************************
  Summary:
    Defines the possible results of Ethernet operations that can succeed or
    fail
  Description:
    Ethernet Operation Result Codes

    This enumeration defines the possible results of any of the Ethernet
    library operations that have the possibility of failing. This result
    should be checked to ensure that the operation achieved the desired
    result.
 ******************************************************************************/
typedef enum
{
    /* Everything ok */
    MAC_DRVR_RES_OK                   /*DOM-IGNORE-BEGIN*/ =  0 /*DOM-IGNORE-END*/,
    /* Ethernet RX, TX, acknowledge packet codes */
    /* No such packet exist */
    MAC_DRVR_RES_NO_PACKET,
    /* Packet is queued (not transmitted or received and not processed) */
    MAC_DRVR_RES_PACKET_QUEUED,
    /* Not enough descriptors available */
    MAC_DRVR_RES_NO_DESCRIPTORS,
    /* Not enough nodes in Tx Queues */
    MAC_DRVR_RES_NO_TX_QUEUE,
    /* Not enough nodes in Rx Queues */
    MAC_DRVR_RES_NO_RX_QUEUE,
    /* Errors: Ethernet buffers, descriptors */
    MAC_DRVR_RES_DESC_CNT_ERR,
    /* Some memory allocation failed */
    MAC_DRVR_RES_OUT_OF_MEMORY        /*DOM-IGNORE-BEGIN*/ = -1 /*DOM-IGNORE-END*/,
    /* We don't support user space buffers. */
    MAC_DRVR_RES_USPACE_ERR           /*DOM-IGNORE-BEGIN*/ = -2 /*DOM-IGNORE-END*/,
    /* The size of the receive buffers too small */
    MAC_DRVR_RES_RX_SIZE_ERR          /*DOM-IGNORE-BEGIN*/ = -3 /*DOM-IGNORE-END*/,
    /* A received packet spans more buffers/descriptors than supplied */
    MAC_DRVR_RES_RX_PKT_SPLIT_ERR     /*DOM-IGNORE-BEGIN*/ = -4 /*DOM-IGNORE-END*/,
    MAC_DRVR_RES_EMPTY_BUFFER         /*DOM-IGNORE-BEGIN*/ = -5 /*DOM-IGNORE-END*/,
    /* Descriptor Queue Error*/
    MAC_DRVR_RES_QUEUE_ERR            /*DOM-IGNORE-BEGIN*/ = -6 /*DOM-IGNORE-END*/,
} MAC_DRVR_RESULT;

/*******************************************************************************
  Summary:
    Defines the different states of RX receive state machine
  Description:
    MAC RX engine copies frame packet to single/multiple RX buffers and update the
    status in associated RX descriptors. The RX state machine reads the different
    RX descriptors to detect a valid RX packet
*******************************************************************************/
typedef enum
{
    MAC_RX_NO_FRAME_STATE,
    MAC_RX_SOF_DETECTED_STATE,
    MAC_RX_VALID_FRAME_DETECTED_STATE
} MAC_RXFRAME_STATE;

/*******************************************************************************
  Summary:
     MAC Rx descriptor data packet attributes
  Description:
     These attributes used during the search for valid Rx packet, on Rx Interrupt.
*******************************************************************************/
typedef struct
{
    uint16_t startIndex;
    uint16_t endIndex;
    uint16_t bufferCount;
} MAC_DRVR_RX_FRAME_INFO;

/*******************************************************************************
  Ethernet Close Flags

  Summary:
    Defines the possible disable codes of Ethernet controller "macDrvrLibClose" call.

  Description:
    This enumeration defines the close capabilities of the Ethernet module.
*******************************************************************************/
typedef enum
{
    /* Wait for the current TX/RX operation to finish */
    MAC_DRVR_CLOSE_GRACEFUL /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,
    // Default close options
    MAC_DRVR_CLOSE_DEFAULT  /*DOM-IGNORE-BEGIN*/ = 0   /*DOM-IGNORE-END*/,
} MAC_DRVR_CLOSE_FLAGS;

// -----------------------------------------------------------------------------
// Section:  Open and Configuration Functions

/*******************************************************************************
  Function:
      void macDrvrLibInitializeEmac( MAC_DRIVER * pMacDrvr )

  Summary:
    Ethernet controller initialization routine
  Description:
    This function performs the initialization of the Ethernet Controller.

  Precondition:
    None.

  Parameters:
    pMacDrvr -  MAC device driver structure.

  Returns:
    None
  Example:
    <code>
    macDrvrLibInitializeEmac( pMacDrvr );
    </code>
  Remarks:
    This function should be called before the macDrvrLibMacOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
 ******************************************************************************/
void macDrvrLibInitializeEmac( MAC_DRIVER * pMacDrvr );

/*******************************************************************************
  Function:
    MAC_DRVR_RESULT macDrvrLibRxInit( MAC_DRIVER * pMacDrvr )

  Summary:
    Receive side initialization
  Description:
    This function performs the initialization of Rx descriptors and Rx Queue buffer
    pointer for all the MAC queues

  Parameters:
    pMacDrvr -  MAC device driver structure.

  Returns:
    MAC_DRVR_RES_OK               - success
    MAC_DRVR_RES_OUT_OF_MEMORY    - out of memory error

  Example:
    <code>
    ethRes = macDrvrLibRxInit( pMacDrvr );
    if( ethRes != MAC_DRVR_RES_OK )
    {
        - Handle error
    }
    </code>
  Remarks:
    This function should be called before the macDrvrLibMacOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
 ******************************************************************************/
MAC_DRVR_RESULT macDrvrLibRxInit( MAC_DRIVER * pMacDrvr );

/*******************************************************************************
  Function:
      MAC_DRVR_RESULT macDrvrLibTxInit( MAC_DRIVER * pMacDrvr )

  Summary:
    MAC transmit side initialization
  Description:
    This function performs the initialization of TX descriptors and TX Queue buffer
    pointer for all the MAC queues

  Precondition:
    macDrvrLibInitializeEmac must have been called.

  Parameters:
    pMacDrvr -  MAC device driver structure.

  Returns:
    MAC_DRVR_RES_OK               - success

  Example:
    <code>
    ethRes = macDrvrLibTxInit( pMacDrvrx );
    if( ethRes != MAC_DRVR_RES_OK )
    {
        - Handle error
    }
    </code>
  Remarks:
    This function should be called before the macDrvrLibMacOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
 ******************************************************************************/
MAC_DRVR_RESULT macDrvrLibTxInit( MAC_DRIVER * pMacDrvr );

/*******************************************************************************
  Function:
      void macDrvrLibTransferEnable( MAC_DRIVER * pMacDrvr )

  Summary:
    MAC Transmit and Receive Enable
  Description:

  Precondition:
    macDrvrLibInitializeEmac must have been called.
    macDrvrLibTxInit must have been called.
    macDrvrLibRxInit must have been called.
  Parameters:
    pMacDrvr -  MAC device driver structure.

  Returns:
    None
  Example:
    <code>
    macDrvrLibTransferEnable( pMacDrvr );
    </code>
  Remarks:
    This function should be called before the macDrvrLibMacOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
 ******************************************************************************/
void macDrvrLibTransferEnable( MAC_DRIVER * pMacDrvr );

/*******************************************************************************
  Function:
    void macDrvrLibMacOpen( MAC_DRIVER *  pMacDrvr, TCPIP_ETH_OPEN_FLAGS  oFlags, TCPIP_ETH_PAUSE_TYPE  pauseType)
  Summary:
    Initializes and configures the Ethernet MAC.

  Description:
    This function performs the open and configuration of the Ethernet MAC.  It
    needs to be called after the macDrvrLibInitializeEmac(MAC_DRIVER * pMacDrvr)
    was called and the PHY initialized.

  Precondition:
    macDrvrLibInitializeEmac must have been called.
    macDrvrLibTxInit must have been called.
    macDrvrLibRxInit must have been called.
    macDrvrLibTransferEnable must have been called.

  Parameters:
    pMacDrvr     - driver instance.
    oFlags      - Open flags
    pauseType   - Pause type to be supported by the MAC

  Returns:
    Ethernet operation result code.

  Example:
    <code>
    - Open and configure the MAC.
    -
    - Since we're not connected to an external PHY there's no negotiation
    - going on and we know all our opening flags
    macDrvrLibMacOpen( pMacDrvr, oFlags, pauseType );
    </code>

  Remarks:
    If no auto-negotiation is performed, the macDrvrLibMacOpen() could
    be called immediately after macDrvrLibInitializeEmac( pMacD )
    and PHY initialization.  Otherwise the open flags and the pause type should
    be the ones obtained as a result of the PHY negotiation procedure!
    <p>Replaces:<p><c><b>void EthMACOpen( oFlags, pauseType )</b></c>
 ******************************************************************************/
void macDrvrLibMacOpen( MAC_DRIVER *  pMacDrvr, TCPIP_ETH_OPEN_FLAGS  oFlags, TCPIP_ETH_PAUSE_TYPE  pauseType);

/*******************************************************************************
  Function:
    void macDrvrLibClose(   MAC_DRIVER *  pMacDrvr, MAC_DRV_CLOSE_FLAGS cFlags)
  Summary:
    Aborts any transactions and disables the Ethernet controller

  Description:
    This function performs an abort of any current Ethernet transactions, disables
    the Ethernet controller and interrupts.  It should be called after the PHY
    close/reset procedure.

  Precondition:
    None.

  Parameters:
    pMacDrvr    - driver instance.
    cFlags      - closing flags

  Returns:
    None.

  Example:
    <code>
    macDrvrLibClose( pMacDrvr, EMAC_DRV_CLOSE_DEFAULT );
    </code>

  Remarks:
    It does not free any allocated memory.
    <p>Replaces:<p><c><b>void EthClose( MAC_DRV_CLOSE_FLAGS cFlags )</b></c>
 ******************************************************************************/
void macDrvrLibClose( MAC_DRIVER *  pMacDrvr, MAC_DRVR_CLOSE_FLAGS  cFlags);

/*******************************************************************************
  Function:
    MAC_DRVR_RESULT macDrvrLibRxFilterHashCalculate( EMAC_DRV_DRIVER *  pMacDrvr, EMAC_DRV_HASH *  hash)
  Summary:
    Calculates the hash for given MAC address and sets the hash register

  Description:

  Precondition:
    None.

  Parameters:
    pMacDrvr     - driver instance.
    hash        - hash structure

  Returns:
    None.

  Example:
    <code>
    macDrvrLibRxFilterHashCalculate( pMacDrvr, &hash );
    </code>

  Remarks:

 ******************************************************************************/
MAC_DRVR_RESULT
macDrvrLibRxFilterHashCalculate( MAC_DRIVER *  pMacDrvr, MAC_DRVR_HASH * hash);

/*******************************************************************************
  Function:
    MAC_DRVR_RESULT macDrvrLibRxAllocate(   MAC_DRIVER *  pMacDrvr, uint16_t  bufferCount, bool  stickyFlag);
  Summary:
    Allocates buffers for the receive process.
 ******************************************************************************/
MAC_DRVR_RESULT macDrvrLibRxAllocate(   MAC_DRIVER *  pMacDrvr, uint16_t  bufferCount, bool  stickyFlag);

/*******************************************************************************
  Function:
    MAC_DRVR_RESULT macDrvrLibGetDynamicBuffers( MAC_DRIVER * pMacDrvr );
  Summary:
    To be called at initialization, or re-init, only

    Allocates dynamic buffers required for initialization.  Assigns dynamic
    buffer set point.
 ******************************************************************************/
MAC_DRVR_RESULT macDrvrLibGetDynamicBuffers( MAC_DRIVER * pMacDrvr );

/*******************************************************************************
  Function:
    MAC_DRVR_RESULT macDrvrLibManageDynamicBuffers( MAC_DRIVER * pMacDrvr );
  Summary:
    A run time call which will
        a) maintain an exponential moving average, ema, of the available buffer
             pool level
        b) allocate buffers into the rx buffer pool when the threshold for
            allocation has been reached
        c) provide for buffer releases from the rx pool when the ema indicates
            action is appropriate
 ******************************************************************************/
MAC_DRVR_RESULT macDrvrLibManageDynamicBuffers( MAC_DRIVER * pMacDrvr );

/*******************************************************************************
  Function:
    MAC_DRVR_RESULT macDrvrLibFreeDynamicBuffers( MAC_DRIVER * pMacDrvr );
  Summary:
    A shutdown, re-init, or closure call which will release all dynamic buffers
 ******************************************************************************/
MAC_DRVR_RESULT macDrvrLibFreeDynamicBuffers( MAC_DRIVER * pMacDrvr );

/*******************************************************************************
  Function:
    void rxExpungeFrameSegments(    MAC_DRIVER *  pMacDrvr, MAC_DRVR_RX_FRAME_INFO *  pFrameInfo);
  Summary:
    Identifies removes packets from the descriptor area, marks the descriptor
    as unused, and places the segment memory back into the avaialable buffer pool.
 ******************************************************************************/
void rxExpungeFrameSegments(    MAC_DRIVER *  pMacDrvr, MAC_DRVR_RX_FRAME_INFO *  pFrameInfo);

/*******************************************************************************
  Function:
    MAC_DRVR_RESULT macDrvrLibRxPopulateDescriptors( MAC_DRIVER * pMacDrvr );
  Summary:
    Identifies used buffers in the descriptor table and replaces them with
    new buffers from the allocated pool.  Advances ownership and the hard stop.
 ******************************************************************************/
MAC_DRVR_RESULT macDrvrLibRxPopulateDescriptors( MAC_DRIVER * pMacDrvr );

// -----------------------------------------------------------------------------
// Section:  TX/RX Functions

/*******************************************************************************
  Function:
    MAC_DRVR_RESULT macDrvrLibTxSendPacket( MAC_DRIVER *  pMacDrvr TCPIP_MAC_PACKET *  pMacPacket, uint32_t  requiredDescCount)

  Summary:
    Schedules the supplied packet for transmission.

  Description:
    This function schedules the supplied packet for transmission.  Each packet
    can consist of multiple buffers, each having a different size, as specified
    in the packet descriptor.  Each buffer will need an associated transmit
    descriptor. Therefore, the number of needed transmit descriptors should be
    available for this function to succeed.  Once a transmission is scheduled
    and done, EMAC_DRV_LibTxGetPacketStatus can be called to check the
    transmission result.

  Precondition:
    macDrvrLibMacOpen, macDrvrLibDescriptorsPoolAssignment should have
    been called.
    Valid packet descriptor pointer required.
    Buffers specified should be >0 and < 2048 bytes in size.

  Parameters:
    pMacDrvr            - driver instance
    pMacPacket          - packet, or linked list of packets, to be sent
    requiredDescCount   - number of descriptor slots which will be consumed by the packet segments

  Returns:
    MAC_DRVR_RES_OK     - success

  Example:
    <code>
    ethRes = macDrvrLibTxSendPacket( pMacDrvr );
    if( ethRes != MAC_DRVR_RES_OK )
    {
        - Transmission failed
    }
    </code>

  Remarks:
    Not multithreaded safe. Don't call from from both ISR and non-ISR code or
    multiple ISR's!

    This function enables the Ethernet transmission.

    The packet construction from buffers stops when a descriptor with NULL buffer
    or 0 size is retrieved or the list ends.
 ******************************************************************************/
MAC_DRVR_RESULT macDrvrLibTxSendPacket( MAC_DRIVER *  pMacDrvr, TCPIP_MAC_PACKET *  pMacPacket, uint32_t  requiredDescCount);

// -----------------------------------------------------------------------------
// Section: Descriptors add and remove functions

void macDrvrLibDescriptorsPoolClear( void );    // clears all pool space ahead of any pool assignment

/*******************************************************************************
  Function:
    void macDrvrLibDescriptorsPoolAssignment( MAC_DRIVER * pMacDrvr, DRV_GMAC_DCPT_TYPE dType);
  Summary:
    Adds transmit & receive descriptors.

  Description:
    This function adds the specified number of transmit/receive descriptors to
    the pool of descriptors.  These descriptors are used whenever a new
    transmission or a reception is initiated.

  Precondition:
    Allocate  RX and TX descriptors in memory.

  Parameters:
    pMacDrvr         - driver instance.
    dType           - TX/RX descriptors requested

  Returns:
    None

  Example:
    <code>
    macDrvrLibDescriptorsPoolAssignment( pMacDrvr, DRV_MAC_DCPT_TYPE_RX );
    </code>

  Remarks:
    DRV_GMAC_DCPT_TYPE_ALL use is invalid. A proper descriptor type (RX or TX)
    has to be used.
 ******************************************************************************/
void macDrvrLibDescriptorsPoolAssignment( MAC_DRIVER *  pMacDrvr, MAC_DRVR_DCPT_TYPE  dType);


// -----------------------------------------------------------------------------
// Section: packet acknowledge functions

// *****************************************************************************
/* Ethernet Buffer Acknowledge Callback Function

  Function:
    void <Function Name>( void * pPktBuff, int buffIx, void * param )

  Summary:
    Pointer to an function called to acknowledge a buffer/packet.

  Description:
    Pointer to a function called to acknowledge the completion of a buffer/packet.

  Precondition:
    A TX or RX packet must have been queued.

  Parameters:
    pPktBuff    - Pointer to the currently acknowledged transmitted or received
                  buffer.
    buffIx      - The 0-based buffer index for a packet that spans multiple
                  buffers.
    param       - extra parameter that will be used by the function call

  Returns:
    None.

  Remarks:
    This function is meant to be used for both RX and TX packets acknowledge.
    It allows the caller to pass an extra parameter that will be used in the
    function call.
 ******************************************************************************/
typedef void (* MAC_DRVR_BUFF_AckF)( void * pPktBuff, int buffIx, void * param );

// -----------------------------------------------------------------------------
// Section: Packet receive functions

/*******************************************************************************
  Function:
    MAC_DRVR_RESULT rxGetPacket( MAC_DRIVER *  pMacDrvr, TCPIP_MAC_PACKET **  ppRxMacPacket, MAC_DRVR_RXDCPT_STATUS *  pRxStatus);
  Summary:
    Provides the layout and the status of the next available received packet.

  Description:
    This function returns the layout and the status of the next available
    received packet.  If there's no available fully received packet the call
    will fail (MAC_DRVR_RES_PACKET_QUEUED/MAC_DRVR_RES_NO_PACKET).
    The supplied pPkt has to be large enough to accommodate the packet layout in
    the case of packets split across multiple buffers/descriptors is less than
    the packet size).  However, if the pPkt is NULL, then the caller can use the
    returned pRxStat to check the total size of the packet. Then using the size
    of the set buffers, the number of buffers in pPkt will result.

  Parameters:
    pMacDrvr        - driver instance.
    ppRxMacPacket   - Pointer to a packet structure: a list of buffer
                      segments large enough to receive the packet layout or
                      NULL.
    pRxStatus       - Address to store the pointer to the packet status or
                      NULL.

  Returns:
    MAC_DRVR_RES_OK         - For success.
    MAC_DRVR_RES_NO_PACKET  - No such packet in the receiving queue/
                              No packets available in the receiving queue.

  Example:
    <code>
    - Wait to receive a packet
    ethRes = rxGetPacket(   pMacDrvr,
                            &pRxPkt,
                            &pRxPktStatus
                            );
    </code>

  Remarks:
    <p>Replaces:<p><c><b>MAC_DRVR_RESULT rxGetPacket( MAC_DRIVER *  pMacDrvr, TCPIP_MAC_PACKET **  ppRxMacPacket, MAC_DRVR_RXDCPT_STATUS *  pRxStatus)</b></c>
 ******************************************************************************/
MAC_DRVR_RESULT rxGetPacket( MAC_DRIVER *  pMacDrvr, TCPIP_MAC_PACKET **  ppRxMacPacket, MAC_DRVR_RXDCPT_STATUS *  pRxStatus);

/*******************************************************************************
  Function:
      MAC_DRVR_RESULT macDrvrLibSetMacAddr( MAC_DRIVER * pMacDrvr )

  Summary:
    Set MAC address for Ethernet controller

  Description:
    This function performs setting of MAC address of the Ethernet Controller.

  Precondition:
    None.

  Parameters:
    pMacDrvr    - driver instance.

  Returns:
    MAC_DRVR_RESULT

 ******************************************************************************/
MAC_DRVR_RESULT macDrvrLibSetMacAddr( MAC_DRIVER * pMacDrvr );

/*******************************************************************************
  Function:
      MAC_DRVR_RESULT macDrvrLibGetMacAddr( MAC_DRIVER * pMacDrvr, uint8_t * pMacAddr )

  Summary:
    Get MAC address for Ethernet controller

  Description:
    This function performs reading of MAC address from the Ethernet Controller.

  Precondition:
    None.

  Parameters:
    pMacDrvr        - driver instance.
    pMacAddr        - used to return the actual address

  Returns:
    MAC_DRVR_RESULT

 ******************************************************************************/
MAC_DRVR_RESULT macDrvrLibGetMacAddr( MAC_DRIVER *  pMacDrvr, uint8_t *  pMacAddr);

// -----------------------------------------------------------------------------
// Section: Mutex Locks

// RX lock functions
static __inline__ bool __attribute__((always_inline))
macDrvrRxCreate( MAC_DRIVER * pMacDrvr )
{
    return true;
}

static __inline__ void __attribute__((always_inline))
macDrvrRxDelete( MAC_DRIVER * pMacDrvr )
{
}

static __inline__ void __attribute__((always_inline))
macDrvrRxLock( MAC_DRIVER * pMacDrvr )
{
    if( pMacDrvr->callBack.synchF != NULL )
    {
        (void)pMacDrvr->callBack.synchF(  &pMacDrvr->syncRxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER);
    }
}

static __inline__ void __attribute__((always_inline))
macDrvrRxUnlock( MAC_DRIVER * pMacDrvr )
{
    if( pMacDrvr->callBack.synchF != NULL )
    {
        (void)pMacDrvr->callBack.synchF(  &pMacDrvr->syncRxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE);
    }
}

// TX lock functions
static __inline__ bool __attribute__((always_inline))
macDrvrTxCreate( MAC_DRIVER * pMacDrvr )
{
    return true;
}

static __inline__ void __attribute__((always_inline))
macDrvrTxDelete( MAC_DRIVER * pMacDrvr )
{
}

static __inline__ void __attribute__((always_inline))
macDrvrTxLock( MAC_DRIVER * pMacDrvr )
{
    if( pMacDrvr->callBack.synchF != NULL )
    {
        (void)pMacDrvr->callBack.synchF(  &pMacDrvr->syncTxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER);
    }
}

static __inline__ void __attribute__((always_inline))
macDrvrTxUnlock( MAC_DRIVER * pMacDrvr )
{
    if( pMacDrvr->callBack.synchF != NULL )
    {
        (void)pMacDrvr->callBack.synchF(  &pMacDrvr->syncTxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE);
    }
}

TCPIP_MAC_RES EmacDrvrStatisticsGet(MAC_DRIVER* pMacDrvr, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);

TCPIP_MAC_RES EmacDrvrRegisterStatisticsGet(MAC_DRIVER* pMacDrvr, TCPIP_MAC_STATISTICS_REG_ENTRY* pStatisticsEntry, size_t maxEntries, size_t* pActualNumEntries);

// -----------------------------------------------------------------------------
/** ISO/IEC 14882:2003(E) - 5.6 Multiplicative operators:
 * The binary / operator yields the quotient, and the binary % operator yields
 * the remainder from the division of the first expression by the second.
 * If the second operand of / or % is zero the behavior is undefined; otherwise
 * (a/b)*b + a%b is equal to a.
 * If both operands are non-negative then the remainder is non-negative;
 * if not, the sign of the remainder is implementation-defined 74).
 */
__STATIC_INLINE uint32_t getModulus( int32_t dividend, int32_t divisor )
{
    int32_t modulus = dividend % divisor;
    while( modulus < 0 )
    {
        modulus += divisor;
    }

    return (uint32_t) modulus;      // should always be non-negative
}

/*  Determine space available, start == stop, is considered to be empty, not full.
*/
__STATIC_INLINE uint32_t getModuloDistance( uint32_t start, uint32_t stop, uint32_t moduloDivisor )
{
    int32_t retval = (int32_t)stop - (int32_t)start;

    if( retval <= 0 )
    {
        retval = (int32_t)moduloDivisor + retval;
    }

    return (uint32_t) retval;
}

__STATIC_INLINE uint32_t moduloIncrement( uint32_t value, uint32_t moduloDivisor )
{
    return (++value >= moduloDivisor) ? 0U : value;
}

__STATIC_INLINE uint32_t moduloDecrement( uint32_t value, uint32_t moduloDivisor )
{
    return value == 0U ? (moduloDivisor - 1U) : value - 1U;
}

static __inline__ TCPIP_MAC_SEGMENT_GAP_DCPT* __attribute__((always_inline)) FC_U8Ptr2GapDcpt( uint8_t* u8Ptr )
{
    union
    {
        uint8_t*                    u8Ptr;
        TCPIP_MAC_SEGMENT_GAP_DCPT* gapDcpt;
    }U_U8_PTR_GAP_DCPT;

    U_U8_PTR_GAP_DCPT.u8Ptr = u8Ptr;
    return U_U8_PTR_GAP_DCPT.gapDcpt;
}

static __inline__ uint32_t* __attribute__((always_inline)) FC_U8Ptr2U32Ptr( uint8_t* u8Ptr )
{
    union
    {
        uint8_t*    u8Ptr;
        uint32_t*   u32Ptr;
    }U_U8_PTR_U32_PTR;

    U_U8_PTR_U32_PTR.u8Ptr = u8Ptr;
    return U_U8_PTR_U32_PTR.u32Ptr;
}

#endif  // H_DRV_EMAC_LIB_H_

