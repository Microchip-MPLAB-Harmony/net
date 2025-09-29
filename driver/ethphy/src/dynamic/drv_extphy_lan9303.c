/*******************************************************************************
  LAN9303 PHY API for Microchip TCP/IP Stack
*******************************************************************************/

/*
Copyright (C) 2014-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#include "driver/ethphy/drv_extphy_lan9303.h"
#include "drv_extphy_lan9303_priv.h"
#include "driver/miim/drv_miim.h"

/****************************************************************************
 *                 Driver Overrides                                         *
 ****************************************************************************/

// specific initialization
static SYS_MODULE_OBJ LAN9303_Initialize ( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init );

// LAN 9303 has 2 external ports (1 and 2)
// The internal port always returns the link up
// The link status procedure is modified
static DRV_ETHPHY_RESULT    LAN9303_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh );

static DRV_ETHPHY_RESULT DRV_ETHPHY_Lan9303PhyAddressGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress);

// own version of the tasks function
static void LAN9303_Tasks( SYS_MODULE_OBJ object );

const DRV_ETHPHY_OBJECT_BASE  DRV_ETHPHY_OBJECT_BASE_lan9303 = 
{
         .phy_Initialize =             &LAN9303_Initialize,
         .phy_Reinitialize =           &DRV_ETHPHY_Reinitialize,
         .phy_Deinitialize =           &DRV_ETHPHY_Deinitialize,
         .phy_Status =                 &DRV_ETHPHY_Status,
         .phy_Tasks =                  &LAN9303_Tasks,
         .phy_Open =                   &DRV_ETHPHY_Open,
         .phy_Close =                  &DRV_ETHPHY_Close,
         .phy_ClientStatus =           &DRV_ETHPHY_ClientStatus,
         .phy_ClientOperationResult =  &DRV_ETHPHY_ClientOperationResult,
         .phy_ClientOperationAbort =   &DRV_ETHPHY_ClientOperationAbort,
         .phy_PhyAddressGet =          &DRV_ETHPHY_Lan9303PhyAddressGet,
         .phy_Setup =                  &DRV_ETHPHY_Setup,
         .phy_RestartNegotiation =     &DRV_ETHPHY_RestartNegotiation,
         .phy_HWConfigFlagsGet =       &DRV_ETHPHY_HWConfigFlagsGet,
         .phy_NegotiationIsComplete =  &DRV_ETHPHY_NegotiationIsComplete,
         .phy_NegotiationResultGet =   &DRV_ETHPHY_NegotiationResultGet,
         .phy_LinkStatusGet =          &LAN9303_LinkStatusGet,
         .phy_Reset =                  &DRV_ETHPHY_Reset,
         .phy_VendorDataGet =          &DRV_ETHPHY_VendorDataGet,
         .phy_VendorDataSet =          &DRV_ETHPHY_VendorDataSet,
         .phy_VendorSMIReadStart =     &DRV_ETHPHY_VendorSMIReadStart,
         .phy_VendorSMIReadResultGet = &DRV_ETHPHY_VendorSMIReadResultGet,
         .phy_VendorSMIWriteStart =    &DRV_ETHPHY_VendorSMIWriteStart,
};

// specific stuff
//

// number of =external ports, port 1 and 2
#define M_LAN9303_EXT_PORTS          2U

// offset of the external port address from the virtual PHY address
#define M_LAN9303_PORT_ADD_OFFSET    1U
//  PHY_1 address == Virt PhyAddress + 1
//  PHY_2 address == Virt PhyAddress + 2
//  Note: No support for now if the addresses have been changed at run time!

// status of an MIIM operation initiated by LAN9303
typedef enum
{
    LAN9303_TASK_OP_IDLE    = 0,
    LAN9303_TASK_OP_LINK_START,
    LAN9303_TASK_OP_LINK_WAIT_READ,

}LAN9303_TASK_OP_TYPE;
#define LAN9303_TASK_OP_LINK_END    LAN9303_TASK_OP_LINK_WAIT_READ

