/*******************************************************************************
  SDHC PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_sdhc.c

  Summary:
    SDHC PLIB Implementation File

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
#include "plib_sdhc.h"


// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "plib_sdhc_common.h"

#define SDHC_INTEN_Msk                                          0x03FF01FF
#define SDHC_EISIER_Msk                                         0x03FF0000
#define SDHC_MODE_RESPTYPE_48_BIT_BUSY                          (0x03 << 16)
#define SDHC_MODE_RESPTYPE_48_BIT                               (0x02 << 16)
#define SDHC_MODE_RESPTYPE_136_BIT                              (0x01 << 16)
#define SDHC_MODE_RESPTYPE_NONE                                 (0x00 << 16)

#define SDHC_DMA_NUM_DESCR_LINES               1
#define SDHC_BASE_CLOCK_FREQUENCY              200000000
#define SDHC_MAX_BLOCK_SIZE                    0x200

typedef unsigned long _paddr_t; /* a physical address */
#define KVA_TO_PA(v)    ((_paddr_t)(v) & 0x1fffffff)

static __attribute__((coherent, aligned(32))) SDHC_ADMA_DESCR sdhcDmaDescrTable[SDHC_DMA_NUM_DESCR_LINES];

static SDHC_OBJECT sdhcObj;

static void SDHC_VariablesInit ( void )
{
    sdhcObj.errorStatus = 0;
    sdhcObj.isCmdInProgress = false;
    sdhcObj.isDataInProgress = false;
    sdhcObj.callback = NULL;
}

static void SDHC_Delay(uint16_t timeout)
{
    while (timeout > 0)
    {
        timeout --;
        Nop ();
    }
}

static uint16_t SDHC_TransferModeSet ( uint32_t opcode )
{
    uint16_t transfer_mode_reg = 0;

    switch(opcode)
    {
        case 51:
        case 6:
        case 17:
            /* Read single block of data from the device. */
            transfer_mode_reg = (_SDHCMODE_DMAEN_MASK | _SDHCMODE_DTXDSEL_MASK);
            break;

        case 18:
            /* Read multiple blocks of data from the device. */
            transfer_mode_reg = (_SDHCMODE_DMAEN_MASK | _SDHCMODE_DTXDSEL_MASK | _SDHCMODE_BSEL_MASK | _SDHCMODE_BCEN_MASK);
            break;

        case 24:
            /* Write single block of data to the device. */
            transfer_mode_reg = _SDHCMODE_DMAEN_MASK;
            break;

        case 25:
            /* Write multiple blocks of data to the device. */
            transfer_mode_reg = (_SDHCMODE_DMAEN_MASK | _SDHCMODE_BSEL_MASK | _SDHCMODE_BCEN_MASK);
            break;

        default:
            break;
    }

    return transfer_mode_reg;
}

void SDHC_InterruptHandler(void)
{
    uint32_t nistr = 0;
    uint32_t eistr = 0;
    SDHC_XFER_STATUS xferStatus = 0;

    nistr = (SDHCINTSTAT & 0x0000FFFF);
    eistr = (SDHCINTSTAT & 0xFFFF0000);

    /* Clear the transmit interrupt flag */
    IFS5CLR = 0x80000000L;

    /* Save the error in a global variable for later use */
    sdhcObj.errorStatus |= eistr;

    if (nistr & _SDHCINTSTAT_CARDIIF_MASK)
    {
        xferStatus |= SDHC_XFER_STATUS_CARD_INSERTED;
    }
    if (nistr & _SDHCINTSTAT_CARDRIF_MASK)
    {
        xferStatus |= SDHC_XFER_STATUS_CARD_REMOVED;
    }

    if (sdhcObj.isCmdInProgress == true)
    {
        if (nistr & (_SDHCINTSTAT_CCIF_MASK | _SDHCINTSTAT_TXCIF_MASK | _SDHCINTSTAT_EIF_MASK))
        {
            if (nistr & _SDHCINTSTAT_EIF_MASK)
            {
                if (eistr & (_SDHCINTSTAT_CTOEIF_MASK |
                                      _SDHCINTSTAT_CCRCEIF_MASK |
                                      _SDHCINTSTAT_CEBEIF_MASK |
                                      _SDHCINTSTAT_CIDXEIF_MASK))
                {
                    SDHC_ErrorReset (SDHC_RESET_CMD);
                }
            }
            sdhcObj.isCmdInProgress = false;
            xferStatus |= SDHC_XFER_STATUS_CMD_COMPLETED;
        }
    }

    if (sdhcObj.isDataInProgress == true)
    {
        if (nistr & (_SDHCINTSTAT_TXCIF_MASK | _SDHCINTSTAT_DMAIF_MASK | _SDHCINTSTAT_EIF_MASK))
        {
            if (nistr & _SDHCINTSTAT_EIF_MASK)
            {
                if (eistr & (_SDHCINTSTAT_DTOEIF_MASK |
                            _SDHCINTSTAT_DCRCEIF_MASK |
                            _SDHCINTSTAT_DEBEIF_MASK))
                {
                    SDHC_ErrorReset (SDHC_RESET_DAT);
                }
            }
            if (nistr & _SDHCINTSTAT_TXCIF_MASK)
            {
                /* Clear the data timeout error as transfer complete has higher priority */
                sdhcObj.errorStatus &= ~_SDHCINTSTAT_DTOEIF_MASK;
            }
            sdhcObj.isDataInProgress = false;
            xferStatus |= SDHC_XFER_STATUS_DATA_COMPLETED;
        }
    }

    /* Clear normal interrupt and error status bits that have been processed */
    SDHCINTSTAT = (nistr | eistr);

    if ((sdhcObj.callback != NULL) && (xferStatus > 0))
    {
        sdhcObj.callback(xferStatus, sdhcObj.context);
    }
}

void SDHC_CardDetectEnable(void)
{
    CFGCON2bits.SDCDEN = 0x1;
}

void SDHC_CardDetectDisable(void)
{
    CFGCON2bits.SDCDEN = 0x0;
}

void SDHC_WriteProtectEnable(void)
{
    CFGCON2bits.SDWPEN = 0x1;
}

void SDHC_WriteProtectDisable(void)
{
    CFGCON2bits.SDWPEN = 0x0;
}

void SDHC_ErrorReset ( SDHC_RESET_TYPE resetType )
{
    SDHCCON2 |= (resetType << 24);

    /* Wait until host resets the error status */
    while (SDHCCON2 & (resetType << 24));
}

uint16_t SDHC_GetError(void)
{
    return sdhcObj.errorStatus;
}

uint16_t SDHC_CommandErrorGet(void)
{
    uint32_t errorStatus = sdhcObj.errorStatus;

    errorStatus &= (_SDHCINTSTAT_CTOEIF_MASK | _SDHCINTSTAT_CCRCEIF_MASK | _SDHCINTSTAT_CEBEIF_MASK | _SDHCINTSTAT_CIDXEIF_MASK);

    return (errorStatus >> 16);
}

uint16_t SDHC_DataErrorGet(void)
{
    uint32_t errorStatus = sdhcObj.errorStatus;

    errorStatus &= (_SDHCINTSTAT_ADEIF_MASK | _SDHCINTSTAT_DTOEIF_MASK | _SDHCINTSTAT_DCRCEIF_MASK | _SDHCINTSTAT_DEBEIF_MASK);

    return (errorStatus >> 16);
}

void SDHC_BusWidthSet ( SDHC_BUS_WIDTH busWidth )
{
    if (busWidth == SDHC_BUS_WIDTH_4_BIT)
    {
        SDHCCON1 |= _SDHCCON1_DTXWIDTH_MASK;
    }
    else
    {
        SDHCCON1 &= ~(_SDHCCON1_DTXWIDTH_MASK);
    }
}

void SDHC_SpeedModeSet ( SDHC_SPEED_MODE speedMode )
{
    if (speedMode == SDHC_SPEED_MODE_HIGH)
    {
        SDHCCON1 |= _SDHCCON1_HSEN_MASK;
    }
    else
    {
       SDHCCON1 &= ~(_SDHCCON1_HSEN_MASK);
    }
}

