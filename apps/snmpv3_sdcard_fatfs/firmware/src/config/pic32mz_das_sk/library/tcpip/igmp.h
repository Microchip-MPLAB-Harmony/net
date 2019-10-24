/*******************************************************************************
  IGMPv3 Module Definitions for the Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    igmp.h

  Summary:
    The Internet Group Management Protocol version 3 implementation.

  Description:
    The Internet Group Management Protocol is used by IPv4 hosts
    to report their IP multicast group membership to 
    multicast routers.

    The module implements the IGMPv3 host requirements, supporting ASM and SSM multicast
    semantics.

Reference: RFC 3376, RFC 3678, RFC 4604, RFC 4607, RFC 1112, RFC 2730, RFC 2771  

*******************************************************************************/
//DOM-IGNORE-BEGIN
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








//DOM-IGNORE-END

#ifndef __IGMP_H
#define __IGMP_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  
  Enumeration:
	TCPIP_IGMP_RESULT

  Summary:
    This enumeration is used  to report the result of an IGMP API call.

  Description:
    These results are returned by the IGMP API functions.
	
  Remarks:
    Negative codes represent errors.

*/
typedef enum
{
    TCPIP_IGMP_OK                       = 0,    // operation successful

    // error codes, < 0
    TCPIP_IGMP_IF_ERROR                 = -1,   // no such interface, or multicast not enabled on this interface
                                                // or already all multicast interfaces are in use
    TCPIP_IGMP_ARG_ERROR                = -2,   // bad call argument 
    TCPIP_IGMP_MCAST_ADDRESS_ERROR      = -3,   // invalid multicast address 
    TCPIP_IGMP_SOURCE_INVALID           = -4,   // invalid/not supported source address 
    TCPIP_IGMP_FILTER_ERROR             = -5,   // unsupported filter
    TCPIP_IGMP_SSM_FILTER_ERROR         = -6,   // invalid filter in SSM mode
    TCPIP_IGMP_GROUP_INVALID            = -7,   // no such multicast source is registered
    TCPIP_IGMP_SOCKET_INVALID           = -8,   // no such socket registered for the multicast source
    TCPIP_IGMP_MCAST_CACHE_FULL         = -9,   // all multicast addresses are already used
                                                // no new group could be added
    TCPIP_IGMP_SOURCE_CACHE_FULL        = -10,  // all source addresses are already used for this multicast address (group)
                                                // some sources could be added but not all!
    TCPIP_IGMP_SOCKET_RECORD_FULL       = -11,  // all socket records already used for source address in the multicast address (group)
    TCPIP_IGMP_REPORT_POOL_EMPTY        = -12,  // all reports are busy, a new one could not be sent
    TCPIP_IGMP_PACKET_ALLOC_ERROR       = -13,  // an IGMP packet could not be allocated
    TCPIP_IGMP_PACKET_TRANSMIT_ERROR    = -14,  // an IGMP packet could not be transmitted

}TCPIP_IGMP_RESULT;

// *****************************************************************************
/*  
  Enumeration:
	TCPIP_IGMP_FILTER_TYPE

  Summary:
    This enumeration lists the supported IGMP filters.

  Description:
    These filters are currently supported by IGMPv3.
	
  Remarks:
    None

*/
typedef enum
{
    TCPIP_IGMP_FILTER_NONE,         // inactive/invalid filter
    TCPIP_IGMP_FILTER_INCLUDE,      // interface is in include mode: listening to a set of sources
    TCPIP_IGMP_FILTER_EXCLUDE,      // interface is in exclude mode: listening to all but a set of sources
}TCPIP_IGMP_FILTER_TYPE;

