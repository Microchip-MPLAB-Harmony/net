/*******************************************************************************
  DBGU PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_dbgu.c

  Summary:
    DBGU PLIB Implementation File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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
#include "plib_dbgu.h"

// *****************************************************************************
// *****************************************************************************
// Section: DBGU Implementation
// *****************************************************************************
// *****************************************************************************

DBGU_RING_BUFFER_OBJECT dbguObj;

#define DBGU_READ_BUFFER_SIZE      512
/* Disable Read, Overrun, Parity and Framing error interrupts */
#define DBGU_RX_INT_DISABLE()      DBGU_REGS->DBGU_IDR = (DBGU_IDR_RXRDY_Msk | DBGU_IDR_FRAME_Msk | DBGU_IDR_PARE_Msk | DBGU_IDR_OVRE_Msk);
/* Enable Read, Overrun, Parity and Framing error interrupts */
#define DBGU_RX_INT_ENABLE()       DBGU_REGS->DBGU_IER = (DBGU_IER_RXRDY_Msk | DBGU_IER_FRAME_Msk | DBGU_IER_PARE_Msk | DBGU_IER_OVRE_Msk);

static uint8_t DBGU_ReadBuffer[DBGU_READ_BUFFER_SIZE];

#define DBGU_WRITE_BUFFER_SIZE     1280
#define DBGU_TX_INT_DISABLE()      DBGU_REGS->DBGU_IDR = DBGU_IDR_TXEMPTY_Msk;
#define DBGU_TX_INT_ENABLE()       DBGU_REGS->DBGU_IER = DBGU_IER_TXEMPTY_Msk;

static uint8_t DBGU_WriteBuffer[DBGU_WRITE_BUFFER_SIZE];

void DBGU_Initialize( void )
{
    /* Reset DBGU */
    DBGU_REGS->DBGU_CR = (DBGU_CR_RSTRX_Msk | DBGU_CR_RSTTX_Msk | DBGU_CR_RSTSTA_Msk);

    /* Enable DBGU */
    DBGU_REGS->DBGU_CR = (DBGU_CR_TXEN_Msk | DBGU_CR_RXEN_Msk);

    /* Configure DBGU mode */
    DBGU_REGS->DBGU_MR = (DBGU_MR_BRSRCCK(0) | (DBGU_MR_PAR_NO) | (0 << DBGU_MR_FILTER_Pos));

    /* Configure DBGU Baud Rate */
    DBGU_REGS->DBGU_BRGR = DBGU_BRGR_CD(108);

    /* Initialize instance object */
    dbguObj.rdCallback = NULL;
    dbguObj.rdInIndex = 0;
    dbguObj.rdOutIndex = 0;
    dbguObj.isRdNotificationEnabled = false;
    dbguObj.isRdNotifyPersistently = false;
    dbguObj.rdThreshold = 0;
    dbguObj.wrCallback = NULL;
    dbguObj.wrInIndex = 0;
    dbguObj.wrOutIndex = 0;
    dbguObj.isWrNotificationEnabled = false;
    dbguObj.isWrNotifyPersistently = false;
    dbguObj.wrThreshold = 0;

    /* Enable receive interrupt */
    DBGU_RX_INT_ENABLE()
}

bool DBGU_SerialSetup( DBGU_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    bool status = false;
    uint32_t baud = setup->baudRate;
    uint32_t brgVal = 0;
    uint32_t dbguMode;

    if (setup != NULL)
    {
        if(srcClkFreq == 0)
        {
            srcClkFreq = DBGU_FrequencyGet();
        }

        /* Calculate BRG value */
        brgVal = srcClkFreq / (16 * baud);

        /* If the target baud rate is acheivable using this clock */
        if (brgVal <= 65535)
        {
            /* Configure DBGU mode */
            dbguMode = DBGU_REGS->DBGU_MR;
            dbguMode &= ~DBGU_MR_PAR_Msk;
            DBGU_REGS->DBGU_MR = dbguMode | setup->parity ;

            /* Configure DBGU Baud Rate */
            DBGU_REGS->DBGU_BRGR = DBGU_BRGR_CD(brgVal);

            status = true;
        }
    }

    return status;
}

static void DBGU_ErrorClear( void )
{
    uint8_t dummyData = 0u;

    DBGU_REGS->DBGU_CR = DBGU_CR_RSTSTA_Msk;

    /* Flush existing error bytes from the RX FIFO */
    while( DBGU_SR_RXRDY_Msk == (DBGU_REGS->DBGU_SR & DBGU_SR_RXRDY_Msk) )
    {
        dummyData = (DBGU_REGS->DBGU_RHR & DBGU_RHR_RXCHR_Msk);
    }

    /* Ignore the warning */
    (void)dummyData;
}

DBGU_ERROR DBGU_ErrorGet( void )
{
    DBGU_ERROR errors = DBGU_ERROR_NONE;
    uint32_t status = DBGU_REGS->DBGU_SR;

    errors = (DBGU_ERROR)(status & (DBGU_SR_OVRE_Msk | DBGU_SR_PARE_Msk | DBGU_SR_FRAME_Msk));

    if(errors != DBGU_ERROR_NONE)
    {
        DBGU_ErrorClear();
    }

    /* All errors are cleared, but send the previous error state */
    return errors;
}

/* This routine is only called from ISR. Hence do not disable/enable DBGU interrupts. */
static inline bool DBGU_RxPushByte(uint8_t rdByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;

    tempInIndex = dbguObj.rdInIndex + 1;

    if (tempInIndex >= DBGU_READ_BUFFER_SIZE)
    {
        tempInIndex = 0;
    }

    if (tempInIndex == dbguObj.rdOutIndex)
    {
        /* Queue is full - Report it to the application. Application gets a chance to free up space by reading data out from the RX ring buffer */
        if(dbguObj.rdCallback != NULL)
        {
            dbguObj.rdCallback(DBGU_EVENT_READ_BUFFER_FULL, dbguObj.rdContext);
        }

        /* Read the indices again in case application has freed up space in RX ring buffer */
        tempInIndex = dbguObj.rdInIndex + 1;

        if (tempInIndex >= DBGU_READ_BUFFER_SIZE)
        {
            tempInIndex = 0;
        }

    }

    if (tempInIndex != dbguObj.rdOutIndex)
    {
        DBGU_ReadBuffer[dbguObj.rdInIndex] = rdByte;
        dbguObj.rdInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Data will be lost. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable DBGU interrupts. */
static void DBGU_ReadNotificationSend(void)
{
    uint32_t nUnreadBytesAvailable;

    if (dbguObj.isRdNotificationEnabled == true)
    {
        nUnreadBytesAvailable = DBGU_ReadCountGet();

        if(dbguObj.rdCallback != NULL)
        {
            if (dbguObj.isRdNotifyPersistently == true)
            {
                if (nUnreadBytesAvailable >= dbguObj.rdThreshold)
                {
                    dbguObj.rdCallback(DBGU_EVENT_READ_THRESHOLD_REACHED, dbguObj.rdContext);
                }
            }
            else
            {
                if (nUnreadBytesAvailable == dbguObj.rdThreshold)
                {
                    dbguObj.rdCallback(DBGU_EVENT_READ_THRESHOLD_REACHED, dbguObj.rdContext);
                }
            }
        }
    }
}

size_t DBGU_Read(uint8_t* pRdBuffer, const size_t size)
{
    size_t nBytesRead = 0;
    uint32_t rdOutIndex;
    uint32_t rdInIndex;

    while (nBytesRead < size)
    {
        DBGU_RX_INT_DISABLE();

        rdOutIndex = dbguObj.rdOutIndex;
        rdInIndex = dbguObj.rdInIndex;

        if (rdOutIndex != rdInIndex)
        {
            pRdBuffer[nBytesRead++] = DBGU_ReadBuffer[dbguObj.rdOutIndex++];

            if (dbguObj.rdOutIndex >= DBGU_READ_BUFFER_SIZE)
            {
                dbguObj.rdOutIndex = 0;
            }
            DBGU_RX_INT_ENABLE();
        }
        else
        {
            DBGU_RX_INT_ENABLE();
            break;
        }
    }

    return nBytesRead;
}

size_t DBGU_ReadCountGet(void)
{
    size_t nUnreadBytesAvailable;
    uint32_t rdInIndex;
    uint32_t rdOutIndex;

    /* Take  snapshot of indices to avoid creation of critical section */
    rdInIndex = dbguObj.rdInIndex;
    rdOutIndex = dbguObj.rdOutIndex;

    if ( rdInIndex >=  rdOutIndex)
    {
        nUnreadBytesAvailable =  rdInIndex - rdOutIndex;
    }
    else
    {
        nUnreadBytesAvailable =  (DBGU_READ_BUFFER_SIZE -  rdOutIndex) + rdInIndex;
    }

    return nUnreadBytesAvailable;
}

size_t DBGU_ReadFreeBufferCountGet(void)
{
    return (DBGU_READ_BUFFER_SIZE - 1) - DBGU_ReadCountGet();
}

size_t DBGU_ReadBufferSizeGet(void)
{
    return (DBGU_READ_BUFFER_SIZE - 1);
}

bool DBGU_ReadNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = dbguObj.isRdNotificationEnabled;

    dbguObj.isRdNotificationEnabled = isEnabled;

    dbguObj.isRdNotifyPersistently = isPersistent;

    return previousStatus;
}

void DBGU_ReadThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        dbguObj.rdThreshold = nBytesThreshold;
    }
}

void DBGU_ReadCallbackRegister( DBGU_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    dbguObj.rdCallback = callback;

    dbguObj.rdContext = context;
}

/* This routine is only called from ISR. Hence do not disable/enable DBGU interrupts. */
static bool DBGU_TxPullByte(uint8_t* pWrByte)
{
    bool isSuccess = false;
    uint32_t wrOutIndex = dbguObj.wrOutIndex;
    uint32_t wrInIndex = dbguObj.wrInIndex;

    if (wrOutIndex != wrInIndex)
    {
        *pWrByte = DBGU_WriteBuffer[dbguObj.wrOutIndex++];

        if (dbguObj.wrOutIndex >= DBGU_WRITE_BUFFER_SIZE)
        {
            dbguObj.wrOutIndex = 0;
        }
        isSuccess = true;
    }

    return isSuccess;
}

static inline bool DBGU_TxPushByte(uint8_t wrByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;

    tempInIndex = dbguObj.wrInIndex + 1;

    if (tempInIndex >= DBGU_WRITE_BUFFER_SIZE)
    {
        tempInIndex = 0;
    }
    if (tempInIndex != dbguObj.wrOutIndex)
    {
        DBGU_WriteBuffer[dbguObj.wrInIndex] = wrByte;
        dbguObj.wrInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Report Error. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable DBGU interrupts. */
static void DBGU_WriteNotificationSend(void)
{
    uint32_t nFreeWrBufferCount;

    if (dbguObj.isWrNotificationEnabled == true)
    {
        nFreeWrBufferCount = DBGU_WriteFreeBufferCountGet();

        if(dbguObj.wrCallback != NULL)
        {
            if (dbguObj.isWrNotifyPersistently == true)
            {
                if (nFreeWrBufferCount >= dbguObj.wrThreshold)
                {
                    dbguObj.wrCallback(DBGU_EVENT_WRITE_THRESHOLD_REACHED, dbguObj.wrContext);
                }
            }
            else
            {
                if (nFreeWrBufferCount == dbguObj.wrThreshold)
                {
                    dbguObj.wrCallback(DBGU_EVENT_WRITE_THRESHOLD_REACHED, dbguObj.wrContext);
                }
            }
        }
    }
}

static size_t DBGU_WritePendingBytesGet(void)
{
    size_t nPendingTxBytes;

    /* Take a snapshot of indices to avoid creation of critical section */
    uint32_t wrOutIndex = dbguObj.wrOutIndex;
    uint32_t wrInIndex = dbguObj.wrInIndex;

    if ( wrInIndex >=  wrOutIndex)
    {
        nPendingTxBytes =  wrInIndex -  wrOutIndex;
    }
    else
    {
        nPendingTxBytes =  (DBGU_WRITE_BUFFER_SIZE -  wrOutIndex) + wrInIndex;
    }

    return nPendingTxBytes;
}

size_t DBGU_WriteCountGet(void)
{
    size_t nPendingTxBytes;

    nPendingTxBytes = DBGU_WritePendingBytesGet();

    return nPendingTxBytes;
}

size_t DBGU_Write(uint8_t* pWrBuffer, const size_t size )
{
    size_t nBytesWritten  = 0;

    DBGU_TX_INT_DISABLE();

    while (nBytesWritten < size)
    {
        if (DBGU_TxPushByte(pWrBuffer[nBytesWritten]) == true)
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
    if (DBGU_WritePendingBytesGet() > 0)
    {
        /* Enable TX interrupt as data is pending for transmission */
        DBGU_TX_INT_ENABLE();
    }

    return nBytesWritten;
}

size_t DBGU_WriteFreeBufferCountGet(void)
{
    return (DBGU_WRITE_BUFFER_SIZE - 1) - DBGU_WriteCountGet();
}

size_t DBGU_WriteBufferSizeGet(void)
{
    return (DBGU_WRITE_BUFFER_SIZE - 1);
}

bool DBGU_WriteNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = dbguObj.isWrNotificationEnabled;

    dbguObj.isWrNotificationEnabled = isEnabled;

    dbguObj.isWrNotifyPersistently = isPersistent;

    return previousStatus;
}

void DBGU_WriteThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        dbguObj.wrThreshold = nBytesThreshold;
    }
}

void DBGU_WriteCallbackRegister( DBGU_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    dbguObj.wrCallback = callback;

    dbguObj.wrContext = context;
}

static void DBGU_ISR_RX_Handler( void )
{
    /* Keep reading until there is a character availabe in the RX FIFO */
    while(DBGU_SR_RXRDY_Msk == (DBGU_REGS->DBGU_SR& DBGU_SR_RXRDY_Msk))
    {
        if (DBGU_RxPushByte( (uint8_t )(DBGU_REGS->DBGU_RHR& DBGU_RHR_RXCHR_Msk) ) == true)
        {
            DBGU_ReadNotificationSend();
        }
        else
        {
            /* DBGU RX buffer is full */
        }
    }
}

static void DBGU_ISR_TX_Handler( void )
{
    uint8_t wrByte;

    /* Keep writing to the TX FIFO as long as there is space */
    while(DBGU_SR_TXRDY_Msk == (DBGU_REGS->DBGU_SR & DBGU_SR_TXRDY_Msk))
    {
        if (DBGU_TxPullByte(&wrByte) == true)
        {
            DBGU_REGS->DBGU_THR |= wrByte;

            /* Send notification */
            DBGU_WriteNotificationSend();
        }
        else
        {
            /* Nothing to transmit. Disable the data register empty interrupt. */
            DBGU_TX_INT_DISABLE();
            break;
        }
    }
}

void DBGU_InterruptHandler( void )
{
    /* Error status */
    uint32_t errorStatus = (DBGU_REGS->DBGU_SR & (DBGU_SR_OVRE_Msk | DBGU_SR_FRAME_Msk | DBGU_SR_PARE_Msk));

    if(errorStatus != 0)
    {
        /* Client must call DBGUx_ErrorGet() function to clear the errors */

        /* Disable Read, Overrun, Parity and Framing error interrupts */

        DBGU_REGS->DBGU_IDR = (DBGU_IDR_RXRDY_Msk | DBGU_IDR_FRAME_Msk | DBGU_IDR_PARE_Msk | DBGU_IDR_OVRE_Msk);

        /* DBGU errors are normally associated with the receiver, hence calling
         * receiver callback */
        if( dbguObj.rdCallback != NULL )
        {
            dbguObj.rdCallback(DBGU_EVENT_READ_ERROR, dbguObj.rdContext);
        }
    }

    /* Receiver status */
    if(DBGU_SR_RXRDY_Msk == (DBGU_REGS->DBGU_SR & DBGU_SR_RXRDY_Msk))
    {
        DBGU_ISR_RX_Handler();
    }

    /* Transmitter status */
    if(DBGU_SR_TXRDY_Msk == (DBGU_REGS->DBGU_SR & DBGU_SR_TXRDY_Msk))
    {
        DBGU_ISR_TX_Handler();
    }

}