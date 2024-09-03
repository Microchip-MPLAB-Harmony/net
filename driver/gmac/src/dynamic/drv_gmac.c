/*******************************************************************************
  MAC Module (Microchip PIC32C) for Microchip TCP/IP Stack
*******************************************************************************/

/*
Copyright (C) 2017-2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#include "driver/gmac/src/dynamic/drv_gmac_lib.h"
/** D E F I N I T I O N S ****************************************************/
//PIC32C internal GMAC peripheral
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MAC_PIC32C
// Active GMAC Instances
#define DRV_GMAC_INSTANCES    TCPIP_STACK_INTMAC_COUNT
// All pause capabilities our MAC supports
#define DRV_GMAC_PAUSE_CPBL_MASK     (TCPIP_ETH_PAUSE_TYPE_ALL)
/******************************************************************************
 * Prototypes
 ******************************************************************************/
//PHY related Init for GMAC
static SYS_MODULE_OBJ _DRV_GMAC_PHYInitialise(DRV_GMAC_DRIVER *pMACDrv);

#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

static void     _MacRxFreePacket( DRV_GMAC_DRIVER * pMACDrv);
static void     _MacTxFreeQueue( DRV_GMAC_DRIVER * pMACDrv);
static void     _MACCleanup(DRV_GMAC_DRIVER * pMACDrv );
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

static void     _MACDeinit(DRV_GMAC_DRIVER * pMACDrv );

static void             _MACTxAcknowledgeEth(DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx);
static TCPIP_MAC_RES    _MacTxPendingPackets(DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx );
static void             _MacTxDiscardQueues(DRV_GMAC_DRIVER * pMACDrv, TCPIP_MAC_PKT_ACK_RES ackRes);

static TCPIP_MAC_RES    DRV_GMAC_EventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam);
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static TCPIP_MAC_RES    DRV_GMAC_EventDeInit(DRV_HANDLE hMac);
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

typedef void (*_DRV_GMAC_LinkStateF)(DRV_GMAC_DRIVER * pMACDrv);

//Ethernet Link Start up states
static void _DRV_GMAC_LinkStateStartLink(DRV_GMAC_DRIVER * pMACDrv);
static void _DRV_GMAC_LinkStateGetLink(DRV_GMAC_DRIVER * pMACDrv);
static void _DRV_GMAC_LinkStateWaitLinkUp(DRV_GMAC_DRIVER * pMACDrv);
static void _DRV_GMAC_LinkStateNegComplete(DRV_GMAC_DRIVER * pMACDrv);
static void _DRV_GMAC_LinkStateNegResult(DRV_GMAC_DRIVER * pMACDrv);

//Transform TCPIP MAC filters to GMAC filters
static GMAC_RX_FILTERS _DRV_GMAC_MacToEthFilter(TCPIP_MAC_RX_FILTER_TYPE macFilter);
   
/******************************************************************************
 * PIC32C GMAC object implementation
 ******************************************************************************/
// the embedded PIC32 MAC object
#if defined(DRV_GMAC) || defined(DRV_ETH)
    const TCPIP_MAC_OBJECT DRV_GMAC_Object =  
    {
        .macId                                  = TCPIP_MODULE_MAC_PIC32C_0,
        .macType                                = TCPIP_MAC_TYPE_ETH, 
        .macName                                = "GMAC",  
        .TCPIP_MAC_Initialize                   = DRV_GMAC_Initialize,
    #if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
        .TCPIP_MAC_Deinitialize                 = DRV_GMAC_Deinitialize,
        .TCPIP_MAC_Reinitialize                 = DRV_GMAC_Reinitialize, 
    #else
        .TCPIP_MAC_Deinitialize                 = 0,
        .TCPIP_MAC_Reinitialize                 = 0,
    #endif  // (TCPIP_STACK_DOWN_OPERATION != 0) 
        .TCPIP_MAC_Status                       = DRV_GMAC_Status,
        .TCPIP_MAC_Tasks                        = DRV_GMAC_Tasks,
        .TCPIP_MAC_Open                         = DRV_GMAC_Open,
        .TCPIP_MAC_Close                        = DRV_GMAC_Close,
        .TCPIP_MAC_LinkCheck                    = DRV_GMAC_LinkCheck,
        .TCPIP_MAC_RxFilterHashTableEntrySet    = DRV_GMAC_RxFilterHashTableEntrySet,
        .TCPIP_MAC_PowerMode                    = DRV_GMAC_PowerMode,
        .TCPIP_MAC_PacketTx                     = DRV_GMAC_PacketTx,
        .TCPIP_MAC_PacketRx                     = DRV_GMAC_PacketRx,
        .TCPIP_MAC_Process                      = DRV_GMAC_Process,
        .TCPIP_MAC_StatisticsGet                = DRV_GMAC_StatisticsGet,
        .TCPIP_MAC_ParametersGet                = DRV_GMAC_ParametersGet,
        .TCPIP_MAC_RegisterStatisticsGet        = DRV_GMAC_RegisterStatisticsGet,
        .TCPIP_MAC_ConfigGet                    = DRV_GMAC_ConfigGet,
        .TCPIP_MAC_EventMaskSet                 = DRV_GMAC_EventMaskSet,
        .TCPIP_MAC_EventAcknowledge             = DRV_GMAC_EventAcknowledge,
        .TCPIP_MAC_EventPendingGet              = DRV_GMAC_EventPendingGet,
    };
    
#if defined(DRV_GMAC)
    bool interruptStat_mac0[DRV_GMAC_NUMBER_OF_QUEUES];
#elif defined(DRV_ETH)
    bool interruptStat_mac0[DRV_ETH_NUMBER_OF_QUEUES];    
#endif    
#endif
    
#ifdef DRV_GMAC0
    const TCPIP_MAC_OBJECT DRV_GMAC0_Object =  
    {
        .macId                                  = TCPIP_MODULE_MAC_PIC32C_0,
        .macType                                = TCPIP_MAC_TYPE_ETH, 
        .macName                                = "GMAC0",  
        .TCPIP_MAC_Initialize                   = DRV_GMAC_Initialize,
    #if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
        .TCPIP_MAC_Deinitialize                 = DRV_GMAC_Deinitialize,
        .TCPIP_MAC_Reinitialize                 = DRV_GMAC_Reinitialize, 
    #else
        .TCPIP_MAC_Deinitialize                 = 0,
        .TCPIP_MAC_Reinitialize                 = 0,
    #endif  // (TCPIP_STACK_DOWN_OPERATION != 0) 
        .TCPIP_MAC_Status                       = DRV_GMAC_Status,
        .TCPIP_MAC_Tasks                        = DRV_GMAC_Tasks,
        .TCPIP_MAC_Open                         = DRV_GMAC_Open,
        .TCPIP_MAC_Close                        = DRV_GMAC_Close,
        .TCPIP_MAC_LinkCheck                    = DRV_GMAC_LinkCheck,
        .TCPIP_MAC_RxFilterHashTableEntrySet    = DRV_GMAC_RxFilterHashTableEntrySet,
        .TCPIP_MAC_PowerMode                    = DRV_GMAC_PowerMode,
        .TCPIP_MAC_PacketTx                     = DRV_GMAC_PacketTx,
        .TCPIP_MAC_PacketRx                     = DRV_GMAC_PacketRx,
        .TCPIP_MAC_Process                      = DRV_GMAC_Process,
        .TCPIP_MAC_StatisticsGet                = DRV_GMAC_StatisticsGet,
        .TCPIP_MAC_ParametersGet                = DRV_GMAC_ParametersGet,
        .TCPIP_MAC_RegisterStatisticsGet        = DRV_GMAC_RegisterStatisticsGet,
        .TCPIP_MAC_ConfigGet                    = DRV_GMAC_ConfigGet,
        .TCPIP_MAC_EventMaskSet                 = DRV_GMAC_EventMaskSet,
        .TCPIP_MAC_EventAcknowledge             = DRV_GMAC_EventAcknowledge,
        .TCPIP_MAC_EventPendingGet              = DRV_GMAC_EventPendingGet,
    };         

    bool interruptStat_mac0[DRV_GMAC0_NUMBER_OF_QUEUES];
#endif
    
#ifdef DRV_GMAC1           
    const TCPIP_MAC_OBJECT DRV_GMAC1_Object =  
    {
        .macId                                  = TCPIP_MODULE_MAC_PIC32C_1,
        .macType                                = TCPIP_MAC_TYPE_ETH,    
        .macName                                = "GMAC1",   
        .TCPIP_MAC_Initialize                   = DRV_GMAC_Initialize,
    #if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
        .TCPIP_MAC_Deinitialize                 = DRV_GMAC_Deinitialize,
        .TCPIP_MAC_Reinitialize                 = DRV_GMAC_Reinitialize, 
    #else
        .TCPIP_MAC_Deinitialize                 = 0,
        .TCPIP_MAC_Reinitialize                 = 0,
    #endif  // (TCPIP_STACK_DOWN_OPERATION != 0) 
        .TCPIP_MAC_Status                       = DRV_GMAC_Status,
        .TCPIP_MAC_Tasks                        = DRV_GMAC_Tasks,
        .TCPIP_MAC_Open                         = DRV_GMAC_Open,
        .TCPIP_MAC_Close                        = DRV_GMAC_Close,
        .TCPIP_MAC_LinkCheck                    = DRV_GMAC_LinkCheck,
        .TCPIP_MAC_RxFilterHashTableEntrySet    = DRV_GMAC_RxFilterHashTableEntrySet,
        .TCPIP_MAC_PowerMode                    = DRV_GMAC_PowerMode,
        .TCPIP_MAC_PacketTx                     = DRV_GMAC_PacketTx,
        .TCPIP_MAC_PacketRx                     = DRV_GMAC_PacketRx,
        .TCPIP_MAC_Process                      = DRV_GMAC_Process,
        .TCPIP_MAC_StatisticsGet                = DRV_GMAC_StatisticsGet,
        .TCPIP_MAC_ParametersGet                = DRV_GMAC_ParametersGet,
        .TCPIP_MAC_RegisterStatisticsGet        = DRV_GMAC_RegisterStatisticsGet,
        .TCPIP_MAC_ConfigGet                    = DRV_GMAC_ConfigGet,
        .TCPIP_MAC_EventMaskSet                 = DRV_GMAC_EventMaskSet,
        .TCPIP_MAC_EventAcknowledge             = DRV_GMAC_EventAcknowledge,
        .TCPIP_MAC_EventPendingGet              = DRV_GMAC_EventPendingGet,
    };

    bool interruptStat_mac1[DRV_GMAC1_NUMBER_OF_QUEUES];
#endif                      
// the embedded PIC32 MAC descriptor
// to support multiple instances
// create an array/list of MAC_DCPT structures
// or allocate dynamically
static DRV_GMAC_DRIVER _gmac_drv_dcpt[] = 
{
    #if defined(DRV_GMAC)  || defined(DRV_ETH)
    {
        &DRV_GMAC_Object,
        {0},
    },
    #elif defined(DRV_GMAC0)  
    {
        &DRV_GMAC0_Object,
        {0},
    },
    #endif 
    #if defined(DRV_GMAC1)   
    {
        &DRV_GMAC1_Object,
        {0},
    }   
    #endif 
};

