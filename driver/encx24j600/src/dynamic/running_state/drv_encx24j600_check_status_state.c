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
/*****************************************************************************
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

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
