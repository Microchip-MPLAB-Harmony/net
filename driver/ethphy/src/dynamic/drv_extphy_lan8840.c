/*******************************************************************************
  LAN8840 PHY API for Microchip TCP/IP Stack
*******************************************************************************/

/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include "driver/ethphy/src/dynamic/drv_extphy_lan8840.h"

/******************************************************************************
 * Prototypes
 ******************************************************************************/
#ifdef DRV_ETHPHY_LAN8840_SKEW_SETTING
static DRV_ETHPHY_RESULT DRV_LAN8840_Skew_Setting(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj);
static DRV_ETHPHY_RESULT DRV_LAN8840_Read_MMD_Reg(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, uint16_t regIndex, uint16_t* pReadOut);
static DRV_ETHPHY_RESULT DRV_LAN8840_Write_MMD_Reg(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, uint16_t regIndex,  uint16_t writeIn);
#endif

/******************************************************************************
 * Definitions
 ******************************************************************************/
#ifdef DRV_ETHPHY_LAN8840_SKEW_SETTING 
typedef enum
{
    //States for MMD registers opeartions
    DRV_LAN8840_MMD_REG_OPR_1 = 0,
    DRV_LAN8840_MMD_REG_OPR_2,
    DRV_LAN8840_MMD_REG_OPR_3,  
    DRV_LAN8840_MMD_REG_OPR_4,
    DRV_LAN8840_MMD_REG_OPR_5
} DRV_LAN8840_MMD_REG_OPR_STATE;

typedef struct 
{
    SKEW_SET_STATES skewSetState;
    uint16_t readResValue;   
} LAN8840_SKEW_STATE_SET_TYPE;

LAN8840_SKEW_STATE_SET_TYPE lan8840_phy_inst_data[DRV_MIIM_INSTANCES_NUMBER] = {0};
#endif

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
    #ifdef DRV_ETHPHY_LAN8840_SKEW_SETTING
    {
        return DRV_LAN8840_Skew_Setting(pBaseObj, hClientObj);
    }
    #else
    {
        return DRV_ETHPHY_RES_OK;
    }
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
const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_LAN8840 = 
{
    .miiConfigure = DRV_EXTPHY_MIIConfigure,
    .mdixConfigure = DRV_EXTPHY_MDIXConfigure,
    .smiClockGet = DRV_EXTPHY_SMIClockGet,
    .wolConfigure = 0,                      // no WOL functionality yet
    .phyDetect = 0,                         // default detection performed
    .bmconDetectMask = 0,                   // standard detection mask
    .bmstatCpblMask = 0,                    // standard capabilities mask
};

#ifdef DRV_ETHPHY_LAN8840_SKEW_SETTING

// a function that processes a skew state
typedef DRV_ETHPHY_RESULT (*LAN8840_SkewFnc)(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);

// skew setting functions declarations 
static DRV_ETHPHY_RESULT txClkSkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT txClkSkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxClkSkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxClkSkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxCtlSkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxCtlSkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT txCtlSkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT txCtlSkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxd3SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxd3SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxd2SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxd2SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxd1SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxd1SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxd0SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT rxd0SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT txd3SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT txd3SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT txd2SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT txd2SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT txd1SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT txd1SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT txd0SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);
static DRV_ETHPHY_RESULT txd0SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew);

