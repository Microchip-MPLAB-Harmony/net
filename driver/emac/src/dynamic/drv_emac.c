/*******************************************************************************
  MAC Module for Microchip TCP/IP Stack
*******************************************************************************/

/*
Copyright (C) 2020-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include "driver/emac/src/dynamic/drv_emac_lib.h"

// All pause capabilities supported
#define macDrvrPAUSE_CPBL_MASK  (TCPIP_ETH_PAUSE_TYPE_ALL)

#if defined(TCPIP_STACK_MAC_DOWN_OPERATION) && (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
#define M_MAC_DOWN_OPERATION    true
#else
#define M_MAC_DOWN_OPERATION    false
#endif  // defined(TCPIP_STACK_MAC_DOWN_OPERATION) && (TCPIP_STACK_MAC_DOWN_OPERATION != 0)


/******************************************************************************/

/*******************************************************************************
 * Local Prototypes
 ******************************************************************************/
/* MISRA C-2012 Rule 8.5 deviated:2 Deviation record ID -  H3_MISRAC_2012_R_8_5_NET_DR_17 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:2 "MISRA C-2012 Rule 8.5" "H3_MISRAC_2012_R_8_5_NET_DR_17" 
void                        EMAC0_InterruptHandler(     void );
void                        EMAC1_InterruptHandler(     void );
#pragma coverity compliance end_block "MISRA C-2012 Rule 8.5"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

// PHY related Initialization
static TCPIP_MAC_RES        macDrvrPhyInitialize(       MAC_DRIVER * pMacDrvr );
static void                 macDrvrDeInit(              MAC_DRIVER * pMacDrvr );
static void                 macDrvrTxAcknowledge(       MAC_DRIVER * pMacDrvr );
static TCPIP_MAC_RES        macDrvrTxPendingPackets(    MAC_DRIVER * pMacDrvr );
static void                 macDrvrTxGetAllPending(     MAC_DRIVER * pMacDrvr, MAC_DRVR_PACKET_LIST*  ackList );
static void                 macDrvrTxPacketPoolReset(   MAC_DRIVER * pMacDrvr, TCPIP_MAC_PKT_ACK_RES ackRes );
// MAC interface functions
#if (M_MAC_DOWN_OPERATION != 0)
static TCPIP_MAC_RES        macDrvrEventDeInit(         MAC_DRIVER * pMacDrvr );
static void                 macDrvrRxPacketPoolReset(   MAC_DRIVER * pMacDrvr );
#endif
static TCPIP_MAC_RES        macDrvrEventInit(           MAC_DRIVER * pMacDrvr, TCPIP_MAC_EventF eventF, const void * eventParam );
static MAC_DRVR_RESULT      macDrvrRemoveDynamicsFromPacketPool( MAC_DRIVER * pMacDrvr );

// Ethernet Link Start up states
static void                 macDrvrLinkStateStartLink(  MAC_DRIVER * pMacDrvr );
static void                 macDrvrLinkStateGetLink(    MAC_DRIVER * pMacDrvr );
static void                 macDrvrLinkStateWaitLinkUp( MAC_DRIVER * pMacDrvr );
static void                 macDrvrLinkStateNegComplete(MAC_DRIVER * pMacDrvr );
static void                 macDrvrLinkStateNegResult(  MAC_DRIVER * pMacDrvr );

static uint32_t             txGetValidPacketCount( TCPIP_MAC_PACKET * pMacPacket, MAC_DRIVER * pMacDrvr );

// conversion functions/helpers
//
static __inline__ TCPIP_MAC_EVENT __attribute__((always_inline)) FC_U16Ev2MacEvent(uint16_t u16Event)
{
    union
    {
        uint16_t u16Event;
        TCPIP_MAC_EVENT macEvent;
    }U_U16_MAC_EVENT;

    U_U16_MAC_EVENT.u16Event = u16Event;
    return U_U16_MAC_EVENT.macEvent;
}

static __inline__ MAC_EVENTS __attribute__((always_inline)) FC_U32Ev2MacEvent(uint32_t u32Event)
{
    union
    {
        uint32_t u32Event;
        MAC_EVENTS macEvent;
    }U_U32_MAC_EVENT;

    U_U32_MAC_EVENT.u32Event = u32Event;
    return U_U32_MAC_EVENT.macEvent;
}

static __inline__ TCPIP_ETH_OPEN_FLAGS __attribute__((always_inline)) FC_U32Flag2OpenFlags(uint32_t u32Flags)
{
    union
    {
        uint32_t u32Flags;
        TCPIP_ETH_OPEN_FLAGS opFlags;
    }U_U32_OPEN_FLAGS;

    U_U32_OPEN_FLAGS.u32Flags = u32Flags;
    return U_U32_OPEN_FLAGS.opFlags;
}

/*******************************************************************************
 * Object implementation
 ******************************************************************************/
// the embedded object
const TCPIP_MAC_OBJECT DRV_EMAC0_Object =
{
    .macId                            = (uint16_t)TCPIP_MODULE_MAC_SAM9X60_0,
    .macType                          = (uint8_t)TCPIP_MAC_TYPE_ETH,
    .macName                          = "EMAC0",
    .MAC_Initialize                   = &MAC_DRVR_Initialize,
    .MAC_Deinitialize                 = &MAC_DRVR_Deinitialize,
    .MAC_Reinitialize                 = &MAC_DRVR_Reinitialize,
    .MAC_Status                       = &MAC_DRVR_Status,
    .MAC_Tasks                        = &MAC_DRVR_Tasks,
    .MAC_Open                         = &MAC_DRVR_Open,
    .MAC_Close                        = &MAC_DRVR_Close,
    .MAC_LinkCheck                    = &MAC_DRVR_LinkCheck,
    .MAC_RxFilterHashTableEntrySet    = &MAC_DRVR_RxFilterHashTableEntrySet,
    .MAC_PowerMode                    = &MAC_DRVR_PowerMode,
    .MAC_PacketTx                     = &MAC_DRVR_PacketTx,
    .MAC_PacketRx                     = &MAC_DRVR_PacketRx,
    .MAC_Process                      = &MAC_DRVR_Process,
    .MAC_StatisticsGet                = &MAC_DRVR_StatisticsGet,
    .MAC_ParametersGet                = &MAC_DRVR_ParametersGet,
    .MAC_RegisterStatisticsGet        = &MAC_DRVR_RegisterStatisticsGet,
    .MAC_ConfigGet                    = &MAC_DRVR_ConfigGet,
    .MAC_EventMaskSet                 = &MAC_DRVR_EventMaskSet,
    .MAC_EventAcknowledge             = &MAC_DRVR_EventAcknowledge,
    .MAC_EventPendingGet              = &MAC_DRVR_EventPendingGet,
};

const TCPIP_MAC_OBJECT DRV_EMAC1_Object =
{
    .macId                            = (uint16_t)TCPIP_MODULE_MAC_SAM9X60_1,
    .macType                          = (uint8_t)TCPIP_MAC_TYPE_ETH,
    .macName                          = "EMAC1",
    .MAC_Initialize                   = &MAC_DRVR_Initialize,
    .MAC_Deinitialize                 = &MAC_DRVR_Deinitialize,
    .MAC_Reinitialize                 = &MAC_DRVR_Reinitialize,
    .MAC_Status                       = &MAC_DRVR_Status,
    .MAC_Tasks                        = &MAC_DRVR_Tasks,
    .MAC_Open                         = &MAC_DRVR_Open,
    .MAC_Close                        = &MAC_DRVR_Close,
    .MAC_LinkCheck                    = &MAC_DRVR_LinkCheck,
    .MAC_RxFilterHashTableEntrySet    = &MAC_DRVR_RxFilterHashTableEntrySet,
    .MAC_PowerMode                    = &MAC_DRVR_PowerMode,
    .MAC_PacketTx                     = &MAC_DRVR_PacketTx,
    .MAC_PacketRx                     = &MAC_DRVR_PacketRx,
    .MAC_Process                      = &MAC_DRVR_Process,
    .MAC_StatisticsGet                = &MAC_DRVR_StatisticsGet,
    .MAC_ParametersGet                = &MAC_DRVR_ParametersGet,
    .MAC_RegisterStatisticsGet        = &MAC_DRVR_RegisterStatisticsGet,
    .MAC_ConfigGet                    = &MAC_DRVR_ConfigGet,
    .MAC_EventMaskSet                 = &MAC_DRVR_EventMaskSet,
    .MAC_EventAcknowledge             = &MAC_DRVR_EventAcknowledge,
    .MAC_EventPendingGet              = &MAC_DRVR_EventPendingGet,
};

// the embedded descriptor to support multiple instances.  Create an array/list
// of MAC_DCPT structures or allocate dynamically
#define DRV_EMAC_INSTANCES      2
static MAC_DRIVER macDrvrDescription[ DRV_EMAC_INSTANCES ];

static const TCPIP_MAC_OBJECT* macDrvrObjects[ DRV_EMAC_INSTANCES ] =
{
    &DRV_EMAC0_Object,
    &DRV_EMAC1_Object,
};

// convert id to index
static __inline__ int __attribute__((always_inline)) macIdToIndex( TCPIP_MODULE_MAC_ID macId )
{
    int macIndex = (int)macId - (int)TCPIP_MODULE_MAC_SAM9X60;

    if( macIndex >= 0 && (size_t)macIndex < sizeof( macDrvrDescription ) / sizeof( *macDrvrDescription ) )
    {
        return macIndex;
    }
    return -1;
}

