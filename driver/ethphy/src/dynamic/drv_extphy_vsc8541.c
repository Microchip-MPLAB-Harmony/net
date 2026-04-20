/*******************************************************************************
  VSC8541 PHY API for Microchip TCP/IP Stack
*******************************************************************************/

/*
Copyright (C) 2026, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include "driver/ethphy/drv_extphy_vsc8541.h"
#include "drv_extphy_vsc8541_priv.h"
#include "configuration.h"

/****************************************************************************
 *                 interface functions
 ****************************************************************************/

/****************************************************************************
 * Function:        VSC8541_MIIConfigure
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

static DRV_ETHPHY_RESULT VSC8541_MIIConfigure(const DRV_ETHPHY_OBJECT_BASE *pBaseObj, DRV_HANDLE hClientObj, DRV_ETHPHY_CONFIG_FLAGS cFlags)
{
    if( ((uint8_t)cFlags & ((uint8_t)DRV_ETHPHY_CFG_RMII | (uint8_t)DRV_ETHPHY_CFG_RGMII)) == 0U)
    {   // RMII and RGMII supported only
        return DRV_ETHPHY_RES_CFG_ERR;
    }

    union
    {
        uint32_t w;
        struct
        {
            uint16_t low;
            uint16_t high;
        };
    } vendorData = {};

    VSC8541_REG_EXTEND_PHY_CTRL_1_bits_t phyCtrl1;
    BMCONbits_t phyBMCON;
    uint16_t phyReg = 0;

    uint32_t configPhase;     // VSC8541_CONF_PHASE
    int phyAddress = 0; 

    DRV_ETHPHY_RESULT res = pBaseObj->phy_VendorDataGet(hClientObj, &vendorData.w);

    if ((int)res < 0)
    { // some error occurred
        return res;
    }

    configPhase = (uint32_t)vendorData.low;

    (void)pBaseObj->phy_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);

    switch (configPhase)
    {
        case  (uint32_t)MAIN_REG_SEL1:
            // RMII_BYPASS is in page 0; select page 0
            res = pBaseObj->phy_VendorSMIWriteStart(hClientObj, VSC8541_REG_EXTEND_PAGE_ACCESS, 0, phyAddress);
            if ((int)res < 0)
            { // some error
                break;
            }
            else if (res == DRV_ETHPHY_RES_PENDING)
            { // retry
                break;
            }
            else
            {
                // OK
            }

            // advance to the next phase
            (void)pBaseObj->phy_VendorDataSet(hClientObj, configPhase + 1U);
            res = DRV_ETHPHY_RES_PENDING;
            break;

        case (uint32_t)PHYCTRL1_READ:
            // read main reg 23 VSC8541_REG_EXTEND_PHY_CTRL_1 and set the MAC_INTERFACE_SEL RMII/RGMII/etc.!
            res = pBaseObj->phy_VendorSMIReadStart(hClientObj, VSC8541_REG_EXTEND_PHY_CTRL_1, phyAddress);
            if ((int)res < 0)
            { // some error
                break;
            }
            else if (res == DRV_ETHPHY_RES_PENDING)
            { // retry
                break;
            }
            else
            {
                // OK
            }

            // advance to the next phase
            (void)pBaseObj->phy_VendorDataSet(hClientObj, configPhase + 1U);
            res = DRV_ETHPHY_RES_PENDING;
            break;

        case (uint32_t)PHYCTRL1_MODIFY:
            res = pBaseObj->phy_VendorSMIReadResultGet(hClientObj, &phyReg);
            if ((int)res < 0)
            { // some error
                break;
            }
            else if (res == DRV_ETHPHY_RES_PENDING)
            { // retry
                break;
            }
            else
            {
                // OK
            }

            // got VSC8541_REG_EXTEND_PHY_CTRL_1 result; update the interface mode
            phyCtrl1.w = phyReg;
            phyCtrl1.w &= ~(uint16_t)M_EXT_PHY_CTRL_MAC_INTERFACE_SEL_MASK;
            if (((uint8_t)cFlags & (uint8_t)DRV_ETHPHY_CFG_RMII) != 0U)
            {
                phyCtrl1.w |= (uint16_t)M_MAC_INTERFACE_SEL_RMII_MASK;
            }
            else if (((uint8_t)cFlags & (uint8_t)DRV_ETHPHY_CFG_MII) != 0U)
            {
                phyCtrl1.w |= (uint16_t)M_MAC_INTERFACE_SEL_MII_MASK;
            }
            else if (((uint8_t)cFlags & (uint8_t)DRV_ETHPHY_CFG_RGMII) != 0U)
            {
                phyCtrl1.w |= (uint16_t)M_MAC_INTERFACE_SEL_RGMII_MASK;
            }
            else if (((uint8_t)cFlags & (uint8_t)DRV_ETHPHY_CFG_GMII) != 0U)
            {
                phyCtrl1.w |= (uint16_t)M_MAC_INTERFACE_SEL_GMII_MASK;
            }
            else
            {   // error
                res = DRV_ETHPHY_RES_CFG_ERR;
               break; 
            }

            // save the phyReg for the next state
            vendorData.high = phyCtrl1.w;
            vendorData.low = (uint16_t)(configPhase + 1U);
            (void)pBaseObj->phy_VendorDataSet(hClientObj, vendorData.w);
            res = DRV_ETHPHY_RES_PENDING;
            break;

        case (uint32_t)PHYCTRL1_WRITE:
            phyReg = vendorData.high;
            // now update VSC8541_REG_EXTEND_PHY_CTRL_1 Register
            res = pBaseObj->phy_VendorSMIWriteStart(hClientObj, VSC8541_REG_EXTEND_PHY_CTRL_1, phyReg, phyAddress);
            if ((int)res < 0)
            { // some error
                break;
            }
            else if (res == DRV_ETHPHY_RES_PENDING)
            { // retry
                break;
            }
            else
            {
                // OK
            }

            // advance to the next phase
            (void)pBaseObj->phy_VendorDataSet(hClientObj, configPhase + 1U);
            res = DRV_ETHPHY_RES_PENDING;
            break;

        case (uint32_t)BMCON_RST_READ:
            // got to reset the PHY after writing the VSC8541_REG_EXTEND_PHY_CTRL_1!
            // read the register for software reset
            res = pBaseObj->phy_VendorSMIReadStart(hClientObj, PHY_REG_BMCON, phyAddress);
            if ((int)res < 0)
            { // some error
                break;
            }
            else if (res == DRV_ETHPHY_RES_PENDING)
            { // retry
                break;
            }
            else
            {
                // OK
            }
            // advance to the next phase
            (void)pBaseObj->phy_VendorDataSet(hClientObj, configPhase + 1U);
            res = DRV_ETHPHY_RES_PENDING;
            break;

        case (uint32_t)BMCON_RST_MODIFY:
            res = pBaseObj->phy_VendorSMIReadResultGet(hClientObj, &phyReg);
            if ((int)res < 0)
            { // some error
                break;
            }
            else if (res == DRV_ETHPHY_RES_PENDING)
            { // retry
                break;
            }
            else
            {
                // OK
            }
            phyBMCON.w = phyReg;
            // overwrite the register to activate
            phyBMCON.RESET = 1;

            // save the phyReg for the next state
            vendorData.high = phyBMCON.w;
            vendorData.low = (uint16_t)(configPhase + 1U);
            (void)pBaseObj->phy_VendorDataSet(hClientObj, vendorData.w);
            res = DRV_ETHPHY_RES_PENDING;
            break;

        case (uint32_t)BMCON_RST_WRITE:
            phyReg = vendorData.high;
            // now update PHY_REG_BMCON Register
            res = pBaseObj->phy_VendorSMIWriteStart(hClientObj, PHY_REG_BMCON, phyReg, phyAddress);
            if ((int)res < 0)
            { // some error
                break;
            }
            else if (res == DRV_ETHPHY_RES_PENDING)
            { // retry
                break;
            }
            else
            {
                // OK
            }

            // advance to the next phase
            (void)pBaseObj->phy_VendorDataSet(hClientObj, configPhase + 1U);
            res = DRV_ETHPHY_RES_PENDING;
            break;

        case (uint32_t)MAIN_REG_SEL2:
            // Restore access to the main register page
            res = pBaseObj->phy_VendorSMIWriteStart(hClientObj, VSC8541_REG_EXTEND_PAGE_ACCESS, 0, phyAddress);
            if ((int)res < 0)
            { // some error
                break;
            }
            else if (res == DRV_ETHPHY_RES_PENDING)
            { // retry
                break;
            }
            else
            {
                // OK
            }

            // done
            res = DRV_ETHPHY_RES_OK;
            break;

        default:
            // shouldn't happen
            res = DRV_ETHPHY_RES_OPERATION_ERR;
            break;
    }
    return res;
}

/****************************************************************************
 * Function:        VSC8541_MDIXConfigure
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
static DRV_ETHPHY_RESULT VSC8541_MDIXConfigure(const DRV_ETHPHY_OBJECT_BASE *pBaseObj, DRV_HANDLE hClientObj, TCPIP_ETH_OPEN_FLAGS oFlags)
{
    return DRV_ETHPHY_RES_OK;
}

/****************************************************************************
 * Function:        VSC8541_SMIClockGet
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
static unsigned int VSC8541_SMIClockGet(const DRV_ETHPHY_OBJECT_BASE *pBaseObj, DRV_HANDLE handle)
{
    return 2500000; //  2.5 MHz max clock supported
}

const DRV_ETHPHY_OBJECT DRV_ETHPHY_OBJECT_VSC8541 =
{
    .miiConfigure = &VSC8541_MIIConfigure,
    .mdixConfigure = &VSC8541_MDIXConfigure,
    .smiClockGet = &VSC8541_SMIClockGet,
    .wolConfigure = NULL,       // no WOL functionality yet
    .phyDetect = NULL,          // default detection performed
    .bmconDetectMask = 0,       // standard detection mask
    .bmstatCpblMask = 0,        // standard capabilities mask
};