static void F_LAN9303_MiimNewOpSet(DRV_ETHPHY_CLIENT_OBJ* hClientObj, LAN9303_TASK_OP_TYPE type);
static void F_LAN9303_MiimOpAbort(DRV_ETHPHY_CLIENT_OBJ* hClientObj);
static void F_LAN9303_MiimOpClear(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_LAN9303_LinkTask( DRV_ETHPHY_CLIENT_OBJ * hClientObj);


static LAN9303_TASK_OP_TYPE         lan9303_task_op_type;                   // current running MIIM operation in the LAN9303 task routine 

// since the MIIM operations do not occur in parallel, these variables can be shared
static DRV_MIIM_OPERATION_HANDLE    lan9303_miim_oph[M_LAN9303_EXT_PORTS];   // handle of the current MIIM operations, one for each port
static DRV_MIIM_RESULT              lan9303_miim_res[M_LAN9303_EXT_PORTS];   // result of each MIIM operation
static uint32_t                     lan9303_miim_data[M_LAN9303_EXT_PORTS];  // MIIM data read 

static __inline__ void __attribute__((always_inline)) F_LAN9303_OpTypeSet(LAN9303_TASK_OP_TYPE opStat)
{ 
    lan9303_task_op_type = opStat;
}


/****************************************************************************
 *                 interface functions
 ****************************************************************************/

//********************** PHY object functionality

static SYS_MODULE_OBJ LAN9303_Initialize ( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init )
{
    SYS_MODULE_OBJ phyObj = DRV_ETHPHY_Initialize(index, init);
    //
    F_LAN9303_OpTypeSet(LAN9303_TASK_OP_IDLE);
    F_LAN9303_MiimOpClear(NULL);

    return phyObj;
}

// regular PHY task routine
// overridden here because some LAN9303 functionality is specific
static void LAN9303_Tasks( SYS_MODULE_OBJ object )
{
    /* Check for the valid driver object passed */
    union
    {
        SYS_MODULE_OBJ object;
        void* pVoid;
    }U_OBJ_VPTR;

    U_OBJ_VPTR.object = object;

    DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(U_OBJ_VPTR.pVoid);
    if(phyInst != NULL)
    {
        DRV_ETHPHY_CLIENT_OBJ* hClientObj = &phyInst->objClient;
        if(hClientObj->clientInUse != 0U)
        {   // active client
            if(LAN9303_TASK_OP_LINK_START <= lan9303_task_op_type && lan9303_task_op_type <= LAN9303_TASK_OP_LINK_END)
            {    // link status procedure
                F_LAN9303_LinkTask(hClientObj);
            }
            else
            {   // regular processing
                DRV_ETHPHY_Tasks(object);
            }
        }
    }
}

// task routine for checking the link status
static void F_LAN9303_LinkTask( DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
    static int linkReported = 0;
    static int errorReported = 0;
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
        //
    DRV_MIIM_OPERATION_HANDLE miimOpHandle;
    DRV_MIIM_RESULT miimRes;
    DRV_ETHPHY_LINK_STATUS  linkStat;
    DRV_ETHPHY_LINK_STATUS* pLinkStat;
    BMSTATbits_t  bmstat_b;
    uint16_t    ix;
    bool    opFailed, opPending;

    switch(lan9303_task_op_type)
    {
        case LAN9303_TASK_OP_LINK_START:
            // start a read of the PHY 1 + 2 status
            opFailed = false;
            for(ix = 0; ix < M_LAN9303_EXT_PORTS; ix++)
            {
                miimOpHandle = DRV_MIIM_Read(hClientObj->miimHandle, (uint16_t)PHY_REG_BMSTAT, hClientObj->smiPhyAddress + ix + (uint16_t)M_LAN9303_PORT_ADD_OFFSET, DRV_MIIM_OPERATION_FLAG_NONE, NULL);
                if(miimOpHandle == NULL)
                {   // read operation failed
                    F_LAN9303_MiimOpAbort(hClientObj);
                    F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
                    if(errorReported == 0)
                    {
                        SYS_CONSOLE_MESSAGE("LAN9303 Link - MIIM Read BMSTAT FAILED - no handle!\r\n");
                        errorReported++;
                    }
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
                    opFailed = true;
                    break;
                }

                // op success
            lan9303_miim_oph[ix] = miimOpHandle;
            }

            if(opFailed == false)
            {   // both op success
                F_LAN9303_OpTypeSet(LAN9303_TASK_OP_LINK_WAIT_READ);
            }
            break;

        case LAN9303_TASK_OP_LINK_WAIT_READ:
            // wait for the operation to complete
            opFailed = false;
            opPending = false;
            for(ix = 0; ix < M_LAN9303_EXT_PORTS; ix++)
            {
                if(lan9303_miim_res[ix] == DRV_MIIM_RES_PENDING)
                {   // needs to be updated
                    miimRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, lan9303_miim_oph[ix], lan9303_miim_data + ix);
                    if((int)miimRes < 0)
                    {
                        F_LAN9303_MiimOpAbort(hClientObj);
                        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
                        if(errorReported == 0)
                        {
                            SYS_CONSOLE_PRINT("LAN9303 Link - MIIM Read BMSTAT FAILED: res %d\r\n", miimRes);
                            errorReported++;
                        }
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
                        opFailed = true;
                        break;
                    }
                    else if(miimRes == DRV_MIIM_RES_PENDING)
                    {   // at least one operation not completed; wait
                        opPending = true;
                        break;
                    }
                    else
                    {
                        // OK
                    }
                    // done
                    lan9303_miim_res[ix] = miimRes;
                }
            }

            if(opFailed || opPending)
            {   // either error or not done
                break;
            }

            // successfully read both PHYs BMSTAT
            linkStat = DRV_ETHPHY_LINK_ST_DOWN;     // if no port up
            for(ix = 0; ix < M_LAN9303_EXT_PORTS; ix++)
            {
                bmstat_b.w = (uint16_t)lan9303_miim_data[ix];
                if(bmstat_b.LINK_STAT == 1U)
                {   // link up
                    linkStat = DRV_ETHPHY_LINK_ST_UP;
                    break;
                }
            }

            pLinkStat = (DRV_ETHPHY_LINK_STATUS*)hClientObj->operParam;
            if(pLinkStat != NULL)
            {
                *pLinkStat = linkStat;
            }
            F_LAN9303_MiimNewOpSet(hClientObj, LAN9303_TASK_OP_IDLE);
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
            if(linkReported == 0)
            {
                SYS_CONSOLE_MESSAGE("LAN9303 Link: Task completed\r\n"); 
                linkReported++;
                errorReported = 0;
            }
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
            break;


        default:
            // should not happen
            F_ETHPHY_AssertCond(false, __func__, __LINE__);
            break;
    }
}