//GMAC statistics register access 
static const DRV_PIC32CGMAC_HW_REG_DCPT macPIC32CHwRegDcpt[] =
{
    {"Tx Byte Count-lower 32bits(TxOTLO)",    DRV_PIC32CGMAC_LibGetTxOctetLow},
    {"Tx Byte Count-upper 16bits(TxOTHI)",    DRV_PIC32CGMAC_LibGetTxOctetHigh},
    {"Tx Frames w/o error(TxFTCNT)",   DRV_PIC32CGMAC_LibGetTxFrameCount},
    {"Tx Broadcast Frames(TxBCFT)",    DRV_PIC32CGMAC_LibGetTxBCastFrameCount},
    {"Tx Multicast Frames(TxMFT)",     DRV_PIC32CGMAC_LibGetTxMCastFrameCount},
    {"Tx Pause Frames(TxPFT)",     DRV_PIC32CGMAC_LibGetTxPauseFrameCount},
    {"Tx 64-byte Frames(TxBFT64)",   DRV_PIC32CGMAC_LibGetTx64ByteFrameCount},
    {"Tx 65-127 byte Frames(TBFT127)",   DRV_PIC32CGMAC_LibGetTx127ByteFrameCount},
    {"Tx 128-255 byte Frames(TBFT255)",   DRV_PIC32CGMAC_LibGetTx255ByteFrameCount},
    {"Tx 256-511 byte Frames(TBFT511)",   DRV_PIC32CGMAC_LibGetTx511ByteFrameCount},
    {"Tx 512-1023 byte Frames(TBFT1023)",  DRV_PIC32CGMAC_LibGetTx1023ByteFrameCount},
    {"Tx 1024-1518 byte Frames(TBFT1518)",  DRV_PIC32CGMAC_LibGetTx1518ByteFrameCount},
    {"Tx Frames >= 1518 byte(GTBF1518)",  DRV_PIC32CGMAC_LibGetTxGT1518ByteFrameCount},
    {"Tx Underrun Frames not TXed(TxTUR)",     DRV_PIC32CGMAC_LibGetTxUnderRunFrameCount},
    {"Tx Single Collision Frames(TxSCF)",     DRV_PIC32CGMAC_LibGetTxSingleCollFrameCount},
    {"Tx 2-15 Collision Frames(TxMCF)",     DRV_PIC32CGMAC_LibGetTxMultiCollFrameCount},    
    {"Tx 16 Collision Frames(TxECCNT)",   DRV_PIC32CGMAC_LibGetTxExcessCollFrameCount},
    {"Tx late Collision Frames(TxLCCNT)",   DRV_PIC32CGMAC_LibGetTxLateCollFrameCount},
    {"Tx Deferred Frames(TxDTF)",     DRV_PIC32CGMAC_LibGetTxDeferFrameCount},  
    {"Tx Carrier Sense error Frames(TxCSE)",     DRV_PIC32CGMAC_LibGetTxCSErrorFrameCount},     
    {"Rx Byte Count-lower 32bits(RxORLO)",    DRV_PIC32CGMAC_LibGetRxOctetLow},
    {"Rx Byte Count-upper 16bits(RxORHI)",    DRV_PIC32CGMAC_LibGetRxOctetHigh},
    {"Rx Frames w/o error(RxFRCNT)",    DRV_PIC32CGMAC_LibGetRxFrameCount},
    {"Rx Broadcast Frames(RxBCFR)",    DRV_PIC32CGMAC_LibGetRxBCastFrameCount},
    {"Rx Multicast Frames (RxMFR)",     DRV_PIC32CGMAC_LibGetRxMCastFrameCount},
    {"Rx Pause Frames(RxPFR)",     DRV_PIC32CGMAC_LibGetRxPauseFrameCount},
    {"Rx 64-byte Frames(RxBFR64)",   DRV_PIC32CGMAC_LibGetRx64ByteFrameCount},
    {"Rx 65-127 byte Frames(TBFR127)",   DRV_PIC32CGMAC_LibGetRx127ByteFrameCount},
    {"Rx 128-255 byte Frames(TBFR255)",   DRV_PIC32CGMAC_LibGetRx255ByteFrameCount},
    {"Rx 256-511 byte Frames(TBFR511)",   DRV_PIC32CGMAC_LibGetRx511ByteFrameCount},
    {"Rx 512-1023 byte Frames(TBFR1023)",  DRV_PIC32CGMAC_LibGetRx1023ByteFrameCount},
    {"Rx 1024-1518 byte Frames(TBFR1518)",  DRV_PIC32CGMAC_LibGetRx1518ByteFrameCount},
    {"Rx Frames >= 1519 bytes(TMXBFR)",    DRV_PIC32CGMAC_LibGetRxGT1518ByteFrameCount},
    {"Rx Frames < 64 bytes(RxUFR)",     DRV_PIC32CGMAC_LibGetRxUnderSizeFrameCount},
    {"Rx Frames > 1518 bytes(RxOFR)",     DRV_PIC32CGMAC_LibGetRxOverSizeFrameCount},
    {"Rx error Frames > 1518 bytes(RxJR)",      DRV_PIC32CGMAC_LibGetRxJabberFrameCount},
    {"Rx CRC error Frames(RxFCSE)",    DRV_PIC32CGMAC_LibGetRxFCSErrorFrameCount},
    {"Rx Length field error Frames(RxLFFE)",    DRV_PIC32CGMAC_LibGetRxLFErrorFrameCount},
    {"Rx symbol error Frames(RxRSE)",     DRV_PIC32CGMAC_LibGetRxSymErrorFrameCount},
    {"Rx alignment error Frames(RxAE)",      DRV_PIC32CGMAC_LibGetRxAlignErrorFrameCount},
    {"Rx Resource Error (RxRRE)",     DRV_PIC32CGMAC_LibGetRxResErrorFrameCount},       
    {"Rx overrun error Frames(RxROE)",     DRV_PIC32CGMAC_LibGetRxOverRunFrameCount},
    {"Rx IPHdr Chksum error Frames(RxIHCE)",    DRV_PIC32CGMAC_LibGetRxIPHdrCSErrorFrameCount}, 
    {"Rx TCP Chksum error Frames(RxTCE)",     DRV_PIC32CGMAC_LibGetRxTCPCSErrorFrameCount}, 
    {"Rx UDP Chksum error Frames(RxUCE)",     DRV_PIC32CGMAC_LibGetRxUDPCSErrorFrameCount},
};

//table of different Link startup
static const _DRV_GMAC_LinkStateF _DRV_GMAC_LinkStateTbl[] =
{
    _DRV_GMAC_LinkStateStartLink,       // DRV_GMAC_LINK_CHECK_START_LINK
    _DRV_GMAC_LinkStateGetLink,         // DRV_GMAC_LINK_CHECK_GET_LINK
    _DRV_GMAC_LinkStateWaitLinkUp,      // DRV_GMAC_LINK_CHECK_WAIT_LINK_UP
    _DRV_GMAC_LinkStateNegComplete,     // DRV_GMAC_LINK_CHECK_NEG_COMPLETE
    _DRV_GMAC_LinkStateNegResult,       // DRV_GMAC_LINK_CHECK_NEG_RESULT
};

//convert mac id to index
static __inline__ int __attribute__((always_inline)) _GmacIdToIndex(TCPIP_MODULE_MAC_ID macId)
{    
    int macCount = sizeof(_gmac_drv_dcpt)/sizeof(*_gmac_drv_dcpt);
    int macIx;
    
    if (macCount >= 1)
    {
        if (macCount == 1)
        {
            macIx = 0;
        }
        else
        {
            macIx = macId - TCPIP_MODULE_MAC_PIC32C_0;
        }        
    }        
    else 
    {
        macIx = -1; //error
    }

    return macIx;
}

#if (DRV_GMAC_INSTANCES == 1)
static __inline__ DRV_GMAC_DRIVER* __attribute__((always_inline)) _GmacHandleToMacInst(uintptr_t handle)
{
    DRV_GMAC_DRIVER* pMacD = (DRV_GMAC_DRIVER*)handle;
    return pMacD == _gmac_drv_dcpt ? pMacD : 0;
}
#else
// multiple instances version
static DRV_GMAC_DRIVER* _GmacHandleToMacInst(uintptr_t handle)
{
    DRV_GMAC_DRIVER* pMacD = (DRV_GMAC_DRIVER*)handle;
    int macIx = pMacD - _gmac_drv_dcpt;
    if(macIx >= 0 && macIx < sizeof(_gmac_drv_dcpt) / sizeof(*_gmac_drv_dcpt))
    {
        if(pMacD == _gmac_drv_dcpt + macIx)
        {
            return pMacD;
        }
    }

    return 0;
}
#endif  // (DRV_GMAC_INSTANCES == 1)


/*
 * interface functions
 *
*/

/****************************************************************************
 * Function:        DRV_GMAC_Initialize
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *                
 * Side Effects:    None
 *
 * Overview:        This function initializes the GMAC controller, the MAC and the associated PHY.
 *                  It should be called to be able to schedule any MAC transmit or receive operation.
 *
 * Note:            Only one client per MAC supported. 
 *****************************************************************************/
