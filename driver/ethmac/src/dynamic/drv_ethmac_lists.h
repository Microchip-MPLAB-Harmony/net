/*
Copyright (C) 2008-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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



#ifndef H_DRV_ETHMAC_LISTS_H_
#define H_DRV_ETHMAC_LISTS_H_

#include <sys/kmem.h>
#include <stdint.h>

// *****************************************************************************
/*  Packet Descriptor

  Summary:
    Descriptor of a packet accepted by the TX/RX Ethernet engine.

  Description:
    A packet handled by the Ethernet TX/RX engine is a list of buffer
    descriptors.  A packet consists of multiple buffers and each buffer needs a
    descriptor.  Although the number of buffers per packet is not limited, note
    that the hardware overhead is higher when many buffers have to be handled
    for one packet.  The list ends when the next field is NULL or when the pBuff
    is NULL.

    NOTE: definition matches the tcpip_mac.h::TCPIP_MAC_DATA_SEGMENT!
*/

typedef struct S_TAG_DRV_ETHMAC_PKT_DCPT
{
    // Next descriptor in chain. NULL to end
    struct S_TAG_DRV_ETHMAC_PKT_DCPT  *next;

    // segment buffer. Not used
    void                    *segBuffer;

    // Buffer to be transmitted
    void                   *pBuff;

    // Number of bytes in the buffer 0-2047 allowed
    uint16_t                nBytes;

} /*DOM-IGNORE-BEGIN*/ __attribute__ ((__packed__)) /*DOM-IGNORE-END*/ DRV_ETHMAC_PKT_DCPT;


// *****************************************************************************
/*  Transmitted Packet Status

  Summary:
    Status of a transmitted packet.

  Description:
    This structure contains the status of a transmitted packet.

  Notes:
    Status is always two "words" (64-bits) long.
*/

typedef union __attribute__ ((__packed__))
{
    struct
    {
        // Total bytes transmitted
        unsigned int        totTxBytes   :16;

        // Control frame transmitted
        unsigned int         txCtrl      : 1;

        // Pause control frame transmitted
        unsigned int         txPause     : 1;

        // Transmit backpressure applied
        unsigned int         txBPres     : 1;

        // Transmit VLAN tagged frame
        unsigned int         txVLAN      : 1;

        unsigned int                     : 12;

        // Transmit bytes count
        unsigned int         bCount      :16;

        // Transmit collision count
        unsigned int        collCount   : 4;

        // Transmit CRC error
        unsigned int         crcError    : 1;

        // Transmit length check error
        unsigned int         lenError    : 1;

        // Transmit length out of range
        unsigned int         lenRange    : 1;

        // Transmit done
        unsigned int         txDone      : 1;

        // Transmit multicast
        unsigned int         mcast       : 1;

        // Transmit broadcast
        unsigned int         bcast       : 1;

        // Transmit packet defer
        unsigned int         defer       : 1;

        // Transmit excessive packet defer
        unsigned int         excDefer    : 1;

        // Transmit maximum collision
        unsigned int         maxColl     : 1;

        // Transmit late collision
        unsigned int         lateColl    : 1;

        // Transmit giant frame (set when pktSz>MaxFrameSz && HugeFrameEn==0)
        uint32_t         giant       : 1;

        // Transmit underrun
        uint32_t         underrun    : 1;
    }__attribute__ ((__packed__));

    // Status is 2 words always
    uint64_t            w64;
    uint32_t            w32[2];

} DRV_ETHMAC_PKT_STAT_TX;


// *****************************************************************************
/*  Received Packet Status

  Summary:
    Status of a received packet.

  Description:
    This structure contains the status of a received packet.

  Notes:
    Status is always two "words" (64-bits) long.
*/

