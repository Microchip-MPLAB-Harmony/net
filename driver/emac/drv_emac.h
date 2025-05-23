/*******************************************************************************
  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_emac.h

  Summary:
    Ethernet MAC device driver interface file

  Description:
    Ethernet MAC Device Driver Interface
    
    The Ethernet MAC device driver provides a simple interface to manage
    the Ethernet peripheral. This file defines the interface definitions
    and prototypes for the Ethernet MAC driver.                         
 ******************************************************************************/
//DOM-IGNORE-BEGIN
/*
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#ifndef H_DRV_EMAC_H_
#define H_DRV_EMAC_H_

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "driver/driver_common.h"

#include "driver/ethphy/drv_ethphy.h"

#include "tcpip/tcpip_mac.h"
#include "tcpip/tcpip_mac_object.h"
#include "tcpip/tcpip_ethernet.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
typedef struct
{
    /*  number of OK RX packets */
    uint32_t    nRxOkPackets;
    /*  number of currently scheduled RX buffers in the driver queues. */
    /* These are available buffers, ready to receive data */
    uint32_t    nRxSchedBuffers;
    /*  number of RX packets with errors */
    uint32_t    nRxErrorPackets;
    /*  number of RX fragmentation errors */
    uint32_t    nRxFragmentErrors;
    /*  number of occurences of 'RX Buffer Not Available' */
    uint32_t    nRxBuffNotAvailable;
    uint32_t    packetPoolBuffers;
    uint32_t    expungedSegments;
    uint32_t    macPacketsInStackHighPoint;
    uint32_t    macPacketsInStack;
    uint32_t    macSegmentsInStack;
    uint32_t    packetAllocFailures;
} TCPIP_EMAC_RX_STATISTICS;

typedef struct
{
    /*  number of OK transmitted packets */
    uint32_t    nTxOkPackets;
    /*  number of unacknowledged pending TX buffers in the driver queues. */
    /*  This is equal with pending TX packets when each packet */
    /*  is contained within a TX buffer. */
    uint32_t    nTxPendBuffers;
    /*  number of packets that could not be transmitted */
    uint32_t    nTxErrorPackets;
    /*  number of times the TX queue was full */
    /*  this may signal that the number of TX descriptors is too small */
    uint32_t    nTxQueueFull;
    uint32_t    pending;
    uint32_t    linkLossResets;
} TCPIP_EMAC_TX_STATISTICS;


typedef struct
{
    TCPIP_MAC_ADDR                  macAddress;         // instance address
    INT_SOURCE                      macIntSrc;
    uint32_t                        macRxFilters;
    /* Configuration for MAC queues */
    uint16_t rxDeviceMaxDescriptors;/** largest descriptor count possible for the device **/
    uint16_t nRxDescCnt;            /** RX Descriptor count */
    uint16_t rxBufferSize;          /** RX buffer size */
    uint16_t nRxStaticBufferCnt;    /** RX static buffer count */
    uint16_t nRxBuffCntThres;       /** RX buffer count threshold */
    uint16_t nRxBuffAllocCnt;       /** RX buffer allocate count */
    uint16_t nTxDescCnt;            /** TX Descriptor count */
    uint16_t txBufferSize;          /** TX buffer size */
    uint8_t queueEnable;            /** Queue Enable status */
    /*  Delay to wait after the link comes up (milliseconds) */
    /*  for insuring that the PHY is ready to transmit data. */
    uint16_t                        linkInitDelay;
    /*  flags to use for the ETH connection */
    TCPIP_ETH_OPEN_FLAGS            ethFlags;
    /* Ethernet module ID for this driver instance: a plib ETH Id value */
    uintptr_t                       ethModuleId;
    /* Non-volatile pointer to the PHY basic object associated with this MAC */
    const struct DRV_ETHPHY_OBJECT_BASE_TYPE * pPhyBase;
    /* Non-volatile pointer to the PHY initialization data */
    const struct DRV_ETHPHY_INIT *  pPhyInit;
} TCPIP_MODULE_MAC_SAM9X60_CONFIG;

/*******************************************************************************
  Ethernet Driver Module Index Count

  Summary:
    Number of valid Ethernet driver indices.

  Description:
    This constant identifies number of valid Ethernet driver indices.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.

    This value is derived from part-specific header files defined as part of the
    peripheral libraries.
*/

// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************

/*******************************************************************************
  Function:
    SYS_MODULE_OBJ MAC_DRVR_Initialize( const SYS_MODULE_INDEX index,
                                        const SYS_MODULE_INIT * const init
                                        );
  Summary:
    Initializes the Ethernet MAC.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function supports the initialization of the Ethernet MAC.
    Used by tcpip_module_manager.

  Precondition:
    None

  Parameters:
    - index - index of the ETH MAC driver to be initialized
    - init  - Pointer to initialization data

  Returns:
    - a valid handle to a driver object, if successful.
    - SYS_MODULE_OBJ_INVALID if initialization failed.

  Example:
    <code>
    </code>

  Remarks:
    This function initializes the Ethernet controller, the MAC and the associated
    PHY.  It should be called to be able to schedule any Ethernet transmit or
    receive operation.
*/
SYS_MODULE_OBJ MAC_DRVR_Initialize( const SYS_MODULE_INDEX  index, const SYS_MODULE_INIT * const  init);

/*******************************************************************************
  Function:
    void            MAC_DRVR_Deinitialize( SYS_MODULE_OBJ object );

  Summary:
    Deinitializes the Ethernet MAC.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function supports tear down of the Ethernet MAC.  This is the opposite 
    of set up and is used by tcpip_module_manager.

  Precondition:
    MAC_DRVR_Initialize must have been called to set up the driver.

  Parameters:
    - object    - Driver object handle, returned from MAC_DRVR_Initialize

  Returns:
     None.

  Example:
    <code>
    </code>

  Remarks:
    This function deinitializes the Ethernet controller, the MAC and the associated PHY.
    It should be called to be release any resources allocated by the initialization
    and return the MAC and the PHY to the idle/power down state.

*/
void MAC_DRVR_Deinitialize( SYS_MODULE_OBJ object );

/*******************************************************************************
  Function:
    void MAC_DRVR_Reinitialize( SYS_MODULE_OBJ  object, const SYS_MODULE_INIT * const init);
  Summary:
    Reinitializes the Ethernet MAC.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function supports re-initialization of the Ethernet MAC.

  Precondition:
    MAC_DRVR_Initialize must have been called to set up the driver.

  Parameters:
    - object    - Driver object handle, returned from MAC_DRVR_Initialize
    - init      - Pointer to initialization data

  Returns:
     None.

  Example:
    <code>
    </code>

  Remarks:
    This function is not supported yet.
 ******************************************************************************/
void MAC_DRVR_Reinitialize( SYS_MODULE_OBJ  object, const SYS_MODULE_INIT * const  init);

/*******************************************************************************
  Function:
       SYS_STATUS MAC_DRVR_Status( SYS_MODULE_OBJ object )
    
  Summary:
    Provides the current status of the MAC driver module.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function provides the current status of the MAC driver
    module.

  Precondition:
    The MAC_DRVR_Initialize function must have been called before calling
    this function.

  Parameters:
    object -  Driver object handle, returned from MAC_DRVR_Initialize

  Returns:
    - SYS_STATUS_READY  - Indicates that any previous module operation for the
                          specified module has completed
    - SYS_STATUS_BUSY   - Indicates that a previous module operation for the
                          specified module has not yet completed
    - SYS_STATUS_ERROR  - Indicates that the specified module is in an error state

  Example:
    <code>
    </code>

  Remarks:
    None.
 ******************************************************************************/
SYS_STATUS MAC_DRVR_Status( SYS_MODULE_OBJ object );

/*******************************************************************************
  Function:
       void MAC_DRVR_Tasks( SYS_MODULE_OBJ object )
    
  Summary:
    Maintains the EThernet MAC driver's state machine.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function is used to maintain the driver's internal state machine

  Precondition:
    The MAC_DRVR_Initialize routine must have been called for the
    specified MAC driver instance.

  Parameters:
    - object -  Object handle for the specified driver instance (returned from
                MAC_DRVR_Initialize)
  Returns:
    None

  Example:
    <code>
    </code>

  Remarks:
    None.                       

 ******************************************************************************/
void MAC_DRVR_Tasks( SYS_MODULE_OBJ object );

