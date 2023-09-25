/*******************************************************************************
  Berkeley Socket Distribution API Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    berkeley_api.h

  Summary:
    The Berkeley Socket Distribution (BSD) APIs provide a BSD wrapper to the native 
    Microchip TCP/IP Stack APIs.
    
  Description:
    The Berkeley Socket Distribution (BSD) APIs provide a BSD wrapper to the native 
    Microchip TCP/IP Stack APIs. Using this interface, programmers familiar with
    BSD sockets can quickly develop applications using Microchip's TCP/IP Stack.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _BERKELEY_API_HEADER_FILE
#define _BERKELEY_API_HEADER_FILE

#include <limits.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

typedef int16_t SOCKET;   //Socket descriptor

#define AF_INET         2           // Internet Address Family - IPv4, UDP, TCP, etc.
#define AF_INET6        10          // Internet Address Family - IPv6
#define AF_UNSPEC       0           // Ipv4 or IPv6

#define IP_ADDR_ANY     0x01000000u         // IP Address for server binding
#define INADDR_ANY      0x01000000u // IP address for server binding.


#define SOCK_STREAM 100  //Connection based byte streams. Use TCP for the Internet address family.
#define SOCK_DGRAM  110  //Connectionless datagram socket. Use UDP for the Internet address family.
 
#define IPPROTO_IP      0   // Indicates IP pseudo-protocol.
#define SOL_SOCKET      1   // Indicates socket level options
#define IPPROTO_TCP     6   // Indicates TCP level options
#define IPPROTO_UDP     17  // Indicates UDP level options
#define IPPROTO_IPV6    41  // Indicates IP v6 Header options
#define IPPROTO_ICMPV6  58  // Indicates IPv6 ICMP Protocol Options

#define SOCKET_ERROR            (-1) //Socket error
#define SOCKET_CNXN_IN_PROGRESS (-2) //Socket connection state.
#define SOCKET_DISCONNECTED     (-3) //Socket disconnected


#define INVALID_TCP_PORT   (0L)  //Invalid TCP port

// Definitions for setsockopt for the IP layer (IPPROTO_IP)
#define IP_OPTIONS              4   //IP Header Options - Not yet supported
#define IP_TOS                  1   //Type of Service - Not yet supported
#define IP_TTL                  2   //Time to Live - Not yet supported
#define IP_MULTICAST_IF         32  //Set the Interface that multicast packets should be sent on  - Not yet supported
#define IP_MULTICAST_TTL        33  //Set the Time to Live option for multicast packets - Not yet supported
#define IP_MULTICAST_LOOP       34  //Specify a copy of multicast packets should be delivered to the sending host - Not yet supported 
#define IP_ADD_MEMBERSHIP       35  //Join a multicast group - Not yet supported
#define IP_DROP_MEMBERSHIP      36  // Leave a multicast group - Not yet supported

// Definitions for setsockopt for the TCP Layer  (IPPROTO_TCP)
#define TCP_NODELAY 1 //Indicates if TCP is to buffer packets - Not yet supported

// Definitions for setsockopt for the socket layer (SOL_SOCKET)
#define SO_BROADCAST            6   //Enables the Socket for sending broadcast data
#define SO_DEBUG                1   //Indicates if low level debut is active - Not yet supported
#define SO_DONTROUTE            5   //Bypass normal routing - Not yet supported
#define SO_KEEPALIVE            9   //Keep the connection alive by sending periodic transmissions - Not yet supported
#define SO_LINGER               13  //Indicates if the system should send any buffered data when a socket is closed
#define SO_OOBINLINE            10  //Indicates whether or not Out of Band Data should be received inline with normal data - Not yet supported
#define SO_RCVBUF               8   //Receive Buffer Size (TCP only)
#define SO_RCVLOWAT             18  //Receive Low Water mark - Not yet supported
#define SO_RCVTIMEO             20  //Set the Receive Timeout - Not yet supported
#define SO_REUSEADDR            2   //Indicates if the local socket can be reused immediately after close - Not yet supported
#define SO_SNDBUF               7   //Send Buffer Size
#define SO_SNDLOWAT             19  //Send Low Water mark - Not yet supported
#define SO_SNDTIMEO             21  //Set the Send Timeout - Not yet supported

//Definitions for setsockopt for the IPv6 Layer (IPPROTO_IPV6)
#define IPV6_UNICAST_HOPS       16  //Set the hop limit for unicast IPv6 packets - Not yet supported
#define IPV6_MULTICAST_IF       17  //Set the Interface that multicast IPv6 packets should be sent on  - Not yet supported
#define IPV6_MULTICAST_HOPS     18  //Set the hop limit for multicast IPv6 packets - Not yet supported
#define IPV6_MULTICAST_LOOP     19  //Specify a copy of multicast IPv6 packets should be delivered to the sending host - Not yet supported 
#define IPV6_JOIN_GROUP         20  //Join an IPv6 multicast group - Not yet supported
#define IPV6_LEAVE_GROUP        21  //Leave an IPv6 multicast group - Not yet supported
#define IPV6_V6ONLY             26  //Sets the socket to IPv6 only - Not yet supported
#define IPV6_CHECKSUM           7   //Sets the IPv6 Checksum options - Not yet supported

// Definitions for setsockopt for the IPv6 ICMP Layer (IPPROTO_ICMPV6)
#define ICMP6_FILTER            1    //SEt the IPv6 Filtering options - Not yet supported

#define HOST_NOT_FOUND          1    // Authoritative Answer host not found
#define TRY_AGAIN               2    //Non authoritative host not found or server fail
#define NO_RECOVERY             3    //Non recoverable errors,
#define NO_DATA                 4    // Valid name, no data record of requested type

# define EAI_BADFLAGS     -1    /* Invalid value for `ai_flags' field.  */
# define EAI_NONAME       -2    /* NAME or SERVICE is unknown.  */
# define EAI_AGAIN        -3    /* Temporary failure in name resolution.  */
# define EAI_FAIL         -4    /* Non-recoverable failure in name res.  */
# define EAI_FAMILY       -6    /* `ai_family' not supported.  */
# define EAI_SOCKTYPE     -7    /* `ai_socktype' not supported.  */
# define EAI_SERVICE      -8    /* SERVICE not supported for `ai_socktype'.  */
# define EAI_MEMORY       -10   /* Memory allocation failure.  */
# define EAI_SYSTEM       -11   /* System error returned in `errno'.  */
# define EAI_OVERFLOW     -12   /* Argument buffer overflow.  */