SYS_MODULE_OBJ DRV_GMAC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    DRV_GMAC_DRIVER * pMACDrv;  
    int             macIx;
    const TCPIP_MAC_MODULE_CTRL* const macControl = ((TCPIP_MAC_INIT*)init)->macControl;    
    const TCPIP_MODULE_MAC_PIC32C_CONFIG* initData = (const TCPIP_MODULE_MAC_PIC32C_CONFIG*)((TCPIP_MAC_INIT*)init)->moduleData;
    TCPIP_MAC_RES       initRes;    
    GMAC_QUE_LIST       queueIdx;   
    GMAC_RX_FILTERS     gmacRxFilt;
    
    macIx = _GmacIdToIndex(index);
    
    if(macIx < 0 )
    {   
        return SYS_MODULE_OBJ_INVALID;      // no such type supported
    }
   
    pMACDrv = _gmac_drv_dcpt + macIx;

    if(pMACDrv->sGmacData._macFlags._init != 0)
    {   // already initialized  
        return (SYS_MODULE_OBJ)pMACDrv;
    }
            
    if(pMACDrv->sGmacData._macFlags._open != 0)
    {
        return SYS_MODULE_OBJ_INVALID;     // have a client connected
    }

    if(macControl->memH == 0)
    {
        return SYS_MODULE_OBJ_INVALID;     // not possible without dynamic memory!
    }

    if(initData == 0)
    {
        return SYS_MODULE_OBJ_INVALID;     // not possible without init data!
    }

    if(initData->pPhyBase == 0 || initData->pPhyInit == 0)
    {
        return SYS_MODULE_OBJ_INVALID;     // not possible without a PHY!
    }
    
    // init the MAC object
    memset(&pMACDrv->sGmacData, 0x0, sizeof(pMACDrv->sGmacData)); 
    pMACDrv->sGmacData.hPhyClient = DRV_HANDLE_INVALID;
    pMACDrv->sGmacData.hPhySysObject = SYS_MODULE_OBJ_INVALID;
    pMACDrv->sGmacData.sysStat = SYS_STATUS_UNINITIALIZED;
    
    pMACDrv->sGmacData._macIx = macIx;
    pMACDrv->sGmacData._phyIx = macIx;    // use the same index for the associated PHY
    pMACDrv->sGmacData._macFlags._linkPrev = 0; 

    // use initialization data
    pMACDrv->sGmacData._AllocH = macControl->memH;
    pMACDrv->sGmacData._callocF = macControl->callocF;
    pMACDrv->sGmacData._freeF = macControl->freeF;
    pMACDrv->sGmacData._dcptOffset = macControl->gapDcptOffset;


    pMACDrv->sGmacData.pktAllocF = macControl->pktAllocF;
    pMACDrv->sGmacData.pktFreeF = macControl->pktFreeF;
    pMACDrv->sGmacData.pktAckF = macControl->pktAckF;

    pMACDrv->sGmacData._synchF = macControl->synchF;
    
    pMACDrv->sGmacData._controlFlags = macControl->controlFlags;
    pMACDrv->sGmacData._dataOffset = (macControl->controlFlags & TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2) == 0 ? 0 : 2;
    pMACDrv->sGmacData._dataOffsetMask = (macControl->controlFlags & TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2) != 0 ? 0xfffffffc : 0xffffffff;
    pMACDrv->sGmacData._txDiscard = false;
    // copy the configuration data
    pMACDrv->sGmacData.gmacConfig = *initData;  
    
    DRV_GMAC_LibDescriptorsPoolAdd (pMACDrv, DRV_GMAC_DCPT_TYPE_RX);
    DRV_GMAC_LibDescriptorsPoolAdd (pMACDrv, DRV_GMAC_DCPT_TYPE_TX);
    
    for(queueIdx = GMAC_QUE_0; queueIdx < pMACDrv->sGmacData.gmacConfig.macQueNum; queueIdx++)
    {
        if(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt < DRV_GMAC_MIN_TX_DESCRIPTORS)
        {
            return SYS_MODULE_OBJ_INVALID; //return invalid than setting min dscp count
        }
        
        uint16_t rxBufferSize = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize;

        if(rxBufferSize < DRV_GMAC_MIN_RX_SIZE)
        {
            return SYS_MODULE_OBJ_INVALID; //return invalid than setting min Rx buffer size
        }

        if(rxBufferSize > DRV_GMAC_RX_MAX_FRAME)
        {   // no need to exceed the max allowed RX frame
            rxBufferSize = DRV_GMAC_RX_MAX_FRAME;
        }
        // GMAC RX buffer size needs to be multiple of 64
        rxBufferSize = ((rxBufferSize + 63) / 64) * 64;
        pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize = rxBufferSize;
    }
    
    if(!DRV_PIC32CGMAC_LibSetPriorityToQueueNum(pMACDrv))
    {
        return SYS_MODULE_OBJ_INVALID; //return invalid
    }
        
    //set the MAC address received from system config
    memcpy((pMACDrv->sGmacData.gmacConfig.macAddress.v), (macControl->ifPhyAddress.v),sizeof(macControl->ifPhyAddress));    

    
    if(!_DRV_GMAC_RxCreate(pMACDrv))
    {
        return SYS_MODULE_OBJ_INVALID;     // failed to create synch lock
    }
    if(!_DRV_GMAC_TxCreate(pMACDrv))
    {
        _DRV_GMAC_RxDelete(pMACDrv);
        return SYS_MODULE_OBJ_INVALID;     // failed to create synch lock
    }
    
    initRes = TCPIP_MAC_RES_OK;
   
    while(1)
    {
        // start the initialization sequence
        DRV_PIC32CGMAC_LibSysInt_Disable(pMACDrv, GMAC_ALL_QUE_MASK, NULL);
            
        initRes = _DRV_GMAC_PHYInitialise(pMACDrv);
        if(initRes != TCPIP_MAC_RES_OK)
        {
            // some error occurred
            initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
            break;
        }
        
        //global configurations for gmac
        DRV_PIC32CGMAC_LibInit(pMACDrv);
                        
        //Receive All Multi-cast packets? then set 64-bit hash value to all ones.
        if((pMACDrv->sGmacData.gmacConfig.macRxFilt) & TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT)
        {
            DRV_GMAC_HASH hash; 
            
            hash.hash_value = -1; //Set 64-bit Hash value to all 1s, to receive all multi-cast
            hash.calculate_hash = false; // No hash calculation; directly set hash register
            
            DRV_PIC32CGMAC_LibRxFilterHash_Calculate(pMACDrv, &hash);
        }
        // Set Rx Filters 
        gmacRxFilt = _DRV_GMAC_MacToEthFilter(pMACDrv->sGmacData.gmacConfig.macRxFilt);
        DRV_PIC32CGMAC_LibSetRxFilter(pMACDrv, gmacRxFilt);

        // Initialize Rx Queue Filters
        if(DRV_PIC32CGMAC_LibRxQueFilterInit(pMACDrv) != DRV_PIC32CGMAC_RES_OK)
        {           
            initRes = TCPIP_MAC_RES_INIT_FAIL;
            break;      
        }
        
        if(DRV_PIC32CGMAC_LibRxInit(pMACDrv) != DRV_PIC32CGMAC_RES_OK)
        {           
            initRes = TCPIP_MAC_RES_INIT_FAIL;
            break;      
        }
        
        if(DRV_PIC32CGMAC_LibTxInit(pMACDrv) != DRV_PIC32CGMAC_RES_OK)
        {
            initRes = TCPIP_MAC_RES_INIT_FAIL;
            break;
        }
        
        for(queueIdx = GMAC_QUE_0; queueIdx < pMACDrv->sGmacData.gmacConfig.macQueNum; queueIdx++)
        {
            //Initialize QUEUES             
            if(DRV_PIC32CGMAC_LibInitTransfer(pMACDrv,queueIdx) != DRV_PIC32CGMAC_RES_OK)
            {
                initRes = TCPIP_MAC_RES_INIT_FAIL;
                break;
            }
        }
        
        DRV_PIC32CGMAC_LibSysIntStatus_Clear(pMACDrv, GMAC_ALL_QUE_MASK);
        DRV_PIC32CGMAC_LibSysInt_Enable(pMACDrv, GMAC_ALL_QUE_MASK);
        
        
        DRV_PIC32CGMAC_LibTransferEnable(pMACDrv); //enable Transmit and Receive of GMAC
        
        if(DRV_GMAC_EventInit((DRV_HANDLE)pMACDrv, macControl->eventF, macControl->eventParam) != TCPIP_MAC_RES_OK)
        {
            initRes = TCPIP_MAC_RES_EVENT_INIT_FAIL;
            break;
        }
        // end of initialization
        break;
        
    }
    
    if(initRes != TCPIP_MAC_RES_OK)
    {   // failed; release what we've allocated
        _MACDeinit(pMACDrv);
        return SYS_MODULE_OBJ_INVALID;
    }
    
    // success
    // remaining initialization is done by DRV_ETHMAC_PIC32MACTasks     
    pMACDrv->sGmacData._macFlags._init = 1; 
    pMACDrv->sGmacData.sysStat = SYS_STATUS_BUSY;
    
    //initialize unimplemented Receive status to -1
    pMACDrv->sGmacData._rxStat.nRxErrorPackets = 0;
    pMACDrv->sGmacData._rxStat.nRxFragmentErrors = -1;
    pMACDrv->sGmacData._rxStat.nRxPendBuffers = 0;
    pMACDrv->sGmacData._rxStat.nRxSchedBuffers = 0;
    
    return (SYS_MODULE_OBJ)pMACDrv;
    
}

/****************************************************************************
 * Function:        DRV_GMAC_Deinitialize
 *
 * PreCondition:    None
 *
 * Input:           stackData - standard stack initialization structure
 *
 * Output:          TCPIP_MAC_RES
 *                
 * Side Effects:    None
 *
 * Overview:        This function de-initializes the Eth MAC controller.
 *
 * Note:            None 
 *****************************************************************************/
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
void DRV_GMAC_Deinitialize(SYS_MODULE_OBJ object)
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(object);
    if(pMACDrv != 0)
    {
        if(pMACDrv->sGmacData._macFlags._init != 0)
        {
            _MACDeinit(pMACDrv);
        }
    }
}

void DRV_GMAC_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init)
{
    // not supported
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

SYS_STATUS DRV_GMAC_Status (SYS_MODULE_OBJ object)
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(object);
    if(pMACDrv != 0)
    {
        if(pMACDrv->sGmacData._macFlags._init != 0)
        {
            return pMACDrv->sGmacData.sysStat;
        }
    }
    return SYS_STATUS_ERROR;
}

void DRV_GMAC_Tasks(SYS_MODULE_OBJ object)
{
    TCPIP_ETH_PAUSE_TYPE        pauseType;
    DRV_ETHPHY_CLIENT_STATUS    phyStat;
    DRV_HANDLE                  hPhyClient;
    DRV_ETHPHY_RESULT           phyInitRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;

    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(object);
    if(pMACDrv == 0 || (pMACDrv->sGmacData._macFlags._init == 0))
    {   // nothing to do
        return;
    }
    
    pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase;
    
    pPhyBase->DRV_ETHPHY_Tasks(pMACDrv->sGmacData.hPhySysObject);
    
    switch(pMACDrv->sGmacData.sysStat)
    {
        case SYS_STATUS_BUSY:
            // perform the PHY initialization

            hPhyClient = pMACDrv->sGmacData.hPhyClient;
            phyStat = pPhyBase->DRV_ETHPHY_ClientStatus(hPhyClient);
            if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
            {   // wait some more
                return;
            }

            phyInitRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(hPhyClient);

            if ( phyInitRes != DRV_ETHPHY_RES_OK)
            {
                _MACDeinit(pMACDrv);
                pMACDrv->sGmacData.sysStat = SYS_STATUS_ERROR;
                SYS_ERROR_PRINT(SYS_ERROR_ERROR, "DRV PHY init failed: %d\r\n", phyInitRes);
                break;
            }

            // PHY was detected properly
            pMACDrv->sGmacData._macFlags._linkPresent = 1;
            if((pMACDrv->sGmacData.gmacConfig.ethFlags & TCPIP_ETH_OPEN_AUTO) != 0)
            {   // we'll just wait for the negotiation to be done
                pMACDrv->sGmacData._macFlags._linkNegotiation = 1;  // performing the negotiation
            }
            else
            {   // no need of negotiation results; just update the MAC
                pauseType = (pMACDrv->sGmacData.gmacConfig.ethFlags & TCPIP_ETH_OPEN_FDUPLEX) ? DRV_GMAC_PAUSE_CPBL_MASK : TCPIP_ETH_PAUSE_TYPE_NONE;
                DRV_PIC32CGMAC_LibMACOpen(pMACDrv, pMACDrv->sGmacData._linkResFlags, pauseType);
            }

            pMACDrv->sGmacData._linkUpTick = SYS_TMR_TickCountGet();     // the initialization time
            pMACDrv->sGmacData._linkWaitTick = ((SYS_TMR_TickCounterFrequencyGet() * pMACDrv->sGmacData.gmacConfig.linkInitDelay) + 999 ) / 1000;
            pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_START_LINK;

            pMACDrv->sGmacData.sysStat = SYS_STATUS_READY;
            break;

        default:    // SYS_STATUS_READY
            if(((pMACDrv->sGmacData._controlFlags & TCPIP_MAC_CONTROL_NO_LINK_CHECK) == 0 && pMACDrv->sGmacData._macFlags._linkPrev == false) ||
                    (pMACDrv->sGmacData._txDiscard == true))
            {                   
                // discard the TX queues
                _MacTxDiscardQueues(pMACDrv, TCPIP_MAC_PKT_ACK_LINK_DOWN);                 
                pMACDrv->sGmacData._txDiscard = false;
            }

            break;
    }

}


size_t DRV_GMAC_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize) 
{   
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    if(pMACDrv != 0)
    {
        if(pConfigSize)
        {
            *pConfigSize =  sizeof(TCPIP_MODULE_MAC_PIC32C_CONFIG);
        }

        if(configBuff && buffSize >= sizeof(TCPIP_MODULE_MAC_PIC32C_CONFIG))
        {   // can copy the data
            TCPIP_MODULE_MAC_PIC32C_CONFIG* pMacConfig = (TCPIP_MODULE_MAC_PIC32C_CONFIG*)configBuff;

            *pMacConfig = pMACDrv->sGmacData.gmacConfig;
            return sizeof(TCPIP_MODULE_MAC_PIC32C_CONFIG);
        }
    }
    return 0;
}