/*******************************************************************************
  Function:
       DRV_HANDLE MAC_DRVR_Open( const SYS_MODULE_INDEX drvIndex,
                                 const DRV_IO_INTENT intent
                                 );
  Summary:
    Opens a client instance of the PIC32 MAC Driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function opens a client instance of the PIC32 MAC Driver.
    Used by tcpip_module_manager.

  Precondition:
    MAC_DRVR_Initialize() should have been called.

  Parameters:
    - drvIndex   - identifier for the driver instance to be opened.
    - intent     - Zero or more of the values from the enumeration
                   DRV_IO_INTENT ORed together to indicate the intended use
                   of the driver

  Return:
    - DRV_HANDLE - pointer to MAC client
    - 0 if call failed

  Example:
    <code>
    </code>

  Remarks:
    The intent parameter is not used in the current implementation and is maintained
    only for compatibility with the generic driver Open function signature.
 ******************************************************************************/
DRV_HANDLE MAC_DRVR_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent );

/*******************************************************************************
  Function:
       void MAC_DRVR_Close( DRV_HANDLE hMac )
    
  Summary:
    Closes a client instance of the PIC32 MAC Driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function closes a client instance of the PIC32 MAC Driver.

  Precondition:
    MAC_DRVR_Open() should have been called.

  Parameters:
    hMac -  valid MAC handle, obtained by a call to MAC_DRVR_Open

  Return:
    None

  Example:
    <code>
    </code>
  Remarks:
    None
 ******************************************************************************/
void MAC_DRVR_Close( DRV_HANDLE hMac );

/*******************************************************************************
  Function:
    bool MAC_DRVR_LinkCheck( DRV_HANDLE hMac )

  Summary:
    Checks current link status.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function checks the link status of the associated network interface.

  Precondition:
    MAC_DRVR_Initialize must have been called to set up the driver.
    MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    hMac - Ethernet MAC client handle

  Returns:
    - true  - If the link is up
    - false - If the link is not up

  Example:
    <code>
    </code>

  Remarks:
    The function will automatically perform a MAC reconfiguration if
    the link went up after being down and the PHY auto negotiation
    is enabled.

 ******************************************************************************/
bool MAC_DRVR_LinkCheck( DRV_HANDLE hMac );

/*******************************************************************************
  Function:
    bool MAC_DRVR_PowerMode( DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode )

  Summary:
    Selects the current power mode for the Ethernet MAC.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the power mode for the Ethernet MAC.

  Precondition:
    MAC_DRVR_Initialize must have been called to set up the driver.
    MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Ethernet MAC client handle
    - pwrMode   - required power mode

  Returns:
    - true if the call succeeded.
    - false if the call failed

  Example:
    <code>
    </code>

  Remarks:
    This function is not currently supported by the Ethernet MAC and
    will always return true.
 ******************************************************************************/
bool MAC_DRVR_PowerMode( DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode );

/*******************************************************************************
  Function:
        TCPIP_MAC_RES MAC_DRVR_RxFilterHashTableEntrySet( DRV_HANDLE  hMac, const TCPIP_MAC_ADDR *  DestMACAddr)
  Summary:
    Sets the current MAC hash table receive filter.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the MAC hash table filtering to allow 
    packets sent to destMacAddr to be received.
    It calculates a CRC-32 using polynomial 0x4C11DB7 over the 6 byte MAC address
    and then, using bits 28:23 of the CRC, will set the appropriate bits in
    the hash table filter registers ( ETHHT0-ETHHT1).

    The function will enable/disable the Hash Table receive filter if needed.
    
  Precondition:
    MAC_DRVR_Initialize() should have been called.
    MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac        -  Ethernet MAC client handle
    - destMacAddr - destination, 6 byte, MAC addresses to be allowed 
                    through the Hash Table Filter.
                    If destMacAddr is set to 00-00-00-00-00-00,
                    then the hash table will be cleared of all entries
                    and the filter will be disabled.
  Return:
    - TCPIP_MAC_RES_OK if success
    - a TCPIP_MAC_RES error value if failed

  Example:
    <code>
    </code>
    
  Remarks:
    - Sets the appropriate bit in the ETHHT0/1 registers to allow packets
      sent to destMacAddr to be received and enabled the Hash Table receive
      filter.
    - There is no way to individually remove destination MAC 
      addresses from the hash table since it is possible to have 
      a hash collision and therefore multiple MAC addresses 
      relying on the same hash table bit.
    - A workaround is to have the stack store each enabled MAC address
      and to perform the comparison at run time.
    - A call to MAC_DRVR_RxFilterHashTableEntrySet() using a 
      00-00-00-00-00-00 destination MAC address, which will clear 
      the entire hash table and disable the hash table filter.  
      This will allow the receive of all packets, regardless of their destination

 ******************************************************************************/