// *****************************************************************************
/*  
  Enumeration:
	TCPIP_IGMP_EVENT_TYPE

  Summary:
    This enumeration lists the IGMP events used to notify IGMP client applications.

  Description:
    These events are used while notifying to the registered applications.
	
  Remarks:
    Multiple events can be set.
*/
typedef enum
{
    TCPIP_IGMP_EVENT_NONE                           = 0x0000 ,  // IGMP no event
    TCPIP_IGMP_EVENT_PACKET_EXCEED_MTU              = 0x0001,   // IGMP packet exceeded MTU and cannot be transmitted
                                                                // currently this situation is handled by truncating the packet
                                                                // decrease number of groups and/or sources
    TCPIP_IGMP_EVENT_PACKET_ALLOC_ERROR             = 0x0002,   // an IGMP packet could not be allocated
    TCPIP_IGMP_EVENT_PACKET_TRANSMIT_ERROR          = 0x0004,   // an IGMP packet could not be transmitted
    TCPIP_IGMP_EVENT_GET_SC_REPORT_ERROR            = 0x0008,   // a free Source Change report could not be obtained
                                                                // (all are already scheduled)
    TCPIP_IGMP_EVENT_GET_GEN_QUERY_REPORT_ERROR     = 0x0010,   // a free general query report could not be obtained
                                                                // (all are already scheduled)
    TCPIP_IGMP_EVENT_GET_GROUP_QUERY_REPORT_ERROR   = 0x0020,   // a free group query report could not be obtained
                                                                // (all are already scheduled)
    TCPIP_IGMP_EVENT_QUERY_SOURCE_NUMBER_EXCEEDED   = 0x0040,   // router is asking fr a report of more sources than we can hold;
                                                                // reported sources will be truncated
                                                                // adjust TCPIP_IGMP_SOURCES_PER_GROUP


}TCPIP_IGMP_EVENT_TYPE;

// *****************************************************************************
/* Type:
		TCPIP_IGMP_EVENT_HANDLER

  Summary:
    Notification handler that can be called when an IGMP event ocurrs and the user needs to be notified
   
  Description:
    The format of a notification handler registered with the IGMP module.
	Once an IGMP event occurs the IGMP service will call for the registered handler.
	
  Parameters:
    mcastAddress    - the multicast group for which the IGMP event occurred
    evType          - the IGMP reported event
    param           - additional user parameter - see TCPIP_IGMP_HandlerRegister 

  Remarks:
    If mcastAddress == 0, the notification is called for events for any multicast group.
*/
typedef void    (*TCPIP_IGMP_EVENT_HANDLER)(IPV4_ADDR mcastAddress, TCPIP_IGMP_EVENT_TYPE evType, const void* param);

// *****************************************************************************
/*
  Type:
    TCPIP_IGMP_HANDLE

  Summary:
    IGMP client handle.

  Description:
    A handle that a application needs to use when deregistering a notification handler. 

  Remarks:
    This handle can be used by the application after the event handler has been registered.
*/
typedef const void* TCPIP_IGMP_HANDLE;


// *****************************************************************************
/*
  Structure:
    TCPIP_IGMP_GROUP_INFO

  Summary:
    IGMP group information

  Description:
    This structure provides IGMP information for a specific group
*/

typedef struct
{
    int                     listSize;           // in: number of elements in the sourceList array
                                                // Could be 0 if the actual sources not needed
    IPV4_ADDR*              sourceList;         // array of IPv4 addresses
                                                // out: array that will store the sources associated to this
                                                // (group, interface) pair.
                                                // Could be NULL if the actual sources not needed
                                                // Up to listSize sources will be copied
                                                // Note that there may be more sources present if 
                                                // presentSources > listSize
    int                     presentSources;     // out: number of sources that are present for this group
    TCPIP_IGMP_FILTER_TYPE  filterType;         // out: current interface filter

} TCPIP_IGMP_GROUP_INFO;


// *****************************************************************************
/*
  Structure:
    TCPIP_IGMP_MODULE_CONFIG

  Summary:
    IGMP module run-time configuration/initialization data.

  Description:
    This structure defines IGMP module run-time configuration/initialization data.

  Remarks:
    Currently the dynamic implementation is not supported.

*/
//
typedef struct
{
    const char* lowSsmAddress;  // the lower limit of the SSM range;
                                // if 0, default value IANA 232.0.0.1 is used
    const char* highSsmAddress; // the high limit of the SSM range;
                                // if 0, default value IANA 232.255.255.255 is used.
                                //
    int         reportInterval; // Unsolicited report interval, ms; default 1 second
                                // The Unsolicited Report Interval is the time between repetitions of a
                                // host’s initial report of membership in a group.
                                // RFC 3376 IGMPv3: Default: 1 second.

    int         nInterfaces;    // the (maximum) number of interfaces that support IGMP multicast
                                // the minimum between this number and the actual number of present interfaces
                                // when the stack is initialized will be selected
 
    // dynamic implementation members
    int         nSockets;       // maximum number of sockets that can subscribe to IGMP    
    int         nGroups;        // the (maximum) number of multicast groups (multicast destination addresses)
                                // that the IGMP module can accomodate
    int         nGroupSources;  // the (maximum) number of sources that can belong to a group
    int         nSourceSockets; // maximum number of different sockets that could listen on a source
                                // as part of a group


} TCPIP_IGMP_MODULE_CONFIG;

        



// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************
        


// *****************************************************************************
/* 
  Function:
    TCPIP_IGMP_RESULT TCPIP_IGMP_Subscribe(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress,
  		              TCPIP_IGMP_FILTER_TYPE filterMode, const IPV4_ADDR* sourceList, size_t* listSize);

  Summary:
    Subscribes to an IGMP multicast group.

  Description:
    This function performs the subscription of a UDP socket to multicast traffic.
    Following this call, the IGMP module will inform nework routers of this
    host desire to take part in multicast traffic.

  Precondition:
    The IGMP module must be initialized.

  Parameters:
    socket       - the UDP multicast socket
    hNet         - Interface handle.
                   if hNet == 0, then the default interface will be used!
    mcastAddress - the multicast group address to subscribe to
                   It has to be within the IANA specified multicast address range.
    filterMode   - filter type required for this subscription: include/exclude
    sourceList   - array of destination sources for which the socket is supposed to listen
                   (filter is include) or is not interested in (filter is exclude).

    listSize     - the number of sources in the sourceList array.
                   On input: if listSize == 0 or *listSize == 0, then no sources will be considered and sourceList will be ignored
                   On output, if listSize != 0, it will contain the number of new sources added/removed

  Returns:
    TCPIP_IGMP_OK if operation succeeded
    An error code otherwise.

  Remarks:
    The function supports both SSM and ASM functionality.
    SSM - Source Specific Multicast
    ASM - Any source multicast

    Subscribe(skt, INCLUDE, {a,b}) and then Subscribe(skt, INCLUDE, {c, d}) is legal  and will just add new sources to include.
    Same for Subscribe(skt, EXCLUDE, {a,b}) and then Subscribe(skt, EXCLUDE, {c, d}) is legal  and will just exclude new sources.

    But first Subscribe(skt, INCLUDE, {a,b}) and then Subscribe(skt, EXCLUDE, {c, d}) will change the socket filter mode and old sources will be removed
    Same for Subscribe(skt, EXCLUDE, {a,b}) and then Subscribe(skt, INCLUDE, {c, d}) will change the socket filter mode and old sources will be removed

    Subscribe(skt, INCLUDE, {}) will remove all subscriptions for the socket

    For SSM sources the EXCLUDE mode is not allowed!

 */
TCPIP_IGMP_RESULT TCPIP_IGMP_Subscribe(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress,
  		              TCPIP_IGMP_FILTER_TYPE filterMode, const IPV4_ADDR* sourceList, size_t* listSize);