DRV_HANDLE DRV_GMAC_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent) 
{
    int                 macIx;
    DRV_GMAC_DRIVER * pMACDrv;
    DRV_HANDLE    hMac = DRV_HANDLE_INVALID;
    
    macIx = _GmacIdToIndex(drvIndex);
    
    if(macIx >= 0 )
    {
        pMACDrv = _gmac_drv_dcpt + macIx;
        if(pMACDrv->sGmacData._macFlags._init == 1)
        {
            if(pMACDrv->sGmacData._macFlags._open == 0)
            {   // only one client for now
                pMACDrv->sGmacData._macFlags._open = 1;
                hMac = (DRV_HANDLE)pMACDrv;
            }

        }
    }

    return hMac;
}


void DRV_GMAC_Close( DRV_HANDLE hMac ) 
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    if(pMACDrv != 0)
    {
        if(pMACDrv->sGmacData._macFlags._init == 1) 
        {
            pMACDrv->sGmacData._macFlags._open = 0;
        }
    }
}

/**************************
 * TX functions
 ***********************************************/

TCPIP_MAC_RES DRV_GMAC_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket)  
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    if(pMACDrv == 0)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    TCPIP_MAC_DATA_SEGMENT* pSeg;
    GMAC_QUE_LIST queueIdx = GMAC_QUE_0;
    
    // Check if priority is between 0 and (Number of Queues -1 )
    if ((ptrPacket->pktPriority) >= pMACDrv->sGmacData.gmacConfig.macQueNum)
    {    
        return TCPIP_MAC_RES_PACKET_ERR;            
    }
    
    queueIdx = (GMAC_QUE_LIST)(pMACDrv->sGmacData.gmacConfig.txPrioNumToQueIndx[ptrPacket->pktPriority]);
    
    if (queueIdx == DRV_GMAC_DUMMY_PRIORITY)
    {
        // fallback to default queue priority
        queueIdx = GMAC_QUE_0 ;
    }
    else if(queueIdx >= pMACDrv->sGmacData.gmacConfig.macQueNum)
    {               
        return TCPIP_MAC_RES_PACKET_ERR; 
    }
    
    //new packet for transmission    
    while(ptrPacket)
    {
        pSeg = ptrPacket->pDSeg;
        // check that packets are properly formatted and
        // have room to store the packet info
        if(pSeg == 0)
        {   // cannot send this packet      
            return TCPIP_MAC_RES_PACKET_ERR;
        }
        else
        {
            _DRV_GMAC_TxLock(pMACDrv); 
            DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxQueue, (DRV_PIC32CGMAC_SGL_LIST_NODE*)ptrPacket);
            _DRV_GMAC_TxUnlock(pMACDrv);
            ptrPacket->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
            ptrPacket = ptrPacket->next;
        }
        
    }       
    _MacTxPendingPackets(pMACDrv,queueIdx);
    if(DRV_PIC32CGMAC_LibIsTxComplete(pMACDrv))
    {
        _MACTxAcknowledgeEth(pMACDrv,queueIdx);
    }
        
    return TCPIP_MAC_RES_OK;
}



/***********************************************
 * RX functions
 ***********************************************/

// returns a pending RX packet if exists
TCPIP_MAC_PACKET* DRV_GMAC_PacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat)  
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    if(pMACDrv == 0)
    {
        return 0;
    }

    TCPIP_MAC_RES           mRes;   
    TCPIP_MAC_PACKET        *pRxPkt = NULL;
    TCPIP_MAC_PACKET_RX_STAT_PIC32C pRxPktStat = {0};
    DRV_PIC32CGMAC_RESULT   ethRes = DRV_PIC32CGMAC_RES_NO_PACKET;  
    int                     buffsPerRxPkt = 0;
    static GMAC_QUE_LIST    queueIndex = DRV_GMAC_NO_ACTIVE_QUEUE;    

    _DRV_GMAC_RxLock(pMACDrv); 
    //get highest priority active queue index
    queueIndex = DRV_PIC32CGMAC_LibGetHighPrioReadyQue(pMACDrv);      
    _DRV_GMAC_RxUnlock(pMACDrv);
    
    //if any any active queue?
    while(queueIndex != DRV_GMAC_NO_ACTIVE_QUEUE)
    { 
        //get Rx packet from Queue
        ethRes = DRV_PIC32CGMAC_LibRxGetPacket (pMACDrv, &pRxPkt, &buffsPerRxPkt, &pRxPktStat, queueIndex);
                
        //if valid rx packet?
        if(ethRes == DRV_PIC32CGMAC_RES_OK)
        {
            //proceed to packet processing
            break;
        }
        else 
        {
            _DRV_GMAC_RxLock(pMACDrv); 
            //clear que event status     
            DRV_PIC32CGMAC_LibClearPriorityQue(pMACDrv,queueIndex);
            //get highest priority active queue index
            queueIndex = DRV_PIC32CGMAC_LibGetHighPrioReadyQue(pMACDrv);
            _DRV_GMAC_RxUnlock(pMACDrv);
        }
    }
    
    if(buffsPerRxPkt > 1)
    {
        pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_SPLIT;
    } 
    if(ethRes == DRV_PIC32CGMAC_RES_PACKET_QUEUED || ethRes == DRV_PIC32CGMAC_RES_NO_PACKET)
    {   // done, no more packets
        mRes = TCPIP_MAC_RES_PENDING;
    }
    else if(ethRes == DRV_PIC32CGMAC_RES_OK)
    {   
        mRes = TCPIP_MAC_RES_OK;
        pMACDrv->sGmacData._rxStat.nRxOkPackets++;      
    }
    else
    {   
        mRes = TCPIP_MAC_RES_PACKET_ERR;
        pMACDrv->sGmacData._rxStat.nRxErrorPackets++;
    }

    if(pRes)
    {
        *pRes = mRes;
    }
    
    if(mRes == TCPIP_MAC_RES_OK)
    {   // valid Ethernet packet;
        // format properly and return the packet pointer
        // restore packet the buffer belongs to
        TCPIP_MAC_DATA_SEGMENT  *pDSeg;
        
        pDSeg = pRxPkt->pDSeg;
        // segLoad for the 1st segment
        // first segment is always offset by Rx Data Offset
        pDSeg->segLoad = pDSeg->segBuffer + pMACDrv->sGmacData._dataOffset;
        
        // segLen for the 1st packet        
        // Ethernet MAC header(14 bytes) stored in data packet (or in first data buffer for multi-buffer data).
        // reduce MAC header length to get data segment length
        pDSeg->segLen = pDSeg->segLen - sizeof(TCPIP_MAC_ETHERNET_HEADER);
        
        //multiple data segments?
        if(pDSeg->next)
        {
            //reduce the segLen by Rx Data Offset, of first data segment 
            pDSeg->segLen = pDSeg->segLen - pMACDrv->sGmacData._dataOffset;
            
            // go through all the data segments; from 2nd data segment to last            
            while (pDSeg->next)
            {
                // move to next data segment
                pDSeg = pDSeg->next; 
                // segLoad for intermediate segments
                pDSeg->segLoad = pDSeg->segBuffer;                
                //segLen for intermediate segments will remain same
            }
            
            // segLen for the last data segment 
            // increase segLen by Rx Data Offset
            pDSeg->segLen = pDSeg->segLen + pMACDrv->sGmacData._dataOffset;
            
        }
        
        // Note: re-set pMacLayer and pNetLayer; IPv6 changes these pointers inside the packet!
        pRxPkt->pMacLayer = pRxPkt->pDSeg->segLoad;
        pRxPkt->pNetLayer = pRxPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);

        pRxPkt->tStamp = SYS_TMR_TickCountGet();
        pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;


        pRxPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_CAST_MASK;
        if(pRxPktStat.bm.bBroadcastDetected)
        {
            pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_BCAST;
        }
        else if(pRxPktStat.bm.bMultiHashMatch)
        {
            pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_MCAST;
        }
        else if(pRxPktStat.bm.bUniHashMatch)
        {
            pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_UNICAST;
        }

        if(pPktStat)
        {
            pPktStat->rxStatPIC32C.val = pRxPktStat.val;
        }


        pRxPkt->pktPriority = DRV_PIC32CGMAC_LibGetPriorityFromQueueNum(pMACDrv, queueIndex);
        // success
        return pRxPkt;
    }
    
    return 0;

}


// verify the link status

bool DRV_GMAC_LinkCheck(DRV_HANDLE hMac) 
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    if(pMACDrv == 0 || (pMACDrv->sGmacData._macFlags._linkPresent == 0))
    {
        return false;
    }

    const DRV_ETHPHY_OBJECT_BASE* pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase;
    pPhyBase->DRV_ETHPHY_Tasks(pMACDrv->sGmacData.hPhySysObject);


    (*_DRV_GMAC_LinkStateTbl[pMACDrv->sGmacData._linkCheckState])(pMACDrv);

    return pMACDrv->sGmacData._macFlags._linkPrev == 1; 

}

static void _DRV_GMAC_LinkStateDown(DRV_GMAC_DRIVER * pMACDrv) 
{
    // link is down; reset state flags, initialization time
    pMACDrv->sGmacData._macFlags._linkPrev = 0;
    pMACDrv->sGmacData._macFlags._linkUpDone = 0;
    pMACDrv->sGmacData._linkUpTick = SYS_TMR_TickCountGet();
    pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_START_LINK;
}

static void _DRV_GMAC_LinkStateStartLink(DRV_GMAC_DRIVER * pMACDrv) 
{
    DRV_ETHPHY_RESULT phyRes;    
   
    phyRes = pMACDrv->sGmacData.gmacConfig.pPhyBase->DRV_ETHPHY_LinkStatusGet(pMACDrv->sGmacData.hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &pMACDrv->sGmacData._negResult.linkStatus, false);
    if(phyRes < 0)
    {   // some error occurred
        _DRV_GMAC_LinkStateDown(pMACDrv);
        return;
    }

    pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_GET_LINK;
}

static void _DRV_GMAC_LinkStateGetLink(DRV_GMAC_DRIVER * pMACDrv) 
{
    DRV_ETHPHY_CLIENT_STATUS phyStat;
    DRV_ETHPHY_RESULT phyRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;
    bool linkCurrUp;

    
    pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase;
    phyStat = pPhyBase->DRV_ETHPHY_ClientStatus( pMACDrv->sGmacData.hPhyClient );
    if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
    {   // not done yet
        return;
    }

    phyRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(pMACDrv->sGmacData.hPhyClient);
    if(phyRes != DRV_ETHPHY_RES_OK)
    {   // some error occurred
        _DRV_GMAC_LinkStateDown(pMACDrv);
        return;
    }

    linkCurrUp = (pMACDrv->sGmacData._negResult.linkStatus & DRV_ETHPHY_LINK_ST_UP) != 0;

    if(linkCurrUp == false)
    {
        _DRV_GMAC_LinkStateDown(pMACDrv);
        return;
    }

    // link up
    if( (bool) pMACDrv->sGmacData._macFlags._linkPrev == linkCurrUp )
    {   // PHY state not changed
        pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_START_LINK;
    }
    else
    {
        pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_WAIT_LINK_UP;
    }
    
}