TCPIP_MAC_RES MAC_DRVR_RxFilterHashTableEntrySet( DRV_HANDLE  hMac, const TCPIP_MAC_ADDR *  destMacAddr);

/*******************************************************************************
  Function:
        TCPIP_MAC_RES MAC_DRVR_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * pMacPacket);
    
  Summary:
    MAC driver transmit function.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This is the MAC transmit function.
    Using this function a packet is submitted to the MAC driver for transmission.

  Precondition:
    MAC_DRVR_Initialize() should have been called.
    MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Ethernet MAC client handle
    - pMacPacket - pointer to a TCPIP_MAC_PACKET that's completely formatted
                  and ready to be transmitted over the network

  Return:
    - TCPIP_MAC_RES_OK if success
    - a TCPIP_MAC_RES error value if failed

  Example:
    <code>
    </code>
                                                                                                       
  Remarks:
    - The MAC driver supports internal queuing.
      A packet is rejected only if it's not properly formatted.
      Otherwise it will be scheduled for transmission and queued internally if needed.
    
    - Once the packet is scheduled for transmission the MAC driver will set
      the TCPIP_MAC_PKT_FLAG_QUEUED flag so that the stack is aware that this
      packet is under processing and cannot be modified.
    
    - Once the packet is transmitted, the TCPIP_MAC_PKT_FLAG_QUEUED will be
      cleared, the proper packet acknowledgment result (ackRes) will be
      set and the packet acknowledgment function (ackFunc) will be called.

 ******************************************************************************/
TCPIP_MAC_RES MAC_DRVR_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * pMacPacket);

/*******************************************************************************
  Function:
    TCPIP_MAC_PACKET * MAC_DRVR_PacketRx(   DRV_HANDLE  hMac, TCPIP_MAC_RES * pRes, const TCPIP_MAC_PACKET_RX_STAT * pPktStatus);

  Summary:
    This is the MAC receive function.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will return a packet if such a pending packet exists.
    
    Additional information about the packet is available by providing the pRes and
    pPktStatus fields.

  Precondition:
    MAC_DRVR_Initialize() should have been called.
    MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac        - Ethernet MAC client handle
    - pRes        - optional pointer to an address that will receive an additional
                    result associated with the operation.
                    Can be 0 if not needed.
    - pPktStatus  - optional pointer to an address that will receive the received
                    packet status.
                    Note that this pointer cannot be used once the packet
                    acknowledgment function was called.
                    Can be 0 if not needed.

  Returns:
    - a valid pointer to an available RX packet
    - 0 if no packet pending/available

  Example:
    <code>
    </code>

  Remarks:
    - Once a pending packet is available in the MAC driver internal RX queues
      this function will dequeue the packet and hand it over to the
      MAC driver's client - i.e., the stack - for further processing.

    - The flags for a RX packet are updated by the MAC driver:
        - TCPIP_MAC_PKT_FLAG_RX will be set
        - TCPIP_MAC_PKT_FLAG_UNICAST is set if that packet is a unicast packet
        - TCPIP_MAC_PKT_FLAG_BCAST is set if that packet is a broadcast packet
        - TCPIP_MAC_PKT_FLAG_MCAST is set if that packet is a multicast packet
        - TCPIP_MAC_PKT_FLAG_QUEUED is set
        - TCPIP_MAC_PKT_FLAG_SPLIT is set if the packet has multiple data segments

    - The MAC driver dequeues and return to the caller just one single packet.
      That is the packets are not chained.

    - The packet buffers are allocated by the Ethernet MAC driver itself,
      Once the higher level layers in the stack are done with processing the RX packet,
      they have to call the corresponding packet acknowledgment function
      that tells the MAC driver that it can resume control of that packet.

    - Once the stack modules are done processing the RX packets and the acknowledge function is called
      the MAC driver will reuse the RX packets.

    - The MAC driver may use the MAC_DRVR_Process() for obtaining new RX packets if needed.

 ******************************************************************************/
