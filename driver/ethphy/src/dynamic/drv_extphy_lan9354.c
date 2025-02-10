/*******************************************************************************
  Ethernet LAN9354 PHY Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_extphy_lan9354.c

  Summary:
    LAN9354 PHY Device Driver Dynamic Implementation

  Description:
    The Ethernet PHY device driver provides a simple interface to manage the
    Ethernet PHY modules on Microchip microcontrollers.  This file Implements
    the core interface routines for the Ethernet PHY driver.

*******************************************************************************/

//DOM-IGNORE-BEGIN
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

//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include "driver/ethphy/src/drv_ethphy_local.h"
#include "system/sys_time_h2_adapter.h"
#include "system/debug/sys_debug.h"

#include "driver/ethphy/drv_extphy_lan9354.h"
#include "drv_extphy_lan9354_priv.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// Section: Driver Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

// The LAN9354 PHY driver object
// Most of the functions are the basic ones
// Functions are overrided as needed
//

// specific initialization
static SYS_MODULE_OBJ LAN9354_Initialize ( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init );

static DRV_ETHPHY_RESULT LAN9354_Setup( DRV_HANDLE  handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags);

// LAN 9354 has 2 external ports (A and B)
// The internal port always returns the link up
// The link status procedure is modified
static DRV_ETHPHY_RESULT LAN9354_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh );

// own version of the tasks function
static void LAN9354_Tasks( SYS_MODULE_OBJ object );

const DRV_ETHPHY_OBJECT_BASE  DRV_ETHPHY_OBJECT_BASE_lan9354 = 
{
     .phy_Initialize =             &LAN9354_Initialize,
     .phy_Reinitialize =           &DRV_ETHPHY_Reinitialize,
     .phy_Deinitialize =           &DRV_ETHPHY_Deinitialize,
     .phy_Status =                 &DRV_ETHPHY_Status,
     .phy_Tasks =                  &LAN9354_Tasks,
     .phy_Open =                   &DRV_ETHPHY_Open,
     .phy_Close =                  &DRV_ETHPHY_Close,
     .phy_ClientStatus =           &DRV_ETHPHY_ClientStatus,
     .phy_ClientOperationResult =  &DRV_ETHPHY_ClientOperationResult,
     .phy_ClientOperationAbort =   &DRV_ETHPHY_ClientOperationAbort,
     .phy_PhyAddressGet =          &DRV_ETHPHY_PhyAddressGet,
     .phy_Setup =                  &LAN9354_Setup,
     .phy_RestartNegotiation =     &DRV_ETHPHY_RestartNegotiation,
     .phy_HWConfigFlagsGet =       &DRV_ETHPHY_HWConfigFlagsGet,
     .phy_NegotiationIsComplete =  &DRV_ETHPHY_NegotiationIsComplete,
     .phy_NegotiationResultGet =   &DRV_ETHPHY_NegotiationResultGet,
     .phy_LinkStatusGet =          &LAN9354_LinkStatusGet,
     .phy_Reset =                  &DRV_ETHPHY_Reset,
     .phy_VendorDataGet =          &DRV_ETHPHY_VendorDataGet,
     .phy_VendorDataSet =          &DRV_ETHPHY_VendorDataSet,
     .phy_VendorSMIReadStart =     &DRV_ETHPHY_VendorSMIReadStart,
     .phy_VendorSMIReadResultGet = &DRV_ETHPHY_VendorSMIReadResultGet,
     .phy_VendorSMIWriteStart =    &DRV_ETHPHY_VendorSMIWriteStart,
     .phy_VendorSMIOperationIsComplete = &DRV_ETHPHY_VendorSMIOperationIsComplete,
};


// specific stuff
//

// number of =external ports, port A and B
#define M_LAN9354_EXT_PORTS          2U

// offset of the external port address from the virtual PHY address
#define M_LAN9354_PORT_ADD_OFFSET    1U
//  PHY_A address == Virt PhyAddress + 1
//  PHY_B address == Virt PhyAddress + 2
//  Note: No support for now if the addresses have been changed at run time!

