/*******************************************************************************
  ENC28J60 RX Packet and state machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_rx_packet.h
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
#include "drv_enc28j60_rx_packet.h"
#include "../drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"
#include "../bus/drv_enc28j60_bus.h"


bool    DRV_ENC28J60_RxPacketAck(TCPIP_MAC_PACKET* pkt,  const void* param)
{
     struct _DRV_ENC28J60_DriverInfo *pDrvInst = ( struct _DRV_ENC28J60_DriverInfo *)param;
     TCPIP_Helper_ProtectedSingleListTailAdd(&pDrvInst->rxFreePackets, (SGL_LIST_NODE*)pkt);
     return true;
}

int32_t DRV_ENC28J60_RxPacketTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_RX_PACKET_INFO *pkt)
{
    uintptr_t ret;
    uint16_t newRXRDLocation; 
    DRV_ENC28J60_BUS_RESULT busRes;


	if (pDrvInst->mainStateInfo.runningInfo.resetRxInfo.state != DRV_ENC28J60_RRX_WAIT)
    {
        return 0;
    }


	switch (pkt->state)
    {
        case DRV_ENC28J60_RX_EMPTY_PACKET:
            break;

        case DRV_ENC28J60_RX_SET_ERDPTR:
            if (pkt->retry >= 3)
            {
                pkt->retry = 0;
                pDrvInst->mainStateInfo.runningInfo.resetRxInfo.state = DRV_ENC28J60_RRX_STARTING;
                pkt->state = DRV_ENC28J60_RX_EMPTY_PACKET;
                pDrvInst->rxPtrVal = pDrvInst->encMemRxStart;
                TCPIP_Helper_ProtectedSingleListTailAdd(&pDrvInst->rxFreePackets, (SGL_LIST_NODE*)(pkt->macPkt));
                break;

            }

            if(pDrvInst->rxPtrVal >= pDrvInst->encMemTxStart)
            {   // TODO aa: Some form of error processing is needed!
                break;
            }

            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_ERDPTL, pDrvInst->rxPtrVal, false);
            if(ret != 0)
            {   // success
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_RX_WAIT_ERDPTR;
            }
            // else retry
            break;

        case DRV_ENC28J60_RX_WAIT_ERDPTR:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                pkt->state = DRV_ENC28J60_RX_READ_RSV;
            }
            else if(busRes < 0)
            {   // some error, retry
                pkt->state = DRV_ENC28J60_RX_SET_ERDPTR;
            }
            // else wait some more
            break;


        case DRV_ENC28J60_RX_READ_RSV:
            ret = (*pDrvInst->busVTable->fpDataRdStart)(pDrvInst, ( uint8_t *)&(pkt->rsv), sizeof(pkt->rsv), false);
            if(ret != 0)
            {   // success
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_RX_WAIT_RSV;
            }
            // else retry
            break;

        case DRV_ENC28J60_RX_WAIT_RSV:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error, retry
                pkt->state = DRV_ENC28J60_RX_READ_RSV;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // else wait some more
                break;
            }

            // success; process the packet RSV
            if(pkt->rsv.pNextPacket % 2 != 0 || pkt->rsv.crcError || pkt->rsv.zero || (pkt->rsv.rxByteCount > 1518) || (!pkt->rsv.rxOk))
            {
                pkt->retry++;
                pkt->state = DRV_ENC28J60_RX_SET_ERDPTR;
                break;
            }
            // valid packet
            pkt->retry = 0;
            // start reading the packet
            pkt->state = DRV_ENC28J60_RX_READ_PKT;
            // no break

        case DRV_ENC28J60_RX_READ_PKT:
            // the TCPIP_MAC_PACKET should have extra byte as a segLoadOffset! 
            ret = (*pDrvInst->busVTable->fpDataRdStart)(pDrvInst, pkt->macPkt->pDSeg->segLoad, pkt->rsv.rxByteCount, false);
            if(ret != 0)
            {   // success
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_RX_WAIT_FOR_READ;
            }
            // else retry
            // TODO aa: retry count needed!
            break;

        case DRV_ENC28J60_RX_WAIT_FOR_READ:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error, retry
                // TODO aa: retry count needed!
                pkt->state = DRV_ENC28J60_RX_READ_PKT;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }

            // success
            // TODO aa: so what happens if more packets are RX-ed and waiting in the memory?
            // It looks like ERDPT is always initialized to rxPtrVal which is initialized to ->encMemRxStart  which is fixed!!!
            pDrvInst->rxPtrVal = pkt->rsv.pNextPacket;
            pkt->macPkt->pDSeg->segLen = pkt->rsv.rxByteCount;
            pkt->macPkt->pMacLayer = pkt->macPkt->pDSeg->segLoad;
            pkt->macPkt->pNetLayer = pkt->macPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);            
            pkt->state = DRV_ENC28J60_RX_PKTDEC;
            // no break;

        case DRV_ENC28J60_RX_PKTDEC:
            ret = (*pDrvInst->busVTable->fpDecPktCnt)(pDrvInst, false); 
            if(ret != 0)
            {   // success
                pkt->operation = ret; 
                pkt->state = DRV_ENC28J60_RX_WAIT_FOR_PKTDEC;
            }
            // else retry
            break;

		case  DRV_ENC28J60_RX_WAIT_FOR_PKTDEC:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error, retry
                // TODO aa: retry count needed!
                pkt->state = DRV_ENC28J60_RX_PKTDEC;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }

            // success
            pDrvInst->mainStateInfo.runningInfo.ctrFromEnc = true;
            pDrvInst->rxPacketPending = false;
            pkt->state = DRV_ENC28J60_RX_RESET_ERXRDPTR;
            // no break;
		
    	case DRV_ENC28J60_RX_RESET_ERXRDPTR:
            // Decrement the next packet pointer before writing it into
            // the ERXRDPT registers.  This is a silicon errata workaround.

            if(pkt->rsv.pNextPacket == pDrvInst->encMemRxStart)
            {
                newRXRDLocation = pDrvInst->encMemRxEnd;
            }
            else
            { 
                newRXRDLocation = pkt->rsv.pNextPacket - 1 ;
            }


            if (newRXRDLocation % 2 == 0)
            {
                newRXRDLocation -= 1;
            }

            if (newRXRDLocation > pDrvInst->encMemRxEnd)
            {
                newRXRDLocation = pDrvInst->encMemRxEnd;
            }			 
            if (newRXRDLocation == pDrvInst->encMemRxStart)
            {
                newRXRDLocation = pDrvInst->encMemRxEnd;
            }

            // Move the receive read pointer to unwrite-protect the memory used by the
            // last packet.  The writing order is important: set the low byte first,
            // high byte last.
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_ERXRDPTL, newRXRDLocation, false);
            if(ret != 0)
            {   // success
                pkt->operation = ret;
                pkt->state = DRV_ENC28J60_RX_RESET_WAIT;		
            }
            // else retry
            break;

		case DRV_ENC28J60_RX_RESET_WAIT:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation, true);
            if(busRes < 0)
            {   // some error, retry
                // TODO aa: retry count needed!
                pkt->state = DRV_ENC28J60_RX_RESET_ERXRDPTR;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }

            // success; mark the packet as available
            if ((pkt->rsv.rxMultcast) && (pkt->rsv.rxBcast))
            {
                pkt->macPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_UNICAST;
            }
            else if (pkt->rsv.rxBcast)
            {
                pkt->macPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_BCAST;
            }
            else if (pkt->rsv.rxMultcast)
            {
                pkt->macPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_MCAST;
            }
            else
            {
                pkt->macPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_UNICAST;
            }

            TCPIP_Helper_ProtectedSingleListTailAdd(&pDrvInst->rxWaitingForPickupPackets, (SGL_LIST_NODE*)pkt->macPkt);
            DRV_ENC28J60_SetEvent(pDrvInst, TCPIP_MAC_EV_RX_DONE);
            memset(&pkt->rsv, 0, sizeof(DRV_ENC28J60_RSV));
            pkt->retry = 0;
            pDrvInst->mainStateInfo.runningInfo.nRxOkPackets ++;
            pkt->state = DRV_ENC28J60_RX_EMPTY_PACKET;
            break;

        default:
            break;
    }

    return 0;
}




int32_t DRV_ENC28J60_RxPacketEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_RX_PACKET_INFO *pkt)
{
    pkt->state = DRV_ENC28J60_RX_EMPTY_PACKET;
    memset(&pkt->rsv, 0, sizeof(DRV_ENC28J60_RSV));
    pkt->retry = 0;
    return 0;
}
int32_t DRV_ENC28J60_RxPacketExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_RX_PACKET_INFO *pkt)
{
    return 0;
}

