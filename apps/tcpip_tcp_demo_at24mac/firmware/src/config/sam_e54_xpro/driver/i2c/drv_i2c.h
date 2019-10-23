/*******************************************************************************
  I2C Driver Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c.h

  Summary:
    I2C Driver Interface Header File

  Description:
    The I2C device driver provides a simple interface to manage the I2C modules
    on Microchip PIC32 microcontrollers.  This file provides the interface
    definition for the I2C driver.
*******************************************************************************/

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

#ifndef DRV_I2C_H
#define DRV_I2C_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "drv_i2c_definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* I2C Driver Transfer Handle

  Summary:
    Handle identifying a read, write or write followed by read transfer passed to
    the driver.

  Description:
    A transfer handle value is returned by a call to the DRV_I2C_ReadTransferAdd/
    DRV_I2C_WriteTransferAdd or DRV_I2C_WriteReadTransferAdd functions. This
    handle is associated with the transfer passed into the function and it allows
    the application to track the completion of the data from (or into) that
    transfer. The transfer handle value returned from the "transfer add" function
    is returned back to the client by the "event handler callback" function
    registered with the driver.

    The transfer handle assigned to a client request expires when the client has
    been notified of the completion of the buffer transfer (after event handler
    function that notifies the client returns) or after the transfer has been
    retired by the driver if no event handler callback was set.

  Remarks:
    None
*/

typedef uintptr_t DRV_I2C_TRANSFER_HANDLE;

// *****************************************************************************
/* I2C Driver Invalid Transfer Handle

  Summary:
    Definition of an invalid transfer handle.

  Description:
    This is the definition of an invalid transfer handle. An invalid transfer
    handle is returned by DRV_I2C_ReadTransferAdd, DRV_I2C_WriteTransferAdd and
    DRV_I2C_WriteReadTransferAdd functions if the buffer add request was not
    successful.

  Remarks:
    None
*/

#define DRV_I2C_TRANSFER_HANDLE_INVALID /*DOM-IGNORE-BEGIN*/((DRV_I2C_TRANSFER_HANDLE)(-1))/*DOM-IGNORE-END*/

// *****************************************************************************
/* I2C Driver Transfer Events

   Summary:
    Identifies the possible events that can result from a buffer add request.

   Description:
    This enumeration identifies the possible events that can result from a
    buffer add request caused by the client calling either the
    DRV_I2C_ReadTransferAdd, DRV_I2C_WriteTransferAdd or
    DRV_I2C_WriteReadTransferAdd functions.

   Remarks:
    One of these values is passed in the "event" parameter of the event
    handling callback function that the client registered with the driver by
    calling the DRV_I2C_TransferEventHandlerSet function when a buffer
    transfer request is completed.
*/

typedef enum
{
    /* Transfer request is pending */
    DRV_I2C_TRANSFER_EVENT_PENDING = 0,

    /* All data from or to the buffer was transferred successfully. */
    DRV_I2C_TRANSFER_EVENT_COMPLETE = 1,

    /* Transfer Handle given is expired. It means transfer
    is completed but with or without error is not known. */
    DRV_I2C_TRANSFER_EVENT_HANDLE_EXPIRED = 2,

    /* There was an error while processing the buffer transfer request. */
    DRV_I2C_TRANSFER_EVENT_ERROR = -1,

    DRV_I2C_TRANSFER_EVENT_HANDLE_INVALID = -2

} DRV_I2C_TRANSFER_EVENT;

// *****************************************************************************
/* I2C Driver Transfer Event Handler Function Pointer

   Summary:
    Pointer to a I2C Driver Transfer Event handler function

   Description:
    This data type defines the required function signature for the I2C driver
    buffer event handling callback function. A client must register a pointer
    using the buffer event handling function whose function signature (parameter
    and return value types) match the types specified by this function pointer
    in order to receive buffer related event calls back from the driver.

    The parameters and return values are described here and
    a partial example implementation is provided.

  Parameters:
    event - Identifies the type of event

    transferHandle - Handle identifying the buffer to which the vent relates

    context - Value identifying the context of the application that
    registered the event handling function.

  Returns:
    None.

  Example:
    <code>
    void APP_MyTransferEventHandler( DRV_I2C_TRANSFER_EVENT event,
                                   DRV_I2C_TRANSFER_HANDLE transferHandle,
                                   uintptr_t context )
    {
        MY_APP_DATA_STRUCT* pAppData = (MY_APP_DATA_STRUCT*) context;

        switch(event)
        {
            case DRV_I2C_TRANSFER_EVENT_COMPLETE:

                // Handle the completed buffer.
                break;

            case DRV_I2C_TRANSFER_EVENT_ERROR:
            default:

                // Handle error.
                break;
        }
    }
    </code>

  Remarks:
    If the event is DRV_I2C_TRANSFER_EVENT_COMPLETE, it means that the data was
    transferred successfully.

    If the event is DRV_I2C_TRANSFER_EVENT_ERROR, it means that the data was not
    transferred successfully.

    The transferHandle parameter contains the transfer handle of the transfer that
    associated with the event. And transferHandle will be valid while the transfer
    request is in the queue and during callback, unless an error occurred.
    After callback returns, the driver will retire the transfer handle.

    The context parameter contains the a handle to the client context,
    provided at the time the event handling function was registered using the
    DRV_I2C_TransferEventHandlerSet function.  This context handle value is
    passed back to the client as the "context" parameter.  It can be any value
    necessary to identify the client context or instance (such as a pointer to
    the client's data) instance of the client that made the buffer add request.

    The event handler function executes in the peripheral's interrupt
    context. It is recommended of the application to not perform process
    intensive or blocking operations with in this function.

    The DRV_I2C_ReadTransferAdd, DRV_I2C_WriteTransferAdd and
    DRV_I2C_WriteReadTransferAdd functions can be called in the event handler to
    add a buffer to the driver queue. These functions can only be called to add
    buffers to the driver whose event handler is running. For example, I2C2
    driver buffers cannot be added in I2C1 driver event handler.
*/

