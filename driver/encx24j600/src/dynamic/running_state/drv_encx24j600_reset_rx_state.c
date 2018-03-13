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
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.
Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).
You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.
SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
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