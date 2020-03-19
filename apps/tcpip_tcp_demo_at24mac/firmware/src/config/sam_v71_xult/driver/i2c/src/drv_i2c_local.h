/*******************************************************************************
  I2C Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_local.h

  Summary:
    I2C Driver Local Data Structures

  Description:
    Driver Local Data Structures
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

#ifndef _DRV_I2C_LOCAL_H
#define _DRV_I2C_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "driver/i2c/drv_i2c_definitions.h"
#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

/* I2C Driver Handle Macros*/
#define DRV_I2C_INDEX_MASK                      (0x000000FF)

#define DRV_I2C_INSTANCE_MASK                   (0x0000FF00)

#define DRV_I2C_TOKEN_MAX                       (0xFFFF)

// *****************************************************************************
/* I2C Transfer Object Flags

  Summary:
    Defines the I2C Transfer Object Flags.

  Description:
    This enumeration defines transfer type (read/write) of the I2C Buffer Object.

  Remarks:
    None.
*/

typedef enum
{
    /* Indicates this buffer was submitted by a read function */
    DRV_I2C_TRANSFER_OBJ_FLAG_READ = 1 << 0,

    /* Indicates this buffer was submitted by a write function */
    DRV_I2C_TRANSFER_OBJ_FLAG_WRITE = 1 << 1,

    /* Indicates this buffer was submitted by a write followed by read function */
    DRV_I2C_TRANSFER_OBJ_FLAG_WRITE_READ = 1 << 2,

    /* Indicates this buffer was submitted by a force write function */
    DRV_I2C_TRANSFER_OBJ_FLAG_WRITE_FORCED = 1 << 3,

} DRV_I2C_TRANSFER_OBJ_FLAGS;

// *****************************************************************************
/* I2C Transfer Object State

  Summary:
    Defines the status of the I2C Transfer Object.

  Description:
    This enumeration defines the status of the I2C Transfer Object.

  Remarks:
    None.
*/

typedef enum
{
    DRV_I2C_TRANSFER_OBJ_IS_FREE,

    DRV_I2C_TRANSFER_OBJ_IS_IN_QUEUE,

    DRV_I2C_TRANSFER_OBJ_IS_PROCESSING,

}DRV_I2C_TRANSFER_OBJ_STATE;

// *****************************************************************************
/* I2C Driver Transfer Object

  Summary:
    Object used to keep track of a client's buffer.

  Description:
    None.

  Remarks:
    None.
*/

typedef struct _DRV_I2C_TRANSFER_OBJ
{
    /* True if object is allocated */
    bool                            inUse;

    /* Slave address */
    uint16_t                        slaveAddress;

    /* Pointer to the application read buffer */
    void*                           readBuffer;

    /* Number of bytes to be read */
    size_t                          readSize;

    /* Pointer to the application write buffer */
    void*                           writeBuffer;

    /* Number of bytes to be written */
    size_t                          writeSize;

    /* Transfer Object Flag */
    DRV_I2C_TRANSFER_OBJ_FLAGS      flag;

    /* Current status of the buffer */
    DRV_I2C_TRANSFER_EVENT          event;

    /* Handle to the client that owns this buffer object */
    DRV_HANDLE                      clientHandle;

    /* Current state of the buffer */
    DRV_I2C_TRANSFER_OBJ_STATE      currentState;

    /* Buffer Handle object that was assigned to this buffer when it was added to the
     * queue. */
    DRV_I2C_TRANSFER_HANDLE         transferHandle;

    /* Errors associated with the I2C transfer */
    volatile DRV_I2C_ERROR          errors;

    /* Next buffer pointer */
    struct _DRV_I2C_TRANSFER_OBJ*   next;

} DRV_I2C_TRANSFER_OBJ;

// *****************************************************************************
/* I2C Driver Instance Object

  Summary:
    Object used to keep any data required for an instance of the I2C driver.

  Description:
    None.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag to indicate this object is in use  */
    bool                        inUse;

    /* The status of the driver */
    SYS_STATUS                  status;

    /* Flag to indicate that driver has been opened Exclusively*/
    bool                        isExclusive;

    /* Keep track of the number of clients that have opened this driver */
    size_t                      nClients;

    /* Maximum number of clients */
    size_t                      nClientsMax;

    /* PLIB API list that will be used by the driver to access the hardware */
    const DRV_I2C_PLIB_INTERFACE* i2cPlib;

    /* Saves the initial value of the I2C clock speed which is assigned to a client when it opens the I2C driver */
    uint32_t                    initI2CClockSpeed;

    /* Current transfer setup will be used to verify change in the transfer setup by client */
    DRV_I2C_TRANSFER_SETUP      currentTransferSetup;

    /* Interrupt Sources of I2C */
    const DRV_I2C_INTERRUPT_SOURCES* interruptSources;

    /* Memory pool for Client Objects */
    uintptr_t                   clientObjPool;

    /* Size of transfer objects queue */
    uint32_t                    transferObjPoolSize;

    /* Pointer to the transfer pool */
    DRV_I2C_TRANSFER_OBJ*       transferObjPool;

    /* Linked list of transfer objects */
    DRV_I2C_TRANSFER_OBJ*       transferObjList;

    /* Instance specific token counter used to generate unique client/transfer handles */
    uint16_t                    i2cTokenCount;

    /* Count to keep track of interrupt nesting */
    uint32_t                    interruptNestingCount;

    /* client array protection mutex */
    OSAL_MUTEX_DECLARE (mutexClientObjects);

    /* transfer onject projection mutex */
    OSAL_MUTEX_DECLARE (mutexTransferObjects);

    bool                                    i2cInt0Status;

    bool                                    i2cInt1Status;

    bool                                    i2cInt2Status;

    bool                                    i2cInt3Status;

    bool                                    i2cInterruptStatus;

} DRV_I2C_OBJ;

// *****************************************************************************
/* I2C Driver Client Object

  Summary:
    Object used to track a single client.

  Description:
    This object is used to keep the data necesssary to keep track of a single
    client.

  Remarks:
    None.
*/

typedef struct
{
    /* The hardware instance object associated with the client */
    uint32_t                        drvIndex;

    /* The IO intent with which the client was opened */
    DRV_IO_INTENT                   ioIntent;

    /* This flags indicates if the object is in use or is available */
    bool                            inUse;

    /* Event handler for this function */
    DRV_I2C_TRANSFER_EVENT_HANDLER  eventHandler;

    /* Application Context associated with this client */
    uintptr_t                       context;

    /* Client handle assigned to this client object when it was opened */
    DRV_HANDLE                      clientHandle;

    /* Client specific transfer setup */
    DRV_I2C_TRANSFER_SETUP          transferSetup;

} DRV_I2C_CLIENT_OBJ;

#endif //#ifndef _DRV_I2C_LOCAL_H
