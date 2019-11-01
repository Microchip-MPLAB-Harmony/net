/*******************************************************************************
  MAC Module for Microchip TCP/IP Stack
*******************************************************************************/

/*****************************************************************************
Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.

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
#include "driver/emac/src/dynamic/mac_drvr_lib.h"
void emacStatisticsClear( void );

/******************************************************************************/
TCPIP_MAC_RES macDrvrStatisticsGet(         DRV_HANDLE                          hMac,
                                            TCPIP_MAC_RX_STATISTICS *           pRxStatistics,
                                            TCPIP_MAC_TX_STATISTICS *           pTxStatistics
                                            );
TCPIP_MAC_RES macDrvrRegisterStatisticsGet( DRV_HANDLE                          hMac,
                                            TCPIP_MAC_STATISTICS_REG_ENTRY *    pStatisticsEntry,
                                            int                                 maxEntries,
                                            int *                               pActualNumEntries
                                            );

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
// Statistics Register access
static uint32_t macDrvrGetEmacAle(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacEle(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacFcse( emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacFro(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacPfr(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacRja(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacRle(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacRov(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacRre(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacRse(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacUsf(  emac_registers_t * pToMacRegs );

static uint32_t macDrvrGetEmacCse(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacDtf(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacEcol( emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacFto(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacLcol( emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacMcf(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacScf(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacSte(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacTpf(  emac_registers_t * pToMacRegs );
static uint32_t macDrvrGetEmacTund( emac_registers_t * pToMacRegs );

// Statistics register access
static const MAC_DRVR_HW_REG_DCPT macHwRegDcpt[] =
{ //   12345678901234567890123456789012345Null 35 + null
    { "Rx ALE : Alignment Errors          ", macDrvrGetEmacAle },
    { "Rx ELE : Excessive Length Errors   ", macDrvrGetEmacEle },
    { "Rx FCSE: Frame Chk Sequence Errors ", macDrvrGetEmacFcse },
    { "Rx FRO : Frames Received OK        ", macDrvrGetEmacFro },
    { "Rx PFR : Pause Frames Received OK  ", macDrvrGetEmacPfr },
    { "Rx RJA : Receive Jabbers           ", macDrvrGetEmacRja },
    { "Rx RLE : Rx Length Field Mismatch  ", macDrvrGetEmacRle },
    { "Rx ROV : Receive Over Run Errors   ", macDrvrGetEmacRov },
    { "Rx RRE : Receive Resource Errors   ", macDrvrGetEmacRre },
    { "Rx RSE : Receive Symbol Errors     ", macDrvrGetEmacRse },
    { "Rx USF : Under Size Frames         ", macDrvrGetEmacUsf },

    { "Tx CSE : Carrier Sense Errors      ", macDrvrGetEmacCse },
    { "Tx DTF : Deferred Transmit Frames  ", macDrvrGetEmacDtf },
    { "Tx ECOL: Excessive Collisions      ", macDrvrGetEmacEcol },
    { "Tx FTO : Frames Tramsmitted OK     ", macDrvrGetEmacFto },
    { "Tx LCOL: Late Collisions           ", macDrvrGetEmacLcol },
    { "Tx MCF : Multi Collision Frames    ", macDrvrGetEmacMcf },
    { "Tx SCF : Single Collision Frames   ", macDrvrGetEmacScf },
    { "Tx STE : SQE Test Errors           ", macDrvrGetEmacSte },
    { "Tx TPF : Transmitted Pause Frames  ", macDrvrGetEmacTpf },
    { "Tx TUND: Transmit Under Runs       ", macDrvrGetEmacTund },
};

TCPIP_MAC_RES macDrvrStatisticsGet(
    DRV_HANDLE                  hMac,
    TCPIP_MAC_RX_STATISTICS *   pRxStatistics,
    TCPIP_MAC_TX_STATISTICS *   pTxStatistics
    )
{
    MAC_DRIVER * pMacDrvr = (MAC_DRIVER *) hMac;

    if( pRxStatistics )
    {
        macDrvrRxLock( pMacDrvr );
        pMacDrvr->rxStat.packetPoolBuffers =    pMacDrvr->rxMacPacketPool.nNodes;
        pRxStatistics->nRxPendBuffers =         pMacDrvr->rxStat.macPacketsInStack;
        pRxStatistics->nRxSchedBuffers =        pMacDrvr->rxStat.nRxSchedBuffers;
        pRxStatistics->nRxErrorPackets =        pMacDrvr->rxStat.nRxErrorPackets;
        pRxStatistics->nRxFragmentErrors =      pMacDrvr->rxStat.nRxFragmentErrors;
        pRxStatistics->nRxBuffNotAvailable =    pMacDrvr->rxStat.nRxBuffNotAvailable;
        macDrvrRxUnlock( pMacDrvr );
    }
    if( pTxStatistics )
    {
        macDrvrTxLock( pMacDrvr );
        pTxStatistics->nTxPendBuffers =         pMacDrvr->txPendingList.nNodes;
        pTxStatistics->nTxOkPackets =           pMacDrvr->txStat.nTxOkPackets;
        pTxStatistics->nTxErrorPackets =        pMacDrvr->txStat.nTxErrorPackets;
        pTxStatistics->nTxQueueFull =           pMacDrvr->txStat.nTxQueueFull;
        macDrvrTxUnlock( pMacDrvr );
    }
    return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_RES macDrvrRegisterStatisticsGet(
    DRV_HANDLE                          hMac,
    TCPIP_MAC_STATISTICS_REG_ENTRY *    pStatisticsEntry,
    int                                 maxEntries,
    int *                               pActualNumEntries
    )
{
    int numEntries = sizeof( macHwRegDcpt ) / sizeof( *macHwRegDcpt );

    if( pActualNumEntries )
    {
        *pActualNumEntries = numEntries;
    }
    if( maxEntries > numEntries )
    {
        maxEntries = numEntries;
    }

    if( pStatisticsEntry )
    {
        // provide EMAC0_REGS, or EMAC1_REGS, as argument to statistics function
        emac_registers_t * pMacRegs = (emac_registers_t *)((MAC_DRIVER *) hMac)->config.ethModuleId;
        unsigned int rr, nameSize;

        for( rr = 0; rr < maxEntries; rr++ )
        {
            nameSize = sizeof( pStatisticsEntry[ rr ].registerName );
            strncpy( pStatisticsEntry[ rr ].registerName, macHwRegDcpt[ rr ].regName, nameSize );
            pStatisticsEntry[ rr ].registerName[ nameSize - 1 ] = 0;
            pStatisticsEntry[ rr ].registerValue = macHwRegDcpt[ rr ].regFunc( pMacRegs );
        }
    }
    return TCPIP_MAC_RES_OK;
}

/*******************************************************************************
 *  MAC Statistics Register Access routines
 */
#define CUMULATIVE_STATISTICS 1

#if CUMULATIVE_STATISTICS
typedef enum {
    // Rx
    RegTotalEmacAle,
    RegTotalEmacEle,
    RegTotalEmacFcse,
    RegTotalEmacFro,
    RegTotalEmacPfr,
    RegTotalEmacRja,
    RegTotalEmacRle,
    RegTotalEmacRov,
    RegTotalEmacRre,
    RegTotalEmacRse,
    RegTotalEmacUsf,
    // Tx
    RegTotalEmacCse,
    RegTotalEmacDtf,
    RegTotalEmacEcol,
    RegTotalEmacFto,
    RegTotalEmacLcol,
    RegTotalEmacMcf,
    RegTotalEmacScf,
    RegTotalEmacSte,
    RegTotalEmacTpf,
    RegTotalEmacTund,
    //
    RegTotalEmacSentinel        // must be last
} RegTotalEmac;

static uint32_t regTotal[ RegTotalEmacSentinel ];
#endif

extern MAC_DRIVER macDrvrDescription[] ;
void emacStatisticsClear( void )
{
    MAC_DRIVER * pMacDrvr = macDrvrDescription;

    macDrvrTxLock( pMacDrvr );
    macDrvrRxLock( pMacDrvr );

    memset( regTotal, 0, sizeof( regTotal ) );
    memset( &pMacDrvr->rxStat, 0, sizeof( pMacDrvr->rxStat ) );
    memset( &pMacDrvr->txStat, 0, sizeof( pMacDrvr->txStat ) );

    macDrvrRxUnlock( pMacDrvr );
    macDrvrTxUnlock( pMacDrvr );
}

uint32_t macDrvrGetEmacAle( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_ALE;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacAle ] += lastCount;
    return regTotal[ RegTotalEmacAle ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacEle( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_ELE;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacEle ] += lastCount;
    return regTotal[ RegTotalEmacEle ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacFcse( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_FCSE;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacFcse ] += lastCount;
    return regTotal[ RegTotalEmacFcse ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacFro( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_FRO;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacFro ] += lastCount;
    return regTotal[ RegTotalEmacFro ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacPfr( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_PFR;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacPfr ] += lastCount;
    return regTotal[ RegTotalEmacPfr ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacRja( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_RJA;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacRja ] += lastCount;
    return regTotal[ RegTotalEmacRja ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacRle( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_RLE;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacRle ] += lastCount;
    return regTotal[ RegTotalEmacRle ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacRov( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_ROV;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacRov ] += lastCount;
    return regTotal[ RegTotalEmacRov ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacRre( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_RRE;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacRre ] += lastCount;
    return regTotal[ RegTotalEmacRre ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacRse( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_RSE;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacRse ] += lastCount;
    return regTotal[ RegTotalEmacRse ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacUsf( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_USF;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacUsf ] += lastCount;
    return regTotal[ RegTotalEmacUsf ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacCse( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_CSE;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacCse ] += lastCount;
    return regTotal[ RegTotalEmacCse ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacDtf( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_DTF;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacDtf ] += lastCount;
    return regTotal[ RegTotalEmacDtf ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacEcol( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_ECOL;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacEcol ] += lastCount;
    return regTotal[ RegTotalEmacEcol ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacFto( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_FTO;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacFto ] += lastCount;
    return regTotal[ RegTotalEmacFto ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacLcol( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_LCOL;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacLcol ] += lastCount;
    return regTotal[ RegTotalEmacLcol ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacMcf( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_MCF;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacMcf ] += lastCount;
    return regTotal[ RegTotalEmacMcf ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacScf( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_SCF;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacScf ] += lastCount;
    return regTotal[ RegTotalEmacScf ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacSte( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_STE;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacSte ] += lastCount;
    return regTotal[ RegTotalEmacSte ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacTpf( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_TPF;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacTpf ] += lastCount;
    return regTotal[ RegTotalEmacTpf ];
#else
    return lastCount;
#endif
}

uint32_t macDrvrGetEmacTund( emac_registers_t * pToMacRegs )
{
    uint32_t lastCount = pToMacRegs->EMAC_TUND;
#if CUMULATIVE_STATISTICS
    regTotal[ RegTotalEmacTund ] += lastCount;
    return regTotal[ RegTotalEmacTund ];
#else
    return lastCount;
#endif
}

