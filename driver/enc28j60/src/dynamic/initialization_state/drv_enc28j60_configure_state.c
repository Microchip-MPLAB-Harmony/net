/*******************************************************************************
  ENC28J60 Configure State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_configure_state.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2015-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include "drv_enc28j60_detect_state.h"
#include "../drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"

// returns 0 for ongoing op
// 1 when successful
int32_t DRV_ENC28J60_ConfigStateTask(struct S_DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_CONFIGURE_STATE_INFO * curSt = &(pDrvInst->mainStateInfo.initInfo.configStateInfo);
    DRV_ENC28J60_RegUnion reg;
    DRV_ENC28J60_PHY_RES  phyRes;
    DRV_ENC28J60_BUS_RESULT busRes;
    uint16_t  sfr16Val;
    uintptr_t ret;
    int32_t  retVal = 0; 

    switch (curSt->state)
    {
        case DRV_ENC28J60_CS_SET_ERXST:
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_ERXSTL, pDrvInst->encMemRxStart, false);
            if(ret != 0) 
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_ERXST;
            }
            // else retry
            break;           

        case DRV_ENC28J60_CS_WAIT_ERXST:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_ERXRDPT;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_ERXST;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_ERXRDPT:
            // RDPT initial value is OK???
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_ERXRDPTL, pDrvInst->encMemRxEnd, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_ERXRDPT;
            }
            // else retry
            break;            

        case DRV_ENC28J60_CS_WAIT_ERXRDPT:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_ERXND;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_ERXRDPT;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_ERXND:
            reg.value = pDrvInst->encMemRxEnd;
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_ERXNDL, pDrvInst->encMemRxEnd, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_ERXND;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_ERXND:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_ETXST;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_ERXND;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_ETXST:
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_ETXSTL, pDrvInst->encMemTxStart, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_ETXST;
            }
            break;
            // else retry

        case DRV_ENC28J60_CS_WAIT_ETXST:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_EWRPT;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_ETXST;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_EWRPT:
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_EWRPTL, pDrvInst->encMemTxStart, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_EWRPT;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_EWRPT:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_MACON1;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_EWRPT;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_MACON1:
            // Enable the receive portion of the MAC
            reg.value = 0x0;
            reg.macon1.MARXEN = 1;
            reg.macon1.TXPAUS = 1;
            reg.macon1.RXPAUS = 1;

            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENC28J60_SFR_MACON1, reg, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_MACON1;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_MACON1:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_MACON3;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_MACON1;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_MACON3:
            // Pad packets to 60 bytes, add CRC, and check Type/Length field.   
            reg.value = 0x00;
            reg.macon3.FULDPX = 0; 
            reg.macon3.FRMLNEN = 1;
            reg.macon3.TXCRCEN = 1;
            reg.macon3.PADCFG = 1;

            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENC28J60_SFR_MACON3, reg, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_MACON3;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_MACON3:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_MABBIPG;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_MACON3;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_MABBIPG:
            // Set the back-to-back inter-packet gap time to IEEE specified
            // requirements.  The meaning of the MABBIPG value changes with the duplex
            // state, so it must be updated in this function.
            // In full duplex, 0x15 represents 9.6us; 0x12 is 9.6us in half duplex
            reg.value = 0x12;   
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENC28J60_SFR_MABBIPG, reg, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_MABBIPG;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_MABBIPG:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_MACON4;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_MABBIPG;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_MACON4:
            // Allow infinite deferals if the medium is continuously busy
            // (do not time out a transmission if the half duplex medium is
            // completely saturated with other people's data)
            reg.value = 0x0;
            reg.macon4.DEFER = 1;

            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENC28J60_SFR_MACON4, reg, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_MACON4;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_MACON4:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_MACLCON2;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_MACON4;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_MACLCON2:
            // Late collisions occur beyond 63+8 bytes (8 bytes for preamble/start of frame delimiter)
            // 55 is all that is needed for IEEE 802.3, but ENC28J60 B5 errata for improper link pulse
            // collisions will occur less often with a larger number.
            reg.value = 0x63;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENC28J60_SFR_MACLCON2, reg, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_MACLCON2;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_MACLCON2:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_MAIPG;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_MACLCON2;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_MAIPG:
            // Set non-back-to-back inter-packet gap to 9.6us.  The back-to-back
            // inter-packet gap (MABBIPG) is set by MACSetDuplex() which is called
            // later.
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_MAIPGL, 0x0C12, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_MAIPG;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_MAIPG:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_MAMXFL;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_MAIPG;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_MAMXFL:
            // Set the maximum packet size which the controller will accept
            // 1518 (0x5EE)is the IEEE 802.3 specified limit
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_MAMXFLL, pDrvInst->drvCfg.maxFrameSize, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_MAMXFL;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_MAMXFL:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_WRITE_MAADR5;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_MAMXFL;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_WRITE_MAADR5:
            sfr16Val = pDrvInst->stackParameters.ifPhyAddress.v[4] | ((uint16_t)pDrvInst->stackParameters.ifPhyAddress.v[5] << 8);
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_MAADR5, sfr16Val, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_MAADR5;
            }
            break;

        case DRV_ENC28J60_CS_WAIT_MAADR5:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_WRITE_MAADR3;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_WRITE_MAADR5;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_WRITE_MAADR3:
            sfr16Val = pDrvInst->stackParameters.ifPhyAddress.v[2] | ((uint16_t)pDrvInst->stackParameters.ifPhyAddress.v[3] << 8);
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_MAADR3, sfr16Val, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_MAADR3;
            }
            break;             

        case DRV_ENC28J60_CS_WAIT_MAADR3:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_WRITE_MAADR1;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_WRITE_MAADR3;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_WRITE_MAADR1:
            sfr16Val = pDrvInst->stackParameters.ifPhyAddress.v[0] | ((uint16_t)pDrvInst->stackParameters.ifPhyAddress.v[1] << 8);
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_MAADR1, sfr16Val, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_MAADR1;
            }
            break;

        case DRV_ENC28J60_CS_WAIT_MAADR1:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_ECOCON;
                pDrvInst->stackParameters.processFlags = TCPIP_MAC_PROCESS_FLAG_NONE;
                pDrvInst->stackParameters.macType = TCPIP_MAC_TYPE_ETH;
                pDrvInst->stackParameters.linkMtu = TCPIP_MAC_LINK_MTU_ETH;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_WRITE_MAADR1;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_ECOCON:
            // Disable the CLKOUT output to reduce EMI generation   
            reg.value = 0x00;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENC28J60_SFR_ECOCON, reg, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_ECOCON;
            }
            break;

        case DRV_ENC28J60_CS_WAIT_ECOCON:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_PHCON2;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_ECOCON;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_PHCON2:
            // Disable half duplex loopback in PHY. 
            phyRes = (*pDrvInst->busVTable->fpPhyWrStart)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHCON2, 1);
            if(phyRes == DRV_ENC28J60_PHY_RES_PENDING)
            {
                curSt->state = DRV_ENC28J60_CS_WAIT_PHCON2;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_PHCON2:
            phyRes = (*pDrvInst->busVTable->fpPhyWr)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHCON2);
            if(phyRes == DRV_ENC28J60_PHY_RES_OK)
            {
                curSt->state = DRV_ENC28J60_CS_SET_PHLCON;
            }
            else if(phyRes < 0)
            {   // error: retry
                curSt->state = DRV_ENC28J60_CS_SET_PHCON2;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_PHLCON:
            phyRes = (*pDrvInst->busVTable->fpPhyWrStart)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHLCON, 0x3472);
            if(phyRes == DRV_ENC28J60_PHY_RES_PENDING)
            {
                curSt->state = DRV_ENC28J60_CS_WAIT_PHLCON;
            }
            // else retry
            break;
            
        case DRV_ENC28J60_CS_WAIT_PHLCON:
            phyRes = (*pDrvInst->busVTable->fpPhyWr)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHLCON);
            if(phyRes == DRV_ENC28J60_PHY_RES_OK)
            {
                curSt->state = DRV_ENC28J60_CS_SET_PHCON1;
            }
            else if(phyRes < 0)
            {   // error: retry
                curSt->state = DRV_ENC28J60_CS_SET_PHLCON;
            }
            else
            {
                // else wait some more
            }
            break;
            
        case DRV_ENC28J60_CS_SET_PHCON1:
            // Set the MAC and PHY into the proper duplex state
            reg.value = 0;
            reg.phcon1.PDPXMD = 0; 
            phyRes = (*pDrvInst->busVTable->fpPhyWrStart)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHCON1, reg.value);
            if(phyRes == DRV_ENC28J60_PHY_RES_PENDING)
            {
                curSt->state = DRV_ENC28J60_CS_WAIT_PHCON1;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_PHCON1:
            phyRes = (*pDrvInst->busVTable->fpPhyWr)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHCON1);
            if(phyRes == DRV_ENC28J60_PHY_RES_OK)
            {
                curSt->state = DRV_ENC28J60_CS_SET_PHIE;
            }
            else if(phyRes < 0)
            {   // error: retry
                curSt->state = DRV_ENC28J60_CS_SET_PHCON1;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_PHIE:
            reg.value = 0;
            reg.phie.PGEIE = 1;
            reg.phie.PLNKIE= 1;
            phyRes = (*pDrvInst->busVTable->fpPhyWrStart)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHIE, reg.value);
            if(phyRes == DRV_ENC28J60_PHY_RES_PENDING)
            {
                curSt->state = DRV_ENC28J60_CS_WAIT_PHIE;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_PHIE:
            phyRes = (*pDrvInst->busVTable->fpPhyWr)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHIE);
            if(phyRes == DRV_ENC28J60_PHY_RES_OK)
            {
                curSt->state = DRV_ENC28J60_CS_SET_EIE;
            }
            else if(phyRes < 0)
            {   // error: retry
                curSt->state = DRV_ENC28J60_CS_SET_PHIE;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_EIE:
            reg.value = 0x5b;   // PKTIE, LINKIE, TXIE, TXERIE, RXERIE
            ret = (*pDrvInst->busVTable->fpSfrBitSet)(pDrvInst, DRV_ENC28J60_SFR_EIE, reg, false);
            if(ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_EIE;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_EIE:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_ECON2;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_EIE;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_ECON2:
            reg.value = 0x80; // ECON2:AUTOINC bit7 is required to be set for AutoInc of the Read pointer 
            ret = (*pDrvInst->busVTable->fpSfrBitSet)(pDrvInst, DRV_ENC28J60_SFR_ECON2, reg, false); 
            if (ret != 0)
            {   // success
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_ECON2;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_ECON2:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_SET_ECON1;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_ECON2;
            }
            else
            {
                // else wait some more
            }
            break;

        case DRV_ENC28J60_CS_SET_ECON1:
            reg.value = 0;
            reg.econ1.RXEN = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitSet)(pDrvInst, DRV_ENC28J60_SFR_ECON1, reg, false); 

            if (ret != 0)
            {   // success
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CS_WAIT_ECON1;
            }
            break;

        case DRV_ENC28J60_CS_WAIT_ECON1:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_CS_DONE;
                retVal = 1;
                break;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_ECON1;
            }
            else
            {
                // else wait some more
            }
            break;

        default:
            // do nothing
            break;
    }

    return retVal;
}


int32_t DRV_ENC28J60_ConfigStateEnter(struct S_DRV_ENC28J60_DriverInfo * pDrvInst)
{
    pDrvInst->mainStateInfo.initInfo.configStateInfo.state = DRV_ENC28J60_CS_SET_ERXST;

    pDrvInst->encRamForAppSize = 1024;
    pDrvInst->encRamForAppStartAdr = DRV_ENC28J60_MEM_SIZE - pDrvInst->encRamForAppSize;
    pDrvInst->encMemTxStart = pDrvInst->encRamForAppStartAdr - (1514U + 7U + 1U);    // max packet + 7 bytes TSV + 1 Control Byte; Should be even!
    pDrvInst->encMemRxStart = 0x0000;
    pDrvInst->encMemRxEnd = pDrvInst->encMemTxStart - 3U;  //  2 bytes gap Tx <-> RX


    //pDrvInst->encMemWrPtr= pDrvInst->encMemTxStart;

    pDrvInst->txPtrVal = pDrvInst->encMemTxStart;
    pDrvInst->rxPtrVal = pDrvInst->encMemRxStart;

    pDrvInst->txBufferRemaining = pDrvInst->encRamForAppStartAdr - pDrvInst->encMemTxStart ;
    return 0;
}

int32_t DRV_ENC28J60_ConfigStateExit(struct S_DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}
