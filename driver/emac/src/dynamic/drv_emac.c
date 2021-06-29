/*******************************************************************************
  MAC Module for Microchip TCP/IP Stack
*******************************************************************************/

/*****************************************************************************
Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software
and any derivatives exclusively with Microchip products. It is your
responsibility to comply with third party license terms applicable to your
use of third party software (including open source software) that may
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/
#include "driver/emac/src/dynamic/drv_emac_lib.h"

// All pause capabilities supported
#define macDrvrPAUSE_CPBL_MASK  (TCPIP_ETH_PAUSE_TYPE_ALL)

/******************************************************************************/
extern TCPIP_MAC_RES EmacDrvrStatisticsGet(             MAC_DRIVER *                        pMacDrvr,
                                                        TCPIP_MAC_RX_STATISTICS *           pRxStatistics,
                                                        TCPIP_MAC_TX_STATISTICS *           pTxStatistics
                                                        );
extern TCPIP_MAC_RES EmacDrvrRegisterStatisticsGet(     MAC_DRIVER *                        pMacDrvr,
                                                        TCPIP_MAC_STATISTICS_REG_ENTRY *    pRegEntries,
                                                        int                                 nEntries,
                                                        int *                               pHwEntries
                                                        );

/*******************************************************************************
 * Local Prototypes
 ******************************************************************************/
void                        macDrvrTaskIsr(             SYS_MODULE_OBJ object );
void                        EMAC0_InterruptHandler(     void );
void                        EMAC1_InterruptHandler(     void );
// PHY related Initialization
static SYS_MODULE_OBJ       macDrvrPhyInitialize(       MAC_DRIVER * pMacDrvr );
static void                 macDrvrDeInit(              MAC_DRIVER * pMacDrvr );
static void                 macDrvrTxAcknowledge(       MAC_DRIVER * pMacDrvr );
static TCPIP_MAC_RES        macDrvrTxPendingPackets(    MAC_DRIVER * pMacDrvr );
static void                 macDrvrTxAckAllPending(     MAC_DRIVER * pMacDrvr, TCPIP_MAC_PKT_ACK_RES ackRes );
static void                 macDrvrTxPacketPoolReset(   MAC_DRIVER * pMacDrvr, TCPIP_MAC_PKT_ACK_RES ackRes );
// MAC interface functions
SYS_MODULE_OBJ              macDrvrInitialize(          const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init );
#if TCPIP_STACK_MAC_DOWN_OPERATION
static void                 macDrvrDeInitialize(        SYS_MODULE_OBJ object );
static void                 macDrvrReInitialize(        SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init );
static TCPIP_MAC_RES        macDrvrEventDeInit(         MAC_DRIVER * pMacDrvr );
static void                 macDrvrRxPacketPoolReset(   MAC_DRIVER * pMacDrvr );
#endif
static SYS_STATUS           macDrvrStatus(              SYS_MODULE_OBJ object );
static void                 macDrvrTasks(               SYS_MODULE_OBJ object );
static DRV_HANDLE           macDrvrOpen(                const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent );
static void                 macDrvrClose(               DRV_HANDLE hMac );
static bool                 macDrvrLinkCheck(           DRV_HANDLE hMac );
static bool                 macDrvrPowerMode(           DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode );
static TCPIP_MAC_RES        macDrvrPacketTx(            DRV_HANDLE hMac, TCPIP_MAC_PACKET * pMacPacket );
static TCPIP_MAC_PACKET *   macDrvrPacketRx (           DRV_HANDLE hMac, TCPIP_MAC_RES * pRes, TCPIP_MAC_PACKET_RX_STAT * pPktStat );
static TCPIP_MAC_RES        macDrvrProcess(             DRV_HANDLE hMac );
static TCPIP_MAC_RES        macDrvrStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);
static TCPIP_MAC_RES        macDrvrRegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);
static TCPIP_MAC_RES        macDrvrParametersGet(       DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS * pMacParams );
static size_t               macDrvrConfigGet(           DRV_HANDLE hMac, void * configBuff, size_t buffSize, size_t * pConfigSize );
static bool                 macDrvrEventMaskSet(        DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpMacEvents, bool enable );
static bool                 macDrvrEventAcknowledge(    DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpMacEvents );
static TCPIP_MAC_EVENT      macDrvrEventPendingGet(     DRV_HANDLE hMac );
static TCPIP_MAC_RES        macDrvrEventInit(           MAC_DRIVER * pMacDrvr, TCPIP_MAC_EventF eventF, const void * eventParam );
static TCPIP_MAC_RES        macDrvrRxFilterHashTableEntrySet( DRV_HANDLE hMac, const TCPIP_MAC_ADDR * destMacAddr );
static MAC_DRVR_RESULT      macDrvrRemoveDynamicsFromPacketPool( MAC_DRIVER * pMacDrvr );

// Ethernet Link Start up states
static void                 macDrvrLinkStateStartLink(  MAC_DRIVER * pMacDrvr );
static void                 macDrvrLinkStateGetLink(    MAC_DRIVER * pMacDrvr );
static void                 macDrvrLinkStateWaitLinkUp( MAC_DRIVER * pMacDrvr );
static void                 macDrvrLinkStateNegComplete(MAC_DRIVER * pMacDrvr );
static void                 macDrvrLinkStateNegResult(  MAC_DRIVER * pMacDrvr );

/*******************************************************************************
 * Object implementation
 ******************************************************************************/
// the embedded object
const TCPIP_MAC_OBJECT DRV_EMAC0_Object =
{
    .macId                                  = TCPIP_MODULE_MAC_SAM9X60_0,
    .macType                                = TCPIP_MAC_TYPE_ETH,
    .macName                                = "EMAC0",
    .TCPIP_MAC_Initialize                   = macDrvrInitialize,
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
    .TCPIP_MAC_Deinitialize                 = macDrvrDeInitialize,
    .TCPIP_MAC_Reinitialize                 = macDrvrReInitialize,
#else
    .TCPIP_MAC_Deinitialize                 = 0,
    .TCPIP_MAC_Reinitialize                 = 0,
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
    .TCPIP_MAC_Status                       = macDrvrStatus,
    .TCPIP_MAC_Tasks                        = macDrvrTasks,
    .TCPIP_MAC_Open                         = macDrvrOpen,
    .TCPIP_MAC_Close                        = macDrvrClose,
    .TCPIP_MAC_LinkCheck                    = macDrvrLinkCheck,
    .TCPIP_MAC_RxFilterHashTableEntrySet    = macDrvrRxFilterHashTableEntrySet,
    .TCPIP_MAC_PowerMode                    = macDrvrPowerMode,
    .TCPIP_MAC_PacketTx                     = macDrvrPacketTx,
    .TCPIP_MAC_PacketRx                     = macDrvrPacketRx,
    .TCPIP_MAC_Process                      = macDrvrProcess,
    .TCPIP_MAC_StatisticsGet                = macDrvrStatisticsGet,
    .TCPIP_MAC_ParametersGet                = macDrvrParametersGet,
    .TCPIP_MAC_RegisterStatisticsGet        = macDrvrRegisterStatisticsGet,
    .TCPIP_MAC_ConfigGet                    = macDrvrConfigGet,
    .TCPIP_MAC_EventMaskSet                 = macDrvrEventMaskSet,
    .TCPIP_MAC_EventAcknowledge             = macDrvrEventAcknowledge,
    .TCPIP_MAC_EventPendingGet              = macDrvrEventPendingGet,
};

