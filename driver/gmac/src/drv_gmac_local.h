/*******************************************************************************
  Ethernet MAC Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gmac_local.h

  Summary:
    Ethernet MAC driver local declarations and definitions.

  Description:
    This file contains the Ethernet MAC driver's local declarations and definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2013-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _DRV_GMAC_LOCAL_H
#define _DRV_GMAC_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "system_config.h"
#include "system/system.h"
#include "system/int/sys_int.h"
#include "system/time/sys_time.h"
#include "tcpip/tcpip_mac_object.h"
#include "driver/gmac/drv_gmac.h"
// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// synchronization for the RX flow
// The RX packets are allocated by the MAC and
// passed to the stack in manager context but
// acknowledged by the stack (user threads)
// when contents is processed
// Synchronization is needed
// If a semaphore is used then the packet RX flow (manager)
// could be blocked waiting for user threads.
// For better performance this should be avoided.
// When not using a semaphore lock, a critical section
// will be used.
//#define   DRV_GMAC_USE_RX_SEMAPHORE_LOCK
#define   DRV_GMAC_USE_TX_SEMAPHORE_LOCK


#define DRV_GMAC_MIN_RX_SIZE            64      // minimum RX buffer size
                                                // less than this creates excessive fragmentation
                                                // Keep it always multiple of 64!
#define DRV_GMAC_RX_MAX_FRAME           1536    // Maximum RX Frame Size for GMAC
#define DRV_GMAC_MIN_TX_DESCRIPTORS     1       // minimum number of TX descriptors
                                                 // needed to accommodate zero copy and TCP traffic

// Maximum RX frame selection
#if (DRV_GMAC_RX_MAX_FRAME == 1536)
#define GMAC_MAX_RXFS       1   // extend frame to 1536 bytes
#elif (DRV_GMAC_RX_MAX_FRAME == 1518)
#define GMAC_MAX_RXFS       0   // standard frame of 1518 bytes
#else
#error "Wrong TCPIP_GMAC_RX_MAX_FRAME value (1518/1536 allowed)!"
#endif                                                 
// *****************************************************************************
/* Ethernet Driver Module Link check states

  Summary:
    List of the states to read the current link status

  Description:
    List of the states to read the current link status

  Remarks:
    None
*/

typedef enum
{
    DRV_GMAC_LINK_CHECK_START_LINK  = 0,
    DRV_GMAC_LINK_CHECK_GET_LINK,
    DRV_GMAC_LINK_CHECK_WAIT_LINK_UP,
    DRV_GMAC_LINK_CHECK_NEG_COMPLETE,
    DRV_GMAC_LINK_CHECK_NEG_RESULT,
}DRV_GMAC_LINK_CHECK_STATE;

/**
 * Configuration Structure for Queues in GMAC.
 */
typedef struct
{   /** Pointer to allocated RX buffer */    
    TCPIP_MAC_PACKET **pRxPckt;
    // So that many number of Rx packet pointers for all Queues
    /** Pointer to Rx TDs (must be 8-byte aligned) */
    DRV_PIC32CGMAC_HW_RXDCPT *pRxDesc;
    // Rx queues for RX packet buffers
    DRV_PIC32CGMAC_SGL_LIST _RxQueue;   
    /** Pointer to Tx TDs (must be 8-byte aligned) */
    DRV_PIC32CGMAC_HW_TXDCPT *pTxDesc;
    //Queue list of packets for transmission
    DRV_PIC32CGMAC_SGL_LIST _TxQueue;   
    //Queue for Unacknowledged Transmit Descriptors
    DRV_PIC32CGMAC_SGL_LIST _TxDescUnAckQueue;  
    //Pool of unused nodes for Transmit packet acknowledgment
    DRV_PIC32CGMAC_SGL_LIST _TxDescAckPoolQueue;  
    /** RX index of current processing Descriptor */
    uint16_t nRxDescIndex;
    /** Circular buffer head pointer by upper layer (buffer to send) */
    uint16_t nTxDescHead;
    /** Circular buffer tail pointer incremented by handlers (buffer sent) */
    uint16_t nTxDescTail;   
} DRV_GMAC_QUEUE;

