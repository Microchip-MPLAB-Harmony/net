/*******************************************************************************
  ENCx24J600 Driver Public Interface
  
  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_encx24j600.h
	
  Summary:
    ENCx24J600 Driver interface definition.
	
  Description:
    This file defines the interface definition for the ENCx24J600 Driver.
	  
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2014-2020 Microchip Technology Inc. and its subsidiaries.

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

// DOM-IGNORE-END

#if !defined(_DRV_ENCX24J600_H_)
#define _DRV_ENCX24J600_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "driver/driver_common.h"
#include "driver/spi/drv_spi_definitions.h"
#include "tcpip/tcpip_mac.h"
#include "tcpip/tcpip_ethernet.h"
#include "tcpip/tcpip_mac_object.h"
#include "system/system_module.h"
#include "system/system_common.h"
#include <stdbool.h>
#include <string.h>    
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* ENCX24J600 Driver MDIX Control type

  Summary:
    Defines the enumeration for controlling the MDIX select.

  Description:
    This type defines the enumeration for controlling the MDIX select.

  Remarks:
    None.
*/
typedef enum
{
    /* No Control */
    DRV_ENCX24J600_NO_CONTROL = 0,
    /* Normal MDIX*/
    DRV_ENCX24J600_NORMAL,
    /* Reverse MDIX*/
    DRV_ENCX24J600_REVERSE = 0,

}DRV_ENCX24J600_MDIX_TYPE;


// *****************************************************************************
/* ENCX24J600 Driver Initialization Data

  Summary:
    Defines the data required to initialize or reinitialize the ENCX24J600 Driver.

  Description:
    This data type defines the data required to initialize or reinitialize the
    ENCX24J600 driver. If the driver is built statically, the members of this data
    structure are statically over-ridden by static override definitions in the
    system_config.h file.

  Remarks:
    None.
*/

typedef struct _DRV_ENCX24J600_Configuration
{
    /* Number of TX Descriptors to Allocate*/
	uint16_t txDescriptors;
    /* Number of RX Descriptors to Allocate*/
	uint16_t rxDescriptors;
    /* Size of the buffer each RX Descriptor will use.  Make sure its not smaller that maxFrameSize*/
	uint16_t rxDescBufferSize;
    /* Index of the SPI driver to use */
	SYS_MODULE_INDEX spiDrvIndex;
    /* Bus speed to use for the SPI interface.  Section 1.0 of the ENCX24J600 data sheets says the maximum is 14000000 Hz.  It is not recommended to go above this value.*/
	uint32_t spiBps;
    /* The ENCX24J600 hardware has a 22 k dram.  rxBufferSize defines how much of that memory is used by the rxBuffer*/
	uint16_t rxBufferSize;
    /* The maximum frame size to be supported by the hardware.  1536 is the default*/
	uint16_t maxFrameSize;

    //SYS_PORT_PIN spiSSPortPin;
    DRV_SPI_TRANSFER_SETUP spiSetup;    
    /* Use Interrupts or not.*/
    bool intEnable;
	/* Ethernet type*/
	TCPIP_ETH_OPEN_FLAGS ethType;
	/* Duplex Mode */
	TCPIP_ETH_OPEN_FLAGS dupMode;
}  DRV_ENCX24J600_Configuration;

// *****************************************************************************
// *****************************************************************************
// Section: Global Data - System Level
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* ENCX24J600 External MAC Virtualization Table
extern const TCPIP_MAC_OBJECT DRV_ENCX24J600_MACObject;
*/

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - System Level
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* ENCX24J600 Initialization

    Summary:
      Initializes the ENCx24J600 Driver Instance, with the configuration data.
	  <p><b>Implementation:</b> Dynamic</p>
    
    Description:
      This function initializes the ENCx24J600 Driver with configuration data 
      passed into it by either the system_init function or by the 
      DRV_ENCX24J600_StackInitialize function. Calling this function alone is 
      not enough to initialize the driver,  DRV_ENCX24J600_SetMacCtrlInfo must 
      be called with valid data before the driver is ready to be opened.
    
    Preconditions:
      None.

    Parameters
      index	- This is the index of the driver instance to be initialized.  
	          The definition DRV_ENCX24J600_NUM_DRV_INSTANCES controls how many instances 
	          are available.
      init	- This is a pointer to a DRV_ENX24J600_CONFIG structure.
    
    Returns
      - Valid handle to the driver instance		- If successful
      - SYS_MODULE_OBJ_INVALID					- If unsuccessful 
