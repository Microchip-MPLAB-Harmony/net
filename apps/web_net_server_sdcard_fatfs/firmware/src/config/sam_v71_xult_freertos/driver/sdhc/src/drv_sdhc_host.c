/*******************************************************************************
  SD Host Controller Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdhc_host.c

  Summary:
    SD Host Controller Device Driver Dynamic Implementation

  Description:
    The SD Host Controller device driver provides a simple interface to manage the
    SD Host Controller modules on Microchip microcontrollers.  This file Implements
    the core interface routines for the SD Host Controller driver.

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


// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "driver/sdhc/src/drv_sdhc_host.h"

/*NOTE:-Buffers need to be 32-byte address and size aligned
        for cache management operations.
*/

uint8_t __attribute__((aligned(32))) gSDHOSTScrReg[32] = {0};
uint8_t gSDHOSTCidReg[SDHOST_CID_REG_LEN] = {0};
uint8_t gSDHOSTCsdReg[SDHOST_CSD_REG_LEN] = {0};
uint8_t gSDHOSTOcrReg[SDHOST_OCR_REG_LEN] = {0};
uint8_t __attribute__((aligned(32))) gSDHOSTSwitchStatusReg[SDHOST_SWITCH_STATUS_REG_LEN] = {0};
uint16_t hsmci_block_size = 0;

SDHOST_CARD_CTXT gSDHOSTCardCtxt;

#define CHANNEL            0

static void sdhostInitVariables ( SDHOST_CARD_CTXT* cardCtxt )
{
    cardCtxt->isAttached = false;
    cardCtxt->busWidth = 0;
    cardCtxt->scr = &gSDHOSTScrReg[0];
    cardCtxt->cid = &gSDHOSTCidReg[0];
    cardCtxt->csd = &gSDHOSTCsdReg[0];
    cardCtxt->ocr = &gSDHOSTOcrReg[0];
    cardCtxt->switchStatus = &gSDHOSTSwitchStatusReg[0];

    cardCtxt->cmd6Mode = false;
    cardCtxt->voltWindow = 0;
    /* HC or Normal card. */
    cardCtxt->cardType = 0;
    /* Capacity of the card in number of blocks. */
    cardCtxt->discCapacity = 0;

    /* Variables to track the command/data status. */
    cardCtxt->dataCompleted = false;
    cardCtxt->commandCompleted = false;
    cardCtxt->waitForCmdResp = false;
    cardCtxt->waitForData = false;
    cardCtxt->isDataPresent = false;
    cardCtxt->commandError = 0;
    cardCtxt->errorFlag = 0;
}

static uint32_t sdhostSetTransferMode ( uint32_t opcode )
{
    uint32_t cmd = 0;

    if (opcode & SDMMC_CMD_WRITE)
    {
        cmd = HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_WRITE;
    }
    else
    {
        cmd = HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_READ;
    }

    if (opcode & SDMMC_CMD_STREAM)
    {
        cmd |= HSMCI_CMDR_TRTYP_STREAM;
    }
    else
    {
        if (opcode & SDMMC_CMD_SINGLE_BLOCK)
        {
            cmd |= HSMCI_CMDR_TRTYP_SINGLE;
        }
        else
        {
            if (opcode & SDMMC_CMD_MULTI_BLOCK)
            {
                cmd |= HSMCI_CMDR_TRTYP_MULTIPLE;
            }
            else
            {
                cmd |= 0;
            }
        }
    }

    return cmd;
}

void sdhostSetThreshold ( void )
{
}

void sdhostCardDetectEnable ( void )
{
}

void sdhostCardDetectDisable ( void )
{
}

void sdhostWriteProtectEnable ( void )
{
}

void sdhostWriteProtectDisable ( void )
{
}

void sdhostResetError ( SDHOST_RESET_TYPE resetType )
{
    HSMCI_REGS->HSMCI_CR |= HSMCI_CR_SWRST_Msk;
}

void sdhostSetBusWidth ( SDHOST_BUS_WIDTH busWidth )
{
    if (busWidth == SDHOST_BUS_WIDTH_4_BIT)
    {
       HSMCI_REGS->HSMCI_SDCR |= HSMCI_SDCR_SDCBUS_4;
    }
    else
    {
       HSMCI_REGS->HSMCI_SDCR |= HSMCI_SDCR_SDCBUS_1;
    }
}

