/*******************************************************************************
  I2C Driver Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_definitions.h

  Summary:
    I2C Driver Definitions Header File

  Description:
    This file provides implementation-specific definitions for the I2C
    driver's system interface.
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

#ifndef DRV_I2C_DEFINITIONS_H
#define DRV_I2C_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "stddef.h"
#include "system/system_module.h"
#include "driver/driver.h"
#include "driver/driver_common.h"
#include "system/int/sys_int.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

#ifndef SYS_DEBUG
    #define SYS_DEBUG(x, y)
#endif
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* I2C Driver Transfer Setup Data

  Summary:
    Defines the data required to setup the I2C transfer

  Description:
    This data type defines the data required to setup the I2C transfer. The
    data is passed to the DRV_I2C_TransferSetup API to setup the I2C peripheral
    settings dynamically.

  Remarks:
    None.
*/

typedef struct
{
    /* clock speed */
    uint32_t clockSpeed;

} DRV_I2C_TRANSFER_SETUP;

// *****************************************************************************
/* I2C Driver Error

  Summary:
    Defines the error values

  Description:
    This data type defines the error values for the errors occured during transfer.

  Remarks:
    None.
*/

typedef enum
{
    /* Busy*/
    DRV_I2C_ERROR_NONE,

    /* Transfer Successful */
    DRV_I2C_ERROR_NACK,

} DRV_I2C_ERROR;


typedef void (* DRV_I2C_PLIB_CALLBACK)( uintptr_t );

typedef bool (* DRV_I2C_PLIB_READ)( uint16_t, uint8_t *, uint32_t );

typedef bool (* DRV_I2C_PLIB_WRITE)( uint16_t, uint8_t *, uint32_t );


typedef bool (* DRV_I2C_PLIB_WRITE_READ)( uint16_t, uint8_t *, uint32_t, uint8_t *, uint32_t );

typedef DRV_I2C_ERROR (* DRV_I2C_PLIB_ERROR_GET)( void );

typedef bool (* DRV_I2C_PLIB_TRANSFER_SETUP)(DRV_I2C_TRANSFER_SETUP* setup, uint32_t srcClkFreq);

typedef void (* DRV_I2C_PLIB_CALLBACK_REGISTER)(DRV_I2C_PLIB_CALLBACK, uintptr_t);

typedef struct
{
    int32_t         i2cInt0;
    int32_t         i2cInt1;
    int32_t         i2cInt2;
    int32_t         i2cInt3;
}DRV_I2C_MULTI_INT_SRC;

typedef union
{
    DRV_I2C_MULTI_INT_SRC           multi;
    int32_t                         i2cInterrupt;
}DRV_I2C_INT_SRC;

typedef struct
{
    bool                            isSingleIntSrc;
    DRV_I2C_INT_SRC                 intSources;
}DRV_I2C_INTERRUPT_SOURCES;

// *****************************************************************************
/* I2C Driver PLib Interface Data

  Summary:
    Defines the data required to initialize the I2C driver PLib Interface.

  Description:
    This data type defines the data required to initialize the I2C driver PLib
    Interface. If the driver is built statically, the members of this data
    structure are statically over-ridden by static override definitions in the
    configuration.h file.

  Remarks:
    None.
*/

typedef struct
{
    /* I2C PLib read API */
    DRV_I2C_PLIB_READ                           read;

    /* I2C PLib write API */
    DRV_I2C_PLIB_WRITE                          write;


    /* I2C PLib writeRead API */
    DRV_I2C_PLIB_WRITE_READ                     writeRead;

    /* I2C PLib transfer */
    DRV_I2C_PLIB_ERROR_GET                      errorGet;

    /* I2C PLib Transfer Setup function */
    DRV_I2C_PLIB_TRANSFER_SETUP                 transferSetup;

    /* I2C PLib callback register API */
    DRV_I2C_PLIB_CALLBACK_REGISTER              callbackRegister;

} DRV_I2C_PLIB_INTERFACE;

// *****************************************************************************
/* I2C Driver Initialization Data

  Summary:
    Defines the data required to initialize the I2C driver

  Description:
    This data type defines the data required to initialize or the I2C driver.
    If the driver is built statically, the members of this data structure are
    statically over-ridden by static override definitions in the
    configuration.h file.

  Remarks:
    None.
*/

typedef struct
{
    /* Identifies the PLIB API set to be used by the driver to access the
     * peripheral. */
    const DRV_I2C_PLIB_INTERFACE*           i2cPlib;

    /* Memory Pool for Client Objects */
    uintptr_t                               clientObjPool;

    /* Number of clients */
    uint32_t                                numClients;

    /* Size of buffer objects queue */
    uint32_t                                transferObjPoolSize;

    /* Pointer to the buffer pool */
    uintptr_t                               transferObjPool;


    const DRV_I2C_INTERRUPT_SOURCES*        interruptSources;

    /* peripheral clock speed */
    uint32_t                                clockSpeed;

} DRV_I2C_INIT;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef DRV_I2C_DEFINITIONS_H