#define M_LAN9354_BYTE_TEST_PATTERN   0x87654321U

// status of an MIIM operation initiated by LAN9354
typedef enum
{
    LAN9354_TASK_OP_IDLE    = 0,
    LAN9354_TASK_OP_SETUP_START,
    LAN9354_TASK_OP_SETUP_READ_BYTE,
    LAN9354_TASK_OP_SETUP_WAIT_BYTE,
    LAN9354_TASK_OP_SETUP_READ_HW_CFG,
    LAN9354_TASK_OP_SETUP_WAIT_HW_CFG,
    LAN9354_TASK_OP_LINK_START,
    LAN9354_TASK_OP_LINK_WAIT_READ,

}LAN9354_TASK_OP_TYPE;
#define LAN9354_TASK_OP_SETUP_END   LAN9354_TASK_OP_SETUP_WAIT_HW_CFG
#define LAN9354_TASK_OP_LINK_END    LAN9354_TASK_OP_LINK_WAIT_READ

// status of the MDIX operation initiated by LAN9354
typedef enum
{
    LAN9354_MDIX_OP_IDLE    = 0,
    LAN9354_MDIX_OP_WAIT_READ,
}LAN9354_MDIX_OP_TYPE;


static LAN9354_TASK_OP_TYPE         lan9354_task_op_type;                   // current running MIIM operation in the LAN9354 task routine 
                                                                            //
static LAN9354_MDIX_OP_TYPE         lan9354_mdix_op_type;                      // current running MDIX status
                                                                            // the MDIX routine is called in the setup procedure and cannot use the same
                                                                            // lan9354_task_op_type value 
                                                                            //
// since the MIIM operations do not occur in parallel, these variables can be shared
static DRV_MIIM_OPERATION_HANDLE    lan9354_miim_oph[M_LAN9354_EXT_PORTS];   // handle of the current MIIM operations, one for each port
static DRV_MIIM_RESULT              lan9354_miim_res[M_LAN9354_EXT_PORTS];   // result of each MIIM operation
static uint32_t                     lan9354_miim_data[M_LAN9354_EXT_PORTS];  // MIIM data read 

static DRV_ETHPHY_SETUP             lan9354_setup;                          // copy of the setup data
static TCPIP_ETH_OPEN_FLAGS*        lan9354_setup_flags;                    // pointer to the setup flags


static void F_LAN9354_MiimNewOpSet(DRV_ETHPHY_CLIENT_OBJ* hClientObj, LAN9354_TASK_OP_TYPE type);
static void F_LAN9354_MdixNewOpSet(DRV_ETHPHY_CLIENT_OBJ* hClientObj, LAN9354_MDIX_OP_TYPE type);
static void F_LAN9354_MiimOpAbort(DRV_ETHPHY_CLIENT_OBJ* hClientObj, bool mdixOp);
static void F_LAN9354_MiimOpClear(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static void F_LAN9354_SetupTask( DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_LAN9354_LinkTask( DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static __inline__ void __attribute__((always_inline)) F_LAN9354_OpTypeSet(LAN9354_TASK_OP_TYPE opStat)
{ 
    lan9354_task_op_type = opStat;
}

static __inline__ void __attribute__((always_inline)) F_LAN9354_MdixOpTypeSet(LAN9354_MDIX_OP_TYPE mdixStat)
{ 
    lan9354_mdix_op_type = mdixStat;
}

//********************** PHY object functionality

static SYS_MODULE_OBJ LAN9354_Initialize ( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init )
{
    SYS_MODULE_OBJ phyObj = DRV_ETHPHY_Initialize(index, init);
    //
    F_LAN9354_OpTypeSet(LAN9354_TASK_OP_IDLE);
    F_LAN9354_MdixOpTypeSet(LAN9354_MDIX_OP_IDLE);

    return phyObj;

}

// regular PHY task routine
// overridden here because some LAN9354 functionality is specific
static void LAN9354_Tasks( SYS_MODULE_OBJ object )
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
            if(LAN9354_TASK_OP_SETUP_START <= lan9354_task_op_type && lan9354_task_op_type <= LAN9354_TASK_OP_SETUP_END)
            {   // setup procedure
                F_LAN9354_SetupTask(hClientObj);
            }
            else if(LAN9354_TASK_OP_LINK_START <= lan9354_task_op_type && lan9354_task_op_type <= LAN9354_TASK_OP_LINK_END)
            {    // link status procedure
                F_LAN9354_LinkTask(hClientObj);
            }
            else
            {   // regular processing
                DRV_ETHPHY_Tasks(object);
            }
        }
    }
}