// ok, the link is coming up
static void _DRV_GMAC_LinkStateWaitLinkUp(DRV_GMAC_DRIVER * pMACDrv) 
{
    DRV_ETHPHY_RESULT   phyRes;

    if(pMACDrv->sGmacData._macFlags._linkUpDone == 0)
    {   // wait the init tmo before checking the link up
        uint32_t currTick = SYS_TMR_TickCountGet();
        if(currTick - pMACDrv->sGmacData._linkUpTick < pMACDrv->sGmacData._linkWaitTick)
        {
            return;
        }
        pMACDrv->sGmacData._macFlags._linkUpDone = 1;
    }

    // initialization delay passed
    if(pMACDrv->sGmacData._macFlags._linkNegotiation == 0)
    {   // no auto negotiation taking place; this is the link status
        pMACDrv->sGmacData._macFlags._linkPrev = true;
        pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_START_LINK;
        return;
    }

    // auto negotiation is turned on
    // we have to re-negotiate but the link is actually up only
    // after the re-negotiation is done
    // wait for negotiation complete
    phyRes = pMACDrv->sGmacData.gmacConfig.pPhyBase->DRV_ETHPHY_NegotiationIsComplete(pMACDrv->sGmacData.hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, false);
    if(phyRes < 0)
    {   // some error occurred
        _DRV_GMAC_LinkStateDown(pMACDrv);
    }
    else
    {   // advance
        pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_NEG_COMPLETE;
    }
   
}

static void _DRV_GMAC_LinkStateNegComplete(DRV_GMAC_DRIVER * pMACDrv) 
{
    DRV_ETHPHY_CLIENT_STATUS phyStat;
    DRV_ETHPHY_RESULT phyRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;

    pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase;
    phyStat = pPhyBase->DRV_ETHPHY_ClientStatus( pMACDrv->sGmacData.hPhyClient );
    if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
    {   // not done yet
        return;
    }

    phyRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(pMACDrv->sGmacData.hPhyClient);
    if(phyRes != DRV_ETHPHY_RES_OK)
    {   // some error occurred
        _DRV_GMAC_LinkStateDown(pMACDrv);
        return;
    }

    phyRes = pPhyBase->DRV_ETHPHY_NegotiationResultGet(pMACDrv->sGmacData.hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &pMACDrv->sGmacData._negResult);

    if(phyRes < 0)
    {   // some error occurred
        _DRV_GMAC_LinkStateDown(pMACDrv);
    }
    else
    {   // advance; get negotiation result
        pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_NEG_RESULT;
    }
   
}

static void _DRV_GMAC_LinkStateNegResult(DRV_GMAC_DRIVER * pMACDrv)  
{
    DRV_ETHPHY_CLIENT_STATUS phyStat;
    DRV_ETHPHY_RESULT phyRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;
    TCPIP_ETH_PAUSE_TYPE        pauseType;
    DRV_ETHPHY_CONFIG_FLAGS phyCfgFlags = 0;

    pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase;
    phyStat = pPhyBase->DRV_ETHPHY_ClientStatus( pMACDrv->sGmacData.hPhyClient );
    if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
    {   // not done yet
        return;
    }

    phyRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(pMACDrv->sGmacData.hPhyClient);
    if(phyRes != DRV_ETHPHY_RES_OK)
    {   // some error occurred
        _DRV_GMAC_LinkStateDown(pMACDrv);
        return;
    }

    
    if( (pMACDrv->sGmacData._negResult.linkStatus & DRV_ETHPHY_LINK_ST_UP) != 0 )
    {   // negotiation succeeded; properly update the MAC
        pPhyBase->DRV_ETHPHY_HWConfigFlagsGet(pMACDrv->sGmacData.hPhyClient, &phyCfgFlags);
        if((phyCfgFlags & DRV_ETHPHY_CFG_GMII))
        {
            pMACDrv->sGmacData._negResult.linkFlags |= TCPIP_ETH_OPEN_GMII;
        }
        else if((phyCfgFlags & DRV_ETHPHY_CFG_RGMII))
        {
            pMACDrv->sGmacData._negResult.linkFlags |= TCPIP_ETH_OPEN_RGMII;
        }
        else if((phyCfgFlags & DRV_ETHPHY_CFG_MII))
        {
            pMACDrv->sGmacData._negResult.linkFlags |= TCPIP_ETH_OPEN_MII;
        }
        else if((phyCfgFlags & DRV_ETHPHY_CFG_RMII))
        {
            pMACDrv->sGmacData._negResult.linkFlags |= TCPIP_ETH_OPEN_RMII;
        }
        pauseType = pMACDrv->sGmacData._negResult.pauseType;
        DRV_PIC32CGMAC_LibMACOpen(pMACDrv, pMACDrv->sGmacData._negResult.linkFlags, pauseType);
        
        pMACDrv->sGmacData._macFlags._linkPrev = 1;
        pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_START_LINK;
    }
    else
    {
        _DRV_GMAC_LinkStateDown(pMACDrv);
    }
    
}

/******************************************************************************
 * Function:        TCPIP_MAC_RES DRV_GMAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, TCPIP_MAC_ADDR* DestMACAddr)
 *
 * PreCondition:    DRV_GMAC_Initialize() should have been called.
 *
 * Input:           DestMACAddr: 6 byte group destination MAC address to allow 
 *                  through the Hash Table Filter.  If DestMACAddr 
 *                  is set to 00-00-00-00-00-00, then the hash 
 *                  table will be cleared of all entries and the 
 *                  filter will be disabled.
 *
 * Output:          Sets the appropriate bit in the GMAC_HRB/GMAC_HRT registers to allow 
 *                  packets sent to DestMACAddr to be received and enabled the 
 *                  Hash Table receive filter (if not already).
 *
 *                  Returns TCPIP_MAC_RES_OK if success,
 *                  an eror code otherwise
 *
 * Side Effects:    None
 *
 *
 * Note:            There is no way to individually unset destination MAC 
 *                  addresses from the hash table since it is possible to have 
 *                  a hash collision and therefore multiple MAC addresses 
 *                  relying on the same hash table bit.  The stack would have 
 *                  to individually store each 6 byte MAC address to support 
 *                  this feature, which would waste a lot of RAM and be 
 *                  unnecessary in most applications.  As a simple compromise, 
 *                  you can call DRV_GMAC_RxFilterHashTableEntrySet() using a 
 *                  00-00-00-00-00-00 destination MAC address, which will clear 
 *                  the entire hash table and disable the hash table filter.  
 *                  This will allow you to then read the necessary destination 
 *                  addresses.
 *****************************************************************************/
TCPIP_MAC_RES DRV_GMAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr) 
{ 
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    if(pMACDrv == 0)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    DRV_GMAC_HASH hash;
    uint8_t nullMACAddr[6] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    // Clear the Hash Table bits and disable the Hash Table Filter if a special 
    // 00-00-00-00-00-00 destination MAC address is provided.
    if( DestMACAddr == 0 || memcmp(DestMACAddr->v, nullMACAddr, sizeof(nullMACAddr))==0 )
    {
        hash.hash_value = 0x0; //Clear to all 0 for 64-bit Hash value to reject all hash match
        hash.calculate_hash = false; // No hash calculation; directly set hash register
    }
    else
    {
        hash.DestMACAddr = DestMACAddr;    
        hash.calculate_hash = true; // calculate hash and set hash register
    }
    
    DRV_PIC32CGMAC_LibRxFilterHash_Calculate(pMACDrv, &hash);
      
    return TCPIP_MAC_RES_OK;
}

bool DRV_GMAC_PowerMode(DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode) 
{   // not supported
    return true;
}


TCPIP_MAC_RES DRV_GMAC_Process(DRV_HANDLE hMac)  
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    int8_t queueIdx = 0;
    
    if(pMACDrv == 0)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    if(DRV_PIC32CGMAC_LibIsTxComplete(pMACDrv))
    {
        // Clear the TXCOMP transmit status
        DRV_PIC32CGMAC_LibClearTxComplete(pMACDrv);
        //start processing high priority Queue first
        for(queueIdx = pMACDrv->sGmacData.gmacConfig.macQueNum -1; queueIdx >= (int32_t)GMAC_QUE_0; queueIdx--)
        {
            if ((pMACDrv->sGmacData.gmac_queue[queueIdx]._TxDescUnAckQueue.head) != NULL)
            {
                _MACTxAcknowledgeEth(pMACDrv,queueIdx); 
            }
        }
    }
    //start processing high priority Queue first
    for(queueIdx = pMACDrv->sGmacData.gmacConfig.macQueNum -1; queueIdx >= (int32_t)GMAC_QUE_0; queueIdx--)
    {
        if ((pMACDrv->sGmacData.gmac_queue[queueIdx]._TxQueue.head) != NULL)
        {
            _MacTxPendingPackets(pMACDrv,queueIdx); 
        }
    }

    return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_RES DRV_GMAC_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics) 
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    if(pMACDrv == 0)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }
    
    if(pRxStatistics)
    {
        _DRV_GMAC_RxLock(pMACDrv);
        DRV_PIC32CGMAC_LibRxBuffersCountGet(pMACDrv, &pMACDrv->sGmacData._rxStat.nRxPendBuffers, &pMACDrv->sGmacData._rxStat.nRxSchedBuffers);
        _DRV_GMAC_RxUnlock(pMACDrv);
        *pRxStatistics = pMACDrv->sGmacData._rxStat;
    }
    if(pTxStatistics)
    {
        _DRV_GMAC_TxLock(pMACDrv);
        pMACDrv->sGmacData._txStat.nTxPendBuffers = 0; //clear the previous pending status
        for(uint16_t index = 0; index < pMACDrv->sGmacData.gmacConfig.macQueNum; index++)
        {
            pMACDrv->sGmacData._txStat.nTxPendBuffers += DRV_PIC32CGMAC_SingleListCount(&(pMACDrv->sGmacData.gmac_queue[index]._TxQueue));
            
        }       
        _DRV_GMAC_TxUnlock(pMACDrv);
        *pTxStatistics = pMACDrv->sGmacData._txStat;
    }

    return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_RES DRV_GMAC_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries)  
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    if(pMACDrv == 0)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    const DRV_PIC32CGMAC_HW_REG_DCPT*   pHwRegDcpt;
    
    int nHwEntries = sizeof(macPIC32CHwRegDcpt)/sizeof(*macPIC32CHwRegDcpt);

    if(pHwEntries)
    {
        *pHwEntries = nHwEntries;
    }

    if(pRegEntries)
    {   // fill data
        unsigned int ix, regLim;

        regLim = nEntries > nHwEntries ? nHwEntries : nEntries;
        pHwRegDcpt = macPIC32CHwRegDcpt;
        for(ix = 0; ix < regLim; ix++, pRegEntries++, pHwRegDcpt++)
        {
            strncpy(pRegEntries->registerName, pHwRegDcpt->regName, sizeof(pRegEntries->registerName));
            pRegEntries->registerValue = (*pHwRegDcpt->regFunc)(pMACDrv);
        }
    }

    return TCPIP_MAC_RES_OK;

}

TCPIP_MAC_RES DRV_GMAC_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams)
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    if(pMACDrv == 0)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    if(pMACDrv->sGmacData.sysStat == SYS_STATUS_READY)
    {
        if(pMacParams)
        {       
            DRV_PIC32CGMAC_LibGetMacAddr(pMACDrv, pMacParams->ifPhyAddress.v);
            
            pMacParams->processFlags = (TCPIP_MAC_PROCESS_FLAG_RX | TCPIP_MAC_PROCESS_FLAG_TX);
            pMacParams->macType = TCPIP_MAC_TYPE_ETH;
            pMacParams->linkMtu = TCPIP_MAC_LINK_MTU_ETH;
            pMacParams->checksumOffloadRx = pMACDrv->sGmacData.gmacConfig.checksumOffloadRx;
            pMacParams->checksumOffloadTx = pMACDrv->sGmacData.gmacConfig.checksumOffloadTx;
            pMacParams->macTxPrioNum = pMACDrv->sGmacData.gmacConfig.macTxPrioNum;
            pMacParams->macRxPrioNum = pMACDrv->sGmacData.gmacConfig.macRxPrioNum;;
        }

        return TCPIP_MAC_RES_OK;
    }

    return TCPIP_MAC_RES_IS_BUSY;
    
}

