/*******************************************************************************
  MAC Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_mac_config.h

  Summary:
    Configuration file
    
  Description:
    This file contains the MAC module configuration options
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END
#ifndef _MAC_CONFIG_H_
#define _MAC_CONFIG_H_

// =======================================================================
//   TCPIP_MODULE_MAC_PIC32INT - PIC32MX PIC32MZ MAC Layer Options
//   If not using a PIC32MX or PIC32MZ device with internal MAC, ignore this section.
// =======================================================================

// MAC Configuration parameters.
// Note: These values are used as defaults.
// The actual values passed in at initialization time
// take precedence.

// Number of the TX descriptors to be created.
// Because a TCP packet can span at most 3 buffers,
// the value should always be >= 4
// The amount of memory needed per descriptor is not high (around 24 bytes)
// so when high MAC TX performance is needed
// make sure that this number is >= 8.
#define TCPIP_EMAC_TX_DESCRIPTORS	8

// Number of the RX descriptors to be created.
// If not using the run time replenish mechanism (see below)
// it should match the number of dedicated buffers:
// TCPIP_EMAC_RX_DEDICATED_BUFFERS;
// Otherwise it should be bigger than the sum of dedicated + non-dedicated buffers:
// TCPIP_EMAC_RX_DESCRIPTORS > TCPIP_EMAC_RX_DEDICATED_BUFFERS + replenish_buffers
#define TCPIP_EMAC_RX_DESCRIPTORS	10

// Number of MAC dedicated RX packet buffers.
// These buffers are always owned by the MAC.
// Note that the MAC driver allocates these buffers for
// storing the incoming network packets.
// The bigger the storage capacity, the higher data throughput can be obtained.
// Note that these packet buffers are allocated from the private TCP/IP heap
// that is specified by the TCPIP_STACK_DRAM_SIZE setting.
#define    TCPIP_EMAC_RX_DEDICATED_BUFFERS  4

// Number of non-dedicated buffers for the MAC initialization
// Buffers allocated at the MAC driver initialization.
#define    TCPIP_EMAC_RX_INIT_BUFFERS       0

// Minumum threshold for the buffer replenish process.
// Whenever the number of RX scheduled buffers is <= than this threshold
// the MAC driver will allocate new non-dedicated buffers
// (meaning that they will be released to the TCP/IP heap once they are processed).
// Setting this value to 0 disables the buffer replenishing process.
#define    TCPIP_EMAC_RX_LOW_THRESHOLD      1

// Number of RX buffers to allocate when below threshold condition is detected.
// If 0, the MAC driver will allocate (scheduled buffers - rxThres)
// If !0, the MAC driver will allocate exactly TCPIP_EMAC_RX_LOW_FILL buffers
#define    TCPIP_EMAC_RX_LOW_FILL           2


// Maximum MAC supported RX/TX frame size.
// The default value is 1536 (allows for VLAN tagged frames,
// although the VLAN tagged frames are discarded on RX).
// On RX: any incoming ETH frame that's longer than this size
// will be discarded.
// On TX: any frame that's longer than this size will be aborted by the MAC.
//
// Normally there's no need to touch this value
// unless you know exactly the maximum size of the 
// frames you want to process on your network.
// Note: It can be used to control RX packets fragmentation
// (together with the TCPIP_EMAC_RX_BUFF_SIZE).
// Note: Always multiple of 16.
#define TCPIP_EMAC_MAX_FRAME   1536

// Link Maximum Transmission Unit (MTU)
// This symbol defines the largest network protocol data unit
// that can be transmitted over this link in a single frame.
// It relates to the TCPIP_EMAC_MAX_FRAME value - see above.
// The default value for an Ethernet link should be 1500.
// The minimum value for an Eternet link should be 576.
// If need to change this, make sure that the 
// TCPIP_EMAC_MAX_FRAME >= TCPIP_EMAC_LINK_MTU + 18
// where the value 18 represents the Ethernet frame header:
//      12 bytes    - destination + source address
//      2 bytes     - frame type
//      4 bytes     - FCS
// Note: setting this value to 0, the maximum possible MTU will be used
#define TCPIP_EMAC_LINK_MTU         1500


// Size of a RX packet buffer. Should be multiple of 16.
// This is the size of all receive packet buffers processed by the ETHC.
// The size should be enough to accommodate any network received packet.
// If the packets are larger, they will have to take multiple RX buffers
// and the packet manipulation is less efficient.
//#define	TCPIP_EMAC_RX_BUFF_SIZE	512
// Together with TCPIP_EMAC_RX_DEDICATED_BUFFERS it has impact on TCPIP_STACK_DRAM_SIZE setting.
#define	TCPIP_EMAC_RX_BUFF_SIZE	1536

// MAC maximum number of supported RX fragments.
// Based on the values of TCPIP_EMAC_MAX_FRAME and TCPIP_EMAC_RX_BUFF_SIZE
// an incoming frame may span multiple RX buffers (fragments).
// Note that excessive fragmentation leads to performance degradation.
// The default and recommended value should be 1.
//#define TCPIP_EMAC_RX_FRAGMENTS      1
// Alternatively you can use the calculation of the
// number of fragments based on the selected RX sizes:
#define TCPIP_EMAC_RX_FRAGMENTS      ((TCPIP_EMAC_MAX_FRAME + (TCPIP_EMAC_RX_BUFF_SIZE -1 )) / (TCPIP_EMAC_RX_BUFF_SIZE))


// MAC RX Filters
// These filters define the packets that are accepted and rejected by the MAC driver
// Adjust to your needs
// The default value allows the processing of unicast, multicast and broadcast packets that 
// have a valid CRC
#define TCPIP_EMAC_RX_FILTERS   (TCPIP_MAC_RX_FILTER_TYPE_DEFAULT)


// MAC Auto Flow Control

// This symbol enables/disables the auto flow control
// When in full-duplex mode and the auto flow control is enabled,
// the MAC will send pause frames whenever the number of pending RX packets
// reached the full watermark.
// Once this number drops to the empty watermark, a pause frame with pause value
// of 0 is transmitted, resuming the normal traffic
// Use 1 to enable, 0 to disable
// Default should be enabled
#define TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE    1

/* Pause Time Value for the Auto Flow Control, in bytes */
/* This represents the number of bytes to request the pause for */
/* when the auto flow control kicks in. */
/* It is normally thought of in terms of RX packets: */
/* pauseBytes / rxBuffSize = pausePackets */
/* Using bytes instead of packets allows better granularity */
/* The value should be a multiple of 64 bytes */
/* The range is : 64 bytes <= TCPIP_EMAC_FLOW_CONTROL_PAUSE_BYTES <=  (4 MB - 64) bytes (0x3fffc0) */
/* An usual value is 2 full packets. For example 2 * 1536 bytes */
/* Used only when flow control is enabled */
#define  TCPIP_EMAC_FLOW_CONTROL_PAUSE_BYTES        3072       

