/*******************************************************************************
  TCP/IP modules manager file

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_module_manager.h

  Summary:
    Internal TCP/IP stack module manager file
    
  Description:
    This header file contains the function prototypes and definitions of the 
    TCP/IP stack manager services
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef __TCPIP_MODULE_MANAGER_H_
#define __TCPIP_MODULE_MANAGER_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// definitions
// 


// ************* stack supported modules and their attached functionality **************
// 

// ******* table with TCPIP stack modules **************
// We use directly the functions names (pointers) rather than providing registration functions
// so that we can create this table in const space
//

// initialization function
// if the module has initialization to do, this function
// will be called. It should return a result to indicate
// if the initialization was successful. If not, the
// interface will not be completed.
typedef bool    (*tcpipModuleInitFunc)(const TCPIP_STACK_MODULE_CTRL* const, const void* );

// de-initialization function
// if the module needs to clean up when the module is
// brought down, this function will be called. It should
// return a result to indicate that everything has been
// cleaned up.
typedef void    (*tcpipModuleDeInitFunc)(const TCPIP_STACK_MODULE_CTRL * const);


// descriptor of an TCPIP stack module entry
// module that's part of the stack
// each module has an ID and init/deinit functions
// 
typedef struct
{
    TCPIP_STACK_MODULE       moduleId;           // module identification
    tcpipModuleInitFunc      initFunc;           // initialization function
#if (TCPIP_STACK_DOWN_OPERATION != 0)
    tcpipModuleDeInitFunc    deInitFunc;         // deinitialization function
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
}TCPIP_STACK_MODULE_ENTRY;



// *********** a stack module exposes an signal handler ****************


typedef struct
{
    tcpipModuleSignalHandler    signalHandler;      // signal handler; regular ModuleTask() function
                                                    // if NULL, the corresponding entry is not used
    TCPIP_MODULE_SIGNAL_FUNC    userSignalF;        // external user signal function                                                    

    uint16_t                    signalVal;          // TCPIP_MODULE_SIGNAL: current signal value;
    int16_t                     asyncTmo;           // module required timeout, msec; 
                                                    // the stack manager checks that the module reached its timeout
    int16_t                     currTmo;            // current module timeout, msec; maintained by the stack manager
    uint16_t                    signalParam;        // some signals have parameters
                                                    // for TCPIP_MODULE_SIGNAL_INTERFACE_CHANGE
                                                    // this is the interface mask: 1 << ifx 

}TCPIP_MODULE_SIGNAL_ENTRY;


// Connection event handler definition.
// The stack calls the handler when a new connection event occurs.
// Note that this call will carry only connection events!
typedef void    (*tcpipModuleConnHandler)(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT connEvent);


// function for getting the MAC module of an interface
static __inline__ const TCPIP_MAC_OBJECT* __attribute__((always_inline))  _TCPIP_STACK_GetMacObject(TCPIP_NET_IF* pNetIf)
{
    return pNetIf ? pNetIf->pMacObj : 0;
}

// function for getting the MAC handle of an interface
static __inline__ SYS_MODULE_OBJ __attribute__((always_inline))  _TCPIP_STACK_GetMacObjectHandle(TCPIP_NET_IF* pNetIf)
{
    return (pNetIf && pNetIf->pMacObj) ? pNetIf->macObjHandle : 0;
}

// function for getting the MAC client handle of an interface
static __inline__ TCPIP_MAC_HANDLE __attribute__((always_inline))  _TCPIP_STACK_GetMacClientHandle(TCPIP_NET_IF* pNetIf)
{
    return pNetIf ? pNetIf->hIfMac : 0;
}

#endif //  __TCPIP_MODULE_MANAGER_H_








