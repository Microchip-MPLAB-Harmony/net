/*******************************************************************************
  HyperText Transfer Protocol (HTTP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    http_config.h

  Summary:
    HTTP configuration file

  Description:
    This file contains the HTTP module configuration options

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2011 released Microchip Technology Inc.  All rights reserved.

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
#ifndef _HTTP_CONFIG_H_
#define _HTTP_CONFIG_H_


// Set to length of longest string above
#define TCPIP_HTTP_MAX_HEADER_LEN     (15u)

// Max lifetime (sec) of static responses as string
#define TCPIP_HTTP_CACHE_LEN          ("600")

// Max time (sec) to await more data before timing out and disconnecting the socket
#define TCPIP_HTTP_TIMEOUT            (45u)

// Maximum numbers of simultaneous supported HTTP connections.
#define TCPIP_HTTP_MAX_CONNECTIONS        (4)

// Indicate what HTTP file to serve when no specific one is requested
#define TCPIP_HTTP_DEFAULT_FILE       "index.htm"

// Indicate what HTTPS file to serve when no specific one is requested
#define TCPIP_HTTPS_DEFAULT_FILE      "index.htm"

// For buffer overrun protection.
// Set to longest length of above two strings.
#define TCPIP_HTTP_DEFAULT_LEN        (10u)

// Configure MPFS over HTTP updating
// Comment this line to disable updating via HTTP
#define TCPIP_HTTP_FILE_UPLOAD_ENABLE
#define TCPIP_HTTP_FILE_UPLOAD_NAME "mpfsupload"

// Require password for MPFS uploads
// Certain firewall and router combinations cause the MPFS2 Utility to fail
// when uploading.  If this happens, comment out this definition.
//#define TCPIP_HTTP_FILE_UPLOAD_REQUIRES_AUTH

// Define which HTTP modules to use
// If not using a specific module, comment it to save resources
// Enable POST support
#define TCPIP_HTTP_USE_POST

// Enable cookie support
#define TCPIP_HTTP_USE_COOKIES

// Enable basic authentication support
#define TCPIP_HTTP_USE_AUTHENTICATION

// Uncomment to require secure connection before requesting a password
#define TCPIP_HTTP_NO_AUTH_WITHOUT_SSL

// Define the maximum data length for reading cookie and GET/POST arguments (bytes)
#define TCPIP_HTTP_MAX_DATA_LEN       (100u)

// Define the minimum number of bytes free in the TX FIFO before executing callbacks
#define TCPIP_HTTP_MIN_CALLBACK_FREE  (16u)

// Authentication requires Base64 decoding
// Enable basic authentication support
#define TCPIP_STACK_USE_BASE64_DECODE


// Define the size of the TX buffer for the HTTP socket
// Use 0 for default TCP socket value
// The default recommended value for high throughput is > 2MSS (3 KB).
// The performance of a socket is highly dependent on the size of its buffers
// so it's a good idea to use as large as possible buffers for the sockets that need
// high throughput. 
#define     TCPIP_HTTP_SKT_TX_BUFF_SIZE   0

// Define the size of the RX buffer for the HTTP socket
// Use 0 for default TCP socket value
// The default recommended value for high throughput is > 2MSS (3 KB).
// The performance of a socket is highly dependent on the size of its buffers
// so it's a good idea to use as large as possible buffers for the sockets that need
// high throughput. 
#define     TCPIP_HTTP_SKT_RX_BUFF_SIZE   0

// Define the HTTP module configuration flags
// Use 0 for default
// See HTTP_MODULE_FLAGS definition for possible values
#define     TCPIP_HTTP_CONFIG_FLAGS      0

// The HTTP task rate, ms
// The default value is 33 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define     TCPIP_HTTP_TASK_RATE   33


// HTTP allocation function, malloc style
// This is the function the HTTP will call to allocate memory
// needed for file uploads.
// Use standard C library 'malloc' or 0 as a default
// If it's 0, HTTP will use the allocation functions passed in at the stack initialization
#define TCPIP_HTTP_MALLOC_FUNC         0


// HTTP corresponding deallocation function, free style
// This is the function the HTTP will call for freeing the allocated memory
// Use standard C library 'free' or 0 as a default
// If it's 0, HTTP will use the allocation functions passed in at the stack initialization
#define TCPIP_HTTP_FREE_FUNC            0

#endif  // _HTTP_CONFIG_H_

