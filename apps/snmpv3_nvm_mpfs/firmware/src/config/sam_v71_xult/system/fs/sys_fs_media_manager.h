/*************************************************************************
File System Service Media Manager Interface Declarations and Types

  Company:
    Microchip Technology Inc.

  File Name:
    sys_fs_media_manager.h

  Summary:
    File System Media Manager interface declarations and types.

  Description:    
    This file contains function and type declarations required to interact
    with the MPLAB Harmony File System Media Manager Framework.                         
  *************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _SYS_FS_MEDIA_MANAGER_H_
#define _SYS_FS_MEDIA_MANAGER_H_

#include "driver/driver_common.h"
#include "system/fs/sys_fs.h"
#include "system/system_media.h"
#include "system/system.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
/* SYS FS Media Block Command Handle

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

typedef SYS_MEDIA_BLOCK_COMMAND_HANDLE  SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE;

// *****************************************************************************
/* SYS FS Media Handle

  Summary:
    Handle identifying the media registered with the media manager.

  Description:
    The media drivers register the media services with the media manager by
    calling the SYS_FS_MEDIA_MANAGER_Register function. On successful
    registration a media handle is returned which can be used by the media
    driver to deregister the services from the media manager layer.

  Remarks:
    None.
*/
typedef uintptr_t SYS_FS_MEDIA_HANDLE;

// *****************************************************************************
/* Invalid media handle

  Summary:
    Defines the invalid media handle.

  Description:
    This value defines invalid media handle. It is returned when the media
    registration is not successful.

  Remarks:
    None.
*/
#define  SYS_FS_MEDIA_HANDLE_INVALID DRV_HANDLE_INVALID

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

#define SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID SYS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID

// *****************************************************************************
/* SYS FS Media Events

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
    SYS_FS_MEDIA_EVENT_BLOCK_COMMAND_COMPLETE = SYS_MEDIA_EVENT_BLOCK_COMMAND_COMPLETE,

    /* There was an error during the block operation */
    SYS_FS_MEDIA_EVENT_BLOCK_COMMAND_ERROR    = SYS_MEDIA_EVENT_BLOCK_COMMAND_ERROR

} SYS_FS_MEDIA_BLOCK_EVENT;

// *****************************************************************************
/* SYS FS Media Property Structure

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
    SYS_FS_MEDIA_SUPPORTS_BYTE_WRITES = SYS_MEDIA_SUPPORTS_BYTE_WRITES,
    
    /* Media supports only Read operation */
    SYS_FS_MEDIA_SUPPORTS_READ_ONLY = SYS_MEDIA_SUPPORTS_READ_ONLY,
    
    /* Media supports OTP (One Time Programming) */
    SYS_FS_MEDIA_SUPPORTS_ONE_TIME_PROGRAMING = SYS_MEDIA_SUPPORTS_ONE_TIME_PROGRAMING,

    /* Read in blocking */
    SYS_FS_MEDIA_READ_IS_BLOCKING = SYS_MEDIA_READ_IS_BLOCKING,

    /* Write is blocking */
    SYS_FS_MEDIA_WRITE_IS_BLOCKING = SYS_MEDIA_WRITE_IS_BLOCKING,
  
} SYS_FS_MEDIA_PROPERTY;

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
    SYS_FS_MEDIA_DETACHED = SYS_MEDIA_DETACHED,

    /* Media is attached */
    SYS_FS_MEDIA_ATTACHED = SYS_MEDIA_ATTACHED

} SYS_FS_MEDIA_STATUS;

// *****************************************************************************
/* State of media

  Summary:
    The enumeration for state of media.

  Description:
    The media manager task picks a disk for analysis and takes it through a
    number of states. This enumeration mentions the state of the media.

  Remarks:
    None.
*/
typedef enum
{
    /* Media is de registered with the media manager */
    SYS_FS_MEDIA_STATE_DEREGISTERED = 0,

    /* Media is registered with the media manager */
    SYS_FS_MEDIA_STATE_REGISTERED,

    /* Check the attach/detach status of the Media */
    SYS_FS_MEDIA_CHECK_ATTACH_STATUS,

    /* Read the first sector of the media */
    SYS_FS_MEDIA_READ_FIRST_SECTOR,

    /* Analyze the FS */
    SYS_FS_MEDIA_ANALYZE_FS,

} SYS_FS_MEDIA_STATE;