// *****************************************************************************
/* 
  Function:
    TCPIP_IGMP_RESULT TCPIP_IGMP_Unsubscribe(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, const IPV4_ADDR* sourceList, size_t* listSize);

  Summary:
    Removes the subscription to sources in an IGMP multicast group.

  Description:
    This function removes the subscription of a UDP socket to multicast traffic sources.
    Following this call, the IGMP module will inform nework routers of the
    host updates regarding the multicast traffic.

  Precondition:
    The IGMP module must be initialized.

  Parameters:
    socket       - the UDP multicast socket
    hNet         - Interface handle.
                   if hNet == 0, then the default interface will be used!
    mcastAddress - the multicast group address to unsubscribe from
                   It has to be within the IANA specified multicast address range.
    filterMode   - filter type required for this subscription: include/exclude
    sourceList   - array of destination sources from which the socket requires unsubscription

    listSize     - the number of sources in the sourceList array.
                   On input: if listSize == 0 or *listSize == 0, then no sources will be considered and sourceList will be ignored
                   On output, if listSize != 0, it will contain the number of new sources removed


  Returns:
    TCPIP_IGMP_OK if operation succeeded
    An error code otherwise.

  Remarks:
    The function supports both SSM and ASM functionality.
    
    The function removes the subscription to the sources no matter the filter mode.

    If sourceList is empty, then unsubscribe all sources for this (G, socket) pair will be performed
    So this call is equivalent to Subscribe(INCLUDE, {})

 */
TCPIP_IGMP_RESULT TCPIP_IGMP_Unsubscribe(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, const IPV4_ADDR* sourceList, size_t* listSize);

// *****************************************************************************
/* 
  Function:
    TCPIP_IGMP_RESULT TCPIP_IGMP_SubscribeGet(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress,
  		              TCPIP_IGMP_FILTER_TYPE* filterMode, IPV4_ADDR* sourceList, size_t* listSize);

  Summary:
    Returns the subscription to sources in an IGMP multicast group.

  Description:
    This function returns the subscription of a UDP socket to a multicast group.

  Precondition:
    The IGMP module must be initialized.

  Parameters:
    socket       - the UDP multicast socket
    hNet         - Interface handle.
                   if hNet == 0, then the default interface will be used!
    mcastAddress - the multicast group address
    filterMode   - address to store the filter type associated with this (Group, interface)tuple
    sourceList   - pointer to an array of destination sources that will be updated with the socket source subscriptions

    listSize     - pointer to the number of sources that could be written in the sourceList array.
                   Could point to 0 if nothing is to be written to sourceList but only the number of sources is to be retrieved.
                   On output: listSize will contain the number of sources that exist for this socket
                   If sourceList != 0 it will contain the sources existent for this socket
                   

  Returns:
    TCPIP_IGMP_OK if operation succeeded
    An error code otherwise.

  Remarks:
    The function supports both SSM and ASM functionality.
    
    listSize == 0 or (*listSize != 0 and sourceList == 0) is invalid!

 */
TCPIP_IGMP_RESULT TCPIP_IGMP_SubscribeGet(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress,
  		              TCPIP_IGMP_FILTER_TYPE* filterMode, IPV4_ADDR* sourceList, size_t* listSize);

// *****************************************************************************
/* 
  Function:
    TCPIP_IGMP_RESULT TCPIP_IGMP_IncludeSource(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress);

  Summary:
    Helper to subscribe to one source.

  Description:
    This function performs the subscription of a UDP socket to multicast traffic
    for one source.


  Precondition:
    The IGMP module must be initialized.

  Parameters:
    socket       - the UDP multicast socket
    hNet         - Interface handle.
                   if hNet == 0, then the default interface will be used!
    mcastAddress - the multicast address to subscribe to
                   It has to be within the IANA specified multicast address range.
    sourceAddress - destination source for which the socket wants to listen



  Returns:
    TCPIP_IGMP_OK if operation succeeded
    An error code otherwise.

  Remarks:
    The function supports both SSM and ASM functionality.

    It is a shortcut to TCPIP_IGMP_Subscribe(INCLUDE, 1 source);

 */