/**********************************************
 * local functions and helpers
 ***********************************************/

// acknowledge the ETHC packets
static void _MACTxAcknowledgeEth(DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx)  
{
    DRV_PIC32CGMAC_LibTxAckPacket(pMACDrv,queueIdx);
}


// transmits pending packets, if any
// if the link is down the TX queued packets are discarded
// returns:
//      - TCPIP_MAC_RES_OK if the queue is empty and another packet can be scheduled for TX
//      - TCPIP_MAC_RES_PENDING if MAC descriptor queue is full and no packet can be scheduled for TX

static TCPIP_MAC_RES _MacTxPendingPackets(DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx ) 
{
    TCPIP_MAC_PACKET* pPkt;
    
    DRV_PIC32CGMAC_RESULT ethRes = DRV_PIC32CGMAC_RES_NO_PACKET;    
    _DRV_GMAC_TxLock(pMACDrv); 
    //packet in queue for transmission
    while((pPkt = (TCPIP_MAC_PACKET*)DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxQueue)) != 0)
    {
        ethRes = DRV_PIC32CGMAC_LibTxSendPacket(pMACDrv,  pPkt->pDSeg, queueIdx);
        if(ethRes == DRV_PIC32CGMAC_RES_NO_DESCRIPTORS)
        {
            pMACDrv->sGmacData._txStat.nTxQueueFull++;
            //not enough descriptors available; add back the packet to _TxQueue head
            DRV_PIC32CGMAC_SingleListHeadAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxQueue, (DRV_PIC32CGMAC_SGL_LIST_NODE*)pPkt); 
            break;
        }
    }   
    _DRV_GMAC_TxUnlock(pMACDrv);
    
    if((ethRes == DRV_PIC32CGMAC_RES_OK)||(ethRes == DRV_PIC32CGMAC_RES_NO_PACKET))
    {
        return TCPIP_MAC_RES_OK;
    }
    else if(ethRes == DRV_PIC32CGMAC_RES_NO_DESCRIPTORS)
    {   
        // not enough resource in the Tx hardware
        return TCPIP_MAC_RES_PENDING;
    }
    else
    {       
        // not transmitted  
        pMACDrv->sGmacData._txStat.nTxErrorPackets++;
        (*pMACDrv->sGmacData.pktAckF)(pPkt, TCPIP_MAC_PKT_ACK_BUFFER_ERR,TCPIP_THIS_MODULE_ID); 
        return TCPIP_MAC_RES_PACKET_ERR; 
    }

}


#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static void _MACDeinit(DRV_GMAC_DRIVER * pMACDrv )
{
     DRV_PIC32CGMAC_LibSysInt_Disable(pMACDrv, GMAC_ALL_QUE_MASK, NULL);
     DRV_PIC32CGMAC_LibClose(pMACDrv, DRV_PIC32CGMAC_CLOSE_DEFAULT);
     DRV_PIC32CGMAC_LibSysIntStatus_Clear(pMACDrv, GMAC_ALL_QUE_MASK);
    
     DRV_GMAC_EventDeInit((DRV_HANDLE)pMACDrv);
     
     const DRV_ETHPHY_OBJECT_BASE* pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase;

     if(pMACDrv->sGmacData.hPhyClient != DRV_HANDLE_INVALID)
     {
         pPhyBase->DRV_ETHPHY_Close(pMACDrv->sGmacData.hPhyClient);
         pMACDrv->sGmacData.hPhyClient = DRV_HANDLE_INVALID;
     }
     if(pMACDrv->sGmacData.hPhySysObject != SYS_MODULE_OBJ_INVALID)
     {
         pPhyBase->DRV_ETHPHY_Deinitialize(pMACDrv->sGmacData.hPhySysObject);
         pMACDrv->sGmacData.hPhySysObject = SYS_MODULE_OBJ_INVALID;
         
     }
     
    _MACCleanup(pMACDrv);
    pMACDrv->sGmacData.sysStat = SYS_STATUS_UNINITIALIZED;
    pMACDrv->sGmacData._macFlags.val = 0;

}

static void _MACCleanup(DRV_GMAC_DRIVER * pMACDrv )
{
    // TX clean up
    _MacTxDiscardQueues(pMACDrv,TCPIP_MAC_PKT_ACK_NET_DOWN);
    _MacTxFreeQueue(pMACDrv);

    // RX clean up
    _MacRxFreePacket(pMACDrv);
   
    _DRV_GMAC_RxDelete(pMACDrv);
    _DRV_GMAC_TxDelete(pMACDrv);
}

static void _MacRxFreePacket( DRV_GMAC_DRIVER * pMACDrv)
{
    TCPIP_MAC_PACKET* pRxPkt;
    DRV_GMAC_QUEUE gmac_queue;
    GMAC_QUE_LIST queueIdx;
    uint16_t index = 0;
    
    //free all the Rx packets linked to New and Ack Queues.
    for(queueIdx = GMAC_QUE_0; queueIdx < pMACDrv->sGmacData.gmacConfig.macQueNum; queueIdx++)
    {   
        gmac_queue = pMACDrv->sGmacData.gmac_queue[queueIdx];
        
        //Free all Rx packets in Rx Queue
        while(true)
        {
            _DRV_GMAC_RxLock(pMACDrv); 
            pRxPkt = (TCPIP_MAC_PACKET*)DRV_PIC32CGMAC_SingleListHeadRemove(&gmac_queue._RxQueue);
            _DRV_GMAC_RxUnlock(pMACDrv);
            
            if(pRxPkt == NULL)
            {
                break;
            }
            // Free the Rx packet
            (*pMACDrv->sGmacData.pktFreeF)(pRxPkt);                   
        }        
        //Free the Rx packets assigned to Rx descriptors and set rx descriptors to default
        for(index = 0; index < pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt; index++ )
        {
            if(gmac_queue.pRxPckt != NULL)
            {
                // get Rx packet for each rx descriptor
                pRxPkt = gmac_queue.pRxPckt[index];
                if(pRxPkt)
                {
                    
                    (*pMACDrv->sGmacData.pktFreeF)(pRxPkt);
                }
                
                gmac_queue.nRxDescIndex = 0;
                gmac_queue.pRxPckt[index] = 0;
                gmac_queue.pRxDesc[index].rx_desc_buffaddr.val = 0;
                gmac_queue.pRxDesc[index].rx_desc_status.val = 0;
            }
        }
        
        if(gmac_queue.pRxPckt != NULL)
        {
            //free rx packet array of pointers 
            (*pMACDrv->sGmacData._freeF)(pMACDrv->sGmacData._AllocH, gmac_queue.pRxPckt); 
            gmac_queue.pRxPckt = NULL;
        }
        
        __DMB();
    }
    
}

static void     _MacTxFreeQueue( DRV_GMAC_DRIVER * pMACDrv)
{
    DRV_GMAC_QUEUE gmac_queue;
    GMAC_QUE_LIST queueIdx;
    DRV_GMAC_TX_DESC_INDEX * pTxQueueNode;
    
    //free all the Tx Ack Queue Nodes     
    for(queueIdx = GMAC_QUE_0; queueIdx < pMACDrv->sGmacData.gmacConfig.macQueNum; queueIdx++)
    {   
        gmac_queue = pMACDrv->sGmacData.gmac_queue[queueIdx];
        
        while(true)    
        { 
            _DRV_GMAC_TxLock(pMACDrv); 
            pTxQueueNode = (DRV_GMAC_TX_DESC_INDEX *)DRV_PIC32CGMAC_SingleListHeadRemove(&gmac_queue._TxDescAckPoolQueue);
            _DRV_GMAC_TxUnlock(pMACDrv);
            if(pTxQueueNode == NULL)
            {
                break;
            }
            //free Tx Queue Node 
            (*pMACDrv->sGmacData._freeF)(pMACDrv->sGmacData._AllocH, pTxQueueNode);             
        }   
        
        while(true)    
        { 
            _DRV_GMAC_TxLock(pMACDrv); 
            pTxQueueNode = (DRV_GMAC_TX_DESC_INDEX *)DRV_PIC32CGMAC_SingleListHeadRemove(&gmac_queue._TxDescUnAckQueue);
            _DRV_GMAC_TxUnlock(pMACDrv);
            if(pTxQueueNode == NULL)
            {
                break;
            }
            //free Tx Queue Node 
            (*pMACDrv->sGmacData._freeF)(pMACDrv->sGmacData._AllocH, pTxQueueNode);             
        } 
    }
}
#else
static void _MACDeinit(DRV_GMAC_DRIVER * pMACDrv )
{

}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

static void _MacTxDiscardQueues(DRV_GMAC_DRIVER * pMACDrv,TCPIP_MAC_PKT_ACK_RES ackRes)  
{
    GMAC_QUE_LIST queueIdx;  
    
    for(queueIdx = GMAC_QUE_0; queueIdx < pMACDrv->sGmacData.gmacConfig.macQueNum; queueIdx++)
    {
        // Discard any queued transmit packets
        if ((pMACDrv->sGmacData.gmac_queue[queueIdx]._TxQueue.head) != NULL)
        {
            // Disable GMAC Transmit
            DRV_PIC32CGMAC_LibTxEnable(pMACDrv, false);
            //Acknowledge the TX packets Queued
            DRV_PIC32CGMAC_LibTxAckPendPacket(pMACDrv, queueIdx, ackRes); 
            // Reset Transmit Indexes
            DRV_PIC32CGMAC_LibClearTxIndex(pMACDrv, queueIdx);
            // Enable GMAC Transmit
            DRV_PIC32CGMAC_LibTxEnable(pMACDrv, true);            
        }
        
        // Discard any unacknowledged transmit packets
        if ((pMACDrv->sGmacData.gmac_queue[queueIdx]._TxDescUnAckQueue.head) != NULL)
        {
            // Disable GMAC Transmit
            DRV_PIC32CGMAC_LibTxEnable(pMACDrv, false);
            //Acknowledge the unacknowledged TX packets
            DRV_PIC32CGMAC_LibTxClearUnAckPacket(pMACDrv, queueIdx, ackRes); 
            // Reset Transmit Indexes
            DRV_PIC32CGMAC_LibClearTxIndex(pMACDrv, queueIdx);
            // Enable GMAC Transmit
            DRV_PIC32CGMAC_LibTxEnable(pMACDrv, true);     
        }
    }
    
}

/*************************
 * local data
 *************************/

/*********************************
 *  local proto 
 ******************************************/

/****************************************************************************
 * Function:        _XtlEventsTcp2Eth
 *
 * PreCondition:    None
 *
 * Input:           tcpEv - the TcpIp events to be translated
 *
 * Output:          Ethernet events
 *
 * Side Effects:    None
 *
 * Overview:        This function translates from  events to Ethernet events
 *
 * Note:            - The translation could be done in a more systematic way
 *                    using a (few) LUT(s).
 *                    For now this direct approach is taken.
 *                  - The CONN_LOST/CONN_ESTABLISHED events do NOT map to Ethernet events! 
 *                    The current translation function assumes no overlap between a CONN event
 *                    and an Ethernet one!
 *                    By this translation the CONN_LOST/CONN_ESTABLISHED event is lost!
 ******************************************************************************/
