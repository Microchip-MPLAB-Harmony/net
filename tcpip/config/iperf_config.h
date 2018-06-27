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
