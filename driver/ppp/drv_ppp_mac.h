/***********************************************************************
  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_ppp_mac.h

  Summary:
    PPP MAC device driver interface file

  Description:
    PPP MAC Device Driver Interface
    
    The PPP MAC device driver provides a simple interface to manage
    the PPP peripheral. This file defines the interface definitions
    and prototypes for the PPP MAC driver.                         
  ***********************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _DRV_PPP_MAC_H_
#define _DRV_PPP_MAC_H_

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/driver_common.h"

#include "tcpip/tcpip_mac.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// Serial object        
// Definition of the serial object supporting the functionality required by PPP/HDLC
// PPP uses HDLC which needs an object to transport PPP frames over a serial interface: asynchronous, synchronous, modems, etc.
// Currently the asynchronous serial line is supported using the UART plib with ring buffer
typedef struct _tag_SERIAL_HDLC_OBJECT
{
    // read data in a buffer
    size_t (*read)(uint8_t* pRdBuffer, const size_t size);
    // the number of available bytes to be read
    size_t (*readCount)(void);
    // write data from a buffer
    size_t (*write)(uint8_t* pWrBuffer, const size_t size );
    // available write space in the TX buffer
    size_t (*writeFreeSpace)(void);
}SERIAL_HDLC_OBJECT;

/*  PPP MAC Start Flags

  Summary:
    PPP flags that configure the PPP module

  Description:
    This enumerated type defines the configuration flags to be passed to PPP at the initialization
    Multiple enumeration flags could be set

 Note:
    Only 8 bits supported
*/
typedef enum
{
    DRV_PPP_CONFIG_FLAG_NONE            = 0x00,     // none
    DRV_PPP_CONFIG_FLAG_START_NO_OPEN   = 0x01,     // do not issue an admin 'Open' event at start up
                                                    // wait for an PPP_SendAdminEvent call; 
    DRV_PPP_CONFIG_FLAG_START_PASSIVE   = 0x02,     // do not initiate a connection, wait for the peer
                                                    // Note that an 'Open' event is stil needed for PPP to start
                                                    // but PPP will go to 'Stopped' state
    DRV_PPP_CONFIG_FLAG_ALWAYS_PASSIVE  = 0x04,     // Remain passive after any 'Open' event, not only at start 
                                                    // i.e. from 'Closed' go to  'Stopped' and wait
    

    // other flags may be eventualy added
}DRV_PPP_CONFIG_FLAG;



/*  PPP LCP Configuration Options

  Summary:
    PPP Options that are to be negotiated with the peer as part of the PPP link configuration.

  Description:
    This enumerated type defines the LCP configuration options to be passed to PPP at the initialization
    Multiple enumeration flags could be set

 Note:
    Only 16 bits supported
*/
typedef enum
{
    DRV_LCP_OPT_FLAG_NONE       = 0x0000,     // none
    DRV_LCP_OPT_FLAG_MRU        = 0x0001,     // LCP: negotiate MRU - Maximum Receive Unit
    DRV_LCP_OPT_FLAG_MAGIC_NO   = 0x0002,     // LCP: negotiate Magic-Number
    DRV_LCP_OPT_FLAG_ACCM       = 0x0004,     // LCP: ACCM - Async Control Character Map


    // Not supported (yet) configuration options.
    // These will be eventually added.
    /*
    DRV_LCP_OPT_FLAG_PFC            , // LCP: Protocol-Field-Compression
    DRV_LCP_OPT_FLAG_ACFC           , // LCP: Address-and-Control-Field-Compression
    DRV_LCP_OPT_FLAG_AUTH_PROT      , // LCP: Authentication-Protocol
    DRV_LCP_OPT_FLAG_QUALITY_PRO    , // LCP: Quality-Protocol
    */
}DRV_LCP_CONFIG_OPTION_FLAG;

/*  PPP IPCP Configuration Options

  Summary:
    PPP Options that are to be negotiated with the peer as part of the PPP IP configuration.

  Description:
    This enumerated type defines the IPCP configuration options to be passed to PPP at the initialization
    Multiple enumeration flags could be set

 Note:
    Only 8 bits supported
*/
typedef enum
{
    DRV_IPCP_OPT_FLAG_NONE                      = 0x00, // none
    DRV_IPCP_OPT_FLAG_NO_LCL_ADDR4              = 0x01, // do not provide/negotiate a local IPv4 address option
                                                        // Normally this should NOT be set as a local IPv4 address is required


    // Other configuration options will be eventually added
    //
}DRV_IPCP_CONFIG_IPCP_OPTION_FLAG;



/*  PPP MAC Initialization Data

  Summary:
    Data that's passed to the MAC at initialization time as part of the
    TCPIP_MAC_INIT data structure.

  Description:
    This structure defines the MAC initialization data for the
    PPP MAC controller.

*/

typedef struct
{
    /* restart timeout, seconds
       Restart timer is used to time transmissions of Configure-Request and Terminate-Request packets.
       Expiration of the Restart timer causes a Timeout event, and retransmission
       of the corresponding Configure-Request or Terminate-Request packet.
       Defaults to 3 seconds. Max value is 65 seconds.
       Leave 0 for default */
    uint16_t    restartTmo;

    /* Max-Terminate counter.
       Indicates the number of Terminate-Request packets sent without receiving
       a Terminate-Ack before assuming that the peer is unable to respond.
       Defaults to 2
       Leave 0 for default */
    uint16_t    maxTerminate;

    /* Max-Configure indicates the number of Configure-Request packets sent
       without receiving a valid Configure-Ack, Configure-Nak or
       Configure-Reject before assuming that the peer is unable to
       respond. 
       Defaults to 10
       Leave 0 for default */
    uint16_t    maxConfigure;

    /*
      Max-Failure indicates the number of Configure-Nak packets sent without sending
      a Configure-Ack before assuming that configuration is not converging.
      Any further Configure-Nak packets for peer requested options are converted to
      Configure-Reject packets.
       Defaults to 5
       Leave 0 for default */
    uint16_t    maxFailure;


    /* Number of MAC dedicated RX buffers */
    /* These buffers/packets are owned by the PPP MAC and are not returned to the packet pool */
    /* They are allocated at MAC initialization time using pktAllocF */ 
    /* and freed at MAC deinitialize time using pktFreeF */
    /* Could be 0, if only not dedicated buffers are needed. */
    /* For best performance usually it's best to have some dedicated buffers */
    /* so as to minimize the run time allocations */
    uint16_t    nRxDedicatedBuffers;
    
    /* Number of MAC non dedicated RX buffers allocated at the MAC initialization pktAllocF */
    /* Note that these buffers are allocated in addition of the nRxDedicatedBuffers */
    /* Freed at run time using pktFreeF */
    uint16_t    nRxInitBuffers;

    /* Minimum threshold for the buffer replenish process */
    /* Whenever the number of RX scheduled buffers is <= than this threshold */
    /* the MAC driver will allocate new non-dedicated buffers */
    /* that will be freed at run time using pktFreeF */
    /* Setting this value to 0 disables the buffer replenishing process */
    uint16_t    rxLowThreshold;

    /* Number of RX buffers to allocate when below threshold condition is detected */
    /* If 0, the MAC driver will allocate (scheduled buffers - rxThres) */
    /* If !0, the MAC driver will allocate exactly rxLowFill buffers */
    uint16_t    rxLowFill;

    // flags for the configuration options to be negotiated with the peer at initialization of the PPP link
    // a DRV_LCP_CONFIG_OPTION_FLAG value
    uint16_t    lcpConfigFlags;


    // flags for the IPCP configuration options to be negotiated with the peer
    // a DRV_IPCP_CONFIG_IPCP_OPTION_FLAG value
    uint8_t     ipcpConfigFlags;

    // flags for the PPP module configuration
    // a DRV_PPP_CONFIG_FLAG value
    uint8_t    configFlags;

    /* Maximum receive unit.
       Used only if lcpConfigFlags.DRV_LCP_OPT_FLAG_MRU is set! 
       Specifies the maximum number of octets in a PPP frame.
       Default value is 1500 bytes. 
       If the mru == 0 or 1500 (default value), then the MRU configuration option
       will NOT be negotiated as it is implied by the standard. */
    uint16_t    mru;

    /* Echo request timeout - in ms
       An echo request will fail if not responded within this timeout
       Usually 100 ms but depends on the link speed
       Used if PPP_ECHO_REQUEST_ENABLE is defined */
    uint16_t    echoTmo;

    /* Local IPv4 address.
        Used if ipcpConfigFlags.DRV_IPCP_OPT_FLAG_NO_LCL_ADDR4 is not set!
        It should be a valid IPv4 address
        Use 0, to request the local IPv4 address from the peer.
        NOTE: once the PPP negotiation is complete the static IP address of the interface is updated! */
    const char* lclIpv4Addr;

    /* Remote peer IPv4 address.
        It should be a valid IPv4 address
        Use 0, to have the peer to provide its IPv4 address.
        If != 0 and the peer has not provided its own address,
        then this address will be negotiated for the peer */
    const char* peerIpv4Addr;

    /* Function to return a 32 bit random magic number.
       Used only if lcpConfigFlags.DRV_LCP_OPT_FLAG_MAGIC_NO is set! 
       A good random number should be returned.
       If the magic_no_func == 0, then the Magic-Number configuration option
       will be negotiated but the random number will be generated using SYS_RANDOM_PseudoGet(). 
       The default is SYS_RANDOM_PseudoGet().*/
    uint32_t    (*magic_no_func)(void);

    /* this side Asynchronous Control Characters Map to be communicated to the peer.
       A 32 bit value identifying the ACCM.
       Any control character (less than 0x20) mapped in this ACCM will be control-escaped when transmitted.
       Default is 0xffffffff meaning all control characters will be escaped.
       For a link where control characters are not introduced, the map could be reduced
       and the amount of extra data transmitted will be lower.
       Used only if lcpConfigFlags.DRV_LCP_OPT_FLAG_ACCM is set!  */
    uint32_t    rxAccm;

    /* Non-volatile pointer to the HDLC object to be used by PPP MAC */
    const struct _tag_DRV_HDLC_OBJECT*   pHdlcObj;   

    /* size of the HDLC processing buffer to perform reads from the serial object
       and assemble HDLC frames.
       Th closer to the MRU the better, as less reads required to assemble a frame
       Recommended size: >= 1500 bytes
       Leave 0 for default */
    uint16_t      hdlcProcSize;

    /* size of aditional HDLC peek buffer to perform reads from the serial object
       and assemble HDLC frames.
       Needed because the serial object doesn't have a peek function
       Recommended size: >= 128 bytes
       Leave 0 for default */
    uint16_t      hdlcPeekSize;

    /* index of the HDLC object to use */
    SYS_MODULE_INDEX    hdlcIx;

    // the serial object to be used
    const SERIAL_HDLC_OBJECT* hdlcSerialObj;
}TCPIP_MODULE_MAC_PPP_CONFIG;

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ      DRV_PPP_MAC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

  Summary:
    Initializes the PPP MAC.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function supports the initialization of the PPP MAC.  Used by tcpip_module_manager.

  Precondition:
    None

  Parameters:
    - index - index of the PPP MAC driver to be initialized
    - init  - Pointer to TCPIP_MAC_INIT initialization data

  Returns:
    - a valid handle to a driver object, if successful.
    - SYS_MODULE_OBJ_INVALID if initialization failed.

  Example:
    <code>
    </code>

  Remarks:
    This function initializes the PPP link
    It should be called to be able to schedule any PPP transmit or receive operation.