/*static __inline__*/static  GMAC_EVENTS /*__attribute__((always_inline))*/ _XtlEventsTcp2Eth(TCPIP_MAC_EVENT tcpEv)
{

    GMAC_EVENTS  eEvents;
    eEvents = (tcpEv&(TCPIP_MAC_EV_TX_DONE))? GMAC_EV_TXCOMPLETE:0; 
    eEvents |= (tcpEv&(TCPIP_MAC_EV_RX_DONE))? GMAC_EV_RXCOMPLETE:0;    
    eEvents |= (tcpEv&(TCPIP_MAC_EV_RX_OVFLOW))? GMAC_EV_RXOVERRUN:0;
    eEvents |= (tcpEv&(TCPIP_MAC_EV_TX_BUSERR))? GMAC_EV_TXFRAMECORRUPT:0;
    eEvents |= (tcpEv&(TCPIP_MAC_EV_TX_ABORT))? GMAC_EV_TXUNDERRUN:0;
    eEvents |= (tcpEv&(TCPIP_MAC_EV_RX_BUFNA))? GMAC_EV_RXUSEDBITREAD:0; // mapping RX Used bit interrupt to BUFNA;
                                                                         // reason is no GMAC interrupt bit for BUFNA
                                                                         // and RXUBR interrupt will trigger with BUFNA
    
    return eEvents;
    
}

/****************************************************************************
 * Function:        _XtlEventsEth2Tcp
 *
 * PreCondition:    None
 *
 * Input:           eEvents - the Eth events to be translated
 *
 * Output:          TcpIp events
 *
 * Side Effects:    None
 *
 * Overview:        This function translates from Ethernet events to TcpIp events
 *
 * Note:            - The translation could be done in a more systematic way
 *                    using a (few) LUT(s).
 *                    For now this direct approach is taken.
 *                  - The CONN_LOST/CONN_ESTABLISHED events do NOT map to Ethernet events! 
 *                    The current translation function assumes no overlap between a CONN event
 *                    and an Ethernet one!
 *                    By this translation a CONN_LOST/CONN_ESTABLISHED event cannot be generated!
 ******************************************************************************/
/*static __inline__*/static  TCPIP_MAC_EVENT /*__attribute__((always_inline))*/ _XtlEventsEth2Tcp(GMAC_EVENTS eEvents)
{
    TCPIP_MAC_EVENT tcpEv;

    tcpEv = (eEvents&(GMAC_EV_TXCOMPLETE))? TCPIP_MAC_EV_TX_DONE:0;
    tcpEv |= (eEvents&(GMAC_EV_RXCOMPLETE))? TCPIP_MAC_EV_RX_DONE:0;
    tcpEv |= (eEvents&(GMAC_EV_RXOVERRUN))? TCPIP_MAC_EV_RX_OVFLOW:0;
    tcpEv |= (eEvents&(GMAC_EV_TXFRAMECORRUPT))? TCPIP_MAC_EV_TX_BUSERR:0;
    tcpEv |= (eEvents&(GMAC_EV_RXUSEDBITREAD))? TCPIP_MAC_EV_RX_BUFNA:0; // mapping RX Used bit interrupt to BUFNA;
                                                                         // reason is no GMAC interrupt bit for BUFNA
                                                                         // and RXUBR interrupt will trigger with BUFNA
    
    return tcpEv;
}

/*********************************
 *  implementation
 ******************************************/

/*******************************************************************************
  Function:
    TCPIP_MAC_RES DRV_GMAC_EventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam)

  Summary:
    Initializes the ethernet event notification.

  Description:
     This function initializes the ethernet event notification.
     It performs any resource allocation that may be needed.

  Precondition:
     None.

  Parameters:
    hMac        - parameter identifying the intended MAC  
    eventF      - notification function
    eventParam  - notification parameter
    
  Returns:
    TCPIP_MAC_RES_OK  if initialization succeeded,
    an error code otherwise

  Example:
    <code>
    DRV_GMAC_EventInit( hMac, eventF, param);
    </code>

  Remarks:

    Not multi-threaded safe.
*****************************************************************************/
static TCPIP_MAC_RES DRV_GMAC_EventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam)
{
    DRV_GMAC_EVENT_DCPT*    pDcpt;
    DRV_GMAC_DRIVER * pMACDrv;

    pMACDrv = (DRV_GMAC_DRIVER*)hMac;

    DRV_PIC32CGMAC_LibSysInt_Disable(pMACDrv, GMAC_ALL_QUE_MASK, NULL);
    DRV_PIC32CGMAC_LibSysIntStatus_Clear(pMACDrv, GMAC_ALL_QUE_MASK);
    
    pDcpt = &pMACDrv->sGmacData._gmac_event_group_dcpt;
    pDcpt->_TcpEnabledEvents = pDcpt->_TcpPendingEvents = TCPIP_MAC_EV_NONE;
    pDcpt->_EthEnabledEvents = pDcpt->_EthPendingEvents = 0;
    pDcpt->_TcpNotifyFnc = eventF;
    pDcpt->_TcpNotifyParam = eventParam;

    return TCPIP_MAC_RES_OK;

}

/*******************************************************************************
  Function:
    TCPIP_MAC_RES    DRV_GMAC_EventDeInit(DRV_HANDLE hMac )

  Summary:
    De-initializes the ethernet event notification.

  Description:
     This function de-initializes the ethernet event notification.
     It performs any resource clean-up that may be needed.

  Precondition:
     None.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    
  Returns:
    TCPIP_MAC_RES_OK  always

  Example:
    <code>
    DRV_GMAC_EventDeInit( hMac );
    </code>

  Remarks:

    Not multi-threaded safe.
*****************************************************************************/
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static TCPIP_MAC_RES DRV_GMAC_EventDeInit(DRV_HANDLE hMac) 
{
    DRV_GMAC_EVENT_DCPT*    pDcpt;
    DRV_GMAC_DRIVER * pMACDrv;

    pMACDrv = (DRV_GMAC_DRIVER*)hMac;

    DRV_PIC32CGMAC_LibSysInt_Disable(pMACDrv, GMAC_ALL_QUE_MASK, NULL);
    DRV_PIC32CGMAC_LibSysIntStatus_Clear(pMACDrv, GMAC_ALL_QUE_MASK);

    pDcpt = &pMACDrv->sGmacData._gmac_event_group_dcpt;
    pDcpt->_TcpNotifyFnc = 0;
    pDcpt->_TcpEnabledEvents = pDcpt->_TcpPendingEvents = TCPIP_MAC_EV_NONE;
    pDcpt->_EthEnabledEvents = pDcpt->_EthPendingEvents = 0;

    return TCPIP_MAC_RES_OK;
    
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)


/*******************************************************************************
  Function:
    bool    DRV_GMAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable)

  Summary:
    Enables the MAC events.

  Description:
     This function sets the enabled events.
     Multiple events can be orr-ed together.
     All events that are set will be added to the notification process. The old events will be disabled.
     The stack (or stack user) has to catch the events that are notified and process them:
         - The stack should process the TCPIP_MAC_EV_RX_PKTPEND/TCPIP_MAC_EV_RX_DONE, TCPIP_MAC_EV_TX_DONE transfer events
         - Process the specific condition and acknowledge them calling DRV_GMAC_EventAcknowledge() so that they can be re-enabled.

  Precondition:
   DRV_GMAC_EventInit should have been called.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    macEvMask - events the user of the stack wants to add/delete for notification
    enable    - if true, the events will  be enabled, else disabled
    
  Returns:
    true  if operation succeeded,
    an error code otherwise

  Example:
    <code>
    DRV_GMAC_EventMaskSet( hMac, TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA, true );
    </code>

  Remarks:
    The event notification system enables the user of the TCPIP stack to call into the stack
    for processing only when there are relevant events rather than being forced to periodically call
    from within a loop.
    
    If the notification events are nill the interrupt processing will be disabled.
    Otherwise the event notification will be enabled and the interrupts relating to the requested events will be enabled.
    
    Note that once an event has been caught by the stack ISR (and reported if a notification handler is in place)
    it will be disabled until the DRV_GMAC_EventAcknowledge() is called.

*****************************************************************************/
bool DRV_GMAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvMask, bool enable) 
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    bool *intStat = 0;
    DRV_GMAC_EVENT_DCPT*  pDcpt = &pMACDrv->sGmacData._gmac_event_group_dcpt;   
    GMAC_QUE_LIST queIdx;
    GMAC_EVENTS ethEvents;
    
    if(pMACDrv == 0)
    {
        return false;
    }
    if(enable)
    {
        GMAC_EVENTS  ethSetEvents;
        
        ethSetEvents = _XtlEventsTcp2Eth(macEvMask);

        if(pDcpt->_TcpEnabledEvents != 0)
        {   // already have some active
            DRV_PIC32CGMAC_LibSysInt_Disable(pMACDrv, GMAC_ALL_QUE_MASK, NULL);
        }

        pDcpt->_TcpEnabledEvents |= macEvMask;        // add more
        pDcpt->_EthEnabledEvents |= ethSetEvents;

        if(pDcpt->_TcpEnabledEvents != 0)
        {
            ethSetEvents &= ~pDcpt->_EthPendingEvents;      // keep just the new un-ack events          
            for(queIdx = GMAC_QUE_0; queIdx < pMACDrv->sGmacData.gmacConfig.macQueNum; queIdx++)
            {
                ethEvents = ethSetEvents;    
                if(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queIdx].queueTxEnable != true)
                {
                    ethEvents = ethEvents & (~GMAC_EV_TX_ALL);
                }
                if(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queIdx].queueRxEnable != true)
                {
                    ethEvents = ethEvents & (~GMAC_EV_RX_ALL);
                }
                //Read ISR register to clear the interrupt status   
                DRV_PIC32CGMAC_LibReadInterruptStatus(pMACDrv,queIdx);
                //Enable GMAC interrupts
                DRV_PIC32CGMAC_LibEnableInterrupt(pMACDrv, queIdx, ethEvents);

            }
            
            DRV_PIC32CGMAC_LibSysInt_Enable(pMACDrv, GMAC_ALL_QUE_MASK);
        }
    }
    else
    {   // disable some events
        GMAC_EVENTS  ethClrEvents;
#if defined(DRV_GMAC0) || defined(DRV_GMAC) || defined(DRV_ETH)      
        if (pMACDrv->pObj->macId == TCPIP_MODULE_MAC_PIC32C_0)
        {
            intStat = interruptStat_mac0;
        }
#endif
#ifdef DRV_GMAC1             
        if (pMACDrv->pObj->macId == TCPIP_MODULE_MAC_PIC32C_1)
        {
            intStat = interruptStat_mac1;
        }
#endif
        
        macEvMask &= pDcpt->_TcpEnabledEvents;                  // keep just the enabled ones
        ethClrEvents = _XtlEventsTcp2Eth(macEvMask);

        if(pDcpt->_TcpEnabledEvents != 0)
        {   // already have some active
            DRV_PIC32CGMAC_LibSysInt_Disable(pMACDrv, GMAC_ALL_QUE_MASK, intStat);
        }

        pDcpt->_TcpEnabledEvents &= ~macEvMask;     // clear some of them
        pDcpt->_EthEnabledEvents &= ~ethClrEvents;

        pDcpt->_TcpPendingEvents &= ~macEvMask;     // remove them from un-ack list
        pDcpt->_EthPendingEvents &= ~ethClrEvents;
        
        for(queIdx = GMAC_QUE_0; queIdx < pMACDrv->sGmacData.gmacConfig.macQueNum; queIdx++)
        {            
            //Disable GMAC interrupts
            DRV_PIC32CGMAC_LibDisableInterrupt(pMACDrv, queIdx, ethClrEvents);
            //Read ISR register to clear the interrupt status   
            DRV_PIC32CGMAC_LibReadInterruptStatus(pMACDrv,queIdx);

        }

        if(pDcpt->_TcpEnabledEvents != 0)
        {
            DRV_PIC32CGMAC_LibSysInt_Restore(pMACDrv, GMAC_ALL_QUE_MASK, intStat);
        }
    }

    return true;

}


