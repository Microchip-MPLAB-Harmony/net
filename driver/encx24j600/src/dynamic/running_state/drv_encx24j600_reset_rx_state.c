/*******************************************************************************
  ENCx24J600 Check TX Status State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_check_tx_status_state.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2014-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include "drv_encx24j600_reset_rx_state.h"
#include "../drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"
#include "drv_encx24j600_running_state.h"

int32_t DRV_ENCX24J600_ResetRxStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_RESET_RX_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.resetRxInfo);
    DRV_ENCX24J600_RegUnion reg = {0};
    uintptr_t ret;
    
    switch (curSt->state)
    {
        case DRV_ENCX24J600_RRX_WAIT:
            break;
        case DRV_ENCX24J600_RRX_STARTING:
            curSt->state = DRV_ENCX24J600_RRX_SET_RXRST;
        case DRV_ENCX24J600_RRX_SET_RXRST:
        {
            reg.value = 0;
            reg.econ2.RXRST = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitSet)(pDrvInst, DRV_ENCX24J600_SFR_ECON2, reg, DRV_ENCX24J600_RRX_OP_SET_RXRST);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_RRX_RESET_RXRST;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_RRX_RESET_RXRST:
        {
            reg.value = 0;
            reg.econ2.RXRST = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitClr)(pDrvInst, DRV_ENCX24J600_SFR_ECON2, reg, DRV_ENCX24J600_RRX_OP_CLEAR_RXRST);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_RRX_READ_ESTAT_START;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_RRX_READ_ESTAT_START:
        {
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_ESTAT, DRV_ENCX24J600_RRX_OP_READ_ESTAT);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_RRX_READ_ESTAT_WAIT;
                curSt->estatOp = ret;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_RRX_READ_ESTAT_WAIT:
        {
            if (_DRV_ENCX24J600_ReadSfr(pDrvInst, curSt->estatOp, &reg, DRV_ENCX24J600_CS_OP_READ_ESTAT))
            {
                if (reg.estat.PKTCNT == 0)
                {
                    curSt->state = DRV_ENCX24J600_RRX_SET_ERXST;
                }
                else
                {
                    uint8_t x = 0;
                    for (x = 0; x < reg.estat.PKTCNT; x++)
                    {
                        ret = (*pDrvInst->busVTable->fpDecPktCnt)(pDrvInst);
                        if (ret == 0)
                        {
                            break;
                        }
                    }
                    curSt->state = DRV_ENCX24J600_RRX_READ_ESTAT_START;
                    break;                    
                }
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_RRX_SET_ERXST:
        {
            reg.value = 0;
            reg.erxst.ERXST = pDrvInst->encMemRxStart;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_ERXST, reg, DRV_ENCX24J600_CS_OP_SET_ERXST);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_RRX_SET_ERXTAIL;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_RRX_SET_ERXTAIL:
        {
            reg.value = 0;
            reg.erxtail.ERXTAIL = pDrvInst->encMemRxEnd - 2;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_ERXTAIL, reg, DRV_ENCX24J600_CS_OP_SET_ERXTAIL);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_RRX_SET_RXEN;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_RRX_SET_RXEN:
        {
            ret = (*pDrvInst->busVTable->fpRxEnable)(pDrvInst);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_RRX_WAIT;
            }
            break;
        }      
        default:
            break;
    }
    return 0;
}