/* An 8 bit value representing the full water mark, in number of packets */
/* When the number of pending received packets reaches this threshold */
/* the auto flow control kicks in */
/* A good rule to use for avoiding any packet overflow is to have enough */
/* receive room for at least 2 maximum packets, i.e. */
/* fullWMMark = (rxScheduledBuffers x rxBuffSize) / TCPIP_EMAC_MAX_FRAME - (2 x TCPIP_EMAC_MAX_FRAME ) / rxBuffSize; */
/* Always TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK > TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK */ 
/* Used only when auto flow control is enabled */
/* Adjust to your own value */
// #define  TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK        2
// Default value should use the calculation of the
// watermark based on the selected RX sizes and buffers
#define  TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK         ((TCPIP_EMAC_RX_DEDICATED_BUFFERS * TCPIP_EMAC_RX_BUFF_SIZE) / TCPIP_EMAC_MAX_FRAME - (2 x TCPIP_EMAC_MAX_FRAME ) / TCPIP_EMAC_RX_BUFF_SIZE)


/* An 8 bit value representing the empty water mark, in number of packets */
/* When the auto flow control is active and the number of pending received packets */
/* falls reaching this threshold the auto flow control pause frames will stop */
/* Always TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK > TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK */ 
/* Used only when auto flow control is enabled */
/* Default value could be 0: resume normal traffic when there is no pending RX packet */
#define  TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK        0


// Flags to use for the Ethernet connection
// A TCPIP_ETH_OPEN_FLAGS value.
// Set to TCPIP_ETH_OPEN_DEFAULT unless very good reason
// to use different value
#define TCPIP_EMAC_ETH_OPEN_FLAGS     (TCPIP_ETH_OPEN_DEFAULT)

// Flags to configure the MAC ->PHY connection
// a DRV_ETHPHY_CONFIG_FLAGS
// This depends on the actual connection
// (MII/RMII, default/alternate I/O)
// The DRV_ETHPHY_CFG_AUTO value will use the configuration
// fuses setting
#define TCPIP_EMAC_PHY_CONFIG_FLAGS   (DRV_ETHPHY_CFG_AUTO)


// The value of the delay for the link initialization, ms
// This insures that the PHY is ready to transmit after it is reset
// A usual value is > 500 ms up to 2000 ms.
// The longer the delay, the less chance that the PHY drops packets when the link is established
// Adjust to your needs.
#define TCPIP_EMAC_PHY_LINK_INIT_DELAY        (1000)


// The PHY address, as configured on the board.
// By default all the PHYs respond to address 0
#define TCPIP_EMAC_PHY_ADDRESS                (0)



// =======================================================================
//   TCPIP_MODULE_MAC_MRF24W - MAC Layer Options
//   If not using an external MRF24W Wi-Fi MAC, ignore this section.
// =======================================================================



#endif  // _MAC_CONFIG_H_


