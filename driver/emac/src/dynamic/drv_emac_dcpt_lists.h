/*
Copyright (C) 2020-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#ifndef _DRV_EMAC_DCPT_LISTS_H_
#define _DRV_EMAC_DCPT_LISTS_H_
// -----------------------------------------------------------------------------
// Section: Constants & Data Types

// *****************************************************************************
/* Ethernet Descriptor Types

  Summary:
    Enumeration defining the Ethernet descriptor types.

  Description:
    This enumeration defines the Ethernet descriptor types.

  Remarks:
    Most descriptor operations support just one type, but multiple flags can be
    set.
*/
typedef enum
{
    MAC_DRVR_DCPT_TYPE_RX = 0x1,        // RX descriptor
    MAC_DRVR_DCPT_TYPE_TX = 0x2,        // TX descriptor
    // All (both) types. Some descriptor operations support multiple types.
    MAC_DRVR_DCPT_TYPE_ALL = (MAC_DRVR_DCPT_TYPE_RX | MAC_DRVR_DCPT_TYPE_TX)
} MAC_DRVR_DCPT_TYPE;

/*******************************************************************************
  TX Packet Descriptor Status Word

  Summary:
    Status word with each Transmit Descriptor

  Description:
    The transmit descriptor consist of two 32-bit word; one is Transmit Status
    word. The status of a packet transmission is stored in this word
*/
typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    bLen:                       11, // b00-10: length of buffer
                    bReserved11_14:             4,  // b11-14
                    bLastBuffer:                1,  // b15   : last buffer in the current frame
                    bNoCrc:                     1,  // b16
                    bReserved17_26:             10, // b17-26
                    bMidFrameBufferExhaustion:  1,  // b27
                    bUnderRun:                  1,  // b28
                    bRetryLimitExceeded:        1,  // b29
                    bWrap:                      1,  // b30
                    bUsed:                      1;  // b31
    } bm;
} MAC_DRVR_TXDCPT_STATUS;

// Transmit buffer descriptor bits
#define EMAC_TX_LAST_BUFFER_BIT     (1u << 15)
#define EMAC_TX_EXHAUST_BIT         (1u << 27)  // Exhausted in mid-frame
#define EMAC_TX_UNDER_RUN_BIT       (1u << 28)  // Tx Buffer Under-run
#define EMAC_TX_RETRY_LIMIT_BIT     (1u << 29)  // Retry Limit Exceeded
#define EMAC_TX_WRAP_BIT            (1u << 30)
#define EMAC_TX_USED_BIT            (1u << 31)
#define EMAC_TX_ERR_BITS            (EMAC_TX_RETRY_LIMIT_BIT | EMAC_TX_UNDER_RUN_BIT | EMAC_TX_EXHAUST_BIT)

#define EMAC_TX_DESC_LEN_MASK       (0x7FF)
#define DRVRnEMAC_TX_USE_BIT        EMAC_TX_USED_BIT

/*******************************************************************************
  RX Packet Descriptor Address Word

  Summary:
    Address word with each Receive Descriptor

  Description:
    The Receive descriptor consist of two 32-bit word; one is Receive address
    word. The Rx buffer address, the Ownership bit and Wrap bit stored in this
    word
*/
typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    bOwnership: 1,  // b00,     User clears, MAC sets when frame written to memory
                    bWrap:      1,  // b01,     Marks last descriptor in receive buffer
                    bAddress:   30; // b02-31:  Address with EMAC_RX_ADDRESS_MASK applied
    } bm;
} MAC_DRVR_RXDCPT_ADDR;

/*******************************************************************************
  RX Packet Descriptor Status Word

  Summary:
    Status word with each Receive Descriptor

  Description:
    The Receive descriptor consist of two 32-bit word; one is Receive Status
    word. Different receive status after receiving a packet is stored in this
    32-bit field by MAC engine
*/
typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    bLen:                           12,// b00-11
                    bBufferOffset:                  2, // b12-13: Receive buffer offset
                    bSOF:                           1, // b14   : Start of Frame
                    bEOF:                           1, // b15   : End of Frame
                    bCFI:                           1, // b16   : Concatenation Format Indicator
                    bVLanPriority:                  3, // b17-19
                    bPriorityTagDetected:           1, // b20
                    bVLanTagDetected:               1, // b21
                    bTypeIdMatch:                   1, // b22
                    bSpecificRegisterAddress4Match: 1, // b23
                    bSpecificRegisterAddress3Match: 1, // b24
                    bSpecificRegisterAddress2Match: 1, // b25
                    bSpecificRegisterAddress1Match: 1, // b26
                    bReserved27:                    1, // b27
                    bExternalAddressMatch:          1, // b28
                    bUnicastHashMatch:              1, // b29
                    bMulticastHashMatch:            1, // b30
                    bBroadcastDetected:             1; // b31   : Global all ones broadcast address detected
    } bm;
} MAC_DRVR_RXDCPT_STATUS;

#define MAC_RX_STICKY_BUFFERS       true
#define MAC_RX_DYNAMIC_BUFFERS      !MAC_RX_STICKY_BUFFERS

// The buffer addresses written into the descriptors must be aligned so the
// last few bits are zero.  These bits have special meaning for the MAC
// peripheral and cannot be used as part of the address.
#define EMAC_RX_ADDRESS_MASK        ((unsigned int)0xFFFFFFFC)
#define EMAC_RX_FRAME_LENGTH_MASK   ((unsigned int)0x0FFF)

// Receive buffer descriptor bits
#define DRVRnEMAC_RX_OWNER_BIT      (1u <<  0)
#define EMAC_RX_WRAP_BIT            (1u <<  1)
#define EMAC_RX_SOF_BIT             (1u << 14)
#define EMAC_RX_EOF_BIT             (1u << 15)

/*******************************************************************************
  Receive Descriptor

  Summary:
    RX descriptor for control and status of MAC RX block

  Description:
    The Receive descriptor consist of two 32-bit word; one is Receive buffer
    address and other for Rx status
*/
typedef struct
{
    volatile MAC_DRVR_RXDCPT_ADDR       bufferAddress;
    volatile MAC_DRVR_RXDCPT_STATUS     status;
} MAC_DRVR_HW_RXDCPT;

/*******************************************************************************
  Transmit Descriptor

  Summary:
    TX descriptor for control and status of MAC Transmit block

  Description:
    The Transmit descriptor consist of two 32-bit word; one is Transmit buffer
    address  and other for Tx status
*/
typedef struct
{
    volatile uint32_t                   bufferAddress;
    volatile MAC_DRVR_TXDCPT_STATUS     status;
} MAC_DRVR_HW_TXDCPT;


#endif // _DRV_EMAC_DCPT_LISTS_H_
