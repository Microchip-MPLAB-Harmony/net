/*******************************************************************************
  User Configuration Header

  File Name:
    user.h

  Summary:
    Build-time configuration header for the user defined by this project.

  Description:
    An MPLAB Project may have multiple configurations.  This file defines the
    build-time options for a single configuration.

  Remarks:
    It only provides macro definitions for build-time configuration options

*******************************************************************************/

#ifndef USER_H
#define USER_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: User Configuration macros
// *****************************************************************************
// *****************************************************************************

#define SWITCH_Get()        PB_USER_Get()
#define SWITCH_Get()        PB_USER_Get()
#define SWITCH_Get()        PB_USER_Get()

#define LED1_Get()          LED_GREEN_Get()
#define LED1_Set()          LED_GREEN_On()
#define LED1_Clear()        LED_GREEN_Off()
#define LED1_Toggle()       LED_GREEN_Toggle()

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // USER_H
/*******************************************************************************
 End of File
*/
