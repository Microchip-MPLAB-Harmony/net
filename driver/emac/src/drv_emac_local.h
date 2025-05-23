/*******************************************************************************
  Ethernet MAC Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_emac_local.h

  Summary:
    Ethernet MAC driver local declarations and definitions.

  Description:
    This file contains the Ethernet MAC driver's local declarations and definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END
#ifndef H_DRV_EMAC_LOCAL_H_
#define H_DRV_EMAC_LOCAL_H_

// -----------------------------------------------------------------------------
// Section: File includes

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "system_config.h"
#include "system/system.h"
#include "system/int/sys_int.h"
#include "system/time/sys_time.h"
#include "tcpip/tcpip_mac_object.h"

#include "driver/emac/drv_emac.h"

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MAC_SAM9X60

// *****************************************************************************
/* Ethernet Event Flags

  Summary:
    Ethernet event flags

  Description:
    This enumeration defines flags for the possible Ethernet events that can
    cause interrupts.
*/
typedef enum
{
    MAC_EV_NONE                 = 0,
    //
    MAC_EV_MGMTFRAMESENT        = EMAC_ISR_MFD_Msk,                         // 0x0001
    MAC_EV_RXCOMPLETE           = EMAC_ISR_RCOMP_Msk,                       // 0x0002
    MAC_EV_RXUSEDBITREAD        = EMAC_ISR_RXUBR_Msk,                       // 0x0004
    MAC_EV_TXUSEDBITREAD        = EMAC_ISR_TXUBR_Msk,                       // 0x0008
    MAC_EV_TXUNDERRUN           = EMAC_ISR_TUND_Msk,                        // 0x0010
    MAC_EV_RETRYLIMITEXCEED     = EMAC_ISR_RLEX_Msk,                        // 0x0020
    MAC_EV_TXFRAMECORRUPT       = EMAC_ISR_TXERR_Msk,                       // 0x0040
    MAC_EV_TXCOMPLETE           = EMAC_ISR_TCOMP_Msk,                       // 0x0080
    MAC_EV_LINK                 = EMAC_ISR_LINK_Msk,                        // 0x0100
    MAC_EV_RXOVERRUN            = EMAC_ISR_ROVR_Msk,                        // 0x0200
    MAC_EV_HRESPNOTOK           = EMAC_ISR_HRESP_Msk,                       // 0x0400
    MAC_EV_PAUSEFRAMENZRECV     = EMAC_ISR_PFRE_Msk,                        // 0x0800
    MAC_EV_PAUSETIMEZERO        = EMAC_ISR_PTZ_Msk,                         // 0x1000
    MAC_EV_WAKE_ON_LAN          = EMAC_ISR_WOL_Msk,                         // 0x2000
    //
    MAC_EV_IGNORED              = ( 0
                                    | MAC_EV_MGMTFRAMESENT
                                    | MAC_EV_LINK
                                    ),
    MAC_EV_RX                   = ( 0
                                    | MAC_EV_WAKE_ON_LAN
                                    | MAC_EV_RXCOMPLETE
                                    | MAC_EV_RXUSEDBITREAD
                                    | MAC_EV_RXOVERRUN
                                    | MAC_EV_PAUSEFRAMENZRECV
                                    ),
    MAC_EV_TX                   = ( 0
                                    | MAC_EV_TXUSEDBITREAD
                                    | MAC_EV_TXUNDERRUN
                                    | MAC_EV_RETRYLIMITEXCEED
                                    | MAC_EV_TXFRAMECORRUPT
                                    | MAC_EV_TXCOMPLETE
                                    | MAC_EV_HRESPNOTOK
//                                    | MAC_EV_PAUSETIMEZERO
                                    ),
    // All events
    MAC_EV_ALL                  = EMAC_ISR_Msk,                             // 0x7EFF
} MAC_EVENTS;

typedef struct
{
    TCPIP_MAC_EVENT             tcpMacEventsEnabled;    // group enabled notification events
    volatile TCPIP_MAC_EVENT    tcpMacEventsPending;    // group notification events that are set, waiting to be re-acknowledged
    MAC_EVENTS                  macEventsEnabled;       // copy in MAC_EVENTS space
    volatile MAC_EVENTS         macEventsPending;       // copy in MAC_EVENTS space
    TCPIP_MAC_EventF            tcpNotifyFnc;           // group notification handler
    const void *                tcpNotifyParam;         // notification parameter
} MAC_DRVR_EVENT_DCPT;                                  // event descriptor per group

typedef enum
{
    MAC_DRIVER_INIT_NONE,
    MAC_DRIVER_INIT_IN_PROGRESS,
    MAC_DRIVER_INIT_COMPLETE,
} MAC_DRVR_EVENT_INIT_STATE;