*/
SYS_MODULE_OBJ DRV_PPP_MAC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);


// *****************************************************************************
/* Function:
    void                DRV_PPP_MAC_Deinitialize(SYS_MODULE_OBJ object);

  Summary:
    Deinitializes the PPP MAC.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function supports teardown of the PPP MAC (opposite of set up).  Used by tcpip_module_manager.

  Precondition:
    DRV_PPP_MAC_Initialize must have been called to set up the driver.

  Parameters:
    - object    - Driver object handle, returned from DRV_PPP_MAC_Initialize

  Returns:
     None.

  Example:
    <code>
    </code>

  Remarks:
    This function deinitializes the PPP MAC and the associated link.
    It should be called to be release any resources allocated by the initialization
    and return the MAC and the link to the idle/power down state.

*/
void DRV_PPP_MAC_Deinitialize(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    void DRV_PPP_MAC_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);

  Summary:
    Reinitializes the PPP MAC.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function supports re-initialization of the PPP MAC (opposite of set up).

  Precondition:
    DRV_PPP_MAC_Initialize must have been called to set up the driver.

  Parameters:
    - object    - Driver object handle, returned from DRV_PPP_MAC_Initialize
    - init      - Pointer to initialization data

  Returns:
     None.

  Example:
    <code>
    </code>

  Remarks:
    This function is not supported yet.

