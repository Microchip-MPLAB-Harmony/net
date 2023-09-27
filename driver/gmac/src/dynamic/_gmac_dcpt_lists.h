/*
Copyright (C) 2008-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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



#ifndef __GMAC_DCPT_LISTS_H_
#define __GMAC_DCPT_LISTS_H_
#include "tcpip/tcpip_mac.h"
// *****************************************************************************
/*  GMAC Queue List
 
    Summary: List of Multiple Queues supported by GMAC    
*/
typedef enum  
{
    GMAC_QUE_0 = 0,
    GMAC_QUE_1 = 1,
    GMAC_QUE_2 = 2,
    GMAC_QUE_3 = 3,
    GMAC_QUE_4 = 4,
    GMAC_QUE_5 = 5
} GMAC_QUE_LIST;

#define GMAC_QUE0_MASK      (1<<GMAC_QUE_0)
#define GMAC_QUE1_MASK      (1<<GMAC_QUE_1)
#define GMAC_QUE2_MASK      (1<<GMAC_QUE_2)
#define GMAC_QUE3_MASK      (1<<GMAC_QUE_3)
#define GMAC_QUE4_MASK      (1<<GMAC_QUE_4)
#define GMAC_QUE5_MASK      (1<<GMAC_QUE_5)

#define GMAC_ALL_QUE_MASK (GMAC_QUE0_MASK | GMAC_QUE1_MASK | GMAC_QUE2_MASK     \
                            | GMAC_QUE3_MASK | GMAC_QUE4_MASK | GMAC_QUE5_MASK)

// *****************************************************************************
/*  TX Packet Descriptor Status Word

  Summary:
    Status word with each Transmit Descriptor

  Description:
    The transmit descriptor consist of two 32-bit word; one is Transmit Status
    word. The status of a packet transmission is stored in this word by GMAC
*/

typedef union
{
    uint32_t val;                   /**< 32-Bit access */
    struct _GmacTxStatusBM 
    {
        uint32_t len: 11,           /**< Length of buffer */
                    reserved: 4,
                    bLastBuffer: 1, /**< Last buffer (in the current frame) */
                    bNoCRC: 1,      /**< No CRC */
                    reserved1: 10,
                    bExhausted: 1,  /**< Buffer exhausted in mid frame */
                    bUnderrun: 1,   /**< Transmit under run */
                    bError: 1,      /**< Retry limit exceeded, error detected */
                    bWrap: 1,       /**< Marks last descriptor in TD list */
                    bUsed: 1;       /**< User clear, GMAC sets this once a frame
                                        has been successfully transmitted */
    } bm;
} DRV_GMAC_TXDCPT_STATUS;

// *****************************************************************************
/*  RX Packet Descriptor Address Word

  Summary:
    Address word with each Receive Descriptor

  Description:
    The Receive descriptor consist of two 32-bit word; one is Receive address
    word. The Rx buffer address , the Ownership bit and Wrap bit stored in this 
    word
*/
typedef union
{
    uint32_t val;
    struct _GmacRxAddrBM 
    {
        uint32_t    bOwnership: 1, /**< User clear, GMAC set this to one once
                                    it has successfully written a frame to
                                        memory */
                    bWrap: 1,       /**< Marks last descriptor in receive buffer */
                    addrDW: 30;     /**< Address in number of DW */
    } bm;
} DRV_GMAC_RXDCPT_ADDR;             /**< Address, Wrap & Ownership */

// *****************************************************************************
/*  Receive Descriptor

  Summary:
    RX descriptor for control and status of GMAC RX block

  Description:
    The Receive descriptor consist of two 32-bit word; one is Receive buffer 
    address and other for Rx status
*/
typedef struct
{
    volatile DRV_GMAC_RXDCPT_ADDR rx_desc_buffaddr;
    volatile TCPIP_MAC_PACKET_RX_STAT_PIC32C rx_desc_status;
    
} DRV_PIC32CGMAC_HW_RXDCPT; 

// *****************************************************************************
/*  Transmit Descriptor

  Summary:
    TX descriptor for control and status of GMAC Transmit block

  Description:
    The Transmit descriptor consist of two 32-bit word; one is Transmit buffer
    address  and other for Tx status
*/
typedef struct
{
    volatile uint32_t tx_desc_buffaddr;
    volatile DRV_GMAC_TXDCPT_STATUS tx_desc_status; 
} DRV_PIC32CGMAC_HW_TXDCPT;   


////////////////////////////////////////////////////////////////////////////////
//////////////  GMAC Single linked lists manipulation //////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct _TAG_DRV_PIC32CGMAC_SGL_LIST_NODE
{
    struct _TAG_DRV_PIC32CGMAC_SGL_LIST_NODE*   next;       /**< next linked list  */      
    void*                                       data[];     /**< generic payload */
}DRV_PIC32CGMAC_SGL_LIST_NODE;                  /**< Single list for GMAC packet */ 


typedef struct
{
    DRV_PIC32CGMAC_SGL_LIST_NODE*   head;   /**< Linked list head  */ 
    DRV_PIC32CGMAC_SGL_LIST_NODE*   tail;   /**< Linked list tail  */ 
    int             nNodes;                 /**< number of nodes in the list */ 

}DRV_PIC32CGMAC_SGL_LIST;   // single linked list

//Initialize Single List
static  __inline__ void  __attribute__((always_inline)) DRV_PIC32CGMAC_SingleListInitialize(DRV_PIC32CGMAC_SGL_LIST* pL)
{
    pL->head = pL->tail = 0;
    pL->nNodes = 0;
}

// Return number of nodes in list
static  __inline__ int __attribute__((always_inline)) DRV_PIC32CGMAC_SingleListCount(DRV_PIC32CGMAC_SGL_LIST* pL)
{
    return pL->nNodes;
}

// Removes the head node from list
DRV_PIC32CGMAC_SGL_LIST_NODE*  DRV_PIC32CGMAC_SingleListHeadRemove(DRV_PIC32CGMAC_SGL_LIST* pL);

// Add a node to the tail of list
void  DRV_PIC32CGMAC_SingleListTailAdd(DRV_PIC32CGMAC_SGL_LIST* pL, DRV_PIC32CGMAC_SGL_LIST_NODE* pN);

// Add a node to the head of list
void  DRV_PIC32CGMAC_SingleListHeadAdd(DRV_PIC32CGMAC_SGL_LIST* pL, DRV_PIC32CGMAC_SGL_LIST_NODE* pN);
#endif //  __GMAC_DCPT_LISTS_H_
