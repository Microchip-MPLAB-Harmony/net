/*******************************************************************************
  MAC Module Definitions for the Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.

  File Name:
    tcpip_mac.h

  Summary:


  Description:

*******************************************************************************/

//DOM-IGNORE-BEGIN
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








//DOM-IGNORE-END

#ifndef H_TCPIP_MAC_H_
#define H_TCPIP_MAC_H_
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "system/system_module.h"
#include "driver/driver_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: MAC interface definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  MAC Address

  Summary:
    Definition of a MAC address.

  Description:
    This structure defines the physical MAC address of an interface.

  Remarks:
    None.
*/

typedef struct __attribute__((__packed__))
{
    uint8_t v[6];
} TCPIP_MAC_ADDR;

// *****************************************************************************
/*  MAC Ethernet Header

  Summary:
    Definition of a MAC frame header.

  Description:
    This structure defines the generic Ethernet header
    that starts all the Ethernet frames.

  Remarks:        
    None.
*/

typedef struct  __attribute__((aligned(2), packed))
{
    TCPIP_MAC_ADDR      DestMACAddr;
    TCPIP_MAC_ADDR      SourceMACAddr;
    uint16_t            Type;
} TCPIP_MAC_ETHERNET_HEADER;

// *****************************************************************************
/*  MAC Modules ID

  Summary:
    IDs of the MAC module supported by the stack.

  Description:
    This type defines the supported MAC types. 

  Remarks:        
    Not all types listed in this enumeration are supported.
    New MAC types will be added as needed.
*/

typedef enum
{
    TCPIP_MODULE_MAC_NONE           = 0x1000U,  // unspecified/unknown MAC

    // External ENC28J60 device: room for 16 ENCJ60 devices
    TCPIP_MODULE_MAC_ENCJ60         = 0x1010U,
    TCPIP_MODULE_MAC_ENCJ60_0       = 0x1010U,  // alternate numbered name

    // External ENCX24J600 device: room for 16 ENCJ600 devices 
    TCPIP_MODULE_MAC_ENCJ600        = 0x1020U,
    TCPIP_MODULE_MAC_ENCJ600_0      = 0x1020U,  // alternate numbered name

    // External ETH97J60 device: room for 16 97J60 devices
    TCPIP_MODULE_MAC_97J60          = 0x1030U,
    TCPIP_MODULE_MAC_97J60_0        = 0x1030U,  // alternate numbered name

    // Internal/Embedded PIC32 MAC: room for 16 PIC32 devices
    TCPIP_MODULE_MAC_PIC32INT       = 0x1040U,
    TCPIP_MODULE_MAC_PIC32INT_0     = 0x1040U,  // alternate numbered name

    // Internal/Embedded GMAC of PIC32C: 
    TCPIP_MODULE_MAC_PIC32C         = 0x1050U,  // instance base
    TCPIP_MODULE_MAC_PIC32C_0       = 0x1050U,  // first mac instance
    TCPIP_MODULE_MAC_PIC32C_1       = 0x1051U,  // second mac instance

    // External MRF24WN Wi-Fi MAC: room for 16 MRF24WN devices
    TCPIP_MODULE_MAC_MRF24WN        = 0x1060U,
    TCPIP_MODULE_MAC_MRF24WN_0      = 0x1060U,  // alternate numbered name

    // External WINC Wi-Fi MAC: room for 16 WINC devices
    TCPIP_MODULE_MAC_WINC           = 0x1070U,
    TCPIP_MODULE_MAC_WINC_0         = 0x1070U,  // alternate numbered name

    // External WILC1000 Wi-Fi MAC: room for 16 WILC1000 devices
    TCPIP_MODULE_MAC_WILC1000       = 0x1080U,
    TCPIP_MODULE_MAC_WILC1000_0     = 0x1080U,  // alternate numbered name

    // Internal/Embedded PIC32WK Wi-Fi MAC: room for 16 PIC32WK devices
    TCPIP_MODULE_MAC_PIC32WK        = 0x1090U,
    TCPIP_MODULE_MAC_PIC32WK_0      = 0x1090U,  // alternate numbered name

    // Internal/Embedded PIC32MZW1 Wi-Fi MAC: room for 16 PIC32MZW1 devices
    TCPIP_MODULE_MAC_PIC32MZW1      = 0x10A0U,
    TCPIP_MODULE_MAC_PIC32MZW1_0    = 0x10A0U,  // alternate numbered name

    // Internal/Embedded EMAC of SAM9x60:
    TCPIP_MODULE_MAC_SAM9X60        = 0x1100U,  // instance base
    TCPIP_MODULE_MAC_SAM9X60_0      = 0x1100U,  // first mac instance
    TCPIP_MODULE_MAC_SAM9X60_1      = 0x1101U,  // second mac instance

    // Internal/Embedded PPP MAC:
    TCPIP_MODULE_MAC_PPP            = 0x1200U,  // instance base
    TCPIP_MODULE_MAC_PPP_0          = 0x1200U,  // first mac instance


    // External LAN865X device: room for 16 LAN865X devices
    TCPIP_MODULE_MAC_LAN865X         = 0x1110U,
    TCPIP_MODULE_MAC_LAN865X_0       = 0x1110U, // first mac instance
    TCPIP_MODULE_MAC_LAN865X_1       = 0x1111U, // second mac instance

    // Internal/Embedded G3 ADP MAC
    TCPIP_MODULE_MAC_G3ADP           = 0x10B0U,
    TCPIP_MODULE_MAC_G3ADP_0         = 0x10B0U,   // alternate numbered name

    // External, non MCHP, MAC modules
    TCPIP_MODULE_MAC_EXTERNAL       = 0x4000U,
}TCPIP_MODULE_MAC_ID;

// *****************************************************************************
/*  MAC Handle

  Summary:
    Handle to a MAC driver.

  Description:
    This data type defines a MAC client handle.
  
  Remarks:        
    Another name for the DRV_HANDLE.
*/

typedef DRV_HANDLE TCPIP_MAC_HANDLE;


// *****************************************************************************
/*  MAC Segment Flags

  Summary:
    Flags belonging to MAC data segment.

  Description:
    This enumeration contains the definitions of MAC segment flags:
    segment allocation flags and general purpose flags.
  
  Remarks:        
    16 bits only segment flags are supported.
*/

typedef enum
{
    /*  Segment can not be dynamically deallocated. */
    /*  Set when the segment is allocated */
    TCPIP_MAC_SEG_FLAG_STATIC           = 0x0001U, 

    /*  If set, it's a TX segment; otherwise, is a RX packet. */
    TCPIP_MAC_SEG_FLAG_TX               = 0x0002U, 

    /*  a MAC RX dedicated/sticky segment; otherwise, a non-dedicated/float segment */
    TCPIP_MAC_SEG_FLAG_RX_STICKY        = 0x0004U, 

    /* Segment carrying user payload */
    /* Higher level protocols (TCP, UDP, etc.) may use it */
    TCPIP_MAC_SEG_FLAG_USER_PAYLOAD     = 0x0008U, 
    /*  Ack is required and has not been performed */
    TCPIP_MAC_SEG_FLAG_ACK_REQUIRED     = 0x0010U,
    /*  User available segment flags. */
    TCPIP_MAC_SEG_FLAG_USER             = 0x0100U, 

}TCPIP_MAC_SEGMENT_FLAGS;


// *****************************************************************************
/*  TCPIP MAC Segment Gap Descriptor

  Summary:
    A data gap that's part of the MAC data segment.

  Description:
    This data gap stores for each segment the pointer of the associated
    TCPIP_MAC_PACKET (each segment is allocated as part of a TCPIP_MAC_PACKET).

    It is intended as an simple way for the MAC driver to recover from the
    segment buffer (the buffer where the payload is set) the corresponding packet pointer.

    It also has a variable gap space for the MAC driver use.

  Remarks:

    Normally only the 1st segment of a packet needs 
    this extra gap at the beginning of the segment buffer.

    The MAC driver may make use of the remaining space in the segmentDataGap,
    except the 4 bytes for segmentPktPtr, which is reserved.

    This segmentDataGap space is reserved for the MAC purposes.
    However, it is not guaranteed to be saved when packets are sent across multiple MACs!

    For the Harmony TCP/IP, the TCPIP_MAC_SEGMENT_GAP_DCPT precedes the 
    segment buffer. The gap is at the beginning of the segment buffer.
    So the actual layout of the memory allocated for a segment segBuffer:
    TCPIP_MAC_SEGMENT_GAP_DCPT:
        | ... alignment space, if needed                                |
        | 4 bytes for storing the packet pointer this buffer belongs to |
        | n bytes gap for MAC use. NOT  PRESERVED across calls!         |
    segment buffer:
        | Cache aligned buffer segBuffer (segSize bytes)                |


    The Harmony TCP/IP stack packet allocator properly sets the segmentPktPtr.
        
*/
typedef struct
{
    /* Packet pointer. This is the packet the segment belongs to.
     * Could be used by the MAC driver to restore the packet to which
     * a payload belongs to. */
    struct S_tag_TCPIP_MAC_PACKET*   segmentPktPtr;

    /* Extra space allocated to be used by the MAC driver
     * The size of the gap is variable:
     *      - usually 4 bytes when only Ethernet drivers are used
     *      - 34 bytes when Wi-Fi drivers are present 
    */
    // uint32_t                        segmentDataGap[];

}TCPIP_MAC_SEGMENT_GAP_DCPT;



// *****************************************************************************
/*  TCPIP MAC Data Segment

  Summary:
    A data segment that's part of a TX/RX packet.

  Description:
    Structure of a segment buffer transferred with the MAC.
    A MAC TX or RX packet can consist of multiple data segments.
    On TX the MAC has to be able to transmit packets that span
    multiple data segments.
    On RX of a network frame the MAC may have to use multiple segments to 
    construct a packet.
    (For performance reasons, a contiguous MAC packet, with just one segment, if possible, is preferred).
   
  
  Remarks:
    The Harmony TCP/IP packet allocator concatenates the TCPIP_MAC_SEGMENT_GAP_DCPT
    with the segment buffer.
    Therefore the actual layout of the memory allocated for segment data:
    TCPIP_MAC_SEGMENT_GAP_DCPT:
        | ... alignment space, if needed                                |
        | 4 bytes for storing the packet pointer this buffer belongs to |
        | n bytes gap for MAC use. NOT  PRESERVED across calls!         |
    segment buffer:
        | Cache aligned buffer segBuffer (segSize bytes)                |

    
    Other alocators can produce a different memory layout.

*/


typedef struct S_tag_MAC_DATA_SEGMENT
{
    /*  Multi-segment support, next segment in the chain. */
    struct S_tag_MAC_DATA_SEGMENT* next;      

    /*  Pointer to the segment data buffer
        It specifies the address where the data starts in the buffer.
        On 32-bit machines, the segment data buffer (segBuffer) is allocated so that it is always
        cache line size aligned and its size is 32-bits multiple.
        If the processor does not have cache, then it is 32 bits aligned */
    uint8_t*                 segBuffer;        

    /*  Pointer to segment data payload
        It specifies the address of the 1st payload byte.
        It is updated at run time and it is
        segLoad = segBuffer + dataOffset (see TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2 flag)

        For the Harmony TCP/IP stack the MAC drivers are required to support
        dataOffset == 2 (i.e. TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2 flag will be set). */
    uint8_t*                 segLoad;        

    /*  Segment payload size;
        TX: Number of bytes from this segment that has to be transmitted.
            This is the total number of bytes including the Ethernet header
            but not the FCS (that should be added by the driver)
        RX: Number of payload bytes in the segment.
            The MAC driver subtracts the FCS and Ethernet header length before
            handing over the packet to the stack
            
            Then the segLen field is updated by each stack layer in turn */
    uint16_t                 segLen;         

    /*  Segment allocated total usable size.
        This does not include the TCPIP_MAC_SEGMENT_GAP_DCPT */
    uint16_t                 segSize;        

    /*  TCPIP_MAC_SEGMENT_FLAGS segment flags:
        TCPIP_MAC_SEG_FLAG_STATIC, 
        TCPIP_MAC_SEG_FLAG_RX, 
        TCPIP_MAC_SEG_FLAG_RX, 
        TCPIP_MAC_SEG_FLAG_RX_STICKY */
    uint16_t                 segFlags;       

    /* The size this segment payload allocation. Debug/trace purposes */
    uint16_t                 segAllocSize;  

    /*  Additional client segment data. Ignored by the MAC driver. */
    union
    {
        uint8_t                  segClientData[4];
        uint8_t                  segClientData8[4];
        uint32_t                 segClientData32;
    };

    /*  Additional client segment payload; Ignored by the MAC driver. */
    //  uint8_t                  segClientLoad[];
}TCPIP_MAC_DATA_SEGMENT;


