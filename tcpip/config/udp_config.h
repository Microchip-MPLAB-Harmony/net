/*******************************************************************************
  User Datagram Protocol (UDP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    udp_config.h

  Summary:
    UDP Configuration file
    
  Description:
    This file contains the UDP module configuration options
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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
#ifndef _UDP_CONFIG_H_
#define _UDP_CONFIG_H_

// Maximum number of UDP sockets that can be opened simultaneously
// These sockets will be created when the module is initialized.
#define TCPIP_UDP_MAX_SOCKETS			(10)

// Default socket TX buffer size.
// Note that this setting affects all UDP sockets that are created
// and, together with TCPIP_UDP_MAX_SOCKETS, has a great impact on
// the heap size that's used by the stack (see TCPIP_STACK_DRAM_SIZE setting).
// When large TX bufferrs are needed, probably a dynamic, per socket approach,
// is a better choice (see TCPIP_UDP_OptionsSet function).
#define TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE	512

// Calculate and transmit a checksum when sending data.
// Checksum is not mandatory for UDP packets but is highly recommended.
// This will affect the UDP TX performance.
#define TCPIP_UDP_USE_TX_CHECKSUM

// Check incoming packets to have proper checksum.
#define TCPIP_UDP_USE_RX_CHECKSUM

// The maximum number of TX packets that can be queued
// by an UDP socket at a certain time.
// For sockets that need to transfer a lot of data (Iperf, for example),
// especially on slow connections this limit prevents running out of memory
// because the MAC/PHY transfer cannot keep up with the UDP packet allocation rate
// imposed by the application.
// Adjust depending on the TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE, the connection speed
// and the amount of memory available to the stack.
#define TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT   3

// The maximum number of RX packets that can be queued
// by an UDP socket at a certain time.
// Note that UDP sockets do not use their own RX buffers
// but instead use the network driver supplied packets and
// a timely processing is critical to avoid packet memory starvation
// for the whole stack.
// This symbol sets the maximum number of UDP buffers/packets that can
// be queued for a UDP socket at a certain time.
// Once this limit is reached further incoming packets are silently discarded.
// Adjust depending on the number of RX buffers that are available for the
// stack and the amount of memory available to the stack.
#define TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT   3


// enable the build of the pre-allocated pool buffers option
#define TCPIP_UDP_USE_POOL_BUFFERS      false

// Number of buffers in the private UDP pool.
// These are preallocated buffers that are to be used
// by UDP sockets only.
// This improves the UDP socket throughput and is meant only for
// UDP sockets that have to sustain high TX traffic rate.
// However, this memory is not returned to the stack heap,
// it always belongs to UDP.
// A socket needs to have an option set in order to use the buffers pool
// (see the UDPSetOptions()).
// Use 0 to disable the feature.
#define TCPIP_UDP_SOCKET_POOL_BUFFERS         4

// Size of the buffers in the UDP pool.
// Any UDP socket that is enabled to use the pool
// and has the TX size <= than this size can use 
// a buffer from the pool.
// Note that this setting, together with TCPIP_UDP_SOCKET_POOL_BUFFERS,
// has impact on the heap size that's used by the stack (see TCPIP_STACK_DRAM_SIZE setting).
#define TCPIP_UDP_SOCKET_POOL_BUFFER_SIZE     512


// TCPIP UDP info commands
// Enables/Disables the UDP command feature.
// This setting will add a UDP "info" command to the TCP/IP command menu
// allowing a run time inspection of the UDP sockets
// Set the value to 1 to enable, 0 to disable the feature.
// Note: to have the UDP commands available the TCP/IP commands need to be enabled.
#define TCPIP_UDP_COMMANDS       0


#endif  // _UDP_CONFIG_H_