void sdhostSetSpeedMode ( SDHOST_SPEED_MODE speedMode )
{
    if (speedMode == SDHOST_SPEED_MODE_HIGH)
    {
       HSMCI_REGS->HSMCI_CFG |= HSMCI_CFG_HSMODE_Msk;
    }
    else
    {
        HSMCI_REGS->HSMCI_CFG &= ~HSMCI_CFG_HSMODE_Msk;
    }
}

void sdhostSetupDma ( uint8_t *buffer, uint16_t numBytes, DRV_SDHC_OPERATION_TYPE operation )
{
    HSMCI_REGS->HSMCI_DMA = HSMCI_DMA_DMAEN_Msk;
    if (operation == DRV_SDHC_OPERATION_TYPE_READ)
    {
        XDMAC_REGS->XDMAC_CHID[CHANNEL].XDMAC_CSA = (uint32_t)&(HSMCI_REGS->HSMCI_FIFO[0]);
        XDMAC_REGS->XDMAC_CHID[CHANNEL].XDMAC_CDA = (uint32_t) buffer;
        XDMAC_REGS->XDMAC_CHID[CHANNEL].XDMAC_CUBC = XDMAC_CUBC_UBLEN((numBytes)/4);
        XDMAC_REGS->XDMAC_CHID[CHANNEL].XDMAC_CC = XDMAC_CC_TYPE_PER_TRAN
                            | XDMAC_CC_MBSIZE_SINGLE
                            | XDMAC_CC_DSYNC_PER2MEM
                            | XDMAC_CC_CSIZE_CHK_1
                            | XDMAC_CC_DWIDTH_WORD
                            | XDMAC_CC_SIF_AHB_IF1
                            | XDMAC_CC_DIF_AHB_IF0
                            | XDMAC_CC_SAM_FIXED_AM
                            | XDMAC_CC_DAM_INCREMENTED_AM
                            | XDMAC_CC_PERID(CHANNEL);
        if (DATA_CACHE_ENABLED == true)
        {
            DCACHE_INVALIDATE_BY_ADDR((uint32_t*)buffer, (int32_t)numBytes);
        }
        XDMAC_REGS->XDMAC_GE = (XDMAC_GE_EN0_Msk << CHANNEL);
    }
    else
    {
        XDMAC_REGS->XDMAC_CHID[CHANNEL].XDMAC_CSA = (uint32_t) buffer;
        XDMAC_REGS->XDMAC_CHID[CHANNEL].XDMAC_CDA = (uint32_t)&(HSMCI_REGS->HSMCI_FIFO[0]);
        XDMAC_REGS->XDMAC_CHID[CHANNEL].XDMAC_CUBC = XDMAC_CUBC_UBLEN((numBytes)/4);
        XDMAC_REGS->XDMAC_CHID[CHANNEL].XDMAC_CC = XDMAC_CC_TYPE_PER_TRAN
                        | XDMAC_CC_MBSIZE_SINGLE
                        | XDMAC_CC_DSYNC_MEM2PER
                        | XDMAC_CC_CSIZE_CHK_1
                        | XDMAC_CC_DWIDTH_WORD
                        | XDMAC_CC_SIF_AHB_IF0
                        | XDMAC_CC_DIF_AHB_IF1
                        | XDMAC_CC_SAM_INCREMENTED_AM
                        | XDMAC_CC_DAM_FIXED_AM
                        | XDMAC_CC_PERID(CHANNEL);
        if (DATA_CACHE_ENABLED == true)
        {
            DCACHE_CLEAN_BY_ADDR((uint32_t*)buffer, (int32_t)numBytes);
        }

        XDMAC_REGS->XDMAC_GE = (XDMAC_GE_EN0_Msk << CHANNEL);
   }
}

void sdhostSetBlockSize ( uint16_t blockSize )
{
    hsmci_block_size = blockSize;
    HSMCI_REGS->HSMCI_BLKR &= ~(HSMCI_BLKR_BLKLEN_Msk);
    HSMCI_REGS->HSMCI_BLKR |= (blockSize << HSMCI_BLKR_BLKLEN_Pos);
}

void sdhostSetBlockCount ( uint16_t numBlocks )
{
    HSMCI_REGS->HSMCI_BLKR &= ~(HSMCI_BLKR_BCNT_Msk);
    HSMCI_REGS->HSMCI_BLKR |= (numBlocks << HSMCI_BLKR_BCNT_Pos);
}

bool sdhostIsCardAttached ( void )
{
    return true;
}

void sdhostSetClock ( uint32_t clock )
{
    uint32_t mck = 150000000;
    uint32_t clkdiv = 0;
    uint32_t rest = 0;

    // Speed = MCK clock / (2 * (CLKDIV + 1))
    if ((clock * 2) < mck)
    {
        clkdiv = mck / (2 * clock);
        rest = mck - (2*clock)*clkdiv;

        if (rest > 0)
        {
             // Ensure that the card speed not be higher than expected.
            clkdiv++;
        }

        if (clkdiv > 0)
        {
            clkdiv -= 1;
        }
    }
    else
    {
        clkdiv = 0;
    }

    HSMCI_REGS->HSMCI_MR &= ~HSMCI_MR_CLKDIV_Msk;
    HSMCI_REGS->HSMCI_MR |= HSMCI_MR_CLKDIV(clkdiv);
}

void sdhostClockEnable ( void )
{
}

void sdhostClockDisable ( void )
{
}

void sdhostInterruptHandler( SDHOST_CARD_CTXT* cardCtxt )
{
    volatile uint32_t intMask = 0;
    volatile uint32_t intFlags = 0;

    intMask = HSMCI_REGS->HSMCI_IMR;
    intFlags = HSMCI_REGS->HSMCI_SR;
    if ((intMask & intFlags) == 0)
    {
        return;
    }

    if ((intFlags & (HSMCI_SR_CMDRDY_Msk | SDHOST_ERROR_INTERRUPT)))
    {
        if (intFlags & SDHOST_ERROR_INTERRUPT)
        {
            if (intFlags & (HSMCI_SR_CSTOE_Msk | HSMCI_SR_RTOE_Msk))
            {
                cardCtxt->errorFlag = 0x01;
            }
            else
            {
                if (intFlags & (HSMCI_SR_RCRCE_Msk))
                {
                    cardCtxt->errorFlag = 0x02;
                }
                else
                {
                    if (intFlags & (HSMCI_SR_RINDE_Msk))
                    {
                        cardCtxt->errorFlag = 0x08;
                    }
                    else
                    {
                        if (intFlags & (HSMCI_SR_RENDE_Msk | HSMCI_SR_RDIRE_Msk))
                        {
                            cardCtxt->errorFlag = 0x04;
                        }
                    }
                }
            }
        }
        else
        {
            cardCtxt->commandCompleted = true;
        }

        if (cardCtxt->isDataPresent)
        {
            cardCtxt->waitForData = true;
        }
    }

    if (cardCtxt->waitForData == true)
    {
        if (intFlags & (HSMCI_SR_XFRDONE_Msk | SDHOST_DATA_ERROR))
        {
            cardCtxt->errorFlag = 0x00;
            if (intFlags & SDHOST_DATA_ERROR)
            {
                cardCtxt->errorFlag = 0x10;
            }
            else
            {
                if (intFlags & (HSMCI_SR_XFRDONE_Msk))
                {
                    cardCtxt->dataCompleted = true;
                    HSMCI_REGS->HSMCI_IDR = 0xffffffff;
                }
            }
        }
    }

    if (cardCtxt->waitForData == false)
    {
        HSMCI_REGS->HSMCI_IDR = 0xffffffff;
    }
}

bool sdhostIsCmdLineBusy ( void )
{
    return (!(HSMCI_REGS->HSMCI_SR & HSMCI_SR_CMDRDY_Msk));
}

bool sdhostIsDat0LineBusy ( void )
{
    return (!(HSMCI_REGS->HSMCI_SR & HSMCI_SR_TXRDY_Msk));
}

void sdhostReadResponse ( SDHOST_READ_RESPONSE_REG respReg, uint32_t* response )
{
    uint32_t response_reg[4] = {0,0,0,0};
    memcpy(response_reg,(void*)&(HSMCI_REGS->HSMCI_RSPR[0]),16);
    switch (respReg)
    {
        case SDHOST_READ_RESP_REG_0:
            *response = response_reg[0];
        break;

        case SDHOST_READ_RESP_REG_1:
            *response = response_reg[1];
        break;

        case SDHOST_READ_RESP_REG_2:
            *response = response_reg[2];
        break;

        case SDHOST_READ_RESP_REG_3:
            *response = response_reg[3];
        break;

        case SDHOST_READ_RESP_REG_ALL:
        {
            uint32_t *ptr = response;
            *ptr++ = response_reg[3];
            *ptr++ = response_reg[2];
            *ptr++ = response_reg[1];
            *ptr++ = response_reg[0];
        }
        break;

        default:
        break;
    }
}

void sdhostSendCommand ( uint8_t opCode, uint8_t respType, uint8_t dataPresent, uint32_t argument )
{
    uint32_t cmd = 0;
    uint32_t opcode_32 = 0;
    uint32_t interrupt = 0;
    SDHOST_CARD_CTXT *cardCtxt = &gSDHOSTCardCtxt;

    switch (opCode)
    {
        case DRV_SDHC_CMD_GO_IDLE_STATE:
            opcode_32 = (DRV_SDHC_CMD_GO_IDLE_STATE | SDMMC_CMD_NO_RESP);
        break;
        case DRV_SDHC_CMD_ALL_SEND_CID :
            opcode_32 = (DRV_SDHC_CMD_ALL_SEND_CID | SDMMC_CMD_R2);
        break;
        case DRV_SDHC_CMD_SEND_RCA:
            opcode_32 = (DRV_SDHC_CMD_SEND_RCA | SDMMC_CMD_R6);
        break;
        case DRV_SDHC_CMD_SWITCH_FUNCTION :
            if (dataPresent)
            {
                opcode_32 = (DRV_SDHC_CMD_SWITCH_FUNCTION | SDMMC_CMD_R1 | SDMMC_CMD_SINGLE_BLOCK);
            }
            else
            {
                opcode_32 = (DRV_SDHC_CMD_SWITCH_FUNCTION | SDMMC_CMD_R1);
            }
        break;
        case DRV_SDHC_CMD_SELECT_DESELECT_CARD :
            switch (respType)
            {
                case SDHOST_CMD_RESP_R1:
                opcode_32 = (DRV_SDHC_CMD_SELECT_DESELECT_CARD | SDMMC_CMD_R1);
                break;

                case SDHOST_CMD_RESP_R1B:
                opcode_32 = (DRV_SDHC_CMD_SELECT_DESELECT_CARD | SDMMC_CMD_R1B);
                break;

                default:
                opcode_32 = (DRV_SDHC_CMD_SELECT_DESELECT_CARD | SDMMC_CMD_NO_RESP);
                break;
            }
      break;
      case DRV_SDHC_CMD_SEND_IF_COND :
          opcode_32 = (DRV_SDHC_CMD_SEND_IF_COND | SDMMC_CMD_R7);
      break;
      case DRV_SDHC_CMD_SEND_CSD:
          opcode_32 = (DRV_SDHC_CMD_SEND_CSD | SDMMC_CMD_R2);
      break;
      case DRV_SDHC_CMD_STOP_TRANSMISSION :
          opcode_32 = (DRV_SDHC_CMD_STOP_TRANSMISSION | SDMMC_CMD_R1B);
      break;
      case DRV_SDHC_CMD_SEND_STATUS :
          opcode_32 = (DRV_SDHC_CMD_SEND_STATUS | SDMMC_CMD_R1);
      break;
      case DRV_SDHC_CMD_SET_BLOCKLEN:
          opcode_32 = (DRV_SDHC_CMD_SET_BLOCKLEN | SDMMC_CMD_R1);
      break;
      case DRV_SDHC_CMD_READ_SINGLE_BLOCK:
          opcode_32 = (DRV_SDHC_CMD_READ_SINGLE_BLOCK | SDMMC_CMD_R1 | SDMMC_CMD_SINGLE_BLOCK);
      break;
      case DRV_SDHC_CMD_READ_MULTI_BLOCK:
          opcode_32 = (DRV_SDHC_CMD_READ_MULTI_BLOCK | SDMMC_CMD_R1 | SDMMC_CMD_MULTI_BLOCK);
      break;
      case DRV_SDHC_CMD_WRITE_SINGLE_BLOCK:
          opcode_32 = (DRV_SDHC_CMD_WRITE_SINGLE_BLOCK | SDMMC_CMD_R1 | SDMMC_CMD_WRITE | SDMMC_CMD_SINGLE_BLOCK);
      break;
      case DRV_SDHC_CMD_WRITE_MULTI_BLOCK:
          opcode_32 = (DRV_SDHC_CMD_WRITE_MULTI_BLOCK | SDMMC_CMD_R1 | SDMMC_CMD_WRITE | SDMMC_CMD_MULTI_BLOCK);
      break;
      case DRV_SDHC_CMD_SD_SEND_OP_COND :
          opcode_32 = (DRV_SDHC_CMD_SD_SEND_OP_COND | SDMMC_CMD_R3 | SDMMC_CMD_OPENDRAIN);
      break;
      case DRV_SDHC_CMD_READ_SCR :
          opcode_32 = (DRV_SDHC_CMD_READ_SCR | SDMMC_CMD_R1 | SDMMC_CMD_SINGLE_BLOCK);
      break;
      case DRV_SDHC_CMD_APP_CMD :
          opcode_32 = (DRV_SDHC_CMD_APP_CMD | SDMMC_CMD_R1);
      break;
      default:
          opcode_32 = 0;
      break;
    }
    if (!dataPresent)
    {
        HSMCI_REGS->HSMCI_DMA = 0;
    }

    HSMCI_REGS->HSMCI_MR &= ~(HSMCI_MR_WRPROOF_Msk | HSMCI_MR_RDPROOF_Msk | HSMCI_MR_FBYTE_Msk);

    cmd |= HSMCI_CMDR_CMDNB(opcode_32) | HSMCI_CMDR_SPCMD_STD;
    if (opcode_32 & SDMMC_RESP_PRESENT)
    {
        cmd |= HSMCI_CMDR_MAXLAT_Msk;
        if (opcode_32 & SDMMC_RESP_136)
        {
            cmd |= HSMCI_CMDR_RSPTYP_136_BIT;
        }
        else
        {
            if (opcode_32 & SDMMC_RESP_BUSY)
            {
                cmd |= HSMCI_CMDR_RSPTYP_R1B;
            }
            else
            {
                cmd |= HSMCI_CMDR_RSPTYP_48_BIT;
            }
        }
    }
    if (opcode_32 & SDMMC_CMD_OPENDRAIN)
    {
        cmd |= HSMCI_CMDR_OPDCMD_OPENDRAIN;
    }

    if (opcode_32 & SDMMC_RESP_CRC)
    {
        interrupt = (HSMCI_SR_CSTOE_Msk | HSMCI_SR_RTOE_Msk | HSMCI_SR_RENDE_Msk | HSMCI_SR_RCRCE_Msk | HSMCI_SR_RDIRE_Msk | HSMCI_SR_RINDE_Msk | HSMCI_SR_CMDRDY_Msk);
    }
    else
    {
        interrupt = (HSMCI_SR_CSTOE_Msk | HSMCI_SR_RTOE_Msk | HSMCI_SR_RENDE_Msk | HSMCI_SR_CMDRDY_Msk | HSMCI_SR_RDIRE_Msk | HSMCI_SR_RINDE_Msk);
    }
    if (dataPresent)
    {
        HSMCI_REGS->HSMCI_MR |= HSMCI_MR_WRPROOF_Msk | HSMCI_MR_RDPROOF_Msk;
        if (hsmci_block_size & 0x3)
        {
            HSMCI_REGS->HSMCI_MR |= HSMCI_MR_FBYTE_Msk;
        }
        else
        {
            HSMCI_REGS->HSMCI_MR &= ~HSMCI_MR_FBYTE_Msk;
        }
        /* Configure the transfer mode register. */
        cardCtxt->isDataPresent = true;
        cmd |= sdhostSetTransferMode(opcode_32);
        interrupt |= (HSMCI_SR_XFRDONE_Msk | HSMCI_SR_UNRE_Msk | HSMCI_SR_OVRE_Msk | HSMCI_SR_DTOE_Msk | HSMCI_SR_DCRCE_Msk);
    }
    // Write argument
    HSMCI_REGS->HSMCI_ARGR = argument;
    // Write and start command
    HSMCI_REGS->HSMCI_CMDR = cmd;
    HSMCI_REGS->HSMCI_IER = interrupt;
}

bool sdhostIsWriteProtected ( void )
{
    return false;
}

bool sdhostInit ( SDHOST_CARD_CTXT** cardCtxt )
{
    // Set the Data Timeout Register to 2 Mega Cycles
    HSMCI_REGS->HSMCI_DTOR = HSMCI_DTOR_DTOMUL_1048576 | HSMCI_DTOR_DTOCYC(2);
    // Set Completion Signal Timeout to 2 Mega Cycles
    HSMCI_REGS->HSMCI_CSTOR = HSMCI_CSTOR_CSTOMUL_1048576 | HSMCI_CSTOR_CSTOCYC(2);
    // Set Configuration Register
    HSMCI_REGS->HSMCI_CFG = HSMCI_CFG_FIFOMODE_Msk | HSMCI_CFG_FERRCTRL_Msk;
    // Set power saving to maximum value
    HSMCI_REGS->HSMCI_MR = HSMCI_MR_PWSDIV_Msk;
    // Enable the HSMCI and the Power Saving
    HSMCI_REGS->HSMCI_CR = HSMCI_CR_MCIEN_Msk | HSMCI_CR_PWSEN_Msk;

    *cardCtxt = (SDHOST_CARD_CTXT *)&gSDHOSTCardCtxt;
    sdhostInitVariables (*cardCtxt);
    HSMCI_REGS->HSMCI_CFG &= ~HSMCI_CFG_HSMODE_Msk;
    sdhostSetClock (SDHOST_CLOCK_FREQ_400_KHZ);
    // Configure command
    HSMCI_REGS->HSMCI_MR &= ~(HSMCI_MR_WRPROOF_Msk | HSMCI_MR_RDPROOF_Msk | HSMCI_MR_FBYTE_Msk);
    // Write argument
    HSMCI_REGS->HSMCI_ARGR = 0;
    // Write and start initialization command
    HSMCI_REGS->HSMCI_CMDR = HSMCI_CMDR_RSPTYP_NORESP
            | HSMCI_CMDR_SPCMD_INIT
            | HSMCI_CMDR_OPDCMD_OPENDRAIN;
    // Wait end of initialization command
    while (!(HSMCI_REGS->HSMCI_SR & HSMCI_SR_CMDRDY_Msk));

    return true;
}
void sdhostParseCsd ( SDHOST_CARD_CTXT *cardCtxt )
{
    uint8_t  cSizeMultiplier = 0;
    uint16_t blockLength = 0;
    uint32_t cSize = 0;
    uint8_t *csdPtr = NULL;
    uint32_t mult = 0;
    /* Note: The structure format depends on if it is a CSD V1 or V2 device.
       Therefore, need to first determine version of the specs that the card
       is designed for, before interpreting the individual fields.
       */
    csdPtr = cardCtxt->csd;
    /* Bits 127:126 */
    if (((csdPtr[15] >> 6) & 0x03) == 0x01)
    {
        /* CSD Version 2.0 */
        cSize = (csdPtr[8] & 0x3F) << 16;
        cSize |= csdPtr[7] << 8;
        cSize |= csdPtr[6];
        cardCtxt->discCapacity = ((uint32_t)(cSize + 1) * (uint32_t)(1024));
    }
    else
    {
        /* CSD Version 1.0 */

        /* Memory capacity = BLOCKNR * BLOCK_LEN
         * BLOCKNR = (C_SIZE + 1) * MULT
         * MULT = 2 POW(C_SIZE_MULT + 2)
         * BLOCK_LEN = 2 POW(READ_BL_LEN)
         */

        /* READ_BL_LEN Bits 83:80 */
        blockLength = csdPtr[10] & 0x0F;
        blockLength = 1 << (blockLength - 9);

        /* CSIZE Bits 73:62 */
        cSize = (csdPtr[9] & 0x03) << 10;
        cSize |= csdPtr[8] << 2;
        cSize |= csdPtr[7] >> 6;

        /* C_SIZE_MULT Bits 49:47 */
        cSizeMultiplier = (csdPtr[6] & 0x03) << 1;
        cSizeMultiplier |= csdPtr[5] >> 7;

        mult = 1 << (cSizeMultiplier + 2);
        cardCtxt->discCapacity = (((uint32_t)(cSize + 1) * mult) * blockLength);
    }
}