// *****************************************************************************
// *****************************************************************************
// Section:  MAC Data Packet Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  PIC32INT Received Packet Status

  Summary:
    Status of a received packet  for PIC32INT MAC driver.

  Description:
    This structure contains the status of a PIC32INT received packet.
  
  Remarks:
    MACs have different hardware support for the received packet status.

*/

typedef struct __attribute__ ((__packed__))
{

    /*  correct checksum filled in */
    unsigned        chksumOk        : 1;

    /*  Packet payload checksum */
    unsigned        pktChecksum     :16;

    /*  Runt packet received */
    unsigned        runtPkt         : 1;

    /*  Unicast, not me packet, */
    unsigned        notMeUcast      : 1;

    /*  Hash table match */
    unsigned        htMatch         : 1;

    /*  Magic packet match */
    unsigned        magicMatch      : 1;

    /*  Pattern match match */
    unsigned        pmMatch         : 1;

    /*  Unicast match */
    unsigned        uMatch          : 1;

    /*  Broadcast match */
    unsigned        bMatch          : 1;

    /*  Multicast match */
    unsigned        mMatch          : 1;

    /*  Received bytes */
    unsigned        rxBytes         :16;

    /*  CRC error in packet */
    unsigned        crcError        : 1;

    /*  Receive length check error */
    unsigned        lenError        : 1;

    /*  Receive length out of range */
    unsigned        lenRange        : 1;

    /*  Receive OK */
    unsigned        rxOk            : 1;

    /*  Multicast packet */
    unsigned        mcast           : 1;

    /*  Broadcast packet */
    unsigned        bcast           : 1;

    /*  Control frame received */
    unsigned        rxCtrl          : 1;

    /*  Received VLAN tagged frame */
    unsigned        rxVLAN          : 1;

}TCPIP_MAC_PACKET_RX_STAT_PIC32INT;

// *****************************************************************************
/*  PIC32C Received Packet Status

  Summary:
    Status of a received packet  for PIC32C MAC driver.

  Description:
    This structure contains the status of a PIC32C received packet.
  
  Remarks:
    MACs have different hardware support for the received packet status.

*/

typedef union  
{
    uint32_t val;
    struct S_TCPRxStatusBM
    {
        unsigned    len: 13;                /** Length of frame including FCS */
        unsigned    offset: 1;              /** Receive buffer offset,
                                            bits 13:12 of frame length for jumbo frame */
        unsigned    bSof: 1;                /** Start of frame */
        unsigned    bEof: 1;                /** End of frame */
        unsigned    bCFI: 1;                /** Concatenation Format Indicator */
        unsigned    vlanPriority: 3;        /** VLAN priority (if VLAN detected) */
        unsigned    bPriorityDetected: 1;   /** Priority tag detected */
        unsigned    bVlanDetected: 1;       /**< VLAN tag detected */
        unsigned    bTypeIDMatch: 2;        /**< Type ID match */
        unsigned    bTypeIDMatchfound: 1;   /**< Type ID match found*/
        unsigned    bAddrMatch: 2;          /**< Specific Address register 1, 2, 3, 4 match */
        unsigned    bAddrMatchfound: 1;     /**< Specific Address match found */
        unsigned    reserved: 1;                
        unsigned    bUniHashMatch: 1;       /**< Unicast hash match */
        unsigned    bMultiHashMatch: 1;     /**< Multicast hash match */
        unsigned    bBroadcastDetected: 1;  /**< Global all ones broadcast address detected */
    } bm __attribute__((packed));
} TCPIP_MAC_PACKET_RX_STAT_PIC32C;

// *****************************************************************************
/*  MAC Received Packet Status

  Summary:
    Status of a received packet.

  Description:
    This structure contains the status of a received packet.
  
  Remarks:
    MACs have different hardware support for the received packet status.
    Not all the MACs have hardware support for the received packet status.

*/

typedef union
{
    TCPIP_MAC_PACKET_RX_STAT_PIC32INT   rxStatPIC32INT;
    TCPIP_MAC_PACKET_RX_STAT_PIC32C     rxStatPIC32C;    
}TCPIP_MAC_PACKET_RX_STAT;


// *****************************************************************************
/*  MAC Packet Flags

  Summary:
    Flags belonging to MAC packet.

  Description:
    This enumeration contains the definitions of MAC packet flags:
    packet allocation flags and general purpose flags.
  
  Remarks:
    32 bits packet flags are supported.
*/

typedef enum
{
    /* no flag set */
    TCPIP_MAC_PKT_FLAG_NONE                 = 0x0U,

    /* Packet can not be dynamically deallocated.
       Set when the packet is allocated. */
    TCPIP_MAC_PKT_FLAG_STATIC               = 0x00000001U,

    /* If set, it is a TX packet/segment. Otherwise, it is a RX packet.*/
    TCPIP_MAC_PKT_FLAG_TX                   = 0x00000002U,

    /* Packet data spans multiple segments - ZC functionality.
       If not set then the packet has only one data segment.
       It is set by the MAC driver when a RX packet spans multiple
       data segments. */
    TCPIP_MAC_PKT_FLAG_SPLIT                = 0x00000004U,       


    /* Packet data is queued somewhere, cannot be freed.
       The flag is set by a module processing the packet to show that
       the packet is in use and queued for further processing
       (normally the MAC driver does that).
       Cleared by the packet destination when the packet processing was completed.*/
    TCPIP_MAC_PKT_FLAG_QUEUED               = 0x00000008U,       


    /* RX flag, MAC updated. Specifies an unicast packet. */
    TCPIP_MAC_PKT_FLAG_UNICAST              = 0x00000010U,      

    /* RX flag, MAC updated. Specifies a broadcast packet. */
    TCPIP_MAC_PKT_FLAG_BCAST                = 0x00000020U,      

    /* RX flag, MAC updated. Specifies an multicast packet. */
    TCPIP_MAC_PKT_FLAG_MCAST                = 0x00000040U,      

    /* Packet cast mask bits. */
    TCPIP_MAC_PKT_FLAG_CAST_MASK            = 0x00000070U,      

    /* Packet cast mask. Specifies a packet where the MCAST/BCAST fields
       are not updated by the MAC RX process. */
    TCPIP_MAC_PKT_FLAG_CAST_DISABLED        = 0x00000000U,      

    /* Reserved for future use */
    TCPIP_MAC_PKT_FLAG_RESERVED             = 0x00000080U,      

    /* RX packet checksum calculation flags.
       MAC driver updates these flags: */

    /* IP header checksum was calculated for this RX packet and is OK; 0x01 << 8 */
    TCPIP_MAC_PKT_FLAG_RX_CHKSUM_IP         = 0x00000100U,   

    /* TCP checksum was calculated for this RX packet and is OK; 0x02 << 8 */
    TCPIP_MAC_PKT_FLAG_RX_CHKSUM_TCP        = 0x00000200U,      

    /* UDP checksum was calculated for this RX packet and is OK; 0x04 << 8 */
    TCPIP_MAC_PKT_FLAG_RX_CHKSUM_UDP        = 0x00000400U,      

    /* internally generated packet */
    TCPIP_MAC_PKT_FLAG_RX_INTERNAL          = 0x00000800U,

    /* Available user flags. */
    TCPIP_MAC_PKT_FLAG_USER                 = 0x00010000U,      

}TCPIP_MAC_PACKET_FLAGS;


// *****************************************************************************
/*  MAC Packet Acknowledge Result

  Summary:
    List of MAC return codes for a packet acknowledge function.

  Description:
    This enumeration contains the list of MAC codes
    used for a packet acknowledgment.
  
  Remarks:        
    16 bits only acknowledge results are supported.
    Positive codes indicate success.
    Negative codes indicate a failure of some sort.

*/

typedef enum
{
    /* packet result unknown, unspecified */
    TCPIP_MAC_PKT_ACK_NONE              = 0,    

    /* TX success code -  packet was transmitted successfully */
    TCPIP_MAC_PKT_ACK_TX_OK             = 1, 

    /* RX success code -  packet was received/processed successfully */
    TCPIP_MAC_PKT_ACK_RX_OK             = 2,

    /* TX:packet was dropped because the link was down */
    TCPIP_MAC_PKT_ACK_LINK_DOWN         = -1, 

    /* TX:packet was dropped because the network is down */ 
    TCPIP_MAC_PKT_ACK_NET_DOWN          = -2,

    /* TX:packet was dropped because the buffer type is not supported */
    TCPIP_MAC_PKT_ACK_BUFFER_ERR        = -3,

    /* TX:packet was dropped because of an ARP timeout */
    TCPIP_MAC_PKT_ACK_ARP_TMO           = -4, 

    /* TX:packet associated interface is down or non existent*/ 
    TCPIP_MAC_PKT_ACK_ARP_NET_ERR       = -5,   

    /* TX:packet rejected by MAC */ 
    TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR    = -6,   

    /* RX: packet was dropped because the checksum was incorrect */
    TCPIP_MAC_PKT_ACK_CHKSUM_ERR        = -10, 

    /* RX: packet was dropped because of wrong interface source address */
    TCPIP_MAC_PKT_ACK_SOURCE_ERR        = -11,

    /* RX: packet was dropped because of wrong destination address */
    TCPIP_MAC_PKT_ACK_DEST_ERR          = -12,

    /* RX: packet was dropped because the type was unknown  */
    TCPIP_MAC_PKT_ACK_TYPE_ERR          = -13,

    /* RX: internal packet structure error  */
    TCPIP_MAC_PKT_ACK_STRUCT_ERR        = -14,  

    /* RX: the packet protocol couldn't find a destination for it */
    TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR    = -15,

    /* RX: the packet too fragmented  */
    TCPIP_MAC_PKT_ACK_FRAGMENT_ERR      = -16, 

    /* RX: the packet destination is closing */
    TCPIP_MAC_PKT_ACK_PROTO_DEST_CLOSE  = -17,

    /* RX: memory allocation error */
    TCPIP_MAC_PKT_ACK_ALLOC_ERR         = -18,  

    /* Write error in the underlying driver when trying to write the data */
    TCPIP_MAC_PKT_ACK_WRITE_ERR         = -19,  

    /* RX/TX: Packet was rejected by the IP layer */
    TCPIP_MAC_PKT_ACK_IP_REJECT_ERR     = -20,  

    /* RX: packet was dropped because it was processed externally */
    TCPIP_MAC_PKT_ACK_EXTERN            = -21,

    /* RX: packet was directly processed successfuly by the bridge */
    TCPIP_MAC_PKT_ACK_BRIDGE_DONE       = -22,

    /* RX: packet was dropped by the bridge */
    TCPIP_MAC_PKT_ACK_BRIDGE_DISCARD    = -23,
}TCPIP_MAC_PKT_ACK_RES;


