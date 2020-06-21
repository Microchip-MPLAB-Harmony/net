/*******************************************************************************
  USB Driver Feature Variant Implementations

  Company:
    Microchip Technology Inc.

  File Name:
    drv_USBFSV1_variant_mapping.h

  Summary:
    USB Driver Feature Variant Implementations

  Description:
    This file implements the functions which differ based on different parts
    and various implementations of the same feature.
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

#ifndef _DRV_USBFSV1_VARIANT_MAPPING_H
#define _DRV_USBFSV1_VARIANT_MAPPING_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************


#include "configuration.h"

/**********************************************
 * Macro Mapping
 **********************************************/

/* SAME5x Devices have USB PADCAL values in SW0_ADDR */ 
#define DRV_USBFSV1_READ_PADCAL_VALUE (*((uint32_t *) SW0_ADDR + 1))

/* SAME5x Family Devices has Four interrupt vectors for USB module */ 
#define DRV_USBFSV1_MULTIPLE_ISR_AVAILABLE true
  
#if (DRV_USBFSV1_MULTIPLE_ISR_AVAILABLE == true)
    #define _DRV_USBFSV1_SYS_INT_SourceEnable(a, b, c, d);          \
            SYS_INT_SourceEnable(a);                                \
            SYS_INT_SourceEnable(b);                                \
            SYS_INT_SourceEnable(c);                                \
            SYS_INT_SourceEnable(d);

    #define _DRV_USBFSV1_SYS_INT_SourceStatusClear(a, b, c, d);     \
            SYS_INT_SourceStatusClear(a);                           \
            SYS_INT_SourceStatusClear(b);                           \
            SYS_INT_SourceStatusClear(c);                           \
            SYS_INT_SourceStatusClear(d);

    #define _DRV_USBFSV1_SYS_INT_SourceDisable(a, b, c, d);         \
            SYS_INT_SourceDisable(a);                               \
            SYS_INT_SourceDisable(b);                               \
            SYS_INT_SourceDisable(c);                               \
            SYS_INT_SourceDisable(d);

    #define _DRV_USBFSV1_SYS_INT_SourceDisableSave(w, a, x, b, y, c, z, d);     \
            w = SYS_INT_SourceDisable(a);                                       \
            x = SYS_INT_SourceDisable(b);                                       \
            y = SYS_INT_SourceDisable(c);                                       \
            z = SYS_INT_SourceDisable(d);

    #define _DRV_USBFSV1_SYS_INT_SourceEnableRestore(w, a, x, b, y, c, z, d);   \
            if(w == true)   SYS_INT_SourceEnable(a);                            \
            if(x == true)   SYS_INT_SourceEnable(b);                            \
            if(y == true)   SYS_INT_SourceEnable(c);                            \
            if(z == true)   SYS_INT_SourceEnable(d);                            \

    #define _DRV_USBFSV1_DECLARE_BOOL_VARIABLE(a);                  \
            bool a = false;                                         \
            bool a##1 = false;                                      \
            bool a##2 = false;                                      \
            bool a##3 = false;


#elif (DRV_USBFSV1_MULTIPLE_ISR_AVAILABLE == false)
    #define _DRV_USBFSV1_SYS_INT_SourceEnable(a, b, c, d);          \
            SYS_INT_SourceEnable(a);

    #define _DRV_USBFSV1_SYS_INT_SourceStatusClear(a, b, c, d);     \
            SYS_INT_SourceStatusClear(a);

    #define _DRV_USBFSV1_SYS_INT_SourceDisable(a, b, c, d);         \
            SYS_INT_SourceStatusClear(a);            

    #define _DRV_USBFSV1_SYS_INT_SourceDisableSave(w, a, x, b, y, c, z, d);     \
            w = SYS_INT_SourceDisable(a);

    #define _DRV_USBFSV1_SYS_INT_SourceEnableRestore(w, a, x, b, y, c, z, d);   \
            if(w == true)   SYS_INT_SourceEnable(a);

    #define _DRV_USBFSV1_DECLARE_BOOL_VARIABLE(a);                              \
            bool a = false;                                                     \

#endif

