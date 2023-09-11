/*******************************************************************************
  PPP in HDLC Driver Object API

  Company:
    Microchip Technology Inc.
    
  FIle Name:
    drv_hdlc_obj.h

  Summary:
    This file contains the API definitions for the HDLC Driver Object.

  Description:
    Definitions for the HDLC Driver library.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


#ifndef _DRV_HDLC_OBJ_H_
#define _DRV_HDLC_OBJ_H_

#include <stdint.h>
#include <stdbool.h>
#include "system/system_module.h"


// handle to be used in the memory allocation functions
// Usually the stack allocation functions are used
typedef const void* HDLC_ALLOC_HANDLE;

// HDLC object configuration:
typedef struct
{
    /* sizeof the buffer for an HDLC frame
       Should match the link MRU.
       Internal minimal value is 1500 bytes */
    uint16_t    rxBuffSize;
    
    /* size of the processing buffer to perform reads from the serial object
       and assemble HDLC frames.
       Th closer to the MRU the better, as less reads required to assemble a frame
       Recommended size: >= 1500 bytes
       Leave 0 for default */
    uint16_t      procBufferSize;

    /* size of aditional peek buffer to perform reads from the serial object
       and assemble HDLC frames.
       Needed because the serial pbject doesn't have a peek function
       Recommended size: >= 128 bytes
       Leave 0 for default */
    uint16_t      peekBufferSize;

    /*  malloc type allocation function */
    void*   (*mallocF)(HDLC_ALLOC_HANDLE allocH, size_t nBytes);
    /*  calloc type allocation function */
    void*   (*callocF)(HDLC_ALLOC_HANDLE allocH, size_t nElems, size_t elemSize);
    /*  free type allocation free function */
    void    (*freeF)(HDLC_ALLOC_HANDLE allocH, const void* pBuff);
    /*  handle to be used in the allocation calls */
    HDLC_ALLOC_HANDLE   allocH; 

    // the serial object to be used
    const struct _tag_SERIAL_HDLC_OBJECT* serialObj;
}DRV_HDLC_CONFIG;

// *****************************************************************************
/* HDLC Device Driver Initialization Data

  Summary:
    Contains all the data necessary to initialize the HDLC device.

  Description:
    This data structure contains all the data necessary to initialize
    the HDLC device.

  Remarks:
    A pointer to a structure of this format containing the desired
    initialization data must be passed into the HDLC initialize routine.
*/

typedef struct
{
    SYS_MODULE_INIT         moduleInit;     // System module initialization
    const DRV_HDLC_CONFIG*  hdlcConfigData;   // HDLC configuration data
}DRV_HDLC_INIT;

// *****************************************************************************
/* HDLC statistics Data Structure

  Summary:
    Contains the statistics data maintained by the HDLC module

  Description:
    This structure defines the statistic counters maintained by the HDLC module 


  Remarks:
    All members are uint32_t!

*/
typedef struct
{
    uint32_t    txFrames;           // number of total TX successful frames
    uint32_t    txTotChars;         // number of total TX characters
    uint32_t    rxFrames;           // number of total RX successful frames
    uint32_t    rxChainedFrames;    // number of detected chained frames
    uint32_t    rxTotChars;         // number of total RX characters
    uint32_t    freeFrames;         // number of available RX frames 
    uint32_t    busyFrames;         // number of assembled RX frames 
    uint32_t    pendFrames;         // number of pending RX frames sent to the use 

    // errors 
    uint32_t    rxBuffNA;           // # of discarded frames beacuse an RX buffer was not available
                                    // (all were in use)
    uint32_t    txAllocErr;         // buffer allocation failed when trying to expand a write frame 
    uint32_t    serialWrSpaceErr;   // serial write space not enough to write the frame
    uint32_t    serialWrErr;        // serial write operation failure: not all data written
    uint32_t    rxShortFrames;      // # of discarded too short frames 
    uint32_t    rxLongFrames;       // # of discarded frames longer than the RX buffer
    uint32_t    rxFormatErr;        // # of discarded frames with format error
                                    // (missing 'all stations address' == 0xff or the 'control field' == 0x03 
    uint32_t    rxFcsErr;           // # of discarded frames with FCS error

}DRV_HDLC_STATISTICS;

