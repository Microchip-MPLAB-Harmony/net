/*******************************************************************************
  UART1 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_uart1.c

  Summary:
    UART1 PLIB Implementation File

  Description:
    None

*******************************************************************************/

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

#include "device.h"
#include "plib_uart1.h"

// *****************************************************************************
// *****************************************************************************
// Section: UART1 Implementation
// *****************************************************************************
// *****************************************************************************

UART_OBJECT uart1Obj;

void static UART1_ErrorClear( void )
{
    uint8_t dummyData = 0u;
    /* rxBufferLen = (FIFO level + RX register) */
    uint8_t rxBufferLen = UART_RXFIFO_DEPTH;

    /* If it's a overrun error then clear it to flush FIFO */
    if(U1STA & _U1STA_OERR_MASK)
    {
        U1STACLR = _U1STA_OERR_MASK;
    }

    /* Read existing error bytes from FIFO to clear parity and framing error flags */
    while(U1STA & (_U1STA_FERR_MASK | _U1STA_PERR_MASK))
    {
        dummyData = (uint8_t )(U1RXREG );
        rxBufferLen--;

        /* Try to flush error bytes for one full FIFO and exit instead of
         * blocking here if more error bytes are received */
        if(0u ==  rxBufferLen)
        {
            break;
        }
    }

    // Ignore the warning
    (void)dummyData;

    /* Clear error interrupt flag */
    IFS3CLR = _IFS3_U1EIF_MASK;

    /* Clear up the receive interrupt flag so that RX interrupt is not
     * triggered for error bytes */
    IFS3CLR = _IFS3_U1RXIF_MASK;

    return;
}

void UART1_Initialize( void )
{
    /*Set up UxMODE bits */
    /*STSEL  = 0*/
    /*PDSEL = 0 */

    U1MODE = 0x8;

    /*Enable UUART1 Receiver and Transmitter */
    U1STASET = (_U1STA_UTXEN_MASK | _U1STA_URXEN_MASK);

    /* BAUD Rate register Setup */
    U1BRG = 216;

    /*Setup UART1_FAULT Interrupt*/
    /*Disable UART1_FAULT Interrupt*/
    IEC3CLR = _IEC3_U1EIE_MASK;

    /*Setup UART1_RX Interrupt*/
    /*Disable UART1_RX Interrupt*/
    IEC3CLR = _IEC3_U1RXIE_MASK;

    /*Setup UART1_TX Interrupt*/
    /*Disable UART1_TX Interrupt*/
    IEC3CLR = _IEC3_U1TXIE_MASK;

    /* Initialize instance object */
    uart1Obj.rxBuffer = NULL;
    uart1Obj.rxSize = 0;
    uart1Obj.rxProcessedSize = 0;
    uart1Obj.rxBusyStatus = false;
    uart1Obj.rxCallback = NULL;
    uart1Obj.txBuffer = NULL;
    uart1Obj.txSize = 0;
    uart1Obj.txProcessedSize = 0;
    uart1Obj.txBusyStatus = false;
    uart1Obj.txCallback = NULL;

    /* Turn ON UART1*/
    U1MODESET = _U1MODE_ON_MASK;
}

bool UART1_SerialSetup( UART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    bool status = false;
    uint32_t baud = setup->baudRate;
    uint32_t brgValHigh = 0;
    uint32_t brgValLow = 0;
    uint32_t brgVal = 0;
    uint32_t uartMode;

    if((uart1Obj.rxBusyStatus == true) || (uart1Obj.txBusyStatus == true))
    {
        /* Transaction is in progress, so return without updating settings */
        return status;
    }

    if (setup != NULL)
    {
        if(srcClkFreq == 0)
        {
            srcClkFreq = UART1_FrequencyGet();
        }

        /* Calculate BRG value */
        brgValLow = ((srcClkFreq / baud) >> 4) - 1;
        brgValHigh = ((srcClkFreq / baud) >> 2) - 1;

        /* Check if the baud value can be set with low baud settings */
        if((brgValHigh >= 0) && (brgValHigh <= UINT16_MAX))
        {
            brgVal =  (((srcClkFreq >> 2) + (baud >> 1)) / baud ) - 1;
            U1MODESET = _U1MODE_BRGH_MASK;
        }
        else if ((brgValLow >= 0) && (brgValLow <= UINT16_MAX))
        {
            brgVal = ( ((srcClkFreq >> 4) + (baud >> 1)) / baud ) - 1;
            U1MODECLR = _U1MODE_BRGH_MASK;
        }
        else
        {
            return status;
        }

        if(setup->dataWidth == UART_DATA_9_BIT)
        {
            if(setup->parity != UART_PARITY_NONE)
            {
               return status;

            }
            else
            {
               /* Configure UART1 mode */
               uartMode = U1MODE;
               uartMode &= ~_U1MODE_PDSEL_MASK;
               U1MODE = uartMode | setup->dataWidth;
            }

        }
        else
        {
            /* Configure UART1 mode */
            uartMode = U1MODE;
            uartMode &= ~_U1MODE_PDSEL_MASK;
            U1MODE = uartMode | setup->parity ;
        }

        /* Configure UART1 mode */
        uartMode = U1MODE;
        uartMode &= ~_U1MODE_STSEL_MASK;
        U1MODE = uartMode | setup->stopBits ;

        /* Configure UART1 Baud Rate */
        U1BRG = brgVal;

        status = true;
    }

    return status;
}

bool UART1_Read( void *buffer, const size_t size )
{
    bool status = false;
    uint8_t * lBuffer = (uint8_t *)buffer;

    if(NULL != lBuffer)
    {
        /* Clear errors before submitting the request.
         * ErrorGet clears errors internally. */
        UART1_ErrorGet();

        /* Check if receive request is in progress */
        if(uart1Obj.rxBusyStatus == false)
        {
            uart1Obj.rxBuffer = lBuffer;
            uart1Obj.rxSize = size;
            uart1Obj.rxProcessedSize = 0;
            uart1Obj.rxBusyStatus = true;
            status = true;
        }

        /*Enable UART1_FAULT Interrupt*/
        IEC3SET = _IEC3_U1EIE_MASK;

        /*Enable UART1_RX Interrupt*/
        IEC3SET = _IEC3_U1RXIE_MASK;
    }

    return status;
}

bool UART1_Write( void *buffer, const size_t size )
{
    bool status = false;
    uint8_t * lBuffer = (uint8_t *)buffer;

    if(NULL != lBuffer)
    {
        /* Check if transmit request is in progress */
        if(uart1Obj.txBusyStatus == false)
        {
            uart1Obj.txBuffer = lBuffer;
            uart1Obj.txSize = size;
            uart1Obj.txProcessedSize = 0;
            uart1Obj.txBusyStatus = true;
            status = true;

            /* Initiate the transfer by sending first byte */
            if(!(U1STA & _U1STA_UTXBF_MASK))
            {
                U1TXREG = *lBuffer;
                uart1Obj.txProcessedSize++;
            }
            IEC3SET = _IEC3_U1TXIE_MASK;
        }
    }

    return status;
}

UART_ERROR UART1_ErrorGet( void )
{
    UART_ERROR errors = UART_ERROR_NONE;
    uint32_t status = U1STA;

    errors = (UART_ERROR)(status & (_U1STA_OERR_MASK | _U1STA_FERR_MASK | _U1STA_PERR_MASK));

    if(errors != UART_ERROR_NONE)
    {
        UART1_ErrorClear();
    }

    /* All errors are cleared, but send the previous error state */
    return errors;
}

void UART1_ReadCallbackRegister( UART_CALLBACK callback, uintptr_t context )
{
    uart1Obj.rxCallback = callback;

    uart1Obj.rxContext = context;
}

bool UART1_ReadIsBusy( void )
{
    return uart1Obj.rxBusyStatus;
}

size_t UART1_ReadCountGet( void )
{
    return uart1Obj.rxProcessedSize;
}

void UART1_WriteCallbackRegister( UART_CALLBACK callback, uintptr_t context )
{
    uart1Obj.txCallback = callback;

    uart1Obj.txContext = context;
}

bool UART1_WriteIsBusy( void )
{
    return uart1Obj.txBusyStatus;
}

size_t UART1_WriteCountGet( void )
{
    return uart1Obj.txProcessedSize;
}

void UART1_FAULT_InterruptHandler (void)
{
    /* Client must call UARTx_ErrorGet() function to clear the errors */
    if( uart1Obj.rxCallback != NULL )
    {
        uart1Obj.rxCallback(uart1Obj.rxContext);
    }

    /* Clear size and rx status */
    uart1Obj.rxBusyStatus = false;
    uart1Obj.rxSize = 0;
    uart1Obj.rxProcessedSize = 0;

    UART1_ErrorClear();

    /* Disable the interrupt*/
    IEC3CLR = _IEC3_U1EIE_MASK;
}

void UART1_RX_InterruptHandler (void)
{
    if(uart1Obj.rxBusyStatus == true)
    {
        while((_U1STA_URXDA_MASK == (U1STA & _U1STA_URXDA_MASK)) && (uart1Obj.rxSize > uart1Obj.rxProcessedSize) )
        {
            uart1Obj.rxBuffer[uart1Obj.rxProcessedSize++] = (uint8_t )(U1RXREG);
        }

        /* Clear UART1 RX Interrupt flag after reading data buffer */
        IFS3CLR = _IFS3_U1RXIF_MASK;

        /* Check if the buffer is done */
        if(uart1Obj.rxProcessedSize >= uart1Obj.rxSize)
        {
            uart1Obj.rxBusyStatus = false;
            uart1Obj.rxSize = 0;
            uart1Obj.rxProcessedSize = 0;

            /* Disable the interrupt*/
            IEC3CLR = _IEC3_U1RXIE_MASK;

            if(uart1Obj.rxCallback != NULL)
            {
                uart1Obj.rxCallback(uart1Obj.rxContext);
            }
        }
    }
    else
    {
        // Nothing to process
        ;
    }
}

void UART1_TX_InterruptHandler (void)
{
    if(uart1Obj.txBusyStatus == true)
    {
        while((!(U1STA & _U1STA_UTXBF_MASK)) && (uart1Obj.txSize > uart1Obj.txProcessedSize) )
        {
            U1TXREG = uart1Obj.txBuffer[uart1Obj.txProcessedSize++];
        }

        /* Clear UART1TX Interrupt flag after writing to buffer */
        IFS3CLR = _IFS3_U1TXIF_MASK;

        /* Check if the buffer is done */
        if(uart1Obj.txProcessedSize >= uart1Obj.txSize)
        {
            uart1Obj.txBusyStatus = false;
            uart1Obj.txSize = 0;
            uart1Obj.txProcessedSize = 0;

            /* Disable the interrupt, to avoid calling ISR continuously*/
            IEC3CLR = _IEC3_U1TXIE_MASK;

            if(uart1Obj.txCallback != NULL)
            {
                uart1Obj.txCallback(uart1Obj.txContext);
            }
        }
    }
    else
    {
        // Nothing to process
        ;
    }
}
