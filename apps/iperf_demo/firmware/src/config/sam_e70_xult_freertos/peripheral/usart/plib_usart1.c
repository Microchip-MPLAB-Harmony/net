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
#include "plib_usart1.h"

// *****************************************************************************
// *****************************************************************************
// Section: USART1 Implementation
// *****************************************************************************
// *****************************************************************************

USART_RING_BUFFER_OBJECT usart1Obj;

#define USART1_READ_BUFFER_SIZE      128
/* Disable Read, Overrun, Parity and Framing error interrupts */
#define USART1_RX_INT_DISABLE()      USART1_REGS->US_IDR = (US_IDR_USART_RXRDY_Msk | US_IDR_USART_FRAME_Msk | US_IDR_USART_PARE_Msk | US_IDR_USART_OVRE_Msk);
/* Enable Read, Overrun, Parity and Framing error interrupts */
#define USART1_RX_INT_ENABLE()       USART1_REGS->US_IER = (US_IER_USART_RXRDY_Msk | US_IER_USART_FRAME_Msk | US_IER_USART_PARE_Msk | US_IER_USART_OVRE_Msk);

static uint8_t USART1_ReadBuffer[USART1_READ_BUFFER_SIZE];

#define USART1_WRITE_BUFFER_SIZE     4096
#define USART1_TX_INT_DISABLE()      USART1_REGS->US_IDR = US_IDR_USART_TXEMPTY_Msk;
#define USART1_TX_INT_ENABLE()       USART1_REGS->US_IER = US_IER_USART_TXEMPTY_Msk;

static uint8_t USART1_WriteBuffer[USART1_WRITE_BUFFER_SIZE];

void USART1_Initialize( void )
{
    /* Reset USART1 */
    USART1_REGS->US_CR = (US_CR_USART_RSTRX_Msk | US_CR_USART_RSTTX_Msk | US_CR_USART_RSTSTA_Msk);

    /* Enable USART1 */
    USART1_REGS->US_CR = (US_CR_USART_TXEN_Msk | US_CR_USART_RXEN_Msk);

    /* Configure USART1 mode */
    USART1_REGS->US_MR = (US_MR_USART_USCLKS_MCK | US_MR_USART_CHRL_8_BIT | US_MR_USART_PAR_NO | US_MR_USART_NBSTOP_1_BIT | (0 << US_MR_USART_OVER_Pos));

    /* Configure USART1 Baud Rate */
    USART1_REGS->US_BRGR = US_BRGR_CD(81);

    /* Initialize instance object */
    usart1Obj.rdCallback = NULL;
    usart1Obj.rdInIndex = 0;
	usart1Obj.rdOutIndex = 0;
    usart1Obj.isRdNotificationEnabled = false;
    usart1Obj.isRdNotifyPersistently = false;
    usart1Obj.rdThreshold = 0;
    usart1Obj.wrCallback = NULL;
    usart1Obj.wrInIndex = 0;
	usart1Obj.wrOutIndex = 0;
    usart1Obj.isWrNotificationEnabled = false;
    usart1Obj.isWrNotifyPersistently = false;
    usart1Obj.wrThreshold = 0;

    /* Enable Read, Overrun, Parity and Framing error interrupts */
    USART1_RX_INT_ENABLE();
}

bool USART1_SerialSetup( USART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    uint32_t baud = setup->baudRate;
    uint32_t brgVal = 0;
    uint32_t overSampVal = 0;
    uint32_t usartMode;
    bool status = false;

    if (setup != NULL)
    {
        baud = setup->baudRate;
        if(srcClkFreq == 0)
        {
            srcClkFreq = USART1_FrequencyGet();
        }

        /* Calculate BRG value */
        if (srcClkFreq >= (16 * baud))
        {
            brgVal = (srcClkFreq / (16 * baud));
        }
        else if (srcClkFreq >= (8 * baud))
        {
            brgVal = (srcClkFreq / (8 * baud));
            overSampVal = US_MR_USART_OVER(1);
        }
        else
        {
            return false;
        }

        if (brgVal > 65535)
        {
            /* The requested baud is so low that the ratio of srcClkFreq to baud exceeds the 16-bit register value of CD register */
            return false;
        }

        /* Configure USART1 mode */
        usartMode = USART1_REGS->US_MR;
        usartMode &= ~(US_MR_USART_CHRL_Msk | US_MR_USART_MODE9_Msk | US_MR_USART_PAR_Msk | US_MR_USART_NBSTOP_Msk | US_MR_USART_OVER_Msk);
        USART1_REGS->US_MR = usartMode | ((uint32_t)setup->dataWidth | (uint32_t)setup->parity | (uint32_t)setup->stopBits | (uint32_t)overSampVal);

        /* Configure USART1 Baud Rate */
        USART1_REGS->US_BRGR = US_BRGR_CD(brgVal);
        status = true;
    }

    return status;
}