// *****************************************************************************
/* MAC RX Filter Types

  Summary:
    Defines the possible MAC RX filter types.

  Description:
    This enumeration defines the RX filtering capabilities for the TCP/IP MAC driver.

  Remarks:
    The Ethernet/Wi-Fi controllers have an RX filter module that takes part in the packet acceptance decision.

    Multiple values can be OR-ed together.

    There usually is a priority in the RX filter processing for a MAC module.
    In order for a packet to be accepted, it has to be specifically accepted by a filter.
    Once a filter accepts or rejects a packet, further filters are not tried.
    If a packet isn't rejected/accepted after all filters are tried, it will be rejected by
    default!

    Not all MACs support all the RX filter types.

    Current implementation does not suport more than 16 filters.
*/

/* MISRA C-2012 Rule 5.2 deviated:4 Deviation record ID -  H3_MISRAC_2012_R_5_2_NET_DR_6 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:4 "MISRA C-2012 Rule 5.2" "H3_MISRAC_2012_R_5_2_NET_DR_6" 
typedef enum
{
    /* Broadcast packets are accepted */
    TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT           /*DOM-IGNORE-BEGIN*/ = 0x0001U /*DOM-IGNORE-END*/,

    /* Multicast packets are accepted */
    TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT           /*DOM-IGNORE-BEGIN*/ = 0x0002U /*DOM-IGNORE-END*/,

    /* Unicast packets to a this host are accepted */
    TCPIP_MAC_RX_FILTER_TYPE_UCAST_ACCEPT           /*DOM-IGNORE-BEGIN*/ = 0x0004U /*DOM-IGNORE-END*/,

    /* Unicast packets to a different host are accepted */
    TCPIP_MAC_RX_FILTER_TYPE_UCAST_OTHER_ACCEPT     /*DOM-IGNORE-BEGIN*/ = 0x0008U /*DOM-IGNORE-END*/,

    /* Runt packets (< 64 bytes) are rejected */
    TCPIP_MAC_RX_FILTER_TYPE_RUNT_REJECT            /*DOM-IGNORE-BEGIN*/ = 0x0010U /*DOM-IGNORE-END*/,

    /* Runt packets (< 64 bytes) are accepted */
    TCPIP_MAC_RX_FILTER_TYPE_RUNT_ACCEPT            /*DOM-IGNORE-BEGIN*/ = 0x0020U /*DOM-IGNORE-END*/,

    /* Packets with CRC errors are rejected */
    TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_REJECT       /*DOM-IGNORE-BEGIN*/ = 0x0040U /*DOM-IGNORE-END*/,

    /* Packets with CRC errors are accepted */
    TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_ACCEPT       /*DOM-IGNORE-BEGIN*/ = 0x0080U /*DOM-IGNORE-END*/,
    
    /* Multicast HASH matching packets are accepted */
    TCPIP_MAC_RX_FILTER_TYPE_MCAST_HASH_ACCEPT       /*DOM-IGNORE-BEGIN*/ = 0x0100U /*DOM-IGNORE-END*/,
    
    /* Unicast HASH matching packets are accepted */
    TCPIP_MAC_RX_FILTER_TYPE_UCAST_HASH_ACCEPT       /*DOM-IGNORE-BEGIN*/ = 0x0200U /*DOM-IGNORE-END*/,
    
    /* Maximum frame size packets(1536 bytes) are accepted */
    TCPIP_MAC_RX_FILTER_TYPE_MAXFRAME_ACCEPT        /*DOM-IGNORE-BEGIN*/ = 0x0400U /*DOM-IGNORE-END*/,

    /* All packets are accepted */
    TCPIP_MAC_RX_FILTER_TYPE_ALL_ACCEPT         /*DOM-IGNORE-BEGIN*/ = 0x0800U /*DOM-IGNORE-END*/,
    
    /* Packets with Frame error are accepted */
    TCPIP_MAC_RX_FILTER_TYPE_FRAMEERROR_ACCEPT       /*DOM-IGNORE-BEGIN*/ = 0x1000U /*DOM-IGNORE-END*/,
    
    /* Packets with JUMBO Frame (up to 10240 bytes) is accepted */
    TCPIP_MAC_RX_FILTER_TYPE_JUMBOFRAME_ACCEPT       /*DOM-IGNORE-BEGIN*/ = 0x2000U /*DOM-IGNORE-END*/,
    
    /* Default RX filter: will accept most valid packets */
    TCPIP_MAC_RX_FILTER_TYPE_DEFAULT     /*DOM-IGNORE-BEGIN*/ = TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_REJECT   | TCPIP_MAC_RX_FILTER_TYPE_RUNT_REJECT |
                                                                TCPIP_MAC_RX_FILTER_TYPE_UCAST_ACCEPT       | TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT |
                                                                TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT
                                         /*DOM-IGNORE-END*/,

}TCPIP_MAC_RX_FILTER_TYPE;
#pragma coverity compliance end_block "MISRA C-2012 Rule 5.2"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */


// *****************************************************************************
/*  MAC Packet forward reference

  Summary:
   Forward reference to a MAC packet.

  Description:
    Forward reference needed in the MAC packet acknowledge function.

  Remarks:        
    None.
*/

typedef struct S_tag_TCPIP_MAC_PACKET    TCPIP_MAC_PACKET;



// *****************************************************************************
/*  MAC Acknowledge Function

  Summary:
    Prototype of a MAC packet acknowledge function.

  Description:
    This is the prototype of the function that the MAC calls
    once the TX/RX packet processing is done.

  Returns:
    None
  
  Remarks:        
    None.
*/
typedef void    (*TCPIP_MAC_PACKET_ACK_FUNC)(TCPIP_MAC_PACKET* pkt,  const void* param);


// *****************************************************************************
/*  MAC Packet

  Summary:
    A TX/RX packet descriptor.

  Description:
    Structure of a packet transferred with the MAC.
    This is the structure used for both TX and RX.
    See the description of each individual field.

  Remarks:
    Specific TCP/IP stack implementations might offer packet support functions
    to assist in driver development.
    (For the MCHP TCP/IP stack see tcpip_packet.h)
      
    Since the packets may be dynamically allocated, an acknowledge function can result
    in data deallocation (blocking).
    Therefore, the acknowledgment function should NOT be called
    from within an interrupt context.

*/
struct S_tag_TCPIP_MAC_PACKET
{
    /* Multi-packet/queuing support.
       This field is used for chaining/queuing packets. */
    struct S_tag_TCPIP_MAC_PACKET*   next;

    /* Multi-packet/queuing support.
       This field is used for chaining packet fragments, etc. */
    struct S_tag_TCPIP_MAC_PACKET*   pkt_next;

    /* Packet acknowledgment function.
       On TX: A stack module owns the packet.
        Once the MAC is done transmitting the packet (success or failure)
        it has to set an appropriate acknowledge result in the ackRes field ( a TCPIP_MAC_PKT_ACK_RES)
        adjust some packet flags (see TCPIP_MAC_PKT_FLAG_QUEUED) and call the packet acknowledge function (ackFunc).
        This call informs the packet owner that the MAC is done with using that packet.
        It is up to the implementation what action the ackFunc takes: reuse, free, discard the packet or if
        some of the above steps are included in the ackFunc itself.
       On RX: The packet is under the MAC control and it's passed to the stack
        when it contains valid data.
        Once the recipient of the packet (IP, ICMP, UDP, TCP, etc.) is done with the packet processing
        it has to set an appropriate acknowledge result in the ackRes field ( a TCPIP_MAC_PKT_ACK_RES)
        adjust some packet flags (see TCPIP_MAC_PKT_FLAG_QUEUED) and call the packet acknowledge function (ackFunc).
        This call informs the packet owner (the MAC) that the processing of the packet is completed.
        It is up to the implementation what action the ackFunc takes: reuse, free, discard the packet or if
        some of the above steps are included in the ackFunc itself. */
    TCPIP_MAC_PACKET_ACK_FUNC       ackFunc;

    /* Associated acknowledgment parameter to be used when ackFunc is called.
       For TX packets: this is a client supplied parameter and
       is not used by the MAC driver.
       For RX: If the MAC owns the RX packet then the MAC driver can use this field
       for further dispatching in the MAC driver owned acknowledgment function. */
    const void*                     ackParam;

    /* Data (root) segment associated to this packet.
       It is up to the design if the root data segment associated with a packet
       is contiguous in memory with the packet itself.
       It can be 0 if the packet has no associated data. */
    TCPIP_MAC_DATA_SEGMENT*         pDSeg;

    /* Pointer to the MAC frame.
       On TX: the sending higher layer protocol updates this field.
       On RX: the MAC driver updates this field before handing over the packet.
       (MCHP TCP/IP stack note: The packet allocation function update this field automatically). */
    uint8_t*                        pMacLayer;

    /* Pointer to the network layer data.
       On TX: the sending higher layer protocol updates this field.
            The PPP MAC driver needs this field. Other MAC drivers do not use the field.
       On RX: the MAC driver updates this field before handing over the packet.
       (MCHP TCP/IP stack note: The packet allocation function updates this field automatically. But not for IPv6!). */
    uint8_t*                        pNetLayer;

    /* Pointer to the transport layer.
       The MAC driver does not use this field. */
    uint8_t*                        pTransportLayer;

    /* TCPIP_MAC_PACKET_FLAGS associated packet flags.
       On TX: the MAC driver has to set:
         TCPIP_MAC_PKT_FLAG_QUEUED - if the driver needs to queue the packet 

       On RX: the MAC driver updates this field before handing over the packet.
       Flags that the MAC driver has to set/clear:
            TCPIP_MAC_PKT_FLAG_RX
               and TCPIP_MAC_PKT_FLAG_UNICAST, TCPIP_MAC_PKT_FLAG_BCAST and TCPIP_MAC_PKT_FLAG_MCAST
            TCPIP_MAC_PKT_FLAG_QUEUED
            TCPIP_MAC_PKT_FLAG_SPLIT
            TCPIP_MAC_PKT_FLAG_RX_CHKSUM_IP, TCPIP_MAC_PKT_FLAG_RX_CHKSUM_TCP, TCPIP_MAC_PKT_FLAG_RX_CHKSUM_UDP */
    uint32_t                        pktFlags;

    /* Time stamp value. Statistics, info.
       On TX: the sending higher layer protocol updates this field.
            The MAC driver shouldn't need this field
       On RX: the MAC driver updates this field before handing over the packet. */
    uint32_t                        tStamp;

    /* The packet interface.
       On TX: the sending higher layer protocol updates this field.
            The MAC driver doesn't use this field.
       On RX: the receiving higher level protocol updates this value.
            The MAC driver doesn't use this field. */
    const void*                     pktIf;

    /* Total length of the transport layer.
       The MAC driver shouldn't need this field. */
    uint16_t                        totTransportLen;

    /* A TCPIP_MAC_PKT_ACK_RES code associated with the packet.
       On TX: The MAC driver sets this field when calling the packet ackFunc.
       On RX: The higher level protocol which is the recipient of the packet
             sets this field when calling the packet ackFunc. */
    int8_t                          ackRes;

    /* Priority associated with the packet.
       On TX: The MAC driver use this field to transmit packet using priority queues.
       On RX: The MAC driver inform stack about the priority of the packet received */
    uint8_t                         pktPriority;

    /* Packet client data; ignored by the MAC driver.
       It can be used by the packet client modules
       (MCHP TCP/IP stack note: the use of this field is assigned as follows:
            - pktClientData16[0 - 3] used by the bridge module
            - pktClientData16[0] reserved for IPv4 module use 
            - pktClientData16[1] reserved for IPv6 module use 
            - pktClientData16[2, 3] can be used by higher layer modules (IGMP, UDP, etc.) */
    union
    {
        uint16_t            pktClientData16[4];
        uint32_t            pktClientData32[2];
        struct
        {
            uint16_t        ipv4PktData;
            uint16_t        ipv6PktData;
            uint16_t        modPktData;
            uint16_t        extPktData;
        };
    };

    /* Additional client packet payload, variable packet data.
       Ignored by the MAC driver. */
    // uint32_t                        pktClientLoad[];
};


