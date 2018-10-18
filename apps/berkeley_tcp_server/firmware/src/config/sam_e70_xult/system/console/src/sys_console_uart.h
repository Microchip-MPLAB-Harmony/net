/*******************************************************************************
  Console System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_console_uart_local.h

  Summary:
    Console System Service local declarations and definitions for uart cdc I/O
    device.

  Description:
    This file contains the Console System Service local declarations and
    definitions for uart I/O device.
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


#ifndef SYS_CONSOLE_UART_H
#define SYS_CONSOLE_UART_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "sys_console_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* UART State Machine States

   Summary
    Defines the various states that can be achieved by the driver operation.

   Description
    This enumeration defines the various states that can be achieved by the
    driver operation.

   Remarks:
    None.
*/

typedef enum
{
    /* Application opens and attaches the device here */
    CONSOLE_UART_STATE_INIT = 0,

    /* Application waits for device configuration*/
    CONSOLE_UART_STATE_WAIT_FOR_CONFIGURATION,

    /* The application checks if a switch was pressed */
    //CONSOLE_UART_STATE_CHECK_SWITCH_PRESSED,
    CONSOLE_UART_STATE_READY,

    /* Wait for a character receive */
    CONSOLE_UART_STATE_SCHEDULE_READ,

    /* A character is received from host */
    CONSOLE_UART_STATE_WAIT_FOR_READ_COMPLETE,

    /* Wait for the TX to get completed */
    CONSOLE_UART_STATE_SCHEDULE_WRITE,

    /* Wait for the write to complete */
    CONSOLE_UART_STATE_WAIT_FOR_WRITE_COMPLETE,

    /* Application Critcal Error state*/
    CONSOLE_UART_STATE_CRITICAL_ERROR,

    /* Application Operational Error state*/
    CONSOLE_UART_STATE_OPERATIONAL_ERROR
} CONSOLE_UART_STATE;


typedef struct
{
    /* Application's current state*/
    CONSOLE_UART_STATE state;

    /* Read Data Buffer */
    uint8_t readBuffer[64];

    /* Break data */
    uint16_t breakData;

    /* True if a character was read */
    bool isReadComplete;

    /* True if a character was written*/
    bool isWriteComplete;

    /* Set when an attempt is made to push to a full write queue */
    bool overflowFlag;

    /* Called when read queue is emptied */
    void (*rdCallback)(void *handle);

    /* Called when write is completed */
    void (*wrCallback)(void *handle);

} CONS_UART_DATA;

struct QPacket
{
    union
    {
        const void *cbuf;
        void *buf;
    } data;
    size_t sz;
};


struct QueueNode
{
    uint32_t tailPos;
    uint32_t nextPos;
    uint32_t numElem;
    struct QPacket *qPkts;
    uint32_t elemArrSz;
};

// *****************************************************************************
// *****************************************************************************
// Section: Extern data Definitions
// *****************************************************************************
// *****************************************************************************

void Console_UART_RegisterCallback(consoleCallbackFunction consCallback, SYS_CONSOLE_EVENT event);


#endif //#ifndef SYS_CONSOLE_UART_H

/*******************************************************************************
 End of File
*/

