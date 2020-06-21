/******************************************************************************
  SD Card (SPI) Library Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdspi.c

  Summary:
    SD Card (SPI) Driver Library Interface implementation

  Description:
    The SD Card (SPI) Library provides a interface to access the SPI based SD
    Card.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "drv_sdspi_plib_interface.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************

static const DRV_SDSPI_CMD_OBJ gDrvSDSPICmdTable[] =
{
    /* Command                             CRC     response    response length*/
    {CMD_VALUE_GO_IDLE_STATE,              0x95,   RESPONSE_R1,         1 },
    {CMD_VALUE_SEND_OP_COND,               0xF9,   RESPONSE_R1,         1 },
    {CMD_VALUE_SEND_IF_COND,               0x87,   RESPONSE_R7,         5 },
    {CMD_VALUE_SEND_CSD,                   0xAF,   RESPONSE_R1,         1 },
    {CMD_VALUE_SEND_CID,                   0x1B,   RESPONSE_R1,         1 },
    {CMD_VALUE_STOP_TRANSMISSION,          0xC3,   RESPONSE_R1b,        1 },
    {CMD_VALUE_SEND_STATUS,                0xAF,   RESPONSE_R2,         2 },
    {CMD_VALUE_SET_BLOCKLEN,               0xFF,   RESPONSE_R1,         1 },
    {CMD_VALUE_READ_SINGLE_BLOCK,          0xFF,   RESPONSE_R1,         1 },
    {CMD_VALUE_READ_MULTI_BLOCK,           0xFF,   RESPONSE_R1,         1 },
    {CMD_VALUE_WRITE_SINGLE_BLOCK,         0xFF,   RESPONSE_R1,         1 },
    {CMD_VALUE_WRITE_MULTI_BLOCK,          0xFF,   RESPONSE_R1,         1 },
    {CMD_VALUE_TAG_SECTOR_START,           0xFF,   RESPONSE_R1,         1 },
    {CMD_VALUE_TAG_SECTOR_END,             0xFF,   RESPONSE_R1,         1 },
    {CMD_VALUE_ERASE,                      0xDF,   RESPONSE_R1b,        1 },
    {CMD_VALUE_APP_CMD,                    0x73,   RESPONSE_R1,         1 },
    {CMD_VALUE_READ_OCR,                   0x25,   RESPONSE_R3,         5 },
    {CMD_VALUE_CRC_ON_OFF,                 0x25,   RESPONSE_R1,         1 },
    {CMD_VALUE_SD_SEND_OP_COND,            0xFF,   RESPONSE_R1,         1 },
    {CMD_VALUE_SET_WR_BLK_ERASE_COUNT,     0xFF,   RESPONSE_R1,         1 }
};

/* This is the driver instance object array. */
static CACHE_ALIGN DRV_SDSPI_OBJ gDrvSDSPIObj[DRV_SDSPI_INSTANCES_NUMBER];


// *****************************************************************************
// *****************************************************************************
// Section: DRV_SDSPI Driver Local Functions
// *****************************************************************************
// *****************************************************************************

static inline uint32_t _DRV_SDSPI_MAKE_HANDLE(
    uint16_t token,
    uint8_t drvIndex,
    uint8_t clientIndex
)
{
    return ((token << 16) | (drvIndex << 8) | clientIndex);
}

static inline uint16_t _DRV_SDSPI_UPDATE_TOKEN( uint16_t token )
{
    token++;
    if (token >= _DRV_SDSPI_TOKEN_MAX)
    {
        token = 1;
    }
    return token;
}

static DRV_SDSPI_CLIENT_OBJ* _DRV_SDSPI_DriverHandleValidate( DRV_HANDLE handle )
{
    /* This function returns the pointer to the client object that is
     * associated with this handle if the handle is valid. Returns NULL
     * otherwise. */

    uint32_t drvInstance = 0;
    DRV_SDSPI_CLIENT_OBJ* clientObj = (DRV_SDSPI_CLIENT_OBJ*)NULL;

    if((handle != DRV_HANDLE_INVALID) && (handle != 0))
    {
        /* Extract the drvInstance value from the handle */
        drvInstance = ((handle & _DRV_SDSPI_INSTANCE_INDEX_MASK) >> 8);

        if (drvInstance >= DRV_SDSPI_INSTANCES_NUMBER)
        {
            return (NULL);
        }

        if ((handle & _DRV_SDSPI_CLIENT_INDEX_MASK) >= gDrvSDSPIObj[drvInstance].nClientsMax)
        {
            return (NULL);
        }

        /* Extract the client index and obtain the client object */
        clientObj = &((DRV_SDSPI_CLIENT_OBJ *)gDrvSDSPIObj[drvInstance].clientObjPool)\
                [handle & _DRV_SDSPI_CLIENT_INDEX_MASK];

        if ((clientObj->clientHandle != handle) || (clientObj->inUse == false))
        {
            return (NULL);
        }
    }

    return(clientObj);
}

static uint32_t _DRV_SDSPI_ProcessCSD(uint8_t* csdPtr)
{
    uint32_t discCapacity;
    uint8_t cSizeMultiplier;
    uint16_t blockLength;
    uint32_t cSize;
    uint32_t mult;

    /* Extract some fields from the response for computing the card capacity. */
    /* Note: The structure format depends on if it is a CSD V1 or V2 device.
       Therefore, need to first determine version of the specs that the card
       is designed for, before interpreting the individual fields.
     */
    /* Calculate the MDD_SDSPI_finalLBA (see SD card physical layer
       simplified spec 2.0, section 5.3.2).
       In USB mass storage applications, we will need this information
       to correctly respond to SCSI get capacity requests.  Note: method
       of computing MDD_SDSPI_finalLBA TODO depends on CSD structure spec
       version (either v1 or v2).
     */
    if (csdPtr[0] == DRV_SDSPI_DATA_START_TOKEN)
    {
        /* Note: This is a workaround. Some cards issue data start token
        before sending the 16 byte csd data and some don't. */
        csdPtr = csdPtr + 1;
    }

    if (csdPtr[0] & _DRV_SDSPI_CHECK_V2_DEVICE)
    {
        /* Check CSD_STRUCTURE field for v2+ struct device */
        /* Must be a v2 device (or a reserved higher version, that
           doesn't currently exist) */
        /* Extract the C_SIZE field from the response.  It is a 22-bit
           number in bit position 69:48.  This is different from v1.
           It spans bytes 7, 8, and 9 of the response.
         */
        cSize = (((uint32_t)csdPtr[7] & 0x3F) << 16) | ((uint16_t)csdPtr[8] << 8) | csdPtr[9];
        discCapacity = ((uint32_t)(cSize + 1) * (uint16_t)(1024u));
    }
    else /* Not a V2 device, Must be a V1 device */
    {
        /* Must be a v1 device. Extract the C_SIZE field from the
           response.  It is a 12-bit number in bit position 73:62.
           Although it is only a 12-bit number, it spans bytes 6, 7,
           and 8, since it isn't byte aligned.
         */
        cSize = csdPtr[6] & 0x3;
        cSize <<= 8;
        cSize |= csdPtr[7];
        cSize <<= 2;
        cSize |= (csdPtr[8] >> 6);
        /* Extract the C_SIZE_MULT field from the response.  It is a
           3-bit number in bit position 49:47 */
        cSizeMultiplier = (csdPtr[9] & 0x03) << 1;
        cSizeMultiplier |= ((csdPtr[10] & 0x80) >> 7);

        /* Extract the BLOCK_LEN field from the response. It is a
           4-bit number in bit position 83:80
         */
        blockLength = csdPtr[5] & 0x0F;
        blockLength = 1 << (blockLength - 9);

        /* Calculate the capacity (see SD card physical layer simplified
           spec 2.0, section 5.3.2). In USB mass storage applications,
           we will need this information to correctly respond to SCSI get
           capacity requests (which will cause MDD_SDSPI_ReadCapacity()
           to get called).
         */

        mult = 1 << (cSizeMultiplier + 2);
        discCapacity = (((uint32_t)(cSize + 1) * mult) * blockLength);
    }

    return discCapacity;
}

