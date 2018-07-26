/*******************************************************************************
  USART1 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_usart1.c

  Summary:
    USART1 PLIB Implementation File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/

#include "device.h"
#include "plib_usart1.h"

// *****************************************************************************
// *****************************************************************************
// Section: USART1 Implementation
// *****************************************************************************
// *****************************************************************************

USART_OBJECT usart1Obj;

void static USART1_ISR_RX_Handler( void )
{
    if(usart1Obj.rxBusyStatus == true)
    {
        while((US_CSR_RXRDY_Msk == (USART1_REGS->US_CSR& US_CSR_RXRDY_Msk)) && (usart1Obj.rxSize > usart1Obj.rxProcessedSize) )
        {
            usart1Obj.rxBuffer[usart1Obj.rxProcessedSize++] = (USART1_REGS->US_RHR& US_RHR_RXCHR_Msk);
        }

        /* Check if the buffer is done */
        if(usart1Obj.rxProcessedSize >= usart1Obj.rxSize)
        {
            if(usart1Obj.rxCallback != NULL)
            {
                usart1Obj.rxCallback(usart1Obj.rxContext);
            }

            usart1Obj.rxBusyStatus = false;
            usart1Obj.rxSize = 0;
            usart1Obj.rxProcessedSize = 0;

            /* Disable Read, Overrun, Parity and Framing error interrupts */
            USART1_REGS->US_IDR = (US_IDR_RXRDY_Msk | US_IDR_FRAME_Msk | US_IDR_PARE_Msk | US_IDR_OVRE_Msk);
        }
    }
    else
    {
        /* Nothing to process */
        ;
    }

    return;
}

void static USART1_ISR_TX_Handler( void )
{
    if(usart1Obj.txBusyStatus == true)
    {
        while((US_CSR_TXEMPTY_Msk == (USART1_REGS->US_CSR& US_CSR_TXEMPTY_Msk)) && (usart1Obj.txSize > usart1Obj.txProcessedSize) )
        {
            USART1_REGS->US_THR|= usart1Obj.txBuffer[usart1Obj.txProcessedSize++];
        }

        /* Check if the buffer is done */
        if(usart1Obj.txProcessedSize >= usart1Obj.txSize)
        {
            usart1Obj.txBusyStatus = false;
            usart1Obj.txSize = 0;
            usart1Obj.txProcessedSize = 0;
            USART1_REGS->US_IDR = US_IDR_TXEMPTY_Msk;

            if(usart1Obj.txCallback != NULL)
            {
                usart1Obj.txCallback(usart1Obj.txContext);
            }
        }
    }
    else
    {
        /* Nothing to process */
        ;
    }

    return;
}

void USART1_InterruptHandler( void )
{
    /* Error status */
    uint32_t errorStatus = (USART1_REGS->US_CSR & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));

    if(errorStatus != 0)
    {
        /* Client must call USARTx_ErrorGet() function to clear the errors */

        /* USART errors are normally associated with the receiver, hence calling
         * receiver context */
        if( usart1Obj.rxCallback != NULL )
        {
            usart1Obj.rxCallback(usart1Obj.rxContext);
        }

        usart1Obj.rxBusyStatus = false;
        usart1Obj.rxSize = 0;
        usart1Obj.rxProcessedSize = 0;

        /* Disable Read, Overrun, Parity and Framing error interrupts */
        USART1_REGS->US_IDR = (US_IDR_RXRDY_Msk | US_IDR_FRAME_Msk | US_IDR_PARE_Msk | US_IDR_OVRE_Msk);
    }

    /* Receiver status */
    if(US_CSR_RXRDY_Msk == (USART1_REGS->US_CSR& US_CSR_RXRDY_Msk))
    {
        USART1_ISR_RX_Handler();
    }

    /* Transmitter status */
    if(US_CSR_TXEMPTY_Msk == (USART1_REGS->US_CSR& US_CSR_TXEMPTY_Msk))
    {
        USART1_ISR_TX_Handler();
    }

    return;
}


