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
#include "drv_encx24j600_rx_packet.h"
#include "../drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"
#include "driver/encx24j600/src/dynamic/running_state/drv_encx24j600_reset_rx_state.h"
#include "system/debug/sys_debug.h"


void    DRV_ENCX24J600_RxPacketAck(TCPIP_MAC_PACKET* pkt,  const void* param)
{
     struct _DRV_ENCX24J600_DriverInfo *pDrvInst = ( struct _DRV_ENCX24J600_DriverInfo *)param;
     TCPIP_Helper_ProtectedSingleListTailAdd(&pDrvInst->rxFreePackets, (SGL_LIST_NODE*)pkt);
}

static CACHE_ALIGN DRV_ENCx24J600_RSV_EXT rsv_temp_ext;
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
            
            ret = (*pDrvInst->busVTable->fpDataRd)(pDrvInst, DRV_ENCX24J600_PTR_RXRD, ( uint8_t *)&rsv_temp_ext.rsv, sizeof(DRV_ENCx24J600_RSV));
            
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
            if (rsv_temp_ext.rsv.rxByteCount > pDrvInst->drvCfg.maxFrameSize)
            {
                pkt->retry++;
                pkt->state = DRV_ENCX24J600_RX_SET_ERXRDPTR;
                //SYS_CONSOLE_PRINT("Received a frame that is too large: %d\r\n", pkt->rsv.rxByteCount);
                break;
            }
            if (rsv_temp_ext.rsv.rxByteCount < 0x40)  // Don't accept runts
            {
                pkt->retry++;
                pkt->state = DRV_ENCX24J600_RX_SET_ERXRDPTR;
                //SYS_CONSOLE_PRINT("Received a frame that is too small: %d\r\n", pkt->rsv.rxByteCount);
                break;
            }
            if ((rsv_temp_ext.rsv.pNextPacket < pDrvInst->encMemRxStart) || (rsv_temp_ext.rsv.pNextPacket > pDrvInst->encMemRxEnd))
            {
                pkt->retry++;
                pkt->state = DRV_ENCX24J600_RX_SET_ERXRDPTR;
                //SYS_CONSOLE_PRINT("Received a Next address that is out of range: %x > %x > %x\r\n", pDrvInst->encMemRxEnd, pkt->rsv.pNextPacket, pDrvInst->encMemRxStart);
                break;
            }

            //SYS_CONSOLE_PRINT("RX Packet @ %x Len %d next %x\r\n", pDrvInst->rxPtrVal, pkt->rsv.rxByteCount, pkt->rsv.pNextPacket);
            memcpy(( uint8_t *)&(pkt->rsv),&rsv_temp_ext.rsv, sizeof(pkt->rsv));
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
            pkt->pkt->pDSeg->segLen = pkt->rsv.rxByteCount - 4 - sizeof(TCPIP_MAC_ETHERNET_HEADER); // remove FCS and Ethernet header size
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