typedef union __attribute__ ((__packed__))
{
    struct 
    {
        // Packet payload checksum
        unsigned int        pktChecksum     :16;

        unsigned int                        : 8;

        // Runt packet received
        unsigned int        runtPkt         : 1;

        // Unicast, not me packet,
        unsigned int        notMeUcast      : 1;

        // Hash table match
        unsigned int        htMatch         : 1;

        // Magic packet match
        unsigned int        magicMatch      : 1;

        // Pattern match match
        unsigned int        pmMatch         : 1;

        // Unicast match
        unsigned int        uMatch          : 1;

        // Broadcast match
        unsigned int        bMatch          : 1;

        // Multicast match
        unsigned int        mMatch          : 1;

        // Received bytes
        unsigned int        rxBytes         :16;

        // Packet previously ignored
        unsigned int        prevIgnore      : 1;

        // RX data valid event previously seen
        unsigned int        prevDV          : 1;

        // Carrier event previously seen
        unsigned int        prevCarrier     : 1;

        // RX code violation
        unsigned int        rxCodeViol      : 1;

        // CRC error in packet
        unsigned int        crcError        : 1;

        // Receive length check error
        unsigned int        lenError        : 1;

        // Receive length out of range
        unsigned int        lenRange        : 1;

        // Receive OK
        unsigned int        rxOk            : 1;

        // Multicast packet
        unsigned int        mcast           : 1;

        // Broadcast packet
        unsigned int        bcast           : 1;

        // Dribble nibble
        unsigned int        dribble         : 1;

        // Control frame received
        unsigned int        rxCtrl          : 1;

        // Pause control frame received
        unsigned int        rxPause         : 1;

        // Received unsupported code
        unsigned int        rxCodeErr       : 1;

        // Received VLAN tagged frame
        unsigned int        rxVLAN          : 1;

        unsigned int                        : 1;

    }__attribute__ ((__packed__));

    // Status is 2 words always
    uint64_t            w64;
    uint32_t            w32[2];

} DRV_ETHMAC_PKT_STAT_RX;


// descriptors

typedef union
{
    struct 
    {
        unsigned int        : 7;
        unsigned int EOWN   : 1;
        unsigned int NPV    : 1;
        unsigned int sticky : 1;    // a receive buffer is sticky
        unsigned int kv0    : 1;    // belongs to k0. else k1
        unsigned int rx_wack: 1;    // RX buffer waiting for acknowledge
        unsigned int rx_nack: 1;    // RX descriptor is not acknowledged
        unsigned int        : 3;
        unsigned int bCount : 11;
        unsigned int        : 3;
        unsigned int EOP    : 1;
        unsigned int SOP    : 1;
    }__attribute__ ((__packed__));
    uint32_t        w;
}DRV_ETHMAC_DCPT_HDR;  // descriptor header

#define SDCPT_HDR_EOWN_MASK_       0x00000080UL
#define SDCPT_HDR_NPV_MASK_        0x00000100UL
#define SDCPT_HDR_STICKY_MASK_     0x00000200UL
#define SDCPT_HDR_KV0_MASK_        0x00000400UL
#define SDCPT_HDR_RX_WACK_MASK_    0x00000800UL
#define SDCPT_HDR_BCOUNT_MASK_     0x07ff0000UL
#define SDCPT_HDR_BCOUNT_POS_      16U
#define SDCPT_HDR_EOP_MASK_        0x40000000UL
#define SDCPT_HDR_SOP_MASK_        0x80000000UL


typedef struct
{
    volatile DRV_ETHMAC_DCPT_HDR       hdr;        // header
    uint8_t*                pEDBuff;    // data buffer address
    volatile uint64_t       stat;       // TX/RX packet status
    uint32_t                next_ed;    // next descriptor (hdr.NPV==1);
}__attribute__ ((__packed__)) DRV_ETHMAC_HW_DCPT;   // hardware TX/RX descriptor (linked).


typedef struct __attribute__ ((__packed__))
{
    volatile DRV_ETHMAC_DCPT_HDR       hdr;        // header
    uint8_t*                pEDBuff;    // data buffer address
    volatile DRV_ETHMAC_PKT_STAT_TX  stat;       // transmit packet status
    uint32_t                next_ed;    // next descriptor (hdr.NPV==1);
} DRV_ETHMAC_HW_DCPT_TX; // hardware TX descriptor (linked).