// *****************************************************************************
/* Type of media

  Summary:
    The enumeration for type of media.

  Description:
    This enumeration is for the type of media registered with the media
    manager.

  Remarks:
    None.
*/
typedef enum
{
    /* Media is of type NVM (internal flash (non volatile) memory)*/
    SYS_FS_MEDIA_TYPE_NVM,
    
    /* Media is of type mass storage device */
    SYS_FS_MEDIA_TYPE_MSD,
    
    /* Media is of type SD card */
    SYS_FS_MEDIA_TYPE_SD_CARD,
    
    /* Media is of type RAM */
    SYS_FS_MEDIA_TYPE_RAM,
    
    /* Media is of type SPI Flash */
    SYS_FS_MEDIA_TYPE_SPIFLASH

} SYS_FS_MEDIA_TYPE;

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
    SYS_FS_MEDIA_COMMAND_COMPLETED          = SYS_MEDIA_COMMAND_COMPLETED,

    /*Scheduled but not started */
    SYS_FS_MEDIA_COMMAND_QUEUED             = SYS_MEDIA_COMMAND_QUEUED,

    /*Currently being in transfer */
    SYS_FS_MEDIA_COMMAND_IN_PROGRESS        = SYS_MEDIA_COMMAND_IN_PROGRESS,

    /*Unknown buffer */
    SYS_FS_MEDIA_COMMAND_UNKNOWN            = SYS_MEDIA_COMMAND_UNKNOWN,

} SYS_FS_MEDIA_COMMAND_STATUS;

// *****************************************************************************
/* SYS FS Media Region Geometry Structure

  Summary:
    Contains information of a sys media region.

  Description:
    This structure contains the information of a sys media region.

  Remarks:
    A media can have multiple regions. Sum of size of all the regions is the
    total memory size of the media. Each region is further divided into blocks
    of identical size.
*/

typedef SYS_MEDIA_REGION_GEOMETRY SYS_FS_MEDIA_REGION_GEOMETRY;

// *****************************************************************************
/* SYS FS Media Device Geometry

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

typedef SYS_MEDIA_GEOMETRY SYS_FS_MEDIA_GEOMETRY;

// *****************************************************************************
/* Media Mount Data

  Summary:
    Structure to obtain the device and mount name of media

  Description:
    This structure is an input for the media manager to auto mount the media
    when the auto mount feature is enabled.

  Remarks:
    None.
*/
typedef struct
{
    /* Media Mount Name */
    const char* mountName;

    /* Media Device Name */
    const char* devName;
    
    /* Media Type */
    SYS_FS_MEDIA_TYPE mediaType;
    
    /* File system type on Media */
    SYS_FS_FILE_SYSTEM_TYPE fsType;
    
} SYS_FS_MEDIA_MOUNT_DATA;

// *****************************************************************************
/* Volume property

  Summary:
    Structure to obtain the property of volume

  Description:
    This structure is passed by sys_fs layer to know the property of a volume.
    The function "SYS_FS_MEDIA_MANAGER_VolumePropertyGet" is used for the call.

  Remarks:
    None.
*/
typedef struct _SYS_FS_VOLUME_PROPERTY
{
    /* Volume # */
    unsigned int volNumber;
    /* File system type */
    SYS_FS_FILE_SYSTEM_TYPE fsType;
} SYS_FS_VOLUME_PROPERTY;

// *****************************************************************************
/* Media function pointers

  Summary:
    Structure of function pointers for media driver

  Description:
    This structure contains the definition for functions of media driver, which
    is registered with the media manager. In future, if any new driver needs to
    be registered with the media manager (say, to act as a media for file
    system), the new driver should have implemented all these functions.

  Remarks:
    None.
*/

