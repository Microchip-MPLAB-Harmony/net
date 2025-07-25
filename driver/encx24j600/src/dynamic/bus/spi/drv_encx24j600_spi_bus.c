/*******************************************************************************
  ENCx24J600 Driver SPI Bus Interface
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_spi_bus.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2014-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/

// DOM-IGNORE-END

#include "drv_encx24j600_spi_bus.h"
#include "../../drv_encx24j600_local.h"

const DRV_ENCX24J600_BusVTable drv_encx24j600_spi_vtable =
{
    .fpOpenIf = &DRV_ENCX24J600_SPI_OpenInterface,
    .fpCloseIf = &DRV_ENCX24J600_SPI_CloseInterface,
    .fpOpResult = &DRV_ENCX24J600_SPI_OperationResult,
    .fpSfrWr = &DRV_ENCX24J600_SPI_SfrWrite,
    .fpSfrRdStart = &DRV_ENCX24J600_SPI_SfrReadStart,
    .fpSfrRdResult = &DRV_ENCX24J600_SPI_SfrReadResult,
    .fpSfrBitSet = &DRV_ENCX24J600_SPI_SfrBitSet,
    .fpSfrBitClr = &DRV_ENCX24J600_SPI_SfrBitClear,
    .fpSysRst = &DRV_ENCX24J600_SPI_SystemReset,
    .fpRxEnable = &DRV_ENCX24J600_SPI_EnableRX,
    .fpRxDisable = &DRV_ENCX24J600_SPI_DisableRX,
    .fpReqPktTx = &DRV_ENCX24J600_SPI_ReqPktTx,
    .fpDecPktCnt = &DRV_ENCX24J600_SPI_DecrPktCtr,
    .fpIntEnable = &DRV_ENCX24J600_SPI_EnableInterrupts,
    .fpIntDisable = &DRV_ENCX24J600_SPI_DisableInterrupts,
    .fpFlowCtrlDisable = &DRV_ENCX24J600_SPI_FlowCtrlDisable,
    .fpFlowCtrlSingle = &DRV_ENCX24J600_SPI_FlowCtrlSingle,
    .fpFlowCtrlMult = &DRV_ENCX24J600_SPI_FlowCtrlMult,
    .fpFlowCtrlClr = &DRV_ENCX24J600_SPI_FlowCtrClear,
    .fpPhyWr = &DRV_ENCX24J600_SPI_PhyWrite,
    .fpPtrWr = &DRV_ENCX24J600_SPI_WritePointer,
    .fpPtrRdStart = &DRV_ENCX24J600_SPI_ReadPointerStart,
    .fpPtrRdResult = &DRV_ENCX24J600_SPI_ReadPointerResult,
    .fpDataWr = &DRV_ENCX24J600_SPI_WriteData,
    .fpDataRd = &DRV_ENCX24J600_SPI_ReadData,

};

static DRV_ENCX24J600_SPI_SFR_MAP F_DRV_ENCX24J600_MapSfrToSpi(DRV_ENCX24J600_SFR_MAP input, uint8_t bank)
{
    uint8_t sfr;
    
    switch (input)
    {
        case DRV_ENCX24J600_SFR_ETXS:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ETXST;
            break;
        case DRV_ENCX24J600_SFR_ETXLEN:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ETXLEN;
            break;
        case DRV_ENCX24J600_SFR_ERXST:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ERXST;
            break;
        case DRV_ENCX24J600_SFR_ERXTAIL:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ERXTAIL;
            break;
        case DRV_ENCX24J600_SFR_ERXHEAD:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ERXHEAD;
            break;
        case DRV_ENCX24J600_SFR_EDMAST:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EDMAST;
            break;
        case DRV_ENCX24J600_SFR_EDMALEN:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EDMALEN;
            break;
        case DRV_ENCX24J600_SFR_EDMADST:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EDMADST;
            break;
        case DRV_ENCX24J600_SFR_EDMACS:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EDMACS;
            break;
        case DRV_ENCX24J600_SFR_ETXSTAT:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ETXSTAT;
            break;
        case DRV_ENCX24J600_SFR_ETXWIRE:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ETXWIRE;
            break;
        case DRV_ENCX24J600_SFR_EUDAST:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EUDAST + (0x20U * bank);
            break;
        case DRV_ENCX24J600_SFR_EUDAND:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EUDAND + (0x20U * bank);
            break;
        case DRV_ENCX24J600_SFR_ESTAT:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ESTAT + (0x20U * bank);
            break;
        case DRV_ENCX24J600_SFR_EIR:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EIR + (0x20U * bank);
            break;
        case DRV_ENCX24J600_SFR_ECON1:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ECON1 + (0x20U * bank);
            break;
        case DRV_ENCX24J600_SFR_EHT1:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EHT1;
            break;
        case DRV_ENCX24J600_SFR_EHT2:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EHT2;
            break;
        case DRV_ENCX24J600_SFR_EHT3:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EHT3;
            break;
        case DRV_ENCX24J600_SFR_EHT4:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EHT4;
            break;
        case DRV_ENCX24J600_SFR_EPMM1:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EPMM1;
            break;
        case DRV_ENCX24J600_SFR_EPMM2:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EPMM2;
            break;
        case DRV_ENCX24J600_SFR_EPMM3:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EPMM3;
            break;
        case DRV_ENCX24J600_SFR_EPMM4:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EPMM4;
            break;
        case DRV_ENCX24J600_SFR_EPMCS:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EPMCS;
            break;
        case DRV_ENCX24J600_SFR_EPMO:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EPMO;
            break;
        case DRV_ENCX24J600_SFR_ERXFCON:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ERXFCON;
            break;
        case DRV_ENCX24J600_SFR_MACON1:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MACON1;
            break;
        case DRV_ENCX24J600_SFR_MACON2:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MACON2;
            break;
        case DRV_ENCX24J600_SFR_MABBIPG:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MABBIPG;
            break;
        case DRV_ENCX24J600_SFR_MAIPG:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MAIPG;
            break;
        case DRV_ENCX24J600_SFR_MACLCON:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MACLCON;
            break;
        case DRV_ENCX24J600_SFR_MAMXFL:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MAMXFL;
            break;
        case DRV_ENCX24J600_SFR_MICMD:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MICMD;
            break;
        case DRV_ENCX24J600_SFR_MIREGADR:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MIREGADR;
            break;
        case DRV_ENCX24J600_SFR_MAADR3:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MAADR3;
            break;
        case DRV_ENCX24J600_SFR_MAADR2:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MAADR2;
            break;
        case DRV_ENCX24J600_SFR_MAADR1:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MAADR1;
            break;
        case DRV_ENCX24J600_SFR_MIWR:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MIWR;
            break;
        case DRV_ENCX24J600_SFR_MIRD:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MIRD;
            break;
        case DRV_ENCX24J600_SFR_MISTAT:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_MISTAT;
            break;
        case DRV_ENCX24J600_SFR_EPAUS:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EPAUS;
            break;
        case DRV_ENCX24J600_SFR_ECON2:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ECON2;
            break;
        case DRV_ENCX24J600_SFR_ERXWM:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ERXWM;
            break;
        case DRV_ENCX24J600_SFR_EIE:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EIE;
            break;
        case DRV_ENCX24J600_SFR_EIDLED:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EIDLED;
            break;
        case DRV_ENCX24J600_SFR_EGPDATA:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EGPDATA;
            break;
        case DRV_ENCX24J600_SFR_ERXDATA:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ERXDATA;
            break;
        case DRV_ENCX24J600_SFR_EUDADTA:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EUDADTA;
            break;
        case DRV_ENCX24J600_SFR_EGPRDPT:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EGPRDPT;
            break;
        case DRV_ENCX24J600_SFR_EGPWRPT:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EGPWRPT;
            break;
        case DRV_ENCX24J600_SFR_ERXRDPT:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ERXRDPT;
            break;
        case DRV_ENCX24J600_SFR_ERXWRPT:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ERXWRPT;
            break;
        case DRV_ENCX24J600_SFR_EUDARDPT:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EUDARDPT;
            break;
        case DRV_ENCX24J600_SFR_EUDAWRPT:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_EUDAWRPT;
            break;
        default:
            sfr = (uint8_t)DRV_ENCX24J600_SPI_SFR_ETXST;
            break;
    }
    return (DRV_ENCX24J600_SPI_SFR_MAP)sfr;
}


int32_t DRV_ENCX24J600_SPI_InitializeInterface(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
{
    pDrvInstance->busData = (*pDrvInstance->stackCfg.callocF)(pDrvInstance->stackCfg.memH, 1, sizeof(DRV_ENCX24J600_spiBusData));
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    if (pDrvInstance->busData == NULL)
    {
        return -1;
    }
    pDrvInstance->busVTable = &drv_encx24j600_spi_vtable;
    pBusInfo->clientHandle = DRV_HANDLE_INVALID;
    pBusInfo->currentBank = 0;  // Some number that isn't 0-4
    pBusInfo->bankSelectNeeded = true;
    return 0;
}

int32_t DRV_ENCX24J600_SPI_DeinitializeInterface(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance)
{
    (void)(*pDrvInstance->stackCfg.freeF)(pDrvInstance->stackCfg.memH, pDrvInstance->busData);
    pDrvInstance->busData = NULL;
    pDrvInstance->busVTable = NULL;
    return 0;
}

// *****************************************************************************
/* Open

    Summary:
    Opens the bus interface

    Details:
    This function call opens the bus interface.

    Preconditions:
    The bus has to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns
        Negative if error
        Valid Handle on success
*/
int32_t DRV_ENCX24J600_SPI_OpenInterface(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
{
   DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    pBusInfo->clientHandle = DRV_SPI_Open(pDrvInstance->drvCfg.spiDrvIndex, DRV_IO_INTENT_READWRITE);
    if (pBusInfo->clientHandle == DRV_HANDLE_INVALID)
    {
        return -1;
    }
      
    (void)DRV_SPI_TransferSetup (pBusInfo->clientHandle, &pDrvInstance->drvCfg.spiSetup);
    
    return 0;
}

// *****************************************************************************
/* Close

    Summary:
    Closes the bus interface

    Details:
    This function call closes the bus interface.

    Preconditions:
    The bus has to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns:
        None
*/
void DRV_ENCX24J600_SPI_CloseInterface( struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance)
{
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    if (pBusInfo->clientHandle == DRV_HANDLE_INVALID)
    {
        return;
    }
    DRV_SPI_Close(pBusInfo->clientHandle);
    pBusInfo->clientHandle = DRV_HANDLE_INVALID;
}

// *****************************************************************************
/* Operation Result

    Summary
    Checks the status of an operation on the bus.

    Details
    This function checks the status of an operation that was previously start.

    Preconditions
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance
        handle - Handle created by the operation.

    Returns
        DRV_ENCX24J600_BR_SUCCESS �?? if the operation was successful
        DRV_ENCX24J600_BR_PENDING �?? if the operation is still pending
        DRV_ENCX24J600_BR_ERROR �?? if there was an error in the operation
*/
DRV_ENCX24J600_BUS_RESULT DRV_ENCX24J600_SPI_OperationResult( struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, uintptr_t  handle )
{
    DRV_ENCX24J600_BUS_RESULT res;
    DRV_SPI_TRANSFER_EVENT evRes = DRV_SPI_TransferStatusGet(handle);

    switch (evRes)
    {
        case DRV_SPI_TRANSFER_EVENT_PENDING:
            res = DRV_ENCX24J600_BR_PENDING;
            break;
        case DRV_SPI_TRANSFER_EVENT_HANDLE_EXPIRED:
        case DRV_SPI_TRANSFER_EVENT_COMPLETE:
            res = DRV_ENCX24J600_BR_SUCCESS;
            break;
        default:
            res = DRV_ENCX24J600_BR_ERROR;
            break;
    }

    return res;
}

static int8_t F_DRV_ENCX24J600_DoBankSelect(DRV_ENCX24J600_spiBusData * pBusInfo,  DRV_ENCX24J600_SPI_SFR_MAP sfr, uint8_t  opIndex)
{
    bool needBank = pBusInfo->bankSelectNeeded;
    uint8_t neededBank = 0;
    if (pBusInfo->currentBank > 3U)
    {
        pBusInfo->currentBank = 0U;
        needBank = true;
    }
    if (sfr < 0x20)
    {
        neededBank = 0;
    }
    else if (sfr < 0x40)
    {
        neededBank = 1;
    }
    else if (sfr < 0x60)
    {
        neededBank = 2;
    }
    else if (sfr < 0x80)
    {
        neededBank = 3;
    }
    else
    {
        return 4;
    }
    if (neededBank != pBusInfo->currentBank)
    {
        needBank = true;
    }
    if (needBank)
    {
        switch (neededBank)
        {
            case 0U:
                pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_B0SEL;
                break;
            case 1U:
                pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_B1SEL;
                break;
            case 2U:
                pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_B2SEL;
                break;
            case 3U:
                pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_B3SEL;
                break;
            default:
                // do nothing
                break;
        }
        return (int8_t)neededBank;
    }
    else
    {
        return -1;
    }
    
}

// *****************************************************************************
/* Special Function Register Write

    Summary:
    Starts a write operation to the special function register.

    Details:
    This function sends a value to the specified register.  It uses an opIndex
    to help with chaining commands to the bus.  Each command to the bus can be
    on a different index to allow them to be buffered.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance
        reg �?? The Special Function Register to write to.
        Value �?? the value to write into the register
        opIndex �?? the index to use for this operation

    Returns:
        NULL �?? On Error
        Valid Handle �?? on success
*/
uintptr_t DRV_ENCX24J600_SPI_SfrWrite(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_SFR_MAP  reg, DRV_ENCX24J600_RegUnion  value,  uint8_t  opIndex )
{
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    int8_t bankSelectResult;

    DRV_ENCX24J600_SPI_SFR_MAP sfr = F_DRV_ENCX24J600_MapSfrToSpi(reg, pBusInfo->currentBank);

    bankSelectResult = F_DRV_ENCX24J600_DoBankSelect(pBusInfo, sfr, opIndex);

    // Put in the data
    pBusInfo->commandWrBuffers[opIndex][2] = (uint8_t)value.value & 0xffU;
    pBusInfo->commandWrBuffers[opIndex][3] = (uint8_t)(value.value >> 8) & 0xffU;

    if (bankSelectResult == 4)
    {
        // Have to do the four byte command
        pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_WCRU;
        pBusInfo->commandWrBuffers[opIndex][1] = (uint8_t)sfr;
        
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex]), 4, &(pBusInfo->bufferHandles[opIndex]));

        //Least intrusive fix for MH-4456
        if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            pBusInfo->bufferHandles[opIndex] = 0;
        }
        return pBusInfo->bufferHandles[opIndex];
    }

    pBusInfo->commandWrBuffers[opIndex][1] = (uint8_t)DRV_ENCX24J600_SPI_WCR | ((uint8_t)sfr & 0x1fU);
    if (bankSelectResult != -1)
    {
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex]), 4, &(pBusInfo->bufferHandles[opIndex]));
        //Least intrusive fix for MH-4456
        if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            pBusInfo->bufferHandles[opIndex] = 0;
        }
        if (pBusInfo->bufferHandles[opIndex] != 0)
        {
            pBusInfo->currentBank = (uint8_t)bankSelectResult;
            pBusInfo->bankSelectNeeded = false;
        }
        return pBusInfo->bufferHandles[opIndex];
    }
    DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex][1]), 3, &(pBusInfo->bufferHandles[opIndex]));
    //Least intrusive fix for MH-4456
    if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        pBusInfo->bufferHandles[opIndex] = 0;
    }
    return pBusInfo->bufferHandles[opIndex];
}

// *****************************************************************************
/* Special Function Register Read Start

    Summary:
    Starts a read operation from the special function register.

    Details
    This function starts a read operation of a value from the specified register.
    It uses an opIndex to help with chaining commands to the bus.  Each command
    to the bus can be on a different index to allow them to be buffered.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance
        reg �?? The Special Function Register to write to.
        opIndex �?? the index to use for this operation

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
uintptr_t DRV_ENCX24J600_SPI_SfrReadStart(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_SFR_MAP  reg, uint8_t  opIndex )
{
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;

    int8_t bankSelectResult;

    DRV_ENCX24J600_SPI_SFR_MAP sfr = F_DRV_ENCX24J600_MapSfrToSpi(reg, pBusInfo->currentBank);

    bankSelectResult = F_DRV_ENCX24J600_DoBankSelect(pBusInfo, sfr, opIndex);

    if (bankSelectResult == 4)
    {
        // Have to do the four byte command
        pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_RCRU;
        pBusInfo->commandWrBuffers[opIndex][1] = (uint8_t)sfr;
        DRV_SPI_WriteReadTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex]), 4, &(pBusInfo->commandRdBuffers[opIndex]), 4, &(pBusInfo->bufferHandles[opIndex]));
        //Least intrusive fix for MH-4456
        if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            pBusInfo->bufferHandles[opIndex] = 0;
        }
        return pBusInfo->bufferHandles[opIndex];
    }

    pBusInfo->commandWrBuffers[opIndex][1] = (uint8_t)DRV_ENCX24J600_SPI_RCR | ((uint8_t)sfr & 0x1fU);
    if (bankSelectResult != -1)
    {
        DRV_SPI_WriteReadTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex]), 4, &(pBusInfo->commandRdBuffers[opIndex]), 4, &(pBusInfo->bufferHandles[opIndex]));
        //Least intrusive fix for MH-4456
        if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            pBusInfo->bufferHandles[opIndex] = 0;
        }
        if (pBusInfo->bufferHandles[opIndex] != 0)
        {
            pBusInfo->currentBank = (uint8_t)bankSelectResult;
            pBusInfo->bankSelectNeeded = false;

        }
        return pBusInfo->bufferHandles[opIndex];
    }
    DRV_SPI_WriteReadTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex][1]), 3, &(pBusInfo->commandRdBuffers[opIndex][1]), 3, &(pBusInfo->bufferHandles[opIndex]));
    //Least intrusive fix for MH-4456
    if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        pBusInfo->bufferHandles[opIndex] = 0;
    }
    
    return pBusInfo->bufferHandles[opIndex];
}

// *****************************************************************************
/* Special Function Register Read Result

    Summary
    Gets the result of a read operation.

    Details
    This function checks the result of the read operation and if it was
    successful it will write the value into the output.

    Preconditions
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance
        handle �?? the handle from the read start operation
        value �?? where to put the results of the operation
        opIndex �?? the index to use for this operation

    Returns
        DRV_ENCX24J600_BR_SUCCESS �?? if the operation was successful
        DRV_ENCX24J600_BR_PENDING �?? if the operation is still pending
        DRV_ENCX24J600_BR_ERROR �?? if there was an error in the operation
*/
DRV_ENCX24J600_BUS_RESULT DRV_ENCX24J600_SPI_SfrReadResult(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, uintptr_t  handle, DRV_ENCX24J600_RegUnion *  value,  uint8_t  opIndex )
{
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    DRV_ENCX24J600_BUS_RESULT res = DRV_ENCX24J600_SPI_OperationResult(pDrvInstance, handle);
    if (res != DRV_ENCX24J600_BR_SUCCESS)
    {
        return res;
    }
    value->value = (uint16_t)pBusInfo->commandRdBuffers[opIndex][2] + ((uint16_t)pBusInfo->commandRdBuffers[opIndex][3] << 8);
    return res;
}

// *****************************************************************************
/* Special Function Register Bit Set

    Summary
    Sets a selection of bits in a special function register.

    Details
    This function will allow the setting of specific bits in a register without
    having to read the register first.  The bits turned on in value are the bits
    that get set.

    Preconditions
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance
        reg �?? The Special Function Register to write to.
        Value �?? The bits to set in the register.
        opIndex �?? the index to use for this operation

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
uintptr_t DRV_ENCX24J600_SPI_SfrBitSet(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_SFR_MAP  reg, DRV_ENCX24J600_RegUnion  value, uint8_t  opIndex )
{
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;

    int8_t bankSelectResult;

    DRV_ENCX24J600_SPI_SFR_MAP sfr = F_DRV_ENCX24J600_MapSfrToSpi(reg, pBusInfo->currentBank);

    bankSelectResult = F_DRV_ENCX24J600_DoBankSelect(pBusInfo, sfr, opIndex);

    // Put in the data
    pBusInfo->commandWrBuffers[opIndex][2] = (uint8_t)value.value & 0xffU;
    pBusInfo->commandWrBuffers[opIndex][3] = (uint8_t)(value.value >> 8) & 0xffU;

    if (bankSelectResult == 4)
    {
        // Have to do the four byte command
        pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_BFSU;
        pBusInfo->commandWrBuffers[opIndex][1] = (uint8_t)sfr;
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex]), 4, &(pBusInfo->bufferHandles[opIndex]));
        //Least intrusive fix for MH-4456
        if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            pBusInfo->bufferHandles[opIndex] = 0;
        }
        return pBusInfo->bufferHandles[opIndex];
    }

    pBusInfo->commandWrBuffers[opIndex][1] = (uint8_t)DRV_ENCX24J600_SPI_BFS | ((uint8_t)sfr & 0x1fU);
    if (bankSelectResult != -1)
    {
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex]), 4, &(pBusInfo->bufferHandles[opIndex]));
        //Least intrusive fix for MH-4456
        if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            pBusInfo->bufferHandles[opIndex] = 0;
        }
        if (pBusInfo->bufferHandles[opIndex] != 0)
        {
            pBusInfo->currentBank = (uint8_t)bankSelectResult;
            pBusInfo->bankSelectNeeded = false;

        }
        return pBusInfo->bufferHandles[opIndex];
    }
    
    DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex][1]), 3, &(pBusInfo->bufferHandles[opIndex]));
    //Least intrusive fix for MH-4456
    if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        pBusInfo->bufferHandles[opIndex] = 0;
    }
    return pBusInfo->bufferHandles[opIndex];

}

// *****************************************************************************
/* Special Function Register Bit Clear

    Summary:
    Clears a selection of bits in a special function register.

    Details:
    This function will allow the clearing of specific bits in a register without
    having to read the register first.  The bits turned on in value are the bits
    that get cleared.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance
        reg �?? The Special Function Register to write to.
        Value �?? The bits to clear in the register.
        opIndex �?? the index to use for this operation

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
 uintptr_t DRV_ENCX24J600_SPI_SfrBitClear(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_SFR_MAP  reg, DRV_ENCX24J600_RegUnion  value, uint8_t  opIndex )
 {
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;

    int8_t bankSelectResult;

    DRV_ENCX24J600_SPI_SFR_MAP sfr = F_DRV_ENCX24J600_MapSfrToSpi(reg, pBusInfo->currentBank);

    bankSelectResult = F_DRV_ENCX24J600_DoBankSelect(pBusInfo, sfr, opIndex);

    // Put in the data
    pBusInfo->commandWrBuffers[opIndex][2] = (uint8_t)value.value & 0xffU;
    pBusInfo->commandWrBuffers[opIndex][3] = (uint8_t)(value.value >> 8) & 0xffU;

    if (bankSelectResult == 4)
    {
        // Have to do the four byte command
        pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_BFCU;
        pBusInfo->commandWrBuffers[opIndex][1] = (uint8_t)sfr;
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex]), 4, &(pBusInfo->bufferHandles[opIndex]));
        //Least intrusive fix for MH-4456
        if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            pBusInfo->bufferHandles[opIndex] = 0;
        }
        return pBusInfo->bufferHandles[opIndex];
    }

    pBusInfo->commandWrBuffers[opIndex][1] = (uint8_t)DRV_ENCX24J600_SPI_BFC | ((uint8_t)sfr & 0x1fU);
    if (bankSelectResult != -1)
    {
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex]), 4, &(pBusInfo->bufferHandles[opIndex]));
        //Least intrusive fix for MH-4456
        if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            pBusInfo->bufferHandles[opIndex] = 0;
        }
        if (pBusInfo->bufferHandles[opIndex] != 0)
        {
            pBusInfo->currentBank = (uint8_t)bankSelectResult;
            pBusInfo->bankSelectNeeded = false;
        }
        return pBusInfo->bufferHandles[opIndex];
    }
    
    DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex][1]), 3, &(pBusInfo->bufferHandles[opIndex]));
    //Least intrusive fix for MH-4456
    if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        pBusInfo->bufferHandles[opIndex] = 0;
    }
    return pBusInfo->bufferHandles[opIndex];
 }

// *****************************************************************************
/* System Reset

    Summary:
    Sends a device reset.

    Details:
    This function sends the system reset command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns:
        NULL �?? On Error
        Valid Handle �?? on success
*/
 static uint8_t rstCmd = (uint8_t)DRV_ENCX24J600_SPI_SETETHRST;  // Need to force the command byte into memory so a pointer can be passed
 
 uintptr_t DRV_ENCX24J600_SPI_SystemReset(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
 {
     DRV_SPI_TRANSFER_HANDLE ret;
     DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
     DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &rstCmd, 1, &ret);
     //Least intrusive fix for MH-4456
     if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
     {
         return 0;
     }
     return ret;
     
 }

// *****************************************************************************
/* Enable Receive

    Summary:
    Sends an enable RX.

    Details:
    This function sends the Enable RX command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
 static uint8_t erxCmd = (uint8_t)DRV_ENCX24J600_SPI_ENABLERX;  // Need to force the command byte into memory so a pointer can be passed

 uintptr_t DRV_ENCX24J600_SPI_EnableRX(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
 {
     DRV_SPI_TRANSFER_HANDLE ret;
     DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
     DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &erxCmd, 1, &ret);
     //Least intrusive fix for MH-4456
     if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
     {
         return 0;
     }
     return ret;
 }

// *****************************************************************************
/* Disable Receive

    Summary:
    Sends a disable RX.

    Details:
    This function sends the Disable RX command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
 static uint8_t drxCmd = (uint8_t)DRV_ENCX24J600_SPI_DISABLERX;  // Need to force the command byte into memory so a pointer can be passed
 uintptr_t DRV_ENCX24J600_SPI_DisableRX(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
 {
     DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
     DRV_SPI_TRANSFER_HANDLE ret;
     
     DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &drxCmd, 1, &ret);
     //Least intrusive fix for MH-4456
     if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
     {
         return 0;
     }
     return ret;
 }

// *****************************************************************************
/* Request Packet Transmission

    Summary:
    Sends a request packet transmission.

    Details:
    This function sends the request packet transmission command to the
    ENCx24J600.  This command is called by the transmit state machine after a
    packet has been primed.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
 static uint8_t rptCmd = (uint8_t)DRV_ENCX24J600_SPI_SETTXRTS;  // Need to force the command byte into memory so a pointer can be passed
 uintptr_t DRV_ENCX24J600_SPI_ReqPktTx(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
 {
     DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
     DRV_SPI_TRANSFER_HANDLE ret;
     DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &rptCmd, 1, &ret);
     //Least intrusive fix for MH-4456
     if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
     {
         return 0;
     }
     return ret;
     
 }


// *****************************************************************************
/* Decrement Packet Counter

    Summary:
    Sends a decrement packet counter.

    Details:
    This function sends the Decrement Packet command to the ENCx24J600.  This is
    called by the RX state machine after it has received a packet.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
 static uint8_t dpcCmd = (uint8_t)DRV_ENCX24J600_SPI_SETPKTDEC;  // Need to force the command byte into memory so a pointer can be passed

 uintptr_t DRV_ENCX24J600_SPI_DecrPktCtr(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
 {
     DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
     DRV_SPI_TRANSFER_HANDLE ret;
     DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &dpcCmd, 1, &ret);
     //Least intrusive fix for MH-4456
     if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
     {
         return 0;
     }
     return ret;
     
 }
// *****************************************************************************
/* Enable Interrupts

    Summary:
    Sends an enable interrupts.

    Details:
    This function sends the Enable Interrupts command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
  static uint8_t eiCmd = (uint8_t)DRV_ENCX24J600_SPI_SETEIE;  // Need to force the command byte into memory so a pointer can be passed

 uintptr_t DRV_ENCX24J600_SPI_EnableInterrupts(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
 {
     DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
     DRV_SPI_TRANSFER_HANDLE ret;
     DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &eiCmd, 1, &ret);
     //Least intrusive fix for MH-4456
     if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
     {
         return 0;
     }
     return ret;
     
 }


// *****************************************************************************
/*  Disable Interrupts

    Summary:
    Sends a disable interrupts.

    Details:
    This function sends the Disable Interrupts command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns:
        NULL �?? On Error
        Valid Handle �?? on success
*/
 static uint8_t diCmd = (uint8_t)DRV_ENCX24J600_SPI_CLREIE;  // Need to force the command byte into memory so a pointer can be passed

 uintptr_t DRV_ENCX24J600_SPI_DisableInterrupts(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
 {
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    DRV_SPI_TRANSFER_HANDLE ret;
    DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &diCmd, 1, &ret);
     //Least intrusive fix for MH-4456
    if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        return 0;
    }
     return ret;
     
 }
// *****************************************************************************
/*  Disable Flow Control

    Summary:
    Sends a disable flow control.

    Details
    This function sends the disable flow control command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
 static uint8_t dfCmd = (uint8_t)DRV_ENCX24J600_SPI_FCDISABLE;  // Need to force the command byte into memory so a pointer can be passed

 uintptr_t DRV_ENCX24J600_SPI_FlowCtrlDisable(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
 {
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    DRV_SPI_TRANSFER_HANDLE ret;
    DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &dfCmd, 1, &ret);
    //Least intrusive fix for MH-4456
    if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        return 0;
    }
    return ret;
 }

// *****************************************************************************
/*  Single Flow Control

    Summary:
    Sends a single flow control.

    Details:
    This function sends the single flow control command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
 static uint8_t sfCmd = (uint8_t)DRV_ENCX24J600_SPI_FCSINGLE;  // Need to force the command byte into memory so a pointer can be passed
 uintptr_t DRV_ENCX24J600_SPI_FlowCtrlSingle(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
  {
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    DRV_SPI_TRANSFER_HANDLE ret;
    DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &sfCmd, 1, &ret);
    //Least intrusive fix for MH-4456
    if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        return 0;
    }
    return ret;
 }

// *****************************************************************************
/*  Multiple Flow Control

    Summary:
    Sends a multiple flow control.

    Details:
    This function sends the multiple flow control command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns:
        NULL �?? On Error
        Valid Handle �?? on success
*/
 static uint8_t mfCmd = (uint8_t)DRV_ENCX24J600_SPI_FCMULTIPLE;  // Need to force the command byte into memory so a pointer can be passed
 uintptr_t DRV_ENCX24J600_SPI_FlowCtrlMult(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
 {
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    DRV_SPI_TRANSFER_HANDLE ret;
    DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &mfCmd, 1, &ret);
    //Least intrusive fix for MH-4456
    if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        return 0;
    }
    return ret;
 }

// *****************************************************************************
/*  Clear Flow Control

    Summary:
    Sends a clear flow control.

    Details:
    This function sends the clear flow control command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns:
        NULL �?? On Error
        Valid Handle �?? on success
*/
 static uint8_t cfCmd = (uint8_t)DRV_ENCX24J600_SPI_FCCLEAR;  // Need to force the command byte into memory so a pointer can be passed
 uintptr_t DRV_ENCX24J600_SPI_FlowCtrClear(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance )
 {
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    DRV_SPI_TRANSFER_HANDLE ret;
    DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &cfCmd, 1, &ret);
    //Least intrusive fix for MH-4456
    if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        return 0;
    }
    return ret;
 }

// *****************************************************************************
/*  PHY Register Write

    Summary:
    Write to a PHY register on the ENC.

    Details:
    This function starts a write to the PHY on the ENC.  It is actually two bus
    operations so when opIndex is used it requires it and the next index to
    store the buffer and handle.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance
        reg �?? The PHY register to write
        value �?? the Value to write
        opIndex �?? the operation index.

    Returns:
        NULL �?? On Error
        Valid Handle �?? on success
*/
 uintptr_t DRV_ENCX24J600_SPI_PhyWrite(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_PHY_SFR_MAP  reg, DRV_ENCX24J600_RegUnion  value, uint8_t  opIndex )
 {
     //DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
     DRV_ENCX24J600_RegUnion phyReg = {0};
     phyReg.miregadr.PHREG = (uint8_t)reg;
     phyReg.miregadr.writeAsOne = 1;
     uintptr_t first = DRV_ENCX24J600_SPI_SfrWrite(pDrvInstance, DRV_ENCX24J600_SFR_MIREGADR, phyReg, opIndex );
     if (first == 0)
     {
         return 0;
     }
     return DRV_ENCX24J600_SPI_SfrWrite(pDrvInstance, DRV_ENCX24J600_SFR_MIWR, value, opIndex + 1U);
 }

// *****************************************************************************
/*  Write Pointer

    Summary:
    Writes a value to one of the registers.

    Details:
    The ENC hardware has six registers.  This function writes a value to one of
    those registers.  The value refers to the memory location in the ENC hardware.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance
        reg �?? The register to write
        value �?? the value to write
        opIndex �?? the operation index.

    Returns:
        NULL �?? On Error
        Valid Handle �?? on success
*/
 uintptr_t DRV_ENCX24J600_SPI_WritePointer(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_POINTER  reg, uint16_t  value, uint8_t  opIndex )
 {
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    switch (reg)
    {
        case DRV_ENCX24J600_PTR_GPWR:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_WGPWRPT;
            break;
        case DRV_ENCX24J600_PTR_GPRD:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_WGPRDPT;
            break;
        case DRV_ENCX24J600_PTR_RXWR:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_WRXWRPT;
            break;
        case DRV_ENCX24J600_PTR_RXRD:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_WRXRDPT;
            break;
        case DRV_ENCX24J600_PTR_UDAWR:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_WUDAWRPT;
            break;
        case DRV_ENCX24J600_PTR_UDARD:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_WUDARDPT;
            break;

        default:
            // do nothing
            break;
    }
    pBusInfo->commandWrBuffers[opIndex][1] = (uint8_t)value & 0xffU;
    pBusInfo->commandWrBuffers[opIndex][2] = (uint8_t)(value >> 8) & 0xffU;
    DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex]), 3, &(pBusInfo->bufferHandles[opIndex]));
    //Least intrusive fix for MH-4456
    if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        pBusInfo->bufferHandles[opIndex] = 0;
    }
    return pBusInfo->bufferHandles[opIndex];

}

// *****************************************************************************
/*  Read Pointer Start

   Summary:
   Starts a read from one of the registers.

   Details:
   The ENC hardware has six registers.  This function starts a read operation
   from one of those registers.

   Preconditions:
   The bus had to have been initialized first.

   Parameters:
       pDrvInstance �?? The driver instance
       reg �?? The register to write
       opIndex �?? the operation index.

   Returns:
       NULL �?? On Error
       Valid Handle �?? on success
*/
uintptr_t DRV_ENCX24J600_SPI_ReadPointerStart(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_POINTER  reg, uint8_t  opIndex )
{
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    switch (reg)
    {
        case DRV_ENCX24J600_PTR_GPWR:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_RGPWRPT;
            break;
        case DRV_ENCX24J600_PTR_GPRD:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_RGPRDPT;
            break;
        case DRV_ENCX24J600_PTR_RXWR:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_RRXWRPT;
            break;
        case DRV_ENCX24J600_PTR_RXRD:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_RRXRDPT;
            break;
        case DRV_ENCX24J600_PTR_UDAWR:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_RUDAWRPT;
            break;
        case DRV_ENCX24J600_PTR_UDARD:
            pBusInfo->commandWrBuffers[opIndex][0] = (uint8_t)DRV_ENCX24J600_SPI_RUDARDPT;
            break;
        default:
            // do nothing
            break;
    }
    
    DRV_SPI_WriteReadTransferAdd(pBusInfo->clientHandle, &(pBusInfo->commandWrBuffers[opIndex]), 3, &(pBusInfo->commandRdBuffers[opIndex]), 3, &(pBusInfo->bufferHandles[opIndex]));

    //Least intrusive fix for MH-4456
    if (pBusInfo->bufferHandles[opIndex] == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        pBusInfo->bufferHandles[opIndex] = 0;
    }

    return pBusInfo->bufferHandles[opIndex];

 }

// *****************************************************************************
/*  Read Pointer Result

    Summary:
    Gets the result from a read pointer operation

    Details:
    This function gets the results from a read pointer operation

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance
        handle �?? From the read operation
        value �?? the location for the results
        opIndex �?? the operation index.

    Returns:
        DRV_ENCX24J600_BR_SUCCESS �?? if the operation was successful
        DRV_ENCX24J600_BR_PENDING �?? if the operation is still pending
        DRV_ENCX24J600_BR_ERROR �?? if there was an error in the operation
*/
 DRV_ENCX24J600_BUS_RESULT DRV_ENCX24J600_SPI_ReadPointerResult(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, uintptr_t handle, uint16_t* value, uint8_t  opIndex )
 {
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    DRV_ENCX24J600_BUS_RESULT res = DRV_ENCX24J600_SPI_OperationResult(pDrvInstance, handle);
    if (res != DRV_ENCX24J600_BR_SUCCESS)
    {
        return res;
    }
    *value = (uint16_t)pBusInfo->commandRdBuffers[opIndex][1] + ((uint16_t)pBusInfo->commandRdBuffers[opIndex][2] << 8);
    return res;
 }

// *****************************************************************************
/* Write Data

    Summary:
    Writes data to the ENC hardware

    Details:
    This function writes data to the ENC.

    Preconditions:
    The bus had to have been initialized first.  The parameters to this function
    are a little different than expected.  The TCP/IP packet segment is allocated
    with some space before the actual pointer in the packet.  For the PIC32
    Internal MAC this is two bytes.  This data is used by the MAC for its own
    purposes.  In the case of the ENCX24J600 only 1 byte is needed for SPI.  For
    PSP a different number may be needed.  The buffer that is passed in is the
    start of the data segment, and this function assumes there is some allocated
    space before the pointer.  The datasize is the size of the data portion, not
    the total size of the buffer.

    Parameters:
        pDrvInstance �?? The driver instance
        reg �?? the register to write to
        buffer �?? the location of the buffer to write
        dataSize �?? the size of the data to write.

    Returns:
        0 �?? on error
        Valid handle �?? on success
*/
 uintptr_t DRV_ENCX24J600_SPI_WriteData(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_POINTER  reg, uint8_t *  buffer, uint16_t  dataSize)
 {
     DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
     DRV_SPI_TRANSFER_HANDLE ret;
     switch (reg)
     {
         case DRV_ENCX24J600_PTR_GPWR:
         case DRV_ENCX24J600_PTR_GPRD:
             buffer[-1] = (uint8_t)DRV_ENCX24J600_SPI_WGPDATA;
             break;
         case DRV_ENCX24J600_PTR_RXWR:
         case DRV_ENCX24J600_PTR_RXRD:
             buffer[-1] = (uint8_t)DRV_ENCX24J600_SPI_WRXDATA;
             break;
         case DRV_ENCX24J600_PTR_UDAWR:
         case DRV_ENCX24J600_PTR_UDARD:
             buffer[-1] = (uint8_t)DRV_ENCX24J600_SPI_WUDADATA;
             break;
         default:
             // do nothing
             break;
     }
     
     DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, &(buffer[-1]), (size_t)dataSize + 1U, &ret);
     //Least intrusive fix for MH-4456
     if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
     {
         return 0;
     }
     return ret;
 }

// *****************************************************************************
/* Read Data Start

    Summary:
    Read data from the ENC hardware.

    Details:
    This function reads data from the ENC. The parameters to this function are a
    little different than expected.  The TCP/IP packet segment is allocated with
    some space before the actual pointer in the packet.  For the PIC32 Internal
    MAC this is two bytes.  This data is used by the MAC for its own purposes.
    n the case of the ENCX24J600 only 1 byte is needed for SPI.  For PSP a
    different number may be needed.  The buffer that is passed in is the start
    of the data segment, and this function assumes there is some allocated space
    before the pointer.  The datasize is the size of the data portion, not the
    total size of the buffer.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance
        reg �?? the register to write to
        buffer �?? the location of the buffer to write
        dataSize �?? the size of the data to read.

    Returns:
        0 �?? on error
        Valid handle �?? on success
*/
 static uint8_t rddCmd;
 uintptr_t DRV_ENCX24J600_SPI_ReadData(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_POINTER  reg, uint8_t *  buffer, uint16_t  dataSize)
 {
    DRV_ENCX24J600_spiBusData * pBusInfo = (DRV_ENCX24J600_spiBusData *)pDrvInstance->busData;
    DRV_SPI_TRANSFER_HANDLE ret;
    switch (reg)
    {
        case DRV_ENCX24J600_PTR_GPWR:
        case DRV_ENCX24J600_PTR_GPRD:
            rddCmd = (uint8_t)DRV_ENCX24J600_SPI_RGPDATA;
            break;
        case DRV_ENCX24J600_PTR_RXWR:
        case DRV_ENCX24J600_PTR_RXRD:
            rddCmd = (uint8_t)DRV_ENCX24J600_SPI_RRXDATA;
            break;
        case DRV_ENCX24J600_PTR_UDAWR:
        case DRV_ENCX24J600_PTR_UDARD:
            rddCmd = (uint8_t)DRV_ENCX24J600_SPI_RUDADATA;
            break;
        default:
            // do nothing
            break;
    }
    
    DRV_SPI_WriteReadTransferAdd(pBusInfo->clientHandle, &rddCmd, 1, &(buffer[-1]), (size_t)dataSize + 1U, &ret);
    //Least intrusive fix for MH-4456
    if (ret == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        return 0;
    }
    return ret;
 }
