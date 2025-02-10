/*******************************************************************************
  KSZ 8863 PHY API for Microchip TCP/IP Stack
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

#include "driver/ethphy/drv_extphy_ksz8863.h"
#include "drv_extphy_ksz8863_priv.h"
#include "driver/miim/drv_miim.h"

/****************************************************************************
 *                 Driver Overrides                                         *
 ****************************************************************************/

DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh );
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863PhyAddressGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress);
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863RestartNegotiation( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex );
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863NegotiationIsComplete( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete );
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863NegotiationResultGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult);

const DRV_ETHPHY_OBJECT_BASE  DRV_ETHPHY_OBJECT_BASE_KSZ8863 = 
{
      .phy_Initialize =                &DRV_ETHPHY_Initialize,
      .phy_Reinitialize =              &DRV_ETHPHY_Reinitialize,
      .phy_Deinitialize =              &DRV_ETHPHY_Deinitialize,
      .phy_Status =                    &DRV_ETHPHY_Status,
      .phy_Tasks =                     &DRV_ETHPHY_Tasks,
      .phy_Open =                      &DRV_ETHPHY_Open,
      .phy_Close =                     &DRV_ETHPHY_Close,
      .phy_ClientStatus =              &DRV_ETHPHY_ClientStatus,
      .phy_ClientOperationResult =     &DRV_ETHPHY_ClientOperationResult,
      .phy_ClientOperationAbort =      &DRV_ETHPHY_ClientOperationAbort,
      .phy_PhyAddressGet =             &DRV_ETHPHY_Ksz8863PhyAddressGet,
      .phy_Setup =                     &DRV_ETHPHY_Setup,
      .phy_RestartNegotiation =        &DRV_ETHPHY_Ksz8863RestartNegotiation,
      .phy_HWConfigFlagsGet =          &DRV_ETHPHY_HWConfigFlagsGet,
      .phy_NegotiationIsComplete =     &DRV_ETHPHY_Ksz8863NegotiationIsComplete,
      .phy_NegotiationResultGet =      &DRV_ETHPHY_Ksz8863NegotiationResultGet,
      .phy_LinkStatusGet =             &DRV_ETHPHY_Ksz8863LinkStatusGet,
      .phy_Reset =                     &DRV_ETHPHY_Reset,
      .phy_VendorDataGet =             &DRV_ETHPHY_VendorDataGet,
      .phy_VendorDataSet =             &DRV_ETHPHY_VendorDataSet,
      .phy_VendorSMIReadStart =        &DRV_ETHPHY_VendorSMIReadStart,
      .phy_VendorSMIReadResultGet =    &DRV_ETHPHY_VendorSMIReadResultGet,
      .phy_VendorSMIWriteStart =       &DRV_ETHPHY_VendorSMIWriteStart,
};

#define KSZ8863_LINK_STATUS_INDEX 0



static DRV_MIIM_OPERATION_HANDLE ksz8863OperationsHandles[3] = {NULL, NULL, NULL};
static uint16_t ksz8863OperationResults[3] = {0};


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
 * Output:          DRV_ETHPHY_RES_OK - success,
 *                  an error code otherwise
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function configures the PHY in one of MII/RMII operation modes.
 *
 *****************************************************************************/



static DRV_ETHPHY_RESULT DRV_EXTPHY_MIIConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj,DRV_ETHPHY_CONFIG_FLAGS cFlags)
{
    return DRV_ETHPHY_RES_OK;   
}