typedef struct
{
    /* To obtains status of media */
    bool (*mediaStatusGet)(DRV_HANDLE handle);
    /* Function to get media geometry */
    SYS_FS_MEDIA_GEOMETRY * (*mediaGeometryGet)(const DRV_HANDLE handle);
    /* Function for sector read */
    void (*sectorRead)(DRV_HANDLE clientHandle,SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE * commandHandle,
                                            void * buffer, uint32_t blockStart, uint32_t nBlock);
    /* Function for sector write */
    void (*sectorWrite)(const DRV_HANDLE handle,SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE * commandHandle,
                                         void * sourceBuffer, uint32_t blockStart,uint32_t nBlock);
    /* Function register the event handler with media */
    void (*eventHandlerset)(DRV_HANDLE handle, const void * eventHandler, const uintptr_t context);
    /* Function to obtain the command status */
    SYS_FS_MEDIA_COMMAND_STATUS (*commandStatusGet)(DRV_HANDLE handle,
                                SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE commandHandle);    
    /* Function to read certain bytes from the media */
    void (*Read) (DRV_HANDLE clientHandle,SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE * commandHandle,
                                         void * buffer, uint32_t blockStart, uint32_t nBlock);
    /* Function to obtain the address of the media (to be used for NVM only) */
    uintptr_t (*addressGet) ( const DRV_HANDLE hClient);
    void (*erase) ( const DRV_HANDLE handle,SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE * commandHandle,
                                                         uint32_t blockStart,uint32_t nBlock);
    /* Function to open the media driver */
    DRV_HANDLE (*open)(SYS_MODULE_INDEX index, DRV_IO_INTENT intent);
    /* Function to close the media */
    void (*close)(DRV_HANDLE client);
    /* Task function of the media */
    void (*tasks)(SYS_MODULE_OBJ obj);

} SYS_FS_MEDIA_FUNCTIONS;

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
typedef SYS_MEDIA_EVENT_HANDLER SYS_FS_MEDIA_EVENT_HANDLER;

//*****************************************************************************
/* Function:
    SYS_FS_MEDIA_COMMAND_STATUS SYS_FS_MEDIA_MANAGER_CommandStatusGet
    (
    uint16_t diskNo,
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE bufferHandle
    )

   Summary:
     Gets the command status.
   
   Description:
     This function gets the command status. The sector read and sector write
     are non-blocking functions. Therefore, this interface is provided where
     the code should periodically poll for the buffer status. If status is
     completed, the read/write operation is considered to be complete.
 
   Precondition:
     None.
 
   Parameters:
     diskNo         - media number
     bufferHandle   - the command handle which was obtained during sector read/ write
 
   Returns:
     Command status of type SYS_FS_MEDIA_COMMAND_STATUS.
 
*/
SYS_FS_MEDIA_COMMAND_STATUS SYS_FS_MEDIA_MANAGER_CommandStatusGet
(
    uint16_t     diskNo,
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE commandHandle
);

// *****************************************************************************
/* Function:
    SYS_FS_MEDIA_HANDLE SYS_FS_MEDIA_MANAGER_Register
    (
        SYS_MODULE_OBJ obj,
        SYS_MODULE_INDEX index,
        const SYS_FS_MEDIA_FUNCTIONS *mediaFunctions,
        SYS_FS_MEDIA_TYPE mediaType
    )

  Summary:
    Function to register media drivers with the media manager.
  
  Description:
    This function is called by the media driver to register the functionalities
    with the media manager. For static media, such as NVM or a SD card, the
    media drivers register with this function at the time of initialization.
    For dynamic media, such as MSD, this register function is called
    dynamically, once the MSD media is connected.

  Precondition:
    None.

  Parameters:
    obj             - driver object (of type SYS_MODULE_OBJ, value returned
                      when driver is initialized)
    index           - driver index (of type SYS_MODULE_INDEX, value passed
                      during driver initialization and opening)
    mediaFunctions  - List of media driver functions
    mediaType       - Type of media
 
  Returns:
    Valid handle of type SYS_FS_MEDIA_HANDLE on successful registration of the
    media driver.
    Invalid handle of type SYS_FS_MEDIA_HANDLE_INVALID on unsuccessful
    registration of the media driver.

  Remarks:
    None
*/
SYS_FS_MEDIA_HANDLE SYS_FS_MEDIA_MANAGER_Register
(
    SYS_MODULE_OBJ obj,
    SYS_MODULE_INDEX index,
    const SYS_FS_MEDIA_FUNCTIONS *mediaFunctions,
    SYS_FS_MEDIA_TYPE mediaType
);

//*****************************************************************************
/*
  Function:
    void SYS_FS_MEDIA_MANAGER_DeRegister
    (
            SYS_FS_MEDIA_HANDLE    handle
    )

  Description:
    Function called by a media to deregister itself to the media manager. For
    static media, (like NVM or SD card), this "deregister function" is never
    called, since static media never gets deregistered once they are
    initialized.  For dynamic media (like MSD), this register function is
    called dynamically, once the MSD media is connected.

  Precondition:
    None.

  Parameters:
    handle          - Handle of type SYS_FS_MEDIA_HANDLE received when the media
                      was registered

  Returns:
    None.
*/
void SYS_FS_MEDIA_MANAGER_DeRegister
(
    SYS_FS_MEDIA_HANDLE    handle
);

//*****************************************************************************
/* Function:
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_SectorRead
    (
        uint16_t diskNo,
        uint8_t * dataBuffer,
        uint32_t sector,
        uint32_t noSectors
    );
 
    Summary:
      Reads a specified media sector.
    
    Description:
      This function reads a specified media (disk) sector. This is the function
      in the media manager layer. This function in turn calls the specific
      sector read function from the list of function pointers of the media
      driver.
 
    Precondition:
      None.
 
    Parameters:
      diskNo         - Media number
      dataBuffer     - Pointer to buffer where data to be placed after read
      sector         - Sector numer to be read
      noSectors      - Number of sectors to read
 
    Returns:
      Buffer handle of type SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE.
*/
SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_SectorRead
(
    uint16_t diskNum,
    uint8_t *dataBuffer,
    uint32_t sector,
    uint32_t numSectors
);

//*****************************************************************************
/* Function:
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_SectorWrite
    (
        uint16_t diskNo,
        uint32_t sector,
        uint8_t * dataBuffer,
        uint32_t noSectors
    );
 
    Summary:
      Writes a sector to the specified media.
    
    Description:
      This function writes to a sector of the specified media (disk). This is
      the function in the media manager layer. This function in turn calls the
      specific sector write function from the list of function pointers of the
      media driver.
 
    Precondition:
      None.
 
    Parameters:
      diskNo         - media number
      sector         - Sector # to which data to be written
      dataBuffer     - pointer to buffer which holds the data to be written
      noSectors      - Number of sectors to be written
 
    Returns:
      Buffer handle of type SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE.
*/
SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_SectorWrite
(
    uint16_t diskNum,
    uint32_t sector,
    uint8_t *dataBuffer,
    uint32_t numSectors
);

//*****************************************************************************
/* Function:
    bool SYS_FS_MEDIA_MANAGER_VolumePropertyGet
    (
        const char *devName
        SYS_FS_VOLUME_PROPERTY *str
    );
 
    Summary:
      Gets the volume property.
    
    Description:
      This function gets the property of the volume. This function is used by
      higher layers (sys_fs layer) to know the property of the volume as
      specified in the SYS_FS_VOLUME_PROPERTY structure.
 
    Precondition:
      None.
 
    Parameters:
      *devName  - String name of the media
      *str      - Pointer to structure of type SYS_FS_VOLUME_PROPERTY
 
    Returns:
    True or false.
*/
bool SYS_FS_MEDIA_MANAGER_VolumePropertyGet
(
    const char *devName, SYS_FS_VOLUME_PROPERTY *str
);

//*****************************************************************************
/* Function:
    bool SYS_FS_MEDIA_MANAGER_MediaStatusGet
    (
        const char *devName
    );
 
  Summary:
    Gets the media status.
    
  Description:
    This function gets the media status. This function is used by higher layers
    (sys_fs layer) to know the status of the media whether the media is attached
    or detached.
 
    Precondition:
      None.
 
    Parameters:
      *devName        - string name of the media
 
    Returns:
      Media attach/detach status of type bool.
*/

bool SYS_FS_MEDIA_MANAGER_MediaStatusGet
(
    const char *devName
);

//*****************************************************************************
/* Function:
    SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_Read
    (
        uint16_t diskNo,
        uint8_t* destination,
        uint8_t * source,
        const unsigned int nBytes
    );

   Summary:
     Gets data from a specific media address.
   
   Description:
     This function gets data from a specific address of media. This function is
     intended to work with NVM media only, which can have byte level
     addressing.  For other media, such as a SD card, byte addressing is not
     possible and this function will not work. Also, this function is intended
     to work with the MPFS2 file system only, which uses byte addressing.

   Precondition:
     None.

   Parameters:
     diskNo         - media number
     destination    - pointer to buffer where data to be placed after read
     source         - pointer from where data to be read
     nBytes         - Number of bytes to be read

   Returns:
     Buffer handle of type SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE
*/
SYS_FS_MEDIA_BLOCK_COMMAND_HANDLE SYS_FS_MEDIA_MANAGER_Read
(
    uint16_t diskNum,
    uint8_t *destination,
    uint8_t *source,
    const uint32_t nBytes
);

