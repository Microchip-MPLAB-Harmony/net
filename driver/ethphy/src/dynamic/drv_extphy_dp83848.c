/*******************************************************************************
  National DP83848 PHY API for Microchip TCP/IP Stack
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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

#include "driver/ethphy/src/drv_ethphy_local.h"

#include "driver/ethphy/src/dynamic/drv_extphy_dp83848.h"

/****************************************************************************
 *                 interface functions
 ****************************************************************************/


/****************************************************************************
 * Function:        DRV_EXTPHY_MIIConfigure
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:           pBaseObj - pointer to the PHY base object
 *                  handle - A valid open-instance handle, returned from the driver's open routine
 *                  cFlags - the requested open flags: DRV_ETHPHY_CFG_RMII/DRV_ETHPHY_CFG_MII
 *
 * Output:          DRV_ETHPHY_RES_OK - operation completed successfully
 *
 *                  DRV_ETHPHY_RES_PENDING - operation pending; the call needs to be re-issued until
 *                                    DRV_ETHPHY_RES_OK or an error reported
 *
 *
 *                  < 0         - an error occurred and the operation was aborted
 *
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function configures the PHY in one of MII/RMII operation modes.
 *
 * Note:            None
 *****************************************************************************/
static DRV_ETHPHY_RESULT DRV_EXTPHY_MIIConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, DRV_ETHPHY_CONFIG_FLAGS cFlags)
{
    union
    {
        struct
        {
            uint16_t low;
            uint16_t high;
        };
        uint32_t    w;
    }vendorData = {};

    uint16_t    phyReg = 0;
    uint16_t    miiConfPhase = 0;
    int         phyAddress = 0;

    DRV_ETHPHY_RESULT res = pBaseObj->DRV_ETHPHY_VendorDataGet(hClientObj, &vendorData.w);

    if(res < 0)
    {   // some error occurred
        return res;
    }

    miiConfPhase = vendorData.low;

    pBaseObj->DRV_ETHPHY_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);

    switch(miiConfPhase)
    {
        case 0:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_RMII_BYPASS, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // advance to the next phase
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++miiConfPhase);
            return DRV_ETHPHY_RES_PENDING;


        case 1:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // got PHY_REG_RMII_BYPASS result
            if(cFlags & DRV_ETHPHY_CFG_RMII)
            {
                phyReg |= _RMIIBYPASS_RMII_MODE_MASK;
                phyReg &= ~_RMIIBYPASS_RMII_REV1_0_MASK;      // use RMII 1.2
            }
            else
            {
                phyReg &= ~(_RMIIBYPASS_RMII_MODE_MASK);  // MII
            }

            // save the phyReg for the next state
            vendorData.high = phyReg;
            vendorData.low = miiConfPhase + 1;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;


        case 2:
            phyReg = vendorData.high;
            // update the RMII and Bypass Register
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_RMII_BYPASS, phyReg, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done    
            return DRV_ETHPHY_RES_OK;


        default:
            // shouldn't happen
            return DRV_ETHPHY_RES_OPERATION_ERR; 
    }
}


/****************************************************************************
 * Function:        DRV_EXTPHY_MDIXConfigure
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:           pBaseObj - pointer to the PHY base object
 *                  handle - A valid open-instance handle, returned from the driver's open routine
 *                  oFlags - the requested open flags: TCPIP_ETH_OPEN_MDIX_AUTO, TCPIP_ETH_OPEN_MDIX_NORM/TCPIP_ETH_OPEN_MDIX_SWAP
 *
 * Output:          DRV_ETHPHY_RES_OK - operation completed successfully
 *
 *                  DRV_ETHPHY_RES_PENDING - operation pending; the call needs to be re-issued until
 *                                    DRV_ETHPHY_RES_OK or an error reported
 *
 *
 *                  < 0         - an error occurred and the operation was aborted
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function configures the MDIX mode for the PHY.
 *
 * Note:            None
 *****************************************************************************/
static DRV_ETHPHY_RESULT DRV_EXTPHY_MDIXConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, TCPIP_ETH_OPEN_FLAGS oFlags)
{
    union
    {
        struct
        {
            uint16_t low;
            uint16_t high;
        };
        uint32_t    w;
    }vendorData = {};

    uint16_t    phyReg = 0;
    uint16_t    mdixConfPhase = 0;
    int         phyAddress = 0;

    DRV_ETHPHY_RESULT res = pBaseObj->DRV_ETHPHY_VendorDataGet(hClientObj, &vendorData.w);

    if(res < 0)
    {   // some error occurred
        return res;
    }

    mdixConfPhase = vendorData.low;

    pBaseObj->DRV_ETHPHY_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);

    switch(mdixConfPhase)
    {
        case 0:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_PHY_CTRL, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // advance to the next phase
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;


        case 1:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // got PHY_REG_PHY_CTRL result
            if(oFlags & TCPIP_ETH_OPEN_MDIX_AUTO)
            {   // enable Auto-MDIX
                phyReg |= _PHYCTRL_MDIX_EN_MASK;
            }
            else
            {   // no Auto-MDIX
                phyReg &= ~(_PHYCTRL_MDIX_EN_MASK);   // disable Auto-MDIX
                if(oFlags & TCPIP_ETH_OPEN_MDIX_SWAP)
                {
                    phyReg |= _PHYCTRL_FORCE_MDIX_MASK;    // swap
                }
                else
                {
                    phyReg &= ~(_PHYCTRL_FORCE_MDIX_MASK); // normal
                }
            }

            vendorData.low = mdixConfPhase + 1;
            vendorData.high = phyReg;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;
 
        case 2:
            phyReg = vendorData.high;
            // update the PHY_REG_PHY_CTRL Register
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_PHY_CTRL, phyReg, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done    
            return DRV_ETHPHY_RES_OK;


        default:
            // shouldn't happen
            return DRV_ETHPHY_RES_OPERATION_ERR; 
    }
}


/****************************************************************************
 * Function:        DRV_EXTPHY_SMIClockGet
 *
 * PreCondition:    None
 *
 * Input:           pBaseObj - pointer to the PHY base object
 *                  handle - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:          PHY MIIM clock, Hz
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the maximum clock frequency that the PHY can use for the MIIM transactions
 *
 * Note:            None
 *****************************************************************************/
static unsigned int DRV_EXTPHY_SMIClockGet(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE handle)
{
     return 25000000;        //  25 MHz max clock supported
}

// the DRV_ETHPHY_OBJECT

const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_National_DP83848 = 
{
    DRV_EXTPHY_MIIConfigure,
    DRV_EXTPHY_MDIXConfigure,
    DRV_EXTPHY_SMIClockGet,
    0,                          // no WOL functionality yet
};