struct in_addr
{
    union
   {
       struct { uint8_t s_b1,s_b2,s_b3,s_b4; } S_un_b; // IP address in Byte
       struct { uint16_t s_w1,s_w2; } S_un_w; //IP address in Word
       uint32_t S_addr; //IP address
   }S_un; //union of IP address
    
#define s_addr  S_un.S_addr //can be used for most TCP & IP code
#define s_host  S_un.S_un_b.s_b2 //host on imp
#define s_net   S_un.S_un_b.s_b1 // network
#define s_imp   S_un.S_un_w.s_w2 // imp
#define s_impno S_un.S_un_b.s_b4 // imp number
#define s_lh    S_un.S_un_b.s_b3 // logical host
}; // in_addr structure

struct in6_addr
{
    union {
        uint8_t u6_addr8[16];  // IP address in Bytes
        uint16_t u6_addr16[8]; // IP address in 16 bit Words
        uint32_t u6_addr32[4]; // IP address in 32 bit Words
    }in6_u;
#define s6_addr in6_u.u6_addr8
#define s6_addr16 in6_u.u6_addr16
#define s6_addr32 S_un.u6_addr32
};

struct __attribute__((__packed__)) sockaddr
{
    unsigned short   sa_family;   //address family
    char    sa_data[14];       //up to 14 bytes of direct address
}; //generic address structure for all address families

struct __attribute__((__packed__)) sockaddr_in
{
    short   sin_family; //Address family; must be AF_INET.
    uint16_t    sin_port;  //Internet Protocol (IP) port.
    struct  in_addr sin_addr; //IP address in network byte order.
    char    sin_zero[8];  //Padding to make structure the same size as SOCKADDR. 
}; //In the Internet address family

typedef struct sockaddr_in SOCKADDR_IN; //In the Internet address family
typedef struct sockaddr SOCKADDR;  // generic address structure for all address families

