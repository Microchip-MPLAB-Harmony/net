/*******************************************************************************
  ENCx24J600 Configure State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_configure_state.c
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
#include "drv_encx24j600_detect_state.h"
#include "../drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"

int32_t DRV_ENCX24J600_ConfigStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CONFIGURE_STATE_INFO * curSt = &(pDrvInst->mainStateInfo.initInfo.configStateInfo);
    DRV_ENCX24J600_RegUnion reg = {0};
    uint16_t regPtr = 0;
    uintptr_t ret;
    switch (curSt->state)
    {
        case DRV_ENCX24J600_CS_SET_EGPRDPT:
        {
            ret = (*pDrvInst->busVTable->fpPtrWr)(pDrvInst, DRV_ENCX24J600_PTR_GPRD, regPtr, DRV_ENCX24J600_CS_OP_SET_EGPRDPT);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_ERXRDPT;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_ERXRDPT:
        {
            regPtr = pDrvInst->encMemRxStart;
            ret = (*pDrvInst->busVTable->fpPtrWr)(pDrvInst, DRV_ENCX24J600_PTR_RXRD, regPtr, DRV_ENCX24J600_CS_OP_SET_ERXRDPT);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_EUDARDPT;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_EUDARDPT:
        {
            regPtr = pDrvInst->encMemUdaStart;
            ret = (*pDrvInst->busVTable->fpPtrWr)(pDrvInst, DRV_ENCX24J600_PTR_UDARD, regPtr, DRV_ENCX24J600_CS_OP_SET_EUDARDPT);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_EGPWRPT;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_EGPWRPT:
        {
            regPtr = 0;
            ret = (*pDrvInst->busVTable->fpPtrWr)(pDrvInst, DRV_ENCX24J600_PTR_GPWR, regPtr, DRV_ENCX24J600_CS_OP_SET_EGPWRPT);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_ERXRDPT;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_ERXWRPT:
        {
            regPtr = pDrvInst->encMemRxStart;
            ret = (*pDrvInst->busVTable->fpPtrWr)(pDrvInst, DRV_ENCX24J600_PTR_RXWR, regPtr, DRV_ENCX24J600_CS_OP_SET_ERXWRPT);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_EUDARDPT;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_EUDAWRPT:
        {
            regPtr = pDrvInst->encMemUdaStart;
            ret = (*pDrvInst->busVTable->fpPtrWr)(pDrvInst, DRV_ENCX24J600_PTR_UDAWR, regPtr, DRV_ENCX24J600_CS_OP_SET_EUDAWRPT);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_EGPWRPT;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_ERXST:
        {
            reg.value = 0;
            reg.erxst.ERXST = pDrvInst->encMemRxStart;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_ERXST, reg, DRV_ENCX24J600_CS_OP_SET_ERXST);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_ERXTAIL;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_ERXTAIL:
        {
            reg.value = 0;
            reg.erxtail.ERXTAIL = pDrvInst->encMemRxEnd - 2;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_ERXTAIL, reg, DRV_ENCX24J600_CS_OP_SET_ERXTAIL);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_PHCON1;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_PHCON1:
        {
            reg.value = 0;
            switch (pDrvInst->drvCfg.ethType)
            {
                case TCPIP_ETH_OPEN_10:
                {
                    if (pDrvInst->drvCfg.dupMode == TCPIP_ETH_OPEN_FDUPLEX)
                    {
                        reg.phcon1.PFULDPX = 1;
                    }
                    ret = (*pDrvInst->busVTable->fpPhyWr)(pDrvInst, DRV_ENCX24J600_PHY_SFR_PHCON1, reg, DRV_ENCX24J600_CS_OP_SET_PHCON1);
                    if (ret != 0)
                    {
                        curSt->state = DRV_ENCX24J600_CS_SET_EUDAST;
                    }
                    else
                    {
                        return 0;
                    }
                    break;
                }
                
                case TCPIP_ETH_OPEN_100:
                {
                    if (pDrvInst->drvCfg.dupMode != TCPIP_ETH_OPEN_HDUPLEX)
                    {
                        reg.phcon1.PFULDPX = 1;
                    }
                    reg.phcon1.SPD100 = 1;
                    ret = (*pDrvInst->busVTable->fpPhyWr)(pDrvInst, DRV_ENCX24J600_PHY_SFR_PHCON1, reg, DRV_ENCX24J600_CS_OP_SET_PHCON1);
                    if (ret != 0)
                    {
                        curSt->state = DRV_ENCX24J600_CS_SET_EUDAST;
                    }
                    else
                    {
                        return 0;
                    }
                    break;                    
                }
                case TCPIP_ETH_OPEN_AUTO:
                default:
                {
                    curSt->state = DRV_ENCX24J600_CS_SET_EUDAST;
                    break;
                }   
            }
        }
            
        
        case DRV_ENCX24J600_CS_SET_EUDAST:
        {
            reg.value = 0;
            reg.eudast.EUDAST = pDrvInst->encMemUdaStart;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_EUDAST, reg, DRV_ENCX24J600_CS_OP_SET_EUDAST);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_EUDAND;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_EUDAND:
        {
            reg.value = 0;
            reg.eudand.EUDAND = pDrvInst->encMemUdaEnd;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_EUDAND, reg, DRV_ENCX24J600_CS_OP_SET_EUDAND);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_ERXFCON;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_ERXFCON:
        {
            reg.value = 0;
            reg.erxfcon.MCEN = 1;
            reg.erxfcon.BCEN = 1;
            ret = (pDrvInst->busVTable->fpSfrBitSet)(pDrvInst, DRV_ENCX24J600_SFR_ERXFCON, reg, DRV_ENCX24J600_CS_OP_SET_ERXFCON);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_READ_MACON2;
            }
            else
            {
                break;
            }
        }
            
        case DRV_ENCX24J600_CS_READ_MACON2:
        {
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_MACON2, DRV_ENCX24J600_CS_OP_READ_MACON2);
            if (ret != 0)
            {
                curSt->waitForMacon2Op = ret;
                if ((pDrvInst->stackCfg.ifPhyAddress.v[0] != 0) ||
                    (pDrvInst->stackCfg.ifPhyAddress.v[1] != 0) ||
                    (pDrvInst->stackCfg.ifPhyAddress.v[2] != 0) ||
                    (pDrvInst->stackCfg.ifPhyAddress.v[3] != 0) ||
                    (pDrvInst->stackCfg.ifPhyAddress.v[4] != 0) ||
                    (pDrvInst->stackCfg.ifPhyAddress.v[5] != 0))
                {
                    curSt->state = DRV_ENCX24J600_CS_WRITE_MAADR1;
                    break;
                }
                curSt->state = DRV_ENCX24J600_CS_READ_MAADR1;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_READ_MAADR1:
        {
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_MAADR1, DRV_ENCX24J600_CS_OP_READ_MAADR1);
            if (ret != 0)
            {
                curSt->waitForMaddr1Op = ret;
                curSt->state = DRV_ENCX24J600_CS_READ_MAADR2;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_READ_MAADR2:
        {
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_MAADR2, DRV_ENCX24J600_CS_OP_READ_MAADR2);
            if (ret != 0)
            {
                curSt->waitForMaddr2Op = ret;
                curSt->state = DRV_ENCX24J600_CS_READ_MAADR3;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_READ_MAADR3:
        {
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_MAADR3, DRV_ENCX24J600_CS_OP_READ_MAADR3);
            if (ret != 0)
            {
                curSt->waitForMaddr3Op = ret;
                curSt->state = DRV_ENCX24J600_CS_WAIT_FOR_MACON2;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_WAIT_FOR_MACON2:
        {
            if (_DRV_ENCX24J600_ReadSfr(pDrvInst, curSt->waitForMacon2Op, &reg, DRV_ENCX24J600_CS_OP_READ_MACON2))
            {
                if ((reg.macon2.PADCFG != 5) || (reg.macon2.TXCRCEN != 1))
                {
                    curSt->state = DRV_ENCX24J600_CS_SET_MACON2;
                    break;
                }
                curSt->state = DRV_ENCX24J600_CS_SET_PHANA;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_PHANA:
        {
            reg.value = 0x5e1;
            ret = (pDrvInst->busVTable->fpPhyWr)(pDrvInst, DRV_ENCX24J600_PHY_SFR_PHANA, reg, DRV_ENCX24J600_CS_OP_SET_PHANA_1);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_EIE;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_SET_EIE:
        {
            reg.value = 0;
            reg.eie.LINKIE = 1;
            reg.eie.PCFULIE = 1;
            reg.eie.PKTIE = 1;
            reg.eie.RXABTIE = 1;
            reg.eie.TXABTIE = 1;
            reg.eie.TXIE = 1;
            //reg.eie.INTIE = 1;
            ret = (pDrvInst->busVTable->fpSfrBitSet)(pDrvInst, DRV_ENCX24J600_SFR_EIE, reg, DRV_ENCX24J600_CS_OP_SET_EIE);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_WAIT_FOR_MAADR3;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_WAIT_FOR_MAADR3:
        {
            if (_DRV_ENCX24J600_ReadSfr(pDrvInst, curSt->waitForMaddr3Op, &reg, DRV_ENCX24J600_CS_OP_READ_MAADR3))
            {
                pDrvInst->stackParameters.ifPhyAddress.v[4] = reg.maadr3.MAADR3 & 0xff;
                pDrvInst->stackParameters.ifPhyAddress.v[5] = (reg.maadr3.MAADR3 >> 8) & 0xff;
                (*pDrvInst->busVTable->fpSfrRdResult)(pDrvInst, curSt->waitForMaddr2Op, &reg, DRV_ENCX24J600_CS_OP_READ_MAADR2);
                pDrvInst->stackParameters.ifPhyAddress.v[2] = reg.maadr2.MAADR2 & 0xff;
                pDrvInst->stackParameters.ifPhyAddress.v[3] = (reg.maadr2.MAADR2 >> 8) & 0xff;
                (*pDrvInst->busVTable->fpSfrRdResult)(pDrvInst, curSt->waitForMaddr1Op, &reg, DRV_ENCX24J600_CS_OP_READ_MAADR1);
                pDrvInst->stackParameters.ifPhyAddress.v[0] = reg.maadr1.MAADR1 & 0xff;
                pDrvInst->stackParameters.ifPhyAddress.v[1] = (reg.maadr1.MAADR1 >> 8) & 0xff;
                pDrvInst->stackParameters.processFlags = 0;
                pDrvInst->stackParameters.macType = TCPIP_MAC_TYPE_ETH;
                pDrvInst->stackParameters.linkMtu = TCPIP_MAC_LINK_MTU_ETH;
                return 1;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_WRITE_MAADR1:
        {
            reg.value = pDrvInst->stackCfg.ifPhyAddress.v[0] | (pDrvInst->stackCfg.ifPhyAddress.v[1]<<8);
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_MAADR1, reg, DRV_ENCX24J600_CS_OP_WRITE_MAADR1);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_WRITE_MAADR2;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_WRITE_MAADR2:
        {
            reg.value = pDrvInst->stackCfg.ifPhyAddress.v[2] | (pDrvInst->stackCfg.ifPhyAddress.v[3]<<8);
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_MAADR2, reg, DRV_ENCX24J600_CS_OP_WRITE_MAADR2);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_WRITE_MAADR3;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CS_WRITE_MAADR3:
        {
            reg.value = pDrvInst->stackCfg.ifPhyAddress.v[4] | (pDrvInst->stackCfg.ifPhyAddress.v[5]<<8);
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_MAADR3, reg, DRV_ENCX24J600_CS_OP_WRITE_MAADR3);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_READ_MAADR1;
            }
            break;
        }
        case DRV_ENCX24J600_CS_SET_MACON2:
        {
            reg.value = 0;
            reg.macon2.TXCRCEN = 1;
            reg.macon2.PADCFG = 0x5;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_MACON2, reg, DRV_ENCX24J600_CS_OP_SET_MACON2);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CS_SET_PHANA;
            }
            break;
        }
    }


    return 0;
}
int32_t DRV_ENCX24J600_ConfigStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    pDrvInst->mainStateInfo.initInfo.configStateInfo.state = DRV_ENCX24J600_CS_SET_EGPRDPT;
    pDrvInst->encMemGpEnd = DRV_ENCX24J600_MEM_SIZE - pDrvInst->drvCfg.rxBufferSize;
    pDrvInst->encMemRxStart = pDrvInst->encMemGpEnd;
    pDrvInst->encMemRxEnd = DRV_ENCX24J600_MEM_SIZE;
    pDrvInst->encMemGpStart = 0;
    pDrvInst->encMemUdaStart = DRV_ENCX24J600_MEM_SIZE +2;
    pDrvInst->encMemUdaEnd = DRV_ENCX24J600_MEM_SIZE +2;
    pDrvInst->gpPtrVal = 0;
    pDrvInst->rxPtrVal = pDrvInst->encMemRxStart;
    pDrvInst->txBufferRemaining = pDrvInst->encMemRxStart;

    return 0;
}
int32_t DRV_ENCX24J600_ConfigStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    return 0;
}
