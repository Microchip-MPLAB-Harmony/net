/*******************************************************************************
  SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_spi0.c

  Summary:
    SPI0 Source File

  Description:
    This file has implementation of all the interfaces provided for particular
    SPI peripheral.

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

#include "plib_spi0.h"

// *****************************************************************************
// *****************************************************************************
// Section: SPI0 Implementation
// *****************************************************************************
// *****************************************************************************
/* Global object to save SPI Exchange related data */
SPI_OBJECT spi0Obj;

void SPI0_Initialize ( void )
{
    /* Disable and Reset the SPI*/
    SPI0_REGS->SPI_CR = SPI_CR_SPIDIS_Msk | SPI_CR_SWRST_Msk;

    /* Enable Master mode, select source clock, select particular NPCS line for chip select and disable mode fault detection */
    SPI0_REGS->SPI_MR =  SPI_MR_MSTR_Msk | SPI_MR_BRSRCCLK_PERIPH_CLK | SPI_MR_PCS_NPCS0 | SPI_MR_MODFDIS_Msk;

    /* Set up clock Polarity, data phase, Communication Width and Baud Rate */
    SPI0_REGS->SPI_CSR[0] = SPI_CSR_CPOL_IDLE_LOW | SPI_CSR_NCPHA_VALID_LEADING_EDGE | SPI_CSR_BITS_8_BIT | SPI_CSR_SCBR(83);

    /* Initialize global variables */
    spi0Obj.transferIsBusy = false;
    spi0Obj.callback = NULL;

    /* Enable SPI0 */
    SPI0_REGS->SPI_CR = SPI_CR_SPIEN_Msk;
    return;
}

bool SPI0_WriteRead (void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize)
{
    bool isRequestAccepted = false;
    uint32_t dummyData;

    /* Verify the request */
    if((((txSize > 0) && (pTransmitData != NULL)) || ((rxSize > 0) && (pReceiveData != NULL))) && (spi0Obj.transferIsBusy == false))
    {
        isRequestAccepted = true;
        spi0Obj.txBuffer = pTransmitData;
        spi0Obj.rxBuffer = pReceiveData;
        spi0Obj.rxCount = 0;
        spi0Obj.txCount = 0;
        spi0Obj.dummySize = 0;
        if (pTransmitData != NULL)
        {
            spi0Obj.txSize = txSize;
        }
        else
        {
            spi0Obj.txSize = 0;
        }

        if (pReceiveData != NULL)
        {
            spi0Obj.rxSize = rxSize;
        }
        else
        {
            spi0Obj.rxSize = 0;
        }

        spi0Obj.transferIsBusy = true;

        /* Flush out any unread data in SPI read buffer */
        dummyData = (SPI0_REGS->SPI_RDR & SPI_RDR_RD_Msk) >> SPI_RDR_RD_Pos;
        (void)dummyData;

        if (spi0Obj.rxSize > spi0Obj.txSize)
        {
            spi0Obj.dummySize = spi0Obj.rxSize - spi0Obj.txSize;
        }

        /* Start the first write here itself, rest will happen in ISR context */
        if((SPI0_REGS->SPI_CSR[0] & SPI_CSR_BITS_Msk) == SPI_CSR_BITS_8_BIT)
        {
            if (spi0Obj.txCount < spi0Obj.txSize)
            {
                SPI0_REGS->SPI_TDR = *((uint8_t*)spi0Obj.txBuffer);
                spi0Obj.txCount++;
            }
            else if (spi0Obj.dummySize > 0)
            {
                SPI0_REGS->SPI_TDR = (uint8_t)(0xff);
                spi0Obj.dummySize--;
            }
        }
        else
        {
            spi0Obj.txSize >>= 1;
            spi0Obj.dummySize >>= 1;
            spi0Obj.rxSize >>= 1;

            if (spi0Obj.txCount < spi0Obj.txSize)
            {
                SPI0_REGS->SPI_TDR = *((uint16_t*)spi0Obj.txBuffer);
                spi0Obj.txCount++;
            }
            else if (spi0Obj.dummySize > 0)
            {
                SPI0_REGS->SPI_TDR = (uint16_t)(0xff);
                spi0Obj.dummySize--;
            }
        }

        if (rxSize > 0)
        {
            /* Enable receive interrupt to complete the transfer in ISR context */
            SPI0_REGS->SPI_IER = SPI_IER_RDRF_Msk;
        }
        else
        {
            /* Enable transmit interrupt to complete the transfer in ISR context */
            SPI0_REGS->SPI_IER = SPI_IER_TDRE_Msk;
        }
    }

    return isRequestAccepted;
}

bool SPI0_Write(void* pTransmitData, size_t txSize)
{
    return(SPI0_WriteRead(pTransmitData, txSize, NULL, 0));
}

bool SPI0_Read(void* pReceiveData, size_t rxSize)
{
    return(SPI0_WriteRead(NULL, 0, pReceiveData, rxSize));
}

bool SPI0_TransferSetup (SPI_TRANSFER_SETUP * setup, uint32_t spiSourceClock )
{
    uint32_t scbr;
    if ((setup == NULL) || (setup->clockFrequency == 0))
	{
		return false;
	}
    if(spiSourceClock == 0)
    {
        // Fetch Master Clock Frequency directly
        spiSourceClock = 83000000;
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

    SPI0_REGS->SPI_CSR[0]= (uint32_t)setup->clockPolarity | (uint32_t)setup->clockPhase | (uint32_t)setup->dataBits | SPI_CSR_SCBR(scbr);

    return true;
}

void SPI0_CallbackRegister (SPI_CALLBACK callback, uintptr_t context)
{
    spi0Obj.callback = callback;
    spi0Obj.context = context;
}

bool SPI0_IsBusy()
{
    return spi0Obj.transferIsBusy;
}

void SPI0_InterruptHandler(void)
{
    uint32_t dataBits ;
    uint32_t receivedData;
    static bool isLastByteTransferInProgress = false;

    dataBits = SPI0_REGS->SPI_CSR[0] & SPI_CSR_BITS_Msk;

    if ((SPI0_REGS->SPI_SR & SPI_SR_RDRF_Msk ) == SPI_SR_RDRF_Msk)
    {
        receivedData = (SPI0_REGS->SPI_RDR & SPI_RDR_RD_Msk) >> SPI_RDR_RD_Pos;

        if (spi0Obj.rxCount < spi0Obj.rxSize)
        {
            if(dataBits == SPI_CSR_BITS_8_BIT)
            {
                ((uint8_t*)spi0Obj.rxBuffer)[spi0Obj.rxCount++] = receivedData;
            }
            else
            {
                ((uint16_t*)spi0Obj.rxBuffer)[spi0Obj.rxCount++] = receivedData;
            }
        }
    }

    /* If there are more words to be transmitted, then transmit them here and keep track of the count */
    if((SPI0_REGS->SPI_SR & SPI_SR_TDRE_Msk) == SPI_SR_TDRE_Msk)
    {

        /* Disable the TDRE interrupt. This will be enabled back if more than
         * one byte is pending to be transmitted */

        SPI0_REGS->SPI_IDR = SPI_IDR_TDRE_Msk;

        if(dataBits == SPI_CSR_BITS_8_BIT)
        {
            if (spi0Obj.txCount < spi0Obj.txSize)
            {
                SPI0_REGS->SPI_TDR = ((uint8_t*)spi0Obj.txBuffer)[spi0Obj.txCount++];
            }
            else if (spi0Obj.dummySize > 0)
            {
                SPI0_REGS->SPI_TDR = (uint8_t)(0xff);
                spi0Obj.dummySize--;
            }
        }
        else
        {
            if (spi0Obj.txCount < spi0Obj.txSize)
            {
                SPI0_REGS->SPI_TDR = ((uint16_t*)spi0Obj.txBuffer)[spi0Obj.txCount++];
            }
            else if (spi0Obj.dummySize > 0)
            {
                SPI0_REGS->SPI_TDR = (uint16_t)(0xff);
                spi0Obj.dummySize--;
            }
        }
        if ((spi0Obj.txCount == spi0Obj.txSize) && (spi0Obj.dummySize == 0))
        {
            /* At higher baud rates, the data in the shift register can be
             * shifted out and TXEMPTY flag can get set resulting in a
             * callback been given to the application with the SPI interrupt
             * pending with the application. This will then result in the
             * interrupt handler being called again with nothing to transmit.
             * To avoid this, a software flag is set, but
             * the TXEMPTY interrupt is not enabled until the very end.
             */

            isLastByteTransferInProgress = true;
        }
        else if (spi0Obj.rxCount == spi0Obj.rxSize)
        {
            /* Enable TDRE interrupt as all the requested bytes are received
             * and can now make use of the internal transmit shift register.
             */
            SPI0_REGS->SPI_IDR = SPI_IDR_RDRF_Msk;
            SPI0_REGS->SPI_IER = SPI_IDR_TDRE_Msk;
        }
    }

    /* See if Exchange is complete */
    if ((isLastByteTransferInProgress == true) && ((SPI0_REGS->SPI_SR & SPI_SR_TXEMPTY_Msk) == SPI_SR_TXEMPTY_Msk))
    {
        if (spi0Obj.rxCount == spi0Obj.rxSize)
        {
            spi0Obj.transferIsBusy = false;

            /* Disable TDRE, RDRF and TXEMPTY interrupts */
            SPI0_REGS->SPI_IDR = SPI_IDR_TDRE_Msk | SPI_IDR_RDRF_Msk | SPI_IDR_TXEMPTY_Msk;

            isLastByteTransferInProgress = false;

            if(spi0Obj.callback != NULL)
            {
                spi0Obj.callback(spi0Obj.context);
            }
        }
    }
    if (isLastByteTransferInProgress == true)
    {
        /* For the last byte transfer, the TDRE interrupt is already disabled.
         * Enable TXEMPTY interrupt to ensure no data is present in the shift
         * register before application callback is called.
         */
        SPI0_REGS->SPI_IER = SPI_IER_TXEMPTY_Msk;
    }

}


/*******************************************************************************
 End of File
*/