// *****************************************************************************
/* GMAC driver data.

  Summary: Data for GMAC driver structure.

  Description: All the data related to GMAC driver
*/
typedef struct {    
    
    unsigned int        _macIx;             // index of the MAC, for multiple MAC's support
    unsigned int        _phyIx;             // index of the associated PHY
    SYS_MODULE_OBJ      hPhySysObject;      // PHY object handle
    SYS_MODULE_OBJ      hPhyClient;         // PHY handle
    SYS_STATUS          sysStat;            // driver status
    union
    {
        uint16_t        val;
        struct
        {
            uint16_t    _init               : 1;    // the corresponding MAC is initialized
            uint16_t    _open               : 1;    // the corresponding MAC is opened
            uint16_t    _linkPresent        : 1;    // lif connection to the PHY properly detected : on/off
            uint16_t    _linkNegotiation    : 1;    // if an auto-negotiation is in effect : on/off
            uint16_t    _linkPrev           : 1;    // last value of the link status: on/off
            uint16_t    _linkUpDone         : 1;    // the link up sequence done
            // add another flags here
        };
    }_macFlags;                                     // corresponding MAC flags
        
    // general stuff
    const void*                     _AllocH ;        // allocation handle
    TCPIP_MAC_HEAP_CallocF          _callocF;        // allocation functions
    TCPIP_MAC_HEAP_FreeF            _freeF;
    
    // packet allocation functions
    TCPIP_MAC_PKT_AllocF            pktAllocF;
    TCPIP_MAC_PKT_FreeF             pktFreeF;
    TCPIP_MAC_PKT_AckF              pktAckF;
    
    // synchronization
    TCPIP_MAC_SynchReqF             _synchF;
    // timing and link status maintenance
    TCPIP_ETH_OPEN_FLAGS            _linkResFlags;        // resulted link flags
    uint32_t                        _linkUpTick;                // tick value when the link up sequence was started
    uint32_t                        _linkWaitTick;              // tick value to wait for
    DRV_ETHPHY_NEGOTIATION_RESULT   _negResult;     // negotiation result storage
    DRV_GMAC_LINK_CHECK_STATE       _linkCheckState;    // link state machine current status
    INT_SOURCE                      _macIntSrc;         // this MAC interrupt source
    
    DRV_GMAC_EVENT_DCPT             _gmac_event_group_dcpt;
    // synch object handle for RX operations
    uintptr_t                       _syncRxH;          
    // synch object handle for TX operations
    uintptr_t                       _syncTxH;          
    
    // debug: run time statistics
    TCPIP_MAC_RX_STATISTICS         _rxStat;
    TCPIP_MAC_TX_STATISTICS         _txStat;
               
    // GMAC queues
    DRV_GMAC_QUEUE                  * gmac_queue;
    // GMAC configuration parameters    
    TCPIP_MODULE_MAC_PIC32C_CONFIG  gmacConfig;  
    
    // gap descriptor offset
    int16_t                        _dcptOffset;
    // control flags from the stack

    // mask corresponding to _dataOffset
    uint32_t                        _dataOffsetMask;

    // TCPIP_MAC_CONTROL_FLAGS
    uint16_t                        _controlFlags;
    // data payload offset required    
    uint8_t                         _dataOffset;
    // flag to discard tx packets and reinitialize Tx    
    uint8_t                         _txDiscard;
    
} DRV_GMAC_INSTANCE;


// *****************************************************************************
/* GMAC driver structure.

  Summary: Driver Apis and Data for GMAC driver.

  Description:

*/
typedef struct
{
    const TCPIP_MAC_OBJECT* pObj;       // safe cast to TCPIP_MAC_DCPT
    DRV_GMAC_INSTANCE   sGmacData;      // specific PIC32C MAC data
} DRV_GMAC_DRIVER;

/* Function prototype for accessing Statistics registers */
typedef uint32_t (*DRV_PIC32CGMAC_HW_REG_FUNC)(DRV_GMAC_DRIVER*);

// *****************************************************************************
/* PIC32 MAC Hardware statistics register access structure

  Summary:

  Description:
    Abstraction of the HW statistics registers

  Remarks:
*/

typedef struct
{
    char                    regName[36];     // hardware name
    DRV_PIC32CGMAC_HW_REG_FUNC  regFunc;    // register access function
} DRV_PIC32CGMAC_HW_REG_DCPT;

// *****************************************************************************
/* Strutcure for PIC32C GMAC Hash Calculation

  Summary:

  Description:
    Structure supports hash value calculation and setting in PIC32C GMAC register

  Remarks:
*/
typedef struct
{
    uint64_t hash_value;            // 64- bit Hash value to Set
    const TCPIP_MAC_ADDR* DestMACAddr;    // MAC address pointer for Hash value calculation
    uint8_t calculate_hash;        // Selects hash calculation or direct hash value
                                    // Clear to 0 for direct loading of hash value to register
                                    // Set to 1for Calculating the Hash from Mac address and load

}DRV_GMAC_HASH; 