struct __attribute__((__packed__)) sockaddr_in6
{
    short   sin6_family; //Address family; must be AF_INET.
    uint16_t    sin6_port;  //Internet Protocol (IP) port.
    uint32_t    sin6_flowinfo; // IPv6 flow information
    struct  in6_addr sin6_addr; //IPv6 address in network byte order.
    uint32_t    sin6_scope_id; // IPv6 Scope Id
}; //In the Internet address family

typedef struct sockaddr_in6 SOCKADDR_IN6;

/* Structure large enough to hold any socket address (with the historical exception of
AF_UNIX). 128 bytes reserved.  */

#if ULONG_MAX > 0xffffffff
# define __ss_aligntype uint64_t
#else
# define __ss_aligntype uint32_t
#endif
#define _SS_SIZE        128
#define _SS_PADSIZE     (_SS_SIZE - (2 * sizeof (__ss_aligntype)))

struct sockaddr_storage // Here for creating portable applications.  Not used in the stack
{
    short ss_family;      /* Address family */
    __ss_aligntype __ss_align;  /* Force desired alignment.  */
    char __ss_padding[_SS_PADSIZE];
};


struct __attribute__((__packed__)) linger
{
    int        l_onoff;  // Determines if the option is set
    int        l_linger; // Time to wait before data is discarded
};

extern int h_errno; // Error location for host lookup errors
struct __attribute__((__packed__)) hostent {
    char * h_name;  // Points to a string containing the name of the host
    char ** h_alias; // points to a null terminated list of pointers that point to the aliases of the host
    int h_addrtype; // Contains the address type for the host, currently only AF_INET is supported
    int h_length;  // Contains the length of the h_addr_list
    char **h_addr_list; // Points to a NULL terminated list of pointers that point to the address of the host
};

struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    size_t           ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};

/*
 * Berkeley API module configuration structure
 */

typedef struct
{
    uint8_t maxSockets;  // Maximum number of sockets supported
} BERKELEY_MODULE_CONFIG;

//*****************************************************************************
/* Function:
     SOCKET socket( int af, int type, int protocol )

   Summary:
     This function creates a new Berkeley socket.

   Description:
     This function creates a new BSD socket for the Microchip
     TCP/IP stack. The return socket descriptor is used for the subsequent
     BSD operations.

   Precondition:
     Berkeley API module should have been initialized.

   Parameters:
   af       - address family - AF_INET for IPv4, AF_INET6 for IPv6
   type     - socket type SOCK_DGRAM or SOCK_STREAM
   protocol - IP protocol IPPROTO_UDP or IPPROTO_TCP

   Returns:
     New socket descriptor. SOCKET_ERROR in case of error (and errno set accordingly).

   Remarks:
     None.
*/
SOCKET  socket( int af, int type, int protocol );

//*****************************************************************************
/* Function:
    int bind( SOCKET s, const struct sockaddr* name, int namelen )

   Summary:
    This function assigns a name to the socket descriptor.

   Description:
    The bind function assigns a name to an unnamed socket. The
    name represents the local address of the communication
    endpoint. For sockets of type SOCK_STREAM, the name of the
    remote endpoint is assigned when a connect or accept function
    is executed.

   Precondition:
    The socket function should be called.

   Parameters:
    s       - Socket descriptor returned from a previous call to socket
    name    - pointer to the sockaddr structure containing the local address of 
              the socket
    namelen - length of the sockaddr structure

   Returns:
    If bind is successful, a value of 0 is returned. A return
    value of SOCKET_ERROR indicates an error (and errno is set accordingly).

   Remarks:
    None.
 */
int     bind( SOCKET s, const struct sockaddr* name, int namelen );

//*****************************************************************************
/* Function:
    int listen( SOCKET s, int backlog )

   Summary:
    The listen function sets the specified socket in a listen mode.

   Description:
    This function sets the specified socket in a listen
    mode. Calling the listen function indicates that the
    application is ready to accept connection requests arriving
    at a socket of type SOCK_STREAM. The connection request is
    queued (if possible) until accepted with an accept function.
    The backlog parameter defines the maximum number of pending
    connections that may be queued.

   Precondition:
    bind() must have been called on the s socket first.

   Parameters:
    s       - Socket identifier returned from a prior socket() call.
    backlog - Maximum number of connection requests that can be queued.  Note
              that each backlog requires a TCP socket to be allocated.

   Returns:
    Returns 0 on success; otherwise returns SOCKET_ERROR (and errno is set accordingly).

  Remarks:
    None.
    
 */
