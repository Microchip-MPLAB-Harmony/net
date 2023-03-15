/*******************************************************************************
  VSC8540 PHY API for Microchip TCP/IP Stack
*******************************************************************************/

/*
Copyright (C) 2022-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include "driver/ethphy/src/dynamic/drv_extphy_vsc8540.h"
#include "configuration.h"

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
static DRV_ETHPHY_RESULT DRV_EXTPHY_MIIConfigure(const DRV_ETHPHY_OBJECT_BASE *pBaseObj, DRV_HANDLE hClientObj, DRV_ETHPHY_CONFIG_FLAGS cFlags)
{

#ifdef TCPIP_INTMAC_PHY_CONFIG_MANAGED_MODE

    union
    {
        uint32_t w;
        struct
        {
            uint16_t low;
            uint16_t high;
        };
    } vendorData = {};

    __PHY_REG_EXTEND_PHY_CONTROL_1_bits_t phyCtrl1;
    __BMCONbits_t phyBMCON;
    __PHY_EXT2_RGMII_CTRL_bits_t phyEXT2CTRL;
    __PHY_EXT_GP_CLKOUT_CTRL_bits_t phyCLKOUTCTRL;
    uint16_t phyReg = 0;

    
    VSC8540_CONF_PHASE configPhase;
    int phyAddress = 0;

    DRV_ETHPHY_RESULT res = pBaseObj->DRV_ETHPHY_VendorDataGet(hClientObj, &vendorData.w);

    if (res < 0)
    { // some error occurred
        return res;
    }

    configPhase = vendorData.low;

    pBaseObj->DRV_ETHPHY_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);

    switch (configPhase)
    {
    case  MAIN_REG_SEL1:
        // RMII_BYPASS is in page 0; select page 0
        res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_EXTENDED_PAGE_ACCESS, 0, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case PHYCTRL1_READ:
        res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_EXTEND_PHY_CONTROL_1, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case PHYCTRL1_MODIFY:
        res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // got PHY_REG_EXTEND_PHY_CONTROL_1 result
        phyCtrl1.w = phyReg;
        if (cFlags & DRV_ETHPHY_CFG_RMII)
        {
            phyCtrl1.MAC_INTERFACE_SEL = 1; // RMII
        }
        else
        {
            phyCtrl1.MAC_INTERFACE_SEL = 0; // MII
        }

        // save the phyReg for the next state
        vendorData.high = phyCtrl1.w;
        vendorData.low = configPhase + 1;
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
        return DRV_ETHPHY_RES_PENDING;

    case PHYCTRL1_WRITE:
        phyReg = vendorData.high;
        // now update PHY_REG_EXTEND_PHY_CONTROL_1 Register
        res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_EXTEND_PHY_CONTROL_1, phyReg, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case BMCON_RST_READ:
        // read the register for software reset
        res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_BMCON, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }
        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case BMCON_RST_MODIFY:
        res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }
        phyBMCON.w = phyReg;
        // overwrite the register to activate
        phyBMCON.RESET = 1;

        // save the phyReg for the next state
        vendorData.high = phyBMCON.w;
        vendorData.low = configPhase + 1;
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
        return DRV_ETHPHY_RES_PENDING;

    case BMCON_RST_WRITE:
        phyReg = vendorData.high;
        // now update PHY_REG_BMCON Register
        res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_BMCON, phyReg, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case BMCON_CONFIG_READ:
        res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_BMCON, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case BMCON_CONFIG_MODIFY:
        res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        phyBMCON.w = phyReg;
        if (TCPIP_INTMAC_PHY_CONFIG_FORCE_SPD_SEL)
        {
            // modify force 100Mbps
            phyBMCON.SPEED = 1;
            phyBMCON.SPEED1000 = 0;
        }
        else
        {
            // modify force 10Mbps
            phyBMCON.SPEED = 0;
            phyBMCON.SPEED1000 = 0;
        }

        if (TCPIP_INTMAC_PHY_CONFIG_DUPLEX_SEL)
        {
            // Mode full duplex
            phyBMCON.DUPLEX = 1;
        }
        else
        {
            // mode half duplex
            phyBMCON.DUPLEX = 0;
        }

        if (TCPIP_INTMAC_PHY_CONFIG_AUTONEG_EN)
        {
            // Enable autoneg
            phyBMCON.AN_ENABLE = 1;
        }
        else
        {
            // Disable autoneg
            phyBMCON.AN_ENABLE = 0;
        }

        vendorData.low = configPhase + 1;
        vendorData.high = phyBMCON.w;
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);

    case BMCON_CONFIG_WRITE:
        phyReg = vendorData.high;
        // update the PHY_REG_BMCON Register
        res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_BMCON, phyReg, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case EXT_PAGE_2_SEL:
        // PHY_EXT2_RGMII_CTRL is in page 2; select page 2
        res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_EXTENDED_PAGE_ACCESS, 2, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case RGMII_CTRL_READ:
        res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_EXT2_RGMII_CTRL, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case RGMII_CTRL_MODIFY:
        res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        phyEXT2CTRL.w = phyReg;
        // modify RX_CLK_Delay register
        phyEXT2CTRL.RX_CLK_DELAY = TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY;

        //  modify RX_CLK_Delay register
        phyEXT2CTRL.TX_CLK_DELAY = TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY;

        vendorData.low = configPhase + 1;
        vendorData.high = phyEXT2CTRL.w;
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);

    case RGMII_CTRL_WRITE:
        phyReg = vendorData.high;
        // update the PHY_EXT2_RGMII_CTRL Register
        res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_EXT2_RGMII_CTRL, phyReg, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case GP_REG_SEL:
        // PHY_EXT_GP_CLKOUT_CTRL is in page 16; select page 16
        res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_EXTENDED_PAGE_ACCESS, 16, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case CLKOUT_CTRL_READ:
        res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_EXT_GP_CLKOUT_CTRL, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case CLKOUT_CTRL_MODIFY:
        res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        phyCLKOUTCTRL.w = phyReg;

        if (TCPIP_INTMAC_PHY_CONFIG_CLKOUT_EN)
        {
            // Enable CLKOUT
            phyCLKOUTCTRL.CLKOUT_EN = 1;

            // Enable CLKOUT
            phyCLKOUTCTRL.CLKOUT_FREQ_SELECT = TCPIP_INTMAC_PHY_CONFIG_CLKOUT_FREQ;
        }
        else
        {
            // Disable CLKOUT
            phyCLKOUTCTRL.CLKOUT_EN = 0;
        }

        vendorData.low = configPhase + 1;
        vendorData.high = phyCLKOUTCTRL.w;
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);

    case CLKOUT_CTRL_WRITE:
        phyReg = vendorData.high;
        // update the PHY_EXT_GP_CLKOUT_CTRL Register
        res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_EXT_GP_CLKOUT_CTRL, phyReg, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // advance to the next phase
        pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++configPhase);
        return DRV_ETHPHY_RES_PENDING;

    case MAIN_REG_SEL2:
        // Come back to the main register page
        res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_EXTENDED_PAGE_ACCESS, 0, phyAddress);
        if (res < 0)
        { // some error
            return res;
        }
        else if (res == DRV_ETHPHY_RES_PENDING)
        { // retry
            return DRV_ETHPHY_RES_PENDING;
        }

        // done
        return DRV_ETHPHY_RES_OK;

    default:
        // shouldn't happen
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }
#else 
    return (cFlags & DRV_ETHPHY_CFG_RMII) ? DRV_ETHPHY_RES_OK : DRV_ETHPHY_RES_CFG_ERR;
#endif
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
static DRV_ETHPHY_RESULT DRV_EXTPHY_MDIXConfigure(const DRV_ETHPHY_OBJECT_BASE *pBaseObj, DRV_HANDLE hClientObj, TCPIP_ETH_OPEN_FLAGS oFlags)
{
    return DRV_ETHPHY_RES_OK;
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
static unsigned int DRV_EXTPHY_SMIClockGet(const DRV_ETHPHY_OBJECT_BASE *pBaseObj, DRV_HANDLE handle)
{
    return 2500000; //  2.5 MHz max clock supported
}

const DRV_ETHPHY_OBJECT DRV_ETHPHY_OBJECT_VSC8540 =
{
    .miiConfigure = DRV_EXTPHY_MIIConfigure,
    .mdixConfigure = DRV_EXTPHY_MDIXConfigure,
    .smiClockGet = DRV_EXTPHY_SMIClockGet,
    .wolConfigure = 0,
    .phyDetect = 0, // default detection performed
    .bmconDetectMask = 0,                   // standard detection mask
    .bmstatCpblMask = 0,                    // standard capabilities mask
};