// a table containing the functions for all the skew settings
static const LAN8840_SkewFnc skew_proc_tbl[] =
{
    txClkSkewReadF,        // TX_CLK_SKEW_READ
    txClkSkewWriteF,       // TX_CLK_SKEW_WRITE
    rxClkSkewReadF,        // RX_CLK_SKEW_READ
    rxClkSkewWriteF,       // RX_CLK_SKEW_WRITE
    rxCtlSkewReadF,        // RX_CTL_SKEW_READ
    rxCtlSkewWriteF,       // RX_CTL_SKEW_WRITE
    txCtlSkewReadF,        // TX_CTL_SKEW_READ
    txCtlSkewWriteF,       // TX_CTL_SKEW_WRITE
    rxd3SkewReadF,         // RXD3_SKEW_READ
    rxd3SkewWriteF,        // RXD3_SKEW_WRITE
    rxd2SkewReadF,         // RXD2_SKEW_READ
    rxd2SkewWriteF,        // RXD2_SKEW_WRITE
    rxd1SkewReadF,         // RXD1_SKEW_READ
    rxd1SkewWriteF,        // RXD1_SKEW_WRITE
    rxd0SkewReadF,         // RXD0_SKEW_READ
    rxd0SkewWriteF,        // RXD0_SKEW_WRITE
    txd3SkewReadF,         // TXD3_SKEW_READ
    txd3SkewWriteF,        // TXD3_SKEW_WRITE
    txd2SkewReadF,         // TXD2_SKEW_READ
    txd2SkewWriteF,        // TXD2_SKEW_WRITE
    txd1SkewReadF,         // TXD1_SKEW_READ
    txd1SkewWriteF,        // TXD1_SKEW_WRITE
    txd0SkewReadF,         // TXD0_SKEW_READ
    txd0SkewWriteF         // TXD0_SKEW_WRITE
};

/****************************************************************************
 * Function:        DRV_LAN8840_Skew_Setting
 *
 * PreCondition:    DRV_ETHPHY_LAN8840_SKEW_SETTING macro should have been defined.
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine
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
 * Overview:        This function does the skew settings by performing read and write operations on the MMD registers corresponding to the relevent skew setting configured.
 *
 * Note:            None
 *****************************************************************************/
static DRV_ETHPHY_RESULT DRV_LAN8840_Skew_Setting(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj)
{
    DRV_ETHPHY_RESULT res = 0;
    DRV_ETHPHY_CLIENT_OBJ *hClient = (DRV_ETHPHY_CLIENT_OBJ *)hClientObj;
    LAN8840_SKEW_STATE_SET_TYPE* pSkew = lan8840_phy_inst_data + hClient->hDriver->miimIndex;
    if((0 <=  pSkew->skewSetState) && (pSkew->skewSetState < (sizeof(skew_proc_tbl) / sizeof(*skew_proc_tbl))))
    {   // fine to execute the state function
        res = skew_proc_tbl[pSkew->skewSetState](pBaseObj, hClientObj, pSkew);
    }
    else
    {
        res = DRV_ETHPHY_RES_OPERATION_ERR;
    }

    if (res == DRV_ETHPHY_RES_OK)
    {
        pSkew->readResValue = 0;
        pSkew->skewSetState = 0;
    }
    
    return res;
}

static DRV_ETHPHY_RESULT txClkSkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_TX_CLK_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_CLK_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
              pSkew->readResValue = readValue;
              pSkew->skewSetState = TX_CLK_SKEW_WRITE;
              res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        pSkew->skewSetState = RX_CLK_SKEW_READ;
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT txClkSkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #ifdef DRV_ETHPHY_LAN8840_TX_CLK_SKEW
        uint16_t writeValue = (DRV_ETHPHY_LAN8840_TX_CLK_SKEW << PHY_MMD_TX_CLK_SKEW_POS) | (pSkew->readResValue & ~(PHY_MMD_TX_CLK_SKEW_MASK)) ;
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_CLK_SKEW_REG, writeValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->skewSetState = RX_CLK_SKEW_READ;
            res = DRV_ETHPHY_RES_PENDING;
        }
    #endif
    return res;
}

static DRV_ETHPHY_RESULT rxClkSkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_RX_CLK_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_CLK_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->readResValue = readValue;
            pSkew->skewSetState = RX_CLK_SKEW_WRITE;
            res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        pSkew->skewSetState = RX_CTL_SKEW_READ;
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT rxClkSkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{   
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #ifdef DRV_ETHPHY_LAN8840_RX_CLK_SKEW
        uint16_t writeValue = DRV_ETHPHY_LAN8840_RX_CLK_SKEW | (pSkew->readResValue & ~(PHY_MMD_RX_CLK_SKEW_MASK)) ;
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_CLK_SKEW_REG, writeValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->skewSetState = RX_CTL_SKEW_READ;   
            res = DRV_ETHPHY_RES_PENDING;
        }
    #endif
    return res;    
}