int     listen( SOCKET s, int backlog );

//*****************************************************************************
/* Function:
    SOCKET accept(SOCKET s, struct sockaddr* addr, int* addrlen)

   Summary:
    This function accepts connection requests queued for a listening socket.

   Description:
    The accept function is used to accept connection requests
    queued for a listening socket. If a connection request is
    pending, accept removes the request from the queue, and a new
    socket is created for the connection. The original listening
    socket remains open and continues to queue new connection
    requests. The socket must be a SOCK_STREAM type socket.

   Precondition:
    The listen function should be called.

   Parameters:
    s       - Socket descriptor returned from a previous call to
              socket. Must be bound to a local name and in listening mode.
    addr    - Optional pointer to a buffer that receives the address
              of the connecting entity.
    addrlen - Optional pointer to an integer that contains the
              length of the address addr

  Returns:
    If the accept function succeeds, it returns a non-negative
    integer that is a descriptor for the accepted socket.
    Otherwise, the value SOCKET_ERROR is returned (and errno is set accordingly).

  Remarks:
    None.
 */
SOCKET  accept( SOCKET s, struct sockaddr* addr, int* addrlen );

//*****************************************************************************
/* Function:
    int connect( SOCKET s, struct sockaddr* name, int namelen )

   Summary:
    This function connects to the peer communications end point.

   Description:
    The connect function assigns the address of the peer
    communications endpoint. For stream sockets, connection is
    established between the endpoints. For datagram sockets, an
    address filter is established between the endpoints until
    changed with another connect() function.

   Precondition:
    The socket function should be called.

   Parameters:
    s       - Socket descriptor returned from a previous call to socket
    name    - Pointer to the sockaddr structure containing the
              peer address and port number
    namelen - Length of the sockaddr structure

   Returns:
    If the connect() function succeeds, it returns 0. Otherwise,
    the value SOCKET_ERROR is returned to indicate an error
    condition (and errno is set accordingly).
    For stream based socket, if the connection is not
    established yet, connect returns SOCKET_ERROR and
    errno = EINPROGRESS.

   Remarks:
    None.
    
 */
int     connect( SOCKET s, struct sockaddr* name, int namelen );

//*****************************************************************************
/* Function:
    int send( SOCKET s, const char* buf, int len, int flags )

   Summary:
    The send function is used to send outgoing data on an already
    connected socket.

   Description:
    The send function is used to send outgoing data on an already
    connected socket. This function is used to send a reliable,
    ordered stream of data bytes on a socket of type SOCK_STREAM
    but can also be used to send datagrams on a socket of type SOCK_DGRAM.

   Precondition:
    The connect function should be called for TCP and UDP sockets.
    Server side, accept function should be called.

   Parameters:
    s     - Socket descriptor returned from a previous call to socket
    buf   - Application data buffer containing data to transmit
    len   - Length of data in bytes
    flags - Message flags (currently this field is not supported)

   Returns:
    On success, send returns number of bytes sent.
    Zero indicates no data send.
    In case of error it returns SOCKET_ERROR (and errno is set accordingly).

  Remarks:
    None.
    
 */
int     send( SOCKET s, const char* buf, int len, int flags );

//*****************************************************************************
/* Function:
    int sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)

   Summary:
    This function used to send the data for both connection oriented and connection-less
    sockets.

   Description:
    The sendto function is used to send outgoing data on a socket.
    The destination address is given by to and tolen. Both
    Datagram and stream sockets are supported.

   Precondition:
    The socket function should be called.

   Parameters:
    s     - Socket descriptor returned from a previous call to socket.
    buf   - application data buffer containing data to transmit.
    len   - length of data in bytes.
    flags - message flags. Currently this field is not supported.
    to    - Optional pointer to the sockaddr structure containing the
            destination address.  If NULL, the currently bound remote port and IP
            address are used as the destination.
    tolen - length of the sockaddr structure.

  Returns:
    On success, sendto returns number of bytes sent. In case of
    error returns SOCKET_ERROR (and errno is set accordingly).

  Remarks:
    None.
    
 */
int     sendto( SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen );

//*****************************************************************************
/* Function:
    int recv( SOCKET s, char* buf, int len, int flags )

   Summary:
    The recv() function is used to receive incoming data that has
    been queued for a socket.

   Description:
    The recv() function is used to receive incoming data that has
    been queued for a socket. This function can be used with both
    datagram and stream socket. If the available data
    is too large to fit in the supplied application buffer buf,
    excess bytes are discarded in case of SOCK_DGRAM type
    sockets.  For SOCK_STREAM types, the data is buffered
    internally so the application can retrieve all data by
    multiple calls of recvfrom.

   Precondition:
    The connect function should be called for TCP and UDP sockets.
    Server side, accept function should be called.

   Parameters:
    s     - Socket descriptor returned from a previous call to socket
    buf   - Application data receive buffer
    len   - Buffer length in bytes
    flags - No significance in this implementation

   Returns:
    If the recv function is successful, the socket is valid and it has pending data:
    - The supplied buffer is non NULL and has non zero length, the function will return
      the number of bytes copied to the application buffer.
    - The supplied buffer is NULL or has zero length then no data will be copied and
      the function will return the number of bytes pending in the socket buffer.

    A return value of SOCKET_ERROR (-1)
    indicates an error condition (and errno set accordingly).
    errno is set to EWOULDBLOCK if there is no data pending in the socket buffer.

    A value of zero indicates socket has been shutdown by the peer. 

  Remarks:
    None.
    
 */
int     recv( SOCKET s, char* buf, int len, int flags );

//*****************************************************************************
/* Function:
    int recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen)

   Summary:
    The recvfrom() function is used to receive incoming data that
    has been queued for a socket.

   Description:
    The recvfrom() function is used to receive incoming data that
    has been queued for a socket. This function can be used with
    both datagram and stream type sockets. If the available data
    is too large to fit in the supplied application buffer buf,
    excess bytes are discarded in case of SOCK_DGRAM type
    sockets. For SOCK_STREAM types, the data is buffered
    internally so the application can retrieve all data by
    multiple calls of recvfrom.

   Precondition:
    The socket function should be called.

   Parameters:
    s       - Socket descriptor returned from a previous call to socket
    buf     - Application data receive buffer
    len     - Buffer length in bytes
    flags   - Message flags (currently this is not supported)
    from    - Pointer to the sockaddr structure that will be filled in with the destination address
    fromlen - Size of buffer pointed by from

  Returns:
    If recvfrom is successful, the number of bytes copied to the application buffer buf is returned.
    A return value of SOCKET_ERROR (-1) indicates an error condition (and errno is set accordingly).
    A value of zero indicates socket has been shutdown by the peer.

  Remarks:
    None.
    
 */
int     recvfrom( SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen );

//*****************************************************************************
/* Function:
    int gethostname(char* name, int namelen )

   Summary:
    Returns the standard host name for the system.

   Description:
    This function returns the standard host name of the system which is
    calling this function.  The returned name is null-terminated.

   Precondition:
    None.

   Parameters:
    name    - Pointer to a buffer that receives the local host name
    namelen - Size of the name array

   Returns:
    Success will return a value of 0.
    If name is too short to hold the host name or any other error occurs,
    SOCKET_ERROR (-1) will be returned (and errno is set accordingly).
    On error, *name will be unmodified and no null terminator will be generated.

   Remarks:
    The function returns the host name as set on the default network interface.
    
 */
int     gethostname(char* name, int namelen);

//*****************************************************************************
/* Function:
    int closesocket( SOCKET s )

   Summary:
    The closesocket function closes an existing socket.

   Description:
    The closesocket function closes an existing socket.
    This function releases the socket descriptor s.
    Any data buffered at the socket is discarded.  If the
    socket s is no longer needed, closesocket() must be
    called in order to release all resources associated with s.

   Precondition:
    None.

   Parameters:
    s - Socket descriptor returned from a previous call to socket

  Returns:
    If closesocket is successful, a value of 0 is returned.
    A return value of SOCKET_ERROR (-1) indicates an error (and errno is set accordingly).

  Remarks:
    None.
    
 */
int     closesocket( SOCKET s );