typedef union
  {
      uint8_t index;
      struct __attribute__((packed))
      {
          uint8_t b0:1;
          uint8_t b1:1;
          uint8_t b2:1;
          uint8_t b3:1;
          uint8_t b4:1;
          uint8_t b5:1;
          uint8_t b6:1;
          uint8_t b7:1;
      } bits;
  }DRV_GMAC_HASH_INDEX;  

  typedef union
  {
      uint8_t addr_val;
      struct __attribute__((packed))
      {
          uint8_t b0:1;
          uint8_t b1:1;
          uint8_t b2:1;
          uint8_t b3:1;
          uint8_t b4:1;
          uint8_t b5:1;
          uint8_t b6:1;
          uint8_t b7:1;
      } bits;
  }DRV_GMAC_MAC_ADDR;
  
// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Ethernet Descriptor Types

  Summary:
    Enumeration defining the Ethernet descriptor types.

  Description:
    This enumeration defines the Ethernet descriptor types.

  Remarks:
    Most descriptor operations support just one type, but multiple flags can be
    set.
*/

typedef enum
{
    // TX descriptor
    DRV_GMAC_DCPT_TYPE_RX    /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,

    // RX descriptor
    DRV_GMAC_DCPT_TYPE_TX    /*DOM-IGNORE-BEGIN*/ = 0x2 /*DOM-IGNORE-END*/,

    // All (both) types.  Some descriptor operations support multiple types.
    DRV_GMAC_DCPT_TYPE_ALL   /*DOM-IGNORE-BEGIN*/ = (DRV_GMAC_DCPT_TYPE_RX|DRV_GMAC_DCPT_TYPE_TX) /*DOM-IGNORE-END*/

} DRV_GMAC_DCPT_TYPE;      // descriptor types
/*------------------------------------------------------------------------------
                            Definitions
------------------------------------------------------------------------------
*/


#define DRV_GMAC_DUMMY_PRIORITY (0xFF)
#define DRV_GMAC_NO_ACTIVE_QUEUE (0xFF)
// *****************************************************************************

/**************************************************************************
  Summary:
    Defines the possible results of Ethernet operations that can succeed or
    fail
  Description:
    Ethernet Operation Result Codes
    
    This enumeration defines the possible results of any of the Ethernet
    library operations that have the possibility of failing. This result
    should be checked to ensure that the operation achieved the desired
    result.                                                                
**************************************************************************/

typedef enum
{
    /* Everything ok */
    DRV_PIC32CGMAC_RES_OK       /*DOM-IGNORE-BEGIN*/ =  0 /*DOM-IGNORE-END*/,

    /* Ethernet RX, TX, acknowledge packet codes */
    /* No such packet exist */
    DRV_PIC32CGMAC_RES_NO_PACKET,

    /* Packet is queued (not transmitted or received and not processed) */
    DRV_PIC32CGMAC_RES_PACKET_QUEUED,

    /* Not enough descriptors available */
    DRV_PIC32CGMAC_RES_NO_DESCRIPTORS,
            
    /* Not enough nodes in Tx Queues */
    DRV_PIC32CGMAC_RES_NO_TX_QUEUE,   
            
    /* Not enough nodes in Rx Queues */
    DRV_PIC32CGMAC_RES_NO_RX_QUEUE,            
             
    /* Errors: Ethernet buffers, descriptors */
    DRV_PIC32CGMAC_RES_DESC_CNT_ERR,
            
    /* Some memory allocation failed */
    DRV_PIC32CGMAC_RES_OUT_OF_MEMORY        /*DOM-IGNORE-BEGIN*/ =  -1 /*DOM-IGNORE-END*/,

    /* We don't support user space buffers. */
    DRV_PIC32CGMAC_RES_USPACE_ERR           /*DOM-IGNORE-BEGIN*/ =  -2 /*DOM-IGNORE-END*/,

    /* The size of the receive buffers too small */
    DRV_PIC32CGMAC_RES_RX_SIZE_ERR          /*DOM-IGNORE-BEGIN*/ =  -3 /*DOM-IGNORE-END*/,

    /* A received packet spans more buffers/descriptors than supplied */
    DRV_PIC32CGMAC_RES_RX_PKT_SPLIT_ERR     /*DOM-IGNORE-BEGIN*/ =  -4 /*DOM-IGNORE-END*/,
    
    DRV_PIC32CGMAC_RES_EMPTY_BUFFER         /*DOM-IGNORE-BEGIN*/  =  -5 /*DOM-IGNORE-END*/,
            
    /* Descriptor Queue Error*/        
    DRV_PIC32CGMAC_RES_QUEUE_ERR            /*DOM-IGNORE-BEGIN*/  =  -6 /*DOM-IGNORE-END*/,
            
    /* Transmission Errors*/        
    DRV_PIC32CGMAC_RES_TX_ERR               /*DOM-IGNORE-BEGIN*/  =  -7 /*DOM-IGNORE-END*/,

} DRV_PIC32CGMAC_RESULT; 