const TCPIP_MAC_OBJECT DRV_EMAC1_Object =
{
    .macId                                  = TCPIP_MODULE_MAC_SAM9X60_1,
    .macType                                = TCPIP_MAC_TYPE_ETH,
    .macName                                = "EMAC1",
    .TCPIP_MAC_Initialize                   = macDrvrInitialize,
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0) 
    .TCPIP_MAC_Deinitialize                 = macDrvrDeInitialize,
    .TCPIP_MAC_Reinitialize                 = macDrvrReInitialize,
#else
    .TCPIP_MAC_Deinitialize                 = 0,
    .TCPIP_MAC_Reinitialize                 = 0,
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
    .TCPIP_MAC_Status                       = macDrvrStatus,
    .TCPIP_MAC_Tasks                        = macDrvrTasks,
    .TCPIP_MAC_Open                         = macDrvrOpen,
    .TCPIP_MAC_Close                        = macDrvrClose,
    .TCPIP_MAC_LinkCheck                    = macDrvrLinkCheck,
    .TCPIP_MAC_RxFilterHashTableEntrySet    = macDrvrRxFilterHashTableEntrySet,
    .TCPIP_MAC_PowerMode                    = macDrvrPowerMode,
    .TCPIP_MAC_PacketTx                     = macDrvrPacketTx,
    .TCPIP_MAC_PacketRx                     = macDrvrPacketRx,
    .TCPIP_MAC_Process                      = macDrvrProcess,
    .TCPIP_MAC_StatisticsGet                = macDrvrStatisticsGet,
    .TCPIP_MAC_ParametersGet                = macDrvrParametersGet,
    .TCPIP_MAC_RegisterStatisticsGet        = macDrvrRegisterStatisticsGet,
    .TCPIP_MAC_ConfigGet                    = macDrvrConfigGet,
    .TCPIP_MAC_EventMaskSet                 = macDrvrEventMaskSet,
    .TCPIP_MAC_EventAcknowledge             = macDrvrEventAcknowledge,
    .TCPIP_MAC_EventPendingGet              = macDrvrEventPendingGet,
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
    int macIndex = macId - TCPIP_MODULE_MAC_SAM9X60;

    if( macIndex >= 0 && macIndex < sizeof( macDrvrDescription ) / sizeof( *macDrvrDescription ) )
    {
        return macIndex;
    }
    return -1;
}

#if (DRV_EMAC_INSTANCES == 1)
static __inline__ MAC_DRIVER* __attribute__((always_inline)) _EmacHandleToMacDrv(uintptr_t handle)
{
    MAC_DRIVER* pMacD = (MAC_DRIVER*)handle;
    return pMacD == macDrvrDescription ? pMacD : 0;
}
#elif (DRV_EMAC_INSTANCES == 2)
static __inline__ MAC_DRIVER* __attribute__((always_inline)) _EmacHandleToMacDrv(uintptr_t handle)
{
    MAC_DRIVER* pMacD = (MAC_DRIVER*)handle;
    return (pMacD == macDrvrDescription || pMacD == macDrvrDescription + 1) ? pMacD : 0;
}
#else
// multiple instances version
static MAC_DRIVER* _EmacHandleToMacDrv(uintptr_t handle)
{
    MAC_DRIVER* pMacD = (MAC_DRIVER*)handle;
    int macIx = pMacD - macDrvrDescription;

    if( macIx >= 0 && macIx < sizeof( macDrvrDescription ) / sizeof( *macDrvrDescription ) )
    {
        if(pMacD == macDrvrDescription + macIx)
        {
            return pMacD;
        }
    }

    return 0;
}

#endif


/*******************************************************************************
 * Function:        macDrvrInitialize
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        This function initializes the MAC controller, the MAC and the associated PHY.
 *                  It should be called to be able to schedule any MAC transmit or receive operation.
 * Note:            Only one client per MAC supported.
 ******************************************************************************/
