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
/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

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
#include "system/debug/sys_debug.h"
#include "system/sys_time_h2_adapter.h"
#include "tcpip/tcpip_ethernet.h"
#include "driver/gmac/src/dynamic/_gmac_dcpt_lists.h"
#include "driver/gmac/drv_gmac.h"
#include "driver/gmac/src/dynamic/drv_gmac_lib.h"
// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************
/** MAC address definition.  The MAC address must be unique on the network. */
#define GMAC_CONF_ETHADDR0                        0x00
#define GMAC_CONF_ETHADDR1                        0x04
#define GMAC_CONF_ETHADDR2                        0x25
#define GMAC_CONF_ETHADDR3                        0x1C
#define GMAC_CONF_ETHADDR4                        0xA0
#define GMAC_CONF_ETHADDR5                        0x02

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
{	/** Pointer to allocated RX buffer */    
    TCPIP_MAC_PACKET **pRxPckt;
    // So that many number of Rx packet pointers for all Queues
	/** Pointer to Rx TDs (must be 8-byte aligned) */
	DRV_PIC32CGMAC_HW_RXDCPT *pRxDesc;
    // Rx queues for RX packet buffers
    DRV_PIC32CGMAC_SGL_LIST _RxQueue;	
	/** Pointer to Tx TDs (must be 8-byte aligned) */
	DRV_PIC32CGMAC_HW_TXDCPT *pTxDesc;
    //Queue list of for transmission
	DRV_PIC32CGMAC_SGL_LIST _TxQueue;    
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
			uint16_t	_linkPrev           : 1;    // last value of the link status: on/off
			uint16_t	_linkUpDone         : 1;    // the link up sequence done
			// add another flags here
		};
	}_macFlags;										// corresponding MAC flags
		
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
	uint32_t                        _linkUpTick;				// tick value when the link up sequence was started
	uint32_t                        _linkWaitTick;				// tick value to wait for
	DRV_ETHPHY_NEGOTIATION_RESULT   _negResult;		// negotiation result storage
	DRV_GMAC_LINK_CHECK_STATE       _linkCheckState;    // link state machine current status
	INT_SOURCE                      _macIntSrc;			// this MAC interrupt source
	
	DRV_GMAC_EVENT_DCPT             _gmac_event_group_dcpt;
    // synch object handle for RX operations
	uintptr_t                       _syncRxH;          
    // synch object handle for TX operations
	uintptr_t                       _syncTxH;          
	
	// debug: run time statistics
	TCPIP_MAC_RX_STATISTICS         _rxStat;
	TCPIP_MAC_TX_STATISTICS         _txStat;
               
    // GMAC queues
	DRV_GMAC_QUEUE                  * gmac_queue;//[DRV_GMAC_NUMBER_OF_QUEUES];
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
	const TCPIP_MAC_OBJECT* pObj;		// safe cast to TCPIP_MAC_DCPT
	DRV_GMAC_INSTANCE   sGmacData;		// specific PIC32C MAC data
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
#endif //#ifndef _DRV_GMAC_LOCAL_H

  
  
  
  
  
  
  
  
  
  
  
  
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
	
	DRV_PIC32CGMAC_RES_EMPTY_BUFFER			/*DOM-IGNORE-BEGIN*/  =  -5 /*DOM-IGNORE-END*/,
            
    /* Descriptor Queue Error*/        
    DRV_PIC32CGMAC_RES_QUEUE_ERR			/*DOM-IGNORE-BEGIN*/  =  -6 /*DOM-IGNORE-END*/,
            
    /* Transmission Errors*/        
    DRV_PIC32CGMAC_RES_TX_ERR			    /*DOM-IGNORE-BEGIN*/  =  -7 /*DOM-IGNORE-END*/,

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
// *****************************************************************************
// Section: Ethernet Peripheral Library Interface Routines
// *****************************************************************************
// *****************************************************************************

// Section:  Open and Configuration Functions