void static USART1_ErrorClear( void )
{
    uint8_t dummyData = 0u;

    USART1_REGS->US_CR|= US_CR_RSTSTA_Msk;

    /* Flush existing error bytes from the RX FIFO */
    while( US_CSR_RXRDY_Msk == (USART1_REGS->US_CSR& US_CSR_RXRDY_Msk) )
    {
        dummyData = (USART1_REGS->US_RHR& US_RHR_RXCHR_Msk);
    }

    /* Ignore the warning */
    (void)dummyData;

    return;
}

void USART1_Initialize( void )
{
    /* Reset USART1 */
    USART1_REGS->US_CR = (US_CR_RSTRX_Msk | US_CR_RSTTX_Msk | US_CR_RSTSTA_Msk);

    /* Enable USART1 */
    USART1_REGS->US_CR = (US_CR_TXEN_Msk | US_CR_RXEN_Msk);

    /* Configure USART1 mode */
    USART1_REGS->US_MR = ((US_MR_USCLKS_MCK) | (0 << US_MR_MODE9_Pos) | US_MR_CHRL_8_BIT | US_MR_PAR_NO | US_MR_NBSTOP_1_BIT | (0 << US_MR_SYNC_Pos) | (0 << US_MR_OVER_Pos));

    /* Configure USART1 Baud Rate */
    USART1_REGS->US_BRGR = US_BRGR_CD(81);

    /* Initialize instance object */
    usart1Obj.rxBuffer = NULL;
    usart1Obj.rxSize = 0;
    usart1Obj.rxProcessedSize = 0;
    usart1Obj.rxBusyStatus = false;
    usart1Obj.rxCallback = NULL;
    usart1Obj.txBuffer = NULL;
    usart1Obj.txSize = 0;
    usart1Obj.txProcessedSize = 0;
    usart1Obj.txBusyStatus = false;
    usart1Obj.txCallback = NULL;

    return;
}

USART_ERROR USART1_ErrorGet( void )
{
    USART_ERROR errors = USART_ERROR_NONE;
    uint32_t status = USART1_REGS->US_CSR;

    /* Collect all errors */
    if(status & US_CSR_OVRE_Msk)
    {
        errors = USART_ERROR_OVERRUN;
    }
    if(status & US_CSR_PARE_Msk)
    {
        errors |= USART_ERROR_PARITY;
    }
    if(status & US_CSR_FRAME_Msk)
    {
        errors |= USART_ERROR_FRAMING;
    }

    if(errors != USART_ERROR_NONE)
    {
        USART1_ErrorClear();
    }

    /* All errors are cleared, but send the previous error state */
    return errors;
}

bool USART1_SerialSetup( USART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    bool status = true;
    uint32_t clk = srcClkFreq;
    uint32_t baud = setup->baudRate;
    uint32_t brgVal = 0;
    uint32_t overSampVal = 0;
    uint32_t mode9Val = 0;
    uint32_t charLengthVal = 0;
    uint32_t parityVal = 0;
    uint32_t stopBitsVal = 0;

    if((usart1Obj.rxBusyStatus == true) || (usart1Obj.txBusyStatus == true))
    {
        /* Transaction is in progress, so return without updating settings */
        return false;
    }

    if(clk == 0)
    {
        clk = USART1_FrequencyGet();
    }

    /* Calculate BRG value */
    if (clk >= (16 * baud))
    {
        brgVal = (clk / (16 * baud));
    }
    else
    {
        brgVal = (clk / (8 * baud));
        overSampVal = (1 << US_MR_OVER_Pos) & US_MR_OVER_Msk;
    }

    /* Get Data width values */
    switch(setup->dataWidth)
    {
        case USART_DATA_5_BIT:
        case USART_DATA_6_BIT:
        case USART_DATA_7_BIT:
        case USART_DATA_8_BIT:
        {
            charLengthVal = US_MR_CHRL(setup->dataWidth);
            break;
        }
        case USART_DATA_9_BIT:
        {
            mode9Val = (1 << US_MR_MODE9_Pos) & US_MR_MODE9_Msk;
            break;
        }
        default:
        {
            status = false;
            break;
        }
    }

    /* Get Parity values */
    switch(setup->parity)
    {
        case USART_PARITY_ODD:
        case USART_PARITY_MARK:
        {
            parityVal = US_MR_PAR(setup->parity);
            break;
        }
        case USART_PARITY_NONE:
        {
            parityVal = US_MR_PAR_NO;
            break;
        }
        case USART_PARITY_EVEN:
        {
            parityVal = US_MR_PAR_EVEN;
            break;
        }
        case USART_PARITY_SPACE:
        {
            parityVal = US_MR_PAR_SPACE;
            break;
        }
        case USART_PARITY_MULTIDROP:
        {
            parityVal = US_MR_PAR_MULTIDROP;
            break;
        }
        default:
        {
            status = false;
            break;
        }
    }

    /* Get Stop bit values */
    switch(setup->stopBits)
    {
        case USART_STOP_1_BIT:
        case USART_STOP_1_5_BIT:
        case USART_STOP_2_BIT:
        {
            stopBitsVal = US_MR_NBSTOP(setup->stopBits);
            break;
        }
        default:
        {
            status = false;
            break;
        }
    }

    if(status != false)
    {
        /* Configure USART1 mode */
        USART1_REGS->US_MR = (mode9Val | charLengthVal | parityVal | stopBitsVal | (0 << US_MR_SYNC_Pos) | overSampVal);

        /* Configure USART1 Baud Rate */
        USART1_REGS->US_BRGR = US_BRGR_CD(brgVal);
    }

    return status;
}

