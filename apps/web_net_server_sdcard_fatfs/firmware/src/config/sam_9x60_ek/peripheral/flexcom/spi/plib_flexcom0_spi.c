/*******************************************************************************
  FLEXCOM0 SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_flexcom0_spi.c

  Summary:
    FLEXCOM0 SPI PLIB Implementation File.

  Description:
    This file defines the interface to the FLEXCOM SPI peripheral library.
    This library provides access to and control of the associated
    peripheral instance.

  Remarks:
    None.

*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#include "plib_flexcom0_spi.h"

// *****************************************************************************
// *****************************************************************************
// Section: FLEXCOM0 SPI Implementation
// *****************************************************************************
// *****************************************************************************
/* Global object to save FLEXCOM SPI Exchange related data */
FLEXCOM_SPI_OBJECT flexcom0SpiObj;

void FLEXCOM0_SPI_Initialize ( void )
{
    /* Set FLEXCOM SPI operating mode */
    FLEXCOM0_REGS->FLEX_MR = FLEX_MR_OPMODE_SPI;

    /* Disable and Reset the FLEXCOM SPI */
    FLEXCOM0_REGS->FLEX_SPI_CR = FLEX_SPI_CR_SPIDIS_Msk | FLEX_SPI_CR_SWRST_Msk;

    /* Enable Master mode, select clock source, select particular NPCS line for chip select and disable mode fault detection */
    FLEXCOM0_REGS->FLEX_SPI_MR = FLEX_SPI_MR_MSTR_Msk | FLEX_SPI_MR_BRSRCCLK_PERIPH_CLK | FLEX_SPI_MR_PCS(0) | FLEX_SPI_MR_MODFDIS_Msk;

    /* Set up clock Polarity, data phase, Communication Width, Baud Rate and Chip select active after transfer */
    FLEXCOM0_REGS->FLEX_SPI_CSR[0]= FLEX_SPI_CSR_CPOL(0) | FLEX_SPI_CSR_NCPHA(1) | FLEX_SPI_CSR_BITS_8_BIT | FLEX_SPI_CSR_SCBR(200) | FLEX_SPI_CSR_CSAAT_Msk;

    /* Initialize global variables */
    flexcom0SpiObj.transferIsBusy = false;
    flexcom0SpiObj.callback = NULL;

    /* Enable FLEXCOM0 SPI */
    FLEXCOM0_REGS->FLEX_SPI_CR = FLEX_SPI_CR_SPIEN_Msk;
    return;
}

bool FLEXCOM0_SPI_WriteRead (void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize)
{
    bool isRequestAccepted = false;
    uint32_t dummyData;

    /* Verify the request */
    if((((txSize > 0) && (pTransmitData != NULL)) || ((rxSize > 0) && (pReceiveData != NULL))) && (flexcom0SpiObj.transferIsBusy == false))
    {
        isRequestAccepted = true;
        flexcom0SpiObj.txBuffer = pTransmitData;
        flexcom0SpiObj.rxBuffer = pReceiveData;
        flexcom0SpiObj.rxCount = 0;
        flexcom0SpiObj.txCount = 0;
        flexcom0SpiObj.dummySize = 0;
        if (pTransmitData != NULL)
        {
            flexcom0SpiObj.txSize = txSize;
        }
        else
        {
            flexcom0SpiObj.txSize = 0;
        }

        if (pReceiveData != NULL)
        {
            flexcom0SpiObj.rxSize = rxSize;
        }
        else
        {
            flexcom0SpiObj.rxSize = 0;
        }

        flexcom0SpiObj.transferIsBusy = true;

        /* Flush out any unread data in SPI read buffer */
        dummyData = (FLEXCOM0_REGS->FLEX_SPI_RDR & FLEX_SPI_RDR_RD_Msk) >> FLEX_SPI_RDR_RD_Pos;
        (void)dummyData;

        if (flexcom0SpiObj.rxSize > flexcom0SpiObj.txSize)
        {
            flexcom0SpiObj.dummySize = flexcom0SpiObj.rxSize - flexcom0SpiObj.txSize;
        }

        /* Start the first write here itself, rest will happen in ISR context */
        if((FLEXCOM0_REGS->FLEX_SPI_CSR[0] & FLEX_SPI_CSR_BITS_Msk) == FLEX_SPI_CSR_BITS_8_BIT)
        {
            if (flexcom0SpiObj.txCount < flexcom0SpiObj.txSize)
            {
                FLEXCOM0_REGS->FLEX_SPI_TDR = *((uint8_t*)flexcom0SpiObj.txBuffer);
                flexcom0SpiObj.txCount++;
            }
            else if (flexcom0SpiObj.dummySize > 0)
            {
                FLEXCOM0_REGS->FLEX_SPI_TDR = (uint8_t)(0xff);
                flexcom0SpiObj.dummySize--;
            }
        }
        else
        {
            flexcom0SpiObj.txSize >>= 1;
            flexcom0SpiObj.dummySize >>= 1;
            flexcom0SpiObj.rxSize >>= 1;

            if (flexcom0SpiObj.txCount < flexcom0SpiObj.txSize)
            {
                FLEXCOM0_REGS->FLEX_SPI_TDR = *((uint16_t*)flexcom0SpiObj.txBuffer);
                flexcom0SpiObj.txCount++;
            }
            else if (flexcom0SpiObj.dummySize > 0)
            {
                FLEXCOM0_REGS->FLEX_SPI_TDR = (uint16_t)(0xff);
                flexcom0SpiObj.dummySize--;
            }
        }

        if (rxSize > 0)
        {
            /* Enable receive interrupt to complete the transfer in ISR context */
            FLEXCOM0_REGS->FLEX_SPI_IER = FLEX_SPI_IER_RDRF_Msk;
        }
        else
        {
            /* Enable transmit interrupt to complete the transfer in ISR context */
            FLEXCOM0_REGS->FLEX_SPI_IER = FLEX_SPI_IER_TDRE_Msk;
        }
    }

    return isRequestAccepted;
}

bool FLEXCOM0_SPI_TransferSetup (FLEXCOM_SPI_TRANSFER_SETUP * setup, uint32_t spiSourceClock )
{
    uint32_t scbr;
    if ((setup == NULL) || (setup->clockFrequency == 0))
    {
        return false;
    }
    if(spiSourceClock == 0)
    {
        // Fetch Master Clock Frequency directly
        spiSourceClock = 200000000;
    }

    scbr = spiSourceClock/setup->clockFrequency;

    if(scbr == 0)
    {
        scbr = 1;
    }
    else if(scbr > 255)
    {
        scbr = 255;
    }

    FLEXCOM0_REGS->FLEX_SPI_CSR[0]= (uint32_t)setup->clockPolarity | (uint32_t)setup->clockPhase | (uint32_t)setup->dataBits | FLEX_SPI_CSR_SCBR(scbr);

    return true;
}

bool FLEXCOM0_SPI_Write(void* pTransmitData, size_t txSize)
{
    return(FLEXCOM0_SPI_WriteRead(pTransmitData, txSize, NULL, 0));
}

bool FLEXCOM0_SPI_Read(void* pReceiveData, size_t rxSize)
{
    return(FLEXCOM0_SPI_WriteRead(NULL, 0, pReceiveData, rxSize));
}

void FLEXCOM0_SPI_CallbackRegister (FLEXCOM_SPI_CALLBACK callback, uintptr_t context)
{
    flexcom0SpiObj.callback = callback;
    flexcom0SpiObj.context = context;
}

bool FLEXCOM0_SPI_IsBusy(void)
{
    return ((flexcom0SpiObj.transferIsBusy) || ((FLEXCOM0_REGS->FLEX_SPI_SR & FLEX_SPI_SR_TXEMPTY_Msk) == 0));
}

void FLEXCOM0_InterruptHandler(void)
{
    uint32_t dataBits ;
    uint32_t receivedData;
    dataBits = FLEXCOM0_REGS->FLEX_SPI_CSR[0] & FLEX_SPI_CSR_BITS_Msk;

    static bool isLastByteTransferInProgress = false;

    /* save the status in global object before it gets cleared */
    flexcom0SpiObj.status = FLEXCOM0_REGS->FLEX_SPI_SR;

    if ((FLEXCOM0_REGS->FLEX_SPI_SR & FLEX_SPI_SR_RDRF_Msk ) == FLEX_SPI_SR_RDRF_Msk)
    {
        receivedData = (FLEXCOM0_REGS->FLEX_SPI_RDR & FLEX_SPI_RDR_RD_Msk) >> FLEX_SPI_RDR_RD_Pos;

        if (flexcom0SpiObj.rxCount < flexcom0SpiObj.rxSize)
        {
            if(dataBits == FLEX_SPI_CSR_BITS_8_BIT)
            {
                ((uint8_t*)flexcom0SpiObj.rxBuffer)[flexcom0SpiObj.rxCount++] = receivedData;
            }
            else
            {
                ((uint16_t*)flexcom0SpiObj.rxBuffer)[flexcom0SpiObj.rxCount++] = receivedData;
            }
        }
    }

    /* If there are more words to be transmitted, then transmit them here and keep track of the count */
    if((FLEXCOM0_REGS->FLEX_SPI_SR & FLEX_SPI_SR_TDRE_Msk) == FLEX_SPI_SR_TDRE_Msk)
    {
        /* Disable the TDRE interrupt. This will be enabled back if more than
         * one byte is pending to be transmitted */
        FLEXCOM0_REGS->FLEX_SPI_IDR = FLEX_SPI_IDR_TDRE_Msk;

        if(dataBits == FLEX_SPI_CSR_BITS_8_BIT)
        {
            if (flexcom0SpiObj.txCount < flexcom0SpiObj.txSize)
            {
                FLEXCOM0_REGS->FLEX_SPI_TDR = ((uint8_t*)flexcom0SpiObj.txBuffer)[flexcom0SpiObj.txCount++];
            }
            else if (flexcom0SpiObj.dummySize > 0)
            {
                FLEXCOM0_REGS->FLEX_SPI_TDR = (uint8_t)(0xff);
                flexcom0SpiObj.dummySize--;
            }
        }
        else
        {
            if (flexcom0SpiObj.txCount < flexcom0SpiObj.txSize)
            {
                FLEXCOM0_REGS->FLEX_SPI_TDR = ((uint16_t*)flexcom0SpiObj.txBuffer)[flexcom0SpiObj.txCount++];
            }
            else if (flexcom0SpiObj.dummySize > 0)
            {
                FLEXCOM0_REGS->FLEX_SPI_TDR = (uint16_t)(0xff);
                flexcom0SpiObj.dummySize--;
            }
        }
        if ((flexcom0SpiObj.txCount == flexcom0SpiObj.txSize) && (flexcom0SpiObj.dummySize == 0))
        {
            /* At higher baud rates, the data in the shift register can be
             * shifted out and TXEMPTY flag can get set resulting in a
             * callback been given to the application with the SPI interrupt
             * pending with the application. This will then result in the
             * interrupt handler being called again with nothing to transmit.
             * To avoid the above mentioned issue, a software flag is set, but
             * the TXEMPTY interrupt is not enabled until the very end.
             * At higher baud rates, if the software flag is set and the
             * TXEMPTY status bit is set, then it means that the transfer is
             * complete and a callback can be given to the application. Since
             * the TXEMPTY interrupt is not enabled there is no need to
             * explicitly clear the pending interrupt from the Interrupt controller.
             */

            isLastByteTransferInProgress = true;
            /* Set Last transfer to deassert NPCS after the last byte written in TDR has been transferred. */
            FLEXCOM0_REGS->FLEX_SPI_CR = FLEX_SPI_CR_LASTXFER_Msk;
        }
        else if (flexcom0SpiObj.rxCount == flexcom0SpiObj.rxSize)
        {
            /* Enable TDRE interrupt as all the requested bytes are received
             * and can now make use of the internal transmit shift register.
             */
            FLEXCOM0_REGS->FLEX_SPI_IDR = FLEX_SPI_IDR_RDRF_Msk;
            FLEXCOM0_REGS->FLEX_SPI_IER = FLEX_SPI_IDR_TDRE_Msk;
        }
    }

    /* See if Exchange is complete */
    if ((isLastByteTransferInProgress == true) && ((FLEXCOM0_REGS->FLEX_SPI_SR & FLEX_SPI_SR_TXEMPTY_Msk) == FLEX_SPI_SR_TXEMPTY_Msk))
    {
        if (flexcom0SpiObj.rxCount == flexcom0SpiObj.rxSize)
        {
            flexcom0SpiObj.transferIsBusy = false;

            /* Disable TDRE, RDRF and TXEMPTY interrupts */
            FLEXCOM0_REGS->FLEX_SPI_IDR = FLEX_SPI_IDR_TDRE_Msk | FLEX_SPI_IDR_RDRF_Msk | FLEX_SPI_IDR_TXEMPTY_Msk;

            isLastByteTransferInProgress = false;

            if(flexcom0SpiObj.callback != NULL)
            {
                flexcom0SpiObj.callback(flexcom0SpiObj.context);
            }
        }
    }
    if (isLastByteTransferInProgress == true)
    {
        /* For the last byte transfer, the TDRE interrupt is already disabled.
         * Enable TXEMPTY interrupt to ensure no data is present in the shift
         * register before application callback is called.
         */
        FLEXCOM0_REGS->FLEX_SPI_IER = FLEX_SPI_IER_TXEMPTY_Msk;
    }

}


/*******************************************************************************
 End of File
*/

