/*******************************************************************************
  AT25DF Driver Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_at25df_definitions.h

  Summary:
    AT25DF Driver Definitions Header File

  Description:
    This file provides implementation-specific definitions for the AT25DF
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

#ifndef DRV_AT25DF_DEFINITIONS_H
#define DRV_AT25DF_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <device.h>
#include "system/ports/sys_ports.h"


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

typedef void (* DRV_AT25DF_PLIB_CALLBACK)( uintptr_t );

typedef bool (* DRV_AT25DF_PLIB_WRITE_READ)(void*, size_t, void *, size_t);

typedef bool (* DRV_AT25DF_PLIB_WRITE)(void*, size_t);

typedef bool (* DRV_AT25DF_PLIB_READ)(void*, size_t);

typedef bool (* DRV_AT25DF_PLIB_IS_BUSY)(void);

typedef void (* DRV_AT25DF_PLIB_CALLBACK_REGISTER)(DRV_AT25DF_PLIB_CALLBACK, uintptr_t);

// *****************************************************************************
/* AT25DF Driver PLIB Interface Data

  Summary:
    Defines the data required to initialize the AT25DF driver PLIB Interface.

  Description:
    This data type defines the data required to initialize the AT25DF driver
    PLIB Interface.

  Remarks:
    None.
*/

typedef struct
{

    /* AT25DF PLIB writeRead API */
    DRV_AT25DF_PLIB_WRITE_READ                writeRead;

    /* AT25DF PLIB write API */
    DRV_AT25DF_PLIB_WRITE                     write;

    /* AT25DF PLIB read API */
    DRV_AT25DF_PLIB_READ                      read;

    /* AT25DF PLIB Transfer status API */
    DRV_AT25DF_PLIB_IS_BUSY                   isBusy;

    /* AT25DF PLIB callback register API */
    DRV_AT25DF_PLIB_CALLBACK_REGISTER         callbackRegister;

} DRV_AT25DF_PLIB_INTERFACE;

// *****************************************************************************
/* AT25DF Driver Initialization Data

  Summary:
    Defines the data required to initialize the AT25DF driver

  Description:
    This data type defines the data required to initialize or the AT25DF driver.

  Remarks:
    None.
*/

typedef struct
{
    /* Identifies the PLIB API set to be used by the driver to access the
     * peripheral. */
    const DRV_AT25DF_PLIB_INTERFACE*    spiPlib;

    /* Number of clients */
    size_t                              numClients;

    SYS_PORT_PIN                        chipSelectPin;

    /* Page size (in Bytes) of the FLASH */
    uint32_t                            pageSize;

    /* Total size (in Bytes) of the FLASH */
    uint32_t                            flashSize;

    uint32_t                            blockStartAddress;

} DRV_AT25DF_INIT;


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef DRV_AT25DF_DEFINITIONS_H

/*******************************************************************************
 End of File
*/