*/
SYS_MODULE_OBJ DRV_ENCX24J600_Initialize(SYS_MODULE_INDEX index, SYS_MODULE_INIT * init);

// *****************************************************************************
/* ENCX24J600 Deinitialization
    Summary:
      Deinitializes the ENCx24J600 Driver Instance.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      This function deallocates any resources allocated by the initialization function.  

    Preconditions:
      The driver had to be successfully initialized with DRV_ENCX24J600_Initialize.
    
    Parameters:
      Object	- the valid object returned from DRV_ENCX24J600_Initialize

    Returns:
      None.
*/
void DRV_ENCX24J600_Deinitialize(SYS_MODULE_OBJ object);

// *****************************************************************************
/* ENCX24J600 Reinitialization

    Summary:
      Reinitializes the instance of the ENCX24J600 driver.
	  <p><b>Implementation:</b> Dynamic</p>
    
    Description:
      This function will deinitialize and initialize the driver instance.  As with 
      DRV_ENCX24J600_Initialize DRV_ENCX24J600_SetMacCtrlInfo must be called for 
      the driver to be useful.  
      Note: This function is not planned to be implemented for the first release.

    Preconditions:
      The driver had to be successfully initialized with DRV_ENCX24J600_Initialize.

    Parameters:
      - object	- The object valid passed back to DRV_ENCX24J600_Initialize
      - init	- The new initialization structure.

    Returns:
      None
	  
    */
void DRV_ENCX24J600_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* ENCX24J600 Status

    Summary:
      Gets the current status of the driver.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      This function will get the status of the driver instance.

    Preconditions:
      The driver had to be successfully initialized with DRV_ENCX24J600_Initialize().

    Parameters:
      object	- The object valid passed back to DRV_ENCX24J600_Initialize()

    Returns:
      - SYS_STATUS_ERROR			- if an invalid handle has been passed in
      - SYS_STATUS_UNINITIALIZED	- if the driver has not completed initialization
      - SYS_STATUS_BUSY				- if the driver is closing and moving to the closed state
      - SYS_STATUS_READY			- if the driver is ready for client commands
*/
SYS_STATUS DRV_ENCX24J600_Status(SYS_MODULE_OBJ obect);

// *****************************************************************************
/* ENCX24J600 Tasks

    Summary:
      Main task function for the driver.
	  <p><b>Implementation:</b> Dynamic</p>
    
    Description:
      This function will execute the main state machine for the ENCX24J600 driver.

    Preconditions:
      The driver had to be successfully initialized with DRV_ENCX24J600_Initialize.

    Parameters:
      object	- The object valid passed back to DRV_ENCX24J600_Initialize

    Returns:
      None.
*/
void DRV_ENCX24J600_Tasks(SYS_MODULE_OBJ object);

// *****************************************************************************
/* ENCX24J600 Set MAC Control Information

    Summary:
      This function sets the MAC control information for the driver.
	  <p><b>Implementation:</b> Dynamic</p>
    
    Description:
      This function is used to pass in the TCPIP_MAC_CONTROL_INIT information that 
      is used for allocation and deallocation of memory, event signaling, etc.  
      This function is needed to be called so that the driver can enter 
      initialization state when the tasks function is called.

    Preconditions:
      The driver had to be successfully initialized with DRV_ENCX24J600_Initialize.

    Parameters:
      - object	- The object valid passed back to DRV_ENCX24J600_Initialize
      - init	- The structure containing the MAC control information
 
    Returns:
      None.
*/
void DRV_ENCX24J600_SetMacCtrlInfo(SYS_MODULE_OBJ object, TCPIP_MAC_MODULE_CTRL * init);

