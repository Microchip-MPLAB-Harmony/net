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
/* I2C Buffer Object Flags

  Summary:
    Defines the I2C Buffer Object Flags.

  Description:
    This enumeration defines the I2C Buffer Object flags.

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
            
} DRV_I2C_TRANSFER_OBJ_FLAGS;

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
    /* Transfer Object array index */
    uint32_t index;
    
    /* Slave address */
    uint16_t slaveAddress;
    
    /* Pointer to the application read buffer */
    void * readBuffer;

    /* Number of bytes to be read */
    size_t readSize;
    
    /* Pointer to the application write buffer */
    void * writeBuffer;
    
    /* Number of bytes to be written */
    size_t writeSize;
    
    /* Transfer Object Flag */
    DRV_I2C_TRANSFER_OBJ_FLAGS flag;
    
    /* Current status of the buffer */
    DRV_I2C_TRANSFER_EVENT event;

    /* The client object that owns this buffer */
    void * hClient;
    
    /* Buffer Handle object that was assigned to this buffer when it was added to the
     * queue. */
    DRV_I2C_TRANSFER_HANDLE transferHandle;
    
    /* Next buffer pointer */
    struct _DRV_I2C_TRANSFER_OBJ * next;

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
    bool inUse;
    
    /* Flag to indicate that driver has been opened Exclusively*/
    bool isExclusive;
    
    /* Keep track of the number of clients 
      that have opened this driver */
    size_t nClients;
    
    /* Maximum number of clients */
    size_t nClientsMax;
    
    /* The status of the driver */
    SYS_STATUS status;
       
    /* PLIB API list that will be used by the driver to access the hardware */
    DRV_I2C_PLIB_INTERFACE *i2cPlib;

    /* Interrupt Source of I2C */
    IRQn_Type interruptI2C;
    
    /* Memory pool for Client Objects */
    uintptr_t clientObjPool;
    
    /* Pointer to Transfer Objects array */
    DRV_I2C_TRANSFER_OBJ * trObjArr;
    
    /* Pointer to free list of Transfer Objects */
    DRV_I2C_TRANSFER_OBJ * trObjFree;

    /* The transfer Q head pointer */
    DRV_I2C_TRANSFER_OBJ * trQueueHead;
    
    /* The transfer Q tail pointer */
    DRV_I2C_TRANSFER_OBJ * trQueueTail;
    
    /* Transfer Queue Size */
    size_t trQueueSize;
    
    /* token for creating transfer handle */
    uint16_t tokenCount;
    
    /* client array protection mutex */
    OSAL_MUTEX_DECLARE (mutexClientObjects);
    
    /* transfer onject projection mutex */
    OSAL_MUTEX_DECLARE (mutexTransferObjects);
    
    /* Count to keep track of interrupt nesting */ 
    uint32_t interruptNestingCount;

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
    uint32_t                       drvIndex;

    /* The IO intent with which the client was opened */
    DRV_IO_INTENT                  ioIntent;
    
    /* Errors associated with the I2C transfer */
    DRV_I2C_ERROR                  errors;

    /* This flags indicates if the object is in use or is
     * available */
    bool                           inUse;

    /* Event handler for this function */
    DRV_I2C_TRANSFER_EVENT_HANDLER eventHandler;

    /* Application Context associated with this client */
    uintptr_t                      context;
    


} DRV_I2C_CLIENT_OBJ;

#endif //#ifndef _DRV_I2C_LOCAL_H
