/*******************************************************************************
  KSZ 8863 PHY API for Microchip TCP/IP Stack
*******************************************************************************/

/*******************************************************************************
File Name:  drv_extphy_ksz8863.c
Copyright © 2017 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
#include "driver/ethphy/src/drv_ethphy_local.h"

#include "drv_extphy_ksz8863.h"
#include "driver/miim/drv_miim.h"

/****************************************************************************
 *                 Driver Overrides                                         *
 ****************************************************************************/

DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh );
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863PhyAddressGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress);
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863RestartNegotiation( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex );
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863NegotiationIsComplete( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete );
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863NegotiationResultGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult);

const DRV_ETHPHY_OBJECT_BASE  DRV_ETHPHY_OBJECT_BASE_ksz8863 = 
{
    DRV_ETHPHY_Initialize,
    DRV_ETHPHY_Reinitialize,
    DRV_ETHPHY_Deinitialize,
    DRV_ETHPHY_Status,
    DRV_ETHPHY_Tasks,
    DRV_ETHPHY_Open,
    DRV_ETHPHY_Close,
    DRV_ETHPHY_ClientStatus,
    DRV_ETHPHY_ClientOperationResult,
    DRV_ETHPHY_ClientOperationAbort,
    DRV_ETHPHY_SMIRead,
    DRV_ETHPHY_SMIWrite,
    DRV_ETHPHY_SMIScanStart,
    DRV_ETHPHY_SMIScanStop,
    DRV_ETHPHY_SMIScanStatusGet,
    DRV_ETHPHY_SMIScanDataGet,
    DRV_ETHPHY_SMIStatus,
    DRV_ETHPHY_SMIClockSet,
    DRV_ETHPHY_Ksz8863PhyAddressGet,
    DRV_ETHPHY_Setup,
    DRV_ETHPHY_Ksz8863RestartNegotiation,
    DRV_ETHPHY_HWConfigFlagsGet,
    DRV_ETHPHY_Ksz8863NegotiationIsComplete,
    DRV_ETHPHY_Ksz8863NegotiationResultGet,
    DRV_ETHPHY_Ksz8863LinkStatusGet,
    DRV_ETHPHY_Reset,
    DRV_ETHPHY_VendorDataGet,
    DRV_ETHPHY_VendorDataSet,
    DRV_ETHPHY_VendorSMIReadStart,
    DRV_ETHPHY_VendorSMIReadResultGet,
    DRV_ETHPHY_VendorSMIWriteStart,
};

#define KSZ8863_LINK_STATUS_INDEX 0



static DRV_MIIM_OPERATION_HANDLE ksz8863OperationsHandles[3] = {0};
static uint16_t ksz8863OperationResults[3] = {0};


/****************************************************************************
 *                 interface functions
 ****************************************************************************/


/****************************************************************************
 * Function:        DRV_EXTPHY_MIIConfigure
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:   		handle - A valid open-instance handle, returned from the driver's open routine   
 *					cFlags - the requested configuration flags: DRV_ETHPHY_CFG_RMII/DRV_ETHPHY_CFG_MII
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
 *					oFlags - the requested open flags: TCPIP_ETH_OPEN_MDIX_AUTO, TCPIP_ETH_OPEN_MDIX_NORM/TCPIP_ETH_OPEN_MDIX_SWAP
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
	return 2500000;		//  2.5 MHz max clock supported
}

// the DRV_ETHPHY_OBJECT
const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_KSZ8863 = 
{
    DRV_EXTPHY_MIIConfigure,
    DRV_EXTPHY_MDIXConfigure,
    DRV_EXTPHY_SMIClockGet,
    0,                          // no WOL functionality yet
};



DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if (portIndex > DRV_ETHPHY_INF_IDX_PORT_2)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;        
    }
    
    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }
    
    int x = 0;
    for (x = 0; x < 3; x++)
    {
        if (ksz8863OperationsHandles[x] != 0)
        {
            uint16_t opResults;
            DRV_MIIM_RESULT mmiRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, ksz8863OperationsHandles[x], &opResults);
            if (mmiRes == DRV_MIIM_RES_OK)
            {
                ksz8863OperationResults[x] = opResults;
                ksz8863OperationsHandles[x] = 0;
            }
            else if (mmiRes != DRV_MIIM_RES_PENDING)
            {
                ksz8863OperationsHandles[x] = 0;              
            }
        }
    }
    DRV_MIIM_RESULT res;
    if (portIndex == DRV_ETHPHY_INF_IDX_ALL_EXTERNAL)
    {
        DRV_MIIM_RESULT res;
        ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + DRV_ETHPHY_INF_IDX_PORT_1 - 1] = DRV_MIIM_Read(hClientObj->miimHandle, PHY_REG_BMSTAT, DRV_ETHPHY_INF_IDX_PORT_1 - 1, DRV_MIIM_OPERATION_FLAG_NONE, &res);
        ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + DRV_ETHPHY_INF_IDX_PORT_2 - 1] = DRV_MIIM_Read(hClientObj->miimHandle, PHY_REG_BMSTAT, DRV_ETHPHY_INF_IDX_PORT_2 - 1, DRV_MIIM_OPERATION_FLAG_NONE, &res);
        __BMSTATbits_t b1, b2;
        b1.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + DRV_ETHPHY_INF_IDX_PORT_1 - 1]);
        b2.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + DRV_ETHPHY_INF_IDX_PORT_2 - 1]);
        if (b1.LINK_STAT || b2.LINK_STAT)
        {
            *pLinkStat = DRV_ETHPHY_LINK_ST_UP;
        }
        else
        {
            *pLinkStat = DRV_ETHPHY_LINK_ST_DOWN;
        }
        if (b1.REM_FAULT || b2.REM_FAULT)
        {
            *pLinkStat |= DRV_ETHPHY_LINK_ST_REMOTE_FAULT;
        }
        return DRV_ETHPHY_RES_PENDING;        
    }
    ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + portIndex - 1] = DRV_MIIM_Read(hClientObj->miimHandle, PHY_REG_BMSTAT, portIndex - 1, DRV_MIIM_OPERATION_FLAG_NONE, &res);
    __BMSTATbits_t b1;
    b1.w = ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + portIndex - 1];
    if (b1.LINK_STAT)
    {
        *pLinkStat = DRV_ETHPHY_LINK_ST_UP;
    }
    else
    {
        *pLinkStat = DRV_ETHPHY_LINK_ST_DOWN;
    }
    if (b1.REM_FAULT)
    {
        *pLinkStat |= DRV_ETHPHY_LINK_ST_REMOTE_FAULT;
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
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }
    if (portIndex == DRV_ETHPHY_INF_IDX_ALL_EXTERNAL)
    {
        *pPhyAddress = 1;
        return DRV_ETHPHY_RES_OK;
    }
    
    
    *pPhyAddress = portIndex - 1;
    return DRV_ETHPHY_RES_OK; 
}

DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863RestartNegotiation( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;
    __BMSTATbits_t b1, b2;
    b1.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + DRV_ETHPHY_INF_IDX_PORT_1 - 1]);
    b2.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + DRV_ETHPHY_INF_IDX_PORT_2 - 1]);
    

    if (portIndex == DRV_ETHPHY_INF_IDX_PORT_1 || portIndex == DRV_ETHPHY_INF_IDX_ALL_EXTERNAL)
    {
        if (b1.AN_ABLE == 0)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
        DRV_MIIM_Write(hClientObj->miimHandle, PHY_REG_BMCON, DRV_ETHPHY_INF_IDX_PORT_1 - 1, _BMCON_AN_ENABLE_MASK | _BMCON_AN_RESTART_MASK, DRV_MIIM_OPERATION_FLAG_DISCARD, 0);       
    }
    if (portIndex == DRV_ETHPHY_INF_IDX_PORT_2 || portIndex == DRV_ETHPHY_INF_IDX_ALL_EXTERNAL)
    {
        if (b2.AN_ABLE == 0)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
        DRV_MIIM_Write(hClientObj->miimHandle, PHY_REG_BMCON, DRV_ETHPHY_INF_IDX_PORT_2 - 1, _BMCON_AN_ENABLE_MASK | _BMCON_AN_RESTART_MASK, DRV_MIIM_OPERATION_FLAG_DISCARD, 0);       
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
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }
    
    int x = 0;
    for (x = 0; x < 3; x++)
    {
        if (ksz8863OperationsHandles[x] != 0)
        {
            uint16_t opResults;
            DRV_MIIM_RESULT mmiRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, ksz8863OperationsHandles[x], &opResults);
            if (mmiRes == DRV_MIIM_RES_OK)
            {
                ksz8863OperationResults[x] = opResults;
                ksz8863OperationsHandles[x] = 0;
            }
            else if (mmiRes != DRV_MIIM_RES_PENDING)
            {
                ksz8863OperationsHandles[x] = 0;                
            }
        }
    }
    if (portIndex == DRV_ETHPHY_INF_IDX_ALL_EXTERNAL)
    {
        DRV_MIIM_RESULT res;
        ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + DRV_ETHPHY_INF_IDX_PORT_1 - 1] = DRV_MIIM_Read(hClientObj->miimHandle, PHY_REG_BMSTAT, DRV_ETHPHY_INF_IDX_PORT_1 - 1, DRV_MIIM_OPERATION_FLAG_NONE, &res);
        ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + DRV_ETHPHY_INF_IDX_PORT_2 - 1] = DRV_MIIM_Read(hClientObj->miimHandle, PHY_REG_BMSTAT, DRV_ETHPHY_INF_IDX_PORT_2 - 1, DRV_MIIM_OPERATION_FLAG_NONE, &res);
        __BMSTATbits_t b1, b2;
        b1.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + DRV_ETHPHY_INF_IDX_PORT_1 - 1]);
        b2.w = (ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + DRV_ETHPHY_INF_IDX_PORT_2 - 1]);
        if (b1.LINK_STAT)
        {
            if (b1.AN_COMPLETE == 0)
            {
                return DRV_ETHPHY_RES_PENDING;
            }
        }
        if (b2.LINK_STAT)
        {
            if (b2.AN_COMPLETE == 0)
            {
                return DRV_ETHPHY_RES_PENDING;
            }            
        }
        return DRV_ETHPHY_RES_OK;
    }
    DRV_MIIM_RESULT res;
    ksz8863OperationsHandles[KSZ8863_LINK_STATUS_INDEX + portIndex - 1] = DRV_MIIM_Read(hClientObj->miimHandle, PHY_REG_BMSTAT, portIndex - 1, DRV_MIIM_OPERATION_FLAG_NONE, &res);
    __BMSTATbits_t b1;
    b1.w = ksz8863OperationResults[KSZ8863_LINK_STATUS_INDEX + portIndex - 1];
    if (b1.LINK_STAT)
    {
        if (b1.AN_COMPLETE == 0)
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
    pNegResult->linkFlags = TCPIP_ETH_OPEN_AUTO | TCPIP_ETH_OPEN_FDUPLEX | TCPIP_ETH_OPEN_100 | TCPIP_ETH_OPEN_RMII;
    pNegResult->pauseType = TCPIP_ETH_PAUSE_TYPE_NONE;
    return DRV_ETHPHY_RES_OK; 
}