*/
void DRV_PPP_MAC_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);



/**************************************************************************
  Function:
       SYS_STATUS DRV_PPP_MAC_Status ( SYS_MODULE_OBJ object )
    
  Summary:
    Provides the current status of the MAC driver module.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function provides the current status of the MAC driver
    module.

  Precondition:
    The DRV_PPP_MAC_Initialize function must have been called before calling
    this function.

  Parameters:
    object -  Driver object handle, returned from DRV_PPP_MAC_Initialize

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
 **************************************************************************/

SYS_STATUS DRV_PPP_MAC_Status ( SYS_MODULE_OBJ object );

/***************************************************************************
  Function:
       void DRV_PPP_MAC_Tasks( SYS_MODULE_OBJ object )
    
  Summary:
    Maintains the PPP MAC driver's state machine.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function is used to maintain the driver's internal state machine

  Precondition:
    The DRV_PPP_MAC_Initialize routine must have been called for the
    specified MAC driver instance.

  Parameters:
    - object -  Object handle for the specified driver instance (returned from
                DRV_PPP_MAC_Initialize)
  Returns:
    None

  Example:
    <code>
    </code>

  Remarks:
    None.                       

  ***************************************************************************/

void DRV_PPP_MAC_Tasks( SYS_MODULE_OBJ object );

/**************************************************************************
  Function:
       DRV_HANDLE DRV_PPP_MAC_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
    
  Summary:
    Opens a client instance of the PPP MAC Driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function opens a client instance of the PPP MAC Driver.
    Used by tcpip_module_manager.

  Precondition:
    DRV_PPP_MAC_Initialize() should have been called.

  Parameters:
    - drvIndex   - identifier for the driver instance to be opened.
    - intent     - Zero or more of the values from the enumeration
                   DRV_IO_INTENT ORed together to indicate the intended use
                   of the driver

  Return:
    - DRV_HANDLE - handle (pointer) to MAC client
    - 0 if call failed

  Example:
    <code>
    </code>

  Remarks:
    The intent parameter is not used in the current implementation and is maintained only for compatibility
    with the generic driver Open function signature.
  **************************************************************************/
DRV_HANDLE DRV_PPP_MAC_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);


/************************************************************************
  Function:
       void DRV_PPP_MAC_Close( DRV_HANDLE hMac )
    
  Summary:
    Closes a client instance of the PPP MAC Driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function closes a client instance of the PPP MAC Driver.

  Precondition:
    DRV_PPP_MAC_Open() should have been called.

  Parameters:
    hMac -  valid MAC handle, obtained by a call to DRV_PPP_MAC_Open

  Return:
    None

  Example:
    <code>
    </code>
  Remarks:
    None
  ************************************************************************/

void DRV_PPP_MAC_Close( DRV_HANDLE hMac );


// *****************************************************************************
/* Function:
    bool DRV_PPP_MAC_LinkCheck( DRV_HANDLE hMac )

  Summary:
    Checks current link status.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function checks the link status of the associated network interface.

  Precondition:
    DRV_PPP_MAC_Initialize must have been called to set up the driver.
    DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    hMac - PPP MAC client handle

  Returns:
    - true  - If the link is up
    - false - If the link is not up

  Example:
<code>
</code>

  Remarks:
    TODO

*/

bool DRV_PPP_MAC_LinkCheck( DRV_HANDLE hMac );