TCPIP_MAC_PACKET * MAC_DRVR_PacketRx( DRV_HANDLE  hMac, TCPIP_MAC_RES *  pRes, TCPIP_MAC_PACKET_RX_STAT *  pPktStatus);

/*****************************************************************************
  Function:
        TCPIP_MAC_RES MAC_DRVR_Process(DRV_HANDLE hMac);
    
  Summary:
    MAC periodic processing function.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This is a function that allows for internal processing by the MAC
    driver. It is meant for processing that cannot be done from within ISR.
        
    Normally this function will be called in response to an TX and/or RX
    event signaled by the driver. This is specified by the MAC driver at
    initialization time using TCPIP_MAC_MODULE_CTRL.
    
  Precondition:
    MAC_DRVR_Initialize() should have been called.
    MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    hMac -  Ethernet MAC client handle

  Return:
    - TCPIP_MAC_RES_OK if all processing went on OK
    - a TCPIP_MAC_RES error code if processing failed for some reason

  Example:
    <code>
    </code>

  Remarks:
    - The MAC driver may use the MAC_DRVR_Process() for:   
      * Processing its pending TX queues
      * RX buffers replenishing functionality.
        If the number of packets in the RX queue falls below a
        specified limit, the MAC driver may use this function to
        allocate some extra RX packets.
        Similarly, if there are too many allocated RX packets,
        the MAC driver can free some of them.

 ******************************************************************************/
TCPIP_MAC_RES MAC_DRVR_Process( DRV_HANDLE hMac );

/*******************************************************************************
  Function:
     TCPIP_MAC_RES MAC_DRVR_StatisticsGet( DRV_HANDLE hMac,
                                           TCPIP_MAC_RX_STATISTICS * pRxStatistics,
                                           TCPIP_MAC_TX_STATISTICS * pTxStatistics
                                           );
  Summary:
    Gets the current MAC statistics.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will get the current value of the statistic counters
    maintained by the MAC driver.

  Precondition:
   MAC_DRVR_Initialize() should have been called.
   MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac          - handle identifying the MAC driver client

    - pRxStatistics - pointer to a TCPIP_MAC_RX_STATISTICS that will receive the
                      current RX statistics counters
                      Can be NULL if not needed

    - pTxStatistics - pointer to a TCPIP_MAC_TX_STATISTICS that will receive the
                      current TX statistics counters
                      Can be NULL if not needed

  Returns:
    - TCPIP_MAC_RES_OK if all processing went on OK.
    - TCPIP_MAC_RES_OP_ERR error code if function not supported by the driver.

  Example:
    <code>
    </code>

  Remarks:
    - The reported values are info only and change dynamically.
 ******************************************************************************/
TCPIP_MAC_RES MAC_DRVR_StatisticsGet( DRV_HANDLE  hMac, TCPIP_MAC_RX_STATISTICS *  pRxStatistics, TCPIP_MAC_TX_STATISTICS *  pTxStatistics);
    
/*******************************************************************************
   MAC Parameter Get function
     TCPIP_MAC_RES     MAC_DRVR_ParametersGet( DRV_HANDLE  hMac, TCPIP_MAC_PARAMETERS *  pMacParams);
  Summary:
    MAC parameter get function.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This is a function that returns the run time parameters of the MAC driver.

  Precondition:
   MAC_DRVR_Initialize() should have been called.
   MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac        - handle identifying the MAC driver client
    - pMacParams  - address to store the MAC parameters

  Returns:
    - TCPIP_MAC_RES_OK if pMacParams updated properly
    - a TCPIP_MAC_RES error code if processing failed for some reason

  Remarks:
    None.
 ******************************************************************************/
TCPIP_MAC_RES MAC_DRVR_ParametersGet( DRV_HANDLE  hMac, TCPIP_MAC_PARAMETERS *  pMacParams);

