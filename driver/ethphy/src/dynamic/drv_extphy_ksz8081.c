/*******************************************************************************
  KSZ8081PHY API for Microchip TCP/IP Stack
*******************************************************************************/

/*
Copyright (C) 2017-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


#include "driver/ethphy/src/drv_ethphy_local.h"

#include "driver/ethphy/drv_extphy_ksz8081.h"
#include "drv_extphy_ksz8081_priv.h"

/****************************************************************************
 *                 interface functions
 ****************************************************************************/


/****************************************************************************
 * Function:        DRV_EXTPHY_MIIConfigure
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:           handle - A valid open-instance handle, returned from the driver's open routine
 *                  cFlags - the requested configuration flags: DRV_ETHPHY_CFG_RMII/DRV_ETHPHY_CFG_MII
 *
 * Output:          DRV_ETHPHY_RES_OK - operation completed successfully
 *
 *                  DRV_ETHPHY_RES_PENDING - operation pending; the call needs to be re-issued until
 *                                    DRV_ETHPHY_RES_OK or an error reported
 *
 *
 *                  < 0         - an error occurred and the operation was aborted
 *
 * Side Effects:    None
 *
 * Overview:        This function configures the PHY in one of MII/RMII operation modes.
 *
 * Note:            
 *****************************************************************************/
static DRV_ETHPHY_RESULT DRV_EXTPHY_MIIConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, DRV_ETHPHY_CONFIG_FLAGS cFlags)
{
#if (KSZ8081RNB == 0)
    return (((uint8_t)cFlags & (uint8_t)DRV_ETHPHY_CFG_RMII) != 0U) ? DRV_ETHPHY_RES_CFG_ERR : DRV_ETHPHY_RES_OK;
#else
    return (((uint8_t)cFlags & (uint8_t)DRV_ETHPHY_CFG_RMII) != 0U) ? DRV_ETHPHY_RES_OK : DRV_ETHPHY_RES_CFG_ERR;
#endif  // (KSZ8081RNB == 0)
}


/****************************************************************************
 * Function:        DRV_EXTPHY_MDIXConfigure
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:           handle - A valid open-instance handle, returned from the driver's open routine
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
        uint32_t    w;
        struct
        {
            uint16_t low;
            uint16_t high;
        };
    } vendorData = { 0 };

    KSZ8081_PhyControl2Bits_t phyCtrl2;
    uint16_t    phyReg = 0;
    uint16_t    mdixConfPhase = 0;
    int         phyAddress = 0;

    DRV_ETHPHY_RESULT res = pBaseObj->phy_VendorDataGet(hClientObj, &vendorData.w);

    if((int)res < 0)
    {   // some error occurred
        return res;
    }

    mdixConfPhase = vendorData.low;

    (void)pBaseObj->phy_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);

    switch(mdixConfPhase)
    {
        case 0U:
            res = pBaseObj->phy_VendorSMIReadStart(hClientObj, PHY_REG_PHY_CONTROL_2, phyAddress);
            if((int)res < 0)
            {   // some error
                // abort
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                // Do Nothing
            }
            else
            {                
                // advance to the next phase
                mdixConfPhase++;
                (void)pBaseObj->phy_VendorDataSet(hClientObj, mdixConfPhase);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;
            
        case 1U:
            res = pBaseObj->phy_VendorSMIReadResultGet(hClientObj, &phyReg);
            if((int)res < 0)
            {   // some error
                // abort
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                // Do Nothing
            }
            else
            {
                // got PHY_REG_PHY_CONTROL_2 result
                phyCtrl2.w = phyReg;
                if(((uint32_t)oFlags & (uint32_t)TCPIP_ETH_OPEN_MDIX_AUTO) != 0U)
                {   // enable Auto-MDIX
                    phyCtrl2.PAIR_SWAP_DISABLE = 1;
                }
                else
                {   // no Auto-MDIX
                    phyCtrl2.PAIR_SWAP_DISABLE = 0;  // disable Auto-MDIX
                    if(((uint32_t)oFlags & (uint32_t)TCPIP_ETH_OPEN_MDIX_SWAP) != 0U)
                    {
                        phyCtrl2.MDI_X_SELECT = 1;    // swap
                    }
                    else
                    {
                        phyCtrl2.MDI_X_SELECT = 0;  // normal   
                    }
                }

                vendorData.low = mdixConfPhase + 1U;
                vendorData.high = phyCtrl2.w;
                (void)pBaseObj->phy_VendorDataSet(hClientObj, vendorData.w);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;
            
        case 2U:
            phyReg = vendorData.high;
            // update the PHY_REG_PHY_CONTROL_2 Register
            res = pBaseObj->phy_VendorSMIWriteStart(hClientObj, PHY_REG_PHY_CONTROL_2, phyReg, phyAddress);
            if((int)res < 0)
            {   // some error
                // abort
            }
            else if((int)res == (int8_t)DRV_ETHPHY_RES_PENDING)
            {   // retry
                // Do Nothing
            }
            else
            {
                // done    
                res = DRV_ETHPHY_RES_OK;
            }
            break;
            
        default:
            // shouldn't happen
            res = DRV_ETHPHY_RES_OPERATION_ERR; 
            break;
    }
    return res;
}


/****************************************************************************
 * Function:        DRV_EXTPHY_SMIClockGet
 *
 * PreCondition:    None
 *
 * Input:           handle - A valid open-instance handle, returned from the driver's open routine
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
    return 10000000;        //  10 MHz max clock supported
}

// the DRV_ETHPHY_OBJECT

const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_KSZ8081 = 
{
    .miiConfigure = &DRV_EXTPHY_MIIConfigure,
    .mdixConfigure = &DRV_EXTPHY_MDIXConfigure,
    .smiClockGet = &DRV_EXTPHY_SMIClockGet,
    .wolConfigure = NULL,                   // no WOL functionality yet
    .phyDetect = NULL,                      // default detection performed
    .bmconDetectMask = 0,                   // standard detection mask
    .bmstatCpblMask = 0,                    // standard capabilities mask
};

