/*******************************************************************************
  IP101GR API for Microchip TCP/IP Stack
*******************************************************************************/

/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#include "driver/ethphy/src/dynamic/drv_extphy_ip101gr.h"

/****************************************************************************
 *                 interface functions
 ****************************************************************************/

/****************************************************************************
 * Function:        DRV_EXTPHY_MIIConfigure
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:   		handle - A valid open-instance handle, returned from the driver's open routine   
 *				cFlags - the requested configuration flags: DRV_ETHPHY_CFG_RMII/DRV_ETHPHY_CFG_MII
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
 *****************************************************************************/
static DRV_ETHPHY_RESULT DRV_EXTPHY_MIIConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj,DRV_ETHPHY_CONFIG_FLAGS cFlags)
{
    union
    {
        uint32_t    w;
        struct
        {
            uint16_t low;
            uint16_t high;
        };
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

            // RMII12  is in page 16 Reg16
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_PAGE_SEL, PAGENUM_16, phyAddress);
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

            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_UTP_CONTROL_MODE, phyAddress);
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

        case 2:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // got PHY_REG_UTP_CONTROL_MODE result
            if(cFlags & DRV_ETHPHY_CFG_RMII)
            {
                // use RMII 1.2 and set the repeater
                phyReg |= _UTP_MODECTRL_RMII_V12_MASK | _UTP_MODECTRL_REPEATER_MODE_MASK;
            }

            // save the phyReg for the next state
            vendorData.high = phyReg;
            vendorData.low = miiConfPhase + 1;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;

        case 3:
            phyReg = vendorData.high;
            // update the RMII and Bypass Register Register
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_UTP_CONTROL_MODE, phyReg, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++miiConfPhase);
            return DRV_ETHPHY_RES_PENDING;


        case 4:
            // go back to Page 0
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_PAGE_SEL, PAGENUM_0, phyAddress);
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
            // Auto MDIX  is in page 16 ,Reg 16
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_PAGE_SEL, PAGENUM_16, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 1:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_UTP_CONTROL_MODE, phyAddress);
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


        case 2:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // got PHY_REG_UTP_CONTROL_MODE result

            vendorData.low = mdixConfPhase + 1;
            vendorData.high = phyReg;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;
 
        case 3:
            phyReg = vendorData.high;
            // update the PHY_REG_UTP_CONTROL_MODE Register
            if(oFlags & TCPIP_ETH_OPEN_MDIX_AUTO)
            {   // enable Auto-MDIX
                phyReg |= _UTP_MODECTRL_AUTO_MDIX_MASK;
            }
            else
            {	// no Auto-MDIX It is a Forced MDIX
                phyReg &= ~(_UTP_MODECTRL_AUTO_MDIX_MASK);	// disable Auto-MDIX
            }

            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_UTP_CONTROL_MODE, phyReg, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done    
            if(oFlags & TCPIP_ETH_OPEN_MDIX_AUTO)
            {   // done for Auto-MDIX
                mdixConfPhase = 8;
            }
            else
            {
                mdixConfPhase = 4;
            }
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 4:
            // Set the Page16 and reg 30 for Forced MDIX
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_PAGE_SEL, PAGENUM_16, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 5:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_MDIX_CNTL_SPFC_STATUS, phyAddress);
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

        case 6:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // got PHY_REG_MDIX_CNTL_SPFC_STATUS result
            if(oFlags & TCPIP_ETH_OPEN_MDIX_SWAP)
            {
                phyReg |= _MDIX_CONTR_STATUS_FORCEMDIX_MASK;	// swap
            }
            else
            {
                phyReg &= ~(_MDIX_CONTR_STATUS_FORCEMDIX_MASK);	// normal
            }

            vendorData.low = mdixConfPhase + 1;
            vendorData.high = phyReg;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;

        case 7:
            phyReg = vendorData.high;
            // Set the PHY_REG_MDIX_CNTL_SPFC_STATUS
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_MDIX_CNTL_SPFC_STATUS, phyReg, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 8:
           // reset the page number
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_PAGE_SEL, PAGENUM_0, phyAddress);
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
	return 2500000;		//  2.5 MHz max clock supported
}


// the DRV_ETHPHY_OBJECT

const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_IP101GR = 
{
    .miiConfigure = DRV_EXTPHY_MIIConfigure,
    .mdixConfigure = DRV_EXTPHY_MDIXConfigure,
    .smiClockGet = DRV_EXTPHY_SMIClockGet,
    .wolConfigure = 0,                      // no WOL functionality yet
    .phyDetect = 0,                         // default detection performed
    .bmconDetectMask = 0,                   // standard detection mask
    .bmstatCpblMask = 0,                    // standard capabilities mask
};