/*******************************************************************************
  Function:
      void DRV_PIC32CGMAC_LibInit (DRV_GMAC_DRIVER* pMACDrv)

  Summary:
    Ethernet controller initialization routine
  Description:
    This function performs the initialization of the Ethernet Controller(GMAC).

  Precondition:
    None.

  Parameters:
    pMACDrv -  GMAC device driver structure.

  Returns:
    None
  Example:
    <code>
    DRV_PIC32CGMAC_LibInit(DRV_GMAC_DRIVER* pMACDrv);
    </code>
  Remarks:
    This function should be called before the DRV_PIC32CGMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/

void DRV_PIC32CGMAC_LibInit (DRV_GMAC_DRIVER* pMACDrv);

/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibInitTransfer(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx)

  Summary:
    GMAC transfer initialization of GMAC queue
  Description:
    This function performs the initialization of DMA and interrupts of GMAC queue.

  Precondition:
    None.

  Parameters:
    pMACDrv -  GMAC device driver structure.
    queueIdx - Queue Index

  Returns:
    DRV_PIC32CGMAC_RES_OK               - success
    DRV_PIC32CGMAC_RES_DESC_CNT_ERR     - descriptor count error
    DRV_PIC32CGMAC_RES_RX_SIZE_ERR      - rx buffer size error
    DRV_PIC32CGMAC_RES_QUEUE_ERR        - Queue Index error
  Example:
    <code>
    ethRes = DRV_PIC32CGMAC_LibInitTransfer(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx);
    if ( ethRes != DRV_PIC32CGMAC_RES_OK )
    {
        // Handle error
    }
    </code>
  Remarks:
    This function should be called before the DRV_PIC32CGMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibInitTransfer(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx);

/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxInit(DRV_GMAC_DRIVER* pMACDrv)

  Summary:
    GMAC receive side initialization
  Description:
    This function performs the initialization of Rx descriptors and Rx Queue buffer
    pointer for all the GMAC queues

  Precondition:
    DRV_PIC32CGMAC_LibInit must have been called.
    DRV_PIC32CGMAC_LibInitTransfer must have been called.

  Parameters:
    pMACDrv -  GMAC device driver structure.
  
  Returns:
    DRV_PIC32CGMAC_RES_OK               - success
    DRV_PIC32CGMAC_RES_OUT_OF_MEMORY    - out of memory error
 
  Example:
    <code>
    ethRes = DRV_PIC32CGMAC_LibRxInit(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx);
    if ( ethRes != DRV_PIC32CGMAC_RES_OK )
    {
        // Handle error
    }
    </code>
  Remarks:
    This function should be called before the DRV_PIC32CGMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxInit(DRV_GMAC_DRIVER* pMACDrv);
/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxQueFilterInit(DRV_GMAC_DRIVER* pMACDrv)

  Summary:
    GMAC receive side filter initialization
  Description:
    This function performs the initialization of Rx Queue filter initialization

  Precondition:
    None.

  Parameters:
    pMACDrv -  GMAC device driver structure.
  
  Returns:
    DRV_PIC32CGMAC_RES_OK               - success
 
  Example:
    <code>
    if(DRV_PIC32CGMAC_LibRxQueFilterInit(pMACDrv) != DRV_PIC32CGMAC_RES_OK)
    {
        // Handle error
    }
    </code>
  Remarks:
    This function should be called before the DRV_PIC32CGMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxQueFilterInit(DRV_GMAC_DRIVER* pMACDrv);
/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxInit(DRV_GMAC_DRIVER* pMACDrv)

  Summary:
    GMAC transmit side initialization
  Description:
    This function performs the initialization of TX descriptors and TX Queue buffer
    pointer for all the GMAC queues

  Precondition:
    DRV_PIC32CGMAC_LibInit must have been called.
    DRV_PIC32CGMAC_LibInitTransfer must have been called.

  Parameters:
    pMACDrv -  GMAC device driver structure.
  
  Returns:
    DRV_PIC32CGMAC_RES_OK               - success
  
  Example:
    <code>
    ethRes = DRV_PIC32CGMAC_LibTxInit(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx);
    if ( ethRes != DRV_PIC32CGMAC_RES_OK )
    {
        // Handle error
    }
    </code>
  Remarks:
    This function should be called before the DRV_PIC32CGMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxInit(DRV_GMAC_DRIVER* pMACDrv);

/*******************************************************************************
  Function:
      void DRV_PIC32CGMAC_LibTransferEnable (DRV_GMAC_DRIVER* pMACDrv)

  Summary:
    GMAC Transmit and Receive Enable
  Description:

  Precondition:
    DRV_PIC32CGMAC_LibInit must have been called.
    DRV_PIC32CGMAC_LibInitTransfer must have been called.
    DRV_PIC32CGMAC_LibTxInit must have been called.
    DRV_PIC32CGMAC_LibRxInit must have been called.
  Parameters:
    pMACDrv -  GMAC device driver structure.

  Returns:
    None
  Example:
    <code>
    DRV_PIC32CGMAC_LibTransferEnable(DRV_GMAC_DRIVER* pMACDrv);
    </code>
  Remarks:
    This function should be called before the DRV_PIC32CGMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
void DRV_PIC32CGMAC_LibTransferEnable (DRV_GMAC_DRIVER* pMACDrv);

/*******************************************************************************
  Function:
      bool DRV_PIC32CGMAC_LibSetPriorityToQueueNum(DRV_GMAC_DRIVER* pMACDrv)

  Summary:
    Assign priority to enabled queues
  Description:

  Precondition:
    None
  Parameters:
    pMACDrv -  GMAC device driver structure.
  Returns:
    True - success
    False - failure

  Remarks:
    None

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
bool DRV_PIC32CGMAC_LibSetPriorityToQueueNum(DRV_GMAC_DRIVER* pMACDrv);

/*******************************************************************************
  Function:
      uint8_t DRV_PIC32CGMAC_LibGetPriorityFromQueueNum(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx)

  Summary:
    Retrieve priority number from queue index
  Description:

  Precondition:
    None
  Parameters:
    pMACDrv -  GMAC device driver structure.
    queueIdx - priority Queue index
  Returns:
    priority number

  Remarks:
    None

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
uint8_t DRV_PIC32CGMAC_LibGetPriorityFromQueueNum(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx);

/*******************************************************************************
  Function:
      uint8_t DRV_PIC32CGMAC_LibGetHighPrioReadyQue(DRV_GMAC_DRIVER* pMACDrv)

  Summary:
    Return the highest priority queue ready
  Description:

  Precondition:
    None
  Parameters:
    pMACDrv -  GMAC device driver structure.
  Returns:
    priority Queue index

  Remarks:
    None

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
uint8_t DRV_PIC32CGMAC_LibGetHighPrioReadyQue(DRV_GMAC_DRIVER* pMACDrv);

/*******************************************************************************
  Function:
      void DRV_PIC32CGMAC_LibClearPriorityQue(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx)

  Summary:
    Clear the ready status of priority queue
  Description:

  Precondition:
    None
  Parameters:
    pMACDrv -  GMAC device driver structure.
    queueIdx - priority Queue index
  Returns:
    None

  Remarks:
    None

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
void DRV_PIC32CGMAC_LibClearPriorityQue(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx);

/*******************************************************************************
  Function:
      void DRV_PIC32CGMAC_LibSysInt_Disable(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask, bool *queStat)

  Summary:
    Disable GMAC priority Queue interrupts
  Description:

  Precondition:
    None
  Parameters:
    pMACDrv -  GMAC device driver structure.
    queMask - priority Queue mask
    queStat - pointer to store the interrupt state (to use with DRV_PIC32CGMAC_LibSysInt_Restore())
  Returns:
    None

  Remarks:
    None

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
void DRV_PIC32CGMAC_LibSysInt_Disable(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask, bool *queStat);

/*******************************************************************************
  Function:
      void DRV_PIC32CGMAC_LibSysIntStatus_Clear(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask)

  Summary:
    Clear GMAC priority Queue interrupt status
  Description:

  Precondition:
    None
  Parameters:
    pMACDrv -  GMAC device driver structure.
    queMask - priority Queue mask
  Returns:
    None

  Remarks:
    None

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
void DRV_PIC32CGMAC_LibSysIntStatus_Clear(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask);

/*******************************************************************************
  Function:
      void DRV_PIC32CGMAC_LibSysInt_Enable(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask)

  Summary:
    Enable GMAC priority Queue interrupt
  Description:

  Precondition:
    None
  Parameters:
    pMACDrv -  GMAC device driver structure.
    queMask - priority Queue mask
  Returns:
    None

  Remarks:
    None

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
void DRV_PIC32CGMAC_LibSysInt_Enable(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask);

/*******************************************************************************
  Function:
      void DRV_PIC32CGMAC_LibSysInt_Restore(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask, bool *queStat)

  Summary:
    Restore GMAC Queue interrupt sources
  Description:

  Precondition:
    None
  Parameters:
    pMACDrv -  GMAC device driver structure.
    queMask - priority Queue mask
    queStat - state to restore interrupts (to work with DRV_PIC32CGMAC_LibSysInt_Disable())
  Returns:
    None

  Remarks:
    None

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
void DRV_PIC32CGMAC_LibSysInt_Restore(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask, bool *queStat);

/*******************************************************************************
  Function:
      uint32_t DRV_PIC32CGMAC_LibReadInterruptStatus(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx)

  Summary:
    Read GMAC interrupt status
  Description:

  Precondition:
    None
  Parameters:
    pMACDrv -  GMAC device driver structure.
    queueIdx - queue index
  Returns:
    GMAC interrupt status

  Remarks:
    None

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
uint32_t DRV_PIC32CGMAC_LibReadInterruptStatus(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx);

/*******************************************************************************
  Function:
      void DRV_PIC32CGMAC_LibEnableInterrupt(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx, GMAC_EVENTS ethEvents)

  Summary:
    Enable GMAC interrupt events
  Description:

  Precondition:
    None
  Parameters:
    pMACDrv -  GMAC device driver structure.
    queueIdx - queue index
    ethEvents - interrupt events
  Returns:
    None

  Remarks:
    None

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
void DRV_PIC32CGMAC_LibEnableInterrupt(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx, GMAC_EVENTS ethEvents);

/*******************************************************************************
  Function:
      void DRV_PIC32CGMAC_LibDisableInterrupt(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx, GMAC_EVENTS ethEvents)

  Summary:
    Disable GMAC interrupt events
  Description:

  Precondition:
    None
  Parameters:
    pMACDrv -  GMAC device driver structure.
    queueIdx - queue index
    ethEvents - interrupt events
  Returns:
    None

  Remarks:
    None

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
void DRV_PIC32CGMAC_LibDisableInterrupt(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx, GMAC_EVENTS ethEvents);

/*******************************************************************************
  Function:
    void DRV_PIC32CGMAC_LibMACOpen(DRV_GMAC_DRIVER * pMACDrv, TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType )

  Summary:
    Initializes and configures the Ethernet MAC.

  Description:
    This function performs the open and configuration of the Ethernet MAC.  It needs
    to be called after the DRV_PIC32CGMAC_LibInit(DRV_GMAC_DRIVER * pMACDrv) was called and the PHY initialized.


  Precondition:
    DRV_PIC32CGMAC_LibInit must have been called.
    DRV_PIC32CGMAC_LibInitTransfer must have been called.
    DRV_PIC32CGMAC_LibTxInit must have been called.
    DRV_PIC32CGMAC_LibRxInit must have been called.
    DRV_PIC32CGMAC_LibTransferEnable must have been called.

  Parameters:
    pMACDrv       - driver instance.
    oFlags      - Open flags
    pauseType   - Pause type to be supported by the MAC


  Returns:
    Ethernet operation result code.

  Example:
    <code>
    // Open and configure the MAC.
    //
    // Since we're not connected to an external PHY there's no negotiation
    // going on and we know all our opening flags
    DRV_PIC32CGMAC_LibMACOpen(DRV_GMAC_DRIVER * pMACDrv, oFlags, pauseType);
    </code>

  Remarks:
    If no auto-negotiation is performed, the DRV_PIC32CGMAC_LibMACOpen() could be called
    immediately after DRV_PIC32CGMAC_LibInit(DRV_GMAC_INSTANCE_DCPT* pMacD) and PHY initialization.  Otherwise the open
    flags and the pause type should be the ones obtained as a result of the
    PHY negotiation procedure!
    <p>Replaces:<p><c><b>void EthMACOpen(TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType)</b></c>
 *****************************************************************************/