static DRV_ETHPHY_RESULT rxCtlSkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_RX_CTL_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_CTL_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->readResValue = readValue;
            pSkew->skewSetState = RX_CTL_SKEW_WRITE;
            res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        pSkew->skewSetState = TX_CTL_SKEW_READ;
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT rxCtlSkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #ifdef DRV_ETHPHY_LAN8840_RX_CTL_SKEW
        uint16_t writeValue = (DRV_ETHPHY_LAN8840_RX_CTL_SKEW << PHY_MMD_RX_CTL_SKEW_POS) | (pSkew->readResValue & ~(PHY_MMD_RX_CTL_SKEW_MASK));
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_CTL_SKEW_REG, writeValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->skewSetState = TX_CTL_SKEW_READ;   
            res = DRV_ETHPHY_RES_PENDING;
        }
    #endif     
    return res;
}

static DRV_ETHPHY_RESULT txCtlSkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_TX_CTL_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_CTL_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->readResValue = readValue;
            pSkew->skewSetState = TX_CTL_SKEW_WRITE;
            res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        pSkew->skewSetState = RXD3_SKEW_READ;
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT txCtlSkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #ifdef DRV_ETHPHY_LAN8840_TX_CTL_SKEW
        uint16_t writeValue = DRV_ETHPHY_LAN8840_TX_CTL_SKEW | (pSkew->readResValue & ~(PHY_MMD_TX_CTL_SKEW_MASK));
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_CTL_SKEW_REG, writeValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->skewSetState = RXD3_SKEW_READ;   
            res = DRV_ETHPHY_RES_PENDING;
        }
    #endif  
    return res;
}

static DRV_ETHPHY_RESULT rxd3SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_RXD3_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_RX_DATA_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->readResValue = readValue;
            pSkew->skewSetState = RXD3_SKEW_WRITE;
            res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        pSkew->skewSetState = RXD2_SKEW_READ;
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT rxd3SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #ifdef DRV_ETHPHY_LAN8840_RXD3_SKEW
        uint16_t writeValue = (DRV_ETHPHY_LAN8840_RXD3_SKEW << PHY_MMD_RXD3_PAD_SKEW_POS ) | (pSkew->readResValue & ~(PHY_MMD_RXD3_PAD_SKEW_MASK));
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_RX_DATA_SKEW_REG, writeValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->skewSetState = RXD2_SKEW_READ;   
            res = DRV_ETHPHY_RES_PENDING;
        }
    #endif
    return res;
}

static DRV_ETHPHY_RESULT rxd2SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_RXD2_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_RX_DATA_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->readResValue = readValue;
            pSkew->skewSetState = RXD2_SKEW_WRITE;
            res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        pSkew->skewSetState = RXD1_SKEW_READ;
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT rxd2SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #ifdef DRV_ETHPHY_LAN8840_RXD2_SKEW
        uint16_t writeValue = (DRV_ETHPHY_LAN8840_RXD2_SKEW << PHY_MMD_RXD2_PAD_SKEW_POS ) | (pSkew->readResValue & ~(PHY_MMD_RXD2_PAD_SKEW_MASK));
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_RX_DATA_SKEW_REG, writeValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->skewSetState = RXD1_SKEW_READ;   
            res = DRV_ETHPHY_RES_PENDING;
        }
    #endif   
    return res;
}

static DRV_ETHPHY_RESULT rxd1SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_RXD1_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_RX_DATA_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->readResValue = readValue;
            pSkew->skewSetState = RXD1_SKEW_WRITE;
            res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        pSkew->skewSetState = RXD0_SKEW_READ;
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT rxd1SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #ifdef DRV_ETHPHY_LAN8840_RXD1_SKEW
        uint16_t writeValue = (DRV_ETHPHY_LAN8840_RXD1_SKEW << PHY_MMD_RXD1_PAD_SKEW_POS ) | (pSkew->readResValue & ~(PHY_MMD_RXD1_PAD_SKEW_MASK));
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_RX_DATA_SKEW_REG, writeValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->skewSetState = RXD0_SKEW_READ;   
            res = DRV_ETHPHY_RES_PENDING;
        }
    #endif     
    return res;
}

