/*******************************************************************************
  KSZ9031 PHY API for Microchip TCP/IP Stack
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

#include "driver/ethphy/src/dynamic/drv_extphy_ksz9131.h"

/******************************************************************************
 * Prototypes
 ******************************************************************************/
static DRV_ETHPHY_RESULT _DRV_KSZ9131_Skew_Setting(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj);

/******************************************************************************
 * Definitions
 ******************************************************************************/
typedef enum
{
    //States for Clock Skew setting
    DRV_KSZ9131_SKEW_CLK_1 = 0,
    DRV_KSZ9131_SKEW_CLK_2,
    DRV_KSZ9131_SKEW_CLK_3,  
    DRV_KSZ9131_SKEW_CLK_4,
    DRV_KSZ9131_SKEW_CLK_5,
} DRV_KSZ9131_SKEW_STATE;

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
    return _DRV_KSZ9131_Skew_Setting(pBaseObj, hClientObj);
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
static unsigned int DRV_EXTPHY_SMIClockGet(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE handle)
{
    return 2500000;     //  2.5 MHz max clock supported
}

// the DRV_ETHPHY_OBJECT

const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_KSZ9131 = 
{
    .miiConfigure = DRV_EXTPHY_MIIConfigure,
    .mdixConfigure = DRV_EXTPHY_MDIXConfigure,
    .smiClockGet = DRV_EXTPHY_SMIClockGet,
    .wolConfigure = 0,                      // no WOL functionality yet
    .phyDetect = 0,                         // default detection performed
};


static DRV_ETHPHY_RESULT _DRV_KSZ9131_Skew_Setting(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj)
{
    uint32_t skewState = 0;
    int phyAddress = 0;
    
    DRV_ETHPHY_RESULT res = pBaseObj->DRV_ETHPHY_VendorDataGet(hClientObj, &skewState);
    if(res < 0)
    {   // some error occurred
        return res;
    }
    
    pBaseObj->DRV_ETHPHY_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);
    
    switch (skewState)
    {
        case DRV_KSZ9131_SKEW_CLK_1:
            //Write to MMD Control register to set MMD Device Address : 02
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_MMD_ACCESS_CONTROL, (_PHY_MMD_CNTL_ACCESS_ADDRESS_MASK | 0x02), phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            skewState++;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, skewState);
            res = DRV_ETHPHY_RES_PENDING;
            break;

        case DRV_KSZ9131_SKEW_CLK_2:
            // wait for write complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIOperationIsComplete(hClientObj);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {   
                // Write to MMD Address register to set Register Address for access : Clock Pad Skew Register
                res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_MMD_ACCESS_DATA_ADDR, (PHY_MMD_CLK_SKEW_REG), phyAddress);
                if(res < 0)
                {   // some error
                    return res;
                }
                skewState++;
                pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, skewState);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;

        case DRV_KSZ9131_SKEW_CLK_3:
            // wait for write complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIOperationIsComplete(hClientObj);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {   
                //Write to MMD Control register to access the data
                res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_MMD_ACCESS_CONTROL, (_PHY_MMD_CNTL_ACCESS_DATA_MASK | 0x02), phyAddress);
                if(res < 0)
                {   // some error
                    return res;
                }
                skewState++;
                pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, skewState);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;
            
        case DRV_KSZ9131_SKEW_CLK_4:
            // wait for write complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIOperationIsComplete(hClientObj);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {   
                //Write to MMD Data register to write data to register : Set Rx/Tx Clock pad skew to maximum
                res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_MMD_ACCESS_DATA_ADDR, 0x3FF, phyAddress);
                if(res < 0)
                {   // some error
                    return res;
                }
                skewState++;
                pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, skewState);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;
            
        case DRV_KSZ9131_SKEW_CLK_5:
            // wait for write complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIOperationIsComplete(hClientObj);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res != DRV_ETHPHY_RES_OK)
            { 
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;
    }
    return res;
}