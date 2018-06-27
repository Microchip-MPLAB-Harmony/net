/*******************************************************************************
  HyperText Transfer Protocol (HTTP) Net Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    http_net_config.h

  Summary:
    HTTP Net configuration file

  Description:
    This file contains the HTTP Net module configuration options

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2015 released Microchip Technology Inc.  All rights reserved.

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
#ifndef _HTTP_NET_CONFIG_H_
#define _HTTP_NET_CONFIG_H_


// The length of longest header string that can be parsed
#define TCPIP_HTTP_NET_MAX_HEADER_LEN     (15u)

// Max lifetime (sec) of static responses as string
#define TCPIP_HTTP_NET_CACHE_LEN          ("600")

// Max time (sec) to await for more HTTP client data in an active connection state
// before timing out and disconnecting the socket
#define TCPIP_HTTP_NET_TIMEOUT            (45u)

// Maximum numbers of simultaneous supported HTTP connections.
#define TCPIP_HTTP_NET_MAX_CONNECTIONS        (4)

// Indicate what HTTP file to serve when no specific one is requested
#define TCPIP_HTTP_NET_DEFAULT_FILE       "index.htm"

// maximum size of a HTTP file name
// with the path removed from the file name
// one extra char added for the string terminator
#define TCPIP_HTTP_NET_FILENAME_MAX_LEN  25 

// Configure MPFS over HTTP updating
// Comment this line to disable updating via HTTP
#define TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE
#define TCPIP_HTTP_NET_FILE_UPLOAD_NAME "mpfsupload"

// Define which HTTP modules to use
// If not using a specific module, comment it to save resources
// Enable POST support
#define TCPIP_HTTP_NET_USE_POST

// Enable cookie support
#define TCPIP_HTTP_NET_USE_COOKIES

// Enable basic authentication support
#define TCPIP_HTTP_NET_USE_AUTHENTICATION

// Define the maximum data length for reading cookie and GET/POST arguments (bytes)
#define TCPIP_HTTP_NET_MAX_DATA_LEN       (100u)


// Define the size of the TX buffer for the HTTP socket
// Use 0 for default TCP socket value
// The default recommended value for high throughput is > 2MSS (3 KB).
// The performance of a socket is highly dependent on the size of its buffers
// so it's a good idea to use as large as possible buffers for the sockets that need
// high throughput. 
#define     TCPIP_HTTP_NET_SKT_TX_BUFF_SIZE   2048

// Define the size of the RX buffer for the HTTP socket
// Use 0 for default TCP socket value
// The default recommended value for high throughput is > 2MSS (3 KB).
// The performance of a socket is highly dependent on the size of its buffers
// so it's a good idea to use as large as possible buffers for the sockets that need
// high throughput. 
#define     TCPIP_HTTP_NET_SKT_RX_BUFF_SIZE   2048

// Define the HTTP module configuration flags
// Use 0 for default
// See HTTP_MODULE_FLAGS definition for possible values
#define     TCPIP_HTTP_NET_CONFIG_FLAGS      0

// The HTTP task rate, ms
// The default value is 33 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define     TCPIP_HTTP_NET_TASK_RATE   33


// Size of the peek buffer to perform searches into. 
// If the underlying transport layer supports offset peak operation with a offset,
// the value could be smaller (80 characters, for example);
// otherwise, a one time peek is required and the buffer should be larger -  
// recommended to be close to the size of the socket RX buffer.
// Note - if upload is enabled (TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE) then it is recommended
// for the peek buffer size to match the size of the socket RX buffer: TCPIP_HTTP_NET_SKT_RX_BUFF_SIZE  
// Note that this is an automatic buffer (created on the stack) and enough stack space
// should be provided for the application. 
// If left to 0 it will use the size of the socket RX buffer - this should be the default
#define TCPIP_HTTP_NET_FIND_PEEK_BUFF_SIZE         0 

// Size of the buffer used for processing HTML, dynamic variable and binary files.
// For dynamic variable files it should be able to accommodate the longest HTML line size,
// including CRLF!
#define TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE        512

// Number of file buffers to be created;
// These buffers are used to store data while file processing is done
// They are organized in a pool
// Each file being processed needs a file buffer and tries to get it from the pool
// If a buffer is not available, the HTTP conenction will wait for one to become available.
// Once the file is done the file buffer is released and could be used by a different file
// The number depends on the number of files that are processed in parallel
// To avoid deadlock the number should be >= than the number of maximum files that can be open simultaneously:
// i.e. for file1 ->include file2 -> include file3 you'll need >= 3 file process buffers
// 
#define TCPIP_HTTP_NET_FILE_PROCESS_BUFFERS_NUMBER     4

// Retry limit for allocating a file buffer from the pool.
// If more retries are not successful the operation will be aborted.
#define TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_RETRIES    10

// Size of the buffer used for sending the response messages to the client.
// Should be able to accommodate the longest server response:
// Default setting should be 300 bytes
#define TCPIP_HTTP_NET_RESPONSE_BUFFER_SIZE        300



// Size of the buffer used for sending the cookies to the client.
// Should be able to accommodate the longest cookie response.
// Otherwise the cookies will be truncated.
#define TCPIP_HTTP_NET_COOKIE_BUFFER_SIZE           200


// how many buffers descriptors for dynamic variable processing
// they are independent of the HTTP connection number
// all the HTTP connections use from the dynamic descriptors pool
#define TCPIP_HTTP_NET_DYNVAR_DESCRIPTORS_NUMBER    10

// The maximum depth of recursive calls for serving a web page:
//      - no dynvars files: 1
//      - file including a file: 2
//      - if the include file includes another file: +1
//      - if a dyn variable: +1
// Default value is 3;
#define TCPIP_HTTP_NET_MAX_RECURSE_LEVEL        3

// number of chunks that are created
// It depends on the TCPIP_HTTP_NET_MAX_RECURSE_LEVEL and
// on the number of connections
// Maximum number should be TCPIP_HTTP_NET_MAX_CONNECTIONS * TCPIP_HTTP_NET_MAX_RECURSE_LEVEL
// i.e. TCPIP_HTTP_NET_MODULE_CONFIG::nConnections * TCPIP_HTTP_NET_MODULE_CONFIG::nChunks
// All the chunks are in a pool and are used by all connections
#define TCPIP_HTTP_NET_CHUNKS_NUMBER            10

// retry limit for allocating a chunk from the pool
// If more retries are not successful the operation will be aborted
#define TCPIP_HTTP_NET_CHUNK_RETRIES    10

// This symbol enables the processing of dynamic variables
// Make it evaluate to false (0) if dynamic variables are not needed
// All the following symbols referring to dynamic variables are relevant
// only when TCPIP_HTTP_NET_DYNVAR_PROCESS != 0
#define TCPIP_HTTP_NET_DYNVAR_PROCESS   1

// maximum size for a complete dynamic variable: name + args
// must be <= TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE!
// If it is much larger than needed then inefficiency occurs when
// reading data from the file and then discarding it because
// a much larger than needed data buffer was read
#define TCPIP_HTTP_NET_DYNVAR_MAX_LEN           50

// maximum number of arguments for a dynamic variable
#define TCPIP_HTTP_NET_DYNVAR_ARG_MAX_NUMBER    10


// retry limit for a dynamic variable processing
// ths puts a limit on the number of times a dynamic variable "dynamicPrint" function
// can return TCPIP_HTTP_DYN_PRINT_RES_AGAIN/TCPIP_HTTP_DYN_PRINT_RES_PROCESS_AGAIN
// and avoids having the HTTP code locked up forever.
// If more retries are attempted the processing will be considered done 
// and dynamicPrint function will not be called again
#define TCPIP_HTTP_NET_DYNVAR_PROCESS_RETRIES       10

// This symbol enables the processing of SSI commands
// Make it evaluate to false (0) if SSI commands are not needed
// All the following symbols referring to SSI commands are relevant
// only when TCPIP_HTTP_NET_SSI_PROCESS != 0
#define TCPIP_HTTP_NET_SSI_PROCESS   1

// maximum number of attributes for a SSI command
// most SSI commands take just one attribute/value pair per line
// but multiple attribute/value pairs on the same line are allowed
// where it makes sense
#define TCPIP_HTTP_NET_SSI_ATTRIBUTES_MAX_NUMBER    4

// number of static attributes associated to a SSI command
// if the command has more attributes than this number
// the excess will be allocated dynamically 
#define TCPIP_HTTP_NET_SSI_STATIC_ATTTRIB_NUMBER        2


// maximum size for a SSI command line: command + attribute/value pairs
// must be <= TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE!
// If it is much larger than needed then inefficiency occurs when
// reading data from the file and then discarding it because
// a much larger than needed data buffer was read
#define TCPIP_HTTP_NET_SSI_CMD_MAX_LEN           100


// maximum number of SSI variables that can be created at run time
// These variables are stored in an internal hash.
// For max. efficiency this number should be a prime.
#define TCPIP_HTTP_NET_SSI_VARIABLES_NUMBER     13

// maximum length of a SSI variable name
// any excess characters will be truncated
// Note that this can result in multiple variables being represented
// as one SSI variable 
#define TCPIP_HTTP_NET_SSI_VARIABLE_NAME_MAX_LENGTH        10

// maximum size of a SSI string variable value
// any excess characters will be truncated
// Note that the variable value requires SSI storage that's allocated dynamically
// Also, this value determines the size of an automatic (stack) buffer
// when the variable is echoed.
// If this value is large, make sure you have enough stack space.
#define TCPIP_HTTP_NET_SSI_VARIABLE_STRING_MAX_LENGTH      10

// message to echo when echoing a not found variable 
#define TCPIP_HTTP_NET_SSI_ECHO_NOT_FOUND_MESSAGE   "SSI Echo - Not Found: "



// Persistent connection idle timeout, in seconds
// If a persistent connection is idle for more that this timeout
// the server will close it.
// Usually the client closes the connection.
// Using this timeout value the server can close the its side of the connection
// to avoid all connections to remain indefinitely open if the client misbehaves.
// Use 0 to never timeout.
// The timeout value has to be <= 32767 seconds.
#define TCPIP_HTTP_NET_CONNECTION_TIMEOUT   60

// HTTP allocation function, malloc style
// This is the function the HTTP will call to allocate memory
// needed for: dynamic variables, SSI or file uploads.
// Use standard C library 'malloc' or 0 as a default
// If it's 0, HTTP will use the allocation functions passed in at the stack initialization
#define TCPIP_HTTP_NET_MALLOC_FUNC         0


// HTTP corresponding deallocation function, free style
// This is the function the HTTP will call for freeing the allocated memory
// Use standard C library 'free' or 0 as a default
// If it's 0, HTTP will use the allocation functions passed in at the stack initialization
#define TCPIP_HTTP_NET_FREE_FUNC            0

#endif  // _HTTP_NET_CONFIG_H_