//******************************************************************************
/* Function:
    int  setsockopt(SOCKET s, uint32_t level, uint32_t option_name, const uint8_t *option_value, 
                    uint32_t option_length)

   Summary:
    Allows setting options to a socket like adjust RX/TX buffer size, etc

   Description:
    Various options can be set at the socket level.
    This function provides compatibility with BSD implementations.

   Precondition:
    None.

   Parameters:
    s               - Socket descriptor returned from a previous call to socket
    level           - On which level the operation is to be performed:
                      * IPPROTO_IP     - Applies to the IP protocol layer (not yet supported)
                      * IPPROTO_TCP    - Applies to the TCP protocol layer
                      * SOL_SOCKET     - Applies to the socket layer
                      * IPPROTO_IPV6   - Applies to the IPv6 protocol layer (not yet supported)
                      * IPPROTO_ICMPV6 - Applies to the ICMPv6 protocol layer (not yet supported)
    option_name     - The name of the option to be set:
                      * IPPROTO_TCP
                      * TCP_NODELAY - Specifies whether or not the stack should use the Nagle algorithm
                      * SOL_SOCKET
                      * SO_LINGER   - Specifies what the stack should do with unsent data on close()
                      * SO_RCVBUF   - Specifies the size of the Receive Buffer (TCP only)
                      * SO_SNDBUF   - Specifies the size of the Transmit Buffer
    option_value    - For all values of option_name this is a pointer to the data, which in most cases 
                      is an integer.  The only exception is SO_LINGER which points to a linger structure.
    option_length   - The size of the data pointed to by option_value

   Returns:
    - 0  - If successful
    - -1 - If unsuccessful
  */


int setsockopt(SOCKET s,
               uint32_t level,
               uint32_t option_name,
               const uint8_t *option_value,
               uint32_t option_length);

//******************************************************************************
/* Function:
    int  getsockopt(SOCKET s, uint32_t level, uint32_t option_name, uint8_t *option_value, uint32_t *option_length)

   Summary:
    Allows setting options to a socket like adjust RX/TX buffer size, etc

   Description:
    Various options can be set at the socket level.
    This function provides compatibility with BSD implementations.

   Precondition:
    None.

   Parameters:
    s               - Socket descriptor returned from a previous call to socket
    level           - On which level the operation is to be performed:
                      * IPPROTO_IP     - Applies to the IP protocol layer (not yet supported)
                      * IPPROTO_TCP    - Applies to the TCP protocol layer
                      * SOL_SOCKET     - Applies to the socket layer
                      * IPPROTO_IPV6   - Applies to the IPv6 protocol layer (not yet supported)
                      * IPPROTO_ICMPV6 - Applies to the ICMPv6 protocol layer (not yet supported)
    option_name     - The name of the option to be set:
                      * IPPROTO_TCP
                      * TCP_NODELAY - Specifies whether or not the stack should use the Nagle algorithm
                      * SOL_SOCKET
                      * SO_LINGER   - Specifies what the stack should do with unsent data on close()
                      * SO_RCVBUF   - Specifies the size of the Receive Buffer (TCP only)
                      * SO_SNDBUF   - Specifies the size of the Transmit Buffer
    option_value    - For all values of option_name this is a pointer to the data, which in most cases 
                      is an integer.  The only exception is SO_LINGER which points to a linger structure.
    option_length   - The size of the data pointed to by option_value

  Returns:
    - 0  - If successful
    - -1 - If unsuccessful
  */

int getsockopt(SOCKET s,
               uint32_t level,
               uint32_t option_name,
               uint8_t *option_value,
               uint32_t *option_length);

//******************************************************************************
/* Function:
    struct hostent * gethostbyname(char *name)

   Summary:
    The gethostbyname function returns a structure of type hostent for the given
    host name.

   Description:
    Note: This function supports IPv4 only.
    h_errno will be set to:
    - TRY_AGAIN     if the DNS query is current in progress,
    - HOST_NOT_FOUND if the DNS query could not find a host name
    - NO_RECOVERY if the DNS query had an unrecoverable error

   Precondition:
    None.

   Parameters:
    name            - The name of the host to be found

   Returns:
    The hostent structure on success, or NULL on error.

  */

struct hostent * gethostbyname(char *name);

//******************************************************************************
/* Function:
    int getsockname( SOCKET s, struct sockaddr *addr, int *addrlen);

   Summary:
    Returns the current address to which the socket is bound.
 
   Description:
    The function returns the current address to which the socket 
    is bound, in the buffer pointed to by addr.
 

   Precondition:
    The socket function should be called.

  Parameters:
    s         - Socket descriptor returned from a previous call to socket
    addr      - address to store the current address to which the socket is bound
    addrlen   - on input it should point to the space (bytes) available in addr
                on output it points to the actual space required for storing the bound address

  Returns:
    - On success - 0 is returned and the data is updated accordingly.
    - On error   - -1 is returned and errno is set appropriately.

 Remarks:
    This function supports IPv4 connections only.

  */

