/*******************************************************************************
  TCP Manager internal stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcp_manager.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef H__TCP_MANAGER_H_
#define H__TCP_MANAGER_H_


/****************************************************************************
  Section:
    Type Definitions
  ***************************************************************************/
// TCP Header Data Structure
typedef struct
{
    uint16_t    SourcePort;     // Local port number
    uint16_t    DestPort;       // Remote port number
    uint32_t    SeqNumber;      // Local sequence number
    uint32_t    AckNumber;      // Acknowledging remote sequence number

    struct __attribute__((packed))
    {
        unsigned Reserved3      : 4;
        unsigned Val            : 4;
    } DataOffset;           // Data offset flags nibble

    union
    {
        struct __attribute__((packed))
        {
            unsigned flagFIN    : 1;
            unsigned flagSYN    : 1;
            unsigned flagRST    : 1;
            unsigned flagPSH    : 1;
            unsigned flagACK    : 1;
            unsigned flagURG    : 1;
            unsigned Reserved2  : 2;
        } bits;
        uint8_t byteVal;
    } Flags;                // TCP Flags as defined in RFC

    uint16_t    Window;         // Local free RX buffer window
    uint16_t    Checksum;       // Data payload checksum
    uint16_t    UrgentPointer;  // Urgent pointer
} TCP_HEADER;


/****************************************************************************
  Section:
    Function Declarations
  ***************************************************************************/

/*****************************************************************************
  Function:
    bool TCPIP_TCP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, TCPIP_TCP_MODULE_CONFIG* pTcpInit)

  Summary:
    Initializes the TCP module.

  Description:
    Initializes the TCP module.  This function allocates memory for the TCP module
    and registers itself with the stack manager.
    If the initialization failed it will return false.

  Precondition:
    None

  Parameters:
    stackInit   - pointer to stack initialization data; contains heap, interfaces, etc

    pTcpInit    - pointer to a TCP initialization structure containing:
                    - nSockets:         number of sockets to be created
                    - sktTxBuffSize:    default TX buffer size
                    - sktRxBuffSize:    default RX buffer size
  Returns:
    true if initialization succeeded
    false otherwise
    
  Remarks:
   Simple protection for calls to TCPIP_TCP_Initialize/TCPIP_TCP_Deinitialize
   from other threads is supported.

 */
bool TCPIP_TCP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, const void* initData);


/*****************************************************************************
  Function:
    void TCPIP_TCP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit)

  Summary:
    De-Initializes the TCP module.

  Description:
    De-Initializes the TCP module.
    This function initializes each socket to the CLOSED state.
    If dynamic memory was allocated for the TCP sockets, the function
    will deallocate it.

  Precondition:
    TCPIP_TCP_Initialize() should have been called

  Parameters:
    stackInit   - pointer to stack initialization data; contains heap, interfaces, etc
                  and interface that's going down

  Returns:
    None

  Remarks:
 */
void TCPIP_TCP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit);



bool TCPIP_TCP_SourceIPAddressSet(TCP_SOCKET s, IP_ADDRESS_TYPE addType, IP_MULTI_ADDRESS* localAddress);

bool TCPIP_TCP_DestinationIPAddressSet(TCP_SOCKET s, IP_ADDRESS_TYPE addType, IP_MULTI_ADDRESS* remoteAddress);

// conversion functions/helpers
//
static __inline__ TCP_HEADER* __attribute__((always_inline)) FC_U8Ptr2TcpHdr(uint8_t* u8Ptr)
{
    union
    {
        uint8_t* u8Ptr;
        TCP_HEADER* pHdr;
    }U_U8_PTR_TCP_HDR;

    U_U8_PTR_TCP_HDR.u8Ptr = u8Ptr;
    return U_U8_PTR_TCP_HDR.pHdr;
}


#endif  // H__TCP_MANAGER_H_
