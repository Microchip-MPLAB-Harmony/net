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

// specific initialization
static SYS_MODULE_OBJ KSZ8863_Initialize ( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init );

// KSZ8863 has 2 external ports (1 and 2)
// The internal port (3) always returns the link up
// The link status procedure is modified
static DRV_ETHPHY_RESULT    KSZ8863_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh );

// own version of the tasks function
static void KSZ8863_Tasks( SYS_MODULE_OBJ object );


DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863PhyAddressGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress);
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863RestartNegotiation( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex );
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863NegotiationIsComplete( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete );
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863NegotiationResultGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult);

const DRV_ETHPHY_OBJECT_BASE  DRV_ETHPHY_OBJECT_BASE_KSZ8863 = 
{
      .phy_Initialize =                &KSZ8863_Initialize,
      .phy_Reinitialize =              &DRV_ETHPHY_Reinitialize,
      .phy_Deinitialize =              &DRV_ETHPHY_Deinitialize,
      .phy_Status =                    &DRV_ETHPHY_Status,
      .phy_Tasks =                     &KSZ8863_Tasks,
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
      .phy_LinkStatusGet =             &KSZ8863_LinkStatusGet,
      .phy_Reset =                     &DRV_ETHPHY_Reset,
      .phy_VendorDataGet =             &DRV_ETHPHY_VendorDataGet,
      .phy_VendorDataSet =             &DRV_ETHPHY_VendorDataSet,
      .phy_VendorSMIReadStart =        &DRV_ETHPHY_VendorSMIReadStart,
      .phy_VendorSMIReadResultGet =    &DRV_ETHPHY_VendorSMIReadResultGet,
      .phy_VendorSMIWriteStart =       &DRV_ETHPHY_VendorSMIWriteStart,
};

// specific stuff
//

// number of =external ports, port 1 and 2
#define M_KSZ8863_EXT_PORTS          2U

// offset of the external port address from the PHY address (should be port 1)
#define KSZ8863_PORT_ADD_OFFSET     0U
//  PHY_1 address == 1
//  PHY_2 address == 2
//  Note: No support for now if the addresses have been changed at run time!

// status of an MIIM operation initiated by KSZ8863  
typedef enum
{
    // link status tasks
    KSZ8863_TASK_OP_IDLE    = 0,
    KSZ8863_TASK_OP_LINK_START,
    KSZ8863_TASK_OP_LINK_WAIT_READ,
    // negotiation tasks
    KSZ8863_TASK_OP_NEG_START,
    KSZ8863_TASK_OP_NEG_WAIT_READ,

}KSZ8863_TASK_OP_TYPE;
#define KSZ8863_TASK_OP_LINK_END    KSZ8863_TASK_OP_LINK_WAIT_READ
#define KSZ8863_TASK_OP_NEG_END     KSZ8863_TASK_OP_NEG_WAIT_READ


static void F_KSZ8863_MiimNewOpSet(DRV_ETHPHY_CLIENT_OBJ* hClientObj, KSZ8863_TASK_OP_TYPE type);
static void F_KSZ8863_MiimOpAbort(DRV_ETHPHY_CLIENT_OBJ* hClientObj);
static void F_KSZ8863_MiimOpClear(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_KSZ8863_LinkTask( DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_KSZ8863_NegTask( DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static KSZ8863_TASK_OP_TYPE         ksz8863_task_op_type;                   // current running MIIM operation in the KSZ8863 task routine 

// since the MIIM operations do not occur in parallel, these variables can be shared
static DRV_MIIM_OPERATION_HANDLE    ksz8863_miim_oph[M_KSZ8863_EXT_PORTS];   // handle of the current MIIM operations, one for each port
static DRV_MIIM_RESULT              ksz8863_miim_res[M_KSZ8863_EXT_PORTS];   // result of each MIIM operation
static uint32_t                     ksz8863_miim_data[M_KSZ8863_EXT_PORTS];  // MIIM data read 

static __inline__ void __attribute__((always_inline)) F_KSZ8863_OpTypeSet(KSZ8863_TASK_OP_TYPE opStat)
{ 
    ksz8863_task_op_type = opStat;
}

/****************************************************************************
 *                 interface functions
 ****************************************************************************/

//********************** PHY object functionality

static SYS_MODULE_OBJ KSZ8863_Initialize ( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init )
{
    SYS_MODULE_OBJ phyObj = DRV_ETHPHY_Initialize(index, init);
    //
    F_KSZ8863_OpTypeSet(KSZ8863_TASK_OP_IDLE);
    F_KSZ8863_MiimOpClear(NULL);

    return phyObj;
}

// regular PHY task routine
// overridden here because some KSZ8863 functionality is specific
static void KSZ8863_Tasks( SYS_MODULE_OBJ object )
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
            if(KSZ8863_TASK_OP_LINK_START <= ksz8863_task_op_type && ksz8863_task_op_type <= KSZ8863_TASK_OP_LINK_END)
            {    // link status procedure
                F_KSZ8863_LinkTask(hClientObj);
            }
            else if(KSZ8863_TASK_OP_NEG_START <= ksz8863_task_op_type && ksz8863_task_op_type <= KSZ8863_TASK_OP_NEG_END)
            {
                F_KSZ8863_NegTask(hClientObj);
            }
            else
            {   // regular processing
                DRV_ETHPHY_Tasks(object);
            }
        }
    }
}

// task routine for checking the link status
static void F_KSZ8863_LinkTask( DRV_ETHPHY_CLIENT_OBJ * hClientObj)
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

    switch(ksz8863_task_op_type)
    {
        case KSZ8863_TASK_OP_LINK_START:
            // start a read of the PHY 1 + 2 status
            opFailed = false;
            for(ix = 0; ix < M_KSZ8863_EXT_PORTS; ix++)
            {
                miimOpHandle = DRV_MIIM_Read(hClientObj->miimHandle, (uint16_t)PHY_REG_BMSTAT, hClientObj->smiPhyAddress + ix + (uint16_t)KSZ8863_PORT_ADD_OFFSET, DRV_MIIM_OPERATION_FLAG_NONE, NULL);
                if(miimOpHandle == NULL)
                {   // read operation failed
                    F_KSZ8863_MiimOpAbort(hClientObj);
                    F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
                    if(errorReported == 0)
                    {
                        SYS_CONSOLE_MESSAGE("KSZ8863 Link - MIIM Read BMSTAT FAILED - no handle!\r\n");
                        errorReported++;
                    }
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
                    opFailed = true;
                    break;
                }

                // op success
            ksz8863_miim_oph[ix] = miimOpHandle;
            }

            if(opFailed == false)
            {   // both op success
                F_KSZ8863_OpTypeSet(KSZ8863_TASK_OP_LINK_WAIT_READ);
            }
            break;

        case KSZ8863_TASK_OP_LINK_WAIT_READ:
            // wait for the operation to complete
            opFailed = false;
            opPending = false;
            for(ix = 0; ix < M_KSZ8863_EXT_PORTS; ix++)
            {
                if(ksz8863_miim_res[ix] == DRV_MIIM_RES_PENDING)
                {   // needs to be updated
                    miimRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, ksz8863_miim_oph[ix], ksz8863_miim_data + ix);
                    if((int)miimRes < 0)
                    {
                        F_KSZ8863_MiimOpAbort(hClientObj);
                        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
                        if(errorReported == 0)
                        {
                            SYS_CONSOLE_PRINT("KSZ8863 Link - MIIM Read BMSTAT FAILED: res %d\r\n", miimRes);
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
                    ksz8863_miim_res[ix] = miimRes;
                }
            }

            if(opFailed || opPending)
            {   // either error or not done
                break;
            }

            // successfully read both PHYs BMSTAT
            linkStat = DRV_ETHPHY_LINK_ST_DOWN;     // if no port up
            for(ix = 0; ix < M_KSZ8863_EXT_PORTS; ix++)
            {
                bmstat_b.w = (uint16_t)ksz8863_miim_data[ix];
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
            F_KSZ8863_MiimNewOpSet(hClientObj, KSZ8863_TASK_OP_IDLE);
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
            if(linkReported == 0)
            {
                SYS_CONSOLE_MESSAGE("KSZ8863 Link: Task completed\r\n"); 
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

// task routine for checking the negotiation status
static void F_KSZ8863_NegTask( DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_NEG) != 0)
    static int negReported = 0;
    static int errorReported = 0;
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_NEG) != 0)
        //
    DRV_MIIM_OPERATION_HANDLE miimOpHandle;
    DRV_MIIM_RESULT miimRes;
    BMSTATbits_t  bmstat_b;
    uint16_t    ix;
    bool    opFailed, opPending, negDone;

    switch(ksz8863_task_op_type)
    {
        case KSZ8863_TASK_OP_NEG_START:
            // start a read of the PHY 1 + 2 status
            opFailed = false;
            for(ix = 0; ix < M_KSZ8863_EXT_PORTS; ix++)
            {
                miimOpHandle = DRV_MIIM_Read(hClientObj->miimHandle, (uint16_t)PHY_REG_BMSTAT, hClientObj->smiPhyAddress + ix + (uint16_t)KSZ8863_PORT_ADD_OFFSET, DRV_MIIM_OPERATION_FLAG_NONE, NULL);
                if(miimOpHandle == NULL)
                {   // read operation failed
                    F_KSZ8863_MiimOpAbort(hClientObj);
                    F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_NEG) != 0)
                    if(errorReported == 0)
                    {
                        SYS_CONSOLE_MESSAGE("KSZ8863 Neg - MIIM Read BMSTAT FAILED - no handle!\r\n");
                        errorReported++;
                    }
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_NEG) != 0)
                    opFailed = true;
                    break;
                }

                // op success
                ksz8863_miim_oph[ix] = miimOpHandle;
            }

            if(opFailed == false)
            {   // both op success
                F_KSZ8863_OpTypeSet(KSZ8863_TASK_OP_NEG_WAIT_READ);
            }
            break;

        case KSZ8863_TASK_OP_NEG_WAIT_READ:
            // wait for the operation to complete
            opFailed = false;
            opPending = false;
            for(ix = 0; ix < M_KSZ8863_EXT_PORTS; ix++)
            {
                if(ksz8863_miim_res[ix] == DRV_MIIM_RES_PENDING)
                {   // needs to be updated
                    miimRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, ksz8863_miim_oph[ix], ksz8863_miim_data + ix);
                    if((int)miimRes < 0)
                    {
                        F_KSZ8863_MiimOpAbort(hClientObj);
                        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_NEG) != 0)
                        if(errorReported == 0)
                        {
                            SYS_CONSOLE_PRINT("KSZ8863 Neg - MIIM Read BMSTAT FAILED: res %d\r\n", miimRes);
                            errorReported++;
                        }
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_NEG) != 0)
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
                    ksz8863_miim_res[ix] = miimRes;
                }
            }

            if(opFailed || opPending)
            {   // either error or not done
                break;
            }

            // successfully read both PHYs BMSTAT
            negDone = false;     // if any port done
            for(ix = 0; ix < M_KSZ8863_EXT_PORTS; ix++)
            {
                bmstat_b.w = (uint16_t)ksz8863_miim_data[ix];
                if(bmstat_b.LINK_STAT != 0U && bmstat_b.AN_COMPLETE != 0U)
                {   // port done
                    negDone = true;
                    break;
                }
            }

            if(negDone == false)
            {   // restart the operation
                F_KSZ8863_MiimNewOpSet(hClientObj, KSZ8863_TASK_OP_NEG_START);
                break;
            }

            // neg done

            F_KSZ8863_MiimNewOpSet(hClientObj, KSZ8863_TASK_OP_IDLE);
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_NEG) != 0)
            if(negReported == 0)
            {
                SYS_CONSOLE_MESSAGE("KSZ8863 Neg: Task completed\r\n"); 
                negReported++;
                errorReported = 0;
            }
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_NEG) != 0)
            break;


        default:
            // should not happen
            F_ETHPHY_AssertCond(false, __func__, __LINE__);
            break;
    }
}

static void F_KSZ8863_MiimOpClear(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    size_t ix;

    for(ix = 0; ix < M_KSZ8863_EXT_PORTS; ix++)
    {
        ksz8863_miim_oph[ix] = NULL;    // clear any previous op handle
        ksz8863_miim_res[ix] = DRV_MIIM_RES_PENDING;   // set initial MIIM operation result
    }
}

static void F_KSZ8863_MiimOpAbort(DRV_ETHPHY_CLIENT_OBJ* hClientObj)
{
    size_t ix;

    for(ix = 0; ix < M_KSZ8863_EXT_PORTS; ix++)
    {
        if(ksz8863_miim_oph[ix] != NULL)
        {
            (void)DRV_MIIM_OperationAbort(hClientObj->miimHandle, ksz8863_miim_oph[ix]);  
            ksz8863_miim_oph[ix] = NULL;
        }
    }

    F_KSZ8863_OpTypeSet(KSZ8863_TASK_OP_IDLE);
}

static void F_KSZ8863_MiimNewOpSet(DRV_ETHPHY_CLIENT_OBJ* hClientObj, KSZ8863_TASK_OP_TYPE type)
{
    F_KSZ8863_MiimOpClear(hClientObj);
    F_KSZ8863_OpTypeSet(type);
}


// to get the result of this operation the MAC driver calls DRV_ETHPHY_ClientOperationResult()
// the actual work is done in the task routine, F_KSZ8863_LinkTask() in this case  
static DRV_ETHPHY_RESULT KSZ8863_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh )
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
            if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY || ksz8863_task_op_type != KSZ8863_TASK_OP_IDLE)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            hClientObj->operParam = (uintptr_t)pLinkStat;
            hClientObj->operReg[0] = refresh ? 1U : 0U;
            F_ETHPHY_SetOperStart(hClientObj, PHY_CLIENT_OP_TYPE_LINK_STAT, DRV_ETHPHY_RES_PENDING);
            F_KSZ8863_MiimNewOpSet(hClientObj, KSZ8863_TASK_OP_LINK_START);

            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
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
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY || ksz8863_task_op_type != KSZ8863_TASK_OP_IDLE)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            (void)DRV_MIIM_Write(hClientObj->miimHandle, (uint16_t)PHY_REG_BMCON, hClientObj->smiPhyAddress + 0U + (uint16_t)KSZ8863_PORT_ADD_OFFSET, (uint16_t)BMCON_AN_ENABLE_MASK | (uint16_t)BMCON_AN_RESTART_MASK, DRV_MIIM_OPERATION_FLAG_DISCARD, NULL);       
            (void)DRV_MIIM_Write(hClientObj->miimHandle, (uint16_t)PHY_REG_BMCON, hClientObj->smiPhyAddress + 1U + (uint16_t)KSZ8863_PORT_ADD_OFFSET, (uint16_t)BMCON_AN_ENABLE_MASK | (uint16_t)BMCON_AN_RESTART_MASK, DRV_MIIM_OPERATION_FLAG_DISCARD, NULL);       
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
            return DRV_ETHPHY_RES_OK;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// the actual work is done in the task routine, F_KSZ8863_NegTask() in this case  
DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863NegotiationIsComplete( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete )
{
    (void)portIndex;
    // basic sanity check
    if(waitComplete == true)
    {   // not supported 
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY || ksz8863_task_op_type != KSZ8863_TASK_OP_IDLE)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            F_ETHPHY_SetOperStart(hClientObj, PHY_CLIENT_OP_TYPE_NEG_COMPLETE, DRV_ETHPHY_RES_PENDING);
            F_KSZ8863_MiimNewOpSet(hClientObj, KSZ8863_TASK_OP_NEG_START);

            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_Ksz8863NegotiationResultGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult)
{
    pNegResult->linkStatus = DRV_ETHPHY_LINK_ST_UP;
    uint32_t linkFlags = (uint32_t)TCPIP_ETH_OPEN_AUTO | (uint32_t)TCPIP_ETH_OPEN_FDUPLEX | (uint32_t)TCPIP_ETH_OPEN_100 | (uint32_t)TCPIP_ETH_OPEN_RMII;
    pNegResult->linkFlags = (TCPIP_ETH_OPEN_FLAGS)linkFlags;
    pNegResult->pauseType = TCPIP_ETH_PAUSE_TYPE_NONE;
    return DRV_ETHPHY_RES_OK; 
}


// the vendor specific DRV_ETHPHY_OBJECT
// these routines get called by the PHY driver during the setup procedure 
// PHY driver calls from its task until they return OK or error

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

// vendor specific DRV_ETHPHY_OBJECT object
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