TCPIP_IGMP_RESULT TCPIP_IGMP_IncludeSource(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress);

// *****************************************************************************
/* 
  Function:
    TCPIP_IGMP_RESULT TCPIP_IGMP_ExcludeSource(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress);

  Summary:
    Helper to unsubscribe from one source.

  Description:
    This function performs the unsubscription of a UDP socket to multicast traffic
    from one source.


  Precondition:
    The IGMP module must be initialized.

  Parameters:
    socket        - the UDP multicast socket
    hNet          - Interface handle.
                    if hNet == 0, then the default interface will be used!
    mcastAddress  - the multicast address to unsubscribe from
                    It has to be within the IANA specified multicast address range.
    sourceAddress - destination source for which the socket wants to unsubscribe from



  Returns:
    TCPIP_IGMP_OK if operation succeeded
    An error code otherwise.

  Remarks:
    The function supports ASM functionality.

    It is a shortcut to TCPIP_IGMP_Subscribe(EXCLUDE, 1 source);

 */
TCPIP_IGMP_RESULT TCPIP_IGMP_ExcludeSource(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress);



// *****************************************************************************
/* 
  Function:
    TCPIP_IGMP_RESULT TCPIP_IGMP_Join(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress);

  Summary:
    IGMPv2 style "join" function.

  Description:
    This function performs the IGMPv2 style "join" operation for a UDP socket to a multicast group.


  Precondition:
    The IGMP module must be initialized.

  Parameters:
    socket       - the UDP multicast socket
    hNet         - Interface handle.
                   if hNet == 0, then the default interface will be used!
    mcastAddress - the multicast group to join to
                   It has to be within the IANA specified multicast address range.



  Returns:
    TCPIP_IGMP_OK if operation succeeded
    An error code otherwise.

  Remarks:
    The function is an ASM  helper.

    It is a shortcut to TCPIP_IGMP_Subscribe(EXCLUDE, {});

 */
TCPIP_IGMP_RESULT TCPIP_IGMP_Join(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress);

// *****************************************************************************
/* 
  Function:
    TCPIP_IGMP_RESULT TCPIP_IGMP_Leave(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress);

  Summary:
    IGMPv2 style "leave" function.

  Description:
    This function performs the IGMPv2 style "leave" operation for a UDP socket from a multicast group.


  Precondition:
    The IGMP module must be initialized.

  Parameters:
    socket       - the UDP multicast socket
    hNet         - Interface handle.
                   if hNet == 0, then the default interface will be used!
    mcastAddress - the multicast group to leave
                   It has to be within the IANA specified multicast address range.



  Returns:
    TCPIP_IGMP_OK if operation succeeded
    An error code otherwise.

  Remarks:
    The function is an ASM  helper.

    It is a shortcut to TCPIP_IGMP_Subscribe(INCLUDE, {});

 */
TCPIP_IGMP_RESULT TCPIP_IGMP_Leave(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress);


// *****************************************************************************
/* 
  Function:
	TCPIP_IGMP_HANDLE TCPIP_IGMP_HandlerRegister(IPV4_ADDR mcastAddress, TCPIP_IGMP_EVENT_HANDLER handler, const void* hParam);

  Summary:
    Registers a IGMP client Handler.

  Description:
    This function registers a IGMP client event handler.
    The IGMP client module will call the registered handler when a
    IGMP client event (TCPIP_IGMP_EVENT_TYPE) occurs.

  Precondition:
    The IGMP module must be initialized.

  Parameters:
    hNet    - Interface handle.
              Use hNet == 0 to register on all interfaces available.
    handler - Handler to be called when a IGMP client event occurs.
    hParam  - Pointer to be used by the application to differentiate
              between IGMP events.
              Not used by IGMP.

  Returns:
    - Returns a valid handle if the call succeeds 
	- Returns null handle if the call failed (out of memory, for example)

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, <i>not</i> for lengthy processing!   
 */