// *****************************************************************************
/* HDLC operation result

  Summary:
    Definitions for a HDLC operation result

  Description:
    This enumeration defines the result of a HDLC operation


  Remarks:
    Currently this applies to the 'write' operation only

*/
typedef enum
{
    DRV_HDLC_RES_OK     = 0,        // operation successful

    // transient errors - operation could be retried
    DRV_HDLC_RES_WRITE_SPACE_ERR    = 1,    // the underlying serial object does not have enough
                                            // write space in its internal buffer
                                            // probably busy with previous write operations

    DRV_HDLC_RES_ALLOC_ERR          = 2,    // failure when trying to allocate memory for the frame
    
    // fatal errors
    DRV_HDLC_RES_HANDLE_ERR         = -1,   // wrong handle for the operation
    DRV_HDLC_RES_WRITE_ERR          = -2,   // the underlying serial object performed a truncated write
    

}DRV_HDLC_RES;


// frames are owned by PPP and passed to HDLC with addRxFrame
typedef struct _tag_DRV_HDLC_FRAME
{
    struct _tag_DRV_HDLC_FRAME* next;   // cast to SGL_LIST_NODE*
                                        // could be used by HDLC client
                                        // when getting a frame with 'readFrame' 
    const void* hdlcPkt;                // pointer to the TCPIP_MAC_PACKET this frame belongs to
                                        // set by frame owner (PPP)
    uint16_t    frameLen;               // size of the PPP_FRAME (TX/RX)
    union
    {
        uint16_t    user16;             // unused by HDLC other than for padding/proper alignment
        uint8_t     user8[2];           // could be used by the client
    };

    uint8_t*        data;       // pointer to this HDLC frame payload: a PPP_FRAME
                                // PPP_FRAME::proto is 16 bit aligned so the LCP/IPCP/IP frames are 32 bit aligned
                                // Note: No header/trailer Flag sequence or FCS is saved
}DRV_HDLC_FRAME;


typedef struct _tag_DRV_HDLC_OBJECT
{
    SYS_MODULE_OBJ      (*initialize)(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);
    void                (*deinitialize)(SYS_MODULE_OBJ object);
    void                (*reinitialize)(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);
    SYS_STATUS          (*status)( SYS_MODULE_OBJ object );
    void                (*task)( SYS_MODULE_OBJ object );
    uint16_t            (*frameOverhead)(SYS_MODULE_OBJ object);
    DRV_HANDLE          (*open)(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
    void                (*close)(DRV_HANDLE hHdlc);

    // adds a new frame for RX
    DRV_HDLC_RES        (*addRxFrame)(DRV_HANDLE hHdlc, DRV_HDLC_FRAME* pFrame);

    // writes a hdlc frame
    // chained frames should be supported!
    // data is copied internally and no acknowledge procedure is needed
    // returns a DRV_HDLC_RES
    DRV_HDLC_RES        (*writeFrame)(DRV_HANDLE hHdlc, const DRV_HDLC_FRAME* pFrame);
    // reads a frame, if available
    // on output pLcpFrame will point to the frame
    // returns the frame size 
    DRV_HDLC_FRAME*     (*readFrame)(DRV_HANDLE hHdlc);
    // acknowledges a frame returned by readFrame - i.e processing of that frame/buffer is done
    void                (*ackRxFrame)(DRV_HANDLE hHdlc, DRV_HDLC_FRAME* pHdlcFrame, bool isSticky);

    // set the RX (peer) ACCM
    // returns:
    //      - the requested ACCM if setting was OK
    //      - the negated value if the call failed
    uint32_t            (*setRxAccm)(DRV_HANDLE hHdlc, uint32_t rxAccm);

    // set the TX (this host) ACCM
    // returns:
    //      - the requested ACCM if setting was OK
    //      - the negated value if the call failed
    uint32_t            (*setTxAccm)(DRV_HANDLE hHdlc, uint32_t txAccm);

    // will copy the current value of the HDLC statistics to the user supplied data structure
    // Implemented only if the build symbol DRV_HDLC_ENABLE_STATISTICS != 0 
    bool                (*getStatistics)(DRV_HANDLE hHdlc, DRV_HDLC_STATISTICS* pStat, bool clear);
    // set the event notification callback
    // the event is a TCPIP_MAC_EVENT
    bool                (*setEventCback)(DRV_HANDLE hHdlc, void (*cback)(DRV_HANDLE, int evMask), DRV_HANDLE handle);
}DRV_HDLC_OBJECT;        // HDLC driver object descriptor


// supported HDLC objects
//
// default DRV_HDLC_AsyncObject: transport over a serial asynchronous line
extern const DRV_HDLC_OBJECT DRV_HDLC_AsyncObject;

#endif  // _DRV_HDLC_OBJ_H_