bool SDHC_IsCmdLineBusy ( void )
{
    return(((SDHCSTAT1 & _SDHCSTAT1_CINHCMD_MASK) == _SDHCSTAT1_CINHCMD_MASK)? true : false);
}

bool SDHC_IsDatLineBusy ( void )
{
    return (((SDHCSTAT1 & _SDHCSTAT1_CINHDAT_MASK) == _SDHCSTAT1_CINHDAT_MASK)? true : false);
}

bool SDHC_IsWriteProtected ( void )
{
    if (CFGCON2bits.SDWPEN)
    {
        /* Write-protect status indication through the WPSLVL bit (SDHCSTAT1<19>) is inverted (See Errata) */
        return (SDHCSTAT1 & _SDHCSTAT1_WPSLVL_MASK) ? true : false;
    }
    else
    {
        return false;
    }
}

bool SDHC_IsCardAttached ( void )
{
    return ((SDHCSTAT1 & _SDHCSTAT1_CARDINS_MASK) == _SDHCSTAT1_CARDINS_MASK)? true : false;
}

void SDHC_BlockSizeSet ( uint16_t blockSize )
{
    if (blockSize > SDHC_MAX_BLOCK_SIZE)
    {
        blockSize = SDHC_MAX_BLOCK_SIZE;
    }
    
    SDHCBLKCON = ((SDHCBLKCON & ~_SDHCBLKCON_BSIZE_MASK) | (blockSize));
}

void SDHC_BlockCountSet ( uint16_t numBlocks )
{
    SDHCBLKCON = ((SDHCBLKCON & ~_SDHCBLKCON_BCOUNT_MASK) | (numBlocks << 16));
}

void SDHC_ClockEnable ( void )
{
    /* Enable internal clock */
    SDHCCON2 |= _SDHCCON2_ICLKEN_MASK;
    
    /* Wait for the internal clock to stabilize */
    SDHC_Delay(1000);

    /* Enable the SDCLK */
    SDHCCON2 |= _SDHCCON2_SDCLKEN_MASK;
}

void SDHC_ClockDisable ( void )
{
    SDHCCON2 &= ~(_SDHCCON2_ICLKEN_MASK | _SDHCCON2_SDCLKEN_MASK);
}

void SDHC_DmaSetup (
    uint8_t* buffer,
    uint32_t numBytes,
    SDHC_DATA_TRANSFER_DIR direction
)
{
    uint32_t i;
    uint32_t pendingBytes = numBytes;
    uint32_t nBytes = 0;

    (void)direction;

    /* Each ADMA2 descriptor can transfer 65536 bytes (or 128 blocks) of data.
     * Block count register being a 16 bit register, maximum number of blocks is
     * limited to 65536 blocks. Hence, combined length of data that can be
     * transferred by all the descriptors is 512 bytes x 65536 blocks, assuming
     * a block size of 512 bytes.
     */

    if (pendingBytes > (65536 * SDHC_DMA_NUM_DESCR_LINES))
    {
        /* Too many blocks requested in one go */
        return;
    }

    for (i = 0; (i < SDHC_DMA_NUM_DESCR_LINES) && (pendingBytes > 0); i++)
    {
        if (pendingBytes > 65536)
        {
            nBytes = 65536;
        }
        else
        {
            nBytes = pendingBytes;
        }
        sdhcDmaDescrTable[i].address = (uint32_t)KVA_TO_PA(buffer);
        sdhcDmaDescrTable[i].length = nBytes;
        sdhcDmaDescrTable[i].attribute = \
            (SDHC_DESC_TABLE_ATTR_XFER_DATA | SDHC_DESC_TABLE_ATTR_VALID | SDHC_DESC_TABLE_ATTR_INTR);

        pendingBytes = pendingBytes - nBytes;
    }

    /* The last descriptor line must indicate the end of the descriptor list */
    sdhcDmaDescrTable[i-1].attribute |= (SDHC_DESC_TABLE_ATTR_END);

    /* Set the starting address of the descriptor table */
    SDHCAADDR = (uint32_t)KVA_TO_PA(&sdhcDmaDescrTable[0]);
}

bool SDHC_ClockSet ( uint32_t speed)
{
    uint32_t div = 0;
    uint32_t baseclk_frq = 0;

    /* Find the maximum clock frequency supported by the Host Controller. */
    baseclk_frq = (SDHCCAP & (_SDHCCAP_BASECLK_MASK)) >> _SDHCCAP_BASECLK_POSITION;
    if (baseclk_frq == 0)
    {
        return false;
    }

    /* Convert to Hertz */
    baseclk_frq *= 1000000;

    /* Disable clock before changing it */
    if (SDHCCON2 & _SDHCCON2_SDCLKEN_MASK)
    {
        while (SDHCSTAT1 & (_SDHCSTAT1_CINHCMD_MASK | _SDHCSTAT1_CINHCMD_MASK));
        SDHCCON2 &= ~(_SDHCCON2_SDCLKEN_MASK | _SDHCCON2_ICLKEN_MASK);
    }

    if (speed < baseclk_frq)
    {
        div = baseclk_frq / speed;
        div >>= 1;
    }

    /* Bits 15-8 sdclock frequency select */
    SDHCCON2 = ((SDHCCON2 & ~_SDHCCON2_SDCLKDIV_MASK) | ((div & 0xFF) << 8));

    /* Bits 7-6 Upper bits of sdclock frequency select */
    SDHCCON2 = ((SDHCCON2 & ~(0x000000C0)) | (((div & 0x3FF) >> 8) << 6));

    /* Enable internal clock */
    SDHCCON2 |= _SDHCCON2_ICLKEN_MASK;

    return true;
}

void SDHC_ResponseRead (
    SDHC_READ_RESPONSE_REG respReg,
    uint32_t* response
)
{
    switch (respReg)
    {
        case SDHC_READ_RESP_REG_0:
        default:
            *response = SDHCRESP0;
            break;

        case SDHC_READ_RESP_REG_1:
            *response = SDHCRESP1;
            break;

        case SDHC_READ_RESP_REG_2:
            *response = SDHCRESP2;
            break;

        case SDHC_READ_RESP_REG_3:
            *response = SDHCRESP3;
            break;

        case SDHC_READ_RESP_REG_ALL:
            response[0] = SDHCRESP0;
            response[1] = SDHCRESP1;
            response[2] = SDHCRESP2;
            response[3] = SDHCRESP3;
            break;
    }
}

void SDHC_CommandSend (
    uint8_t opCode,
    uint32_t argument,
    uint8_t respType,
    SDHC_DataTransferFlags transferFlags
)
{
    uint32_t mode_reg = 0;
    uint32_t cmd_reg = 0;
    uint32_t transfer_mode_reg = 0;
    uint16_t normal_int_sig_enable_reg = 0;

    /* Clear the flags */
    sdhcObj.isCmdInProgress = false;
    sdhcObj.isDataInProgress = false;
    sdhcObj.errorStatus = 0;

    /* Keep the card insertion and removal interrupts enabled */
    normal_int_sig_enable_reg = (_SDHCINTSEN_CARDIISE_MASK | _SDHCINTSEN_CARDRISE_MASK);

    switch (respType)
    {
        case SDHC_CMD_RESP_R1:
        case SDHC_CMD_RESP_R5:
        case SDHC_CMD_RESP_R6:
        case SDHC_CMD_RESP_R7:
            cmd_reg = (SDHC_MODE_RESPTYPE_48_BIT | _SDHCMODE_CCRCCEN_MASK | _SDHCMODE_CIDXCEN_MASK);
            break;

        case SDHC_CMD_RESP_R3:
        case SDHC_CMD_RESP_R4:
            cmd_reg = SDHC_MODE_RESPTYPE_48_BIT;
            break;

        case SDHC_CMD_RESP_R1B:
            cmd_reg = (SDHC_MODE_RESPTYPE_48_BIT_BUSY | _SDHCMODE_CCRCCEN_MASK | _SDHCMODE_CIDXCEN_MASK);

            /* Commands with busy response will wait for transfer complete bit */
            normal_int_sig_enable_reg |= _SDHCINTSEN_TXCISE_MASK;
            break;

        case SDHC_CMD_RESP_R2:
            cmd_reg = (SDHC_MODE_RESPTYPE_136_BIT | _SDHCMODE_CCRCCEN_MASK);
            break;

        default:
            cmd_reg = SDHC_MODE_RESPTYPE_NONE;
            break;
    }

    /* Enable command complete interrupt, for commands that do not have busy response type */
    if (respType != SDHC_CMD_RESP_R1B)
    {
        normal_int_sig_enable_reg |= _SDHCINTSEN_CCISE_MASK;
    }

    if (transferFlags.isDataPresent == true)
    {
        sdhcObj.isDataInProgress = true;
        cmd_reg |= ((uint32_t)transferFlags.isDataPresent << _SDHCMODE_DPSEL_POSITION);
        transfer_mode_reg = SDHC_TransferModeSet(opCode);
        /* Enable data transfer complete and DMA interrupt */
        normal_int_sig_enable_reg |= (_SDHCINTSEN_TXCISE_MASK | _SDHCINTSEN_DMAISE_MASK);
    }

    /* Enable all error interrupt signals and required normal interrupt signals */
    SDHCINTSEN = (SDHC_EISIER_Msk | normal_int_sig_enable_reg);

    SDHCARG = argument;

    sdhcObj.isCmdInProgress = true;

    mode_reg = ( ((uint32_t)opCode << _SDHCMODE_CIDX_POSITION) | cmd_reg | transfer_mode_reg);
    SDHCMODE = mode_reg;
}

void SDHC_ModuleInit( void )
{
    /* Disable SDWPEN# pin */
    CFGCON2bits.SDWPEN = 0x0;

    CFGCON2bits.SDRDFTHR = 0x200;
    CFGCON2bits.SDWRFTHR = 0x200;

    /* Reset module*/
    SDHCCON2 |= _SDHCCON2_SWRALL_MASK;
    while((SDHCCON2 & _SDHCCON2_SWRALL_MASK) == _SDHCCON2_SWRALL_MASK);

    /* Clear the normal and error interrupt status flags */
    SDHCINTSTAT = SDHCINTSTAT;

    /* Enable all the normal interrupt status and error status generation */
    SDHCINTEN = SDHC_INTEN_Msk;

    /* Set timeout control register */
    SDHCCON2 = ((SDHCCON2 & ~_SDHCCON2_DTOC_MASK) | (0x0E << _SDHCCON2_DTOC_POSITION));

    /* Enable ADMA2 (Check CA0R capability register first) */
    SDHCCON1 = ((SDHCCON1 & ~_SDHCCON1_DMASEL_MASK) | (0x02 << _SDHCCON1_DMASEL_POSITION));

    /* Enable the card detect line SDCD */
    CFGCON2bits.SDCDEN = 0x1;

    /* SD Bus Voltage Select = 3.3V, SD Bus Power = On */
    SDHCCON1 |= _SDHCCON1_SDBP_MASK;

    /* Set initial clock to 400 KHz*/
    SDHC_ClockSet (SDHC_CLOCK_FREQ_400_KHZ);

    /* Wait for the internal clock to stabilize */
    SDHC_Delay(1000);

    /* Enable the SDCLK */
    SDHCCON2 |= _SDHCCON2_SDCLKEN_MASK;

    /* Clear the high speed bit and set the data width to 1-bit mode */
    SDHCCON1 &= ~(_SDHCCON1_HSEN_MASK | _SDHCCON1_DTXWIDTH_MASK);

    /* Enable card inserted and card removed interrupt signals */
    SDHCINTSEN = (_SDHCINTSEN_CARDIISE_MASK | _SDHCINTSEN_CARDRISE_MASK);

    /* Enable SDHC interrupt */
    IEC5SET = 0x80000000L;
}

void SDHC_Initialize( void )
{
    SDHC_VariablesInit();
    SDHC_ModuleInit();
}

void SDHC_CallbackRegister(SDHC_CALLBACK callback, uintptr_t contextHandle)
{
    if (callback != NULL)
    {
        sdhcObj.callback = callback;
        sdhcObj.context = contextHandle;
    }
}