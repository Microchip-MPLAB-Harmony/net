/*******************************************************************************
  SYS UART CONSOLE Support Layer

  File Name:
    sys_console_uart.c

  Summary:
    SYS UART CONSOLE Support Layer

  Description:
    This file contains the SYS UART CONSOLE support layer logic.
 *******************************************************************************/


// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2018 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/console/sys_console.h"
#include "sys_console_uart.h"
#include "configuration.h"
#include "peripheral/usart/plib_usart1.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Variable Definitions
// *****************************************************************************
// *****************************************************************************

SYS_CONSOLE_DEV_DESC consUsartDevDesc =
{
    .consoleDevice = SYS_CONSOLE_DEV_USART,
    .intent = DRV_IO_INTENT_READWRITE,
    .sysConsoleRead = Console_UART_Read,
    .sysConsoleReadC = Console_UART_ReadC,
    .sysConsoleWrite = Console_UART_Write,
    .sysConsoleRegisterCallback = Console_UART_RegisterCallback,
    .sysConsoleTasks = Console_UART_Tasks,
    .sysConsoleStatus = Console_UART_Status,
    .sysConsoleFlush = Console_UART_Flush
};

static struct QPacket wrQueueElements[SYS_CONSOLE_UART_WR_QUEUE_DEPTH];
static struct QPacket rdQueueElements[SYS_CONSOLE_UART_RD_QUEUE_DEPTH];

static struct QueueNode writeQueue = {0, 0, 0, wrQueueElements, SYS_CONSOLE_UART_WR_QUEUE_DEPTH};
static struct QueueNode readQueue = {0, 0, 0, rdQueueElements, SYS_CONSOLE_UART_RD_QUEUE_DEPTH};

static void pushQueue(struct QueueNode *q, struct QPacket pkt)
{
    q->qPkts[q->nextPos] = pkt;
    (q->nextPos < (q->elemArrSz - 1)) ? q->nextPos++ : (q->nextPos = 0);
    q->numElem++;
}

static void popQueue(struct QueueNode *q)
{
    (q->tailPos < (q->elemArrSz - 1)) ? q->tailPos++ : (q->tailPos = 0);
    q->numElem--;
}

CONS_UART_DATA consUartData =
{
    /* console state */
    .state = CONSOLE_UART_STATE_INIT,

    /* Intialize the read complete flag */
    .isReadComplete = true,

    /* Initialize the write complete flag*/
    .isWriteComplete = true,

    .overflowFlag = false,

    .rdCallback = NULL,

    .wrCallback = NULL
};

SYS_CONSOLE_STATUS Console_UART_Status(void)
{
    SYS_CONSOLE_STATUS status = SYS_CONSOLE_STATUS_NOT_CONFIGURED;

    if (consUartData.state == CONSOLE_UART_STATE_INIT)
        return status;

    if (consUartData.state == CONSOLE_UART_STATE_CRITICAL_ERROR || consUartData.state == CONSOLE_UART_STATE_OPERATIONAL_ERROR || consUartData.overflowFlag)
    {
        status = SYS_CONSOLE_STATUS_ERROR;
    }
    else
    {
        status = (writeQueue.numElem || readQueue.numElem) ? SYS_CONSOLE_STATUS_BUSY : SYS_CONSOLE_STATUS_CONFIGURED;
    }
    return status;
}

ssize_t Console_UART_Write(int fd, const void *buf, size_t count)
{
    struct QPacket pkt;

    pkt.data.cbuf = buf;
    pkt.sz = count;

    //Pop the most recent if the queue is full
    if (writeQueue.numElem >= writeQueue.elemArrSz)
    {
        popQueue(&writeQueue);
    }

    pushQueue(&writeQueue, pkt);

    return count;
}

ssize_t Console_UART_Read(int fd, void *buf, size_t count)
{
    struct QPacket pkt;
    size_t rdBytes = 0;

    pkt.data.buf = buf;
    pkt.sz = 1;

    while ((readQueue.numElem < readQueue.elemArrSz) && (rdBytes < count))
    {
        pushQueue(&readQueue, pkt);
        pkt.data.buf = (char*)pkt.data.buf + 1;
        rdBytes++;
    }

    return rdBytes;
}

char Console_UART_ReadC(int fd)
{
    char readBuffer;

    USART1_Read(&readBuffer, 1);

    while (USART1_ReadCountGet() == 0);

    return readBuffer;
}

void Console_UART_RegisterCallback(consoleCallbackFunction consCallback, SYS_CONSOLE_EVENT event)
{
    switch (event)
    {
        case SYS_CONSOLE_EVENT_READ_COMPLETE:
            consUartData.rdCallback = consCallback;
            break;
        case SYS_CONSOLE_EVENT_WRITE_COMPLETE:
            consUartData.wrCallback = consCallback;
            break;
        default:
            break;
    }
}