/**************************************************************************
  Summary:
    Defines the different states of RX receive state machine
  Description:
    GMAC RX engine copies frame packet to single/multiple RX buffers and update the 
    status in associated RX descriptors. The RX state machine reads the different
    RX descriptors to detect a valid RX packet
**************************************************************************/
typedef enum  
{
    GMAC_RX_NO_FRAME_STATE,
    GMAC_RX_SOF_DETECTED_STATE,
    GMAC_RX_VALID_FRAME_DETECTED_STATE
} GMAC_RXFRAME_STATE;

/**************************************************************************
  Summary:
    Defines the different TX Frame Ack Status
  Description:
    GMAC Tx packets after transmission should be removed from Tx Descriptors.
    Following are the states used for searching Tx buffers already transmitted
**************************************************************************/
typedef enum  
{
    GMAC_TX_NO_FRAME_DETECTED,
    GMAC_TX_FIRST_BUFFER_DETECTED,
    GMAC_TX_LAST_BUFFER_DETECTED,
    GMAC_TX_ERROR_DETECTED,        
} GMAC_TXBUFFER_STATE;

/**************************************************************************
  Summary:
     GMAC Rx descriptor data packet attributes
  Description:
     These attributes used during the search for valid Rx packet,on GMAC Rx Interrupt.
**************************************************************************/
typedef struct
{
    uint16_t startIndex;
    uint16_t endIndex;
    uint16_t buffer_count;  
} DRV_PIC32CGMAC_RX_FRAME_INFO; 

/**************************************************************************
  Summary:
     GMAC TX descriptor Data packet Attributes
  Description:
    These attributes used during the search for transmitted Tx packets
**************************************************************************/
typedef struct
{
    uint16_t startIndex;
    uint16_t endIndex;
    uint16_t buffer_count;  
} DRV_GMAC_TX_FRAME_INFO; 

typedef struct
{
    struct DRV_GMAC_TX_DESC_INDEX*   next;
    uint16_t startIndex;
    uint16_t endIndex;
    uint16_t buffer_count;  
} DRV_GMAC_TX_DESC_INDEX; 
// *****************************************************************************
/* Ethernet Close Flags

  Summary:
    Defines the possible disable codes of Ethernet controller "DRV_PIC32CGMAC_LibClose" call.

  Description:
    This enumeration defines the close capabilities of the Ethernet module.
*/

typedef enum
{
    /* Wait for the current TX/RX operation to finish */
    DRV_PIC32CGMAC_CLOSE_GRACEFUL  /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,

    // Default close options
    DRV_PIC32CGMAC_CLOSE_DEFAULT /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,

} DRV_PIC32CGMAC_CLOSE_FLAGS; 