// *****************************************************************************
/* ENCX24J600 Stack Initialization

    Summary:
      This function initializes the driver with a TCPIP_MAC_INIT object.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      This function is used by the TCP/IP stack to fully initialize the driver with 
      both the ENCX24J600 specific configuration and the MAC control information.  
      With this function there is no need to call DRV_ENCX24J600_SetMacCtrlInfo.

    Preconditions:
      None.
    
    Parameters:
      index	- This is the index of the driver instance to be initialized. The definition 
			  DRV_ENCX24J600_NUM_DRV_INSTANCES controls how many instances are available.
      init	- This is a pointer to a TCPIP_MAC_INIT structure.

    Returns:
      Returns a valid handle to the driver instance	- If successful
      SYS_MODULE_OBJ_INVALID						- If unsuccessful
*/
SYS_MODULE_OBJ DRV_ENCX24J600_StackInitialize(SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* ENCX24J600 Open
    Summary:
      This function is called by the client to open a handle to a driver instance.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      The client will call this function to open a handle to the driver.  When the 
      first instance is opened than the driver will send the RX enabled command to 
      the ENC hardware.

    Preconditions:
      The driver had to be successfully initialized with DRV_ENCX24J600_Initialize.

    Parameters:
      index	 - This is the index of the driver instance to be initialized.  The
               definition DRV_ENCX24J600_NUM_DRV_INSTANCES controls how many 
               instances are available.
      intent - The intent to use when opening the driver.  Only exclusive is supported

    Returns:
      Returns a valid handle - If successful
      INVALID_HANDLE		 - If unsuccessful
*/
DRV_HANDLE DRV_ENCX24J600_Open(SYS_MODULE_INDEX index, DRV_IO_INTENT intent);


// *****************************************************************************
/* ENCX24J600 Close

    Summary:
      Closes a client handle to the driver.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      This function closes a handle to the driver.  If it is the last client open, 
      the driver will send an RX Disable command to the ENC hardware and move to 
	  the closed state.

    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      handle - The successfully opened handle

    Returns:
      None.
*/
void DRV_ENCX24J600_Close(DRV_HANDLE handle);

// *****************************************************************************
/* ENCX24J600 Link Check

    Summary:
      This function returns the status of the link.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      This function checks the status of the link and returns it to the caller.

    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      hMac: the successfully opened handle

    Returns:
      - true	- if the link is active
      - false	- all other times
*/
bool DRV_ENCX24J600_LinkCheck(DRV_HANDLE hMac);

// *****************************************************************************
/* ENCX24J600 Receive Filter Hash Table Entry Set

    Summary:
      This function adds an entry to the hash table.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      This function adds to the MAC's hash table for hash table matching.  
      Note: This functionality is not implemented in the first release.

    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      hMac			- the successfully opened handle
      DestMACAddr	- MAC address to add to the hash table

    Returns:
      - TCPIP_MAC_RES_TYPE_ERR	- if the hMac is invalid
      - TCPIP_MAC_RES_OP_ERR	- if the hMac is valid
 */
TCPIP_MAC_RES DRV_ENCX24J600_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr);

// *****************************************************************************
/* ENCX24J600 Power Mode

    Summary:
      This function sets the power mode of the device.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      This function sets the power mode of the ENCX24J600.
	  Note: This functionality  is not implemented in the first release.

    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      hMac		- the successfully opened handle
      pwrMode	- the power mode to set

    Returns:
      - false	- This functionality is not supported in this version of the driver
*/
bool  DRV_ENCX24J600_PowerMode(DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode);

// *****************************************************************************
/* ENCX24J600 Packet Transmit

    Summary:
      This function queues a packet for transmission.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      This function will take a packet and add it to the queue for transmission.  
      When the packet has finished transmitting the driver will call the packets 
      acknowledge function.  When that acknowledge function is complete the driver 
      will forget about the packet.
    
    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters
      hMac		- the successfully opened handle
      ptrPacket	- pointer to the packet

    Returns:
      - TCPIP_MAC_RES_OP_ERR		- if the client handle is invalid
      - TCPIP_MAC_RES_IS_BUSY		- if the driver is not in the run state
      - TCPIP_MAC_RES_QUEUE_TX_FULL	- if there are no free descriptors
      - TCPIP_MAC_RES_OK			- on successful queuing of the packet
*/
TCPIP_MAC_RES DRV_ENCX24J600_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);

// *****************************************************************************
/* ENCX24J600 Receive Packet

    Summary:
      Receive a packet from the driver.
	  <p><b>Implementation:</b> Dynamic</p>
    
    Description:
      This function retrieves a packet from the driver.  The packet needs to be 
      acknowledged with the linked acknowledge function so it can be reused. 
      Note: pPktStat is ignored in the first release.

    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      hMac			- the successfully opened handle
      pRes			- the result of the operation
      pPktStat		- address to the receive statistics

    Returns:
      - Pointer to a valid packet	- if successful
      - NULL						- if unsuccessful
*/
TCPIP_MAC_PACKET* DRV_ENCX24J600_PacketRx(DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat);