int getsockname( SOCKET s, struct sockaddr *addr, int *addrlen);

//******************************************************************************
/* Function:
    int TCPIP_BSD_Socket(SOCKET s);

   Summary:
    Returns the native socket number associated with the BSD socket.
 

   Description:
    The function returns the native socket number associated with the BSD socket.
    Using this call the caller can switch to the native TCP/IP API.
   
   Precondition:
    The socket() function should have been called.

   Parameters:
    s         - Socket descriptor returned from a previous call to socket
   
  Returns:
    - On success - a socket number >= 0 is returned.
    - On error   - -1 is returned if no such socket exists and errno is set to EBADF.

  Remarks:
    This function works for both TCP and UDP sockets.

    The native UDP sockets are created after a call to bind (server sockets)
    or connect (client sockets).
    The native TCP sockets are created after a call to listen (server sockets)
    or connect (client sockets).
    Please note that calling the TCPIP_BSD_Socket before one of these calls will 
    return an INVALID_SOCKET.

    The BSD module uses the NET_PRES layer for supporting encrypted connections.
    This function returns the transport socket (TCP/UDP) associated with the NET_PRES
    connection.
    See TCPIP_BSD_PresSocket for getting the NET_PRES socket

  */

int TCPIP_BSD_Socket(SOCKET s);

//******************************************************************************
/* Function:
    int TCPIP_BSD_PresSocket(SOCKET s);

   Summary:
    Returns the NET_PRES socket number associated with the BSD socket.
 

   Description:
    The function returns the NET_PRES socket number associated with the BSD socket.
    Using this call the caller can access the NET_PRES API.
   
   Precondition:
    The socket() function should have been called.

   Parameters:
    s         - Socket descriptor returned from a previous call to socket
   
  Returns:
    - On success - a socket number >= 0 is returned.
    - On error   - -1 is returned if no such socket exists and errno is set to EBADF.

  Remarks:
    This function works for both TCP and UDP sockets.

    The NET_PRES UDP sockets are created after a call to bind (server sockets)
    or connect (client sockets).
    The NET_PRES TCP sockets are created after a call to listen (server sockets)
    or connect (client sockets).
    Calling the TCPIP_BSD_PresSocket before the sockets are created will
    return an INVALID_SOCKET.

    The BSD module uses the NET_PRES layer for supporting encrypted connections.
    This function returns the NET_PRES socket used for the connection.

  */

int TCPIP_BSD_PresSocket(SOCKET s);

//******************************************************************************
/* Function:
    int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res);

   Summary:
    Does an address look up for the provided node name.


   Description:
    This function deprecates gethostbyname.  It handles both IPv4 and IPv6.

   Precondition:
    The MPLAB Harmony DNS client services must be active.

   Parameters:
    node         - The name of the server to look up.
    service      - Unused
    hints        - hints to the function.  Currently only ai_family is used.  
                   Set to either 0, AF_INET or AF_INET6
    res          - Memory location of where the results are.  Results must be freed with freeaddrinfo

   Returns:
    On success, a 0 is returned.
    - EAI_NONAME - When no name could be found
    - EAI_FAIL   - When a failure has occurred
    - EAI_AGAIN  - When the look-up is in progress.  Call the function again later 
                   to check the results.

  */
int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res);

//******************************************************************************
/* Function:
    int freeaddrinfo(struct addrinfo *res);

   Summary:
    Frees the memory allocated by getaddrinfo

   Description:
    Frees the memory allocated by getaddrinfo

   Precondition:
    getaddrinfo must have returned successfully.

  Parameters:
    res          - Memory allocated by getaddrinfo

  Returns:
   None.

  */

void freeaddrinfo(struct addrinfo *res);

// *****************************************************************************
/*
  Function:
    void  TCPIP_BSD_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs BSD module tasks in the TCP/IP stack.

  Precondition:
    BSD module should have been initialized

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_BSD_Task(void);



//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // _BERKELEY_API_HEADER_FILE