// *****************************************************************************
/*  MAC Result Enumeration

  Summary:
    List of return codes from MAC functions.

  Description:
    This is the list of codes that the MAC uses to specify
    the outcome of a MAC function.
  
  Remarks:        
    Benign operation results - always have positive values.
    Error codes - always have negative values.

*/
typedef enum
{
    /*  operation successful */
    TCPIP_MAC_RES_OK                = 0,   

    /*  operation is pending upon some hardware resource.
        call again to completion */
    TCPIP_MAC_RES_PENDING           = 1,    

    /*  unsupported type */
    TCPIP_MAC_RES_TYPE_ERR          = -1,   

    /*  device is in use */
    TCPIP_MAC_RES_IS_BUSY           = -2,   

    /*  generic initialization failure */
    TCPIP_MAC_RES_INIT_FAIL         = -3,   

    /*  PHY initialization failure */
    TCPIP_MAC_RES_PHY_INIT_FAIL     = -4,   

    /*  Event system initialization failure */
    TCPIP_MAC_RES_EVENT_INIT_FAIL   = -5,   

    /*  unsupported operation */
    TCPIP_MAC_RES_OP_ERR            = -6,   

    /*  memory allocation error */
    TCPIP_MAC_RES_ALLOC_ERR         = -7,   

    /*  not enough descriptors */
    TCPIP_MAC_RES_DCPT_ERR         = -8,   

    /*  already instantiated, initialized error */
    TCPIP_MAC_RES_INSTANCE_ERR      = -9,   

    /*  too fragmented, RX buffer too small */
    TCPIP_MAC_RES_FRAGMENT_ERR      = -10,   

    /*  unsupported/corrupted packet error */
    TCPIP_MAC_RES_PACKET_ERR        = -11,  

    /*  TX queue exceeded the limit */
    TCPIP_MAC_RES_QUEUE_TX_FULL     = -12,  

    /* Synchronization object lock failed */
    /* Could not get a lock */
    TCPIP_MAC_RES_SYNCH_LOCK_FAIL   = -13,  

    /* MAC is not ready for the operation */
    TCPIP_MAC_RES_NOT_READY_ERR     = -14,  

    /* incorrect argument supplied */
    TCPIP_MAC_RES_BAD_ARG           = -15,  

    /* The driver requires TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2
       flag to be set */ 
    TCPIP_MAC_RES_PAYLOAD_OFFSET_ERR = -16,

    /* The size of the MAC gapDcptSize is too small */
    TCPIP_MAC_RES_GAP_SIZE_ERR      = -17,

    /* the packet has too many segments and it is not supported
       The TX was rejected */    
    TCPIP_MAC_RES_PACKET_SEG_ERR    = -18,

    /* No driver object supplied
       This is when the MAC driver itself needs another
       driver for its operation */
    TCPIP_MAC_RES_NO_DRIVER         = -19,

    /* Driver write error.
       The write operation was truncated or otherwise not completed successfully */
    TCPIP_MAC_RES_DRIVER_WRITE_ERR  = -20,

    /* Driver internal error. Should not happen */
    TCPIP_MAC_RES_INTERNAL_ERR      = -21,


}TCPIP_MAC_RES;


// *****************************************************************************
// *****************************************************************************
// Section: MAC interface Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* TCP/IP MAC Action

  Summary:
    Network interface action for initialization/deinitialization

  Description:
    This enumeration defines network interface action for initialization and 
    deinitialization.

  Remarks:
    None.

*/

typedef enum
{
    /*  stack is initialized  */
    TCPIP_MAC_ACTION_INIT,         

    /*  stack is reinitialized */
    TCPIP_MAC_ACTION_REINIT,       

    /*  stack is deinitialized */
    TCPIP_MAC_ACTION_DEINIT,       

    /*  interface is brought up  */
    TCPIP_MAC_ACTION_IF_UP,        

    /*  interface is brought down */
    TCPIP_MAC_ACTION_IF_DOWN,      

}TCPIP_MAC_ACTION;

// *****************************************************************************
/* TCP/IP MAC Power Mode

  Summary:
    Supported MAC power mode state.

  Description:
    List of supported MAC power modes.

  Remarks:
    8 bit values only are supported!

*/

typedef enum
{
    /*  unknown power mode */
    TCPIP_MAC_POWER_NONE,     

    /*  up and running; valid for init/reinit */
    TCPIP_MAC_POWER_FULL,     

    /*  low power mode; valid for init/reinit */
    TCPIP_MAC_POWER_LOW,      

    /*  interface is down */
    TCPIP_MAC_POWER_DOWN,     

}TCPIP_MAC_POWER_MODE;



// *****************************************************************************
/* TCP/IP MAC Event

  Summary:
    Defines the possible MAC event types.

  Description:
    TCP/IP MAC Events Codes.
    
    This enumeration defines all the possible events that can be reported
    by the MAC to the stack.
    
    Depending on the type of the hardware Ethernet/Wi-Fi interface, etc.,
    not all events are possible.

  Remarks:
    16 bits only events are supported.

*/

typedef enum
{
    /*  no event */
    TCPIP_MAC_EV_NONE               = 0x0000U,
    
    /*  RX triggered events: A receive packet is pending */
    TCPIP_MAC_EV_RX_PKTPEND         = 0x0001U,

    /*  RX triggered events: RX FIFO overflow (system level latency, no descriptors, etc.) */
    TCPIP_MAC_EV_RX_OVFLOW          = 0x0002U,

    /*  RX triggered events: no RX descriptor available to receive a new packet */
    TCPIP_MAC_EV_RX_BUFNA           = 0x0004U,

    /*  RX triggered events: There's RX data available */
    TCPIP_MAC_EV_RX_ACT             = 0x0008U,

    /*  RX triggered events: A packet was successfully received */
    TCPIP_MAC_EV_RX_DONE            = 0x0010U,

    /*  RX triggered events: the number of received packets is greater than or equal to than the RX Full Watermark */
    TCPIP_MAC_EV_RX_FWMARK          = 0x0020U,

    /*  RX triggered events: the number of received packets is less than or equal to than the RX Empty Watermark */
    TCPIP_MAC_EV_RX_EWMARK          = 0x0040U,

    /*  RX triggered events: a bus error encountered during an RX transfer */
    TCPIP_MAC_EV_RX_BUSERR          = 0x0080U,

    /*  TX triggered events: A packet was transmitted and its status is available */
    TCPIP_MAC_EV_TX_DONE            = 0x0100U,          

    /*  TX triggered events: a TX packet was aborted by the MAC 
        (jumbo/system underrun/excessive defer/late collision/excessive collisions) */
    TCPIP_MAC_EV_TX_ABORT           = 0x0200U,

    /*  TX triggered events: a bus error encountered during a TX transfer */
    TCPIP_MAC_EV_TX_BUSERR          = 0x0400U,

    /*  Connection triggered events: Connection established */
    TCPIP_MAC_EV_CONN_ESTABLISHED   = 0x0800U,

    /*  Connection triggered events: Connection lost */
    TCPIP_MAC_EV_CONN_LOST          = 0x1000U,

    /*  Useful Masks: all RX related events */
    TCPIP_MAC_EV_RX_ALL             = (TCPIP_MAC_EV_RX_PKTPEND|TCPIP_MAC_EV_RX_OVFLOW|
                                       TCPIP_MAC_EV_RX_BUFNA|TCPIP_MAC_EV_RX_ACT|
                                       TCPIP_MAC_EV_RX_DONE|TCPIP_MAC_EV_RX_FWMARK|
                                       TCPIP_MAC_EV_RX_EWMARK|TCPIP_MAC_EV_RX_BUSERR),

    /*  Useful Masks: all TX related events */
    TCPIP_MAC_EV_TX_ALL            = (TCPIP_MAC_EV_TX_DONE|TCPIP_MAC_EV_TX_ABORT|TCPIP_MAC_EV_TX_BUSERR),

    /*  abnormal traffic/system events: Action should be taken accordingly by the stack (or the stack user) */
    TCPIP_MAC_EV_RXTX_ERRORS        = (TCPIP_MAC_EV_RX_OVFLOW|TCPIP_MAC_EV_RX_BUFNA|
                                       TCPIP_MAC_EV_RX_BUSERR|TCPIP_MAC_EV_TX_ABORT|
                                       TCPIP_MAC_EV_TX_BUSERR),

    /*  Mask of all Connection related events */
    TCPIP_MAC_EV_CONN_ALL            = (TCPIP_MAC_EV_CONN_ESTABLISHED|TCPIP_MAC_EV_CONN_LOST)

}TCPIP_MAC_EVENT;

// *****************************************************************************
/* TCP/IP MAC Synchronization object request

  Summary:
    Defines the possible MAC synchronization request types.

  Description:
    TCP/IP MAC synchronization request codes.
    
    This enumeration defines all the possible synchronization actions that can be
    requested by the MAC to the stack at run time.
    
  Remarks:
    None.

*/

typedef enum
{
    /*  no request */
    TCPIP_MAC_SYNCH_REQUEST_NONE    = 0,
    
    /*  request to create a synchronization object */
    /*  usually a binary semaphore */
    TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE,

    /*  request to delete a previously created synchronization object */
    TCPIP_MAC_SYNCH_REQUEST_OBJ_DELETE,

    /*  request to lock access using a previously created synchronization object */
    TCPIP_MAC_SYNCH_REQUEST_OBJ_LOCK,

    /*  request to unlock/release access using a previously created synchronization object */
    TCPIP_MAC_SYNCH_REQUEST_OBJ_UNLOCK,

    /*  request to enter a critical section */
    TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER,

    /*  request to exit from a critical section */
    TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE,

}TCPIP_MAC_SYNCH_REQUEST;

// *****************************************************************************
/* TCP/IP MAC Checksum calculation offloading

  Summary:
    Defines the possible MAC checksum offloading capabilities.

  Description:
    Lists different TCP/IP layer checksum calculation supported by MAC
    
  Remarks:
    Multiple values can be OR-ed together

    8 bit values only are supported!
*/
typedef enum
{
    /* No IP/TCP/UDP Checksum calculation by MAC driver */
    TCPIP_MAC_CHECKSUM_NONE     = 0x00U,
            
    /* TCP Checksum calculation by MAC driver */        
    TCPIP_MAC_CHECKSUM_TCP      = 0x01U,
            
    /* UDP Checksum calculation by MAC driver */        
    TCPIP_MAC_CHECKSUM_UDP      = 0x02U,
            
    /* IPv4 Checksum calculation by MAC driver */        
    TCPIP_MAC_CHECKSUM_IPV4     = 0x04U,            
            
    /* IPv6 Checksum calculation by MAC driver */        
    TCPIP_MAC_CHECKSUM_IPV6     = 0x08U,             
            
}TCPIP_MAC_CHECKSUM_OFFLOAD_FLAGS;

// *****************************************************************************
/*  Handle to a heap

  Summary:
   A handle used for memory allocation functions.

  Description:
    This is the handle that's required for accessing memory allocation functions.
   
  Remarks:        
    None
*/
typedef const void* TCPIP_MAC_HEAP_HANDLE;

// *****************************************************************************
/*
  Memory Allocation Function:
    typedef void*   (*TCPIP_MAC_HEAP_MallocF)(TCPIP_MAC_HEAP_HANDLE heapH, size_t nBytes);

  Summary:
    MAC allocation function prototype.

  Description:
    This malloc style function is used by the MAC to allocate memory.


  Parameters:
    - heapH       - heap handle to be used in call
    - nBytes      - number of bytes requested

  Returns:
   - valid pointer  - if the allocation request succeeded
   - 0 - if the allocation request failed

  Remarks:   
   The debug version adds the module identifier and source file line number.

*/
typedef void*   (*TCPIP_MAC_HEAP_MallocF)(TCPIP_MAC_HEAP_HANDLE heapH, size_t nBytes);
typedef void*   (*TCPIP_MAC_HEAP_MallocFDbg)(TCPIP_MAC_HEAP_HANDLE heapH, size_t nBytes, 
                                             int moduleId, int lineNo);

// *****************************************************************************
/*
  Memory Allocation Function:
    typedef void*   (*TCPIP_MAC_HEAP_CallocF)(TCPIP_MAC_HEAP_HANDLE heapH, size_t nElems, size_t elemSize);

  Summary:
    MAC allocation function prototype.

  Description:
    This calloc style function is used by the MAC to allocate memory.


  Parameters:
    heapH       - heap handle to be used in call
    nElems      - number of identical elements requested
    elemSize    - size of one element, in bytes

  Returns:
   - valid pointer - if the allocation request succeeded
   - 0 - if the allocation request failed

  Remarks:   
   The debug version adds the module identifier and source file line number.

*/
typedef void*   (*TCPIP_MAC_HEAP_CallocF)(TCPIP_MAC_HEAP_HANDLE heapH, size_t nElems, size_t elemSize);
typedef void*   (*TCPIP_MAC_HEAP_CallocFDbg)(TCPIP_MAC_HEAP_HANDLE heapH, size_t nElems, size_t elemSize, 
                                             int moduleId, int lineNo);

// *****************************************************************************
/*
  Memory Allocation Function:
    typedef size_t     (*TCPIP_MAC_HEAP_FreeF)(TCPIP_MAC_HEAP_HANDLE heapH, const void* pBuff);

  Summary:
    MAC allocation function prototype.

  Description:
    This free style function is used by the MAC to free allocated memory.

  Parameters:
    - heapH       - heap handle to be used in call
    - pBuff       - pointer to memory previously returned by a memory allocation function 

  Returns:
   - Non-zero number - if the free request succeeded
   - 0 - if the free request failed

  Remarks:   
   The debug version adds the module identifier and source file line number.

*/
typedef size_t     (*TCPIP_MAC_HEAP_FreeF)(TCPIP_MAC_HEAP_HANDLE heapH, const void* pBuff);
typedef size_t     (*TCPIP_MAC_HEAP_FreeFDbg)(TCPIP_MAC_HEAP_HANDLE heapH, const void* pBuff, 
                                           int moduleId, int lineNo);


// *****************************************************************************
/*
  Packet Allocation Function:
    typedef TCPIP_MAC_PACKET* (*TCPIP_MAC_PKT_AllocF)(uint16_t pktLen, uint16_t segLoadLen, 
                                TCPIP_MAC_PACKET_FLAGS flags);

  Summary:
    MAC packet allocation function prototype.

  Description:
    This function is used by the MAC to allocate a TCPIP_MAC_PACKET packet.


  Parameters:
    pktLen      - the size of the packet (it will be 32 bits rounded up)
    segLoadLen  - the payload size for the segment associated to this packet.
                  Payload is always 32 bit aligned.
                  If 0 no segment is created/attached to the packet.
   flags        - packet flags

  Returns:
   - Valid packet pointer  - if the allocation request succeeded
   - 0 - if the allocation request failed


  Remarks:   
   The returned allocated packet should always have the TCPIP_MAC_ETHERNET_HEADER 
   added to the packet.
   
   The debug version adds the module identifier.

*/
typedef TCPIP_MAC_PACKET* (*TCPIP_MAC_PKT_AllocF)(uint16_t pktLen, uint16_t segLoadLen, 
                                                  TCPIP_MAC_PACKET_FLAGS flags);
typedef TCPIP_MAC_PACKET* (*TCPIP_MAC_PKT_AllocFDbg)(uint16_t pktLen, uint16_t segLoadLen, 
                                            TCPIP_MAC_PACKET_FLAGS flags, int moduleId);


// *****************************************************************************
/*
  Packet Allocation Function:
    typedef void      (*TCPIP_MAC_PKT_FreeF)(TCPIP_MAC_PACKET* pPkt);

  Summary:
    MAC packet free function prototype.

  Description:
    This function is used by the MAC to free a previously allocated TCPIP_MAC_PACKET packet.

  Parameters:
    pPkt      - pointer to a previously allocated packet.

  Returns:
   None.

  Remarks:   
    The function will free a previously allocated packet.
      However packets or segments marked with TCPIP_MAC_PKT_FLAG_STATIC/TCPIP_MAC_SEG_FLAG_STATIC
      are not freed.
      
    Also note that this function does not free explicitly the external segment payload.
      A payload that was created contiguously when the segment was created
      will be automatically freed by this function.

   The debug version adds the module identifier.

*/
typedef void      (*TCPIP_MAC_PKT_FreeF)(TCPIP_MAC_PACKET* pPkt);
typedef void      (*TCPIP_MAC_PKT_FreeFDbg)(TCPIP_MAC_PACKET* pPkt, int moduleId);

// *****************************************************************************
/*
  Packet Acknowledgment Function:
    typedef void      (*TCPIP_MAC_PKT_AckF)(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, int moduleId);

  Summary:
    MAC packet acknowledge function prototype.

  Description:
    This function is used by the MAC to acknowledge a TCPIP_MAC_PACKET packet
    when the packet processing is completed.

  Parameters:
    pPkt      - pointer to a valid TCPIP_MAC_PACKET packet.
    ackRes    - the result of the packet processing
    moduleId  - the id of the module calling the acknowledge function

  Returns:
   None.

  Remarks:
    A TCPIP_MAC_PACKET packet always has an acknowledgment function.
    This function should clear the TCPIP_MAC_PKT_FLAG_QUEUED flag.
    The packet's ackRes is updated only if the parameter ackRes != TCPIP_MAC_PKT_ACK_NONE.


*/
typedef void      (*TCPIP_MAC_PKT_AckF)(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, int moduleId);


// *****************************************************************************
/*
  Event notification Function:
    typedef void    (*TCPIP_MAC_EventF)(TCPIP_MAC_EVENT event, const void* eventParam);

  Summary:
    MAC event notification handler.

  Description:
    This function describes the MAC event notification handler.
    This is a handler specified by the user of the MAC (the TCP/IP stack).
    The stack can use the handler to be notified of MAC events.
    Whenever a notification occurs the passed events have to be eventually processed:
    - Stack should process the TCPIP_EV_RX_PKTPEND/TCPIP_EV_RX_DONE, TCPIP_EV_TX_DONE  events
    - Process the specific (error) condition
    - Acknowledge the events by calling TCPIP_MAC_EventAcknowledge() so that they can be re-enabled.

  Parameters:
    event        - event that's reported (multiple events can be OR-ed)  
    eventParam   - user parameter that's used in the notification handler
    
  Returns:
   None

  Remarks:   
    The notification handler will be called from the ISR which detects the corresponding event.
      The event notification handler has to be kept as short as possible and non-blocking.
      Mainly useful for RTOS integration where this handler will wake-up a thread that waits for 
      a MAC event to occur.
    The event notification system also enables the user of the TCPIP stack to call into the stack
      for processing only when there are relevant events rather than being forced to periodically call
      from within a loop at unknown moments.
    Without a notification handler the stack user can still call TCPIP_MAC_EventPendingGet to see 
    if processing by the stack needed.
    This is a default way of adding MAC interrupt processing to the TCP/IP stack.
*/
typedef void    (*TCPIP_MAC_EventF)(TCPIP_MAC_EVENT event, const void* eventParam);


// *****************************************************************************
/*
  Synchronization request Function:
    typedef bool    (*TCPIP_MAC_SynchReqF)(void* synchHandle, TCPIP_MAC_SYNCH_REQUEST request);

  Summary:
    MAC synchronization request function definition.

  Description:
    This function describes the MAC synchronization request function.
    The MAC driver will call this function whenever it needs to create, delete, lock or unlock
    a synchronization object.

  Parameters:
    synchHandle:
    - pointer to a valid storage area
    - for TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE it is an address that will store a handle
      to identify the synchronization object to the OS (OSAL).  
    - for other synchronization object request types the handle has to be valid and identify
      the synchronization object to the OS (OSAL).  
    - for Critical sections it maintains OS passed info
    request - a TCPIP_MAC_SYNCH_REQUEST type

  Returns:
   - true     - if the call is successful
   - false    - if the call failed

  Remarks:   
    None.

*/
typedef bool    (*TCPIP_MAC_SynchReqF)(void* synchHandle, TCPIP_MAC_SYNCH_REQUEST request);

// *****************************************************************************
/*
  Retrieve packet Function:
    typedef TCPIP_MAC_PACKET* (*TCPIP_MAC_PKT_RetrieveF)(uint8_t* segBuffer, bool isRx, bool peek);

  Summary:
    Helper function for the MAC driver to retrieve a TCPIP_MAC_PACKET packet belonging to a segment buffer

  Description:
    This function returns a TCPIP_MAC_PACKET pointer corresponding to a segment buffer.
    The user of the MAC driver, the TCP/IP stack, can implement a fast mechanism for maintaining
    the packet <-> buffer association.

    The MAC driver will call the function to retrieve the TCPIP_MAC_PACKET that corresponds 
    to a transmitted/received segment buffer.


  Parameters:
    segBuffer:      - pointer to a TX/RX buffer
    isRx            - boolean specifying if an RX/TX packet is needed
                      This allows implementing different RX/TX mappings/tables
    peek            - boolean to specify if a peek operation is needed
                      If true, the corresponding TCPIP_MAC_PACKET is not removed from the tables
                      Otherwise the call will remove the TCPIP_MAC_PACKET from the tables

  Returns:
   - a valid TCPIP_MAC_PACKET pointer if the call was successful and the corresponding packet was found
   - 0 if the call failed and no packet corresponding to the segment buffer was found

  Remarks:   
    This mechanism is optional and maintaining the TCPIP_MAC_PACKET pointer as part of the
    TCPIP_MAC_SEGMENT_GAP_DCPT is preferred.
    
    If this function is provided, the MAC driver will have to call it to retrieve the TCPIP_MAC_PACKET that corresponds 
    to a transmitted/received segment buffer.

    The Harmony TCP/IP stack does not use this mechanism.

*/
typedef TCPIP_MAC_PACKET* (*TCPIP_MAC_PKT_RetrieveF)(uint8_t* segBuffer, bool isRx, bool peek);


// *****************************************************************************
/*  MAC Types

  Summary:
    List of the MAC types.

  Description:
    List of specific MAC types that indicate to the user of the MAC
    (TCP/IP stack) the actual type of the MAC driver.
  
  Remarks:        
    Other types will be eventually added.

    8 bit value only!
*/
typedef enum
{
    /*  invalid/unknown MAC type */
    TCPIP_MAC_TYPE_NONE     = 0, 

    /*  wired Ethernet MAC type */
    TCPIP_MAC_TYPE_ETH,

    /*  wireless, Wi-Fi type MAC*/
    TCPIP_MAC_TYPE_WLAN,

    /*  PPP/serial MAC type */
    TCPIP_MAC_TYPE_PPP,

    /*  G3 ADP MAC type */
    TCPIP_MAC_TYPE_G3ADP,

    /* supported types */
    TCPIP_MAC_TYPES,

}TCPIP_MAC_TYPE;

// *****************************************************************************
/*  Link MTU per MAC Type

  Summary:
    MTU size corresponding to the MAC types.

  Description:
    Lists the Maximum Transmission Unit corresponding to a MAC type.
  
  Remarks:        
    The MTU is usually set by the standards.
    For special links the values can be updated. 

    The default value is normally used.
    However the individual MAC drivers can be set to use
    other values if needed.
*/
typedef enum
{
    /*  default MTU link */
    TCPIP_MAC_LINK_MTU_DEFAULT  = 1500U, 

    /*  wired Ethernet MAC type */
    TCPIP_MAC_LINK_MTU_ETH      = 1500U,

    /*  wireless, Wi-Fi type MAC*/
    TCPIP_MAC_LINK_MTU_WLAN     = 1500U,

    /*  G3 ADP MAC type */
    TCPIP_MAC_LINK_MTU_G3ADP    = 1280U,

}TCPIP_MAC_LINK_MTU;