typedef void ( *DRV_I2C_TRANSFER_EVENT_HANDLER )( DRV_I2C_TRANSFER_EVENT event, DRV_I2C_TRANSFER_HANDLE transferHandle, uintptr_t context );


// *****************************************************************************
// *****************************************************************************
// Section: I2C Driver System Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_I2C_Initialize
    (
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the I2C instance for the specified driver index.

  Description:
    This routine initializes the I2C driver instance for the specified driver
    index, making it ready for clients to open and use it. The initialization
    data is specified by the init parameter. The initialization may fail if the
    number of driver objects allocated are insufficient or if the specified
    driver instance is already initialized. The driver instance index is
    independent of the I2C peripheral instance it is associated with.
    For example, driver instance 0 can be assigned to I2C peripheral instance 2.

  Precondition:
    None.

  Parameters:
    drvIndex - Identifier for the instance to be initialized

    init - Pointer to the init data structure containing any data necessary to
    initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    // The following code snippet shows an example I2C driver initialization.

    SYS_MODULE_OBJ objectHandle;

    DRV_I2C_CLIENT_OBJ drvI2C0ClientObjPool[DRV_I2C_CLIENTS_NUMBER_IDX0] = {0};
    DRV_I2C_TRANSFER_OBJ drvI2C0TransferObjPool[DRV_I2C_QUEUE_SIZE_IDX0] = {0};

    DRV_I2C_PLIB_INTERFACE drvI2C0PLibAPI = {
        .read = (DRV_I2C_PLIB_READ)TWIHS0_Read,
        .write = (DRV_I2C_PLIB_WRITE)TWIHS0_Write,
        .writeRead = (DRV_I2C_PLIB_WRITE_READ)TWIHS0_WriteRead,
        .errorGet = (DRV_I2C_PLIB_ERROR_GET)TWIHS0_ErrorGet,
        .callbackRegister = (DRV_I2C_PLIB_CALLBACK_REGISTER)TWIHS0_CallbackRegister,
    };

    DRV_I2C_INIT drvI2C0InitData = {

        .i2cPlib = &drvI2C0PLibAPI,
        .numClients = DRV_I2C_CLIENTS_NUMBER_IDX0
        .clientObjPool = (uintptr_t)&drvI2C0ClientObjPool[0],
        .interruptI2C = DRV_I2C_INT_SRC_IDX0,
        .queueSize = DRV_I2C_QUEUE_SIZE_IDX0,
        .transferObj = (uintptr_t)&drvI2C0TransferObj[0],
        .clockSpeed = DRV_I2C_CLOCK_SPEED_IDX0,
    };

    objectHandle = DRV_I2C_Initialize(DRV_I2C_INDEX_0, (SYS_MODULE_INIT*)&drvI2C0InitData);
    if (objectHandle == SYS_MODULE_OBJ_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other I2C routine is called.
    This routine should only be called once during system initialization.
*/

SYS_MODULE_OBJ DRV_I2C_Initialize( const SYS_MODULE_INDEX drvIndex, const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
    SYS_STATUS DRV_I2C_Status( const SYS_MODULE_OBJ object)

  Summary:
    Gets the current status of the I2C driver module.

  Description:
    This routine provides the current status of the I2C driver module.

  Precondition:
    Function DRV_I2C_Initialize should have been called before calling this
    function.

  Parameters:
    object - Driver object handle, returned from the DRV_I2C_Initialize routine

  Returns:
    SYS_STATUS_READY - Initialization has succeeded and the I2C is
    ready for additional operations.

    SYS_STATUS_UNINITIALIZED - Indicates that the driver is not initialized.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_I2C_Initialize
    SYS_STATUS          i2cStatus;

    i2cStatus = DRV_I2C_Status(object);
    if (i2cStatus == SYS_STATUS_READY)
    {
        // This means the driver can be opened using the
        // DRV_I2C_Open() function.
    }
    </code>

  Remarks:
    A driver can be opened only when its status is SYS_STATUS_READY.
*/

SYS_STATUS DRV_I2C_Status( const SYS_MODULE_OBJ object);

// *****************************************************************************
// *****************************************************************************
// Section: I2C Driver Common Client Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_I2C_Open
    (
        const SYS_MODULE_INDEX drvIndex,
        const DRV_IO_INTENT ioIntent
    )

  Summary:
    Opens the specified I2C driver instance and returns a handle to it.

  Description:
    This routine opens the specified I2C driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver. The ioIntent
    parameter defines how the client interacts with this driver instance.

    Specifying a DRV_IO_INTENT_EXCLUSIVE will cause the driver to provide
    exclusive access to this client. The driver cannot be opened by any
    other client.

  Precondition:
    Function DRV_I2C_Initialize must have been called before calling this
    function.

  Parameters:
    drvIndex - Identifier for the object instance to be opened

    ioIntent - Zero or more of the values from the enumeration DRV_IO_INTENT
    "ORed" together to indicate the intended use of the driver.
    See function description for details.

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).

    If an error occurs, the return value is DRV_HANDLE_INVALID. Error can occur
    - if the number of client objects allocated via DRV_I2C_CLIENTS_NUMBER is
      insufficient.
    - if the client is trying to open the driver but driver has been opened
      exclusively by another client.
    - if the driver instance being opened is not initialized or is invalid.
    - if the client is trying to open the driver exclusively, but has already
      been opened in a non exclusive mode by another client.
    - if the driver is not ready to be opened, typically when the initialize
      routine has not completed execution.

  Example:
    <code>
    DRV_HANDLE handle;

    handle = DRV_I2C_Open(DRV_I2C_INDEX_0, DRV_IO_INTENT_EXCLUSIVE);
    if (handle == DRV_HANDLE_INVALID)
    {
        // Unable to open the driver
        // May be the driver is not initialized or the initialization
        // is not complete.
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_I2C_Close routine is called.
*/

DRV_HANDLE DRV_I2C_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent );

// *****************************************************************************
/* Function:
    void DRV_I2C_Close( const DRV_HANDLE handle )

  Summary:
    Closes an opened-instance of the I2C driver.

  Description:
    This routine closes an opened-instance of the I2C driver, invalidating the
    handle. Any buffers in the driver queue that were submitted by this client
    will be removed. A new handle must be obtained by calling DRV_I2C_Open
    before the caller may use the driver again.

  Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's
    open routine

  Returns:
    None.

  Example:
    <code>
    // 'handle', returned from the DRV_I2C_Open

    DRV_I2C_Close(handle);

    </code>

  Remarks:
    None.
*/

void DRV_I2C_Close( const DRV_HANDLE handle );

// *****************************************************************************
/*
  Function:
    bool DRV_I2C_TransferSetup ( DRV_HANDLE handle, DRV_I2C_TRANSFER_SETUP* setup )

  Summary:
    Sets the dynamic transfer setup of the driver.

  Description:
    This function should be used to update any of the DRV_I2C_TRANSFER_SETUP
    parameters for the selected client of the driver dynamically. It is mainly
    helpful for multi client scenario where different clients need different
    setup like clock speed. The DRV_I2C_TransferSetup function must be called
    before submitting any I2C driver read/write requests.

  Preconditions:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.
    In case of asynchronous driver, all transfer requests from the queue must
    have been processed.

  Parameters:
    handle      - A valid open-instance handle, returned from the driver's
                   open routine
    setup       - Pointer to the structure containing the new configuration settings

  Returns:
    None.

  Example:
    <code>
        // myI2CHandle is the handle returned by the DRV_I2C_Open function.
        DRV_I2C_TRANSFER_SETUP setup;

        setup.clockSpeed = 400000;

        DRV_I2C_TransferSetup ( myI2CHandle, &setup );
    </code>

  Remarks:
    None.
*/

bool DRV_I2C_TransferSetup( const DRV_HANDLE handle, DRV_I2C_TRANSFER_SETUP* setup);

// *****************************************************************************
/* Function:
    DRV_I2C_ERROR DRV_I2C_ErrorGet( const DRV_HANDLE handle )

   Summary:
    Gets the I2C hardware errors associated with the client.

   Description:
    This function returns the errors associated with the given client.
    The call to this function also clears all the associated error flags.

   Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

   Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine
    DRV_I2C_Open function.

   Returns:
    Errors occurred as listed by DRV_I2C_ERROR.
    This function reports I2C errors if occurred.

  Example:
    <code>
    // 'handle', returned from the DRV_I2C_Open

    if (DRV_I2C_ErrorGet(handle) == DRV_I2C_ERROR_NACK)
    {
        //Errors are cleared by the driver, take respective action
        //for the error case.
    }
    </code>

  Remarks:
    The driver clears all the errors internally.
*/

DRV_I2C_ERROR DRV_I2C_ErrorGet( const DRV_HANDLE handle );

// *****************************************************************************
// *****************************************************************************
// Section: I2C Driver Asynchronous(Queuing Model) Transfer Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void DRV_I2C_WriteTransferAdd(
        const DRV_HANDLE handle,
        const uint16_t address,
        void * const buffer,
        const size_t size,
        DRV_I2C_TRANSFER_HANDLE * const transferHandle
    )

  Summary:
    Queues a write operation.

  Description:
    This function schedules a non-blocking write operation. The function returns
    with a valid transfer handle in the transferHandle argument if the write
    request was scheduled successfully. The function adds the request to the
    driver instance transfer queue and returns immediately. While the request is
    in the queue, the application buffer is owned by the driver and should not be
    modified.  On returning, the transferHandle parameter may be
    DRV_I2C_TRANSFER_HANDLE_INVALID for the following reasons:
    - if a transfer buffer could not be allocated to the request
    - if the input buffer pointer is NULL
    - if the buffer size is 0

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_I2C_TRANSFER_EVENT_COMPLETE event if the buffer was
    processed successfully or a DRV_I2C_TRANSFER_EVENT_ERROR event if the buffer
    was not processed successfully.

  Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine
    DRV_I2C_Open function.

    address - Slave address

    buffer - Data to be written.

    size - Transfer size in bytes.

    transferHandle - Pointer to an argument that will contain the return
    transfer handle. This will be DRV_I2C_TRANSFER_HANDLE_INVALID if the
    function was not successful.

  Returns:
    None.

  Example:
    <code>
    uint8_t myBuffer[MY_BUFFER_SIZE];
    DRV_I2C_TRANSFER_HANDLE transferHandle;

    // myI2CHandle is the handle returned
    // by the DRV_I2C_Open function.

    // slaveAddress is address of I2C slave device
    // to which data is to be written

    DRV_I2C_WriteTransferAdd(myI2CHandle, slaveAddress, myBuffer, MY_BUFFER_SIZE, &transferHandle);

    if(transferHandle == DRV_I2C_TRANSFER_HANDLE_INVALID)
    {
        // Error handling here
    }

    // Event is received when the buffer is processed.
    </code>

  Remarks:
    This function is thread safe in a RTOS application. It can be called from
    within the I2C Driver Transfer Event Handler that is registered by this
    client. It should not be called in the event handler associated with another
    I2C driver instance. It should not otherwise be called directly in an ISR.
    This function is available only in the asynchronous mode.

*/

void DRV_I2C_WriteTransferAdd(
    const DRV_HANDLE handle,
    const uint16_t address,
    void * const buffer,
    const size_t size,
    DRV_I2C_TRANSFER_HANDLE * const transferHandle
);

// *****************************************************************************
/* Function:
    void DRV_I2C_ForcedWriteTransferAdd(
        const DRV_HANDLE handle,
        const uint16_t address,
        void * const buffer,
        const size_t size,
        DRV_I2C_TRANSFER_HANDLE * const transferHandle
    )

  Summary:
    Queues a write operation.

  Description:
    I2C Master calls this function to transmit the entire buffer to the slave even
    if the slave ACKs or NACKs the address or any of the data bytes. This is
    typically used for slaves that have to initiate a reset sequence by sending
    a dummy I2C transaction. Since the slave is still in reset, any or all the
    bytes can be NACKed. In the normal operation of the driver if the address
    or data byte is NACKed, then the transmission is aborted and a STOP condition
    is asserted on the bus.

    This function schedules a non-blocking write operation. The function returns
    with a valid transfer handle in the transferHandle argument if the write
    request was scheduled successfully. The function adds the request to the
    driver instance transfer queue and returns immediately. While the request is
    in the queue, the application buffer is owned by the driver and should not be
    modified.  On returning, the transferHandle parameter may be
    DRV_I2C_TRANSFER_HANDLE_INVALID for the following reasons:
    - if a transfer buffer could not be allocated to the request
    - if the input buffer pointer is NULL
    - if the buffer size is 0

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_I2C_TRANSFER_EVENT_COMPLETE event if the buffer was
    processed successfully or a DRV_I2C_TRANSFER_EVENT_ERROR event if the buffer
    was not processed successfully.

  Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine
    DRV_I2C_Open function.

    address - Slave address

    buffer - Data to be written.

    size - Transfer size in bytes.

    transferHandle - Pointer to an argument that will contain the return
    transfer handle. This will be DRV_I2C_TRANSFER_HANDLE_INVALID if the
    function was not successful.

  Returns:
    None.

  Example:
    <code>
    uint8_t myBuffer[MY_BUFFER_SIZE];
    DRV_I2C_TRANSFER_HANDLE transferHandle;

    // myI2CHandle is the handle returned
    // by the DRV_I2C_Open function.

    // slaveAddress is address of I2C slave device
    // to which data is to be written

    DRV_I2C_ForcedWriteTransferAdd(myI2CHandle, slaveAddress, myBuffer, MY_BUFFER_SIZE, &transferHandle);

    if(transferHandle == DRV_I2C_TRANSFER_HANDLE_INVALID)
    {
        // Error handling here
    }

    // Event is received when the buffer is processed.
    </code>

  Remarks:
    This API must be used only if the underlying PLIB is enabled to generate the
    Forced write API. If the PLIB is not enabled to generate the Forced Write API,
    the API will return an invalid transfer handle (DRV_I2C_TRANSFER_HANDLE_INVALID).

    This function is thread safe in a RTOS application. It can be called from
    within the I2C Driver Transfer Event Handler that is registered by this
    client. It should not be called in the event handler associated with another
    I2C driver instance. It should not otherwise be called directly in an ISR.
    This function is available only in the asynchronous mode.

*/

void DRV_I2C_ForcedWriteTransferAdd(
    const DRV_HANDLE handle,
    const uint16_t address,
    void * const buffer,
    const size_t size,
    DRV_I2C_TRANSFER_HANDLE * const transferHandle
);

// *****************************************************************************
/* Function:
    void DRV_I2C_ReadTransferAdd(
        const DRV_HANDLE handle,
        const uint16_t address,
        void * const buffer,
        const size_t size,
        DRV_I2C_TRANSFER_HANDLE * const transferHandle
    )

  Summary:
    Queues a read operation.

  Description:
    This function schedules a non-blocking read operation. The function returns
    with a valid transfer handle in the transferHandle argument if the read request
    was scheduled successfully. The function adds the request to the driver
    instance transfer queue and returns immediately. While the request is in the
    queue, the application buffer is owned by the driver and should not be
    modified. The function returns DRV_I2C_TRANSFER_HANDLE_INVALID in the
    transferHandle argument:
    - if a buffer could not be allocated to the request
    - if the input buffer pointer is NULL
    - if the buffer size is 0
    - if the driver handle is invalid

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_I2C_TRANSFER_EVENT_COMPLETE event if the buffer was
    processed successfully of DRV_I2C_TRANSFER_EVENT_ERROR event if the buffer
    was not processed successfully.

  Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine
    DRV_I2C_Open function.

    address - Slave address

    buffer - buffer where the read data will be stored.

    size - Transfer size in bytes.

    transferHandle - Pointer to an argument that will contain the return
    transfer handle. This is DRV_I2C_TRANSFER_HANDLE_INVALID if the
    request was not successful.

  Returns:
     None

  Example:
    <code>
    uint8_t myBuffer[MY_BUFFER_SIZE];
    DRV_I2C_TRANSFER_HANDLE transferHandle;

    // myI2CHandle is the handle returned
    // by the DRV_I2C_Open function.

    // slaveAddress is address of I2C slave device
    // to which data is to be written

    DRV_I2C_ReadTransferAdd(myI2CHandle, slaveAddress, myBuffer, MY_BUFFER_SIZE, &transferHandle);

    if(transferHandle == DRV_I2C_TRANSFER_HANDLE_INVALID)
    {
        // Error handling here
    }

    // Event is received when the buffer is processed.
    </code>

  Remarks:
    This function is thread safe in a RTOS application. It can be called from
    within the I2C Driver Transfer Event Handler that is registered by the
    client. It should not be called in the event handler associated with another
    I2C driver instance. It should not be called directly in an ISR.
    This function is available only in the asynchronous mode.
*/

void DRV_I2C_ReadTransferAdd(
    const DRV_HANDLE handle,
    const uint16_t address,
    void * const buffer,
    const size_t size,
    DRV_I2C_TRANSFER_HANDLE * const transferHandle
);

// *****************************************************************************
/* Function:
    void DRV_I2C_WriteReadTransferAdd (
        const DRV_HANDLE handle,
        const uint16_t address,
        void * const writeBuffer,
        const size_t writeSize,
        void * const readBuffer,
        const size_t readSize,
        DRV_I2C_TRANSFER_HANDLE * const transferHandle
    )

  Summary:
    Queues a write followed by read operation.

  Description:
    This function schedules a non-blocking write followed by read operation. The
    function returns with a valid transfer handle in the transferHandle argument
    if the write request was scheduled successfully. The function adds the request
    to the driver instance transfer queue and returns immediately. While the
    request is in the queue, the application buffer is owned by the driver and
    should not be modified.  On returning, the transferHandle parameter may be
    DRV_I2C_TRANSFER_HANDLE_INVALID for the following reasons:
    - if a buffer could not be allocated to the request
    - if the input write or read buffer pointer is NULL
    - if the write or read buffer size is 0

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_I2C_TRANSFER_EVENT_COMPLETE event if the buffer was
    processed successfully or a DRV_I2C_TRANSFER_EVENT_ERROR event if the buffer
    was not processed successfully.

  Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine
    DRV_I2C_Open function.

    address - Slave address

    writeBuffer - Data to be written.

    writeSize - Size of write buffer in bytes.

    readBuffer - Buffer where data to be read is stored.

    readSize - Size of the read buffer in bytes.

    transferHandle - Pointer to an argument that will contain the return
    transfer handle. This will be DRV_I2C_TRANSFER_HANDLE_INVALID if the
    function was not successful.

  Returns:
    None.

  Example:
    <code>
    uint8_t myTxBuffer[MY_TX_BUFFER_SIZE];
    uint8_t myRxBuffer[MY_RX_BUFFER_SIZE];
    DRV_I2C_TRANSFER_HANDLE transferHandle;

    // myI2CHandle is the handle returned
    // by the DRV_I2C_Open function.

    // slaveAddress is address of I2C slave device
    // to which data is to be written

    DRV_I2C_WriteReadTransferAdd(myI2CHandle, slaveAddress, myTxBuffer, MY_TX_BUFFER_SIZE, myRxBuffer, MY_RX_BUFFER_SIZE, &transferHandle);

    if(transferHandle == DRV_I2C_TRANSFER_HANDLE_INVALID)
    {
        // Error handling here
    }

    // Event is received when the buffer is processed.
    </code>

  Remarks:
    This function is thread safe in a RTOS application. It can be called from
    within the I2C Driver Transfer Event Handler that is registered by this
    client. It should not be called in the event handler associated with another
    I2C driver instance. It should not otherwise be called directly in an ISR.
    This function is available only in the asynchronous mode.

*/

void DRV_I2C_WriteReadTransferAdd (
    const DRV_HANDLE handle,
    const uint16_t address,
    void * const writeBuffer,
    const size_t writeSize,
    void * const readBuffer,
    const size_t readSize,
    DRV_I2C_TRANSFER_HANDLE * const transferHandle
);

// *****************************************************************************
/* Function:
    void DRV_I2C_TransferEventHandlerSet
    (
        const DRV_HANDLE handle,
        const DRV_I2C_TRANSFER_EVENT_HANDLER eventHandler,
        const uintptr_t context
    )

  Summary:
    Allows a client to identify a transfer event handling function for the driver
    to call back when queued transfers have finished.

  Description:
    This function allows a client to register a transfer event handling function
    with the driver to call back when queued transfers have finished.
    When a client calls either the DRV_I2C_ReadTransferAdd, DRV_I2C_WriteTransferAdd
    or DRV_I2C_WriteReadTransferAdd function, it is provided with a handle identifying
    the transfer that was added to the driver's transfer queue. The driver will
    pass this handle back to the client by calling "eventHandler" function when
    the transfer has completed.

    The event handler should be set before the client performs any "transfer add"
    operations that could generate events. The event handler once set, persists
    until the client closes the driver or sets another event handler (which
    could be a "NULL" pointer to indicate no callback).

  Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine
    DRV_I2C_Open function.

    eventHandler - Pointer to the event handler function.

    context - The value of parameter will be passed back to the client
    unchanged, when the eventHandler function is called.  It can be used to
    identify any client specific data object that identifies the instance of the
    client module (for example, it may be a pointer to the client module's state
    structure).

  Returns:
    None.

  Example:
    <code>
    // myAppObj is an application specific state data object.
    MY_APP_OBJ myAppObj;

    uint8_t myBuffer[MY_BUFFER_SIZE];
    DRV_I2C_TRANSFER_HANDLE transferHandle;

    // myI2CHandle is the handle returned
    // by the DRV_I2C_Open function.

    // Client registers an event handler with driver. This is done once

    DRV_I2C_TransferEventHandlerSet( myI2CHandle, APP_I2CTransferEventHandler, (uintptr_t)&myAppObj );

    DRV_I2C_ReadTransferAdd(myI2CHandle, slaveAddress, myBuffer, MY_BUFFER_SIZE, &transferHandle);

    if(transferHandle == DRV_I2C_TRANSFER_HANDLE_INVALID)
    {
        // Error handling here
    }

    // The registered event handler is called when the transfer is completed.

    void APP_I2CTransferEventHandler(DRV_I2C_TRANSFER_EVENT event, DRV_I2C_TRANSFER_HANDLE handle, uintptr_t context)
    {
        // The context handle was set to an application specific
        // object. It is now retrievable easily in the event handler.
        MY_APP_OBJ* pMyAppObj = (MY_APP_OBJ *) context;

        switch(event)
        {
            case DRV_I2C_TRANSFER_EVENT_COMPLETE:
                // This means the data was transferred.
                break;

            case DRV_I2C_TRANSFER_EVENT_ERROR:
                // Error handling here.
                break;

            default:
                break;
        }
    }
    </code>

  Remarks:
    If the client does not want to be notified when the queued buffer transfer
    has completed, it does not need to register a callback. This function is
    thread safe when called in a RTOS application.
    This function is available only in the asynchronous mode.
*/

void DRV_I2C_TransferEventHandlerSet(
    const DRV_HANDLE handle,
    const DRV_I2C_TRANSFER_EVENT_HANDLER eventHandler,
    const uintptr_t context
);

// *****************************************************************************
/* Function:
    DRV_I2C_TRANSFER_EVENT DRV_I2C_TransferStatusGet(
        const DRV_I2C_TRANSFER_HANDLE transferHandle
    )

  Summary:
    Returns the status of the write/read/write-read transfer request.

  Description:
    This function can be used to poll the status of the queued transfer request
    if the application doesn't prefer to use the event handler (callback)
    function to get notified.

  Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

    Either the DRV_I2C_ReadTransferAdd, DRV_I2C_WriteTransferAdd or
    DRV_I2C_WriteReadTransferAdd function must have been called and a valid
    buffer handle returned.

  Parameters:
    transferHandle - Handle for the buffer of which the processed number of
    bytes to be obtained.

  Returns:
    The success or error event of the transfer.

  Example:
  <code>
    uint8_t myBuffer[MY_BUFFER_SIZE];
    DRV_I2C_TRANSFER_HANDLE transferHandle;
    DRV_I2C_TRANSFER_EVENT event;

    // myI2CHandle is the handle returned
    // by the DRV_I2C_Open function.

    DRV_I2C_ReadTransferAdd(myI2CHandle, slaveAddress, myBuffer, MY_BUFFER_SIZE, &transferHandle);

    if(transferHandle == DRV_I2C_TRANSFER_HANDLE_INVALID)
    {
        // Error handling here
    }

    //Check the status of the transfer
    //This call can be used to wait until the transfer is processed.

    event = DRV_I2C_TransferStatusGet(transferHandle);
  </code>

  Remarks:
    This function is available only in the asynchronous mode.
*/

DRV_I2C_TRANSFER_EVENT DRV_I2C_TransferStatusGet( const DRV_I2C_TRANSFER_HANDLE transferHandle );

// *****************************************************************************
// *****************************************************************************
// Section: I2C Driver Synchronous(Blocking Model) Transfer Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    bool DRV_I2C_WriteTransfer(
        const DRV_HANDLE handle,
        uint16_t address,
        void* const buffer,
        const size_t size
    )

  Summary:
    This is a blocking function that performs a I2C write operation.

  Description:
    This function does a blocking write operation. The function blocks till
    the write is complete or error has occurred during write. Function will
    return false to report failure. The failure will occur for the following
    reasons:
    - Invalid input parameters

  Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine
    DRV_I2C_Open function.

    address - Slave Address

    buffer - Source buffer containing data to be written.

    size - Size in bytes of data to be written.

  Returns:
    true - write is successful
    false - error has occurred

  Example:
    <code>
    uint8_t myTxBuffer[MY_TX_BUFFER_SIZE];

    // myI2CHandle is the handle returned
    // by the DRV_I2C_Open function.

    // slaveAddress is address of I2C slave device
    // to which data is to be written

    if (DRV_I2C_WriteTransfer(myI2CHandle, slaveAddress, myTxBuffer, MY_TX_BUFFER_SIZE) == false)
    {
        // Error handling here
    }

    </code>

  Remarks:
    This function is thread safe in a RTOS application.
    This function should not be called from an interrupt context.
    This function is available only in the synchronous mode.
*/

bool DRV_I2C_WriteTransfer(
    const DRV_HANDLE handle,
    uint16_t address,
    void* const buffer,
    const size_t size
);

// *****************************************************************************
/* Function:
    bool DRV_I2C_ForcedWriteTransfer(
        const DRV_HANDLE handle,
        uint16_t address,
        void* const buffer,
        const size_t size
    )

  Summary:
    This is a blocking function that performs a I2C write operation.

  Description:
    I2C Master calls this function to transmit the entire buffer to the slave even
    if the slave ACKs or NACKs the address or any of the data bytes. This is
    typically used for slaves that have to initiate a reset sequence by sending
    a dummy I2C transaction. Since the slave is still in reset, any or all the
    bytes can be NACKed. In the normal operation of the driver if the address
    or data byte is NACKed, then the transmission is aborted and a STOP condition
    is asserted on the bus.

    This function does a blocking write operation. The function blocks till
    the write is complete or error has occurred during write. Function will
    return false to report failure. The failure will occur for the following
    reasons:
    - Invalid input parameters

  Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine
    DRV_I2C_Open function.

    address - Slave Address

    buffer - Source buffer containing data to be written.

    size - Size in bytes of data to be written.

  Returns:
    true - write is successful
    false - error has occurred

  Example:
    <code>
    uint8_t myTxBuffer[MY_TX_BUFFER_SIZE];

    // myI2CHandle is the handle returned
    // by the DRV_I2C_Open function.

    // slaveAddress is address of I2C slave device
    // to which data is to be written

    if (DRV_I2C_ForcedWriteTransfer(myI2CHandle, slaveAddress, myTxBuffer, MY_TX_BUFFER_SIZE) == false)
    {
        // Error handling here
    }

    </code>

  Remarks:
    This API must be used only if the underlying PLIB is enabled to generate the
    Forced write API. If the PLIB is not enabled to generate the Forced Write API,
    the API will return false.

    This function is thread safe in a RTOS application.
    This function should not be called from an interrupt context.
    This function is available only in the synchronous mode.
*/