typedef struct __attribute__ ((__packed__))
{
    volatile DRV_ETHMAC_DCPT_HDR       hdr;        // header
    uint8_t*                pEDBuff;    // data buffer address
    volatile DRV_ETHMAC_PKT_STAT_RX  stat;       // transmit packet status
    uint32_t                next_ed;    // next descriptor (hdr.NPV==1);
} DRV_ETHMAC_HW_DCPT_RX; // hardware Rx descriptor (linked).



typedef struct S_TAG_DRV_ETHMAC_DCPT_NODE
{
    struct S_TAG_DRV_ETHMAC_DCPT_NODE*      next;       // next pointer in virtual space
    DRV_ETHMAC_HW_DCPT          hwDcpt;     // the associated hardware descriptor
}DRV_ETHMAC_DCPT_NODE; // Ethernet descriptor node: generic linked descriptor for both TX/RX.


typedef struct
{
    struct S_TAG_DRV_ETHMAC_DCPT_NODE*      next;       // next pointer in virtual space
    DRV_ETHMAC_HW_DCPT_TX            hwDcpt;     // the associated hardware descriptor
}DRV_ETHMAC_DCPT_NODE_TX;   // TX linked descriptor


typedef struct
{
    struct S_TAG_DRV_ETHMAC_DCPT_NODE*      next;       // next pointer in virtual space
    DRV_ETHMAC_HW_DCPT_RX            hwDcpt;     // the associated hardware descriptor
}DRV_ETHMAC_DCPT_NODE_RX;   // Rx linked descriptor

// although there could be unlinked TX and RX descriptors (hdr.NPV==0), we don't use them in this implementation


#if defined(__PIC32MZ__)
// for PIC32MZ these lists have to be aligned on a cache line
#define DRV_ETHMAC_DCPT_LIST_ALIGN      16
typedef struct __attribute__((aligned(16)))
{
    uint64_t    pad;
    DRV_ETHMAC_DCPT_NODE*  head;   // list head
    DRV_ETHMAC_DCPT_NODE*  tail;
}DRV_ETHMAC_DCPT_LIST;  // single linked list
#else
// for PIC32MX alignment is 1 byte
#define DRV_ETHMAC_DCPT_LIST_ALIGN      1
typedef struct
{
    DRV_ETHMAC_DCPT_NODE*  head;   // list head
    DRV_ETHMAC_DCPT_NODE*  tail;
}DRV_ETHMAC_DCPT_LIST;  // single linked list
#endif  // defined(__PIC32MZ__)

/////  single linked lists manipulation ///////////
//
#define     DRV_ETHMAC_LIB_ListIsEmpty(pL)       ((pL)->head == NULL)

static __inline__ int __attribute__((always_inline)) DRV_ETHMAC_LIB_ListCount(DRV_ETHMAC_DCPT_LIST* pL)
{

    DRV_ETHMAC_DCPT_NODE*  pN;
    int     nNodes=0;
    for(pN=pL->head; (pN != NULL); pN=pN->next)
    {
        nNodes++;
    }
    return nNodes;
}


static __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_ListAddHead(DRV_ETHMAC_DCPT_LIST* pL, DRV_ETHMAC_DCPT_NODE* pN)
{
    pN->next=pL->head;
    pN->hwDcpt.next_ed=KVA_TO_PA(&pL->head->hwDcpt);
    pL->head=pN;
    if(pL->tail == NULL)
    {  // empty list
        pL->tail=pN;
    }

}


static __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_ListAddTail(DRV_ETHMAC_DCPT_LIST* pL, DRV_ETHMAC_DCPT_NODE* pN)
{
    pN->next = NULL;
    pN->hwDcpt.next_ed = 0;
    if(pL->tail == NULL)
    {
        pL->head = pN;
        pL->tail = pN;
    }
    else
    {
        pL->tail->next=pN;
        pL->tail->hwDcpt.next_ed=KVA_TO_PA(&pN->hwDcpt);
        pL->tail=pN;
    }

}


// insertion in the middle, not head or tail
#define     SlAddMid(pL, pN, after) do{ (pN)->next=(after)->next; (pN)->next_ed=(after)->next_ed; \
                            (after)->next=(pN); (after)->next_ed=KVA_TO_PA(&pN->hwDcpt); \
                        }while(0)


static __inline__ DRV_ETHMAC_DCPT_NODE* __attribute__((always_inline)) DRV_ETHMAC_LIB_ListRemoveHead(DRV_ETHMAC_DCPT_LIST* pL)
{
    DRV_ETHMAC_DCPT_NODE* pN=pL->head;
    if(pL->head==pL->tail)
    {
        pL->head = NULL;
        pL->tail = NULL;
    }
    else
    {
        pL->head=pN->next;
    }

    return pN;
}


static __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_ListAppendTail(DRV_ETHMAC_DCPT_LIST* pL, DRV_ETHMAC_DCPT_LIST* pAList)
{
    DRV_ETHMAC_DCPT_NODE* pN;
    while((pN=DRV_ETHMAC_LIB_ListRemoveHead(pAList)) != NULL)
    {
        DRV_ETHMAC_LIB_ListAddTail(pL, pN);
    }
}


#if defined(__PIC32MZ__)
// flushes a data cache line/address
static __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_DataLineFlush(void* address)
{   // issue a hit-writeback invalidate order
    __asm__ __volatile__ ("cache 0x15, 0(%0)" ::"r"(address));
    __asm__ __volatile__ ("sync");
}
#endif  // defined(__PIC32MZ__)


#if defined(__PIC32MZ__)
/* MISRA C-2012 Rule 11.6 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_11_6_NET_DR_9 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 11.6" "H3_MISRAC_2012_R_11_6_NET_DR_9" 
static __inline__ DRV_ETHMAC_DCPT_LIST* __attribute__((always_inline)) DRV_ETHMAC_LIB_ListInit(DRV_ETHMAC_DCPT_LIST* pL)
{
    DRV_ETHMAC_LIB_DataLineFlush(pL);
    DRV_ETHMAC_DCPT_LIST* pNL = (DRV_ETHMAC_DCPT_LIST*)KVA0_TO_KVA1(pL);

    pNL->head = NULL;
    pNL->tail = NULL;
    return pNL;
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 11.6"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

#else
static __inline__ DRV_ETHMAC_DCPT_LIST* __attribute__((always_inline)) DRV_ETHMAC_LIB_ListInit(DRV_ETHMAC_DCPT_LIST* pL)
{
    DRV_ETHMAC_DCPT_LIST* pNL = pL;

    pNL->head = NULL;
    pNL->tail = NULL;
    return pNL;
}
#endif


/////  generic single linked lists manipulation ///////////
//
//
typedef struct S_TAG_DRV_ETHMAC_SGL_LIST_NODE
{
    struct S_TAG_DRV_ETHMAC_SGL_LIST_NODE*   next;       // next node in list
    void*                                   data;     // generic payload    
}DRV_ETHMAC_SGL_LIST_NODE;  // generic linked list node definition


typedef struct
{
    DRV_ETHMAC_SGL_LIST_NODE*   head;   // list head
    DRV_ETHMAC_SGL_LIST_NODE*   tail;
    int             nNodes; // number of nodes in the list

}DRV_ETHMAC_SGL_LIST;   // single linked list

static __inline__ void  __attribute__((always_inline)) DRV_ETHMAC_SingleListInitialize(DRV_ETHMAC_SGL_LIST* pL)
{
    pL->head = NULL;
    pL->tail = NULL;
    pL->nNodes = 0;
}



static __inline__ int __attribute__((always_inline)) DRV_ETHMAC_SingleListCount(DRV_ETHMAC_SGL_LIST* pL)
{
    return pL->nNodes;
}

// removes the head node
DRV_ETHMAC_SGL_LIST_NODE*  DRV_ETHMAC_SingleListHeadRemove(DRV_ETHMAC_SGL_LIST* pL);

// add tail
void  DRV_ETHMAC_SingleListTailAdd(DRV_ETHMAC_SGL_LIST* pL, DRV_ETHMAC_SGL_LIST_NODE* pN);

// append a list
void  DRV_ETHMAC_SingleListAppend(DRV_ETHMAC_SGL_LIST* pDstL, DRV_ETHMAC_SGL_LIST* pAList);

#endif //  H_DRV_ETHMAC_LISTS_H_