void Console_UART_Flush(void)
{
    if (consUartData.state != CONSOLE_UART_STATE_INIT)
    {
        consUartData.state = CONSOLE_UART_STATE_READY;
    }

    consUartData.overflowFlag = false;

    consUartData.isWriteComplete = true;
    writeQueue.nextPos = 0;
    writeQueue.tailPos = 0;
    writeQueue.numElem = 0;

    consUartData.isReadComplete = true;
    readQueue.nextPos = 0;
    readQueue.tailPos = 0;
    readQueue.numElem = 0;
}

void UARTDeviceWriteCallback(uintptr_t context)
{
    consUartData.isWriteComplete = true;
}

void UARTDeviceReadCallback(uintptr_t context)
{
    if ( USART1_ErrorGet() == 0)
    {
        consUartData.isReadComplete = true;
    }
}

void Console_UART_Tasks(SYS_MODULE_OBJ object)
{
    /* Update the application state machine based
     * on the current state */
    struct QPacket pkt;
    size_t *sizeRead;

    switch(consUartData.state)
    {
        case CONSOLE_UART_STATE_INIT:

            /* Register a callback with device layer to get event notification (for end point 0) */
            USART1_WriteCallbackRegister(UARTDeviceWriteCallback, 0);
            USART1_ReadCallbackRegister(UARTDeviceReadCallback, 0);

            /* If the driver was opened, it is ready for operation */
            consUartData.state = CONSOLE_UART_STATE_READY;

            break;

        case CONSOLE_UART_STATE_SCHEDULE_READ:

            consUartData.state = CONSOLE_UART_STATE_WAIT_FOR_READ_COMPLETE;

            if (readQueue.numElem)
            {
                consUartData.isReadComplete = false;
                pkt = rdQueueElements[readQueue.tailPos];

                USART1_Read (pkt.data.buf, pkt.sz);
            }
            break;

        case CONSOLE_UART_STATE_WAIT_FOR_READ_COMPLETE:

            if(consUartData.isReadComplete)
            {
                sizeRead = &rdQueueElements[readQueue.tailPos].sz;
                popQueue(&readQueue);
                if (readQueue.numElem == 0)
                {
                    if (consUartData.rdCallback != NULL)
                    {
                        consUartData.rdCallback(sizeRead);
                    }
                    consUartData.state = CONSOLE_UART_STATE_READY;
                }
                else
                {
                    consUartData.state = CONSOLE_UART_STATE_SCHEDULE_READ;
                }
            }
            else
            {
                if (writeQueue.numElem)
                {
                    consUartData.state = CONSOLE_UART_STATE_SCHEDULE_WRITE;
                }
            }
            break;

        case CONSOLE_UART_STATE_READY:

            if (readQueue.numElem)
            {
                consUartData.state = CONSOLE_UART_STATE_SCHEDULE_READ;
            }
            else if (writeQueue.numElem)
            {
                /* If there is data to be written, then try writing it */
                consUartData.state = CONSOLE_UART_STATE_SCHEDULE_WRITE;
            }
            break;

        case CONSOLE_UART_STATE_SCHEDULE_WRITE:

            if (writeQueue.numElem)
            {
                /* This means there is data to sent out */
                pkt = wrQueueElements[writeQueue.tailPos];

                // Use write with callback version for buffer size greater than 1
                do
                {
                    /* This loop will NOT run forever. It will run till
                     * either there are elements in the write or till USART
                     * driver buffer queue is full */

                    bool status = USART1_Write(pkt.data.buf, pkt.sz);

                    if(status == true)
                    {
                        /* This means this buffer was added successfully to
                         * the PLIB. Do a callback. */
                        if (consUartData.wrCallback != NULL)
                        {
                            consUartData.wrCallback((void *)wrQueueElements[writeQueue.tailPos].data.cbuf);
                        }

                        /* Remove this element from the write queue */
                        popQueue(&writeQueue);
                        pkt = wrQueueElements[writeQueue.tailPos];
                    }
                    else
                    {
                        /* Got an invalid handle. This most likely means
                         * that the write buffer queue is full */
                    }

                } while(writeQueue.numElem);

                /* If we are here, it either means there is nothing more to
                 * write of the write buffer queue is full. We cannot do much
                 * with respect to write at point. We move the state to
                 * ready */

                consUartData.state = CONSOLE_UART_STATE_READY;
            }
            break;

        case CONSOLE_UART_STATE_OPERATIONAL_ERROR:

            /* We arrive at this state if the UART driver reports an error on a read or write operation
               We will attempt to recover by flushing the local buffers */

            Console_UART_Flush();

            break;

        case CONSOLE_UART_STATE_CRITICAL_ERROR:
            break;
        default:
            break;
    }
}

/*******************************************************************************
 End of File
 */