/*******************************************************************************
  Function:
     TCPIP_MAC_RES MAC_DRVR_RegisterStatisticsGet( DRV_HANDLE  hMac, TCPIP_MAC_STATISTICS_REG_ENTRY *  pRegEntries, size_t  nEntries, size_t *  pHwEntries);
  Summary:
    Gets the current MAC hardware statistics registers.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will get the current value of the statistic registers
    of the associated MAC controller.

  Precondition:
   MAC_DRVR_Initialize() should have been called.
   MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac          - handle identifying the MAC driver client
    - pRegEntries   - pointer to an array of TCPIP_MAC_STATISTICS_REG_ENTRY that
                      will receive the current hardware statistics registers
                      Can be NULL if not needed
    - nEntries      - number of TCPIP_MAC_STATISTICS_REG_ENTRY in the array.
    - pHwEntries    - address to store the number of hardware supported registers
                      by the associated MAC
                      Can be NULL if not needed
                
  Returns:
    - TCPIP_MAC_RES_OK if all processing went on OK.
    - TCPIP_MAC_RES_OP_ERR error code if function not supported by the driver.

  Example:
    <code>
    </code>

  Remarks:
    - The reported values are info only and change dynamically.
 ******************************************************************************/
TCPIP_MAC_RES MAC_DRVR_RegisterStatisticsGet( DRV_HANDLE  hMac, TCPIP_MAC_STATISTICS_REG_ENTRY *  pRegEntries, size_t  nEntries, size_t *  pHwEntries);
    
/*******************************************************************************
   Function:
     size_t MAC_DRVR_ConfigGet( DRV_HANDLE  hMac, void *  configBuff, size_t  buffSize, size_t *  pConfigSize);
  Summary:
    Gets the current MAC driver configuration.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will get the current MAC driver configuration and store it into
    a supplied buffer.

  Precondition:
   MAC_DRVR_Initialize() should have been called.
   MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac          - handle identifying the MAC driver client

    - configBuff    - pointer to a buffer to store the configuration
                      Can be NULL if not needed

    - buffSize      - size of the supplied buffer

    - pConfigSize   - address to store the number of bytes needed for the storage of the MAC configuration.
                      Can be NULL if not needed
                
  Returns:
    - number of bytes copied into the supplied storage buffer

  Example:
    <code>
    </code>

  Remarks:
    - None
 ******************************************************************************/
size_t MAC_DRVR_ConfigGet( DRV_HANDLE  hMac, void *  configBuff, size_t  buffSize, size_t *  pConfigSize);
    
/*******************************************************************************
  Function:
        bool MAC_DRVR_EventMaskSet( DRV_HANDLE  hMac, TCPIP_MAC_EVENT tcpMacEvents, bool  enable);
  Summary:
    Enables/disables the MAC events.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This is a function that enables or disables the events to be reported
    to the Ethernet MAC client (TCP/IP stack).

     All events that are to be enabled will be added to the notification process.
     All events that are to be disabled will be removed from the notification process.
     The stack has to catch the events that are notified and process them.
     After that the stack should call MAC_DRVR_EventAcknowledge()
     so that the events can be re-enable

     The stack should process at least the following transfer events:
        - TCPIP_MAC_EV_RX_PKTPEND
        - TCPIP_MAC_EV_RX_DONE
        - TCPIP_MAC_EV_TX_DONE

  Precondition:
    MAC_DRVR_Initialize() should have been called.
    MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Ethernet MAC client handle
    - macEvMask - events the user of the stack wants to add/delete for
                  notification
    - enable    - if true, the events will be enabled, else disabled

  Return:
    always true, operation succeeded.

  Example:
    <code>
        MAC_DRVR_EventMaskSet( hMac, TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA, true );
    </code>

  Remarks:
    - The event notification system enables the user of the TCP/IP stack to
      call into the stack for processing only when there are relevant events
      rather than being forced to periodically call from within a loop.
    
    - If the notification events are nil, the interrupt processing will be
      disabled. Otherwise, the event notification will be enabled and the
      interrupts relating to the requested events will be enabled.
    
    - Note that once an event has been caught by the stack ISR (and reported
      if a notification handler is in place) it will be disabled until the
      MAC_DRVR_EventAcknowledge() is called.                                                        
 ******************************************************************************/
bool MAC_DRVR_EventMaskSet( DRV_HANDLE  hMac, TCPIP_MAC_EVENT tcpMacEvents, bool  enable);

/*******************************************************************************
  Function:
   bool    MAC_DRVR_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpMacEvents);
    
  Summary:
    Acknowledges and re-enables processed events.
    <p><b>Implementation:</b> Dynamic</p>
    
  Description:
    This function acknowledges and re-enables processed events. Multiple
    events can be ORed together as they are processed together. The events
    acknowledged by this function should be the events that have been
    retrieved from the stack by calling MAC_DRVR_EventPendingGet()
    or have been passed to the stack by the driver using the registered notification
    handler and have been processed and have to be re-enabled.

  Precondition:
    MAC_DRVR_Initialize() should have been called.
    MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Ethernet MAC client handle
    - tcpMacEvents  - the events that the user processed and need to be re-enabled

  Return:
    - true if events acknowledged
    - false if no events to be acknowledged

  Example:
    <code>
        MAC_DRVR_EventAcknowledge( hMac, stackNewEvents );
    </code>

  Remarks:
    - All events should be acknowledged, in order to be re-enabled.
    
    - Some events are fatal errors and should not be acknowledged
      i.e. TCPIP_MAC_EV_RX_BUSERR, TCPIP_MAC_EV_TX_BUSERR.
      Driver/stack re-initialization is needed under such circumstances.
    
    - Some events are just system/application behavior and they are intended
      only as simple info, i.e. TCPIP_MAC_EV_RX_OVFLOW, TCPIP_MAC_EV_RX_BUFNA,
      TCPIP_MAC_EV_TX_ABORT, TCPIP_MAC_EV_RX_ACT.
    
    - If auto flow control is enabled, the TCPIP_MAC_EV_RX_FWMARK and 
      TCPIP_MAC_EV_RX_EWMARK events are part of the normal flow control.
    
    - The events are persistent. They shouldn't be re-enabled unless they
      have been processed and the condition that generated them was removed.
      Re-enabling them immediately without proper processing will have
      dramatic effects on system performance.                                                          
 ******************************************************************************/
bool MAC_DRVR_EventAcknowledge( DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpMacEvents);

/*******************************#***********************************************
  Function:
      TCPIP_MAC_EVENT MAC_DRVR_EventPendingGet(DRV_HANDLE hMac)
    
  Summary:
    Returns the currently pending events.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the currently pending Ethernet MAC events.
    Multiple events will be ORed together as they accumulate.
    The stack should perform processing whenever a transmission related
    event such as TCPIP_MAC_EV_RX_PKTPEND, or TCPIP_MAC_EV_TX_DONE, is present.
    The other, non critical events, may not be managed by the stack and passed
    to a user. They will have to be eventually acknowledged if re-enabling is
    needed.

  Precondition:
    MAC_DRVR_Initialize() should have been called.
    MAC_DRVR_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac -  parameter identifying the intended MAC client

  Return:
    The currently stack pending events.

  Example:
    <code>
    TCPIP_MAC_EVENT currEvents = MAC_DRVR_EventPendingGet( hMac);
    </code>

  Remarks:
    - This is the preferred method to get the current pending MAC events. The
      stack maintains a proper image of the events from their occurrence to
      their acknowledgment.
    
    - Even with a notification handler in place it's better to use this
      function to get the current pending events rather than using the
      events passed by the notification handler which could be stale.
    
    - The events are persistent. They shouldn't be re-enabled unless they
      have been processed and the condition that generated them was removed.
      Re-enabling them immediately without proper processing will have
      dramatic effects on system performance.
    
    - The returned value is just a momentary value. The pending events can
      change any time.                                                           
 ******************************************************************************/
TCPIP_MAC_EVENT MAC_DRVR_EventPendingGet( DRV_HANDLE hMac );

/*******************************************************************************
  Function:
      void MAC_DRVR_Tasks_ISR( SYS_MODULE_OBJ sysObjHandle )
    
  Summary:
    Ethernet MAC driver interrupt function.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This is the Ethernet MAC driver interrupt service routine.
    It processes the Ethernet related interrupts and notifies the 
    events to the driver user, the TCP/IP stack. 

  Precondition:
    MAC_DRVR_Initialize() should have been called.
    The TCP/IP stack event notification should be enabled.

  Parameters:
    - sysObjHandle -  parameter identifying the intended MAC client

  Return:
    None.

  Remarks:
    None.
 ******************************************************************************/
void MAC_DRVR_Tasks_ISR( SYS_MODULE_OBJ sysObjHandle );

// supported MAC objects
extern const TCPIP_MAC_OBJECT DRV_EMAC0_Object;
extern const TCPIP_MAC_OBJECT DRV_EMAC1_Object;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // H_DRV_EMAC_H_