// *****************************************************************************
/*  MAC Control Flags

  Summary:
    List of the MAC control flags.

  Description:
    List of specific MAC flags that control the MAC
    functionality.
  
  Remarks:        
    Multiple flags can be "ORed".

    16 bits are reserved for now
*/
typedef enum
{
    /*  No special flag set */
    TCPIP_MAC_CONTROL_FLAG_NONE         = 0x0000U, 

    /* Data payload offset of 2 bytes is required
       This is the 2 bytes offset from the segment data buffer: TCPIP_MAC_DATA_SEGMENT.segBuffer
       to where the payload data actually resides.
       This has currently a value of 0 or 2.
       When this flag is set, then dataOffset == 2
            then the network header layers are properly aligned.
            this should be the default
       Otherwise dataOffset = 0;
       Notes: the Harmony TCP/IP stack requires dataOffset == 2! 
              If the driver does not support the offset value, then it needs to perform
              the buffer adjustment internally!
              
              For extracting the segBuffer from segLoad the MAC driver should use the 
              mask corresponding to the dataOffset:
              TCPIP_MAC_DATA_SEGMENT::segBuffer = TCPIP_MAC_DATA_SEGMENT:: segLoad & dataOffsetMask
              where
                dataOffsetMask == 0xfffffffc if dataOffset == 2
                dataOffsetMask == 0xffffffff if dataOffset == 0
              */
    TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2  = 0x0001U,

    

    /*  The driver should not reuse its dynamically allocated buffers
     *  even when the current number of buffers is below the threshold.
     *  Dynamic buffers should be always freed */
    TCPIP_MAC_CONTROL_NO_SMART_ALLOC    = 0x0002U, 

    /* The driver is not requested to verify the link status and/or
       discard packets when link is down.
       Note: the Harmony stack uses MAC link check */
    TCPIP_MAC_CONTROL_NO_LINK_CHECK     = 0x0004U, 

    /* Other flags eventually added */

}TCPIP_MAC_CONTROL_FLAGS;

// *****************************************************************************
/*  MAC Process Flags

  Summary:
    List of the MAC processing flags.

  Description:
    List of specific MAC processing flags that indicate to the user of the MAC
    (TCP/IP stack) the processing that is expected by the MAC driver.
  
  Remarks:        
    Multiple flags can be "ORed".
*/
typedef enum
{
    /*  the stack never has to call the TCPIP_MAC_Process function */
    TCPIP_MAC_PROCESS_FLAG_NONE     = 0x0000U, 

    /*  the stack has to call the TCPIP_MAC_Process after an RX signal */
    TCPIP_MAC_PROCESS_FLAG_RX       = 0x0001U, 

    /*  the stack has to call the TCPIP_MAC_Process after an TX signal */
    TCPIP_MAC_PROCESS_FLAG_TX       = 0x0002U, 


    /*  the stack has to call the TCPIP_MAC_Process after any type of signal */
    TCPIP_MAC_PROCESS_FLAG_ANY      = 0x0100U, 

}TCPIP_MAC_PROCESS_FLAGS;



// *****************************************************************************
/*  MAC Initialization Data

  Summary:
    Data structure that's passed to the MAC at the initialization time.

  Description:
    This is the data structure that the MAC user (TCP/IP stack)
    passes on to the MAC driver at the MAC initialization time.
    It contains all the data needed for the MAC to initialize itself and
    to start processing packets.
  
  Remarks:        
    Most of the data that's passed in this structure is permanent data.
      It is maintained by the stack for one full session
      i.e., across Initialize() -> DeInitialize() calls.
      
    Some fields are module specific though
      (like the memory allocation handle, allocation functions, etc.)
      that could be different from one module to the other.

*/
typedef struct
{
    /*  malloc type allocation function */
    TCPIP_MAC_HEAP_MallocF  mallocF;

    /*  calloc type allocation function */
    TCPIP_MAC_HEAP_CallocF  callocF;

    /*  free type allocation free function */
    TCPIP_MAC_HEAP_FreeF    freeF;

    /*  handle to be used in the stack allocation service calls */
    TCPIP_MAC_HEAP_HANDLE   memH; 

    /*  packet allocation function */
    TCPIP_MAC_PKT_AllocF    pktAllocF;

    /*  packet free function */
    TCPIP_MAC_PKT_FreeF     pktFreeF;

    /*  packet acknowledge function */
    TCPIP_MAC_PKT_AckF      pktAckF;

    /* Synchronization object request function */
    TCPIP_MAC_SynchReqF     synchF;

    /*  Event notification function. 
        used by the MAC for event reporting. */
    TCPIP_MAC_EventF        eventF;

    /*  Parameter to be used when the event function is called. */
    const void*             eventParam;    

    /* 
    Function to retrieve a TCPIP_MAC_PACKET pointer corresponding to a TX/RX buffer.
    This mechanism is optional and maintaining the TCPIP_MAC_PACKET pointer as part of the
    TCPIP_MAC_SEGMENT_GAP_DCPT is preferred.
    
    If this function is provided, the MAC driver will have to call it to retrieve the TCPIP_MAC_PACKET that corresponds 
    to a transmitted/received segment buffer.
    Otherwise the TCPIP_MAC_SEGMENT_GAP_DCPT mechanism should be used.

    The Harmony TCP/IP stack does not use this mechanism and the retrieveF should == 0. */
    TCPIP_MAC_PKT_RetrieveF retrieveF;

    /*  number of the interfaces supported in this session */
    uint16_t                nIfs;         

    /*  index of the current interface */
    uint16_t                netIx;

    /*  Offset in bytes between the address pointed by segBuffer 
        and the address of the TCPIP_MAC_SEGMENT_GAP_DCPT for this segment buffer is stored.
        This offset has the same value for all packets RX/TX packets passed to the driver.

        The MAC driver can use this offset to restore the TCPIP_MAC_SEGMENT_GAP_DCPT
        and the packet pointer the segment belongs to:
            TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(TCPIP_MAC_DATA_SEGMENT::segBuffer + gapDcptOffset);
            TCPIP_MAC_PACKET* pMacPkt = pGap->segmentPktPtr;


        Note: signed value!
        */
    int16_t                 gapDcptOffset;  

    /*  Size of the TCPIP_MAC_SEGMENT_GAP_DCPT structure (the segmentDataGap size is variable) 
        It specifies the gap space present in the TCPIP_MAC_SEGMENT_GAP_DCPT
        gapDcptSize ==  sizeof(TCPIP_MAC_SEGMENT_GAP_DCPT) == 
        == sizeof(TCPIP_MAC_SEGMENT_GAP_DCPT::segmentPktPtr) + sizeof(TCPIP_MAC_SEGMENT_GAP_DCPT::segmentDataGap)
        
        It has the same value for all packets allocated by the stack.

        This currently is greater or equal than 8 bytes depending on the MAC drivers
        that are part of the build!
        See TCPIP_MAC_SEGMENT_GAP_DCPT.

        For implementations that may work outside the Harmony TCP/IP stack
        it is up to the MAC to check that the value of this offset is enforced.
        */
    uint16_t                gapDcptSize;

    /*  current action for the MAC/stack: TCPIP_MAC_ACTION value*/
    uint8_t                 macAction;

    /*  The power mode for this interface to go to. 
        Valid only if stackAction == init/reinit. 
        Ignored for deinitialize operation.
        TCPIP_MAC_POWER_MODE value */
    uint8_t                 powerMode;

    /* A 16 bit value corresponding to TCPIP_MAC_CONTROL_FLAGS */
    uint16_t                controlFlags;

    /*  Physical address of the interface. 
        MAC sets this field as part of the initialization function. 
        The stack will use this data as the interface address. */
    TCPIP_MAC_ADDR          ifPhyAddress;

}TCPIP_MAC_MODULE_CTRL;

// *****************************************************************************
/* MAC Device Driver Initialization Data

  Summary:
    Contains all the data necessary to initialize the MAC device.

  Description:
    This data structure contains all the data necessary to initialize
    the MAC device.

  Remarks:
    A pointer to a structure of this format containing the desired
    initialization data must be passed into the TCPIP_MAC_Initialize routine.
*/

typedef struct
{
    SYS_MODULE_INIT                 moduleInit;     // System module initialization
    const TCPIP_MAC_MODULE_CTRL*    macControl;     // Stack prepared data 
    const void*                     moduleData;     // Driver specific initialization data
} TCPIP_MAC_INIT;



// *****************************************************************************
/*  MAC Run time parameters

  Summary:
    Data structure that tells the MAC run time parameters.

  Description:
    This is the data structure that the MAC user (TCP/IP stack)
    passes on to the MAC driver after the MAC initialization time
    to retrieve the settings resulted.

  Remarks:        
    None.

*/
typedef struct
{
    /*  Physical address of the interface. 
        MAC sets this field as part of the initialization process. */
    TCPIP_MAC_ADDR          ifPhyAddress;

    /*  MAC process flags. 
        The stack will use this value to call into  
        the MAC process function after receiving a MAC event. */
    TCPIP_MAC_PROCESS_FLAGS processFlags;

    /* MAC type: ETH, Wi-Fi, etc. */
    TCPIP_MAC_TYPE          macType;

    /* MAC link MTU size */
    TCPIP_MAC_LINK_MTU      linkMtu;
    
    /* Rx Checksum offload Enable */
    TCPIP_MAC_CHECKSUM_OFFLOAD_FLAGS    checksumOffloadRx;
    /* Tx Checksum offload Enable */
    TCPIP_MAC_CHECKSUM_OFFLOAD_FLAGS    checksumOffloadTx;
    
    /* number of Tx priorities supported by MAC*/
    uint8_t macTxPrioNum;
    /* number of Rx priorities supported by MAC*/
    uint8_t macRxPrioNum;
}TCPIP_MAC_PARAMETERS;


// *****************************************************************************
/*  MAC RX Statistics Data

  Summary:
    MAC receive statistics data gathered at run time.

  Description:
    This structure defines the RX statistics data for the
    MAC  driver.

  Remarks:        
    This statistics are recorded by the driver, not by the hardware.

*/

typedef struct
{
    /*  number of OK RX packets */
    uint32_t    nRxOkPackets;

    /*  number of unacknowledged pending RX buffers in the driver queues. */
    /*  If each incoming packet fits within a RX buffer (the RX buffer is large enough) */
    /*  than this corresponds to the number of unacknowledged pending RX packets. */
    /*  Otherwise the number of packets is less than the pending buffers. */
    uint32_t    nRxPendBuffers;

    /*  number of currently scheduled RX buffers in the driver queues. */
    /* These are available buffers, ready to receive data */
    uint32_t    nRxSchedBuffers;

    /*  number of RX packets with errors */
    uint32_t    nRxErrorPackets;

    /*  number of RX fragmentation errors */
    uint32_t    nRxFragmentErrors;
    
    /*  number of occurences of 'RX Buffer Not Available' */
    uint32_t    nRxBuffNotAvailable;

}TCPIP_MAC_RX_STATISTICS;

// *****************************************************************************
/*  MAC TX Statistics Data

  Summary:
    MAC transmit statistics data gathered at run time.

  Description:
    This structure defines the TX statistics data for the
    MAC  driver.

  Remarks:        
    This statistics are recorded by the driver, not by the hardware.

*/

typedef struct
{
    /*  number of OK transmitted packets */
    uint32_t    nTxOkPackets;

    /*  number of unacknowledged pending TX buffers in the driver queues. */
    /*  This is equal with pending TX packets when each packet */
    /*  is contained within a TX buffer. */
    uint32_t    nTxPendBuffers;

    /*  number of packets that could not be transmitted */
    uint32_t    nTxErrorPackets;

    /*  number of times the TX queue was full */
    /*  this may signal that the number of TX descriptors is too small */
    uint32_t    nTxQueueFull;

}TCPIP_MAC_TX_STATISTICS;


// *****************************************************************************
/*  MAC Hardware Statistics Register Entry

  Summary:
    Describes a MAC hardware statistics register.

  Description:
    This structure defines an interface for gathering 
    the MAC hardware statistics registers data.

  Remarks:        
    None

*/

typedef struct
{
    /* name of the hardware register */
    char        registerName[36];           

    /*  value of the hardware register */
    uint32_t    registerValue;


}TCPIP_MAC_STATISTICS_REG_ENTRY;




// *****************************************************************************
/*
  MAC Initialize function.
    SYS_MODULE_OBJ       TCPIP_MAC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

  Summary:
    Generic MAC driver initialization function.

  Description:
    This is the function that initializes the MAC.
    It is called by the stack as a result of one interface going up.


  Precondition:
   None.


  Parameters:
    index  - Index for the MAC driver instance to be initialized
    init   - pointer to TCPIP_MAC_INIT initialization data containing:
    - macControl  - Stack prepared data.
    - moduleData  - Driver specific. Dependent on the MAC type.
      For PIC32 MAC driver, the TCPIP_MODULE_MAC_PIC32INT_CONFIG is used. 

  Returns:
    - Valid handle to a driver object - if successful
    - SYS_MODULE_OBJ_INVALID - if initialization failed

 Remarks:
    If this function fails, the stack won't turn up that interface.
    If the operation needs to wait for the hardware, the initialization
      function can return a pending code.

    The returned object must be passed as argument to TCPIP_MAC_Reinitialize,
      TCPIP_MAC_Deinitialize, TCPIP_MAC_Tasks and TCPIP_MAC_Status routines.


*/
SYS_MODULE_OBJ       TCPIP_MAC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/*  
  MAC De-Initialize function.
    void       TCPIP_MAC_Deinitialize(SYS_MODULE_OBJ object);

  Summary:
    Generic MAC driver deinitialization function.

  Description:
    This is the function that deinitializes the MAC.
    It is called by the stack as a result of one interface going down.

  Precondition:
   TCPIP_MAC_Initialize should have been called.

  Parameters:
    object    - Driver object handle, returned from TCPIP_MAC_Initialize

  Returns:
    None.

 Remarks:
    None.

*/
void       TCPIP_MAC_Deinitialize(SYS_MODULE_OBJ object);


// *****************************************************************************
/*  
  MAC Re-Initialize function.
    void       TCPIP_MAC_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);

  Summary:
    Generic MAC driver reinitialization function.

  Description:
    This is the function that re-initializes the MAC.
    It is called by the stack as a result of the system changing power modes.
    Allows reinitialization of the MAC with different power modes, etc.

  Precondition:
   TCPIP_MAC_Initialize should have been called.

  Parameters:
    object - Driver object handle, returned from the TCPIP_MAC_Initialize function

    init   - pointer to TCPIP_MAC_INIT initialization data containing:
    - macControl  - Stack prepared data.
    - moduleData  - Driver specific. Dependent on the MAC type.
      For PIC32 MAC driver, the TCPIP_MODULE_MAC_PIC32INT_CONFIG is used. 

  Returns:
    None.

 Remarks:
    This function is optional and is not currently implemented.

*/
void       TCPIP_MAC_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);

//*************************************************************************
/*
  Function:
    SYS_STATUS TCPIP_MAC_Status ( SYS_MODULE_OBJ object )
    
  Summary:
    Generic function that provides the current status of the MAC driver module.

  Description:
    This function provides the current status of the MAC driver
    module.

  Precondition:
    The TCPIP_MAC_Initialize function must have been called before calling
    this function.

  Parameters:
    object -  Driver object handle, returned from TCPIP_MAC_Initialize

  Returns:
    - SYS_STATUS_READY  - Indicates that any previous module operation for the
                          specified module has completed
    - SYS_STATUS_BUSY   - Indicates that a previous module operation for the
                          specified module has not yet completed
    - SYS_STATUS_ERROR  - Indicates that the specified module is in an error state

  Example:
    <code>
    SYS_MODULE_OBJ      object;     - Returned from TCPIP_MAC_Initialize
    SYS_STATUS          status;
    
    status = TCPIP_MAC_Status(object);
    if (SYS_STATUS_ERROR >= status)
    {
        - Handle error
    }
    </code>

  Remarks:
    Any value greater than SYS_STATUS_READY is also a normal running state
      in which the driver is ready to accept new operations.

    SYS_STATUS_BUSY - Indicates that the driver is busy with a previous
      system level operation and cannot start another

     SYS_STATUS_ERROR - Indicates that the driver is in an error state
     Any value less than SYS_STATUS_ERROR is also an error state.

     SYS_MODULE_DEINITIALIZED - Indicates that the driver has been
      deinitialized

     This operation can be used to determine when any of the driver's
      module level operations has completed.

     If the status operation returns SYS_STATUS_BUSY, then a previous
      operation has not yet completed. Once the status operation returns
      SYS_STATUS_READY, any previous operations have completed.

     The value of SYS_STATUS_ERROR is negative (-1). Any value less than
      that is also an error state.

     This function will NEVER block waiting for hardware.

     If the Status operation returns an error value, the error may be
      cleared by calling the reinitialize operation. If that fails, the
      deinitialize operation will need to be called, followed by the
      initialize operation to return to normal operations.                   
*/

SYS_STATUS TCPIP_MAC_Status ( SYS_MODULE_OBJ object );

//***************************************************************************
/*
  Function:
    void TCPIP_MAC_Tasks( SYS_MODULE_OBJ object )
    
  Summary:
    Generic function: Maintains the MAC driver's state machine.

  Description:
    This function is used to maintain the driver's internal state machine

  Precondition:
    The TCPIP_MAC_Initialize routine must have been called for the
    specified MAC driver instance.

  Parameters:
    object -  Object handle for the specified driver instance (returned from
                TCPIP_MAC_Initialize)
  Returns:
    None.

  Remarks:
    This function is normally not called directly by an application. It is
      called by the system's Tasks routine (SYS_Tasks)

    This function will never block or access any resources that may cause
      it to block.                        

*/

void TCPIP_MAC_Tasks( SYS_MODULE_OBJ object );


// *****************************************************************************
/*  
  Function:
    DRV_HANDLE    TCPIP_MAC_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent );

  Summary:
    Generic MAC driver open function.

  Description:
    This is the function that opens an instance of the MAC.
    Once a MAC client handle is obtained all the MAC functions can be accessed
    using that handle.

  Precondition:
   TCPIP_MAC_Initialize should have been called.

  Parameters:
    drvIndex   - identifier for the driver instance to be opened.
    intent     - Zero or more of the values from the enumeration
                   DRV_IO_INTENT ORed together to indicate the intended use
                   of the driver

  Returns:
    - Valid handle - if the open function succeeded
    - DRV_HANDLE_INVALID - if an error occurs

 Remarks:
    The intent parameter is not used in the current implementation and is maintained 
    only for compatibility with the generic driver Open function signature.

*/
DRV_HANDLE    TCPIP_MAC_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);


// *****************************************************************************
/*  
  Function:
    void       TCPIP_MAC_Close(DRV_HANDLE hMac);

  Summary:
    Generic MAC driver close function.

  Description:
    This is the function that closes an instance of the MAC.
    All per client data is released and the handle can no longer be used
    after this function is called.

  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac   - MAC client handle obtained by a call to TCPIP_MAC_Open.

  Returns:
    None.

 Remarks:
    None.

*/
void       TCPIP_MAC_Close(DRV_HANDLE hMac);


// *****************************************************************************
/*  
  Function:
    TCPIP_MAC_RES       TCPIP_MAC_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);

  Summary:
    Generic MAC driver transmit function.

  Description:
    This is the MAC transmit function.
    Using this function a packet is submitted to the MAC driver for transmission.
       
  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac      - MAC client handle obtained by a call to TCPIP_MAC_Open.

    ptrPacket - pointer to a TCPIP_MAC_PACKET that's completely formatted
                  and ready to be transmitted over the network

  Returns:
    - TCPIP_MAC_RES_OK - if all processing went on OK
    - TCPIP_MAC_RES error code - if processing failed for some reason

 Remarks:
     A success code returned from this function signals only that the 
      packet was successfully scheduled for transmission over the interface
      and not that the packet was actually transmitted.
      An event will be triggered when the packet is transmitted.
      
     The MAC driver has to support internal queuing.
      Since the TCPIP_MAC_PACKET data structure contains internal queuing members
      the MAC can queue the packet at no expense.
      Therefore a packet is to be rejected only if it's not properly formatted.
      Otherwise it has to be scheduled for transmission in an internal MAC queue.
      
     Once the packet is scheduled for transmission the MAC driver has to set
      the TCPIP_MAC_PKT_FLAG_QUEUED flag so that the stack is aware that this
      packet is under processing and cannot be modified.
      
     Once the packet is transmitted, the TCPIP_MAC_PKT_FLAG_QUEUED has to be
      cleared, the proper packet acknowledgment result (ackRes) has to be
      set and the packet acknowledgment function (ackFunc) has to be called.
      It is implementation dependent if all these steps are implemented as
      part of the ackFunc itself or as discrete steps.
      
     On 32-bit machines, the 1st segment payload of a packet is allocated so
      that it is always cache line size aligned and its size is a cache line multiple.
      
     The packet is not required to contain the Frame Check Sequence
      (FCS/CRC32) field. The MAC driver/controller will insert that field
      itself, if it's required.
      
     The MAC driver is required to support the transmission of multiple
      chained packets.                                                       

*/
TCPIP_MAC_RES       TCPIP_MAC_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);

// *****************************************************************************
/* Function:
     TCPIP_MAC_PACKET*   TCPIP_MAC_PacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, 
                                      TCPIP_MAC_PACKET_RX_STAT* pPktStat);

  Summary:
    Generic function: A packet is returned if such a pending packet exists.

  Description:
    This is the MAC receive function.
    
    Once a pending packet is available in the MAC driver internal RX queues
    this function will dequeue the packet and hand it over to the
    MAC driver's client - i.e., the stack - for further processing.

    The flags for a RX packet have to be updated by the MAC driver: TCPIP_MAC_PKT_FLAG_RX 
    has to be set. If the MAC supports it, it should set: 
    - TCPIP_MAC_PKT_FLAG_UNICAST has to be set if that packet is a unicast packet
    - TCPIP_MAC_PKT_FLAG_BCAST has to be set if that packet is a broadcast packet
    - TCPIP_MAC_PKT_FLAG_MCAST has to be set if that packet is a multicast packet
    - TCPIP_MAC_PKT_FLAG_QUEUED has to be set
    - TCPIP_MAC_PKT_FLAG_SPLIT has to be set if the packet has multiple data segments

    Additional information about the packet is available by providing the pRes and
    pPktStat fields.

  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac        - handle identifying the MAC driver client
    pRes        - optional pointer to an address that will receive an additional
                    result associated with the operation.
                    Can be 0 if not needed.
    pPktStat    - optional pointer to an address where the received packet status will be copied
                    Can be 0 if not needed.

  Returns:
    - Valid pointer to an available RX packet
    - 0 if no packet pending/available

  Remarks:
     The MAC driver should dequeue and return to the caller just one single packet, 
      and not multiple chained packets!
      
     Once the higher level layers in the stack are done with processing the RX packet,
      they have to call the corresponding packet acknowledgment function
      that tells the owner of that packet that it can resume control of that packet.
      
     Once the stack modules are done processing the RX packets and the acknowledge function is called
      it is up to the driver design to reuse the RX packets, or simply return them to the pool
      they were allocated from (assuming that some sort of allocation is implemented).
      This document makes no requirement about how the MAC RX packets are obtained,
      using dynamic or static allocation techniques.
      This is up to the design of the MAC.
      
     The MAC driver can use the TCPIP_MAC_Process() for obtaining new RX packets if needed.
    
     Not all the MACs have hardware support for the received packet status.
      If the MAC driver cannot supply the TCPIP_MAC_PACKET_RX_STAT info,
      it should set the *pPktStat to 0.

*/
TCPIP_MAC_PACKET*   TCPIP_MAC_PacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat);

// *****************************************************************************
/* Function:
     TCPIP_MAC_RES       TCPIP_MAC_Process(DRV_HANDLE hMac);

  Summary:
    Generic MAC periodic processing function.

  Description:
    This is a function that allows for internal processing by the MAC driver.
    It is meant for processing that cannot be done from within ISR.

    Normally this function will be called in response to an TX and/or RX event
    signaled by the driver.
    This is specified by the MAC driver using the
    TCPIP_MAC_PARAMETERS::processFlags.

    An alternative approach is that the MAC driver uses a system service
    to create a timer signal that will call the TCPIP_MAC_Process on a periodic basis.

  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac        - handle identifying the MAC driver client

  Returns:
    - TCPIP_MAC_RES_OK - if all processing went on OK
    - TCPIP_MAC_RES error code - if processing failed for some reason

  Remarks:
    Some of the processing that this function is intended for:
    - The MAC driver can process its pending TX queues
          (although it could do that from within the TX ISR)
    - RX buffers replenishing. If the number of packets in the RX queue
          falls below a specified limit, the MAC driver can use this function
          to allocate some extra RX packets.
          Similarly, if there are too many allocated RX packets, the MAC driver
          can free some of them.

*/
TCPIP_MAC_RES       TCPIP_MAC_Process(DRV_HANDLE hMac);

// *****************************************************************************
/* Function:
     TCPIP_MAC_RES     TCPIP_MAC_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);

  Summary:
    Generic MAC parameter get function.

  Description:
    This is a function that returns the run time parameters of the MAC driver.

  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac        - handle identifying the MAC driver client
    pMacParams  - address to store the MAC parameters

  Returns:
    - TCPIP_MAC_RES_OK - if pMacParams updated properly
    - TCPIP_MAC_RES error code - if processing failed for some reason

  Remarks:
    None.

*/
TCPIP_MAC_RES     TCPIP_MAC_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);

// *****************************************************************************
/* Function:
     bool TCPIP_MAC_LinkCheck(DRV_HANDLE hMac);

  Summary:
    Generic MAC link checking function.

  Description:
    This is a function that returns the current status of the link.

  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac        - handle identifying the MAC driver client

  Returns:
    - true  - if the link is up
    - false - if the link is down

  Remarks:
    The stack will call this function periodically.
      Therefore this function is not required to initiate a new PHY transaction
      and to wait for the result.
      It can just initiate a new PHY transaction and return immediately the result of the
      previous transaction. 

*/
bool                TCPIP_MAC_LinkCheck(DRV_HANDLE hMac);


// *****************************************************************************
/* Function:
     bool TCPIP_MAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);

  Summary:
    Generic MAC events report enable/disable function.

  Description:
    This is a function that enables or disables the events to be reported
    to the MAC client (TCP/IP stack).

     All events that are to be enabled will be added to the notification process.
     All events that are to be disabled will be removed from the notification process.
     The stack (or stack user) has to catch the events that are notified and process them.
     After that the stack should call TCPIP_MAC_EventAcknowledge() so that the events can be re-enabled.

  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac        - handle identifying the MAC driver client
    macEvents   - events that the MAC client wants to add/delete for notification
    enable      - if true, the events will  be enabled, else disabled

  Returns:
    - true  - if operation succeeded
    - false - if some error occurred and the operation failed

  Example:
    <code>
    TCPIP_MAC_EventMaskSet( hMac, TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA, true );
    </code>

  Remarks:
     Multiple events can be "ORed" together.
    
     The event notification system enables the user of the MAC and of the stack to call
      into the stack for processing only when there are relevant events rather than
      being forced to periodically call from within a loop.
      
     If the notification events are null the interrupt processing will be disabled.
      Otherwise the event notification will be enabled and the interrupts relating
      to the requested events will be enabled.
      
     Note that once an event has been caught by the MAC and reported through the notification handler
      it may be disabled until the TCPIP_MAC_EventAcknowledge is called.

*/
bool                    TCPIP_MAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);

// *****************************************************************************
/* Function:
     bool TCPIP_MAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents);

  Summary:
     Generic function: This function acknowledges a previously reported MAC event.

  Description:
    This function acknowledges and re-enables processed events.
    Multiple events can be "ORed" together as they are processed together.
    The events acknowledged by this function should be the events that have
    been retrieved from the MAC by calling TCPIP_MAC_EventPendingGet or have
    been passed to the MAC client by using the notification handler.
    Once the events are acknowledged they will be re-enabled.

  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac        - handle identifying the MAC driver client
    macEvents   - MAC events that are acknowledged and re-enabled

  Returns:
    - true  - if events acknowledged
    - false - if no events to be acknowledged

  Example:
    <code>
    TCPIP_MAC_EventAcknowledge( hMac, newMacEvents );
    </code>

  Remarks:
     All events should be acknowledged, in order to be re-enabled.
    
     Some events are fatal errors and should not be acknowledged
      (TCPIP_MAC_EV_RX_BUSERR, TCPIP_MAC_EV_TX_BUSERR).
      Stack re-initialization is needed under such circumstances.
      
     Some events are just system/application behavior and they are intended only as simple info
      (TCPIP_MAC_EV_RX_OVFLOW, TCPIP_MAC_EV_RX_BUFNA, TCPIP_MAC_EV_TX_ABORT, TCPIP_MAC_EV_RX_ACT).
      
     The TCPIP_MAC_EV_RX_FWMARK and TCPIP_MAC_EV_RX_EWMARK events are part of the normal flow control operation
      (if auto flow control was enabled).
      They should be enabled alternatively, if needed.
      
     The events are persistent. They shouldn't be re-enabled unless they have been processed
      and the condition that generated them was removed.
      Re-enabling them immediately without proper processing will have dramatic
      effects on the system performance.

*/
bool                    TCPIP_MAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents);

// *****************************************************************************
/* Function:
     TCPIP_MAC_EVENT TCPIP_MAC_EventPendingGet(DRV_HANDLE hMac);

  Summary:
    Generic function: Returns the currently pending MAC events.

  Description:
    This function returns the currently MAC pending events.
    Multiple events will be "ORed" together as they accumulate.
    MAC processing is needed whenever a transmission related event is present
    (TCPIP_MAC_EV_RX_PKTPEND, TCPIP_MAC_EV_TX_DONE).
    Other, non critical events, may be passed to an user for informational purposes.
    All events have to be eventually acknowledged if re-enabling is needed.

  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac        - handle identifying the MAC driver client

  Returns:
    The currently stack pending events.

  Example:
    <code>
    TCPIP_MAC_EVENT currEvents = TCPIP_MAC_EventPendingGet( hMac);
    </code>

  Remarks:
     This is the preferred method to get the current pending MAC events.
      Even with a notification handler in place it's better to use this function
      to get the current pending events rather than using the events passed by the
      notification handler which could be stale.
      
     The returned value is just a momentary value. The pending events can change any time.

*/
TCPIP_MAC_EVENT         TCPIP_MAC_EventPendingGet(DRV_HANDLE hMac);


// *****************************************************************************
/* Function:
     TCPIP_MAC_RES TCPIP_MAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr);

  Summary:
    Generic function: Sets the current MAC hash table receive filter.

  Description:
    This function sets the MAC hash table filtering to allow 
    packets sent to DestMACAddr to be received.
    It calculates a CRC-32 using polynomial 0x4C11DB7 over the 6 byte MAC address
    and then, using bits 28:23 of the CRC, will set the appropriate bits in
    the hash table filter registers.

    The function will enable/disable the Hash Table receive filter if needed.

  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac        - handle identifying the MAC driver client
    DestMACAddr - destination MAC address (6 bytes) to allow 
                    through the Hash Table Filter.
                    If DestMACAddr is set to 00-00-00-00-00-00,
                    then the hash table will be cleared of all entries
                    and the filter will be disabled.

  Returns:
    - TCPIP_MAC_RES_OK - if all processing went on OK.
    - TCPIP_MAC_RES error code - if function failed for some reason.

  Remarks:
     There is no way to individually remove destination MAC 
      addresses from the hash table since it is possible to have 
      a hash collision and therefore multiple MAC addresses 
      relying on the same hash table bit.
      
     A workaround is to have the stack store each enabled MAC address
      and to perform the comparison at run time.
      
     A call to TCPIP_MAC_RxFilterHashTableEntrySet() using a 
      00-00-00-00-00-00 destination MAC address, which will clear 
      the entire hash table and disable the hash table filter.  
      This will allow the receive of all packets, regardless of their destination.

*/
TCPIP_MAC_RES        TCPIP_MAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr);

// *****************************************************************************
/*  Function:
     TCPIP_MAC_RES TCPIP_MAC_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, 
                                           TCPIP_MAC_TX_STATISTICS* pTxStatistics);

  Summary:
    Generic function: Gets the current MAC statistics.

  Description:
    This function will get the current value of the statistic counters
    maintained by the MAC driver.

  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac          - handle identifying the MAC driver client
    pRxStatistics - pointer to a TCPIP_MAC_RX_STATISTICS that will receive the current 
                      RX statistics counters. Can be NULL if not needed.
    pTxStatistics - pointer to a TCPIP_MAC_TX_STATISTICS that will receive the current 
                      TX statistics counters. Can be NULL if not needed.

  Returns:
    - TCPIP_MAC_RES_OK - if all processing went on OK
    - TCPIP_MAC_RES_OP_ERR error code - if function not supported by the driver

  Remarks:
    The reported values are info only and change dynamically.

*/
TCPIP_MAC_RES        TCPIP_MAC_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, 
                                     TCPIP_MAC_TX_STATISTICS* pTxStatistics);
    
// *****************************************************************************
/*  Function:
     TCPIP_MAC_RES        TCPIP_MAC_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* 
                                     pRegEntries, size_t nEntries, size_t* pHwEntries);

  Summary:
    Generic function: Gets the current MAC hardware statistics registers.

  Description:
    This function will get the current value of the statistic registers
    maintained by the MAC hardware.

  Precondition:
   TCPIP_MAC_Initialize should have been called.
   TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
    hMac           - handle identifying the MAC driver client
    pRegStatistics - pointer to a pRegEntries that will receive the current 
                       hardware statistics registers values.
                       Can be 0, if only the number of supported hardware registers is requested
    nEntries       - provides the number of TCPIP_MAC_STATISTICS_REG_ENTRY structures present in pRegEntries
                       Can be 0, if only the number of supported hardware registers is requested
                       The register entries structures will be filled by the driver, up to the
                       supported hardware registers.
    pHwEntries    - pointer to an address to store the number of the statistics registers that the hardware supports
                      It is updated by the driver.
                      Can be NULL if not needed

  Returns:
    - TCPIP_MAC_RES_OK - if all processing went on OK
    - TCPIP_MAC_RES_OP_ERR error code - if function not supported by the driver

  Remarks:
    The reported values are info only and change dynamically.

*/
TCPIP_MAC_RES        TCPIP_MAC_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* 
                                               pRegEntries, size_t nEntries, size_t* pHwEntries);

//*****************************************************************************
/*
  Function:
       size_t TCPIP_MAC_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, 
       size_t* pConfigSize);
    
  Summary:
    Generic function: Gets the current MAC driver configuration.

  Description:
    This function will get the current MAC driver configuration and store it into
    a supplied buffer.

  Precondition:
    TCPIP_MAC_Initialize must have been called to set up the driver.
    TCPIP_MAC_Open should have been called to obtain a valid handle.

  Parameters:
     hMac          - handle identifying the MAC driver client
     configBuff    - pointer to a buffer to store the configuration. Can be NULL if not needed.
     buffSize      - size of the supplied buffer
     pConfigSize   - address to store the number of bytes needed for the storage of the 
                     MAC configuration. Can be NULL if not needed.
                
  Returns:
    The number of bytes copied into the supplied storage buffer

  Remarks:
    None.

*/
size_t TCPIP_MAC_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END
    
#endif // H_TCPIP_MAC_H_