// *****************************************************************************
// Section: Mutex Locks
// *****************************************************************************
// *****************************************************************************
// RX lock functions
#if defined(DRV_GMAC_USE_RX_SEMAPHORE_LOCK)
static __inline__ bool __attribute__((always_inline)) _DRV_GMAC_RxCreate(DRV_GMAC_DRIVER * pMACDrv)
{
    return (pMACDrv->sGmacData._synchF == 0) ? true : (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE);
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxDelete(DRV_GMAC_DRIVER * pMACDrv)
{
    if(pMACDrv->sGmacData._synchF != 0)
    {
        (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_DELETE);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxLock(DRV_GMAC_DRIVER * pMACDrv)
{
    if(pMACDrv->sGmacData._synchF != 0)
    {
        (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_LOCK);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxUnlock(DRV_GMAC_DRIVER * pMACDrv)
{
    if(pMACDrv->sGmacData._synchF != 0)
    {
        (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_UNLOCK);
    }
}

#else
// use critical sections
static __inline__ bool __attribute__((always_inline)) _DRV_GMAC_RxCreate(DRV_GMAC_DRIVER * pMACDrv)
{
    return true;
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxDelete(DRV_GMAC_DRIVER * pMACDrv)
{
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxLock(DRV_GMAC_DRIVER * pMACDrv)
{
    if(pMACDrv->sGmacData._synchF != 0)
    {
        (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxUnlock(DRV_GMAC_DRIVER * pMACDrv)
{
    if(pMACDrv->sGmacData._synchF != 0)
    {
        (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE);
    }
}
#endif  // defined(DRV_GMAC_USE_RX_SEMAPHORE_LOCK)


// TX lock functions
#if defined(DRV_GMAC_USE_TX_SEMAPHORE_LOCK)
static __inline__ bool __attribute__((always_inline)) _DRV_GMAC_TxCreate(DRV_GMAC_DRIVER * pMACDrv)
{
    return (pMACDrv->sGmacData._synchF == 0) ? true : (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE);
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxDelete(DRV_GMAC_DRIVER * pMACDrv)
{

    if(pMACDrv->sGmacData._synchF != 0)
    {
        (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_DELETE);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxLock(DRV_GMAC_DRIVER * pMACDrv)
{
    if(pMACDrv->sGmacData._synchF != 0)
    {
        (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_LOCK);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxUnlock(DRV_GMAC_DRIVER * pMACDrv)
{

    if(pMACDrv->sGmacData._synchF != 0)
    {
        (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_UNLOCK);
    }
}
#else
// use critical sections
static __inline__ bool __attribute__((always_inline)) _DRV_GMAC_TxCreate(DRV_GMAC_DRIVER * pMACDrv)
{
    return true;
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxDelete(DRV_GMAC_DRIVER * pMACDrv)
{
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxLock(DRV_GMAC_DRIVER * pMACDrv)
{
    if(pMACDrv->sGmacData._synchF != 0)
    {
        (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxUnlock(DRV_GMAC_DRIVER * pMACDrv)
{
    if(pMACDrv->sGmacData._synchF != 0)
    {
        (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE);
    }
}
#endif  // defined(DRV_GMAC_USE_TX_SEMAPHORE_LOCK)



// *****************************************************************************
// *****************************************************************************
// Section: Helper Macros
// *****************************************************************************
// *****************************************************************************

/****************************************************************************
 * Function: _DRV_GMAC_DescSpace
 * Summary:  Returns number of available GMAC descriptors
 *****************************************************************************/
static __inline__ uint16_t __attribute__((always_inline)) _DRV_GMAC_DescSpace(uint16_t head, uint16_t tail, uint16_t size)
{
    return (tail > head)? (tail - head) : (size - head + tail);
}

/** ISO/IEC 14882:2003(E) - 5.6 Multiplicative operators:
 * The binary / operator yields the quotient, and the binary % operator yields
 * the remainder from the division of the first expression by the second.
 * If the second operand of / or % is zero the behaviour is undefined; otherwise
 *  (a/b)*b + a%b is equal to a.
 * If both operands are non-negative then the remainder is non-negative;
 * if not, the sign of the remainder is implementation-defined 74).
 */
__STATIC_INLINE int fixed_mod(int a, int b)
{
    int rem = a % b;

    while (rem < 0)
        rem += b;

    return rem;
}

/** Return count in buffer */
#define GCIRC_CNT(head,tail,size)  fixed_mod((head) - (tail), (size))

/** Return space available, 0..size-1. always leave one free char as a
    completely full buffer has head == tail, which is the same as empty */
#define GCIRC_SPACE(head,tail,size) GCIRC_CNT((tail),((head)+1),(size))

#define GCIRC_BUFFCNT(head,tail,size) (size - GCIRC_SPACE(head,tail,size))
/** Return count up to the end of the buffer. Carefully avoid accessing head
    and tail more than once, so they can change underneath us without returning
    inconsistent results */
#define GCIRC_CNT_TO_END(head,tail,size) \
    ({int end = (size) - (tail); \
        int n = fixed_mod((head) + end, (size));    \
        n < end ? n : end;})

/** Return space available up to the end of the buffer */
#define GCIRC_SPACE_TO_END(head,tail,size) \
    ({int end = (size) - 1 - (head); \
        int n = fixed_mod(end + (tail), (size));    \
        n <= end ? n : end+1;})

/** Increment head or tail */
#define GCIRC_INC(headortail,size) \
    headortail++;             \
    if (headortail >= size) {  \
        headortail = 0;       \
    }

/** Decrement head or tail */
#define GCIRC_DEC(headortail,size) \
    headortail = headortail;    \
    if(headortail == 0) {           \
        headortail = size - 1;  \
    }                   \
    else                \
    {                   \
        headortail--;   \
    }

/** Circular buffer is empty ? */
#define GCIRC_EMPTY(head, tail)     (head == tail)

/** Clear circular buffer */
#define GCIRC_CLEAR(head, tail)  (head = tail = 0)


/*******************************************************************************
 End of File
*/

#endif //#ifndef _DRV_GMAC_LOCAL_H 