#if (DRV_EMAC_INSTANCES == 1)
static __inline__ MAC_DRIVER* __attribute__((always_inline)) F_EmacHandleToMacDrv(uintptr_t handle)
{
    MAC_DRIVER* pMacD = (MAC_DRIVER*)handle;
    return pMacD == macDrvrDescription ? pMacD : 0;
}
#elif (DRV_EMAC_INSTANCES == 2)
static __inline__ MAC_DRIVER* __attribute__((always_inline)) F_EmacHandleToMacDrv(uintptr_t handle)
{
    MAC_DRIVER* pMacD = (MAC_DRIVER*)handle;
    return (pMacD == macDrvrDescription || pMacD == macDrvrDescription + 1) ? pMacD : NULL;
}
#else
// multiple instances version
static MAC_DRIVER* F_EmacHandleToMacDrv(uintptr_t handle)
{
    MAC_DRIVER* pMacD = (MAC_DRIVER*)handle;
    int macIx = pMacD - macDrvrDescription;

    if( macIx >= 0 && (size_t)macIx < sizeof( macDrvrDescription ) / sizeof( *macDrvrDescription ) )
    {
        if(pMacD == macDrvrDescription + macIx)
        {
            return pMacD;
        }
    }

    return NULL;
}

#endif


/*******************************************************************************
 * Function:        MAC_DRVR_Initialize
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        This function initializes the MAC controller, the MAC and the associated PHY.
 *                  It should be called to be able to schedule any MAC transmit or receive operation.
 * Note:            Only one client per MAC supported.
 ******************************************************************************/
SYS_MODULE_OBJ MAC_DRVR_Initialize( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const  init)
{
    union
    {
        const SYS_MODULE_INIT*  modInit;
        const TCPIP_MAC_INIT*   macInit;

    }U_MOD_INIT_MAC_INIT;

    U_MOD_INIT_MAC_INIT.modInit = init;

    const TCPIP_MAC_MODULE_CTRL * const     macControl = U_MOD_INIT_MAC_INIT.macInit->macControl;
    const TCPIP_MODULE_MAC_SAM9X60_CONFIG * initData = (const TCPIP_MODULE_MAC_SAM9X60_CONFIG *)U_MOD_INIT_MAC_INIT.macInit->moduleData;
    MAC_DRIVER *    pMacDrvr;
    int             macIndex;
    TCPIP_MAC_RES   initRes;
    MAC_DRVR_RESULT macRes;

    macIndex = macIdToIndex( (TCPIP_MODULE_MAC_ID)index );
    if(macIndex < 0)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    pMacDrvr = macDrvrDescription + macIndex;

    if( SYS_STATUS_UNINITIALIZED != pMacDrvr->sysStat )
    {   // already initialized
        return (SYS_MODULE_OBJ) pMacDrvr;
    }
    if( pMacDrvr->macFlags.open != 0U )
    {   // have a client connected
        return SYS_MODULE_OBJ_INVALID;
    }
    if( macControl->memH == NULL)
    {     // not possible without dynamic memory
        return SYS_MODULE_OBJ_INVALID;
    }
    if( initData == NULL )
    {   // not possible without init data!
        return SYS_MODULE_OBJ_INVALID;
    }
    if( (initData->pPhyBase == NULL) || (initData->pPhyInit == NULL) )
    {   // not possible without a PHY!
        return SYS_MODULE_OBJ_INVALID;
    }
    if(     (initData->nRxDescCnt < 1U)
       ||   (initData->nTxDescCnt < (uint16_t)EMAC_MIN_TX_DESCRIPTORS)
    )
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    if( initData->rxBufferSize < (uint16_t)EMAC_MIN_RX_SIZE )
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    if( !macDrvrRxCreate( pMacDrvr ) )
    {   // failed to create synch lock
        return SYS_MODULE_OBJ_INVALID;
    }
    if( !macDrvrTxCreate( pMacDrvr ) )
    {   // failed to create synch lock
        return SYS_MODULE_OBJ_INVALID;
    }
    // initialize the MAC object
    (void)memset( pMacDrvr, 0x0, sizeof( *pMacDrvr ) );

    pMacDrvr->pObj = macDrvrObjects[macIndex];
    pMacDrvr->hPhyClient = DRV_HANDLE_INVALID;
    pMacDrvr->hPhySysObject = SYS_MODULE_OBJ_INVALID;
    pMacDrvr->sysStat = SYS_STATUS_UNINITIALIZED;

    pMacDrvr->macIx = (uint32_t)macIndex;
    pMacDrvr->phyIx = (uint32_t)macIndex;     // use the same index for the associated PHY
    pMacDrvr->macFlags.linkPrev = 0U;

    // use initialization data
    pMacDrvr->allocH = macControl->memH;
    pMacDrvr->callBack.callocF = macControl->callocF;
    pMacDrvr->callBack.freeF = macControl->freeF;
    pMacDrvr->callBack.pktAllocF = macControl->pktAllocF;
    pMacDrvr->callBack.pktFreeF = macControl->pktFreeF;
    pMacDrvr->callBack.pktAckF = macControl->pktAckF;
    pMacDrvr->callBack.synchF = macControl->synchF;

    pMacDrvr->gapDcptOffset = macControl->gapDcptOffset;
    pMacDrvr->gapDcptSize = macControl->gapDcptSize;
    pMacDrvr->controlFlags = macControl->controlFlags;
    pMacDrvr->dataOffset = (macControl->controlFlags & (uint16_t)TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2) != 0U ? 2U : 0U;

    // copy the configuration data
    pMacDrvr->config = *initData;
    // set the MAC address received from system config
    (void)memcpy( pMacDrvr->config.macAddress.v, macControl->ifPhyAddress.v, sizeof( macControl->ifPhyAddress ));

    (void)SYS_INT_SourceDisable( pMacDrvr->config.macIntSrc );    // disable interrupt vector
    macDrvrLibInitializeEmac( pMacDrvr );                   // stop and configure mac registers
    initRes = macDrvrEventInit( pMacDrvr, macControl->eventF, macControl->eventParam);
    if( TCPIP_MAC_RES_OK == initRes )
    {
        macDrvrLibDescriptorsPoolClear();
        macDrvrLibDescriptorsPoolAssignment( pMacDrvr, MAC_DRVR_DCPT_TYPE_TX );
        macDrvrLibDescriptorsPoolAssignment( pMacDrvr, MAC_DRVR_DCPT_TYPE_RX );

        initRes = macDrvrPhyInitialize( pMacDrvr );
    }
    if( TCPIP_MAC_RES_OK == initRes )
    {
        macRes = macDrvrLibTxInit( pMacDrvr );
        if( MAC_DRVR_RES_OK == macRes )
        {
            macRes = macDrvrLibRxInit( pMacDrvr );
        }
        if( MAC_DRVR_RES_OK == macRes )
        {
            macRes = macDrvrLibRxAllocate( pMacDrvr, pMacDrvr->config.nRxStaticBufferCnt, MAC_RX_STICKY_BUFFERS);
        }
        if( MAC_DRVR_RES_OK == macRes )
        {
            macRes = macDrvrLibGetDynamicBuffers( pMacDrvr );
        }
        if( MAC_DRVR_RES_OK == macRes )
        {
            macRes = macDrvrLibRxPopulateDescriptors( pMacDrvr );
        }
    }

    if( initRes == TCPIP_MAC_RES_OK  && macRes == MAC_DRVR_RES_OK)
    {
        // remaining initialization is done by DRV_EMAC_MACTasks
        pMacDrvr->sysStat = SYS_STATUS_BUSY;
        SYS_INT_SourceStatusClear( pMacDrvr->config.macIntSrc );    // clear any pending interrupt flag
        SYS_INT_SourceEnable( pMacDrvr->config.macIntSrc );         // enable interrupt vector
     // macDrvrLibTransferEnable( pMacDrvr );                       // called by DRV_EMAC_MACTasks
        return (SYS_MODULE_OBJ) pMacDrvr;
    }
    else
    {   // failed; release what we've allocated
        macDrvrDeInit( pMacDrvr );
        return SYS_MODULE_OBJ_INVALID;
    }
}

#if (M_MAC_DOWN_OPERATION != 0)
/*******************************************************************************
 * Function:        MAC_DRVR_Deinitialize
 * PreCondition:    None
 * Input:           stackData - standard stack initialization structure
 * Output:          TCPIP_MAC_RES
 * Side Effects:    None
 * Overview:        This function de-initializes the Eth MAC controller.
 * Note:            None
 ******************************************************************************/
void MAC_DRVR_Deinitialize( SYS_MODULE_OBJ object )
{
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( object );
    if(pMacDrvr != NULL)
    {
        if( pMacDrvr->sysStat == SYS_STATUS_BUSY || pMacDrvr->sysStat == SYS_STATUS_READY || pMacDrvr->sysStat == SYS_STATUS_ERROR)
        {
            macDrvrDeInit( pMacDrvr );
        }
    }
}
#else
void MAC_DRVR_Deinitialize( SYS_MODULE_OBJ object )
{
}
#endif


void MAC_DRVR_Reinitialize( SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init )
{
    // not supported
}

SYS_STATUS MAC_DRVR_Status( SYS_MODULE_OBJ object )
{
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( object );
    if(pMacDrvr != NULL)
    {
        return pMacDrvr->sysStat;
    }

    return SYS_STATUS_ERROR;
}

void MAC_DRVR_Tasks( SYS_MODULE_OBJ object )
{
    TCPIP_ETH_PAUSE_TYPE            pauseType;
    DRV_ETHPHY_CLIENT_STATUS        phyStat;
    DRV_HANDLE                      hPhyClient;
    DRV_ETHPHY_RESULT               phyInitRes;
    const DRV_ETHPHY_OBJECT_BASE *  pPhyBase;
    static bool                     discardFlag = false;

    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( object );
    if(pMacDrvr == NULL || ( SYS_STATUS_UNINITIALIZED == pMacDrvr->sysStat ))
    {   // nothing to do
        return;
    }

    pPhyBase = pMacDrvr->config.pPhyBase;

    if((pMacDrvr->controlFlags & (uint16_t)TCPIP_MAC_CONTROL_NO_LINK_CHECK) == 0U)
    {
        pPhyBase->phy_Tasks( pMacDrvr->hPhySysObject );

        if( ((uint32_t)DRV_ETHPHY_LINK_ST_UP & (uint32_t)pMacDrvr->linkInfo.negResult.linkStatus) == 0U)
        {
            if( discardFlag == true )
            {
                pMacDrvr->txStat.linkLossResets++;
                macDrvrTxPacketPoolReset( pMacDrvr, TCPIP_MAC_PKT_ACK_LINK_DOWN );
                discardFlag = false;
            }
        }
        else
        {
            discardFlag = true;
        }
    }

    switch( pMacDrvr->sysStat )
    {
        case SYS_STATUS_BUSY:
            // perform the PHY initialization
            hPhyClient = pMacDrvr->hPhyClient;
            phyStat = pPhyBase->phy_ClientStatus( hPhyClient );
            if( phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY )
            {   // wait some more
                break;
            }

            phyInitRes = pPhyBase->phy_ClientOperationResult( hPhyClient );
            if ( phyInitRes != DRV_ETHPHY_RES_OK )
            {
                macDrvrDeInit( pMacDrvr );
                // keep the error status though
                pMacDrvr->sysStat = SYS_STATUS_ERROR;
                SYS_ERROR_PRINT( SYS_ERROR_ERROR, "DRV PHY init failed: %d\r\n", phyInitRes );
                break;
            }

            // PHY was detected properly
            pMacDrvr->macFlags.linkPresent = 1U;
            if( ((uint32_t)pMacDrvr->config.ethFlags & (uint32_t)TCPIP_ETH_OPEN_AUTO) != 0U)
            {   // we'll just wait for the negotiation to be done
                pMacDrvr->macFlags.linkNegotiation = 1U;       // performing the negotiation
            }
            else
            {   // no need of negotiation results; just update the MAC
                pauseType = ((uint32_t)pMacDrvr->config.ethFlags & (uint32_t)TCPIP_ETH_OPEN_FDUPLEX) != 0U ? macDrvrPAUSE_CPBL_MASK : TCPIP_ETH_PAUSE_TYPE_NONE;
                macDrvrLibMacOpen( pMacDrvr, pMacDrvr->linkInfo.flags, pauseType );
            }

            pMacDrvr->linkInfo.upTick = SYS_TMR_TickCountGet();     // the initialization time
            pMacDrvr->linkInfo.waitTick =
                ((SYS_TMR_TickCounterFrequencyGet() * pMacDrvr->config.linkInitDelay) + 999U) / 1000U;
            pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_START_LINK;

            pMacDrvr->sysStat = SYS_STATUS_READY;
            break;

        case SYS_STATUS_ERROR_EXTENDED:
        case SYS_STATUS_ERROR:
        case SYS_STATUS_UNINITIALIZED:
        case SYS_STATUS_READY:
        case SYS_STATUS_READY_EXTENDED:
        default:
            // do nothing
            break;
    }
}

size_t MAC_DRVR_ConfigGet( DRV_HANDLE  hMac, void *  configBuff, size_t  buffSize, size_t *  pConfigSize)
{
    size_t          response = 0;
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    if(pMacDrvr != NULL)
    {
        if( pConfigSize != NULL)
        {
            *pConfigSize = sizeof( TCPIP_MODULE_MAC_SAM9X60_CONFIG );
        }
        if( configBuff != NULL && buffSize >= sizeof( TCPIP_MODULE_MAC_SAM9X60_CONFIG ) )
        {   // can copy the data
            TCPIP_MODULE_MAC_SAM9X60_CONFIG * pMacConfig = (TCPIP_MODULE_MAC_SAM9X60_CONFIG *) configBuff;

            *pMacConfig = pMacDrvr->config;
            response = sizeof( TCPIP_MODULE_MAC_SAM9X60_CONFIG );
        }
    }
    return response;
}

DRV_HANDLE MAC_DRVR_Open( const SYS_MODULE_INDEX  drvIndex, const DRV_IO_INTENT  intent)
{
    int             macIndex;
    MAC_DRIVER *    pMacDrvr;
    DRV_HANDLE      hMac = DRV_HANDLE_INVALID;

    macIndex = macIdToIndex( (TCPIP_MODULE_MAC_ID)drvIndex );
    if( macIndex >= 0 )
    {
        pMacDrvr = macDrvrDescription + macIndex;
        switch( pMacDrvr->sysStat )
        {
            case SYS_STATUS_UNINITIALIZED:
            case SYS_STATUS_READY:
            case SYS_STATUS_BUSY:
                if( pMacDrvr->macFlags.open == 0U )
                {   // only one client for now
                    pMacDrvr->macFlags.open = 1U;
                    hMac = (DRV_HANDLE) pMacDrvr;
                }
#if defined(macDrvrCLIENTS_NUMBER) && (macDrvrCLIENTS_NUMBER > 1)
                else
                {   // allow multiple clients
                    hMac = (DRV_HANDLE) pMacDrvr;
                }
#endif  // defined(macDrvrCLIENTS_NUMBER) && (macDrvrCLIENTS_NUMBER > 1)
                break;
            case SYS_STATUS_ERROR_EXTENDED:
            case SYS_STATUS_ERROR:
            default:
                // do nothing
                break;
        }
    }
    return hMac;
}

void MAC_DRVR_Close( DRV_HANDLE hMac )
{
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    if(pMacDrvr != NULL)
    {
        pMacDrvr->macFlags.open = 0U;
    }
}

/*******************************************************************************
 * TX functions
 */
static uint32_t txGetValidPacketCount( TCPIP_MAC_PACKET * pMacPacket, MAC_DRIVER * pMacDrvr )
{
    TCPIP_MAC_PACKET *  pTemp;
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    uint32_t            packetCount = 0;

    for( pTemp = pMacPacket; pTemp != NULL; pTemp = pTemp->next )
    {
        pSeg = pTemp->pDSeg;
        if(pSeg == NULL || pSeg->segLen == 0U)
        {   // having data is the only integrity check for validity
            // one bad apple ruins the barrel
            packetCount = 0U;
            break;
        }
        ++packetCount;
    }
    return packetCount;
}

TCPIP_MAC_RES MAC_DRVR_PacketTx( DRV_HANDLE hMac, TCPIP_MAC_PACKET * pMacPacket )
{
    TCPIP_MAC_PACKET *  pTemp;
    TCPIP_MAC_RES       response = TCPIP_MAC_RES_PACKET_ERR;
    uint32_t            numPacketsToSend = 0;

    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    if(pMacDrvr == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    numPacketsToSend = txGetValidPacketCount( pMacPacket, pMacDrvr );

    if( numPacketsToSend != 0U)
    {
        response = TCPIP_MAC_RES_OK;

        macDrvrTxLock( pMacDrvr );
        pTemp = pMacPacket;
        while(numPacketsToSend-- != 0U)  
        {   // Update pktFlags
            // packets are expected to have pDSeg updated with the packet pointer
            pTemp->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
            pTemp = pTemp->next;
        }
        __DSB();
        macDrvrPacketListTailAdd(   &pMacDrvr->txPendingList, pMacPacket);
        (void)macDrvrTxPendingPackets( pMacDrvr );
        macDrvrTxUnlock( pMacDrvr );

        macDrvrTxAcknowledge( pMacDrvr );
    }

    return response;
}

/*******************************************************************************
 * RX functions
 */
// returns a pending RX packet if any have been captured by the mac dma
TCPIP_MAC_PACKET * MAC_DRVR_PacketRx( DRV_HANDLE  hMac, TCPIP_MAC_RES *  pRes, TCPIP_MAC_PACKET_RX_STAT *  pPktStatus)
{
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    if(pMacDrvr == NULL)
    {
        return NULL;
    }

    TCPIP_MAC_PACKET *      pMacPacket = NULL;
    union
    {
        MAC_DRVR_RXDCPT_STATUS  packetStatus;
        TCPIP_MAC_PACKET_RX_STAT rxStat;
    }U_MAC_PIC32C_RX_STAT;

    TCPIP_MAC_RES           response = TCPIP_MAC_RES_PENDING;

    U_MAC_PIC32C_RX_STAT.packetStatus.val = 0;
    macDrvrRxLock( pMacDrvr );
    if( MAC_DRVR_RES_OK == rxGetPacket( pMacDrvr, &pMacPacket, &U_MAC_PIC32C_RX_STAT.packetStatus)
    )
    {   // valid ethernet packet;
        // format properly and return the packet pointer
        // Ethernet header(14 bytes) stored in data packet (or in first data buffer for multi-buffer data).
        // Reduce header length to get data segment length
        response = TCPIP_MAC_RES_OK;
        pMacDrvr->rxStat.nRxOkPackets++;
        // set proper segment length
        pMacPacket->pDSeg->segLen -= (uint16_t)sizeof( TCPIP_MAC_ETHERNET_HEADER );
        // Note: re-set pMacLayer and pNetLayer; IPv6 changes these pointers inside the packet!
        pMacPacket->pMacLayer = pMacPacket->pDSeg->segLoad;
        pMacPacket->pNetLayer = pMacPacket->pMacLayer + sizeof( TCPIP_MAC_ETHERNET_HEADER );
        pMacPacket->tStamp = SYS_TMR_TickCountGet();
        pMacPacket->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
        pMacPacket->pktFlags &= ~(uint32_t)TCPIP_MAC_PKT_FLAG_CAST_MASK;
        if( U_MAC_PIC32C_RX_STAT.packetStatus.bm.bBroadcastDetected != 0U)
        {
            pMacPacket->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_BCAST;
        }
        else if( U_MAC_PIC32C_RX_STAT.packetStatus.bm.bMulticastHashMatch != 0U)
        {
            pMacPacket->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_MCAST;
        }
        else if( U_MAC_PIC32C_RX_STAT.packetStatus.bm.bUnicastHashMatch != 0U)
        {
            pMacPacket->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_UNICAST;
        }
        else
        {
            // do nothing
        }
        __DSB();
    }
    macDrvrRxUnlock( pMacDrvr );
    if( pRes != NULL)
    {
        *pRes = response;
    }
    if( pPktStatus != NULL)
    {
        (void)memcpy(pPktStatus, &U_MAC_PIC32C_RX_STAT.rxStat, sizeof(TCPIP_MAC_PACKET_RX_STAT_PIC32C));
    }

    return pMacPacket;
}

/*******************************************************************************
 * Function:        TCPIP_MAC_RES MAC_DRVR_RxFilterHashTableEntrySet( *  DRV_HANDLE hMac, *  TCPIP_MAC_ADDR * destMacAddr)
 *
 * PreCondition:    MAC_DRVR_Initialize() should have been called.
 *
 * Input:           destMacAddr: 6 byte group destination MAC address to allow
 *                  through the Hash Table Filter.  If destMacAddr
 *                  is set to 00-00-00-00-00-00, then the hash
 *                  table will be cleared of all entries and the
 *                  filter will be disabled.
 *
 * Output:          Sets the appropriate bit in the GMAC_HRB/GMAC_HRT registers to allow
 *                  packets sent to destMacAddr to be received and enabled the
 *                  Hash Table receive filter (if not already).
 *
 *                  Returns TCPIP_MAC_RES_OK if success,
 *                  an eror code otherwise
 *
 * Side Effects:    None
 *
 * Note:            There is no way to individually unset destination MAC
 *                  addresses from the hash table since it is possible to have
 *                  a hash collision and therefore multiple MAC addresses
 *                  relying on the same hash table bit.  The stack would have
 *                  to individually store each 6 byte MAC address to support
 *                  this feature, which would waste a lot of RAM and be
 *                  unnecessary in most applications.  As a simple compromise,
 *                  you can call MAC_DRVR_RxFilterHashTableEntrySet() using a
 *                  00-00-00-00-00-00 destination MAC address, which will clear
 *                  the entire hash table and disable the hash table filter.
 *                  This will allow you to then read the necessary destination
 *                  addresses.
 ******************************************************************************/
TCPIP_MAC_RES MAC_DRVR_RxFilterHashTableEntrySet( DRV_HANDLE  hMac, const TCPIP_MAC_ADDR *  destMacAddr)
{
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    if(pMacDrvr == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    MAC_DRVR_HASH   hash;
    uint8_t         nullMacAddr[ 6 ] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Clear the Hash Table bits and disable the Hash Table Filter if a special
    // 00-00-00-00-00-00 destination MAC address is provided.
    (void)memset(&hash, 0x0, sizeof(hash));
    if(destMacAddr != NULL && memcmp(destMacAddr->v, nullMacAddr, sizeof( nullMacAddr )) != 0)
    {
        hash.destMacAddr = destMacAddr;
        hash.calculate = 0U;      // calculate hash and set hash register
    }
    (void)macDrvrLibRxFilterHashCalculate( pMacDrvr, &hash );

    return TCPIP_MAC_RES_OK;
}

bool MAC_DRVR_PowerMode( DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode )
{   // not supported
    return true;
}

TCPIP_MAC_RES MAC_DRVR_Process( DRV_HANDLE hMac )
{

    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    if(pMacDrvr == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    macDrvrTxLock( pMacDrvr );
    TCPIP_MAC_RES response = macDrvrTxPendingPackets( pMacDrvr );
    macDrvrTxUnlock( pMacDrvr );

    macDrvrTxAcknowledge( pMacDrvr );

    return response;
}

TCPIP_MAC_RES MAC_DRVR_ParametersGet( DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS * pMacParams )
{
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    if(pMacDrvr == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    TCPIP_MAC_RES   response = TCPIP_MAC_RES_IS_BUSY;
    if( pMacDrvr->sysStat == SYS_STATUS_READY )
    {
        if( pMacParams != NULL)
        {
            (void)macDrvrLibGetMacAddr( pMacDrvr, pMacParams->ifPhyAddress.v );
            uint16_t procFlags = (uint16_t)TCPIP_MAC_PROCESS_FLAG_RX | (uint16_t)TCPIP_MAC_PROCESS_FLAG_TX; 
            pMacParams->processFlags = (TCPIP_MAC_PROCESS_FLAGS)procFlags;
            pMacParams->macType = TCPIP_MAC_TYPE_ETH;
            pMacParams->linkMtu = TCPIP_MAC_LINK_MTU_ETH;
        }
        response = TCPIP_MAC_RES_OK;
    }
    return response;
}

TCPIP_MAC_RES MAC_DRVR_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics)
{
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    return (pMacDrvr != NULL) ? EmacDrvrStatisticsGet(pMacDrvr, pRxStatistics, pTxStatistics) : TCPIP_MAC_RES_OP_ERR;
}

TCPIP_MAC_RES MAC_DRVR_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, size_t nEntries, size_t* pHwEntries)
{
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    return (pMacDrvr != NULL) ?  EmacDrvrRegisterStatisticsGet(pMacDrvr, pRegEntries, nEntries, pHwEntries) : TCPIP_MAC_RES_OP_ERR;
}


/*******************************************************************************
 * local functions and helpers
 */
// TX should be locked!
static TCPIP_MAC_RES macDrvrTxPendingPackets( MAC_DRIVER * pMacDrvr )
{
    uint32_t                    segsAvailable;
    uint32_t                    segments;
    TCPIP_MAC_PACKET *          pMacPacket;
    TCPIP_MAC_DATA_SEGMENT *    pDSeg;

    while( pMacDrvr->txPendingList.nNodes != 0U)
    {
        segments = 0;
        pDSeg = pMacDrvr->txPendingList.head->pDSeg;
        while( pDSeg != NULL)
        {
            pDSeg = pDSeg->next;
            ++segments;
        }
        // number of available spots is equal to the moduloDistance; but, we reserve
        // one space for the hard stop -- hence '>=' is the appropriate test for space
        segsAvailable = getModuloDistance(  (uint32_t)pMacDrvr->txInsertPt, (uint32_t)pMacDrvr->txExtractPt, (uint32_t)pMacDrvr->config.nTxDescCnt);
        if( segments >= segsAvailable )
        {   // not enough descriptors to place the entire packet
            pMacDrvr->txStat.nTxQueueFull++;
            break;
        }
        pMacPacket = macDrvrPacketListHeadRemove( &pMacDrvr->txPendingList );
        (void)macDrvrLibTxSendPacket( pMacDrvr, pMacPacket, segments);
    }

    return TCPIP_MAC_RES_OK;
}

// TX should NOT be locked! performs lock/unlock as needed
static void macDrvrTxAcknowledge( MAC_DRIVER * pMacDrvr )
{
    TCPIP_MAC_PACKET *  pMacPacket;
    uint16_t            segmentsReady;
    uint16_t            extract;
    MAC_DRVR_PACKET_LIST ackList, errList;

    macDrvrPacketListInitialize(&ackList);
    macDrvrPacketListInitialize(&errList);

    macDrvrTxLock( pMacDrvr );

    // acknowledge when a complete segment is sent
    // only first segment has correct ownership setting -- emac does not update the others!
    while(true)
    {
        if(pMacDrvr->txExtractPt == pMacDrvr->txInsertPt)
        {
            break;
        }
        if((DRVRnEMAC_TX_USE_BIT & pMacDrvr->pTxDesc[ pMacDrvr->txExtractPt ].status.val) == 0U)
        {
            break;
        }

        segmentsReady = 0;
        extract = pMacDrvr->txExtractPt;
        if( (EMAC_TX_ERR_BITS & pMacDrvr->pTxDesc[ extract ].status.val) != 0U)
        {   // transmit error: emac has restarted at base, descriptors should be reset
            macDrvrTxGetAllPending( pMacDrvr, &errList );
            (void)macDrvrLibTxInit( pMacDrvr );
            break;
        }

        // get aligned buffer address from Tx Descriptor Buffer Address
        uint8_t* pSegBuff = (uint8_t*)(EMAC_RX_ADDRESS_MASK & pMacDrvr->pTxDesc[ extract ].bufferAddress);
        // get packet pointer from buffer gap descriptor
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = FC_U8Ptr2GapDcpt(pSegBuff + pMacDrvr->gapDcptOffset);
        pMacPacket = pGap->segmentPktPtr;

        while( extract != pMacDrvr->txInsertPt )
        {
            ++segmentsReady;
            // only first segment will have correct ownership bit setting -- emac does not update the others!
            if( (EMAC_TX_LAST_BUFFER_BIT & pMacDrvr->pTxDesc[ extract ].status.val) == 0U )
            {
                extract = (uint16_t)moduloIncrement( (uint32_t)extract, (uint32_t)pMacDrvr->config.nTxDescCnt );
            }
            else
            {   // complete segment sent
                pMacDrvr->txExtractPt = (uint16_t)moduloIncrement( (uint32_t)extract, (uint32_t)pMacDrvr->config.nTxDescCnt );
                macDrvrPacketListTailAdd(&ackList, pMacPacket);
                pMacDrvr->txStat.nTxOkPackets++;
                while( segmentsReady-- != 0U)
                {   // backup and cleanup descriptors, leaving the appropriate bits in place
                    pMacDrvr->pTxDesc[ extract ].status.val |= DRVRnEMAC_TX_USE_BIT;
                    __DSB();
                    pMacDrvr->pTxDesc[ extract ].status.val &= (DRVRnEMAC_TX_USE_BIT | EMAC_TX_WRAP_BIT);
                    pMacDrvr->pTxDesc[ extract ].bufferAddress = 0;
                    extract = (uint16_t)moduloDecrement((uint32_t)extract, (uint32_t)pMacDrvr->config.nTxDescCnt );
                }
                break;      // inner loop finished with this packet; rise to outer loop now
            }
        }
    }

    macDrvrTxUnlock( pMacDrvr );

    // acknowledge the finished packets
    while( (pMacPacket = macDrvrPacketListHeadRemove(&ackList)) != NULL )
    {
        pMacDrvr->callBack.pktAckF( pMacPacket, TCPIP_MAC_PKT_ACK_TX_OK, pMacDrvr->pObj->macId );
    }

    // acknowledge the error packets
    while( (pMacPacket = macDrvrPacketListHeadRemove(&errList)) != NULL )
    {
        pMacDrvr->callBack.pktAckF( pMacPacket, TCPIP_MAC_PKT_ACK_BUFFER_ERR, pMacDrvr->pObj->macId );
    }
}

#if (M_MAC_DOWN_OPERATION != 0)
static void macDrvrDeInit( MAC_DRIVER * pMacDrvr )
{
    const DRV_ETHPHY_OBJECT_BASE *  pPhyBase = pMacDrvr->config.pPhyBase;
    emac_registers_t *              pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;

    pMacRegs->EMAC_NCR &= ~(EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk); // must be done first
    (void)SYS_INT_SourceDisable( pMacDrvr->config.macIntSrc );        // disable interrupt vector
    SYS_INT_SourceStatusClear( pMacDrvr->config.macIntSrc );    // clear any pending interrupt flag
    macDrvrLibClose( pMacDrvr, MAC_DRVR_CLOSE_DEFAULT );
    (void)macDrvrEventDeInit( pMacDrvr );
    if( pMacDrvr->hPhyClient != DRV_HANDLE_INVALID )
    {
        pPhyBase->phy_Close( pMacDrvr->hPhyClient );
        pMacDrvr->hPhyClient = DRV_HANDLE_INVALID;
    }
    if( pMacDrvr->hPhySysObject != SYS_MODULE_OBJ_INVALID )
    {
        pPhyBase->phy_Deinitialize( pMacDrvr->hPhySysObject );
        pMacDrvr->hPhySysObject = SYS_MODULE_OBJ_INVALID;
    }
    // RX clean up
    macDrvrRxPacketPoolReset( pMacDrvr );
    macDrvrRxDelete( pMacDrvr );
    // TX clean up
    macDrvrTxPacketPoolReset( pMacDrvr, TCPIP_MAC_PKT_ACK_NET_DOWN );
    macDrvrTxDelete( pMacDrvr );
    //
    pMacDrvr->sysStat = SYS_STATUS_UNINITIALIZED;
    pMacDrvr->macFlags.val = 0;
}

static MAC_DRVR_RESULT macDrvrRemoveDynamicsFromPacketPool( MAC_DRIVER * pMacDrvr )
{   // NOTE: this can contend with macDrvrLibRxAllocate, macDrvrLibRxPopulateDescriptors
    //  or rxMacPacketAck.  This is only to be called by macDrvrRxPacketPoolReset.
    uint16_t            ii;
    TCPIP_MAC_PACKET *  pMacPacket;
    MAC_DRVR_RESULT     retval = MAC_DRVR_RES_OK;

    for( ii = (uint16_t)pMacDrvr->rxMacPacketPool.nNodes; ii != 0U; --ii )
    {
        pMacPacket = macDrvrPacketListHeadRemove( &pMacDrvr->rxMacPacketPool );
        if( pMacPacket != NULL)
        {
            if( (pMacPacket->pDSeg->segFlags & (uint16_t)TCPIP_MAC_SEG_FLAG_RX_STICKY) != 0U )
            {
                macDrvrPacketListTailAdd( &pMacDrvr->rxMacPacketPool, pMacPacket);
            }
            else
            {
                macDrvrPacketListTailAdd( &pMacDrvr->rxDynamicReleasePool, pMacPacket);
            }
        }
    }
    return retval;
}

static void macDrvrRxPacketPoolReset( MAC_DRIVER * pMacDrvr )
{
    TCPIP_MAC_PACKET *      pMacPacket;
    MAC_DRVR_RX_FRAME_INFO  frameInfo = { 0, 0, 0 };

    // put segments in descriptors back on the side line - into the rxMacPacketPool
    while( pMacDrvr->rxExtractPt != pMacDrvr->rxHardStop )
    {
        frameInfo.startIndex = frameInfo.endIndex = pMacDrvr->rxExtractPt;
        frameInfo.bufferCount = 1;
        rxExpungeFrameSegments( pMacDrvr, &frameInfo );
    }
    // move dynamic buffers from rxMacPacketPool to rxDynamicReleasePool
    (void)macDrvrRemoveDynamicsFromPacketPool( pMacDrvr );
    // free all buffers in the rxDynamicReleasePool
    (void)macDrvrLibFreeDynamicBuffers( pMacDrvr );
    // free the packets on the side line
    while( pMacDrvr->rxMacPacketPool.nNodes != 0U)
    {
        pMacPacket = macDrvrPacketListHeadRemove( &pMacDrvr->rxMacPacketPool );
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
        pMacDrvr->callBack.pktFreeFDbg( pMacPacket, TCPIP_THIS_MODULE_ID);
#else
        pMacDrvr->callBack.pktFreeF( pMacPacket );
#endif
    }
}
#else
static void macDrvrDeInit( MAC_DRIVER * pMacDrvr )
{
}
#endif

// TX should be locked!
static void macDrvrTxGetAllPending( MAC_DRIVER *  pMacDrvr, MAC_DRVR_PACKET_LIST*  ackList)
{   // remove apackets in transmission queue and add to the supplied list
    TCPIP_MAC_PACKET * pMacPacket;

    while( pMacDrvr->txPendingList.nNodes != 0U)
    {   // release pending list packets
        pMacPacket = macDrvrPacketListHeadRemove( &pMacDrvr->txPendingList );
        macDrvrPacketListTailAdd(ackList, pMacPacket);
    }
}

static void macDrvrTxPacketPoolReset( MAC_DRIVER *  pMacDrvr, TCPIP_MAC_PKT_ACK_RES  ackRes)
{
    MAC_DRVR_PACKET_LIST txList;
    macDrvrPacketListInitialize(&txList);

    // acknowledge packets the mac has in it's descriptors
    macDrvrTxAcknowledge( pMacDrvr );

    // remove the packets queued for transmission
    macDrvrTxLock( pMacDrvr );
    macDrvrTxGetAllPending( pMacDrvr, &txList );
    // everything should be recovered so now we can reset the tx
    (void)macDrvrLibTxInit( pMacDrvr );       // also called at standard init
    macDrvrTxUnlock( pMacDrvr );

    // acknowledge
    TCPIP_MAC_PACKET *  pMacPacket;
    while( (pMacPacket = macDrvrPacketListHeadRemove(&txList)) != NULL )
    {
        pMacDrvr->callBack.pktAckF( pMacPacket, ackRes, pMacDrvr->pObj->macId );
    }

}

/*******************************************************************************
 * Function:        translateTcpMacToMacEvent
 * PreCondition:    None
 * Input:           tcpEv - the TcpIp events to be translated
 * Output:          Ethernet events
 * Side Effects:    None
 * Overview:        This function translates from  events to Ethernet events
 * Note:            - The translation could be done in a more systematic way
 *                    using a (few) LUT(s).
 *                    For now this direct approach is taken.
 *                  - The CONN_LOST/CONN_ESTABLISHED events do NOT map to Ethernet events!
 *                    The current translation function assumes no overlap between a CONN event
 *                    and an Ethernet one!
 *                    By this translation the CONN_LOST/CONN_ESTABLISHED event is lost!
 ******************************************************************************/
static MAC_EVENTS translateTcpMacToMacEvent( MAC_DRIVER *  pMacDrvr, TCPIP_MAC_EVENT tcpMacEvents
    )
{
    // mapping RX Used bit interrupt to BUFNA; since there is no interrupt bit
    // for BUFNA and RXUBR interrupt will trigger with BUFNA
    uint16_t macEvents = 0U;

    macEvents |= ((uint16_t)tcpMacEvents & (uint16_t)TCPIP_MAC_EV_RX_DONE   ) != 0U ? (uint16_t)MAC_EV_RXCOMPLETE : 0U;
    macEvents |= ((uint16_t)tcpMacEvents & (uint16_t)TCPIP_MAC_EV_RX_OVFLOW ) != 0U ? (uint16_t)MAC_EV_RXOVERRUN : 0U;
    macEvents |= ((uint16_t)tcpMacEvents & (uint16_t)TCPIP_MAC_EV_RX_BUFNA  ) != 0U ? (uint16_t)MAC_EV_RXUSEDBITREAD : 0U;
    macEvents |= ((uint16_t)tcpMacEvents & (uint16_t)TCPIP_MAC_EV_TX_DONE   ) != 0U ? (uint16_t)MAC_EV_TXCOMPLETE : 0U;
    macEvents |= ((uint16_t)tcpMacEvents & (uint16_t)TCPIP_MAC_EV_TX_ABORT  ) != 0U ? ((uint16_t)MAC_EV_TXUSEDBITREAD
                                                            | (uint16_t)MAC_EV_TXUNDERRUN
                                                            | (uint16_t)MAC_EV_RETRYLIMITEXCEED
                                                            | (uint16_t)MAC_EV_TXFRAMECORRUPT
                                                            )
                                                            : 0U;
    macEvents |= ((uint16_t)tcpMacEvents & (uint16_t)TCPIP_MAC_EV_TX_BUSERR ) != 0U ? (uint16_t)MAC_EV_HRESPNOTOK : 0U;

    return (MAC_EVENTS)macEvents;
}

/*******************************************************************************
 * Function:        translateMacToTcpMacEvent
 * PreCondition:    None
 * Input:           eEvents - the Eth events to be translated
 * Output:          TcpIp events
 * Side Effects:    None
 * Overview:        This function translates from Ethernet events to TcpIp events
 * Note:            - The translation could be done in a more systematic way
 *                    using a (few) LUT(s).
 *                    For now this direct approach is taken.
 *                  - The CONN_LOST/CONN_ESTABLISHED events do NOT map to Ethernet events!
 *                    The current translation function assumes no overlap between a CONN event
 *                    and an Ethernet one!
 *                    By this translation a CONN_LOST/CONN_ESTABLISHED event cannot be generated!
 ******************************************************************************/
static TCPIP_MAC_EVENT translateMacToTcpMacEvent( MAC_DRIVER *  pMacDrvr, MAC_EVENTS  macEvents)
{
    // mapping RX Used bit interrupt to BUFNA; since there is no interrupt bit
    // for BUFNA and RXUBR interrupt will trigger with BUFNA
    uint16_t tcpMacEvents = 0U;
    tcpMacEvents |= ((uint16_t)macEvents & (uint16_t)MAC_EV_RXCOMPLETE) != 0U ? (uint16_t)TCPIP_MAC_EV_RX_DONE : 0U;
    tcpMacEvents |= ((uint16_t)macEvents & (uint16_t)MAC_EV_RXOVERRUN) != 0U ? (uint16_t)TCPIP_MAC_EV_RX_OVFLOW : 0U;
    tcpMacEvents |= ((uint16_t)macEvents & (uint16_t)MAC_EV_RXUSEDBITREAD) != 0U ? (uint16_t)TCPIP_MAC_EV_RX_BUFNA : 0U;
    tcpMacEvents |= ((uint16_t)macEvents & (uint16_t)MAC_EV_TXCOMPLETE) != 0U ? (uint16_t)TCPIP_MAC_EV_TX_DONE : 0U;
    tcpMacEvents |= ((uint16_t)macEvents & ((uint16_t)MAC_EV_TXUSEDBITREAD
                                | (uint16_t)MAC_EV_TXUNDERRUN
                                | (uint16_t)MAC_EV_RETRYLIMITEXCEED
                                | (uint16_t)MAC_EV_TXFRAMECORRUPT
                                )) != 0U
                                ? (uint16_t)TCPIP_MAC_EV_TX_ABORT
                                : 0U;
    tcpMacEvents |= ((uint16_t)macEvents & (uint16_t)MAC_EV_HRESPNOTOK) != 0U ? (uint16_t)TCPIP_MAC_EV_TX_BUSERR : 0U;

    return (TCPIP_MAC_EVENT)tcpMacEvents;
}

/*******************************************************************************
  Function:
    TCPIP_MAC_RES macDrvrEventInit( MAC_DRIVER *  pMacDrvr, TCPIP_MAC_EventF  eventF, const void *  eventParam)
  Summary:
    Initializes the ethernet event notification.

  Description:
     This function initializes the ethernet event notification.
     It performs any resource allocation that may be needed.

  Precondition:
     None.

  Parameters:
    pMacDrvr    - parameter identifying the intended MAC
    eventF      - notification function
    eventParam  - notification parameter

  Returns:
    TCPIP_MAC_RES_OK  if initialization succeeded,
    an error code otherwise

  Example:
    <code>
    macDrvrEventInit( pMacDrvr, eventF, param );
    </code>

  Remarks:
    Not multi-threaded safe.
*******************************************************************************/
static TCPIP_MAC_RES macDrvrEventInit( MAC_DRIVER *  pMacDrvr, TCPIP_MAC_EventF  eventF, const void *  eventParam)
{
    MAC_DRVR_EVENT_DCPT * pEventDescription = &pMacDrvr->eventDescription;
    MAC_EVENTS            macEvents = (MAC_EVENTS)((uint16_t)MAC_EV_RX | (uint16_t)MAC_EV_TX);

    pEventDescription->macEventsEnabled = macEvents;
    pEventDescription->macEventsPending = MAC_EV_NONE;
    pEventDescription->tcpMacEventsEnabled = translateMacToTcpMacEvent( pMacDrvr, macEvents );
    pEventDescription->tcpMacEventsPending = TCPIP_MAC_EV_NONE;

    pEventDescription->tcpNotifyFnc = eventF;
    pEventDescription->tcpNotifyParam = eventParam;

    return TCPIP_MAC_RES_OK;
}

/*******************************************************************************
  Function:
    TCPIP_MAC_RES macDrvrEventDeInit( MAC_DRIVER * pMacDrvr )

  Summary:
    De-initializes the ethernet event notification.

  Description:
     This function de-initializes the ethernet event notification.
     It performs any resource clean-up that may be needed.

  Precondition:
     None.

  Parameters:
    pMacDrvr    - parameter identifying the intended MAC

  Returns:
    TCPIP_MAC_RES_OK  always

  Example:
    <code>
    macDrvrEventDeInit( pMacDrvr );
    </code>

  Remarks:
    Not multi-threaded safe.
*******************************************************************************/
#if (M_MAC_DOWN_OPERATION != 0)
static TCPIP_MAC_RES macDrvrEventDeInit( MAC_DRIVER * pMacDrvr )
{
    MAC_DRVR_EVENT_DCPT * pEventDescription = &pMacDrvr->eventDescription;

    pEventDescription->macEventsEnabled = MAC_EV_NONE;
    pEventDescription->tcpMacEventsEnabled = TCPIP_MAC_EV_NONE;

    pEventDescription->macEventsPending = MAC_EV_NONE;
    pEventDescription->tcpMacEventsPending = TCPIP_MAC_EV_NONE;

    return TCPIP_MAC_RES_OK;
}
#endif

/*******************************************************************************
  Function:
    bool    MAC_DRVR_EventMaskSet( DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable )

  Summary:
    Enables the MAC events.

  Description:
     This function sets the enabled events.
     Multiple events can be OR'd together.
     All events that are set will be added to the notification process. The old
     events will be disabled.  The stack (or stack user) has to catch the events
     that are notified and process them:
         - The stack should process the transfer events
         - Process the specific condition and acknowledge them calling
             MAC_DRVR_EventAcknowledge() so that they can be re-enabled.

  Precondition:
   macDrvrEventInit should have been called.

  Parameters:
    hMac            - parameter identifying the intended MAC
    tcpMacEvents    - events the user of the stack wants to add/delete for notification
    enable          - if true, the events will  be enabled, else disabled

  Returns:
    true  if operation succeeded,
    an error code otherwise

  Example:
    <code>
    MAC_DRVR_EventMaskSet( hMac, TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA, true );
    </code>

  Remarks:
    The event notification system enables the user of the TCPIP stack to call into
    the stack for processing only when there are relevant events rather than being
    forced to periodically call from within a loop.

    If the notification events are nill the interrupt processing will be disabled.
    Otherwise the event notification will be enabled and the interrupts relating
    to the requested events will be enabled.

    Note that once an event has been caught by the stack ISR (and reported if a
    notification is in place) it will be disabled until the MAC_DRVR_EventAcknowledge()
    is called.

*******************************************************************************/
bool MAC_DRVR_EventMaskSet( DRV_HANDLE  hMac, TCPIP_MAC_EVENT tcpMacEvents, bool  enable)
{
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    if(pMacDrvr == NULL)
    {
        return false;
    }

    MAC_DRVR_EVENT_DCPT *   pEventDescription = &pMacDrvr->eventDescription;
    emac_registers_t *      pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    MAC_EVENTS              macEvents;
    bool                    previouslyActive;
    uint16_t                u16TcpMacEvents = (uint16_t)tcpMacEvents;

    previouslyActive = SYS_INT_SourceDisable( pMacDrvr->config.macIntSrc );
    if( enable )
    {
        previouslyActive = true;            // will start interrupts even if formerly not on
        u16TcpMacEvents = u16TcpMacEvents | (uint16_t)pEventDescription->tcpMacEventsEnabled;
    }
    else
    {
        u16TcpMacEvents = u16TcpMacEvents & ~(uint16_t)pEventDescription->tcpMacEventsEnabled;
    }
    tcpMacEvents = (TCPIP_MAC_EVENT)u16TcpMacEvents;
    pEventDescription->tcpMacEventsEnabled = tcpMacEvents;
    macEvents = translateTcpMacToMacEvent( pMacDrvr, tcpMacEvents );
    pEventDescription->macEventsEnabled = macEvents;

    uint32_t u32MacEvents = (uint32_t)macEvents;
    u32MacEvents &= ~(uint32_t)pEventDescription->macEventsPending;      // keep un-ack events
    (void) pMacRegs->EMAC_ISR;                              // clear the interrupt status
    pMacRegs->EMAC_IDR = (~u32MacEvents) & EMAC_IDR_Msk;
    pMacRegs->EMAC_IER =  u32MacEvents & EMAC_IER_Msk;
    SYS_INT_SourceRestore( pMacDrvr->config.macIntSrc, previouslyActive );
    if( previouslyActive )
    {
        macDrvrLibTransferEnable( pMacDrvr );
    }

    return true;
}

/*******************************************************************************
  Function:
    bool MAC_DRVR_EventAcknowledge( DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpMacEvents )

  Summary:
    Acknowledges and re-enables processed events.

  Description:
    This function acknowledges and re-enables processed events.
    Multiple events can be or'd together as they are processed together.
    The events acknowledged by this function should be the events that have been
    retrieved from the stack by calling MAC_DRVR_EventPendingGet() or have been
    passed to the user by the stack using a notification, have been processed
    and have to be re-enabled.

  Precondition:
   macDrvrEventInit should have been called.

  Parameters:
    hMac      - parameter identifying the intended MAC
    macEvents - the events that the user processed and need to be re-enabled

  Returns:
    true if events acknowledged
    false if no events to be acknowledged

  Example:
    <code>
    MAC_DRVR_EventAcknowledge( hMac, stackNewEvents );
    </code>

  Remarks:
    All events should be acknowledged, in order to be re-enabled.

    Some events are fatal errors and should not be acknowledged (TCPIP_MAC_EV_RX_BUSERR,
    TCPIP_MAC_EV_TX_BUSERR). Stack re-initialization is needed under such circumstances.

    Some events are just system/application behavior and they are intended only as
    simple info (TCPIP_MAC_EV_RX_OVFLOW, TCPIP_MAC_EV_RX_BUFNA, TCPIP_MAC_EV_TX_ABORT,
    TCPIP_MAC_EV_RX_ACT).

    The TCPIP_MAC_EV_RX_FWMARK and TCPIP_MAC_EV_RX_EWMARK events are part of the normal
    flow control operation (if auto flow control was enabled). They should be enabled
    alternatively, if needed.

    The events are persistent. They shouldn't be re-enabled unless they have been
    processed and the condition that generated them was removed.  Re-enabling them
    immediately without proper processing will have dramatic effects on system performance.

*****************************************************************************/
bool MAC_DRVR_EventAcknowledge( DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpMacEvents )
{
    bool    retval = false;
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    if(pMacDrvr != NULL)
    {

        MAC_DRVR_EVENT_DCPT *   pEventDescription = &pMacDrvr->eventDescription;
        MAC_EVENTS              macEvents;
        bool                    previouslyActive;

        if( pEventDescription->tcpMacEventsEnabled != TCPIP_MAC_EV_NONE)
        {   // already have some active
            retval = true;
            macEvents = translateTcpMacToMacEvent( pMacDrvr, tcpMacEvents );
            previouslyActive = SYS_INT_SourceDisable( pMacDrvr->config.macIntSrc );    // stop ints for a while
            pEventDescription->tcpMacEventsPending = FC_U16Ev2MacEvent((uint16_t)pEventDescription->tcpMacEventsPending & ~(uint16_t)tcpMacEvents);    // no longer pending
            pEventDescription->macEventsPending = FC_U32Ev2MacEvent((uint32_t)pEventDescription->macEventsPending & ~(uint32_t)macEvents);          // no longer pending
            SYS_INT_SourceRestore( pMacDrvr->config.macIntSrc, previouslyActive );     // re-enable
        }
    }
    return retval;
}

/*******************************************************************************
  Function:
    TCPIP_MAC_EVENT MAC_DRVR_EventPendingGet( DRV_HANDLE hMac )

  Summary:
    Returns the currently pending events.

  Description:
    This function returns the currently pending events belonging to a group.
    Multiple events can be orr-ed together as they accumulate.  The stack should
    be called for processing whenever a stack managed event is present.  The
    other, non critical events, may not be managed by the stack and passed to
    an user.  They will have to be eventually acknowledged if re-enabling is
    needed.

  Precondition:
   hMac      - parameter identifying the intended MAC

   macDrvrEventInit should have been called.

  Parameters:

  Returns:
    The currently stack pending events.

  Example:
    <code>
    TCPIP_MAC_EVENT currEvents = MAC_DRVR_EventPendingGet( hMac );
    </code>

  Remarks:
    This is the preferred method to get the current pending MAC events.  The stack
    maintains a proper image of the events from their occurrence to their acknowledgement.

    Even with notifications in place it's better to use this function to get the
    current pending events rather than using the events passed via notification
    which could be stale.

    The events are persistent. They shouldn't be re-enabled unless they have been
    processed and the condition that generated them was removed.  Re-enabling them
    immediately without proper processing will have dramatic effects on system performance.

    The returned value is just a momentary value. The pending events can change any time.
*****************************************************************************/
TCPIP_MAC_EVENT MAC_DRVR_EventPendingGet( DRV_HANDLE hMac )
{
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    return pMacDrvr != NULL ? pMacDrvr->eventDescription.tcpMacEventsPending : TCPIP_MAC_EV_NONE;
}

/*******************************************************************************
 * local functions
 */

/*******************************************************************************
 * Function:        MAC_DRVR_Tasks_ISR
 *
 * PreCondition:    macDrvrEventInit, MAC_DRVR_EventMaskSet should have been called
 *
 * Input:           macIndex - MAC object index
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function processes the Ethernet interrupts and reports the
 *                  events back to the user.
 *
 * Note:            None
 ******************************************************************************/
void EMAC0_InterruptHandler( void )
{
    MAC_DRVR_Tasks_ISR( (SYS_MODULE_OBJ) TCPIP_MODULE_MAC_SAM9X60_0 );
}

void EMAC1_InterruptHandler( void )
{
    MAC_DRVR_Tasks_ISR( (SYS_MODULE_OBJ) TCPIP_MODULE_MAC_SAM9X60_1 );
}

void MAC_DRVR_Tasks_ISR( SYS_MODULE_OBJ sysObjHandle )
{
    MAC_DRIVER *            pMacDrvr = &macDrvrDescription[ (int) sysObjHandle - (int)TCPIP_MODULE_MAC_SAM9X60 ];
    emac_registers_t *      pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    MAC_DRVR_EVENT_DCPT *   pEventDescription = &pMacDrvr->eventDescription;
    MAC_EVENTS              macEventsOfInterest = (MAC_EVENTS) pMacRegs->EMAC_ISR;

    macEventsOfInterest = FC_U32Ev2MacEvent((uint32_t)macEventsOfInterest & (uint32_t)pEventDescription->macEventsEnabled); // keep just the relevant events
    if( macEventsOfInterest != MAC_EV_NONE)
    {
        pEventDescription->macEventsPending = FC_U32Ev2MacEvent((uint32_t)pEventDescription->macEventsPending | (uint32_t)macEventsOfInterest);
        pEventDescription->tcpMacEventsPending = FC_U16Ev2MacEvent((uint16_t)pEventDescription->tcpMacEventsPending | (uint16_t)translateMacToTcpMacEvent( pMacDrvr, macEventsOfInterest));
        // clear the tx transmit status
        pMacRegs->EMAC_TSR = EMAC_TSR_COMP_Msk;
        if( pEventDescription->tcpNotifyFnc != NULL)
        {   // let the user know
            pEventDescription->tcpNotifyFnc( pEventDescription->tcpMacEventsPending, pEventDescription->tcpNotifyParam);
        }
    }
}

/*******************************************************************************
 * Function:    macDrvrPhyInitialize
 * Summary:     PHY initialization
 ******************************************************************************/
static TCPIP_MAC_RES macDrvrPhyInitialize( MAC_DRIVER * pMacDrvr )
{
    DRV_HANDLE                      hPhyClient;
    DRV_ETHPHY_SETUP                phySetup;
    const DRV_ETHPHY_OBJECT_BASE *  pPhyBase =  pMacDrvr->config.pPhyBase;
    uint16_t                        phyIx = (uint16_t)pMacDrvr->phyIx;
    TCPIP_MAC_RES                   result = TCPIP_MAC_RES_PHY_INIT_FAIL;
    SYS_MODULE_OBJ                  hPhySysObject;

    union
    {
        const struct DRV_ETHPHY_INIT *  pPhyInit;
        const SYS_MODULE_INIT*          sysModInit;
    }U_PHY_MOD_INIT;

    U_PHY_MOD_INIT.pPhyInit = pMacDrvr->config.pPhyInit;
    hPhySysObject = pPhyBase->phy_Initialize( phyIx, U_PHY_MOD_INIT.sysModInit);
    if( SYS_MODULE_OBJ_INVALID != hPhySysObject )
    {
        hPhyClient = pPhyBase->phy_Open( phyIx, DRV_IO_INTENT_READWRITE );
        if ( DRV_HANDLE_INVALID != hPhyClient )
        {
            pMacDrvr->hPhySysObject = hPhySysObject;
            pMacDrvr->hPhyClient = hPhyClient;

            phySetup.phyAddress = (int)pMacDrvr->config.pPhyInit->phyAddress;
            phySetup.openFlags = pMacDrvr->config.ethFlags;
            phySetup.configFlags = pMacDrvr->config.pPhyInit->phyFlags;
            phySetup.macPauseType = macDrvrPAUSE_CPBL_MASK;
            phySetup.resetFunction = pMacDrvr->config.pPhyInit->resetFunction;
            if( 0 <= (int)pPhyBase->phy_Setup( hPhyClient, &phySetup, &pMacDrvr->linkInfo.flags ) )
            {
                result = TCPIP_MAC_RES_OK;
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
typedef void (* macDrvrLinkStateF)( MAC_DRIVER * pMacDrvr );

static const macDrvrLinkStateF macDrvrLinkStateTbl[] =
{   // table of different Link status functions
    &macDrvrLinkStateStartLink,
    &macDrvrLinkStateGetLink,
    &macDrvrLinkStateWaitLinkUp,
    &macDrvrLinkStateNegComplete,
    &macDrvrLinkStateNegResult,
};

bool MAC_DRVR_LinkCheck( DRV_HANDLE hMac )
{   // verify the link status
    bool            retval = false;
    MAC_DRIVER * pMacDrvr = F_EmacHandleToMacDrv( hMac );
    if(pMacDrvr != NULL && pMacDrvr->macFlags.linkPresent != 0U)
    {
        const DRV_ETHPHY_OBJECT_BASE *  pPhyBase = pMacDrvr->config.pPhyBase;
        pPhyBase->phy_Tasks( pMacDrvr->hPhySysObject );

        macDrvrLinkStateTbl[ pMacDrvr->linkInfo.checkState ]( pMacDrvr );
        if( pMacDrvr->macFlags.linkPrev != 0U)
        {
            retval = true;
        }
    }
    return retval;
}

static void macDrvrLinkStateDown( MAC_DRIVER * pMacDrvr )
{
    // link is down; reset state flags, initialization time
    pMacDrvr->macFlags.linkPrev = 0U;
    pMacDrvr->macFlags.linkUpDone = 0U;
    pMacDrvr->linkInfo.upTick = SYS_TMR_TickCountGet();
    pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_START_LINK;
}

static void macDrvrLinkStateStartLink( MAC_DRIVER * pMacDrvr )
{
    const DRV_ETHPHY_OBJECT_BASE * pPhyBase = pMacDrvr->config.pPhyBase;

    if( 0 > (int)pPhyBase->phy_LinkStatusGet( pMacDrvr->hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &pMacDrvr->linkInfo.negResult.linkStatus, false))
    {   // some error occurred
        macDrvrLinkStateDown( pMacDrvr );
    }
    else
    {
        pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_GET_LINK;
    }
}

static void macDrvrLinkStateGetLink( MAC_DRIVER * pMacDrvr )
{
    const DRV_ETHPHY_OBJECT_BASE * pPhyBase = pMacDrvr->config.pPhyBase;

    if( DRV_ETHPHY_CLIENT_STATUS_BUSY != pPhyBase->phy_ClientStatus( pMacDrvr->hPhyClient ) )
    {
        if( DRV_ETHPHY_RES_OK != pPhyBase->phy_ClientOperationResult( pMacDrvr->hPhyClient ) )
        {   // some error occurred
            macDrvrLinkStateDown( pMacDrvr );
        }
        else if( ((uint32_t)pMacDrvr->linkInfo.negResult.linkStatus & (uint32_t)DRV_ETHPHY_LINK_ST_UP) == 0U)
        {   // down
            macDrvrLinkStateDown( pMacDrvr );
        }
        else if( pMacDrvr->macFlags.linkPrev != 0U)
        {   // up now and up before
            pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_START_LINK;
        }
        else
        {   // up now and down before
            pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_WAIT_LINK_UP;
        }
    }
}

static void macDrvrLinkStateWaitLinkUp( MAC_DRIVER * pMacDrvr )
{   // auto negotiation is turned on
    // we have to re-negotiate but the link is actually up only
    // after the re-negotiation is done
    const DRV_ETHPHY_OBJECT_BASE * pPhyBase = pMacDrvr->config.pPhyBase;

    if( pMacDrvr->macFlags.linkUpDone == 0U)
    {   // wait the init tmo before checking the link up
        uint32_t currTick = SYS_TMR_TickCountGet();
        if( (currTick - pMacDrvr->linkInfo.upTick) < pMacDrvr->linkInfo.waitTick )
        {
            return;
        }
        pMacDrvr->macFlags.linkUpDone = 1U;
    }
    // initialization delay passed
    if( pMacDrvr->macFlags.linkNegotiation == 0U )
    {   // no auto negotiation taking place; this is the link status
        pMacDrvr->macFlags.linkPrev = 1U;
        pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_START_LINK;
    }
    else if( 0 > (int)pPhyBase->phy_NegotiationIsComplete(    pMacDrvr->hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, false))
    {   // some error occurred
        macDrvrLinkStateDown( pMacDrvr );
    }
    else
    {   // can advance -- wait for negotiation complete
        pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_NEG_COMPLETE;
    }
}

static void macDrvrLinkStateNegComplete( MAC_DRIVER * pMacDrvr )
{
    const DRV_ETHPHY_OBJECT_BASE * pPhyBase = pMacDrvr->config.pPhyBase;

    if( DRV_ETHPHY_CLIENT_STATUS_BUSY != pPhyBase->phy_ClientStatus( pMacDrvr->hPhyClient ) )
    {
        if( DRV_ETHPHY_RES_OK != pPhyBase->phy_ClientOperationResult( pMacDrvr->hPhyClient ) )
        {   // some error occurred
            macDrvrLinkStateDown( pMacDrvr );
        }
        else if( 0 > (int)pPhyBase->phy_NegotiationResultGet( pMacDrvr->hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &pMacDrvr->linkInfo.negResult)
        )
        {   // some error occurred
            macDrvrLinkStateDown( pMacDrvr );
        }
        else
        {   // advance; get negotiation result
            pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_NEG_RESULT;
        }
    }
}

static void macDrvrLinkStateNegResult( MAC_DRIVER * pMacDrvr )
{
    const DRV_ETHPHY_OBJECT_BASE *  pPhyBase = pMacDrvr->config.pPhyBase;
    DRV_ETHPHY_CONFIG_FLAGS         phyCfgFlags = DRV_ETHPHY_CFG_DEFAULT;

    if( DRV_ETHPHY_CLIENT_STATUS_BUSY != pPhyBase->phy_ClientStatus( pMacDrvr->hPhyClient ) )
    {
        if( DRV_ETHPHY_RES_OK != pPhyBase->phy_ClientOperationResult( pMacDrvr->hPhyClient ) )
        {   // some error occurred
            macDrvrLinkStateDown( pMacDrvr );
        }
        else if( ((uint16_t)pMacDrvr->linkInfo.negResult.linkStatus & (uint16_t)DRV_ETHPHY_LINK_ST_UP) != 0U )
        {   // negotiation succeeded; properly update the MAC
            (void)pPhyBase->phy_HWConfigFlagsGet( pMacDrvr->hPhyClient, &phyCfgFlags );
            TCPIP_ETH_OPEN_FLAGS miimFlags = ((uint32_t)phyCfgFlags & (uint32_t)DRV_ETHPHY_CFG_RMII) != 0U ? TCPIP_ETH_OPEN_RMII : TCPIP_ETH_OPEN_MII;
            pMacDrvr->linkInfo.negResult.linkFlags = FC_U32Flag2OpenFlags((uint32_t)pMacDrvr->linkInfo.negResult.linkFlags | (uint32_t)miimFlags);
            macDrvrLibMacOpen(  pMacDrvr, pMacDrvr->linkInfo.negResult.linkFlags, pMacDrvr->linkInfo.negResult.pauseType);
            pMacDrvr->macFlags.linkPrev = 1U;
            pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_START_LINK;
        }
        else
        {
            macDrvrLinkStateDown( pMacDrvr );
        }
    }
}

MAC_DRIVER* macDrvrObjGet(size_t* pnObjects)
{
    if(pnObjects != NULL)
    {
        *pnObjects = sizeof(macDrvrDescription) / sizeof(*macDrvrDescription);
    }
    return macDrvrDescription;
}
