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
/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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
#include "drv_enc28j60_detect_state.h"
#include "../drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"

// returns 0 for ongoing op
// 1 when successful
int32_t DRV_ENC28J60_ConfigStateTask(DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_CONFIGURE_STATE_INFO * curSt = &(pDrvInst->mainStateInfo.initInfo.configStateInfo);
    DRV_ENC28J60_RegUnion reg;
    DRV_ENC28J60_PHY_RES  phyRes;
    DRV_ENC28J60_BUS_RESULT busRes;
    uint16_t  sfr16Val;
    uintptr_t ret;

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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
                pDrvInst->stackParameters.processFlags = 0;
                pDrvInst->stackParameters.macType = TCPIP_MAC_TYPE_ETH;
                pDrvInst->stackParameters.linkMtu = TCPIP_MAC_LINK_MTU_ETH;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_WRITE_MAADR1;
            }
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
            // else wait some more
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
                return 1;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_CS_SET_ECON1;
            }
            // else wait some more
            break;

        default:
            break;
    }

    return 0;
}


int32_t DRV_ENC28J60_ConfigStateEnter(DRV_ENC28J60_DriverInfo * pDrvInst)
{
    pDrvInst->mainStateInfo.initInfo.configStateInfo.state = DRV_ENC28J60_CS_SET_ERXST;

	pDrvInst->encRamForAppSize = 1024;
	pDrvInst->encRamForAppStartAdr = DRV_ENC28J60_MEM_SIZE - pDrvInst->encRamForAppSize;
	pDrvInst->encMemTxStart=pDrvInst->encRamForAppStartAdr - (1514 + 7 + 1);    // max packet + 7 bytes TSV + 1 Control Byte; Should be even!
	pDrvInst->encMemRxStart = 0x0000;
    pDrvInst->encMemRxEnd = pDrvInst->encMemTxStart - 3;  //  2 bytes gap Tx <-> RX


	//pDrvInst->encMemWrPtr= pDrvInst->encMemTxStart;

   	pDrvInst->txPtrVal = pDrvInst->encMemTxStart;
	pDrvInst->rxPtrVal = pDrvInst->encMemRxStart;

	pDrvInst->txBufferRemaining = pDrvInst->encRamForAppStartAdr - pDrvInst->encMemTxStart ;
    return 0;
}

int32_t DRV_ENC28J60_ConfigStateExit(DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}