static void USART1_ErrorClear( void )
{
    uint8_t dummyData = 0u;

    USART1_REGS->US_CR = US_CR_USART_RSTSTA_Msk;

    /* Flush existing error bytes from the RX FIFO */
    while( US_CSR_USART_RXRDY_Msk == (USART1_REGS->US_CSR & US_CSR_USART_RXRDY_Msk) )
    {
        dummyData = (USART1_REGS->US_RHR & US_RHR_RXCHR_Msk);
    }

    /* Ignore the warning */
    (void)dummyData;
}

USART_ERROR USART1_ErrorGet( void )
{
    USART_ERROR errors = USART_ERROR_NONE;
    uint32_t status = USART1_REGS->US_CSR;

    errors = (USART_ERROR)(status & (US_CSR_USART_OVRE_Msk | US_CSR_USART_PARE_Msk | US_CSR_USART_FRAME_Msk));

    if(errors != USART_ERROR_NONE)
    {
        USART1_ErrorClear();
    }

    /* All errors are cleared, but send the previous error state */
    return errors;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static inline bool USART1_RxPushByte(uint8_t rdByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;

    tempInIndex = usart1Obj.rdInIndex + 1;

    if (tempInIndex >= USART1_READ_BUFFER_SIZE)
    {
        tempInIndex = 0;
    }

    if (tempInIndex == usart1Obj.rdOutIndex)
    {
        /* Queue is full - Report it to the application. Application gets a chance to free up space by reading data out from the RX ring buffer */
        if(usart1Obj.rdCallback != NULL)
        {
            usart1Obj.rdCallback(USART_EVENT_READ_BUFFER_FULL, usart1Obj.rdContext);

            /* Read the indices again in case application has freed up space in RX ring buffer */
            tempInIndex = usart1Obj.rdInIndex + 1;

            if (tempInIndex >= USART1_READ_BUFFER_SIZE)
            {
                tempInIndex = 0;
            }
        }
    }

    if (tempInIndex != usart1Obj.rdOutIndex)
    {
        USART1_ReadBuffer[usart1Obj.rdInIndex] = rdByte;
        usart1Obj.rdInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Data will be lost. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void USART1_ReadNotificationSend(void)
{
    uint32_t nUnreadBytesAvailable;

    if (usart1Obj.isRdNotificationEnabled == true)
    {
        nUnreadBytesAvailable = USART1_ReadCountGet();

        if(usart1Obj.rdCallback != NULL)
        {
            if (usart1Obj.isRdNotifyPersistently == true)
            {
                if (nUnreadBytesAvailable >= usart1Obj.rdThreshold)
                {
                    usart1Obj.rdCallback(USART_EVENT_READ_THRESHOLD_REACHED, usart1Obj.rdContext);
                }
            }
            else
            {
                if (nUnreadBytesAvailable == usart1Obj.rdThreshold)
                {
                    usart1Obj.rdCallback(USART_EVENT_READ_THRESHOLD_REACHED, usart1Obj.rdContext);
                }
            }
        }
    }
}

size_t USART1_Read(uint8_t* pRdBuffer, const size_t size)
{
    size_t nBytesRead = 0;
	uint32_t rdOutIndex;
	uint32_t rdInIndex;

    while (nBytesRead < size)
    {
        USART1_RX_INT_DISABLE();
		
		rdOutIndex = usart1Obj.rdOutIndex;
		rdInIndex = usart1Obj.rdInIndex;

        if (rdOutIndex != rdInIndex)
        {
            pRdBuffer[nBytesRead++] = USART1_ReadBuffer[usart1Obj.rdOutIndex++];

            if (usart1Obj.rdOutIndex >= USART1_READ_BUFFER_SIZE)
            {
                usart1Obj.rdOutIndex = 0;
            }
            USART1_RX_INT_ENABLE();
        }
        else
        {
            USART1_RX_INT_ENABLE();
            break;
        }
    }

    return nBytesRead;
}

size_t USART1_ReadCountGet(void)
{
    size_t nUnreadBytesAvailable;	
	uint32_t rdOutIndex;
	uint32_t rdInIndex;    
	
	/* Take a snapshot of indices to avoid creation of critical section */
	rdOutIndex = usart1Obj.rdOutIndex;
	rdInIndex = usart1Obj.rdInIndex;

    if ( rdInIndex >=  rdOutIndex)
    {
        nUnreadBytesAvailable =  rdInIndex - rdOutIndex;
    }
    else
    {
        nUnreadBytesAvailable =  (USART1_READ_BUFFER_SIZE -  rdOutIndex) + rdInIndex;
    }    

    return nUnreadBytesAvailable;
}

size_t USART1_ReadFreeBufferCountGet(void)
{
    return (USART1_READ_BUFFER_SIZE - 1) - USART1_ReadCountGet();
}

size_t USART1_ReadBufferSizeGet(void)
{
    return (USART1_READ_BUFFER_SIZE - 1);
}

bool USART1_ReadNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = usart1Obj.isRdNotificationEnabled;

    usart1Obj.isRdNotificationEnabled = isEnabled;

    usart1Obj.isRdNotifyPersistently = isPersistent;

    return previousStatus;
}

void USART1_ReadThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        usart1Obj.rdThreshold = nBytesThreshold;
    }
}