// *****************************************************************************
/* Function:
    bool DRV_PPP_MAC_PowerMode( DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode )

  Summary:
    Selects the current power mode for the PPP MAC.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the power mode for the PPP MAC.

  Precondition:
    DRV_PPP_MAC_Initialize must have been called to set up the driver.
    DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - PPP MAC client handle
    - pwrMode   - required power mode

  Returns:
    - true if the call succeeded.
    - false if the call failed

  Example:
<code>
</code>

  Remarks:
    This function is not currently supported by the PPP MAC and
    will always return true.
*/

bool DRV_PPP_MAC_PowerMode( DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode );

/*********************************************************************************************************************
  Function:
        TCPIP_MAC_RES DRV_PPP_MAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr)
    
  Summary:
    Sets the current MAC hash table receive filter.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    The function does not apply to the PPP link.
    
  Precondition:
    DRV_PPP_MAC_Initialize() should have been called.
    DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac        -  PPP MAC client handle
    - DestMACAddr - not used

  Return:
    - TCPIP_MAC_RES_OK

  Example:
    <code>
    </code>
    
  Remarks:
    This function is not supported by the PPP MAC and
    will always return TCPIP_MAC_RES_OK.

  *********************************************************************************************************************/
TCPIP_MAC_RES DRV_PPP_MAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr);

/******************************************************************************************************
  Function:
        TCPIP_MAC_RES DRV_PPP_MAC_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);
    
  Summary:
    MAC driver transmit function.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This is the MAC transmit function.
    Using this function a packet is submitted to the MAC driver for transmission.

  Precondition:
    DRV_PPP_MAC_Initialize() should have been called.
    DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - PPP MAC client handle
    - ptrPacket - pointer to a TCPIP_MAC_PACKET that's completely formatted
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

  ******************************************************************************************************/
TCPIP_MAC_RES DRV_PPP_MAC_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);


// *****************************************************************************
/* Function:
    TCPIP_MAC_PACKET* DRV_PPP_MAC_PacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat);

  Summary:
    This is the MAC receive function.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will return a packet if such a pending packet exists.
    
    Additional information about the packet is available by providing the pRes and
    pPktStat fields.

  Precondition:
    DRV_PPP_MAC_Initialize() should have been called.
    DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac        - PPP MAC client handle
    - pRes        - optional pointer to an address that will receive an additional
                    result associated with the operation.
                    Can be 0 if not needed.
    - pPktStat    - optional pointer to an address where the received packet status will be copied
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
      MAC driver's client - i.e. the stack - for further processing.

    - The flags for a RX packet are updated by the MAC driver:
        - TCPIP_MAC_PKT_FLAG_RX will be set
        - TCPIP_MAC_PKT_FLAG_UNICAST is set if that packet is a unicast packet
        - TCPIP_MAC_PKT_FLAG_BCAST is set if that packet is a broadcast packet
        - TCPIP_MAC_PKT_FLAG_MCAST is set if that packet is a multicast packet
        - TCPIP_MAC_PKT_FLAG_QUEUED is set

    - The MAC driver dequeues and return to the caller just one single packet.
      That is the packets are not chained.

    - The packet buffers are allocated by the PPP MAC driver itself,
      Once the higher level layers in the stack are done with processing the RX packet,
      they have to call the corresponding packet acknowledgment function
      that tells the MAC driver that it can resume control of that packet.

    - Once the stack modules are done processing the RX packets and the acknowledge function is called
      the MAC driver will reuse the RX packets.

    - The MAC driver may use the DRV_PPP_MAC_Process() for obtaining new RX packets if needed.

*/
TCPIP_MAC_PACKET* DRV_PPP_MAC_PacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat);


/*****************************************************************************
  Function:
        TCPIP_MAC_RES DRV_PPP_MAC_Process(DRV_HANDLE hMac);
    
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
    DRV_PPP_MAC_Initialize() should have been called.
    DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    hMac -  PPP MAC client handle

  Return:
    - TCPIP_MAC_RES_OK if all processing went on OK
    - a TCPIP_MAC_RES error code if processing failed for some reason

  Example:
    <code>
    
    </code>

  Remarks:
    - The MAC driver may use the DRV_PPP_MAC_Process() for:   
      * Processing its pending TX queues
      * RX buffers replenishing functionality.
        If the number of packets in the RX queue falls below a
        specified limit, the MAC driver may use this function to
        allocate some extra RX packets.
        Similarly, if there are too many allocated RX packets,
        the MAC driver can free some of them.

  *****************************************************************************/
TCPIP_MAC_RES DRV_PPP_MAC_Process(DRV_HANDLE hMac);


// *****************************************************************************
/*  Function:
     TCPIP_MAC_RES       DRV_PPP_MAC_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);

  Summary:
    Gets the current MAC statistics.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will get the current value of the statistic counters
    maintained by the PPP MAC driver.


  Precondition:
   DRV_PPP_MAC_Initialize() should have been called.
   DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac          - handle identifying the MAC driver client

    - pRxStatistics - pointer to a TCPIP_MAC_RX_STATISTICS that will receive the current 
                      RX statistics counters
                      Can be NULL if not needed

    - pTxStatistics - pointer to a TCPIP_MAC_TX_STATISTICS that will receive the current 
                      TX statistics counters
                      Can be NULL if not needed

  Returns:
    - TCPIP_MAC_RES_OK if all processing went on OK.
    - TCPIP_MAC_RES_OP_ERR error code if function not supported by the driver.

  Example:
    <code>
    </code>

  Remarks:
    - The reported values are info only and change dynamically.

*/
TCPIP_MAC_RES       DRV_PPP_MAC_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);
    
// *****************************************************************************
/*  MAC Parameter Get function
     TCPIP_MAC_RES     DRV_PPP_MAC_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);

  Summary:
    MAC parameter get function.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This is a function that returns the run time parameters of the MAC driver.

  Precondition:
   DRV_PPP_MAC_Initialize() should have been called.
   DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac        - handle identifying the MAC driver client
    - pMacParams  - address to store the MAC parameters

  Returns:
    - TCPIP_MAC_RES_OK if pMacParams updated properly
    - a TCPIP_MAC_RES error code if processing failed for some reason

  Remarks:
    Note that some parameters may NOT apply to a PPP link over a serial line.

*/
TCPIP_MAC_RES     DRV_PPP_MAC_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);

// *****************************************************************************
/*  Function:
     TCPIP_MAC_RES       DRV_PPP_MAC_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);

  Summary:
    Gets the current MAC hardware statistics registers.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will get the current value of the statistic registers
    of the associated MAC controller.



  Precondition:
   DRV_PPP_MAC_Initialize() should have been called.
   DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac          - handle identifying the MAC driver client

    - pRegEntries   - pointer to an array of TCPIP_MAC_STATISTICS_REG_ENTRY that will receive the current 
                      hardware statistics registers
                      Can be NULL if not needed

    - nEntries      - number of TCPIP_MAC_STATISTICS_REG_ENTRY in the array.

    - pHwEntries    - address to store the number of hardware supported registers by the associated MAC
                      Can be NULL if not needed
                
  Returns:
    - TCPIP_MAC_RES_OK if all processing went on OK.
    - TCPIP_MAC_RES_OP_ERR error code if function not supported by the driver.

  Example:
    <code>
    </code>

  Remarks:
    The reported values are info only and change dynamically.

    Note that this function may NOT apply to a PPP link over a serial line.

*/
TCPIP_MAC_RES       DRV_PPP_MAC_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);
    
// *****************************************************************************
/*  Function:
     size_t      DRV_PPP_MAC_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);

  Summary:
    Gets the current MAC driver configuration.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will get the current MAC driver configuration and store it into
    a supplied buffer.

  Precondition:
   DRV_PPP_MAC_Initialize() should have been called.
   DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

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

*/
size_t      DRV_PPP_MAC_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);
    
