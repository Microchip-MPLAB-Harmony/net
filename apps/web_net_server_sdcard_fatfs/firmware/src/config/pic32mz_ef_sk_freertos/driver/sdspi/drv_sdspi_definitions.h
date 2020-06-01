/*******************************************************************************
  SD Card (SPI) Driver Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdspi_definitions.h

  Summary:
    SD Card (SPI) Driver Definitions Header File

  Description:
    This file provides implementation-specific definitions for the SD Card (SPI)
    driver's system interface.
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef DRV_SDSPI_DEFINITIONS_H
#define DRV_SDSPI_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/ports/sys_ports.h"
#include "system/dma/sys_dma.h"

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
typedef enum
{
    DRV_SDSPI_CLOCK_PHASE_VALID_TRAILING_EDGE = 0,
    DRV_SDSPI_CLOCK_PHASE_VALID_LEADING_EDGE = 1,

    /* Force the compiler to reserve 32-bit memory space for each enum */
    DRV_SDSPI_CLOCK_PHASE_INVALID = 0xFFFFFFFF

} DRV_SDSPI_CLOCK_PHASE;

typedef enum
{
    DRV_SDSPI_CLOCK_POLARITY_IDLE_LOW = 0,
    DRV_SDSPI_CLOCK_POLARITY_IDLE_HIGH = 1,

    /* Force the compiler to reserve 32-bit memory space for each enum */
    DRV_SDSPI_CLOCK_POLARITY_INVALID = 0xFFFFFFFF

} DRV_SDSPI_CLOCK_POLARITY;

typedef enum
{
    DRV_SDSPI_DATA_BITS_8 = 0,
    DRV_SDSPI_DATA_BITS_9 = 1,
    DRV_SDSPI_DATA_BITS_10 = 2,
    DRV_SDSPI_DATA_BITS_11 = 3,
    DRV_SDSPI_DATA_BITS_12 = 4,
    DRV_SDSPI_DATA_BITS_13 = 5,
    DRV_SDSPI_DATA_BITS_14 = 6,
    DRV_SDSPI_DATA_BITS_15 = 7,
    DRV_SDSPI_DATA_BITS_16 = 8,

    /* Force the compiler to reserve 32-bit memory space for each enum */
    DRV_SDSPI_DATA_BITS_INVALID = 0xFFFFFFFF

} DRV_SDSPI_DATA_BITS;

typedef enum
{
    DRV_SDSPI_CS_POLARITY_ACTIVE_LOW = 0,
    DRV_SDSPI_CS_POLARITY_ACTIVE_HIGH = 1

} DRV_SDSPI_CS_POLARITY;

// *****************************************************************************
/* SDSPI Driver Setup Data

  Summary:
    Defines the data required to setup the SPI transfer

  Description:
    This data type defines the data required to setup the SPI transfer. The
    data is passed to the DRV_SDSPI_TransferSetup API to setup the SPI peripheral
    settings dynamically.

  Remarks:
    None.
*/

typedef struct
{
    uint32_t                          baudRateInHz;

    DRV_SDSPI_CLOCK_PHASE             clockPhase;

    DRV_SDSPI_CLOCK_POLARITY          clockPolarity;

    DRV_SDSPI_DATA_BITS               dataBits;

    SYS_PORT_PIN                      chipSelect;

    DRV_SDSPI_CS_POLARITY             csPolarity;

} DRV_SDSPI_TRANSFER_SETUP;

typedef    void (* DRV_SDSPI_PLIB_CALLBACK)( uintptr_t );

typedef    bool (* DRV_SDSPI_PLIB_WRITEREAD)(void*, size_t, void *, size_t);

typedef    bool (* DRV_SDSPI_PLIB_WRITE)(void*, size_t);

typedef    bool (* DRV_SDSPI_PLIB_READ)(void*, size_t);

typedef    bool (* DRV_SDSPI_PLIB_SETUP)(DRV_SDSPI_TRANSFER_SETUP * setup, uint32_t spiSourceClock);

typedef    bool (* DRV_SDSPI_PLIB_IS_BUSY)(void);

typedef    void (* DRV_SDSPI_PLIB_CALLBACK_REGISTER)(DRV_SDSPI_PLIB_CALLBACK, uintptr_t);

// *****************************************************************************
/* SDSPI Driver PLIB Interface Data

  Summary:
    Defines the data required to initialize the SDSPI driver PLIB Interface.

  Description:
    This data type defines the data required to initialize the SDSPI driver
    PLIB Interface.

  Remarks:
    None.
*/

typedef struct
{
    /* SDSPI PLIB writeRead API */
    DRV_SDSPI_PLIB_WRITEREAD               writeRead;

    /* SDSPI PLIB write API */
    DRV_SDSPI_PLIB_WRITE                   write;

    /* SDSPI PLIB read API */
    DRV_SDSPI_PLIB_READ                    read;

    DRV_SDSPI_PLIB_SETUP                   transferSetup;

    /* SDSPI PLIB Transfer status API */
    DRV_SDSPI_PLIB_IS_BUSY                 isBusy;

    /* SDSPI PLIB callback register API */
    DRV_SDSPI_PLIB_CALLBACK_REGISTER       callbackRegister;

} DRV_SDSPI_PLIB_INTERFACE;


// *****************************************************************************
/* SDSPI Driver Initialization Data

  Summary:
    Defines the data required to initialize the SDSPI driver

  Description:
    This data type defines the data required to initialize or the SDSPI driver.

  Remarks:
    None.
*/

typedef struct
{
    /* Identifies the PLIB API set to be used by the driver to access the
     * peripheral. */
    const DRV_SDSPI_PLIB_INTERFACE*       spiPlib;

    const uint32_t*                 remapDataBits;

    const uint32_t*                 remapClockPolarity;

    const uint32_t*                 remapClockPhase;

    /* Transmit DMA Channel */
    SYS_DMA_CHANNEL                 txDMAChannel;

    /* Receive DMA Channel */
    SYS_DMA_CHANNEL                 rxDMAChannel;

    /* This is the SPI transmit register address. Used for DMA operation. */
    void*                           txAddress;

    /* This is the SPI receive register address. Used for DMA operation. */
    void*                           rxAddress;


    bool                            isFsEnabled;

    /* Number of clients */
    size_t                          numClients;

    /* Memory Pool for Client Objects */
    uintptr_t                       clientObjPool;

    SYS_PORT_PIN                    chipSelectPin;

    SYS_PORT_PIN                    writeProtectPin;

    uint32_t                        blockStartAddress;

    /* Speed at which SD card communication should happen */
    uint32_t                        sdcardSpeedHz;

    uint32_t                        pollingIntervalMs;


} DRV_SDSPI_INIT;


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef DRV_SDSPI_DEFINITIONS_H

/*******************************************************************************
 End of File
*/