/*******************************************************************************
  Function:
    bool    DRV_GMAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv)

  Summary:
    Acknowledges and re-enables processed events.

  Description:
    This function acknowledges and re-enables processed events.
    Multiple events can be orr-ed together as they are processed together.
    The events acknowledged by this function should be the events that have been retrieved from the stack
    by calling DRV_GMAC_EventPendingGet() or have been passed to the user by the stack using the notification handler
    (PIC32MACEventSetNotifyHandler()) and have been processed and have to be re-enabled.


  Precondition:
   DRV_GMAC_EventInit should have been called.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    macEvents - the events that the user processed and need to be re-enabled
    
  Returns:
    true if events acknowledged
    false if no events to be acknowledged

  Example:
    <code>
    DRV_GMAC_EventAcknowledge( hMac, stackNewEvents );
    </code>

  Remarks:   
    All events should be acknowledged, in order to be re-enabled.

    Some events are fatal errors and should not be acknowledged (TCPIP_MAC_EV_RX_BUSERR, TCPIP_MAC_EV_TX_BUSERR).
    Stack re-initialization is needed under such circumstances.

    Some events are just system/application behavior and they are intended only as simple info (TCPIP_MAC_EV_RX_OVFLOW,
    TCPIP_MAC_EV_RX_BUFNA, TCPIP_MAC_EV_TX_ABORT, TCPIP_MAC_EV_RX_ACT).

    The TCPIP_MAC_EV_RX_FWMARK and TCPIP_MAC_EV_RX_EWMARK events are part of the normal flow control operation (if auto flow control was enabled).
    They should be enabled alternatively, if needed.

    The events are persistent. They shouldn't be re-enabled unless they have been processed and the condition that generated them was removed.
    Re-enabling them immediately without proper processing will have dramatic effects on system performance.

*****************************************************************************/
bool DRV_GMAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv) 
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    bool *intStat = 0;
    DRV_GMAC_EVENT_DCPT*  pDcpt = &pMACDrv->sGmacData._gmac_event_group_dcpt;
    GMAC_QUE_LIST queIdx;
    if(pMACDrv == 0)
    {
        return false;
    }
        
#if defined(DRV_GMAC0) || defined(DRV_GMAC) || defined(DRV_ETH)       
    if (pMACDrv->pObj->macId == TCPIP_MODULE_MAC_PIC32C_0)
    {
        intStat = interruptStat_mac0;
    }
#endif
#ifdef DRV_GMAC1             
    if (pMACDrv->pObj->macId == TCPIP_MODULE_MAC_PIC32C_1)
    {
        intStat = interruptStat_mac1;
    }
#endif
    
    if(pDcpt->_TcpEnabledEvents != 0)
    {   
        // already have some active
        GMAC_EVENTS  ethAckEv;

        ethAckEv=_XtlEventsTcp2Eth(tcpAckEv);

        // stop interrupts for a while
        DRV_PIC32CGMAC_LibSysInt_Disable(pMACDrv, GMAC_ALL_QUE_MASK, intStat);

        pDcpt->_TcpPendingEvents &= ~tcpAckEv;         // no longer pending
        pDcpt->_EthPendingEvents &= ~ethAckEv;         // no longer pending
        
        for(queIdx = GMAC_QUE_0; queIdx < pMACDrv->sGmacData.gmacConfig.macQueNum; queIdx++)
        {   
            //Read ISR register to clear the interrupt status   
            DRV_PIC32CGMAC_LibReadInterruptStatus(pMACDrv, queIdx);
            //Enable GMAC interrupts
            DRV_PIC32CGMAC_LibEnableInterrupt(pMACDrv, queIdx, ethAckEv);
            
        }

        DRV_PIC32CGMAC_LibSysInt_Restore(pMACDrv, GMAC_ALL_QUE_MASK, intStat);
        
        return true;
    }

    return false;
    
}


/*******************************************************************************
  Function:
    TCPIP_MAC_EVENT DRV_GMAC_EventPendingGet(DRV_HANDLE hMac)

  Summary:
    Returns the currently pending events.

  Description:
    This function returns the currently pending events belonging to a group.
    Multiple events can be orr-ed together as they accumulate.
    The stack should be called for processing whenever a stack managed event (TCPIP_MAC_EV_RX_PKTPEND, TCPIP_MAC_EV_TX_DONE) is present.
    The other, non critical events, may not be managed by the stack and passed to an user.
    They will have to be eventually acknowledged if re-enabling is needed.

  Precondition:
   hMac      - parameter identifying the intended MAC  
   DRV_GMAC_EventInit should have been called.

  Parameters:
    
  Returns:
    The currently stack pending events.

  Example:
    <code>
    TCPIP_MAC_EVENT currEvents = DRV_GMAC_EventPendingGet( hMac);
    </code>

  Remarks:   
    This is the preferred method to get the current pending MAC events.
    The stack maintains a proper image of the events from their occurrence to their acknowledgement.
    
    Even with a notification handler in place it's better to use this function to get the current pending events
    rather than using the events passed by the notification handler which could be stale.
    
    The events are persistent. They shouldn't be re-enabled unless they have been processed and
    the condition that generated them was removed.
    Re-enabling them immediately without proper processing will have dramatic effects on system performance.

    The returned value is just a momentary value. The pending events can change any time.
*****************************************************************************/
TCPIP_MAC_EVENT DRV_GMAC_EventPendingGet(DRV_HANDLE hMac) 
{
    DRV_GMAC_DRIVER * pMACDrv = _GmacHandleToMacInst(hMac);
    if(pMACDrv != 0)
    {
        return pMACDrv->sGmacData._gmac_event_group_dcpt._TcpPendingEvents;
    }

    return TCPIP_MAC_EV_NONE;
}


/**************************************
 * local functions
 ****************************************/
// Rx Filter for PIC32C
static GMAC_RX_FILTERS _DRV_GMAC_MacToEthFilter(TCPIP_MAC_RX_FILTER_TYPE macFilter) 
{
    GMAC_RX_FILTERS rxFilter = 0;
    
    rxFilter = (macFilter & TCPIP_MAC_RX_FILTER_TYPE_FRAMEERROR_ACCEPT) ? GMAC_FILT_PREAMBLE_ERR_ACCEPT: 0;
    
    rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_ACCEPT) ? GMAC_FILT_CRC_ERR_ACCEPT:0;
    
    rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_MAXFRAME_ACCEPT) ? GMAC_FILT_MAXFRAME_ACCEPT:0;
    
    rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_UCAST_HASH_ACCEPT) ? GMAC_FILT_UCASTHASH_ACCEPT:0;
    
    rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT) ? GMAC_FILT_MCASTHASH_ACCEPT:0;
    
    rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_MCAST_HASH_ACCEPT) ? GMAC_FILT_MCASTHASH_ACCEPT:0;
    
    rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT) ? 0:GMAC_FILT_BCAST_REJECT;

    rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_ALL_ACCEPT) ? GMAC_FILT_ALLFRAME_ACCEPT:0;
    
    rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_JUMBOFRAME_ACCEPT) ? GMAC_FILT_JUMBOFRAME_ACCEPT:0;
    
    return rxFilter;

}

/****************************************************************************
 * Function:        DRV_GMAC_Tasks_ISR
 *
 * PreCondition:    DRV_GMAC_EventInit, DRV_GMAC_EventMaskSet should have been called.
 *
 * Input:           macIndex - PIC32 MAC object index
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function processes the Ethernet interrupts and reports the events back to the user.
 *
 * Note:            None
 ******************************************************************************/
void DRV_GMAC_Tasks_ISR( SYS_MODULE_OBJ macIndex, uint32_t  currEthEvents)
{
    GMAC_EVENTS currGroupEvents = (GMAC_EVENTS)GMAC_EV_NONE;
    DRV_GMAC_EVENT_DCPT* pDcpt; 
    DRV_GMAC_DRIVER * pMACDrv = &_gmac_drv_dcpt[macIndex];      

    // process interrupts
    pDcpt = &pMACDrv->sGmacData._gmac_event_group_dcpt;
    //  keep just the relevant ones
    currGroupEvents = ((GMAC_EVENTS)currEthEvents) & pDcpt->_EthEnabledEvents;
    
    if(currGroupEvents)
    {
        // add the new events
        pDcpt->_EthPendingEvents |= currGroupEvents;                    
        pDcpt->_TcpPendingEvents |= _XtlEventsEth2Tcp(currGroupEvents);

        if(pDcpt->_TcpNotifyFnc)
        {
            // let the user know
            (*pDcpt->_TcpNotifyFnc)(pDcpt->_TcpPendingEvents, pDcpt->_TcpNotifyParam);     
        }
    }

}

/****************************************************************************
 * Function:        _DRV_GMAC_PHYInitialise
 * Summary: PHY initialization 
 *****************************************************************************/
static SYS_MODULE_OBJ _DRV_GMAC_PHYInitialise(DRV_GMAC_DRIVER *pMACDrv) 
{
    SYS_MODULE_OBJ hPhySysObject;
    DRV_HANDLE     hPhyClient;  
    DRV_ETHPHY_SETUP phySetup;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase; 
    int phyIx = pMACDrv->sGmacData._phyIx;
    TCPIP_MAC_RES   initRes;
    DRV_ETHPHY_RESULT   phyInitRes; 

    pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase; 

    hPhySysObject = pPhyBase->DRV_ETHPHY_Initialize(phyIx,  (SYS_MODULE_INIT *)pMACDrv->sGmacData.gmacConfig.pPhyInit);
    if ( hPhySysObject == SYS_MODULE_OBJ_INVALID )
    {
        initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
        return initRes;
    }

    pMACDrv->sGmacData.hPhySysObject = hPhySysObject;
    hPhyClient = pPhyBase->DRV_ETHPHY_Open(phyIx, DRV_IO_INTENT_READWRITE);
    if ( DRV_HANDLE_INVALID == hPhyClient )
    {
        initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
        return initRes;
    }
    
    pMACDrv->sGmacData.hPhyClient = hPhyClient;
    phySetup.phyAddress = pMACDrv->sGmacData.gmacConfig.pPhyInit->phyAddress;
    phySetup.openFlags = pMACDrv->sGmacData.gmacConfig.ethFlags;
    phySetup.configFlags = pMACDrv->sGmacData.gmacConfig.pPhyInit->phyFlags;
    phySetup.macPauseType = DRV_GMAC_PAUSE_CPBL_MASK;
    phySetup.resetFunction = pMACDrv->sGmacData.gmacConfig.pPhyInit->resetFunction;
        
    phyInitRes = pPhyBase->DRV_ETHPHY_Setup(hPhyClient, &phySetup, &pMACDrv->sGmacData._linkResFlags);
    
    if(phyInitRes < 0)
    {   // some error occurred
        initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
        return initRes;
    }
    initRes = TCPIP_MAC_RES_OK;
    return initRes;
}