/*******************************************************************************
  Ethernet Driver Module Link check states

  Summary:
    List of the states to read the current link status

  Description:
    List of the states to read the current link status

  Remarks:
    None
*/
typedef enum
{
    MAC_DRVR_LINK_CHECK_START_LINK = 0,
    MAC_DRVR_LINK_CHECK_GET_LINK,
    MAC_DRVR_LINK_CHECK_WAIT_LINK_UP,
    MAC_DRVR_LINK_CHECK_NEG_COMPLETE,
    MAC_DRVR_LINK_CHECK_NEG_RESULT,
} MAC_DRVR_LINK_CHECK_STATE;

/**
 * Configuration Structure for Queues in MAC
 */
typedef struct
{
    TCPIP_MAC_HEAP_CallocF          callocF;            // allocation functions
    TCPIP_MAC_HEAP_FreeF            freeF;
    // packet allocation functions
    union
    {
        TCPIP_MAC_PKT_AllocF    pktAllocF;
        TCPIP_MAC_PKT_AllocFDbg pktAllocFDbg;
    };
    union
    {
        TCPIP_MAC_PKT_FreeF     pktFreeF;
        TCPIP_MAC_PKT_FreeFDbg  pktFreeFDbg;
    };
    
    TCPIP_MAC_PKT_AckF              pktAckF;
    // synchronization
    TCPIP_MAC_SynchReqF             synchF;
} MAC_DRVR_CALLBACK;

typedef struct
{
    // timing and link status maintenance
    TCPIP_ETH_OPEN_FLAGS            flags;              // resulted link flags
    uint32_t                        upTick;             // tick value when the link up sequence was started
    uint32_t                        waitTick;           // tick value to wait for
    MAC_DRVR_LINK_CHECK_STATE       checkState;         // link state machine current status
    DRV_ETHPHY_NEGOTIATION_RESULT   negResult;          // negotiation result storage
} MAC_DRVR_LINK_INFO;

typedef struct
{
    TCPIP_MAC_PACKET *      head;       // list head
    TCPIP_MAC_PACKET *      tail;       // list tail
    uint32_t                nNodes;     // number of packet pointers in the list
} MAC_DRVR_PACKET_LIST;

typedef struct
{   // acc = ((alpha * newValue) + (1-alpha)(acc)) -- with alpha as a percentage
    int32_t                 scale;                      // scaling factor for alpha & oneMinusAlpha
    int32_t                 alpha;                      // alpha,       but scaled instead of percentage
    int32_t                 oneMinusAlpha;              // (1-alpha),   but scaled instead of percentage
    int32_t                 accumulation;
    int32_t                 setPoint;

} ExpMovingAve;

/*******************************************************************************
  MAC driver data.

  Summary: Data for MAC driver structure.

  Description: All the data related to MAC driver
*/
typedef struct
{
    const TCPIP_MAC_OBJECT *        pObj;                   // safe cast to TCPIP_MAC_DCPT
    //
    uint32_t                        macIx;                  // index of the MAC, for multiple MAC's support
    uint32_t                        phyIx;                  // index of the associated PHY
    SYS_MODULE_OBJ                  hPhySysObject;          // PHY object handle
    SYS_MODULE_OBJ                  hPhyClient;             // PHY handle
    SYS_STATUS                      sysStat;                // driver status
    union
    {
        uint8_t         val;
        struct __attribute__((packed))
        {
            unsigned     open               : 1;            // the corresponding MAC is opened
            unsigned     linkPresent        : 1;            // lif connection to the PHY properly detected : on/off
            unsigned     linkNegotiation    : 1;            // if an auto-negotiation is in effect : on/off
            unsigned     linkPrev           : 1;            // last value of the link status: on/off
            unsigned     linkUpDone         : 1;            // the link up sequence done
            unsigned     reserved           : 3;            // add another flags here
        };
    } macFlags;                                             // corresponding MAC flags
    uint8_t                         dataOffset;            // the data offset (TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2 flag)

    int16_t                         gapDcptOffset;         // gap descriptor offset  
    uint16_t                        gapDcptSize;           // gap descriptor size
    // control flags from the stack
    uint16_t                        controlFlags;



    // general stuff
    MAC_DRVR_CALLBACK               callBack;
    MAC_DRVR_LINK_INFO              linkInfo;
    MAC_DRVR_EVENT_DCPT             eventDescription;
    const void *                    allocH;                 // allocation handle
    uintptr_t                       syncRxH;                // synch object handle for RX operations
    uintptr_t                       syncTxH;                // synch object handle for TX operations

    MAC_DRVR_HW_RXDCPT *            pRxDesc;                // Pointer to Rx TDs (must be 8-byte aligned)
    uint16_t                        rxExtractPt;            // where cpu software is extracting
    uint16_t                        rxHardStop;             // descriptor location where next buffer will be provided
    ExpMovingAve                    rxPoolLevelEma;         // a smoothed average of the spare buffers available to populate descriptors
    MAC_DRVR_PACKET_LIST            rxMacPacketPool;        // a queue of buffers to be used to populate descriptors
    MAC_DRVR_PACKET_LIST            rxDynamicReleasePool;   // a queue of buffers which will be released/freed
    uint16_t                        rxDynamicAllocation;    // number of buffers that have been dynamically allocated
    uint16_t                        rxDynamicRelease;       // buffer count to be moved from rxMacPacketPool to rxDynamicReleasePool
    TCPIP_EMAC_RX_STATISTICS        rxStat;                 // debug: run time statistics

    MAC_DRVR_HW_TXDCPT *            pTxDesc;                // Pointer to Tx TDs (must be 8-byte aligned)
    uint16_t                        txInsertPt;             // pTxDesc leading index to use for inserting packets for transmission
    uint16_t                        txExtractPt;            // pTxDesc trailing index to use for checking on packet transmission done
    MAC_DRVR_PACKET_LIST            txPendingList;          // list of packets waiting for insertion at the txInsertPt
    TCPIP_EMAC_TX_STATISTICS        txStat;                 // debug: run time statistics

    TCPIP_MODULE_MAC_SAM9X60_CONFIG config;                 // configuration parameters
} MAC_DRIVER;

