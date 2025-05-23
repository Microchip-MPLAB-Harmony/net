/*******************************************************************************
  Transmission Control Protocol (TCP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcp_config.h

  Summary:
    TCP configuration file
    
  Description:
    This file contains the TCP module configuration options

    This file is not part of the project, it is a configuration template file only. 
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2011-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#ifndef _TCP_CONFIG_H_
#define _TCP_CONFIG_H_


// TCP Maximum Segment Size for TX.  The TX maximum segment size is actually 
// governed by the remote node's MSS option advertised during connection 
// establishment.  However, if the remote node specifies an unmanageably large 
// MSS (ex: > Ethernet MTU), this define sets a hard limit so that TX buffers 
// are not overflowed.  If the remote node does not advertise a MSS 
// option, all TX segments are fixed at 536 bytes maximum.
// This symbol sets an upper bound on the TCP MSS on the transmit side
// which can ignore/override the remote node advertising.
#define TCPIP_TCP_MAX_SEG_SIZE_TX           (1460u)

// Default socket TX buffer size
// Note that this setting affects all TCP sockets that are created
// and, together with TCPIP_TCP_MAX_SOCKETS, has a great impact on
// the heap size that's used by the stack (see TCPIP_STACK_DRAM_SIZE setting).
// When large TX bufferrs are needed, probably a dynamic, per socket approach,
// is a better choice (see TCPIP_TCP_OptionsSet function).
// The performance of a socket is highly dependent on the size of its buffers
// so it's a good idea to use as large as possible buffers for the sockets that need
// high throughput. 
// Note that some modules (like HTTP) use their own settings to specify the buffer size
// for their TCP sockets.
#define TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE        512         

// Default socket RX buffer size
// Note that this setting affects all TCP sockets that are created
// and, together with TCPIP_TCP_MAX_SOCKETS, has a great impact on
// the heap size that's used by the stack (see TCPIP_STACK_DRAM_SIZE setting).
// When large RX bufferrs are needed, probably a dynamic, per socket approach,
// is a better choice (see TCPIP_TCP_OptionsSet function).
// The performance of a socket is highly dependent on the size of its buffers
// so it's a good idea to use as large as possible buffers for the sockets that need
// high throughput. 
// Note that some modules (like HTTP) use their own settings to specify the buffer size
// for their TCP sockets.
#define TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE        512         
        
// Enable the TCP sockets dynamic options set/get functionality
// If enabled, the functions: TCPIP_TCP_OptionsSet, TCPIP_TCP_OptionsGet and TCPIP_TCP_FifoSizeAdjust
// exist and are compiled in
// If disabled, these functions do not exist and cannot be used/called 
// Note that this setting can affect modules that use TCP sockets
#define TCPIP_TCP_DYNAMIC_OPTIONS               1

// Timeout to syn retransmit, data retry - ms
#define TCPIP_TCP_START_TIMEOUT_VAL     (1000ul)

// Timeout for delayed-acknowledgment algorithm, ms
#define TCPIP_TCP_DELAYED_ACK_TIMEOUT       (100ul)

// Timeout for FIN WAIT 2 state, ms
#define TCPIP_TCP_FIN_WAIT_2_TIMEOUT        (5000ul)

// Maximum Segment Length (MSL) timeout, seconds
// This value sets the time a socket will be in the TIME_WAIT/2MSL state
// after the socket performed an active close.
// RFC recommended value is 120 sec.
// Other common values are: 30/60 sec
// Note that if this symbol is defined to 0, then the TIME_WAIT/2MSL state is skipped!
// This could lead to duplicate sequence numbers problems
// if sockets are frequently/rapidly closed and reopened
// and normally should be avoided.
#define     TCPIP_TCP_MSL_TIMEOUT           (30)


// This value specifies the TCP quiet time:
// TCP will not send any data for TCPIP_TCP_QUIET_TIME seconds after rebooting!
// Usualy this value should match the MSL value.
// Note that if this symbol is defined to 0, then the TCP quiet time state is skipped!
// During the quiet time there will be no TCP traffic initiated by TCP.
// So, clients won't be able to connect to remote hosts
// and servers won't reply to client requests.
#define     TCPIP_TCP_QUIET_TIME           (30)

// Timeout for keep-alive messages when no traffic is sent, ms
#define TCPIP_TCP_KEEP_ALIVE_TIMEOUT        (10000ul)

// Timeout for the CLOSE_WAIT state, ms
// If the remote socket closes the connection
// the TCP/IP stack will automatically close the socket
// after this timeout.
// If the value is set to 0 then the socket will wait forever
// for the application to close the socket (default behavior). 
// Default should be 0
#define TCPIP_TCP_CLOSE_WAIT_TIMEOUT        (0)

// Maximum number of retransmission attempts
#define TCPIP_TCP_MAX_RETRIES           (5u)

// Maximum number of keep-alive messages that can be sent 
// without receiving a response before automatically closing 
// the connection
#define TCPIP_TCP_MAX_UNACKED_KEEP_ALIVES   (6u)

// Smaller than all other retries to reduce SYN flood DoS duration
#define TCPIP_TCP_MAX_SYN_RETRIES       (2u)

// Timeout before automatically transmitting unflushed data, ms.
// Default value is 40 ms.
#define TCPIP_TCP_AUTO_TRANSMIT_TIMEOUT_VAL (40ul)

// Timeout before automatically transmitting a window update
// due to a TCPIP_TCP_Get() or TCPIP_TCP_ArrayGet() function call, ms.
#define TCPIP_TCP_WINDOW_UPDATE_TIMEOUT_VAL (200ul)

//  The maximum number of sockets to create in the stack.
//  When defining TCPIP_TCP_MAX_SOCKETS take into account
//  the number of interfaces the stack is supporting.
#define TCPIP_TCP_MAX_SOCKETS           (10)



// The TCP task processing rate: number of milliseconds to generate an TCP tick.
// This is the tick that advances the TCP state machine.
// The default value is 5 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_TCP_TASK_TICK_RATE        (5)

// TCPIP TCP info commands
// Enables/Disables the TCP command feature.
// This setting will add a TCP "info" command to the TCP/IP command menu
// allowing a run time inspection of the TCP sockets
// Set the value to 1 to enable, 0 to disable the feature.
// Note: to have the TCP commands available the TCP/IP commands need to be enabled.
#define TCPIP_TCP_COMMANDS       0

// This setting enables/disables the processing of the RX packets
// by an external handler
// The user of the TCP can register an external function to 
// process the incoming packets
// If true, the functionality is built in and could be used by the application
// If false, the functionality does not exist and the generated code is slightly smaller 
#define TCPIP_TCP_EXTERN_PACKET_PROCESS true

// This setting enables/disables the usage of the Crypto RNG + Hash by the TCP module
// TCP uses Crypto RNG + MD5 hash to compute the socket ISN (Initial Sequence Number)
// Crypto RNG is also used to allocate a new ephemeral port
// If Crypto usage is disabled then TCP will use Pseudo Random calls instead.
// This will require much less resources but it will be insecure and exposed to attacks.
// See RFC 6528 and 6056
// Default should be false.
// Note: even with the Crypto usage disabled an application can make the ISN and port numbers
// harder to guess by seeding perriodically the pseudo random generator at run time
// with a good random value 
// Note: For applications that use use the crypto library anyway (TLS for example),
// the impact in the resources when using the Crypto RNG is minimal and should be preferred
#define TCPIP_TCP_DISABLE_CRYPTO_USAGE  false


// Retransmission timeout for unacknowledged packets, ms
// Default is 1.5 seconds
#define TCP_RETRANSMISSION_TMO      1500

#endif  // _TCP_CONFIG_H_
