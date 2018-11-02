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