static bool _DRV_SDSPI_CommandSend(
    DRV_SDSPI_OBJ* const dObj,
    uint8_t command,
    uint32_t arg
)
{
    uint32_t i;
    bool isSuccess = false;
    uint32_t nBytes = DRV_SDSPI_PACKET_SIZE;
    uint32_t ncrTries = _DRV_SDSPI_COMMAND_RESPONSE_TRIES;

    /* Frame the command */
    dObj->cmdRespBuffer[0] = (gDrvSDSPICmdTable[command].commandCode | DRV_SDSPI_TRANSMIT_SET);
    /* SD Card expects argument in big-endian format */
    dObj->cmdRespBuffer[1] = ((uint8_t*)&arg)[3];
    dObj->cmdRespBuffer[2] = ((uint8_t*)&arg)[2];
    dObj->cmdRespBuffer[3] = ((uint8_t*)&arg)[1];
    dObj->cmdRespBuffer[4] = ((uint8_t*)&arg)[0];
    dObj->cmdRespBuffer[5] = gDrvSDSPICmdTable[command].crc;
    /* Dummy data. Only used in case of DRV_SDSPI_STOP_TRANSMISSION */
    dObj->cmdRespBuffer[6] = 0xFF;

    if(command == DRV_SDSPI_STOP_TRANSMISSION)
    {
        /* Transmit an extra byte before reading the response for Stop Transmission command */
        nBytes += 1;
    }
    /* Send the command bytes */
    if (_DRV_SDSPI_SPIWrite(dObj, (void*)dObj->cmdRespBuffer, nBytes) == false)
    {
        return isSuccess;
    }

     /* Wait for a response from SD Card. Try Ncr times before giving up. */
    for (i = 0; i < ncrTries; i++)
    {
        if (_DRV_SDSPI_SPIRead(dObj, (void*)dObj->cmdRespBuffer, 1) == false)
        {
            return isSuccess;
        }
        else if (dObj->cmdRespBuffer[0] != 0xFF)
        {
            break;
        }
    }

    if (dObj->cmdRespBuffer[0] == 0xFF)
    {
        return isSuccess;
    }

    /* Copy R1 response to cmdResponse buffer */
    dObj->cmdResponse[0] = dObj->cmdRespBuffer[0];

    if (gDrvSDSPICmdTable[command].responseType == RESPONSE_R1b)
    {
        /* For R1B response type, an optional busy signal is transmitted on the line.
         * Wait until the busy status (indicated by 0x00 response) is cleared.
         * Recommended timeout is 100 ms.
         */
        if (_DRV_SDSPI_CmdResponseTimerStart(dObj, _DRV_SDSPI_R1B_RESP_TIMEOUT) == false)
        {
            return isSuccess;
        }
        else
        {
            do
            {
                if (_DRV_SDSPI_SPIRead(dObj, (void*)dObj->cmdRespBuffer, 1) == false)
                {
                    return isSuccess;
                }
            } while ((dObj->cmdRespTmrExpired == false) && (dObj->cmdRespBuffer[0] != 0x00));

            _DRV_SDSPI_CmdResponseTimerStop(dObj);

            /* Return failure if the card is busy even after waiting for 100ms */
            if (dObj->cmdRespBuffer[0] == 0x00)
            {
                return isSuccess;
            }
        }
    }
    else
    {
        /* Get the total length of response bytes. Note that R1 is already read,
         * hence subtract one byte from the total response length */
        nBytes = gDrvSDSPICmdTable[command].responseLength - 1;

        /* Now, receive remaining response bytes (if any) + send the dummy byte.
        * Device requires at least 8 clock pulses after the response has been sent,
        * before it can process the next command */
        if (_DRV_SDSPI_SPIRead(dObj, (void*)&dObj->cmdRespBuffer[0], (nBytes + 1)) == false)
        {
            return isSuccess;
        }

        /* Save the response in little-endian format */
        for (i = 0 ; i < nBytes; i++)
        {
            ((uint8_t*)&dObj->cmdResponse)[nBytes-i] = dObj->cmdRespBuffer[i];
        }
    }

    isSuccess = true;

    return isSuccess;
}

static bool _DRV_SDSPI_SendInitClockPulses(DRV_SDSPI_OBJ* const dObj)
{
    uint8_t i;

    /* Fill dummy-data to generate clock pulses */
    for (i = 0; i < MEDIA_INIT_ARRAY_SIZE; i++)
    {
        dObj->cmdRespBuffer[i] = 0xFF;
    }

    /* Generate 74 clock pulses with CS = HIGH */
    return _DRV_SDSPI_SPIWriteWithChipSelectDisabled(dObj, (void*)dObj->cmdRespBuffer, MEDIA_INIT_ARRAY_SIZE);
}

static bool _DRV_SDSPI_EnterIdleState(DRV_SDSPI_OBJ* const dObj)
{
    bool isSuccess = false;
    DRV_SDSPI_RESPONSE_1* r1Response = (DRV_SDSPI_RESPONSE_1*)&dObj->cmdResponse[0];

    if (_DRV_SDSPI_CommandSend(dObj, DRV_SDSPI_GO_IDLE_STATE, 0) == true)
    {
        if (r1Response->inIdleState == CMD_R1_END_BIT_SET)
        {
            isSuccess = true;
        }
    }
    return isSuccess;
}

static bool _DRV_SDSPI_CheckIFCondition(DRV_SDSPI_OBJ* const dObj)
{
    bool isSuccess = false;
    DRV_SDSPI_RESPONSE_1* r1Response = (DRV_SDSPI_RESPONSE_1*)&dObj->cmdResponse[0];

    /*  Send CMD8 (SEND_IF_COND) to specify/request the SD card interface
        condition (ex: indicate what voltage the host runs at).
        0x000001AA --> VHS = 0001b = 2.7V to 3.6V.  The 0xAA LSB is the check
        pattern, and is arbitrary, but 0xAA is recommended (good blend of 0's
        and '1's). The SD card has to echo back the check pattern correctly
        however, in the R7 response. If the SD card doesn't support the
        operating voltage range of the host, then it may not respond. If it
        does support the range, it will respond with a type R7 response packet
        (6 bytes/48 bits). Additionally, if the SD card is MMC or SD card
        v1.x spec device, then it may respond with invalid command.  If it is
        a v2.0 spec SD card, then it is mandatory that the card respond to CMD8
    */
    if (_DRV_SDSPI_CommandSend(dObj, DRV_SDSPI_SEND_IF_COND, 0x000001AA) == true)
    {
        if (r1Response->illegalCommand == false)
        {
            /* Version 2.0 SD Memory Card (SDSC, SDHC/SDXC).
             * The CCS bit in response to CMD58 will indicate if its a SDSC (CCS = 0).
             * OR a SDHC/SDXC (CCS = 1) card
             * Check the echoed pattern and the echoed VCA (Card accepted voltage range)
            */
            if ((dObj->cmdResponse[1] == 0xAA) && ((dObj->cmdResponse[2] & 0x0F) == 0x01))
            {
                /* Card with compatible voltage range.
                 * Set sdHcHost to 1. This will be used in HCS bit of the ACMD41
                 * to tell the SD Card that the host supports SDHC cards. */
                dObj->sdHcHost = 1;
                isSuccess = true;
            }
            else
            {
                /* Card with incompatible voltage range.
                 * Return false and don't send further initialization commands. */
                dObj->sdHcHost = 0;
            }
        }
        else
        {
            /* Either version 1.X SD Memory Card (Standard Capacity) or Not a SD Memory Card.
             * Although HCS bit in ACMD41 is ignored by the version 1.x cards, sdHcHost must
             * be set to 0. */
            dObj->sdHcHost = 0;
            isSuccess = true;
        }
    }
    return isSuccess;
}

static bool _DRV_SDSPI_SendACMD41(DRV_SDSPI_OBJ* const dObj)
{
    bool isSuccess = false;
    DRV_SDSPI_RESPONSE_1* r1Response = (DRV_SDSPI_RESPONSE_1*)&dObj->cmdResponse[0];

    /* Send ACMD41.  This is to check if the SD card is finished booting
     * up/ready for full frequency and all further commands.  Response is R1 type. */

    /* Note: When sending ACMD41, the HCS bit is bit 30, and must be = 1 to
     * tell SD card that the host supports SDHC/SDXC */

    if (_DRV_SDSPI_TimerStart(dObj, _DRV_SDSPI_APP_CMD_RESP_TIMEOUT_IN_MS) == false)
    {
        /* Could not start the timer */
        return isSuccess;
    }

    do
    {
        if (_DRV_SDSPI_CommandSend(dObj, DRV_SDSPI_APP_CMD, 0) == false)
        {
            _DRV_SDSPI_TimerStop(dObj);
            return isSuccess;
        }

        if (_DRV_SDSPI_CommandSend(dObj, DRV_SDSPI_SD_SEND_OP_COND, dObj->sdHcHost << 30) == false)
        {
            _DRV_SDSPI_TimerStop(dObj);
            return isSuccess;
        }

    }while ((r1Response->inIdleState == CMD_R1_END_BIT_SET) && (dObj->timerExpired == false));

    _DRV_SDSPI_TimerStop(dObj);

    if (dObj->timerExpired == false)
    {
        isSuccess = true;
    }

    return isSuccess;
}

static bool _DRV_SDSPI_ReadOCR(DRV_SDSPI_OBJ* const dObj)
{
    bool isSuccess = false;
    uint32_t ocrRegister;

    if (_DRV_SDSPI_CommandSend(dObj, DRV_SDSPI_READ_OCR, 0x00) == true)
    {
        /* OCR[31] = Card power up status bit (1 = ready, 0 = busy)
           OCR[30] = Card Capacity Status CCS (1 = SDHC/SDXC, 0 = SDSC).
           This bit is valid only if card power up status bit is 1 (ready).
           OCR[15:23] = VDD Voltage Window supported by the card.
        */

        ocrRegister = (((uint32_t)dObj->cmdResponse[4] << 24)|((uint32_t)dObj->cmdResponse[3] << 16) |
                ((uint32_t)dObj->cmdResponse[2] << 8) | ((uint32_t)dObj->cmdResponse[1]));

        if (ocrRegister & (0x01U << 31))
        {
            if (ocrRegister & (0x01U << 30))
            {
                dObj->sdCardType = DRV_SDSPI_MODE_HC;
            }
            else
            {
                dObj->sdCardType = DRV_SDSPI_MODE_NORMAL;
            }
            /* Card initialization is complete, device is ready */
            isSuccess = true;
        }
    }
    return isSuccess;
}

static bool _DRV_SDSPI_ReadCSD(DRV_SDSPI_OBJ* const dObj)
{
    bool isSuccess = false;

    if (_DRV_SDSPI_CommandSend(dObj, DRV_SDSPI_SEND_CSD, 0x00) == true)
    {
        /* Data token(1) + CSD(16) + CRC(2) + Dummy(1) = 20 Bytes */
        if (_DRV_SDSPI_SPIRead(dObj, (void*)dObj->cmdRespBuffer, _DRV_SDSPI_CSD_READ_SIZE) == true)
        {
            memcpy(dObj->csdData, (const void*)dObj->cmdRespBuffer, _DRV_SDSPI_CSD_READ_SIZE);
            /* Process the received CSD data from the SD Card */
            dObj->discCapacity = _DRV_SDSPI_ProcessCSD(dObj->csdData);
            isSuccess = true;
        }
    }
    return isSuccess;
}

static bool _DRV_SDSPI_ReadCID(DRV_SDSPI_OBJ* const dObj)
{
    bool isSuccess = false;

    if (_DRV_SDSPI_CommandSend(dObj, DRV_SDSPI_SEND_CID, 0x00) == true)
    {
        /* Data token(1) + CID(16) + CRC(2) + Dummy(1) = 20 Bytes */
        if (_DRV_SDSPI_SPIRead(dObj, (void*)dObj->cmdRespBuffer, _DRV_SDSPI_CID_READ_SIZE) == true)
        {
            memcpy(dObj->cidData, (const void*)dObj->cmdRespBuffer, _DRV_SDSPI_CID_READ_SIZE);
            isSuccess = true;
        }
    }
    return isSuccess;
}

static bool _DRV_SDSPI_TurnOffCRC(DRV_SDSPI_OBJ* const dObj)
{
    /* Turn off CRC7 if we can, might be an invalid cmd on some cards (CMD59). */
    /* Note: POR default for the media is normally with CRC checking off in SPI
     * mode anyway, so this is typically redundant */

    return _DRV_SDSPI_CommandSend(dObj, DRV_SDSPI_CRC_ON_OFF, 0x00);
}

static bool _DRV_SDSPI_SetBlockLen(DRV_SDSPI_OBJ* const dObj)
{
    /* Set the block length to media sector size. It should be already set to this. */
    return _DRV_SDSPI_CommandSend(dObj, DRV_SDSPI_SET_BLOCKLEN, _DRV_SDSPI_MEDIA_BLOCK_SIZE);
}

static void _DRV_SDSPI_CheckWriteProtectStatus
(
    DRV_SDSPI_OBJ *dObj
)
{
    dObj->isWriteProtected = false;

    /* Check if the Write Protect check is enabled */
    if (_DRV_SDSPI_EnableWriteProtectCheck())
    {
        /* Read from the pin */
        dObj->isWriteProtected = SYS_PORT_PinRead (dObj->writeProtectPin);
    }
}

static bool _DRV_SDSPI_ReadResponseWithTimeout(
    DRV_SDSPI_OBJ* const dObj,
    uint8_t expectedResponse,
    uint32_t timeout
)
{
    uint8_t i;
    bool isSuccess = false;

    if (_DRV_SDSPI_TimerStart(dObj, timeout) == false)
    {
        /* Could not start the timer */
        return isSuccess;
    }

    /* Wait for the SD card to send the data start token: 0xFE */
    do
    {
        if (_DRV_SDSPI_SPIRead(dObj, (void*)dObj->cmdRespBuffer, 1) == false)
        {
            _DRV_SDSPI_TimerStop(dObj);
            return isSuccess;
        }
    }while ((dObj->cmdRespBuffer[0] != expectedResponse) && (dObj->timerExpired == false));

    _DRV_SDSPI_TimerStop(dObj);

    /* It could happen that timer timed out while this thread was swapped out.
       Make sure we read the status once after the timeout */
    if ((dObj->timerExpired == true) && (dObj->cmdRespBuffer[0] != expectedResponse))
    {
        for (i = 0; i < 2; i++)
        {
            if (_DRV_SDSPI_SPIRead(dObj, (void*)dObj->cmdRespBuffer, 1) == true)
            {
                if (dObj->cmdRespBuffer[0] == expectedResponse)
                {
                    isSuccess = true;
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        isSuccess = true;
    }

    return isSuccess;
}

static bool _DRV_SDSPI_PollBusyStatus( DRV_SDSPI_OBJ* const dObj )
{
    bool isSuccess = false;

    /* Poll the status of the SD card's internal write cycle.
     * SD card indicates busy by sending 0x00 */
    if (_DRV_SDSPI_ReadResponseWithTimeout(dObj, 0xFF, _DRV_SDSPI_WRITE_TIMEOUT_IN_MS) == true)
    {
        isSuccess = true;
    }

    return isSuccess;
}

static bool _DRV_SDSPI_ReadBlock(
    DRV_SDSPI_OBJ* const dObj,
    void* targetBuffer
)
{
    bool isSuccess = false;

    /* Wait for the SD card to send the data start token: 0xFE */
    if (_DRV_SDSPI_ReadResponseWithTimeout(dObj, DRV_SDSPI_DATA_START_TOKEN, _DRV_SDSPI_READ_TIMEOUT_IN_MS) == true)
    {
         /* Token received, now read one block of data */
        if (_DRV_SDSPI_SPIBlockRead(dObj, targetBuffer) == true)
        {
            /* Data received, now read and discard the dummy CRC bytes */
            if (_DRV_SDSPI_SPIRead(dObj, (void*)dObj->cmdRespBuffer, 2) == true)
            {
                isSuccess = true;
            }
        }
    }

    return isSuccess;
}

static bool _DRV_SDSPI_WriteBlock(
    DRV_SDSPI_OBJ* const dObj,
    void* sourceBuffer,
    DRV_SDSPI_COMMANDS command
)
{
    bool isSuccess = false;

    if (command == DRV_SDSPI_WRITE_SINGLE_BLOCK)
    {
        dObj->cmdRespBuffer[0] = DRV_SDSPI_DATA_START_TOKEN;
    }
    else
    {
        dObj->cmdRespBuffer[0] = DRV_SDSPI_DATA_START_MULTI_BLOCK_TOKEN;
    }

    /* Send the Data Start token */
    if (_DRV_SDSPI_SPIWrite(dObj, (void*)dObj->cmdRespBuffer, 1) == false)
    {
        return isSuccess;
    }

    /* Write one block of data */
    if (_DRV_SDSPI_SPIBlockWrite(dObj, sourceBuffer) == false)
    {
        return isSuccess;
    }

    /* Write two dummy bytes of CRC */
    dObj->cmdRespBuffer[0] = 0xFF;
    dObj->cmdRespBuffer[1] = 0xFF;

    if (_DRV_SDSPI_SPIWrite(dObj, (void*)dObj->cmdRespBuffer, 2) == false)
    {
        return isSuccess;
    }

    /* Read the data response token and then poll busy status of the SD Card */
    if (_DRV_SDSPI_SPIRead(dObj, (void*)dObj->cmdRespBuffer, 1) == true)
    {
        if ((dObj->cmdRespBuffer[0] & DRV_SDSPI_WRITE_RESPONSE_TOKEN_MASK) ==
                DRV_SDSPI_DATA_ACCEPTED)
        {
            /* Card accepted the data, now poll the BUSY status of the SD card's
             * internal write cycle */
            if (_DRV_SDSPI_PollBusyStatus(dObj) == true)
            {
                isSuccess = true;
            }
        }
    }

    return isSuccess;
}

static bool _DRV_SDSPI_Write(
    DRV_SDSPI_OBJ* const dObj,
    void* sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
)
{
    uint32_t i;
    DRV_SDSPI_RESPONSE_1* r1Response = NULL;
    bool isSuccess = false;

    /*
     * <-Send Write Command (0x58)
     * ->Read and verify Response (0x00)
     * ------------------------------------------
     * <-Send Data Start Token (0xFE)
     * <-Send 512 bytes of Data
     * <-Send 2 (dummy) bytes of CRC (0xFF, 0xFF)
     * ->Read Response (0xE5)
     * ->Poll Busy Status (wait till non-zero response)
     *-------------------------------------------
     * <-Send Stop Transfer Token
     * <-Send dummy byte
     * ->Read Response
     * ->Poll Busy Status
     * <-Send dummy byte
     */

    DRV_SDSPI_COMMANDS writeCommand = DRV_SDSPI_WRITE_SINGLE_BLOCK;

    if (nBlock > 1)
    {
        writeCommand = DRV_SDSPI_WRITE_MULTI_BLOCK;
    }

    /* SDSC Card (CCS = 0) uses byte unit address and SDHC and SDXC Cards (CCS = 1)
     * use block unit address (512 bytes unit)
     */
    if (dObj->sdCardType == DRV_SDSPI_MODE_NORMAL)
    {
        blockStart <<= 9;
    }

    /* Send the single/multi-block write command */
    if (_DRV_SDSPI_CommandSend(dObj, writeCommand, blockStart) == true)
    {
        /* Verify the response */
        r1Response = (DRV_SDSPI_RESPONSE_1*)&dObj->cmdResponse[0];
        if (r1Response->byte != 0x00)
        {
            return isSuccess;
        }
        /* Write the requested blocks of data */
        for (i = 0; i < nBlock; i++)
        {
            /* Write a single block of data */
            if (_DRV_SDSPI_WriteBlock(dObj, sourceBuffer, writeCommand) == true)
            {
                sourceBuffer = (void *)((uint8_t *)sourceBuffer + _DRV_SDSPI_MEDIA_BLOCK_SIZE);
            }
            else
            {
                return isSuccess;
            }
        }
        /* For a multi-block write, send stop transmission token and a dummy byte
         * and then poll the SD card busy status */
        if (writeCommand == DRV_SDSPI_WRITE_MULTI_BLOCK)
        {
            dObj->cmdRespBuffer[0] = DRV_SDSPI_DATA_STOP_TRAN_TOKEN;
            dObj->cmdRespBuffer[1] = 0xFF;

            if (_DRV_SDSPI_SPIWrite(dObj, (void*)dObj->cmdRespBuffer, 2) == true)
            {
                /* Poll the SD Card busy status */
                if (_DRV_SDSPI_PollBusyStatus(dObj) == true)
                {
                    isSuccess = true;
                }
            }
        }
        else
        {
            /* Send a dummy data byte */
            dObj->cmdRespBuffer[0] = 0xFF;
            if (true == _DRV_SDSPI_SPIWrite(dObj, (void*)dObj->cmdRespBuffer, 1))
            {
                isSuccess = true;
            }
        }
    }
    return isSuccess;
}

static bool _DRV_SDSPI_Read(
    DRV_SDSPI_OBJ* const dObj,
    void* targetBuffer,
    uint32_t blockStart,
    uint32_t nBlock
)
{
    uint32_t i;
    DRV_SDSPI_RESPONSE_1* r1Response;
    DRV_SDSPI_COMMANDS readCommand = DRV_SDSPI_READ_SINGLE_BLOCK;
    bool isSuccess = false;

    /*
     * <-Send Read Command
     * ->Read and verify Response
     * ------------------------------------------
     * ->Read Data Start Token
     * ->Read 512 bytes of Data
     * ->Read (and discard) 2 bytes of CRC
     * ------------------------------------------
     * <-Send Stop Transfer Command
     * <-Send Dummy Byte
     * <-Read Response
     * <-Send Dummy Byte
     */

    if (nBlock > 1)
    {
        readCommand = DRV_SDSPI_READ_MULTI_BLOCK;
    }

    /* SDSC Card (CCS = 0) uses byte unit address and SDHC and SDXC Cards (CCS = 1)
     * use block unit address (512 bytes unit)
     */
    if (dObj->sdCardType == DRV_SDSPI_MODE_NORMAL)
    {
        blockStart <<= 9;
    }

    /* Send the single/multi-block read command */
    if (_DRV_SDSPI_CommandSend(dObj, readCommand, blockStart) == true)
    {
        /* Verify the response */
        r1Response = (DRV_SDSPI_RESPONSE_1*)&dObj->cmdResponse[0];
        if (r1Response->byte != 0x00)
        {
            /* Perhaps the card isn't initialized or present */
            return isSuccess;
        }
        /* Read the requested blocks of data */
        for (i = 0; i < nBlock; i++)
        {
            /* Read a single block of data */
            if (_DRV_SDSPI_ReadBlock(dObj, targetBuffer) == true)
            {
                targetBuffer = (void *)((uint8_t *)targetBuffer + _DRV_SDSPI_MEDIA_BLOCK_SIZE);
            }
            else
            {
                return isSuccess;
            }
        }
        /* For a multi-block read, send the Stop Command */
        if (readCommand == DRV_SDSPI_READ_MULTI_BLOCK)
        {
            if (false == _DRV_SDSPI_CommandSend(dObj, DRV_SDSPI_STOP_TRANSMISSION, 0x00))
            {
                return isSuccess;
            }
        }
        /* Send a dummy data byte */
        dObj->cmdRespBuffer[0] = 0xFF;
        if (true == _DRV_SDSPI_SPIWrite(dObj, (void*)dObj->cmdRespBuffer, 1))
        {
            isSuccess = true;
        }
    }
    return isSuccess;
}

static bool _DRV_SDSPI_SetupXfer (
    const DRV_HANDLE handle,
    DRV_SDSPI_COMMAND_HANDLE* commandHandle,
    DRV_SDSPI_OPERATION_TYPE opType,
    void* buffer,
    uint32_t blockStart,
    uint32_t nBlock
)
{
    bool isSuccess = false;
    DRV_SDSPI_CLIENT_OBJ* clientObj = _DRV_SDSPI_DriverHandleValidate(handle);
    DRV_SDSPI_OBJ* dObj = NULL;
    DRV_SDSPI_EVENT evtStatus = DRV_SDSPI_EVENT_COMMAND_ERROR;

    if (commandHandle)
    {
        *commandHandle = DRV_SDSPI_COMMAND_HANDLE_INVALID;
    }

    if (clientObj != NULL)
    {
        dObj = &gDrvSDSPIObj[clientObj->drvIndex];
    }
    else
    {
        return isSuccess;
    }

    if (dObj->mediaState == DRV_SDSPI_IS_DETACHED)
    {
        return isSuccess;
    }

    if (buffer == NULL || nBlock == 0)
        return isSuccess;

    if (opType == DRV_SDSPI_OPERATION_TYPE_READ)
    {
        if (((blockStart + nBlock) > dObj->mediaGeometryTable[SYS_MEDIA_GEOMETRY_TABLE_READ_ENTRY].numBlocks))
            return isSuccess;
    }
    else
    {
        if (((blockStart + nBlock) > dObj->mediaGeometryTable[SYS_MEDIA_GEOMETRY_TABLE_WRITE_ENTRY].numBlocks))
            return isSuccess;

        /* Return error if the card is write protected */
        if (dObj->isWriteProtected)
            return isSuccess;
    }

    /* Block other clients/threads from accessing the SD Card */
    if (OSAL_MUTEX_Lock(&dObj->transferMutex, OSAL_WAIT_FOREVER ) != OSAL_RESULT_TRUE)
    {
        return isSuccess;
    }

    if (commandHandle)
    {
        dObj->cmdStatus = DRV_SDSPI_COMMAND_IN_PROGRESS;
        /* Command accepted, assign a unique command handle */
        dObj->commandHandle = _DRV_SDSPI_MAKE_HANDLE(dObj->commandToken, clientObj->drvIndex, 0);
        *commandHandle = dObj->commandHandle;
        /* Update the token number. */
        dObj->commandToken = _DRV_SDSPI_UPDATE_TOKEN(dObj->commandToken);
    }

    if (opType == DRV_SDSPI_OPERATION_TYPE_READ)
    {
        if (_DRV_SDSPI_Read(dObj, buffer, blockStart, nBlock) == true)
        {
            isSuccess = true;
            evtStatus = DRV_SDSPI_EVENT_COMMAND_COMPLETE;
        }
    }
    else
    {
        if (_DRV_SDSPI_Write(dObj, buffer, blockStart, nBlock) == true)
        {
            isSuccess = true;
            evtStatus = DRV_SDSPI_EVENT_COMMAND_COMPLETE;
        }
    }

    if (commandHandle)
    {
        dObj->cmdStatus = DRV_SDSPI_COMMAND_COMPLETED;
    }

    if(clientObj->eventHandler != NULL)
    {
        /* Call the event handler (needed for compatibility with the file system) */
        clientObj->eventHandler(
            (SYS_MEDIA_BLOCK_EVENT)evtStatus,
            (DRV_SDSPI_COMMAND_HANDLE)commandHandle,
            clientObj->context
        );
    }

    OSAL_MUTEX_Unlock(&dObj->transferMutex);

    return isSuccess;
}

void DRV_SDSPI_Read (
    const DRV_HANDLE handle,
    DRV_SDSPI_COMMAND_HANDLE* commandHandle,
    void* targetBuffer,
    uint32_t blockStart,
    uint32_t nBlock
)
{
    (void) _DRV_SDSPI_SetupXfer(
                handle,
                commandHandle,
                DRV_SDSPI_OPERATION_TYPE_READ,
                targetBuffer,
                blockStart,
                nBlock
            );
}

void DRV_SDSPI_Write(
    const DRV_HANDLE handle,
    DRV_SDSPI_COMMAND_HANDLE* commandHandle,
    void* sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
)
{
    (void) _DRV_SDSPI_SetupXfer(
                handle,
                commandHandle,
                DRV_SDSPI_OPERATION_TYPE_WRITE,
                sourceBuffer,
                blockStart,
                nBlock
            );
}

static void _DRV_SDSPI_UpdateGeometry( DRV_SDSPI_OBJ *dObj )
{
    uint8_t i = 0;

    /* Update the Media Geometry Table */
    for (i = 0; i <= SYS_MEDIA_GEOMETRY_TABLE_ERASE_ENTRY; i++)
    {
        dObj->mediaGeometryTable[i].blockSize = 512;
        dObj->mediaGeometryTable[i].numBlocks = dObj->discCapacity;
    }

    /* Update the Media Geometry Main Structure */
    dObj->mediaGeometryObj.mediaProperty = (SYS_MEDIA_PROPERTY)(SYS_MEDIA_READ_IS_BLOCKING | SYS_MEDIA_WRITE_IS_BLOCKING),

    /* Number of read, write and erase entries in the table */
    dObj->mediaGeometryObj.numReadRegions = 1,
    dObj->mediaGeometryObj.numWriteRegions = 1,
    dObj->mediaGeometryObj.numEraseRegions = 1,
    dObj->mediaGeometryObj.geometryTable = (SYS_MEDIA_REGION_GEOMETRY *)&dObj->mediaGeometryTable;
}

static void _DRV_SDSPI_MediaInitialize( SYS_MODULE_OBJ object )
{
    DRV_SDSPI_OBJ *dObj;
    dObj = (DRV_SDSPI_OBJ*)_DRV_SDSPI_INSTANCE_GET(object);

    switch(dObj->mediaInitState)
    {
        case DRV_SDSPI_INIT_SPI:

            _DRV_SDSPI_SPISpeedSetup(dObj, _DRV_SDSPI_SPI_INITIAL_SPEED);
            dObj->sdCardType = DRV_SDSPI_MODE_NORMAL;
            dObj->mediaInitState = DRV_SDSPI_INIT_RAMP_TIME;
            /* Fall through */
        case DRV_SDSPI_INIT_RAMP_TIME:
            /* Send 74 clock pulses */
            if (_DRV_SDSPI_SendInitClockPulses(dObj) == false)
            {
                /* Stay in the same state */
                break;
            }
            else
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_RESET_SDCARD;
            }
            /* Fall through */
        case DRV_SDSPI_INIT_RESET_SDCARD:
            /* Send CMD_0 */
            if (_DRV_SDSPI_EnterIdleState(dObj) == false)
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_RAMP_TIME;
                break;
            }
            else
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_CHK_IFACE_CONDITION;
            }
            /* Fall through */

        case DRV_SDSPI_INIT_CHK_IFACE_CONDITION:
            /* Send CMD_8 */
            if (_DRV_SDSPI_CheckIFCondition(dObj) == false)
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_ERROR;
                break;
            }
            else
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_SEND_ACMD41;
            }
            /* Fall through */

        case DRV_SDSPI_INIT_SEND_ACMD41:
            /* Send CMD_55 (0x77) + ACMD_41 (0x69) */
            if (_DRV_SDSPI_SendACMD41(dObj) == false)
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_ERROR;
                break;
            }
            else
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_READ_OCR;
            }
            /* Fall through */

        case DRV_SDSPI_INIT_READ_OCR:
            /* Send CMD 58 (0x3A) */
            if (_DRV_SDSPI_ReadOCR(dObj) == false)
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_ERROR;
                break;
            }
            else
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_INCR_CLOCK_SPEED;
            }
            /* Fall through */

        case DRV_SDSPI_INIT_INCR_CLOCK_SPEED:
            /* Initialization complete. We can now operate at higher SPI speeds. */
            _DRV_SDSPI_SPISpeedSetup(dObj, dObj->sdcardSpeedHz);

            dObj->mediaInitState = DRV_SDSPI_INIT_READ_CSD;
            /* Fall through */

        case DRV_SDSPI_INIT_READ_CSD:
            /* Send CMD 9 (0x49) */
            if (_DRV_SDSPI_ReadCSD(dObj) == false)
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_ERROR;
                break;
            }
            else
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_READ_CID;
            }
            /* Fall through */

        case DRV_SDSPI_INIT_READ_CID:
            /* Send CMD 10 (0x4A) */
            if (_DRV_SDSPI_ReadCID(dObj) == false)
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_ERROR;
                break;
            }
            else
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_TURN_OFF_CRC;
            }
            /* Fall through */

        case DRV_SDSPI_INIT_TURN_OFF_CRC:
            /* Send CMD 59 (0x7B) */
            if (_DRV_SDSPI_TurnOffCRC(dObj) == false)
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_ERROR;
                break;
            }
            else
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_SET_BLOCKLEN;
            }
            /* Fall through */

        case DRV_SDSPI_INIT_SET_BLOCKLEN:
            /* Send CMD 16 (0x50)*/
            if (_DRV_SDSPI_SetBlockLen(dObj) == false)
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_ERROR;
            }
            else
            {
                dObj->mediaInitState = DRV_SDSPI_INIT_SD_INIT_DONE;
            }
            break;

        case DRV_SDSPI_INIT_SD_INIT_DONE:
            dObj->mediaInitState = DRV_SDSPI_INIT_SPI;
            break;

        case DRV_SDSPI_INIT_ERROR:
        default:
            dObj->mediaInitState = DRV_SDSPI_INIT_SPI;
            break;
    }
}

static DRV_SDSPI_ATTACH _DRV_SDSPI_MediaCommandDetect ( SYS_MODULE_OBJ object )
{
    DRV_SDSPI_OBJ* dObj;
    DRV_SDSPI_ATTACH isCardAttached = DRV_SDSPI_IS_DETACHED;

    dObj = (DRV_SDSPI_OBJ*)_DRV_SDSPI_INSTANCE_GET(object);

    switch ( dObj->cmdDetectState )
    {
        case DRV_SDSPI_CMD_DETECT_START_INIT:
            /* Reset to the initial SPI speed*/
            _DRV_SDSPI_SPISpeedSetup(dObj, _DRV_SDSPI_SPI_INITIAL_SPEED);
            dObj->cmdDetectState = DRV_SDSPI_CMD_DETECT_CHECK_FOR_CARD;
            /* Fall through */
        case DRV_SDSPI_CMD_DETECT_CHECK_FOR_CARD:

            if (_DRV_SDSPI_SendInitClockPulses(dObj) == true)
            {
                /* Send CMD_0 */
                if (_DRV_SDSPI_EnterIdleState(dObj) == true)
                {
                    isCardAttached = DRV_SDSPI_IS_ATTACHED;
                }
            }
            break;

        case DRV_SDSPI_CMD_DETECT_CHECK_FOR_DETACH:
            /* Make sure no read/write transfer is currently in progress */
            if (_DRV_SDSPI_CommandSend(dObj, DRV_SDSPI_SEND_CID, 0x00) == true)
            {
                /* Data token(1) + CID(16) + CRC(2) + Dummy(1) = 20 Bytes */
                if (_DRV_SDSPI_SPIRead(dObj, (void*)dObj->cmdRespBuffer, _DRV_SDSPI_CID_READ_SIZE) == true)
                {
                    if (memcmp(dObj->cidData, (const void*)dObj->cmdRespBuffer, _DRV_SDSPI_CID_READ_SIZE - 1) == 0)
                    {
                        isCardAttached = DRV_SDSPI_IS_ATTACHED;
                    }
                }
            }
        default:
            break;
    }
    return isCardAttached;
}

static void _DRV_SDSPI_AttachDetachTasks ( SYS_MODULE_OBJ object )
{
    DRV_SDSPI_OBJ* dObj;
    dObj = (DRV_SDSPI_OBJ*)_DRV_SDSPI_INSTANCE_GET(object);

    /* Block other clients/threads from accessing the SD Card */
    if (OSAL_MUTEX_Lock(&dObj->transferMutex, OSAL_WAIT_FOREVER ) != OSAL_RESULT_TRUE)
    {
        return;
    }

    switch ( dObj->taskState )
    {
        case DRV_SDSPI_TASK_START_POLLING_TIMER:
            if (_DRV_SDSPI_CardDetectPollingTimerStart(dObj, dObj->pollingIntervalMs) == true)
            {
                dObj->taskState = DRV_SDSPI_TASK_CHECK_DEVICE;
            }
            break;

        case DRV_SDSPI_TASK_CHECK_DEVICE:
            /* Check for device attach */
            if (dObj->cardPollingTimerExpired == true)
            {
                dObj->cardPollingTimerExpired = false;
                dObj->taskState = DRV_SDSPI_TASK_START_POLLING_TIMER;
                dObj->isAttached = _DRV_SDSPI_MediaCommandDetect (object);
                if (dObj->isAttachedLastStatus != dObj->isAttached)
                {
                    dObj->isAttachedLastStatus = dObj->isAttached;
                    /* We should call a function on device attach and detach */
                    if (DRV_SDSPI_IS_ATTACHED == dObj->isAttached)
                    {
                        /* An SD card seems to be present. Initiate a full card initialization. */
                        dObj->taskState = DRV_SDSPI_TASK_MEDIA_INIT;
                    }
                    else
                    {
                        dObj->mediaState = DRV_SDSPI_IS_DETACHED;
                        /* SD Card seems to have been removed, check for attach */
                        dObj->cmdDetectState = DRV_SDSPI_CMD_DETECT_START_INIT;
                    }
                }
            }
            break;

        case DRV_SDSPI_TASK_MEDIA_INIT:
            /* Update the card details to the internal data structure */
            _DRV_SDSPI_MediaInitialize (object);

            /* Once the initialization is complete, move to the next stage */
            if (dObj->mediaInitState == DRV_SDSPI_INIT_SD_INIT_DONE)
            {
                /* Check and update the card's write protected status */
                _DRV_SDSPI_CheckWriteProtectStatus (dObj);

                /* Update the Media Geometry structure */
                _DRV_SDSPI_UpdateGeometry (dObj);

                /* State that the device is attached. */
                dObj->mediaState = DRV_SDSPI_IS_ATTACHED;
                dObj->cmdDetectState = DRV_SDSPI_CMD_DETECT_CHECK_FOR_DETACH;
                dObj->taskState = DRV_SDSPI_TASK_START_POLLING_TIMER;
            }
            else if (dObj->mediaInitState == DRV_SDSPI_INIT_ERROR)
            {
                /* The SD card is probably removed. Go back and check for card insertion. */
                dObj->isAttachedLastStatus = dObj->isAttached = DRV_SDSPI_IS_DETACHED;
                dObj->cmdDetectState = DRV_SDSPI_CMD_DETECT_START_INIT;
                dObj->taskState = DRV_SDSPI_TASK_START_POLLING_TIMER;
            }
            break;

        case DRV_SDSPI_TASK_IDLE:
        default:
            break;
    }

    /* Release the Mutex to allow other clients/threads to access the SD Card */
    OSAL_MUTEX_Unlock(&dObj->transferMutex);
}

void DRV_SDSPI_Tasks ( SYS_MODULE_OBJ object )
{
    _DRV_SDSPI_AttachDetachTasks (object);
}

// *****************************************************************************
// *****************************************************************************
// Section: Driver Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

__WEAK void DRV_SDSPI_RegisterWithSysFs
(
    const SYS_MODULE_INDEX drvIndex
)
{
    /* Weak function to avoid compiler warning when registration with FS is
     * not enabled. */
}

SYS_MODULE_OBJ DRV_SDSPI_Initialize(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT * const init
)
{
    const DRV_SDSPI_INIT* sdSPIInit = (const DRV_SDSPI_INIT *)init;
    DRV_SDSPI_OBJ* dObj = NULL;

    /* Validate the request */
    if(drvIndex >= DRV_SDSPI_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    dObj = &gDrvSDSPIObj[drvIndex];

    if(dObj->inUse == true)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if (OSAL_MUTEX_Create(&dObj->transferMutex) == OSAL_RESULT_FALSE)
    {
        /* If the mutex was not created because the memory required to
        hold the mutex could not be allocated then NULL is returned. */
        return SYS_MODULE_OBJ_INVALID;
    }

    if (OSAL_MUTEX_Create(&dObj->clientMutex) == OSAL_RESULT_FALSE)
    {
        /* If the mutex was not created because the memory required to
        hold the mutex could not be allocated then NULL is returned. */
        return SYS_MODULE_OBJ_INVALID;
    }

    if (OSAL_SEM_Create(&dObj->transferDone,OSAL_SEM_TYPE_BINARY, 0, 0) == OSAL_RESULT_FALSE)
    {
        /* There was insufficient heap memory available for the semaphore to
        be created successfully. */
        return SYS_MODULE_OBJ_INVALID;
    }

    dObj->status                = SYS_STATUS_UNINITIALIZED;
    dObj->inUse                 = true;
    dObj->nClients              = 0;
    dObj->nClientsMax           = sdSPIInit->numClients;
    dObj->clientObjPool         = sdSPIInit->clientObjPool;

    dObj->spiPlib               = sdSPIInit->spiPlib;
    dObj->remapClockPhase       = sdSPIInit->remapClockPhase;
    dObj->remapClockPolarity    = sdSPIInit->remapClockPolarity;
    dObj->remapDataBits         = sdSPIInit->remapDataBits;
    dObj->rxDMAChannel          = sdSPIInit->rxDMAChannel;
    dObj->txDMAChannel          = sdSPIInit->txDMAChannel;
    dObj->txAddress             = sdSPIInit->txAddress;
    dObj->rxAddress             = sdSPIInit->rxAddress;

    dObj->isFsEnabled           = sdSPIInit->isFsEnabled;
    dObj->writeProtectPin       = sdSPIInit->writeProtectPin;
    dObj->chipSelectPin         = sdSPIInit->chipSelectPin;
    dObj->sdcardSpeedHz         = sdSPIInit->sdcardSpeedHz;
    dObj->pollingIntervalMs     = sdSPIInit->pollingIntervalMs;
    dObj->blockStartAddress     = sdSPIInit->blockStartAddress;
    dObj->clientToken           = 1;
    dObj->commandToken          = 1;

    /* Reset the SDSPI attach/detach variables */
    dObj->isAttached            = DRV_SDSPI_IS_DETACHED;
    dObj->isAttachedLastStatus  = DRV_SDSPI_IS_DETACHED;
    dObj->mediaState            = DRV_SDSPI_IS_DETACHED;

    dObj->taskState             = DRV_SDSPI_TASK_START_POLLING_TIMER;
    dObj->cmdDetectState        = DRV_SDSPI_CMD_DETECT_START_INIT;
    dObj->mediaInitState        = DRV_SDSPI_INIT_SPI;
    dObj->spiTransferStatus     = DRV_SDSPI_SPI_TRANSFER_STATUS_COMPLETE;

    /* De-assert Chip Select pin to begin with */
    SYS_PORT_PinSet(dObj->chipSelectPin);


    /* Register call-backs with the DMA System Service */
    if (dObj->txDMAChannel != SYS_DMA_CHANNEL_NONE && dObj->rxDMAChannel != SYS_DMA_CHANNEL_NONE)
    {
        SYS_DMA_DataWidthSetup(dObj->rxDMAChannel, SYS_DMA_WIDTH_8_BIT);

        SYS_DMA_DataWidthSetup(dObj->txDMAChannel, SYS_DMA_WIDTH_8_BIT);

        SYS_DMA_AddressingModeSetup(
            dObj->rxDMAChannel,
            SYS_DMA_SOURCE_ADDRESSING_MODE_FIXED,
            SYS_DMA_DESTINATION_ADDRESSING_MODE_INCREMENTED
        );

        SYS_DMA_AddressingModeSetup(
            dObj->txDMAChannel,
            SYS_DMA_SOURCE_ADDRESSING_MODE_INCREMENTED,
            SYS_DMA_DESTINATION_ADDRESSING_MODE_FIXED
        );

        SYS_DMA_ChannelCallbackRegister(dObj->txDMAChannel, _DRV_SDSPI_TX_DMA_CallbackHandler, (uintptr_t)dObj);
        SYS_DMA_ChannelCallbackRegister(dObj->rxDMAChannel, _DRV_SDSPI_RX_DMA_CallbackHandler, (uintptr_t)dObj);
    }
    else
    {
        /* Register call-back with the SPI PLIB */
        dObj->spiPlib->callbackRegister(_DRV_SDSPI_SPIPlibCallbackHandler, (uintptr_t)dObj);
    }

    /* Register with file system*/
    if (dObj->isFsEnabled == true)
    {
        DRV_SDSPI_RegisterWithSysFs(drvIndex);
    }

    /* Update the status */
    dObj->status = SYS_STATUS_READY;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)drvIndex );
}

