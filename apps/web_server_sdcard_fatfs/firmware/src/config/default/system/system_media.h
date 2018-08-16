/*************************************************************************
System Service Media Interface Declarations and Types

  Company:
    Microchip Technology Inc.

  File Name:
    sys_media.h

  Summary:
    System Media declarations and types.

  Description:    
    This file contains function and type declarations required to interact
    with the MPLAB Harmony Media Drivers and File System.                         
  *************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
//DOM-IGNORE-END

#ifndef _SYS_MEDIA_INTERFACE_H_
#define _SYS_MEDIA_INTERFACE_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END  

// *****************************************************************************
/* SYS Media Block Command Handle

  Summary:
    Handle identifying block commands of the media.

  Description:
    A block command handle is returned by a call to the Read, Write, or Erase
    functions. This handle allows the application to track the completion of
    the operation. The handle is returned back to the client by the "event
    handler callback" function registered with the driver.

    The handle assigned to a client request expires when the client has been
    notified of the completion of the operation (after event handler function
    that notifies the client returns) or after the buffer has been retired by
    the driver if no event handler callback was set.

  Remarks:
    None.
*/

typedef uintptr_t SYS_MEDIA_BLOCK_COMMAND_HANDLE;

// *****************************************************************************
/* Invalid media block command handle

  Summary:
    Defines the invalid media block command handle.

  Description:
    This value defines invalid handle for the media block command. It is
    returned by read/write/erase routines when the request could not be taken.

  Remarks:
    None.
*/

#define SYS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID /*DOM-IGNORE-BEGIN*/((SYS_MEDIA_BLOCK_COMMAND_HANDLE)(-1))/*DOM-IGNORE-END*/

// *****************************************************************************
/* SYS Media Events

   Summary
    Identifies the possible events that can result from a request.

   Description
    This enumeration identifies the possible events that can result from a
    media.

   Remarks:
    One of these values is passed in the "event" parameter of the event
    handling callback function that client registered with the driver by
    setting the event handler when a block request is completed.
*/

typedef enum
{
    /* Block operation has been completed successfully. */
    SYS_MEDIA_EVENT_BLOCK_COMMAND_COMPLETE,

    /* There was an error during the block operation */
    SYS_MEDIA_EVENT_BLOCK_COMMAND_ERROR

} SYS_MEDIA_BLOCK_EVENT;

// *****************************************************************************
/* SYS Media Property Structure

  Summary:
    Contains information of property of a media.

  Description:
    This structure contains the information of property of a media device.

  Remarks:
    For a device, if multiple properties are applicable, they can be ORed
    together and used.
*/

typedef enum
{
    /* Media supports Byte Write */
    SYS_MEDIA_SUPPORTS_BYTE_WRITES = 0x01,
    
    /* Media supports only Read operation */
    SYS_MEDIA_SUPPORTS_READ_ONLY = 0x02,
    
    /* Media supports OTP (One Time Programming) */
    SYS_MEDIA_SUPPORTS_ONE_TIME_PROGRAMING = 0x04,

    /* Read in blocking */
    SYS_MEDIA_READ_IS_BLOCKING = 0x08,

    /* Write is blocking */
    SYS_MEDIA_WRITE_IS_BLOCKING = 0x10,
  
} SYS_MEDIA_PROPERTY;

// *****************************************************************************
/* Status of media

  Summary:
    The state of media.

  Description:
    This enumeration states if the media is attached or not

  Remarks:
    None.
*/

typedef enum
{
    /* Media is detached */
    SYS_MEDIA_DETACHED,

    /* Media is attached */
    SYS_MEDIA_ATTACHED

} SYS_MEDIA_STATUS;

// *****************************************************************************
/* Status of buffer

  Summary:
    The enumeration for status of buffer

  Description:
    This enumeration contains the various status of buffer.

  Remarks:
    None.
*/
typedef enum
{
    /*Done OK and ready */
    SYS_MEDIA_COMMAND_COMPLETED          = 0 ,

    /*Scheduled but not started */
    SYS_MEDIA_COMMAND_QUEUED             = 1,

    /*Currently being in transfer */
    SYS_MEDIA_COMMAND_IN_PROGRESS        = 2,

    /*Unknown buffer */
    SYS_MEDIA_COMMAND_UNKNOWN            = -1,

} SYS_MEDIA_COMMAND_STATUS;

// *****************************************************************************
/* SYS Media Region Geometry Structure

  Summary:
    Contains information of a sys media region.

  Description:
    This structure contains the information of a sys media region.

  Remarks:
    A media can have multiple regions. Sum of size of all the regions is the
    total memory size of the media. Each region is further divided into blocks
    of identical size.
*/

typedef struct
{
    /* Size of a each block in Bytes */
    uint32_t blockSize;
    
    /* Number of Blocks of identical size within the Region */
    uint32_t numBlocks;
    
} SYS_MEDIA_REGION_GEOMETRY;

// *****************************************************************************
/* SYS Media Device Geometry

  Summary:
    Contains all the geometrical information of a media device.

  Description:
    This structure contains all the geometrical information of a media device.
    the structure also has property of the media like media is one time
    programmable (OTP) or Read Only etc.

  Remarks:
    A memory device can have multiple erase block regions. Sum of all the
    regions is the total memory size of the device.
*/

typedef struct 
{
    /* Properties of a Media. For a device, if multiple properties  are
       applicable, they can be ORed */
    SYS_MEDIA_PROPERTY mediaProperty;
    
    /* Number of Read Regions */
    uint32_t numReadRegions;
    
    /* Number of Write Regions */
    uint32_t numWriteRegions;
    
    /* Number of Erase Regions */
    uint32_t numEraseRegions;
    
    /* Pointer to the table containing the geometry information */
    SYS_MEDIA_REGION_GEOMETRY *geometryTable;
    
} SYS_MEDIA_GEOMETRY;

// *****************************************************************************
/* Media Event Handler function pointer

  Summary:
    Pointer to the Media Event Handler function.

  Description
    This data type defines the required function signature for the media event
    handling callback function. A client must register a pointer to an event
    handling function whose function signature (parameter and return value
    types) match the types specified by this function pointer in order to
    receive event calls back from the driver.
    
  Parameters:
    event           - Identifies the type of event
    commandHandle   - Handle returned from the media operation requests
    context         - Value identifying the context of the application that
                      registered the event handling function

  Returns:
    None.

  Remarks:
    None.
*/
typedef void (* SYS_MEDIA_EVENT_HANDLER)
(
    SYS_MEDIA_BLOCK_EVENT event,
    SYS_MEDIA_BLOCK_COMMAND_HANDLE commandHandle,
    uintptr_t context
);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif

