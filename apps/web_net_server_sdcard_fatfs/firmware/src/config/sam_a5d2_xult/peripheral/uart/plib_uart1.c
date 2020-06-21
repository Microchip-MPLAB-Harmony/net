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

#include "device.h"
#include "plib_uart1.h"

// *****************************************************************************
// *****************************************************************************
// Section: UART1 Implementation
// *****************************************************************************
// *****************************************************************************

UART_RING_BUFFER_OBJECT uart1Obj;

#define UART1_READ_BUFFER_SIZE      512
/* Disable Read, Overrun, Parity and Framing error interrupts */
#define UART1_RX_INT_DISABLE()      UART1_REGS->UART_IDR = (UART_IDR_RXRDY_Msk | UART_IDR_FRAME_Msk | UART_IDR_PARE_Msk | UART_IDR_OVRE_Msk);
/* Enable Read, Overrun, Parity and Framing error interrupts */
#define UART1_RX_INT_ENABLE()       UART1_REGS->UART_IER = (UART_IER_RXRDY_Msk | UART_IER_FRAME_Msk | UART_IER_PARE_Msk | UART_IER_OVRE_Msk);

static uint8_t UART1_ReadBuffer[UART1_READ_BUFFER_SIZE];

#define UART1_WRITE_BUFFER_SIZE     2560
#define UART1_TX_INT_DISABLE()      UART1_REGS->UART_IDR = UART_IDR_TXEMPTY_Msk;
#define UART1_TX_INT_ENABLE()       UART1_REGS->UART_IER = UART_IER_TXEMPTY_Msk;

static uint8_t UART1_WriteBuffer[UART1_WRITE_BUFFER_SIZE];

void UART1_Initialize( void )
{
    /* Reset UART1 */
    UART1_REGS->UART_CR = (UART_CR_RSTRX_Msk | UART_CR_RSTTX_Msk | UART_CR_RSTSTA_Msk);

    /* Enable UART1 */
    UART1_REGS->UART_CR = (UART_CR_TXEN_Msk | UART_CR_RXEN_Msk);

    /* Configure UART1 mode */
    UART1_REGS->UART_MR = ((UART_MR_BRSRCCK_PERIPH_CLK) | (UART_MR_PAR_NO) | (0 << UART_MR_FILTER_Pos));

    /* Configure UART1 Baud Rate */
    UART1_REGS->UART_BRGR = UART_BRGR_CD(45);

    /* Initialize instance object */
    uart1Obj.rdCallback = NULL;
    uart1Obj.rdInIndex = 0;
	uart1Obj.rdOutIndex = 0;
    uart1Obj.isRdNotificationEnabled = false;
    uart1Obj.isRdNotifyPersistently = false;
    uart1Obj.rdThreshold = 0;
    uart1Obj.wrCallback = NULL;
    uart1Obj.wrInIndex = 0;
	uart1Obj.wrOutIndex = 0;
    uart1Obj.isWrNotificationEnabled = false;
    uart1Obj.isWrNotifyPersistently = false;
    uart1Obj.wrThreshold = 0;

    /* Enable receive interrupt */
    UART1_RX_INT_ENABLE()
}

bool UART1_SerialSetup( UART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    bool status = false;
    uint32_t baud = setup->baudRate;
    uint32_t brgVal = 0;
    uint32_t uartMode;

    if (setup != NULL)
    {
        if(srcClkFreq == 0)
        {
            srcClkFreq = UART1_FrequencyGet();
        }

        /* Calculate BRG value */
        brgVal = srcClkFreq / (16 * baud);

        /* If the target baud rate is acheivable using this clock */
        if (brgVal <= 65535)
        {
            /* Configure UART1 mode */
            uartMode = UART1_REGS->UART_MR;
            uartMode &= ~UART_MR_PAR_Msk;
            UART1_REGS->UART_MR = uartMode | setup->parity ;

            /* Configure UART1 Baud Rate */
            UART1_REGS->UART_BRGR = UART_BRGR_CD(brgVal);

            status = true;
        }
    }

    return status;
}

static void UART1_ErrorClear( void )
{
    uint8_t dummyData = 0u;

    UART1_REGS->UART_CR = UART_CR_RSTSTA_Msk;

    /* Flush existing error bytes from the RX FIFO */
    while( UART_SR_RXRDY_Msk == (UART1_REGS->UART_SR & UART_SR_RXRDY_Msk) )
    {
        dummyData = (UART1_REGS->UART_RHR & UART_RHR_RXCHR_Msk);
    }

    /* Ignore the warning */
    (void)dummyData;
}

UART_ERROR UART1_ErrorGet( void )
{
    UART_ERROR errors = UART_ERROR_NONE;
    uint32_t status = UART1_REGS->UART_SR;

    errors = (UART_ERROR)(status & (UART_SR_OVRE_Msk | UART_SR_PARE_Msk | UART_SR_FRAME_Msk));

    if(errors != UART_ERROR_NONE)
    {
        UART1_ErrorClear();
    }

    /* All errors are cleared, but send the previous error state */
    return errors;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static inline bool UART1_RxPushByte(uint8_t rdByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;

    tempInIndex = uart1Obj.rdInIndex + 1;

    if (tempInIndex >= UART1_READ_BUFFER_SIZE)
    {
        tempInIndex = 0;
    }

    if (tempInIndex == uart1Obj.rdOutIndex)
    {
        /* Queue is full - Report it to the application. Application gets a chance to free up space by reading data out from the RX ring buffer */
        if(uart1Obj.rdCallback != NULL)
        {
            uart1Obj.rdCallback(UART_EVENT_READ_BUFFER_FULL, uart1Obj.rdContext);
        }

        /* Read the indices again in case application has freed up space in RX ring buffer */
        tempInIndex = uart1Obj.rdInIndex + 1;

        if (tempInIndex >= UART1_READ_BUFFER_SIZE)
        {
            tempInIndex = 0;
        }

    }

    if (tempInIndex != uart1Obj.rdOutIndex)
    {
        UART1_ReadBuffer[uart1Obj.rdInIndex] = rdByte;
        uart1Obj.rdInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Data will be lost. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void UART1_ReadNotificationSend(void)
{
    uint32_t nUnreadBytesAvailable;

    if (uart1Obj.isRdNotificationEnabled == true)
    {
        nUnreadBytesAvailable = UART1_ReadCountGet();

        if(uart1Obj.rdCallback != NULL)
        {
            if (uart1Obj.isRdNotifyPersistently == true)
            {
                if (nUnreadBytesAvailable >= uart1Obj.rdThreshold)
                {
                    uart1Obj.rdCallback(UART_EVENT_READ_THRESHOLD_REACHED, uart1Obj.rdContext);
                }
            }
            else
            {
                if (nUnreadBytesAvailable == uart1Obj.rdThreshold)
                {
                    uart1Obj.rdCallback(UART_EVENT_READ_THRESHOLD_REACHED, uart1Obj.rdContext);
                }
            }
        }
    }
}

size_t UART1_Read(uint8_t* pRdBuffer, const size_t size)
{
    size_t nBytesRead = 0;
	uint32_t rdOutIndex;
	uint32_t rdInIndex;

    while (nBytesRead < size)
    {
        UART1_RX_INT_DISABLE();
		
		rdOutIndex = uart1Obj.rdOutIndex;
		rdInIndex = uart1Obj.rdInIndex;

        if (rdOutIndex != rdInIndex)
        {
            pRdBuffer[nBytesRead++] = UART1_ReadBuffer[uart1Obj.rdOutIndex++];

            if (uart1Obj.rdOutIndex >= UART1_READ_BUFFER_SIZE)
            {
                uart1Obj.rdOutIndex = 0;
            }
            UART1_RX_INT_ENABLE();
        }
        else
        {
            UART1_RX_INT_ENABLE();
            break;
        }
    }

    return nBytesRead;
}

size_t UART1_ReadCountGet(void)
{
    size_t nUnreadBytesAvailable;
	uint32_t rdInIndex;
	uint32_t rdOutIndex;
    
	/* Take  snapshot of indices to avoid creation of critical section */
	rdInIndex = uart1Obj.rdInIndex;
	rdOutIndex = uart1Obj.rdOutIndex;

    if ( rdInIndex >=  rdOutIndex)
    {
        nUnreadBytesAvailable =  rdInIndex - rdOutIndex;
    }
    else
    {
        nUnreadBytesAvailable =  (UART1_READ_BUFFER_SIZE -  rdOutIndex) + rdInIndex;
    }
    
    return nUnreadBytesAvailable;
}

size_t UART1_ReadFreeBufferCountGet(void)
{
    return (UART1_READ_BUFFER_SIZE - 1) - UART1_ReadCountGet();
}

size_t UART1_ReadBufferSizeGet(void)
{
    return (UART1_READ_BUFFER_SIZE - 1);
}

bool UART1_ReadNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = uart1Obj.isRdNotificationEnabled;

    uart1Obj.isRdNotificationEnabled = isEnabled;

    uart1Obj.isRdNotifyPersistently = isPersistent;

    return previousStatus;
}

void UART1_ReadThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        uart1Obj.rdThreshold = nBytesThreshold;
    }
}

void UART1_ReadCallbackRegister( UART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    uart1Obj.rdCallback = callback;

    uart1Obj.rdContext = context;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static bool UART1_TxPullByte(uint8_t* pWrByte)
{
    bool isSuccess = false;
	uint32_t wrOutIndex = uart1Obj.wrOutIndex;
	uint32_t wrInIndex = uart1Obj.wrInIndex;

    if (wrOutIndex != wrInIndex)
    {
        *pWrByte = UART1_WriteBuffer[uart1Obj.wrOutIndex++];

        if (uart1Obj.wrOutIndex >= UART1_WRITE_BUFFER_SIZE)
        {
            uart1Obj.wrOutIndex = 0;
        }
        isSuccess = true;
    }

    return isSuccess;
}

static inline bool UART1_TxPushByte(uint8_t wrByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;

    tempInIndex = uart1Obj.wrInIndex + 1;

    if (tempInIndex >= UART1_WRITE_BUFFER_SIZE)
    {
        tempInIndex = 0;
    }
    if (tempInIndex != uart1Obj.wrOutIndex)
    {
        UART1_WriteBuffer[uart1Obj.wrInIndex] = wrByte;
        uart1Obj.wrInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Report Error. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void UART1_WriteNotificationSend(void)
{
    uint32_t nFreeWrBufferCount;

    if (uart1Obj.isWrNotificationEnabled == true)
    {
        nFreeWrBufferCount = UART1_WriteFreeBufferCountGet();

        if(uart1Obj.wrCallback != NULL)
        {
            if (uart1Obj.isWrNotifyPersistently == true)
            {
                if (nFreeWrBufferCount >= uart1Obj.wrThreshold)
                {
                    uart1Obj.wrCallback(UART_EVENT_WRITE_THRESHOLD_REACHED, uart1Obj.wrContext);
                }
            }
            else
            {
                if (nFreeWrBufferCount == uart1Obj.wrThreshold)
                {
                    uart1Obj.wrCallback(UART_EVENT_WRITE_THRESHOLD_REACHED, uart1Obj.wrContext);
                }
            }
        }
    }
}

static size_t UART1_WritePendingBytesGet(void)
{
    size_t nPendingTxBytes;
	
	/* Take a snapshot of indices to avoid creation of critical section */
	uint32_t wrOutIndex = uart1Obj.wrOutIndex;
	uint32_t wrInIndex = uart1Obj.wrInIndex;

    if ( wrInIndex >=  wrOutIndex)
    {
        nPendingTxBytes =  wrInIndex -  wrOutIndex;
    }
    else
    {
        nPendingTxBytes =  (UART1_WRITE_BUFFER_SIZE -  wrOutIndex) + wrInIndex;
    }

    return nPendingTxBytes;
}

size_t UART1_WriteCountGet(void)
{
    size_t nPendingTxBytes;    

    nPendingTxBytes = UART1_WritePendingBytesGet();

    return nPendingTxBytes;
}

size_t UART1_Write(uint8_t* pWrBuffer, const size_t size )
{
    size_t nBytesWritten  = 0;

    UART1_TX_INT_DISABLE();

    while (nBytesWritten < size)
    {
        if (UART1_TxPushByte(pWrBuffer[nBytesWritten]) == true)
        {
            nBytesWritten++;
        }
        else
        {
            /* Queue is full, exit the loop */
            break;
        }
    }

    /* Check if any data is pending for transmission */
    if (UART1_WritePendingBytesGet() > 0)
    {
        /* Enable TX interrupt as data is pending for transmission */
        UART1_TX_INT_ENABLE();
    }

    return nBytesWritten;
}

size_t UART1_WriteFreeBufferCountGet(void)
{
    return (UART1_WRITE_BUFFER_SIZE - 1) - UART1_WriteCountGet();
}

size_t UART1_WriteBufferSizeGet(void)
{
    return (UART1_WRITE_BUFFER_SIZE - 1);
}

bool UART1_WriteNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = uart1Obj.isWrNotificationEnabled;

    uart1Obj.isWrNotificationEnabled = isEnabled;

    uart1Obj.isWrNotifyPersistently = isPersistent;

    return previousStatus;
}

void UART1_WriteThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        uart1Obj.wrThreshold = nBytesThreshold;
    }
}

void UART1_WriteCallbackRegister( UART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    uart1Obj.wrCallback = callback;

    uart1Obj.wrContext = context;
}

static void UART1_ISR_RX_Handler( void )
{
    /* Keep reading until there is a character availabe in the RX FIFO */
    while(UART_SR_RXRDY_Msk == (UART1_REGS->UART_SR& UART_SR_RXRDY_Msk))
    {
        if (UART1_RxPushByte( (uint8_t )(UART1_REGS->UART_RHR& UART_RHR_RXCHR_Msk) ) == true)
        {
            UART1_ReadNotificationSend();
        }
        else
        {
            /* UART RX buffer is full */
        }
    }
}

static void UART1_ISR_TX_Handler( void )
{
    uint8_t wrByte;

    /* Keep writing to the TX FIFO as long as there is space */
    while(UART_SR_TXRDY_Msk == (UART1_REGS->UART_SR & UART_SR_TXRDY_Msk))
    {
        if (UART1_TxPullByte(&wrByte) == true)
        {
            UART1_REGS->UART_THR |= wrByte;

            /* Send notification */
            UART1_WriteNotificationSend();
        }
        else
        {
            /* Nothing to transmit. Disable the data register empty interrupt. */
            UART1_TX_INT_DISABLE();
            break;
        }
    }
}

void UART1_InterruptHandler( void )
{
    /* Error status */
    uint32_t errorStatus = (UART1_REGS->UART_SR & (UART_SR_OVRE_Msk | UART_SR_FRAME_Msk | UART_SR_PARE_Msk));

    if(errorStatus != 0)
    {
        /* Client must call UARTx_ErrorGet() function to clear the errors */

        /* Disable Read, Overrun, Parity and Framing error interrupts */

        UART1_REGS->UART_IDR = (UART_IDR_RXRDY_Msk | UART_IDR_FRAME_Msk | UART_IDR_PARE_Msk | UART_IDR_OVRE_Msk);

        /* UART errors are normally associated with the receiver, hence calling
         * receiver callback */
        if( uart1Obj.rdCallback != NULL )
        {
            uart1Obj.rdCallback(UART_EVENT_READ_ERROR, uart1Obj.rdContext);
        }
    }

    /* Receiver status */
    if(UART_SR_RXRDY_Msk == (UART1_REGS->UART_SR & UART_SR_RXRDY_Msk))
    {
        UART1_ISR_RX_Handler();
    }

    /* Transmitter status */
    if(UART_SR_TXRDY_Msk == (UART1_REGS->UART_SR & UART_SR_TXRDY_Msk))
    {
        UART1_ISR_TX_Handler();
    }

}