// *****************************************************************************
/* ENCX24J600 Process

    Summary:
      Additional processing that happens outside the tasks function.
	  <p><b>Implementation:</b> Dynamic</p>
    
    Description:
      This function does additional processing that is not done inside the 
	  tasks function.  
	  Note: This function does nothing in the first release.
    
    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      hMac	- the successfully opened handle

    Returns:
      - TCPIP_MAC_RES_TYPE_ERR	- if the hMac is invalid
      - TCPIP_MAC_RES_OP_ERR	- if the hMac is valid
*/
TCPIP_MAC_RES DRV_ENCX24J600_Process(DRV_HANDLE hMac);

// *****************************************************************************
/* ENCX24J600 Get Statistics

    Summary:
    Retrieve the devices statistics.
	<p><b>Implementation:</b> Dynamic</p>

    Description:
    Get the current statistics stored in the driver.  
	Note: Statistics are not planned for the first release.

    Preconditions:
    The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      hMac	- the successfully opened handle

    Returns:
      - TCPIP_MAC_RES_TYPE_ERR	- if the hMac is invalid
      - TCPIP_MAC_RES_OP_ERR	- if the hMac is valid
*/
TCPIP_MAC_RES DRV_ENCX24J600_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);

// *****************************************************************************
/* ENCX24J600 Get Parameters

    Summary:
      Get the parameters of the device.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      Get the parameters of the device, which includes that it is an Ethernet device 
      and what it's MAC address is.
    
    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      hMac			- the successfully opened handle
      pMacParams	- pointer to put the parameters

    Returns:
      - TCPIP_MAC_RES_TYPE_ERR	- if the hMac is invalid
      - TCPIP_MAC_RES_OK		- if the hMac is valid
*/
TCPIP_MAC_RES DRV_ENCX24J600_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);

// *****************************************************************************
/* ENCX24J600 Get Register Statistics

    Summary:
      Get the register statistics.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      Get the device specific statistics.
	  Note: Statistics are not planned for the first release

    Preconditions:
    The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      hMac			- the successfully opened handle
      pRegEntries	- 
      nEntries		- 
      pHwEntries	- 

    Returns:
      - TCPIP_MAC_RES_TYPE_ERR 	- if the hMac is invalid
      - TCPIP_MAC_RES_OP_ERR	- if the hMac is valid
*/
TCPIP_MAC_RES DRV_ENCX24J600_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);

// *****************************************************************************
/* ENCX24J600 Get Configuration

    Summary:
      Gets the current configuration.
	  <p><b>Implementation:</b> Dynamic</p>
    
    Description:
      Gets the current configuration.  
	  Note: This function does nothing in the first release.

    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.
    
    Parameters:
      hMac			- the successfully opened handle
      configBuff	- location to copy the configuration too
      buffSize		- buffer size
      pConfigSize	- configuration size needed

    Returns:
        Number of bytes copied to the buffer
*/
size_t DRV_ENCX24J600_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);

// *****************************************************************************
/* ENCX24J600 Set Event Mask

    Summary:
      Sets the event mask.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      Sets the event mask to what is passed in.

    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      hMac		- the successfully opened handle
      macEvents	- the mask to enable or disable
      enable	- to enable or disable events

    Returns
      - true 	- if the mask could be set
      - false	- if the mast could not be set
*/
bool DRV_ENCX24J600_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);

// *****************************************************************************
/* ENCX24J600 Acknowledge Event

    Summary:
      Acknowledges an event.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      This function acknowledges an event.
    
    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      hMac 		- the successfully opened handle
      macEvents - the events to acknowledge

    Returns:
      - true	- if successful
      - false	- if not successful
*/
bool DRV_ENCX24J600_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents);

// *****************************************************************************
/* ENCX24J600 Get Events

    Summary:
      Gets the current events.
	  <p><b>Implementation:</b> Dynamic</p>

    Description:
      This function gets the current events.

    Preconditions:
      The client had to be successfully opened with DRV_ENCX24J600_Open.

    Parameters:
      hMac - the successfully opened handle

    Returns:
	  - TCPIP_MAC_EV_NONE	- Returned on an error
	  - List of events		- Returned on event other than an error

*/
TCPIP_MAC_EVENT DRV_ENCX24J600_EventPendingGet(DRV_HANDLE hMac);

#ifdef __cplusplus
}
#endif

#endif /*!defined(_DRV_ENCX24J600_H_)*/
 