// to get the result of this operation the MAC driver calls DRV_ETHPHY_ClientOperationResult()
// the actual work is done in the task routine, F_LAN9354_SetupTask() in this case  
static DRV_ETHPHY_RESULT LAN9354_Setup( DRV_HANDLE  handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags)
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == NULL)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
    if(phyInst == NULL)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    // copy the setup data; pSetUp is transient!
    lan9354_setup = *pSetUp;
    lan9354_setup_flags = pSetupFlags;
    F_LAN9354_MiimNewOpSet(hClientObj, LAN9354_TASK_OP_SETUP_START);
    return DRV_ETHPHY_RES_PENDING;
}

// to get the result of this operation the MAC driver calls DRV_ETHPHY_ClientOperationResult()
// the actual work is done in the task routine, F_LAN9354_LinkTask() in this case  
static DRV_ETHPHY_RESULT LAN9354_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh )
{
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY || lan9354_task_op_type != LAN9354_TASK_OP_IDLE)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            // basic sanity check
            if(pLinkStat == NULL)
            {
                return DRV_ETHPHY_RES_OPERATION_ERR;
            }

            hClientObj->operParam = (uintptr_t)pLinkStat;
            hClientObj->operReg[0] = refresh ? 1U : 0U;
            F_ETHPHY_SetOperStart(hClientObj, PHY_CLIENT_OP_TYPE_LINK_STAT, DRV_ETHPHY_RES_PENDING);
            F_LAN9354_MiimNewOpSet(hClientObj, LAN9354_TASK_OP_LINK_START);

            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


//********************** LAN9354 object specific implementation

// task routine for checking the link status
static void F_LAN9354_SetupTask( DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    LAN9354_REG_HW_CFG_bits_t hwcfg_b;
    DRV_MIIM_RESULT miimRes;
    DRV_ETHPHY_RESULT phyRes;
    DRV_MIIM_OPERATION_HANDLE miimOpHandle;

    switch(lan9354_task_op_type)
    {
        case LAN9354_TASK_OP_SETUP_START:
            if(lan9354_setup.resetFunction != NULL)
            {
                lan9354_setup.resetFunction(hClientObj->hDriver->pBaseObj, (DRV_HANDLE)hClientObj);
                lan9354_setup.resetFunction = NULL;   // do not reset the PHY again...
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
                SYS_CONSOLE_MESSAGE("LAN9354 setup - reset the PHY\r\n");
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
            }

            F_LAN9354_OpTypeSet(LAN9354_TASK_OP_SETUP_READ_BYTE);
            break;

        case LAN9354_TASK_OP_SETUP_READ_BYTE:

            miimOpHandle = DRV_MIIM_ReadExt(hClientObj->miimHandle, (uint16_t)LAN9354_BYTE_TEST, hClientObj->smiPhyAddress, DRV_MIIM_OPERATION_FLAG_EXT_SMI_SLAVE, NULL);
            if(miimOpHandle == NULL)
            {   // read operation failed
                F_LAN9354_OpTypeSet(LAN9354_TASK_OP_IDLE);
                F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
                SYS_CONSOLE_MESSAGE("LAN9354 setup - MIIM Read BTEST FAILED - no handle!\r\n");
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
                break;
            }
            // wait the read result
            lan9354_miim_oph[0] = miimOpHandle;
            F_LAN9354_OpTypeSet(LAN9354_TASK_OP_SETUP_WAIT_BYTE);
            break;

        case LAN9354_TASK_OP_SETUP_WAIT_BYTE:
            miimRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, lan9354_miim_oph[0], lan9354_miim_data + 0);
            if((int)miimRes < 0)
            {
                F_LAN9354_OpTypeSet(LAN9354_TASK_OP_IDLE);
                F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
                SYS_CONSOLE_PRINT("LAN9354 setup - MIIM Read BTEST FAILED: res %d!\r\n", miimRes);
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
                break;
            }
            else if(miimRes == DRV_MIIM_RES_PENDING)
            {   // operation not completed; wait
                break;
            }
            else
            {
                // OK
            }

            // obtained the BYTE_TEST register
            if(lan9354_miim_data[0] != M_LAN9354_BYTE_TEST_PATTERN)
            {   // redo
                F_LAN9354_OpTypeSet(LAN9354_TASK_OP_SETUP_READ_BYTE);
                break;
            }

            // obtained the proper pattern
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
            SYS_CONSOLE_PRINT("LAN9354 setup - got BYTE TEST pattern: 0x%x!\r\n", lan9354_miim_data[0]);
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)

            F_LAN9354_OpTypeSet(LAN9354_TASK_OP_SETUP_READ_HW_CFG);
            break;

        case LAN9354_TASK_OP_SETUP_READ_HW_CFG:
            // read the HW_CFG register
            miimOpHandle = DRV_MIIM_ReadExt(hClientObj->miimHandle, (uint16_t)LAN9354_HW_CFG, hClientObj->smiPhyAddress, DRV_MIIM_OPERATION_FLAG_EXT_SMI_SLAVE, NULL);
            if(miimOpHandle == NULL)
            {   // read operation failed
                F_LAN9354_OpTypeSet(LAN9354_TASK_OP_IDLE);
                F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
                SYS_CONSOLE_MESSAGE("LAN9354 setup - MIIM Read HW_CFG FAILED - no handle!\r\n");
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
                break;
            }
            // wait the read result
            lan9354_miim_oph[0] = miimOpHandle;
            F_LAN9354_OpTypeSet(LAN9354_TASK_OP_SETUP_WAIT_HW_CFG);
            break;
            
        case LAN9354_TASK_OP_SETUP_WAIT_HW_CFG:
            miimRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, lan9354_miim_oph[0], lan9354_miim_data + 0);
            if((int)miimRes < 0)
            {
                F_LAN9354_OpTypeSet(LAN9354_TASK_OP_IDLE);
                F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
                SYS_CONSOLE_PRINT("LAN9354 setup - MIIM Read HW_CFG FAILED: res: %d!\r\n", miimRes);
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)

                break;
            }
            else if(miimRes == DRV_MIIM_RES_PENDING)
            {   // operation not completed; wait
                break;
            }
            else
            {
                // OK
            }

            // obtained the HW_CFG register
            hwcfg_b.w = lan9354_miim_data[0];
            if(hwcfg_b.READY == 0U)
            {   // re-read, wait some more... 
                F_LAN9354_OpTypeSet(LAN9354_TASK_OP_SETUP_READ_HW_CFG);
                break;
            }

            // we're good to go...
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
            SYS_CONSOLE_PRINT("LAN9354 setup - MIIM Read HW_CFG got: 0x%x\r\n", lan9354_miim_data[0]);
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
            F_LAN9354_OpTypeSet(LAN9354_TASK_OP_IDLE);
            // run the regular setup procedure now
            phyRes = DRV_ETHPHY_Setup((DRV_HANDLE)hClientObj, &lan9354_setup, lan9354_setup_flags);
            if((int)phyRes < 0)
            {   // some error occurred
                F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
                SYS_CONSOLE_PRINT("LAN9354 setup - DRV_ETHPHY_Setup FAILED: res: %d!\r\n", phyRes);
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
            }
            else
            {
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
                SYS_CONSOLE_MESSAGE("LAN9354 setup - Completed\r\n");
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_SETUP) != 0)
            }
            break;


        default:
            F_ETHPHY_AssertCond(false, __func__, __LINE__);
            break;

    }
}



// task routine for checking the link status
static void F_LAN9354_LinkTask( DRV_ETHPHY_CLIENT_OBJ * hClientObj)
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

    switch(lan9354_task_op_type)
    {
        case LAN9354_TASK_OP_LINK_START:
            // start a read of the PHY A + B status
            for(ix = 0; ix < M_LAN9354_EXT_PORTS; ix++)
            {
                miimOpHandle = DRV_MIIM_Read(hClientObj->miimHandle, (uint16_t)PHY_REG_BMSTAT, hClientObj->smiPhyAddress + ix + (uint16_t)M_LAN9354_PORT_ADD_OFFSET, DRV_MIIM_OPERATION_FLAG_NONE, NULL);
                if(miimOpHandle == NULL)
                {   // read operation failed
                    F_LAN9354_MiimOpAbort(hClientObj, false);
                    F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
                    if(errorReported == 0)
                    {
                        SYS_CONSOLE_MESSAGE("LAN9354 Link - MIIM Read BMSTAT FAILED - no handle!\r\n");
                        errorReported++;
                    }
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
                    break;
                }

                // op success
            lan9354_miim_oph[ix] = miimOpHandle;
            }

            // both op success
            F_LAN9354_OpTypeSet(LAN9354_TASK_OP_LINK_WAIT_READ);
            break;

        case LAN9354_TASK_OP_LINK_WAIT_READ:
            // wait for the operation to complete
            for(ix = 0; ix < M_LAN9354_EXT_PORTS; ix++)
            {
                if(lan9354_miim_res[ix] == DRV_MIIM_RES_PENDING)
                {   // needs to be updated
                    miimRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, lan9354_miim_oph[ix], lan9354_miim_data + ix);
                    if((int)miimRes < 0)
                    {
                        F_LAN9354_MiimOpAbort(hClientObj, false);
                        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
                        if(errorReported == 0)
                        {
                            SYS_CONSOLE_PRINT("LAN9354 Link - MIIM Read BMSTAT FAILED: res %d\r\n", miimRes);
                            errorReported++;
                        }
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
                        break;
                    }
                    else if(miimRes == DRV_MIIM_RES_PENDING)
                    {   // at least one operation not completed; wait
                        break;
                    }
                    else
                    {
                        // OK
                    }
                    // done
                    lan9354_miim_res[ix] = miimRes;
                }
            }

            // successfully read both PHYs BMSTAT
            linkStat = DRV_ETHPHY_LINK_ST_DOWN;     // if no port up
            for(ix = 0; ix < M_LAN9354_EXT_PORTS; ix++)
            {
                bmstat_b.w = (uint16_t)lan9354_miim_data[ix];
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
            F_LAN9354_MiimNewOpSet(hClientObj, LAN9354_TASK_OP_IDLE);
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_LINK) != 0)
            if(linkReported == 0)
            {
                SYS_CONSOLE_MESSAGE("LAN9354 Link: Task completed\r\n"); 
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

static void F_LAN9354_MiimOpClear(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    size_t ix;

    for(ix = 0; ix < M_LAN9354_EXT_PORTS; ix++)
    {
        lan9354_miim_oph[ix] = NULL;    // clear any previous op handle
        lan9354_miim_res[ix] = DRV_MIIM_RES_PENDING;   // set initial MIIM operation result
    }
}

// mdixOp == true for a MDIX operation
// else is a task operation
static void F_LAN9354_MiimOpAbort(DRV_ETHPHY_CLIENT_OBJ* hClientObj, bool mdixOp)
{
    size_t ix;

    for(ix = 0; ix < M_LAN9354_EXT_PORTS; ix++)
    {
        if(lan9354_miim_oph[ix] != NULL)
        {
            (void)DRV_MIIM_OperationAbort(hClientObj->miimHandle, lan9354_miim_oph[ix]);  
            lan9354_miim_oph[ix] = NULL;
        }
    }

    if(mdixOp)
    {
        F_LAN9354_MdixOpTypeSet(LAN9354_MDIX_OP_IDLE); 
    }
    else
    {
        F_LAN9354_OpTypeSet(LAN9354_TASK_OP_IDLE);
    }
}

static void F_LAN9354_MiimNewOpSet(DRV_ETHPHY_CLIENT_OBJ* hClientObj, LAN9354_TASK_OP_TYPE type)
{
    F_LAN9354_MiimOpClear(hClientObj);

    F_LAN9354_OpTypeSet(type);
}

static void F_LAN9354_MdixNewOpSet(DRV_ETHPHY_CLIENT_OBJ* hClientObj, LAN9354_MDIX_OP_TYPE type)
{
    F_LAN9354_MiimOpClear(hClientObj);

    F_LAN9354_MdixOpTypeSet(type);
}


// the vendor specific DRV_ETHPHY_OBJECT
// these routines get called by the PHY driver during the setup procedure 
// PHY driver calls from its task until they return OK or error


static DRV_ETHPHY_RESULT DRV_LAN9354_MIIConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, DRV_ETHPHY_CONFIG_FLAGS cFlags)
{
    // RMII PHY
    return (((uint8_t)cFlags & (uint8_t)DRV_ETHPHY_CFG_RMII) != 0U) ? DRV_ETHPHY_RES_OK : DRV_ETHPHY_RES_CFG_ERR;
}

static DRV_ETHPHY_RESULT DRV_LAN9354_MDIXConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE handle, TCPIP_ETH_OPEN_FLAGS oFlags)
{
    DRV_MIIM_OPERATION_HANDLE miimOpHandle;
    DRV_MIIM_RESULT miimRes;
    LAN9354_REG_CTRL_STAT_IND_bits_t  stat_ind_b;
    uint16_t or_mask;
    uint16_t ix;

    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == NULL)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(lan9354_mdix_op_type != LAN9354_MDIX_OP_IDLE && lan9354_mdix_op_type != LAN9354_MDIX_OP_WAIT_READ)
    {   // this should not happen
        F_ETHPHY_AssertCond(false, __func__, __LINE__);
        return DRV_ETHPHY_RES_OPERATION_ERR;
    } 

    if(lan9354_mdix_op_type == LAN9354_MDIX_OP_IDLE)
    {   // start a read of the PHY A + B register 27
        for(ix = 0; ix < M_LAN9354_EXT_PORTS; ix++)
        {
            miimOpHandle = DRV_MIIM_Read(hClientObj->miimHandle, (uint16_t)LAN9354_REG_CTRL_STAT_IND, hClientObj->smiPhyAddress + ix + (uint16_t)M_LAN9354_PORT_ADD_OFFSET, DRV_MIIM_OPERATION_FLAG_NONE, NULL);
            if(miimOpHandle == NULL)
            {   // read operation failed
                F_LAN9354_MiimOpAbort(hClientObj, true);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_MDIX) != 0)
                SYS_CONSOLE_MESSAGE("LAN9354 MDIX - MIIM Read STAT_IND FAILED - no handle!\r\n");
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_MDIX) != 0)
                return (DRV_ETHPHY_RES_OPERATION_ERR);
            }

            // op success
            lan9354_miim_oph[ix] = miimOpHandle;
        }

        // both op success
        F_LAN9354_MdixOpTypeSet(LAN9354_MDIX_OP_WAIT_READ);
        return DRV_ETHPHY_RES_PENDING;
    }


    // lan9354_mdix_op_type == LAN9354_MDIX_OP_WAIT_READ

    // wait for the read operations to complete
    for(ix = 0; ix < M_LAN9354_EXT_PORTS; ix++)
    {
        if(lan9354_miim_res[ix] == DRV_MIIM_RES_PENDING)
        {   // needs to be updated
            miimRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, lan9354_miim_oph[ix], lan9354_miim_data + ix);
            if((int)miimRes < 0)
            {
                F_LAN9354_MiimOpAbort(hClientObj, true);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_MDIX) != 0)
                SYS_CONSOLE_PRINT("LAN9354 MDIX - MIIM Read STAT_IND FAILED: res: %d!\r\n", miimRes);
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_MDIX) != 0)
                return DRV_ETHPHY_RES_OPERATION_ERR;
            }
            else if(miimRes == DRV_MIIM_RES_PENDING)
            {   // at least one operation not completed; wait
                return DRV_ETHPHY_RES_PENDING;
            }
            else
            {
                // OK
            }
            // done
            lan9354_miim_res[ix] = miimRes;
        }
    }

    // successfully completed the read for both PHYs SPECIAL_CTRL_STAT_IND reg
    // get the value sto be set in the SPECIAL_CTRL_STAT_IND register
    if(((uint32_t)oFlags & ((uint32_t)TCPIP_ETH_OPEN_MDIX_AUTO | (uint32_t)TCPIP_ETH_OPEN_MDIX_NORM | (uint32_t)TCPIP_ETH_OPEN_MDIX_SWAP)) == 0U)
    {
        or_mask = 0;
    }
    else
    {
        or_mask = LAN9354_AMDIXCTRL_MASK;   // enable the MDIX control
        if(((uint32_t)oFlags & (uint32_t)TCPIP_ETH_OPEN_MDIX_AUTO) != 0U)
        {
            or_mask |= LAN9354_AMDIXEN_MASK;
        }
        else if (((uint32_t)oFlags & (uint32_t)TCPIP_ETH_OPEN_MDIX_NORM) != 0U)
        {
            // manual mode, no crossover
        }
        else
        {
            or_mask |= LAN9354_AMDIXSTATE_MASK;
        }
    }

    F_LAN9354_MiimOpClear(hClientObj);
    for(ix = 0; ix < M_LAN9354_EXT_PORTS; ix++)
    {
        stat_ind_b.w = (uint16_t)lan9354_miim_data[ix];
        stat_ind_b.w &= ~(uint16_t)LAN9354_AMDIX_BITS_MASK;   // clear the MDIX bits
        stat_ind_b.w |= or_mask;                    // set the required bits

        // write data
        miimOpHandle = DRV_MIIM_Write(hClientObj->miimHandle, (uint16_t)LAN9354_REG_CTRL_STAT_IND, hClientObj->smiPhyAddress + ix + (uint16_t)M_LAN9354_PORT_ADD_OFFSET, stat_ind_b.w, DRV_MIIM_OPERATION_FLAG_DISCARD, NULL);
        if(miimOpHandle == NULL)
        {   // read operation failed
            F_LAN9354_MiimOpAbort(hClientObj, true);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_MDIX) != 0)
            SYS_CONSOLE_MESSAGE("LAN9354 MDIX - MIIM Write STAT_IND FAILED - no handle!\r\n");
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_MDIX) != 0)
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }

        // op success
        lan9354_miim_oph[ix] = miimOpHandle;
    }

    // we're done
    F_LAN9354_MdixNewOpSet(hClientObj, LAN9354_MDIX_OP_IDLE);
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_MDIX) != 0)
            SYS_CONSOLE_MESSAGE("LAN9354 MDIX - Completed\r\n");
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_MDIX) != 0)
    return DRV_ETHPHY_RES_OK;

}

static unsigned int DRV_LAN9354_SMIClockGet(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE handle)
{
    return 2500000;     //  2.5 MHz max clock supported
}



// vendor specific object
const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_LAN9354 = 
{
    .miiConfigure = &DRV_LAN9354_MIIConfigure,
    .mdixConfigure = &DRV_LAN9354_MDIXConfigure,
    .smiClockGet = &DRV_LAN9354_SMIClockGet,
    .wolConfigure = NULL,                   // no WOL functionality yet
    .phyDetect = NULL,                      // default detection performed
    .bmconDetectMask = 0,                   // standard detection mask
    .bmstatCpblMask = 0,                    // standard capabilities mask
};

/*******************************************************************************
End of File
*/