void DRV_PIC32CGMAC_LibMACOpen (DRV_GMAC_DRIVER * pMACDrv, TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType );


/*******************************************************************************
  Function:
    void DRV_PIC32CGMAC_LibClose(DRV_GMAC_DRIVER * pMACDrv, DRV_PIC32CGMAC_CLOSE_FLAGS cFlags )

  Summary:
    Aborts any transactions and disables the Ethernet controller

  Description:
    This function performs an abort of any current Ethernet transactions, disables
    the Ethernet controller and interrupts.  It should be called after the PHY
    close/reset procedure.

  Precondition:
    None.

  Parameters:
    pMACDrv       - driver instance.
    cFlags - closing flags

  Returns:
    None.

  Example:
    <code>
    DRV_PIC32CGMAC_LibClose(DRV_GMAC_DRIVER * pMACDrv, DRV_PIC32CGMAC_CLOSE_DEFAULT);
    </code>

  Remarks:
    It does not free any allocated memory.
    <p>Replaces:<p><c><b>void EthClose(DRV_PIC32CGMAC_CLOSE_FLAGS cFlags)</b></c>
 *****************************************************************************/

void DRV_PIC32CGMAC_LibClose (DRV_GMAC_DRIVER * pMACDrv, DRV_PIC32CGMAC_CLOSE_FLAGS cFlags );

