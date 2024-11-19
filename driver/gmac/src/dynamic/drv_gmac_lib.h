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
Copyright (C) 2017-2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


#ifndef _DRV_PIC32CGMAC_LIB_H_
#define _DRV_PIC32CGMAC_LIB_H_

#include <stdlib.h>
#include "system_config.h"
#include "system/debug/sys_debug.h"
#include "system/time/sys_time.h"
#include "system/sys_time_h2_adapter.h"
#include "tcpip/tcpip_ethernet.h"
#include "driver/gmac/src/dynamic/_gmac_dcpt_lists.h"
#if defined (DRV_ETH)
    #if defined (DRV_PIC32CZCA8) || defined (DRV_PIC32CZCA9) 
        #include "driver/gmac/src/dynamic/drv_gmac_lib_pic32cz.h"
    #elif defined (DRV_PIC32CK)
        #include "driver/gmac/src/dynamic/drv_gmac_lib_pic32ck.h"
    #elif defined (DRV_WBZ653)
        #include "driver/gmac/src/dynamic/drv_gmac_lib_pic32cxbz6.h"
    #elif defined (DRV_PIC32CXBZ6)
        #include "driver/gmac/src/dynamic/drv_gmac_lib_pic32cxbz6.h"
    #endif
#else
    #include "driver/gmac/src/dynamic/drv_gmac_lib_sam.h"
#endif
#include "driver/gmac/src/drv_gmac_local.h"
#include "device.h"

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

/*******************************************************************************
  Function:
    void DRV_PIC32CGMAC_LibTxClearUnAckPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes);
 
  Summary:
    Acknowledge the Unacknowledged Transmit packets during transmit errors

  Description:
    This function acknowledges packets added to MAC transmit descriptors. This function will
    acknowledge all the unacknowledged transmitted or not transmitted packets in transmit descriptor. 
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
    DRV_PIC32CGMAC_LibTxClearUnAckPacket(pMACDrv, queueIdx, ackRes); 
    </code>

  Remarks:
    Any TX packets in transmit descriptors has to be acknowledged during error condition, 
    otherwise the memory allocated for the packet will not be freed.

    <p>Replaces:<p><c><b>void DRV_PIC32CGMAC_LibTxClearUnAckPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes)</b></c>
 *****************************************************************************/
void DRV_PIC32CGMAC_LibTxClearUnAckPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes);
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
/*******************************************************************************
  Function:
    void  DRV_PIC32CGMAC_LibClearTxIndex(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx);
 
  Summary:
    Reset Transmit processing indexes.

  Description:
    After TXEN reset, the Transmit Queue Pointer will point to the start of the 
    transmit descriptor list. The GMAC H/W engine will look into the first descriptor for 
    any subsequent transmission. So, the software indexes used for Transmit descriptors
    must be cleared.

  Precondition:
    None

  Parameters:
    pMACDrv         - driver instance.
    queueIdx        - queue Index

  Returns:
    None

  Example:
    <code>
    DRV_PIC32CGMAC_LibClearTxIndex(pMACDrv, queueIdx); 
    </code>

  Remarks:
    Any TX packets in transmit queues must be acked/cleared

    <p>Replaces:<p><c><b>void  DRV_PIC32CGMAC_LibClearTxIndex(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx)</b></c>
 *****************************************************************************/
void  DRV_PIC32CGMAC_LibClearTxIndex(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx);
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
#endif  // _DRV_PIC32CGMAC_LIB_H_