static DRV_ETHPHY_RESULT rxd0SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_RXD0_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_RX_DATA_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->readResValue = readValue;
            pSkew->skewSetState = RXD0_SKEW_WRITE;
            res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        pSkew->skewSetState = TXD3_SKEW_READ;
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT rxd0SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #ifdef DRV_ETHPHY_LAN8840_RXD0_SKEW
        uint16_t writeValue = DRV_ETHPHY_LAN8840_RXD0_SKEW | (pSkew->readResValue & ~(PHY_MMD_RXD0_PAD_SKEW_MASK));
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_RX_DATA_SKEW_REG, writeValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->skewSetState = TXD3_SKEW_READ;   
            res = DRV_ETHPHY_RES_PENDING;
        }
    #endif  
    return res;
}

static DRV_ETHPHY_RESULT txd3SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_TXD3_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_TX_DATA_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->readResValue = readValue;
            pSkew->skewSetState = TXD3_SKEW_WRITE;
            res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        pSkew->skewSetState = TXD2_SKEW_READ;
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT txd3SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #ifdef DRV_ETHPHY_LAN8840_TXD3_SKEW
        uint16_t writeValue = (DRV_ETHPHY_LAN8840_TXD3_SKEW << PHY_MMD_TXD3_PAD_SKEW_POS ) | (pSkew->readResValue & ~(PHY_MMD_TXD3_PAD_SKEW_MASK)) ;
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_TX_DATA_SKEW_REG, writeValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->skewSetState = TXD2_SKEW_READ;   
            res = DRV_ETHPHY_RES_PENDING;
        }
    #endif     
    return res;
}

static DRV_ETHPHY_RESULT txd2SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_TXD2_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_TX_DATA_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->readResValue = readValue;
            pSkew->skewSetState = TXD2_SKEW_WRITE;
            res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        pSkew->skewSetState = TXD1_SKEW_READ;
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT txd2SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #ifdef DRV_ETHPHY_LAN8840_TXD2_SKEW
        uint16_t writeValue = (DRV_ETHPHY_LAN8840_TXD2_SKEW << PHY_MMD_TXD2_PAD_SKEW_POS ) | (pSkew->readResValue & ~(PHY_MMD_TXD2_PAD_SKEW_MASK));
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_TX_DATA_SKEW_REG, writeValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->skewSetState = TXD1_SKEW_READ;   
            res = DRV_ETHPHY_RES_PENDING;
        }
    #endif    
    return res;
}

static DRV_ETHPHY_RESULT txd1SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_TXD1_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_TX_DATA_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->readResValue = readValue;
            pSkew->skewSetState = TXD1_SKEW_WRITE;
            res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        pSkew->skewSetState = TXD0_SKEW_READ;
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT txd1SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    #ifdef DRV_ETHPHY_LAN8840_TXD1_SKEW
        uint16_t writeValue = (DRV_ETHPHY_LAN8840_TXD1_SKEW << PHY_MMD_TXD1_PAD_SKEW_POS ) | (pSkew->readResValue & ~(PHY_MMD_TXD1_PAD_SKEW_MASK));
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_TX_DATA_SKEW_REG, writeValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->skewSetState = TXD0_SKEW_READ;   
            res = DRV_ETHPHY_RES_PENDING;
        }
    #endif     
    return res;
}

static DRV_ETHPHY_RESULT txd0SkewReadF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    #ifdef DRV_ETHPHY_LAN8840_TXD0_SKEW
        uint16_t readValue = 0;
        DRV_ETHPHY_RESULT res = DRV_LAN8840_Read_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_TX_DATA_SKEW_REG, &readValue);
        if(res == DRV_ETHPHY_RES_OK)
        {
            pSkew->readResValue = readValue;
            pSkew->skewSetState = TXD0_SKEW_WRITE;
            res = DRV_ETHPHY_RES_PENDING;
        }
        else
        {
            // do nothing
        }
    #else
        DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_OK;
    #endif

    return res;
}

static DRV_ETHPHY_RESULT txd0SkewWriteF(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, LAN8840_SKEW_STATE_SET_TYPE* pSkew)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_OK;
    #ifdef DRV_ETHPHY_LAN8840_TXD0_SKEW
        uint16_t writeValue = DRV_ETHPHY_LAN8840_TXD0_SKEW  | (pSkew->readResValue & ~(PHY_MMD_TXD0_PAD_SKEW_MASK));
        res = DRV_LAN8840_Write_MMD_Reg(pBaseObj, hClientObj, PHY_MMD_RGMII_TX_DATA_SKEW_REG, writeValue);
    #endif   
   return res;
}

/****************************************************************************
 * Function:        DRV_LAN8840_Read_MMD_Reg
 *
 * PreCondition:    DRV_ETHPHY_LAN8840_SKEW_SETTING macro should have been defined.
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine.
 *                  regIndex - The MMD register location to read from.
 *                  pReadOut - A pointer, to which the read result will be updated in this function.
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
 * Overview:        This function performs read from the MMD register and updates the pReadOut with the read result.
 *
 * Note:            None
 *****************************************************************************/
static DRV_ETHPHY_RESULT DRV_LAN8840_Read_MMD_Reg(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, uint16_t regIndex,  uint16_t* pReadOut)
{
    uint32_t readState = 0;
    int phyAddress = 0;
    
    DRV_ETHPHY_RESULT res = pBaseObj->DRV_ETHPHY_VendorDataGet(hClientObj, &readState);
    if(res < 0)
    {   // some error occurred
        return res;
    }

    pBaseObj->DRV_ETHPHY_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);
    
    switch (readState)
    {
        case DRV_LAN8840_MMD_REG_OPR_1:
            //Write to MMD Control register to set MMD Device Address
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteWaitComplete(hClientObj, PHY_MMD_ACCESS_CONTROL, (M_PHY_MMD_CNTL_ACCESS_ADDRESS_MASK | PHY_MMD_DEVICE_ADDRESS), phyAddress);
            if(res < 0)
            {   // some error
                // do nothing
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {
                readState++;
                pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, readState);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;

        case DRV_LAN8840_MMD_REG_OPR_2:
            // wait for write complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIOperationIsComplete(hClientObj);
            if(res < 0)
            {   // some error
                // do nothing
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {   
                // Write to MMD Address register to set Register Address for access
                res = pBaseObj->DRV_ETHPHY_VendorSMIWriteWaitComplete(hClientObj, PHY_MMD_ACCESS_DATA_ADDR, regIndex, phyAddress);
                if(res < 0)
                {   // some error
                    // do nothing
                }
                else if(res == DRV_ETHPHY_RES_OK)
                {
                    readState++;
                    pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, readState);
                    res = DRV_ETHPHY_RES_PENDING;
                }
            }
            break;

        case DRV_LAN8840_MMD_REG_OPR_3:
            // wait for write complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIOperationIsComplete(hClientObj);
            if(res < 0)
            {   // some error
                // do nothing
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {   
                //Write to MMD Control register to access the data
                res = pBaseObj->DRV_ETHPHY_VendorSMIWriteWaitComplete(hClientObj, PHY_MMD_ACCESS_CONTROL, (M_PHY_MMD_CNTL_ACCESS_DATA_MASK | PHY_MMD_DEVICE_ADDRESS), phyAddress);
                if(res < 0)
                {   // some error
                    // do nothing
                }
                else if(res == DRV_ETHPHY_RES_OK)
                {
                    readState++;
                    pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, readState);
                    res = DRV_ETHPHY_RES_PENDING;
                }
            }
            break;
            
        case DRV_LAN8840_MMD_REG_OPR_4:
            // wait for write complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIOperationIsComplete(hClientObj);
            if(res < 0)
            {   // some error
                // do nothing
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {   
                //Initiate read from MMD Data register
                res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_MMD_ACCESS_DATA_ADDR, phyAddress);
                if(res < 0)
                {   // some error
                    // do nothing
                }
                else if(res == DRV_ETHPHY_RES_OK)
                {
                    readState++;
                    pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, readState);
                    res = DRV_ETHPHY_RES_PENDING;
                }
            }
            break;
            
        case DRV_LAN8840_MMD_REG_OPR_5:
            //Read result from MMD Data register
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, pReadOut);
            if(res < 0)
            {   // some error
                // do nothing
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {           
                readState = 0;
                pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, readState);
            }
            break;
    }
    return res;
}

/****************************************************************************
 * Function:        DRV_LAN8840_Write_MMD_Reg
 *
 * PreCondition:    DRV_ETHPHY_LAN8840_SKEW_SETTING macro should have been defined.
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine.
 *                  regIndex - The MMD register location to write to.
 *                  writeIn - The value which is to be written to the MMD register.
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
 * Overview:        This function writes to the MMD register with the value given in writeIn argument.
 *
 * Note:            None
 *****************************************************************************/
static DRV_ETHPHY_RESULT DRV_LAN8840_Write_MMD_Reg(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, uint16_t regIndex,  uint16_t writeIn)
{    
    uint32_t writeState = 0;
    int phyAddress = 0;
    
    DRV_ETHPHY_RESULT res = pBaseObj->DRV_ETHPHY_VendorDataGet(hClientObj, &writeState);
    if(res < 0)
    {   // some error occurred
        return res;
    }

    pBaseObj->DRV_ETHPHY_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);
    
    switch (writeState)
    {
        case DRV_LAN8840_MMD_REG_OPR_1:
            //Write to MMD Control register to set MMD Device Address
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteWaitComplete(hClientObj, PHY_MMD_ACCESS_CONTROL, (M_PHY_MMD_CNTL_ACCESS_ADDRESS_MASK | PHY_MMD_DEVICE_ADDRESS), phyAddress);
            if(res < 0)
            {   // some error
                // do nothing
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {
                writeState++;
                pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, writeState);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;

        case DRV_LAN8840_MMD_REG_OPR_2:
            // wait for write complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIOperationIsComplete(hClientObj);
            if(res < 0)
            {   // some error
                // do nothing
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {   
                // Write to MMD Address register to set Register Address for access
                res = pBaseObj->DRV_ETHPHY_VendorSMIWriteWaitComplete(hClientObj, PHY_MMD_ACCESS_DATA_ADDR, regIndex, phyAddress);
                if(res < 0)
                {   // some error
                    // do nothing
                }
                else if(res == DRV_ETHPHY_RES_OK)
                {
                    writeState++;
                    pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, writeState);
                    res = DRV_ETHPHY_RES_PENDING;
                }
            }
            break;

        case DRV_LAN8840_MMD_REG_OPR_3:
            // wait for write complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIOperationIsComplete(hClientObj);
            if(res < 0)
            {   // some error
                // do nothing
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {   
                //Write to MMD Control register to access the data
                res = pBaseObj->DRV_ETHPHY_VendorSMIWriteWaitComplete(hClientObj, PHY_MMD_ACCESS_CONTROL, (M_PHY_MMD_CNTL_ACCESS_DATA_MASK | PHY_MMD_DEVICE_ADDRESS), phyAddress);
                if(res < 0)
                {   // some error
                    // do nothing
                }
                else if(res == DRV_ETHPHY_RES_OK)
                {
                    writeState++;
                    pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, writeState);
                    res = DRV_ETHPHY_RES_PENDING;
                }    
            }
            break;
            
        case DRV_LAN8840_MMD_REG_OPR_4:
            // wait for write complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIOperationIsComplete(hClientObj);
            if(res < 0)
            {   // some error
                // do nothing
            }
            else if(res == DRV_ETHPHY_RES_OK)
            {   
                //Write to MMD Data register
                res = pBaseObj->DRV_ETHPHY_VendorSMIWriteWaitComplete(hClientObj, PHY_MMD_ACCESS_DATA_ADDR, writeIn, phyAddress);
                if(res < 0)
                {   // some error
                    // do nothing
                }
                else if(res == DRV_ETHPHY_RES_OK)
                {
                    writeState++;
                    pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, writeState);
                    res = DRV_ETHPHY_RES_PENDING;
                }    
            }
            break;
            
        case DRV_LAN8840_MMD_REG_OPR_5:
            // wait for write complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIOperationIsComplete(hClientObj);
            if(res < 0)
            {   // some error
                // do nothing
            }
            else if(res == DRV_ETHPHY_RES_OK)
            { 
                writeState = 0;
                pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, writeState); 
            }
            break;
    }
    return res;
}
#endif