SYS_MODULE_OBJ macDrvrInitialize(
    const SYS_MODULE_INDEX          index,
    const SYS_MODULE_INIT * const   init
    )
{
    const TCPIP_MAC_MODULE_CTRL * const     macControl = ((TCPIP_MAC_INIT *) init)->macControl;
    const TCPIP_MODULE_MAC_SAM9X60_CONFIG * initData = (const TCPIP_MODULE_MAC_SAM9X60_CONFIG *)((TCPIP_MAC_INIT *)init)->moduleData;
    MAC_DRIVER *    pMacDrvr;
    int             macIndex;
    TCPIP_MAC_RES   initRes;

    macIndex = macIdToIndex( index );
    if(macIndex < 0)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    pMacDrvr = macDrvrDescription + macIndex;

    if( SYS_STATUS_UNINITIALIZED != pMacDrvr->sysStat )
    {   // already initialized
        return (SYS_MODULE_OBJ) pMacDrvr;
    }
    if( pMacDrvr->macFlags._open )
    {   // have a client connected
        return SYS_MODULE_OBJ_INVALID;
    }
    if( macControl->memH == 0)
    {     // not possible without dynamic memory
        return SYS_MODULE_OBJ_INVALID;
    }
    if( initData == 0 )
    {   // not possible without init data!
        return SYS_MODULE_OBJ_INVALID;
    }
    if( (initData->pPhyBase == 0) || (initData->pPhyInit == 0) )
    {   // not possible without a PHY!
        return SYS_MODULE_OBJ_INVALID;
    }
    if(     (initData->nRxDescCnt < 1)
       ||   (initData->nTxDescCnt < EMAC_MIN_TX_DESCRIPTORS)
    )
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    if( initData->rxBufferSize < EMAC_MIN_RX_SIZE )
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
    memset( pMacDrvr, 0x0, sizeof( *pMacDrvr ) );

    pMacDrvr->pObj = macDrvrObjects[macIndex];
    pMacDrvr->hPhyClient = DRV_HANDLE_INVALID;
    pMacDrvr->hPhySysObject = SYS_MODULE_OBJ_INVALID;
    pMacDrvr->sysStat = SYS_STATUS_UNINITIALIZED;

    pMacDrvr->macIx = macIndex;
    pMacDrvr->phyIx = macIndex;     // use the same index for the associated PHY
    pMacDrvr->macFlags._linkPrev = 0;

    // use initialization data
    pMacDrvr->allocH = macControl->memH;
    pMacDrvr->callBack.callocF = macControl->callocF;
    pMacDrvr->callBack.freeF = macControl->freeF;
    pMacDrvr->callBack.pktAllocF = macControl->pktAllocF;
    pMacDrvr->callBack.pktFreeF = macControl->pktFreeF;
    pMacDrvr->callBack.pktAckF = macControl->pktAckF;
    pMacDrvr->callBack.synchF = macControl->synchF;

    pMacDrvr->_gapDcptOffset = macControl->gapDcptOffset;
    pMacDrvr->_gapDcptSize = macControl->gapDcptSize;
    pMacDrvr->_controlFlags = macControl->controlFlags;
    pMacDrvr->_dataOffset = (macControl->controlFlags & TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2) ? 2 : 0;

    // copy the configuration data
    pMacDrvr->config = *initData;
    // set the MAC address received from system config
    memcpy( pMacDrvr->config.macAddress.v,
            macControl->ifPhyAddress.v,
            sizeof( macControl->ifPhyAddress )
            );

    SYS_INT_SourceDisable( pMacDrvr->config.macIntSrc );    // disable interrupt vector
    macDrvrLibInitializeEmac( pMacDrvr );                   // stop and configure mac registers
    initRes = macDrvrEventInit( pMacDrvr,
                                macControl->eventF,
                                macControl->eventParam
                               );
    if( TCPIP_MAC_RES_OK == initRes )
    {
        macDrvrLibDescriptorsPoolClear();
        macDrvrLibDescriptorsPoolAssignment( pMacDrvr, MAC_DRVR_DCPT_TYPE_TX );
        macDrvrLibDescriptorsPoolAssignment( pMacDrvr, MAC_DRVR_DCPT_TYPE_RX );

        initRes = macDrvrPhyInitialize( pMacDrvr );
    }
    if( TCPIP_MAC_RES_OK == initRes )
    {
        initRes = macDrvrLibTxInit( pMacDrvr );
    }
    if( TCPIP_MAC_RES_OK == initRes )
    {
        initRes = macDrvrLibRxInit( pMacDrvr );
    }
    if( TCPIP_MAC_RES_OK == initRes )
    {
        initRes = macDrvrLibRxAllocate( pMacDrvr,
                                        pMacDrvr->config.nRxStaticBufferCnt,
                                        MAC_RX_STICKY_BUFFERS
                                        );
    }
    if( TCPIP_MAC_RES_OK == initRes )
    {
        initRes = macDrvrLibGetDynamicBuffers( pMacDrvr );
    }
    if( TCPIP_MAC_RES_OK == initRes )
    {
        initRes = macDrvrLibRxPopulateDescriptors( pMacDrvr );
    }
    if( initRes == TCPIP_MAC_RES_OK )
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

#if TCPIP_STACK_MAC_DOWN_OPERATION
/*******************************************************************************
 * Function:        macDrvrDeInitialize
 * PreCondition:    None
 * Input:           stackData - standard stack initialization structure
 * Output:          TCPIP_MAC_RES
 * Side Effects:    None
 * Overview:        This function de-initializes the Eth MAC controller.
 * Note:            None
 ******************************************************************************/
void macDrvrDeInitialize( SYS_MODULE_OBJ object )
{
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( object );
    if(pMacDrvr != 0)
    {
        switch( pMacDrvr->sysStat )
        {
            case SYS_STATUS_BUSY:
            case SYS_STATUS_READY:
            case SYS_STATUS_ERROR:
                macDrvrDeInit( pMacDrvr );
                break;
            case SYS_STATUS_ERROR_EXTENDED:
            case SYS_STATUS_UNINITIALIZED:
            default:
                break;
        }
    }
}

void macDrvrReInitialize( SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init )
{
    // not supported
}
#endif

SYS_STATUS macDrvrStatus( SYS_MODULE_OBJ object )
{
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( object );
    if(pMacDrvr != 0)
    {
        return pMacDrvr->sysStat;
    }

    return SYS_STATUS_ERROR;    
}

void macDrvrTasks( SYS_MODULE_OBJ object )
{
    TCPIP_ETH_PAUSE_TYPE            pauseType;
    DRV_ETHPHY_CLIENT_STATUS        phyStat;
    DRV_HANDLE                      hPhyClient;
    DRV_ETHPHY_RESULT               phyInitRes;
    const DRV_ETHPHY_OBJECT_BASE *  pPhyBase;
    static bool                     discardFlag = false;

    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( object );
    if(pMacDrvr == 0 || ( SYS_STATUS_UNINITIALIZED == pMacDrvr->sysStat ))
    {   // nothing to do
        return;
    }

    pPhyBase = pMacDrvr->config.pPhyBase;

    if((pMacDrvr->_controlFlags & TCPIP_MAC_CONTROL_NO_LINK_CHECK) == 0) 
    {
        pPhyBase->DRV_ETHPHY_Tasks( pMacDrvr->hPhySysObject );

        if( !(DRV_ETHPHY_LINK_ST_UP & pMacDrvr->linkInfo.negResult.linkStatus) )
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
            phyStat = pPhyBase->DRV_ETHPHY_ClientStatus( hPhyClient );
            if( phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY )
            {   // wait some more
                break;
            }

            phyInitRes = pPhyBase->DRV_ETHPHY_ClientOperationResult( hPhyClient );
            if ( phyInitRes != DRV_ETHPHY_RES_OK )
            {
                macDrvrDeInit( pMacDrvr );
                // keep the error status though
                pMacDrvr->sysStat = SYS_STATUS_ERROR;
                SYS_ERROR_PRINT( SYS_ERROR_ERROR, "DRV PHY init failed: %d\r\n", phyInitRes );
                break;
            }

            // PHY was detected properly
            pMacDrvr->macFlags._linkPresent = 1;
            if( (pMacDrvr->config.ethFlags & TCPIP_ETH_OPEN_AUTO) != 0 )
            {   // we'll just wait for the negotiation to be done
                pMacDrvr->macFlags._linkNegotiation = 1;       // performing the negotiation
            }
            else
            {   // no need of negotiation results; just update the MAC
                pauseType = (pMacDrvr->config.ethFlags & TCPIP_ETH_OPEN_FDUPLEX) ? macDrvrPAUSE_CPBL_MASK : TCPIP_ETH_PAUSE_TYPE_NONE;
                macDrvrLibMacOpen( pMacDrvr, pMacDrvr->linkInfo.flags, pauseType );
            }

            pMacDrvr->linkInfo.upTick = SYS_TMR_TickCountGet();     // the initialization time
            pMacDrvr->linkInfo.waitTick =
                ((SYS_TMR_TickCounterFrequencyGet() * pMacDrvr->config.linkInitDelay) + 999 ) / 1000;
            pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_START_LINK;

            pMacDrvr->sysStat = SYS_STATUS_READY;
            break;

        case SYS_STATUS_ERROR_EXTENDED:
        case SYS_STATUS_ERROR:
        case SYS_STATUS_UNINITIALIZED:
        case SYS_STATUS_READY:
        case SYS_STATUS_READY_EXTENDED:
        default:
            break;
    }
}

size_t macDrvrConfigGet(
    DRV_HANDLE  hMac,
    void *      configBuff,
    size_t      buffSize,
    size_t *    pConfigSize
    )
{
    size_t          response = 0;
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    if(pMacDrvr != 0)
    {
        if( pConfigSize )
        {
            *pConfigSize = sizeof( TCPIP_MODULE_MAC_SAM9X60_CONFIG );
        }
        if( configBuff && buffSize >= sizeof( TCPIP_MODULE_MAC_SAM9X60_CONFIG ) )
        {   // can copy the data
            TCPIP_MODULE_MAC_SAM9X60_CONFIG * pMacConfig = (TCPIP_MODULE_MAC_SAM9X60_CONFIG *) configBuff;

            *pMacConfig = pMacDrvr->config;
            response = sizeof( TCPIP_MODULE_MAC_SAM9X60_CONFIG );
        }
    }
    return response;
}

DRV_HANDLE macDrvrOpen(
    const SYS_MODULE_INDEX  drvIndex,
    const DRV_IO_INTENT     intent
    )
{
    int             macIndex;
    MAC_DRIVER *    pMacDrvr;
    DRV_HANDLE      hMac = DRV_HANDLE_INVALID;

    macIndex = macIdToIndex( drvIndex );
    if( macIndex >= 0 )
    {
        pMacDrvr = macDrvrDescription + macIndex;
        switch( pMacDrvr->sysStat )
        {
            case SYS_STATUS_UNINITIALIZED:
            case SYS_STATUS_READY:
            case SYS_STATUS_BUSY:
                if( !pMacDrvr->macFlags._open )
                {   // only one client for now
                    pMacDrvr->macFlags._open = 1;
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
                break;
        }
    }
    return hMac;
}

void macDrvrClose( DRV_HANDLE hMac )
{
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    if(pMacDrvr != 0)
    {
        pMacDrvr->macFlags._open = 0;
    }
}

/*******************************************************************************
 * TX functions
 */
uint32_t txGetValidPacketCount( TCPIP_MAC_PACKET * pMacPacket, MAC_DRIVER * pMacDrvr )
{
    TCPIP_MAC_PACKET *  pTemp;
    TCPIP_MAC_DATA_SEGMENT* pSeg;    
    uint32_t            packetCount = 0;

    for( pTemp = pMacPacket; pTemp; pTemp = pTemp->next )
    {
        pSeg = pTemp->pDSeg;
        if(pSeg == 0 || pSeg->segLen == 0)
        {   // having data is the only integrity check for validity
            // one bad apple ruins the barrel
            packetCount = 0;
            break;
        }
        ++packetCount;
    }
    return packetCount;
}

TCPIP_MAC_RES macDrvrPacketTx( DRV_HANDLE hMac, TCPIP_MAC_PACKET * pMacPacket )
{
    TCPIP_MAC_PACKET *  pTemp;
    TCPIP_MAC_RES       response = TCPIP_MAC_RES_PACKET_ERR;
    uint32_t            numPacketsToSend = 0;

    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    if(pMacDrvr == 0)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }
    
    macDrvrTxLock( pMacDrvr );
    numPacketsToSend = txGetValidPacketCount( pMacPacket, pMacDrvr );
    if( numPacketsToSend )
    {
        response = TCPIP_MAC_RES_OK;
        for( pTemp = pMacPacket; numPacketsToSend--; pTemp = pTemp->next )
        {   // Update pktFlags
            // packets are expected to have pDSeg updated with the packet pointer
            pTemp->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
        }
        __DSB();
        macDrvrPacketListTailAdd(   &pMacDrvr->txPendingList,
                                    pMacPacket
                                    );
        macDrvrTxPendingPackets( pMacDrvr );
        macDrvrTxAcknowledge( pMacDrvr );
    }
    macDrvrTxUnlock( pMacDrvr );

    return response;
}

/*******************************************************************************
 * RX functions
 */
// returns a pending RX packet if any have been captured by the mac dma
TCPIP_MAC_PACKET * macDrvrPacketRx(
    DRV_HANDLE                          hMac,
    TCPIP_MAC_RES *                     pRes,
    TCPIP_MAC_PACKET_RX_STAT *   pPktStatus
    )
{
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    if(pMacDrvr == 0)
    {
        return 0;
    }

    TCPIP_MAC_PACKET *      pMacPacket = 0;
    MAC_DRVR_RXDCPT_STATUS  packetStatus = {.val = 0};
    TCPIP_MAC_RES           response = TCPIP_MAC_RES_PENDING;

    macDrvrRxLock( pMacDrvr );
    if( MAC_DRVR_RES_OK == rxGetPacket( pMacDrvr,
                                        &pMacPacket,
                                        &packetStatus
                                        )
    )
    {   // valid ethernet packet;
        // format properly and return the packet pointer
        // Ethernet header(14 bytes) stored in data packet (or in first data buffer for multi-buffer data).
        // Reduce header length to get data segment length
        response = TCPIP_MAC_RES_OK;
        pMacDrvr->rxStat.nRxOkPackets++;
        // set proper segment length
        pMacPacket->pDSeg->segLen -= sizeof( TCPIP_MAC_ETHERNET_HEADER );
        // Note: re-set pMacLayer and pNetLayer; IPv6 changes these pointers inside the packet!
        pMacPacket->pMacLayer = pMacPacket->pDSeg->segLoad;
        pMacPacket->pNetLayer = pMacPacket->pMacLayer + sizeof( TCPIP_MAC_ETHERNET_HEADER );
        pMacPacket->tStamp = SYS_TMR_TickCountGet();
        pMacPacket->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
        pMacPacket->pktFlags &= ~TCPIP_MAC_PKT_FLAG_CAST_MASK;
        if( packetStatus.bm.bBroadcastDetected )
        {
            pMacPacket->pktFlags |= TCPIP_MAC_PKT_FLAG_BCAST;
        }
        else if( packetStatus.bm.bMulticastHashMatch )
        {
            pMacPacket->pktFlags |= TCPIP_MAC_PKT_FLAG_MCAST;
        }
        else if( packetStatus.bm.bUnicastHashMatch )
        {
            pMacPacket->pktFlags |= TCPIP_MAC_PKT_FLAG_UNICAST;
        }
        __DSB();
    }
    macDrvrRxUnlock( pMacDrvr );
    if( pRes )
    {
        *pRes = response;
    }
    if( pPktStatus )
    {
        memcpy( (void *) pPktStatus, (TCPIP_MAC_PACKET_RX_STAT *) &packetStatus, sizeof( TCPIP_MAC_PACKET_RX_STAT ) );
    }

    return pMacPacket;
}

/*******************************************************************************
 * Function:        TCPIP_MAC_RES macDrvrRxFilterHashTableEntrySet(
 *                                  DRV_HANDLE hMac,
 *                                  TCPIP_MAC_ADDR * destMacAddr
 *                                  )
 *
 * PreCondition:    macDrvrInitialize() should have been called.
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
 *                  you can call macDrvrRxFilterHashTableEntrySet() using a
 *                  00-00-00-00-00-00 destination MAC address, which will clear
 *                  the entire hash table and disable the hash table filter.
 *                  This will allow you to then read the necessary destination
 *                  addresses.
 ******************************************************************************/
TCPIP_MAC_RES macDrvrRxFilterHashTableEntrySet(
    DRV_HANDLE              hMac,
    const TCPIP_MAC_ADDR *  destMacAddr
    )
{
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    if(pMacDrvr == 0)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    MAC_DRVR_HASH   hash;
    uint8_t         nullMacAddr[ 6 ] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Clear the Hash Table bits and disable the Hash Table Filter if a special
    // 00-00-00-00-00-00 destination MAC address is provided.
    if(     0 == destMacAddr
        ||  0 == memcmp( destMacAddr->v, nullMacAddr, sizeof( nullMacAddr ) )
    )
    {
        hash.value = 0x0;   // Clear to all 0 for 64-bit Hash value to reject all hash match
        hash.calculate = false;     // No hash calculation; directly set hash register
    }
    else
    {
        hash.destMacAddr = destMacAddr;
        hash.calculate = true;      // calculate hash and set hash register
    }
    macDrvrLibRxFilterHashCalculate( pMacDrvr, &hash );

    return TCPIP_MAC_RES_OK;
}

bool macDrvrPowerMode( DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode )
{   // not supported
    return true;
}

TCPIP_MAC_RES macDrvrProcess( DRV_HANDLE hMac )
{

    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    if(pMacDrvr == 0)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    TCPIP_MAC_RES   response;
    macDrvrTxLock( pMacDrvr );
    response = macDrvrTxPendingPackets( pMacDrvr );
    macDrvrTxAcknowledge( pMacDrvr );
    macDrvrTxUnlock( pMacDrvr );

    return response;
}

TCPIP_MAC_RES macDrvrParametersGet( DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS * pMacParams )
{
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    if(pMacDrvr == 0)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    TCPIP_MAC_RES   response = TCPIP_MAC_RES_IS_BUSY;
    if( pMacDrvr->sysStat == SYS_STATUS_READY )
    {
        if( pMacParams )
        {
            macDrvrLibGetMacAddr( pMacDrvr, pMacParams->ifPhyAddress.v );
            pMacParams->processFlags = (TCPIP_MAC_PROCESS_FLAG_RX | TCPIP_MAC_PROCESS_FLAG_TX);
            pMacParams->macType = TCPIP_MAC_TYPE_ETH;
            pMacParams->linkMtu = TCPIP_MAC_LINK_MTU_ETH;
        }
        response = TCPIP_MAC_RES_OK;
    }
    return response;
}

static TCPIP_MAC_RES macDrvrStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics)
{
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    return (pMacDrvr != 0) ? EmacDrvrStatisticsGet(pMacDrvr, pRxStatistics, pTxStatistics) : TCPIP_MAC_RES_OP_ERR;
}