bool DRV_I2C_ForcedWriteTransfer(
    const DRV_HANDLE handle,
    uint16_t address,
    void* const buffer,
    const size_t size
);

// *****************************************************************************
/* Function:
    bool DRV_I2C_ReadTransfer(
        const DRV_HANDLE handle,
        uint16_t address,
        void* const buffer,
        const size_t size
    )

  Summary:
    This is a blocking function that performs a I2C read operation.

  Description:
    This function does a blocking read operation. The function blocks till
    the read is complete or error has occurred during read. Function will
    return false to report failure. The failure will occur for the following
    reasons:
    - Invalid input parameters
    - Hardware error

    Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine
    DRV_I2C_Open function.

    address - Slave Address

    buffer - Destination buffer where read data is stored.

    size - Size in bytes of data to be read.

  Returns:
    true - read is successful
    false - error has occurred

  Example:
    <code>
    uint8_t myRxBuffer[MY_RX_BUFFER_SIZE];

    // myI2CHandle is the handle returned
    // by the DRV_I2C_Open function.

    // slaveAddress is address of I2C slave device
    // to which data is to be written

    if (DRV_I2C_ReadTransfer(myI2CHandle, slaveAddress, myRxBuffer, MY_RX_BUFFER_SIZE) == false)
    {
        // Error handling here
    }

    </code>

  Remarks:
    This function is thread safe in a RTOS application.
    This function should not be called from an interrupt context.
    This function is available only in the synchronous mode.
*/

