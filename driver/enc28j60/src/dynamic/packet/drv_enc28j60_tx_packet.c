/*******************************************************************************
  ENC28J60 TX Packet and State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_tx_packet.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.
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
#include "drv_enc28j60_tx_packet.h"
#include "../drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"
#include "../drv_enc28j60_ds_defs.h"
#include "../running_state/drv_enc28j60_running_state.h"
#include "../bus/drv_enc28j60_bus.h"

int32_t DRV_ENC28J60_TxPacketTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_TX_PACKET_INFO *pkt)
{
  
    uintptr_t ret;
    DRV_ENC28J60_RegUnion reg;
    uint16_t count;
    DRV_ENC28J60_BUS_RESULT busRes;
    TCPIP_MAC_RES macTxRes;
   
    switch (pkt->state)
    {
        case DRV_ENC28J60_TP_NO_PKT_STATE:
            break;

        case DRV_ENC28J60_TP_WRITE_TXST:
            reg.value = pDrvInst->encMemTxStart;
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_ETXSTL, pDrvInst->encMemTxStart, false);
            if (ret != 0)
            {
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_TP_WAIT_TXST;
            }
            // else retry
            break;

		case DRV_ENC28J60_TP_WAIT_TXST:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error; retry
                // TODO aa: retry count is needed!
                pkt->state = DRV_ENC28J60_TP_WRITE_TXST;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success
            pkt->state = DRV_ENC28J60_TP_SET_ETXND;
            // no break

		case DRV_ENC28J60_TP_SET_ETXND:
            count = 0;

            // TODO aa: there should be a TCPIP packet function available
            pkt->pDSeg = pkt->macPkt->pDSeg;
            while (pkt->pDSeg != NULL)
            {
                count += pkt->pDSeg->segLen;
                pkt->pDSeg = pkt->pDSeg->next;
            }

            pkt->pktLen = count;
            // TODO aa: note that the packet ends at count + 1 because of the control byte
            // but ETXND points to the last byte of the message, so offset is count
            reg.value = pDrvInst->encMemTxStart + count;

            if((reg.value >= pDrvInst->encRamForAppStartAdr) || (reg.value < pDrvInst->encMemTxStart))
            {
                DRV_ENC28J60_Assert(false, __func__, __LINE__);
                break;  // TODO aa: error processing needed!
            }

            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_ETXNDL, reg.value, false);
            if (ret != 0)
            {
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_TP_WAIT_ETXND;
            }
            // else retry
            break;

		case DRV_ENC28J60_TP_WAIT_ETXND:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error; retry
                // TODO aa: retry count is needed!
                pkt->state = DRV_ENC28J60_TP_SET_ETXND;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success
            pkt->state = DRV_ENC28J60_TP_SET_WRPT;
            // no break;

        case DRV_ENC28J60_TP_SET_WRPT:
            pkt->gpPtr = pDrvInst->encMemTxStart;
            pkt->pDSeg = pkt->macPkt->pDSeg;

            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_EWRPTL, pDrvInst->encMemTxStart, false);
            if (ret != 0)
            {   // success
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_TP_WAIT_WRPT;
            }
            // else retry
            break;

        case DRV_ENC28J60_TP_WAIT_WRPT:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error; retry
                // TODO aa: retry count is needed!
                pkt->state = DRV_ENC28J60_TP_SET_WRPT;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success
            pkt->state = DRV_ENC28J60_TP_SEND_PKT;
            // no break

        case DRV_ENC28J60_TP_SEND_PKT:
            // transmit the packet
            macTxRes = (*pDrvInst->busVTable->fpPktWr)(pDrvInst, pkt->macPkt, &pkt->operation, false);
            if(macTxRes == TCPIP_MAC_RES_PENDING)
            {   // retry needed
                break;
            }
            // TODO aa: actually it can happen if the packet is larger than the space reserved for ENC copy buffer!
            // TODO aa: if failed, packet needs to be discarded somehow and acknowledged!
            DRV_ENC28J60_Assert(macTxRes != TCPIP_MAC_RES_PACKET_ERR, __func__, __LINE__);

            pkt->state = DRV_ENC28J60_TP_SEND_PKT_WAIT;
            break;

		case DRV_ENC28J60_TP_SEND_PKT_WAIT:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error; retry
                // TODO aa: retry count is needed!
                pkt->state = DRV_ENC28J60_TP_SEND_PKT;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // successfully sent the packet to ENC
            DRV_ENC28J60_AddGpData(pDrvInst, pkt->pktLen);  // TODO aa: what is this good for???
            pkt->gpPtr = pDrvInst->encMemTxStart;

            // before transmitting the packet, take care of the TX stall - errata
            pkt->state = DRV_ENC28J60_TP_SET_TXRST;
            break;

        case DRV_ENC28J60_TP_READ_EIR:
            ret = (pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENC28J60_SFR_EIR, false);
            if(ret != 0)
            {   // success
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_TP_WAIT_EIR;
            }
            // else retry
            break;

		case DRV_ENC28J60_TP_WAIT_EIR:
            busRes = (*pDrvInst->busVTable->fpSfrRdResult)(pDrvInst, pkt->operation, &reg, true);
            if(busRes < 0)
            {   // some error; retry
                // TODO aa: retry count is needed!
                pkt->state = DRV_ENC28J60_TP_READ_EIR;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success
            if(reg.eir.TXERIF == 0)
            {   // no error reported, so no TX reset needed
                // go ahead and wait to transmit the packet
                pkt->state = DRV_ENC28J60_TP_READY_FOR_CTTX;
                break;
            }
            // need to reset the TX state machine
            pkt->state = DRV_ENC28J60_TP_SET_TXRST;
            // no break

		case DRV_ENC28J60_TP_SET_TXRST:
            reg.value = 0;
            reg.econ1.TXRST = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitSet)(pDrvInst, DRV_ENC28J60_SFR_ECON1, reg, false);
            if (ret != 0)
            {
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_TP_WAIT_SET_TXRST;
            }
            // else retry
            break;
 
		case DRV_ENC28J60_TP_WAIT_SET_TXRST:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error; retry
                // TODO aa: retry count is needed!
                pkt->state = DRV_ENC28J60_TP_SET_TXRST;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success
            pkt->state = DRV_ENC28J60_TP_CLR_TXRST;
            // no break;

		case DRV_ENC28J60_TP_CLR_TXRST:
            reg.value = 0;
            reg.econ1.TXRST = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitClr)(pDrvInst, DRV_ENC28J60_SFR_ECON1, reg, false);
            if (ret != 0)
            {
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_TP_WAIT_CLR_TXRST;
            }
            // else retry
            break;

		case DRV_ENC28J60_TP_WAIT_CLR_TXRST:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error; retry
                // TODO aa: retry count is needed!
                pkt->state = DRV_ENC28J60_TP_CLR_TXRST;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success
            pkt->state = DRV_ENC28J60_TP_CLR_TXERIF;
            // no break

		case DRV_ENC28J60_TP_CLR_TXERIF:
            reg.value = 0;
            reg.eir.TXERIF = 1;
            reg.eir.TXIF = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitClr)(pDrvInst, DRV_ENC28J60_SFR_EIR, reg, false);
            if (ret != 0)
            {
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_TP_WAIT_CLR_TXERIF;
            }
            // else retry
            break;

		case DRV_ENC28J60_TP_WAIT_CLR_TXERIF:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error; retry
                // TODO aa: retry count is needed!
                pkt->state = DRV_ENC28J60_TP_CLR_TXERIF;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success; can transmit the packet
            pkt->state = DRV_ENC28J60_TP_READY_FOR_CTTX;
            break;

        case DRV_ENC28J60_TP_READY_FOR_CTTX:
            pkt->state = DRV_ENC28J60_TP_WAIT_FOR_CTTX;
            pDrvInst->mainStateInfo.runningInfo.ctTx = true;
            pDrvInst->mainStateInfo.runningInfo.ctsToEnc = true;
            break;

        case DRV_ENC28J60_TP_RQ_PKT_TX:
            // Transmit packet
            reg.value = 0;
            reg.econ1.TXRTS = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitSet)(pDrvInst, DRV_ENC28J60_SFR_ECON1, reg, false);
            if (ret != 0)
            {   // success
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_TP_WAIT_PKT_TX;
            }
            // else retry
            break;

		case DRV_ENC28J60_TP_WAIT_PKT_TX:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error; retry
                // TODO aa: retry count is needed!
                pkt->state = DRV_ENC28J60_TP_RQ_PKT_TX;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success; transmitted the packet

            pkt->state = DRV_ENC28J60_TP_WAIT_FOR_COMPLETE;
            break;

		case DRV_ENC28J60_TP_WAIT_FOR_COMPLETE:
            break;

		case DRV_ENC28J60_TP_RST_EIR:
            if(pkt->macPkt != NULL)
            {   // update the packet status
                // TODO aa: the packet should be discarded from the beginning if the link was down!!!
                if (!pDrvInst->mainStateInfo.runningInfo.chkStaInfo.linkState)
                {
                    pkt->macPkt->ackRes = TCPIP_MAC_PKT_ACK_LINK_DOWN;
                }
                else
                {
                    pkt->macPkt->ackRes = TCPIP_MAC_PKT_ACK_TX_OK;
                }

                pkt->macPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
                (*pkt->macPkt->ackFunc)(pkt->macPkt, pkt->macPkt->ackParam);

                DRV_ENC28J60_SetEvent(pDrvInst, TCPIP_MAC_EV_TX_DONE);
                pkt->macPkt = NULL;
                DRV_ENC28J60_TxAck(pDrvInst, pkt->pktLen);    // TODO aa: ???
                pDrvInst->mainStateInfo.runningInfo.nTxOkPackets ++;
                pkt->gpPtr = DRV_ENC28J60_MEM_SIZE;
            }

            // clear the TX int flags
            pkt->state = DRV_ENC28J60_TP_CLR_EIR;
            // no break

		case DRV_ENC28J60_TP_CLR_EIR:
            reg.value = 0;
            reg.eir.TXIF = 1;
            reg.eir.TXERIF = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitClr)(pDrvInst, DRV_ENC28J60_SFR_EIR, reg, false);
            if (ret != 0)
            {
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_TP_WAIT_CLR_EIR;
            }
            // else retry
            break;

        case DRV_ENC28J60_TP_WAIT_CLR_EIR:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error; retry
                // TODO aa: retry count is needed!
                pkt->state = DRV_ENC28J60_TP_CLR_EIR;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success; done
            pkt->state = DRV_ENC28J60_TP_NO_PKT_STATE;
            break;


        case DRV_ENC28J60_TP_RESET_TX:
            // special state for TX logic reset
            pkt->state = DRV_ENC28J60_TP_SET_TXRST;
            break;

        case DRV_ENC28J60_TP_RESET_DONE:
            pkt->state = DRV_ENC28J60_TP_NO_PKT_STATE;
            break;

        default:
            break;

    }
    return 0;
}


 
int32_t DRV_ENC28J60_TxPacketEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_TX_PACKET_INFO *pkt)
{
    pkt->state = DRV_ENC28J60_TP_NO_PKT_STATE;
    pkt->macPkt = NULL;
    pkt->gpPtr = DRV_ENC28J60_MEM_SIZE;
    return 0;
}

int32_t DRV_ENC28J60_TxPacketExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_TX_PACKET_INFO *pkt)
{
    return 0;
}


