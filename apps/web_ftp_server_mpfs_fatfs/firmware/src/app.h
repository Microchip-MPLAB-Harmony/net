/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

#if defined (__PIC32C__) || defined(__SAMA5D2__)        
/* This section is highly customizable based on application's specific needs. */
#define APP_SWITCH_1StateGet()      SWITCH_Get()
#define APP_SWITCH_2StateGet()      SWITCH_Get()
#define APP_SWITCH_3StateGet()      SWITCH_Get()
#elif defined (__PIC32MZ__)
/* This section is highly customizable based on application's specific needs. */
#define APP_SWITCH_1StateGet()      SWITCH1_Get()
#define APP_SWITCH_2StateGet()      SWITCH2_Get()
#define APP_SWITCH_3StateGet()      SWITCH2_Get()
#endif

#define APP_LED_1StateSet()         LED1_Set()
#define APP_LED_1StateGet()         LED1_Get()
#define APP_LED_1StateClear()       LED1_Clear()
#define APP_LED_1StateToggle()      LED1_Toggle()

#if defined(__PIC32MZ__)
#define APP_LED_2StateSet()         LED2_Set()
#define APP_LED_2StateGet()         LED2_Get()
#define APP_LED_2StateClear()       LED2_Clear()
#define APP_LED_2StateToggle()      LED2_Toggle()

#define APP_LED_3StateSet()         LED3_Set()
#define APP_LED_3StateGet()         LED3_Get()
#define APP_LED_3StateClear()       LED3_Clear()
#define APP_LED_3StateToggle()      LED3_Toggle()
#endif

// Application SYS_FS mount points
// Adjust as needed
#define APP_SYS_FS_SD_VOL                   "/dev/mmcblka1"
#define APP_SYS_FS_SDCARD_MOUNT_POINT       "/mnt/mchpDrive1"
#define APP_SYS_FS_SDCARD_TYPE              FAT
#define APP_SYS_FS_SDCARD_TYPE_STRING       "FATFS"
    
#define APP_SYS_FS_NVM_VOL                  "/dev/nvma1"
#define APP_SYS_FS_NVM_MOUNT_POINT          "/mnt/mchpDrive2"
#define APP_SYS_FS_NVM_TYPE                 MPFS2
#define APP_SYS_FS_NVM_TYPE_STRING          "MPFS"

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application States

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* The application mounts the disk. */
    /* The app mounts the disk */
    APP_MOUNT_DISK_MEDIA_NVM = 0,

    /* The app mounts the disk */
    APP_MOUNT_DISK_MEDIA_SD,

    /* In this state, the application waits for the initialization of the TCP/IP stack
       to complete. */
    APP_TCPIP_WAIT_INIT,

    /* In this state, the application can do TCP/IP transactions. */
    APP_TCPIP_TRANSACT,

    /* The application waits in this state for the driver to be ready
       before sending the "hello world" message. */
    //APP_STATE_WAIT_FOR_READY,

    /* The application waits in this state for the driver to finish
       sending the message. */
    //APP_STATE_WAIT_FOR_DONE,

    /* The application does nothing in the idle state. */
    APP_STATE_IDLE,

    //
    APP_USERIO_LED_DEASSERTED,

    APP_USERIO_LED_ASSERTED,

    APP_TCPIP_ERROR,

} APP_STATES;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_STATES state;

    TCPIP_FTP_HANDLE    ftpHandle;        
    
    /* TODO: Define any additional data used by the application. */

} APP_DATA;

// *****************************************************************************
/* LED State

  Summary:
    Enumerates the supported LED states.

  Description:
    This enumeration defines the supported LED states.

  Remarks:
    None.
*/
typedef enum
{
    /* LED State is on */
    APP_LED_STATE_OFF = 0,
   /* LED State is off */
    APP_LED_STATE_ON = 1,
} APP_LED_STATE;


// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_Tasks ( void );

// *****************************************************************************
// *****************************************************************************
// Section: External Declarations
// *****************************************************************************
// *****************************************************************************

// HTTP application processing
#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)
extern void HTTP_APP_Initialize(void);
#endif // defined(TCPIP_STACK_USE_HTTP_NET_SERVER)

#endif /* _APP_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