DRV_HANDLE DRV_SDSPI_Open(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT ioIntent
)
{
    DRV_SDSPI_CLIENT_OBJ* clientObj = NULL;
    DRV_SDSPI_OBJ* dObj = NULL;
    uint8_t iClient;

    /* Validate the request */
    if (drvIndex >= DRV_SDSPI_INSTANCES_NUMBER)
    {
        return DRV_HANDLE_INVALID;
    }

    /* Allocate the driver object and set the operation flag to be in use */
    dObj = (DRV_SDSPI_OBJ*)&gDrvSDSPIObj[drvIndex];

    if(dObj->status != SYS_STATUS_READY)
    {
        return DRV_HANDLE_INVALID;
    }

    /* Acquire the instance specific mutex to protect the instance specific
     * client pool */
    if (OSAL_MUTEX_Lock(&dObj->clientMutex , OSAL_WAIT_FOREVER ) == OSAL_RESULT_FALSE)
    {
        return DRV_HANDLE_INVALID;
    }

    if((dObj->inUse == false) ||
       (dObj->isExclusive == true) ||
       (dObj->nClients >= dObj->nClientsMax) ||
       ((dObj->nClients > 0) && (ioIntent & DRV_IO_INTENT_EXCLUSIVE)))
    {
        OSAL_MUTEX_Unlock( &dObj->clientMutex);
        return DRV_HANDLE_INVALID;
    }

    for(iClient = 0; iClient != dObj->nClientsMax; iClient++)
    {
        if(((DRV_SDSPI_CLIENT_OBJ *)dObj->clientObjPool)[iClient].inUse == false)
        {
            /* This means we have a free client object to use */

            clientObj = &((DRV_SDSPI_CLIENT_OBJ *)dObj->clientObjPool)[iClient];
            clientObj->inUse        = true;
            clientObj->context      = 0;
            clientObj->intent       = ioIntent;
            clientObj->drvIndex     = drvIndex;

            if(ioIntent & DRV_IO_INTENT_EXCLUSIVE)
            {
                /* Set the driver exclusive flag */
                dObj->isExclusive = true;
            }

            dObj->nClients++;

            /* Generate and save the client handle in the client object, which will
             * be then used to verify the validity of the client handle.
             */
            clientObj->clientHandle = (DRV_HANDLE)_DRV_SDSPI_MAKE_HANDLE(dObj->clientToken,
                    (uint8_t)drvIndex, iClient);

            /* Increment the instance specific token counter */
            dObj->clientToken = _DRV_SDSPI_UPDATE_TOKEN(dObj->clientToken);
            break;
        }
    }

    OSAL_MUTEX_Unlock(&dObj->clientMutex);

    /* Driver index is the handle */
    return clientObj ? ((DRV_HANDLE)clientObj->clientHandle) : DRV_HANDLE_INVALID;
}

SYS_STATUS DRV_SDSPI_Status( SYS_MODULE_OBJ object )
{
    /* Validate the request */
    if( (object == SYS_MODULE_OBJ_INVALID) || (object >= DRV_SDSPI_INSTANCES_NUMBER) )
    {
        return SYS_STATUS_UNINITIALIZED;
    }

    /* Return the driver status */
    return (gDrvSDSPIObj[object].status);
}

void DRV_SDSPI_Close( const DRV_HANDLE handle )
{
    DRV_SDSPI_CLIENT_OBJ* clientObj;
    DRV_SDSPI_OBJ* dObj;

    /* Validate the handle */
    clientObj = _DRV_SDSPI_DriverHandleValidate(handle);

    if(clientObj != NULL)
    {
        dObj = (DRV_SDSPI_OBJ* )&gDrvSDSPIObj[clientObj->drvIndex];

        /* Acquire the client mutex to protect the client pool */
        if (OSAL_MUTEX_Lock(&dObj->clientMutex , OSAL_WAIT_FOREVER ) == OSAL_RESULT_TRUE)
        {
            /* Reduce the number of clients */
            dObj->nClients--;

            /* Reset the exclusive flag */
            dObj->isExclusive = false;

            /* De-allocate the object */
            clientObj->inUse = false;

            /* Release the client mutex */
            OSAL_MUTEX_Unlock( &dObj->clientMutex );
        }
    }
}

bool DRV_SDSPI_SyncRead (
    const DRV_HANDLE handle,
    void* targetBuffer,
    uint32_t blockStart,
    uint32_t nBlock
)
{
    return _DRV_SDSPI_SetupXfer(
                handle,
                NULL,
                DRV_SDSPI_OPERATION_TYPE_READ,
                targetBuffer,
                blockStart,
                nBlock
            );
}

bool DRV_SDSPI_SyncWrite(
    const DRV_HANDLE handle,
    void* sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
)
{
    return _DRV_SDSPI_SetupXfer(
                handle,
                NULL,
                DRV_SDSPI_OPERATION_TYPE_WRITE,
                sourceBuffer,
                blockStart,
                nBlock
            );
}

bool DRV_SDSPI_IsAttached( const DRV_HANDLE handle )
{
    DRV_SDSPI_CLIENT_OBJ* clientObj;
    DRV_SDSPI_OBJ* dObj;

    /* Validate the driver handle */
    clientObj = _DRV_SDSPI_DriverHandleValidate(handle);
    if (clientObj == NULL)
    {
        return false;
    }
    else
    {
        dObj = (DRV_SDSPI_OBJ*)&gDrvSDSPIObj[clientObj->drvIndex];
        return dObj->mediaState;
    }
}

bool DRV_SDSPI_IsWriteProtected( const DRV_HANDLE handle )
{
    DRV_SDSPI_CLIENT_OBJ* clientObj;
    DRV_SDSPI_OBJ* dObj;

    clientObj = _DRV_SDSPI_DriverHandleValidate (handle);
    if (clientObj == NULL)
        return false;

    dObj = (DRV_SDSPI_OBJ*)&gDrvSDSPIObj[clientObj->drvIndex];

    if (dObj->mediaState == DRV_SDSPI_IS_DETACHED)
    {
        return false;
    }

    return dObj->isWriteProtected;
}

SYS_MEDIA_GEOMETRY* DRV_SDSPI_GeometryGet ( const DRV_HANDLE handle )
{
    DRV_SDSPI_CLIENT_OBJ* clientObj;
    DRV_SDSPI_OBJ* dObj;

    clientObj = _DRV_SDSPI_DriverHandleValidate (handle);
    if (clientObj != NULL)
    {
        dObj = (DRV_SDSPI_OBJ*)&gDrvSDSPIObj[clientObj->drvIndex];
        return (&dObj->mediaGeometryObj);
    }

    return NULL;
}

void DRV_SDSPI_EventHandlerSet
(
    const DRV_HANDLE handle,
    const void* eventHandler,
    const uintptr_t context
)
{
    DRV_SDSPI_CLIENT_OBJ* clientObj;

    clientObj = _DRV_SDSPI_DriverHandleValidate (handle);
    if (clientObj != NULL)
    {
        /* Set the event handler */
        clientObj->eventHandler = (DRV_SDSPI_EVENT_HANDLER)eventHandler;
        clientObj->context = context;
    }
}

DRV_SDSPI_COMMAND_STATUS DRV_SDSPI_CommandStatusGet(
    const DRV_HANDLE handle,
    const DRV_SDSPI_COMMAND_HANDLE commandHandle
)
{
    DRV_SDSPI_CLIENT_OBJ* clientObj;
    DRV_SDSPI_OBJ* dObj;
    DRV_SDSPI_COMMAND_STATUS status = DRV_SDSPI_COMMAND_COMPLETED;

    clientObj = _DRV_SDSPI_DriverHandleValidate (handle);

    if (clientObj != NULL)
    {
        dObj = (DRV_SDSPI_OBJ*)&gDrvSDSPIObj[clientObj->drvIndex];

        /* Compare the buffer handle with buffer handle in the object */
        if(dObj->commandHandle == commandHandle)
        {
            status = dObj->cmdStatus;
        }
    }
    return status;
}