TCPIP_IGMP_HANDLE TCPIP_IGMP_HandlerRegister(IPV4_ADDR mcastAddress, TCPIP_IGMP_EVENT_HANDLER handler, const void* hParam);

// *****************************************************************************
/* Function:
	bool TCPIP_IGMP_HandlerDeRegister(TCPIP_IGMP_HANDLE hIgmp);

  Summary:
    Deregisters a previously registered IGMP client handler.
    
  Description:
    This function deregisters the IGMP client event handler.

  Precondition:
    The IGMP module must be initialized.

  Parameters:
    hIgmp   - A handle returned by a previous call to TCPIP_IGMP_HandlerRegister.

  Returns:
   - true  - if the call succeeds
   - false - if no such handler is registered
 */
bool TCPIP_IGMP_HandlerDeRegister(TCPIP_IGMP_HANDLE hIgmp);

// *****************************************************************************
/* Function:
    TCPIP_IGMP_RESULT TCPIP_IGMP_GroupsGet(IPV4_ADDR* groupsList, int listSize, int* pnGroups);


  Summary:
    Gets current info about multicast groups.
    
  Description:
    This function return the current list of the multicast groups that are subscribed with IGMP.
    Additional details about a specific group could be obtained with TCPIP_IGMP_GroupInfoGet.
   
  Precondition:
    The IGMP module must be initialized.

  Parameters:
    groupsList  - pointer to an array of IPv4 addresses that will receive the subscribed multicast groups.
                  Could be NULL if listSize == 0.
    listSize    - The number of entries in the groupsList array
                  If 0, the groupsList will not be used/populated.
    pnGroups    - address to store the number of all subscribed groups
                  Could be NULL if not needed

  Returns:
   - TCPIP_IGMP_OK  - if the call succeeds and the pInfo is populated
   - TCPIP_IGMP_GROUP_INVALID - if no such multicast group exists 
   - TCPIP_IGMP_ARG_ERROR - pInfo is NULL
   - TCPIP_IGMP_IF_ERROR - no such interface
 
  Remarks:
   None

 */
TCPIP_IGMP_RESULT TCPIP_IGMP_GroupsGet(IPV4_ADDR* groupsList, int listSize, int* pnGroups);

// *****************************************************************************
/* Function:
    TCPIP_IGMP_RESULT TCPIP_IGMP_GroupInfoGet(TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, TCPIP_IGMP_GROUP_INFO* pInfo);

  Summary:
    Gets current info about a multicast group.
    
  Description:
    This function return current info about a multicast group status on a specified interface.
   
  Precondition:
    The IGMP module must be initialized.

  Parameters:
    hNet         - interface handle for which the query is made
                   if 0, the default interface will be used
    mcastAddress - the multicast group for which the query is made 
    pInfo        - pointer to a TCPIP_IGMP_GROUP_INFO structure that contains in/out parameters
                   See the description of TCPIP_IGMP_GROUP_INFO.

  Returns:
   - TCPIP_IGMP_OK  - if the call succeeds and the pInfo is populated
   - TCPIP_IGMP_GROUP_INVALID - if no such multicast group exists 
   - TCPIP_IGMP_ARG_ERROR - pInfo is NULL
   - TCPIP_IGMP_IF_ERROR - no such interface
 
  Remarks:
   None

 */
TCPIP_IGMP_RESULT TCPIP_IGMP_GroupInfoGet(TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, TCPIP_IGMP_GROUP_INFO* pInfo);

// *****************************************************************************
/*
  Function:
    void  TCPIP_IGMP_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs IGMP module tasks in the TCP/IP stack.

  Precondition:
    The IGMP module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_IGMP_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __IGMP_H
