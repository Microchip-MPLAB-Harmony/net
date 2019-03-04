/*******************************************************************************
  XDMAC Peripheral Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    plib_xdmac_common.h

  Summary
    XDMAC peripheral library interface.

  Description
    This file defines the interface to the XDMAC peripheral library.  This
    library provides access to and control of the XDMAC controller.

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

#ifndef PLIB_XDMAC_COMMON_H    // Guards against multiple inclusion
#define PLIB_XDMAC_COMMON_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/*  This section lists the other files that are included in this file.
*/

#include <stddef.h>

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
/* XDMAC Transfer Events

   Summary:
    Enumeration of possible XDMAC transfer events.

   Description:
    This data type provides an enumeration of all possible XDMAC transfer
    events.

   Remarks:
    None.

*/
typedef enum
{
    /* Data was transferred successfully. */
    XDMAC_TRANSFER_COMPLETE,

    /* Error while processing the request */
    XDMAC_TRANSFER_ERROR

} XDMAC_TRANSFER_EVENT;


// *****************************************************************************
/* DMA Channel Settings

  Summary:
    Defines the channel settings.

  Description:
    This data type defines the channel settings and can be used to update the
    channel settings dynamically .

  Remarks:
    This feature may not be available on all devices. Refer to the specific
    device data sheet to determine availability.
*/

typedef uint32_t XDMAC_CHANNEL_CONFIG;


// *****************************************************************************
/* DMA descriptor control

  Summary:
    Defines the descriptor control for linked list operation.

  Description:
    This data type defines the descriptor control for linked list operation.
    Descriptor control always applies to the next descriptor in the chain.
    That is, descriptor control parameters defined in descriptor 'n' applies to
    the transfer of descriptor "n+1".

  Remarks:
    This feature may not be available on all devices. Refer to the specific
    device data sheet to determine availability.
*/

typedef union
{
    struct
    {
        /* Descriptor fetch enable.
           Zero in this field indicates the end of linked list. */
        uint8_t fetchEnable:1;

        /* Enable/Disable source address update when the descriptor
           is retrieved*/
        uint8_t sourceUpdate:1;

        /* Enable/Disable destination address update when the descriptor
           is retrieved*/
        uint8_t destinationUpdate:1;

        /* Descriptor view type.
           Views can be changed when switching descriptors. */
        uint8_t view:2;

        /* Reserved */
        uint8_t :3;
    };

    uint8_t descriptorControl;

}XDMAC_DESCRIPTOR_CONTROL;


// *****************************************************************************
/* DMA Micro Block Control

  Summary:
    Defines the control parameters for linked list operation.

  Description:
    This data type defines the control parameters for linked list operation.
    Block length applies to the current descriptor and XDMAC_DESCRIPTOR_CONTROL
    applies to the next descriptor.

  Remarks:
    This feature may not be available on all devices. Refer to the specific
    device data sheet to determine availability.
*/

typedef struct {

    /* Size of block for the current descriptor. */
    uint32_t blockDataLength:24;

    /* Next Descriptor Control Setting */
    XDMAC_DESCRIPTOR_CONTROL nextDescriptorControl;

} XDMAC_MICRO_BLOCK_CONTROL;


// *****************************************************************************
/* DMA descriptor views

  Summary:
    Defines the different descriptor views available for master transfer.

  Description:
    This data type defines the different descriptor views available.

  Remarks:
    This feature may not be available on all devices. Refer to the specific
    device data sheet to determine availability.
*/

/* View 0 */
typedef struct {

    /* Next Descriptor Address number. */
    uint32_t mbr_nda;

    /* Micro-block Control Member. */
    XDMAC_MICRO_BLOCK_CONTROL mbr_ubc;

    /* Destination Address Member. */
    uint32_t mbr_da;

} XDMAC_DESCRIPTOR_VIEW_0;

/* View 1 */
typedef struct {

    /* Next Descriptor Address number. */
    uint32_t mbr_nda;

    /* Micro-block Control Member. */
    XDMAC_MICRO_BLOCK_CONTROL mbr_ubc;

    /* Source Address Member. */
    uint32_t mbr_sa;

    /* Destination Address Member. */
    uint32_t mbr_da;

} XDMAC_DESCRIPTOR_VIEW_1;

/* View 2 */
typedef struct {

    /* Next Descriptor Address number. */
    uint32_t mbr_nda;

    /* Micro-block Control Member. */
    XDMAC_MICRO_BLOCK_CONTROL mbr_ubc;

    /* Source Address Member. */
    uint32_t mbr_sa;

    /* Destination Address Member. */
    uint32_t mbr_da;

    /* Configuration Register. */
    /* TODO: Redefine type to XDMAC_CC white updating to N type */
    uint32_t mbr_cfg;

} XDMAC_DESCRIPTOR_VIEW_2;

/* View 3 */
typedef struct {

    /* Next Descriptor Address number. */
    uint32_t mbr_nda;

    /* Micro-block Control Member. */
    XDMAC_MICRO_BLOCK_CONTROL mbr_ubc;

    /* Source Address Member. */
    uint32_t mbr_sa;

    /* Destination Address Member. */
    uint32_t mbr_da;

    /* Configuration Register. */
    uint32_t mbr_cfg;

    /* Block Control Member. */
    uint32_t mbr_bc;

    /* Data Stride Member. */
    uint32_t mbr_ds;

    /* Source Micro-block Stride Member. */
    uint32_t mbr_sus;

    /* Destination Micro-block Stride Member. */
    uint32_t mbr_dus;

} XDMAC_DESCRIPTOR_VIEW_3;


// *****************************************************************************
/* XDMAC Transfer Event Handler Function

   Summary:
    Pointer to a XDMAC Transfer Event handler function.

   Description:
    This data type defines a XDMAC Transfer Event Handler Function.

    A XDMAC PLIB client must register a transfer event handler function of this
    type to receive transfer related events from the PLIB.

    If the event is XDMAC_TRANSFER_EVENT_COMPLETE, this means that the data
    was transferred successfully.

    If the event is XDMAC_TRANSFER_EVENT_ERROR, this means that the data was
    not transferred successfully.

    The contextHandle parameter contains the context handle that was provided by
    the client at the time of registering the event handler. This context handle
    can be anything that the client consider helpful or necessary to identify
    the client context object associated with the channel of the XDMAC PLIB that
    generated the event.

    The event handler function executes in an interrupt context of XDMAC.
    It is recommended to the application not to perform process intensive
    operations with in this function.

   Remarks:
    None.

*/
typedef void (*XDMAC_CHANNEL_CALLBACK) (XDMAC_TRANSFER_EVENT event, uintptr_t contextHandle);



// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //PLIB_XDMAC_COMMON_H