bool DRV_I2C_ReadTransfer(
    const DRV_HANDLE handle,
    uint16_t address,
    void* const buffer,
    const size_t size
);

// *****************************************************************************
/* Function:
    bool DRV_I2C_WriteReadTransfer (
        const DRV_HANDLE handle,
        uint16_t address,
        void* const writeBuffer,
        const size_t writeSize,
        void* const readBuffer,
        const size_t readSize
    )

  Summary:
    This is a blocking function that performs a I2C write followed by a I2C read operation.

  Description:
    This function does a blocking write and read operation. The function blocks till
    the write and read is complete or error has occurred during data transfer. Function will
    return false to report failure. The failure will occur for the following
    reasons:
    - Invalid input parameters
    - Hardware error

  Precondition:
    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine
    DRV_I2C_Open function.

    address - Slave Address

    writeBuffer - Source buffer containing data to be written.

    writeSize - Size in bytes of data to be written.

    readBuffer - Destination buffer where read data is stored.

    readSize - Size in bytes of data to be read.

  Returns:
    true - transfer is successful
    false - error has occurred

  Example:
    <code>
    uint8_t myTxBuffer[MY_TX_BUFFER_SIZE];
    uint8_t myRxBuffer[MY_RX_BUFFER_SIZE];

    // myI2CHandle is the handle returned
    // by the DRV_I2C_Open function.

    // slaveAddress is address of I2C slave device
    // to which data is to be written

    if (DRV_I2C_WriteReadTransfer(myI2CHandle, slaveAddress, myTxBuffer, MY_TX_BUFFER_SIZE, myRxBuffer, MY_RX_BUFFER_SIZE) == false)
    {
        // Error handling here
    }

    </code>

  Remarks:
    This function is thread safe in a RTOS application.
    This function should not be called from an interrupt context.
    This function is available only in the synchronous mode.
*/

bool DRV_I2C_WriteReadTransfer (
    const DRV_HANDLE handle,
    uint16_t address,
    void* const writeBuffer,
    const size_t writeSize,
    void* const readBuffer,
    const size_t readSize
);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#include "driver/i2c/src/drv_i2c_local.h"

#endif // #ifndef DRV_I2C_H