/*******************************************************************************
  Function:
    DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxFilterHash_Calculate(DRV_GMAC_DRIVER* pMACDrv, DRV_GMAC_HASH *hash) 

  Summary:
    Calculates the hash for given MAC address and sets the hash in PIC32C GMAC register

  Description:

  Precondition:
    None.

  Parameters:
    pMACDrv  - driver instance.
    hash  - hash structure

  Returns:
    None.

  Example:
    <code>
    DRV_PIC32CGMAC_LibRxFilterHash_Calculate(pMACDrv, &hash);
    </code>

  Remarks:
    
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxFilterHash_Calculate(DRV_GMAC_DRIVER* pMACDrv, DRV_GMAC_HASH *hash);
/*******************************************************************************
  Function:
    DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxBuffersAppend(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx, uint16_t start_index, uint16_t nDscCnt)

  Summary:
    Supplies buffers to the receiving process and enables the receiving part of
    the controller.

  Description:
    This function supplies buffers to the receiving process and enables the
    receiving part of the controller.  As soon as the controller starts
    receiving data packets these will be stored into memory at the addresses
    specified by these buffers.  A received packet can consist of multiple
    buffers, split across buffers with the SAME size.  Each buffer needs an associated receive
    descriptor. Therefore, the number of needed receive descriptors should be
    available for this function to succeed.  Once a receive operation is
    scheduled, DRV_PIC32CGMAC_LibRxGetPacket(DRV_GMAC_DRIVER* pMACDrv, ) can be called to get the received packet.

  Precondition:
    DRV_PIC32CGMAC_LibInit and other GMAC initialization routines should have been called.
  
  Parameters:
    pMACDrv       - driver instance.
    queueIdx      - Queue index

    start_index      - Descriptor Start index

    nDscCnt     - number of descriptors

  Returns:
    DRV_PIC32CGMAC_RES_OK               - success

    DRV_PIC32CGMAC_RES_OUT_OF_MEMORY    - no enough memory

  Example:
    <code>
    ethRes = DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx,cstart_index,cnDscCnt);
    if ( ethRes != DRV_PIC32CGMAC_RES_OK )
    {
        // Handle error
    }
    </code>

  Remarks:
    Not multithreaded safe. Don't call from from both ISR -non ISR code or
    multiple ISR's!

    This function enables the Ethernet receiving.

    When a packet is split into multiple buffers, all buffers have the same
    size.

    The append process continues until a NULL buffer pointer is retrieved or
    nBuffs buffers are appended.

    Only RX DRV_PIC32CGMAC_BUFF_FLAGS are relevant for this function.
    <p>Replaces:<p><c><b>DRV_PIC32CGMAC_LibRxBuffersAppend(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx, uint16_t start_index, uint16_t nDscCnt);</b></c>
 *****************************************************************************/

DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxBuffersAppend(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx, uint16_t start_index, uint16_t nDscCnt);

// Section:  TX/RX Functions


/*******************************************************************************
  Function:
    DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxSendPacket (DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx)

  Summary:
    Schedules the supplied packet for transmission.

  Description:
    This function schedules the supplied packet for transmission.  Each packet
    can consist of multiple buffers, each having a different size, as specified
    in the packet descriptor.  Each buffer will need an associated transmit
    descriptor. Therefore, the number of needed transmit descriptors should be
    available for this function to succeed.  Once a transmission is scheduled
    and done, DRV_PIC32CGMAC_LibTxGetPacketStatus can be called to check the transmission
    result.

  Precondition:
    DRV_PIC32CGMAC_LibMACOpen, DRV_PIC32CGMAC_LibDescriptorsPoolAdd should have been called.
    Valid packet descriptor pointer required.
    Buffers specified should be >0 and < 2048 bytes in size.

  Parameters:
    pMACDrv         - driver instance.
    pPktDSeg        - pointer to data segment of tx packet
    queueIdx        - Queue Index.

  Returns:
    DRV_PIC32CGMAC_RES_OK              - success

    DRV_PIC32CGMAC_RES_NO_DESCRIPTORS  - no descriptors available

    DRV_PIC32CGMAC_RES_NO_PACKET      -  no packet for transmission

  Example:
    <code>
    ethRes = DRV_PIC32CGMAC_LibTxSendPacket (pMACDrv,queueIdx);
    if ( ethRes != DRV_PIC32CGMAC_RES_OK )
    {
        // Transmission failed
    }
    </code>

  Remarks:
    Not multithreaded safe. Don't call from from both ISR and non-ISR code or
    multiple ISR's!

    This function enables the Ethernet transmission.

    The packet construction from buffers stops when a descriptor with NULL buffer
    or 0 size is retrieved or the list ends.
    <p>Replaces:<p><c><b>DRV_PIC32CGMAC_LibTxSendPacket (DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx)</b></c>
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxSendPacket(DRV_GMAC_DRIVER * pMACDrv, TCPIP_MAC_DATA_SEGMENT * pPktDSeg,  GMAC_QUE_LIST queueIdx);

// *****************************************************************************
// *****************************************************************************
// Section: Descriptors add and remove functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void DRV_GMAC_LibDescriptorsPoolAdd (DRV_GMAC_DRIVER * pMACDrv, DRV_GMAC_DCPT_TYPE dType);

  Summary:
    Adds transmit & receive descriptors.

  Description:
    This function adds the specified number of transmit/receive descriptors to
    the pool of descriptors.  These descriptors are used whenever a new
    transmission or a reception is initiated.  

  Precondition:
    Allocate  RX and TX descriptors in memory.

  Parameters:
    pMACDrv           - driver instance.
  
    dType           - TX/RX descriptors requested


  Returns:
    None

  Example:
    <code>
    DRV_GMAC_LibDescriptorsPoolAdd (pMACDrv, DRV_GMAC_DCPT_TYPE_RX);
    </code>

  Remarks:
    DRV_GMAC_DCPT_TYPE_ALL use is invalid. A proper descriptor type (RX or TX) has
    to be used.   
 *****************************************************************************/
void DRV_GMAC_LibDescriptorsPoolAdd (DRV_GMAC_DRIVER * pMACDrv, DRV_GMAC_DCPT_TYPE dType);

// *****************************************************************************
// *****************************************************************************
// Section: packet acknowledge functions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Ethernet Buffer Acknowledge Callback Function

  Function:
    void <Function Name> ( void *pPktBuff, int buffIx, void* param )

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
*/

typedef void ( *DRV_PIC32CGMAC_BUFF_AckF ) ( void *pPktBuff, int buffIx, void* param );

/*******************************************************************************
  Function:
    DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxAckPacket(DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx);
 
  Summary:
    Acknowledges a transmitted packet

  Description:
    This function acknowledges a transmitted packet.  The transmission
    of this packet has to have been completed otherwise the call will fail.
    The TX acknowledgment function will be called from this routine and free the
    used TX descriptors

  Precondition:
   DRV_PIC32CGMAC_LibTxSendPacket should be called before

  Parameters:
    pMACDrv       - driver instance.
    queueIdx      - queue Index


  Returns:
    DRV_PIC32CGMAC_RES_OK              - Success

    DRV_PIC32CGMAC_RES_DESC_CNT_ERR   - descriptor error

  Example:
    <code>
    ethRes= DRV_PIC32CGMAC_LibTxAckPacket(pMACDrv,queueIdx);


    </code>

  Remarks:
    Any transmitted packet has to be acknowledged, otherwise the Ethernet library will
    run out of transmission descriptors.

    <p>Replaces:<p><c><b>DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxAckPacket(DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx)</b></c>
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxAckPacket(DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx);

/*******************************************************************************
  Function:
    void DRV_PIC32CGMAC_LibTxAckPendPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes);
 
  Summary:
    Acknowledge pending packets in transmit queue

  Description:
    This function acknowledges all packet queued for transmission. The packet is not
    yet written to transmit descriptors. This will be called during error condition
    to discard queued TX packets. The TX acknowledgment function will be called from this routine.

  Precondition:
    None

  Parameters:
    pMACDrv         - driver instance.
    queueIdx        - queue Index
    ackRes          - packet acknowledgment result

  Returns:
    None

  Example:
    <code>
    DRV_PIC32CGMAC_LibTxAckPendPacket(pMACDrv, queueIdx, ackRes); 
    </code>

  Remarks:
    Any pending TX packet has to be acknowledged during error condition, otherwise the memory 
    allocated for the packet will not be freed.

    <p>Replaces:<p><c><b>void DRV_PIC32CGMAC_LibTxAckPendPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes)</b></c>
 *****************************************************************************/
void DRV_PIC32CGMAC_LibTxAckPendPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes);

/*******************************************************************************
  Function:
    void DRV_PIC32CGMAC_LibTxAckErrPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes);
 
  Summary:
    Acknowledge packets in transmit descriptors during transmit errors

  Description:
    This function acknowledges packets added to MAC transmit descriptors. This function will
    acknowledge all the transmitted or not transmitted packets in transmit descriptor. 
    This will be called during error condition to discard queued TX packets. 
    The TX acknowledgment function will be called from this routine.

  Precondition:
    None

  Parameters:
    pMACDrv         - driver instance.
    queueIdx        - queue Index
    ackRes          - packet acknowledgment result

  Returns:
    None

  Example:
    <code>
    DRV_PIC32CGMAC_LibTxAckErrPacket(pMACDrv, queueIdx, ackRes); 
    </code>

  Remarks:
    Any TX packets in transmit descriptors has to be acknowledged during error condition, 
    otherwise the memory allocated for the packet will not be freed.

    <p>Replaces:<p><c><b>void DRV_PIC32CGMAC_LibTxAckErrPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes)</b></c>
 *****************************************************************************/
void DRV_PIC32CGMAC_LibTxAckErrPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes);
// *****************************************************************************
// *****************************************************************************
// Section: Packet receive functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxGetPacket (DRV_GMAC_DRIVER * pMACDrv,  TCPIP_MAC_PACKET **pPkt, int *pnBuffs, 
 *                          TCPIP_MAC_PACKET_RX_STAT_PIC32C* pRxStat,GMAC_QUE_LIST queueIdx )

  Summary:
    Provides the layout and the status of the next available received packet.

  Description:
    This function returns the layout and the status of the next available
    received packet.  If there's no available fully received packet the call
    will fail (DRV_PIC32CGMAC_RES_PACKET_QUEUED/DRV_PIC32CGMAC_RES_NO_PACKET).  The supplied pPkt has
    to be large enough to accommodate the packet layout in the case of packets
    split across multiple buffers/descriptors (i.e. when the size of the buffer,
    DRV_PIC32CGMAC_LibRxSetBufferSize(DRV_GMAC_INSTANCE_DCPT* pMacD, ) is less than the packet size) otherwise
    DRV_PIC32CGMAC_RES_RX_PKT_SPLIT_ERR will be returned.  However, if the pPkt is NULL,
    then the caller can use the returned pRxStat to check the total size of the
    packet. Then using the size of the set buffers, the number of buffers in
    pPkt will result.  pnBuffs can be used for the same purpose. If pnBuffs is
    supplied, the numbers of buffers in the packet will be returned.

  Precondition:
    DRV_PIC32CGMAC_LibRxSetBufferSize, DRV_PIC32CGMAC_LibRxBuffersAppend should have been called.

  Parameters:
    pMACDrv       - driver instance.
    pPkt            - Pointer to a packet descriptor: a list of buffer
                      descriptors large enough to receive the packet layout or
                      NULL.

    pnBuffs         - Pointer to store the number of buffers in the packet or
                      NULL.

    pRxStat         - Address to store the pointer to the packet status or
                      NULL.
    queueIdx        - Queue index

  Returns:
    DRV_PIC32CGMAC_RES_OK                  - For success.

    DRV_PIC32CGMAC_RES_NO_PACKET           - No such packet in the receiving queue/
                                  No packets available in the receiving queue.

  Example:
    <code>
    // Wait to receive a packet
    ethRes = DRV_PIC32CGMAC_LibRxGetPacket (pMACDrv, &pRxPkt, &buffsPerRxPkt, &pRxPktStat, GMAC_QUE_0);    
    </code>

  Remarks:
    <p>Replaces:<p><c><b>DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxGetPacket (DRV_GMAC_DRIVER * pMACDrv,  TCPIP_MAC_PACKET **pPkt, int *pnBuffs, TCPIP_MAC_PACKET_RX_STAT_PIC32C* pRxStat,GMAC_QUE_LIST queueIdx )</b></c>
 *****************************************************************************/

DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxGetPacket (DRV_GMAC_DRIVER * pMACDrv,  TCPIP_MAC_PACKET **pPkt, int *pnBuffs, TCPIP_MAC_PACKET_RX_STAT_PIC32C* pRxStat,GMAC_QUE_LIST queueIdx );


/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibSetMacAddr (DRV_GMAC_DRIVER* pMACDrv, const uint8_t * pMacAddr)

  Summary:
    Set MAC address for Ethernet controller
  Description:
    This function performs setting of MAC address of the Ethernet Controller(GMAC).

  Precondition:
    None.

  Parameters:
    pMACDrv  - driver instance.
    pMacAddr -  address of MAC Address array.

  Returns:
    DRV_PIC32CGMAC_RESULT

  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibSetMacAddr (DRV_GMAC_DRIVER* pMACDrv, const uint8_t * pMacAddr);

/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibGetMacAddr (DRV_GMAC_DRIVER* pMACDrv, uint8_t * pMacAddr)

  Summary:
    Get MAC address for Ethernet controller
  Description:
    This function performs reading of MAC address from the Ethernet Controller(GMAC).

  Precondition:
    None.

  Parameters:
    pMACDrv  - driver instance.
    pMacAddr -  address of MAC Address array.

  Returns:
    DRV_PIC32CGMAC_RESULT

  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibGetMacAddr (DRV_GMAC_DRIVER* pMACDrv, uint8_t * pMacAddr);

/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxBuffersCountGet(DRV_GMAC_DRIVER* pMACDrv, int* pendBuffs, int* schedBuffs)

  Summary:
    Returns the number of pending RX buffers and scheduled buffers in the GMAC queues.
  Description:
    This function count the number of pending RX buffers and scheduled buffers in the GMAC queues
    from GMAC RX descriptors

  Precondition:
    pMACDrv       - driver instance.
    pendBuffs     - pointer to an address to store the number of pending buffers
    schedBuffs    - pointer to an address to store the number of scheduled buffers 

  Parameters:
    pMacAddr -  address of MAC Address array.

  Returns:
    DRV_PIC32CGMAC_RESULT

  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxBuffersCountGet(DRV_GMAC_DRIVER* pMACDrv, int* pendBuffs, int* schedBuffs);

/****************************************************************************
  Function: 
    void DRV_PIC32CGMAC_LibSetRxFilter(DRV_GMAC_DRIVER* pMACDrv, GMAC_RX_FILTERS  gmacRxFilt)
 
  Summary: 
    Set GMAC Rx Filters
 
  Precondition: 
    None
 
  Parameters: 
    pMACDrv    - driver instance.
    gmacRxFilt - RX Filter for GMAC
 
  Returns:
    None
 *****************************************************************************/
void DRV_PIC32CGMAC_LibSetRxFilter(DRV_GMAC_DRIVER* pMACDrv, GMAC_RX_FILTERS  gmacRxFilt);

/****************************************************************************
  Function: 
    void DRV_PIC32CGMAC_LibClearTxComplete(DRV_GMAC_DRIVER* pMACDrv)
 
  Summary: 
    Clear GMAC Tx Complete status
 
  Precondition: 
    None
 
  Parameters: 
    pMACDrv    - driver instance. 
     
  Returns:
    None
 *****************************************************************************/
void DRV_PIC32CGMAC_LibClearTxComplete(DRV_GMAC_DRIVER* pMACDrv);

/****************************************************************************
  Function: 
    bool DRV_PIC32CGMAC_LibIsTxComplete(DRV_GMAC_DRIVER* pMACDrv)
 
  Summary: 
    Check GMAC Tx Complete status
 
  Precondition: 
    None
 
  Parameters: 
    pMACDrv    - driver instance. 
     
  Returns:
    true or false
 *****************************************************************************/
bool DRV_PIC32CGMAC_LibIsTxComplete(DRV_GMAC_DRIVER* pMACDrv);

/****************************************************************************
  Function: 
    void DRV_PIC32CGMAC_LibTxEnable(DRV_GMAC_DRIVER* pMACDrv, bool enable)
 
  Summary: 
    Enable/Disable GMAC Transmit
 
  Precondition: 
    None
   
  Parameters: 
    pMACDrv    - driver instance. 
    bool enable - true/false
 
  Returns:
    None
 *****************************************************************************/
void DRV_PIC32CGMAC_LibTxEnable(DRV_GMAC_DRIVER* pMACDrv, bool enable);

/****************************************************************************
  Function: 
    uint32_t DRV_PIC32CGMAC_LibGetxxxxxx(void)
 
  Summary: 
    Read GMAC Statistics registers 
 
  Precondition: 
    None
 
  Parameters: 
    pMACDrv    - driver instance. 
 
  Returns:
    32-bit statistics register value
 *****************************************************************************/
uint32_t DRV_PIC32CGMAC_LibGetTxOctetLow(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxOctetHigh(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxBCastFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxMCastFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxPauseFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTx64ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTx127ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTx255ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTx511ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTx1023ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTx1518ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxGT1518ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxUnderRunFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxSingleCollFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxMultiCollFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxExcessCollFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxLateCollFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxDeferFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetTxCSErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxOctetLow(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxOctetHigh(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxBCastFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxMCastFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxPauseFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRx64ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRx127ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRx255ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRx511ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRx1023ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRx1518ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxGT1518ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxUnderSizeFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxOverSizeFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxJabberFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxFCSErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxLFErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxSymErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxAlignErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxResErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxOverRunFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxIPHdrCSErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxTCPCSErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv);
uint32_t DRV_PIC32CGMAC_LibGetRxUDPCSErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv);
// *****************************************************************************
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
	headortail = headortail;	\
	if(headortail == 0)	{			\
		headortail = size - 1;	\
	}					\
	else				\
	{					\
		headortail--;	\
	}

/** Circular buffer is empty ? */
#define GCIRC_EMPTY(head, tail)     (head == tail)

/** Clear circular buffer */
#define GCIRC_CLEAR(head, tail)  (head = tail = 0)

/****************************************************************************
 * Function:        _DRV_GMAC_HashValueSet
 * Summary: Set Hash Value in GMAC register
 *****************************************************************************/
static __inline__ void __attribute__((always_inline)) _DRV_GMAC_HashValueSet(DRV_GMAC_DRIVER* pMACDrv, uint64_t hash_value)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    pGmacRegs->GMAC_HRB = hash_value & 0xffffffff;
    pGmacRegs->GMAC_HRT = (hash_value >> 32) & 0xffffffff;    
}

/****************************************************************************
 * Function:        _DRV_GMAC_HashValueGet
 * Summary: Read Hash Value in GMAC register
 *****************************************************************************/
static __inline__ uint64_t __attribute__((always_inline)) _DRV_GMAC_HashValueGet(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    uint64_t hash_value = 0;    
    hash_value = pGmacRegs->GMAC_HRT;
    hash_value = (hash_value << 32) | pGmacRegs->GMAC_HRB;
    return hash_value;
}
/*******************************************************************************
 End of File
*/

