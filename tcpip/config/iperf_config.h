/*******************************************************************************
  Iperf Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    iperf_config.h

  Summary:
    Iperf configuration file
    
  Description:
    This file contains the Iperf configuration options

    This file is not part of the project, it is a configuration template file only. 
    
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END
#ifndef _IPERF_CONFIG_H_
#define _IPERF_CONFIG_H_

// Default size of the Iperf TX buffer
// The default value is 4KB.
// The performance of a socket is highly dependent on the size of its buffers
// so it's a good idea to use as large as possible buffers for the sockets that need
// high throughput. 
// Bigger buffers will help obtain higher performance numbers
#define TCPIP_IPERF_TX_BUFFER_SIZE            4096

// Default size of the Iperf RX buffer
// The default value is 4KB.
// The performance of a socket is highly dependent on the size of its buffers
// so it's a good idea to use as large as possible buffers for the sockets that need
// high throughput. 
// Bigger buffers will help obtain higher performance numbers
#define TCPIP_IPERF_RX_BUFFER_SIZE            4096 


// timeout to wait for TX channel to be ready to transmit a new packet; ms
// depends on the channel bandwidth
#define TCPIP_IPERF_TX_WAIT_TMO               100

// for Iperf UDP client, the limit to set to avoid memory allocation
// overflow on slow connections
#define TCPIP_IPERF_TX_QUEUE_LIMIT              2 


// iperf timing error, ms. Accounts for timing uncertainty
// The higher the value, the more relaxed is the iperf timing
// Lower limit is 0
#define TCPIP_IPERF_TIMING_ERROR_MARGIN     0

// maximum number of simultaneously running iperf instances
#define TCPIP_IPERF_MAX_INSTANCES                1

//Default TCP Tx Bandwidth
#define TCPIP_IPERF_TX_BW_LIMIT  			1		

#endif /* __IPERFAPP_H__ */