void USART1_ReadCallbackRegister( USART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    usart1Obj.rdCallback = callback;

    usart1Obj.rdContext = context;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static bool USART1_TxPullByte(uint8_t* pWrByte)
{
    bool isSuccess = false;
	uint32_t wrOutIndex = usart1Obj.wrOutIndex;
	uint32_t wrInIndex = usart1Obj.wrInIndex;

    if (wrOutIndex != wrInIndex)
    {
        *pWrByte = USART1_WriteBuffer[usart1Obj.wrOutIndex++];

        if (usart1Obj.wrOutIndex >= USART1_WRITE_BUFFER_SIZE)
        {
            usart1Obj.wrOutIndex = 0;
        }
        isSuccess = true;
    }

    return isSuccess;
}

static inline bool USART1_TxPushByte(uint8_t wrByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;

    tempInIndex = usart1Obj.wrInIndex + 1;

    if (tempInIndex >= USART1_WRITE_BUFFER_SIZE)
    {
        tempInIndex = 0;
    }
    if (tempInIndex != usart1Obj.wrOutIndex)
    {
        USART1_WriteBuffer[usart1Obj.wrInIndex] = wrByte;
        usart1Obj.wrInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Report Error. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void USART1_WriteNotificationSend(void)
{
    uint32_t nFreeWrBufferCount;

    if (usart1Obj.isWrNotificationEnabled == true)
    {
        nFreeWrBufferCount = USART1_WriteFreeBufferCountGet();

        if(usart1Obj.wrCallback != NULL)
        {
            if (usart1Obj.isWrNotifyPersistently == true)
            {
                if (nFreeWrBufferCount >= usart1Obj.wrThreshold)
                {
                    usart1Obj.wrCallback(USART_EVENT_WRITE_THRESHOLD_REACHED, usart1Obj.wrContext);
                }
            }
            else
            {
                if (nFreeWrBufferCount == usart1Obj.wrThreshold)
                {
                    usart1Obj.wrCallback(USART_EVENT_WRITE_THRESHOLD_REACHED, usart1Obj.wrContext);
                }
            }
        }
    }
}

static size_t USART1_WritePendingBytesGet(void)
{
    size_t nPendingTxBytes;
	
	/* Take a snapshot of indices to avoid creation of critical section */
	uint32_t wrOutIndex = usart1Obj.wrOutIndex;
	uint32_t wrInIndex = usart1Obj.wrInIndex;

    if ( wrInIndex >= wrOutIndex)
    {
        nPendingTxBytes =  wrInIndex -  wrOutIndex;
    }
    else
    {
        nPendingTxBytes =  (USART1_WRITE_BUFFER_SIZE -  wrOutIndex) + wrInIndex;
    }

    return nPendingTxBytes;
}

size_t USART1_WriteCountGet(void)
{
    size_t nPendingTxBytes;

    nPendingTxBytes = USART1_WritePendingBytesGet();    

    return nPendingTxBytes;
}

size_t USART1_Write(uint8_t* pWrBuffer, const size_t size )
{
    size_t nBytesWritten  = 0;

    USART1_TX_INT_DISABLE();

    while (nBytesWritten < size)
    {
        if (USART1_TxPushByte(pWrBuffer[nBytesWritten]) == true)
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
    if (USART1_WritePendingBytesGet() > 0)
    {
        /* Enable TX interrupt as data is pending for transmission */
        USART1_TX_INT_ENABLE();
    }

    return nBytesWritten;
}

size_t USART1_WriteFreeBufferCountGet(void)
{
    return (USART1_WRITE_BUFFER_SIZE - 1) - USART1_WriteCountGet();
}

size_t USART1_WriteBufferSizeGet(void)
{
    return (USART1_WRITE_BUFFER_SIZE - 1);
}

bool USART1_WriteNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = usart1Obj.isWrNotificationEnabled;

    usart1Obj.isWrNotificationEnabled = isEnabled;

    usart1Obj.isWrNotifyPersistently = isPersistent;

    return previousStatus;
}

void USART1_WriteThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        usart1Obj.wrThreshold = nBytesThreshold;
    }
}

void USART1_WriteCallbackRegister( USART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    usart1Obj.wrCallback = callback;

    usart1Obj.wrContext = context;
}

static void USART1_ISR_RX_Handler( void )
{
    /* Keep reading until there is a character availabe in the RX FIFO */
    while(US_CSR_USART_RXRDY_Msk == (USART1_REGS->US_CSR& US_CSR_USART_RXRDY_Msk))
    {
        if (USART1_RxPushByte( (uint8_t )(USART1_REGS->US_RHR& US_RHR_RXCHR_Msk) ) == true)
        {
            USART1_ReadNotificationSend();
        }
        else
        {
            /* UART RX buffer is full */
        }
    }
}

static void USART1_ISR_TX_Handler( void )
{
    uint8_t wrByte;

    /* Keep writing to the TX FIFO as long as there is space */
    while(US_CSR_USART_TXEMPTY_Msk == (USART1_REGS->US_CSR& US_CSR_USART_TXEMPTY_Msk))
    {
        if (USART1_TxPullByte(&wrByte) == true)
        {
            USART1_REGS->US_THR |= wrByte;

            /* Send notification */
            USART1_WriteNotificationSend();
        }
        else
        {
            /* Nothing to transmit. Disable the data register empty interrupt. */
            USART1_TX_INT_DISABLE();
            break;
        }
    }
}

void USART1_InterruptHandler( void )
{
    /* Error status */
    uint32_t errorStatus = (USART1_REGS->US_CSR & (US_CSR_USART_OVRE_Msk | US_CSR_USART_FRAME_Msk | US_CSR_USART_PARE_Msk));

    if(errorStatus != 0)
    {
        /* Client must call USARTx_ErrorGet() function to clear the errors */

        /* Disable Read, Overrun, Parity and Framing error interrupts */
        USART1_REGS->US_IDR = (US_IDR_USART_RXRDY_Msk | US_IDR_USART_FRAME_Msk | US_IDR_USART_PARE_Msk | US_IDR_USART_OVRE_Msk);

        /* USART errors are normally associated with the receiver, hence calling receiver callback */
        if( usart1Obj.rdCallback != NULL )
        {
            usart1Obj.rdCallback(USART_EVENT_READ_ERROR, usart1Obj.rdContext);
        }
    }

    /* Receiver status */
    if(US_CSR_USART_RXRDY_Msk == (USART1_REGS->US_CSR & US_CSR_USART_RXRDY_Msk))
    {
        USART1_ISR_RX_Handler();
    }

    /* Transmitter status */
    if(US_CSR_USART_TXRDY_Msk == (USART1_REGS->US_CSR & US_CSR_USART_TXRDY_Msk))
    {
        USART1_ISR_TX_Handler();
    }
}