/***********************************************************************************************************
  Function:
        bool DRV_PPP_MAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);
    
  Summary:
    Enables/disables the MAC events.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This is a function that enables or disables the events to be reported
    to the PPP MAC client (TCP/IP stack).

     All events that are to be enabled will be added to the notification process.
     All events that are to be disabled will be removed from the notification process.
     The stack has to catch the events that are notified and process them.
     After that the stack should call DRV_PPP_MAC_EventAcknowledge()
     so that the events can be re-enable

     The stack should process at least the following transfer events:
        - TCPIP_MAC_EV_RX_PKTPEND
        - TCPIP_MAC_EV_RX_DONE
        - TCPIP_MAC_EV_TX_DONE

  Precondition:
    DRV_PPP_MAC_Initialize() should have been called.
    DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - PPP MAC client handle
    - macEvMask - events the user of the stack wants to add/delete for
                  notification
    - enable    - if true, the events will be enabled, else disabled

  Return:
    always true, operation succeeded.

  Example:
    <code>
        DRV_PPP_MAC_EventMaskSet( hMac, TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA, true );
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
      DRV_PPP_MAC_EventAcknowledge() is called.                                                        
  ***********************************************************************************************************/
bool DRV_PPP_MAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);


/****************************************************************************************************
  Function:
   bool    DRV_PPP_MAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv);
    
  Summary:
    Acknowledges and re-enables processed events.
    <p><b>Implementation:</b> Dynamic</p>
    
  Description:
    This function acknowledges and re-enables processed events. Multiple
    events can be ORed together as they are processed together. The events
    acknowledged by this function should be the events that have been
    retrieved from the stack by calling DRV_PPP_MAC_EventPendingGet()
    or have been passed to the stack by the driver using the registered notification handler
    and have been processed and have to be re-enabled.

  Precondition:
    DRV_PPP_MAC_Initialize() should have been called.
    DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - PPP MAC client handle
    - tcpAckEv  - the events that the user processed and need to be re-enabled

  Return:
    - true if events acknowledged
    - false if no events to be acknowledged

  Example:
    <code>
        DRV_PPP_MAC_EventAcknowledge( hMac, stackNewEvents );
    </code>

  Remarks:
    - All events should be acknowledged, in order to be re-enabled.
    
    - Some events are fatal errors and should not be acknowledged
      ( TCPIP_MAC_EV_RX_BUSERR, TCPIP_MAC_EV_TX_BUSERR).
      Driver/stack re-initialization is needed under such circumstances.
    
    - Some events are just system/application behavior and they are intended
      only as simple info (TCPIP_MAC_EV_RX_OVFLOW, TCPIP_MAC_EV_RX_BUFNA,
      TCPIP_MAC_EV_TX_ABORT, TCPIP_MAC_EV_RX_ACT).
    
    - The TCPIP_MAC_EV_RX_FWMARK and TCPIP_MAC_EV_RX_EWMARK events are part
      of the normal flow control operation (if auto flow control was
      enabled). They should be enabled alternatively, if needed.
    
    - The events are persistent. They shouldn't be re-enabled unless they
      have been processed and the condition that generated them was removed.
      Re-enabling them immediately without proper processing will have
      dramatic effects on system performance.                                                          
  ****************************************************************************************************/
bool    DRV_PPP_MAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv);



/******************************************************************************
  Function:
      TCPIP_MAC_EVENT DRV_PPP_MAC_EventPendingGet(DRV_HANDLE hMac)
    
  Summary:
    Returns the currently pending events.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the currently pending PPP MAC events.
    Multiple events will be ORed together as they accumulate.
    The stack should perform processing whenever a transmission related
    event (TCPIP_MAC_EV_RX_PKTPEND, TCPIP_MAC_EV_TX_DONE) is present.
    The other, non critical events, may not be managed by the stack and passed to an
    user. They will have to be eventually acknowledged if re-enabling is
    needed.

  Precondition:
    DRV_PPP_MAC_Initialize() should have been called.
    DRV_PPP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac -  parameter identifying the intended MAC client

  Return:
    The currently stack pending events.

  Example:
    <code>
    TCPIP_MAC_EVENT currEvents = DRV_PPP_MAC_EventPendingGet( hMac);
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
TCPIP_MAC_EVENT DRV_PPP_MAC_EventPendingGet(DRV_HANDLE hMac);



//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _DRV_PPP_MAC_H_

/*******************************************************************************
 End of File
*/