#if defined(DRV_USB_INSTANCES_NUMBER)
    #define DRV_USBFSV1_INSTANCES_NUMBER  DRV_USB_INSTANCES_NUMBER
#endif

#if defined(DRV_USB_DEVICE_SUPPORT)
    #define DRV_USBFSV1_DEVICE_SUPPORT  DRV_USB_DEVICE_SUPPORT
#endif

#if defined(DRV_USB_HOST_SUPPORT)
    #define DRV_USBFSV1_HOST_SUPPORT  DRV_USB_HOST_SUPPORT
#endif

#if defined(DRV_USB_ENDPOINTS_NUMBER)
    #define DRV_USBFSV1_ENDPOINTS_NUMBER  DRV_USB_ENDPOINTS_NUMBER
#endif

/**********************************************
 * Sets up driver mode-specific init routine
 * based on selected support.
 *********************************************/

#ifndef DRV_USBFSV1_DEVICE_SUPPORT
    #error "DRV_USBFSV1_DEVICE_SUPPORT must be defined and be either true or false"
#endif

#ifndef DRV_USBFSV1_HOST_SUPPORT
    #error "DRV_USBFSV1_HOST_SUPPORT must be defined and be either true or false"
#endif

    #define _DRV_USBFSV1_ISR(x)                 DRV_USBFSV1_Tasks_ISR(x)
    #define _DRV_USBFSV1_ISR_OTHER(x)           DRV_USBFSV1_Tasks_ISR(x)
    #define _DRV_USBFSV1_ISR_SOF_HSOF(x)        DRV_USBFSV1_Tasks_ISR(x)
    #define _DRV_USBFSV1_ISR_TRCPT0(x)          DRV_USBFSV1_Tasks_ISR(x)
    #define _DRV_USBFSV1_ISR_TRCPT1(x)          DRV_USBFSV1_Tasks_ISR(x)

#if (DRV_USBFSV1_DEVICE_SUPPORT == true)
    #define _DRV_USBFSV1_DEVICE_INIT(x, y)      _DRV_USBFSV1_DEVICE_Initialize(x , y)
    #define _DRV_USBFSV1_DEVICE_TASKS_ISR(x)    _DRV_USBFSV1_DEVICE_Tasks_ISR(x)
    #define _DRV_USBFSV1_FOR_DEVICE(x, y)       x y
    
#elif (DRV_USBFSV1_DEVICE_SUPPORT == false)
    #define _DRV_USBFSV1_DEVICE_INIT(x, y)  
    #define _DRV_USBFSV1_DEVICE_TASKS_ISR(x) 
    #define _DRV_USBFSV1_FOR_DEVICE(x, y)
#endif
 
#if (DRV_USBFSV1_HOST_SUPPORT == true)
    #define _DRV_USBFSV1_HOST_INIT(x, y)    _DRV_USBFSV1_HOST_Initialize(x , y)
    #define _DRV_USBFSV1_HOST_TASKS_ISR(x)  _DRV_USBFSV1_HOST_Tasks_ISR(x)
    #define _DRV_USBFSV1_HOST_ATTACH_DETACH_STATE_MACHINE(x)  _DRV_USBFSV1_HOST_AttachDetachStateMachine(x)
    #define _DRV_USBFSV1_FOR_HOST(x, y)     x y

    #define max(x, y) ((x) > (y) ? (x) : (y))
    #define min(x, y) ((x) > (y) ? (y) : (x))
    #if (defined __GNUC__) || (defined __CC_ARM)
        #define clz(u) __builtin_clz(u)
    #else
        static __INLINE uint32_t clz(uint32_t data)
        {
            uint32_t count = 0;
            uint32_t mask = 0x80000000;

            while((data & mask) == 0)
            {
                count += 1u;
                mask = mask >> 1u;
            }
            return (count);
        }
    #endif

#elif (DRV_USBFSV1_HOST_SUPPORT == false)
    #define _DRV_USBFSV1_HOST_INIT(x, y)
    #define _DRV_USBFSV1_HOST_TASKS_ISR(x) 
    #define _DRV_USBFSV1_HOST_ATTACH_DETACH_STATE_MACHINE(x)
    #define _DRV_USBFSV1_FOR_HOST(x, y)
#endif

#endif
