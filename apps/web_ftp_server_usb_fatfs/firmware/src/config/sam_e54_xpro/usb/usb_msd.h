/*******************************************************************************
  USB MSD class definitions

  Company:
    Microchip Technology Inc.

  File Name:
    usb_msd.h

  Summary:
    USB MSD class definitions

  Description:
    This file describes the MSD class specific definitions. This file is
    included by usb_device_msd.h and usb_host_msd.h header files. File needs to
    be included by the application for USB MSD host/device functionality.
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
// DOM-IGNORE-END

#ifndef _USB_MSD_H
#define _USB_MSD_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Constants
// *****************************************************************************
// *****************************************************************************

#define USB_MSD_CLASS_CODE                                  0x08
#define USB_MSD_SUBCLASS_CODE_SCSI_TRANSPARENT_COMMAND_SET  0x06 
#define USB_MSD_PROTOCOL                                    0x50
#define USB_MSD_REQUEST_CLASS_SPECIFIC                      0x21
#define USB_MSD_REQUEST_ENDPONT_SPECIFIC                    0x02

/* This is a useful constant */
#define USB_MSD_CBW_DIRECTION_BITMASK                   0x80

/* Maximum size of the command block in the CBW */
#define USB_MSD_MAX_CB_SIZE 0x10    

/* Bit mask to set bits 6-0 of the bmCBWFlags to zero */
#define USB_MSD_CBWFLAGS_RESERVED_BITS_MASK    0x7F 

/* MSD CBW Signature */
#define USB_MSD_VALID_CBW_SIGNATURE ((uint32_t)0x43425355)

/* MSD CSW Signature */
#define USB_MSD_VALID_CSW_SIGNATURE ((uint32_t)0x53425355)

// *****************************************************************************
/* MSD Command Status Wrapper Status

  Summary:
    Identifies the possible values of the bCSWStatus field in the CSW.

  Description:
    Identifies the possible values of the bCSWStatus field in the CSW.

  Remarks:
    None.
*/

typedef enum
{
    USB_MSD_CSW_COMMAND_PASSED  = 0x00,
    USB_MSD_CSW_COMMAND_FAILED  = 0x01,
    USB_MSD_CSW_PHASE_ERROR     = 0x02

} USB_MSD_CSW_STATUS;

// *****************************************************************************
/* USB MSD Command.

  Summary:
    Identified the USB MSD Commands used by the stack.

  Description:
    Identified the USB MSD Commands used by the stack.

  Remarks:
    None.
*/

typedef enum
{
    USB_MSD_RESET       = 0xFF,
    USB_MSD_GET_MAX_LUN = 0xFE

} USB_MSD_COMMAND;

// *****************************************************************************
/* USB MSD Command Block Wrapper structure.

  Summary:
    USB MSD Command Block Wrapper structure.

  Description:
    USB MSD Command Block Wrapper structure.

  Remarks:
    None.
*/

typedef struct __attribute__((packed)) 
{
    /* CBW Signature always 55 53 42 43h */
    uint32_t dCBWSignature;

    /* Sent by host, device echoes this value in CSW (associated a CSW with a CBW) */
    uint32_t dCBWTag;                    

    /* Number of bytes of data host expects to transfer */
    uint32_t dCBWDataTransferLength;     
   
    /* Specified the direction of the data transport phase */ 
    union
    {
        uint8_t value;
        struct __attribute__((packed))
        {
            uint8_t            :7;
            uint8_t direction  :1;
        }bits;
    } bmCBWFlags;

    /* Specifies the target LUN */ 
    uint8_t bCBWLUN;                    
    
    /* Length of the command descriptor block */
    uint8_t bCBWCBLength;                
    
    /* Command block to be executed by the device */
    uint8_t CBWCB[16];                    

} USB_MSD_CBW;

// *****************************************************************************
/* USB device MSD CSW structure.

  Summary:
   CSW structure.

  Description:
   CSW structure.

  Remarks:
    This structure is private to USB MSD function driver.
*/

typedef struct __attribute__((packed))            
{
    /* CSW signature. Always 55 53 42 53h */
    uint32_t dCSWSignature;            

    /* The value of the dCBWTag in the CBW packet*/
    uint32_t dCSWTag;                

    /* Difference in data expected (dCBWDataTransferLength) and 
       actual amount processed/sent */
    uint32_t dCSWDataResidue;

    /* Status */    
    uint8_t bCSWStatus;                
    
} USB_MSD_CSW;



//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif
