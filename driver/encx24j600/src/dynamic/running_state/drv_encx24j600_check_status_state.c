/*******************************************************************************
  ENCx24J600 Check Status State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_check_status_state.c
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
#include "drv_encx24j600_check_status_state.h"
#include "../drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"
#include "drv_encx24j600_running_state.h"


int32_t DRV_ENCX24J600_ChkStatusStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHECK_STATUS_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkStaInfo);
    DRV_ENCX24J600_RegUnion reg = {0};
    uintptr_t ret;
    switch(curSt->state)
    {
        case DRV_ENCX24J600_CS_WAIT:
        {
            if (curSt->statusUpdateNeeded)
            {
                curSt->state = DRV_ENCX24J600_CS_READ_ESTAT;
            }
            else
            {
                break;  // For some reason the Interrupts aren't firing.
            }
        }
        case DRV_ENCX24J600_CS_READ_ESTAT:
        {
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_ESTAT, DRV_ENCX24J600_CS_OP_READ_ESTAT);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_WAIT_FOR_ESTAT;
                curSt->estatOp = ret;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_WAIT_FOR_ESTAT:
        {
            if (_DRV_ENCX24J600_ReadSfr(pDrvInst, curSt->estatOp, &reg, DRV_ENCX24J600_CS_OP_READ_ESTAT))
            {
                curSt->statusUpdateNeeded = false;
                if (reg.estat.PHYDPX == 1)
                {
                    if (pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexMode != true)
                    {
                        pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexMode = true;
                        pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexChangeNeeded = true;
                    }
                }
                else
                {
                    if (pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexMode == true)
                    {
                        pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexMode = false;
                        pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexChangeNeeded = true;
                    }
                }
                if (curSt->linkState != reg.estat.PHYLNK)
                {
                    /*  // Not needed as this is handled by tcpip_manager.c
                    if (reg.estat.PHYLNK)
                    {
                        DRV_ENCX24J600_SetEvent(pDrvInst, TCPIP_MAC_EV_CONN_ESTABLISHED);
                    }
                    else
                    {
                        DRV_ENCX24J600_SetEvent(pDrvInst, TCPIP_MAC_EV_CONN_LOST);
                    }*/
                    curSt->linkState = reg.estat.PHYLNK;
                }
                if (reg.estat.PKTCNT != 0)
                {
                    //SYS_CONSOLE_PRINT("There are %d packets waiting \r\n", reg.estat.PKTCNT);
                    pDrvInst->rxPacketPending = true;

                }
                else
                {
                    pDrvInst->rxPacketPending = false;

                }
                curSt->state = DRV_ENCX24J600_CS_CLR_EIR;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_CLR_EIR:
        {
            reg.value = 0;
            reg.eir.LINKIF = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitClr)(pDrvInst, DRV_ENCX24J600_SFR_EIR, reg, DRV_ENCX24J600_CS_OP_RST_EIR);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_WAIT;
            }
        }

    }

    return 0;
}
int32_t DRV_ENCX24J600_ChkStatusStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHECK_STATUS_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkStaInfo);
    curSt->state = DRV_ENCX24J600_CS_WAIT;
    curSt->statusUpdateNeeded = true;
    curSt->linkState = false;


    return 0;
}
int32_t DRV_ENCX24J600_ChkStatusStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    return 0;
}