static TCPIP_MAC_RES macDrvrRegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries)
{
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    return (pMacDrvr != 0) ?  EmacDrvrRegisterStatisticsGet(pMacDrvr, pRegEntries, nEntries, pHwEntries) : TCPIP_MAC_RES_OP_ERR; 
}


/*******************************************************************************
 * local functions and helpers
 */
static TCPIP_MAC_RES macDrvrTxPendingPackets( MAC_DRIVER * pMacDrvr )
{
    uint32_t                    segsAvailable;
    uint32_t                    segments;
    TCPIP_MAC_PACKET *          pMacPacket;
    TCPIP_MAC_DATA_SEGMENT *    pDSeg;

    while( pMacDrvr->txPendingList.nNodes )
    {
        segments = 0;
        pDSeg = pMacDrvr->txPendingList.head->pDSeg;
        while( pDSeg )
        {
            pDSeg = pDSeg->next;
            ++segments;
        }
        // number of available spots is equal to the moduloDistance; but, we reserve
        // one space for the hard stop -- hence '>=' is the appropriate test for space
        segsAvailable = getModuloDistance(  pMacDrvr->txInsertPt,
                                            pMacDrvr->txExtractPt,
                                            pMacDrvr->config.nTxDescCnt
                                            );
        if( segments >= segsAvailable )
        {   // not enough descriptors to place the entire packet
            pMacDrvr->txStat.nTxQueueFull++;
            break;
        }
        pMacPacket = macDrvrPacketListHeadRemove( &pMacDrvr->txPendingList );
        macDrvrLibTxSendPacket( pMacDrvr,
                                pMacPacket,
                                segments
                                );
    }

    return TCPIP_MAC_RES_OK;
}

static void macDrvrTxAcknowledge( MAC_DRIVER * pMacDrvr )
{
    TCPIP_MAC_PACKET *  pMacPacket;
    uint16_t            segmentsReady;
    uint16_t            extract;

    // acknowledge when a complete segment is sent
    // only first segment has correct ownership setting -- emac does not update the others!
    while(  (pMacDrvr->txExtractPt != pMacDrvr->txInsertPt)
        &&  (DRVRnEMAC_TX_USE_BIT & pMacDrvr->pTxDesc[ pMacDrvr->txExtractPt ].status.val)
    )
    {
        segmentsReady = 0;
        extract = pMacDrvr->txExtractPt;
        if( (EMAC_TX_ERR_BITS & pMacDrvr->pTxDesc[ extract ].status.val) )
        {   // transmit error: emac has restarted at base, descriptors should be reset
            macDrvrTxAckAllPending( pMacDrvr, TCPIP_MAC_PKT_ACK_BUFFER_ERR );
            macDrvrLibTxInit( pMacDrvr );
            break;
        }

        // get aligned buffer address from Tx Descriptor Buffer Address
        uint8_t* pSegBuff = (uint8_t*)(EMAC_RX_ADDRESS_MASK & pMacDrvr->pTxDesc[ extract ].bufferAddress);
        // get packet pointer from buffer gap descriptor
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(pSegBuff + pMacDrvr->_gapDcptOffset);
        pMacPacket = pGap->segmentPktPtr;
        
        while( extract != pMacDrvr->txInsertPt )
        {
            ++segmentsReady;
            // only first segment will have correct ownership bit setting -- emac does not update the others!
            if( !(EMAC_TX_LAST_BUFFER_BIT & pMacDrvr->pTxDesc[ extract ].status.val) )
            {
                extract = moduloIncrement( extract, pMacDrvr->config.nTxDescCnt );
            }
            else
            {   // complete segment sent
                pMacDrvr->txExtractPt = moduloIncrement( extract, pMacDrvr->config.nTxDescCnt );
                pMacPacket->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
                pMacDrvr->callBack.pktAckF( pMacPacket, TCPIP_MAC_PKT_ACK_TX_OK, pMacDrvr->pObj->macId );
                pMacDrvr->txStat.nTxOkPackets++;
                while( segmentsReady-- )
                {   // backup and cleanup descriptors, leaving the appropriate bits in place
                    pMacDrvr->pTxDesc[ extract ].status.val |= DRVRnEMAC_TX_USE_BIT;
                    __DSB();
                    pMacDrvr->pTxDesc[ extract ].status.val &= (DRVRnEMAC_TX_USE_BIT | EMAC_TX_WRAP_BIT);
                    pMacDrvr->pTxDesc[ extract ].bufferAddress = 0;
                    extract = moduloDecrement( extract, pMacDrvr->config.nTxDescCnt );
                }
                break;      // inner loop finished with this packet; rise to outer loop now
            }
        }
    }
}

#if (TCPIP_STACK_MAC_DOWN_OPERATION == 0)
static void macDrvrDeInit( MAC_DRIVER * pMacDrvr )
{
}
#else
static void macDrvrDeInit( MAC_DRIVER * pMacDrvr )
{
    const DRV_ETHPHY_OBJECT_BASE *  pPhyBase = pMacDrvr->config.pPhyBase;
    emac_registers_t *              pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;

    pMacRegs->EMAC_NCR &= ~(EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk); // must be done first
    SYS_INT_SourceDisable( pMacDrvr->config.macIntSrc );        // disable interrupt vector
    SYS_INT_SourceStatusClear( pMacDrvr->config.macIntSrc );    // clear any pending interrupt flag
    macDrvrLibClose( pMacDrvr, MAC_DRVR_CLOSE_DEFAULT );
    macDrvrEventDeInit( pMacDrvr );
    if( pMacDrvr->hPhyClient != DRV_HANDLE_INVALID )
    {
        pPhyBase->DRV_ETHPHY_Close( pMacDrvr->hPhyClient );
        pMacDrvr->hPhyClient = DRV_HANDLE_INVALID;
    }
    if( pMacDrvr->hPhySysObject != SYS_MODULE_OBJ_INVALID )
    {
        pPhyBase->DRV_ETHPHY_Deinitialize( pMacDrvr->hPhySysObject );
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

    for( ii = pMacDrvr->rxMacPacketPool.nNodes; ii; --ii )
    {
        pMacPacket = macDrvrPacketListHeadRemove( &pMacDrvr->rxMacPacketPool );
        if( pMacPacket )
        {
            if( (pMacPacket->pDSeg->segFlags & TCPIP_MAC_SEG_FLAG_RX_STICKY) )
            {
                macDrvrPacketListTailAdd( &pMacDrvr->rxMacPacketPool,
                                          pMacPacket
                                          );
            }
            else
            {
                macDrvrPacketListTailAdd( &pMacDrvr->rxDynamicReleasePool,
                                          pMacPacket
                                          );
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
    macDrvrRemoveDynamicsFromPacketPool( pMacDrvr );
    // free all buffers in the rxDynamicReleasePool
    macDrvrLibFreeDynamicBuffers( pMacDrvr );
    // free the packets on the side line
    while( pMacDrvr->rxMacPacketPool.nNodes )
    {
        pMacPacket = macDrvrPacketListHeadRemove( &pMacDrvr->rxMacPacketPool );
        pMacDrvr->callBack.pktFreeF( pMacPacket );
    }
}
#endif

static void macDrvrTxAckAllPending(
    MAC_DRIVER *            pMacDrvr,
    TCPIP_MAC_PKT_ACK_RES   ackRes
    )
{   // remove and callback acknowledgment functions of packets in transmission queue
    TCPIP_MAC_PACKET * pMacPacket;

    while( pMacDrvr->txPendingList.nNodes )
    {   // release pending list packets
        pMacPacket = macDrvrPacketListHeadRemove( &pMacDrvr->txPendingList );
        if( pMacDrvr->callBack.pktAckF )
        {
            pMacDrvr->callBack.pktAckF( pMacPacket,
                                        ackRes,
                                        pMacDrvr->pObj->macId
                                        );
        }
    }
}

static void macDrvrTxPacketPoolReset(
    MAC_DRIVER *            pMacDrvr,
    TCPIP_MAC_PKT_ACK_RES   ackRes
    )
{
    // acknowledge packets the mac has in it's descriptors
    macDrvrTxAcknowledge( pMacDrvr );
    // remove the packets queued for transmission
    macDrvrTxAckAllPending( pMacDrvr, ackRes );
    // everything should be recovered so now we can reset the tx
    macDrvrLibTxInit( pMacDrvr );       // also called at standard init
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
static MAC_EVENTS translateTcpMacToMacEvent(
    MAC_DRIVER *    pMacDrvr,
    TCPIP_MAC_EVENT tcpMacEvents
    )
{
    // mapping RX Used bit interrupt to BUFNA; since there is no interrupt bit
    // for BUFNA and RXUBR interrupt will trigger with BUFNA
    MAC_EVENTS macEvents = 0;

    macEvents |= (tcpMacEvents & TCPIP_MAC_EV_RX_DONE   ) ? MAC_EV_RXCOMPLETE : 0;
    macEvents |= (tcpMacEvents & TCPIP_MAC_EV_RX_OVFLOW ) ? MAC_EV_RXOVERRUN : 0;
    macEvents |= (tcpMacEvents & TCPIP_MAC_EV_RX_BUFNA  ) ? MAC_EV_RXUSEDBITREAD : 0;
    macEvents |= (tcpMacEvents & TCPIP_MAC_EV_TX_DONE   ) ? MAC_EV_TXCOMPLETE : 0;
    macEvents |= (tcpMacEvents & TCPIP_MAC_EV_TX_ABORT  ) ? ( MAC_EV_TXUSEDBITREAD
                                                            | MAC_EV_TXUNDERRUN
                                                            | MAC_EV_RETRYLIMITEXCEED
                                                            | MAC_EV_TXFRAMECORRUPT
                                                            )
                                                            : 0;
    macEvents |= (tcpMacEvents & TCPIP_MAC_EV_TX_BUSERR ) ? MAC_EV_HRESPNOTOK : 0;

    return macEvents;
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
static TCPIP_MAC_EVENT translateMacToTcpMacEvent(
    MAC_DRIVER *    pMacDrvr,
    MAC_EVENTS      macEvents
    )
{
    // mapping RX Used bit interrupt to BUFNA; since there is no interrupt bit
    // for BUFNA and RXUBR interrupt will trigger with BUFNA
    TCPIP_MAC_EVENT tcpMacEvents = 0;
    tcpMacEvents |= (macEvents & MAC_EV_RXCOMPLETE      ) ? TCPIP_MAC_EV_RX_DONE : 0;
    tcpMacEvents |= (macEvents & MAC_EV_RXOVERRUN       ) ? TCPIP_MAC_EV_RX_OVFLOW : 0;
    tcpMacEvents |= (macEvents & MAC_EV_RXUSEDBITREAD   ) ? TCPIP_MAC_EV_RX_BUFNA : 0;
    tcpMacEvents |= (macEvents & MAC_EV_TXCOMPLETE  ) ? TCPIP_MAC_EV_TX_DONE : 0;
    tcpMacEvents |= (macEvents & (MAC_EV_TXUSEDBITREAD
                                | MAC_EV_TXUNDERRUN
                                | MAC_EV_RETRYLIMITEXCEED
                                | MAC_EV_TXFRAMECORRUPT
                                ))
                                ? TCPIP_MAC_EV_TX_ABORT
                                : 0;
    tcpMacEvents |= (macEvents & MAC_EV_HRESPNOTOK) ? TCPIP_MAC_EV_TX_BUSERR : 0;

    return tcpMacEvents;
}

/*******************************************************************************
  Function:
    TCPIP_MAC_RES macDrvrEventInit( MAC_DRIVER *        pMacDrvr,
                                    TCPIP_MAC_EventF    eventF,
                                    const void *        eventParam
                                    )
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
static TCPIP_MAC_RES macDrvrEventInit(
    MAC_DRIVER *        pMacDrvr,
    TCPIP_MAC_EventF    eventF,
    const void *        eventParam
    )
{
    MAC_DRVR_EVENT_DCPT * pEventDescription = &pMacDrvr->eventDescription;
    MAC_EVENTS            macEvents = (MAC_EV_RX | MAC_EV_TX);

    pEventDescription->macEventsEnabled = macEvents;
    pEventDescription->macEventsPending = 0;
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
#if TCPIP_STACK_MAC_DOWN_OPERATION
static TCPIP_MAC_RES macDrvrEventDeInit( MAC_DRIVER * pMacDrvr )
{
    MAC_DRVR_EVENT_DCPT * pEventDescription = &pMacDrvr->eventDescription;

    pEventDescription->macEventsEnabled = 0;
    pEventDescription->tcpMacEventsEnabled = TCPIP_MAC_EV_NONE;

    pEventDescription->macEventsPending = 0;
    pEventDescription->tcpMacEventsPending = TCPIP_MAC_EV_NONE;

    return TCPIP_MAC_RES_OK;
}
#endif

/*******************************************************************************
  Function:
    bool    macDrvrEventMaskSet( DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable )

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
             macDrvrEventAcknowledge() so that they can be re-enabled.

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
    macDrvrEventMaskSet( hMac, TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA, true );
    </code>

  Remarks:
    The event notification system enables the user of the TCPIP stack to call into
    the stack for processing only when there are relevant events rather than being
    forced to periodically call from within a loop.

    If the notification events are nill the interrupt processing will be disabled.
    Otherwise the event notification will be enabled and the interrupts relating
    to the requested events will be enabled.

    Note that once an event has been caught by the stack ISR (and reported if a
    notification is in place) it will be disabled until the macDrvrEventAcknowledge()
    is called.

*******************************************************************************/
bool macDrvrEventMaskSet(
    DRV_HANDLE      hMac,
    TCPIP_MAC_EVENT tcpMacEvents,
    bool            enable
    )
{
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    if(pMacDrvr == 0)
    {
        return false;
    }

    MAC_DRVR_EVENT_DCPT *   pEventDescription = &pMacDrvr->eventDescription;
    emac_registers_t *      pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    MAC_EVENTS              macEvents;
    bool                    previouslyActive;

    previouslyActive = SYS_INT_SourceDisable( pMacDrvr->config.macIntSrc );
    if( enable )
    {
        previouslyActive = true;            // will start interrupts even if formerly not on
        tcpMacEvents = tcpMacEvents | pEventDescription->tcpMacEventsEnabled;
    }
    else
    {
        tcpMacEvents = tcpMacEvents & ~pEventDescription->tcpMacEventsEnabled;;
    }
    pEventDescription->tcpMacEventsEnabled = tcpMacEvents;
    macEvents = translateTcpMacToMacEvent( pMacDrvr, tcpMacEvents );
    pEventDescription->macEventsEnabled = macEvents;

    macEvents &= ~pEventDescription->macEventsPending;      // keep un-ack events
    (void) pMacRegs->EMAC_ISR;                              // clear the interrupt status
    pMacRegs->EMAC_IDR = (~macEvents) & EMAC_IDR_Msk;
    pMacRegs->EMAC_IER =  macEvents & EMAC_IER_Msk;
    SYS_INT_SourceRestore( pMacDrvr->config.macIntSrc, previouslyActive );
    if( previouslyActive )
    {
        macDrvrLibTransferEnable( pMacDrvr );
    }

    return true;
}

/*******************************************************************************
  Function:
    bool macDrvrEventAcknowledge( DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpMacEvents )

  Summary:
    Acknowledges and re-enables processed events.

  Description:
    This function acknowledges and re-enables processed events.
    Multiple events can be or'd together as they are processed together.
    The events acknowledged by this function should be the events that have been
    retrieved from the stack by calling macDrvrEventPendingGet() or have been
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
    macDrvrEventAcknowledge( hMac, stackNewEvents );
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
bool macDrvrEventAcknowledge( DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpMacEvents )
{
    bool    retval = false;
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    if(pMacDrvr != 0)
    {

        MAC_DRVR_EVENT_DCPT *   pEventDescription = &pMacDrvr->eventDescription;
        MAC_EVENTS              macEvents;
        bool                    previouslyActive;

        if( pEventDescription->tcpMacEventsEnabled )
        {   // already have some active
            retval = true;
            macEvents = translateTcpMacToMacEvent( pMacDrvr, tcpMacEvents );
            previouslyActive = SYS_INT_SourceDisable( pMacDrvr->config.macIntSrc );    // stop ints for a while
            pEventDescription->tcpMacEventsPending &= ~tcpMacEvents;    // no longer pending
            pEventDescription->macEventsPending &= ~macEvents;          // no longer pending
            SYS_INT_SourceRestore( pMacDrvr->config.macIntSrc, previouslyActive );     // re-enable
        }
    }
    return retval;
}

/*******************************************************************************
  Function:
    TCPIP_MAC_EVENT macDrvrEventPendingGet( DRV_HANDLE hMac )

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
    TCPIP_MAC_EVENT currEvents = macDrvrEventPendingGet( hMac );
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
TCPIP_MAC_EVENT macDrvrEventPendingGet( DRV_HANDLE hMac )
{
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    return pMacDrvr != 0 ? pMacDrvr->eventDescription.tcpMacEventsPending : TCPIP_MAC_EV_NONE;
}

/*******************************************************************************
 * local functions
 */

/*******************************************************************************
 * Function:        macDrvrTaskIsr
 *
 * PreCondition:    macDrvrEventInit, macDrvrEventMaskSet should have been called
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
    macDrvrTaskIsr( (SYS_MODULE_OBJ) TCPIP_MODULE_MAC_SAM9X60_0 );
}

void EMAC1_InterruptHandler( void )
{
    macDrvrTaskIsr( (SYS_MODULE_OBJ) TCPIP_MODULE_MAC_SAM9X60_1 );
}

void macDrvrTaskIsr( SYS_MODULE_OBJ sysObjHandle )
{
    MAC_DRIVER *            pMacDrvr = &macDrvrDescription[ (TCPIP_MODULE_MAC_ID) sysObjHandle - TCPIP_MODULE_MAC_SAM9X60 ];
    emac_registers_t *      pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    MAC_DRVR_EVENT_DCPT *   pEventDescription = &pMacDrvr->eventDescription;
    MAC_EVENTS              macEventsOfInterest = (MAC_EVENTS) pMacRegs->EMAC_ISR;

    macEventsOfInterest &= pEventDescription->macEventsEnabled; // keep just the relevant events
    if( macEventsOfInterest )
    {
        pEventDescription->macEventsPending |= macEventsOfInterest;
        pEventDescription->tcpMacEventsPending |= translateMacToTcpMacEvent( pMacDrvr, macEventsOfInterest );
        // clear the tx transmit status
        pMacRegs->EMAC_TSR = EMAC_TSR_COMP_Msk;
        if( pEventDescription->tcpNotifyFnc )
        {   // let the user know
            pEventDescription->tcpNotifyFnc( pEventDescription->tcpMacEventsPending,
                                             pEventDescription->tcpNotifyParam
                                             );
        }
    }
}

/*******************************************************************************
 * Function:    macDrvrPhyInitialize
 * Summary:     PHY initialization
 ******************************************************************************/
static SYS_MODULE_OBJ
macDrvrPhyInitialize( MAC_DRIVER * pMacDrvr )
{
    DRV_HANDLE                      hPhyClient;
    DRV_ETHPHY_SETUP                phySetup;
    const DRV_ETHPHY_OBJECT_BASE *  pPhyBase =  pMacDrvr->config.pPhyBase;
    int                             phyIx = pMacDrvr->phyIx;
    TCPIP_MAC_RES                   result = TCPIP_MAC_RES_PHY_INIT_FAIL;
    SYS_MODULE_OBJ                  hPhySysObject;

    hPhySysObject = pPhyBase->DRV_ETHPHY_Initialize( phyIx, (SYS_MODULE_INIT *) pMacDrvr->config.pPhyInit );
    if( SYS_MODULE_OBJ_INVALID != hPhySysObject )
    {
        hPhyClient = pPhyBase->DRV_ETHPHY_Open( phyIx, DRV_IO_INTENT_READWRITE );
        if ( DRV_HANDLE_INVALID != hPhyClient )
        {
            pMacDrvr->hPhySysObject = hPhySysObject;
            pMacDrvr->hPhyClient = hPhyClient;

            phySetup.phyAddress = pMacDrvr->config.pPhyInit->phyAddress;
            phySetup.openFlags = pMacDrvr->config.ethFlags;
            phySetup.configFlags = pMacDrvr->config.pPhyInit->phyFlags;
            phySetup.macPauseType = macDrvrPAUSE_CPBL_MASK;
            phySetup.resetFunction = pMacDrvr->config.pPhyInit->resetFunction;
            if( 0 <= pPhyBase->DRV_ETHPHY_Setup( hPhyClient, &phySetup, &pMacDrvr->linkInfo.flags ) )
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
    macDrvrLinkStateStartLink,
    macDrvrLinkStateGetLink,
    macDrvrLinkStateWaitLinkUp,
    macDrvrLinkStateNegComplete,
    macDrvrLinkStateNegResult,
};

bool macDrvrLinkCheck( DRV_HANDLE hMac )
{   // verify the link status
    bool            retval = false;
    MAC_DRIVER * pMacDrvr = _EmacHandleToMacDrv( hMac );
    if(pMacDrvr != 0 && pMacDrvr->macFlags._linkPresent )
    {
        const DRV_ETHPHY_OBJECT_BASE *  pPhyBase = pMacDrvr->config.pPhyBase;
        pPhyBase->DRV_ETHPHY_Tasks( pMacDrvr->hPhySysObject );

        macDrvrLinkStateTbl[ pMacDrvr->linkInfo.checkState ]( pMacDrvr );
        if( pMacDrvr->macFlags._linkPrev )
        {
            retval = true;
        }
    }
    return retval;
}

static void macDrvrLinkStateDown( MAC_DRIVER * pMacDrvr )
{
    // link is down; reset state flags, initialization time
    pMacDrvr->macFlags._linkPrev = 0;
    pMacDrvr->macFlags._linkUpDone = 0;
    pMacDrvr->linkInfo.upTick = SYS_TMR_TickCountGet();
    pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_START_LINK;
}

static void macDrvrLinkStateStartLink( MAC_DRIVER * pMacDrvr )
{
    const DRV_ETHPHY_OBJECT_BASE * pPhyBase = pMacDrvr->config.pPhyBase;

    if( 0 > pPhyBase->DRV_ETHPHY_LinkStatusGet( pMacDrvr->hPhyClient,
                                                DRV_ETHPHY_INF_IDX_ALL_EXTERNAL,
                                                &pMacDrvr->linkInfo.negResult.linkStatus,
                                                false
                                                )
    )
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

    if( DRV_ETHPHY_CLIENT_STATUS_BUSY != pPhyBase->DRV_ETHPHY_ClientStatus( pMacDrvr->hPhyClient ) )
    {
        if( DRV_ETHPHY_RES_OK != pPhyBase->DRV_ETHPHY_ClientOperationResult( pMacDrvr->hPhyClient ) )
        {   // some error occurred
            macDrvrLinkStateDown( pMacDrvr );
        }
        else if( !(pMacDrvr->linkInfo.negResult.linkStatus & DRV_ETHPHY_LINK_ST_UP) )
        {   // down
            macDrvrLinkStateDown( pMacDrvr );
        }
        else if( pMacDrvr->macFlags._linkPrev )
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

    if( !pMacDrvr->macFlags._linkUpDone )
    {   // wait the init tmo before checking the link up
        uint32_t currTick = SYS_TMR_TickCountGet();
        if( (currTick - pMacDrvr->linkInfo.upTick) < pMacDrvr->linkInfo.waitTick )
        {
            return;
        }
        pMacDrvr->macFlags._linkUpDone = 1;
    }
    // initialization delay passed
    if( !pMacDrvr->macFlags._linkNegotiation )
    {   // no auto negotiation taking place; this is the link status
        pMacDrvr->macFlags._linkPrev = 1;
        pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_START_LINK;
    }
    else if( 0 > pPhyBase->DRV_ETHPHY_NegotiationIsComplete(    pMacDrvr->hPhyClient,
                                                                DRV_ETHPHY_INF_IDX_ALL_EXTERNAL,
                                                                false
                                                                )
    )
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

    if( DRV_ETHPHY_CLIENT_STATUS_BUSY != pPhyBase->DRV_ETHPHY_ClientStatus( pMacDrvr->hPhyClient ) )
    {
        if( DRV_ETHPHY_RES_OK != pPhyBase->DRV_ETHPHY_ClientOperationResult( pMacDrvr->hPhyClient ) )
        {   // some error occurred
            macDrvrLinkStateDown( pMacDrvr );
        }
        else if( 0 > pPhyBase->DRV_ETHPHY_NegotiationResultGet( pMacDrvr->hPhyClient,
                                                                DRV_ETHPHY_INF_IDX_ALL_EXTERNAL,
                                                                &pMacDrvr->linkInfo.negResult
                                                                )
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
    DRV_ETHPHY_CONFIG_FLAGS         phyCfgFlags = 0;

    if( DRV_ETHPHY_CLIENT_STATUS_BUSY != pPhyBase->DRV_ETHPHY_ClientStatus( pMacDrvr->hPhyClient ) )
    {
        if( DRV_ETHPHY_RES_OK != pPhyBase->DRV_ETHPHY_ClientOperationResult( pMacDrvr->hPhyClient ) )
        {   // some error occurred
            macDrvrLinkStateDown( pMacDrvr );
        }
        else if( (pMacDrvr->linkInfo.negResult.linkStatus & DRV_ETHPHY_LINK_ST_UP) )
        {   // negotiation succeeded; properly update the MAC
            pPhyBase->DRV_ETHPHY_HWConfigFlagsGet( pMacDrvr->hPhyClient, &phyCfgFlags );
            pMacDrvr->linkInfo.negResult.linkFlags |= (phyCfgFlags & DRV_ETHPHY_CFG_RMII) ? TCPIP_ETH_OPEN_RMII : TCPIP_ETH_OPEN_MII;
            macDrvrLibMacOpen(  pMacDrvr,
                                pMacDrvr->linkInfo.negResult.linkFlags,
                                pMacDrvr->linkInfo.negResult.pauseType
                                );
            pMacDrvr->macFlags._linkPrev = 1;
            pMacDrvr->linkInfo.checkState = MAC_DRVR_LINK_CHECK_START_LINK;
        }
        else
        {
            macDrvrLinkStateDown( pMacDrvr );
        }
    }
}