//*****************************************************************************
/* Function:
    uintptr_t SYS_FS_MEDIA_MANAGER_AddressGet
    (
        uint16_t diskNo
    );
 
  Summary:
    Gets the starting media address based on a disk number.

  Description:
    This function gets the starting address of a media. This function is
    intended to work only with MPFS2, which does byte addressing and needs a
    memory address (not disk number).
 
  Precondition:
     None.
 
  Parameters:
     diskNo         - media number
 
  Returns:
     Memory address of type uintptr_t.
*/
uintptr_t SYS_FS_MEDIA_MANAGER_AddressGet
(
    uint16_t diskNo
);

//*****************************************************************************
/* Function:
    void SYS_FS_MEDIA_MANAGER_EventHandlerSet
    (
        const void * eventHandler,
        const uintptr_t context
    );
 
  Summary:
    Register the event handler for Mount/Un-Mount events.

  Parameters:
    eventHandler - FS Client event handler pointer
    context - FS Client context

  Description:
    This function is used to register a FS client event handler for notifying the
    Mount/Un-Mount events when AutoMount feature is enabled for File system.

    On Mount/Un-Mount of a volume all the registered clients will be notified.
    The client should check if the mount name passed when event handler is called
    is the one it is expecting and then proceed.

  Note:
    This API should not be called directly from Application. Application should use
    SYS_FS_EventHandlerSet() instead.

    This API is Available only when SYS_FS_AUTOMOUNT_ENABLE is set to true.

    See sys_fs.h for usage information.
***************************************************************************/
void SYS_FS_MEDIA_MANAGER_EventHandlerSet
(
    const void * eventHandler,
    const uintptr_t context
);

//*****************************************************************************
/* Function:
    void SYS_FS_MEDIA_MANAGER_RegisterTransferHandler
    (
        const void *eventHandler
    );
 
  Summary:
    Register the event handler for data transfer events.
    
  Description:
    This function is used to send the command status for the disk operation.
 
  Precondition:
    None.
 
  Parameters:
    eventHandler - Event handler pointer.
 
  Returns:
    Pointer to the media geometry on Success else NULL.
*/
void SYS_FS_MEDIA_MANAGER_RegisterTransferHandler
(
    const void *eventHandler
);

//*****************************************************************************
/* Function:
    SYS_FS_MEDIA_GEOMETRY * SYS_FS_MEDIA_MANAGER_GetMediaGeometry
    (
        uint16_t diskNum
    );
 
  Summary:
    Gets the media geometry information.
    
  Description:
    This function gets the media geometry information.
 
  Precondition:
    None.
 
  Parameters:
    diskNum - Media disk number.
 
  Returns:
    Pointer to the media geometry on Success else NULL.
*/
SYS_FS_MEDIA_GEOMETRY * SYS_FS_MEDIA_MANAGER_GetMediaGeometry
(
    uint16_t diskNum
);

//*****************************************************************************
/* Function:
    void SYS_FS_MEDIA_MANAGER_TransferTask
    (
        uint8_t mediaIndex
    );
 
  Summary:
    Media manager transfer task function.
     
  Description:
    This is the media manager transfer task function. This task is repeatedly
    called by the disk io layer of the native file system for driving the
    current disk read/write operation to completion.
 
  Precondition:
    None
 
  Parameters:
    mediaIndex - disk number of the media
 
  Returns:
    None.
*/
void SYS_FS_MEDIA_MANAGER_TransferTask
(
    uint8_t mediaIndex
);

//*****************************************************************************
/* Function:
    void SYS_FS_MEDIA_MANAGER_Tasks
    (
        void
    );
 
  Summary:
    Media manager task function.
     
  Description:
    This is the media manager task function. This task must be called
    repeatedly from the main loop.
 
  Precondition:
    None
 
  Parameters:
    None.
 
  Returns:
    None.
*/
void SYS_FS_MEDIA_MANAGER_Tasks
(
    void
);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif

