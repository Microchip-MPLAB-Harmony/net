/*******************************************************************************
  ENCx24J600 RX Packet and state machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_rx_packet.h
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
#include "drv_encx24j600_rx_packet.h"
#include "../drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"
#include "driver/encx24j600/src/dynamic/running_state/drv_encx24j600_reset_rx_state.h"
#include "system/console/sys_console.h"


bool    DRV_ENCX24J600_RxPacketAck(TCPIP_MAC_PACKET* pkt,  const void* param)
{
     struct _DRV_ENCX24J600_DriverInfo *pDrvInst = ( struct _DRV_ENCX24J600_DriverInfo *)param;
     TCPIP_Helper_ProtectedSingleListTailAdd(&pDrvInst->rxFreePackets, (SGL_LIST_NODE*)pkt);
     return true;
}


int32_t DRV_ENCX24J600_RxPacketTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_RX_PACKET_INFO *pkt)
{
    uintptr_t ret;
    DRV_ENCX24J600_RegUnion reg = {0};
    if (pDrvInst->mainStateInfo.runningInfo.resetRxInfo.state != DRV_ENCX24J600_RRX_WAIT)
    {
        return 0;
    }
    switch (pkt->state)
    {
        case DRV_ENCX24J600_RX_EMPTY_PACKET:
            break;
        case DRV_ENCX24J600_RX_SET_ERXRDPTR:
        {
            //SYS_CONSOLE_PRINT("Trying RX @ %x\r\n", pDrvInst->rxPtrVal);
            if (pkt->retry >= 3)
            {
                SYS_CONSOLE_PRINT("Received an invalid frame size %d address %x next %x\r\n", pkt->rsv.rxByteCount, pDrvInst->rxPtrVal, pkt->rsv.pNextPacket);
                pkt->retry = 0;
                pDrvInst->mainStateInfo.runningInfo.resetRxInfo.state = DRV_ENCX24J600_RRX_STARTING;
                pkt->state = DRV_ENCX24J600_RX_EMPTY_PACKET;
                pDrvInst->rxPtrVal = pDrvInst->encMemRxStart;
                TCPIP_Helper_ProtectedSingleListTailAdd(&pDrvInst->rxFreePackets, (SGL_LIST_NODE*)(pkt->pkt));
                break;
            }
            ret = (*pDrvInst->busVTable->fpPtrWr)(pDrvInst, DRV_ENCX24J600_PTR_RXRD, pDrvInst->rxPtrVal, DRV_ENCX24J600_RP_OP_SET_RXRDPTR);
            if (ret == 0)
            {
                break;
            }
            pkt->state = DRV_ENCX24J600_RX_READ_RSV;
        }
        case DRV_ENCX24J600_RX_READ_RSV:
        {
            ret = (*pDrvInst->busVTable->fpDataRd)(pDrvInst, DRV_ENCX24J600_PTR_RXRD, ( uint8_t *)&(pkt->rsv), sizeof(DRV_ENCx24J600_RSV));
            if (ret == 0)
            {
                break;
            }
            pkt->operation = ret;
            pkt->state = DRV_ENCX24J600_RX_WAIT_FOR_RSV;
        }
        case DRV_ENCX24J600_RX_WAIT_FOR_RSV:
        {
            if ((*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation) == DRV_ENCX24J600_BR_PENDING)
            {
                break;
            }
            if (pkt->rsv.rxByteCount > pDrvInst->drvCfg.maxFrameSize)
            {
                pkt->retry++;
                pkt->state = DRV_ENCX24J600_RX_SET_ERXRDPTR;
                //SYS_CONSOLE_PRINT("Received a frame that is too large: %d\r\n", pkt->rsv.rxByteCount);
                break;
            }
            if (pkt->rsv.rxByteCount < 0x40)  // Don't accept runts
            {
                pkt->retry++;
                pkt->state = DRV_ENCX24J600_RX_SET_ERXRDPTR;
                //SYS_CONSOLE_PRINT("Received a frame that is too small: %d\r\n", pkt->rsv.rxByteCount);
                break;
            }
            if ((pkt->rsv.pNextPacket < pDrvInst->encMemRxStart) || (pkt->rsv.pNextPacket > pDrvInst->encMemRxEnd))
            {
                pkt->retry++;
                pkt->state = DRV_ENCX24J600_RX_SET_ERXRDPTR;
                //SYS_CONSOLE_PRINT("Received a Next address that is out of range: %x > %x > %x\r\n", pDrvInst->encMemRxEnd, pkt->rsv.pNextPacket, pDrvInst->encMemRxStart);
                break;
            }

            //SYS_CONSOLE_PRINT("RX Packet @ %x Len %d next %x\r\n", pDrvInst->rxPtrVal, pkt->rsv.rxByteCount, pkt->rsv.pNextPacket);
            pDrvInst->rxPtrVal = pkt->rsv.pNextPacket;
            pkt->state = DRV_ENCX24J600_RX_READ_PKT;
            pkt->retry = 0;
        }
        case DRV_ENCX24J600_RX_READ_PKT:
        {
            ret = (*pDrvInst->busVTable->fpDataRd)(pDrvInst, DRV_ENCX24J600_PTR_RXRD, pkt->pkt->pDSeg->segLoad, pkt->rsv.rxByteCount);
            if (ret == 0)
            {
                break;
            }
            pkt->operation = ret;
            pkt->state = DRV_ENCX24J600_RX_WAIT_FOR_READ;
            pkt->pkt->pDSeg->segLen = pkt->rsv.rxByteCount;
        }
        case DRV_ENCX24J600_RX_WAIT_FOR_READ:
        {
            if ((*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation) == DRV_ENCX24J600_BR_PENDING)
            {
                break;
            }
            pkt->pkt->pMacLayer = pkt->pkt->pDSeg->segLoad;
            pkt->pkt->pNetLayer = pkt->pkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);
            pkt->state = DRV_ENCX24J600_RX_SET_ERXTAIL;
        }
        case DRV_ENCX24J600_RX_SET_ERXTAIL:
        {
            uint16_t rxTail = pkt->rsv.pNextPacket - 2;
            if (rxTail < pDrvInst->encMemRxStart)
            {
                rxTail = pDrvInst->encMemRxEnd - (pDrvInst->encMemRxStart - rxTail);
            }
            reg.value = 0;
            reg.erxtail.ERXTAIL = rxTail;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_ERXTAIL, reg, DRV_ENCX24J600_RP_OP_SET_RXTAIL);
            //SYS_CONSOLE_PRINT("Setting RXTail to %x\r\n", rxTail);
            if (ret == 0)
            {
                break;
            }
            pkt->state = DRV_ENCX24J600_RX_SEND_PKTDEC;
        }
        case DRV_ENCX24J600_RX_SEND_PKTDEC:
        {
            ret = (*pDrvInst->busVTable->fpDecPktCnt)(pDrvInst);
            if (ret == 0)
            {
                break;
            }
            pkt->state = DRV_ENCX24J600_RX_RST_EIR;
            pDrvInst->mainStateInfo.runningInfo.ctrFromEnc = true;
        }
        case DRV_ENCX24J600_RX_RST_EIR:
        {
            reg.value = 0;
            reg.eir.RXABTIF = 1;
            reg.eir.PCFULIF = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitClr)(pDrvInst, DRV_ENCX24J600_SFR_EIR, reg, DRV_ENCX24J600_TP_OP_RST_EIR);
            if (ret == 0)
            {
                break;
            }
            // Moved down to past the reset interrupt state, since the SPI driver might not keep up.  MH-4456
            pDrvInst->rxPacketPending = false;
            pDrvInst->rxPacketJustComplete = true;
            if (pkt->rsv.rxBcast)
            {
                pkt->pkt->pktFlags |= TCPIP_MAC_PKT_FLAG_BCAST;
            }
            else if (pkt->rsv.rxMultcast)
            {
                pkt->pkt->pktFlags |= TCPIP_MAC_PKT_FLAG_MCAST;
            }
            else
            {
                pkt->pkt->pktFlags |= TCPIP_MAC_PKT_FLAG_UNICAST;
            }
            TCPIP_Helper_ProtectedSingleListTailAdd(&pDrvInst->rxWaitingForPickupPackets, (SGL_LIST_NODE*)pkt->pkt);

            DRV_ENCX24J600_SetEvent(pDrvInst, TCPIP_MAC_EV_RX_DONE);
            pkt->state = DRV_ENCX24J600_RX_EMPTY_PACKET;
            memset(&pkt->rsv, 0, sizeof(DRV_ENCx24J600_RSV));
            pkt->retry = 0;
            pDrvInst->mainStateInfo.runningInfo.nRxOkPackets ++;


        }
        case DRV_ENCX24J600_RX_WAIT_FOR_PICKUP:
            break;
        case DRV_ENCX24J600_RX_WAIT_FOR_ACK:
            break;
        case DRV_ENCX24J600_RX_RESET_ERXRDPTR:
            break;
    }

    return 0;
}
int32_t DRV_ENCX24J600_RxPacketEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_RX_PACKET_INFO *pkt)
{
    pkt->state = DRV_ENCX24J600_RX_EMPTY_PACKET;
    memset(&pkt->rsv, 0, sizeof(DRV_ENCx24J600_RSV));
    pkt->retry = 0;
    return 0;
}
int32_t DRV_ENCX24J600_RxPacketExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_RX_PACKET_INFO *pkt)
{
    return 0;
}