/* Function prototype for accessing Statistics registers */
typedef uint32_t (* MAC_DRVR_HW_REG_FUNC)( emac_registers_t * pMacRegs);

// *****************************************************************************
/* MAC Hardware statistics register access structure

  Summary:

  Description:
    Abstraction of the HW statistics registers

  Remarks:
*/
typedef struct
{
    const char *                    regName;            // hardware name
    MAC_DRVR_HW_REG_FUNC            regFunc;         // register access function
} MAC_DRVR_HW_REG_DCPT;

// *****************************************************************************
/* Structure for MAC Hash Calculation

  Summary:

  Description:
    Structure supports hash value calculation and setting in the MAC hash register

  Remarks:
*/
typedef struct
{
    uint64_t                value;          // 64- bit Hash value to Set
    const TCPIP_MAC_ADDR *  destMacAddr;    // MAC address pointer for Hash value calculation
    uint8_t                 calculate;      // Selects hash calculation or direct hash value
                                            // Clear to 0 for direct loading of hash value to register
                                            // Set to 1for Calculating the Hash from Mac address and load
} MAC_DRVR_HASH;

typedef union
{
    uint8_t index;
    struct __attribute__((packed))
    {
        unsigned b0:1;
        unsigned b1:1;
        unsigned b2:1;
        unsigned b3:1;
        unsigned b4:1;
        unsigned b5:1;
        unsigned b6:1;
        unsigned b7:1;
    } bits;
} MAC_DRVR_HASH_INDEX;

typedef union
{
    uint8_t addr_val;
    struct __attribute__((packed))
    {
        unsigned b0:1;
        unsigned b1:1;
        unsigned b2:1;
        unsigned b3:1;
        unsigned b4:1;
        unsigned b5:1;
        unsigned b6:1;
        unsigned b7:1;
    } bits;
} MAC_DRVR_ADDR_OCTET;

/*******************************************************************************
  MAC Single linked lists manipulation
*/
// Initialize Single List
static  __inline__ void  __attribute__((always_inline))
macDrvrPacketListInitialize( MAC_DRVR_PACKET_LIST * pList )
{
    pList->head = pList->tail = NULL;
    pList->nNodes = 0;
}

// Return number of nodes in list
static  __inline__ uint32_t __attribute__((always_inline))
macDrvrPacketListCount( MAC_DRVR_PACKET_LIST * pList )
{
    return pList->nNodes;
}

TCPIP_MAC_PACKET * macDrvrPacketListHeadRemove( MAC_DRVR_PACKET_LIST * pList);

void macDrvrPacketListTailAdd( MAC_DRVR_PACKET_LIST *  pList, TCPIP_MAC_PACKET *  pMacPacket);

void macDrvrPacketListHeadAdd( MAC_DRVR_PACKET_LIST *  pList, TCPIP_MAC_PACKET *  pMacPacket);

// statistics access to the MAC_DRIVER object
MAC_DRIVER* macDrvrObjGet(size_t* pnObjects);

#endif // H_DRV_EMAC_LOCAL_H_