bool USART1_Read( void *buffer, const size_t size )
{
    bool status = false;
    uint8_t * lBuffer = (uint8_t *)buffer;

    if(NULL != lBuffer)
    {
        /* Clear errors before submitting the request.
         * ErrorGet clears errors internally. */
        USART1_ErrorGet();

        /* Check if receive request is in progress */
        if(usart1Obj.rxBusyStatus == false)
        {
            usart1Obj.rxBuffer = lBuffer;
            usart1Obj.rxSize = size;
            usart1Obj.rxProcessedSize = 0;
            usart1Obj.rxBusyStatus = true;
            status = true;

            /* Enable Read, Overrun, Parity and Framing error interrupts */
            USART1_REGS->US_IER = (US_IER_RXRDY_Msk | US_IER_FRAME_Msk | US_IER_PARE_Msk | US_IER_OVRE_Msk);
        }
    }

    return status;
}

bool USART1_Write( void *buffer, const size_t size )
{
    bool status = false;
    uint8_t * lBuffer = (uint8_t *)buffer;

    if(NULL != lBuffer)
    {
        /* Check if transmit request is in progress */
        if(usart1Obj.txBusyStatus == false)
        {
            usart1Obj.txBuffer = lBuffer;
            usart1Obj.txSize = size;
            usart1Obj.txProcessedSize = 0;
            usart1Obj.txBusyStatus = true;
            status = true;

            /* Initiate the transfer by sending first byte */
            if(US_CSR_TXEMPTY_Msk == (USART1_REGS->US_CSR& US_CSR_TXEMPTY_Msk))
            {
                USART1_REGS->US_THR = (US_THR_TXCHR(*lBuffer) & US_THR_TXCHR_Msk);
                usart1Obj.txProcessedSize++;
            }

            USART1_REGS->US_IER = US_IER_TXEMPTY_Msk;
        }
    }

    return status;
}

bool USART1_WriteCallbackRegister( USART_CALLBACK callback, uintptr_t context )
{
    usart1Obj.txCallback = callback;
    usart1Obj.txContext = context;

    return true;
}

bool USART1_ReadCallbackRegister( USART_CALLBACK callback, uintptr_t context )
{
    usart1Obj.rxCallback = callback;
    usart1Obj.rxContext = context;

    return true;
}

bool USART1_WriteIsBusy( void )
{
    return usart1Obj.txBusyStatus;
}

bool USART1_ReadIsBusy( void )
{
    return usart1Obj.rxBusyStatus;
}

size_t USART1_WriteCountGet( void )
{
    return usart1Obj.txProcessedSize;
}

size_t USART1_ReadCountGet( void )
{
    return usart1Obj.rxProcessedSize;
}

