/*******************************************************************************
  LAN8770 PHY API for Microchip TCP/IP Stack
*******************************************************************************/

/*
Copyright (C) 2024-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#include "driver/ethphy/drv_extphy_lan8770.h"
#include "drv_extphy_lan8770_priv.h"

/****************************************************************************
 *                 interface functions
 ****************************************************************************/


/****************************************************************************
 * Function:        DRV_LAN8770_MIIConfigure
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
static DRV_ETHPHY_RESULT DRV_LAN8770_MIIConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, DRV_ETHPHY_CONFIG_FLAGS cFlags)
{
    return (((uint8_t)cFlags & (uint8_t)DRV_ETHPHY_CFG_RMII) != 0U) ? DRV_ETHPHY_RES_OK : DRV_ETHPHY_RES_CFG_ERR;
}


/****************************************************************************
 * Function:        DRV_LAN8770_MDIXConfigure
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
static DRV_ETHPHY_RESULT DRV_LAN8770_MDIXConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, TCPIP_ETH_OPEN_FLAGS oFlags)
{
    // no MDIX supported
    return DRV_ETHPHY_RES_OK;
}


/****************************************************************************
 * Function:        DRV_LAN8770_SMIClockGet
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
static unsigned int DRV_LAN8770_SMIClockGet(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE handle)
{
    return 4000000;     //  4 MHz max clock supported
}


// the DRV_ETHPHY_OBJECT

const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_LAN8770 = 
{
    .miiConfigure = &DRV_LAN8770_MIIConfigure,
    .mdixConfigure = &DRV_LAN8770_MDIXConfigure,
    .smiClockGet = &DRV_LAN8770_SMIClockGet,
    .wolConfigure = NULL,                   // no WOL functionality yet
    .phyDetect = NULL,                      // default detection function
    .bmconDetectMask = BMCON_LOOPBACK_MASK,        // no support for the DUPLEX mask! 
    .bmstatCpblMask = BMSTAT_BASE100TX_FDX_MASK,   // show 100 Mbps FD capability
};