static void F_LAN9303_MiimOpClear(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    size_t ix;

    for(ix = 0; ix < M_LAN9303_EXT_PORTS; ix++)
    {
        lan9303_miim_oph[ix] = NULL;    // clear any previous op handle
        lan9303_miim_res[ix] = DRV_MIIM_RES_PENDING;   // set initial MIIM operation result
    }
}

static void F_LAN9303_MiimOpAbort(DRV_ETHPHY_CLIENT_OBJ* hClientObj)
{
    size_t ix;

    for(ix = 0; ix < M_LAN9303_EXT_PORTS; ix++)
    {
        if(lan9303_miim_oph[ix] != NULL)
        {
            (void)DRV_MIIM_OperationAbort(hClientObj->miimHandle, lan9303_miim_oph[ix]);  
            lan9303_miim_oph[ix] = NULL;
        }
    }

    F_LAN9303_OpTypeSet(LAN9303_TASK_OP_IDLE);
}

static void F_LAN9303_MiimNewOpSet(DRV_ETHPHY_CLIENT_OBJ* hClientObj, LAN9303_TASK_OP_TYPE type)
{
    F_LAN9303_MiimOpClear(hClientObj);
    F_LAN9303_OpTypeSet(type);
}

// to get the result of this operation the MAC driver calls DRV_ETHPHY_ClientOperationResult()
// the actual work is done in the task routine, F_LAN9303_LinkTask() in this case  
static DRV_ETHPHY_RESULT LAN9303_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh )
{
    (void)portIndex;
    // basic sanity check
    if(pLinkStat == NULL)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY || lan9303_task_op_type != LAN9303_TASK_OP_IDLE)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            hClientObj->operParam = (uintptr_t)pLinkStat;
            hClientObj->operReg[0] = refresh ? 1U : 0U;
            F_ETHPHY_SetOperStart(hClientObj, PHY_CLIENT_OP_TYPE_LINK_STAT, DRV_ETHPHY_RES_PENDING);
            F_LAN9303_MiimNewOpSet(hClientObj, LAN9303_TASK_OP_LINK_START);

            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

static DRV_ETHPHY_RESULT DRV_ETHPHY_Lan9303PhyAddressGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress)
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
        *pPhyAddress = (int)hClientObj->smiPhyAddress;
        return DRV_ETHPHY_RES_OK;
    }
    
    
    *pPhyAddress = (int)hClientObj->smiPhyAddress + (int)portIndex - 1;
    return DRV_ETHPHY_RES_OK; 
}

// the vendor specific DRV_ETHPHY_OBJECT
// these routines get called by the PHY driver during the setup procedure 
// PHY driver calls from its task until they return OK or error


static DRV_ETHPHY_RESULT DRV_ETHPHY_SMC9303_VendorSMIExtWrite(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE handle, uint16_t rIx, uint16_t val);

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
#if defined(LAN9303_CMD_PROCESSOR) && (LAN9303_CMD_PROCESSOR != 0)
    sHClientObj = hClientObj;
#endif
        LAN9303_LED_CFG_t led;
        led.w = 0;
        led.LED_EN0 = 1;
        led.LED_EN1 = 1;
        led.LED_EN2 = 0;
        led.LED_EN3 = 1;
        led.LED_EN4 = 1;
        led.LED_EN5 = 0;

        led.LED_FUN = 2;

        return DRV_ETHPHY_SMC9303_VendorSMIExtWrite(pBaseObj, hClientObj, (uint16_t)PHY_REG_LED_CFG, (uint16_t)led.w);
        
        
        
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
const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_LAN9303 = 
{
    .miiConfigure = &DRV_EXTPHY_MIIConfigure,
    .mdixConfigure = &DRV_EXTPHY_MDIXConfigure,
    .smiClockGet = &DRV_EXTPHY_SMIClockGet,
    .wolConfigure = NULL,                   // no WOL functionality yet
    .phyDetect = NULL,                      // default detection performed
    .bmconDetectMask = 0,                   // standard detection mask
    .bmstatCpblMask = 0,                    // standard capabilities mask
};

#define DRV_ETHPHY_SMC32_PhyAddr(reg) ((((reg) >> 6) & 0xfU) | 0x10U)
#define DRV_ETHPHY_SMC32_RegAddr(reg) (((reg) >> 1) & 0x1fU)

static DRV_ETHPHY_RESULT DRV_ETHPHY_SMC9303_VendorSMIExtWrite(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE handle, uint16_t rIx, uint16_t val)
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    (void)DRV_MIIM_Write(hClientObj->miimHandle, DRV_ETHPHY_SMC32_RegAddr(rIx), DRV_ETHPHY_SMC32_PhyAddr(rIx), val & 0xffffU, DRV_MIIM_OPERATION_FLAG_DISCARD, NULL);       
    (void)DRV_MIIM_Write(hClientObj->miimHandle, DRV_ETHPHY_SMC32_RegAddr(rIx), DRV_ETHPHY_SMC32_PhyAddr(rIx), val & 0xffffU, DRV_MIIM_OPERATION_FLAG_DISCARD, NULL);       
    
    return DRV_ETHPHY_RES_OK;
}