/****************************************************************************
 * Function:        DRV_EXTPHY_MDIXConfigure
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:           handle - A valid open-instance handle, returned from the driver's open routine
 *                  oFlags - the requested open flags: TCPIP_ETH_OPEN_MDIX_AUTO, TCPIP_ETH_OPEN_MDIX_NORM/TCPIP_ETH_OPEN_MDIX_SWAP
 *
 * Output:          DRV_ETHPHY_RES_OK - success,
 *                  an error code otherwise
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
    return DRV_ETHPHY_RES_OK;   

}

/****************************************************************************
 * Function:        EthPhyMIIMClock
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
const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_KSZ8863 = 
{
    .miiConfigure = &DRV_EXTPHY_MIIConfigure,
    .mdixConfigure = &DRV_EXTPHY_MDIXConfigure,
    .smiClockGet = &DRV_EXTPHY_SMIClockGet,
    .wolConfigure = NULL,                   // no WOL functionality yet
    .phyDetect = NULL,                      // default detection performed
    .bmconDetectMask = 0,                   // standard detection mask
    .bmstatCpblMask = 0,                    // standard capabilities mask
};


DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if (portIndex > DRV_ETHPHY_INF_IDX_PORT_2)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;        
    }
    
    /* Check for the Client validity */
    if(hClientObj == NULL)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }
    
    int x = 0;
    for (x = 0; x < 3; x++)
    {
        if (ksz8863OperationsHandles[x] != NULL)
        {
            uint32_t opResults;
            DRV_MIIM_RESULT mmiRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, ksz8863OperationsHandles[x], &opResults);
            if (mmiRes == DRV_MIIM_RES_OK)
            {
                ksz8863OperationResults[x] = (uint16_t)opResults;
                ksz8863OperationsHandles[x] = NULL;
            }
            else if (mmiRes != DRV_MIIM_RES_PENDING)
            {
                ksz8863OperationsHandles[x] = NULL;              
            }
            else
            {
                // OK
            }
        }
    }
    if (portIndex == DRV_ETHPHY_INF_IDX_ALL_EXTERNAL)
    {
        ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + (int)DRV_ETHPHY_INF_IDX_PORT_1 - 1] = DRV_MIIM_Read(hClientObj->miimHandle, (uint16_t)PHY_REG_BMSTAT, (uint16_t)DRV_ETHPHY_INF_IDX_PORT_1 - 1U, DRV_MIIM_OPERATION_FLAG_NONE, NULL);
        ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + (int)DRV_ETHPHY_INF_IDX_PORT_2 - 1] = DRV_MIIM_Read(hClientObj->miimHandle, (uint16_t)PHY_REG_BMSTAT, (uint16_t)DRV_ETHPHY_INF_IDX_PORT_2 - 1U, DRV_MIIM_OPERATION_FLAG_NONE, NULL);
        BMSTATbits_t b1, b2;
        b1.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + (int)DRV_ETHPHY_INF_IDX_PORT_1 - 1]);
        b2.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + (int)DRV_ETHPHY_INF_IDX_PORT_2 - 1]);
        if (b1.LINK_STAT != 0U || b2.LINK_STAT != 0U)
        {
            *pLinkStat = DRV_ETHPHY_LINK_ST_UP;
        }
        else
        {
            *pLinkStat = DRV_ETHPHY_LINK_ST_DOWN;
        }
        if (b1.REM_FAULT != 0U || b2.REM_FAULT != 0U)
        {
            uint16_t linkStat = (uint16_t)*pLinkStat;
            linkStat |= (uint16_t)DRV_ETHPHY_LINK_ST_REMOTE_FAULT;
            *pLinkStat = (DRV_ETHPHY_LINK_STATUS)linkStat;
        }
        return DRV_ETHPHY_RES_PENDING;        
    }
    ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + (int)portIndex - 1] = DRV_MIIM_Read(hClientObj->miimHandle, (uint16_t)PHY_REG_BMSTAT, (uint16_t)portIndex - 1U, DRV_MIIM_OPERATION_FLAG_NONE, NULL);
    BMSTATbits_t b1;
    b1.w = ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + (int)portIndex - 1];
    if (b1.LINK_STAT != 0U)
    {
        *pLinkStat = DRV_ETHPHY_LINK_ST_UP;
    }
    else
    {
        *pLinkStat = DRV_ETHPHY_LINK_ST_DOWN;
    }
    if (b1.REM_FAULT != 0U)
    {
        uint16_t linkStat = (uint16_t)*pLinkStat;
        linkStat |= (uint16_t)DRV_ETHPHY_LINK_ST_REMOTE_FAULT;
        *pLinkStat = (DRV_ETHPHY_LINK_STATUS)linkStat;
    }    
    return DRV_ETHPHY_RES_PENDING;            
}


DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863PhyAddressGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress)
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;
    if (portIndex > DRV_ETHPHY_INF_IDX_PORT_2)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;        
    }
    
    /* Check for the Client validity */
    if(hClientObj == NULL)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }
    if (portIndex == DRV_ETHPHY_INF_IDX_ALL_EXTERNAL)
    {
        *pPhyAddress = 1;
        return DRV_ETHPHY_RES_OK;
    }
    
    
    *pPhyAddress = (int)portIndex - 1;
    return DRV_ETHPHY_RES_OK; 
}

DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863RestartNegotiation( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;
    BMSTATbits_t b1, b2;
    b1.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + (int)DRV_ETHPHY_INF_IDX_PORT_1 - 1]);
    b2.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + (int)DRV_ETHPHY_INF_IDX_PORT_2 - 1]);
    

    if (portIndex == DRV_ETHPHY_INF_IDX_PORT_1 || portIndex == DRV_ETHPHY_INF_IDX_ALL_EXTERNAL)
    {
        if (b1.AN_ABLE == 0U)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
        (void)DRV_MIIM_Write(hClientObj->miimHandle, (uint16_t)PHY_REG_BMCON, (uint16_t)DRV_ETHPHY_INF_IDX_PORT_1 - 1U, (uint16_t)BMCON_AN_ENABLE_MASK | (uint16_t)BMCON_AN_RESTART_MASK, DRV_MIIM_OPERATION_FLAG_DISCARD, NULL);       
    }
    if (portIndex == DRV_ETHPHY_INF_IDX_PORT_2 || portIndex == DRV_ETHPHY_INF_IDX_ALL_EXTERNAL)
    {
        if (b2.AN_ABLE == 0U)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
        (void)DRV_MIIM_Write(hClientObj->miimHandle, (uint16_t)PHY_REG_BMCON, (uint16_t)DRV_ETHPHY_INF_IDX_PORT_2 - 1U, (uint16_t)BMCON_AN_ENABLE_MASK | (uint16_t)BMCON_AN_RESTART_MASK, DRV_MIIM_OPERATION_FLAG_DISCARD, NULL);       
    }
    
    return DRV_ETHPHY_RES_PENDING;
}
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863NegotiationIsComplete( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if (portIndex > DRV_ETHPHY_INF_IDX_PORT_2)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;        
    }
    
    /* Check for the Client validity */
    if(hClientObj == NULL)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }
    
    int x = 0;
    for (x = 0; x < 3; x++)
    {
        if (ksz8863OperationsHandles[x] != NULL)
        {
            uint32_t opResults;
            DRV_MIIM_RESULT mmiRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, ksz8863OperationsHandles[x], &opResults);
            if (mmiRes == DRV_MIIM_RES_OK)
            {
                ksz8863OperationResults[x] = (uint16_t)opResults;
                ksz8863OperationsHandles[x] = NULL;
            }
            else if (mmiRes != DRV_MIIM_RES_PENDING)
            {
                ksz8863OperationsHandles[x] = NULL;                
            }
            else
            {
                // OK
            }
        }
    }
    if (portIndex == DRV_ETHPHY_INF_IDX_ALL_EXTERNAL)
    {
        ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + (int)DRV_ETHPHY_INF_IDX_PORT_1 - 1] = DRV_MIIM_Read(hClientObj->miimHandle, (uint16_t)PHY_REG_BMSTAT, (uint16_t)DRV_ETHPHY_INF_IDX_PORT_1 - 1U, DRV_MIIM_OPERATION_FLAG_NONE, NULL);
        ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + (int)DRV_ETHPHY_INF_IDX_PORT_2 - 1] = DRV_MIIM_Read(hClientObj->miimHandle, (uint16_t)PHY_REG_BMSTAT, (uint16_t)DRV_ETHPHY_INF_IDX_PORT_2 - 1U, DRV_MIIM_OPERATION_FLAG_NONE, NULL);
        BMSTATbits_t b1, b2;
        b1.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + (int)DRV_ETHPHY_INF_IDX_PORT_1 - 1]);
        b2.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + (int)DRV_ETHPHY_INF_IDX_PORT_2 - 1]);
        if (b1.LINK_STAT != 0U)
        {
            if (b1.AN_COMPLETE == 0U)
            {
                return DRV_ETHPHY_RES_PENDING;
            }
        }
        if (b2.LINK_STAT != 0U)
        {
            if (b2.AN_COMPLETE == 0U)
            {
                return DRV_ETHPHY_RES_PENDING;
            }            
        }
        return DRV_ETHPHY_RES_OK;
    }
    ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + (int)portIndex - 1] = DRV_MIIM_Read(hClientObj->miimHandle, (uint16_t)PHY_REG_BMSTAT, (uint16_t)portIndex - 1U, DRV_MIIM_OPERATION_FLAG_NONE, NULL);
    BMSTATbits_t b1;
    b1.w = ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + (int)portIndex - 1];
    if (b1.LINK_STAT != 0U)
    {
        if (b1.AN_COMPLETE == 0U)
        {
            return DRV_ETHPHY_RES_PENDING;
        }
        return DRV_ETHPHY_RES_OK;
    }
    return DRV_ETHPHY_RES_PENDING;              
}

DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863NegotiationResultGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult)
{
    pNegResult->linkStatus = DRV_ETHPHY_LINK_ST_UP;
    uint32_t linkFlags = (uint32_t)TCPIP_ETH_OPEN_AUTO | (uint32_t)TCPIP_ETH_OPEN_FDUPLEX | (uint32_t)TCPIP_ETH_OPEN_100 | (uint32_t)TCPIP_ETH_OPEN_RMII;
    pNegResult->linkFlags = (TCPIP_ETH_OPEN_FLAGS)linkFlags;
    pNegResult->pauseType = TCPIP_ETH_PAUSE_TYPE_NONE;
    return DRV_ETHPHY_RES_OK; 
}
